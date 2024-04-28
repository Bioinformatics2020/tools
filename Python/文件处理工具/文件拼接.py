# -*- coding:utf-8*-
import sys

import os
import os.path
import time
time1=time.time()



##########################合并同一个文件夹下多个txt################
def MergeTxt(filepath,outfile):
    k = open(outfile,'+a',encoding='utf-8')
    for parent, dirnames, filenames in os.walk(filepath):
        for filepath in filenames:
            txtPath = os.path.join(parent, filepath)  # txtpath就是所有文件夹的路径
            ##########换行写入##################
            with open(txtPath,encoding='utf-8') as file:
                for line in file:
                    k.write(line)
            k.write(u'\n')
            print(txtPath)
            

    
    k.close()

    print("finished")


if __name__ == '__main__':
    filepath=r"C:\Users\Administrator\Desktop\a"
    outfile=r"C:\Users\Administrator\Desktop\aresult.txt"
    MergeTxt(filepath,outfile)
    time2 = time.time()
    print(u'总共耗时：' + str(time2 - time1) + 's')