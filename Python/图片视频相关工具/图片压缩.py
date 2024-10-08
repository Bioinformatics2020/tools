import os
from PIL import Image
from PIL import ImageFile
import shutil
 
# 压缩图片文件
def compress_image(file, kb=5, quality=85, k=0.9): # 通常你只需要修改kb大小
    """不改变图片尺寸压缩到指定大小
    :param file: 输入文件地址
    :param kb: 压缩目标
    :param k: 每次调整的压缩比率
    :param quality: 初始压缩比率
    :return: 压缩文件地址，压缩文件大小
    """
 
    o_size = os.path.getsize(file) / 1024  # 函数返回为字节，除1024转为kb（1kb = 1024 bit）
    print('before_size:{:.2f} after_size:{}'.format(o_size, kb))
    if o_size <= kb:
        return file
    
    
    ImageFile.LOAD_TRUNCATED_IMAGES = True  # 防止图像被截断而报错
    
    filename, file_extension = os.path.splitext(file)
    outfile = filename + "_" + str(kb) + file_extension
    shutil.copy(file, outfile)

    while o_size > kb:
        im = Image.open(outfile)
        x, y = im.size
        out = im.resize((int(x*k), int(y*k)))  # 最后一个参数设置可以提高图片转换后的质量
        try:
            out.save(outfile, quality=quality)  # quality为保存的质量，从1（最差）到95（最好），此时为85
        except Exception as e:
            print(e)
            break
        o_size = os.path.getsize(outfile) / 1024
    return outfile


# 修改图片分辨率
def resize_image(input_image_path, output_image_path, new_width, new_height):
    image = Image.open(input_image_path)
    resized_image = image.resize((new_width, new_height))
    resized_image.save(output_image_path)


# 修改图片分辨率为多种分辨率
def multiple_resize_image(input_image_path, size):
    image_name = os.path.basename(input_image_path)
    output_path =  os.path.dirname(input_image_path) + '\\'
    for i in size:
        name = output_path + image_name.split(".")[0] + str(i)+'_'+str(i) + '.' + image_name.split(".")[1]
        resize_image(input_image_path,name,i,i)
        print(name)


if __name__ == "__main__":
    # 批量压缩图片
    # path = r'D:\hryt\UE4_Code\NIC\Content\Movies\VC1_Baby - 副本\\' # 待压缩图片文件夹
    # for img in os.listdir(path):
    #     compress_image(path + str(img),kb=1.5)

    # 修改图片尺寸
    # resize_image(r'C:\Users\Administrator\Downloads\aaa.png',r'C:\Users\Administrator\Downloads\aaa40.png',40,40)
    # multiple_resize_image(r'C:\Users\Administrator\Downloads\aaa1024.png',[40,2048])

    compress_image(r'C:\Users\Administrator\Downloads\aaa2048.png')