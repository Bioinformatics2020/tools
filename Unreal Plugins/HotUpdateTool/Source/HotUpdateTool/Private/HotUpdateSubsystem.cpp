// Fill out your copyright notice in the Description page of Project Settings.

#include "HotUpdateSubsystem.h"
#include "HotUpdatePrimaryData.h"
#include "HotUpdateTool/Public/HotUpdateTool.h"

#include "Engine/AssetManager.h"
#include "HAL/FileManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/CoreDelegates.h"

UHotUpdateSubsystem* UHotUpdateSubsystem::Get(const UObject *WorldContextObject)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogHotUpdateTool, Error, TEXT("GetWorldContextObject is invalid!"));
        return nullptr;
    }
    UWorld* WorldContext = WorldContextObject->GetWorld();
    
    if (!WorldContext->IsValidLowLevel())
    {
        UE_LOG(LogHotUpdateTool, Error, TEXT("GetWorld is invalid!"));
        return nullptr;
    }
    
    return WorldContext->GetGameInstance()->GetSubsystem<UHotUpdateSubsystem>();
}

bool UHotUpdateSubsystem::InitRootPrimaryData(UHotUpdatePrimaryData *InRootPrimaryData)
{
    //热更新只在打包后支持
#if WITH_EDITOR && HOTUPDATE_NOTCODEREVIEW
    //查找所有UHotUpdatePrimaryData资产
    const IAssetRegistry &AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
    TArray<FAssetData> Assets;
    AssetRegistry.GetAssetsByClass(FName(UHotUpdatePrimaryData::StaticClass()->GetName()), Assets, true);

    //保存查找到的资产，便于编辑器模式下调用LoadModule
    for ( const FAssetData &AssetData : Assets )
    {
        AssetData.PrintAssetData();
        UHotUpdatePrimaryData *NewData = TSoftObjectPtr<UHotUpdatePrimaryData>(AssetData.ObjectPath.ToString()).LoadSynchronous();
        NewData->Register();
        EditorAllPrimaryData.Add(NewData->ModuleName, NewData);
        NewData->AddToRoot();
    }
    return true;
#endif

    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_InitRootPrimaryData)

    //Pak文件加载时机最好是提前到BeginPlay之前
    if (!LoadSavedPak())
    {
        UE_LOG(LogHotUpdateTool, Error,
               TEXT("Failed to load Pak file, please check if the Pak file format of the module is correct"));
        return false;
    }

    if (!InRootPrimaryData)
    {
        UE_LOG(LogHotUpdateTool, Error, TEXT("RootPrimaryData is invalid!"));
        return false;
    }

    UHotUpdatePrimaryData *RootData = InRootPrimaryData;

    RootPrimaryData.Reset(RootData);
    FModuleInfo NewModuleInfo;
    NewModuleInfo.ModuleId = RootData->GetPrimaryAssetId();
    NewModuleInfo.PrimaryData.Reset(RootData);
    NewModuleInfo.SuperModuleName = "";
    NewModuleInfo.ModuleState = FModuleInfo::Loaded;
    NewModuleInfo.ModuleName = RootData->ModuleName;
    const FModuleInfo &ModuleInfo = AllModuleInfo.Add(RootData->ModuleName, NewModuleInfo);

    RegisterModule(ModuleInfo);
    return true;
}

bool UHotUpdateSubsystem::InitRootPrimaryData(const FString& InRootPrimaryDataName)
{
    UHotUpdatePrimaryData* RootData = LoadObject<UHotUpdatePrimaryData>(nullptr,*InRootPrimaryDataName);
    return InitRootPrimaryData(RootData);
}

