#pragma once

#include "FHotUpdateState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FHotUpdateStateDelegate, EHotUpdateState, float);

//更新状态枚举
UENUM(BlueprintType)
enum class EHotUpdateState : uint8
{
    Error = 0,
    Complete,       //1

    BeginUpdate,    //2

    LoadManifest,   //3
    UnLoadPak,      //4
    MovePak,        //5
    ReLoadPak,      //6

    UnLoadModule,   //7
    ModuleGC,       //8
    LoadRootModule, //9

    LoadAsset,      //10
};

class FHotUpdateState
{
    //每一个更新状态对应的进度，目前这个值是初略设计，如果要求进度精确，需要实际测试调整
    TMap<EHotUpdateState, float> StateToProgress =
    {
        { EHotUpdateState::Error, 1.0f },
        { EHotUpdateState::Complete, 1.0f },

        { EHotUpdateState::BeginUpdate, 0.0f },

        { EHotUpdateState::LoadManifest, 0.1f },
        { EHotUpdateState::UnLoadPak, 0.2f },
        { EHotUpdateState::MovePak, 0.3f },
        { EHotUpdateState::ReLoadPak, 0.4f },

        { EHotUpdateState::UnLoadModule, 0.5f },
        { EHotUpdateState::ModuleGC, 0.6f },
        { EHotUpdateState::LoadRootModule, 0.7f },

        { EHotUpdateState::LoadAsset, 0.8f },
    };

public:
    /**
     * 获取输入状态定义的进度
     * @param State 进度状态
     * @return 范围为0-1
     */
    float GetStateBaseProgress(EHotUpdateState State);
    /**
     * 获取当前总进度，计算方法为 (当前进度 + 当前进度总时长*当前进度内部的详细进度)
     * @return 范围为0-1
     */
    float GetTotalProgress();

    /**
     * 设置当前状态和状态内详细进度,应该只由HotUpdateSubsystem调用
     * @param State 最新状态
     * @param InCurrentStateProgress 状态内的详细进度(0-1)
     * @return 是否成功设置
     */
    bool SetStateAndStateProgress(EHotUpdateState State, float InCurrentStateProgress = 0.0f);

    //状态变更回调，状态变更可能发生在异步线程或异步任务，需要转发到主线程
    FHotUpdateStateDelegate& OnStateChangedEvent()
    {
        return OnStateChanged;
    }
    
private:
    /**
     * 获取当前状态对应的总占用进度
     * @return 当前进度总时长
     */
    float GetCurrentStateAllProgress();

    //状态变更回调
    FHotUpdateStateDelegate OnStateChanged;

    //当前状态类型以及在当前状态内详细的进度
    EHotUpdateState CurrentState = EHotUpdateState::Complete;
    float CurrentStateProgress = 0;

    //结合当前状态和状态内详细进度，计算总进度
    float TotalProgress = 0;
};