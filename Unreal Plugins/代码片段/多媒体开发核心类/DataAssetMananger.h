#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "GameFramework/Actor.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"

#include "Components/Widget.h"
#include "Components/WrapBox.h"

#include "DataAssetMananger.generated.h"

UCLASS()
class  UMediaDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, TSoftObjectPtr<UObject>> SoftObjectPtrs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FName, TSoftClassPtr<UObject>> SoftClassPtrs;
};



class FAsyncLoad {
public:
	TSharedPtr<FStreamableHandle> Handle;
};

USTRUCT()
struct FDataAssetObjects
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UObject*> Objects;
	
};

UCLASS()
class  ADataAssetMananger : public AActor
{
	GENERATED_BODY()

#pragma region "AssetLoadBase"
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMediaDataAsset*> DataAssets;
private:
	virtual void BeginPlay()override;
	virtual void Destroyed()override;

private:
	static ADataAssetMananger* GetInst(const UObject* Obj);
private:
	static FSoftObjectPath GetSoftObjectPathByName(const UObject* Obj, const FName AssetName);
	static TArray<FSoftObjectPath> GetSoftObjectPathsByNames(UObject* Obj, TArray<FName> AssetNames);
	static FSoftObjectPath GetSoftClassPathByName(UObject* Obj, FName AssetName);
public:

	//异步加载一个对象(资产)  LoadObject
	static void RequestAsyncLoadObject(UObject* Obj, FName AssetName, TFunction<void(UObject* AssetObj)>&& Callback);
	//异步加载多个对象(资产)  LoadObjects
	static void RequestAsyncLoadObjects(UObject* Obj, TArray<FName> AssetNames, TFunction<void(TArray<UObject*> AssetObjs)>&& Callback);

	//异步加载一个类型(资产)  
	static void RequestAsyncLoadClass(UObject* Obj, FName AssetName, TFunction<void(UClass* ClassAsset)>&& Callback);
	//同步加载一个类型(资产) LoadClass
	UFUNCTION(BlueprintCallable)
	static UClass* RequestSyncLoadClass(UObject* Obj, FName AssetName);
	
	//加载一个AActor单例
	UFUNCTION(BlueprintCallable)
	static AActor* SyncLoadActorSingleton(UObject* Obj, FName AssetName, const FTransform& UserTransformPtr);
	
private:

	TMap<FName,TWeakObjectPtr<UObject>> SingletonObjectMap;
#pragma endregion

#pragma region "ObjectPool"
public:
	//同步加载Class，根据资产名称，使用对象池的方式，返回对应的UObject，要求该对象使用DeleteObjectOfName进行释放
	template<typename DataType>
	static DataType* GetObjectOfNameSync(UObject* Outer,FName AssetName);

	UFUNCTION(BlueprintCallable)
	static UObject* BP_GetObjectOfNameSync(UObject* Outer, FName AssetName);


	//释放资源到对象池
	//传入对象指针与资产名称
	static void DeleteObjectOfName(UObject* Outer, UObject* Object,FName AssetName);


	//释放资源到对象池
	//传入对象指针数组与资产名称
	UFUNCTION(BlueprintCallable)
	static void DeleteObjectArrayOfName(UObject* Outer,const TArray<UObject*>& Objects,FName AssetName);
private:
	//对象池每60秒清理一次
	float DeleteTime = 60;
	//定时清空对象池
	FTimerHandle ClearPoolHandle;
	void ClearPool();
	
	//存储所有的对象
	UPROPERTY()
	TMap<FName,FDataAssetObjects> ObjectPool;

	//获取到对象池
	TArray<UObject*>& GetObjectPool(FName AssetName);
#pragma endregion

#pragma region "UIChild_Manager"
public:
	//维护一个UWrapBox的元素，实现自动添加或销毁子项，保证UWrapBox子项的数量满足要求
	//传入 对应的WrapBox，子项资产名称，目标数量，初始化子项时的回调，移除子项时的回调
	template<typename DataType>
	static void UpdateWrapBoxFromChildName(UWrapBox* WrapBox,FName ChileAssertName,int TargetNum,
		TFunction<void(DataType* AssetObj)>&& NewChild,
		TFunction<void(DataType* AssetObj)>&& DeleteChild);