bool UHotUpdateSubsystem::LoadModule(FName ModuleName, FHotUpdateModuleLoad OnModuleLoad,
                                     FHotUpdateModuleUnload OnModuleUnload)
{
    //热更新只在打包后支持
#if WITH_EDITOR && HOTUPDATE_NOTCODEREVIEW
    if ( EditorAllPrimaryData.Find(ModuleName) )
    {
        //添加或替换旧元素
        EditorPrimaryDataUnLoad.Add(ModuleName, OnModuleUnload);
        //延迟执行，模拟异步效果
        GetWorld()->GetTimerManager().SetTimerForNextTick([this,ModuleName,OnModuleLoad]()
        {
            OnModuleLoad.ExecuteIfBound(EditorAllPrimaryData[ModuleName]);
        });
    }
    return true;
#endif
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_LoadModule)
    if (!RootPrimaryData.IsValid())
    {
        UE_LOG(LogHotUpdateTool, Error,
               TEXT("The root module was not initialized when loading %s,please execute InitRootPrimaryData"),
               *ModuleName.ToString());
        return false;
    }

    FModuleInfo *Module = AllModuleInfo.Find(ModuleName);

    if (!Module)
    {
        UE_LOG(LogHotUpdateTool, Error,
               TEXT("%s Module not found, module may not have been referenced"), *ModuleName.ToString());
        return false;
    }

    if (Module->ModuleState == FModuleInfo::Loaded)
    {
        UE_LOG(LogHotUpdateTool, Error,
               TEXT("Repeatedly loading modules %s"), *Module->ModuleName.ToString());
        return false;
    }
    else if (Module->ModuleState == FModuleInfo::Loading)
    {
        if (Module->LoadHandle.IsValid() && Module->LoadHandle.Pin()->IsActive())
        {
            UE_LOG(LogHotUpdateTool, Error,
                   TEXT("Module %s Loading in progress"), *Module->ModuleName.ToString());
        }
        else
        {
            UE_LOG(LogHotUpdateTool, Error,
                   TEXT("Module %s Loading ModuleState error"), *Module->ModuleName.ToString());
        }
        return false;
    }

    //应该处于未加载状态
    UAssetManager &Manager = UAssetManager::Get();

    Module->OnModuleLoad = OnModuleLoad;
    Module->OnModuleUnload = OnModuleUnload;
    Module->ModuleState = FModuleInfo::Loading;

    //异步加载主资产
    auto OnLoadComplete = FStreamableDelegate::CreateLambda([ModuleName,this]()
    {
        OnPrimaryAssetLoaded(ModuleName);
    });
    Module->LoadHandle = Manager.LoadPrimaryAsset(Module->ModuleId, TArray<FName>(), OnLoadComplete);

    UE_LOG(LogHotUpdateTool, Log, TEXT("Loading Module %s"), *Module->ModuleName.ToString())
    return true;
}

bool UHotUpdateSubsystem::UnLoadModule(FName ModuleName)
{
    //热更新只在打包后支持
#if WITH_EDITOR && HOTUPDATE_NOTCODEREVIEW
    if ( EditorAllPrimaryData.Find(ModuleName) )
    {
        FHotUpdateModuleUnload OnModuleUnload = EditorPrimaryDataUnLoad[ModuleName];
        //延迟执行，模拟异步效果
        OnModuleUnload.ExecuteIfBound();
    }
    return true;
#endif
    return RecursionUnLoadModule(ModuleName, 0);
}

FHotUpdateStateDelegate& UHotUpdateSubsystem::OnStateChangedEvent()
{
    return UpdateState.OnStateChangedEvent();
}

void UHotUpdateSubsystem::OnPrimaryAssetLoaded(FName ModuleName)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_LoadModuleComplete)
    FModuleInfo *Module = AllModuleInfo.Find(ModuleName);
    //主资产加载完成
    UObject *AssetObject = UAssetManager::Get().GetPrimaryAssetObject(Module->ModuleId);
    UHotUpdatePrimaryData *PrimaryData = Cast<UHotUpdatePrimaryData>(AssetObject);
    if (PrimaryData)
    {
        //注册模块到子系统
        Module->PrimaryData.Reset(PrimaryData);
        Module->ModuleState = FModuleInfo::Loaded;

        RegisterModule(*Module);
        UE_LOG(LogHotUpdateTool, Log,
               TEXT("Module %s Loading completed"), *Module->ModuleName.ToString());

        Module->LoadHandle.Reset();
        //触发加载完成事件
        Module->OnModuleLoad.ExecuteIfBound(PrimaryData);
    }
}

