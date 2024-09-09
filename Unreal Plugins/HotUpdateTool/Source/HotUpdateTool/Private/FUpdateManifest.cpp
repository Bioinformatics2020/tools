#include "FUpdateManifest.h"
#include "HotUpdateTool.h"

#include "CoreGlobals.h"
#include "HAL/FileManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"

bool FExportChunkInfo::operator==(const FExportChunkInfo &Other) const
{
    return ChunkId == Other.ChunkId &&
           ModuleName == Other.ModuleName &&
           PakFileName == Other.PakFileName;
}

#if WITH_EDITOR
bool FUpdateManifest::SaveToLocal(const FString &UpdatePakDirectory)
{
    //读取版本时间戳
    if(!GConfig->GetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"), UpdateTime, GGameIni))
    {
        UpdateTime = FDateTime::Now().ToString();
    }

    //记录需要更新的包文件
    FString AllData = UpdateTime + '\n';
    for ( int i = 0; i < PakFileInfo.Num(); i++ )
    {
        AllData += PakFileInfo[i].ModuleName.ToString() + ' ' + PakFileInfo[i].PakFileName + '\n';
    }
    const uint64 Hash = CityHash64WithSeed((char *)*AllData, AllData.Len() * sizeof(TCHAR), ManifestHashSeed);

    //写入到本地文件
    if(FFileHelper::SaveStringToFile(AllData, *(UpdatePakDirectory + TEXT("UpdateManifest.txt"))))
    {
        if(FFileHelper::SaveStringToFile(FString::Printf(TEXT("%llu"), Hash),
                                  *(UpdatePakDirectory + TEXT("UpdateManifestHash.txt"))))
        {
            return true;
        }
    }
    UE_LOG(LogHotUpdateTool, Warning, TEXT("UpdateManifest.txt save failed"));
    return false;
}
#endif

bool FUpdateManifest::DeleteManifestFile(const FString &UpdatePakDirectory)
{
    if(IFileManager::Get().Delete(*(UpdatePakDirectory + TEXT("UpdateManifest.txt"))))
    {
        if(IFileManager::Get().Delete(*(UpdatePakDirectory + TEXT("UpdateManifestHash.txt"))))
        {
            return true;
        }
    }
    UE_LOG(LogHotUpdateTool, Warning, TEXT("UpdateManifest.txt delete failed"));
    return false;
}

FUpdateManifest FUpdateManifest::LoadFromLocal(const FString &UpdatePakDirectory)
{
    //校验Manifest文件是否完整，Pak文件由UE校验
    uint64 UpdateManifestHash = 0;
    FString HashStr;
    if ( !FFileHelper::LoadFileToString(HashStr, *(UpdatePakDirectory + TEXT("UpdateManifestHash.txt"))) )
    {
        UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifestHash.txt not found : %s"),
               *(UpdatePakDirectory + TEXT("UpdateManifestHash.txt")));
        return FUpdateManifest();
    }
    UpdateManifestHash = FCString::Strtoui64(*HashStr, nullptr, 10);
    
    //加载详细的清单文件
    FString AllData;
    if ( !FFileHelper::LoadFileToString(AllData, *(UpdatePakDirectory + TEXT("UpdateManifest.txt"))) )
    {
        UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt not found : %s"),
               *(UpdatePakDirectory + TEXT("UpdateManifest.txt")));
        return FUpdateManifest();
    }
    //若校验失败则直接返回
    const uint64 BaseHash = CityHash64WithSeed((char *)*AllData, AllData.Len() * sizeof(TCHAR), ManifestHashSeed);
    if ( UpdateManifestHash != BaseHash )
    {
        UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt Hash Verification error"));
        return FUpdateManifest();
    }

    return ParseManifestFromString(AllData);
}

FUpdateManifest FUpdateManifest::ParseManifestFromString(const FString &ManifestString)
{
    FUpdateManifest Manifest;
    
    //若无文件需要更新则直接返回
    TArray<FString> AllDataLines;
    ManifestString.ParseIntoArray(AllDataLines,TEXT("\n"), true);
    if ( AllDataLines.Num() < 1 )
    {
        UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt file is null"));
        return FUpdateManifest();
    }
    
    //解析文件到结构体
    Manifest.UpdateTime = AllDataLines[0];
    for ( int i = 1; i < AllDataLines.Num(); i++ )
    {
        TArray<FString> Data;
        AllDataLines[i].ParseIntoArray(Data,TEXT(" "), true);
        if ( Data.Num() != 2 )
        {
            UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt file format error"));
            return FUpdateManifest();
        }
        FExportChunkInfo ExportChunkInfo;
        ExportChunkInfo.ChunkId = 0;
        ExportChunkInfo.ModuleName = FName(*Data[0]);
        ExportChunkInfo.PakFileName = Data[1];
        Manifest.PakFileInfo.Add(ExportChunkInfo);
    }

    //前方已经校验资产正常情况不应该出现空更新包，这里输出log作为警示
    if ( Manifest.PakFileInfo.Num() == 0 )
    {
        UE_LOG(LogHotUpdateTool, Warning,
               TEXT("The PakFile information that needs to be updated is empty. Please check the Pak file"));
    }
    return Manifest;
}