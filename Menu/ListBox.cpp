#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"

#include "i_menu.h"
#include "Fonts.h"

#include "Window.h"
#include "TextBox.h"
#include "ScrollBar.h"

#include "ListBox.h"

#define _ABOVE_LIST		-1
#define _BELOW_LIST		-10

struct _ListBox {

	MC mc;
	hTXT hTxt;
	MenuNode sTextList;
	HMC hScroll;
	
	int nFont;
	int nColor, nHighColor, nHighlight;
	int nTextHeight, nXOffset, nYOffset;
	int nMaxItems, nTopItem, nViewable, nNextId;
};


extern struct _MenuFuncTable gsWindowFuncTable;
//extern struct _MenuFuncTable gsTextBoxFuncTable;

int ListBoxWindowProc ( UINT msg, WPARAM wParam, LPARAM lParam ) {

	return 0;
}


//draws the background of the box, then the text and the scroll bar
void ListBoxDraw ( HMC hMc, int x, int y ) {

	if ( hMc->nStatus == HMC_INVIS ) return;

	HLISTBOX hListBox = (HLISTBOX) hMc;
	x += hListBox->mc.x;
	y += hListBox->mc.y;

	//draw the box itself
	TextureStretchBlt ( hListBox->hTxt, x, y, hListBox->mc.nWidth, hListBox->mc.nHeight, NULL, NULL, 0 );

	MenuNode* pNode = hListBox->sTextList.pNext;

	while ( pNode ) {

		if ( pNode->pItem->y != _ABOVE_LIST ) {

			if ( pNode->pItem->y == _BELOW_LIST ) {

				break;
			}

			pNode->pItem->psFuncTable->MenuDrawFunc ( pNode->pItem, x, y );
		}

		pNode = pNode->pNext;
	}

	if ( hListBox->hScroll != NULL ) {

		hListBox->hScroll->psFuncTable->MenuDrawFunc ( hListBox->hScroll, x, y );
	}
}


//the List box doesn't really process mouse messages
int ListBoxMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

	int yhit;
	MenuNode* pNode;

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	//change the position to reflect the position of the listbox
	x -= hMc->x;
	y -= hMc->y;

	HLISTBOX hList = (HLISTBOX) hMc;

	//if there is a scroll bar, make sure it processes info as well
	if ( hList->hScroll ) {

		if ( hList->hScroll->psFuncTable->MenuMouseProcFunc ( hList->hScroll, msg, x, y, wParam ) ) {

			wParam &= 128;
		}
	}

	if ( wParam & 128 ) return 0;

	//only respond to button hits
	if ( msg != WM_LBUTTONDOWN ) return 0;
	
	//bounds check
	if ( x < hList->nXOffset || x > hMc->nWidth ) return 0;

	//get the location of the hit in the y
	yhit = ( y - hList->nYOffset ) / hList->nTextHeight;

	//if its not on the list, exit
	if ( yhit < 0 || yhit > hList->nViewable ) return 0;

	//move yhit down as far as the list is
	yhit += hList->nTopItem;

	x = 0;	// x is being used to count the amount of visible nodes
	y = 0;	// y is used to store the the new id of the highlighted text

	pNode = hList->sTextList.pNext;

	while ( pNode ) {

		//if it is the formerly highlighted one
		if ( x == hList->nHighlight ) {

			//make it normal again
			TextBoxChangeTextColor ( (HTEXTBOX)pNode->pItem, hList->nColor );
		}

		if ( x == yhit ) {

			//if it is the clicked one, change its color
			TextBoxChangeTextColor ( (HTEXTBOX)pNode->pItem, hList->nHighColor );
			y = x;
		}

		x++;
		pNode = pNode->pNext;
	}

	hList->nHighlight = y;

	wParam &= 128;
	return 1;
}


//the listbox doesn't take any keyboard input
int ListBoxKeyboard (HMC hMc, UINT msg, LPARAM lParam ) {

	if ( hMc->nStatus == 0 ) return 0;

	return 0;
}


//Listboxes don't have children
int ListBoxAddChild ( HMC hParent, HMC hChild, int nId ) {

	return 0;
}