bool UHotUpdateSubsystem::RecursionUnLoadModule(FName ModuleName, int Level)
{
    FModuleInfo *Module = AllModuleInfo.Find(ModuleName);
    if (!Module)
    {
        UE_LOG(LogHotUpdateTool, Warning, TEXT("UnLoadModule %s erroe, no found module"), *ModuleName.ToString());
        return false;
    }
    if (Module->ModuleState == FModuleInfo::Finded)
    {
        UE_LOG(LogHotUpdateTool, Warning, TEXT("UnLoadModule %s erroe, module is not loaded"), *ModuleName.ToString());
        return false;
    }
    bool AllUnload = true;
    Level++;
    //注意，这种递归删除的结构要求模块之间为树状关系，不可两个模块共同管理一个子模块
    //目前已提供模块引用查看器，辅助检查依赖关系
    TArray<FName> SubModuleNames;
    Module->PrimaryData->SubModuleAssets.GetKeys(SubModuleNames);
    for (FName SubModule : SubModuleNames)
    {
        if (!RecursionUnLoadModule(SubModule, Level))
        {
            AllUnload = false;
        }
    }
    if (!Module->OnModuleUnload.ExecuteIfBound())
    {
        UE_LOG(LogHotUpdateTool, Warning, TEXT("UnloadModule No Bind Event %s"), *ModuleName.ToString());
    }
    //模块本身只需要卸载即可，引用关系保留，子模块需要彻底移除引用
    if (Level == 1)
    {
        AllModuleInfo[ModuleName].ModuleState = FModuleInfo::Finded;
        AllModuleInfo[ModuleName].PrimaryData.Reset();
    }
    else
    {
        AllModuleInfo.Remove(ModuleName);
    }
    return AllUnload;
}

void UHotUpdateSubsystem::RegisterModule(const FModuleInfo &Module)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_RegisterModule)
    Module.PrimaryData->Register();

    // 加载子模块
    auto &ReferenceModule = Module.PrimaryData->SubModuleAssets;
    for (auto &Asset : ReferenceModule)
    {
        FModuleInfo SubModuleInfo;
        SubModuleInfo.SuperModuleName = Module.ModuleName;
        SubModuleInfo.ModuleId = Asset.Value;
        SubModuleInfo.ModuleState = FModuleInfo::Finded;
        SubModuleInfo.ModuleName = Asset.Key;
        AllModuleInfo.Add(Asset.Key, SubModuleInfo);
    }
}

FDateTime UHotUpdateSubsystem::LoadUpdateTime()
{
    FString UpdateTimeStr;
    FDateTime OutTime;
#if HOTUPDATE_SAVE_CONFIG
    if ( GConfig->GetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"), UpdateTimeStr, GGameIni) )
    {
        FDateTime::Parse(UpdateTimeStr, OutTime);
    }
#endif
    return OutTime;
}

bool UHotUpdateSubsystem::LoadSavedPak()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_LoadSavedPak)
    FString PakPath = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY;
    //在指定目录下查找最新的Pak文件
    TArray<FString> Result;
    IFileManager::Get().FindFiles(Result, *(PakPath + "*"), true, false);

    int MountNum = 0;
    for (const FString &PakFile : Result)
    {
        bool bMount = FCoreDelegates::MountPak.Execute(PakPath + PakFile, -1) != nullptr;
        if (bMount)
        {
            MountNum++;
            UE_LOG(LogHotUpdateTool, Log, TEXT("LoadMountPak success %s"), *PakFile);
        }
        else
        {
            UE_LOG(LogHotUpdateTool, Error, TEXT("LoadMountPak error %s"), *PakFile);
        }
    }
    return MountNum == Result.Num();
}

