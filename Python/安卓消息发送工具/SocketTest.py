#coding：utf-8

import socket
import time
import extractDateWithRegex
import queue
import threading

# 用于通信的消息队列
sendQueue = queue.Queue()

def doworkTcpServer():
    server_socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    server_socket.bind(('192.168.0.108',9876))

    server_socket.listen()

    while True:
        client_socket,client_address = server_socket.accept()
        print('开始连接',client_address)

        delayTickTime = 5
        tickTime = delayTickTime
        while True:

            if sendQueue.empty():
                haveSend = False
                if tickTime <= 0:
                    haveSend = True
                    send_str = '!' # 心跳包
                    tickTime = delayTickTime
            else:  
                haveSend = True
                send_str = sendQueue.get()
                tickTime = delayTickTime

            if haveSend:
                try:
                    send_str = 'TcpDataHead:' + send_str + ':TcpDataEnd'
                    client_socket.send(send_str.encode('utf-8'))
                except ConnectionAbortedError:
                    break
                except ConnectionResetError:
                    break
                time.sleep(0.1)
            else:
                time.sleep(0.5)
                tickTime -= 0.1
        client_socket.close()
        print('断开连接,请重新链接并重新发送',client_address)


def doworkUDP():
    UDP_IP = "127.0.0.1" # 目标IP地址
    UDP_PORT = 9876     # 目标端口号

    # 创建一个UDP套接字
    sock = socket.socket(socket.AF_INET, # IPv4
                        socket.SOCK_DGRAM) # UDP

    data_array = extractDateWithRegex.extract_data_with_regex(r'C:\Users\Administrator\Desktop\ue4og\NIC.log')
    last_time = 0
    for data in data_array:
        # 发送数据
        time.sleep((data[0] - last_time)/1000)
        sock.sendto(data[2].encode('utf-8'),(UDP_IP, UDP_PORT))
        last_time = data[0]
        print(data[1])
    
    # 关闭套接字
    sock.close()

if __name__ == '__main__':
    # data_array = extractDateWithRegex.extract_data_with_regex(r'C:\Users\Administrator\Downloads\NIC.log')
    # print('end')
    # print(len(data_array))
    doworkTcpServer()