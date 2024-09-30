#pragma once

struct FDouble2D
{
	double X;
	double Y;

	FDouble2D() = default;
	
	FDouble2D(double InX, double InY)
		: X(InX)
		, Y(InY)
	{}

	FDouble2D operator+(FDouble2D Other) const
	{
		return FDouble2D(X + Other.X, Y + Other.Y);
	}

	FDouble2D operator-(FDouble2D Other) const
	{
		return FDouble2D(X - Other.X, Y - Other.Y);
	}
	FDouble2D operator*(double Other) const
	{
		return FDouble2D(X * Other, Y * Other);
	}
	FDouble2D operator/(double Other) const
	{
		return FDouble2D(X / Other, Y / Other);
	}
};

class STestFractal : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(STestFractal)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override
	{
		return FVector2D(100, 100);
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	// pow(3,0.5)/2
	double Length = 0.86602540378443864676372317075294;
	//绘制科赫雪花线
	void KochSnowflakePoints(TArray<FDouble2D>& Points, int32 Level);
	int32 PaintKochSnowflake(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;
	
	TArray<FDouble2D> LinePoints;
};

