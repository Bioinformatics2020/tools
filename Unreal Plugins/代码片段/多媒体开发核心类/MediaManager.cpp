#include "MediaManager.h"
#include "NIC/Manager/AppManager.h"
#include "MediaSources.h"
#include "MediaSubSources/QQMediaSources.h"
#include "NIC/UIControl/Media/MediaCtl.h"
#include "Components/Overlay.h"
#include "MediaSubSources/AudioMediaSources.h"
#include "MediaSubSources/BtMediaSources.h"
#include "MediaSubSources/RadioMediaSources.h"
#include "MediaSubSources/UsbMediaSources.h"
#include "NIC/UIControl/Media/MediaSubCtl/MiniPlayerSubCtl.h"

UOverlay* FMediaManager::GetRootOverlay()
{
	return SourceRoot;
}

void FMediaManager::Initial(UOverlay* InSourceRoot)
{
	SourceRoot = InSourceRoot;

	MediaCtl& MediaAppCtl = MediaCtl::GetInstance();

	if (AllMediaSources.Num() == 0)
	{
		//初始化5个音源管理器,需要与枚举值的顺序一致
		AllMediaSources.Add(new FQQMediaSources);
		AllMediaSources.Add(new FRadioMediaSources);
		AllMediaSources.Add(new FAudioMediaSources);
		AllMediaSources.Add(new FBtMediaSources);
		AllMediaSources.Add(new FUsbMediaSources);

		for (auto MediaSources : AllMediaSources)
		{
			MediaSources->Init(MakeShareable(this));
		}
	}
}

void FMediaManager::InitMiniPlayer(UMiniPlayer* InMiniPlayer)
{
	if(InMiniPlayer)
	{
		MiniPlayer = InMiniPlayer;
	}
}

void FMediaManager::AddPageToRoot(UWidget* InPage)
{
	if (IsValid(SourceRoot) && IsValid(InPage))
	{
		SourceRoot->AddChildToOverlay(InPage);
	}
}

void FMediaManager::RemovePageToRoot(UWidget* InPage)
{
	if (IsValid(SourceRoot) && IsValid(InPage))
	{
		SourceRoot->RemoveChild(InPage);
	}
}

void FMediaManager::SwitchSource(EMediaSource SourceType)
{
	OnMediaSwitchSourceChange.Broadcast(SourceType);
	if (CurrentSource == SourceType)
	{
		int Current = static_cast<int>(CurrentSource);
		if (AllMediaSources.IsValidIndex(Current))
		{
			AllMediaSources[Current]->SwitchPage(ESwitchPageMode::BackToHome);
		}
		return;
	}
	int Current = static_cast<int>(CurrentSource);
	if (AllMediaSources.IsValidIndex(Current))
	{
		AllMediaSources[Current]->Leave(false);
	}
	int NewSource = static_cast<int>(SourceType);
	if (AllMediaSources.IsValidIndex(NewSource))
	{
		AllMediaSources[NewSource]->Enter(false);
	}

	CurrentSource = SourceType;
}

void FMediaManager::SwitchApp(bool InOpen)
{
	if (InOpen == bOpen)
	{
		return;
	}
	if (AllMediaSources.IsValidIndex(static_cast<int>(CurrentSource)))
	{
		if (InOpen)
		{
			AllMediaSources[static_cast<int>(CurrentSource)]->Enter(true);
		}
		else
		{
			AllMediaSources[static_cast<int>(CurrentSource)]->Leave(true);
		}
	}

	bOpen = InOpen;
}

EMediaSource FMediaManager::GetCurrentSource()
{
	return CurrentSource;
}

TArray<FString> FMediaManager::GetCurrentMediaPage()
{
	TArray<FString>PageNames;
	if (MiniPlayer->bPlayListVisible)
	{
		PageNames.Add("MusicPlayList");

	}
	if (MiniPlayer->bLyricVisible)
	{
		PageNames.Add("Lyric");
	
	}
	//int CurrentMediaPage = AllMediaSources[static_cast<int>(CurrentSource)]->CurrentMediaPage;

	////获取当前子页面

	//if (CurrentMediaPage > 0)
	//{
	//	PageNames.Add(AllMediaSources[static_cast<int>(CurrentSource)]->PageMap[CurrentMediaPage].ToString());
	//}
	if (CurrentSource == EMediaSource::EMediaSourceQQMusic)
	{
		PageNames.Add("QQMusic");
		
	}
	else if (CurrentSource == EMediaSource::EMediaSourceAudioBooks)
	{
		PageNames.Add("AudioBook");
		
	}
	else if (CurrentSource == EMediaSource::EMediaSourceBT)
	{
		PageNames.Add("BTMusic");
		
	}
	else if (CurrentSource == EMediaSource::EMediaSourceUSB)
	{
		PageNames.Add("USBMusic");
		
	}
	else if (CurrentSource == EMediaSource::EMediaSourceRadio)
	{
		PageNames.Add("Radio");
		
	}
	
	return PageNames;
}

