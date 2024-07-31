// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HotUpdateEditorToolStyle.h"
#include "Framework/Commands/Commands.h"

class FHotUpdateEditorToolCommands : public TCommands<FHotUpdateEditorToolCommands>
{
public:

	FHotUpdateEditorToolCommands()
		: TCommands<FHotUpdateEditorToolCommands>(TEXT("HotUpdateEditorTool"), NSLOCTEXT("Contexts", "HotUpdateEditorTool", "HotUpdateEditorTool Plugin"), NAME_None, FHotUpdateEditorToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenChunkReferenceWindow;
	TSharedPtr< FUICommandInfo > OpenGenerateProjectWindow;
};