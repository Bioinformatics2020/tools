#pragma once
#include "JsonStruct/JsonStructBase.h"

DECLARE_DELEGATE_OneParam(FMessageResponseDelegate, TSharedPtr<void>);

struct FMessageResponse
{
	FMessageResponse(const FString&& InID,const UStruct* InStructDefinition,FMessageResponseDelegate&& InMessageResponseDelegate,bool InLoadCache=true)
		:ID(InID),StructDefinition(InStructDefinition),MessageResponseDelegate(InMessageResponseDelegate),LoadCache(InLoadCache){};
	
	FString ID;
	const UStruct* StructDefinition;
	FMessageResponseDelegate MessageResponseDelegate;
	bool LoadCache;

	bool operator==(const FMessageResponse& Other) const
	{
		return ID == Other.ID;
	}
};

class FJsonDispather
{
public:
	//分发函数在消息解析线程进行
	void DispathMessage(const FString& MessageName, TSharedRef<FJsonObject> MessageProtocol);

	//缓存的消息分发在消息注册时执行
	void DispathCacheMessage(const FString& MessageName, const FString& ID);

	void Register(const FString&& MessageName, const FMessageResponse&& MessageResponseDelegate);
	void UnRegister(const FString&& MessageName, const FString&& ID);
private:
	
	
	TMap<FString, TArray<FMessageResponse>> MessageResponses;
	FRWLock MessageResponsesRWLock;

	TMap<FString, TSharedRef<FJsonObject>> MessageCache;
	FRWLock CacheRWLock;
};
