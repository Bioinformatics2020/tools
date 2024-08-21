#include "ChooseManager.h"

#include "MediaConfig.h"

void DeleteManager::Init(TFunction<void(bool)> ChooseAll)
{
	ChooseAllStyle = ChooseAll;
}

void DeleteManager::ChooseAll(bool InChoose)
{
	if(InChoose)
	{
		ChoosedNum = AllChooseItem.Num();
	}
	else
	{
		ChoosedNum = 0;
	}
	
	for (auto& Player : AllChooseItem)
	{
		if(!Player.Value == InChoose)
		{
			Player.Key->SetChooseMode(InChoose);
			
			Player.Value = InChoose;
		}
	}
	CheckChooseMode();
}

void DeleteManager::ChangePlayer(IChooseComment* ChooseComment)
{
	if(ChooseComment == nullptr)
	{
		return;
	}
	
	if(AllChooseItem.Contains(ChooseComment))
	{
		if(AllChooseItem[ChooseComment])
		{
			ChooseComment->SetChooseMode(false);
			(*(AllChooseItem.Find(ChooseComment))) = false;
			ChoosedNum--;
		}
		else
		{
			ChooseComment->SetChooseMode(true);
			(*(AllChooseItem.Find(ChooseComment))) = true;
			ChoosedNum++;
		}
		CheckChooseMode();
	}
}

void DeleteManager::AddPlayer(IChooseComment* ChooseComment, bool InbChoosed)
{
	if(ChooseComment == nullptr)
	{
		return;
	}
	if(InbChoosed)
	{
		ChoosedNum++;
	}
	ChooseComment->SetChooseMode(InbChoosed);
	AllChooseItem.Add(ChooseComment,InbChoosed);
	CheckChooseMode();
}

void DeleteManager::AddPlayers(TArray<IChooseComment *> ChooseComments, bool InbChoosed)
{
	for (auto ChooseComment : ChooseComments)
	{
		checkf(ChooseComment,TEXT("ChooseComment is null"));
		ChooseComment->SetChooseMode(InbChoosed);
		AllChooseItem.Add(ChooseComment,InbChoosed);
	}
	if(InbChoosed)
	{
		ChoosedNum += ChooseComments.Num();
	}
	CheckChooseMode();
}

void DeleteManager::RemovePlayer(IChooseComment* ChooseComment)
{
	if(ChooseComment == nullptr)
	{
		return;
	}
	if(AllChooseItem.Find(ChooseComment))
	{
		if(*(AllChooseItem.Find(ChooseComment)))
		{
			ChoosedNum--;
		}
		AllChooseItem.Remove(ChooseComment);
		CheckChooseMode();
	}
}

void DeleteManager::ClearPlayer()
{
	AllChooseItem.Empty();
	ChoosedNum = 0;
	CheckChooseMode();
}

int DeleteManager::GetChooseNum()
{
	return ChoosedNum;
}

EMediaEditType DeleteManager::GetMediaEditType()
{
	return MediaEditType;
}

TMap<IChooseComment *, bool> & DeleteManager::GetAllChooseItem()
{
	return AllChooseItem;
}

void DeleteManager::CheckChooseMode()
{
	if(ChoosedNum == 0)
	{
		MediaEditType = EMediaEditType::ChooseNull;
	}
	else if(ChoosedNum == AllChooseItem.Num())
	{
		MediaEditType = EMediaEditType::ChooseAll;
	}
	else
	{
		MediaEditType = EMediaEditType::ChooseNormal;
	}
	OnMediaEditTypeChange.ExecuteIfBound(MediaEditType);
	OnNumberChange.ExecuteIfBound(ChoosedNum);
	
	if(ChooseAllStyle != nullptr)
	{
		//当所有元素都被选中时
		ChooseAllStyle(ChoosedNum == AllChooseItem.Num());
	}
}

TArray<FString> MediaDeleteManager::GetChooseId()
{
	return Ids.Array();
}

void MediaDeleteManager::ChooseAll(bool InChoose)
{
	//标记为全选状态
	DeleteManager::ChooseAll(InChoose);

	//记录ID
	Ids.Empty();
	if(InChoose)
	{
		for (auto Player : AllChooseItem)
		{
			Ids.Add(static_cast<IMediaChooseComment*>(Player.Key)->GetMediaChooseId());
		}
	}
}

void MediaDeleteManager::ChangePlayer(IChooseComment *ChooseComment)
{
	if(ChooseComment == nullptr)
	{
		return;
	}

	
	if(AllChooseItem.Contains(ChooseComment))
	{
		if(AllChooseItem[ChooseComment])
		{
			ChooseComment->SetChooseMode(false);
			(*(AllChooseItem.Find(ChooseComment))) = false;
			Ids.Remove(static_cast<IMediaChooseComment*>(ChooseComment)->GetMediaChooseId());
			ChoosedNum--;
		}
		else
		{
			ChooseComment->SetChooseMode(true);
			(*(AllChooseItem.Find(ChooseComment))) = true;
			Ids.Add(static_cast<IMediaChooseComment*>(ChooseComment)->GetMediaChooseId());
			ChoosedNum++;
		}
		CheckChooseMode();
	}
}

void MediaDeleteManager::EntryEditMode(bool InEditMode, bool DefaultChooseMode)
{
	for (auto Player : AllChooseItem)
	{
		static_cast<IMediaChooseComment*>(Player.Key)->EntryEditMode(InEditMode);
	}
	ChooseAll(DefaultChooseMode);
	CheckChooseMode();
}