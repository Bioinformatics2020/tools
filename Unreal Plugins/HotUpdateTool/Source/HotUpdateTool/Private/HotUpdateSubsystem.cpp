// Fill out your copyright notice in the Description page of Project Settings.


#include "HotUpdateSubsystem.h"
#include "HotUpdateTool/Public/HotUpdateTool.h"
#include "HotUpdatePrimaryData.h"
#include "IPlatformFilePak.h"
#include "Engine/AssetManager.h"
#include "CoreGlobals.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/CoreDelegates.h"

UHotUpdateSubsystem* LoadAssetSubsystem = nullptr;

UHotUpdateSubsystem* UHotUpdateSubsystem::Get()
{
	return LoadAssetSubsystem;
}

void UHotUpdateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadAssetSubsystem = this;
}

void UHotUpdateSubsystem::Deinitialize()
{
	LoadAssetSubsystem = nullptr;
	Super::Deinitialize();
}

void UHotUpdateSubsystem::InitRootPrimaryData(UHotUpdatePrimaryData* InRootPrimaryData)
{
	//TODO Pak文件加载时机最好是提前到引擎初始化阶段
	bool bLoadPak = LoadPak();
	if(!bLoadPak)
	{
		UE_LOG(LogHotUpdateTool,Warning,TEXT("加载Pak文件失败，请检查模块的Pak文件是否格式正确"));
		return;
	}
	
	//checkf(InRootPrimaryData.IsValid(),TEXT("RootPrimaryData is invalid!"));
	UHotUpdatePrimaryData* RootData = InRootPrimaryData;
	checkf(RootData,TEXT("RootPrimaryData load failed!"));
	
	RootPrimaryData.Reset(RootData);
	FModuleInfo NewModuleInfo;
	NewModuleInfo.ModuleId = RootData->GetPrimaryAssetId();
	NewModuleInfo.PrimaryData = RootData;
	NewModuleInfo.SuperModuleName = "";
	NewModuleInfo.ModuleState = FModuleInfo::Loaded;
	NewModuleInfo.ModuleName = RootData->ModuleName;
	const FModuleInfo& ModuleInfo = AllModuleInfo.Add(RootData->ModuleName,NewModuleInfo);
	
	RegisterModule(ModuleInfo);
}

void UHotUpdateSubsystem::LoadModule(FName ModuleName, FHotUpdateModuleLoad OnModuleLoad,
	FHotUpdateModuleUnload OnModuleUnload)
{
	if(!RootPrimaryData.IsValid())
	{
		UE_LOG(LogHotUpdateTool,Warning,TEXT("加载%s时根模块未初始化，请执行InitRootPrimaryData"),*ModuleName.ToString());
		return;
	}

	FModuleInfo* Module = AllModuleInfo.Find(ModuleName);

	if(!Module)
	{
		UE_LOG(LogHotUpdateTool,Warning,TEXT("未找到模块 %s，模块未引用"),*ModuleName.ToString());
		return;
	}
	if(Module->ModuleState == FModuleInfo::Loaded)
	{
		UE_LOG(LogHotUpdateTool,Warning,TEXT("重复加载模块 %s"),*Module->ModuleName.ToString());
	}
	else
	{
		if (UAssetManager* Manager = UAssetManager::GetIfValid())
		{
			if(Module->LoadHandle.IsValid() && Module->LoadHandle.Pin()->IsActive())
			{
				UE_LOG(LogHotUpdateTool,Warning,TEXT("模块 %s 正在加载中"),*Module->ModuleName.ToString());
				return;
			}
			Module->OnModuleLoad = OnModuleLoad;
			Module->OnModuleUnload = OnModuleUnload;
			
			Module->LoadHandle = Manager->LoadPrimaryAsset(Module->ModuleId, TArray<FName>(), FStreamableDelegate::CreateLambda([ModuleName,this]()
			{
				FModuleInfo* Module = AllModuleInfo.Find(ModuleName);
				//主资产加载完成
				UObject* AssetObject = UAssetManager::Get().GetPrimaryAssetObject(Module->ModuleId);
				UHotUpdatePrimaryData* PrimaryData = Cast<UHotUpdatePrimaryData>(AssetObject);
				if (PrimaryData)
				{
					//注册模块到子系统
					Module->PrimaryData = PrimaryData;
					Module->ModuleState = FModuleInfo::Loaded;
					
					RegisterModule(*Module);
					UE_LOG(LogHotUpdateTool,Log,TEXT("模块 %s 加载完成"),*Module->ModuleName.ToString());

					Module->LoadHandle.Reset();
					//触发加载完成事件
					Module->OnModuleLoad.ExecuteIfBound(PrimaryData);
				}
			}
			));
		}
	}
}

