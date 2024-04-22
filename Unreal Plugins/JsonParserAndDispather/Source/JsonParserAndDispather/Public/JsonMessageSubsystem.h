#pragma once
#include "CoreMinimal.h"
#include "JsonDispather.h"
#include "JsonParserAndDispatherCommon.h"
#include "JsonParserRunnable.h"
#include "JsonMessageSubsystem.generated.h"

class FJsonParserRunnable;

UCLASS(Blueprintable)
class JSONPARSERANDDISPATHER_API UJsonMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UJsonMessageSubsystem();
	~UJsonMessageSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UJsonMessageSubsystem& GetInstance();

	//解析消息，暴露给蓝图，便于调试
	UFUNCTION(BlueprintCallable)
	void AddMessage(const FString& Message);

	//注册监听事件
	void Register(const FString&& MessageName, const FMessageResponse&& MessageResponseDelegate);
	//取消注册监听事件
	static void UnRegister(const FString&& MessageName, const FString&& ID);
private:
	//解析线程  
	FJsonParserRunnable JsonParser;
	FJsonDispather JsonDispather;

	static UJsonMessageSubsystem* Instance;
};
