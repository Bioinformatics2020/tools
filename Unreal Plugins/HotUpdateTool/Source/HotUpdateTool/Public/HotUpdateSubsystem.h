// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FHotUpdateState.h"
#include "FUpdateManifest.h"

#include "CoreMinimal.h"
#include "TimerManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/Object.h"
#include "UObject/StrongObjectPtr.h"
#include "HotUpdateSubsystem.generated.h"

class UHotUpdatePrimaryData;

/** 宏定义配置项 */
//热更新的资源目录名称，位于ProjectName/Content/下
#define HOTUPDATE_DIRECTORY TEXT("HotUpdate/")

//编辑器模式下运行调试时屏蔽热更新相关代码，可手动关闭此功能便于代码高亮、阅读与跳转
#define HOTUPDATE_NOTCODEREVIEW 1

//某些情况下不允许保存配置文件，此宏定义为false会关闭时间戳校验
#define HOTUPDATE_SAVE_CONFIG 0

//资产可以加载事件回调
DECLARE_DYNAMIC_DELEGATE_OneParam(FHotUpdateModuleLoad, UHotUpdatePrimaryData*, AssetData);

//资产需要卸载事件回调
DECLARE_DYNAMIC_DELEGATE(FHotUpdateModuleUnload);

struct FModuleInfo
{
    //模块基本信息
    FName ModuleName;
    FPrimaryAssetId ModuleId;
    FName SuperModuleName;
    //当前模块的主资产文件
    TStrongObjectPtr<UHotUpdatePrimaryData> PrimaryData;


    //记录模块的加载状态
    enum EModuleState
    {
        Loaded, //主资产已加载
        Loading, //加载中
        Finded //仅引用到模块，但主资产未加载
    } ModuleState;

    //加载完成事件委托
    FHotUpdateModuleLoad OnModuleLoad;
    //卸载完成事件委托
    FHotUpdateModuleUnload OnModuleUnload;

    //避免资源重复加载或资源在加载时卸载
    TWeakPtr<FStreamableHandle> LoadHandle;
};

/**
 * 热更新运行时子系统
 * TODO 修改子系统为UObject单例
 */
UCLASS()
class HOTUPDATETOOL_API UHotUpdateSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    /**
     * 提供简单的子系统访问方法
     * @param WorldContextObject 世界上下文对象
     * @return 找到的子系统
     */
    static UHotUpdateSubsystem* Get(const UObject *WorldContextObject);

    //------模块加载管理-------
public:
    /**
     * 初始化主资产根节点，同时完成Pak文件加载
     * @param InRootPrimaryData 主资产根模块
     * @return 是否初始化成功
     */
    UFUNCTION(BlueprintCallable)
    bool InitRootPrimaryData(UHotUpdatePrimaryData *InRootPrimaryData);
    bool InitRootPrimaryData(const FString& InRootPrimaryDataName);

    /**
     * 加载模块主资产
     * @param ModuleName 模块名称
     * @param OnModuleLoad 模块加载完成回调事件
     * @param OnModuleUnload 模块卸载完成回调事件
     * @return 是否加载成功
     */
    UFUNCTION(BlueprintCallable)
    bool LoadModule(FName ModuleName, FHotUpdateModuleLoad OnModuleLoad, FHotUpdateModuleUnload OnModuleUnload);

    /**
     * 卸载模块主资产，同时自动卸载该模块所有子模块
     * @param ModuleName 模块名称
     * @return 是否卸载成功
     */
    UFUNCTION(BlueprintCallable)
    bool UnLoadModule(FName ModuleName);

    /**
     * 监听热更新状态变更事件
     * @return 返回委托对象，用法形如OnStateChangedEvent().AddUObject(this, &Fun))
     */
    FHotUpdateStateDelegate& OnStateChangedEvent();

private:
    /**
     * 响应模块主资产加载完成事件
     * @param ModuleName 模块名称 
     */
    void OnPrimaryAssetLoaded(FName ModuleName);

    /**
     * 递归卸载模块以及对应的子模块
     * @param ModuleName 模块名称
     * @param Level 递归层数
     * @return 是否全部卸载成功
     */
    bool RecursionUnLoadModule(FName ModuleName, int Level);

    /**
     * 运行时某个Module加载完成之后，需要将当前模块以及对应的子模块加载到热更新系统中，便于子模块的查找
     * @param Module 模块基本信息
     */
    void RegisterModule(const FModuleInfo &Module);

    //主资产根节点
    TStrongObjectPtr<UHotUpdatePrimaryData> RootPrimaryData;

    //模块的详细信息
    TMap<FName, FModuleInfo> AllModuleInfo;

#if WITH_EDITOR && HOTUPDATE_NOTCODEREVIEW
    // 这个数据只在编辑器模式下，用于调试LoadModule
    TMap<FName, UHotUpdatePrimaryData *> EditorAllPrimaryData;
    TMap<FName, FHotUpdateModuleUnload> EditorPrimaryDataUnLoad;
#endif


    //------Pak包更新管理-------
