#include "STestCompoundWidget.h"

int32 STestCompoundWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bEnabled = ShouldBeEnabled(bParentEnabled);
		
	TArray<FVector2D> Coordinates;
	Coordinates.SetNum(BezierPoints.Num());
	CalPaintPoint(AllottedGeometry,Coordinates);
		
	// Draw control points.
	for (int i = 0; i < BezierPoints.Num(); ++i)
	{
		FLinearColor ColorToUse = FLinearColor(i*15,1,1).HSVToLinearRGB();

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(2* BezierPointRadius, FSlateLayoutTransform(Coordinates[i]- BezierPointRadius)),
			&WhiteBox,				
			bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
			(i%3 == 0) ? FLinearColor::Red : FLinearColor::Green
		);
	}

	FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(2* BezierPointRadius, FSlateLayoutTransform(CalScreenPoints(AllottedGeometry,InputPoint)- BezierPointRadius)),
			&WhiteBox,				
			bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
			FLinearColor::Black
		);

	++LayerId;

	//至少4个节点才能绘制Bezier曲线，每增加3个点就添加一条曲线(第四个控制点与下一条曲线的第一条控制点是共用的)
	for(int i =0; i < BezierPoints.Num()-3;i+=3)
	{
		FSlateDrawElement::MakeCubicBezierSpline(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			Coordinates[i], Coordinates[i+1], Coordinates[i+2], Coordinates[i+3],
			SplineThickness.Get() * AllottedGeometry.Scale,
			bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
			FColor::White
		);
	}

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void STestCompoundWidget::CalPaintPoint(const FGeometry& AllottedGeometry, TArray<FVector2D>& Coordinates) const
{
	//坐标归一化
	for (int i = 0; i < BezierPoints.Num() && i < Coordinates.Num(); ++i)
	{
		Coordinates[i] = CalScreenPoints(AllottedGeometry,BezierPoints[i]);
	}
}

FVector2D STestCompoundWidget::CalBezierPoints(const FGeometry& MyGeometry, FVector2D InSoftwareCursorPosition) const
{
	FVector2D BezierPoint;
	switch (CoordinateType)
	{
	case ECoordinateType::Coordinate:
		BezierPoint = InSoftwareCursorPosition;
		break;
	case ECoordinateType::UV_0:
		BezierPoint = InSoftwareCursorPosition/MyGeometry.Size;
		break;
	case ECoordinateType::UV_1:
		BezierPoint = (InSoftwareCursorPosition/MyGeometry.Size - FVector2D(0.5,0.5)) * 2;
		break;
	case ECoordinateType::UV_2:
		BezierPoint = InSoftwareCursorPosition/MyGeometry.Size - FVector2D(0.5,0.5);
		break;
	default: ;
	}
	
	return BezierPoint;
}

FVector2D STestCompoundWidget::CalScreenPoints(const FGeometry& MyGeometry, FVector2D InUV) const
{
	FVector2D ScreenPoint;
	switch (CoordinateType)
	{
	case ECoordinateType::Coordinate:
		ScreenPoint = InUV;
		break;
	case ECoordinateType::UV_0:
		ScreenPoint = InUV * MyGeometry.Size;
		break;
	case ECoordinateType::UV_1:
		ScreenPoint = (InUV * 0.5 + FVector2D(0.5,0.5)) * MyGeometry.Size;
		break;
	case ECoordinateType::UV_2:
		ScreenPoint = (InUV + FVector2D(0.5,0.5)) * MyGeometry.Size;
		break;
	default: ;
	}
	return ScreenPoint;
}

void STestCompoundWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if(AutoMove)
	{
		MoveAllPoint(MoveDistance * InDeltaTime);
	
		AddPointTime -= InDeltaTime;
		if(AddPointTime <= 0)
		{
			AutoAddPoint();
			AddPointTime += FMath::FRand() + 0.5;
		}
	}
}

void STestCompoundWidget::MoveAllPoint(FVector2D InMoveDistance)
{
	for (auto& BezierPoint : BezierPoints)
	{
		BezierPoint += InMoveDistance;
	}

	LastInputPoint += InMoveDistance;
	InputPoint += InMoveDistance;
}

void STestCompoundWidget::AutoAddPoint()
{
	float NewPointX = 0.2 + InputPoint.X;
	float NewPointY = (FMath::FRand() - 0.5) * 0.2 + InputPoint.Y;
	NewPointX = FMath::Clamp<float>(NewPointX,-1,1);
	NewPointY = FMath::Clamp<float>(NewPointY,-1,1);
	AddPoint(FVector2D(NewPointX,NewPointY));
}

