// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugConfigLibrary.generated.h"

#define MONKEY_TEST 1
#define DebugSlateInfo 1

/**
 * 
 */
UCLASS()
class NIC_API UDebugConfigLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//设置配置文件,传入参数形如："DebugConfig|SectionName|KeyName|Value"
	UFUNCTION(BlueprintCallable, Category = "DebugConfigLibrary | OpenWidget")
	static void SetDebugConfig(const FString& Message);

	//开启APP相关配置
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | OpenWidget")
	static bool NotOpenApp();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | OpenWidget")
	static bool NotSwitchApp();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | OpenWidget")
	static bool NotCloseApp();
	//是否可以进入apa
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | OpenWidget")
	static bool NotEnterApa();

	//显示widget调试信息
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | WidgetInfo")
	static bool DebugUserWidgetInfo();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | WidgetInfo")
	static bool DebugWidgetContainer();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | WidgetInfo")
	static bool DebugLayoutPointWidget();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | WidgetInfo")
	static bool DebugWidgetForce();

	//消息系统调试
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | DebugMessage")
	static bool DebugMessage();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | DebugMessage")
	static FString DebugMessageIp();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | DebugMessage")
	static int DebugMessagePort();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | DebugMessage")
	static bool CloseAplRequest();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category = "DebugConfigLibrary | DebugMessage")
	static bool CloseAplResponse();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category= "DebugConfigLibrary | DebugMessage")
	static FString GetAnimationSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DebugConfigLibrary | DebugMessage")
	static bool IsMonkeyTest();
	
private:
	static void ReLoadConfig();
	static FString GetFileName();
};
