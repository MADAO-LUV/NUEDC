import sensor , image , time , lcd
from fpioa_manager import fm
from machine import UART
from board import board_info
from maix import GPIO
from collections import OrderedDict # 用于创建有序字典
# 定时器模块
from machine import Timer
import struct

"""
*******************通信协议的定义**********************
数据包格式：
    数据包头：0xAA
    数据包长度：数据长度
    数据：
    数据校验和：
    数据包尾：0xBB
数据包格式：
    数据包头：0xCC
    数据包长度：数据长度
    数据：
    数据校验和：
    数据包尾：0xDD
b'\xcc@\x01\xf0\x00R\x00\xdc\x00\x89\rw\xdd'  --- 接收示例
将需要发送的数据打包 类
"""
class Uart_RecvPack():
    def __init__(self,packmsg,dataformat):

        # 传输的数据包
        self.msg = packmsg

        # 传输的数据格式
        self.recvformat = dataformat

        # 要传输的数据长度
        self.data_len = struct.calcsize(self.recvformat)

    # BCC校验函数
    def calculate_BCC(self,datalist,datalen):
        ref = 0
        for i in range(datalen):
            ref = (ref^datalist[i])
        return ref&0xff

    # 接收读取到的数据列表并解包
    def unpack_value(self,datalist):
        try:
            bccref = self.calculate_BCC( datalist , self.data_len-2 )
            if bccref == datalist[self.data_len-2]:
                tmpmsg = bytes(datalist)
                tmpmsg = struct.unpack(self.recvformat,tmpmsg)
                self.msg.update( zip(self.msg.keys(),tmpmsg ) )
            else:
                return False
        except Exception as e:
            return False

        return True

"""
将需要接收的数据解包 类
"""
class Uart_SendPack():
    def __init__(self,packmsg,dataformat):

        # 传输的数据包
        self.msg = packmsg

        # 传输的数据格式
        self.sendformat = dataformat

        # 要传输的数据长度
        self.data_len = struct.calcsize(self.sendformat)


    # BCC校验函数
    def calculate_BCC(self,datalist,datalen):
        ref = 0
        for i in range(datalen):
            ref = (ref^datalist[i])
        return ref&0xff

    # 将要打包的数据进行BCC校验,并返回最终BCC校验的值
    def pack_BCC_Value(self):
        tmp_list = list( self.msg.values() ) # 将字典的值取出转换成列表

        tmp_packed = struct.pack(self.sendformat,*tmp_list) # 根据指定的数据类型对列表值进行自动打包

        # 自动打包后的数值，进行BCC校验，获得最终需要发送的BCC校验值
        return self.calculate_BCC(tmp_packed,len(tmp_packed)-2)

    # 获取最终要发送的数据列表
    def get_Pack_List(self):
        tmplist = list(self.msg.values())# 将字典的值取出,转换成列表
        return struct.pack(self.sendformat,*tmplist)
"""
*****************************上面是关于通信协议的data包定义********************************
格式[head][w_low][w_high][h_low][h_high][x_low][x_high][y_low][y_high][color_low][color_high][BccCheck][End]
帧头:0xCC
帧尾:0xDD
由于是小端模式，所以数据包中的字节的高位是在后面的。
"""




""" 创建需要往stm32端发送数据的数据包 """
# 需要发送的数据(如果需要增删数据,则修改此处以及修改数据格式,同时在32端也对应增删即可)
send_pack1_msg = OrderedDict([
        ('Head',0xCC),  # 帧头           uint8_t类型
        ('Cam_W', 320), # 相机的像素宽度  uint16_t 类型
        ('Cam_H', 240), # 相机的像素长度  uint16_t 类型
        ('follow_x',0),  # 需要跟踪的点x   uint16_t 类型
        ('follow_y',0),  # 需要跟踪的点y   uint16_t 类型
        ('area',0),     #面积的大小       uint16_t 类型
        ('BccCheck',0), # 数据BCC校验位   uint8_t类型
        ('End',0xDD)    # 帧尾            uint8_t类型
])

