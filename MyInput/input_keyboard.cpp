#include "input.h"
#include "i_input.h"

/*****************************************************************
/																 /
/						Keyboard Functions						 /
/																 /
*****************************************************************/

/**********************************************************
;
;	Name:		InputKbAnyKeyPressed
;
;	Purpose:	Determines whether any of the keyboard
;				keys are depressed
;
;	Input:		Global keyboard array
;
;	Output:		N/A	
;
;	Return:		Returns TRUE/FALSE if a key is down
;
**********************************************************/
PUBLIC BOOL InputKbAnyKeyPressed()
{
	int count;

	for (count = 0; count < kMaxKeycodes; count++)
	{
		if(TheInput.KBState[count] == keyPressed)//if (TheInput->Keyboard[count] & 0x80)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**********************************************************
;
;	Name:		InputKbAnyKeyReleased
;
;	Purpose:	checks to see if any of the keys were released
;
;	Input:		none
;
;	Output:		none
;
;	Return:		true if indeed one of the key is pressed
;
**********************************************************/
PUBLIC BOOL InputKbAnyKeyReleased()
{
	int count;

	for (count = 0; count < kMaxKeycodes; count++)
	{
		if(TheInput.KBState[count] == keyReleased)//if (TheInput->Keyboard[count] & 0x80)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**********************************************************
;
;	Name:		InputKbIsPressed
;
;	Purpose:	Determines whether the specific key
;				is pressed
;
;	Input:		Takes a keycode as input
;
;	Output:		N/A	
;
;	Return:		Returns TRUE/FALSE if the key is pressed
;
**********************************************************/
PUBLIC BOOL __fastcall InputKbIsPressed(BYTE kcode)
{
	return (TheInput.KBState[kcode] == keyPressed);//(TheInput->Keyboard[kcode] & 0x80) ? TRUE : FALSE;
}

/**********************************************************
;
;	Name:		InputKbIsReleased
;
;	Purpose:	checks to see if the given kcode is released
;
;	Input:		the kcode
;
;	Output:		none
;
;	Return:		if the given kcode was released
;
**********************************************************/
PUBLIC BOOL __fastcall InputKbIsReleased(BYTE kcode)
{
	return (TheInput.KBState[kcode] == keyReleased);
}

/**********************************************************
;
;	Name:		InputKbUpdate
;
;	Purpose:	Updates keyboard array with regard
;				to which keys are currently depressed
;				...blah.blah.blah.yaddy.yaddy.yadda
;
;	Input:		None
;
;	Output:		The keyboard array is updated according
;				to its input
;
;	Return:		if any key pressed		
;
**********************************************************/
PUBLIC bool InputKbUpdate()
{
	//GetKeyboardState (TheInput.Keyboard);

	int count;
	bool somethingpressed;

	for (count = 0; count < kMaxKeycodes; count++)
	{
		//if (TheInput.Keyboard[count] & 0x80)
		if(HIWORD(GetAsyncKeyState(count)))
		{ TheInput.KBState[count] = keyPressed; somethingpressed = true; }
		//the button wasn't down, but it was a while ago...
		//that means it got released
		else if(TheInput.KBState[count] == keyPressed)
			TheInput.KBState[count] = keyReleased;
		//otherwise, nothing was pressed nor released
		else
			TheInput.KBState[count] = keyUntouched;
	}

	return somethingpressed;
}

/**********************************************************
;
;	Name:		InputKbClear
;
;	Purpose:	useless...
;
;	Input:		no really, it's useless.
;
;	Output:		I don't know why I have this function.
;
;	Return:		but if you want to use it, go ahead.
;
**********************************************************/
PUBLIC void InputKbClear()
{
	memset(TheInput.Keyboard, 0, sizeof(BYTE)*kMaxKeycodes);
}

/**********************************************************
;
;	Name:		InputKbGetAscii
;
;	Purpose:	grabs the ascii version of the given code.
;				a flag if there is a menu
;
;	Input:		the code
;
;	Output:		none
;
;	Return:		the ascii
;
**********************************************************/
PUBLIC char InputKbGetAscii(BYTE kcode, UINT activeMenuFlag)
{
	char buff[2];

	ToAscii(
  kcode,     // virtual-key code
  kcode,    // scan code
  TheInput.Keyboard,  // key-state array
  (WORD*)buff,     // buffer for translated key
  activeMenuFlag        // active-menu flag
	);

	return buff[0]; 
}