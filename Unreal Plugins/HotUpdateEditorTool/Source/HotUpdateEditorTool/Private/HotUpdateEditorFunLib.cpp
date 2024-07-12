// Fill out your copyright notice in the Description page of Project Settings.


#include "HotUpdateEditorFunLib.h"
#include "HotUpdateSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/MonitoredProcess.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "CoreGlobals.h"
#include "Misc/ConfigCacheIni.h"

DEFINE_LOG_CATEGORY(LogHotUpdateEditorTool);

void UHotUpdateEditorFunLib::CreateUpdatePak(const TArray<UHotUpdatePrimaryData*>& HotUpdatePrimaryDatas, const FString& PackageDirectory)
{
	//生成Manifest数据
	FUpdateManifest UpdateManifest;

	FString PakDirectoryName = PackageDirectory + TEXT("Content/Paks/");
	FString UpdatePakDirectory = PackageDirectory + TEXT("../UpdatePak/");
	if(!FPaths::DirectoryExists(PakDirectoryName))
	{
		UE_LOG(LogHotUpdateEditorTool,Error,TEXT("PakDirectoryName %s not exist"),*PakDirectoryName);
		return;
	}
	
	//在指定目录下查找最新的Pak文件
	TArray<FString> Result;
	IFileManager::Get().FindFiles(Result, *(PakDirectoryName+"*"), true, false);

	if(Result.Num() == 0)
	{
		UE_LOG(LogHotUpdateEditorTool,Error,TEXT("not finded PakFile in %s"),*PakDirectoryName);
		return;
	}

	//匹配文件ChunkId
	TArray<FExportChunkInfo> ChunkIdAndNames = GetChunkInfo(HotUpdatePrimaryDatas);
	for(auto& PakFileName:Result)
	{
		//取出文件名中间的ChunkId，PakFile格式为pakchunk1001-platform.pak
		FString RightString = PakFileName.RightChop(8);//去除pakchunk前缀
		FString ChunkIdStr = RightString.Left(RightString.Find(TEXT("-")));//去除platform.pak后缀
		int ChunkId = FCString::Atoi(*ChunkIdStr);
		
		//匹配到的文件需要导出
		FExportChunkInfo* ExportChunk = ChunkIdAndNames.FindByPredicate([ChunkId](const FExportChunkInfo& ChunkIdAndName) {return ChunkIdAndName.ChunkId == ChunkId;});
		if(ExportChunk)
		{
			ExportChunk->PakFileName = PakFileName;
			UpdateManifest.PakFileInfo.Add(*ExportChunk);
		}
	}

	//拷贝需要导出的文件
	for(auto& PakFile:UpdateManifest.PakFileInfo)
	{
		IFileManager::Get().Copy(*(UpdatePakDirectory + PakFile.PakFileName),*(PakDirectoryName + PakFile.PakFileName));
	}
	//生成Manifest文件
	UpdateManifest.SaveToLocal(UpdatePakDirectory);
}

void UHotUpdateEditorFunLib::CreateBasePak(const FString& PackageDirectory)
{
	FString PakDirectoryName = PackageDirectory + TEXT("Content/Paks/");
	if(!FPaths::DirectoryExists(PakDirectoryName))
	{
		UE_LOG(LogHotUpdateEditorTool,Error,TEXT("PakDirectoryName %s not exist"),*PakDirectoryName);
		return;
	}

	//在指定目录下查找最新的Pak文件
	TArray<FString> Result;
	IFileManager::Get().FindFiles(Result, *(PakDirectoryName+"*"), true, true);

	if(Result.Num() == 0)
	{
		UE_LOG(LogHotUpdateEditorTool,Error,TEXT("not finded PakFile in %s"),*PakDirectoryName);
		return;
	}

	TArray<FString> NeedMoveFiles;
	//匹配文件ChunkId,确定需要移动的pak文件
	for(auto& PakFileName:Result)
	{
		//取出文件名中间的ChunkId，PakFile格式为pakchunk1001-platform.pak
		FString RightString = PakFileName.RightChop(8);//去除pakchunk前缀
		FString ChunkIdStr = RightString.Left(RightString.Find(TEXT("-")));//去除platform.pak后缀
		int ChunkId = FCString::Atoi(*ChunkIdStr);
		
		//根模块的包不需要导出
		if(ChunkId != 0)
		{
			NeedMoveFiles.Add(PakFileName);
		}
	}

	if(NeedMoveFiles.Num() <= 0)
	{
		UE_LOG(LogHotUpdateEditorTool,Warning,TEXT("Pak文件移动失败，没有文件需要移动 %s"),*PakDirectoryName);
		return;
	}

	FString TargetDirectory = PackageDirectory + TEXT("Content/") + HOTUPDATE_DIRECTORY + TEXT("/");

	IFileManager::Get().DeleteDirectory(*(TargetDirectory),false,true);
	//移动需要导出的文件
	for(auto& PakFile:NeedMoveFiles)
	{
		IFileManager::Get().Move(*(TargetDirectory + PakFile),*(PakDirectoryName + PakFile));
	}
}

