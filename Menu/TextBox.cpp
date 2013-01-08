#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"

#include "I_menu.h"
#include "Fonts.h"
//#include "menu.h"
#include "TextBox.h"

struct _TextBox {

	MC mc;
	char* pText;
	int nColor;
	hFNT hFnt;
};


void TextBoxDraw ( HMC hMc, int x, int y ) {

	if ( hMc->nStatus == HMC_INVIS ) return;

	HTEXTBOX hTextBox = (HTEXTBOX) hMc;
	x += hTextBox->mc.x;
	y += hTextBox->mc.y;

	if ( hTextBox->mc.nStatus )
		FontPrintf2D ( hTextBox->hFnt, x, y, hTextBox->nColor, hTextBox->pText );
}


//the text box doesn't really process mouse messages, it just acknowledges that
// it has been hit
int TextBoxMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	x -= hMc->x;
	y -= hMc->y;
	
	if ( msg == WM_LBUTTONDOWN ) {

		//check for collision
		if ( x >= 0 && x <= hMc->nWidth ) {

			if ( y >= 0 && y <= hMc->nHeight ) {

				return 1;
			}
		}
	}

	return 0;
}


//the textbox doesn't take any input really
int TextBoxKeyboard (HMC hMc, UINT msg, LPARAM lParam ) {

	return 0;
}


//Textboxes don't have children
int TextBoxAddChild ( HMC hParent, HMC hChild, int nId ) {

	return 0;
}

//gives the textbox the new value
void TextBoxShow ( HMC hMc, int bShow, int bAll ) {

	hMc->nStatus = bShow;
}


_MenuFuncTable gsTextBoxFuncTable = {TextBoxDraw, TextBoxMouseProc, TextBoxKeyboard, TextBoxDestroy, TextBoxShow, TextBoxAddChild};


//changes the text that the text box displays, the text cannot be a blank
// string, a space is suggested as it will appear to be nothing on the screen
PUBLIC void TextBoxChangeText ( HTEXTBOX hTextBox, char* pNewText ) {

	MemFree ( (void**)&hTextBox->pText );

	MemAlloc ( (void**)&hTextBox->pText, strlen (pNewText) + 1, 0 );
	strcpy ( hTextBox->pText, pNewText );
}


//changes the text color of the text box text, I suggest cunning use of
// D3DCOLOR_RGBA
PUBLIC void TextBoxChangeTextColor ( HTEXTBOX hTextBox, int nColor ) {

	hTextBox->nColor = nColor;
}


//this function returns a pointer to the text in the textbox
// this pointer is not owned by the user, but by the box
// changing the information in it could be very very bad
PUBLIC char* TextBoxGetText ( HTEXTBOX hTextBox ) {

	return hTextBox->pText;
}

//creates a menu item with the normal info plus all the font stuff that it needs
PUBLIC HTEXTBOX TextBoxCreate ( int nId, int nX, int nY, int nWidth, int nHeight, char* pText, int nColor, int nFont ) {

	HTEXTBOX hTextBox;

	MemAlloc ( (void**)&hTextBox, sizeof(*hTextBox), 0);

	hTextBox->mc.x = nX;
	hTextBox->mc.y = nY;
	hTextBox->mc.nId = nId;
	hTextBox->mc.nWidth = nWidth;
	hTextBox->mc.nHeight = nHeight;

	MemAlloc ( (void**)&hTextBox->pText, strlen (pText) + 1, 0 );
	strcpy ( hTextBox->pText, pText );

	hTextBox->hFnt = MenuFontGet ( nFont );	//get a handle to the font
	hTextBox->nColor = nColor;

	hTextBox->mc.psFuncTable = &gsTextBoxFuncTable;

	MenuCreateItem ( (HMC) hTextBox );

	return hTextBox;
}

HMC TextBoxCreateFromFile ( FILE* pFile ) {

	int nId, nX, nY, nWidth, nHeight, nR, nG, nB, nA, nFont, nShow;
	char pText[180];

	fscanf ( pFile, "%i %i %i %i %i %i %i %i %i %i %i\n", &nId, &nX, &nY, &nWidth, &nHeight, &nR, &nG, &nB, &nA, &nFont, &nShow );
	fscanf ( pFile, "%[^\n]\n", pText );

	HMC hMc = (HMC) TextBoxCreate ( nId, nX, nY, nWidth, nHeight, pText, D3DCOLOR_RGBA ( nR, nG, nB, nA ), nFont );

	TextBoxShow ( hMc, nShow, 0 );

	return hMc;
}


//Frees up all the stuff associated with teh TExtBOx
void TextBoxDestroy ( HMC hMc, int bAll ) {

	HTEXTBOX hTextBox = (HTEXTBOX) hMc;

	MenuDestroyItem ( hMc );
	MemFree ( (void**)&hTextBox->pText );
	MemFree ( (void**)&hTextBox );
}