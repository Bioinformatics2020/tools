import cv2

# 读取视频文件
video = cv2.VideoCapture(r'C:\Users\Administrator\Videos\Captures\1.mp4')

# 获取视频的帧大小
frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))

# 设置裁剪的边框大小
top = 100
down = 200
left = 0
right = 0

# 计算裁剪后的视频帧大小
crop_width = frame_width - left - right
crop_height = frame_height - top - down

# 设置输出视频的参数
fps = video.get(cv2.CAP_PROP_FPS)
output_filename = r'C:\Users\Administrator\Videos\Captures\1_out.mp4'
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
output_video = cv2.VideoWriter(output_filename, fourcc, fps, (crop_width, crop_height))

ship = False
# 逐帧读取并裁剪视频
while True:
    ret, frame = video.read()
    if not ret:
        break

    ship = not ship
    if ship:
        continue

    # 裁剪画面边框
    cropped_frame = frame[top:frame_height - down, left:frame_width - right]

    # 写入裁剪后的帧
    output_video.write(cropped_frame)

# 释放资源
video.release()
output_video.release()