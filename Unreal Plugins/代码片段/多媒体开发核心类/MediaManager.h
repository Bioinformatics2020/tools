#pragma once

#include "CoreMinimal.h"
#include "MediaConfig.h"
#include "Core/Base/WidgetBase.h"
#include "NIC/UI/Media/SubWidget/MiniPlayer.h"

class FMediaSources;
class UOverlay;
class UWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FMediaSwitchSource,EMediaSource)

class FMediaManager
{
public:
	UOverlay* GetRootOverlay();

	//用于管理子页面的添加或销毁 页面栈的子页面是需要添加上去，不在页面栈上需要销毁
	void AddPageToRoot(UWidget* InPage);
	void RemovePageToRoot(UWidget* InPage);
	
protected:
	UOverlay* SourceRoot = nullptr;

public:
	//设置多媒体子页面的根节点 提供关联到world,关联到UMG,显示子页面的功能。只有这个函数被执行之后，才能开启整个管理器的运行
	//完成初始化 音源管理器的初始化，迷你播放器的初始化等
	void Initial(UOverlay* InSourceRoot);
	void InitMiniPlayer(UMiniPlayer* InMiniPlayer);

	//管理多个音源状态
	void SwitchSource(EMediaSource SourceType);
	void SwitchApp(bool InOpen);

	EMediaSource GetCurrentSource();
	FMediaSwitchSource OnMediaSwitchSourceChange;
	
	//用来获取当前打开的是什么页面
	TArray<FString> GetCurrentMediaPage();
	//这一部分用来打开页面并传递给页面参数，用于APP外部调用(MeidaUIProxy的OpenChildWidget)
	ESwitchPageResult OpenSourceParam(const FString& Paths);
	ESwitchPageResult CloseSourceParam(const FString& Paths);
protected:
	EMediaSource CurrentSource = EMediaSource::EMediaSourceQQMusic;      //当前源，默认是QQ音乐
	EMediaSource PlayFocusSource = EMediaSource::EMediaSourceCount;      //当前音源焦点

	bool bOpen = false;

	TArray<FMediaSources*> AllMediaSources;

	UMiniPlayer* MiniPlayer;
};

