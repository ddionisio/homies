#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GDIX.h"

#include "i_menu.h"
#include "Window.h"

struct _WINDOW {

	MC mc;
	int nChildren;
	MenuNode sChildList;
	WINDOWPROC WindowProc;
	hTXT hTxt;
	MC* pFocus;
};



void WindowDraw ( HMC hMc, int x, int y ) {

	if ( hMc->nStatus == 0 ) return;

	HWINDOW hWindow = (HWINDOW) hMc;

	MenuNode* pNode;

	RECT sRect = {hWindow->mc.x + x, hWindow->mc.y + y,
		x + hWindow->mc.x + hWindow->mc.nWidth, y + hWindow->mc.y + hWindow->mc.nHeight};

	if ( hWindow->mc.nStatus && hWindow->hTxt)
		TextureStretchBlt ( hWindow->hTxt, sRect.left, sRect.top, hWindow->mc.nWidth, hWindow->mc.nHeight, NULL, NULL, 0 );

	pNode = hWindow->sChildList.pPrev;

	while ( pNode != NULL ) {

		pNode->pItem->psFuncTable->MenuDrawFunc ( pNode->pItem, sRect.left, sRect.top );

		pNode  = pNode->pPrev;
	}
}


int WindowMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

	MenuNode* pNode;
	HWINDOW hWindow = (HWINDOW) hMc;

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	x -= hWindow->mc.x;
	y -= hWindow->mc.y;

	int ret = 0;


	pNode = hWindow->sChildList.pNext;

	while ( pNode != NULL ) {

		if ( pNode->pItem->psFuncTable->MenuMouseProcFunc ( pNode->pItem, msg, x, y, wParam ) ) {
			
			hWindow->WindowProc ( hWindow->mc.nId, msg, wParam + (pNode->pItem->nId<<16), x + (y<<16) );

			if ( msg == WM_LBUTTONDOWN )
				hWindow->pFocus = pNode->pItem;
			wParam |= 128;
			ret = 1;
		}

		pNode = pNode->pNext;
	}

	if ( hWindow->mc.nStatus && !ret ) {

		if ( msg == WM_LBUTTONDOWN )
			hWindow->pFocus = 0;

		if ( x >= 0 && x <= hWindow->mc.nWidth ) {

			if ( y >= 0 && y <= hWindow->mc.nHeight ) {

				hWindow->WindowProc ( hWindow->mc.nId, msg, wParam + (hWindow->mc.nId<<16), x + ( y<<16) );
				ret = 1;
			}
		}
	}

	return ret;
}



int WindowKeyboard (HMC hMc, UINT msg, LPARAM lParam ) {

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	HWINDOW hWin = (HWINDOW) hMc;

	if ( hWin->pFocus == 0 ) return 0;

	return hWin->pFocus->psFuncTable->MenuKeyboardFunc ( hWin->pFocus, msg, lParam );
}


void WindowShow ( HMC hMc, int bShow, int bAll ) {

	hMc->nStatus = bShow;

	if ( bAll ) {

		HWINDOW hWindow = (HWINDOW) hMc;
		MenuNode* pNode;

		pNode = hWindow->sChildList.pNext;
		
		while ( pNode != NULL ) {

			pNode->pItem->psFuncTable->MenuShowFunc ( pNode->pItem, bShow, bAll );
			pNode = pNode->pNext;
		}
	}
}

_MenuFuncTable gsWindowFuncTable = {WindowDraw, WindowMouseProc, WindowKeyboard, WindowDestroy, WindowShow, WindowAddChild};

int WindowGetStructSize () { 
	return sizeof ( struct _WINDOW );
}


PUBLIC HWINDOW WindowCreate ( int nId, WINDOWPROC WinProc, int nBitmap, int nX, int nY, int nWidth, int nHeight ) {

	HWINDOW hWindow;

	MemAlloc ( (void**)&hWindow, sizeof(*hWindow), 0);

	hWindow->mc.x = nX;
	hWindow->mc.y = nY;
	hWindow->mc.nId = nId;
	hWindow->mc.nWidth = nWidth;
	hWindow->mc.nHeight = nHeight;

	hWindow->pFocus = 0;

	hWindow->nChildren = 0;
	hWindow->sChildList.pItem = NULL;
	hWindow->sChildList.pNext = NULL;
	hWindow->sChildList.pPrev = NULL;

	hWindow->WindowProc = WinProc;
	hWindow->mc.psFuncTable = &gsWindowFuncTable;

	if(nBitmap >= 0)
	{
		hWindow->hTxt = TextureSearch ( nBitmap );
		TextureAddRef ( hWindow->hTxt );
	}
	else
		hWindow->hTxt = 0;

	MenuCreateItem ( (HMC) hWindow );

	return hWindow;
}


