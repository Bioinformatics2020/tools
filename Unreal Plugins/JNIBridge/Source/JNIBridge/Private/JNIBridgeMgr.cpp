#include "JNIBridgeMgr.h"

#if PLATFORM_ANDROID
#include "Runtime/Launch/Public/Android/AndroidJNI.h"
#include "Runtime/ApplicationCore/Public/Android/AndroidApplication.h"
#endif

JNIBridgeMgr* JNIBridgeMgr::Instance = nullptr;

void JNIBridgeMgr::JNIRegist()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		static jmethodID MethonId_Test = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "InitAndroidtoue4library", "()V", bIsOptional);
		UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::JNIRegist"));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId_Test);
	}
#endif
}

void JNIBridgeMgr::BeginPlaySendToAndroid()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		static jmethodID MethonId_Test = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "BeginPlaySendToAndroid", "()V", bIsOptional);
		UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::JNIRegist"));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId_Test);
	}
#endif
}

void JNIBridgeMgr::StartOTAActivitSendToAndroidy()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		static jmethodID MethonId_Test = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "startOTAActivitSendToAndroidy", "()V", bIsOptional);
		UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::JNIRegist"));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId_Test);
	}
#endif
}

void JNIBridgeMgr::Request(const FString& Msg)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		static jmethodID MethonId_Test = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "SendToAndroidtoue4library", "(Ljava/lang/String;)V", bIsOptional);
		UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::Request %s"), *Msg);
		auto TokenJava = FJavaHelper::ToJavaString(Env, Msg);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId_Test,*TokenJava);
	}
#endif
}

void JNIBridgeMgr::RequestLightEnableState(const bool& EnableState)
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		static jmethodID MethonId_Test = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "LightStateEnableSendToAndroid", "(Z)V", bIsOptional);
		UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::RequestLightEnableState %d"), EnableState);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId_Test,EnableState);
	}
#endif
}

void JNIBridgeMgr::RequestCarLightShowPicture()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		static jmethodID MethonId_Test = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "CarLightShowPictureSendToAndroid", "()V", bIsOptional);
		UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::RequestCarLightShowPicture"));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId_Test);
	}
#endif
}

void JNIBridgeMgr::RequestStartAVMActivity()
{
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		static jmethodID MethonId_Test = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "startAVMActivitySendToAndroid", "()V", bIsOptional);
		UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::RequestStartAVMActivity"));
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId_Test);
	}
#endif
}

void JNIBridgeMgr::Response(const FString& Rep)
{
	UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::Response %s"), *Rep);
	if (OnMessageEvent.IsBound())
	{
		OnMessageEvent.Execute(Rep);
	}
}

void JNIBridgeMgr::ResponseOTA(int Rep)
{
	UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::ResponseOTA %d"), Rep);
	FFunctionGraphTask::CreateAndDispatchWhenReady( 
					[this,Rep]() 
					{
						OTAEvent.ExecuteIfBound(Rep);
						UE_LOG(LogTemp, Log, TEXT("JNIBridgeMgr::ResponseOTA %d over"), Rep);
					}, 
					TStatId(), 
					nullptr, 
					ENamedThreads::GameThread); 
}

#if PLATFORM_ANDROID
//Java调用C++代码示例，JNIEnv * LocalJNIEnv, jobject LocalThiz  必须要有，jint value为java函数形参，jint对应c++的int
extern "C" void Java_com_epicgames_ue4_GameActivity_nativeReaponseAndroidtoue4library(JNIEnv * LocalJNIEnv, jobject LocalThiz,jstring Value)
{
	FString UeString = FJavaHelper::FStringFromParam(LocalJNIEnv, Value);
	JNIBridgeMgr::GetInstance().Response(UeString);
}

//Java调用C++代码示例，JNIEnv * LocalJNIEnv, jobject LocalThiz  必须要有，jint value为java函数形参，jint对应c++的int
extern "C" void Java_com_epicgames_ue4_GameActivity_nativenotifyOtaStatus(JNIEnv * LocalJNIEnv, jobject LocalThiz,jint Value)
{
	JNIBridgeMgr::GetInstance().ResponseOTA(Value);
}
#endif