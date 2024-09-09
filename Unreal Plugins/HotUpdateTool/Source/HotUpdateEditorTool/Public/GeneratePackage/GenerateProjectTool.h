#pragma once

/**
 * 封装了UE原始的打包功能，这里的函数均拷贝于4.27源码版本中打包部分代码，未修改打包的基本流程
 */
class FGenerateProjectTool
{
public:
    /**
     * 打包项目，拷贝自MainFrameAction的PackageProject函数
     * @param OutPath 打包输出路径
     * @param InPlatformInfoName 打包的平台类型 
     * @param Callback 打包完成之后的回调事件
     */
    static void PackageProject(const FString &OutPath, const FName InPlatformInfoName, TFunction<void(FString, double)> Callback);

    /**
     * 输出log，在PackageProject函数中被使用，但是为了简化流程，这部分代码实现相比MainFrameAction之中已经简化
     */
    static void AddMessageLog(const FText &Text, const FText &Detail, const FString &TutorialLink, const FString &DocumentationLink);

    /**
     * 获取支持打包的平台，参考FPackageProjectMenu::MakeMenu函数实现
     * @return 返回所有支持的平台
     */
    static TArray<FName> GetAllPlatfrom();
};