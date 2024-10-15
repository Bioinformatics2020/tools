#include <stdio.h>
#include <time.h>

#define Print(fmt,...) \
printf("TimeMs:%f  " fmt "\n", GetTimeMs(),__VA_ARGS__ ); \
fflush(stdout)

typedef void (*UserThreadFunc)(int, int);

clock_t GTimeStart;

float GetTimeMs()
{
    return (double)(clock() - GTimeStart) / CLOCKS_PER_SEC * 1000.0;
}

UserThreadFunc GDelayTask[10];
int GTaskBeginCode[10];
int GTaskParam[10];
float GTaskCallTime[10];
int GTaskNum = 0;

void AddTask(float EndTime, UserThreadFunc Fun, int NextCode, int Param)
{
    if (GTaskNum >= 10)
        return;
    GDelayTask[GTaskNum] = Fun;
    GTaskBeginCode[GTaskNum] = NextCode;
    GTaskParam[GTaskNum] = Param;
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
            GDelayTask[i](GTaskBeginCode[i], GTaskParam[i]);
            if (i < GTaskNum - 1)
            {
                GDelayTask[i] = GDelayTask[GTaskNum - 1];
                GTaskBeginCode[i] = GTaskBeginCode[GTaskNum - 1];
                GTaskParam[i] = GTaskParam[GTaskNum - 1];
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

void DelayMs(int DelayTime, UserThreadFunc Fun, int NextCode, int Param)
{
    float EndTime = GetTimeMs() + float(DelayTime);
    AddTask(EndTime, Fun, NextCode, Param);
}

// Test
#if 0 //函数实现方式
void Task1(int BeginCode, int Param)
{
    switch (BeginCode)
    {
    case 0:
        Print("Task1 0");
        DelayMs(1000, Task1, 1, 0);
        break;
    case 1:
        //模拟while循环
        if (Param < 6)
        {
            Print("Task1 1 Param:%d", Param);
            DelayMs(300, Task1, 1, Param + 1);
            break;
        }
        DelayMs(1000, Task1, 2, 0);
        break;
    case 2:
        Print("Task1 2");
        DelayMs(5000, Task1, 3, 0);
        break;
    case 3:
        Print("Task1 3");
        break;
    default:
        break;
    }
}

void Task2(int BeginCode, int Param)
{
    switch (BeginCode)
    {
    case 0:
        //模拟do_While循环
        Print("Task2 0 Param:%d", Param);
        if (Param + 1 < 15)
        {
            DelayMs(400, Task2, 0, Param + 1);
            break;
        }

        DelayMs(1000, Task2, 1, 0);
        break;
    case 1:
        Print("Task2 1");
        DelayMs(2000, Task2, 2, 0);
        break;
    case 2:
        Print("Task2 2");
        DelayMs(5000, Task2, 3, 0);
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
void FunName(int BeginCode, int Param) \
{ \
switch (BeginCode) \
{ \
case 0:

#define DELAY_AND_YIELD(DelayTimeMs,FunName,NextCode,Param) \
DelayMs(DelayTimeMs, FunName, NextCode, Param); \
break; \
case NextCode:

CAN_YIELD_FUNC(Task1)
    Print("Task1 0");

    DELAY_AND_YIELD(1000, Task1, 1, 0)
    //模拟while循环
    if (Param < 6)
    {
        Print("Task1 1 Param:%d", Param);
        DelayMs(300, Task1, 1, Param + 1);
        return;
    }

    DELAY_AND_YIELD(5000, Task1, 2, 0)
    Print("Task1 2");

    DELAY_AND_YIELD(5000, Task1, 3, 0)
    Print("Task1 3");

CAN_YIELD_END


CAN_YIELD_FUNC(Task2)
    //模拟do_While循环
    Print("Task2 0 Param:%d", Param);
    if (Param + 1 < 15)
    {
        DelayMs(400, Task2, 0, Param + 1);
        break;
    }

    DELAY_AND_YIELD(1000, Task2, 1, 0)
    Print("Task2 1");

    DELAY_AND_YIELD(5000, Task2, 2, 0)
    Print("Task2 2");

    DELAY_AND_YIELD(5000, Task2, 3, 0)
    Print("Task2 3");

CAN_YIELD_END
#endif

int main(int argc, char *argv[])
{
    GTimeStart = clock();

    Task1(0, 0);
    Task2(0, 0);
    while (GetTaskNum())
    {
        Yield();
    }
    return 0;
}
