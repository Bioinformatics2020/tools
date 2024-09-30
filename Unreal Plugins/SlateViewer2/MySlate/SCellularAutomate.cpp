// Fill out your copyright notice in the Description page of Project Settings.


#include "SCellularAutomate.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SCellularAutomate::Construct(const FArguments &InArgs)
{
	if (Height <= 0 || Width <= 0)
	{
		return;
	}

	TSharedPtr<SVerticalBox> VerticalBox;
	ChildSlot
	[
		SAssignNew(VerticalBox, SVerticalBox)
	];

	for (int i = 0; i < Height; ++i)
	{
		TSharedPtr<SHorizontalBox> HorizontalBox;
		VerticalBox->AddSlot()
		           .FillHeight(1)
		[
			SAssignNew(HorizontalBox, SHorizontalBox)
		];
		for (int j = 0; j < Width; ++j)
		{
			HorizontalBox->AddSlot()
			             .Padding(1)
			             .FillWidth(1)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SCellularAutomate::GetColor, i, j)
				.Image(FCoreStyle::Get().GetBrush("WhiteBrush"))
			];
		}
	}
	Cells = std::vector<std::vector<bool>>(Height, std::vector<bool>(Width,false));
	LastCells = Cells;
	BeginPattern();
	
	//这个回调事件存在于基类，不必担心生命周期
	RegisterActiveTimer(0.5, FWidgetActiveTimerDelegate::CreateRaw(this, &SCellularAutomate::TimerTick));

	// while (true)
	{
		TimerTick(0,0);
	}
}

void SCellularAutomate::BeginPattern()
{
	Cells[Height / 2][Width / 2] = true;
}

EActiveTimerReturnType SCellularAutomate::TimerTick(double InCurrentTime, float InDeltaTime)
{
	TickCount++;
	OutPutHash();
	
	Swap(LastCells, Cells);
	for (int i = 0; i < Height; ++i)
	{
		for (int j = 0; j < Width; ++j)
		{
			Cells[i][j] = TickPattern(i, j);
		}
	}
	return EActiveTimerReturnType::Continue;
}

bool SCellularAutomate::TickPattern(int i, int j) const
{
	int AliveCount = 0;
	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			if (i + y >= 0 && i + y < Height && j + x >= 0 && j + x < Width)
			{
				if (LastCells[i + y][j + x])
				{
					AliveCount++;
				}
			}
		}
	}
	return AliveCount > 0 && AliveCount <= 4;
}

void SCellularAutomate::OutPutHash()
{
	int Len = Width/8 + (Width%8 > 0 ? 1:0);
	uint64 Hash = 0;
	for (int i = 0; i < Height; ++i)
	{
		Hash = CityHash64WithSeed((char*)Cells[i].begin()._Myptr, Len, Hash);
	}
	
	if (HashMap.Find(Hash))
	{
		UE_LOG(LogTemp, Log, TEXT("LashCount:%-4d Count:%-4d"), HashMap[Hash], TickCount);
		HashMap[Hash] = TickCount;
	}
	else
	{
		HashMap.Add(Hash, TickCount);
	}
}

FSlateColor SCellularAutomate::GetColor(int i, int j) const
{
	return Cells[i][j] ? FSlateColor(FLinearColor::White) : FSlateColor(FLinearColor::Black);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
