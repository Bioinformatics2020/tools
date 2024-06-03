#pragma once

class SGames101_P0 : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGames101_P0)
	{}
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs){}

protected:
	//~ SWidget
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

};

