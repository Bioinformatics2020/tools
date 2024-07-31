// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/AssetManager.h"
#include "EdGraphNode_ChunkReference.generated.h"

/**
 * 
 */
UCLASS()
class HOTUPDATEEDITORTOOL_API UEdGraphNode_ChunkReference : public UEdGraphNode
{
	GENERATED_BODY()

public:
	struct FAssetDependenciesInfo
	{
		FString AssetName;
		FString ReferenceChunk;
		bool bHard;

		enum
		{
			OtherChunk,
			ChildChunk,
			ParentChunk,
			MultiChunk,
			NoChunk
		} ChunkType;

		//排序比较时，按照资产名称比较
		bool operator<(const FAssetDependenciesInfo& Other) const
		{
			return AssetName < Other.AssetName;
		}
	};

	/**
	 * @brief 返回Chunk关联的所有资产
	 * @param InternalDependencies 返回模块内部引用资产
	 * @param ExternalDependencies 返回模块引用的外部资产,以及资产对应的信息
	 */
	void GetDependenciesAsset(TArray<FString>& InternalDependencies, TArray<FAssetDependenciesInfo>& ExternalDependencies);

	//初始化当前结点的信息
	void SetupReferenceNode(const FPrimaryAssetId& InAssetIdentifier, const FIntPoint& NodeLoc);

	//添加引用的节点，引用节点指向当前节点
	void AddReferencer(UEdGraphNode_ChunkReference* ReferencerNode);

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

	UEdGraphPin* DependencyPin;
	UEdGraphPin* ReferencerPin;
};
