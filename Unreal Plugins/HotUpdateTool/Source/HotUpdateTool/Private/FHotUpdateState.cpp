#include "FHotUpdateState.h"

float FHotUpdateState::GetStateBaseProgress(EHotUpdateState State)
{
	return StateToProgress[State];
}

float FHotUpdateState::GetTotalProgress()
{
	return TotalProgress;
}

void FHotUpdateState::SetStateAndStateProgress(EHotUpdateState State, float InCurrentStateProgress)
{
	CurrentState = State;
	CurrentStateProgress = InCurrentStateProgress;
	
	TotalProgress = GetStateBaseProgress(CurrentState) + GetCurrentStateAllProgress() * CurrentStateProgress;
}

float FHotUpdateState::GetCurrentStateAllProgress()
{
	EHotUpdateState NextState = EHotUpdateState::Complete;
	//TODO 简化switch 为枚举数值计算，因为这里是枚举值是逐渐增加的
	switch (CurrentState)
	{
	case EHotUpdateState::Error:
		NextState = EHotUpdateState::Error;
		break;
	case EHotUpdateState::Complete:
		NextState = EHotUpdateState::Complete;
		break;
	case EHotUpdateState::BeginUpdate:
		NextState = EHotUpdateState::LoadManifest;
		break;
	case EHotUpdateState::LoadManifest:
		NextState = EHotUpdateState::UnLoadPak;
		break;
	case EHotUpdateState::UnLoadPak:
		NextState = EHotUpdateState::MovePak;
		break;
	case EHotUpdateState::MovePak:
		NextState = EHotUpdateState::ReLoadPak;
		break;
	case EHotUpdateState::ReLoadPak:
		NextState = EHotUpdateState::UnLoadModule;
		break;
	case EHotUpdateState::UnLoadModule:
		NextState = EHotUpdateState::ModuleGC;
		break;
	case EHotUpdateState::ModuleGC:
		NextState = EHotUpdateState::LoadRootModule;
		break;
	case EHotUpdateState::LoadRootModule:
		NextState = EHotUpdateState::LoadAsset;
		break;
	case EHotUpdateState::LoadAsset:
		NextState = EHotUpdateState::Complete;
		break;
	default: ;
	}
	return GetStateBaseProgress(NextState) - GetStateBaseProgress(CurrentState);
}
