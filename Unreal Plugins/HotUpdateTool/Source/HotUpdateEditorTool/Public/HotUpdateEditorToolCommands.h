// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HotUpdateEditorToolStyle.h"

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * UE编辑器插件按钮基本参数注册
 * 
 * 当前类无自定义内容，请参考UE自定义编辑器控件实现方法来理解代码格式
 */
class FHotUpdateEditorToolCommands : public TCommands<FHotUpdateEditorToolCommands>
{
public:
    /**
     * 定义编辑器窗口的名称
     */
    FHotUpdateEditorToolCommands()
        : TCommands<FHotUpdateEditorToolCommands>(TEXT("HotUpdateEditorTool"), NSLOCTEXT("Contexts", "HotUpdateEditorTool", "HotUpdateEditorTool Plugin"), NAME_None, FHotUpdateEditorToolStyle::GetStyleSetName())
    {
    }

    /**
     * 注册窗口名称与点击事件类型
     */
    virtual void RegisterCommands() override;

public:
    /**
     * 自定义窗口的基本信息
     */
    TSharedPtr<FUICommandInfo> OpenChunkReferenceWindow;
    TSharedPtr<FUICommandInfo> OpenGenerateProjectWindow;
};