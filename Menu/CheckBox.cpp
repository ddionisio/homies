#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"

#include "i_menu.h"
#include "CheckBox.h"



struct _CheckBox {

	MC mc;
	int bChecked;
	hTXT tBackground, tCheckmark;
};


void CheckBoxDraw ( HMC hMc, int x, int y ) {

	if ( hMc->nStatus == 0 ) return;

	HCHECKBOX hCheckBox = (HCHECKBOX) hMc;

	RECT sRect = {hCheckBox->mc.x + x, hCheckBox->mc.y + y,
		x + hCheckBox->mc.x + hCheckBox->mc.nWidth, y + hCheckBox->mc.y + hCheckBox->mc.nHeight};

	if ( hCheckBox->mc.nStatus != HMC_INVIS ) {

		TextureStretchBlt ( hCheckBox->tBackground, sRect.left, sRect.top, hCheckBox->mc.nWidth, hCheckBox->mc.nHeight, NULL, NULL, 0 );

		if ( hCheckBox->bChecked ) {

			TextureBlt ( hCheckBox->tCheckmark, sRect.left, sRect.top, 0, NULL, 0 );
		}
	}

}


int CheckBoxMouseProc ( HMC hMc, UINT msg, int x, int y, WPARAM wParam ) {

	HCHECKBOX hCheckBox = (HCHECKBOX) hMc;

	if ( hMc->nStatus == 0 ) return 0;
	
	switch ( msg ) {

	case WM_LBUTTONDOWN:

		if ( hCheckBox->mc.nStatus ) {
			
			if ( x >= hCheckBox->mc.x && x <= hCheckBox->mc.x + hCheckBox->mc.nWidth ) {

				if ( y >= hCheckBox->mc.y && y <= hCheckBox->mc.y + hCheckBox->mc.nHeight ) {

					hCheckBox->bChecked = !hCheckBox->bChecked;

					return 1;
				}
			}
		}

		break;

	}
	return 0;
}


int CheckBoxKeyboard (HMC hMc, UINT msg, LPARAM lParam ) {

	return 0;
}


void CheckBoxShow ( HMC hMc, int bShow, int bAll ) {

	hMc->nStatus = bShow;
}

_MenuFuncTable gsCheckBoxFuncTable = {CheckBoxDraw, CheckBoxMouseProc, CheckBoxKeyboard, CheckBoxDestroy, CheckBoxShow};



PUBLIC HCHECKBOX CheckBoxCreate ( int nId, int nX, int nY, int nWidth, int nHeight, int bChecked, int nBitmap, int nCheckMap ) {

	HCHECKBOX hCheckBox;

	MemAlloc ( (void**)&hCheckBox, sizeof(*hCheckBox), 0);

	hCheckBox->bChecked = bChecked;
	hCheckBox->mc.x = nX;
	hCheckBox->mc.y = nY;
	hCheckBox->mc.nId = nId;
	hCheckBox->mc.nWidth = nWidth;
	hCheckBox->mc.nHeight = nHeight;

	hCheckBox->mc.psFuncTable = &gsCheckBoxFuncTable;

	hCheckBox->tBackground = TextureSearch ( nBitmap );
	hCheckBox->tCheckmark = TextureSearch ( nCheckMap );
	TextureAddRef ( hCheckBox->tBackground );
	TextureAddRef ( hCheckBox->tCheckmark );
	MenuCreateItem ( (HMC) hCheckBox );

	return hCheckBox;
}


PUBLIC void CheckBoxDestroy ( HMC hMc, int bAll ) {

	MenuDestroyItem ( hMc );
	HCHECKBOX hCheckBox = (HCHECKBOX) hMc;
	MemFree ( (void**)&hCheckBox );
}

HMC CheckBoxCreateFromFile ( FILE* pFile ) {

	int nId, nX, nY, nWidth, nHeight, bChecked, nShow, nBitmap, nCheckMap;

	fscanf ( pFile, "%i %i %i %i %i %i %i %i %i\n", &nId, &nX, &nY, &nWidth, &nHeight, &bChecked, &nBitmap, &nCheckMap, &nShow );

	HMC hMc = (HMC) CheckBoxCreate ( nId, nX, nY, nWidth, nHeight, bChecked, nBitmap, nCheckMap );

	CheckBoxShow ( hMc, nShow, 0);

	return hMc;
}

//takes a check box handle and returns if the box is checked or not
PUBLIC int CheckBoxIsChecked ( HCHECKBOX hCheckBox ) {

	return hCheckBox->bChecked;
}

//set check state
PUBLIC void CheckBoxSetCheck ( HCHECKBOX hCheckBox, bool bSet ) {

	hCheckBox->bChecked = bSet;
}