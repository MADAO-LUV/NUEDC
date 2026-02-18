from machine import PWM
from machine import FPIOA
import time,os,gc
from media.sensor import *
from media.display import *
from media.media import *

#from libs.YbProtocol import YbProtocol
#from ybUtils.YbUart import YbUart
# uart = None 暂时不需要
#uart = YbUart(baudrate=115200)
#pto = YbProtocol()


# 灰度值(79, 106)

# 显示参数 / Display parameters
DISPLAY_WIDTH = 640   # LCD显示宽度 / LCD display width
DISPLAY_HEIGHT = 480  # LCD显示高度 / LCD display height


# LAB颜色空间阈值 / LAB color space thresholds
# (L Min, L Max, A Min, A Max, B Min, B Max)
THRESHOLDS = [
    (17, 100, 31, 127, 0, 49),    # 红色阈值 / Red threshold
    (42, 100, -128, -17, 6, 66),     # 绿色阈值 / Green threshold
    (43, 99, -43, -4, -56, -7),       # 蓝色阈值 / Blue threshold
    (37, 100, -128, 127, -128, -27)    # 亚博智能Logo的颜色 color of YAHBOOM
]

class PID:
    def __init__(self,kp,ki,input_value,target=320):
        self.e = 0
        self.e_last = 0
        self.kp = kp
        self.ki = ki
        self.target = target
        self.input_value = input_value
    #把观测值放入
    def cal(self,value):
        self.e = self.target - value
        delta = self.kp * (self.e - self.e_last) + self.ki * self.e
        self.e_last = self.e
        self.input_value += delta
        return self.input_value

"""
img = img.copy(roi=(320,280,640,640))
img.compressed_for_ide()
用于图片裁剪 请放到while(1)中
"""
#初始化摄像头
def init_sensor():
    """初始化摄像头 / Initialize camera sensor"""
    sensor = Sensor()
    sensor.reset()
    sensor.set_framesize(width=DISPLAY_WIDTH, height=DISPLAY_HEIGHT)
    sensor.set_pixformat(Sensor.RGB565)
    return sensor

def init_display():
    """初始化显示 / Initialize display"""
    Display.init(Display.ST7701, to_ide=True)
    MediaManager.init()

#            process_blobs(img, blobs, detect_color)
def process_blobs(img, blobs, color,pid_x,pid_y,pwm0,pwm1):
    """处理检测到的色块 / Process detected color blobs"""
    for blob in blobs:
        img.draw_rectangle(blob[0:4], color=color, thickness=4)
        img.draw_cross(blob[5], blob[6], color=color, thickness=2)
        x = blob[0]
        y = blob[1]
        w = blob[2]
        h = blob[3]
        c_x = blob[5]
        c_y = blob[6]

        angle_x = pid_x.cal(c_x)
        angle_y = pid_y.cal(c_y)
        # 限幅在舵机控制范围内
        angle_x = max(2.5, min(angle_x, 12.5))
        angle_y = max(2.5, min(angle_y, 12.5))

        pwm0.duty(round(angle_x, 2))
        pwm1.duty(round(angle_y, 2))
        break
        #pto_data = pto.get_color_data(x, y, w, h)
        #uart.send(pto_data)
        #print(pto_data)

def get_closest_rgb(lab_threshold):
    """根据LAB阈值计算最接近的RGB颜色 / Calculate closest RGB color based on LAB threshold"""
    # 获取LAB空间的中心点值
    l_center = (lab_threshold[0] + lab_threshold[1]) // 2
    a_center = (lab_threshold[2] + lab_threshold[3]) // 2
    b_center = (lab_threshold[4] + lab_threshold[5]) // 2
    return image.lab_to_rgb((l_center,a_center,b_center))

def draw_fps(img, fps):
    """绘制FPS信息 / Draw FPS information"""
    img.draw_string_advanced(0, 0, 30, f'FPS: {fps:.3f}', color=(255, 255, 255))


# 实例化FPIOA
# Instantiate FPIOA
def init_servo():
    fpioa = FPIOA()
    fpioa.set_function(42, fpioa.PWM0)
    #实例化PWM通道0 频率为50Hz，(1.5/20)*100   前面的是频率 后面的是duty
    pwm0 = PWM(0, 50, (1.5/20) * 100, enable = True)
    fpioa.set_function(43, fpioa.PWM1)
    pwm1 = PWM(1, 50, (1.5/20) * 100, enable = True)
    pwm0.enable(1)
    pwm1.enable(1)    #使能引脚
    return pwm0,pwm1


"""
# 检测指定颜色 / Detect specified color
blobs = img.find_blobs([threshold], area_threshold=1000, merge=True)
if blobs:
    process_blobs(img,blobs,detect_color,pid_x,pid_y,pwm0,pwm1)
"""

try:
    # 初始化设备 / Initialize devices
    sensor = init_sensor()
    init_display()
    sensor.run()
    pwm0,pwm1 = init_servo()
    clock = time.clock()

    # 选择要检测的颜色索引 (0:红, 1:绿, 2:蓝) / Select color index to detect
    color_index = 0  # 可以修改这个值来选择检测不同的颜色
    threshold = THRESHOLDS[color_index]
    detect_color = get_closest_rgb(threshold)

    """
    初始化部分
    """
    flag = 0
    c_x = 0
    c_y = 0
    pid_x = PID(0.15,0.08,1.5/20*100,target=DISPLAY_WIDTH // 2)
    pid_y = PID(0.15,0.08,1.5/20*100,target=DISPLAY_HEIGHT // 2)

    while True:
        clock.tick()
        img = sensor.snapshot()
        img = img.to_grayscale()
        # 检测指定颜色 / Detect specified color
        blobs = img.find_blobs([threshold], area_threshold=1000, merge=True)
        if blobs:
            process_blobs(img,blobs,detect_color,pid_x,pid_y,pwm0,pwm1)

        fps = clock.fps()
        draw_fps(img, fps)
        print(fps)

        Display.show_image(img)

finally:
    if 'sensor' in locals() and isinstance(sensor, Sensor):
        sensor.stop()
    Display.deinit()
    MediaManager.deinit()
    pwm0.deinit()
    pwm1.deinit()
