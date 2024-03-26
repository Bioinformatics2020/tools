
import numpy as np
import cv2 as cv
 
# 捕获本地视频,请自行修改自己存放视频的路径
cap = cv.VideoCapture(r"C:\Users\Administrator\Downloads\Tab切换_221230.mp4")
 
# 指定VideoWrite 的fourCC视频编码
fourcc = cv.VideoWriter_fourcc(*'DIVX')
 
# 指定输出文件，fourCC视频编码，FPS帧率，画面大小
# out = cv.VideoWriter('output1.avi', fourcc, 20.0, (640, 480))
 
# 检查是否导入视频成功
if not cap.isOpened():
    print("视频无法打开")
    exit()
 
# 获取视频的宽，高信息,    cap.get()，传入的参数可以是0-18的整数
print('WIDTH', cap.get(3))
print('HEIGHT', cap.get(4))

fps = cap.get(cv.CAP_PROP_FPS) #视频平均帧率

frames = []
while True:
    # 捕获视频帧，返回ret，frame
    # ret的true与false反应是否捕获成功，frame是画面
    ret, frame = cap.read()
 
    if not ret:
        print("视频播放完毕")
        break
    frames.append(frame)
 
    # 处理帧， 将画面转化为灰度图
    # gray1 = cv.cvtColor(frame, cv.COLOR_BGRA2GRAY)
 
    # 对画面帧进进行处理，这里对画面进行翻转
    # gray2 = cv.flip(gray1, 0)
 
    # 将处理后的视频逐帧地显示
cv.namedWindow("frame_window", cv.WINDOW_NORMAL)  # 0可调大小，注意：窗口名必须imshow里面的一窗口名一直
cv.resizeWindow("frame_window", int(cap.get(3) * 0.4), int(cap.get(4) * 0.4))    # 设置长和宽

i=0
play = 0

while i < len(frames) and i > -1:
    print("第",i,"帧")
    frame = frames[i]
    cv.imshow('frame_window', frame)
 
    # 将处理后的画面逐帧地保存到output文件中
    # out.write(gray2)
 
    # 获取按键动作，如果按下q，则退出循环
    # 25毫秒是恰好的，如果太小，播放速度会很快，如果太小，播放速度会很慢
    if play != 0:
        inKey = cv.waitKey(int(1000/fps))
    else:
        inKey = cv.waitKey(0)

    if inKey == ord('q'):
        break
    elif inKey == ord('s'):
        i += 1
    elif inKey == ord('a'):
        i -= 1
    elif inKey == ord(' '):
        if play == 0:
            play = 1
        else:
            play = 0
        print("play mode change",play)
    else:
        i += 1
 
 
cap.release()
cv.destroyAllWindows()