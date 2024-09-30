#include "STestFractal.h"

void STestFractal::Construct(const FArguments& InArgs)
{
	KochSnowflakePoints(LinePoints,5);
}

int32 STestFractal::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                            FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                            bool bParentEnabled) const
{
	// 绘制科赫雪花
	LayerId = PaintKochSnowflake(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	return LayerId;
}

void STestFractal::KochSnowflakePoints(TArray<FDouble2D>& Points, int32 Level)
{
	Points.Add(FDouble2D(-Length * 0.8,-0.4));
	Points.Add(FDouble2D(Length * 0.8,-0.4));
	Points.Add(FDouble2D(0,0.8));
	Points.Add(FDouble2D(-Length * 0.8,-0.4));
	
	for(int CurentLevel = 0; CurentLevel < Level; CurentLevel++)
	{
		TArray<FDouble2D> NewPoints;
		NewPoints.SetNum((Points.Num()-1) * 4 + 1);
		NewPoints[0] = Points[0];

		for (int32 i = 1; i < Points.Num(); i++)
		{
			int32 j = (i-1)*4 + 1;
			NewPoints[j] = Points[i-1]*2/3 + Points[i]*1/3;
			NewPoints[j+2] = Points[i-1]*1/3 + Points[i]*2/3;
			NewPoints[j+3] = Points[i];
			
			FDouble2D Cross = NewPoints[j+2]-NewPoints[j];
			double temp = Cross.X;
			Cross.X = Cross.Y;
			Cross.Y = -temp;
			
			NewPoints[j+1] = Cross*Length + NewPoints[j+2]/2 + NewPoints[j]/2;
		}

		Points = NewPoints;
	}
}

int32 STestFractal::PaintKochSnowflake(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	TArray<FVector2D> PaintLinePoints;
	//坐标归一化
	for(auto& Point : LinePoints)
	{
		const FDouble2D Temp = (Point * 0.5 + FDouble2D(0.5,0.5)) * AllottedGeometry.Size.X;
		PaintLinePoints.Add(FVector2D(Temp.X, Temp.Y));
	}
	
	// Draw lines
	FSlateDrawElement::MakeLines( 
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(),
		PaintLinePoints,
		ESlateDrawEffect::None,
		FLinearColor::White,
		true,
		1
		);

	return LayerId;
}