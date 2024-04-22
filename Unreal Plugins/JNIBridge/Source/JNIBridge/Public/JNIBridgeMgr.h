#pragma once
#include "CoreMinimal.h"

class JNIBRIDGE_API JNIBridgeMgr
{
	DECLARE_DELEGATE_OneParam(FAPLMessageEvent, const FString&);
	DECLARE_DELEGATE_OneParam(FAPLMessageOTAEvent, int);

public:
	static JNIBridgeMgr& GetInstance()
	{
		if (Instance == nullptr)
		{
			Instance = new JNIBridgeMgr();
		}
		return *Instance;
	}

	static void Destory()
	{
		if (Instance != nullptr)
		{
			delete Instance;
			Instance = nullptr;
		}
	}

	void JNIRegist();

	// void JNIRemove();
	void BeginPlaySendToAndroid();
	void StartOTAActivitSendToAndroidy();
	
	void Request(const FString& Msg);


	
	void RequestLightEnableState(const bool& EnableState);
	void RequestCarLightShowPicture();
	void RequestStartAVMActivity();

	void Response(const FString& Rep);
	void ResponseOTA(int Rep);
	
	FAPLMessageEvent& OnResponseMessage() const
	{
		return OnMessageEvent;
	}
	FAPLMessageOTAEvent OTAEvent;
private:
	JNIBridgeMgr() = default;
	~JNIBridgeMgr() = default;

	JNIBridgeMgr(const JNIBridgeMgr&) = delete;
	JNIBridgeMgr& operator=(const JNIBridgeMgr&) = delete;

	mutable FAPLMessageEvent OnMessageEvent;

	static JNIBridgeMgr* Instance;
};


