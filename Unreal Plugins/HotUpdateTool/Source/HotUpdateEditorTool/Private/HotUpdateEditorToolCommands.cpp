// Copyright Epic Games, Inc. All Rights Reserved.

#include "HotUpdateEditorToolCommands.h"

#define LOCTEXT_NAMESPACE "FHotUpdateEditorToolModule"

void FHotUpdateEditorToolCommands::RegisterCommands()
{
    UI_COMMAND(OpenChunkReferenceWindow, "ChunkReference", "Bring up ChunkReference window", EUserInterfaceActionType::Button, FInputGesture());
    UI_COMMAND(OpenGenerateProjectWindow, "GenerateProject", "Bring up GenerateProject window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE