#pragma once

#include "CoreMinimal.h"
#include "JsonMessageSubsystem.h"
#include "JsonStruct/JsonStructBase.h"
#include "JsonParserAndDispatherCommon.h"
#include "JsonParserExample.generated.h"

USTRUCT()
struct FJsonStructExample : public FJsonStructBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FString data;
};

class FJsonParserExample
{
	JSONPARSERANDDISPATHER_DECLARE_SINGLE_INSTANCE(FJsonParserExample)
public:
	void Begin()
	{
		//注册监听事件
		REGREST_MESSAGE(TEXT("ExampleAddr1"),TEXT("ID"),FJsonStructExample,FJsonParserExample::ExampleMessage1);
		REGREST_MESSAGE(TEXT("ExampleAddr2"),TEXT("ID1"),FJsonStructExample,FJsonParserExample::ExampleMessage1);
		//-----基本功能示例
		//发送消息
		UJsonMessageSubsystem::GetInstance().AddMessage(TEXT("{\"Name\":\"ExampleAddr1\",\"data\":\"ExampleData\"}"));

		
		//-----同一条消息，多个回调示例
		//发送消息
		UJsonMessageSubsystem::GetInstance().AddMessage(TEXT("{\"Name\":\"ExampleAddr2\",\"data\":\"ExampleData\"}"));

		
		//-----异常消息示例
		//JSON格式错误
		UJsonMessageSubsystem::GetInstance().AddMessage(TEXT("{\"Name\":\"ExampleAddr3\",\"data\":\"ExampleData\",\"err\"}"));
		//JSON格式不匹配
		UJsonMessageSubsystem::GetInstance().AddMessage(TEXT("{\"Name\":\"ExampleAddr3\",\"data2\":\"ExampleData\"}"));
		

		//JSON未监听，直接发送消息
		UJsonMessageSubsystem::GetInstance().AddMessage(TEXT("{\"Name\":\"ExampleAddr4\",\"data\":\"ExampleData\"}"));

		
		REGREST_MESSAGE(TEXT("ExampleAddr2"),TEXT("ID2"),FJsonStructExample,FJsonParserExample::ExampleMessage2);
		REGREST_MESSAGE(TEXT("ExampleAddr3"),TEXT("ID1"),FJsonStructExample,FJsonParserExample::ExampleMessage1);
	}

	void End()
	{
		UNREGREST_MESSAGE(TEXT("ExampleAddr1"),TEXT("ID"));
		UNREGREST_MESSAGE(TEXT("ExampleAddr2"),TEXT("ID1"));
		UNREGREST_MESSAGE(TEXT("ExampleAddr2"),TEXT("ID2"));
		UNREGREST_MESSAGE(TEXT("ExampleAddr3"),TEXT("ID1"));
	}
	
	void ExampleMessage1(TSharedPtr<void> InData)
	{
		FJsonStructExample* ExampleData = static_cast<FJsonStructExample*>(InData.Get());
		if(ExampleData)
		{
			UE_LOG(LogJsonParser,Log,TEXT("ExampleData1 Name:%s Data:%s"),*ExampleData->Name,*ExampleData->data);
		}
	}
	void ExampleMessage2(TSharedPtr<void> InData)
	{
		FJsonStructExample* ExampleData = static_cast<FJsonStructExample*>(InData.Get());
		if(ExampleData)
		{
			UE_LOG(LogJsonParser,Log,TEXT("ExampleData2 Name:%s Data:%s"),*ExampleData->Name,*ExampleData->data);
		}
	}

};
