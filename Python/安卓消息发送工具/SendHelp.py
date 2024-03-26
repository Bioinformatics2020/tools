#coding：utf-8

import SocketTest
import time
import threading
import extractDateWithRegex


global endAutoSendFlag
endAutoSendFlag = False

global autoDelayTime
autoDelayTime = 2

global data_array
global lastSendId
lastSendId = -1
def autoSend(maxIndex,index):
    global endAutoSendFlag
    endAutoSendFlag = True
    while index<maxIndex and endAutoSendFlag:
        sendOne(index)
        index += 1
        time.sleep(autoDelayTime)

def sendStr(string):
    SocketTest.sendQueue.put(string)

def sendOne(index):
    global lastSendId
    if 0<=index<len(data_array):
        SocketTest.sendQueue.put(data_array[index][3])
        lastSendId = index

def autoSendStart(index = 0):
    global endAutoSendFlag
    if endAutoSendFlag == False:
        t2 = threading.Thread(target=autoSend,args=(len(data_array),index))
        t2.start()

def autoSendEnd():
    global endAutoSendFlag
    endAutoSendFlag = False

def autoSendDelay(delay):
    global autoDelayTime
    autoDelayTime = delay

def SendDataSource(file):
    global data_array

    if file.find('.csv') != -1:
        data_array = extractDateWithRegex.ReadTable(file)
    else:
        data_array = extractDateWithRegex.extract_data_with_regex(file)

    t1 = threading.Thread(target=SocketTest.doworkTcpServer)
    t1.start()

def sendNext(pos):
    newId = lastSendId + pos
    dataLen = len(data_array)
    while 0>newId or newId>=dataLen:
        if 0>newId:
            newId += dataLen
        else:
            newId -= dataLen
    sendOne(newId)

def getData(index):
    return data_array[index][3]

def setData(index,string):
    data_array[index][3] = string

def addData(string):
    lenth = len(data_array)
    data_array.append([lenth,"","",string])

def save(file):
    extractDateWithRegex.WriteTable(data_array,file)

def sendGroup(datas):
    for i in datas:
        sendOne(i)