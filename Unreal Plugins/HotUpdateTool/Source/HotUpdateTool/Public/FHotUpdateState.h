#pragma once

enum class EHotUpdateState
{
	Error = 0,
	Complete,//1

	BeginUpdate,//2
	
	LoadManifest,//3
	UnLoadPak,//4
	MovePak,//5
	ReLoadPak,//6
	
	UnLoadModule,//7
	ModuleGC,//8
	LoadRootModule,//9

	LoadAsset,//10
};

class FHotUpdateState
{
	TMap<EHotUpdateState,float> StateToProgress =
	{
		{EHotUpdateState::Error,1.0f},
		{EHotUpdateState::Complete,1.0f},
		
		{EHotUpdateState::BeginUpdate,0.0f},
		
		{EHotUpdateState::LoadManifest,0.1f},
		{EHotUpdateState::UnLoadPak,0.2f},
		{EHotUpdateState::MovePak,0.3f},
		{EHotUpdateState::ReLoadPak,0.4f},
		
		{EHotUpdateState::UnLoadModule,0.5f},
		{EHotUpdateState::ModuleGC,0.6f},
		{EHotUpdateState::LoadRootModule,0.7f},
		
		{EHotUpdateState::LoadAsset,0.8f},
	};
public:
	float GetStateBaseProgress(EHotUpdateState State);
	float GetTotalProgress();

	void SetStateAndStateProgress(EHotUpdateState State,float InCurrentStateProgress = 0.0f);

private:
	float GetCurrentStateAllProgress();
	
	EHotUpdateState CurrentState = EHotUpdateState::Complete;
	float CurrentStateProgress = 0;

	float TotalProgress = 0;
};
