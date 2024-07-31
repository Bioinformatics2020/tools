// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneratePackage/SGeneratePackage.h"

#include "HotUpdateEditorFunLib.h"
#include "SlateOptMacros.h"
#include "GeneratePackage/GenerateProjectTool.h"
#include "Widgets/Layout/SScrollBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGeneratePackage::Construct(const FArguments& InArgs)
{
	//使用配置文件管理窗口的配置信息，避免每次重新输入
	GConfig->GetString(TEXT("HotUpdateEditorTool"),TEXT("PackagePath"),PackagePath,GEditorIni);
	GConfig->GetString(TEXT("HotUpdateEditorTool"),TEXT("PackageVersion"),PackageVersion,GEditorIni);
	GConfig->GetString(TEXT("HotUpdateEditorTool"),TEXT("UpdatePackageVersion"),UpdatePackageVersion,GEditorIni);
	SelectPlatform = TEXT("None");
	GConfig->GetString(TEXT("HotUpdateEditorTool"),TEXT("Platfrom"),SelectPlatform,GEditorIni);

	PakFileParams = TEXT("-list");
	
	//记录所有支持的模块
	int SelectPlatformIndex = 0;
	TArray<FName> AllPlatfroms = FGenerateProjectTool::GetAllPlatfrom();
	for (int i=0;i<AllPlatfroms.Num();i++ )
	{
		TSharedPtr<FString> PlatfromStr = MakeShareable(new FString(AllPlatfroms[i].ToString()));
		AllPlatfromsStr.Add(PlatfromStr);
		//选中上次选择的模块
		if(PlatfromStr->Equals(SelectPlatform))
		{
			SelectPlatformIndex = i;
		}
	}
	
	//创建窗口内容
	ChildSlot
	[
		SNew(SVerticalBox)
		//选择打包路径
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("打包路径: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(PackagePath))
				.HintText(FText::FromString(TEXT("C:/OutPutPackage/")))
				.OnTextCommitted_Lambda([this] (const FText& InText, ETextCommit::Type InCommitType)
				{
					PackagePath = InText.ToString();
					GConfig->SetString(TEXT("HotUpdateEditorTool"),TEXT("PackagePath"),*PackagePath,GEditorIni);
					GConfig->Flush(false, GEditorIni);
				})
			]
		]
		//打包版本号与打包平台
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			//基础包版本号
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("基础包版本号: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(PackageVersion))
				.HintText(FText::FromString(TEXT("1.0")))
				.OnTextCommitted_Lambda([this] (const FText& InText, ETextCommit::Type InCommitType)
				{
					PackageVersion = InText.ToString();
					GConfig->SetString(TEXT("HotUpdateEditorTool"),TEXT("PackageVersion"),*PackageVersion,GEditorIni);
					GConfig->Flush(false, GEditorIni);
				})
			]

			//更新包版本号
			+ SHorizontalBox::Slot()
			.Padding(20, 0, 0, 0)
			.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("补丁包版本号: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(FText::FromString(UpdatePackageVersion))
				.HintText(FText::FromString(TEXT("1.0")))
				.OnTextCommitted_Lambda([this] (const FText& InText, ETextCommit::Type InCommitType)
				{
					UpdatePackageVersion = InText.ToString();
					GConfig->SetString(TEXT("HotUpdateEditorTool"),TEXT("UpdatePackageVersion"),*UpdatePackageVersion,GEditorIni);
					GConfig->Flush(false, GEditorIni);
				})
			]

			//打包平台
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(20, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("选择平台")))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&AllPlatfromsStr)
				.InitiallySelectedItem(AllPlatfromsStr[SelectPlatformIndex])
				.OnSelectionChanged_Lambda([this] (TSharedPtr<FString> InItem, ESelectInfo::Type InSelectInfo)
				{
					SelectPlatform = *InItem;
					ComboBoxText->SetText(FText::FromString(*InItem));
					GConfig->SetString(TEXT("HotUpdateEditorTool"),TEXT("Platfrom"),*SelectPlatform,GEditorIni);
					GConfig->Flush(false, GEditorIni);
				})
				.OnGenerateWidget_Lambda( [] (TSharedPtr<FString> InItem)
				{
					return SNew(STextBlock)
					.Text(FText::FromString(*InItem));
				})
				.Content()
				[
					SAssignNew(ComboBoxText, STextBlock)
					.Text(FText::FromString(*AllPlatfromsStr[SelectPlatformIndex]))
				]
			]
		]
		//发布基础包
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FText::FromString(TEXT("发布基础包")))
			.OnClicked(FOnClicked::CreateSP(this, &SGeneratePackage::OnGenerateBasePackage))
		]

		//发布更新包
		+ SVerticalBox::Slot()
		.Padding(5.0f)
		.AutoHeight()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("选择更新包")))
				.OnClicked(FOnClicked::CreateSP(this, &SGeneratePackage::OnSelectUpdatePackage))
			]
			//点击选择更新包后，在滚动框展示可以选择的模块
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(UpdateChunkScrollBox,SScrollBox)
			]
			//开始打包
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(GenerateUpdatePackageButton,SButton)
				.Text(FText::FromString(TEXT("发布更新包")))
				.OnClicked(FOnClicked::CreateSP(this, &SGeneratePackage::OnGenerateUpdatePackage))
			]
		]

		//报错提示
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(ErrorText,STextBlock)
			.ColorAndOpacity(FLinearColor::Red)
		]

		//显示PakFile的详细信息
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				//输入Pak文件全路径
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Pak路径: ")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.7f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(PakFile))
					.HintText(FText::FromString(TEXT("全路径")))
					.OnTextCommitted_Lambda([this] (const FText& InText, ETextCommit::Type InCommitType)
					{
						PakFile = InText.ToString();
					})
				]
				//输入UnrealPak.exe命令参数，解压缩命令将自动输入
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(20, 0, 0, 0)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("UnrealPak参数：")))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.3f)
				[
					SNew(SEditableTextBox)
					.Text(FText::FromString(PakFileParams))
					.HintText(FText::FromString(TEXT("路径为空时输入-h查看参数")))
					.OnTextCommitted_Lambda([this] (const FText& InText, ETextCommit::Type InCommitType)
					{
						PakFileParams = InText.ToString();
					})
				]
				//开始执行命令
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(FText::FromString(TEXT("生成Pak信息")))
					.ToolTipText(FText::FromString(TEXT("未使用异步功能，可能会卡顿几秒")))
					.OnClicked(FOnClicked::CreateSP(this, &SGeneratePackage::OnGeneratePakInfo))
				]
			]
			//显示UnrealPak输出的所有内容
			+ SScrollBox::Slot()
			.Padding(2.0f)
			[
				SAssignNew(PakInfo,SMultiLineEditableTextBox)
				.HintText(FText::FromString("Output"))
			]
		]
	];
	GenerateUpdatePackageButton->SetVisibility(EVisibility::Collapsed);
}

