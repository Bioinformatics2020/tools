#pragma once
#include "HotUpdatePrimaryData.h"

const uint64 ManifestHashSeed = 999599;//一个比较大的数作为hash种子

struct HOTUPDATETOOL_API FExportChunkInfo
{
	int ChunkId;
	FName ModuleName;
	FString PakFileName;

	bool operator==(const FExportChunkInfo& Other) const;
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