//gives the Listbox the new value
void ListBoxShow ( HMC hMc, int bShow, int bAll ) {

	HLISTBOX hList = (HLISTBOX) hMc;
	MenuNode* pNode = hList->sTextList.pNext;

	hMc->nStatus = bShow;

	if ( hList->hScroll )
		(hList->hScroll)->psFuncTable->MenuShowFunc ( hList->hScroll, bShow, 1 );

	while ( pNode ) {

		pNode->pItem->nStatus = bShow;
		pNode = pNode->pNext;
	}
}


//destroy the list box and all its stuff
void ListBoxDestroy ( HMC hMc, int bAll ) {

	assert ( hMc );

	MenuNode* pNode, *pNext;
	HLISTBOX hList = (HLISTBOX) hMc;

	MenuDestroyItem ( hMc );
//	MemFree ( (void**) &hList->pFont );

	if ( hList->hScroll )
		((HMC)hList->hScroll)->psFuncTable->MenuDestroyFunc ( (HMC) hList->hScroll, 1 );

	pNode = hList->sTextList.pNext;

	while ( pNode ) {

		pNext = pNode->pNext;

		pNode->pItem->psFuncTable->MenuDestroyFunc ( pNode->pItem, 1 );

		MemFree ( (void**) &pNode );

		pNode = pNext;
	}

	MemFree ( (void**) &hMc );
}


//sets the new top of the list box
void ListBoxSetTop ( HLISTBOX hList, int nPos ) {

	int x;
	MenuNode* pNode;

	assert ( hList );

	if ( hList->nMaxItems == 0 ) return;

	if ( nPos > hList->nMaxItems - hList->nViewable ) {
		
		nPos  = hList->nMaxItems - hList->nViewable;
	}

	if ( nPos < 0 ) nPos = 0;

	hList->nTopItem = nPos;

	x = 0;
	pNode = hList->sTextList.pNext;

	//while before the viewable and their is still items in the list
	while ( x < nPos && pNode ) {

		pNode->pItem->y = _ABOVE_LIST;
		pNode = pNode->pNext;
		x++;
	}

	x = 0;

	while ( pNode && x < hList->nViewable ) {

		pNode->pItem->y = x * hList->nTextHeight + hList->nYOffset;
		pNode = pNode->pNext;
		x++;
	}

	while ( pNode && pNode->pItem->y != _BELOW_LIST ) {

		pNode->pItem->y = _BELOW_LIST;
		pNode = pNode->pNext;
	}
}


//returns the 0 based index of the selected item
//returns -1 if no item is selected
int ListBoxGetSelected ( HLISTBOX hList ) {

	return hList->nHighlight;
}


//remove an item from the list box by 0 based index
//if the index doesn't exist, the function fails
int ListBoxDeleteItem ( HLISTBOX hList, int nPos ) {

	int x;
	MenuNode* pNode;

	assert ( hList );

	//if the index isn't viable, fail
	if ( nPos > hList->nMaxItems || nPos < 0 ) return 0;

	x = 0;
	pNode = hList->sTextList.pNext;

	//go to the nPos node
	while ( pNode && x != nPos ) {

		pNode = pNode->pNext;
		x++;
	}

	//set the next node to have the previous node
	if ( pNode->pNext ) {

		pNode->pNext->pPrev = pNode->pPrev;
	}

	else {
		
		hList->sTextList.pPrev = pNode->pPrev;
	}

	//sets the previous node to point at the next
	if ( pNode->pPrev ) {

		pNode->pPrev->pNext = pNode->pNext;
	}

	else {

		hList->sTextList.pNext = pNode->pNext;
	}

	//destroy the item
	pNode->pItem->psFuncTable->MenuDestroyFunc ( pNode->pItem, 1 );

	MemFree ( (void**) &pNode );

	hList->nMaxItems--;

	//if its the highlighted one that is deleted,
	//make it so nothing is highlightd
	if ( nPos == hList->nHighlight ) {

		hList->nHighlight = -1;
	}

	ListBoxSetTop ( hList, hList->nTopItem );

	return 1;
}