# 数据格式 <代表小端模式, B代表uint8_t类型,4H代表4个uint16_t类型,2B代表2个uint8_t类型
send_pack1_format = "<B5H2B"

#实例化数据打包对象
send_pack1 = Uart_SendPack(send_pack1_msg,send_pack1_format)

# 更新需要发送的数据并返回打包结果,将结果直接发送到stm32端即可
def update_sendpack1_data(follow_x,follow_y,area):
    global send_pack1
    send_pack1.msg['follow_x'] = follow_x
    send_pack1.msg['follow_y'] = follow_y
    send_pack1.msg['area']      = area
    send_pack1.msg['BccCheck'] = send_pack1.pack_BCC_Value()
    return send_pack1.get_Pack_List()
""" 创建需要往stm32端发送数据的数据包 END """


""" 创建需要接收stm32端数据的数据包 """
# (如果需要增删数据,则修改此处以及修改数据格式,同时在32端也对应增删即可)
recv_pack1_msg = OrderedDict([
        ('Head',0xAF),        # 帧头
        ('seltec_color', 0),  # stm32需要追踪的颜色
        ('BccCheck',0),       # BCC校验
        ('End',0xFA)          # 帧尾
])
recv_pack1_format = "<4B"
recv_pack1 = Uart_RecvPack(recv_pack1_msg,recv_pack1_format)
""" 创建需要接收stm32端数据的数据包 END """


# 各传感器初始化
lcd.init()
sensor.reset()
sensor.reset(freq=24000000, dual_buff=1)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)

sensor.set_auto_gain(False)      # 颜色跟踪必须关闭自动增益
sensor.set_auto_whitebal(False)  # 颜色跟踪必须关闭白平衡

sensor.skip_frames(time = 1000)

#帧率时钟
clock = time.clock()

# 指定串口引脚并构造UART对象
fm.register(0, fm.fpioa.UART1_RX)
fm.register(1, fm.fpioa.UART1_TX)
uart1 = UART(UART.UART1,115200)

color_thresholds = [
    (0, 15, -128, 127, -128, 127) # 黑色标准阈值
]


# 追踪的颜色选择
cmd_color = 0

# 辅组接收数据的变量
Last_recv32 = None

# tmp:设置一个按键,用于切换颜色
fm.register(board_info.BOOT_KEY, fm.fpioa.GPIOHS0, force=True)
key=GPIO(GPIO.GPIOHS0, GPIO.IN, GPIO.PULL_NONE)


# 感兴趣的区域设置:x,y,w,h
# 像素共有：320x240
roi = (40, 200, 240, 40)




g_follow_blob = None
g_last_blob = None
g_errortime = 0


# 定时器中断回调函数
def timer_callback(timer):
    global g_follow_blob
    global g_last_blob
    global g_errortime

    # 最大最小面积定义(通过实测获得)
    #min_area = 500
    #max_area = 2500

    min_area = 1000
    max_area = 11000

    try:
        # 将最大物体的中心坐标发送到stm32
        if g_follow_blob!=None:

            # 计算识别到的面积
            area = g_follow_blob.rect()[2]*g_follow_blob.rect()[3]

            if area > min_area and area < max_area:
                g_errortime = 0
                send_to_32 = update_sendpack1_data(g_follow_blob.cx(),g_follow_blob.cy(),area)
                uart1.write(send_to_32)
                print(send_to_32)
            else:
                g_errortime = 1
            #if area > min_area and area < max_area:
                #g_errortime = 0
            #else: # 识别面积不符合要求,使用上一次的数据
                #g_errortime = g_errortime + 1
                #if g_errortime > 60:
                    #g_errortime = 60
                #else:
                    #g_follow_blob = g_last_blob

            #g_last_blob = g_follow_blob

            ## 长时间识别到不满足要求的数据,则不再向stm32发送数据
            #if g_errortime < 50:
                #send_to_32 = update_sendpack1_data(g_follow_blob.cx(),g_follow_blob.cy())
                #uart1.write(send_to_32)
    except Exception as e:
        print(e)


