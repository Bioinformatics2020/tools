// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EdGraph_ChunkReference.generated.h"

class UEdGraphNode_ChunkReference;

//记录当前模块的关键信息
struct FPrimaryAssetDependencies
{
	//当前模块所依赖的子模块数量，递归包含子模块
	int LeafNodeCount;
	//当前模块直接依赖的子模块，不递归
	TArray<FPrimaryAssetId> Identifiers;
	//当前模块对应的Node结点
	UEdGraphNode_ChunkReference* Node = nullptr;

	FPrimaryAssetDependencies(int InLeafNodeCount, const TArray<FPrimaryAssetId>& InIdentifiers)
		: LeafNodeCount(InLeafNodeCount), Identifiers(InIdentifiers)
	{
	};

	FPrimaryAssetDependencies(int InLeafNodeCount, const TArray<FPrimaryAssetId>&& InIdentifiers)
		: LeafNodeCount(InLeafNodeCount), Identifiers(InIdentifiers)
	{
	};
};

/**
 * 
 */
UCLASS()
class HOTUPDATEEDITORTOOL_API UEdGraph_ChunkReference : public UEdGraph
{
	GENERATED_BODY()

public:
	//生成引用关系图
	void RebuildGraph();

	//递归获取每一个主资产的所有子模块依赖
	void RecursivelyGatherSizes(FPrimaryAssetId Identifier,
	                            TMap<FPrimaryAssetId, FPrimaryAssetDependencies>& AllIdentifiers);

	//递归完成关系图的构造，从根节点开始，深度遍历
	void RecursivelyConstructNodes(FPrimaryAssetId ParentId, UEdGraphNode_ChunkReference* ParentNode,
	                               TMap<FPrimaryAssetId, FPrimaryAssetDependencies>& AllIdentifiers, int Depth,
	                               float Height);
};
