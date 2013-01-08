#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"

#include "i_menu.h"
#include "Fonts.h"
#include "ListBox.h"

#include "MultiLine.h"

struct _MultiLine {

	MC mc;
	hFNT hFont;
	int nTextWidth;
	HLISTBOX hList;
};

#define _MAX_LINE	512


//draws the background of the box, then the text and the scroll bar
void MultiLineDraw ( HMC hMc, int x, int y ) {

	if ( hMc->nStatus == HMC_INVIS ) return;

	HMC hList = (HMC) ((HMULTILINE) hMc)->hList;
	x += hMc->x;
	y += hMc->y;

	hList->psFuncTable->MenuDrawFunc ( hList, x, y );

}


//the List box doesn't really process mouse messages
int MultiLineMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

//	int yhit;
//	MenuNode* pNode;

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	//change the position to reflect the position of the listbox

//	if ( wParam & 128 ) return 0;

	x -= hMc->x;
	y -= hMc->y;

	HMC hList = (HMC) ((HMULTILINE)hMc)->hList;

	return hList->psFuncTable->MenuMouseProcFunc ( hList, msg, x, y, wParam );
}


//the listbox doesn't take any keyboard input
int MultiLineKeyboard (HMC hMc, UINT msg, LPARAM lParam ) {

	if ( hMc->nStatus == 0 ) return 0;

	return 0;
}


//Listboxes don't have children
int MultiLineAddChild ( HMC hParent, HMC hChild, int nId ) {

	return 0;
}

//gives the Listbox the new value
void MultiLineShow ( HMC hMc, int bShow, int bAll ) {

	HMC hList = (HMC) ((HMULTILINE)hMc)->hList;
	hMc->nStatus = bShow;

	hList->psFuncTable->MenuShowFunc ( hList, bShow, 1 );
}


//destroy the list box and all its stuff
void MultiLineDestroy ( HMC hMc, int bAll ) {

	assert ( hMc );

	HMULTILINE hMulti = (HMULTILINE) hMc;

	((HMC)hMulti->hList)->psFuncTable->MenuDestroyFunc ( (HMC) hMulti->hList, 1 );

	MenuDestroyItem ( hMc );

	MemFree ( (void**) &hMc );
}

struct _MenuFuncTable gsMultiLineFuncTable = { MultiLineDraw, MultiLineMouseProc, MultiLineKeyboard, MultiLineDestroy, MultiLineShow, MultiLineAddChild };

void MultiLineAddText ( HMULTILINE hMulti, char* pText, int nWidth ) {

	HLISTBOX hList = hMulti->hList;

	char pHolder[_MAX_LINE];
	SIZE sSize;

	hFNT hFnt = hMulti->hFont;
	FontGetStrSize ( hFnt, pText, &sSize );

	if ( sSize.cx > nWidth ) {
		
		int n = 0;
		int nLast = 0;
		int nWord = 0;

		do {

			//nL is the position in in the text
			int nL = n + nLast;

			//if it is a alphanumeric character
			while ( isgraph ( pText[nL] ) ) {

				//copy the char and move forward
				pHolder[n] = pText[nL];
				nL++;
				n++;
			}

			//end the string and get its size
			pHolder[n] = '\0';
			FontGetStrSize ( hFnt, pHolder, &sSize );

			//if it is too long
			if ( sSize.cx > nWidth ) {

				pHolder [nWord-nLast-1] = '\0';
				ListBoxAddItem ( hList, pHolder, -1 );

				nLast = nWord;
				n = 0;
			}

			//if the string is not too long
			else {

				//if its printable, add it to the line and continue
				if ( isprint ( pText[nL] ) ) {

					pHolder[n++] = pText[nL];
				}

				//if its not printable, its a line break or the end of the string
				else {

					//add line
					pHolder [n++] = '\0';
					ListBoxAddItem ( hList, pHolder, -1 );

					//break out if its the end
					if ( pText[nL] != '\n' ) break;

					//it was a line break, move on
					nWord = nL + 1;
					nLast = nWord;
					n = 0;
				}
			}

			//point to the next word
			nWord = n + nLast;

		} while ( pText[n + nLast] != '\0' );
	}

	else {

		ListBoxAddItem ( hList, pText, -1 );
	}
}


//clears the multiline text box
void MultiLineClear ( HMULTILINE hMulti ) {

	ListBoxClear ( (HLISTBOX) hMulti->hList );
}



HMULTILINE MultiLineCreate ( int nId, int nX, int nY, int nWidth, int nHeight, int nTextWidth, int nBitmap, int nScrollBar, int nFont, int nColor ) {

	HMULTILINE hMulti;

	MemAlloc ( (void**) &hMulti, sizeof ( *hMulti ), 0 );

	assert ( hMulti );

	if ( hMulti == 0 ) return 0;

	hMulti->mc.nId = nId;
	hMulti->mc.x = nX;
	hMulti->mc.y = nY;
	hMulti->mc.nWidth = nWidth;
	hMulti->mc.nHeight = nHeight;
	hMulti->mc.psFuncTable = &gsMultiLineFuncTable;


	hMulti->nTextWidth = nTextWidth;
	hMulti->hFont = MenuFontGet ( nFont );
	hMulti->hList = ListBoxCreate ( nId+1, 0, 0, nWidth, nHeight, nBitmap, nScrollBar, nFont, nColor, nColor );

	MenuCreateItem ( (HMC) hMulti );

	return hMulti;
}

HMC MultiLineCreateFromFile ( FILE* pFile ) {


	int nId, nX, nY, nWidth, nHeight, nBitmap, nFont, nShow;
	int r,g,b,a;
	int nTopOffset, nLeftOffset, nTextWidth, nTextHeight, nScrollBar, nTexts;

	fscanf ( pFile, "%i %i %i %i %i %i %i %i\n%i %i %i %i\n", &nId, &nX, &nY, &nWidth, &nHeight, &nBitmap, &nFont, &nShow, &r, &g, &b, &a );
	fscanf ( pFile, "%i %i %i %i %i %i\n", &nTopOffset, &nLeftOffset, &nTextWidth, &nTextHeight, &nScrollBar, &nTexts );

	HMULTILINE hMulti = MultiLineCreate ( nId, nX, nY, nWidth, nHeight, nTextWidth, nBitmap, nScrollBar, nFont, D3DCOLOR_RGBA( r, g, b, a ) );

	ListBoxSetOffsets ( hMulti->hList, nLeftOffset, nTopOffset, nTextHeight );

	for ( r=0; r< nTexts; r++ ) {

		int c, n;
		char* pText;

		n = 0;

		do {
			c = fgetc ( pFile );
			n++;
		} while ( c != '\n' );
		
		fseek ( pFile, -(n+1), SEEK_CUR );

		MemAlloc ( (void**) &pText, n+1, 0 );

		fread ( (void*) pText, n , 1, pFile );
		pText[n-1] = '\n';
		pText[n] = '\0';
		MultiLineAddText ( hMulti, pText, nTextWidth );

		MemFree ( (void**) &pText );
	}
	fscanf ( pFile, "\n" );

	MultiLineShow ( (HMC) hMulti, nShow, 1 );

	return (HMC) hMulti;
}