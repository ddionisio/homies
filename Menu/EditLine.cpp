#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"
#include "..\Timer\Timer.h"

#include "i_menu.h"
#include "Fonts.h"
#include "EditLine.h"


struct _EditLine {

	MC mc;
	int nChars, nMaxChars, nTextWidth;
	char* pText;
	int nColor;
	hTXT hTxt;
	hFNT hFnt;

	char cLast;
	timer sTimer;
	timer sRepeatTimer;
	int bRepeat;
};




void EditLineDraw ( HMC hMc, int x, int y ) {

	HEDITLINE hEditLine = (HEDITLINE) hMc;

	if ( hMc->nStatus == HMC_INVIS ) return;

	x += hEditLine->mc.x;
	y += hEditLine->mc.y;

	if ( hEditLine->nChars )

		FontPrintf2D ( hEditLine->hFnt, x, y, hEditLine->nColor, hEditLine->pText );
}


//the edit line only needs to process mouse down messages to determine
// keyboard focus
int EditLineMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	x -= hMc->x;
	y -= hMc->y;

	if ( msg == WM_LBUTTONDOWN ) {

		if ( x >= 0 && x <= hMc->nWidth ) {

			if ( y >= 0 && y <= hMc->nHeight ) {

				return 1;
			}
		}
	}

	return 0;
}

#define PAUSE 400

int EditLineKeyboard (HMC hMc, UINT msg, LPARAM lParam ) {

	HEDITLINE hEdit = (HEDITLINE) hMc;

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	char c = (char) lParam;

	switch ( msg ) {

	case WM_KEYDOWN:

		//if the key is printable
		if ( isprint (c) ) {

			if ( hEdit->nTextWidth >= hEdit->mc.nWidth ) break;

			//if it is the same as the last key pressed
			if ( hEdit->cLast == c ) {

				//if it is repeating
				if ( hEdit->bRepeat ) {

					if ( TimerEllapse ( &hEdit->sRepeatTimer ) ) {

						EditLineAddChar ( hEdit, c );
					}
				}

				//if it is the second character hasn't been drawn yet, longer wait for it
				else {
					
					if ( TimerEllapse ( &hEdit->sTimer ) ) {
						
						hEdit->bRepeat = 1;
						EditLineAddChar ( hEdit, c );
						TimerEllapse ( &hEdit->sRepeatTimer );
					}
				}
			}

			//if this is the first time on the character
			else {
			
				if ( TimerEllapse ( &hEdit->sRepeatTimer )) {
				
					hEdit->cLast = c;
					//TimerInit ( &hEdit->sTimer, 1000, PAUSE);
					TimerEllapse ( &hEdit->sTimer );
					EditLineAddChar ( hEdit, c );
				}
			}
		}

		else {

			if ( c == VK_BACK ) {

				if ( hEdit->cLast == VK_BACK ) {

					if ( hEdit->bRepeat ) {

						if ( TimerEllapse ( &hEdit->sRepeatTimer ) ) {

							EditLineDeleteChar ( hEdit );
						}
					}

					else {
					
						if ( TimerEllapse ( &hEdit->sTimer ) ) {
						
							hEdit->bRepeat = 1;
							EditLineDeleteChar ( hEdit );
							TimerEllapse ( &hEdit->sRepeatTimer );
						}
					}
				}

				else {

					if ( TimerEllapse ( &hEdit->sRepeatTimer ) ) {
					
						hEdit->cLast = c;
						TimerEllapse ( &hEdit->sTimer );
						EditLineDeleteChar ( hEdit );
					}
				}
			}
		}
		break;

	case WM_KEYUP:

		if ( c == hEdit->cLast ) {

			hEdit->cLast = 0;
			hEdit->bRepeat = 0;
			TimerEllapse ( &hEdit->sRepeatTimer );
		}

		break;

	default:
		break;
	}

	return 0;
}


//EditLines don't have children
int EditLineAddChild ( HMC hParent, HMC hChild, int nId ) {

	return 0;
}

//gives the EditLine the new value for its visible state
void EditLineShow ( HMC hMc, int bShow, int bAll ) {

	hMc->nStatus = bShow;
}


_MenuFuncTable gsEditLineFuncTable = {EditLineDraw, EditLineMouseProc, EditLineKeyboard, EditLineDestroy, EditLineShow, EditLineAddChild};


//clears the text from the edit line
PUBLIC void EditLineClear ( HEDITLINE hEditLine ) {

	hEditLine->nChars = 0;
	hEditLine->pText [0] = '\0';
}


//tries to add the character to the edit line, returns 0 on failure
PUBLIC int EditLineAddChar ( HEDITLINE hEditLine, char cChar ) {

	SIZE sSize;

	if ( hEditLine->nChars == hEditLine->nMaxChars ) return 0;
	
	hEditLine->pText [hEditLine->nChars] = cChar;

	hEditLine->nChars++;
	hEditLine->pText [hEditLine->nChars] = '\0';

	FontGetStrSize ( hEditLine->hFnt, hEditLine->pText, &sSize );

	if ( sSize.cx > hEditLine->mc.nWidth ) {

		hEditLine->nChars--;
		hEditLine->pText [hEditLine->nChars] = '\0';
		return 0;
	}

	hEditLine->nTextWidth = sSize.cx;

	return 1;
}