bool UHotUpdateSubsystem::UpdatePak(const FString &UpdatePakDirectory)
{
    //热更新只在打包后支持
#if WITH_EDITOR && HOTUPDATE_NOTCODEREVIEW
    return false;
#endif
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_UpdatePak)

    UpdateState.SetStateAndStateProgress(EHotUpdateState::BeginUpdate);
    UpdateError = false;

    //拷贝Pak包到项目目录
    FGraphEventRef Move = FFunctionGraphTask::CreateAndDispatchWhenReady([this,UpdatePakDirectory]()
    {
        //加载Manifest并检查
        if (!LoadManifestAndCheckPak(UpdatePakDirectory))
        {
            UpdateState.SetStateAndStateProgress(EHotUpdateState::Error);
            return;
        }

        //卸载Pak包
        if (!UnLoadPak(UpdateManifest))
        {
            UpdateState.SetStateAndStateProgress(EHotUpdateState::Error);
            return;
        }

        //移动Pak包
        if (!MovePak(UpdateManifest, UpdatePakDirectory))
        {
            UpdateState.SetStateAndStateProgress(EHotUpdateState::Error);
            return;
        }

        //删除Manifest，完成pak拷贝
        if (!UpdateManifest.DeleteManifestFile(UpdatePakDirectory))
        {
            UpdateState.SetStateAndStateProgress(EHotUpdateState::Error);
            return;
        }

        //重新加载Pak包
        if (!ReLoadPak(UpdateManifest))
        {
            UpdateState.SetStateAndStateProgress(EHotUpdateState::Error);
            return;
        }

#if HOTUPDATE_SAVE_CONFIG
        //更新当前版本信息
        GConfig->SetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"), *UpdateManifest.UpdateTime, GGameIni);
        GConfig->Flush(false, GGameIni);
#endif

        UE_LOG(LogHotUpdateTool, Log, TEXT("Pak file updated successfully"));

        //将事件转发回主线程，开始更新模块，模块更新在一帧内完成，并且需要触发GC
        CreateTaskReLoadModule();
    });

    return true;
}

void UHotUpdateSubsystem::ModuleUnLoadOver(FName ModuleName, bool bNextTick)
{
    UE_LOG(LogHotUpdateTool, Log, TEXT("ModuleUnLoadOver %s, bNextTick %d"), *ModuleName.ToString(), bNextTick);
    if (bNextTick)
    {
        //延迟到下一帧执行
        GetWorld()->GetTimerManager().SetTimerForNextTick([this,ModuleName]()
        {
            WaitUnloadModule.Remove(ModuleName);
        });
    }
    else
    {
        WaitUnloadModule.Remove(ModuleName);
    }
}

bool UHotUpdateSubsystem::CheckPak(const FUpdateManifest &Manifest, const FString &UpdatePakDirectory)
{
    bool bAllCheck = true;
    for (auto &PakFile : Manifest.PakFileInfo)
    {
        //Check文件是否在更新文件中存在，如果需要，也可以在这里补充Check hash值、文件大小等属性
        FString PakPathOnDisk = UpdatePakDirectory + PakFile.PakFileName;
        const bool bHaveFile = FPaths::FileExists(PakPathOnDisk);
        if (!bHaveFile)
        {
            UE_LOG(LogHotUpdateTool, Error, TEXT("OnCheckPak error %s"), *PakPathOnDisk);
            bAllCheck = false;
        }
    }
    return bAllCheck;
}

bool UHotUpdateSubsystem::LoadManifestAndCheckPak(const FString &UpdatePakDirectory)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_LoadManifest)
    UpdateState.SetStateAndStateProgress(EHotUpdateState::LoadManifest);

    //加载Manifest
    UpdateManifest = FUpdateManifest::LoadFromLocal(UpdatePakDirectory);
    if (UpdateManifest.PakFileInfo.Num() == 0)
    {
        return false;
    }

#if HOTUPDATE_SAVE_CONFIG
    //检测版本时间戳
    FString UpdateTime;
    GConfig->GetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"), UpdateTime, GGameIni);
    if ( UpdateTime == UpdateManifest.UpdateTime )
    {
        UE_LOG(LogHotUpdateTool, Log, TEXT("No update required, last update time:%s"), *UpdateTime);
        UpdateState.SetStateAndStateProgress(EHotUpdateState::Error);
        return false;
    }
#endif

    //检查Pak文件是否存在
    if (!CheckPak(UpdateManifest, UpdatePakDirectory))
    {
        return false;
    }
    return true;
}

bool UHotUpdateSubsystem::UnLoadPak(const FUpdateManifest &Manifest)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_UnLoadPak)
    UpdateState.SetStateAndStateProgress(EHotUpdateState::UnLoadPak);

    bool bAllUnMount = true;
    for (auto &PakFile : Manifest.PakFileInfo)
    {
        FString PakPathOnDisk = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY + PakFile.PakFileName;
        //Check文件是否存在，避免新Pak没有对应的Pak文件需要卸载
        const bool bHaveFile = FPaths::FileExists(PakPathOnDisk);
        if (!bHaveFile)
        {
            UE_LOG(LogHotUpdateTool, Log, TEXT("OldPak is not found %s"), *PakPathOnDisk);
            continue;
        }
        //卸载旧Pak包
        const bool bUnMount = FCoreDelegates::OnUnmountPak.Execute(PakPathOnDisk);
        if (!bUnMount)
        {
            //Pak文件有可能卸载失败
            //需要调研Pak何时失败，以及为什么失败，目前的做法是暴露这个问题，遇到了再解决
            //我们需要解决卸载失败的所有情况，或者万一失败时取消本次更新
            UE_LOG(LogHotUpdateTool, Error, TEXT("OnUnMountPak error %s"), *PakPathOnDisk);
            bAllUnMount = false;
        }
    }

    return bAllUnMount;
}

