#include "homies.h"
#include "homies_i.h"
#include "homies_menu_id.h"

#define WINALLFILE "winall.txt"

static HMC g_win=0;

typedef enum {
	eEXIT_NO,
	eEXIT_OK,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

PROTECTED int WinAllWinProc(int nWinId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		switch(HIWORD(wParam))
		{
		case MID_WINALL_OK:
			g_exit=eEXIT_OK;
			break;
		}
		break;
	}

	return 1;
}

PROTECTED RETCODE WinAllProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;
		
		_GUTBKFXEnterProc(eBKFX_FADEINOUT,0);

		g_win = MenuGetItem(MID_WINALL);

		if(!g_win)
		{
			char path[MAXCHARBUFF];

			//load up the menu
			strcpy(path, WINDOWDIR);
			strcat(path, WINALLFILE);

			FILE *fp = fopen(path, "rt");
			MenuLoadFromFile (fp);
			fclose(fp);

			g_win = MenuGetItem(MID_WINALL);

			WindowChangeSize((HWINDOW)g_win, GFXGetScrnSize().cx, GFXGetScrnSize().cy);
			WindowNewProc((HWINDOW)g_win, WinAllWinProc);
		}
		else
			MenuShowItem(g_win, HMC_ACTIVE, 1);

		GUTPlayMusic(eMUSIC_END);

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
				//check score
				if(GUTCheckScore())
				{
					SETPROCTYPE(eGotHiScore);
					SENDPROCMSG(GMSG_INIT, 0, 0);
				}
				else
				{
					SETPROCTYPE(eTopTen);
					SENDPROCMSG(GMSG_INIT, 0, 0);
				}
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