
#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\MyInput\Input.h"


#include "i_menu.h"
#include "Menu.h"




int gnActive;
int gnItems;

MenuNode gsMenuList;
MenuNode gsItemList;

MC* pFocus;		//the item that has the focus



PUBLIC void MenuDraw () {

	MenuNode* pNode;


	pNode = gsMenuList.pPrev;

	while ( pNode != NULL ) {

		pNode->pItem->psFuncTable->MenuDrawFunc ( pNode->pItem, 0, 0 );
		pNode = pNode->pPrev;
	}
}

//initializes global menu variables to empty
PUBLIC RETCODE MenuInit () {

	gnActive = 0;
	memset ( &gsMenuList, 0, sizeof(gsMenuList) );

	return RETCODE_SUCCESS;
}


//frees global list of menu's
PUBLIC RETCODE MenuTerm ( int bAll ) {

	MenuNode* pNode, *pNext;

	gnActive = 0;
	pNode = gsMenuList.pNext;

	if ( bAll ) {

		while ( pNode != NULL ) {

			pNext = pNode->pNext;

			pNode->pItem->psFuncTable->MenuDestroyFunc ( pNode->pItem, bAll );
			MemFree ( (void**) &pNode );
			pNode = pNext;
		}

		pNode = gsItemList.pNext;

		while ( pNode != NULL ) {

			pNext = pNode->pNext;
			pNode->pItem->psFuncTable->MenuDestroyFunc ( pNode->pItem, 0 );
			pNode = pNext;
		}
	}

	else {

		while ( pNode != NULL ) {

			pNext = pNode->pNext;

			MemFree ( (void**) &pNode );
			pNode = pNext;
		}

		pNode = gsItemList.pNext;

		while ( pNode != NULL ) {

			pNext = pNode->pNext;
			pNode->pItem->psFuncTable->MenuDestroyFunc ( pNode->pItem, 0 );
			pNode = pNext;
		}
	}

	return RETCODE_SUCCESS;
}


void MenuCreateItem ( HMC hMc ) {

	MenuNode* pNode;
	
	MemAlloc ( (void**) &pNode, sizeof(*pNode), 0 );

	if ( gsItemList.pNext )
		gsItemList.pNext->pPrev = pNode;

	else
		gsItemList.pPrev = pNode;

	pNode->pItem = hMc;
	pNode->pNext = gsItemList.pNext;
	pNode->pPrev = NULL;
	gsItemList.pNext = pNode;

	gnItems++;
}


//when an object is destroyed, it calls this function so
// it can be removed from the global list
void MenuDestroyItem ( HMC hMc ) {

	MenuNode* pNode;

	pNode = gsItemList.pNext;

	while ( pNode != NULL ) {

		if ( pNode->pItem == hMc ) {

			if ( pNode->pPrev )
				pNode->pPrev->pNext = pNode->pNext;

			else
				gsItemList.pNext = pNode->pNext;

			if ( pNode->pNext )
				pNode->pNext->pPrev = pNode->pPrev;

			pNode->pItem->nId = -1;
			MemFree ( (void**) &pNode );
			gnItems--;
			break;
		}

		pNode = pNode->pNext;
	}
}

//searches through the list of objects for the menu item
// with the given ID.  returns NULL if the item cannot
// be found.
HMC MenuGetItem ( int nId ) {

	MenuNode* pNode;

	pNode = gsItemList.pNext;

	while ( pNode != NULL ) {

		if ( pNode->pItem->nId == nId )
			return pNode->pItem;

		pNode = pNode->pNext;
	}

	return NULL;
}


//sets the windows show value to true of false based on bShow
// if bAll is true, then all of the items children will also
// have there show value changed
PUBLIC void MenuShowItem ( HMC hMc, int bShow, int bAll ) {

	if ( bAll ) {

		hMc->psFuncTable->MenuShowFunc ( hMc, bShow, bAll );
	}

	else {
		hMc->nStatus = bShow;
	}
}


//move the item to the new x and y coordinates
PUBLIC void MenuChangePos ( HMC hMc, int newX, int newY ) {

	assert ( hMc );
	hMc->x = newX;
	hMc->y = newY;
}


//get the items x position
PUBLIC int MenuGetItemX ( HMC hMc ) {

	assert ( hMc );
	return hMc->x;
}


//get the items y position
PUBLIC int MenuGetItemY ( HMC hMc ) {

	assert ( hMc );
	return hMc->y;
}


//move the item by the specified amounts
PUBLIC void MenuMoveItem ( HMC hMc, int dX, int dY ) {

	assert ( hMc );
	hMc->x += dX;
	hMc->y += dY;
}


// returns the show status of the item
PUBLIC int MenuGetShow ( HMC hMc ) {

	return hMc->nStatus;
}


//tells the menu to draw the specified object and set coordinates
void MenuDrawItem ( HMC hMc, int x, int y ) {

	hMc->psFuncTable->MenuDrawFunc ( hMc, x, y );
}