bool UHotUpdateSubsystem::MovePak(const FUpdateManifest &Manifest, const FString &UpdatePakDirectory)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Hot_MovePak)
    UpdateState.SetStateAndStateProgress(EHotUpdateState::MovePak);

    bool bAllMove = true;
    for (auto &PakFile : Manifest.PakFileInfo)
    {
        FString PakPathOnDisk = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY + PakFile.PakFileName;
        bool bMove = IFileManager::Get().Move(*PakPathOnDisk, *(UpdatePakDirectory + PakFile.PakFileName));
        if (!bMove)
        {
            //前面已经校验过Pak文件有效性，此时移动文件不应该会失败
            //如果失败大概率会是由系统原因引起，也可能是旧Pak依然被程序占用，未卸载成功。
            //如果出现问题，再针对性分析。
            UE_LOG(LogHotUpdateTool, Error, TEXT("MovePak error. Dest:%s Src:%s"), *PakPathOnDisk,
                   *(UpdatePakDirectory + PakFile.PakFileName));
            bAllMove = false;
        }
    }

    return bAllMove;
}

bool UHotUpdateSubsystem::ReLoadPak(const FUpdateManifest &Manifest)
{
    UpdateState.SetStateAndStateProgress(EHotUpdateState::ReLoadPak);
    bool bAllMount = true;
    for (auto &PakFile : Manifest.PakFileInfo)
    {
        FString PakPathOnDisk = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY + PakFile.PakFileName;
        const bool bMount = FCoreDelegates::MountPak.Execute(PakPathOnDisk, 0) != nullptr;
        if (!bMount)
        {
            UE_LOG(LogHotUpdateTool, Error, TEXT("OnMountPak error %s"), *PakPathOnDisk);
            bAllMount = false;
        }
    }
    return bAllMount;
}

bool UHotUpdateSubsystem::ReLoadModule(const FUpdateManifest &Manifest)
{
    //1. 完成资源卸载，并且需要考虑异步的资源卸载进度，特别是Level
    //2. 等待所有资源卸载完成，重新开始加载模块

    UpdateState.SetStateAndStateProgress(EHotUpdateState::UnLoadModule);
    TMap<FName, FModuleInfo> NeedUnloadModule;
    //找到需要卸载的模块
    for (auto &Module : AllModuleInfo)
    {
        //如果已经加载完成则准备卸载
        if (Module.Value.ModuleState == FModuleInfo::Loaded)
        {
            auto *Info = Manifest.PakFileInfo.FindByPredicate([&Module](const FExportChunkInfo &Other)
            {
                return Other.ModuleName == Module.Key;
            });
            if (Info)
            {
                NeedUnloadModule.Add(Module.Key, Module.Value);
            }
        }
        //如果尚未加载完成则取消加载
        else if (Module.Value.ModuleState == FModuleInfo::Loading)
        {
            if (Module.Value.LoadHandle.IsValid())
            {
                Module.Value.LoadHandle.Pin()->CancelHandle();
            }
        }
    }
    if (NeedUnloadModule.Num() == 0)
    {
        UE_LOG(LogHotUpdateTool, Log, TEXT("No need to reload the module"));
        UpdateState.SetStateAndStateProgress(EHotUpdateState::Complete);
        return false;
    }

    //获取模块依赖列表
    //不需要拿到所有依赖关系，只需要拿到根节点即可，这个算法可以优化
    int RootModeNum = 0;
    TArray<FName> DependList = GetDependList(NeedUnloadModule, RootModeNum);
    UE_LOG(LogHotUpdateTool, Log, TEXT("Number of root dependent modules:%d ,Module Dependency List:"), RootModeNum);
    for (auto Depend : DependList)
    {
        UE_LOG(LogHotUpdateTool, Log, TEXT("%s"), *Depend.ToString());
    }

    //卸载模块
    //解除除根模块以外的模块引用	
    //设置重新加载的模块的根节点为未加载状态
    for (int i = 0; i < RootModeNum; i++)
    {
        WaitUnloadModule.Add(DependList[i]);
        UnLoadModule(DependList[i]);
    }

    //TODO 检查模块是否卸载干净，处理模块卸载失败的问题
    //模块卸载失败时需要抛出失败原因，告诉开发是哪个模块哪个资源引用导致的。
    //模块卸载失败的问题并不致命，跳过这个模块的卸载依然可以运行，此时会从之前缓存的资源中重新加载模块
    //模块卸载失败的同时有可能会导致子模块的对父模块的引用出现问题，例如子模块更新完成但是父模块没有更新
    //此时建议重启程序，避免引起特殊问题。由于因为Pak文件已经更新成功，重启之后这个问题不会出现。
    //或者选择跨模块调用时提供充足的资源检查，那么这个问题也不会产生严重后果。

    //按照目前的需求，我只需要重新加载根模块即可，子模块会在根模块初始化时自动加载
    DependList.SetNum(RootModeNum);
    NextTickLoadModule(DependList);
    return true;
}

