import uos as os
import time
from media.sensor import *
from media.display import *
from media.media import *
import ybUtils.YbKey as YbKey
import image  # 确保image模块已正确导入

save_path = "/data/snapshot"  # 保存位置
img_save_name_begin = "headphones_"
prefix = "headphon"
num = 0  # 照片计数器

def ensure_dir(directory):
    if not directory or directory == '/':
        return
    directory = directory.rstrip('/')
    try:
        os.stat(directory)
        print(f'目录已存在: {directory}')
        return
    except OSError:
        if '/' in directory:
            parent = directory[:directory.rindex('/')]
            if parent and parent != directory:
                ensure_dir(parent)
        try:
            os.mkdir(directory)
            print(f'已创建目录: {directory}')
        except OSError as e:
            try:
                os.stat(directory)
                print(f'目录已被其他进程创建: {directory}')
            except:
                print(f'创建目录时出错: {e}')
    except Exception as e:
        print(f'处理目录时出错: {e}')

if __name__ == "__main__":
    try:
        key = YbKey.YbKey()

        sensor = Sensor()  # 添加 sensor 实例化
        sensor.reset()
        sensor.set_framesize(width=640, height=480, chn=CAM_CHN_ID_1)
        sensor.set_pixformat(Sensor.RGB565, chn=CAM_CHN_ID_1)

        Display.init(Display.ST7701, width=640, height=480, to_ide=True)

        MediaManager.init()
        sensor.run()

        last_status = False

        while True:
            img = sensor.snapshot(chn=CAM_CHN_ID_1)
            img2 = image.Image(640, 480, image.RGB565)
            img2.clear()
            img2.copy_from(img)
            Display.show_image(img2, 0, 0, Display.LAYER_OSD2)

            key = YbKey.YbKey()
            if key.is_pressed() == 1:
                if not last_status:
                    last_status = True
                    ensure_dir(save_path + "/" + str(prefix) + "/")
                    path = save_path + "/" + str(prefix) + "/" + img_save_name_begin + str(num) + ".jpg"
                    print(f"保存路径: {path}")
                    img.save(path)
                    num += 1
                    time.sleep_ms(1)
            else:
                last_status = False

    except KeyboardInterrupt:
        print("用户停止程序 User stopped the program")
    except BaseException as e:
        print(f"发生异常 Exception occurred: '{e}'")
    finally:
        if isinstance(sensor, Sensor):
            sensor.stop()
        Display.deinit()
        os.exitpoint(os.EXITPOINT_ENABLE_SLEEP)
        time.sleep_ms(100)
        MediaManager.deinit()
