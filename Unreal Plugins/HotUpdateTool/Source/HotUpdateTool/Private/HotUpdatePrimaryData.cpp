// Fill out your copyright notice in the Description page of Project Settings.


#include "HotUpdatePrimaryData.h"

#include "Engine/AssetManager.h"
#include "HotUpdateSubsystem.h"
#include "HotUpdateTool.h"
#include "AssetRegistry/AssetRegistryModule.h"

#if WITH_EDITOR
void UHotUpdatePrimaryData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	bool DirectorysChange = PropertyChangedEvent.MemberProperty->GetName() == TEXT("Directorys");
	bool bForceRefreshChange = PropertyChangedEvent.MemberProperty->GetName() == TEXT("bForceRefresh");
	bool OtherAssetsChange = PropertyChangedEvent.MemberProperty->GetName() == TEXT("OtherAssets");
	bool OtherBlueprintsChange = PropertyChangedEvent.MemberProperty->GetName() == TEXT("OtherBlueprints");
	
	if(bForceRefreshChange)
	{
		bForceRefresh = false;
	}
	
	if(DirectorysChange || bForceRefreshChange)
	{
		//自动根据传入路径加载路径内的所有资产
		for(int i=0; i<Directorys.Num(); i++)
		{
			auto &Asset = Directorys[i];
			// 处理资源路径
			FString& Path = Asset.DirectoryName.Path;

			//默认选择路径时输入的是系统全路径，这里转换为content相对路径
			if(Path.Find(TEXT("/Game/")) < 0)
			{
				if(!FPaths::DirectoryExists(Path))
				{
					continue;
				}
				FPaths::MakePathRelativeTo(Path, *FPaths::ProjectContentDir());
				Path = TEXT("/Game/") + Path;
			}

			// 根据路径获取资源
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

			TArray<FAssetData> OutAssets;
			AssetRegistry.GetAssetsByPath(FName(Path), OutAssets, true);
			Asset.DirectoryAssets.Empty();
			Asset.DirectoryBlueprints.Empty();
			
			// 添加到主资产列表中
			for (int j=0; j<OutAssets.Num(); j++)
			{
				FAssetData &AssetData = OutAssets[j];
				if(AssetData.AssetClass == TEXT("Blueprint"))
				{
					TSoftClassPtr<UObject> ObjectPtr(AssetData.ToSoftObjectPath().ToString() + TEXT("_C"));
					Asset.DirectoryBlueprints.Add(AssetData.AssetName, ObjectPtr);
				}
				else
				{
					TSoftObjectPtr<UObject> ObjectPtr(AssetData.ToSoftObjectPath());
					Asset.DirectoryAssets.Add(AssetData.AssetName, ObjectPtr);
				}
			}
		}
	}

	//全部重新注册一次
	if(DirectorysChange || bForceRefreshChange || OtherAssetsChange || OtherBlueprintsChange)
	{
		Register();
	}
}
#endif


UHotUpdatePrimaryData::UHotUpdatePrimaryData()
{
	// 主产默认需要标记未运行时
	bIsRuntimeLabel = true;
}

void UHotUpdatePrimaryData::Register()
{
	// 添加管理的资源到主资产列表中
	ModuleAssetIndex.Empty();
	ExplicitAssets.Empty();
	ExplicitBlueprints.Empty();

	// 目录内的资产
	for(int i=0; i<Directorys.Num(); i++)
	{
		auto &Asset = Directorys[i];
		
		for (const auto& Pair : Asset.DirectoryAssets)
		{
			ExplicitAssets.Add(Pair.Value);
			ModuleAssetIndex.Add(Pair.Key, ExplicitAssets.Num()-1);
		}
		for (const auto& Pair : Asset.DirectoryBlueprints)
		{
			ExplicitBlueprints.Add(Pair.Value);
			ModuleBlueprintIndex.Add(Pair.Key, ExplicitBlueprints.Num()-1);
		}
	}

	// 单独的资产
	for(const auto& Pair : OtherAssets)
	{
		ExplicitAssets.Add(Pair.Value);
		ModuleAssetIndex.Add(Pair.Key, ExplicitAssets.Num()-1);
	}
	for(const auto& Pair : OtherBlueprints)
	{
		ExplicitBlueprints.Add(Pair.Value);
		ModuleBlueprintIndex.Add(Pair.Key, ExplicitBlueprints.Num()-1);
	}
}

TSoftObjectPtr<UObject> UHotUpdatePrimaryData::FindObjectFromName(FName Name)
{
	auto Value = ModuleAssetIndex.Find(Name);
	if(Value)
	{
		if(ExplicitAssets.IsValidIndex(*Value))
		{
			return ExplicitAssets[*Value];
		}
	}
	return nullptr;
}

TSoftClassPtr<UObject> UHotUpdatePrimaryData::FindClassFromName(FName Name)
{
	auto Value = ModuleBlueprintIndex.Find(Name);
	if(Value)
	{
		if(ExplicitBlueprints.IsValidIndex(*Value))
		{
			return ExplicitBlueprints[*Value];
		}
	}
	return nullptr;
}

UClass* UHotUpdatePrimaryData::LoadClassFromName(FName Name)
{
	TSoftClassPtr<UObject> ClassPtr = FindClassFromName(Name);
	if(ClassPtr.IsNull())
	{
		UE_LOG(LogHotUpdateTool,Warning,TEXT("类资产加载失败:%s"),*Name.ToString());
	}
	UClass* Class = ClassPtr.Get();
	if(Class)
	{
		UE_LOG(LogHotUpdateTool,Warning,TEXT("类资产存在于缓存中:%s"),*Name.ToString());
		return Class;
	}
	return ClassPtr.LoadSynchronous();
}
