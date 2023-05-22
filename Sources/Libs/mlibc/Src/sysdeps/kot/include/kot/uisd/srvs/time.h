#ifndef KOT_SRV_TIME_H
#define KOT_SRV_TIME_H 1

#include <kot/sys.h>
#include <kot/uisd.h>
#include <kot/types.h>
#include <kot/memory.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FEMOSECOND_IN_NANOSECOND     1000000 // femosecond 10E15 to nanosecond 10E9

typedef KResult (*kot_TimeCallbackHandler)(KResult Status, struct kot_srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef struct{
    uint64_t Year;
    uint64_t Month;
    uint64_t Day;
    uint64_t WeekDay;
    uint64_t Hour;
    uint64_t Minute;
    uint64_t Second;
}kot_time_t;

struct kot_srv_time_callback_t{
    kot_thread_t Self;
    uintptr_t Data;
    size64_t Size;
    bool IsAwait;
    KResult Status;
    kot_TimeCallbackHandler Handler;
};

void kot_Srv_Time_Initialize();

void kot_Srv_Time_Callback(KResult Status, struct kot_srv_time_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct kot_srv_time_callback_t* kot_Srv_Time_SetTimePointerKey(kot_time_t** Time, bool IsAwait);
struct kot_srv_time_callback_t* kot_Srv_Time_SetTickPointerKey(uint64_t* TimePointer, uint64_t TickPeriod, bool IsAwait);


uint64_t kot_GetYear();
uint64_t kot_GetMonth();
uint64_t kot_GetDay();
uint64_t kot_GetWeekDay();
uint64_t kot_GetHour();
uint64_t kot_GetMinute();
uint64_t kot_GetSecond();
KResult kot_Sleep(uint64_t duration);
KResult kot_SleepFromTick(uint64_t* tick, uint64_t duration);
KResult kot_GetActualTick(uint64_t* tick);
KResult kot_GetTimeFromTick(uint64_t* time, uint64_t tick);
KResult kot_GetFemosecondTimeFromTick(uint64_t* time, uint64_t tick);
KResult kot_GetTickFromTime(uint64_t* tick, uint64_t time);
KResult kot_GetTime(uint64_t* time);
KResult kot_CompareTime(uint64_t* compare, uint64_t time0, uint64_t time1);

uint64_t kot_GetPosixTime();

#if defined(__cplusplus)
} 
#endif

#endif