#ifndef _i_input_h
#define _i_input_h

#include "input.h"

//
// Enums
//
typedef enum {
	keyUntouched,
	keyPressed,
	keyReleased,
	maxKeyState
} eKEYSTATE;

/*****************************************************************
/																 /
/							Typedefs							 /
/																 /
*****************************************************************/



typedef struct _mouse
{
	LOCATION  location;			 //location of mouse
	LOCATION  moveamt;			 //amount of mouse movement in x & y
    BOOL    button[eMaxButtons]; //button states
    BOOL    isHidden;			 //If hidden or not
} mouse;

typedef struct __Input {
	mouse Mouse;
	BYTE Keyboard[kMaxKeycodes]; //The whole keyboard TRUE(1) if pressed
	eKEYSTATE KBState[kMaxKeycodes]; //the whole keyboard state(keyUntouched,keyPressed,keyReleased)
} _Input, * PInput;

//
// Internal global
//
extern _Input TheInput;

#endif