# 创建一个定时器,period为定时时间,单位ms.
timer = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PERIODIC, period=10, callback=timer_callback)

# 启动定时器
timer.start()



while True:

    #用于计算帧率的函数，这里表示开始计时
    clock.tick()

    try:
        #从传感器捕获一张图像
        img = sensor.snapshot()

        # 框选感兴趣的区域用于提示
        img.draw_rectangle(roi, color=(255 ,255,255),thickness = 3)

        # 根据色块阈值找出感兴趣区域的色块
        blobs = img.find_blobs([color_thresholds[cmd_color]], pixels_threshold=100, area_threshold=100, merge=True, margin=10,roi = roi)

        max_box_size = 0
        max_blob = None

        if blobs:

            max_blob = blobs[0]
            for blob in blobs:
                # 面积计算：blob.rect()[2]*blob.rect()[3]
                if blob.rect()[2]*blob.rect()[3] > max_blob.rect()[2]*max_blob.rect()[3]:
                    max_blob = blob

            # 框选出识别到的色块
            g_follow_blob = max_blob
            img.draw_rectangle(g_follow_blob.rect(), color=(255 , 0,0),thickness = 3)
            img.draw_cross(g_follow_blob.cx(), g_follow_blob.cy(), color=(255 , 0,0))


        # 显示帧率
        fps = clock.fps()
        img.draw_string(0, 0, "%2.1fps" %(fps), color=(0, 60, 255), scale=2.0)

        if max_blob!=None:
            img.draw_string(0, 20, "size:%d" %(g_follow_blob.rect()[2]*g_follow_blob.rect()[3]), color=(255, 60, 255), scale=2.0)
            img.draw_string(0, 40, "err:%d" %(g_errortime), color=(255, 60, 255), scale=2.0)


        #显示在LCD上
        lcd.display(img)

    except Exception as e:
        print(e)


# 按键扫描
# onceflag = 1
#if key.value() == 0 and onceflag == 1:
    #cmd_color = cmd_color + 1
    #if cmd_color > len(color_strings)-1:
       #cmd_color = 0
    #onceflag = 0
#elif key.value() == 1 and onceflag == 0:
    #onceflag = 1

###################################### 接收 STM32 端数据 ############################
# 接收到stm32串口数据
#if uart1.any()!=0:
    #recv_from_32 = ord(uart1.read(1)) # 将读取到的bytes字节数据,使用ord方法转成10进制数
    ## 读取到帧头与上一帧数据的帧尾,说明是正确数据(注意：此方法接收数据会丢失最开始的一帧数据,后续所有数据都正常)
    #if recv_from_32 == recv_pack1.msg["Head"] and Last_recv32 == recv_pack1.msg["End"]:

        #recv_from_32 = bytes([recv_from_32]) # 恢复第一字节数据成bytes类型,并与下面读取到的数据相加得到一帧完整的数据
        #recv_from_32 += uart1.read(recv_pack1.data_len-1)

        ## 将接收到的一帧完整数据送入解包方法,进行数据解包
        #if recv_pack1.unpack_value(recv_from_32) == True:

            ## 判断接收到的目标颜色是否超出索引值
            #if recv_pack1.msg["seltec_color"] > len(color_strings)-1:
                #print("out of index")
            #else:
                #cmd_color = recv_pack1.msg["seltec_color"]
                #print( "Now Tracked :"+ color_strings[cmd_color] )

        ## 解包完毕,保存最后一帧数据方便下次解包使用
        #recv_from_32 = recv_from_32[recv_pack1.data_len-1]

    ## 保存上一帧数据,用于辅组接收数据判断
    #Last_recv32 = recv_from_32
###################################### 接收 STM32 端数据 ############################
