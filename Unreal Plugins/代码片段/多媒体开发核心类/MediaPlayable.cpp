#include "MediaPlayable.h"

FUIPlayStatusManager* FUIPlayStatusManager::UIPlayStatusManager = nullptr;

FUIPlayStatusManager* FUIPlayStatusManager::Get()
{
	if(!UIPlayStatusManager)
	{
		UIPlayStatusManager = new FUIPlayStatusManager;
	}
	return UIPlayStatusManager;
}

void FUIPlayStatusManager::Delete()
{
	delete UIPlayStatusManager;
	UIPlayStatusManager = nullptr;
}

void FUIPlayStatusManager::ChangeSong(const FString& Source, const FString& Album, const FString& Song, EPlayStatus InStatus)
{
	
	//将上一个元素恢复为默认状态
	if(AllPlayer.Contains(AlbumID))
	{
		TArray<FUIPlayStatus*> OutPlayers;
		AllPlayer.MultiFind(AlbumID,OutPlayers);
		for (FUIPlayStatus* OutPlayer : OutPlayers)
		{
			if(OutPlayer->IsSpecialHight)  //识别是小图标还是列表item
				{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) ) //识别是通过playmediachange进入还是playstatechange
					{
					OutPlayer->AutoSetStatus(EPlayStatus::Pause);
					}
				else
				{
					OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
				}
				}
			else
			{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) )
				{
					
				}
				else
				{
					OutPlayer->AutoSetStatus(EPlayStatus::Pause);
				}
			}
			/*OutPlayer->AutoSetStatus(DefaultStatus);*/
		}
	}
	if(AllPlayer.Contains(SongID))
	{
		TArray<FUIPlayStatus*> OutPlayers;
		AllPlayer.MultiFind(SongID,OutPlayers);
		for (FUIPlayStatus* OutPlayer : OutPlayers)
		{
			if(OutPlayer->IsSpecialHight)  //识别是小图标还是列表item
				{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) ) //识别是通过playmediachange进入还是playstatechange
					{
					OutPlayer->AutoSetStatus(EPlayStatus::Pause);
					}
				else
				{
					OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
				}
				}
			else
			{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) )
				{
					
				}
				else
				{
					OutPlayer->AutoSetStatus(EPlayStatus::Pause);
				}
			}
		}
	}
	if(AllPlayer.Contains(OnlySongID))
	{
		TArray<FUIPlayStatus*> OutPlayers;
		AllPlayer.MultiFind(OnlySongID,OutPlayers);
		for (FUIPlayStatus* OutPlayer : OutPlayers)
		{
			if(OutPlayer->IsSpecialHight)  //识别是小图标还是列表item
				{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) ) //识别是通过playmediachange进入还是playstatechange
					{
					OutPlayer->AutoSetStatus(EPlayStatus::Pause);
					}
				else
				{
					OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
				}
				}
			else
			{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) )
				{
					
				}
				else
				{
					OutPlayer->AutoSetStatus(EPlayStatus::Pause);
				}
			}
		}
	}
	                          
	//设置新元素的状态
	if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) )
	{
		
	}
	else
	{
		AlbumID = Source + Album;
	}
	if(InStatus == EPlayStatus::EffectPlaymedia)
	{
		Status = EPlayStatus::Playing;
	}
	else if(InStatus == EPlayStatus::EffectPlaymediaPause)
	{
		Status = EPlayStatus::EffectPause;
	}
	else
	{
		Status = InStatus;
	}
	
	if(AllPlayer.Contains(AlbumID))
	{
		TArray<FUIPlayStatus*> OutPlayers;
		AllPlayer.MultiFind(AlbumID,OutPlayers);
		for (FUIPlayStatus* OutPlayer : OutPlayers)
		{
			if(OutPlayer->IsSpecialHight)  //识别是小图标还是列表item
			{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) ) //识别是通过playmediachange进入还是playstatechange
				{
					if((InStatus ==  EPlayStatus::EffectPlaymedia))
					{
						OutPlayer->AutoSetStatus(EPlayStatus::Playing);
					}
					else
					{
						OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
					}
					
				}
				else
				{
					if(InStatus ==  EPlayStatus::Playing)
					{
						OutPlayer->AutoSetStatus(EPlayStatus::Playing);
					}
					else if(InStatus ==  EPlayStatus::Pause)
					{
						OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
					}
				}
			}
			else
			{
				if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) )
				{
					
				}
				else
				{
					OutPlayer->AutoSetStatus(Status);
				}
			}
		}
	}
	SongID = Source + Album + Song;
	RawSongID = Song;
	if(Song != "")
	{
		if(AllPlayer.Contains(SongID))
		{
			TArray<FUIPlayStatus*> OutPlayers;
			AllPlayer.MultiFind(SongID,OutPlayers);
			for (FUIPlayStatus* OutPlayer : OutPlayers)
			{
				if(OutPlayer->IsSpecialHight)  //识别是小图标还是列表item
					{
					if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) ) //识别是通过playmediachange进入还是playstatechange
						{
							if((InStatus ==  EPlayStatus::EffectPlaymedia))
							{
								OutPlayer->AutoSetStatus(EPlayStatus::Playing);
							}
							else
							{
								OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
							}
						}
					else
					{
						if(Status ==  EPlayStatus::Playing)
						{
							OutPlayer->AutoSetStatus(EPlayStatus::Playing);
						}
						else if(Status ==  EPlayStatus::Pause)
						{
							OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
						}
					}
					}
				else
				{
					if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) )
					{
					
					}
					else
					{
						OutPlayer->AutoSetStatus(Status);
					}
				}
			}
		}
	}
	OnlySongID = Source +"song"+ Song;
	if(Song != "")
	{
		if(AllPlayer.Contains(OnlySongID))
		{
			TArray<FUIPlayStatus*> OutPlayers;
			AllPlayer.MultiFind(OnlySongID,OutPlayers);
			for (FUIPlayStatus* OutPlayer : OutPlayers)
			{
				if(OutPlayer->IsSpecialHight)  //识别是小图标还是列表item
					{
					if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) ) //识别是通过playmediachange进入还是playstatechange
						{
						if((InStatus ==  EPlayStatus::EffectPlaymedia))
						{
							OutPlayer->AutoSetStatus(EPlayStatus::Playing);
						}
						else
						{
							OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
						}
						}
					else
					{
						if(Status ==  EPlayStatus::Playing)
						{
							OutPlayer->AutoSetStatus(EPlayStatus::Playing);
						}
						else if(Status ==  EPlayStatus::Pause)
						{
							OutPlayer->AutoSetStatus(EPlayStatus::EffectPause);
						}
					}
					}
				else
				{
					if((InStatus ==  EPlayStatus::EffectPlaymedia) || (InStatus ==  EPlayStatus::EffectPlaymediaPause) )
					{
					
					}
					else
					{
						OutPlayer->AutoSetStatus(Status);
					}
				}
			}
		}
	}
}

