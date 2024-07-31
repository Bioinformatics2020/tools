// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SScrollBox.h"

class UEdGraph_ChunkReference;
/**
 * Chunk引用查看器，显示主资产的依赖关系，显示每个主资产包括哪些资源，高亮异常引用的资源
 */
class HOTUPDATEEDITORTOOL_API SChunkReference : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SChunkReference)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	~SChunkReference();

	//双击引用节点显示资源详细信息
	void OnNodeDoubleClicked(UEdGraphNode* Node) const;

	//资源详细信息被点击时，跳转资源浏览器，显示当前资源
	FReply OnAssetNameClicked(FString AssetPath) const;

private:
	//显示主资产详细信息的控件容器
	TSharedPtr<SScrollBox> NodeInfoBox;

	//主资产的引用关系图生成器
	UEdGraph_ChunkReference* GraphObj = nullptr;
};