FReply SGeneratePackage::OnGenerateBasePackage()
{
	//异常输入检测
	if(PackagePath.IsEmpty())
	{
		ErrorText->SetText(FText::FromString(TEXT("请先设置打包路径")));
		return FReply::Handled();
	}

	if(PackageVersion.IsEmpty())
	{
		ErrorText->SetText(FText::FromString(TEXT("请先设置基础包打包版本")));
		return FReply::Handled();
	}
	
	if(SelectPlatform.IsEmpty() || SelectPlatform == "None")
	{
		ErrorText->SetText(FText::FromString(TEXT("请先选择打包平台")));
		return FReply::Handled();
	}
	ErrorText->SetText(FText::FromString(TEXT("")));

	//更新时间戳
	UHotUpdateEditorFunLib::SaveUpdateTime();
	//开始打包
	FGenerateProjectTool::PackageProject(PackagePath,FName(SelectPlatform),
		[this](FString ResultType, double Time)
		{
			UE_LOG(LogTemp,Warning,TEXT("打包结果:%s,耗时:%f"),*ResultType,Time);
			if(ResultType == TEXT("Completed"))
			{
				//打包完成后取出包体，取出热更新模块，生成基础包
				FString OutDirectory = PackagePath + "/" + "Output/" + SelectPlatform + "/";
				UHotUpdateEditorFunLib::CreateBasePak(PackagePath + "/" + SelectPlatform + "/",OutDirectory,PackageVersion);
			}
		});
	return FReply::Handled();
}

