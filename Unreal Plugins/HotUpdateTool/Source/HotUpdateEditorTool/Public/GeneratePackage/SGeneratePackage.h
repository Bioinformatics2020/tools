// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HotUpdateTool/Public/FUpdateManifest.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SScrollBox.h"

/**
 * 
 */
class HOTUPDATEEDITORTOOL_API SGeneratePackage : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGeneratePackage)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	//------打包功能------

	//生成基础包
	FReply OnGenerateBasePackage();

	//选择更新模块
	FReply OnSelectUpdatePackage();
	//生成更新包
	FReply OnGenerateUpdatePackage();

	//所有支持的平台
	TArray<TSharedPtr<FString>> AllPlatfromsStr;
	FString SelectPlatform;
	FString PackagePath;
	FString PackageVersion;
	FString UpdatePackageVersion;

	//错误文本提示
	TSharedPtr<STextBlock> ErrorText;
	
	//ComboBox显示的平台字符串
	TSharedPtr<STextBlock> ComboBoxText;

	//生成更新包，默认隐藏，点击选择更新包后显示
	TSharedPtr<SButton> GenerateUpdatePackageButton;
	//显示支持热更新模块的滚动框容器
	TSharedPtr<SScrollBox> UpdateChunkScrollBox;
	//选中的更新包
	TArray<FExportChunkInfo> UpdateChunk;


	//------pak文件详细信息------
	FReply OnGeneratePakInfo();
	
	FString PakFile;
	FString PakFileParams = TEXT("-list");
	TSharedPtr<SMultiLineEditableTextBox> PakInfo;
};
