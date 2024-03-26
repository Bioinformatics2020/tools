import os
# -*- coding:utf-8 -*-

stringH = '''// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FileName.generated.h"

/**
 * 
 */
UCLASS()
class NIC_API UFileName : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
};'''
stringCpp = '''// Fill out your copyright notice in the Description page of Project Settings.


#include "FileName.h"

void UFileName::NativeConstruct()
{
	Super::NativeConstruct();
}

void UFileName::NativeDestruct()
{
	Super::NativeDestruct();
}'''

# 遍历文件夹
def walkFile(file):
    # for root, dirs, files in os.walk(file):
    #     # root 表示当前正在访问的文件夹路径
    #     # dirs 表示该文件夹下的子目录名list
    #     # files 表示该文件夹下的文件list
    #     # 遍历文件
    #     for f in files:
    #         # print(os.path.join(root, f))
    #         txt = os.path.join(root, f)

    #         # 创建新文件，移除旧文件
    #         x = txt.find(r".uasset")
    #         if x != -1:
    #             a = txt.rfind("\\")
    #             filePath = txt[0:a+1]
    #             fileName = txt[a+5:-7]
    #             f1 = open(filePath + fileName +".h", "x")
    #             f2 = open(filePath + fileName +".cpp", "x")

                # os.remove(txt)

    for root, dirs, files in os.walk(file):   
        for f in files:
            txt = os.path.join(root, f)
            # 写入新文件
            x = txt.find(".h")
            if x!= -1:
                a = txt.rfind("\\")
                fileName = txt[a+1:-2]
                str = stringH
                str2 = str.replace("FileName",fileName)
                f = open(txt, "w")
                f.write(str2)

            x = txt.find(".cpp")
            if x!= -1:
                a = txt.rfind("\\")
                fileName = txt[a+1:-4]
                str = stringCpp
                str2 = str.replace("FileName",fileName)
                f = open(txt, "w")
                f.write(str2)

        # 遍历所有的文件夹
        # for d in dirs:
            # print(os.path.join(root, d))
            

def main():
    walkFile(r"Base")
    print("OK")
if __name__ == '__main__':
    main()