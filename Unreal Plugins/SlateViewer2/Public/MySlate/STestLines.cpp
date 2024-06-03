#include "STestLines.h"

int32 STestLines::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	TArray< FVector2D > LinePoints;

	// LinePoints.Add(FVector2D(-1,-1));
	// LinePoints.Add(FVector2D(1,1));


	float ratio = AllottedGeometry.Size.X / AllottedGeometry.Size.Y;
	for(int i=0;i<20 * ratio;i++)
	{
		float x = i/10.0;
		float y = -sin(PI * x) * 0.5;
		LinePoints.Add(FVector2D(x,y));
	}


	//坐标归一化
	for(auto& Point : LinePoints)
	{
		Point = (Point * 0.5 + FVector2D(0,0.5)) * AllottedGeometry.Size.Y;
	}
	
	// Draw lines
	FSlateDrawElement::MakeLines( 
		OutDrawElements,
		LayerId++,
		AllottedGeometry.ToPaintGeometry(),
		LinePoints,
		ESlateDrawEffect::None,
		FLinearColor::White,
		true,
		2
		);

	FVector2D Centre = AllottedGeometry.Size / 2;
	FSlateColorBrush WhiteBox = FSlateColorBrush(FColor::White);
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(2,2), FSlateLayoutTransform(Centre)),
		&WhiteBox,				
		ESlateDrawEffect::None,
		FLinearColor::Red
	);
	
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
									bParentEnabled);
}
