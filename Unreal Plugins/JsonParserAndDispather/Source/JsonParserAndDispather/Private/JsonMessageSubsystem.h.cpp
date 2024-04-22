#include "JsonMessageSubsystem.h"

//运行测试案例
#include "Example/JsonParserExample.h"


UJsonMessageSubsystem* UJsonMessageSubsystem::Instance = nullptr;

UJsonMessageSubsystem::UJsonMessageSubsystem()
{
	Instance = this;
	UE_LOG(LogJsonParser,Log,TEXT("JsonParserAndDispather JSON解析与分发模块初始化"));
	JsonParser.OnComplete().BindRaw(&JsonDispather, &FJsonDispather::DispathMessage);
}

UJsonMessageSubsystem::~UJsonMessageSubsystem()
{
	Instance = nullptr;
	
	JsonParser.Stop();
	JsonParser.OnComplete().Unbind();
}

void UJsonMessageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	//运行测试案例
	// FJsonParserExample::GetInstance().Begin();
}

void UJsonMessageSubsystem::Deinitialize()
{
	//结束运行测试案例
	// FJsonParserExample::GetInstance().End();
	
	Super::Deinitialize();
}

UJsonMessageSubsystem& UJsonMessageSubsystem::GetInstance()
{
	checkf(Instance,TEXT("UJsonMessageSubsystem 尚未初始化"));
	return *Instance;
}

void UJsonMessageSubsystem::AddMessage(const FString& Message)
{
	JsonParser.AddMessage(Message);
}

void UJsonMessageSubsystem::Register(const FString&& MessageName, const FMessageResponse&& MessageResponseDelegate)
{
	JsonDispather.Register(Forward<const FString>(MessageName), Forward<const FMessageResponse>(MessageResponseDelegate));
}

void UJsonMessageSubsystem::UnRegister(const FString&& MessageName, const FString&& ID)
{
	if(Instance)
	{
		Instance->JsonDispather.UnRegister(Forward<const FString>(MessageName), Forward<const FString>(ID));
	}
}
