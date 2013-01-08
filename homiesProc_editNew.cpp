#include "homies.h"
#include "homies_i.h"
#include "homies_menu_id.h"

#include "menu\\editline.h"
#include "menu\\textbox.h"

#define EDITNEWFILE "editor_new.txt"

#define MAXWIDTH	20
#define MAXHEIGHT	20

static HMC g_win=0;

typedef enum {
	eEXIT_NO,
	eEXIT_OK,
	eEXIT_CANCEL,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

static mapEditInfo g_mapEdit={0};

PROTECTED int EditNewWinProc(int nWinId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buff[MAXCHARBUFF];

	switch(msg)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		switch(HIWORD(wParam))
		{
		case MID_EDITNEW_UPWIDTH:
			if(g_mapEdit.col < MAXWIDTH)
				g_mapEdit.col++;
			sprintf(buff, "%d", g_mapEdit.col);
			TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTWIDTH), buff);
			break;
		case MID_EDITNEW_DNWIDTH:
			if(g_mapEdit.col > 1)
				g_mapEdit.col--;
			sprintf(buff, "%d", g_mapEdit.col);
			TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTWIDTH), buff);
			break;
		case MID_EDITNEW_UPHEIGHT:
			if(g_mapEdit.row < MAXHEIGHT)
				g_mapEdit.row++;
			sprintf(buff, "%d", g_mapEdit.row);
			TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTHEIGHT), buff);
			break;
		case MID_EDITNEW_DNHEIGHT:
			if(g_mapEdit.row > 1)
				g_mapEdit.row--;
			sprintf(buff, "%d", g_mapEdit.row);
			TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTHEIGHT), buff);
			break;
		case MID_EDITNEW_OK:
			g_exit=eEXIT_OK;
			break;
		case MID_EDITNEW_CANCEL:
			g_exit=eEXIT_CANCEL;
			break;
		}
		break;
	}

	return 1;
}

PROTECTED RETCODE EditNewProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;

		_GUTBKFXEnterProc(eBKFX_FADEINOUT,0);

		g_win = MenuGetItem(MID_EDITNEW);

		if(!g_win)
		{
			char path[MAXCHARBUFF];

			//load up the menu
			strcpy(path, WINDOWDIR);
			strcat(path, EDITNEWFILE);

			FILE *fp = fopen(path, "rt");
			MenuLoadFromFile (fp);
			fclose(fp);

			g_win = MenuGetItem(MID_EDITNEW);

			WindowChangeSize((HWINDOW)g_win, GFXGetScrnSize().cx, GFXGetScrnSize().cy);
			WindowNewProc((HWINDOW)g_win, EditNewWinProc);
		}
		else
			MenuShowItem(g_win, HMC_ACTIVE, 1);

		{
			g_mapEdit.col = 1;
			g_mapEdit.row = 1;

			char buff[MAXCHARBUFF];

			sprintf(buff, "%d", g_mapEdit.col);
			TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTWIDTH), buff);

			sprintf(buff, "%d", g_mapEdit.row);
			TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTHEIGHT), buff);
		}
		break;

	case GMSG_UPDATE:
		if(g_exit==eEXIT_NO)
		{
			MenuProc();			 //update the windows

			if(g_exit!=eEXIT_NO)
				_GUTBKFXExitProc(eBKFX_FADEINOUT,0);
		}
		break;

	case GMSG_DISPLAY:
		GFXClear(0, 0, 0);   //clears screen within rects with given color

		GFXBltModeEnable(false);
		MenuDraw();			 //draw all menus
		GFXBltModeDisable();

		if(_GUTBKFXUpdate() == RETCODE_BREAK && g_exit != eEXIT_NO)
		{
			SENDPROCMSG(GMSG_DESTROY, 0, 0);

			switch(g_exit)
			{
			case eEXIT_OK:
				SETPROCTYPE(eEditMap);

				EditLineGetString((HEDITLINE)MenuGetItem(MID_EDITNEW_ELINE), g_mapEdit.mapName);
				g_mapEdit.bCreateNew = true;

				sscanf(TextBoxGetText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTWIDTH)), "%d", &g_mapEdit.col);
				sscanf(TextBoxGetText((HTEXTBOX)MenuGetItem(MID_EDITNEW_TXTHEIGHT)), "%d", &g_mapEdit.row);
				
				SENDPROCMSG(GMSG_INIT, (WPARAM)&g_mapEdit, 0);
				break;
			case eEXIT_CANCEL:
				SETPROCTYPE(eEditor);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			}
		}
		
		GFXUpdate(0, 0, 0);  //update frame on display
		break;

	case GMSG_LOAD:
		break;

	case GMSG_DESTROY:
		if(g_win)
			MenuShowItem(g_win, HMC_INVIS, 1);
		break;
	}

	return RETCODE_SUCCESS;
}