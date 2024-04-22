// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "JsonStruct/JsonStructBase.h"
#include "UObject/Object.h"

/**
 * 一个Json解析线程
 */
class FJsonParserRunnable final : public FRunnable
{
public:
	DECLARE_DELEGATE_TwoParams(FsonParserComplete, const FString&, TSharedRef<FJsonObject>);

	FJsonParserRunnable()
	{
		bRunning = true;
		// 初始化线程
		Thread = FRunnableThread::Create(this, TEXT("JsonParserThread"));
	}

	virtual ~FJsonParserRunnable() override
	{
		if (Thread)
		{
			Thread->Kill(true);
			Thread = nullptr;
		}
	}

	void AddMessage(const FString& Message)
	{
		Messages.Enqueue(Message);
	}

	virtual bool Init() override
	{
		return true;
	}

	virtual uint32 Run() override
	{
		while (bRunning)
		{
			FString Message;
			if (Messages.Dequeue(Message))
			{
				UE_LOG(LogJsonParser,Log,TEXT("消息出队，开始解析begin:[%s]"), *Message);
				const double StartTime = FPlatformTime::Seconds();
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Message);
				if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
				{
					FJsonStructBase MessageData;
					if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &MessageData))
					{
						if(MessageData.Name == "")
						{
							UE_LOG(LogJsonParser, Warning, TEXT("解析失败, 请检查 Json 是否包含Name字段"));
							continue;
						}
						if (OnParserComplete.IsBound())
						{
							OnParserComplete.Execute(MessageData.Name, JsonObject.ToSharedRef());
							const double EndTime = FPlatformTime::Seconds();
							double ExecutionTime = EndTime - StartTime;
							UE_LOG(LogJsonParser,Verbose, TEXT("消息解析End: %f seconds"), ExecutionTime);
							continue;
						}
					}
				}
				UE_LOG(LogJsonParser, Warning, TEXT("解析失败, 请检查 Json 格式是否正确!"));
				
			}
			else
			{
				//没有任务等待
				FPlatformProcess::Sleep(0.01f);
			}
		}
		return 0;
	}

	virtual void Stop() override
	{
		Messages.Empty();
		bRunning = false;
	}

	virtual void Exit() override { }

	FsonParserComplete& OnComplete()
	{
		return OnParserComplete;
	}

private:
	TQueue<FString, EQueueMode::Spsc> Messages;
	FsonParserComplete OnParserComplete;
	FRunnableThread* Thread;
	bool bRunning;
};