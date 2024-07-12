#pragma once
#include "HotUpdatePrimaryData.h"

struct FExportChunkInfo
{
	int ChunkId;
	FName ModuleName;
	FString PakFileName;
};

class HOTUPDATETOOL_API FUpdateManifest
{
public:
	void SaveToLocal(const FString& UpdatePakDirectory);
	void DeleteManifestFile(const FString& UpdatePakDirectory);

	static FUpdateManifest LoadFromLocal(const FString& UpdatePakDirectory);
	
	FString UpdateTime;
	TArray<FExportChunkInfo> PakFileInfo;
};
