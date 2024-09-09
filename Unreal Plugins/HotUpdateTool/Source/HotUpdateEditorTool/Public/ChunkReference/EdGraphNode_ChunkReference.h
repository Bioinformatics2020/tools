// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/AssetRegistryInterface.h"
#include "UObject/Object.h"
#include "EdGraphNode_ChunkReference.generated.h"

/**
 * 模块查看器的单个节点
 */
UCLASS()
class HOTUPDATEEDITORTOOL_API UEdGraphNode_ChunkReference : public UEdGraphNode
{
    GENERATED_BODY()

public:
    //记录模块内部的资产依赖信息
    struct FAssetDependenciesInfo
    {
        FString AssetName;
        FString ReferenceChunk;
        bool bHard;

        //资产被依赖的类型
        enum
        {
            OtherChunk,
            ChildChunk,
            ParentChunk,
            MultiChunk,
            NoChunk
        } ChunkType;

        //排序比较时，按照资产名称比较
        bool operator<(const FAssetDependenciesInfo &Other) const
        {
            return AssetName < Other.AssetName;
        }
    };

    /**
     * @brief 返回Chunk关联的所有资产
     * @param InternalDependencies 返回模块内部引用资产
     * @param ExternalDependencies 返回模块引用的外部资产,以及资产对应的信息
     */
    void GetDependenciesAsset(TArray<FString> &InternalDependencies, TArray<FAssetDependenciesInfo> &ExternalDependencies);

    /**
     * 初始化当前结点的信息
     * @param InAssetIdentifier 主资产ID
     * @param NodeLoc 节点显示坐标
     */
    void SetupReferenceNode(const FPrimaryAssetId &InAssetIdentifier, const FIntPoint &NodeLoc);

    /**
     * 添加引用的节点，引用节点指向当前节点
     * @param ReferencerNode 被引用的节点
     */
    void AddReferencer(UEdGraphNode_ChunkReference *ReferencerNode);

private:
    /**
     * 判断当前资产是否为硬引用，函数实现参考于引用查看器IsHard lambda
     * @param Properties 资产引用属性
     * @return 是否为硬引用
     */
    bool AssetReferenceIsHard(UE::AssetRegistry::EDependencyProperty Properties);

    /**
     * 根据资源的引用属性，设置节点信息
     * @param Info 需要被修改的节点
     * @param AssetManager 资源管理器，用于查找节点属性
     */
    void SetNodeInfo(FAssetDependenciesInfo& Info,const UAssetManager *AssetManager);
    
protected:
    // UEdGraphNode implementation
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    // End UEdGraphNode implementation

private:
    FPrimaryAssetId AssetIdentifier;

    FText NodeTitle;
    FName AssetName;

    //记录当前节点的ChunkId
    int ChunkId;
    //记录所有子节点或父节点的ChunkId，便于资源引用分析时确定异常资源详细信息
    TSet<int> ChildChunkId;
    TSet<int> ParentChunkId;

    UEdGraphPin *DependencyPin;
    UEdGraphPin *ReferencerPin;
};