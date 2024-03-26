#coding：utf-8
import os,time
from threading import Thread
  
#代码所在目录
FILE_PATH = r'D:\hryt\UE4_Code\NIC\Source\NIC\Media\QQMusic'

global percent
percent = 0.0
global starNum
starNum = 0

def analyze_code(codefilesource):
    '''
    打开一个文件,统计其中的代码行数,包括空行和注释
    返回含该文件总行数，注释行数，空行数的列表
    :param codefilesource:
    :return:
    '''

    total_line = 0
    comment_line = 0
    blank_line = 0
    # 打开某一个文件
    with open(codefilesource,errors='ignore') as f:
        lines = f.readlines()
        total_line = len(lines)
        line_index = 0
        #遍历每一行
        while line_index < total_line:
            line = lines[line_index]
            line_index += 1

            #检查是否为空行
            if line =='\n':
                blank_line += 1
                continue
            

            line=line.strip()
            #检查是否为注释
            if line.startswith("*"):
                comment_line += 1
            elif line.startswith("/"):
                comment_line += 1

    print("在%s中:"%codefilesource)
    print("代码行数：",total_line)
    print("注释行数:",comment_line,"占%0.2f%%"%(comment_line*100/total_line))
    print("空行数:", blank_line, "占%0.2f%%"%(blank_line * 100 / total_line))
    return [total_line,comment_line,blank_line]


def progressBar():
    '''
    专门用于实时打印进度信息
    '''
    global percent
    global starNum
    startTime = time.perf_counter()
    while(1):
        durTime = time.perf_counter() - startTime
        # 读取全局变量，用于线程通信，这里只读，不必上锁
        a = percent * 100
        b = starNum * '*'
        c = (50-starNum) * '-'
        print("\r{:^3.0f}%[{}{}->]{:.2f}s".format(a, b, c, durTime), end = "")
        if a == 100:
            break
        time.sleep(0.033)


def run(FILE_PATH):
    #遍历文件
    nowFolder = 0
    total_lines = 0
    total_comment_lines = 0
    total_blank_lines = 0
    
    walks = list(os.walk(FILE_PATH))
    allFolder = len(walks)
    fileNum = 0

    # 进度条线程
    global percent
    global starNum
    t1 = Thread(target=progressBar)
    t1.start()

    for root, dirs, files in walks:
        nowFolder += 1
        
        for f in files:
            txt = os.path.join(root, f)
            # 统计哪些类型的文件
            if txt.find(".h") != -1 or txt.find(".cpp") != -1 or txt.find(".cs") != -1:
                line = analyze_code(txt)
                total_lines,total_comment_lines,total_blank_lines = total_lines+line[0],total_comment_lines+line[1],total_blank_lines+line[2]
                fileNum += 1
            
        # 进度条
        percent = nowFolder / allFolder
        starNum = int(percent*50)

    t1.join()
    print()
    print("总文件夹个数:",allFolder,"被统计文件个数:",fileNum)
    print("总代码行数:",total_lines)
    print("总注释行数:",total_comment_lines,"占%0.2f%%"%(total_comment_lines*100/total_lines))
    print("总空行数:", total_blank_lines, "占%0.2f%%"% (total_blank_lines * 100 / total_lines))
    
  
if __name__ == '__main__':

    run(FILE_PATH)