#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"

#include "i_menu.h"
#include "Button.h"

struct _Button {

	MC mc;
	int nPress;
	hTXT hTxt;
	unsigned int color; //David's addition
	hSPRITE hSprite;
	int bClicked;
};




void ButtonDraw ( HMC hMc, int x, int y ) {

	if ( hMc->nStatus == HMC_INVIS ) return;

	HBUTTON hButton = (HBUTTON) hMc;

	RECT sRect = {hButton->mc.x + x, hButton->mc.y + y,
		x + hButton->mc.x + hButton->mc.nWidth, y + hButton->mc.y + hButton->mc.nHeight};

	float sloc [eMaxPt] = {sRect.left, sRect.top};
	SpriteStretchBlt ( hButton->hSprite, sloc, (float)hButton->mc.nWidth, (float)hButton->mc.nHeight, &hButton->color, 0 );
	SpriteUpdateFrame ( hButton->hSprite );
}



//process mouse messages
int ButtonMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

	HBUTTON hButton = (HBUTTON) hMc;
	
	if ( hMc->nStatus != HMC_ACTIVE ) return 0;

	x -= hButton->mc.x;
	y -= hButton->mc.y;
	
	switch ( msg ) {

	case WM_MOUSEMOVE:

		//check for collision
		if ( !(wParam & 128) && x >= 0 && x <= hButton->mc.nWidth ) {

			if ( y >= 0 && y <= hButton->mc.nHeight ) {


				//if the left mouse button is down
				if ( wParam & MK_LBUTTON ) {

					//if the the button isn't already pressed
					if ( hButton->nPress != BUTTON_DOWN ) {

						//if the button owns the click message
						if ( hButton->bClicked == 1 ) {
						
							//set the button to be clicked down
							hButton->nPress = BUTTON_DOWN;
							SpriteSetState ( hButton->hSprite, BUTTON_DOWN );
						}
					}
				}

				//if the left mouse button isn't down
				else {

					//and the button isn't already in its mouse over state
					if ( hButton->nPress != BUTTON_OVER ) {

						//set the button to the mouse over state
						hButton->nPress = BUTTON_OVER;
						SpriteSetState ( hButton->hSprite, BUTTON_OVER );
					}
				}
				return 1;
			}
		}

		//if the mouse is not over the button and the button isn't up
		if ( hButton->nPress != BUTTON_UP ) {

			//set the button to be up
			hButton->nPress = BUTTON_UP;
			SpriteSetState ( hButton->hSprite, hButton->nPress );
		}
		break;

	case WM_LBUTTONDOWN:

		//check for collision
		if ( !( wParam & 128 ) && x >= 0 && x <= hButton->mc.nWidth ) {

			if ( y >= 0 && y <= hButton->mc.nHeight ) {

				//if the button isn't already down
				if ( hButton->nPress != BUTTON_DOWN ) {

					//this button owns the click, set the button to down
					hButton->bClicked = 1;
					hButton->nPress = BUTTON_DOWN;
					SpriteSetState ( hButton->hSprite, BUTTON_DOWN );
				}
				return 1;
			}
		}

		//set the button to up if it isn't already
		if ( hButton->nPress != BUTTON_UP ) {

			hButton->nPress = BUTTON_UP;
			SpriteSetState ( hButton->hSprite, hButton->nPress );
		}
		break;


	case WM_LBUTTONUP:

		//check for collision
		if ( !( wParam & 128) && x >= 0 && x <= hButton->mc.nWidth ) {

			if ( y >= 0 && y <= hButton->mc.nHeight ) {

				//if the button isn't already in mouse over mode
				if ( hButton->nPress != BUTTON_OVER ) {

					//make the button mouse over
					hButton->nPress = BUTTON_OVER;
					SpriteSetState ( hButton->hSprite, hButton->nPress );

					//if the button owned the click
					if ( hButton->bClicked ) {

						//return that something happened
						hButton->bClicked = 0;
						return 1;
					}
					return 0;
				}
			}
		}

		hButton->bClicked = 0;

		if ( hButton->nPress != BUTTON_UP ) {
			hButton->nPress = BUTTON_UP;
			SpriteSetState ( hButton->hSprite, hButton->nPress );
		}
		break;

	}
	return 0;
}



int ButtonKeyboard (HMC hMc, UINT msg, LPARAM lParam ) {

	if ( hMc->nStatus == 0 ) return 0;

	return 0;
}


//sets the button's visible state to the value of bShow
void ButtonShowFunc ( HMC hMc, int bShow, int bAll ) {

	hMc->nStatus = bShow;
}


_MenuFuncTable gsButtonFuncTable = {ButtonDraw, ButtonMouseProc, ButtonKeyboard, ButtonDestroy, ButtonShowFunc};



PUBLIC HBUTTON ButtonCreate ( int nId, char* pSprite, int nX, int nY, int nWidth, int nHeight, int nBitmap ) {

	HBUTTON hButton;

	MemAlloc ( (void**)&hButton, sizeof(*hButton), 0);

	hButton->nPress = BUTTON_UP;
	hButton->mc.x = nX;
	hButton->mc.y = nY;
	hButton->mc.nId = nId;
	hButton->mc.nWidth = nWidth;
	hButton->mc.nHeight = nHeight;

	hButton->mc.psFuncTable = &gsButtonFuncTable;

	MenuCreateItem ( (HMC) hButton );

	hButton->hTxt = TextureSearch ( nBitmap );
	LOCATION sLoc = {0,0};
	hButton->hSprite = SpriteLoad ( hButton->hTxt, 0, &sLoc, pSprite );
	TextureAddRef ( hButton->hTxt );

	//David's addition
	hButton->color = 0xffffffff;

	return hButton;
}


PUBLIC void ButtonDestroy ( HMC hMc, int bAll ) {

	if(hMc)
	{
		MenuDestroyItem ( hMc );
		HBUTTON hButton = (HBUTTON) hMc;
		SpriteDestroy ( &hButton->hSprite );
	//	TextureDestroy ( &hButton->hTxt );
		MemFree ( (void**)&hButton );
	}
}



//this function is called from MenuLoadFromFile
// it is given a pointer to button data in a file
// reads the data and creates the button
HMC ButtonCreateFromFile ( FILE* pFile ) {

	 int nId, nX, nY, nWidth, nHeight, nShow, nBitmap;
	 char pSprite[180];

	 fscanf ( pFile, "%i %i %i %i %i %i %i\n", &nId, &nX, &nY, &nWidth, &nHeight, &nBitmap, &nShow );

	 fscanf ( pFile, "%[^\n]\n", pSprite );

	 HMC hMc = (HMC) ButtonCreate ( nId, pSprite, nX, nY, nWidth, nHeight, nBitmap );

	 ButtonShowFunc ( hMc, nShow, 0 );

	 return hMc;
}

//This will set the button's texture color
//David's addition
void ButtonSetImgColor (HBUTTON hButton, unsigned int newC)
{
	//David's addition
	hButton->color = newC;
}

//This will change the image and sprite of button
//David's addition
void ButtonChange (HBUTTON hButton, unsigned int newTxtID, const char *newSpriteFile)
{
	SpriteDestroy ( &hButton->hSprite );
	TextureDestroy ( &hButton->hTxt );

	hButton->hTxt = TextureSearch ( newTxtID );
	hButton->hSprite = SpriteLoad ( hButton->hTxt, 0, 0, newSpriteFile );
	TextureAddRef ( hButton->hTxt );
}