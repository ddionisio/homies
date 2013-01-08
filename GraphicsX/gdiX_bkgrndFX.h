#ifndef _gdix_bkgrndfx_h
#define _gdix_bkgrndfx_h

#include "gdix.h"

typedef struct _gfxBkFX		*hBKFX;	//handle to background FX

typedef enum {
	eBKFX_FADEINOUT,	//in  - fade from color to background
						//out - fade from background to color
	eBKFX_IMGFADEINOUT,	//same as above...except transition would be an image
	eBKFX_CURTAIN,		//curtain goes down and fade out to color
	eBKFX_MAX
} eBKFXType;

/**********************************************************
***********************************************************

  The init structs you must fill up for FX

***********************************************************
**********************************************************/

//
// background FX init struct for each type
//
typedef struct _fadeinout_init {
	BYTE r,g,b,a;
	BYTE spd;  //speed of change
	bool bDir; //true = in, false = out
} fadeinout_init;

typedef struct _imgfadeinout_init {
	hTXT theImg;	//image to transit to
	BYTE a;			//starting/ending alpha
	BYTE spd;		//speed of change
	bool bDir;		//true = in, false = out
} imgfadeinout_init;

typedef struct _curtain_init {
	hTXT curtainImg;		//curtain to fall down
	BYTE r,g,b,a;
	BYTE spd;  //speed of change
} curtain_init;

/////////////////////////////////////
// Name:	BKFXCreate
// Purpose:	creates a background FX
// Output:	none
// Return:	the new background FX
/////////////////////////////////////
PUBLIC hBKFX BKFXCreate(eBKFXType type, void *initStruct, int maxDuration,
						int delay);

/////////////////////////////////////
// Name:	BKFXUpdate
// Purpose:	updates and displays the
//			background FX
// Output:	stuff displayed
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE BKFXUpdate(hBKFX bkFX);

/////////////////////////////////////
// Name:	BKFXDestroy
// Purpose:	destroys given bkFX
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void BKFXDestroy(hBKFX *bkFX);

#endif