	//维护一个UListView的元素，实现自动添加或销毁子项，保证UListView子项的数量满足要求
	//传入 对应的UListView，子项资产名称，目标数量，初始化子项时的回调，移除子项时的回调
	template<typename ListViewType, typename DataType>
	static void UpdateListViewFromChildName(ListViewType* ListView,FName ChileAssertName,int TargetNum,
		TFunction<void(DataType* AssetObj)>&& NewChild,
		TFunction<void(DataType* AssetObj)>&& DeleteChild);
#pragma endregion
};

//加载UObject,并将其保存在一个map中,实现简单的对象池
template <typename DataType>
DataType* ADataAssetMananger::GetObjectOfNameSync(UObject* Outer, FName AssetName)
{
	TArray<UObject*>& ObjectArray = GetInst(Outer)->GetObjectPool(AssetName);
	
	//返回对象
	UObject* Object = nullptr;
	if(ObjectArray.Num() == 0)
	{
		const UClass* PageClass = ADataAssetMananger::RequestSyncLoadClass(Outer, AssetName);
		checkf(PageClass,TEXT("AssetName is error name"));
		Object = NewObject<DataType>(Outer, PageClass);
		checkf(Object,TEXT("Asset type error"));
		//if(GEngine != nullptr)
		    //GEngine->AddOnScreenDebugMessage(-1,5,FColor::Yellow,TEXT("Creat Widget"));
		UE_LOG(LogTemp,Warning,TEXT("Creat Widget"));
	}
	else
	{
		Object = ObjectArray.Pop();
		//if(GEngine != nullptr)
			//GEngine->AddOnScreenDebugMessage(-1,5,FColor::Yellow,TEXT("Get Widget"));
		UE_LOG(LogTemp,Warning,TEXT("Get Widget"));
	}
	return Cast<DataType>(Object);
}

template<typename DataType>
void ADataAssetMananger::UpdateWrapBoxFromChildName(UWrapBox* WrapBox, FName ChileAssertName, int TargetNum,
	TFunction<void(DataType* AssetObj)>&& NewChild,
	TFunction<void(DataType* AssetObj)>&& DeleteChild)
{
	TArray<UWidget*> ChildWidgets = WrapBox->GetAllChildren();
	bool ChildBiger = ChildWidgets.Num() > TargetNum;
	int min = ChildBiger ? TargetNum : ChildWidgets.Num();

	//保证ChildWidgets.Num() == TargetNum，多余的控件释放，缺少的控件补充
	if(ChildBiger)
	{
		for(int i = ChildWidgets.Num() - 1;i>=min;i--)
		{
			DataType* TrackItem = Cast<DataType>(ChildWidgets[i]);
			ADataAssetMananger::DeleteObjectOfName(WrapBox,TrackItem,ChileAssertName);
			WrapBox->RemoveChildAt(i);
			DeleteChild(TrackItem);
		}
	}
	else
	{
		for(int i=min;i<TargetNum;i++)
		{
			DataType* TrackItem = ADataAssetMananger::GetObjectOfNameSync<DataType>(WrapBox,ChileAssertName);
			checkf(TrackItem,TEXT("TrackItem type is error"));
			WrapBox->AddChild(TrackItem);
			NewChild(TrackItem);
		}
	}
}

template<typename ListViewType, typename DataType>
void ADataAssetMananger::UpdateListViewFromChildName(ListViewType* ListView, FName ChileAssertName, int TargetNum,
	TFunction<void(DataType* AssetObj)>&& NewChild,
	TFunction<void(DataType* AssetObj)>&& DeleteChild)
{
	
	TArray<UObject*> Items = ListView->GetListItems();
	bool ChildBiger = Items.Num() > TargetNum;
	int min = ChildBiger ? TargetNum : Items.Num();

	//保证ChildWidgets.Num() == TargetNum，多余的控件释放，缺少的控件补充
	if(ChildBiger)
	{
		for(int i = Items.Num() - 1;i>=min;i--)
		{
			DataType* Item = Cast<DataType>(Items[i]);
			checkf(Item,TEXT("Item type is error"));
			ADataAssetMananger::DeleteObjectOfName(ListView,Item,ChileAssertName);
			ListView->RemoveItem(Item);
			DeleteChild(Item);
		}
	}
	else
	{
		for(int i=min;i<TargetNum;i++)
		{
			DataType* Item = ADataAssetMananger::GetObjectOfNameSync<DataType>(ListView,ChileAssertName);
			checkf(Item,TEXT("Item type is error"));
			ListView->AddItem(Item);
			NewChild(Item);
		}
	}
}