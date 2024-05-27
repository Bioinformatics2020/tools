import cv2

def extract_frames_to_images(video_path, output_folder):
    """
    提取视频中的每一帧并保存为单独的图像文件。
    
    :param video_path: 输入视频文件的路径。
    :param output_folder: 保存图像文件的输出文件夹路径。
    """
    # 创建VideoCapture对象
    cap = cv2.VideoCapture(video_path)
    
    if not cap.isOpened():
        print("Error opening video file")
        return
    
    # 获取视频的总帧数
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    print(f"视频总帧数: {total_frames}")
    
    # 创建输出文件夹如果不存在
    import os
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
    
    # 读取并保存每一帧
    for i in range(total_frames):
        ret, frame = cap.read()
        if not ret:
            break
        
        # 构建输出文件名
        frame_filename = f"{i}.temp"
        frame_path = os.path.join(output_folder, frame_filename)

        # 打开文件以二进制写入模式
        with open(frame_path, 'wb') as f:
            # 遍历数组，将每个元素写入文件
            for byte in frame.tobytes():  # tobytes()将数组转换为字节序列
                f.write(bytes([byte]))  # 写入单个字节

        print(f"{total_frames}:{i+1}")

        # 保存当前帧为png图像文件
        # cv2.imwrite(frame_path, frame)
    
    cap.release()
    print("所有帧已保存。")

if __name__ == "__main__":
    # 使用函数，指定视频路径和输出文件夹
    extract_frames_to_images(r'C:\Users\Administrator\Videos\Captures\3.mp4',r'C:\Users\Administrator\Videos\Captures\aaa\temp')