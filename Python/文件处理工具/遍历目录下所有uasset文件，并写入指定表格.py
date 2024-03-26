import pandas as pd
import json
import os

def ShowFileName(file):
    fileNames = []
    packages = []
    for root, dirs, files in os.walk(file):
        # root 表示当前正在访问的文件夹路径
        # dirs 表示该文件夹下的子目录名list
        # files 表示该文件夹下的文件list
        # 遍历文件

        for f in files:
            # print(os.path.join(root, f))
            txt = os.path.join(root, f)

            # 确认筛选条件
            x = txt.find(r".png")
            
            fileName = txt.split('\\')[-1].split('.')[0]

            # 目标字符串 "QQMusicA1MainPage" "/Game/NIC/Core/Widgets/Multi-Media/QQMusic/QQMusicA1MainPage.QQMusicA1MainPage_C"
            package = '/Game/'+(root.split('\\Content\\')[-1]+'\\'+fileName+'.'+fileName).replace('\\','/')
            fileNames.append(fileName)
            packages.append(package)

    return fileNames,packages

def WriteTable(fileNames,packages):
    WriteStr = ',Name,Ptr\n'
    i = 0
        

    for i in range(1,len(fileNames)):
        WriteStr += str(i) + ',' + fileNames[i-1] + ',' + packages[i-1] + '\n'
        
    writeFile = r"D:\hryt\UE4_Code\NIC\Source\NIC\Media\DataAssetTableUObject.csv"
    with open(writeFile,'w') as f:
        f.write(WriteStr)


File = r'D:\hryt\UE4_Code\NIC\Content\Movies\VC1_Baby'

if __name__ == '__main__':
    fileNames,packages = ShowFileName(File)
    for i in fileNames:
        print(i)
    WriteTable(fileNames,packages)
    