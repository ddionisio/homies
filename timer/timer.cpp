//#include "timer.h"
#include "timer_i.h"

LARGE_INTEGER g_freq;		//used by QueryPerformanceFrequency

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimerQuery()
{
	if(QueryPerformanceFrequency(&g_freq))
	{
		//query to awesome timer
		_pTimerEllapse = _TimerFreqEllapse;
		_pTimerInit    = _TimerFreqInit;
		_pTimerFPS	   = _TimerFreqFPS;
	}
	else
	{
		//query to crappy timer
		_pTimerEllapse = _TimerTimeEllapse;
		_pTimerInit    = _TimerTimeInit;
		_pTimerFPS	   = _TimerTimeFPS;
	}
 
}

PROTECTED bool _TimerFreqEllapse(Ptimer timer)
{
	timer->current.freq.QuadPart = GetGameTime();
	
	//timer->current.freq.QuadPart *= timer->spd;
	//timer->current.freq.QuadPart /= g_freq.QuadPart;

	timer->ellapsed = ((timer->current.freq.QuadPart - timer->last.freq.QuadPart)*timer->spd)/g_freq.QuadPart;

	if(timer->ellapsed >= timer->delay)
	{
		//timer->ellapsed = timer->current.freq.QuadPart - timer->last.freq.QuadPart;
		timer->last = timer->current;
		timer->counter++;
		return true;
	}
	
	return false;
}

PROTECTED bool _TimerTimeEllapse(Ptimer timer)
{
	return true;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC bool TimerEllapse(Ptimer timer)
{
	return _pTimerEllapse(timer);
}

PROTECTED void _TimerFreqInit(Ptimer timer)
{
	timer->current.freq.QuadPart = GetGameTime( );
	
	//timer->current.freq.QuadPart *= timer->spd;
	//timer->current.freq.QuadPart /= g_freq.QuadPart;

	timer->last = timer->current;
}

PROTECTED void _TimerTimeInit(Ptimer timer)
{
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TimerInit(Ptimer timer, LONGLONG spd, LONGLONG delay)
{
	memset(timer, 0, sizeof(struct _timer));
	timer->spd = spd;
	timer->delay = delay;

	_pTimerInit(timer);
}

PROTECTED void _TimerFreqFPS(Ptimer timer, float *fps)
{
	timer->ellapsed++;

	if(timer->ellapsed > timer->delay)
	{
		timer->current.freq.QuadPart = GetGameTime ( );
		
		*fps=(float)timer->delay/((float)(timer->current.freq.QuadPart - timer->last.freq.QuadPart)/(float)g_freq.QuadPart);
		
		timer->ellapsed=0;
		
		timer->last.freq.QuadPart = GetGameTime ( );
	}
}

PROTECTED void _TimerTimeFPS(Ptimer timer, float *fps)
{
}

PUBLIC void TimerFPS(Ptimer timer, float *fps)
{
	_pTimerFPS(timer, fps);
}
