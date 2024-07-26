// Fill out your copyright notice in the Description page of Project Settings.


#include "RoadSpawn.h"

TArray<FVector> URoadSpawn::GetRoadPoints(TArray<FVector> LinePoints, float Width)
{
	TArray<FVector> RoadPoints;
	
	//点的数量过少时无法生成道路
	int PointNum = LinePoints.Num();
	if(PointNum < 2)
	{
		return RoadPoints;
	}

	//计算起点位置道路左右扩展方向
	FVector BeginDirector = LinePoints[1]-LinePoints[0];
	FVector2D ExtendDirection2D(BeginDirector.Y,-BeginDirector.X);
	ExtendDirection2D.Normalize();
	ExtendDirection2D = ExtendDirection2D * Width;

	//额外添加终点，保证最后一段线正常生成
	LinePoints.Add(LinePoints[PointNum-1]*2 - LinePoints[PointNum-2]);

	//记录上一个拐角点，避免连续的极小拐角导致向量中点归一化后为0
	FVector LastPoint = LinePoints[0]*2 - LinePoints[1];
	for(int i=0;i<PointNum;i++)
	{
		FVector Left = LastPoint - LinePoints[i];
		FVector2D Left2D(Left.X,Left.Y);
		Left2D.Normalize();

		FVector Right = LinePoints[i+1] - LinePoints[i];
		FVector2D Right2D(Right.X,Right.Y);
		Right2D.Normalize();

		//计算角平分线，沿角平分线方向生成道路边缘点
		FVector2D AngularBisector = (Left2D + Right2D)/2.0;
		FVector2D AngularBisectorNormal = AngularBisector.GetSafeNormal();

		FVector2D NowExtendDirection2D;
		//如果角平分线长度接近0，则切线方向生成道路边缘点
		if(AngularBisectorNormal != FVector2D::ZeroVector)
		{
			LastPoint = LinePoints[i];
			FVector2D Angular2 = (Left2D - Right2D)/2.0;
			NowExtendDirection2D = AngularBisectorNormal * Width / Angular2.Size();

			bool NeedReverse = FVector2D::DotProduct(AngularBisector,ExtendDirection2D) < 0;

			//记录道路切线方向，若接下来道路夹角为180度时，沿切线方向生成道路边缘点
			ExtendDirection2D = NowExtendDirection2D - Right2D*Width / Angular2.Size() * AngularBisector.Size();

			//角平分线是小于180度角的平分线，当道路转弯角度为180-360度时，需要反转角平分线方向
			if(NeedReverse)
			{
				NowExtendDirection2D = -NowExtendDirection2D;
				ExtendDirection2D = -ExtendDirection2D;
			}
		}
		else
		{
			NowExtendDirection2D = ExtendDirection2D;
		}

		//添加道路边缘点
		RoadPoints.Add(LinePoints[i] + FVector(NowExtendDirection2D.X,NowExtendDirection2D.Y,0));
		RoadPoints.Add(LinePoints[i] - FVector(NowExtendDirection2D.X,NowExtendDirection2D.Y,0));
	}
	return RoadPoints;
}
