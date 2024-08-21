#pragma once
#include "Components/Overlay.h"
#include "Blueprint/UserWidget.h"
#include "NIC/Template/Singleton.h"

DECLARE_DELEGATE_OneParam(FVisibleChange,bool);

enum class EPopWidget
{
	WBP_PopUp_Delete,
	WBP_Radio_Local_Tuning,
	WBP_Radio_Tuning_Auto,
	WBP_PopToast,
};

class FMediaPopupMananger : public TSingleton<FMediaPopupMananger>
{
public:
	void Initial(UOverlay* InPopupRoot, UWidget* InBackGround);

	//显示某一个弹窗
	UUserWidget* ShowPage(EPopWidget PageType,bool InHaveBG = true);
	//关闭某一个弹窗
	void HidePage(EPopWidget PageType, bool InHaveBG = true);

	FVisibleChange OnVisibleChange;
private:
	UUserWidget* GetPopPage(EPopWidget PageType);
	
	TArray<TWeakObjectPtr<UUserWidget>> Pages;		//管理弹窗页面
	TMap<EPopWidget,FName> PageMap= 
	{
		{EPopWidget::WBP_PopUp_Delete,TEXT("WBP_PopUp_Delete")},
		{EPopWidget::WBP_Radio_Local_Tuning,TEXT("WBP_Radio_Local_Tuning")},
		{EPopWidget::WBP_Radio_Tuning_Auto,TEXT("WBP_Radio_Tuning_Auto")},
		{EPopWidget::WBP_PopToast,TEXT("WBP_PopToast")},
	};			//页面ID,页面名字的map表
	
	UOverlay* PopupRoot = nullptr;
	UWidget* BackGround = nullptr; //半透明背景板

	int ShowPageNumber = 0;
	int ShowPageNumberBG = 0;
};
