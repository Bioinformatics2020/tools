#include "MediaSources.h"
#include "Components/Overlay.h"
#include "MediaPage.h"
#include "DataAssetMananger.h"

void FMediaSources::Init(TSharedPtr<FMediaManager> InMediaManager)
{
	MediaManager = InMediaManager;
}

void FMediaSources::Enter(bool OtherApp)
{
	if (!MediaManager.IsValid())
	{
		UE_LOG(MediaAppLog, Error, TEXT("MediaManager is NULL"));
		return;
	}
	if (bShowing)
	{
		return;
	}
	bShowing = true;

	int NewPageId =0;
	if (PageStacks.Num() != 0)
	{
		UMediaPage* TailPage = PageStacks.Last();
		if(IsValid(TailPage))
		{
			NewPageId = TailPage->MediaPageID;
		}
	}
	
	if (OtherApp)
	{
		SwitchPage(ESwitchPageMode::OtherAppForward,NewPageId);
	}
	else
	{
		SwitchPage(ESwitchPageMode::OtherSourceForward,NewPageId);
	}
}

void FMediaSources::Leave(bool OtherApp)
{
	if (!MediaManager.IsValid())
	{
		UE_LOG(MediaAppLog, Error, TEXT("MediaManager is NULL"));
		return;
	}
	if (PageStacks.Num() <= 0)
	{
		UE_LOG(MediaAppLog, Error, TEXT("PageStacks is NULL"));
		return;
	}
	bShowing = false;
	UMediaPage* TailPage = PageStacks.Last();
	if (OtherApp)
	{
		SwitchPage(ESwitchPageMode::BackOtherApp);
	}
	else
	{
		SwitchPage(ESwitchPageMode::BackOtherSource);
	}
}

void FMediaSources::SetPlayFocus(bool InPlayFocus)
{
	bPlayFocus = InPlayFocus;
}

void FMediaSources::SwitchPage(ESwitchPageMode EnterPageMode, int InPageIndex, void* Params)
{
	UE_LOG(MediaAppLog, Log, TEXT("SwitchPage Mode:%d source:%d Index:%d"),static_cast<int>(EnterPageMode),static_cast<int>(MediaSourceID),InPageIndex);
	
	switch (EnterPageMode)
	{
		case ESwitchPageMode::Forward:
			//直接创建新页面
			{
				if(PageStacks.Num() > 0)
				{
					UMediaPage* TailPage = PageStacks.Last();
					TailPage->Hide(EnterPageMode);
				}
				UMediaPage* NewPage = CreatePageWidget(InPageIndex);
				PageStacks.Add(NewPage);
				MediaManager->AddPageToRoot(NewPage);
				NewPage->Show(EnterPageMode, Params);
			}
			break;
		case ESwitchPageMode::Back:
		case ESwitchPageMode::BackAndRefresh:
			//若页面栈数量大于1，返回上一个页面，销毁当前页面
			{
				if(PageStacks.Num() > 1)
				{
					UMediaPage* TailPage = PageStacks.Last();
					TailPage->Hide(EnterPageMode);
					MediaManager->RemovePageToRoot(TailPage);
					PageStacks.Pop();

					PageStacks.Last()->Show(EnterPageMode, Params);
				}
			}
			break;
		case ESwitchPageMode::BackToHome:
			//循环销毁当前页面，直到显示最后一个页面(首页)
			{
				if(PageStacks.Num() > 1)
				{
					while (PageStacks.Num() > 1)
					{
						UMediaPage* TailPage = PageStacks.Pop();
						TailPage->Hide(EnterPageMode);
						MediaManager->RemovePageToRoot(TailPage);
					}
					PageStacks.Last()->Show(EnterPageMode, Params);
				}
			}
			break;
		case ESwitchPageMode::OtherSourceForward:
		case ESwitchPageMode::OtherAppForward:
			//直接显示或创建页面
			{
				if(PageStacks.Num() > 0)
				{
					PageStacks.Last()->Show(EnterPageMode, Params);
				}
				else
				{
					UMediaPage* NewPage = CreatePageWidget(InPageIndex);
					PageStacks.Add(NewPage);
					MediaManager->AddPageToRoot(NewPage);
					NewPage->Show(EnterPageMode, Params);
				}
			}
			break;
		case ESwitchPageMode::BackOtherSource:
		case ESwitchPageMode::BackOtherApp:
			//直接隐藏页面
			{
				PageStacks.Last()->Hide(EnterPageMode);
			}
			break;
	}
}

bool FMediaSources::ReJoinPageAndStack(int InPageIndex, void* Params)
{
	int NeedRemoveIndex = -1;
	//寻找指定的页面
	for (int i = 0; i < PageStacks.Num(); i++)
	{
		if (PageStacks[i]->MediaPageID == InPageIndex)
		{
			NeedRemoveIndex = i;
			break;
		}
	}
	//找到了就移除该页面以及栈后方的所有页面
	if (NeedRemoveIndex != -1)
	{
		for (int i = PageStacks.Num()-1; i >= NeedRemoveIndex ; i--)
		{
			UMediaPage* TailPage = PageStacks.Pop();
			TailPage->Hide(ESwitchPageMode::BackToHome);
			MediaManager->RemovePageToRoot(TailPage);
		}
	}
	else//未找到就仅仅隐藏最后
	{
		if (PageStacks.Num() > 0)
		{
			UMediaPage* TailPage = PageStacks.Last();
			TailPage->Hide(ESwitchPageMode::BackToHome);
		}
	}

	//添加新的页面到栈尾
	UMediaPage* NewPage = CreatePageWidget(InPageIndex);
	PageStacks.Add(NewPage);
	MediaManager->AddPageToRoot(NewPage);
	NewPage->Show(ESwitchPageMode::Forward, Params);

	return NeedRemoveIndex != -1;
}

UMediaPage* FMediaSources::CreatePageWidget(int NewPageIndex)
{
	UOverlay* Root = MediaManager->GetRootOverlay();
	check(IsValid(Root));
	const UClass* PageClass = ADataAssetMananger::RequestSyncLoadClass(Root, PageMap.FindChecked(NewPageIndex));
	checkf(PageClass, TEXT("UMediaSources::ShowPage ADataAssetMananger no find"));
	UMediaPage* CurrentPage = NewObject<UMediaPage>(Root, PageClass);
	check(IsValid(CurrentPage));
	CurrentPage->Init(this);
	CurrentPage->MediaPageID = NewPageIndex;
	return CurrentPage;
}