// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MediaManager.h"
#include "MediaConfig.h"

enum class ESwitchPageMode : uint8
{
	//直接进入某个页面
	Forward,
	//返回上一级页面
	Back,
	//在从无网络页面或其它特殊情况返回时需要重新请求数据
	BackAndRefresh,
	//返回首页
	BackToHome,
	
	//其它源进入
	OtherSourceForward,
	//其它APP进入
	OtherAppForward,
	
	//返回到其它源
	BackOtherSource,
	//返回到从其它APP
	BackOtherApp
};

class UMediaPage;
class FMediaManager;
/**
 * 
 */
class FMediaSources
{
public:
	//首次创建源后调用
	virtual void Init(TSharedPtr<FMediaManager> InMediaManager);
	virtual ~FMediaSources() = default;
	
	//进入当前源时调用
	void Enter(bool OtherApp);
	//进入离开源时调用
	void Leave(bool OtherApp);

	//是否获得音频焦点
	void SetPlayFocus(bool InPlayFocus);

	//切换页面
	void SwitchPage(ESwitchPageMode EnterPageMode, int InPageIndex = -1, void* Params = nullptr);

	//销毁指定页面以及页面栈的之上的所有页面,重新进入这个页面,返回是否成功移除任意页面
	bool ReJoinPageAndStack(int InPageIndex, void* Params = NULL);

	//新建页面
	UMediaPage* CreatePageWidget(int NewPageIndex);
public:

	//数据部分
	EMediaSource MediaSourceID = EMediaSource::EMediaSourceCount;	
	int CurrentMediaPage = -1;
	bool bPlayFocus = false;	//是否为正准备播放的源
	bool bShowing = false;		//是否正在显示当前源

	TArray<UMediaPage*> PageStacks;//源管理自己的所有页面,使用TStrongObjectPtr保证UObject不被销毁,只销毁Slate
	TMap<int,FName> PageMap;					//页面ID,页面名字的map表

	TSharedPtr<FMediaManager> MediaManager = nullptr;
};


