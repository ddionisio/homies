#ifndef _input_h
#define _input_h

#include "..\common.h"
#include "..\Memory\memory.h"
//#include "dinputstuff.h"
//#include "themain.h"

/* ////////////////////////////////////////////////////////////////
				This contains all the input data struct.
		   Anything that has input happens here

     //////////////////////////////////////////////////////////////// */
#define kButtonUp	  FALSE
#define kButtonDown	  TRUE

//this is obsolete
//use dinput for now!!!
//enum eMouseButton {DummyButtonThatYouShouldNeverEverForeverUse, eLeftButton, eRightButton, eMaxButtons};
typedef enum {eLeftButton, eRightButton, eMaxButtons} eMouseButton;

//This includes the direct input device(mouse, keyboard, joystick)
//extern TrioInput G_diitems;
//This has all the information about the mouse
//extern DImouse G_mouse_data;

//
// KEY CODES so that you don't have to look-up MSDN
// Good old Van Ly stuff...err actually no
//

/***************************************************************
;	Keyboard code constant (using windows virtual-key codes
;***************************************************************/
#define kCode0					0x30
#define kCode1					0x31
#define kCode2					0x32
#define kCode3					0x33
#define kCode4					0x34
#define kCode5					0x35
#define kCode6					0x36
#define kCode7					0x37
#define kCode8					0x38
#define kCode9					0x39
#define kCodeA					0x41
#define kCodeB					0x42
#define kCodeC					0x43
#define kCodeD					0x44
#define kCodeE					0x45
#define kCodeF					0x46
#define kCodeG					0x47
#define kCodeH					0x48
#define kCodeI					0x49
#define kCodeJ					0x4A
#define kCodeK					0x4B
#define kCodeL					0x4C
#define kCodeM					0x4D
#define kCodeN					0x4E
#define kCodeO					0x4F
#define kCodeP					0x50
#define kCodeQ					0x51
#define kCodeR					0x52
#define kCodeS					0x53
#define kCodeT					0x54
#define kCodeU					0x55
#define kCodeV					0x56
#define kCodeW					0x57
#define kCodeX					0x58
#define kCodeY					0x59
#define kCodeZ					0x5A
#define kCodeComma			0xBC
#define kCodePeriod			0xBE
#define kCodeBackslash		0xDC
#define kCodeSlash			0xBF
#define kCodeSemicolon		0xBA
#define kCodeApostrophe		0xDE
#define kCodeLfbrace			0xDB
#define kCodeRtbrace			0xDD
#define kCodeMinus			0xBD
#define kCodeEqual			0xBB
#define kCodeTilde			0xC0
#define kCodeNumpad0			VK_NUMPAD0
#define kCodeNumpad1			VK_NUMPAD1
#define kCodeNumpad2			VK_NUMPAD2
#define kCodeNumpad3			VK_NUMPAD3
#define kCodeNumpad4			VK_NUMPAD4
#define kCodeNumpad5			VK_NUMPAD5
#define kCodeNumpad6			VK_NUMPAD6
#define kCodeNumpad7			VK_NUMPAD7
#define kCodeNumpad8			VK_NUMPAD8
#define kCodeNumpad9			VK_NUMPAD9
#define kCodeNumpadMult		VK_MULTIPLY
#define kCodeNumpadAdd		VK_ADD
#define kCodeNumpadSub		VK_SUBTRACT
#define kCodeNumpadDiv		VK_DIVIDE
#define kCodeNumpadDec		VK_DECIMAL
#define kCodeF1				VK_F1
#define kCodeF2				VK_F2
#define kCodeF3				VK_F3
#define kCodeF4				VK_F4
#define kCodeF5				VK_F5
#define kCodeF6				VK_F6
#define kCodeF7				VK_F7
#define kCodeF8				VK_F8
#define kCodeF9				VK_F9
#define kCodeF10				VK_F10
#define kCodeF11				VK_F11
#define kCodeF12				VK_F12
#define kCodeF13				VK_F13
#define kCodeF14				VK_F14
#define kCodeF15				VK_F15
#define kCodeF16				VK_F16
#define kCodeF17				VK_F17
#define kCodeF18				VK_F18
#define kCodeF19				VK_F19
#define kCodeF20				VK_F20
#define kCodeF21				VK_F21
#define kCodeF22				VK_F22
#define kCodeF23				VK_F23
#define kCodeF24				VK_F24
#define kCodeShift			VK_SHIFT
#define kCodeCntrl			VK_CONTROL
#define kCodeSpc				VK_SPACE
#define kCodeUp				VK_UP
#define kCodeDown				VK_DOWN
#define kCodeLeft				VK_LEFT
#define kCodeRight			VK_RIGHT
#define kCodeEsc				VK_ESCAPE
#define kCodeRet				VK_RETURN
#define kCodeBkspc			VK_BACK
#define kCodeTab				VK_TAB
#define kCodeClear			VK_CLEAR
#define kCodeAlt				VK_MENU
#define kCodePause			VK_PAUSE
#define kCodeCaps				VK_CAPITAL
#define kCodePgup				VK_PRIOR
#define kCodePgdown			VK_NEXT
#define kCodeEnd				VK_END
#define kCodeHome				VK_HOME
#define kCodeSelect			VK_SELECT
#define kCodeExecute			VK_EXECUTE
#define kCodePrtScr			VK_SNAPSHOT
#define kCodeIns				VK_INSERT
#define kCodeDel				VK_DELETE
#define kCodeHelp				VK_HELP
#define kCodeNunLock			VK_NUMLOCK
#define kCodeScrollLock		VK_SCROLL
#define kMaxKeycodes			256

/*****************************************************************
/																 /
/						Main Functions							 /
/																 /
*****************************************************************/


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
PUBLIC LOCATION InputMouseGetLoc();

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
PUBLIC LOCATION InputMouseGetMove();

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
PUBLIC BOOL InputMouseIsBtnDwn(eMouseButton which);

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
PUBLIC BOOL InputMouseIsHidden();

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
PUBLIC void InputMouseHide();

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
PUBLIC void InputMouseShow();

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
PUBLIC void InputMouseUpdateBtn(eMouseButton which, BOOL status);

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
PUBLIC void InputMouseUpdateLoc(HWND hwndClient);

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
PUBLIC BOOL InputKbAnyKeyPressed();

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
PUBLIC BOOL InputKbAnyKeyReleased();

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
PUBLIC BOOL __fastcall InputKbIsPressed(BYTE kcode);

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
PUBLIC BOOL __fastcall InputKbIsReleased(BYTE kcode);

/**********************************************************
;
;	Name:		InputKbIsUnpressed
;
;	Purpose:	GOBBLES!
;
;	Input:		any key code
;
;	Output:		none
;
;	Return:		TRUE if key unpressed
;
**********************************************************/
//PUBLIC BOOL InputKbIsUnpressed(BYTE kcode);

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
PUBLIC bool InputKbUpdate();

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
PUBLIC void InputKbClear();

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
PUBLIC char InputKbGetAscii(BYTE kcode, UINT activeMenuFlag=0);

#endif