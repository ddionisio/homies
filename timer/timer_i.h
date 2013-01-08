#ifndef _timer_i_h
#define _timer_i_h

#include "timer.h"

extern LARGE_INTEGER g_freq;		//used by QueryPerformanceFrequency

typedef void (* TIMEINIT) (Ptimer timer);
typedef bool (* TIMEELLAPSE) (Ptimer timer);
typedef void (* TIMEFPS) (Ptimer timer, float *fps);

PROTECTED TIMEELLAPSE _pTimerEllapse;
PROTECTED TIMEINIT _pTimerInit;
PROTECTED TIMEFPS _pTimerFPS;

PROTECTED bool _TimerFreqEllapse(Ptimer timer);
PROTECTED bool _TimerTimeEllapse(Ptimer timer);

PROTECTED void _TimerFreqInit(Ptimer timer);
PROTECTED void _TimerTimeInit(Ptimer timer);

PROTECTED void _TimerFreqFPS(Ptimer timer, float *fps);
PROTECTED void _TimerTimeFPS(Ptimer timer, float *fps);

#endif