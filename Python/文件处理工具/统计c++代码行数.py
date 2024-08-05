#coding：utf-8
import os,time
from threading import Thread

#代码所在目录
FILE_PATH = r'D:\UE_4.27\4.27.2-release\Engine\Source\Runtime'
#详细log，每个文件均输出统计信息
DETAILl_LOG = False
#统计子目录的每个文件夹内的代码
SUB_DIR = True
#进度条刷新时间间隔
PROGRESSBAR_DELAYTIME = 0.1


global percent
percent = 0.0

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
    startTime = time.perf_counter()
    while(1):
        durTime = time.perf_counter() - startTime
        # 读取全局变量，用于线程通信，这里只读且没有时效或准确性要求，不必上锁
        star_num = int(percent*50)
        star = star_num * '*'
        line = (50-star_num) * '-'
        print("{:^3}%[{}{}>] {:.2f}s".format(int(percent * 100), star, line, durTime))
        if percent == 1:
            break
        time.sleep(PROGRESSBAR_DELAYTIME)


def stat_folder(file_path):
    #遍历文件
    now_folder = 0
    total_lines = 0
    total_comment_lines = 0
    total_blank_lines = 0

    #文件类型统计
    total_h = 0
    total_cpp = 0
    total_csharp = 0
    
    walks = list(os.walk(file_path))
    all_folder = len(walks)
    file_num = 0
    analyze_file_num = 0

    # 进度条线程
    global percent
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
        
    t1.join()
    
    print("总文件夹个数:",all_folder,"总文件个数:",file_num,"被统计文件个数:",analyze_file_num)
    print("统计的文件详情 c++头文件:",total_h,"c++源文件:",total_cpp,"csharp文件",total_csharp)
    print("总代码行数:",total_lines)
    print("注释行数:",total_comment_lines,"占%0.2f%%"%(total_comment_lines * 100 / total_lines))
    print("空行数:", total_blank_lines, "占%0.2f%%"% (total_blank_lines * 100 / total_lines))
    

def stat_sub_folder(file_path):
    # 列出指定目录中的所有条目
    entries = os.listdir(file_path)

    # 遍历条目，检查是否为目录
    for entry in entries:
        full_path = os.path.join(file_path, entry)
        if os.path.isdir(full_path):
            print()
            print(full_path)
            stat_folder(full_path)


def run():
    if SUB_DIR:
        stat_sub_folder(FILE_PATH)
    else:
        stat_folder(FILE_PATH)

if __name__ == '__main__':
    run()
    
    
    


r'''
一部分之前的统计数据

ffmpeg-6.1.1
总文件夹个数: 98 总文件个数: 8167 被统计文件个数: 4158
统计的文件详情 c++头文件: 1094 c++源文件: 3061 csharp文件 3
总代码行数: 1617350
注释行数: 178529 占11.04%
空行数: 189945 占11.74%

hryt/Source
总文件夹个数: 246 总文件个数: 1143 被统计文件个数: 1138
统计的文件详情 c++头文件: 583 c++源文件: 552 csharp文件 3
总代码行数: 313571
注释行数: 19753 占6.30%
空行数: 27077 占8.64%

hryt/Source/NIC/UI/Media
总文件夹个数: 25 总文件个数: 280 被统计文件个数: 280
统计的文件详情 c++头文件: 140 c++源文件: 140 csharp文件 0
总代码行数: 36057
注释行数: 3405 占9.44%
空行数: 4115 占11.41%

4.27.2-release/Engine/Source
总文件夹个数: 10483 总文件个数: 86954 被统计文件个数: 60377
统计的文件详情 c++头文件: 36491 c++源文件: 21437 csharp文件 2449
总代码行数: 20914240
注释行数: 3612243 占17.27%
空行数: 2980613 占14.25%

JsonParserAndDispather/Source
总文件夹个数: 7 总文件个数: 13 被统计文件个数: 13
统计的文件详情 c++头文件: 7 c++源文件: 5 csharp文件 1
总代码行数: 639
注释行数: 45 占7.04%
空行数: 73 占11.42%

JNIBridge/Source
总文件夹个数: 4 总文件个数: 6 被统计文件个数: 5
统计的文件详情 c++头文件: 2 c++源文件: 2 csharp文件 1
总代码行数: 297
注释行数: 13 占4.38%
空行数: 38 占12.79%

'''