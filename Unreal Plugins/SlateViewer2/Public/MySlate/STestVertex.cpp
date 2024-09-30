#include "STestVertex.h"

void STestVertex::DrawCicle()
{
	//这里表示绘制圆形
	int PointNum = 12;
	for (int i = 0; i <= PointNum; ++i)
	{
		Points.Add(FVector2D(0));
		FVector2D OuterPoint;
		float Radius = 0.5;
		float Angle = (float)i / PointNum * 2 * PI;

		OuterPoint.X = Radius * FMath::Cos(Angle);
		OuterPoint.Y = Radius * FMath::Sin(Angle);
		Points.Add(OuterPoint);
	}

	//顶点的绘制顺序
	for (int i = 2; i < Points.Num(); ++i)
	{
		Indexes.Add(i-2);
		Indexes.Add(i-1);
		Indexes.Add(i);
	}
}

void STestVertex::DrawCircularRing()
{
	//这里表示绘制圆环
	int PointNum = 10;
	for (int i = 0; i <= PointNum; ++i)
	{
		float Angle = (float)i / PointNum * 2 * PI;

		float InsideRadius = 0.2;
		FVector2D InsidePoint;
		InsidePoint.X = InsideRadius * FMath::Sin(Angle);
		InsidePoint.Y = -InsideRadius * FMath::Cos(Angle);
		Points.Add(InsidePoint);
		
		float OuterRadius = 0.5;
		FVector2D OuterPoint;
		OuterPoint.X = OuterRadius * FMath::Sin(Angle);
		OuterPoint.Y = -OuterRadius * FMath::Cos(Angle);
		Points.Add(OuterPoint);
	}

	//顶点的绘制顺序
	for (int i = 2; i < Points.Num(); ++i)
	{
		Indexes.Add(i-2);
		Indexes.Add(i-1);
		Indexes.Add(i);
	}
}

int32 STestVertex::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                           FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
                           bool bParentEnabled) const
{
	const bool bEnabled = ShouldBeEnabled(bParentEnabled);

	const FSlateBrush* MyBrush = FCoreStyle::Get().GetBrush("ColorWheel.HueValueCircle");

	FSlateResourceHandle Handle = MyBrush->GetRenderingResource();

	//需要绘制的顶点
	TArray<FSlateVertex> vertexs;
	vertexs.SetNum(Points.Num());

	for (int i = 0; i < Points.Num(); ++i)
	{
		FVector2D Coordinate = (Points[i] * 0.5 + FVector2D(0.5,0.5)) * AllottedGeometry.Size.Y;
		vertexs[i].Position = AllottedGeometry.GetAccumulatedRenderTransform().TransformPoint(Coordinate);
	}
	
	//顶点的UV
	for (FSlateVertex& tempvertex:vertexs)
	{
		tempvertex.TexCoords[0] = 0.0f;
		tempvertex.TexCoords[1] = 0.0f;
		tempvertex.TexCoords[2] = 0.0f;
		tempvertex.TexCoords[3] = 0.0f;
		tempvertex.MaterialTexCoords = FVector2D(0.0f,0.0f);
		tempvertex.PixelSize[0] = 0.0f;
		tempvertex.PixelSize[1] = 0.0f;
		tempvertex.Color = FColor::White;
	}
	
	FSlateDrawElement::MakeCustomVerts(
		OutDrawElements,
		LayerId,
		Handle,
		vertexs,
		Indexes,
		nullptr, 
		0,
		0);
	
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                                bParentEnabled);
}
