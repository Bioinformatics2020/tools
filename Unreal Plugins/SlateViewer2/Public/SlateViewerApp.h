// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Widgets/Docking/SDockTab.h"


/**
 * Run the SlateViewer .
 */
int RunSlateViewer(const TCHAR* Commandline);

void MyRestoreSlateTestSuite();

TSharedRef<SDockTab> MySDockTab1( const FSpawnTabArgs& Args );
TSharedRef<SDockTab> MySDockTab2( const FSpawnTabArgs& Args );
TSharedRef<SDockTab> MySDockTab3( const FSpawnTabArgs& Args );
