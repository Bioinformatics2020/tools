#coding：utf-8
import sys,os
import SendHelp

help_str = '''
1 [2] [3] 发送第1,2,3条消息
auto      自动发送消息
    start [i]  从i索引处开始，默认是0
    end        结束自动发送消息
    delay i    设置消息的时间间隔,默认0.5
send i    发送第i条消息
next [i]  发送第(lastid+i)条消息,i默认为1
lastid    获取上一条消息的id
getdata i 获取第i条消息对应的数据
setdata i data 设置第i条消息对应的数据
save f    保存解析后的csv文件到f文件 
add [多行] 添加数据
'''

def mySplit(inStr):
    
    i=0
    params=[]
    left = inStr.find("'''",0)
    if left != -1:
        right = inStr.find("'''",left+1)

        strArr = inStr[0:left].split(' ')
        strArr.append(inStr[left+3:right])
        strArr += inStr[right+3:].split(' ')
    else:
        strArr = inStr.split(' ')
    return strArr

def inputData():
    '''输入多行数据,以空行作为结束标志'''
    data=''
    while(True):
        inData = input('')
        if inData=='':
            break
        data+=inData
    return data

if __name__ == '__main__':
    # file = input('从哪个文件开始读取(log/csv):')
    # file = r'c:\Users\Administrator\Downloads\NIC.log'
    file = r'C:\Users\Administrator\Desktop\ZXTools\pythonProject\安卓消息发送工具\zx.csv'
    SendHelp.SendDataSource(file)

    while True:
        inStr = input(':>')
        params = mySplit(inStr)
        if len(params) == 0:
            continue
        try:
            # 连续发送多条数据
            if params[0].isdigit():
                datas = []
                for i in params:
                    datas.append(int(i))
                SendHelp.sendGroup(datas)
            elif params[0] == 'auto':
                if params[1] == 'start':
                    index = 0
                    if len(params)>=3:
                        index = int(params[2])
                    SendHelp.autoSendStart(index)
                elif params[1] == 'end':
                    SendHelp.autoSendEnd()
                elif params[1] == 'delay':
                    SendHelp.autoSendDelay(int(params[2]))
            elif params[0] == 'send':
                SendHelp.sendOne(int(params[1]))
            elif params[0] == 'next':
                index = 1
                if len(params)>=2:
                    index = int(params[1])
                SendHelp.sendNext(index)
            elif params[0] == 'lastid':
                print(SendHelp.lastSendId)
            elif params[0] == 'maxid':
                print(len(SendHelp.data_array))
            elif params[0] == 'getdata':
                SendHelp.getData(params[1])
            elif params[0] == 'setdata':
                SendHelp.setData(params[1],params[2])
            elif params[0] == 'save':
                SendHelp.save(params[1])
            elif params[0] == 'add':
                SendHelp.addData(inputData())
            elif params[0] == 'help':
                print(help_str)
            else:
                SendHelp.sendStr(inStr)
        except NameError:
            print('参数错误')
