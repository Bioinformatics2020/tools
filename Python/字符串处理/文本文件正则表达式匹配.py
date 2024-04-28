# -*- coding:utf-8 -*-

import re

def filter_content(input_file, output_file, start_str, end_str):
    with open(input_file, 'r', encoding='utf-8') as infile, open(output_file, 'w', encoding='utf-8') as outfile:
        for line in infile:
            match = re.search(f'{start_str}(.*){end_str}$', line)
            if match:
                outfile.write(match.group(1) + '\n')
                outfile.write(':TcpDataEnd' + '\n')
                # print(match.group(1) + '\n')

# 示例用法
input_file = r'C:\Users\Administrator\Downloads\NIC多媒体 - 副本.log'

# FAplMgr::Response:[
start_str = r'FAplMgr::Response:\['
end_str = r'\]'

filtered_suffix = '_已筛选'

output_file = input_file[:input_file.rfind('.')] + filtered_suffix + input_file[input_file.rfind('.'):]
print(output_file)

filter_content(input_file, output_file, start_str, end_str)