public:
    /**
     * 获取上次更新时间
     * @return UE基本的时间格式
     */
    UFUNCTION(BlueprintCallable)
    FDateTime LoadUpdateTime();

    /**
     * 该函数负责将指定目录下的Pak文件更新到系统中
     * 
     * 注意：如果在更新过程中，特别是MovePak阶段突然关闭程序会导致热更新失败，目前并没有提供失败后的恢复逻辑，
     * 此时会导致一部分模块更新完成，一部分模块未更新，
     * 需要下次启动时重新接收系统的更新信号，重头开始完整的更新流程。
     * 
     * @param UpdatePakDirectory 更新的Pak文件目录,形如 FPaths::ProjectDir() + TEXT("UpdatePak/")
     * @return 是否开始检查更新
     */
    UFUNCTION(BlueprintCallable)
    bool UpdatePak(const FString &UpdatePakDirectory);

    /**
     * 模块卸载完成之后，需要手动将模块从热更新系统中移除
     * 异步的资产卸载时，热更新系统将难以监控卸载的状态，因此需要手动调用该函数
     * 
     * @param ModuleName 卸载完成的模块
     * @param bNextTick 是否在下一帧才完成卸载。流关卡的卸载是在下一帧完成，而不是在流关卡卸载回调的时候。
     */
    UFUNCTION(BlueprintCallable)
    void ModuleUnLoadOver(FName ModuleName, bool bNextTick = false);

private:
    /**
     * 在特定的目录下加载Pak文件
     * @return 是否加载成功
     */
    bool LoadSavedPak();

    /**
     * 加载更新清单并检查资源有效性
     * @param UpdatePakDirectory 更新包目录
     */
    bool LoadManifestAndCheckPak(const FString &UpdatePakDirectory);

    /**
     * 检查Pak是否存在
     * @param Manifest 记录pak更新信息的清单数据 
     * @param UpdatePakDirectory 更新包目录
     * @return Pak是否存在
     */
    bool CheckPak(const FUpdateManifest &Manifest, const FString &UpdatePakDirectory);

    /**
     * 根据Manifest配置卸载需要更新的Pak文件
     * @param Manifest 记录pak更新信息的清单数据
     * @return pak包是否卸载成功
     */
    bool UnLoadPak(const FUpdateManifest &Manifest);

    /**
     * 旧Pak卸载完成后，需要将新Pak移动到本地Pak加载目录，便于下次启动时直接加载
     * @param Manifest 记录pak更新信息的清单数据
     * @param UpdatePakDirectory 更新包目录
     * @return pak包是否移动成功
     */
    bool MovePak(const FUpdateManifest &Manifest, const FString &UpdatePakDirectory);

    /**
     * 根据Manifest重新触发Pak加载操作
     * @param Manifest 记录pak更新信息的清单数据
     * @return pak包是否重新加载成功
     */
    bool ReLoadPak(const FUpdateManifest &Manifest);


    //记录热更新状态
    FHotUpdateState UpdateState;

    //在热更新过程中需要保存的变量
    FUpdateManifest UpdateManifest;
    bool UpdateError = false;


    //------模块更新管理-------
private:
    /**
     * Pak包更新完成之后，触发模块重新加载
     * 注意目前的策略是重新加载时只负责根模块的重新加载，
     * 因为模块加载时会根据内部的缓存信息，自动完成子模块的创建
     * 
     * @param Manifest 记录pak更新信息的清单数据
     * @return 是否开启了重新加载，如果被更新的模块未加载则返回false
     */
    bool ReLoadModule(const FUpdateManifest &Manifest);
    void CreateTaskReLoadModule();

    /**
     * 在下一帧完成GC与模块重新加载
     * @param DependList 需要重新加载的模块列表
     */
    void NextTickLoadModule(const TArray<FName> &DependList);

    /**
     * 检查模块是否全部卸载完成
     * @return 是否卸载成功
     */
    bool CheckModuleUnLoadState();

    //模块依赖树节点信息
    struct NodeInfo
    {
        FName NodeName;
        TArray<NodeInfo*> ChildNode;
        NodeInfo *SuperNode = nullptr;
    };

    /**
     * 对已加载的模块按依赖关系,生成依赖树
     * @param LoadedModule 需要重新加载的模块
     * @return 模块依赖树
     */
    TArray<NodeInfo*> CreateModuleDependTree(const TMap<FName, FModuleInfo> &LoadedModule);

    /**
     * 销毁依赖树，注意每次生成的依赖树必须主动销毁
     * @param ModuleTree 模块依赖树
     */
    void DeleteModuleDependTree(TArray<NodeInfo*> &ModuleTree);

    /**
     * 将依赖树转换为依赖列表，类似堆排序思路，根节点在列表前端
     * @param ModuleTree 模块依赖树
     * @return 模块依赖堆
     */
    TArray<FName> DependTreeToDependList(const TArray<NodeInfo*> &ModuleTree);

    /**
     * 传入已加载的部分模块，获取对应的依赖列表，并返回根节点的数量
     * @param LoadedModule 
     * @param OutRootNum 
     * @return 
     */
    TArray<FName> GetDependList(const TMap<FName, FModuleInfo> &LoadedModule, int &OutRootNum);


    //等待被卸载的模块
    TArray<FName> WaitUnloadModule;

    //------其它debug方法
public:
    /**
     * 输出UObject的引用关系
     * @param Object 需要查找的对象
     * @return 是否存在引用信息
     */
    UFUNCTION(BlueprintCallable)
    static bool PrintReferenced(UObject *Object);
};
