#ifndef _dinputstuff_h
#define _dinputstuff_h

// You must include this define to use QueryInterface
#define INITGUID

#define BUFFERSIZE 16

extern enum eDIMouseButton {DummyButtonThatYouShouldNeverUse, MiddleButton, RightButton, LeftButton, MaxButtons};
#define LEFTMOUSEBUTTON   12
#define MIDDLEMOUSEBUTTON 14
#define RIGHTMOUSEBUTTON  13


#include "dinput.h"

typedef struct _DImouse
{
	COORD  location;			 //location of mouse
    BOOL    button[MaxButtons];			//button states(one click)
	BOOL    double_click[MaxButtons];	//button states(double click)
	DWORD   DoubleClickTime;			//This is tells how fast the user must click twice to get a double click
	DWORD   LastClickTime;				//This is used to tell when the user double clicked(DON'T MODIFY THIS)
	DWORD   LastButtonClicked;			//This is used for double clicking(DON'T MODIFY THIS)
    BOOL    isHidden;			 //If hidden or not
} DImouse, * PDImouse;


//
// This includes everthing including all 3 input devices: keyboard, mouse and joystick
//
typedef struct _TrioInput {
	LPDIRECTINPUT dinputobj; //This is the direct input object, used for retrieving input devices
	LPDIRECTINPUTDEVICE2 dkeyboard;	//input device for keyboard
	LPDIRECTINPUTDEVICE2 dmouse;	//input device for the mouse
	LPDIRECTINPUTDEVICE2 djoystick;	//the joystick device
} TrioInput, *PTrioInput;

/**********************************************************
;
;	Name:		dinput_init
;
;	Purpose:	initializes the direct input object and the three devices...joystick support is minimum
;
;	Input:		The handle to window, the hinstance and the data struct which includes the 3 devices
;				The last three are cooperation for each device (in order: mouse, keyboard, joystick)
;
;	Output:		stuff are put in the data struct
;
;	Return:		TRUE if success...and FALSE for failure
;
**********************************************************/
BOOL dinput_init(HWND hwnd, HINSTANCE hinst, PTrioInput pdiitems, DWORD mouse_coop_flag, DWORD kybrd_coop_flag, DWORD jystck_coop_flag);

/**********************************************************
;
;	Name:		dinput_term
;
;	Purpose:	This will destroy the 3 devices and the direct input object
;
;	Input:		The data structure that contians all 3 devices and the direct input object
;
;	Output:		The data is destroyed
;
;	Return:		none
;
**********************************************************/
void dinput_term(PTrioInput pdiitems);

/**********************************************************
;
;	Name:		diupdate_mouse
;
;	Purpose:	To get the location and button status of the mouse
;
;	Input:		The mouse direct input device and it better damn be it!  As well as the mouse data
;
;	Output:		updates the global mouse data location and button
;
;	Return:		none
;
**********************************************************/
void diupdate_mouse(HWND hwnd, LPDIRECTINPUTDEVICE2 dmouse, PDImouse mouse_data);

/**********************************************************
;
;	Name:		diupdate_keyboard
;
;	Purpose:	Updates the 256 array keys passed in...
;
;	Input:		hwnd for error stuff, keyboard direct input device and of course the keys[256]
;				...it better damn be 256!!!
;
;	Output:		the keys array is updated
;
;	Return:		none
;
**********************************************************/
void diupdate_keyboard(HWND hwnd, LPDIRECTINPUTDEVICE2 dkeyboard, BYTE *keys);

#endif