/***********************************************************************************************

		Purpose  : To get yet another good grade in CS160
		Author   : David Dionisio
		Revision : 3/27/00
		Version  : 1

 **********************************************************************************************/

#ifndef _mywin_h
#define _mywin_h

/* ////////////////////////////////////////////////////////////////
defines
//////////////////////////////////////////////////////////////// */
//we want to be LEAN AND MEAN!
#define WIN32_LEAN_AND_MEAN


/* ////////////////////////////////////////////////////////////////
includes
     //////////////////////////////////////////////////////////////// */
//#include "themain.h"
/*#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
//#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <math.h>
#include <commdlg.h>
#include <time.h>*/
#include "common.h" 
#include <commdlg.h>

#define NOTHINGATALL 0

//Gets the index of an array as if it was a double array
//#define BOARDPOS(board, row, col, numcols) (board[((row) * (numcols)) + (col)])

//These are global stuff so that we can access it anywhere
extern RECT G_clientarea;
extern char G_filepath[MAXCHARBUFF];

/**********************************************************
;
;	Name:		Create_Window_Overlapped
;
;	Purpose:	Just so that I don't have to cut and paste.
;
;	Input:		title of window, the Window Function Callback,
;				the hinstance handle, the menu handler if you want, 
;				data to be passed to WM_CREATE, x & y position 
;				and dimension.
;
;	Output:		a lot of stuff happens
;
;	Return:		The handle of the window, if you want...better to use G_hwnd instead.
;
**********************************************************/
HWND Create_Window_Overlapped(char *name, char *menu_name, DWORD customStyle, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey);

/**********************************************************
;
;	Name:		Create_Window_Popup
;
;	Purpose:	Just so that I don't have to cut and paste.
;
;	Input:		title of window, the Window Function Callback,
;				the hinstance handle, the menu handler if you want, 
;				data to be passed to WM_CREATE, x & y position 
;				and dimension.
;
;	Output:		a lot of stuff happens
;
;	Return:		The handle of the window, if you want...better to use G_hwnd instead.
;
**********************************************************/
HWND Create_Window_Popup(char *name, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey);

/**********************************************************
;
;	Name:		WindowDestroy
;
;	Purpose:	Destroys given window
;
;	Input:		the HWND, HINSTANCE and it's class name
;
;	Output:		destroys window, don't forget to set HWND and
;				HINSTANCE to NULL after this function.
;
;	Return:		none
;
**********************************************************/
void WindowDestroy(HWND hwnd, HINSTANCE hinst, const char *className);

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
MSG GetWindowMessage();

/**********************************************************
;
;	Name:		LoadBox
;
;	Purpose:	A simple load dialog box is opened and will return the pathname
;
;	Input:		HWND, max char, filter ie. *.exe, DefExt ie. exe, title of load box,
;				flags.
;
;	Output:		load box is shown
;
;	Return:		Full pathname of the file
;
**********************************************************/
char * LoadBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags);

/**********************************************************
;
;	Name:		SaveBox
;
;	Purpose:	A simple save dialog box is opened and will return the pathname
;
;	Input:		HWND, max char, filter ie. *.exe, DefExt ie. exe, title of load box,
;				flags.
;
;	Output:		load box is shown
;
;	Return:		Full pathname of the file
;
**********************************************************/
char * SaveBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags);

/**********************************************************
;
;	Name:		GrabAllText
;
;	Purpose:	allocates a big string buffer and stores all
;				text from a file to the big string buffer
;				DONT FORGET TO FREE THE BUFFER AFTER USE
;
;	Input:		the filename
;
;	Output:		none
;
;	Return:		NULL if there was a failure and something if successful
;
**********************************************************/
char * GrabAllText(char *filename);

/**********************************************************
;
;	Name:	Random	
;
;	Purpose:	makes a random number between a given range
;
;	Input:		min and max number
;
;	Output:		none
;
;	Return:		the generated number
;
**********************************************************/
int Random(int a1, int a2);

#endif