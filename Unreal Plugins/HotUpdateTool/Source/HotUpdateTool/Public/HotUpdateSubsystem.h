// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/Object.h"
#include "UObject/StrongObjectPtr.h"
#include "Engine/StreamableManager.h"
#include "FUpdateManifest.h"
#include "HotUpdateSubsystem.generated.h"

class UHotUpdatePrimaryData;

#define HOTUPDATE_DIRECTORY TEXT("HotUpdate")


DECLARE_DYNAMIC_DELEGATE_OneParam(FHotUpdateModuleLoad,UHotUpdatePrimaryData*, AssetData);
DECLARE_DYNAMIC_DELEGATE(FHotUpdateModuleUnload);

struct FModuleInfo
{
	//模块基本信息
	FName ModuleName;
	FPrimaryAssetId ModuleId;
	FName SuperModuleName;
	//当前模块的主资产文件
	UHotUpdatePrimaryData* PrimaryData;

	
	//记录模块的加载状态
	enum EModuleState
	{
		Loaded,//主资产已加载
		Finded//仅引用到模块，但主资产未加载
	}ModuleState;

	//加载完成事件委托
	FHotUpdateModuleLoad OnModuleLoad;
	//卸载完成事件委托
	FHotUpdateModuleUnload OnModuleUnload;
	
	//避免资源重复加载或资源在加载时卸载
	TWeakPtr<FStreamableHandle> LoadHandle;
};

/**
 * 
 */
UCLASS()
class HOTUPDATETOOL_API UHotUpdateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	//提供简单的子系统访问方法
	static UHotUpdateSubsystem* Get();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//------模块加载管理-------
public:
	//初始化主资产根节点，同时完成Pak文件加载
	UFUNCTION(BlueprintCallable)
	void InitRootPrimaryData(UHotUpdatePrimaryData* InRootPrimaryData);

	// 加载模块主资产
	UFUNCTION(BlueprintCallable)
	void LoadModule(FName ModuleName, FHotUpdateModuleLoad OnModuleLoad, FHotUpdateModuleUnload OnModuleUnload);
	
	UFUNCTION(BlueprintCallable)
	void UnLoadModule(FName ModuleName);
	
private:
	void RegisterModule(const FModuleInfo& Module);
	
	//主资产根节点
	TStrongObjectPtr<UHotUpdatePrimaryData> RootPrimaryData;

	//模块的详细信息
	TMap<FName,FModuleInfo> AllModuleInfo;

	
	//------Pak包更新管理-------
public:
	//获取上次更新时间
	UFUNCTION(BlueprintCallable)
	FDateTime LoadUpdateTime();
	
	UFUNCTION(BlueprintCallable)
	bool UpdatePak(const FString& UpdatePakDirectory);
	
private:
	//在特定的目录下加载Pak文件
	bool LoadPak();

	//根据Manifest配置卸载需要更新的Pak文件
	bool UnLoadPak(const FUpdateManifest& Manifest);

	//旧Pak卸载完成后，需要将新Pak移动到本地Pak加载目录，便于下次启动时直接加载
	bool MovePak(const FUpdateManifest& Manifest, const FString& UpdatePakDirectory);

	//根据Manifest重新触发Pak加载操作
	bool ReLoadPak(const FUpdateManifest& Manifest);

	
	//------模块更新管理-------
private:
	//Pak包更新完成之后，触发模块重新加载
	//注意目前的策略是重新加载时只负责根模块的重新加载，
	//因为模块加载时会根据内部的缓存信息，自动完成子模块的创建
	void ReLoadModule(const FUpdateManifest& Manifest);

	//模块依赖树节点信息
	struct NodeInfo
	{
		FName NodeName;
		TArray<NodeInfo*> ChildNode;
		NodeInfo* SuperNode = nullptr;
	};
	//对已加载的模块按依赖关系,生成依赖树
	TArray<NodeInfo*> CreateModuleDependTree(const TMap<FName,FModuleInfo>& LoadedModule);
	
	//销毁依赖树，注意每次生成的依赖树必须主动销毁
	void DeleteModuleDependTree(TArray<NodeInfo*>& ModuleTree);

	//将依赖树转换为依赖列表，类似堆排序思路，根节点在列表前端
	TArray<FName> DependTreeToDependList(const TArray<NodeInfo*>& ModuleTree);

	//传入已加载的部分模块，获取对应的依赖列表，并返回根节点的数量
	TArray<FName> GetDependList(const TMap<FName,FModuleInfo>& LoadedModule, int& OutRootNum);


	//------其它debug方法
public:
	UFUNCTION(BlueprintCallable)
	static void PrintReferenced(UObject* Object);
};
