#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MediaSources.h"
#include "MediaPage.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class NIC_API UMediaPage : public UUserWidget
{
	GENERATED_BODY()
public:
	//创建页面后调用
	virtual void Init(FMediaSources* Sources);
	
	//页面被显示时调用
	virtual void Show(ESwitchPageMode EnterPageMode, void* Params = nullptr);
	
	//页面被隐藏时调用
	virtual void Hide(ESwitchPageMode EnterPageMode);
	
	//进行页面跳转
	virtual void Forward(int Page, void* Params = nullptr);
	UFUNCTION(BlueprintCallable)
	virtual void Back();
	UFUNCTION(BlueprintCallable)
	void BP_Back();
	virtual void BackToHome();
public:
	//页面数据
	int MediaPageID;				//当前页面的ID
	bool IsCurrentPage;				//当前页面是否显示
	FMediaSources* CurrentSource;   //所属源
};
