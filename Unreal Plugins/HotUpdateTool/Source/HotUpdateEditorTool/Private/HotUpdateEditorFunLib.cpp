// Fill out your copyright notice in the Description page of Project Settings.


#include "HotUpdateEditorFunLib.h"
#include "HotUpdateSubsystem.h"

#include "CoreGlobals.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/FileManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/MonitoredProcess.h"
#include "Misc/Paths.h"
DEFINE_LOG_CATEGORY(LogHotUpdateEditorTool);

void UHotUpdateEditorFunLib::CreateUpdatePak(const TArray<FExportChunkInfo> &ChunkIdAndNames, const FString &PackageDirectory, const FString &OutDirectory, const FString &Version)
{
    //生成Manifest数据
    FUpdateManifest UpdateManifest;

    const FString PakDirectoryName = PackageDirectory + GInternalProjectName + TEXT("/Content/Paks/");
    const FString UpdatePakDirectory = OutDirectory + TEXT("UpdatePak") + Version + TEXT("/");
    if ( !FPaths::DirectoryExists(PakDirectoryName) )
    {
        UE_LOG(LogHotUpdateEditorTool, Error, TEXT("PakDirectoryName %s not exist"), *PakDirectoryName);
        return;
    }

    //在指定目录下查找最新的Pak文件
    TArray<FString> Result;
    IFileManager::Get().FindFiles(Result, *(PakDirectoryName + "*"), true, false);

    if ( Result.Num() == 0 )
    {
        UE_LOG(LogHotUpdateEditorTool, Error, TEXT("not finded PakFile in %s"), *PakDirectoryName);
        return;
    }

    //匹配文件ChunkId
    for ( auto &PakFileName : Result )
    {
        //取出文件名中间的ChunkId，PakFile格式为pakchunk1001-platform.pak
        FString RightString = PakFileName.RightChop(8);                     //去除pakchunk前缀
        FString ChunkIdStr = RightString.Left(RightString.Find(TEXT("-"))); //去除platform.pak后缀
        int ChunkId = FCString::Atoi(*ChunkIdStr);

        //匹配到的文件需要导出
        const FExportChunkInfo *FindedChunk = ChunkIdAndNames.FindByPredicate([ChunkId](const FExportChunkInfo &ChunkIdAndName) { return ChunkIdAndName.ChunkId == ChunkId; });
        if ( FindedChunk )
        {
            FExportChunkInfo ExportChunk;
            ExportChunk.ChunkId = FindedChunk->ChunkId;
            ExportChunk.ModuleName = FindedChunk->ModuleName;
            ExportChunk.PakFileName = PakFileName;
            UpdateManifest.PakFileInfo.Add(ExportChunk);
        }
    }

    //拷贝需要导出的文件
    for ( const auto &PakFile : UpdateManifest.PakFileInfo )
    {
        IFileManager::Get().Copy(*(UpdatePakDirectory + PakFile.PakFileName), *(PakDirectoryName + PakFile.PakFileName));
    }
    //生成Manifest文件
    UpdateManifest.SaveToLocal(UpdatePakDirectory);
}

