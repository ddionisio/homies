#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"

#include "i_menu.h"
#include "Window.h"
#include "Button.h"

#include "ScrollBar.h"

#define SCROLLBAR_MIN_HEIGHT	8

struct _ScrollBar {

	MC mc;
	int nSliderPos;		//the slider pos in the y
	int nSliderHeight;
	int nButtonHeight;  //height of the various stuff
	int nWindowHeight;

	int nVisibleItems;	//the number of items visible at once
	int nMaxItems;		//the total number of items
	int nTopItem;		//the item that is at the top right now

	int nLastPos;		//this is the last position of the mouse
						//when it is dragging the slider, it is
						//set to -1 when it isn't being slid

	SCROLLBARFUNC pSbf;	//function called when scroll bar is used

	HWINDOW hWin;
};


extern struct _MenuFuncTable gsWindowFuncTable;


//when the scroll bar draws, it only need to tell its
// window to draw
void ScrollBarDrawFunc ( HMC hMc, int x, int y ) {

	if ( hMc->nStatus == HMC_INVIS ) return;

	HSCROLLBAR hScroll = (HSCROLLBAR) hMc;

	gsWindowFuncTable.MenuDrawFunc ( (HMC) hScroll->hWin, x + hMc->x, y + hMc->y );
}


int ScrollBarMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	int ret = 0;
	HSCROLLBAR hScroll = (HSCROLLBAR) hMc;

	x -= hMc->x;
	y -= hMc->y;

	
	//if the mouse button was released, dont process
	//mouse move messages anymore
	if ( msg == WM_LBUTTONUP ) hScroll->nLastPos = -1;

	//if the message has already been processed, dont worry about it
	if ( wParam & 128 ) return 0;

	//just let the window process messages
	ret = gsWindowFuncTable.MenuMouseProcFunc ( (HMC) hScroll->hWin, msg, x, y, wParam );

	//if the scroll bar didn't get any input, make sure it knows that
	if ( ret == 0 && msg == WM_LBUTTONDOWN ) {

		hScroll->nLastPos = -1;
	}

	return ret;
}

//the scroll bar doesn't take much keyboard input
int ScrollBarKeyboard ( HMC hMc, UINT msg, LPARAM lParam ) {

	assert ( hMc );

	if ( hMc->nStatus == 0 ) return 0;

	return 0;
};


//the scroll bar doesn't have children
int ScrollBarAddChild ( HMC hParent, HMC hChild, int nId ) {

	assert ( 0 );

	return 0;
}

void ScrollBarShow ( HMC hMc, int bShow, int bAll ) {

	assert ( hMc );

	hMc->nStatus = bShow;

	if ( bAll ) {

		HSCROLLBAR hScroll = (HSCROLLBAR) hMc;

		gsWindowFuncTable.MenuShowFunc ( (HMC) hScroll->hWin, bShow, bAll );
	}
}

void ScrollBarDestroy ( HMC hMc, int bAll ) {

	gsWindowFuncTable.MenuDestroyFunc ( (HMC)((HSCROLLBAR)hMc)->hWin, 1 );
	MenuDestroyItem ( hMc );

	MemFree ( (void**) &hMc );

}


struct _MenuFuncTable gsScrollBarFuncTable = { ScrollBarDrawFunc, ScrollBarMouseProc, ScrollBarKeyboard, ScrollBarDestroy, ScrollBarShow, ScrollBarAddChild };


