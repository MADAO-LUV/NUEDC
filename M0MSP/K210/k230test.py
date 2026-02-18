# k230_color_tracking.py
# 基于 Yahboom 平台的颜色识别组合上传协议，重写为包启用 BCC 校验的协议: [0xFF][xH][xL][yH][yL][BCC][0xFC]

import time, os, sys
from media.sensor import *
from media.display import *
from media.media import *

from ybUtils.YbUart import YbUart
import image

# UART初始化
uart = YbUart(baudrate=115200)

# LCD屏显初始化
DISPLAY_WIDTH = 640
DISPLAY_HEIGHT = 480

# LAB颜色阈值
THRESHOLDS = [
    (0, 66, 7, 127, 3, 127),        # 红色
    (42, 100, -128, -17, 6, 66),    # 绿色
    (43, 99, -43, -4, -56, -7),     # 蓝色
    (37, 100, -128, 127, -128, -27) # Logo 颜色
]

# 返回lab中心的红维rgb值
def get_closest_rgb(lab_threshold):
    l_center = (lab_threshold[0] + lab_threshold[1]) // 2
    a_center = (lab_threshold[2] + lab_threshold[3]) // 2
    b_center = (lab_threshold[4] + lab_threshold[5]) // 2
    return image.lab_to_rgb((l_center,a_center,b_center))

# 初始化sensor
def init_sensor():
    sensor = Sensor()
    sensor.reset()
    sensor.set_framesize(width=DISPLAY_WIDTH, height=DISPLAY_HEIGHT)
    sensor.set_pixformat(Sensor.RGB565)
    return sensor

def init_display():
    Display.init(Display.ST7701, to_ide=True)
    MediaManager.init()

# 计算 BCC 校验值
def calculate_bcc(data):
    bcc = 0
    for b in data:
        bcc ^= b
    return bcc & 0xFF

# 自定义协议打包函数
# [0xFF][xH][xL][yH][yL][BCC][0xFC]
def send_coord_packet(x, y):
    data = bytearray(5)
    data[0] = (x >> 8) & 0xFF
    data[1] = x & 0xFF
    data[2] = (y >> 8) & 0xFF
    data[3] = y & 0xFF
    data[4] = 0x00  # 先置为0
    bcc = calculate_bcc(data[:4])
    packet = bytearray(7)
    packet[0] = 0xFF
    packet[1:5] = data[:4]
    packet[5] = bcc
    packet[6] = 0xFC
    uart.send(packet)
    print("Sent:", [hex(i) for i in packet])

# FPS显示

def draw_fps(img, fps):
    img.draw_string_advanced(0, 0, 30, f'FPS: {fps:.2f}', color=(255, 255, 255))

def main():
    try:
        sensor = init_sensor()
        init_display()
        sensor.run()

        clock = time.clock()
        color_index = 0  # 红色
        threshold = THRESHOLDS[color_index]
        track_color = get_closest_rgb(threshold)

        while True:
            clock.tick()
            img = sensor.snapshot()

            blobs = img.find_blobs([threshold], area_threshold=3000, merge=True)
            if blobs:
                blob = max(blobs, key=lambda b: b[2]*b[3])
                x = blob[5]  # cx
                y = blob[6]  # cy
                img.draw_rectangle(blob[0:4], color=track_color, thickness=4)
                img.draw_cross(x, y, color=(255,255,255), thickness=2)
                send_coord_packet(x, y)

            draw_fps(img, clock.fps())
            Display.show_image(img)

    except Exception as e:
        print("Exception:", e)
    finally:
        Display.deinit()
        MediaManager.deinit()

if __name__ == "__main__":
    main()
