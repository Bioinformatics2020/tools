// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HotUpdateTool/Public/HotUpdatePrimaryData.h"
#include "HotUpdateTool/Public/FUpdateManifest.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HotUpdateEditorFunLib.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHotUpdateEditorTool, Log, All);

/**
 * 热更新的编辑器工具函数库
 */
UCLASS()
class HOTUPDATEEDITORTOOL_API UHotUpdateEditorFunLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * @brief 生成更新包，从打包完成的资产中提取出需要更新的模块
	 * @param HotUpdatePrimaryDatas 需要更新的模块列表
	 * @param PackageDirectory UE打包工具打包完成后输出package的目录
	 * @param OutDirectory 将UE原始的package转换为更新包之后，更新包的目录
	 */
	static void CreateUpdatePak(const TArray<FExportChunkInfo>& HotUpdatePrimaryDatas, const FString& PackageDirectory, const FString& OutDirectory, const FString& Version);

	/**
	 * @brief 生成基础包，将每个子模块Pak包移动到热更新模块目录下：ProjectName/Content/HOTUPDATE_DIRECTORY
	 * @param PackageDirectory UE打包工具打包完成后输出package的目录
	 * @param OutDirectory 将UE原始的package转换为基础包之后，基础包的目录
	 * @param Version 基础包的版本号
	 */
	static void CreateBasePak(const FString& PackageDirectory, const FString& OutDirectory, const FString& Version);

	//设置版本更新时间，应该在打包开始时调用
	static void SaveUpdateTime();

	/**
	 * @brief 通过UnrealPak工具获取Pak文件信息
	 * @param PackFilePath Pak路径
	 * @param Option 命令行选项
	 * @return 命令输出的信息
	 */
	static FString GatPakFileInfo(const FString& PackFilePath, const FString& Option);

	//查找基础包内所有Pak文件，返回文件全路径
	static TArray<FString> FindPakInBasePackage(const FString& BasePackageDirectory);

	//查找更新包内所有Pak文件，返回文件全路径
	static TArray<FString> FindPakInUpdatePackage(const FString& UpdatePackageDirectory);
	
	//查找需要热更新的主资产对象，返回主资产对象的指针，注意root模块不能热更新
	static TArray<UHotUpdatePrimaryData*> GetAllHotUpdatePrimaryData();

	//查找所有热更新的主资产
	static TArray<UHotUpdatePrimaryData*> FindUserPrimaryDate();
	
	//查找热更新的主资产根模块
	static UHotUpdatePrimaryData* GetRootHotUpdatePrimaryData();

	//批量查找主资产的ChunkID与ModuleName
	static TArray<FExportChunkInfo> GetChunkInfo(const TArray<UHotUpdatePrimaryData*>& HotUpdatePrimaryData);
	
	//查找主资产的ChunkID与ModuleName
	static FExportChunkInfo GetChunkInfo(const UHotUpdatePrimaryData* HotUpdatePrimaryData);
};
