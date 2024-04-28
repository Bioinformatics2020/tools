# -*- coding:utf-8 -*-

import os
import csv

# 保存所有文件
out_files = []

def traverse_folder(folder_path):
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            file_path = os.path.join(root, file)
            out_files.append(file_path)
            print(file_path)


# 调用函数遍历文件夹并输出到CSV文件
root = r'C:\Users\Administrator\Desktop\ZX_Note\ZX_github\files'
traverse_folder(root)

# 保存所有的目录
paths = {""}

with open('output.csv', 'w',encoding='utf-8', newline='') as csvfile:
    writer = csv.writer(csvfile)
    for path in out_files:
        
        writer.writerow([path, os.path.getsize(path)])
        paths.add(path)

print(len(paths))