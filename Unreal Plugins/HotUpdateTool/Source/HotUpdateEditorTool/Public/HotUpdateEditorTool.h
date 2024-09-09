// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * 热更新编辑器模块，用于在编辑器中添加热更新工具菜单
 */
class FHotUpdateEditorToolModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /**
     * 向编辑器窗口添加热更新工具菜单
     */
    void RegisterMenus();

    /**
    * 绑定菜单栏按钮UI命令
    */
    void RegisterCommands();

    /**
     * Chunk引用查器页面开启按钮点击事件
     */
    void ChunkReferenceButtonClicked();
    /**
     * 创建ChunkReference窗口
     * @param SpawnTabArgs UE窗口创建时默认传入的属性
     * @return 新窗口的slate对象
     */
    TSharedRef<class SDockTab> OnSpawnChunkReferenceTab(const class FSpawnTabArgs &SpawnTabArgs);

    /**
     * 打包工具开启按钮点击事件
     */
    void GenerateProjectButtonClicked();
    /**
     * 创建ChunkReference窗口
     * @param SpawnTabArgs UE窗口创建时默认传入的属性
     * @return 新窗口的slate对象
     */
    TSharedRef<class SDockTab> OnSpawnGenerateProjectTab(const class FSpawnTabArgs &SpawnTabArgs);

private:
    /**
     * 菜单栏按钮对应的事件记录
     */
    TSharedPtr<class FUICommandList> PluginCommands;
};