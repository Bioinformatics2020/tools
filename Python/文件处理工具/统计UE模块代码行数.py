#coding：utf-8

import importlib.util
import sys
import os

# 假设你的文件名为 "示例文件.py" 且位于当前工作目录
filename = "统计c++代码行数.py"

# 获取文件的完整路径
file_path = os.path.join(os.getcwd(), filename)

# 将中文文件名转换为模块名，这里我们使用拼音来代替中文
module_name = "MyModule"  # 示例文件的拼音

# 使用 importlib 加载模块
spec = importlib.util.spec_from_file_location(module_name, file_path)
module = importlib.util.module_from_spec(spec)
sys.modules[module_name] = module
spec.loader.exec_module(module)


global ModuleNum
ModuleNum = 0
def TraverseUeModule(file_path):
    # 列出指定目录中的所有条目
    entries = os.listdir(file_path)

    # TODO 这里统计的内容过少，参考统计UE模块依赖关系重写
    haveBuildcs = False
    moduleName = ""
    # 遍历条目，检查是否为目录
    for entry in entries:
        if entry.endswith('.Build.cs'):
            haveBuildcs = True
            moduleName = entry.split('.Build.cs')[0]

    if haveBuildcs:
        global ModuleNum
        ModuleNum+=1
        module.stat_folder(file_path)
        print('模块序号：',ModuleNum,' 名称：',moduleName)
    else:
        for entry in entries:
            full_path = os.path.join(file_path, entry)
            if os.path.isdir(full_path):
                TraverseUeModule(full_path)


if __name__ == '__main__':
    TraverseUeModule( r'D:\UE_4.27\4.27.2-release\Engine\Source\Runtime')

    print()
    print('总模块数量：',ModuleNum)