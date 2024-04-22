#pragma once
#include "CoreMinimal.h"
#include "JsonStructBase.generated.h"

USTRUCT()
struct FJsonStructBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FString Name;
};
