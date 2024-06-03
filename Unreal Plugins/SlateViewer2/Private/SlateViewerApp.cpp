// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateViewerApp.h"
#include "SlateViewer.h"

#include "RequiredProgramMainCPPInclude.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Application/SlateApplication.h"
#include "AppFramework/Private/Framework/Testing/TestStyle.h"

#include "MySlate/STestCompoundWidget.h"
#include "MySlate/STestVertex.h"
#include "MySlate/SGames101_P0.h"
#include "MySlate/STestLines.h"
#include "ReadWriteFile/FReadWriteFileTest.h"


IMPLEMENT_APPLICATION(SlateViewer, "SlateViewer");

#define LOCTEXT_NAMESPACE "SlateViewer"

namespace WorkspaceMenu
{
	TSharedRef<FWorkspaceItem> DeveloperMenu = FWorkspaceItem::NewGroup(LOCTEXT("DeveloperMenu", "Developer"));
}


int RunSlateViewer( const TCHAR* CommandLine )
{
	// start up the main loop
	GEngineLoop.PreInit(CommandLine);

	// Make sure all UObject classes are registered and default properties have been initialized
	ProcessNewlyLoadedUObjects();
	
	// Tell the module manager it may now process newly-loaded UObjects when new C++ modules are loaded
	FModuleManager::Get().StartProcessingNewlyLoadedObjects();

	// crank up a normal Slate application using the platform's standalone renderer
	FSlateApplication::InitializeAsStandaloneApplication(GetStandardStandaloneRenderer());
	
	// Bring up the test suite.
	MyRestoreSlateTestSuite();
	
	//测试文件读写
	//FReadWriteFileTest::BeginTest();

	// loop while the server does the rest
	while (!IsEngineExitRequested())
	{
		BeginExitIfRequested();

		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
		FStats::AdvanceFrame(false);
		FTicker::GetCoreTicker().Tick(FApp::GetDeltaTime());
		FSlateApplication::Get().PumpMessages();
		FSlateApplication::Get().Tick();		
		FPlatformProcess::Sleep(0);
	}

	FCoreDelegates::OnExit.Broadcast();
	FSlateApplication::Shutdown();
	FModuleManager::Get().UnloadModulesAtShutdown();


	return 0;
}

void MyRestoreSlateTestSuite()
{
	FTestStyle::ResetToDefault();

	FGlobalTabmanager::Get()->RegisterTabSpawner("MySDockTab1", FOnSpawnTab::CreateStatic(&MySDockTab1));
	FGlobalTabmanager::Get()->RegisterTabSpawner("MySDockTab2", FOnSpawnTab::CreateStatic(&MySDockTab2));
	FGlobalTabmanager::Get()->RegisterTabSpawner("MySDockTab3", FOnSpawnTab::CreateStatic(&MySDockTab3));

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout( "SlateTestSuite_Layout" )
	->AddArea
	(
		FTabManager::NewArea(800, 600)
		->SetWindow( FVector2D(420, 10), false )
		->Split
		(
			FTabManager::NewStack()
			->AddTab("MySDockTab1", ETabState::OpenedTab)
			->AddTab( "MySDockTab2", ETabState::OpenedTab )
			->AddTab( "MySDockTab3", ETabState::InvalidTab )
		)		
	)
	;

	FGlobalTabmanager::Get()->RestoreFrom( Layout, TSharedPtr<SWindow>() );
}

TSharedRef<SDockTab> MySDockTab1( const FSpawnTabArgs& Args )
{	
	TSharedRef<SDockTab> RenderTransformManipulatorTab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(FText::FromString(TEXT("STestVertex")));

	RenderTransformManipulatorTab->SetContent
	(
	SNew(STestVertex)
		.TestDrawShape(ETestDrawShape::Cicle)
	);

	return RenderTransformManipulatorTab;
}

TSharedRef<SDockTab> MySDockTab2( const FSpawnTabArgs& Args )
{	
	TSharedRef<SDockTab> RenderTransformManipulatorTab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(FText::FromString(TEXT("STestCompoundWidget")));

	RenderTransformManipulatorTab->SetContent
	(
	SNew(STestCompoundWidget)
		.SplineThickness(2.0)
		.CoordinateType(ECoordinateType::UV_1)
		.AutoMove(false)
	);

	return RenderTransformManipulatorTab;
}

TSharedRef<SDockTab> MySDockTab3( const FSpawnTabArgs& Args )
{	
	TSharedRef<SDockTab> RenderTransformManipulatorTab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(FText::FromString(TEXT("STestLines")));

	RenderTransformManipulatorTab->SetContent
	(
	SNew(STestLines)
	);

	return RenderTransformManipulatorTab;
}

#undef LOCTEXT_NAMESPACE