//removes all the items from the list box
void ListBoxClear ( HLISTBOX hList ) {

	MenuNode* pNode, *pNext;

	pNode = hList->sTextList.pNext;

	while ( pNode ) {

		pNext = pNode->pNext;

		pNode->pItem->psFuncTable->MenuDestroyFunc ( pNode->pItem, 1 );

		MemFree ( (void**) &pNode );

		pNode = pNext;
	}

	hList->nMaxItems = 0;
	hList->nTopItem = 0;

	hList->sTextList.pItem = 0;
	hList->sTextList.pPrev = 0;
	hList->sTextList.pNext = 0;

	if ( hList->hScroll )
		ScrollBarSetInfo ( (HSCROLLBAR)hList->hScroll, 0, 1 );
}


//adds another line of text to the list box
int ListBoxAddItem ( HLISTBOX hList, char* pText, int nPos ) {

	HMC hText;
	MenuNode* pNode;

	if ( nPos > 0  || nPos < -1 ) nPos = -1;

	//create the text for the line
	hText = (HMC) TextBoxCreate ( hList->nNextId++, hList->nXOffset, ++hList->nMaxItems, hList->mc.nWidth, hList->nTextHeight, pText, hList->nColor, hList->nFont );

	assert ( hText );
	if ( hText == 0 ) return 0;


	MemAlloc ( (void**) &pNode, sizeof (*pNode), 0 );

	//if you are adding to the beginnning of the list
	if ( nPos == 0 ) {

		pNode->pPrev = 0;
		pNode->pItem = (HMC) hText;
		pNode->pNext = hList->sTextList.pNext;
		hList->sTextList.pNext = pNode;

		if ( pNode->pNext ) {

			pNode->pNext->pPrev = pNode;
		}

		//if it is also the end of the list, ie first item in it
		if ( hList->sTextList.pPrev == 0 ) {

			hList->sTextList.pPrev = pNode;
		}

		//if the item won't be displayed anyway
		if ( hList->nTopItem > 0 ) {

			pNode->pItem->y = _ABOVE_LIST;
		}

		//if you need to move everything because the new item
		//is going to be displayed
		else {

			ListBoxSetTop ( hList, 0 );
		}
	}
	
	//adding the item to the end
	else if ( nPos == -1 ) {

		pNode->pNext = 0;
		pNode->pItem = hText;
		pNode->pPrev = hList->sTextList.pPrev;
		hList->sTextList.pPrev = pNode;

		if ( pNode->pPrev ) {

			int nHeight;

			pNode->pPrev->pNext = pNode;

			//get the next height
			nHeight = pNode->pPrev->pItem->y + hList->nTextHeight;

			//if it is below the list, mark it that way
			if ( nHeight + hList->nTextHeight > hList->mc.nHeight || pNode->pPrev->pItem->y == _BELOW_LIST )
				nHeight = _BELOW_LIST;

			hText->y = nHeight;
		}

		else {

			//it is the first item in the list, must be displayed
			hText->y = hList->nYOffset;
		}

		if ( hList->sTextList.pNext == 0 ) {

			hList->sTextList.pNext = pNode;
		}
	}

	//as of right now, there is no adding to the middle of the list
	else {

		//HAHAHAHAHAHAHA!!!!1
		assert ( 0 );
	}

	ScrollBarSetInfo ( (HSCROLLBAR) hList->hScroll, hList->nMaxItems, hList->nViewable );
	return 1;
}


void ListBoxScroll ( HMC hParent, HSCROLLBAR hScroll, int nPos ) {

	HLISTBOX hList = (HLISTBOX) hParent;

	ListBoxSetTop ( hList, nPos );
}

//change the offsets for the text displayed in the list box
PUBLIC void ListBoxSetOffsets ( HLISTBOX hList, int nXOffset, int nYOffset, int nTextHeight ) {

	assert ( hList );

	//make sure it isn't to far to the right
	if ( nXOffset < hList->mc.nWidth )
		hList->nXOffset = nXOffset;

	//make sure at least one item will be visible
	if ( nYOffset + nTextHeight < hList->mc.nHeight ) {

		hList->nYOffset = nYOffset;
		hList->nTextHeight = nTextHeight;

		hList->nViewable = (hList->mc.nHeight-nYOffset) / nTextHeight;
	}
}