void FUIPlayStatusManager::AddPlayer(FUIPlayStatus* InPlayer, const FString& Source,const FString& Album,const FString& Song)
{
	FString InID = Source + Album + Song;
	if(InPlayer == nullptr || InID == "")
	{
		return;
	}
	//添加新元素，同时需要检查新元素是否正在播放
	AllPlayer.Add(InID,InPlayer);
	if(InID == OnlySongID || InID == AlbumID || InID == SongID)
	{
		InPlayer->AutoSetStatus(Status);
	}
	else
	{
		InPlayer->AutoSetStatus(EPlayStatus::Pause);
	}
}

void FUIPlayStatusManager::DeletePlayer(FUIPlayStatus* InPlayer,FString Source,FString Album,FString Song)
{
	FString InID = Source + Album + Song;
	if(InPlayer == nullptr || InID == "")
	{
		return;
	}
	if(AllPlayer.Contains(InID))
	{
		AllPlayer.RemoveSingle(InID,InPlayer);
	}
}

void FUIPlayStatusManager::GetCurrentPlayMediaInfo(FString& OutAlbumID, FString& OutSongID, FString& OutOnlySongID, EPlayStatus& OutStatus)
{
	OutAlbumID = AlbumID;
	OutSongID = SongID;
	OutOnlySongID = OnlySongID;
	OutStatus = Status;
}

void FUIPlayStatusManager::GetRawSongID(FString& OutSongID)
{
	OutSongID = RawSongID;
}
