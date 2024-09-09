// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

//定义热更新运行时模块日志标签
DECLARE_LOG_CATEGORY_EXTERN(LogHotUpdateTool, Log, All);

class FHotUpdateToolModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};