void UHotUpdateEditorFunLib::CreateBasePak(const FString &PackageDirectory, const FString &OutDirectory, const FString &Version)
{
    const FString PackageName = TEXT("BasePackage") + Version + TEXT("/");
    //将文件复制一份再进行修改
    {
        //检查源文件是否存在
        if ( !FPaths::DirectoryExists(PackageDirectory) )
        {
            UE_LOG(LogHotUpdateEditorTool, Error, TEXT("PackageDirectory %s not exist"), *PackageDirectory);
            return;
        }

        //检查目标文件是否存在
        if ( !FPaths::DirectoryExists(OutDirectory + PackageName) )
        {
            //目标文件不存在时，需要手动创建目标目录，CopyDirectoryTree不支持自动目录创建
            if ( !IFileManager::Get().MakeDirectory(*(OutDirectory + PackageName), true) )
            {
                UE_LOG(LogHotUpdateEditorTool, Error, TEXT("create OutDirectory failed"));
                return;
            }
        }

        //拷贝Package目录
        if ( !FPlatformFileManager::Get().GetPlatformFile().CopyDirectoryTree(*(OutDirectory + PackageName), *PackageDirectory, true) )
        {
            UE_LOG(LogHotUpdateEditorTool, Error, TEXT("copy PackageDirectory to OutDirectory failed"));
            return;
        }
    }
    //将Pak文件从content/pak目录中拷贝出来，因为这个目录的资产会在游戏启动时自动加载
    //我们希望手动管理并加载pak资产

    //Pak目录检查
    FString PakDirectoryName = OutDirectory + PackageName + GInternalProjectName + TEXT("/Content/Paks/");
    if ( !FPaths::DirectoryExists(PakDirectoryName) )
    {
        UE_LOG(LogHotUpdateEditorTool, Error, TEXT("PakDirectoryName %s not exist"), *PakDirectoryName);
        return;
    }

    //在指定目录下查找最新的Pak文件
    TArray<FString> Result;
    IFileManager::Get().FindFiles(Result, *(PakDirectoryName + "*"), true, true);

    if ( Result.Num() == 0 )
    {
        UE_LOG(LogHotUpdateEditorTool, Error, TEXT("not finded PakFile in %s"), *PakDirectoryName);
        return;
    }

    TArray<FString> NeedMoveFiles;
    //匹配文件ChunkId,确定需要移动的pak文件
    for ( auto &PakFileName : Result )
    {
        //取出文件名中间的ChunkId，PakFile格式为pakchunk1001-platform.pak
        //去除pakchunk前缀
        FString RightString = PakFileName.RightChop(8);
        //去除platform.pak后缀
        FString ChunkIdStr = RightString.Left(RightString.Find(TEXT("-")));
        //转换为整数，便于后方进行比较
        int ChunkId = FCString::Atoi(*ChunkIdStr);

        //根模块的包不需要导出
        //这里我们可以手动指定哪些Pak执行动态加载，目前不需要扩展
        if ( ChunkId != 0 )
        {
            NeedMoveFiles.Add(PakFileName);
        }
    }

    //若没有需要移动的文件，则返回
    if ( NeedMoveFiles.Num() <= 0 )
    {
        UE_LOG(LogHotUpdateEditorTool, Warning, TEXT("Pak file move error, no files need to be moved %s"), *PakDirectoryName);
        return;
    }

    //确定pak文件移动的目标位置
    const FString TargetDirectory = OutDirectory + PackageName + GInternalProjectName + TEXT("/Content/") + HOTUPDATE_DIRECTORY;

    //删除目录原有的数据，避免版本号相同，但是Chunk变更的情况下形成干扰
    IFileManager::Get().DeleteDirectory(*(TargetDirectory), false, true);
    //移动需要导出的文件
    for ( const auto &PakFile : NeedMoveFiles )
    {
        IFileManager::Get().Move(*(TargetDirectory + PakFile), *(PakDirectoryName + PakFile));
    }
}

TArray<FExportChunkInfo> UHotUpdateEditorFunLib::GetChunkInfo(const TArray<UHotUpdatePrimaryData *> &HotUpdatePrimaryData)
{
    TArray<FExportChunkInfo> ChunkInfos;
    for ( auto &PrimaryData : HotUpdatePrimaryData )
    {
        ChunkInfos.Add(GetChunkInfo(PrimaryData));
    }
    return ChunkInfos;
}

FExportChunkInfo UHotUpdateEditorFunLib::GetChunkInfo(const UHotUpdatePrimaryData *HotUpdatePrimaryData)
{
    FExportChunkInfo ChunkInfo;
    ChunkInfo.ChunkId = HotUpdatePrimaryData->Rules.ChunkId;
    ChunkInfo.ModuleName = HotUpdatePrimaryData->ModuleName;
    return ChunkInfo;
}

void UHotUpdateEditorFunLib::SaveUpdateTime()
{
    //每次打包时都需要更新打包的时间戳，热更新包的版本本质上是基于时间戳实现，而不是版本号
    GConfig->SetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"), *FDateTime::Now().ToString(), GGameIni);
    GConfig->Flush(false, GGameIni);
}

TArray<UHotUpdatePrimaryData *> UHotUpdateEditorFunLib::FindUserPrimaryDate()
{
    //查找所有UHotUpdatePrimaryData资产
    const IAssetRegistry &AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
    TArray<FAssetData> Assets;
    AssetRegistry.GetAssetsByClass(FName(UHotUpdatePrimaryData::StaticClass()->GetName()), Assets, true);

    //根据查找到的资产信息，返回资产地址
    TArray<UHotUpdatePrimaryData *> OutData;
    for ( const FAssetData &AssetData : Assets )
    {
        AssetData.PrintAssetData();
        OutData.Add(TSoftObjectPtr<UHotUpdatePrimaryData>(AssetData.ObjectPath.ToString()).LoadSynchronous());
    }
    return OutData;
}

//用于接收UnrealPak.exe程序输出
static FString OutMessage;

static void OnStatusOutput(FString Message)
{
    //返回的数据不包含换行符，需要手动添加
    OutMessage += Message + '\n';
}