//this function is handling all the messages that the scroll bar gets when a user
//starts interacting with it
int ScrollBarWindowFunc ( int nWinId, UINT msg, WPARAM wParam, LPARAM lParam ) {

	int nId = HIWORD ( wParam );

	HSCROLLBAR hScroll = (HSCROLLBAR) MenuGetItem ( nWinId-1 );

	if ( hScroll->nVisibleItems >= hScroll->nMaxItems ) return 0;

	int nOldPos = hScroll->nSliderPos;

	assert ( hScroll->pSbf );


	//if the user wants to push the slider around
	if ( msg == WM_MOUSEMOVE  &&  wParam & MK_LBUTTON ) {

		//and they are clicking on the sliding area
		if ( nId == nWinId || nId == nWinId + 3 ) {
			
			//and the slider is able to move
			if ( hScroll->nLastPos != -1 ) {

				int y = HIWORD ( lParam );

				//get the new position
				hScroll->nSliderPos += y - hScroll->nLastPos;

				//bounds checking
				if ( hScroll->nSliderPos < 0 ) 
					hScroll->nSliderPos = 0;

				else 
					if ( hScroll->nSliderPos > hScroll->nWindowHeight - hScroll->nSliderHeight )
						hScroll->nSliderPos = hScroll->nWindowHeight - hScroll->nSliderHeight;

				//update the buttons position
				HMC hBut = MenuGetItem ( nWinId + 3 );
				hBut->y = hScroll->nSliderPos;
				hScroll->nLastPos = y;

				//get the new top item displayed
				hScroll->nTopItem = hScroll->nSliderPos * ( hScroll->nMaxItems - hScroll->nVisibleItems ) / (hScroll->nWindowHeight - hScroll->nSliderHeight );

				//call the parent, inform of new position
				hScroll->pSbf ( (HMC) hScroll->mc.psParent, hScroll, hScroll->nTopItem );
			}
		}
	}

	//if they pressed duh buttons
	else if ( msg == WM_LBUTTONDOWN ) {
		
		hScroll->nLastPos = -1;
		if ( nId == nWinId + 3 ) {

			hScroll->nLastPos = HIWORD ( lParam );
			return 1;
		}

		//if it was the bar itself that was clicked
		else if ( nId == nWinId ) {

			int y = HIWORD ( lParam );
			
			//if it was about the slider
			if ( y < hScroll->nSliderPos ) {

				//move the slider up half a page
				hScroll->nTopItem -= hScroll->nVisibleItems/2;

				if ( hScroll->nTopItem < 0 )
					hScroll->nTopItem = 0;
			}

			//if it was below the slider
			else {

				//move the slider down a page
				hScroll->nTopItem += hScroll->nVisibleItems/2;

				if ( hScroll->nTopItem + hScroll->nVisibleItems > hScroll->nMaxItems )
					hScroll->nTopItem = hScroll->nMaxItems - hScroll->nVisibleItems;
			}

			hScroll->pSbf ( (HMC) hScroll->mc.psParent, hScroll, hScroll->nTopItem );
		}

		//if it was the bottom button pressed
		else if ( nId == nWinId + 2 ) {

			if ( hScroll->nTopItem + hScroll->nVisibleItems < hScroll->nMaxItems ) {

				hScroll->nTopItem++;
				hScroll->pSbf ( (HMC) hScroll->mc.psParent, hScroll, hScroll->nTopItem );
			}
		}

		//if it was the top button pressed
		else if ( nId == nWinId + 1 ) {

			if ( hScroll->nTopItem > 0 )

				hScroll->nTopItem--;
				hScroll->pSbf ( (HMC) hScroll->mc.psParent, hScroll, hScroll->nTopItem );
		}

		hScroll->nSliderPos = ( hScroll->nWindowHeight - hScroll->nSliderHeight ) * (hScroll->nTopItem ) / ( hScroll->nMaxItems - hScroll->nVisibleItems );

		//if the slider should move
		if ( hScroll->nSliderPos != nOldPos ) {
			
			//get the new position, and set the slider to be there
			HMC hBut = MenuGetItem ( nWinId + 3 );
			hBut->y = hScroll->nSliderPos;
		}

	}
	return 0;
}


//associate the scroll bar with a parent and its function
void ScrollBarAssociate ( HSCROLLBAR hScroll, HMC hParent, SCROLLBARFUNC pSbf ) {

	assert ( pSbf );
	assert ( hScroll );
	assert ( hParent );

	hScroll->mc.psParent = hParent;
	hScroll->pSbf = pSbf;
}


//sets the scroll bar to react with the new data
void ScrollBarSetInfo ( HSCROLLBAR hScroll, int nMaxItems, int nVisibleItems ) {

	assert ( hScroll );
	assert ( nVisibleItems > 0 );

	hScroll->nMaxItems = nMaxItems;
	hScroll->nVisibleItems = nVisibleItems;

	//make sure there are items to scroll around
	if ( nMaxItems > 0 ) {

		hScroll->nSliderHeight = hScroll->nWindowHeight * hScroll->nVisibleItems / hScroll->nMaxItems;
		
		if ( hScroll->nSliderHeight > hScroll->nWindowHeight )
			hScroll->nSliderHeight = hScroll->nWindowHeight;

		if ( hScroll->nSliderHeight < SCROLLBAR_MIN_HEIGHT )
			hScroll->nSliderHeight = SCROLLBAR_MIN_HEIGHT;
	}

	else {

		//if there is nothing, the scroll bar takes up the whole window
		hScroll->nSliderHeight = hScroll->nWindowHeight;
	}

	HMC hBut = MenuGetItem ( hScroll->mc.nId + 4 );
	hBut->nHeight = hScroll->nSliderHeight;
}


