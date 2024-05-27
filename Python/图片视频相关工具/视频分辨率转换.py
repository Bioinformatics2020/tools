import cv2

def resize_video(input_path, output_path, new_width, new_height):
    # 创建一个VideoCapture对象来读取视频
    cap = cv2.VideoCapture(input_path)
    
    # 获取视频的帧率
    fps = cap.get(cv2.CAP_PROP_FPS)
    
    # 获取视频的总帧数
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    
    # 获取视频的宽度和高度
    frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    
    print(f"原视频分辨率: {frame_width}x{frame_height}, 帧率: {fps}")
    
    # 根据新的宽度和高度计算缩放比例，保持宽高比不变
    aspect_ratio = frame_width / frame_height
    if new_width > 0 and new_height > 0:
        # 直接使用给定的新分辨率
        new_aspect_ratio = new_width / new_height
    elif new_width > 0:
        # 只给了宽度，根据宽高比计算高度
        new_height = int(new_width / aspect_ratio)
    elif new_height > 0:
        # 只给了高度，根据宽高比计算宽度
        new_width = int(new_height * aspect_ratio)
    else:
        print("分辨率设置错误，请至少指定宽度或高度。")
        return
    
    print(f"目标分辨率: {new_width}x{new_height}")
    
    # 创建一个VideoWriter对象来写入新的视频文件
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # 使用mp4格式编码
    out = cv2.VideoWriter(output_path, fourcc, fps, (new_width, new_height))
    
    # 读取并处理每一帧
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break
        
        # 调整帧的大小到新分辨率
        resized_frame = cv2.resize(frame, (new_width, new_height), interpolation=cv2.INTER_LINEAR)
        
        # 写入帧到输出文件
        out.write(resized_frame)
    
    # 释放资源
    cap.release()
    out.release()
    print("视频分辨率转换完成。")




if __name__ == "__main__":  
    # 使用函数，指定输入视频路径、输出视频路径以及新的宽度和高度
    resize_video(r'C:\Users\Administrator\Videos\Captures\2.mp4', r'C:\Users\Administrator\Videos\Captures\3.mp4', 1920, 1080)