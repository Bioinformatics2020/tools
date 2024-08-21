#pragma once

#include "CoreMinimal.h"
#include "MediaConfig.h"


//播放状态：背景高亮动效高亮 背景取消高亮动效取消高亮 背景高亮动效取消高亮，从playmededia进入播放，从playmededia进入暂停
enum class EPlayStatus
{
	Playing,
	Pause,
	Loading,
	EffectPause,
	EffectPlaying,
	EffectPlaymedia,
	EffectPlaymediaPause
};
class FUIPlayStatus;

//UI播放状态管理功能的管理器，支持播放 加载中 暂停
//是根据互斥事件管理器演化而来的专用版本
class FUIPlayStatusManager
{
public:
	/**
	 * @brief 以单例模式获取UI播放状态管理器
	 * @return 返回管理器实例
	 */
	static FUIPlayStatusManager* Get();
	//当前项目中，我在MediaCtl中销毁该类，不销毁会导致编辑器模式下第二次进入程序之后崩溃
	static void Delete();

	/**
	 * @brief 每当播放元素改变时需要调用这个函数，来对其管理的元素状态进行修改，将修改歌曲以及其对应的歌单
	 * @param Source 
	 * @param Album 
	 * @param Song 
	 * @param InStatus 
	 */
	void ChangeSong(const FString& Source, const FString& Album, const FString& Song, EPlayStatus InStatus);

	/**
	 * @brief 添加到管理器中
	 */
	void AddPlayer(FUIPlayStatus* InPlayer,const FString& Source,const FString& Album,const FString& Song="");
	
	/**
	 * @brief 从管理器中移除
	 */
	void DeletePlayer(FUIPlayStatus* InPlayer,FString Source,FString Album,FString Song="");

	/**
	 * @brief 获取当前播放元素的信息
	 * @param OutAlbumID		 Source + Album
	 * @param OutSongID			 Source + Album + Song
	 * @param OutOnlySongID		 Source +"song"+ Song
	 * @param OutStatus 播放状态
	 */
	void GetCurrentPlayMediaInfo(FString& OutAlbumID, FString& OutSongID, FString& OutOnlySongID, EPlayStatus& OutStatus);

	//获取原始播放元素的歌曲ID信息
	void GetRawSongID(FString& OutSongID);

private:
	static FUIPlayStatusManager* UIPlayStatusManager;
	
	//所有的被管理元素
	TMultiMap<FString,FUIPlayStatus*> AllPlayer;
	//最新改变状态的元素
	FString AlbumID;
	FString SongID;
	FString OnlySongID;
	//最新元素的背景最新状态,最新元素的动效高亮
	EPlayStatus Status = EPlayStatus::Pause;
	//原始的SongID
	FString RawSongID;
	//上一个改变状态的元素
	EPlayStatus DefaultStatus = EPlayStatus::Pause;
};


//UI播放状态管理功能的元素组件
class FUIPlayStatus
{
	friend FUIPlayStatusManager;
public:
	/**
	 * @brief 更改当前组件的ID，ID由3个部分拼接而成
	 * @param InSource 音源
	 * @param InAlbum 专辑
	 * @param InSong 歌曲
	 */
	void ChangeID(FString InSource,FString InAlbum,FString InSong="")
	{
		FUIPlayStatusManager* StatusManager = FUIPlayStatusManager::Get();
		StatusManager->DeletePlayer(this,Source,Album,Song);
		Source = InSource;
		Album = InAlbum;
		Song = InSong;
		StatusManager->AddPlayer(this,InSource,InAlbum,InSong);
	}

	/**
	 * @brief 更改当前组件的ID，ID由3个部分拼接而成
	 * @param InSource 音源
	 * @param InAlbum 专辑
	 * @param InSong 歌曲
	 */
	void ChangeSongID(FString InSource,FString InSong="")
	{
		FUIPlayStatusManager* StatusManager = FUIPlayStatusManager::Get();
		StatusManager->DeletePlayer(this,Source,Album,Song);
		Source = InSource;
		Album = "song";
		Song = InSong;
		StatusManager->AddPlayer(this,InSource,Album,InSong);
	}

	/**
	 * @brief 绑定各个事件发生时的委托
	 * @param InOuter 组件拥有者
	 * @param InPlayStatusChange 状态改变时会调用的委托 
	 */
	void Init(UObject* InOuter, bool InIsSpecialHight,
	          TFunction<void(UObject*,EPlayStatus)> InPlayStatusChange)
	{
		Outer = InOuter;
		IsSpecialHight = InIsSpecialHight;
		PlayStatusChange = InPlayStatusChange;
	}

	/**
	 * @brief 用于用户手动修改某个元素的状态
	 * @param InStatus 新的状态
	 */
	void UserSetStatus(EPlayStatus InStatus = EPlayStatus::Loading)
	{
		FUIPlayStatusManager::Get()->ChangeSong(Source,Album,Song,InStatus);
	}

	/**
	 * @brief 有init就应该有Destory,在这里清空当前可播放元素
	 */
	void Destory()
	{
		const FString InID = Source + Album + Song;
		if(InID != "")
		{
			FUIPlayStatusManager* StatusManager = FUIPlayStatusManager::Get();
			StatusManager->DeletePlayer(this,Source,Album,Song);
			Source = "";
			Album = "";
			Song = "";
			PlayStatusChange = nullptr;
		}
	}
	
	EPlayStatus GetStatus()
	{
		return Status;
	}

	/**
	 * @brief 被销毁时应该自动清空
	 */
	 ~FUIPlayStatus()
	{
		Destory();
	}
	
private:
	void AutoSetStatus(EPlayStatus InStatus)
	{
		Status = InStatus;
		if(PlayStatusChange)
		{
			if(Outer && Outer->IsValidLowLevel() && !Outer->IsUnreachable())
			{
				PlayStatusChange(Outer,Status);
			}
		}
		else
		{
			UE_LOG(MediaAppLog,Warning,TEXT("FUIPlayStatus Need Init"))
		}
	}
	
	UObject* Outer = nullptr;
	TFunction<void(UObject*,EPlayStatus)> PlayStatusChange;
	EPlayStatus Status = EPlayStatus::Pause;
	
	FString Source;
	FString Album;
	FString Song;

	bool IsSpecialHight; //有两种方式来高亮图片，一种是根据playmediachange和playmediastatus来.一种是根据playmediachange来
};