void UHotUpdateSubsystem::CreateTaskReLoadModule()
{
    //将事件转发回主线程，开始更新模块，模块更新在一帧内完成，并且需要触发GC
    FGraphEventRef LoadModule = FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
    {
        //使用TaskGraph将消息转发回主线程时，最好是在下一帧再执行。
        //如果在当前帧的最后一次Wait for Event前，AnyThread的任务完成执行，
        //此时CPU接收到新任务会继续执行任务，导致当前帧过于延长。
        //这种现象在AnyThread的任务时长较短的情况下很容易发生，所以需要避免。

        //FTicker::GetCoreTicker()会在一帧的结尾执行，无法使用
        //TimerManager在WorldTick时执行，若执行时间过长时，会自动减少Wait for Event的CPU空转时间
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(Hot_ReLoadModule)
            ReLoadModule(UpdateManifest);
        });
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

void UHotUpdateSubsystem::NextTickLoadModule(const TArray<FName> &DependList)
{
    //类似ReLoadModule的执行策略，将GC放到下一帧进行，尽量减少Game线程每一帧的计算量
    GetWorld()->GetTimerManager().SetTimerForNextTick([this,DependList]()
    {
        if (CheckModuleUnLoadState())
        {
            //如果所有模块的未卸载完成则继续等待
            NextTickLoadModule(DependList);
            UE_LOG(LogHotUpdateTool, Log, TEXT("NextTickLoadModule WaitUnloadModule.Num:%d"), WaitUnloadModule.Num());
            return;
        }
        UE_LOG(LogHotUpdateTool, Log, TEXT("NextTickLoadModule ReloadModule"));
        UpdateState.SetStateAndStateProgress(EHotUpdateState::ModuleGC);
        //强制GC
        CollectGarbage(RF_NoFlags);

        UpdateState.SetStateAndStateProgress(EHotUpdateState::LoadRootModule);
        //筛选出需要重新加载的模块的根节点,重新加载根模块
        for (auto Depend : DependList)
        {
            const FModuleInfo &Module = AllModuleInfo[Depend];
            LoadModule(Module.ModuleName, Module.OnModuleLoad, Module.OnModuleUnload);
        }
        //结束热更新状态
        UpdateState.SetStateAndStateProgress(EHotUpdateState::Complete);
    });
}

bool UHotUpdateSubsystem::CheckModuleUnLoadState()
{
    return WaitUnloadModule.Num() != 0;
}

