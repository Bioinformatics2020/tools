#include "MediaPage.h"
#include "MediaSources.h"

void UMediaPage::Init(FMediaSources* Source)
{
	this->CurrentSource = Source;
}

void UMediaPage::Forward(int Page, void* Params)
{
	CurrentSource->SwitchPage(ESwitchPageMode::Forward, Page, Params);
}

void UMediaPage::Back()
{
	CurrentSource->SwitchPage(ESwitchPageMode::Back);
}

void UMediaPage::BP_Back()
{
	Back();
}

void UMediaPage::BackToHome()
{
	CurrentSource->SwitchPage(ESwitchPageMode::BackToHome);
}

void UMediaPage::Show(ESwitchPageMode EnterPageMode, void* Params)
{
	SetVisibility(ESlateVisibility::Visible);
	IsCurrentPage = true;
	UE_LOG(MediaAppLog, Log, TEXT("ShowPage Mode:%d source:%d Index:%d"),static_cast<int>(EnterPageMode),static_cast<int>(CurrentSource->MediaSourceID),MediaPageID);
}

void UMediaPage::Hide(ESwitchPageMode EnterPageMode)
{
	SetVisibility(ESlateVisibility::Collapsed);
	IsCurrentPage = false;
	UE_LOG(MediaAppLog, Log, TEXT("HidePage Mode:%d source:%d Index:%d"),static_cast<int>(EnterPageMode),static_cast<int>(CurrentSource->MediaSourceID),MediaPageID);
}