FReply STestCompoundWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FVector2D SoftwareCursorPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		
		NearPoint = -1;
		NearOffset = FVector2D(0);
		for (int i = 0; i < BezierPoints.Num(); ++i)
		{
			FVector2D PointOffset = CalScreenPoints(MyGeometry,BezierPoints[i]) - SoftwareCursorPosition;
			
			FVector2D Distance = PointOffset.GetAbs();

			// * 1.5是为了让点击热区更大，更好操作
			if(Distance.X < BezierPointRadius.X * 1.5 && Distance.Y < BezierPointRadius.Y * 1.5)
			{
				//UE_LOG(LogTemp,Warning,TEXT("BezierPoints %d  Distance %s  MouseUV %s"),i,*Distance.ToString(), *PointOffset.ToString());
				NearOffset = PointOffset;
				NearPoint = i;
				break;
			}
		}
		if(NearPoint == -1)
		{
			FVector2D NewPoint = CalBezierPoints(MyGeometry,SoftwareCursorPosition);
			AddPoint(NewPoint);
		}
		return FReply::Handled();
	}
	else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		RemoveLastPoint();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply STestCompoundWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseMove(MyGeometry, MouseEvent);
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if(NearPoint >= 0 && NearPoint < BezierPoints.Num())
		{
			FVector2D SoftwareCursorPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
			FVector2D NewPoint = CalBezierPoints(MyGeometry,SoftwareCursorPosition + NearOffset);
			FVector2D PointMove = NewPoint - BezierPoints[NearPoint];

			//希望1、4控制点的移动影响2、3控制点。
			//例如：控制点4向左移动时，希望控制点3、5跟着向左移动
			if(NearPoint % 3 == 0)
			{
				if(NearPoint >= 1)
				{
					BezierPoints[NearPoint - 1] += PointMove;
				}
				if(NearPoint + 1 < BezierPoints.Num())
				{
					BezierPoints[NearPoint + 1] += PointMove;
				}
			}
			BezierPoints[NearPoint] = NewPoint;
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}


void STestCompoundWidget::AddPoint(FVector2D NewPoint)
{
	//思路为：根据1、4、7控制点，自动插入3、5控制点
	//实际上输入第7控制点时，插入3、4、5控制点
	
	if(BezierPoints.Num() == 0)
	{
		InputPoint = NewPoint;
		BezierPoints.Add(InputPoint);
		return;
	}
	
	if(BezierPoints.Num() == 1)
	{
		BezierPoints.Add(InputPoint + (NewPoint - InputPoint) / 4);
	}
	else
	{
		FVector2D Direction = (NewPoint-LastInputPoint).GetSafeNormal();
		BezierPoints.Add(InputPoint - Direction * (InputPoint - LastInputPoint).Size() / 4);
		BezierPoints.Add(InputPoint);
		BezierPoints.Add(InputPoint + Direction * (NewPoint - InputPoint).Size() / 4);
	}

	//UE_LOG(LogTemp,Warning,TEXT("Last %s  Now %s  Next %s"),*(InputPoint - Direction).ToString(),*InputPoint.ToString(), *(InputPoint + Direction).ToString());
	LastInputPoint = InputPoint;
	InputPoint = NewPoint;

	if(AutoMove)
	{
		//优化内存
		if(BezierPoints[0].X < -2)
		{
			int BeginPoint = -1;
			for(int i=0;i<BezierPoints.Num()-3;i+=3)
			{
				if(BezierPoints[i + 3].X >= -1)
				{
					BeginPoint = i;
					break;
				}
			}
			//移除前一部分的元素
			if(BeginPoint >= 0)
			{
				TArray<FVector2D> NewBezierPoints;
				NewBezierPoints.Append(&BezierPoints[BeginPoint],BezierPoints.Num() - BeginPoint);
				BezierPoints = NewBezierPoints;
			}
		}
	}
}

void STestCompoundWidget::RemoveLastPoint()
{
	if(BezierPoints.Num() == 0)
	{
		return;
	}
	if(BezierPoints.Num() == 1)
	{
		BezierPoints.Pop(false);
		return;
	}
	
	if(BezierPoints.Num() == 2)
	{
		BezierPoints.Pop(false);
	}
	else
	{
		BezierPoints.Pop(false);
		BezierPoints.Pop(false);
		BezierPoints.Pop(false);
	}
	
	InputPoint = LastInputPoint;
    if(BezierPoints.Num() > 0)
    {
    	LastInputPoint = BezierPoints.Last();
    }
}