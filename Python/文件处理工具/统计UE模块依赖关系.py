#coding：utf-8
import os
import re

import networkx as nx
import matplotlib.pyplot as plt

def GetUeModuleImpl(file_path,AllModule):
    # 列出指定目录中的所有条目
    entries = os.listdir(file_path)

    # 遍历条目，检查是否为目录
    for entry in entries:
        if entry.endswith('.Build.cs'):
            full_path = os.path.join(file_path, entry)
            AllModule.append((full_path,entry.split('.Build.cs')[0]))
        if entry.endswith('.build.cs'):
            full_path = os.path.join(file_path, entry)
            AllModule.append((full_path,entry.split('.build.cs')[0]))

    for entry in entries:
        full_path = os.path.join(file_path, entry)
        if os.path.isdir(full_path):
            GetUeModuleImpl(full_path,AllModule)

def GetUeModule(file_path):
    AllModule = []
    GetUeModuleImpl(file_path,AllModule)
    return AllModule

def GetChildModules(filename):
    pattern = r'(PublicDependencyModuleNames|PrivateDependencyModuleNames|AddEngineThirdPartyPrivateStaticDependencies).*?;'

    # 打开文件并读取内容
    with open(filename, 'r', encoding='utf-8') as file:
        content = file.read()

        # 使用 finditer 方法找到所有匹配项，并获取整个匹配的字符串
        matches = [match.group(0) for match in re.finditer(pattern, content, re.DOTALL)]

        child_module = []
        # 输出匹配结果
        for match in matches:

            # 正则表达式用于匹配双引号内的内容
            pattern2 = r'"([^"]*)"'

            # 使用 findall 方法找到所有匹配项
            matches2 = re.findall(pattern2, match)
            
            child_module += matches2

        return child_module


def GetModuleReference():
    EngineDir = r'D:\UE_4.27\4.27.2-release\Engine\Source' + '\\'
    # ue中无重复名称模块
    module_reference = {}
    AllModule = GetUeModule( EngineDir + r'Developer')
    for i in AllModule:
        # print(i[0],i[1])
        child_module = GetChildModules(i[0])
        module_reference[i[1]] = ['Developer',child_module]

    AllModule = GetUeModule( EngineDir + r'Editor')
    for i in AllModule:
        child_module = GetChildModules(i[0])
        module_reference[i[1]] = ['Editor',child_module]

    AllModule = GetUeModule( EngineDir + r'Programs')
    for i in AllModule:
        child_module = GetChildModules(i[0])
        module_reference[i[1]] = ['Programs',child_module]

    AllModule = GetUeModule( EngineDir + r'Runtime')
    for i in AllModule:
        child_module = GetChildModules(i[0])
        module_reference[i[1]] = ['Runtime',child_module]

    AllModule = GetUeModule( EngineDir + r'ThirdParty')
    for i in AllModule:
        child_module = GetChildModules(i[0])
        module_reference[i[1]] = ['ThirdParty',child_module]
    return module_reference


if __name__ == '__main__':
    module_reference = GetModuleReference()
    module_reference_lower = {}

    # 转换为小写名称
    for i,j in module_reference.items():
        module_reference_lower[i.lower()] = [j[0],[s.lower() for s in j[1]]]

    module_root = set()
    module_link = set()

    # 处理无效的模块
    new_module_root = []
    for i,j in module_reference_lower.items():
        for k in j[1]:
            if k not in module_reference_lower:
                new_module_root.append(k)
                print(k)
    
    print('new_module_root', len(new_module_root))
    for i in new_module_root:
        module_root.add(i)
    
    print('module_reference_lower', len(module_reference_lower))
    for i,j in module_reference_lower.items():
        remove_module = []
        
        for k in j[1]:
            if k in new_module_root:
                module_link.add((k,i))
                remove_module.append(k)
        
        for k in remove_module:
            j[1].remove(k)


    while len(module_reference_lower) > 0:
        new_module_root = []
        for i,j in module_reference_lower.items():
            if len(j[1]) == 0:
                new_module_root.append(i)
                print(i)

        if 0 == len(new_module_root):
            break
        # print('new_module_root', len(new_module_root))
        for i in new_module_root:
            module_reference_lower.pop(i)
            module_root.add(i)
        
        # print('module_reference_lower', len(module_reference_lower))
        for i,j in module_reference_lower.items():
            remove_module = []
            
            for k in j[1]:
                if k in new_module_root:
                    module_link.add((k,i))
                    remove_module.append(k)
            
            for k in remove_module:
                j[1].remove(k)
        
        print('new_module_root: ', len(new_module_root))
        

    # for i in module_root:
        # print("({1}) #lightblue",i)