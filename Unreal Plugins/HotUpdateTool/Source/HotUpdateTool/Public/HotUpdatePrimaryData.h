// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryAssetLabel.h"
#include "UObject/Object.h"
#include "HotUpdatePrimaryData.generated.h"

//资产类型，这个属性并未强制使用，功能目前只类似注释信息，标识每个目录内的资产是什么类型
//在以后版本扩展时，可以检验导入的资产是否符合类型，增加目录结构的规范性
UENUM(BlueprintType)
enum class EAssetType : uint8
{
    None,
    BluePrint,
    WidgetBlueprint,

    Material,
    Texture,
    StaticMesh,
    SkeletalMesh,
    ParticleSystem,
    Animations,
    Sounds,
    Miscellaneous,

    Map
};

//文件目录内对应的详细资产信息，在编辑器模式下输入目录，自动刷新并填入目录下的资产
//DirectoryAssets与DirectoryBlueprints需要分开处理，更加符合UE通用格式
USTRUCT(BlueprintType)
struct FAssetDirectory
{
    GENERATED_BODY()

public:
    //资产类型，不强制指定
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EAssetType AssetType;

    //输入目录，自动填充目录下的资产
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FDirectoryPath DirectoryName;

    //当前目录下的普通资产
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FName, TSoftObjectPtr<UObject>> DirectoryAssets;

    //当前目录下的蓝图类资产
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TMap<FName, TSoftClassPtr<UObject>> DirectoryBlueprints;
};

/**
 * 热更新主资产数据类型，用于描述一个模块的资产信息，每个主资产以及对应的资源将作为一个单独的pak文件打包出来
 */
UCLASS()
class HOTUPDATETOOL_API UHotUpdatePrimaryData : public UPrimaryAssetLabel
{
    GENERATED_BODY()

public:
    /**
     * 构造函数，设置一些默认属性
     */
    UHotUpdatePrimaryData();

    /**
     * 注册资源，将刷新所有的目录对应资源文件
     */
    void Register();

    //-------主资产内部引用资源查找方法，通过名称查找资源将避免复杂的资源引用
    /**
     * 根据名称查找UObject资产软引用
     * @param Name 输出资产名称，对应主资产内自定义的文件
     * @return 找到的资产软引用
     */
    UFUNCTION(BlueprintCallable)
    TSoftObjectPtr<UObject> FindObjectFromName(FName Name);

    /**
     * 根据名称查找UClass资产软引用
     * @param Name 输出资产名称，对应主资产内自定义的文件
     * @return 找到的资产软引用
     */
    UFUNCTION(BlueprintCallable)
    TSoftClassPtr<UObject> FindClassFromName(FName Name);

    /**
     * 根据名称查找UClass资产软引用，并同步加载资产
     * @param Name 输出资产名称，对应主资产内自定义的文件
     * @return 找到并加载完成的资产指针
     */
    UFUNCTION(BlueprintCallable)
    UClass *LoadClassFromName(FName Name);

    //------主资产对应的具体数据
#if WITH_EDITOR
    /**
     * 响应蓝图中对属性的编辑事件
     * @param PropertyChangedEvent 蓝图输入的编辑事件
     */
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent &PropertyChangedEvent) override;
#endif

    //模块名称，每个模块名称应该唯一
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit", ToolTip = "模块名称"))
    FName ModuleName;

    // 父模块会管理子模块，所以需要引用子模块
    UPROPERTY(EditAnywhere, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit", ToolTip = "子模块资产"))
    TMap<FName, FPrimaryAssetId> SubModuleAssets;

protected:
    // 刷新目录下的文件资产，仅编辑器模式下生效
    UPROPERTY(EditAnywhere, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit", ToolTip = "刷新后，自动包含的资产将根据目录名重新刷新，手动修改将会丢失"))
    bool bForceRefresh = false;

    // 通过目录包含的次级资产，注意资产名称不能重复
    UPROPERTY(EditAnywhere, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit", ToolTip = "用于自动包含资产的目录"))
    TArray<FAssetDirectory> Directorys;

    // 单独包含的次级资产，注意需要手动指定名称
    UPROPERTY(EditAnywhere, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit"))
    TMap<FName, TSoftObjectPtr<UObject>> OtherAssets;

    // 单独包含的次级资产，注意需要手动指定名称
    UPROPERTY(EditAnywhere, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit", BlueprintBaseOnly))
    TMap<FName, TSoftClassPtr<UObject>> OtherBlueprints;

private:
    // 最终的数据保留在基类中，这里只根据资产名称查找资产索引
    TMap<FName, int> ModuleAssetIndex;
    TMap<FName, int> ModuleBlueprintIndex;
};