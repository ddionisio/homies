#include "input.h"
#include "i_input.h"

/*****************************************************************
/																 /
/						Mouse Functions							 /
/																 /
*****************************************************************/

/**********************************************************
;
;	Name:		InputMouseGetLoc
;
;	Purpose:	to get the mouse location
;
;	Input:		themouse
;
;	Output:		the location in COORD
;
;	Return:		A COORD with X & Y
;
**********************************************************/
PUBLIC LOCATION InputMouseGetLoc()
{
	return TheInput.Mouse.location;
}

/**********************************************************
;
;	Name:		InputMouseGetMove
;
;	Purpose:	grabs the amount of movement the mouse made
;
;	Input:		none
;
;	Output:		none
;
;	Return:		x & y amt of movement
;
**********************************************************/
PUBLIC LOCATION InputMouseGetMove()
{
	return TheInput.Mouse.moveamt;
}

/**********************************************************
;
;	Name:		InputMouseIsBtnDwn
;
;	Purpose:	To determine whether a given mouse
;				button is down	
;
;	Input:		An enumerated index into the mouse
;				button array is given as input
;
;	Output:		A bool value which determines if it is down
;
;	Return:		TRUE/FALSE
;
**********************************************************/
PUBLIC BOOL InputMouseIsBtnDwn(eMouseButton which)
{
	return TheInput.Mouse.button[which];
}

/**********************************************************
;
;	Name:		InputMouseIsHidden
;
;	Purpose:	determine if mouse is hidden or not
;
;	Input:		none
;
;	Output:		A bool value which determines if hidden or not
;
;	Return:		TRUE/FALSE
;
**********************************************************/
PUBLIC BOOL InputMouseIsHidden()
{
	return TheInput.Mouse.isHidden;
}

/**********************************************************
;
;	Name:		InputMouseHide
;
;	Purpose:	Hides the mouse cursor
;
;	Input:		The isHidden field of the global
;				mouse structure	
;
;	Output:		Changes the isHidden field of the	
;				global mouse structure	
;
;	Return:		Nothing	
;
**********************************************************/
PUBLIC void InputMouseHide()
{
	if(!TheInput.Mouse.isHidden)
		TheInput.Mouse.isHidden = TRUE;
	ShowCursor (FALSE);
}

/**********************************************************
;
;	Name:		InputMouseShow
;
;	Purpose:	Shows the mouse cursor	
;
;	Input:		The isHidden field of the global
;				mouse structure	
;
;	Output:		Changes the isHidden field of the
;				global mouse structure
;
;	Return:		Nothing	
;
**********************************************************/
PUBLIC void InputMouseShow()
{
	if(TheInput.Mouse.isHidden)
		TheInput.Mouse.isHidden = FALSE;
	ShowCursor (TRUE);
}

/**********************************************************
;
;	Name:		InputMouseUpdateBtn	
;
;	Purpose:	To update the button array of the
;				global mouse structure	
;
;	Input:		An index into the button array and a
;				button status flag are given as input
;
;	Output:		The button array is updated according
;				to its input
;
;	Return:		Nothing	
;
**********************************************************/
PUBLIC void InputMouseUpdateBtn(eMouseButton which, BOOL status)
{
	TheInput.Mouse.button[which] = status;
}

/**********************************************************
;
;	Name:		InputMouseUpdateLoc
;
;	Purpose:	To update the current mouse location
;
;	Input:		the hwnd
;
;	Output:		it_GetMouseLocation returns the	current 
;				mouse location, which is passed
;				to the input structure	
;
;	Return:		Nothing
;
**********************************************************/
PUBLIC void InputMouseUpdateLoc(HWND hwndClient)
{
	LOCATION oldLoc = TheInput.Mouse.location;

	GetCursorPos(&TheInput.Mouse.location);
	ScreenToClient(hwndClient, &TheInput.Mouse.location);

	TheInput.Mouse.moveamt.x = TheInput.Mouse.location.x - oldLoc.x;
	TheInput.Mouse.moveamt.y = TheInput.Mouse.location.y - oldLoc.y;
}