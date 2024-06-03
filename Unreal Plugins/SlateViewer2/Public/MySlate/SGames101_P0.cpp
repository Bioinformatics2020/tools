#include "SGames101_P0.h"

inline int32 SGames101_P0::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	FSlateColorBrush WhiteBox = FSlateColorBrush(FColor::White);

	//给定输出坐标：以屏幕中心为(0,0)
	FVector2D InPoint(0.2,0.1);

	//坐标变换
	//方法1，手动直接计算
	//FVector2D OutPoint = FVector2D(-InPoint.Y,InPoint.X);
	//方法2，矩阵运算库
	FMatrix2x2 Matrix(0,1,-1,0);
	FVector2D OutPoint = Matrix.TransformPoint(InPoint);

	
	//归一化到屏幕坐标
	OutPoint = (OutPoint + FVector2D(0.5,0.5)) * AllottedGeometry.Size;
	//翻转坐标系，使左下角为(0,0)，右上角为(AllottedGeometry.Size.X,AllottedGeometry.Size.Y)
	OutPoint.Y = AllottedGeometry.Size.Y - OutPoint.Y;
	
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(16,16), FSlateLayoutTransform(OutPoint)),
		&WhiteBox
		);
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
									bParentEnabled);
}