#pragma once

#include "Widgets/SCompoundWidget.h"

enum class ECoordinateType
{
	Coordinate,//相对控件左上角的像素坐标
	UV_0,//从(0,0)到(1,1)
	UV_1,//从(-1,-1)到(1,1)
	UV_2,//从(-0.5,-0.5)到(0.5,0.5)
};

class STestCompoundWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STestCompoundWidget)
	: _SplineThickness(1.0f)
	, _CoordinateType(ECoordinateType::Coordinate)
	{}
		SLATE_ATTRIBUTE(float, SplineThickness)
		SLATE_ARGUMENT(bool, AutoMove)
		SLATE_ARGUMENT(ECoordinateType,CoordinateType)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		SplineThickness = InArgs._SplineThickness;
		CoordinateType = InArgs._CoordinateType;
		AutoMove = InArgs._AutoMove;
	}

private:
	
	//~ SWidget
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void CalPaintPoint(const FGeometry& AllottedGeometry,TArray<FVector2D>& Coordinates) const;

	FVector2D CalBezierPoints(const FGeometry& MyGeometry, FVector2D InSoftwareCursorPosition) const;
	FVector2D CalScreenPoints(const FGeometry& MyGeometry, FVector2D InUV) const;

	const FVector2D BezierPointRadius = FVector2D(2,2);
	FSlateColorBrush WhiteBox = FSlateColorBrush(FColor::White);
	TArray<FVector2D> BezierPoints;
	TAttribute<float> SplineThickness;
	bool AutoMove;
	ECoordinateType CoordinateType = ECoordinateType::Coordinate;

public:
	//实现曲线移动的动画
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void MoveAllPoint(FVector2D InMoveDistance);
	void AutoAddPoint();
private:
	FVector2D MoveDistance = FVector2D(-0.1,0);

	float AddPointTime = 0.5;

public:
	//处理输入相关的数据
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void AddPoint(FVector2D NewPoint);
	void RemoveLastPoint();
private:
	//最接近的控制点索引
	int NearPoint = -1;
	//距离控制点中心的距离
	FVector2D NearOffset;

	//输入点
	FVector2D LastInputPoint = FVector2D(-1,0);
	FVector2D InputPoint = FVector2D(-1,0);
	float Step = 0.02;
};