//give the scroll bar at new start position
void ScrollBarSetTop ( HSCROLLBAR hScroll, int nTop ) {

	assert ( hScroll );

	if ( nTop > hScroll->nMaxItems - hScroll->nVisibleItems )
		nTop = hScroll->nMaxItems - hScroll->nVisibleItems;

	if ( nTop < 0 )
		nTop = 0;

	//if the slider should move
	if ( hScroll->nTopItem != nTop ) {
		
		hScroll->nTopItem = nTop;
		//get the new position, and set the slider to be there
		hScroll->nSliderPos = ( hScroll->nWindowHeight - hScroll->nSliderHeight ) * (hScroll->nTopItem ) / ( hScroll->nMaxItems - hScroll->nVisibleItems );
		HMC hBut = MenuGetItem ( hScroll->mc.nId + 4 );
		hBut->y = hScroll->nSliderPos;
	}
}


//create that funky scroll bar white boy
//create that funky scroll bar right
HSCROLLBAR ScrollBarCreate ( int nId, int nX, int nY, int nWidth, int nHeight, int nButtonHeight, int nBitmap ) {

	HSCROLLBAR hScroll;

	if ( MemAlloc ( (void**) &hScroll, sizeof ( *hScroll ), 0 ) == RETCODE_FAILURE ) {

		return 0;
	}

	//fill out the standard info
	hScroll->mc.nId = nId;
	hScroll->mc.x = nX;
	hScroll->mc.y = nY;
	hScroll->mc.nWidth = nWidth;
	hScroll->mc.nHeight = nHeight;
	hScroll->mc.psFuncTable = &gsScrollBarFuncTable;

	//scroll bar specific info
	hScroll->nTopItem = 0;
	hScroll->nMaxItems = 0;
	hScroll->nSliderPos = 0;
	hScroll->nVisibleItems = 1;
	hScroll->nButtonHeight = nButtonHeight;
	hScroll->nWindowHeight = nHeight - ( 2 * nButtonHeight );
	hScroll->nSliderHeight = hScroll->nWindowHeight;

	//create the window
	hScroll->hWin = WindowCreate ( nId + 1, ScrollBarWindowFunc, nBitmap, 0, nButtonHeight, nWidth, hScroll->nWindowHeight );

	MenuCreateItem ( (HMC) hScroll );

	return hScroll;
}



ScrollBarAddButton ( HSCROLLBAR hScroll, HMC hBut ) {

	return gsWindowFuncTable.MenuAddChild ( (HMC) hScroll->hWin, hBut, 0 );
}


HMC ScrollBarCreateFromFile ( FILE* pFile ) {

	HSCROLLBAR hScroll;

	int nId, nX, nY, nWidth, nHeight, nButtonHeight, nShow, nBitmap;
	char pText[180];


	//read all the general info for the scroll bar
	fscanf ( pFile, "%i %i %i %i %i %i %i %i\n", &nId, &nX, &nY, &nWidth, &nHeight, &nButtonHeight, &nBitmap, &nShow );

	//create the main scroll bar window
	hScroll = ScrollBarCreate ( nId, nX, nY, nWidth, nHeight, nButtonHeight, nBitmap );

	if ( !hScroll ) return 0;


	//read info for the top button
	fscanf ( pFile, "%i %[^\n]\n", &nBitmap, pText );

	//id + 2 = top button
	HBUTTON hBut = ButtonCreate ( nId + 2, pText, 0, -nButtonHeight, nWidth, nButtonHeight, nBitmap );
	ScrollBarAddButton ( hScroll, (HMC) hBut );


	//get all the info for the bottom button now
	fscanf ( pFile, "%i %[^\n]\n", &nBitmap, pText );

	//create the next button, id+3 = bottom button
	hBut = ButtonCreate ( nId +3, pText, 0, nHeight - ( 2 * nButtonHeight), nWidth, nButtonHeight, nBitmap );
	ScrollBarAddButton ( hScroll, (HMC) hBut );


	//get all the info for the Slider now
	fscanf ( pFile, "%i %[^\n]\n", &nBitmap, pText );

	//create the next button, id+3 = bottom button
	hBut = ButtonCreate ( nId + 4, pText, 0, 0, nWidth, hScroll->nSliderHeight, nBitmap );
	ScrollBarAddButton ( hScroll, (HMC) hBut );

	ScrollBarShow ( (HMC) hScroll, nShow, 1 );

	return (HMC) hScroll;
}
