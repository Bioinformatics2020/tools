#pragma once

#include "Widgets/SCompoundWidget.h"

enum class ETestDrawShape
{
	Cicle,
	CircularRing
};

class STestVertex : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STestVertex)
		:_TestDrawShape(ETestDrawShape::CircularRing)
	{}
	SLATE_ARGUMENT(ETestDrawShape,TestDrawShape)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		switch (InArgs._TestDrawShape)
		{
		case ETestDrawShape::Cicle:
			DrawCicle();
			break;
		case ETestDrawShape::CircularRing:
			DrawCircularRing();
			break;
		default: ;
		}
	}

protected:
	void DrawCicle();
	void DrawCircularRing();

protected:
	//~ SWidget
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	//顶点UV坐标
	TArray<FVector2D> Points;
	//顶点的绘制顺序
	TArray<SlateIndex> Indexes;
};
