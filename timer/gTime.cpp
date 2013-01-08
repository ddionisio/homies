#include <windows.h>

LARGE_INTEGER FreqTime;
LARGE_INTEGER StartTime;
LARGE_INTEGER CurrentTime;

void ResetStartTime ( ) {

	QueryPerformanceCounter ( &StartTime );
	QueryPerformanceFrequency ( &FreqTime );
}

void SetGameTimeEllapsed ( __int64 currentTime ) {

	QueryPerformanceCounter ( &CurrentTime );
	StartTime.QuadPart = CurrentTime.QuadPart - currentTime;
}

//returns the amount of time that has passed since the last
//reset was called, " GetGameTime / FreqTime " is the 
//amount of time that has passed in minutes
__int64 GetGameTime ( ) {

	QueryPerformanceCounter ( &CurrentTime );
	return CurrentTime.QuadPart - StartTime.QuadPart;
}

//Get the amount of time that has passed in miliseconds
int GetGameTimeMS ( ) {

	return (int) ( ( GetGameTime ( ) * 1000 ) / FreqTime.QuadPart );
}