TArray<FExportChunkInfo> UHotUpdateEditorFunLib::GetChunkInfo(const TArray<UHotUpdatePrimaryData*>& HotUpdatePrimaryDatas)
{
	TArray<FExportChunkInfo> ChunkIdAndNames;
	for (auto& PrimaryData : HotUpdatePrimaryDatas)
	{
		int ChunkId = PrimaryData->Rules.ChunkId;
		FExportChunkInfo ChunkIdAndName;
		ChunkIdAndName.ChunkId = ChunkId;
		ChunkIdAndName.ModuleName = PrimaryData->ModuleName;
		ChunkIdAndNames.Add(ChunkIdAndName);
	}
	return ChunkIdAndNames;
}

void UHotUpdateEditorFunLib::SaveUpdateTime()
{
	GConfig->SetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"),*FDateTime::Now().ToString(),GGameIni);
	GConfig->Flush(false,GGameIni);
}

TArray<UHotUpdatePrimaryData*> UHotUpdateEditorFunLib::FindUserPrimaryDate()
{
	//查找所有UHotUpdatePrimaryData资产
	const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	TArray<FAssetData> Assets;
	AssetRegistry.GetAssetsByClass(FName(UHotUpdatePrimaryData::StaticClass()->GetName()), Assets, true);

	//根据查找到的资产信息，返回资产地址
	TArray<UHotUpdatePrimaryData*> OutData;
	for (const FAssetData& AssetData : Assets)
	{
		AssetData.PrintAssetData();
		OutData.Add(TSoftObjectPtr<UHotUpdatePrimaryData>(AssetData.ObjectPath.ToString()).LoadSynchronous());
	}
	return OutData;
}

static FString OutMessage;
static void OnStatusOutput(FString Message)
{
	OutMessage += Message + '\n';
}

FString UHotUpdateEditorFunLib::GatPakFileInfo(const FString& PackFilePath, const FString& Option)
{
	FString CmdExe = TEXT("cmd.exe");
	FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));
	FString FullCommandLine = FString::Printf(TEXT("/c \"\"%s\" %s %s\""), *UnrealPakPath, *PackFilePath, *Option);
	
	TSharedPtr<FMonitoredProcess> UBTProcess = MakeShareable(new FMonitoredProcess(CmdExe, FullCommandLine, true));
	UBTProcess->OnOutput().BindStatic(&OnStatusOutput);
	OutMessage = TEXT("");
	UBTProcess->Launch();
	while(UBTProcess->Update())
	{
		FPlatformProcess::Sleep(0.01f);
	}
	return OutMessage; 
}

TArray<FString> UHotUpdateEditorFunLib::GetAllPak(const FString& PackageDirectory)
{
	FString PakDirectoryName = PackageDirectory + TEXT("Content/Paks/");
	FString PakDirectoryName2 = PackageDirectory + TEXT("Content/") + HOTUPDATE_DIRECTORY + TEXT("/");
	if(!FPaths::DirectoryExists(PakDirectoryName))
	{
		UE_LOG(LogHotUpdateEditorTool,Error,TEXT("PakDirectoryName %s not exist"),*PakDirectoryName);
		return {};
	}
	
	//在指定目录下查找最新的Pak文件
	TArray<FString> BasePak;
	IFileManager::Get().FindFiles(BasePak, *(PakDirectoryName+"*"), true, false);
	for (auto& Pak : BasePak)
	{
		Pak = PakDirectoryName + Pak;
	}

	TArray<FString> BasePak2;
	IFileManager::Get().FindFiles(BasePak2, *(PakDirectoryName2+"*"), true, false);
	for (auto& Pak : BasePak2)
	{
		Pak = PakDirectoryName2 + Pak;
	}
	BasePak.Append(BasePak2);
	return BasePak;
}

TArray<UHotUpdatePrimaryData*> UHotUpdateEditorFunLib::GetAllHotUpdatePrimaryData()
{
	// 获取AssetRegistry模块
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// 查询所有UHotUpdatePrimaryData类型的资产
	TArray<FAssetData> Assets;
	AssetRegistryModule.Get().GetAssetsByClass(UHotUpdatePrimaryData::StaticClass()->GetFName(), Assets, true /*递归搜索子类*/);

	TArray<UHotUpdatePrimaryData*> HotUpdatePrimaryDatas;
	for(const FAssetData& AssetData : Assets)
	{
		UHotUpdatePrimaryData* AssetPtr = TSoftObjectPtr<UHotUpdatePrimaryData>(AssetData.ObjectPath.ToString()).LoadSynchronous();
		checkf(AssetPtr, TEXT("主资产加载失败"));
		if(AssetPtr->ModuleName != TEXT("Root"))
		{
			HotUpdatePrimaryDatas.Add(AssetPtr);
		}
	}
	return HotUpdatePrimaryDatas;
}
