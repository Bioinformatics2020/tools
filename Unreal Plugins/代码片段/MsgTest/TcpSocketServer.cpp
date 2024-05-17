// Fill out your copyright notice in the Description page of Project Settings.

#include "TcpSocketServer.h"

#include "TcpSocketSetting.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Sockets/Public/Sockets.h"
#include "Common/TcpListener.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "NIC/APL/APLMgr.h"
#include "NIC/UIControl/Setting/SettingCtl.h"

void ATcpSocketServer::Key_F_Event()
{
	SettingCtl::GetInstance().CallOSDByKeyBoard_2_F();
}

void ATcpSocketServer::Key_T_Event()
{
	SettingCtl::GetInstance().CallOSDByKeyBoard_1_T();
}

void ATcpSocketServer::Key_V_Event()
{
	SettingCtl::GetInstance().CallOSDByKeyBoard_3_V();
}

// Sets default values
ATcpSocketServer::ATcpSocketServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

bool ATcpSocketServer::HandleConnectionAccepted(FSocket* ClientSocket, const FIPv4Endpoint& ClientEndpoint)
{
	UE_LOG(LogTemp, Warning, TEXT("Connection accepted from %s"), *ClientEndpoint.ToString());

	
	// 创建TCP socket
	TSharedPtr<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	ClientSocket->GetPeerAddress(*RemoteAddress);

	// 接收消息
	TArray<uint8> ReceiveData;
	uint32 ReceivedDataSize = 0;
	
	FString ReceiveStr;
	const int32 BufferSize = 1024;
	uint8 Buffer[1024];
	int32 BytesRead = 0;
	ClientSocket->SetNonBlocking(false);
	//while(1)
	{
		ReceiveStr.Empty();
		ReceiveData.Empty();
		memset(Buffer,0,BufferSize);
		while (ClientSocket->Recv(Buffer, BufferSize, BytesRead))
		{
			ReceiveData.Append(Buffer,BytesRead);
			ReceivedDataSize += BytesRead;
			memset(Buffer,0,BufferSize);
		}
		ReceiveData.Push(0);
		ReceiveStr = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceiveData.GetData())));
		UE_LOG(LogTemp, Log, TEXT("Receive22: %s %d"),*ReceiveStr,ReceivedDataSize);
		FAplMgr::GetInstance().Response(ReceiveStr);
	}

	// 回显消息
	const FString Message = FString::Printf(TEXT("bye"));
	TArray<uint8> ResponseData;
	FTCHARToUTF8 Converter(*Message);
	ResponseData.Append((uint8*)Converter.Get(), Converter.Length());
	int32 BytesSent = 0;
	ClientSocket->Send(ResponseData.GetData(), ResponseData.Num(),BytesSent);

	// 关闭连接
	ClientSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
	return true;
}

// Called when the game starts or when spawned
void ATcpSocketServer::BeginPlay()
{
	Super::BeginPlay();

	check(!Thread && "Thread wasn't null at the start!");
	check(FPlatformProcess::SupportsMultithreading() && "This platform doesn't support multithreading!");	
	if (Thread)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Thread isn't null. It's: %s"), *Thread->GetThreadName());
	}
	Thread = FRunnableThread::Create(this, TEXT("FTcpSocketWorker"), 128 * 1024, TPri_Normal);
	UE_LOG(LogTemp, Log, TEXT("Log: Created thread"));
	if (InputComponent)
	{
		InputComponent->BindAction("Key_F", IE_Pressed, this, &ATcpSocketServer::Key_F_Event);
		InputComponent->BindAction("Key_T", IE_Pressed, this, &ATcpSocketServer::Key_T_Event);
		InputComponent->BindAction("Key_V", IE_Pressed, this, &ATcpSocketServer::Key_V_Event);
	}
}

void ATcpSocketServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TcpListener->Stop();
	if(TcpListener)
	{
		delete TcpListener;
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATcpSocketServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATcpSocketServer::PrintToConsole(FString Str, bool Error)
{
	if (auto tcpSocketSettings = GetDefault<UTcpSocketSetting>())
	{
		if (Error && tcpSocketSettings->bPostErrorsToMessageLog)
		{
			auto messageLog = FMessageLog("Tcp Socket Plugin");
			messageLog.Open(EMessageSeverity::Error, true);
			messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
		}
	}
}

bool ATcpSocketServer::Init()
{
	bRun = true;
	bConnected = false;
	return FRunnable::Init();
}

uint32 ATcpSocketServer::Run()
{
	const int32 Port = 5021;
	FString ipAddress = "127.0.0.1";
	FIPv4Address ip;
	FIPv4Address::Parse(ipAddress, ip);
	FIPv4Endpoint Endpoint(ip,Port);
	TcpListener = new FTcpListener(Endpoint);
	TcpListener->OnConnectionAccepted().BindUObject(this, &ATcpSocketServer::HandleConnectionAccepted);
	return 0;
}

void ATcpSocketServer::Stop()
{
	FRunnable::Stop();
}

void ATcpSocketServer::Exit()
{
	FRunnable::Exit();
}

