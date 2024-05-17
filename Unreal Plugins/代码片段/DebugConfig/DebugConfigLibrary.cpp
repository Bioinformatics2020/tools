// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugConfigLibrary.h"

void UDebugConfigLibrary::SetDebugConfig(const FString& Message)
{
	FString Section;
	FString RStr;
	TArray<FString> Outs;
	Message.ParseIntoArray(Outs, TEXT("|"));
	if (Outs.Num() == 4 && Outs[0] == "DebugConfig")
	{
		GConfig->SetString(*Outs[1], *Outs[2], *Outs[3], GetFileName());
	}
}

bool UDebugConfigLibrary::NotOpenApp()
{
#ifdef MONKEY_TEST
	ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("OpenWidget"), TEXT("NotOpenApp"), Ans, GetFileName());
	return Ans;
#elif
	return false;
#endif	
}

bool UDebugConfigLibrary::NotSwitchApp()
{
#ifdef MONKEY_TEST
	ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("OpenWidget"), TEXT("NotSwitchApp"), Ans, GetFileName());
	return Ans;
#elif
	return false;
#endif	
}

bool UDebugConfigLibrary::NotCloseApp()
{
#ifdef MONKEY_TEST
	ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("OpenWidget"), TEXT("NotCloseApp"), Ans, GetFileName());
	return Ans;
#elif
	return false;
#endif	
}

bool UDebugConfigLibrary::NotEnterApa()
{
#ifdef MONKEY_TEST
	ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("OpenWidget"), TEXT("NotEnterApa"), Ans, GetFileName());
	return Ans;
#elif
	return false;
#endif
}

bool UDebugConfigLibrary::DebugUserWidgetInfo()
{
#ifdef DebugSlateInfo
	//ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("DebugSlateInfo"), TEXT("DebugUserWidgetInfo"), Ans, GetFileName());
	return Ans;
#elif
	return false;
#endif	
}

bool UDebugConfigLibrary::DebugWidgetContainer()
{
#ifdef DebugSlateInfo
	//ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("DebugSlateInfo"), TEXT("DebugWidgetContainer"), Ans, GetFileName());
	return Ans;
#elif
	return false;
#endif	
}

bool UDebugConfigLibrary::DebugLayoutPointWidget()
{
#ifdef DebugSlateInfo
	//ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("DebugSlateInfo"), TEXT("DebugLayoutPointWidget"), Ans, GetFileName());
	return Ans;
#elif
	return false;
#endif	
}

bool UDebugConfigLibrary::DebugWidgetForce()
{
	bool Ans;
	GConfig->GetBool(TEXT("DebugSlateInfo"), TEXT("DebugWidgetForce"), Ans, GetFileName());
	return Ans;
}

bool UDebugConfigLibrary::DebugMessage()
{
	//ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("DebugMessage"), TEXT("DebugMessage"), Ans, GetFileName());
	return Ans;
}

FString UDebugConfigLibrary::DebugMessageIp()
{
	//ReLoadConfig();
	FString Ans;
	GConfig->GetString(TEXT("DebugMessage"), TEXT("DebugMessageIp"), Ans, GetFileName());
	return Ans;
}

int UDebugConfigLibrary::DebugMessagePort()
{
	//ReLoadConfig();
	int Ans;
	GConfig->GetInt(TEXT("DebugMessage"), TEXT("DebugMessagePort"), Ans, GetFileName());
	return Ans;
}

bool UDebugConfigLibrary::CloseAplRequest()
{
	//ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("DebugMessage"), TEXT("CloseAplRequest"), Ans, GetFileName());
	return Ans;
}

bool UDebugConfigLibrary::CloseAplResponse()
{
	//ReLoadConfig();
	bool Ans;
	GConfig->GetBool(TEXT("DebugMessage"), TEXT("CloseAplRequest"), Ans, GetFileName());
	return Ans;
}

FString UDebugConfigLibrary::GetAnimationSpeed()
{
	ReLoadConfig();
	return GConfig->GetStr(TEXT("DebugSlateInfo"), TEXT("AnimationSpeed"), GetFileName());
}

bool UDebugConfigLibrary::IsMonkeyTest()
{
#ifdef MONKEY_TEST 
	bool IsMonkey;
	GConfig->GetBool(TEXT("MonkeyTest"), TEXT("IsMonkeyTest"), IsMonkey, GetFileName());
	return IsMonkey;
#elif
	return false;
#endif
}

void UDebugConfigLibrary::ReLoadConfig()
{
	bool ForceReLoad;
	GConfig->GetBool(TEXT("ReLoad"), TEXT("ForceReLoad"), ForceReLoad, GetFileName());
	if (ForceReLoad)
	{
		GConfig->UnloadFile(GetFileName());
		GConfig->LoadFile(GetFileName());
	}
}

FString UDebugConfigLibrary::GetFileName()
{
	return FPaths::ProjectDir() / "Config/DebugConfig.ini";
}