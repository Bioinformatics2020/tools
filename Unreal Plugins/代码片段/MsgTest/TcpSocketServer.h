// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TcpSocketServer.generated.h"

UCLASS()
class NIC_API ATcpSocketServer : public AActor,public FRunnable
{
	GENERATED_BODY()
	
public:
	void Key_F_Event();
	void Key_T_Event();
	void Key_V_Event();
	// Sets default values for this actor's properties
	ATcpSocketServer();
	bool HandleConnectionAccepted(class FSocket* ClientSocket, const struct FIPv4Endpoint& ClientEndpoint);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	static void PrintToConsole(FString Str, bool Error);

	class FTcpListener* TcpListener;
	FRunnableThread* Thread = nullptr;
	FThreadSafeBool bRun = false;
	FThreadSafeBool bConnected = false;
	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	// End FRunnable interface
};