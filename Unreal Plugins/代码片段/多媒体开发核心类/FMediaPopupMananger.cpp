#include "FMediaPopupMananger.h"
#include "DataAssetMananger.h"
#include "MediaConfig.h"

void FMediaPopupMananger::Initial(UOverlay *InPopupRoot, UWidget* InBackGround)
{
	Pages.Empty();
	Pages.SetNumZeroed(PageMap.Num());
	ShowPageNumber = 0;

	BackGround = InBackGround;
	PopupRoot = InPopupRoot;
}

UUserWidget* FMediaPopupMananger::ShowPage(EPopWidget PageType,bool InHaveBG)
{
	if(ShowPageNumber==0)
	{
		PopupRoot->SetVisibility(ESlateVisibility::Visible);
	}

	//重复打开某一个弹窗时，不是重复创建弹窗，而是直接返回首次创建的弹窗。现象将是执行多个ShowPage却只有一个HidePage
	//有两个问题：
	//1.对返回的弹窗直接进行设置时，需要注意旧数据的销毁时机与新数据的创建时机
	//		临时方案是由控件自己处理，当前管理器仅仅只是创建控件
	//2.没有对ShowPageNumber与ShowPageNumberBG进行特殊处理，ShowPageNumber++会执行多次，ShowPageNumber--只会执行1次
	//		临时方案是如果此弹窗已经创建，将不执行ShowPageNumber++，直接返回弹窗
	int PageIndex = static_cast<int>(PageType);
    if(Pages[PageIndex].IsValid())
    {
    	UUserWidget* PopPage = Pages[PageIndex].Get();
    	if(PopupRoot->HasChild(PopPage))
    	{
    		PopPage->SetVisibility(ESlateVisibility::Visible);
    		return PopPage;
    	}
    }
    		
	if(InHaveBG)
	{
		ShowPageNumberBG++;
		if(ShowPageNumberBG == 1)
		{
			BackGround->SetVisibility(ESlateVisibility::Visible);
			OnVisibleChange.ExecuteIfBound(true);
		}
	}
	//从缓存里复用控件或动态创建控件
	UUserWidget* ShowPage = GetPopPage(PageType);
	ShowPage->SetVisibility(ESlateVisibility::Visible);
	if (IsValid(ShowPage))
	{
		ShowPageNumber++;
	}
	return ShowPage;
}

void FMediaPopupMananger::HidePage(EPopWidget PageType, bool InHaveBG)
{
	int PageIndex = static_cast<int>(PageType);
	check(Pages.IsValidIndex(PageIndex));
	TWeakObjectPtr<UUserWidget> ShowPage =  Pages[PageIndex];

	//从父节点移除页面，等待GC
	if (ShowPage.IsValid())
	{
		ShowPage->RemoveFromParent();
		//ShowPage->SetVisibility(ESlateVisibility::Collapsed);
		ShowPageNumber--;
		if(ShowPageNumber == 0)
		{
			PopupRoot->SetVisibility(ESlateVisibility::Collapsed);
		}
		if(InHaveBG)
		{
			ShowPageNumberBG--;
			if(ShowPageNumberBG == 0)
			{
				BackGround->SetVisibility(ESlateVisibility::Collapsed);
				OnVisibleChange.ExecuteIfBound(false);
			}
		}
	}
}

UUserWidget* FMediaPopupMananger::GetPopPage(EPopWidget PageType)
{
	int PageIndex = static_cast<int>(PageType);

	check(Pages.IsValidIndex(PageIndex));
	UUserWidget* PopPage = nullptr;
	if(!Pages[PageIndex].IsValid())
	{
		const TSubclassOf<UUserWidget> PageClass = ADataAssetMananger::RequestSyncLoadClass(PopupRoot,PageMap[PageType]);
		PopPage = CreateWidget<UUserWidget>(PopupRoot, PageClass);
		
		UE_LOG(MediaAppLog, Log, TEXT("UMediaPopManager CreateWidget %s"),*PageMap[PageType].ToString());
		Pages[PageIndex] = PopPage;
	}
	else
	{
		PopPage = Pages[PageIndex].Get();
	}

	PopupRoot->AddChildToOverlay(PopPage);
	return PopPage;
}
