from pathlib import Path
import os

#代码所在目录
FILE_PATH = r'D:\UE_4.27\4.27.2-release\Engine\Source\Runtime\Advertising\Advertising'
#输出格式 0:tree 1:makedown 2:filenames
GENERATE_MODE = 2
#输出目标 0:控制台 1:output.csv文件
OUTPUT_TARGET = 0

tree_str = ''
def generate_tree(pathname, n=0):
    '''输出的目录在前面文件在后面,输出结果类似cmd的tree命令'''
    global tree_str
    if pathname.is_file():
        tree_str += '    |' * n + '-' * 4 + pathname.name + '\n'
    elif pathname.is_dir():
        tree_str += '    |' * n + '-' * 4 + str(pathname.relative_to(pathname.parent)) + '\\' + '\n'
        for cp in pathname.iterdir():
            generate_tree(cp, n + 1)


def generate_tree_MakeDown(pathname, n=0):
    '''输出的文件在前面目录在后面,输出结果针对MakeDown文档进行适配'''
    global tree_str
    if not pathname.is_dir():
        return
    
    if(n>0):
        # 将目录作为标题
        tree_str += '#' * n + ' ' + str(pathname.relative_to(pathname.parent)) + '\n\n'
    folder = []
    for i in pathname.iterdir():
        if i.is_file():
            # 将文件名加粗
            tree_str += '**' +i.name.split('.')[0] + '**' + '\n\n'
        elif i.is_dir():
            folder.append(i)

    for i in folder:
        generate_tree_MakeDown(i, n + 1)


def generate_filenames(pathname):
    '''输出所有目录与文件'''
    global tree_str
    for root, dirs, files in os.walk(pathname):
        tree_str += root + '\n'
        for f in files:
            tree_str += os.path.join(root, f) + '\n'


if __name__ == '__main__':
    path = Path(FILE_PATH)
    if GENERATE_MODE == 0:
        generate_tree(path)
    elif GENERATE_MODE == 1:
        generate_tree_MakeDown(path)
    elif GENERATE_MODE == 2:
        generate_filenames(path)
    
    if OUTPUT_TARGET == 0:
        print(tree_str)
    elif OUTPUT_TARGET == 1:
        with open('output.csv', 'w',encoding='utf-8', newline='') as csvfile:
            csvfile.write(tree_str)