FString UHotUpdateEditorFunLib::GatPakFileInfo(const FString &PackFilePath, const FString &Option)
{
    //准备一个cmd命令，运行UnrealPak.exe程序来查询pak信息
    const FString CmdExe = TEXT("cmd.exe");
    const FString UnrealPakPath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries/Win64/UnrealPak.exe"));
    const FString FullCommandLine = FString::Printf(TEXT("/c \"\"%s\" %s %s\""), *UnrealPakPath, *PackFilePath, *Option);

    UE_LOG(LogHotUpdateEditorTool, Log, TEXT("GatPakFileInfo FullCommandLine:%s"), *FullCommandLine);
    //创建命令行执行程序工具
    const TSharedPtr<FMonitoredProcess> UnrealPakProcess = MakeShareable(new FMonitoredProcess(CmdExe, FullCommandLine, true));

    //清空之前的消息数据，绑定输出回调
    OutMessage = TEXT("");
    UnrealPakProcess->OnOutput().BindStatic(&OnStatusOutput);

    //开始执行命令
    UnrealPakProcess->Launch();

    //阻塞执行，直到命令行程序结束
    //这种方法很简单，但是会造成程序卡顿，FMonitoredProcess支持异步执行，可以考虑异步执行
    while ( UnrealPakProcess->Update() )
    {
        ;
    }
    return OutMessage;
}

TArray<FString> UHotUpdateEditorFunLib::FindPakInBasePackage(const FString &BasePackageDirectory)
{
    //按照基础包的目录结构，进入可能包含pak文件的目录
    const FString RootPak = BasePackageDirectory + GInternalProjectName + TEXT("Content/Paks/");
    const FString HotUpdatePak = BasePackageDirectory + TEXT("Content/") + HOTUPDATE_DIRECTORY;
    if ( !FPaths::DirectoryExists(RootPak) )
    {
        UE_LOG(LogHotUpdateEditorTool, Error, TEXT("PakDirectoryName %s not exist"), *RootPak);
        return {};
    }

    //查找Root模块
    TArray<FString> BasePak;
    IFileManager::Get().FindFiles(BasePak, *(RootPak + "*"), true, false);
    for ( auto &Pak : BasePak )
    {
        Pak = RootPak + Pak;
    }

    //查找子模块
    TArray<FString> BasePakChild;
    IFileManager::Get().FindFiles(BasePakChild, *(HotUpdatePak + "*"), true, false);
    for ( auto &Pak : BasePakChild )
    {
        Pak = HotUpdatePak + Pak;
    }
    BasePak.Append(BasePakChild);
    return BasePak;
}

TArray<FString> UHotUpdateEditorFunLib::FindPakInUpdatePackage(const FString &UpdatePackageDirectory)
{
    //查找Root模块
    TArray<FString> UpdatePaks;
    IFileManager::Get().FindFiles(UpdatePaks, *(UpdatePackageDirectory + "*"), true, false);
    for ( auto &Pak : UpdatePaks )
    {
        //更新包内包含Pak文件以外的文件，需要过滤
        if ( Pak.Find(".pak", ESearchCase::IgnoreCase, ESearchDir::FromStart) >= 0 )
        {
            Pak = UpdatePackageDirectory + Pak;
        }
    }
    return UpdatePaks;
}

TArray<UHotUpdatePrimaryData *> UHotUpdateEditorFunLib::GetAllHotUpdatePrimaryData()
{
    // 获取AssetRegistry模块
    const FAssetRegistryModule &AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    // 查询所有UHotUpdatePrimaryData类型的资产
    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssetsByClass(UHotUpdatePrimaryData::StaticClass()->GetFName(), Assets, true /*递归搜索子类*/);

    TArray<UHotUpdatePrimaryData *> HotUpdatePrimaryData;
    for ( const FAssetData &AssetData : Assets )
    {
        UHotUpdatePrimaryData *AssetPtr = TSoftObjectPtr<UHotUpdatePrimaryData>(AssetData.ObjectPath.ToString()).LoadSynchronous();
        if ( !AssetPtr )
        {
            UE_LOG(LogHotUpdateEditorTool, Error, TEXT("asset loading failed"));
            break;
        }
        if ( AssetPtr->ModuleName != TEXT("Root") )
        {
            HotUpdatePrimaryData.Add(AssetPtr);
        }
    }
    return HotUpdatePrimaryData;
}

UHotUpdatePrimaryData *UHotUpdateEditorFunLib::GetRootHotUpdatePrimaryData()
{
    // 获取AssetRegistry模块
    const FAssetRegistryModule &AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    // 查询所有UHotUpdatePrimaryData类型的资产
    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssetsByClass(UHotUpdatePrimaryData::StaticClass()->GetFName(), Assets, true /*递归搜索子类*/);

    TArray<UHotUpdatePrimaryData *> HotUpdatePrimaryDatas;
    for ( const FAssetData &AssetData : Assets )
    {
        UHotUpdatePrimaryData *AssetPtr = TSoftObjectPtr<UHotUpdatePrimaryData>(AssetData.ObjectPath.ToString()).LoadSynchronous();
        if ( !AssetPtr )
        {
            UE_LOG(LogHotUpdateEditorTool, Error, TEXT("asset loading failed"));
            return nullptr;
        }
        if ( AssetPtr->ModuleName == TEXT("Root") )
        {
            return AssetPtr;
        }
    }
    return nullptr;
}