#include "input.h"
#include "i_input.h"

//This includes the direct input device(mouse, keyboard, joystick)
//TrioInput G_diitems;

//DImouse G_mouse_data;



//mouse themouse;

_Input TheInput = {0};


/*****************************************************************
/																 /
/						Main Functions							 /
/																 /
*****************************************************************/

/**********************************************************
;
;	Name:		InputInit
;
;	Purpose:	initializes input
;
;	Input:		the handle to client window(the main one to use)
;
;	Output:		the input is intialized
;
;	Return:		SUCCESS or FAILURE!!!
;
**********************************************************/
/*PUBLIC RETCODE InputInit(HWND hwndClient)
{
	//there better be none
	assert(!TheInput);

	if(MemAlloc((void**)&TheInput, sizeof(_Input)) != RETCODE_SUCCESS)
	{ assert(!"Unable to allocate TheInput, error in InputInit!"); return RETCODE_FAILURE; }

	//update the mouse loc
	InputMouseUpdateLoc(hwndClient);

	//we are done...
	return RETCODE_SUCCESS;
}*/

/**********************************************************
;
;	Name:		InputTerm
;
;	Purpose:	terminates the input
;
;	Input:		none
;
;	Output:		the input is killed
;
;	Return:		none
;
**********************************************************/
/*PUBLIC void InputTerm()
{
	if(TheInput)
	{
		MemFree((void**)&TheInput);
	}
}*/