void UHotUpdateSubsystem::UnLoadModule(FName ModuleName)
{
	//TODO 主动卸载模块，目前不需要
}

void UHotUpdateSubsystem::RegisterModule(const FModuleInfo& Module)
{
	Module.PrimaryData->Register();

	// 加载子模块
	auto& ReferenceModule = Module.PrimaryData->SubModuleAssets;
	for (auto& Asset : ReferenceModule)
	{
		FModuleInfo SubModuleInfo;
		SubModuleInfo.SuperModuleName = Module.ModuleName;
		SubModuleInfo.ModuleId = Asset.Value;
		SubModuleInfo.ModuleState = FModuleInfo::Finded;
		SubModuleInfo.ModuleName = Asset.Key;
		AllModuleInfo.Add(Asset.Key,SubModuleInfo);
	}
}

FDateTime UHotUpdateSubsystem::LoadUpdateTime()
{
	FString UpdateTimeStr;
	FDateTime OutTime;
	if(GConfig->GetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"),UpdateTimeStr,GGameIni))
	{
		FDateTime::Parse(UpdateTimeStr,OutTime);
	}
	return OutTime;
}

bool UHotUpdateSubsystem::LoadPak()
{
	FString PakPath = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY + TEXT("/");
	//在指定目录下查找最新的Pak文件
	TArray<FString> Result;
	IFileManager::Get().FindFiles(Result, *(PakPath+"*"), true, false);

	int MountNum = 0;
	for(const FString& PakFile : Result)
	{
		bool bMount = FCoreDelegates::MountPak.Execute(PakPath + PakFile, -1) != nullptr;
		if (bMount)
		{
			MountNum++;
			UE_LOG(LogHotUpdateTool, Log,TEXT("LoadMountPak success %s"),*PakFile);
		}
		else
		{
			UE_LOG(LogHotUpdateTool, Warning,TEXT("LoadMountPak error %s"),*PakFile);
		}
	}
	return MountNum == Result.Num();
}

bool UHotUpdateSubsystem::UpdatePak(const FString& UpdatePakDirectory)
{
	//加载Manifest
	FUpdateManifest Manifest = FUpdateManifest::LoadFromLocal(UpdatePakDirectory);
	if(Manifest.PakFileInfo.Num() == 0)
	{
		return false;
	}

	//检测版本
	FString UpdateTime;
	GConfig->GetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"),UpdateTime,GGameIni);
	if(UpdateTime == Manifest.UpdateTime)
	{
		UE_LOG(LogHotUpdateTool,Log,TEXT("无需更新，上次更新时间：%s"),*UpdateTime);
		return false;
	}
	//TODO manifest关联的Chunk文件检测

	//更新Pak包
	UnLoadPak(Manifest);
	if(!MovePak(Manifest, UpdatePakDirectory))
	{
		UE_LOG(LogHotUpdateTool,Log,TEXT("移动Pak包失败"));
		return false;
	}
	Manifest.DeleteManifestFile(UpdatePakDirectory);
	if(!ReLoadPak(Manifest))
	{
		UE_LOG(LogHotUpdateTool,Log,TEXT("重新加载Pak包失败"));
		return false;
	}

	//更新当前版本信息
	GConfig->SetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"),*Manifest.UpdateTime,GGameIni);
	GConfig->Flush(false,GGameIni);

	UE_LOG(LogHotUpdateTool,Log,TEXT("Pak包更新成功"));

	//重新加载模块资产
	ReLoadModule(Manifest);
	return true;
}

bool UHotUpdateSubsystem::UnLoadPak(const FUpdateManifest& Manifest)
{
	bool bAllUnMount = true;
	for (auto& PakFile : Manifest.PakFileInfo)
	{
		FString PakPathOnDisk = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY / PakFile.PakFileName;
		bool bUnMount = FCoreDelegates::OnUnmountPak.Execute(PakPathOnDisk);
		if (!bUnMount)
		{
			UE_LOG(LogHotUpdateTool, Warning,TEXT("OnUnMountPak error %s"),*PakPathOnDisk);
			bAllUnMount = false;
		}
	}
	return bAllUnMount;
}

bool UHotUpdateSubsystem::MovePak(const FUpdateManifest& Manifest, const FString& UpdatePakDirectory)
{
	//TODO 这里处理异常Pak恢复逻辑
	bool bAllMove = true;
	for (auto& PakFile : Manifest.PakFileInfo)
	{
		FString PakPathOnDisk = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY / PakFile.PakFileName;
		bool bMove = IFileManager::Get().Move(*PakPathOnDisk,*(UpdatePakDirectory + PakFile.PakFileName));
		if (!bMove)
		{
			UE_LOG(LogHotUpdateTool, Warning,TEXT("MovePak error. Dest:%s Src:%s"),*PakPathOnDisk,*(UpdatePakDirectory + PakFile.PakFileName));
			bAllMove = false;
		}
	}
	return bAllMove;
}

bool UHotUpdateSubsystem::ReLoadPak(const FUpdateManifest& Manifest)
{
	bool bAllMount = true;
	for (auto& PakFile : Manifest.PakFileInfo)
	{
		FString PakPathOnDisk = FPaths::ProjectContentDir() / HOTUPDATE_DIRECTORY / PakFile.PakFileName;
		bool bMount = FCoreDelegates::MountPak.Execute(PakPathOnDisk, 0) != nullptr;
		if (!bMount)
		{
			UE_LOG(LogHotUpdateTool, Warning,TEXT("OnMountPak error %s"),*PakPathOnDisk);
			bAllMount = false;
		}
	}
	return bAllMount;
}

void UHotUpdateSubsystem::ReLoadModule(const FUpdateManifest& Manifest)
{
	TMap<FName,FModuleInfo> NeedUnloadModule;
	//找到需要卸载的模块
	for(auto& Module : AllModuleInfo)
	{
		if(Module.Value.ModuleState == FModuleInfo::Loaded)
		{
			auto* Info = Manifest.PakFileInfo.FindByPredicate([&Module](const FExportChunkInfo& Other){return Other.ModuleName == Module.Key;});
			if(Info)
			{
				NeedUnloadModule.Add(Module.Key,Module.Value);
				//UE_LOG(LogHotUpdateTool,Log,TEXT("NeedUnloadModule %s"),*Info->ModuleName.ToString());
			}
		}
		//TODO 加载中的模块怎么处理？
	}
	if(NeedUnloadModule.Num() == 0)
	{
		UE_LOG(LogHotUpdateTool,Log,TEXT("无需重新加载模块"));
		return;
	}

	//获取模块依赖列表
	int RootModeNum = 0;
	TArray<FName> DependList = GetDependList(NeedUnloadModule,RootModeNum);
	check(RootModeNum != 0);
	check(DependList.Num()>=RootModeNum);
	UE_LOG(LogHotUpdateTool,Log,TEXT("根依赖模块数量:%d,模块依赖列表:"),RootModeNum);
	for (auto Depend : DependList)
	{
		UE_LOG(LogHotUpdateTool,Log,TEXT("%s"),*Depend.ToString());
	}
	
	//卸载模块
	for(int i=DependList.Num()-1; i>=0; i--)
	{
		NeedUnloadModule[DependList[i]].OnModuleUnload.ExecuteIfBound();
	}

	//解除除根模块以外的模块引用
	for(int i=DependList.Num()-1; i>=RootModeNum; i--)
	{
		AllModuleInfo.Remove(DependList[i]);
	}
	
	//设置重新加载的模块的根节点为未加载状态
	for(int i=0; i<RootModeNum; i++)
	{
		const FModuleInfo& Module = AllModuleInfo[DependList[i]];
		AllModuleInfo[Module.ModuleName].ModuleState = FModuleInfo::Finded;
	}

	//强制GC
	CollectGarbage(RF_NoFlags);

	//筛选出需要重新加载的模块的根节点,重新加载根模块
	for(int i=0; i<RootModeNum; i++)
	{
		const FModuleInfo& Module = AllModuleInfo[DependList[i]];
		LoadModule(Module.ModuleName,Module.OnModuleLoad,Module.OnModuleUnload);
	}
}

TArray<UHotUpdateSubsystem::NodeInfo*> UHotUpdateSubsystem::CreateModuleDependTree(const TMap<FName, FModuleInfo>& LoadedModule)
{
	//---------将模块数据转换为简洁的树状结构所需节点
	//这里节点数量不会太多，使用TArray查找更简单
	TArray<NodeInfo*> AllNodes;
	for(auto& Module:LoadedModule)
	{
		NodeInfo* NewNode = new NodeInfo;
		NewNode->NodeName = Module.Key;
		AllNodes.Add(NewNode);
	}

	//---------将每个节点连接到树上
	TArray<NodeInfo*> RootNodes;
	//每个节点需要找到自己的父节点
	for(auto Node : AllNodes)
	{
		FName NodeName = Node->NodeName;
		const FModuleInfo& ModuleInfo = LoadedModule[NodeName];
		FModuleInfo* SuperModule = AllModuleInfo.Find(ModuleInfo.SuperModuleName);
		do
		{
			checkf(SuperModule,TEXT("除根模块外，每个模块的父节点都不应该为空"));
			if(SuperModule->PrimaryData == RootPrimaryData.Get())
			{
				//根节点需要保存到RootNodes中
				RootNodes.Add(Node);
				break;
			}
			FName SuperModuleName = SuperModule->ModuleName;
			if(LoadedModule.Contains(SuperModuleName))
			{
				//找到当前节点的父节点
				auto* FindedNode = AllNodes.FindByPredicate([SuperModuleName](const NodeInfo* Other){return Other->NodeName == SuperModuleName;});
				check(FindedNode);
				(*FindedNode)->ChildNode.Add(Node);
				Node->SuperNode = *FindedNode;
				break;
			}
			SuperModule = AllModuleInfo.Find(SuperModule->SuperModuleName);
		}
		while (true);
	}
	
	return RootNodes;
}

void UHotUpdateSubsystem::DeleteModuleDependTree(TArray<NodeInfo*>& ModuleTree)
{
	//广度优先遍历，delete树的每一个节点
	TArray<NodeInfo*>& CurrentLevelNodes = ModuleTree;
	while(CurrentLevelNodes.Num() > 0)
	{
		TArray<NodeInfo*> NextLevelNodes;
		for(auto& Node : CurrentLevelNodes)
		{
			NextLevelNodes.Append(Node->ChildNode);
			delete Node;
			Node = nullptr;
		}
		CurrentLevelNodes = NextLevelNodes;
	}
}

TArray<FName> UHotUpdateSubsystem::DependTreeToDependList(const TArray<NodeInfo*>& ModuleTree)
{
	TArray<FName> DependList;
	//广度优先遍历，返回树的每一个节点信息
	TArray<NodeInfo*> CurrentLevelNodes = ModuleTree;
	while(CurrentLevelNodes.Num() > 0)
	{
		TArray<NodeInfo*> NextLevelNodes;
		for(auto& Node : CurrentLevelNodes)
		{
			NextLevelNodes.Append(Node->ChildNode);
			DependList.Add(Node->NodeName);
		}
		CurrentLevelNodes = NextLevelNodes;
	}
	return DependList;
}

TArray<FName> UHotUpdateSubsystem::GetDependList(const TMap<FName, FModuleInfo>& LoadedModule, int& OutRootNum)
{
	//以下个步骤可以合并成一个函数，遍历树时会有性能优化，但是结构清晰度降低
	TArray<NodeInfo*> ModuleTree = CreateModuleDependTree(LoadedModule);
	OutRootNum = ModuleTree.Num();
	TArray<FName> DependList = DependTreeToDependList(ModuleTree);
	DeleteModuleDependTree(ModuleTree);
	return DependList;
}

void UHotUpdateSubsystem::PrintReferenced(UObject* Object)
{
	// for (TObjectIterator<APawn> ObjectIt; ObjectIt; ++ObjectIt)
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("对象:%s"),*ObjectIt->GetName());
	// }
	FReferencerInformationList MemoryReferences;
	if(IsReferenced(Object, RF_NoFlags, EInternalObjectFlags::GarbageCollectionKeepFlags, true, &MemoryReferences))
	{
		for(int32 i = 0; i < MemoryReferences.ExternalReferences.Num(); i++)
		{
			UE_LOG(LogTemp,Warning,TEXT("外部引用:%s"),*MemoryReferences.ExternalReferences[i].Referencer->GetName());
		}
		for(int32 i = 0; i < MemoryReferences.InternalReferences.Num(); i++)
		{
			UE_LOG(LogTemp,Warning,TEXT("内部引用:%s"),*MemoryReferences.ExternalReferences[i].Referencer->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("无引用"));
	}
}