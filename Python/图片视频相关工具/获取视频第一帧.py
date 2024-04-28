import cv2
import os

def get_jpg(input_mp4_path, output_img_path):
    '''
    获取视频第一帧

    input_mp4_path 输入的视频路径
    output_img_path 输出的图片路径
    '''

    # 读取视频文件
    video = cv2.VideoCapture(input_mp4_path)

    # 获取视频第一帧
    ret, frame = video.read()
    video.release()

    succeed = cv2.imwrite(output_img_path, frame)
    if succeed:
        print(output_img_path ,"输出完成")


if __name__ == "__main__":

    dataset_dir = r'D:\CarGradle\BJCarExhibition\BJMotorShow\Content\Movies\ReportMp4\Source\Raw'
    output_dir = r'D:\CarGradle\BJCarExhibition\BJMotorShow\Content\Movies\ReportMp4\Source\RawImage'

    # 检查并创建 图片输出目录
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    # 获得需要转化的图片路径并生成目标路径
    image_filenames = [(os.path.join(dataset_dir, x), os.path.join(output_dir, x.replace("mp4","jpg")))
                        for x in os.listdir(dataset_dir)]

    # 转化所有图片
    for path in image_filenames:
        get_jpg(path[0], path[1])