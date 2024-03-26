# -*- coding:utf-8 -*-

import os
import pandas as pd

def walkFileR(fileName,findStr,inStr):
    '''
    在文件的指定字符串前方插入字符串(从后向前查找)
    :param file:文件名称
    :param findStr:查找的字符串
    :param inStr:插入的字符串
    '''
    newStr = ""
    with open(fileName,'r') as f:
        fileStr = f.read()
        if findStr != "":
            rPos = fileStr.rfind(findStr)
            newStr = fileStr[0:rPos-1] + inStr + fileStr[rPos-1:]
        else:
            newStr = fileStr + inStr
    with open(fileName,'w') as f:
        f.write(newStr)


# 存储需要插入的数据
# 内容分别为：文件名称 .h文件的查找字符串 .h文件的添加字符串 .cpp文件的查找字符串 .cpp文件的添加字符串
# TABLE = {
#     "RadioA1MainPage":["};","\nhello","","\nhello_cpp"]
# }
def Each(filePath:str,table:map):
    '''
    按照map中的配置信息,批量对一个目录下多个文件进行处理
    在相同文件名称的.h与.cpp文件的特定位置中分别插入不同的数据
    '''
    walks = list(os.walk(filePath))

    for root, dirs, files in walks:
        # 遍历文件列表，进入满足条件的文件
        for f in files:
            txt = os.path.join(root, f)
            # 统计哪些类型的文件
            if txt.find(".h") != -1 :
                className = txt.split('\\')[-1][:-2]
                if className in table.keys():
                    walkFileR(txt,table[className][0],table[className][1])
                
            if txt.find(".cpp") != -1:
                className = txt.split('\\')[-1][:-4]
                if className in table.keys():
                    walkFileR(txt,table[className][2],table[className][3])


def NativeEventStr(className,funInfo:str):
    hStrTemplate = '''
    UFUNCTION(BlueprintNativeEvent)
    void SetFunName(Params);
    '''
    cppStrTemplate = '''
void Uclass::SetFunName_Implementation(Params)
{
}
'''
    FunName = funInfo.split(':')[0]
    Params = ""
    x = funInfo.find(r":")
    if x != -1:
        Params = funInfo.split(':')[1]
    hStrTemplate = hStrTemplate.replace('FunName',FunName)
    hStrTemplate = hStrTemplate.replace('Params',Params)
    cppStrTemplate = cppStrTemplate.replace('FunName',FunName)
    cppStrTemplate = cppStrTemplate.replace('Params',Params)
    cppStrTemplate = cppStrTemplate.replace('class',className)
    return hStrTemplate,cppStrTemplate


def CallableStr(className,funInfo:str):
    hStrTemplate = '''
    UFUNCTION(BlueprintCallable)
    void OnFunNameClicked(Params);
    '''
    cppStrTemplate = '''
void Uclass::OnFunNameClicked(Params)
{
}
'''
    FunName = funInfo.split(':')[0]
    Params = ""
    x = funInfo.find(r":")
    if x != -1:
        Params = funInfo.split(':')[1]
    hStrTemplate = hStrTemplate.replace('FunName',FunName)
    hStrTemplate = hStrTemplate.replace('Params',Params)
    cppStrTemplate = cppStrTemplate.replace('FunName',FunName)
    cppStrTemplate = cppStrTemplate.replace('Params',Params)
    cppStrTemplate = cppStrTemplate.replace('class',className)
    return hStrTemplate,cppStrTemplate


def SetTable(file):
    data = pd.read_excel(file)
    classNames = data.iloc[:,[0]].values
    table = {}
    for name in classNames:
        table[name[0]]=[]
    for i in range(len(classNames)):
        NativeEvent = []
        Callable = []
        if isinstance(data.iloc[i,2],str):
            NativeEvent = data.iloc[i,2].split(';')
        if isinstance(data.iloc[i,1],str):
            Callable = data.iloc[i,1].split(';')
        hAddStr = ""
        cppAddStr = ""
        for funInfo in NativeEvent:
            rev = NativeEventStr(data.iloc[i,0],funInfo)
            hAddStr = hAddStr + rev[0]
            cppAddStr = cppAddStr + rev[1]

        for funInfo in Callable:
            rev = CallableStr(data.iloc[i,0],funInfo)
            hAddStr = hAddStr + rev[0]
            cppAddStr = cppAddStr + rev[1]

        table[data.iloc[i,0]] = ['};',hAddStr,'',cppAddStr]
    return table


def main():
    table = SetTable(r"media重构.xls")
    Each(r"QQMusic",table)
    print("OK")

if __name__ == '__main__':
    main()