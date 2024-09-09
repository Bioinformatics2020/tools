// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * UE编辑器插件按钮样式定义与纹理资源加载
 * 
 * 当前类无自定义内容，请参考UE自定义编辑器控件实现方法来理解代码格式
 */
class FHotUpdateEditorToolStyle
{
public:
    /**
     * 向slate style注册新的样式与资产
     */
    static void Initialize();

    /**
     * 取消注册自定义的style资产
     */
    static void Shutdown();

    /**
     * 重新加载纹理并显示 
     */
    static void ReloadTextures();

    /**
     * @return 返回style资源对象，通过ISlateStyle获取具体的资产
     */
    static const ISlateStyle &Get();

    /**
     * @return 返回style样式注册名称
     */
    static FName GetStyleSetName();

private:
    /**
     * @return 从slate style管理器中创建当前类对应的样式
     */
    static TSharedRef<class FSlateStyleSet> Create();

private:
    /**
     * 保存style样式，避免重复创建
     */
    static TSharedPtr<class FSlateStyleSet> StyleInstance;
};