TArray<UHotUpdateSubsystem::NodeInfo*> UHotUpdateSubsystem::CreateModuleDependTree(
    const TMap<FName, FModuleInfo> &LoadedModule)
{
    //---------将模块数据转换为简洁的树状结构所需节点
    //这里节点数量不会太多，使用TArray查找更简单
    TArray<NodeInfo*> AllNodes;
    for (auto &Module : LoadedModule)
    {
        //这里的new不会失败，因为是调用的UE重载后的内存分配器
        NodeInfo *NewNode = new NodeInfo;
        NewNode->NodeName = Module.Key;
        AllNodes.Add(NewNode);
    }

    //---------将每个节点连接到树上
    TArray<NodeInfo*> RootNodes;
    //每个节点需要找到自己的父节点
    for (auto Node : AllNodes)
    {
        FName NodeName = Node->NodeName;
        const FModuleInfo &ModuleInfo = LoadedModule[NodeName];
        FModuleInfo *SuperModule = AllModuleInfo.Find(ModuleInfo.SuperModuleName);
        do
        {
            if (!SuperModule)
            {
                UE_LOG(LogHotUpdateTool, Error,
                       TEXT("Except for the root module, the parent node of each module should not be empty"));
            }
            if (SuperModule->PrimaryData == RootPrimaryData)
            {
                //根节点需要保存到RootNodes中
                RootNodes.Add(Node);
                break;
            }
            FName SuperModuleName = SuperModule->ModuleName;
            if (LoadedModule.Contains(SuperModuleName))
            {
                //找到当前节点的父节点
                auto *FoundNode = AllNodes.FindByPredicate([SuperModuleName](const NodeInfo *Other)
                {
                    return Other->NodeName == SuperModuleName;
                });
                (*FoundNode)->ChildNode.Add(Node);
                Node->SuperNode = *FoundNode;
                break;
            }
            SuperModule = AllModuleInfo.Find(SuperModule->SuperModuleName);
        }
        while (true);
    }

    return RootNodes;
}

void UHotUpdateSubsystem::DeleteModuleDependTree(TArray<NodeInfo*> &ModuleTree)
{
    //广度优先遍历，delete树的每一个节点
    TArray<NodeInfo*> &CurrentLevelNodes = ModuleTree;
    while (CurrentLevelNodes.Num() > 0)
    {
        TArray<NodeInfo*> NextLevelNodes;
        for (auto &Node : CurrentLevelNodes)
        {
            NextLevelNodes.Append(Node->ChildNode);
            delete Node;
            Node = nullptr;
        }
        CurrentLevelNodes = NextLevelNodes;
    }
}

TArray<FName> UHotUpdateSubsystem::DependTreeToDependList(const TArray<NodeInfo*> &ModuleTree)
{
    TArray<FName> DependList;
    //广度优先遍历，返回树的每一个节点信息
    TArray<NodeInfo*> CurrentLevelNodes = ModuleTree;
    while (CurrentLevelNodes.Num() > 0)
    {
        TArray<NodeInfo*> NextLevelNodes;
        for (auto &Node : CurrentLevelNodes)
        {
            NextLevelNodes.Append(Node->ChildNode);
            DependList.Add(Node->NodeName);
        }
        CurrentLevelNodes = NextLevelNodes;
    }
    return DependList;
}

TArray<FName> UHotUpdateSubsystem::GetDependList(const TMap<FName, FModuleInfo> &LoadedModule, int &OutRootNum)
{
    //以下个步骤可以合并成一个函数，遍历树时会有性能优化，但是结构清晰度降低
    TArray<NodeInfo*> ModuleTree = CreateModuleDependTree(LoadedModule);
    OutRootNum = ModuleTree.Num();
    TArray<FName> DependList = DependTreeToDependList(ModuleTree);
    DeleteModuleDependTree(ModuleTree);
    return DependList;
}

bool UHotUpdateSubsystem::PrintReferenced(UObject *Object)
{
    if (!IsValid(Object))
    {
        return false;
    }
    //输出对象的引用信息
    FReferencerInformationList MemoryReferences;
    if (IsReferenced(Object, RF_NoFlags, EInternalObjectFlags::GarbageCollectionKeepFlags, true, &MemoryReferences))
    {
        for (int32 i = 0; i < MemoryReferences.ExternalReferences.Num(); i++)
        {
            UE_LOG(LogTemp, Warning, TEXT("External references:%s"),
                   *MemoryReferences.ExternalReferences[i].Referencer->GetName());
        }
        for (int32 i = 0; i < MemoryReferences.InternalReferences.Num(); i++)
        {
            UE_LOG(LogTemp, Warning, TEXT("Internal references:%s"),
                   *MemoryReferences.ExternalReferences[i].Referencer->GetName());
        }
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No reference"));
        return false;
    }
}
