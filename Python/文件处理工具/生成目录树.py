from pathlib import Path
import os

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

if __name__ == '__main__':
    path = r'D:\UnrealEngine-4.27\Project\UltraDynamicSky\Content\UltraDynamicSky'
    generate_tree_MakeDown(Path(path))
    print(tree_str)


# 保存所有文件
out_files = []



