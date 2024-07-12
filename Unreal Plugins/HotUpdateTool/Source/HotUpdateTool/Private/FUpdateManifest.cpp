#include "FUpdateManifest.h"
#include "CoreGlobals.h"
#include "Misc/ConfigCacheIni.h"
#include "HotUpdateTool.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"

void FUpdateManifest::SaveToLocal(const FString& UpdatePakDirectory)
{
	//读取版本时间戳
	GConfig->GetString(TEXT("HotUpdateTool"),TEXT("UpdateTime"),UpdateTime,GGameIni);
	
	FString AllData = UpdateTime + '\n';

	for(int i=0;i<PakFileInfo.Num();i++)
	{
		AllData += PakFileInfo[i].ModuleName.ToString() + ' ' + PakFileInfo[i].PakFileName + '\n';
	}
	FFileHelper::SaveStringToFile(AllData,*(UpdatePakDirectory + TEXT("UpdateManifest.txt")));
}

void FUpdateManifest::DeleteManifestFile(const FString& UpdatePakDirectory)
{
	IFileManager::Get().Delete(*(UpdatePakDirectory + TEXT("UpdateManifest.txt")));
}

FUpdateManifest FUpdateManifest::LoadFromLocal(const FString& UpdatePakDirectory)
{
	FUpdateManifest Manifest;
	
	FString AllData;
	if(FFileHelper::LoadFileToString(AllData,*(UpdatePakDirectory + TEXT("UpdateManifest.txt"))))
	{
		TArray<FString> AllDataLines;
		AllData.ParseIntoArray(AllDataLines,TEXT("\n"),true);
		if(AllDataLines.Num()<1)
		{
			return FUpdateManifest();
		}
		Manifest.UpdateTime = AllDataLines[0];
		for(int i=1;i<AllDataLines.Num();i++)
		{
			TArray<FString> Data;
			AllDataLines[i].ParseIntoArray(Data,TEXT(" "),true);
			if(Data.Num() != 2)
			{
				UE_LOG(LogHotUpdateTool,Error,TEXT("UpdateManifest.txt format error"));
				return FUpdateManifest();
			}
			FExportChunkInfo ExportChunkInfo;
			ExportChunkInfo.ChunkId = 0;
			ExportChunkInfo.ModuleName = FName(*Data[0]);
			ExportChunkInfo.PakFileName = Data[1];
			Manifest.PakFileInfo.Add(ExportChunkInfo);
		}

		if(Manifest.PakFileInfo.Num() == 0)
		{
			UE_LOG(LogHotUpdateTool,Log,TEXT("需要更新的PakFile信息为空，请检查Pak文件是否正确：%s"),*UpdatePakDirectory);
		}
	}
	else
	{
		UE_LOG(LogHotUpdateTool,Error,TEXT("UpdateManifest.txt not found : %s"),*(UpdatePakDirectory + TEXT("UpdateManifest.txt")));
	}
	return Manifest;
}
