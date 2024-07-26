// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RoadSpawn.generated.h"

/**
 * 
 */
UCLASS()
class HOTUPDATE_API URoadSpawn : public UObject
{
	GENERATED_BODY()

public:
	//基于道路中心轴线，生成道路网格体顶点，要求中心点不能出现180度转弯，不能垂直向上，不支持倾斜路面
	UFUNCTION(BlueprintCallable)
	static TArray<FVector> GetRoadPoints(TArray<FVector> LinePoints, float Width);
};