PUBLIC void WindowDestroy ( HMC hMc, int bAll ) {

	HWINDOW hWindow = ( HWINDOW ) hMc;

	MenuNode* pNode, *pNext;

	hWindow->nChildren = 0;
	pNode = hWindow->sChildList.pNext;

	if ( bAll ) {

		while ( pNode != NULL ) {

//			MenuDestroyItem ( pNode->pItem );
			pNext = pNode->pNext;

			pNode->pItem->psFuncTable->MenuDestroyFunc ( pNode->pItem, bAll );
			MemFree ( (void**) &pNode );

			pNode = pNext;
		}
	}

	else {

		while ( pNode != NULL ) {

			pNext = pNode->pNext;

			MemFree ( (void**) &pNode );

			pNode = pNext;
		}
	}

//	TextureDestroy ( &hWindow->hTxt );
	MenuDestroyItem ( (HMC) hWindow );
	MemFree ( (void**)&hWindow );

}

int DefaultWinProc ( int nWinId, UINT msg, WPARAM wParam, LPARAM lParam ) {

	return 0;
}


HMC WindowCreateFromFile ( FILE* pFile ) {

	int nId, nX, nY, nWidth, nHeight, nBitmap, nShow;

	fscanf ( pFile, "%i %i %i %i %i %i %i\n", &nId, &nX, &nY, &nWidth, &nHeight, &nBitmap, &nShow );

	HMC hMc = (HMC) WindowCreate ( nId, DefaultWinProc, nBitmap, nX, nY, nWidth, nHeight );

	WindowShow ( hMc, nShow, 0);

	return hMc;
}


//attempts to add the given object to the given window, if the handle
// is not NULL, nId is ignored, if the handle is NULL, the function
// attempts to find the object associated with the id, if it cannot
// find the object, the function will fail
int WindowAddChild ( HMC hParent, HMC hChild, int nId) {

	HWINDOW hWindow = (HWINDOW) hParent;

	MenuNode* pNode;

	if ( hChild == NULL ) {

		hChild = MenuGetItem ( nId );
		if ( hChild == NULL ) return RETCODE_FAILURE;
	}

	MemAlloc ( (void**)&pNode, sizeof(*pNode), 0 );

	if ( hWindow->sChildList.pNext )
		hWindow->sChildList.pNext->pPrev = pNode;

	else
		hWindow->sChildList.pPrev = pNode;

	pNode->pItem = hChild;
	pNode->pNext = hWindow->sChildList.pNext;
	pNode->pPrev = NULL;
	hWindow->sChildList.pNext = pNode;

	hChild->psParent = hParent;
	hWindow->nChildren++;
	return RETCODE_SUCCESS;
}

PUBLIC RETCODE WindowRemoveChild ( HWINDOW hWindow, HMC hMc ) {

	MenuNode* pNode;

	pNode = hWindow->sChildList.pNext;

	while ( pNode != NULL ) {

		if ( pNode->pItem == hMc ) {

			hMc->psParent = NULL;

			if ( pNode->pItem == hWindow->pFocus ) hWindow->pFocus = 0;

			if ( pNode->pPrev )
				pNode->pPrev->pNext = pNode->pNext;

			else
				hWindow->sChildList.pNext = pNode->pNext;

			if ( pNode->pNext )
				pNode->pNext->pPrev = pNode->pPrev;

			MemFree ( (void**)&pNode );

			hWindow->nChildren--;
			break;
		}

		pNode = pNode->pNext;
	}

	return RETCODE_SUCCESS;
}


//////////////////////////////
// Give the window a new proc
//////////////////////////////
PUBLIC WINDOWPROC WindowNewProc ( HWINDOW hWindow, WINDOWPROC WindowProc ) {

	WINDOWPROC old = hWindow->WindowProc;
	hWindow->WindowProc = WindowProc;

	return old;
}


//move the item to the new x and y coordinates
PUBLIC void WindowChangeSize ( HWINDOW hWindow, int newWidth, int newHeight ) {

	hWindow->mc.nWidth = newWidth;
	hWindow->mc.nHeight = newHeight;
}


//get the items x position
PUBLIC int WindowGetItemWidth ( HWINDOW hWindow ) {

	return hWindow->mc.nWidth;
}

//get the items y position
PUBLIC int WindowGetItemHeight ( HWINDOW hWindow ) {

	return hWindow->mc.nHeight;
}


//change the item's size by the specified amounts
PUBLIC void WindowSizeItem ( HWINDOW hWindow, int dWidth, int dHeight ) {

	hWindow->mc.nWidth += dWidth;
	hWindow->mc.nHeight += dHeight;
}