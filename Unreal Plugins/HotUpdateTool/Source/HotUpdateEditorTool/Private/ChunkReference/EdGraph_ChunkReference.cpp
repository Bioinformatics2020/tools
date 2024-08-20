// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkReference/EdGraph_ChunkReference.h"

#include "HotUpdateEditorFunLib.h"
#include "ChunkReference/EdGraphNode_ChunkReference.h"
#include "Engine/AssetManager.h"

//节点之间的间距
constexpr float NodeHight = 200.0f;
constexpr float NodeWidth = 400.0f;

void UEdGraph_ChunkReference::RebuildGraph()
{
	//获取到主资产，这一步会保证主资产加载完成
	UHotUpdatePrimaryData* RootPrimaryData = UHotUpdateEditorFunLib::GetRootHotUpdatePrimaryData();
	TArray<UHotUpdatePrimaryData*> OtherPrimaryData = UHotUpdateEditorFunLib::GetAllHotUpdatePrimaryData();
	if (!RootPrimaryData)
	{
		return;
	}

	//递归统计所有主资产依赖
	FPrimaryAssetId RootId = RootPrimaryData->GetPrimaryAssetId();
	TMap<FPrimaryAssetId, FPrimaryAssetDependencies> AllIdentifiers;
	RecursivelyGatherSizes(RootId, AllIdentifiers);

	//创建根节点
	UEdGraphNode_ChunkReference* ParentNode = Cast<UEdGraphNode_ChunkReference>(
		CreateNode(UEdGraphNode_ChunkReference::StaticClass(), false));
	ParentNode->SetupReferenceNode(RootId, FIntPoint(0, 0));

	//递归创建子节点
	float BeginHeight = - (AllIdentifiers[RootId].LeafNodeCount - 1) * 0.5 * NodeHight;
	RecursivelyConstructNodes(RootId, ParentNode, AllIdentifiers, 1, BeginHeight);
}

void UEdGraph_ChunkReference::RecursivelyGatherSizes(FPrimaryAssetId Identifier,
                                                     TMap<FPrimaryAssetId, FPrimaryAssetDependencies>& AllIdentifiers)
{
	//通过主资产管理器，使用主资产id获取主资产
	UAssetManager* Manager = UAssetManager::GetIfValid();
	UHotUpdatePrimaryData* ChildPrimaryData = Manager->GetPrimaryAssetObject<UHotUpdatePrimaryData>(Identifier);
	if (!ChildPrimaryData)
	{
		UE_LOG(LogHotUpdateEditorTool, Error, TEXT("未找到资产:%s"), *Identifier.PrimaryAssetName.ToString());
		return;
	}

	//获取子模块资产，创建主资产依赖列表
	TMap<FName, FPrimaryAssetId>& SubModuleAssets = ChildPrimaryData->SubModuleAssets;
	TArray<FPrimaryAssetId> LinksToAsset;
	for (auto ModuleAsset : SubModuleAssets)
	{
		LinksToAsset.Add(ModuleAsset.Value);
	}

	//将遍历完成的子模块添加到Map中
	AllIdentifiers.Add(Identifier, FPrimaryAssetDependencies(0, LinksToAsset));

	//递归子模块
	int LeafNum = 0;
	for (FPrimaryAssetId& AssetIdentifier : LinksToAsset)
	{
		if (AllIdentifiers.Contains(AssetIdentifier))
		{
			continue;
		}
		RecursivelyGatherSizes(AssetIdentifier, AllIdentifiers);
		LeafNum += AllIdentifiers[AssetIdentifier].LeafNodeCount;
	}

	//记录子模块叶子节点的数量，用于节点布局
	AllIdentifiers[Identifier].LeafNodeCount = (LeafNum == 0) ? 1 : LeafNum;
}

void UEdGraph_ChunkReference::RecursivelyConstructNodes(FPrimaryAssetId ParentId,
                                                        UEdGraphNode_ChunkReference* ParentNode,
                                                        TMap<FPrimaryAssetId, FPrimaryAssetDependencies>&
                                                        AllIdentifiers, int Depth, float Height)
{
	//获取子节点信息
	const TArray<FPrimaryAssetId>& LinksToAsset = AllIdentifiers[ParentId].Identifiers;

	float CurrentHight = Height;
	for (const FPrimaryAssetId& AssetIdentifier : LinksToAsset)
	{
		//创建子节点
		if (AllIdentifiers[AssetIdentifier].Node)
		{
			AllIdentifiers[AssetIdentifier].Node->AddReferencer(ParentNode);
			continue;
		}
		UEdGraphNode_ChunkReference* Node = Cast<UEdGraphNode_ChunkReference>(
			CreateNode(UEdGraphNode_ChunkReference::StaticClass(), false));
		AllIdentifiers[AssetIdentifier].Node = Node;

		//设置子节点位置
		float NodeHeight = (AllIdentifiers[AssetIdentifier].LeafNodeCount - 1) * NodeHight * 0.5 + CurrentHight;
		Node->SetupReferenceNode(AssetIdentifier, FIntPoint(Depth * NodeWidth, NodeHeight));

		//连接到父节点
		Node->AddReferencer(ParentNode);

		//深度递归创建子节点
		RecursivelyConstructNodes(AssetIdentifier, Node, AllIdentifiers, Depth + 1, CurrentHight);

		//更新节点高度
		CurrentHight += AllIdentifiers[AssetIdentifier].LeafNodeCount * NodeHight;
	}
}
