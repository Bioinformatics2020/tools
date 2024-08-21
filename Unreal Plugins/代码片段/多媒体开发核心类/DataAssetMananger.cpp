
#include "DataAssetMananger.h"

#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

ADataAssetMananger* DataAssetManangerInst = nullptr;

void ADataAssetMananger::BeginPlay()
{
	//为了保证编辑器模式下使用，需要手动管理单例，这里可以替换为Subsystem
	DataAssetManangerInst = this;
	//定时清空一下缓冲池
	GetWorld()->GetTimerManager().SetTimer(ClearPoolHandle,this, &ADataAssetMananger::ClearPool, DeleteTime, true,DeleteTime);
}

void ADataAssetMananger::Destroyed()
{
	Super::Destroyed();
	//为了保证编辑器模式下使用，需要手动管理单例
	if (DataAssetManangerInst) {
		DataAssetManangerInst->DataAssets.Empty();
		DataAssetManangerInst = nullptr;
	}
}

ADataAssetMananger * ADataAssetMananger::GetInst(const UObject* Obj)
{
	if (DataAssetManangerInst == nullptr)
	{
		TArray<AActor*> OutActorArray;
		UGameplayStatics::GetAllActorsOfClass(Obj, ADataAssetMananger::StaticClass(), OutActorArray);
		checkf(OutActorArray.Num() == 1, TEXT("ADataAssetMananger Obj Only One In Scene, now is %d"),OutActorArray.Num());
		DataAssetManangerInst = Cast<ADataAssetMananger>(OutActorArray[0]);
	}
	check(DataAssetManangerInst);
	return DataAssetManangerInst;
}

FSoftObjectPath ADataAssetMananger::GetSoftObjectPathByName(const UObject* Obj, const FName AssetName)
{
	FSoftObjectPath SoftObjectPath;
	ADataAssetMananger* Inst = GetInst(Obj);
	check(Inst->DataAssets.Num() != 0);
	for (UMediaDataAsset* DataAsset : Inst->DataAssets)
	{
		check(DataAsset);
		if (DataAsset)
		{
			check(DataAsset->SoftObjectPtrs.Num() != 0);
			if (DataAsset->SoftObjectPtrs.Contains(AssetName))
			{
				SoftObjectPath = (DataAsset->SoftObjectPtrs[AssetName].ToSoftObjectPath());
				break;
			}
		}
	}

	checkf(SoftObjectPath.IsValid(),TEXT("Cant Find ObjectAsset : %s"), *(AssetName.ToString()));
	return SoftObjectPath;
}

TArray<FSoftObjectPath> ADataAssetMananger::GetSoftObjectPathsByNames(UObject * Obj, TArray<FName> AssetNames)
{
	TArray<FSoftObjectPath> SoftObjectPaths;
	for (const FName AssetName: AssetNames)
	{
		SoftObjectPaths.Add(GetSoftObjectPathByName(Obj,AssetName));
	}
	return SoftObjectPaths;
}

FSoftObjectPath  ADataAssetMananger::GetSoftClassPathByName(UObject * Obj, FName AssetName)
{
	FSoftObjectPath SoftObjectPath ;

	ADataAssetMananger* Inst = GetInst(Obj);
	check(Inst->DataAssets.Num() != 0);
	for (UMediaDataAsset* DataAsset : Inst->DataAssets)
	{
		check(DataAsset);
		if (DataAsset)
		{
			if (DataAsset->SoftClassPtrs.Contains(AssetName))
			{
				SoftObjectPath = (DataAsset->SoftClassPtrs[AssetName].ToSoftObjectPath());
				break;
			}
		}
	}
	checkf(SoftObjectPath.IsValid(), TEXT("Cant Find ClassAsset : %s"), *(AssetName.ToString()));
	return SoftObjectPath;
}

void ADataAssetMananger::RequestAsyncLoadObject(UObject * Obj, FName AssetName,TFunction<void(UObject* AssetObj)>&& Callback)
{
	FSoftObjectPath Path = GetSoftObjectPathByName(Obj,  AssetName);

	FAsyncLoad* AsyncLoad = new FAsyncLoad();
	
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	AsyncLoad->Handle = StreamableManager.RequestAsyncLoad(
		Path, 
		[AsyncLoad,Callback, Obj]()
		{
			check(AsyncLoad->Handle);
			UObject* AssetObj = AsyncLoad->Handle->GetLoadedAsset();
			check(AssetObj);

			Callback(AssetObj);
			delete 	AsyncLoad;
		}
	);
}

void ADataAssetMananger::RequestAsyncLoadObjects(UObject* Obj, TArray<FName> AssetNames, TFunction<void(TArray<UObject*> AssetObjs)>&& Callback)
{
	const TArray<FSoftObjectPath> Paths = GetSoftObjectPathsByNames(Obj, AssetNames);

	if (Paths.Num()!=0)
	{
		FAsyncLoad* AsyncLoad = new FAsyncLoad();
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		AsyncLoad->Handle = StreamableManager.RequestAsyncLoad(
			Paths,
			[AsyncLoad, Callback, Obj]()
			{
				check(AsyncLoad->Handle);
				TArray<UObject*> AssetObjs;
				AsyncLoad->Handle->GetLoadedAssets(AssetObjs);
				check(AssetObjs.Num()!=0);
				Callback(AssetObjs);
				delete 	AsyncLoad;
			}
		);
	}
}

