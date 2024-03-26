#coding：utf-8

import re
import csv

csv.field_size_limit(500 * 1024 * 1024)

def extract_data_with_regex(file_path):
    
    pattern =  '-(\d{2}).(\d{2}).(\d{2}):(\d{3})]\[...\]LogTemp: Response:\(Client recv :\) (.*?)\[202'
    with open(file_path, 'r',encoding='utf-8') as f:
        text = f.read()

    # 使用正则表达式模式查找匹配项
    matches = re.findall(pattern, text, re.DOTALL)

    # 将匹配项保存到一个数组中
    data_array = []
    i = 0
    for match in matches:
        time = ((int(match[0])*60+int(match[1]))*60+int(match[2]))*1000+int(match[3])
        data_str = match[4].replace('\n','')
        data_str = data_str.replace(' ','')

        try:
            addr = re.search('addr":"/(.*?)"',data_str).groups()[0]
        except:
            addr = 'ErrorAddr'
        
        data_one = [i,time,addr,data_str]
        i+=1
        data_array.append(data_one)

    # t0 = data_array[0][1]
    # for i in range(len(data_array)):
    #     data_array[i][1] -= t0
    print('文件读取完成')
    return data_array

# 保存每个匹配项的数据
def WriteTable(data_array,file):
    WriteStr = ',time,Name,Str\n'
    i = 0
        
    for i,j,k,l in data_array:
        str1 = '"' + k.replace('"',r'""') +'"'
        str2 = '"' + l.replace('"',r'""') +'"'
        WriteStr += str(i) + ',' + str(j) + ',' + str1 + ','+ str2 + '\n'
        
    with open(file,'w',encoding='utf-8') as f:
        f.write(WriteStr)

def ReadTable(readFile):
    data=[]
    with open(readFile) as f:
        reader = csv.reader(f)
        header_row = next(reader)
        for line in reader:
            data.append(line)
    return data


if __name__ == '__main__':
    # 打开文件并提取所有匹配项的数据 Client recv :
     # '\(send to Server\) (.*?)\[2023.02'
    # data_array = extract_data_with_regex(r'C:\Users\Administrator\Desktop\ue4og\NIC.log')
    for i in ReadTable(r"TestResponse.csv"):
        print(i)
    