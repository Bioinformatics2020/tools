// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Misc/MonitoredProcess.h"
#include "HotUpdateTool/Public/HotUpdatePrimaryData.h"
#include "HotUpdateTool/Public/FUpdateManifest.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HotUpdateEditorFunLib.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogHotUpdateEditorTool, Log, All);


DECLARE_DYNAMIC_DELEGATE_OneParam(FMonitoredProcessOutPut,FString, Output);

/**
 * 
 */
UCLASS()
class HOTUPDATEEDITORTOOL_API UHotUpdateEditorFunLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * @brief 生成更新包
	 * @param HotUpdatePrimaryDatas 需要更新的主资产
	 * @param PakDirectoryName 打包完成后，输出pak的目录，Windows下为ProjectName/Content/Paks
	 * @param UpdatePakDirectory 将希望更新的pak另存到单独的目录，并添加更新列表
	 */
	UFUNCTION(BlueprintCallable)
	static void CreateUpdatePak(const TArray<UHotUpdatePrimaryData*>& HotUpdatePrimaryDatas, const FString& PackageDirectory);

	/**
	 * @brief 生成基础包，将每个子模块Pak包移动好热更新模块目录下下为ProjectName/Content/HOTUPDATE_DIRECTORY
	 * @param PakDirectoryName 打包完成后，输出pak的目录，Windows下为ProjectName/Content/Paks
	 */
	UFUNCTION(BlueprintCallable)
	static void CreateBasePak(const FString& PackageDirectory);

	/**
	 * @brief 设置版本更新时间，应该在打包开始时调用
	 */
	UFUNCTION(BlueprintCallable)
	static void SaveUpdateTime();

	/**
	 * @brief 查找所有热更新的主资产
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	static TArray<UHotUpdatePrimaryData*> FindUserPrimaryDate();
	
	static TArray<FExportChunkInfo> GetChunkInfo(const TArray<UHotUpdatePrimaryData*>& HotUpdatePrimaryDatas);

	/**
	 * @brief 通过UnrealPak工具获取Pak文件信息
	 * @param PackFilePath Pak路径
	 * @param Option 命令行选项
	 * @return 命令输出的信息
	 */
	UFUNCTION(BlueprintCallable)
	static FString GatPakFileInfo(const FString& PackFilePath, const FString& Option);

	/**
	 * @brief 获取所有Pak文件
	 * @param PackageDirectory 打包完成后，输出pak的目录，Windows下为ProjectName/Content/Paks
	 * @return 所有打包出来的Pak文件
	 */
	UFUNCTION(BlueprintCallable)
	static TArray<FString> GetAllPak(const FString& PackageDirectory);

	UFUNCTION(BlueprintCallable)
	static TArray<UHotUpdatePrimaryData*> GetAllHotUpdatePrimaryData();
};
