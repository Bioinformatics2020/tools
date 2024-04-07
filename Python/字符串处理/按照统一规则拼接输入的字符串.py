

def Splice(str1,str2:str):
    newStr2 = ''
    for i in str2:
        if(i == '"'):
            newStr2 += '\\'
        newStr2 += i
    res = r'''
    //zx test response1
    FString str=TEXT("{\"addr\":\"'''+ str1 +r'''\",\"data\":'''+newStr2+r'''}");
UAPLBPLibrary::Response(str);
    '''
    return res


if __name__ == '__main__':
    # str1 = input("str1")
    while(True):
        print('开始拼接')
        str1 = input("str1:")
        str1 = str1.replace('request','response')

        str2 = ''
        inputStr = input('str2:')
        while( inputStr != ''):
            inputStr = input()
            str2 += inputStr
        
        print(Splice(str1,str2))
        
