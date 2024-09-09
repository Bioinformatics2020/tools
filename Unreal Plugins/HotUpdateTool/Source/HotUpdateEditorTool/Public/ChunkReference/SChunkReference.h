// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraphNode_ChunkReference.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SScrollBox.h"

class UEdGraph_ChunkReference;
/**
 * Chunk引用查看器，显示主资产的依赖关系，显示每个主资产包括哪些资源，高亮异常引用的资源
 */
class HOTUPDATEEDITORTOOL_API SChunkReference : public SCompoundWidget
{
public:
    /**
     * 定义slate传入参数
     */
    SLATE_BEGIN_ARGS(SChunkReference)
        {
        }

    SLATE_END_ARGS()

    /**
     * 在创建窗口时自动执行，完成窗口详细结构的创建
     * @param InArgs 
     */
    void Construct(const FArguments &InArgs);

    /**
     * slate资产需要手段释放
     */
    ~SChunkReference();

    /**
     * 双击引用节点显示资源详细信息
     * @param Node 被双击的节点，用来查找这个节点对应的详细信息
     */
    void OnNodeDoubleClicked(UEdGraphNode *Node) const;

    /**
     * 使用button显示间接引用的资产
     * @param ExternalDependencies 所有间接引用的资产信息
     */
    void AddExternalDependenciesItem(const TArray<UEdGraphNode_ChunkReference::FAssetDependenciesInfo>& ExternalDependencies) const;

    /**
     * 使用button显示直接引用的资产
     * @param InternalDependencies 所有直接引用的资产信息
     */
    void AddInternalDependenciesItem(const TArray<FString>& InternalDependencies) const;    

    /**
     * 资源详细信息被点击时，跳转资源浏览器，显示当前资源
     * @param AssetPath 被点击的资产路径
     * @return 返回当前点击事件是否被消耗
     */
    FReply OnAssetNameClicked(FString AssetPath) const;

private:
    /**
     * 显示主资产详细信息的控件容器
     */
    TSharedPtr<SScrollBox> NodeInfoBox;

    /**
     * 主资产的引用关系图生成器
     */
    UEdGraph_ChunkReference *GraphObj = nullptr;
};