FReply SGeneratePackage::OnSelectUpdatePackage()
{
	UpdateChunk.SetNum(0);
	UpdateChunkScrollBox->ClearChildren();

	//第一次点击时显示,第二次点击时隐藏
	if(GenerateUpdatePackageButton->GetVisibility() == EVisibility::Visible)
	{
		GenerateUpdatePackageButton->SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		GenerateUpdatePackageButton->SetVisibility(EVisibility::Visible);
	
		TArray<UHotUpdatePrimaryData*> PrimaryDatas = UHotUpdateEditorFunLib::FindUserPrimaryDate();
		for (auto PrimaryData : PrimaryDatas)
		{
			if(PrimaryData->ModuleName == "root")
			{
				continue;
			}
			//对每一个模块添加一个选择框
			FExportChunkInfo ChunkInfo = UHotUpdateEditorFunLib::GetChunkInfo(PrimaryData);
			UpdateChunkScrollBox->AddSlot()
			[
				SNew(SCheckBox)
				.OnCheckStateChanged_Lambda([this](ECheckBoxState InState,FExportChunkInfo ChunkInfo)
				{
					if(InState == ECheckBoxState::Checked)
					{
						UpdateChunk.AddUnique(ChunkInfo);
					}
					else if(InState == ECheckBoxState::Unchecked)
					{
						UpdateChunk.Remove(ChunkInfo);
					}
				},ChunkInfo)
				[
					SNew(STextBlock)
					.Text(FText::FromString(PrimaryData->ModuleName.ToString()))
				]
			];
		}
	}
	return FReply::Handled();
}

FReply SGeneratePackage::OnGenerateUpdatePackage()
{
	//异常输入检测
	if(PackagePath.IsEmpty())
	{
		ErrorText->SetText(FText::FromString(TEXT("请先设置打包路径")));
		return FReply::Handled();
	}

	if(PackageVersion.IsEmpty())
	{
		ErrorText->SetText(FText::FromString(TEXT("请先设置基础包打包版本")));
		return FReply::Handled();
	}
	
	if(SelectPlatform.IsEmpty() || SelectPlatform == "None")
	{
		ErrorText->SetText(FText::FromString(TEXT("请先选择打包平台")));
		return FReply::Handled();
	}

	if(UpdatePackageVersion.IsEmpty())
	{
		ErrorText->SetText(FText::FromString(TEXT("请先设置打包版本")));
		return FReply::Handled();
	}

	if(UpdateChunk.Num() == 0)
	{
		ErrorText->SetText(FText::FromString(TEXT("请先选择更新包")));
		return FReply::Handled();
	}
	ErrorText->SetText(FText::FromString(TEXT("")));

	//更新时间戳
	UHotUpdateEditorFunLib::SaveUpdateTime();
	//开始打包
	FGenerateProjectTool::PackageProject(PackagePath,FName(SelectPlatform),
		[this](FString ResultType, double Time)
		{
			UE_LOG(LogTemp,Warning,TEXT("打包结果:%s,耗时:%f"),*ResultType,Time);
			if(ResultType == TEXT("Completed"))
			{
				//打包完成后取出更新模块，形成更新包
				FString OutDirectory = PackagePath + "/" + "Output/" + SelectPlatform + "/";
				UHotUpdateEditorFunLib::CreateUpdatePak(UpdateChunk,PackagePath + "/" + SelectPlatform + "/",OutDirectory,PackageVersion+"-"+UpdatePackageVersion);
				GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue"));
			}
			else
			{
				GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
			}
		});
	return FReply::Handled();
}

FReply SGeneratePackage::OnGeneratePakInfo()
{
	//自动解密Pak文件
	const FString Params = TEXT("-cryptokeys=") + FPaths::ProjectSavedDir()/TEXT("Cooked")/SelectPlatform/GInternalProjectName/TEXT("Metadata/Crypto.json") + TEXT(" ") + PakFileParams;
	const FString PakFileInfo = UHotUpdateEditorFunLib::GatPakFileInfo(PakFile,Params);
	PakInfo->SetText(FText::FromString(PakFileInfo));
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
