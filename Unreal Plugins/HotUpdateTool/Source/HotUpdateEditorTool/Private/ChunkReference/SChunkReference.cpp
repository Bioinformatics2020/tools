// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkReference/SChunkReference.h"

#include "ContentBrowserModule.h"
#include "SlateOptMacros.h"
#include "IContentBrowserSingleton.h"
#include "ChunkReference/EdGraphNode_ChunkReference.h"
#include "ChunkReference/EdGraph_ChunkReference.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "FHotUpdateEditorToolModule"

void SChunkReference::Construct(const FArguments& InArgs)
{
	// 使用UEdGraph_ChunkReference来生成引用关系网络
	GraphObj = NewObject<UEdGraph_ChunkReference>();
	GraphObj->Schema = UEdGraphSchema::StaticClass();
	GraphObj->AddToRoot();

	// 绑定node节点的双击点击事件
	SGraphEditor::FGraphEditorEvents GraphEvents;
	GraphEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &SChunkReference::OnNodeDoubleClicked);

	// 完成页面布局与控件绑定
	ChildSlot
	[
		SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.7f)
			[
				//显示引用关系图
				SNew(SGraphEditor)
				.GraphToEdit(GraphObj)
				.GraphEvents(GraphEvents)
				.ShowGraphStateOverlay(false)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.3f)
			[
				SNew(SScrollBox)
				.Orientation(EOrientation::Orient_Horizontal)
				.ConsumeMouseWheel(EConsumeMouseWheel::Never)
				+ SScrollBox::Slot()
				[
					//显示主资产详细信息
					SAssignNew(NodeInfoBox, SScrollBox)
				]
			]
		]
	];

	//开始构造引用图表，并完成显示
	GraphObj->RebuildGraph();
}

SChunkReference::~SChunkReference()
{
	if (!GExitPurge)
	{
		if (ensure(GraphObj))
		{
			GraphObj->RemoveFromRoot();
		}
	}
}

void SChunkReference::OnNodeDoubleClicked(UEdGraphNode* Node) const
{
	//清空旧控件
	NodeInfoBox->ClearChildren();

	//获取Node对应的资产信息
	UEdGraphNode_ChunkReference* ChunkNode = Cast<UEdGraphNode_ChunkReference>(Node);
	TArray<FString> InternalDependencies;
	TArray<UEdGraphNode_ChunkReference::FAssetDependenciesInfo> ExternalDependencies;
	ChunkNode->GetDependenciesAsset(InternalDependencies, ExternalDependencies);

	//创建标记颜色的字体，用于显示跨模块引用的资产，建议尽量减少跨模块资产
	auto TextStyle = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");

	//循环添加间接引用的子控件，展示资产信息
	for (int i = 0; i < ExternalDependencies.Num(); i++)
	{
		//引擎资源与c++资源不用显示，肯定在root模块
		if (ExternalDependencies[i].AssetName.Find("/Script") == 0
			|| ExternalDependencies[i].AssetName.Find("/Engine") == 0)
		{
			continue;
		}

		//不同类型的引用关系，使用不同的颜色标注
		switch (ExternalDependencies[i].ChunkType)
		{
		//父模块加载时，子模块不一定加载，使用子模块的资产时需要非常谨慎
		case UEdGraphNode_ChunkReference::FAssetDependenciesInfo::ChildChunk:
			TextStyle.ColorAndOpacity = FLinearColor::Yellow;
			break;
		//子模块加载时，父模块必然加载完成，依赖父模块资产不会造成问题
		case UEdGraphNode_ChunkReference::FAssetDependenciesInfo::ParentChunk:
			TextStyle.ColorAndOpacity = FLinearColor::Green;
			break;
		//以下情况不被允许，不要使用与自己无关的资产
		case UEdGraphNode_ChunkReference::FAssetDependenciesInfo::OtherChunk: 
		case UEdGraphNode_ChunkReference::FAssetDependenciesInfo::MultiChunk: 
		case UEdGraphNode_ChunkReference::FAssetDependenciesInfo::NoChunk:
			TextStyle.ColorAndOpacity = FLinearColor::Red;
			break;
		}

		//标识出软引用或硬引用，便于用户定位问题
		FText Text = FText::FromString(
			(ExternalDependencies[i].bHard ? "" : "Soft ") + ExternalDependencies[i].AssetName);

		//添加控件
		NodeInfoBox->AddSlot()
		           .HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.ToolTipText(FText::FromString(ExternalDependencies[i].ReferenceChunk))
			.TextStyle(&TextStyle)
			.Text(Text)
			.OnClicked_Raw(this, &SChunkReference::OnAssetNameClicked, ExternalDependencies[i].AssetName)
		];
	}

	//显示主资产包含的资产
	for (int i = 0; i < InternalDependencies.Num(); i++)
	{
		//引擎资源与c++资源不用显示，肯定在root模块
		if (InternalDependencies[i].Find("/Script") == 0
			|| InternalDependencies[i].Find("/Engine") == 0)
		{
			continue;
		}
		FText Text = FText::FromString(InternalDependencies[i]);

		//添加控件
		NodeInfoBox->AddSlot()
		           .HAlign(HAlign_Fill)
		[
			SNew(SButton)
			.Text(Text)
			.OnClicked_Raw(this, &SChunkReference::OnAssetNameClicked, InternalDependencies[i])
		];
	}
}

FReply SChunkReference::OnAssetNameClicked(FString AssetPath) const
{
	//通过资产注册表找到资产数据
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
		"AssetRegistry");

	FARFilter Filter;
	Filter.PackageNames.Add(FName(AssetPath));

	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);

	//资源浏览器完成页面跳转
	const FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(
		"ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToAssets(AssetDataList);

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