void ADataAssetMananger::RequestAsyncLoadClass(UObject * Obj, FName AssetName, TFunction<void(UClass* ClassAsset)>&& Callback)
{
	const FSoftObjectPath  Path = GetSoftClassPathByName(Obj, AssetName);
	
	FAsyncLoad* AsyncLoad = new FAsyncLoad();
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	AsyncLoad->Handle = StreamableManager.RequestAsyncLoad(
		Path,
		[AsyncLoad, Callback, Obj]()
		{
			//UKismetSystemLibrary::PrintString(Obj, TEXT(__FUNCTION__), true, false, FColor::Green, 100);
			check(AsyncLoad->Handle);
			UClass* AssetClass = Cast<UClass>( AsyncLoad->Handle->GetLoadedAsset());
			check(AssetClass);

			Callback(AssetClass);
			delete 	AsyncLoad;
		}
	);
}

UClass * ADataAssetMananger::RequestSyncLoadClass(UObject * Obj, FName AssetName)
{
	const FSoftObjectPath Path = GetSoftClassPathByName(Obj, AssetName);
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	return Cast<UClass>( StreamableManager.RequestSyncLoad(Path)->GetLoadedAsset());
}

AActor* ADataAssetMananger::SyncLoadActorSingleton(UObject* Obj, FName AssetName, const FTransform& UserTransformPtr)
{
	// ADataAssetMananger* Inst = GetInst(Obj);
	AActor* ActorPtr = nullptr;
	// //若对象未创建过则直接创建一个
	// TWeakObjectPtr<UObject>* WeakObject = Inst->SingletonObjectMap.Find(AssetName);
	// if(!WeakObject)
	// {
	// 	UClass* ActorClass = RequestSyncLoadClass(Obj, AssetName);
	// 	ActorPtr = Inst->GetWorld()->SpawnActor(ActorClass,&UserTransformPtr);
	// 	checkf(ActorPtr,TEXT("ADataAssetMananger::SyncLoadActorSingleton %s is not Actor"), *AssetName.ToString());
	// 	Inst->SingletonObjectMap.Add(AssetName,ActorPtr);
	// 	return ActorPtr;
	// }
	//
	// //若对象已被销毁则重新创建
	// if(!WeakObject->IsValid())
	// {
	// 	UClass* ActorClass = RequestSyncLoadClass(Obj, AssetName);
	// 	ActorPtr = Inst->GetWorld()->SpawnActor(ActorClass,&UserTransformPtr);
	// 	checkf(ActorPtr,TEXT("ADataAssetMananger::SyncLoadActorSingleton %s is not Actor"), *AssetName.ToString());
	// 	*WeakObject = ActorPtr;
	// 	return ActorPtr;
	// }
	//
	// //若对象存在则返回保存的对象
	// ActorPtr = Cast<AActor>(WeakObject->Get());
	// checkf(ActorPtr,TEXT("ADataAssetMananger::SyncLoadActorSingleton %s is not Actor"), *AssetName.ToString());
	return ActorPtr;
}

UObject* ADataAssetMananger::BP_GetObjectOfNameSync(UObject* Outer, FName AssetName)
{
	return GetObjectOfNameSync<UObject>(Outer,AssetName);
}

void ADataAssetMananger::DeleteObjectOfName(UObject* Outer, UObject* Object, FName AssetName)
{
	TArray<UObject*>& ObjectArray = GetInst(Outer)->GetObjectPool(AssetName);
	ObjectArray.Push(Object);
}

void ADataAssetMananger::DeleteObjectArrayOfName(UObject* Outer,const TArray<UObject*>& Objects, FName AssetName)
{
	TArray<UObject*>& ObjectArray = GetInst(Outer)->GetObjectPool(AssetName);
	ObjectArray.Append(Objects);
}

void ADataAssetMananger::ClearPool()
{
	//目前是每次清空一半缓冲池
	for(auto& Pool:ObjectPool)
	{
		if(Pool.Value.Objects.Num()>10)
		{
			Pool.Value.Objects.SetNum(Pool.Value.Objects.Num()/2);
		}
	}
}

TArray<UObject*>& ADataAssetMananger::GetObjectPool(FName AssetName)
{
	FDataAssetObjects* ArrayPtr = ObjectPool.Find(AssetName);
	if(ArrayPtr == nullptr)
	{
		ObjectPool.Add(AssetName,FDataAssetObjects());
		ArrayPtr = ObjectPool.Find(AssetName);
		check(ArrayPtr);
	}
	return ArrayPtr->Objects;
}