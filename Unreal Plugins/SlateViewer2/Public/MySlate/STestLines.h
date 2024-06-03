#pragma once

class STestLines : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STestLines){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs){}

protected:
	//~ SWidget
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};
