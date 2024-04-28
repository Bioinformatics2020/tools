import csv
import re

#读取cpp文件，并生成代码
def replaces_file_string(path,startCode,endCode,replacement):
   with open(path, "r",encoding='utf-8') as file:
        original_content = file.read()
        replacement = '\n'+replacement+'\n'
        pattern = re.compile(rf"({re.escape(startCode)})(.*?)(?={re.escape(endCode)})", re.DOTALL)
        result = re.sub(pattern, rf"\1{replacement}", original_content)
   
        file.close()

        with open(path, "w",encoding='utf-8') as filew:
            filew.write(result)
            filew.close()

def LoadCSV():
    filename=r'D:\hryt\NIC_UE4_Engine\HMI\UE4\Content\NIC\Core\Config\DataTable\系统页面切换矩阵.csv'
    data = []
    with open(filename) as csvfile:
        csv_reader = csv.reader(csvfile)  # 使用csv.reader读取csvfile中的文件
        #header = next(csv_reader)        # 读取第一行每一列的标题
        data = []
        for row in csv_reader:            # 将csv 文件中的数据保存到data中
            data_line=[]
            for col in row:
                data_line.append(col)
            data.append(data_line)
        
        '''
            TMap<FString,FString> VehicleControlMap =
            {
                {"VehicleControl","00"}
            };
        ModuleSwitchMatrix =
            {
                {"VehicleControl",VehicleControlMap}
            };
        '''
        col ='        {{"{Name}",{Code}}},\n'
        Map = '''
    TMap<FString,FString> {Name}Map = 
    {{
{Data}    }};'''
        ModuleSwitchMatrix=''
        ModuleState=''
        summarizeMap = '''
    {Name} = 
    {{
{Data}    }};'''
        
        outstr=''

        for i in range(len(data)):
            if(i==0 or data[i][0]==''):
                continue
            out_col_str=''
            for j in range(len(data[i])):
                if(j==0):
                    continue
                out_col_str+=col.format(Name=data[0][j],Code='"'+data[i][j]+'"')
                ''
            outstr+=Map.format(Name=data[i][0].replace('.','_'),Data=out_col_str)

            # 汇总数据
            ModuleSwitchMatrix+=col.format(Name=data[i][0],Code=data[i][0].replace('.','_')+'Map')
            ModuleState+=col.format(Name=data[i][0],Code='FModuleState("'+data[i][0]+'")')

        outstr+=summarizeMap.format(Name='ModuleSwitchMatrix',Data=ModuleSwitchMatrix)
        outstr+=summarizeMap.format(Name='ModuleState',Data=ModuleState)
        print(outstr)
    return outstr
        

if __name__ == '__main__':
    filename=r'D:\hryt\NIC_UE4_Engine\HMI\UE4\Source\NIC\Manager\SystemLayerConfig.cpp'
    replaces_file_string(filename,'#pragma region "Read CSV Config"','#pragma endregion',LoadCSV())