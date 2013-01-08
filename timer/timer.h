#ifndef _timer_h
#define _timer_h

#include "..\common.h"

#include <winnt.h>
#include <winbase.h>

typedef union _timerFreq {
	LARGE_INTEGER freq;
	DWORD		   sysTime;		//used by timeGetTime
} timerFreq;

typedef struct _timer {
	LONGLONG ellapsed;
	LONGLONG delay;
	LONGLONG counter;
	LONGLONG spd;
	timerFreq current;
	timerFreq last;
} timer, * Ptimer;

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimerQuery();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC bool TimerEllapse(Ptimer timer);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimerInit(Ptimer timer, LONGLONG spd, LONGLONG delay);

PUBLIC void TimerFPS(Ptimer timer, float *fps);


/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void ResetStartTime ( );

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
__int64 GetGameTime ( );

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
void SetGameTimeEllapsed ( __int64 currentTime );

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
int GetGameTimeMS ( );

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC inline LONGLONG TimerGetCounter(Ptimer timer)
{
	return timer->counter;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC inline LONGLONG TimerGetEllapse(Ptimer timer)
{
	return timer->ellapsed;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC inline LONGLONG TimerGetDelay(Ptimer timer)
{
	return timer->delay;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC inline LONGLONG TimerGetSpeed(Ptimer timer)
{
	return timer->spd;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC inline void TimerResetCounter(Ptimer timer)
{
	timer->counter = 0;
}

#endif