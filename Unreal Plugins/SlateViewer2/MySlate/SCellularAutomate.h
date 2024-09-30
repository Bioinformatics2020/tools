// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class SLATEVIEWER2_API SCellularAutomate : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCellularAutomate)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments &InArgs);

	void BeginPattern();
	EActiveTimerReturnType TimerTick(double InCurrentTime, float InDeltaTime);
	bool TickPattern(int i, int j) const;
	void OutPutHash();

	FSlateColor GetColor(int i,int j) const;


	std::vector<std::vector<bool>> Cells;
	std::vector<std::vector<bool>> LastCells;

	TMap<uint64,int> HashMap;

	int TickCount=0;
	int Width=64;
	int Height=64;
};
