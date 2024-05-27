#coding：utf-8
import os,time
from threading import Thread
  
#代码所在目录
FILE_PATH = r'D:\UE_4.27\4.27.2-release\Engine\Source'
DETAILl_LOG = False

global percent
percent = 0.0
global star_num
star_num = 0

def analyze_code(codefile):
    '''
    打开一个文件,统计其中的代码行数,包括空行和注释
    返回含该文件总行数，注释行数，空行数的列表
    :param codefile:
    :return:
    '''

    total_line = 0
    comment_line = 0
    blank_line = 0
    # 打开某一个文件
    with open(codefile,errors='ignore') as f:
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

    if DETAILl_LOG:
        print("在%s中:"%codefile)
        print("代码行数：",total_line)
        print("注释行数:",comment_line,"占%0.2f%%"%(comment_line * 100 / total_line))
        print("空行数:", blank_line, "占%0.2f%%"%(blank_line * 100 / total_line))
    return [total_line,comment_line,blank_line]


def progressBar():
    '''
    专门用于实时打印进度信息
    '''
    global percent
    global star_num
    startTime = time.perf_counter()
    while(1):
        durTime = time.perf_counter() - startTime
        # 读取全局变量，用于线程通信，这里只读，不必上锁
        a = percent * 100
        b = star_num * '*'
        c = (50-star_num) * '-'
        print("\r{:^3.0f}%[{}{}>] {:.2f}s".format(a, b, c, durTime), end = "")
        if a == 100:
            break
        time.sleep(0.5)


def run(FILE_PATH):
    #遍历文件
    now_folder = 0
    total_lines = 0
    total_comment_lines = 0
    total_blank_lines = 0

    #文件类型统计
    total_h = 0
    total_cpp = 0
    total_csharp = 0
    
    walks = list(os.walk(FILE_PATH))
    all_folder = len(walks)
    file_num = 0
    analyze_file_num = 0

    # 进度条线程
    global percent
    global star_num
    t1 = Thread(target=progressBar)
    t1.start()

    for root, dirs, files in walks:
        now_folder += 1
        
        for f in files:
            txt = os.path.join(root, f)
            file_num += 1

            need_analyze = False
            # 统计哪些类型的文件
            if txt.endswith((".h",".hpp")):
                total_h += 1
                need_analyze = True
            elif txt.endswith((".c",".cc",".cpp")):
                total_cpp += 1
                need_analyze = True
            elif txt.find(".cs") != -1:
                total_csharp += 1
                need_analyze = True

            if need_analyze:
                line = analyze_code(txt)
                total_lines,total_comment_lines,total_blank_lines = total_lines+line[0],total_comment_lines+line[1],total_blank_lines+line[2]
                analyze_file_num += 1
                
            
        # 进度条
        percent = now_folder / all_folder
        star_num = int(percent*50)

    t1.join()
    print()
    print("总文件夹个数:",all_folder,"总文件个数:",file_num,"被统计文件个数:",analyze_file_num)
    print("统计的文件详情 c++头文件:",total_h,"c++源文件:",total_cpp,"csharp文件",total_csharp)
    print("总代码行数:",total_lines)
    print("注释行数:",total_comment_lines,"占%0.2f%%"%(total_comment_lines * 100 / total_lines))
    print("空行数:", total_blank_lines, "占%0.2f%%"% (total_blank_lines * 100 / total_lines))
    
  
if __name__ == '__main__':

    run(FILE_PATH)