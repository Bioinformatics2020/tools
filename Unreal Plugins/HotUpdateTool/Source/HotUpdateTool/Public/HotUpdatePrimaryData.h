// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryAssetLabel.h"
#include "UObject/Object.h"
#include "HotUpdatePrimaryData.generated.h"



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

USTRUCT(BlueprintType)
struct FAssetDirectory
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAssetType AssetType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDirectoryPath DirectoryName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, TSoftObjectPtr<UObject>> DirectoryAssets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, TSoftClassPtr<UObject>> DirectoryBlueprints;
};

/**
 * 
 */
UCLASS()
class HOTUPDATETOOL_API UHotUpdatePrimaryData : public UPrimaryAssetLabel
{
	GENERATED_BODY()
public:
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FLoadSubModule,UHotUpdatePrimaryData*, AssetData);
	
	UHotUpdatePrimaryData();
	void Register();

	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UObject> FindObjectFromName(FName Name);
	
	UFUNCTION(BlueprintCallable)
	TSoftClassPtr<UObject> FindClassFromName(FName Name);

	UFUNCTION(BlueprintCallable)
	UClass* LoadClassFromName(FName Name);
#if WITH_EDITOR	
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit", ToolTip = "模块名称"))
	FName ModuleName;

	// 父模块会管理子模块，所以需要引用子模块
	UPROPERTY(EditAnywhere, Category = HotUpdatePrimaryData, meta = (AssetBundles = "Explicit", ToolTip = "子模块资产"))
	TMap<FName,FPrimaryAssetId> SubModuleAssets;

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
	// 根据资产名称查找资产索引
	TMap<FName,int> ModuleAssetIndex;
	TMap<FName,int> ModuleBlueprintIndex;
};