ESwitchPageResult FMediaManager::OpenSourceParam(const FString& Paths)
{
	ESwitchPageResult Res = ESwitchPageResult::NONE;
	
	//todo 解析字符串
	FString SourcePath = Paths;
	FString PagePath;
	FString MultiTurnKeyword;
	Paths.Split(".", &SourcePath, &PagePath);
	PagePath.Split(".",&PagePath,&MultiTurnKeyword);
	
	//打开指定音源
	EMediaSource TargetSource = EMediaSource::EMediaSourceCount;
	if (SourcePath == "QQMusic")
	{
		TargetSource = EMediaSource::EMediaSourceQQMusic;
	}
	else if(SourcePath == "AudioBook")
	{
		TargetSource = EMediaSource::EMediaSourceAudioBooks;
	}
	else if(SourcePath == "BTMusic")
	{
		TargetSource = EMediaSource::EMediaSourceBT;
	}
	else if(SourcePath == "USBMusic")
	{
		TargetSource = EMediaSource::EMediaSourceUSB;
	}
	else if(SourcePath == "Radio")
	{
		TargetSource = EMediaSource::EMediaSourceRadio;
	}
	else
	{
		//迷你播放器处理
		return MiniPlayer->OpenPage(SourcePath);
	}
	
	if (CurrentSource != TargetSource)
	{
		SwitchSource(TargetSource);
		Res = ESwitchPageResult::SUCCEED;
	}
	else
	{
		Res = ESwitchPageResult::NOACTION;
	}
	//不进入子页面时直接返回
	if(PagePath == "")
	{
		return Res;
	}

	//int CurrentMediaPage = AllMediaSources[static_cast<int>(CurrentSource)]->CurrentMediaPage;

	////获取当前子页面
	//FString PageName = "";
	//if(CurrentMediaPage>0)
	//{
	//	PageName = AllMediaSources[static_cast<int>(CurrentSource)]->PageMap[CurrentMediaPage].ToString();
	//}
	//else
	//{
	//	return ESwitchPageResult::FAILED;
	//}

	////若当前已经在所需要的页面，则直接返回
	//if(PageName == PagePath)
	//{
	//	return ESwitchPageResult::NOACTION;
	//}
	
	//打开页面并传递参数
	switch (CurrentSource)
	{
		case EMediaSource::EMediaSourceQQMusic:
		{
			if (PagePath == "Search")
			{
				FString IsVoice = MultiTurnKeyword;
				MiniPlayerSubCtl::GetInstance().OnGetsourceType.Clear();
				MediaCtl::GetInstance().IsVoiceMusic = true;
				AllMediaSources[static_cast<int>(CurrentSource)]->ReJoinPageAndStack(static_cast<int>(EMediaQQMusicPage::EMediaQQMusicA10SearchPage), &IsVoice);
			}
			else if (PagePath == "WBP_MediaQQMusicA14LoginPage")//QQMusic.Login
			{
				FString IsVoice = MultiTurnKeyword;
				MiniPlayerSubCtl::GetInstance().OnGetsourceType.Clear();
				AllMediaSources[static_cast<int>(CurrentSource)]->ReJoinPageAndStack(static_cast<int>(EMediaQQMusicPage::EMediaQQMusicA14LoginPage), &IsVoice);
			}
		}
		break;
		case EMediaSource::EMediaSourceRadio:
			break;
		case EMediaSource::EMediaSourceAudioBooks:
		{
			if (PagePath == "Search")
			{
				FString IsVoice = MultiTurnKeyword;
				MiniPlayerSubCtl::GetInstance().OnGetsourceType.Clear();
				MediaCtl::GetInstance().IsVoiceAudioBooks = true;
				AllMediaSources[static_cast<int>(CurrentSource)]->ReJoinPageAndStack(static_cast<int>(EMediaAudioBooksPage::EMediaAudioBooksA6SearchPage), &IsVoice);
			}
			else if (PagePath == "Classify")
			{

				FAudioBooksA2ClassifyPageParams TagId;
				TagId.FromVr = true;
				TagId.KeyWord = MultiTurnKeyword;
				
				MiniPlayerSubCtl::GetInstance().OnGetsourceType.Clear();
				AllMediaSources[static_cast<int>(CurrentSource)]->ReJoinPageAndStack(static_cast<int>(EMediaAudioBooksPage::EMediaAudioBooksA2ClassifyPage), &TagId);
			}
			else if (PagePath == "Login")//AudioBooks.Login
			{
				FString IsVoice = MultiTurnKeyword;
				MiniPlayerSubCtl::GetInstance().OnGetsourceType.Clear();
				AllMediaSources[static_cast<int>(CurrentSource)]->ReJoinPageAndStack(static_cast<int>(EMediaAudioBooksPage::EMediaAudioBooksA14Mine), &IsVoice);
			}
		}
			break;
		case EMediaSource::EMediaSourceBT:
			break;
		case EMediaSource::EMediaSourceUSB:
			break;
		case EMediaSource::EMediaSourceCount:
			break;
		default: ;
	}
	return ESwitchPageResult::SUCCEED;
}

ESwitchPageResult FMediaManager::CloseSourceParam(const FString& Paths)
{
	ESwitchPageResult Res = ESwitchPageResult::NONE;
	
	if ("MusicPlayList" ==Paths)
	{
		if (MiniPlayer->bPlayListVisible)
		{
			Res = ESwitchPageResult::SUCCEED;
			return MiniPlayer->ClosePage("MusicPlayList");
		}
		else
		{
			return ESwitchPageResult::FAILED;
		}

	}
	else if ("Lyric" == Paths)
	{
		if (MiniPlayer->bLyricVisible)
		{
			Res = ESwitchPageResult::SUCCEED;
			return MiniPlayer->ClosePage("Lyric");
		}
		else
		{
			return ESwitchPageResult::FAILED;
		}
	}
	else
	{
		if ("QQMusic" == Paths)
		{
			if (MiniPlayer)
			{
				AAppManager::GetAppManager(MiniPlayer)->CloseApp();
				Res = ESwitchPageResult::SUCCEED;
			}
			else
			{
				Res = ESwitchPageResult::FAILED;
			}
			
		}
		else
		{
			if (CurrentSource != EMediaSource::EMediaSourceQQMusic)
			{
				AAppManager::GetAppManager(MiniPlayer)->CloseApp();
				Res = ESwitchPageResult::SUCCEED;
			}
			else
			{
				Res = ESwitchPageResult::FAILED;
			}
		}
		
	}
	return Res;
}
