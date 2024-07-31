#include "FUpdateManifest.h"
#include "CoreGlobals.h"
#include "HotUpdateTool.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"

bool FExportChunkInfo::operator==(const FExportChunkInfo& Other) const
{
	return ChunkId == Other.ChunkId &&
		ModuleName == Other.ModuleName &&
		PakFileName == Other.PakFileName;
}

void FUpdateManifest::SaveToLocal(const FString& UpdatePakDirectory)
{
	//读取版本时间戳
	GConfig->GetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"), UpdateTime, GGameIni);

	FString AllData = UpdateTime + '\n';

	for (int i = 0; i < PakFileInfo.Num(); i++)
	{
		AllData += PakFileInfo[i].ModuleName.ToString() + ' ' + PakFileInfo[i].PakFileName + '\n';
	}
	const uint64 Hash = CityHash64WithSeed((char*)*AllData, AllData.Len() * sizeof(TCHAR), ManifestHashSeed);

	FFileHelper::SaveStringToFile(AllData, *(UpdatePakDirectory + TEXT("UpdateManifest.txt")));
	FFileHelper::SaveStringToFile(FString::Printf(TEXT("%llu"), Hash),
	                              *(UpdatePakDirectory + TEXT("UpdateManifestHash.txt")));
}

void FUpdateManifest::DeleteManifestFile(const FString& UpdatePakDirectory)
{
	IFileManager::Get().Delete(*(UpdatePakDirectory + TEXT("UpdateManifest.txt")));
}

FUpdateManifest FUpdateManifest::LoadFromLocal(const FString& UpdatePakDirectory)
{
	FUpdateManifest Manifest;

	uint64 UpdateManifestHash = 0;
	FString HashStr;
	if (FFileHelper::LoadFileToString(HashStr, *(FPaths::ProjectDir() + TEXT("UpdateManifestHash.txt"))))
	{
		UpdateManifestHash = FCString::Strtoui64(*HashStr, nullptr, 10);
	}

	FString AllData;
	if (FFileHelper::LoadFileToString(AllData, *(UpdatePakDirectory + TEXT("UpdateManifest.txt"))))
	{
		const uint64 BaseHash = CityHash64WithSeed((char*)*AllData, AllData.Len() * sizeof(TCHAR), ManifestHashSeed);
		if (UpdateManifestHash != BaseHash)
		{
			UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt Hash校验错误"));
			return FUpdateManifest();
		}

		TArray<FString> AllDataLines;
		AllData.ParseIntoArray(AllDataLines,TEXT("\n"), true);
		if (AllDataLines.Num() < 1)
		{
			UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt 文件为空"));
			return FUpdateManifest();
		}
		Manifest.UpdateTime = AllDataLines[0];
		for (int i = 1; i < AllDataLines.Num(); i++)
		{
			TArray<FString> Data;
			AllDataLines[i].ParseIntoArray(Data,TEXT(" "), true);
			if (Data.Num() != 2)
			{
				UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt 文件格式错误"));
				return FUpdateManifest();
			}
			FExportChunkInfo ExportChunkInfo;
			ExportChunkInfo.ChunkId = 0;
			ExportChunkInfo.ModuleName = FName(*Data[0]);
			ExportChunkInfo.PakFileName = Data[1];
			Manifest.PakFileInfo.Add(ExportChunkInfo);
		}

		if (Manifest.PakFileInfo.Num() == 0)
		{
			UE_LOG(LogHotUpdateTool, Log, TEXT("需要更新的PakFile信息为空，请检查Pak文件是否正确：%s"), *UpdatePakDirectory);
		}
	}
	else
	{
		UE_LOG(LogHotUpdateTool, Error, TEXT("UpdateManifest.txt not found : %s"),
		       *(UpdatePakDirectory + TEXT("UpdateManifest.txt")));
	}
	return Manifest;
}