//tries to add the character to the edit line, returns 0 on failure
//truncates string if edit line is full
PUBLIC int EditLineSetString ( HEDITLINE hEditLine, const char *str) {

	EditLineClear(hEditLine);

	SIZE sSize;
	FontGetStrSize ( hEditLine->hFnt, hEditLine->pText, &sSize );

	int strInd=0, ret=1;

	if ( hEditLine->nChars == hEditLine->nMaxChars ) return 0;
	
	while(str[strInd] != 0)
	{
		hEditLine->pText [hEditLine->nChars] = str[strInd];

		hEditLine->nChars++;
		hEditLine->pText [hEditLine->nChars] = '\0';
		FontGetStrSize ( hEditLine->hFnt, hEditLine->pText, &sSize );

		if ( sSize.cx > hEditLine->mc.nWidth ) {
			hEditLine->nChars--;
			hEditLine->pText [hEditLine->nChars] = '\0';
			ret = 0;
			break;
		}

		strInd++;
	}

	hEditLine->nTextWidth = sSize.cx;

	return ret;
}

PUBLIC void EditLineGetString ( HEDITLINE hEdit, char* pString ) {

	strcpy ( pString, hEdit->pText );
}


PUBLIC int EditLineGetStringLength ( HEDITLINE hEdit ) {

	return hEdit->nChars;
}


//removes the last character from the string, returns 0 on failure
PUBLIC int EditLineDeleteChar ( HEDITLINE hEditLine ) {

	SIZE sSize;

	if ( hEditLine->nChars == 0 ) return 0;

	hEditLine->nChars--;

	hEditLine->pText [hEditLine->nChars] = '\0';

	FontGetStrSize ( hEditLine->hFnt, hEditLine->pText, &sSize );

	hEditLine->nTextWidth = sSize.cx;

	return 1;
}

	

//creates a menu item with the normal info plus all the font stuff that it needs
PUBLIC HEDITLINE EditLineCreate ( int nId, int nX, int nY, int nWidth, int nHeight,
								int bText, char* pText, int nColor, int nFont, int nMaxChars ) {

	HEDITLINE hEditLine;
	SIZE sSize;

	MemAlloc ( (void**)&hEditLine, sizeof(*hEditLine), 0);

	hEditLine->mc.x = nX;
	hEditLine->mc.y = nY;
	hEditLine->mc.nId = nId;
	hEditLine->mc.nWidth = nWidth;
	hEditLine->mc.nHeight = nHeight;

	
	hEditLine->nChars = 0;
	hEditLine->nMaxChars = nMaxChars;
	hEditLine->nTextWidth = 0;

	MemAlloc ( (void**)&hEditLine->pText, nMaxChars + 1, 0 );

	hEditLine->hFnt = MenuFontGet ( nFont );
	hEditLine->nColor = nColor;

	if ( bText ) {

		FontGetStrSize ( hEditLine->hFnt, pText, &sSize );
		hEditLine->nTextWidth = sSize.cx;

		hEditLine->nChars = strlen ( pText );
		strcpy ( hEditLine->pText, pText );
	}

	TimerInit ( &hEditLine->sTimer, 1000, 450 );
	TimerInit ( &hEditLine->sRepeatTimer, 1000, 100 );

	hEditLine->mc.psFuncTable = &gsEditLineFuncTable;

	MenuCreateItem ( (HMC) hEditLine );

	return hEditLine;
}

HMC EditLineCreateFromFile ( FILE* pFile ) {

	int nId, nX, nY, nWidth, nHeight, nR, nG, nB, nA, nShow, nFont, bText, nMaxChars;
	char pText[180];

	fscanf ( pFile, "%i %i %i %i %i %i %i %i %i %i %i %i %i\n", &nId, &nX, &nY, &nWidth,
		&nHeight, &nR, &nG, &nB, &nA, &nFont, &nMaxChars, &bText, &nShow );

	if ( bText )
		fscanf ( pFile, "%[^\n]\n", pText );

	HMC hMc = (HMC) EditLineCreate ( nId, nX, nY, nWidth, nHeight, bText, pText, 
		D3DCOLOR_RGBA ( nR, nG, nB, nA ), nFont, nMaxChars );

	EditLineShow ( hMc, nShow, 0 );

	return hMc;
}


//Frees up all the stuff associated with teh editline
void EditLineDestroy ( HMC hMc, int bAll ) {

	HEDITLINE hEditLine = (HEDITLINE) hMc;

	MenuDestroyItem ( hMc );
	MemFree ( (void**)&hEditLine->pText );
	MemFree ( (void**)&hEditLine );
}