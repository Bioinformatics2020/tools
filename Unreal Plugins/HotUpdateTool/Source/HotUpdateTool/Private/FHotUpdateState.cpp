#include "FHotUpdateState.h"

#include "HotUpdateTool.h"

float FHotUpdateState::GetStateBaseProgress(EHotUpdateState State)
{
    return StateToProgress[State];
}

float FHotUpdateState::GetTotalProgress()
{
    return TotalProgress;
}

bool FHotUpdateState::SetStateAndStateProgress(EHotUpdateState State, float InCurrentStateProgress)
{
    if(InCurrentStateProgress >= 0.0f && InCurrentStateProgress <= 1.0f)
    {
        CurrentState = State;
        CurrentStateProgress = InCurrentStateProgress;

        TotalProgress = GetStateBaseProgress(CurrentState) + GetCurrentStateAllProgress() * CurrentStateProgress;
    
        // 发送状态变化消息
        OnStateChanged.Broadcast(CurrentState, TotalProgress);
        return true;
    }
    else
    {
        UE_LOG(LogHotUpdateTool, Warning,
            TEXT("FHotUpdateState::SetStateAndStateProgress Progress is invalid %f"), InCurrentStateProgress);
        return false;
    }
}

float FHotUpdateState::GetCurrentStateAllProgress()
{
    EHotUpdateState NextState;
    //获取下一个状态
    if ( CurrentState == EHotUpdateState::Complete || CurrentState == EHotUpdateState::Error )
    {
        //完成或者错误状态是最终状态，下一个状态为当前状态
        NextState = CurrentState;
    }
    else
    {
        //一般情况下，下一个状态为当前状态的下一个状态，与枚举顺序一致，只需要加一即可
        NextState = static_cast<EHotUpdateState>(static_cast<int>(CurrentState) + 1);
    }
    //当前状态对应的进度减去上一个状态的进度，等于当前状态的进度对应总时长
    return GetStateBaseProgress(NextState) - GetStateBaseProgress(CurrentState);
}