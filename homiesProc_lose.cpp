#include "homies.h"
#include "homies_i.h"
#include "homies_menu_id.h"

#define LOSEFNTIND	0

#define LOSEFILE "lose.txt"

static HMC g_win=0;

typedef enum {
	eEXIT_NO,
	eEXIT_CONTINUE,
	eEXIT_WUSSOUT,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

static hFNT g_fnt=0;

PROTECTED int LoseWinProc(int nWinId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		switch(HIWORD(wParam))
		{
		case MID_LOSE_CONTINUE:
			g_gameLives--;
			g_exit=eEXIT_CONTINUE;
			break;
		case MID_LOSE_WUSSOUT:
			g_exit=eEXIT_WUSSOUT;
			break;
		}
		break;
	}

	return 1;
}

PROTECTED RETCODE LoseProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;

		g_fnt = MenuFontGet(LOSEFNTIND);

		g_win = MenuGetItem(MID_LOSE);

		if(!g_win)
		{
			g_win = (HMC)WindowCreate(MID_LOSE, LoseWinProc, LOSEIMGID, 0, 0, GFXGetScrnSize().cx, GFXGetScrnSize().cy);
			MenuAddWindow(g_win, 0);

			char path[MAXCHARBUFF];

			//load up the menu
			strcpy(path, WINDOWDIR);
			strcat(path, LOSEFILE);

			FILE *fp = fopen(path, "rt");
			MenuLoadFromFile (fp);
			fclose(fp);
		}
		
		MenuShowItem(g_win, HMC_ACTIVE, 1);

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

		{
			char buff[MAXCHARBUFF];

			SIZE fntSz;

			float x=GFXGetScrnSize().cx/2;
			float y=GFXGetScrnSize().cy/4;

			sprintf(buff, "Lives Left: %d", g_gameLives);
			FontGetStrSize(g_fnt, buff, &fntSz);
			FontPrintf2D(g_fnt, x-(fntSz.cx/2), y, 0x7fffffff, buff);
		}

		GFXBltModeDisable();
		
		if(_GUTBKFXUpdate() == RETCODE_BREAK && g_exit != eEXIT_NO)
		{
			SENDPROCMSG(GMSG_DESTROY, 0, 0);

			switch(g_exit)
			{
			case eEXIT_CONTINUE:
				SETPROCTYPE(eGame);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			case eEXIT_WUSSOUT:
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