_MenuFuncTable gsListBoxFuncTable = {ListBoxDraw, ListBoxMouseProc, ListBoxKeyboard, ListBoxDestroy, ListBoxShow, ListBoxAddChild};


//creates an empty list box with no scroll bar or items
HLISTBOX ListBoxCreate ( int nId, int nX, int nY, int nWidth, int nHeight, int nBitmap, int nScrollBar, int nFont, int nColor, int nHighColor ) {

	HLISTBOX hList;

	MemAlloc ( (void**) &hList, sizeof ( *hList ), 0 );

	assert ( hList );
	if ( hList == 0 ) return 0;

	hList->mc.nId = nId;
	hList->mc.x = nX;
	hList->mc.y = nY;
	hList->mc.nWidth = nWidth;
	hList->mc.nHeight = nHeight;

	hList->nColor = nColor;
	hList->nHighColor = nHighColor;
	hList->nHighlight = nId;

	hList->nFont = nFont;
	
	hList->nXOffset = 0;
	hList->nYOffset = 0;
	hList->nTextHeight = 1;
	hList->nViewable = 1;
	hList->nMaxItems = 0;
	hList->nTopItem = 0;
	hList->hScroll = 0;
	hList->nNextId = nId+1;

	hList->sTextList.pNext = 0;
	hList->sTextList.pItem = 0;
	hList->sTextList.pPrev = 0;


	hList->hScroll = MenuGetItem ( nScrollBar );

	if ( hList->hScroll ) {
		
		ScrollBarAssociate ( (HSCROLLBAR) hList->hScroll, (HMC) hList, ListBoxScroll );
		ScrollBarSetInfo ( (HSCROLLBAR) hList->hScroll, hList->nMaxItems, hList->nViewable );
	}

	hList->mc.psFuncTable = &gsListBoxFuncTable;

	hList->hTxt = TextureSearch ( nBitmap );

	MenuCreateItem ( (HMC) hList );
	
	return hList;
}


HMC ListBoxCreateFromFile ( FILE* pFile ) {

	int nId, nX, nY, nWidth, nHeight, nR, nG, nB, nA, hR, hG, hB, hA, nFont, nBitmap, nShow;
	int nTopOffset, nLeftOffset, nTextHeight, nTexts, nScrollBar;
	char pText[180];

	fscanf ( pFile, "%i %i %i %i %i %i %i\n", &nId, &nX, &nY, &nWidth, &nHeight, &nBitmap, &nShow );
	fscanf ( pFile, "%i %i %i %i %i %i %i %i %i\n", &nFont, &nR, &nG, &nB, &nA, &hR, &hG, &hB, &hA );
	fscanf ( pFile, "%i %i %i %i %i\n", &nTopOffset, &nLeftOffset, &nTextHeight, &nScrollBar, &nTexts );

	HLISTBOX hList = ListBoxCreate ( nId, nX, nY, nWidth, nHeight, nBitmap, nScrollBar, nFont, D3DCOLOR_RGBA ( nR, nG, nB, nA ), D3DCOLOR_RGBA ( hR, hG, hB, hA ) );

	HMC hMc = (HMC) hList;

	//set da offsets for the text in the listbox
	ListBoxSetOffsets ( hList, nTopOffset, nLeftOffset, nTextHeight );

	//add the specified info to the list box
	for ( nX = 0; nX < nTexts; nX++ ) {
		
		fscanf ( pFile, "%[^\n]\n", pText );
		ListBoxAddItem ( hList, pText, -1 );
	}


	
	ListBoxShow ( hMc, nShow, 1 );

	return hMc;
}

//returns number of item in the list
PUBLIC int ListBoxGetNumItem(HLISTBOX hList)
{
	return hList->nMaxItems;
}

//returns the string of given ind
//NULL if ind is invalid
PUBLIC char * ListBoxGetItemString(HLISTBOX hList, int ind)
{
	if(ind < 0 || ind >= hList->nMaxItems)
		return 0;

	MenuNode* pNode = hList->sTextList.pNext;

	int i = 0;

	while(pNode) 
	{
		if(i == ind) 
			return TextBoxGetText((HTEXTBOX)pNode->pItem);

		pNode = pNode->pNext;
		i++;
	}

	return 0;
}