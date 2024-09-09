// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkReference/EdGraphNode_ChunkReference.h"

#include "HotUpdateEditorFunLib.h"
#include "HotUpdatePrimaryData.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Misc/AssetRegistryInterface.h"

#define LOCTEXT_NAMESPACE "FHotUpdateEditorToolModule"

void UEdGraphNode_ChunkReference::SetupReferenceNode(const FPrimaryAssetId &InAssetIdentifier, const FIntPoint &NodeLoc)
{
    AssetIdentifier = InAssetIdentifier;

    // 设置节点的坐标
    NodePosX = NodeLoc.X;
    NodePosY = NodeLoc.Y;

    //根据当前Node的主资产ID，使用资产管理器找到资产名称
    UAssetManager *Manager = UAssetManager::GetIfValid();
    UHotUpdatePrimaryData *ChildPrimaryData = Manager->GetPrimaryAssetObject<UHotUpdatePrimaryData>(AssetIdentifier);

    //缓存主资产的关键信息
    AssetName = ChildPrimaryData->GetPackage()->FileName;
    ChunkId = ChildPrimaryData->Rules.ChunkId;

    // 如果节点注释不为空，则更新节点注释并设置注释气泡为可见
    NodeComment = FString::Printf(TEXT("ChunkId:%d Priority:%d"), ChunkId, ChildPrimaryData->Rules.Priority);
    bCommentBubbleVisible = true;

    // 设置节点标题
    NodeTitle = FText::FromString(AssetIdentifier.PrimaryAssetName.ToString());

    // 创建引脚（输入引脚），并设置为隐藏
    ReferencerPin = CreatePin(EEdGraphPinDirection::EGPD_Input, NAME_None, NAME_None);
    ReferencerPin->bHidden = true;
    // 创建引脚（输出引脚），并设置为隐藏
    DependencyPin = CreatePin(EEdGraphPinDirection::EGPD_Output, NAME_None, NAME_None);
    DependencyPin->bHidden = true;
}

void UEdGraphNode_ChunkReference::AddReferencer(UEdGraphNode_ChunkReference *ReferencerNode)
{
    // 检查ReferencerNode是否有效，并获取其依赖引脚。引脚方向ReferencerNode --> this
    UEdGraphPin *ReferencerDependencyPin = ReferencerNode->DependencyPin;

    // 确保ReferencerDependencyPin不为空，如果是有效的，那么就解除其隐藏状态
    if ( ensure(ReferencerDependencyPin) )
    {
        //记录当前Node关联的所有Chunk
        ReferencerNode->ChildChunkId.Add(ChunkId);
        ReferencerNode->ChildChunkId.Append(ChildChunkId);

        ParentChunkId.Add(ReferencerNode->ChunkId);
        ParentChunkId.Append(ReferencerNode->ParentChunkId);

        // 显示Pin节点，完成连线
        ReferencerDependencyPin->bHidden = false;
        ReferencerPin->bHidden = false;
        ReferencerPin->MakeLinkTo(ReferencerDependencyPin);
    }
}

bool UEdGraphNode_ChunkReference::AssetReferenceIsHard(UE::AssetRegistry::EDependencyProperty Properties)
{
    return static_cast<bool>(((Properties & UE::AssetRegistry::EDependencyProperty::Hard) != UE::AssetRegistry::EDependencyProperty::None)
                             | ((Properties & UE::AssetRegistry::EDependencyProperty::Direct) != UE::AssetRegistry::EDependencyProperty::None));
}

void UEdGraphNode_ChunkReference::SetNodeInfo(FAssetDependenciesInfo& Info,const UAssetManager *AssetManager)
{
    if(!AssetManager)
    {
        UE_LOG(LogHotUpdateEditorTool,Warning,TEXT("AssetManager is null"))
        return;
    }
    //为异常引用的资产查找所属Chunk
    TArray<int32> FoundChunks;
    AssetManager->GetPackageChunkIds(FName(Info.AssetName), nullptr, {}, FoundChunks);

    //分析并记录资源不同的引用关系
    if ( FoundChunks.Num() > 0 )
    {
        if ( FoundChunks.Num() == 1 )
        {
            if ( ParentChunkId.Contains(FoundChunks[0]) )
            {
                //被父模块引用
                Info.ChunkType = FAssetDependenciesInfo::ParentChunk;
            }
            else if ( ChildChunkId.Contains(FoundChunks[0]) )
            {
                //被子模块引用
                Info.ChunkType = FAssetDependenciesInfo::ChildChunk;
            }
            else
            {
                //被无关模块引用
                Info.ChunkType = FAssetDependenciesInfo::OtherChunk;
            }

            Info.ReferenceChunk = TEXT("ReferenceChunk: ") + FString::FromInt(FoundChunks[0]);
        }
        else
        {
            //被多个模块引用
            Info.ChunkType = FAssetDependenciesInfo::MultiChunk;
            Info.ReferenceChunk = TEXT("ReferenceChunks:");
            for ( auto Chunk : FoundChunks )
            {
                Info.ReferenceChunk += " " + FString::FromInt(Chunk);
            }
        }
    }
    else
    {
        //未找到资产引用
        Info.ChunkType = FAssetDependenciesInfo::NoChunk;
        Info.ReferenceChunk = "NoChunk";
    }
}

FText UEdGraphNode_ChunkReference::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return NodeTitle;
}

FText UEdGraphNode_ChunkReference::GetTooltipText() const
{
    return FText::FromName(AssetName);
}

void UEdGraphNode_ChunkReference::GetDependenciesAsset(TArray<FString> &InternalDependencies, TArray<FAssetDependenciesInfo> &ExternalDependencies)
{
    //使用资产管理器找到直接依赖的资源
    IAssetRegistry &AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
    TArray<FAssetDependency> LinksToAsset;
    AssetRegistry.GetDependencies(AssetName, LinksToAsset);

    //导出直接引用的资产，记录已经查找过的资产，避免重复查找或循环查找
    TSet<FString> AllDependencies;
    for ( auto &Link : LinksToAsset )
    {
        InternalDependencies.Add(Link.AssetId.ToString());
        AllDependencies.Add(Link.AssetId.ToString());
    }
    InternalDependencies.Sort();

    UAssetManager *AssetManager = UAssetManager::GetIfValid();
    //首次执行时需要一段时间，拷贝自引用查看器功能
    AssetManager->UpdateManagementDatabase();

    //基于上方的资源引用查找方法，广度递归查找依赖的资产
    TArray<FAssetDependency> NeedFindAsset = LinksToAsset;
    for ( int i = 0; i < NeedFindAsset.Num(); i++ )
    {
        if ( NeedFindAsset[i].AssetId.IsValid() )
        {
            LinksToAsset.SetNum(0, false);
            AssetRegistry.GetDependencies(NeedFindAsset[i].AssetId, LinksToAsset);
            for ( auto &Link : LinksToAsset )
            {
                FString PackageName = Link.AssetId.ToString();
                if ( AllDependencies.Contains(PackageName) )
                {
                    continue;
                }
                FAssetDependenciesInfo Info;
                Info.bHard = AssetReferenceIsHard(Link.Properties);
                Info.AssetName = Link.AssetId.ToString();

                //针对所属Chunk添加不同的标记内容
                SetNodeInfo(Info, AssetManager);

                ExternalDependencies.Add(Info);
                AllDependencies.Add(PackageName);
                NeedFindAsset.Add(Link);
            }
        }
    }

    //按照资产名称排序
    ExternalDependencies.Sort();
}
#undef LOCTEXT_NAMESPACE