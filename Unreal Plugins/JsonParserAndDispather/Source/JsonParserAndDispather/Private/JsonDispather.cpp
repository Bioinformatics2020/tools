#include "JsonDispather.h"

#include "JsonObjectConverter.h"
#include "JsonParserAndDispatherCommon.h"

void FJsonDispather::DispathMessage(const FString& MessageName, TSharedRef<FJsonObject> MessageProtocol)
{
	MessageResponsesRWLock.ReadLock();
	const auto Responses = MessageResponses.Find(MessageName);
	if(Responses)
	{
		for (auto& Response : *Responses)
		{
			void* SampleStructMemory = (void*)FMemory::Malloc(Response.StructDefinition->GetStructureSize());
			Response.StructDefinition->InitializeStruct(SampleStructMemory);
			TSharedPtr<void> StructMemoryPtr(SampleStructMemory,[](void* ptr){FMemory::Free(ptr);});
			
			if(FJsonObjectConverter::JsonObjectToUStruct(MessageProtocol,Response.StructDefinition,SampleStructMemory,0,0))
			{
				FFunctionGraphTask::CreateAndDispatchWhenReady( 
					[StructMemoryPtr,Response]() 
					{  
						if (Response.MessageResponseDelegate.IsBound()) 
						{ 
							Response.MessageResponseDelegate.Execute(StructMemoryPtr);
						}
						UE_LOG(LogJsonParser,Verbose,TEXT("JSON消息处理完成 Name:%s"),*Response.ID,*Response.StructDefinition->GetName());
					}, 
					TStatId(), 
					nullptr, 
					ENamedThreads::GameThread); 
				UE_LOG(LogJsonParser,Verbose,TEXT("JSON解析与分发完成 Name:%s Struct:%s"),*MessageName,*Response.StructDefinition->GetName());
			}
			else
			{
				UE_LOG(LogJsonParser,Warning,TEXT("JSON解析失败 Name:%s Struct:%s"),*MessageName,*Response.StructDefinition->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogJsonParser,Warning,TEXT("JSON消息未监听 Name:%s"),*MessageName);
	}
	MessageResponsesRWLock.ReadUnlock();
	
	//缓存消息，便于新注册时直接使用
	CacheRWLock.WriteLock();
	MessageCache.FindOrAdd(MessageName,MessageProtocol);
	CacheRWLock.WriteUnlock();
}

void FJsonDispather::DispathCacheMessage(const FString& MessageName, const FString& ID)
{
	CacheRWLock.ReadLock();
	auto MessagePtr = MessageCache.Find(MessageName);
	if(MessagePtr)
	{
		//目前DispathCacheMessage运行于游戏线程，MessageResponses不必加锁
		const auto Responses = MessageResponses.Find(MessageName);
		FMessageResponse* ResponsePtr = Responses->FindByPredicate(
			[ID](FMessageResponse& a){return a.ID == ID;});
		if(ResponsePtr)
		{
			auto& Response = *ResponsePtr;
			auto& MessageProtocol = *MessagePtr;
			void* SampleStructMemory = (void*)FMemory::Malloc(Response.StructDefinition->GetStructureSize());
			Response.StructDefinition->InitializeStruct(SampleStructMemory);
			TSharedPtr<void> StructMemoryPtr(SampleStructMemory,[](void* ptr){FMemory::Free(ptr);});
			
			if(FJsonObjectConverter::JsonObjectToUStruct(MessageProtocol,Response.StructDefinition,SampleStructMemory,0,0))
			{
				FFunctionGraphTask::CreateAndDispatchWhenReady( 
					[StructMemoryPtr,Response]() 
					{  
						if (Response.MessageResponseDelegate.IsBound()) 
						{ 
							Response.MessageResponseDelegate.Execute(StructMemoryPtr);
						}
						UE_LOG(LogJsonParser,Verbose,TEXT("JSON消息处理完成 Name:%s"),*Response.ID,*Response.StructDefinition->GetName());
					}, 
					TStatId(), 
					nullptr, 
					ENamedThreads::GameThread); 
				UE_LOG(LogJsonParser,Verbose,TEXT("JSON解析与分发完成 Name:%s Struct:%s"),*MessageName,*Response.StructDefinition->GetName());
			}
			else
			{
				UE_LOG(LogJsonParser,Warning,TEXT("JSON解析失败 Name:%s Struct:%s"),*MessageName,*Response.StructDefinition->GetName());
			}
		}
	}
	
	CacheRWLock.ReadUnlock();
}

void FJsonDispather::Register(const FString&& MessageName, const FMessageResponse&& MessageResponseDelegate)
{
	//这一行不会出现写操作，不必加锁
	const auto Responses = MessageResponses.Find(MessageName);
	if(Responses)
	{
		FMessageResponse* Response = Responses->FindByPredicate(
			[MessageResponseDelegate](FMessageResponse& a){return a.ID == MessageResponseDelegate.ID;});
		if(Response)
		{
			MessageResponsesRWLock.WriteLock();
			*Response = MessageResponseDelegate;
			MessageResponsesRWLock.WriteUnlock();
		}
		else
		{
			MessageResponsesRWLock.WriteLock();
			Responses->Add(MessageResponseDelegate);
			MessageResponsesRWLock.WriteUnlock();

			UE_LOG(LogJsonParser,Verbose,TEXT("JSON消息注册 Name:%s ID:%s"),*MessageName, *MessageResponseDelegate.ID);
			//注册新消息时加载缓存
			DispathCacheMessage(MessageName,MessageResponseDelegate.ID);
		}
	}
	else
	{
		MessageResponsesRWLock.WriteLock();
		MessageResponses.Add(MessageName,{MessageResponseDelegate});
		MessageResponsesRWLock.WriteUnlock();

		UE_LOG(LogJsonParser,Verbose,TEXT("JSON Register Name:%s"),*MessageName);
		//注册新消息时加载缓存
		DispathCacheMessage(MessageName,MessageResponseDelegate.ID);
	}
}

void FJsonDispather::UnRegister(const FString&& MessageName, const FString&& ID)
{
	//这一行不会出现写操作，不必加锁
	const auto Responses = MessageResponses.Find(MessageName);
	if(Responses)
	{
		FMessageResponse* Response = Responses->FindByPredicate(
			[ID](FMessageResponse& a){return a.ID == ID;});
		if(Response)
		{
			UE_LOG(LogJsonParser,Warning,TEXT("解除JSON消息注册 Name:%s ID:%s"),*MessageName,*ID);
			
			MessageResponsesRWLock.WriteLock();
			Responses->RemoveSingleSwap(*Response);
			MessageResponsesRWLock.WriteUnlock();
			if(Responses->Num()==0)
			{
				MessageResponsesRWLock.WriteLock();
				MessageResponses.Remove(MessageName);
				MessageResponsesRWLock.WriteUnlock();
			}
		}
		else
		{
			UE_LOG(LogJsonParser,Warning,TEXT("无效的解除JSON消息注册 Name:%s ID:%s"),*MessageName,*ID);
		}
	}
	else
	{
		UE_LOG(LogJsonParser,Warning,TEXT("无效的解除JSON消息注册 Name:%s"),*MessageName);
	}
}
