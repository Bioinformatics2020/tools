// Copyright Epic Games, Inc. All Rights Reserved.

#include "HotUpdateEditorTool.h"
#include "HotUpdateEditorToolCommands.h"
#include "HotUpdateEditorToolStyle.h"
#include "ChunkReference/SChunkReference.h"
#include "GeneratePackage/SGeneratePackage.h"

#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FHotUpdateEditorToolModule"

static const FName ChunkReferenceTabName("ChunkReference");
static const FName GenerateProjectTabName("GenerateProject");

void FHotUpdateEditorToolModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    // 加载插件所需的纹理资产
    FHotUpdateEditorToolStyle::Initialize();
    FHotUpdateEditorToolStyle::ReloadTextures();

    //绑定菜单栏按钮UI命令
    RegisterCommands();

    //向工具栏注册热更新工具点击事件
    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FHotUpdateEditorToolModule::RegisterMenus));

    //注册ChunkReference窗口
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ChunkReferenceTabName,
                                                      FOnSpawnTab::CreateRaw(
                                                          this, &FHotUpdateEditorToolModule::OnSpawnChunkReferenceTab))
                            .SetDisplayName(LOCTEXT("ChunkReferenceTabTitle", "ChunkReference"))
                            .SetMenuType(ETabSpawnerMenuType::Hidden);

    //注册GenerateProject窗口
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(GenerateProjectTabName,
                                                      FOnSpawnTab::CreateRaw(
                                                          this, &FHotUpdateEditorToolModule::OnSpawnGenerateProjectTab))
                            .SetDisplayName(LOCTEXT("GenerateProjectTabTitle", "GenerateProject"))
                            .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FHotUpdateEditorToolModule::ShutdownModule()
{
    //参考资源加载过程，逐步卸载资源
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);

    FHotUpdateEditorToolStyle::Shutdown();
    FHotUpdateEditorToolCommands::Unregister();

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ChunkReferenceTabName);
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(GenerateProjectTabName);
}

TSharedRef<SDockTab> FHotUpdateEditorToolModule::OnSpawnChunkReferenceTab(const FSpawnTabArgs &SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SChunkReference)
        ];
}

void FHotUpdateEditorToolModule::ChunkReferenceButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(ChunkReferenceTabName);
}

TSharedRef<SDockTab> FHotUpdateEditorToolModule::OnSpawnGenerateProjectTab(const FSpawnTabArgs &SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SGeneratePackage)
        ];
}

void FHotUpdateEditorToolModule::GenerateProjectButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(GenerateProjectTabName);
}

void FHotUpdateEditorToolModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    //注册菜单内容
    {
        UToolMenu *Menu = UToolMenus::Get()->RegisterMenu("LevelEditor.LevelEditorToolBar.HotUpdate");
        Menu->bShouldCloseWindowAfterMenuSelection = true;
        FToolMenuSection &Section = Menu->AddSection("HotUpdateEditorTool",
                                                     LOCTEXT("HotUpdateEditorTool_NewHeading", "HotUpdate"));

        Section.AddMenuEntry(FHotUpdateEditorToolCommands::Get().OpenChunkReferenceWindow);
        Section.AddMenuEntry(FHotUpdateEditorToolCommands::Get().OpenGenerateProjectWindow);
    }

    //动态创建菜单详情
    auto GenerateMenuContent = [this]() -> TSharedRef<SWidget>
    {
        FToolMenuContext MenuContext(PluginCommands);
        MenuContext.AddObject(UToolMenus::Get());

        return UToolMenus::Get()->GenerateWidget("LevelEditor.LevelEditorToolBar.HotUpdate", MenuContext);
    };

    //添加工具栏扩展按钮
    UToolMenu *ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
    {
        //将按钮添加到Misc类别
        FToolMenuSection &Section = ToolbarMenu->FindOrAddSection("Misc");
        {
            Section.AddEntry(FToolMenuEntry::InitComboButton(
                "HotUpdate",
                FUIAction(),
                FOnGetContent::CreateLambda(GenerateMenuContent),
                LOCTEXT("HotUpdateEditorTool", "HotUpdate"),
                LOCTEXT("HotUpdateEditorTool_ToolTip", "HotUpdateEditorTool"),
                FSlateIcon(FHotUpdateEditorToolStyle::GetStyleSetName(), "HotUpdateEditorTool.OpenPluginWindow"),
                false,
                "HotUpdateEditorTool"
                ));
        }
    }
}

void FHotUpdateEditorToolModule::RegisterCommands()
{
    FHotUpdateEditorToolCommands::Register();
    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FHotUpdateEditorToolCommands::Get().OpenChunkReferenceWindow,
        FExecuteAction::CreateRaw(this, &FHotUpdateEditorToolModule::ChunkReferenceButtonClicked));

    PluginCommands->MapAction(
        FHotUpdateEditorToolCommands::Get().OpenGenerateProjectWindow,
        FExecuteAction::CreateRaw(this, &FHotUpdateEditorToolModule::GenerateProjectButtonClicked));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHotUpdateEditorToolModule, HotUpdateEditorTool)