import numpy as np
import cv2
import os
 
def update(input_img_path, output_img_path):
 
    image = cv2.imread(input_img_path)
    print(image.shape)
    size = image.shape
    cropped = image[int(size[0]/2):int(size[0]), 0:int(size[1]/2)] # 裁剪坐标为[y0:y1, x0:x1]
    cv2.imwrite(output_img_path, cropped)
 
dataset_dir = r'D:\hryt\UE4_Code\NIC\Content\Movies\VC1_Baby_cut3'
output_dir = r'D:\hryt\UE4_Code\NIC\Content\Movies\VC1_Baby_cut4'
 
 
# 获得需要转化的图片路径并生成目标路径
image_filenames = [(os.path.join(dataset_dir, x), os.path.join(output_dir, x))
                    for x in os.listdir(dataset_dir)]
# 转化所有图片
for path in image_filenames:
    update(path[0], path[1])