#include <stdio.h>
#include <time.h>

#define Print(fmt,...) \
printf("TimeMs:%f  " fmt "\n", GetTimeMs(),__VA_ARGS__ ); \
fflush(stdout)

typedef void (*UserThreadFunc)(int);

time_t GTimeStart;

float GetTimeMs()
{
    return difftime(time(NULL), GTimeStart) * 1000.0f;
}

UserThreadFunc GDelayTask[10];
int GTaskBeginCode[10];
float GTaskCallTime[10];
int GTaskNum = 0;

void AddTask(float EndTime, UserThreadFunc Fun, int NextCode)
{
    if (GTaskNum >= 10)
        return;
    GDelayTask[GTaskNum] = Fun;
    GTaskBeginCode[GTaskNum] = NextCode;
    GTaskCallTime[GTaskNum] = EndTime;
    GTaskNum++;
}

void Yield()
{
    if (GTaskNum <= 0)
        return;
    float NowTime = GetTimeMs();
    for (int i = 0; i < GTaskNum; i++)
    {
        if (NowTime >= GTaskCallTime[i])
        {
            GDelayTask[i](GTaskBeginCode[i]);
            if (i < GTaskNum - 1)
            {
                GDelayTask[i] = GDelayTask[GTaskNum - 1];
                GTaskBeginCode[i] = GTaskBeginCode[GTaskNum - 1];
                GTaskCallTime[i] = GTaskCallTime[GTaskNum - 1];
            }
            GTaskNum--;
            break;
        }
    }
}

int GetTaskNum()
{
    return GTaskNum;
}

void DelayMs(int DelayTime, UserThreadFunc Fun, int NextCode)
{
    float EndTime = GetTimeMs() + DelayTime;
    AddTask(EndTime, Fun, NextCode);
}

// Test
#if 1 //函数实现方式
void Task1(int BeginCode)
{
    switch (BeginCode)
    {
    case 0:
        Print("Task1 0");
        DelayMs(5000, Task1, 1);
        break;
    case 1:
        Print("Task1 1");
        DelayMs(5000, Task1, 2);
        break;
    case 2:
        Print("Task1 2");
        DelayMs(5000, Task1, 3);
        break;
    case 3:
        Print("Task1 3");
        break;
    default:
        break;
    }
}

void Task2(int BeginCode)
{
    switch (BeginCode)
    {
    case 0:
        Print("Task2 0");
        DelayMs(1000, Task2, 1);
        break;
    case 1:
        Print("Task2 1");
        DelayMs(5000, Task2, 2);
        break;
    case 2:
        Print("Task2 2");
        DelayMs(5000, Task2, 3);
        break;
    case 3:
        Print("Task2 3");
        break;
    default:
        break;
    }
}

#else //使用宏简化可切换函数实现代码
#define CAN_YIELD_END \
break; \
default:\
Print("ErrorCode");\
break; \
}}

#define CAN_YIELD_FUNC(FunName) \
void FunName(int BeginCode) \
{ \
switch (BeginCode) \
{ \
case 0:

#define DELAY_AND_YIELD(DelayTimeMs,FunName,NextCode) \
DelayMs(DelayTimeMs, FunName, NextCode); \
break; \
case NextCode:

CAN_YIELD_FUNC(Task1)
    Print("Task1 0");

    DELAY_AND_YIELD(5000, Task1, 1)
    Print("Task1 1");

    DELAY_AND_YIELD(5000, Task1, 2)
    Print("Task1 2");

    DELAY_AND_YIELD(5000, Task1, 3)
    Print("Task1 3");

CAN_YIELD_END


CAN_YIELD_FUNC(Task2)
    Print("Task2 0");

    DELAY_AND_YIELD(1000, Task2, 1)
    Print("Task2 1");

    DELAY_AND_YIELD(5000, Task2, 2)
    Print("Task2 2");

    DELAY_AND_YIELD(5000, Task2, 3)
    Print("Task2 3");

CAN_YIELD_END
#endif

int main(int argc, char *argv[])
{
    GTimeStart = time(NULL);

    Task1(0);
    Task2(0);
    while (GetTaskNum())
    {
        Yield();
    }
    return 0;
}
