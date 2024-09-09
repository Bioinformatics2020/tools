#pragma once
#include "HotUpdatePrimaryData.h"

//一个比较大的质数数作为hash种子，在自己的项目中建议自己定义一个值
const uint64 ManifestHashSeed = 999599;

/**
 * 每一个需要更新的模块对应的详细信息
 */
struct HOTUPDATETOOL_API FExportChunkInfo
{
    int ChunkId;
    FName ModuleName;
    FString PakFileName;

    //用于比较模块信息是否相同
    bool operator==(const FExportChunkInfo &Other) const;
};

/**
 * 保存与读取热更新生成的更新清单
 */
class HOTUPDATETOOL_API FUpdateManifest
{
public:
#if WITH_EDITOR
    /**
     * 保存更新清单到指定目录
     * @param UpdatePakDirectory 更新清单输出目录
     * @return 是否保存成功
     */
    bool SaveToLocal(const FString &UpdatePakDirectory);
#endif

    /**
     * 删除对应的更新清单文件，清单文件使用完成后应该删除
     * @param UpdatePakDirectory 更新清单所在目录
     */
    bool DeleteManifestFile(const FString &UpdatePakDirectory);

    /**
     * 收到热更新信号后从本地加载更新清单
     * @param UpdatePakDirectory 更新清单所处的目录 
     * @return 创建完成的更新清单类
     */
    static FUpdateManifest LoadFromLocal(const FString &UpdatePakDirectory);

private:
    /**
     * 解析清单文件字符串到FUpdateManifest类中
     * @param ManifestString 清单文件字符串
     * @return 解析完成的更新清单类
     */
    static FUpdateManifest ParseManifestFromString(const FString &ManifestString);
    
public:
    //更新时间
    FString UpdateTime;
    //更新清单详细信息
    TArray<FExportChunkInfo> PakFileInfo;
};