//Create menu node and place at beginning of list with pointer
// to the menu object, if hMc is not NULL, nId is ignored, if
// hMc is NULL, the function attempts to find the object in
// an internal list
PUBLIC RETCODE MenuAddWindow ( HMC hMc, int nId ) {

	MenuNode* pNode;

	if ( hMc == NULL ) {
		
		hMc = MenuGetItem ( nId );

		//if the id could not be found, must exit
		if ( hMc == NULL ) return RETCODE_FAILURE;
	}
	
	MemAlloc ( (void**)&pNode, sizeof(*pNode), 0 );

	if ( gsMenuList.pNext )
		gsMenuList.pNext->pPrev = pNode;

	else
		gsMenuList.pPrev = pNode;

	pNode->pItem = hMc;
	pNode->pNext = gsMenuList.pNext;
	pNode->pPrev = NULL;
	gsMenuList.pNext = pNode;

	gnActive++;

	return RETCODE_SUCCESS;
}

PUBLIC RETCODE MenuAddChild ( int nParent, int nChild ) {

	HMC hParent, hChild;

	hChild = MenuGetItem ( nChild );
	hParent = MenuGetItem ( nParent );

	return hParent->psFuncTable->MenuAddChild ( hParent, hChild, 0 );
}

//remove indicated window
PUBLIC RETCODE MenuRemoveWindow ( HMC hMc ) {

	MenuNode* pNode;

	pNode = gsMenuList.pNext;

	while ( pNode != NULL ) {

		if ( pNode->pItem == hMc ) {

			if ( pNode->pItem == pFocus ) pFocus = 0;

			if ( pNode == gsMenuList.pPrev ) {
			
				gsMenuList.pPrev = pNode->pPrev;
			}

			if ( pNode->pPrev )
				pNode->pPrev->pNext = pNode->pNext;
			
			else
				gsMenuList.pNext = pNode->pNext;

			if ( pNode->pNext )
				pNode->pNext->pPrev = pNode->pPrev;
			
			MemFree ( (void**)&pNode );

			gnActive--;
			break;
		}

		pNode = pNode->pNext;
	}

	return RETCODE_SUCCESS;
}


PUBLIC RETCODE MenuProc ( ) {

	int x,y;
	int bHit = 0;
//	MC* pItem;
	MenuNode* pNode;

	static WPARAM wParam = 0;
	LOCATION sMouseLoc = InputMouseGetLoc ( );
	LOCATION sMouseMove = InputMouseGetMove ( );


	x = sMouseLoc.x;
	y = sMouseLoc.y;


	/*
MK_CONTROL Set if the ctrl key is down. 
MK_LBUTTON Set if the left mouse button is down. 
MK_MBUTTON Set if the middle mouse button is down. 
MK_RBUTTON Set if the right mouse button is down. 
MK_SHIFT Set if the shift key is down. 
/*/

	pNode = gsMenuList.pNext;

	wParam = wParam & (~128);

	if ( sMouseMove.x !=0 || sMouseMove.y != 0 ) {

		while ( pNode != NULL ) {
			
			if ( pNode->pItem->psFuncTable->MenuMouseProcFunc ( pNode->pItem, WM_MOUSEMOVE, x, y, wParam ) && !bHit) {

				bHit = 1;
//				pFocus = pNode->pItem;
				wParam = wParam | 128;
			}
			pNode = pNode->pNext;
		}
	}

	if ( InputMouseIsBtnDwn ( eLeftButton ) ) {
		
		if ( !(wParam & MK_LBUTTON) ) {

			wParam  = wParam ^ MK_LBUTTON;
			wParam = wParam & (~128);

			pNode = gsMenuList.pNext;
			bHit = 0;

			while ( pNode != NULL ) {
				
				if (pNode->pItem->psFuncTable->MenuMouseProcFunc ( pNode->pItem, WM_LBUTTONDOWN, x, y, wParam ) && !bHit) {
				
					bHit = 1;
					pFocus = pNode->pItem;
					wParam = wParam | 128;
				}

				pNode = pNode->pNext;
			}

		}
	}

	else if ( wParam & MK_LBUTTON ) {

		wParam = wParam ^ MK_LBUTTON;
		wParam = wParam & (~128);

		pNode = gsMenuList.pNext;

		while ( pNode != NULL ) {
			
			if ( pNode->pItem->psFuncTable->MenuMouseProcFunc ( pNode->pItem, WM_LBUTTONUP, x, y, wParam ) && !bHit ) {
			
				bHit = 1;
				wParam = wParam | 128;
			}
			pNode = pNode->pNext;
		}
	}

	if ( InputKbIsPressed ( VK_SHIFT ) ) {

		wParam = wParam | MK_SHIFT;
	}

	if ( InputKbIsReleased ( VK_SHIFT ) ) {

		wParam = wParam & (~MK_SHIFT);
	}

	if ( pFocus && InputKbAnyKeyPressed() ) {

		for ( int x=5; x<kMaxKeycodes; x++ ) {

			if ( InputKbIsPressed ( x ) ) {
				
				char c = InputKbGetAscii ( x, 0 );

				if ( wParam & MK_SHIFT )
					c = toupper ( c );

				pFocus->psFuncTable->MenuKeyboardFunc ( pFocus, WM_KEYDOWN, (LPARAM)c );
			}
		}
	}

	if ( pFocus && InputKbAnyKeyReleased () ) {

		for ( int x=5; x<kMaxKeycodes; x++ ) {

			if ( InputKbIsReleased ( x ) ) {

				char c = InputKbGetAscii ( x, 0 );

				pFocus->psFuncTable->MenuKeyboardFunc ( pFocus, WM_KEYUP, (LPARAM)c );
			}
		}
	}

	return RETCODE_FAILURE;
}
