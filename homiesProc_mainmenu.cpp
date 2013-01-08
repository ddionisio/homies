#include "homies.h"
#include "homies_i.h"
#include "homies_menu_id.h"

#include "Parser\\parser.h"

#include "menu\textbox.h"

#define MAINMENUFILE "mainmenu.txt"

static HMC g_win=0;

typedef enum {
	eEXIT_NO,
	eEXIT_PLAY,
	eEXIT_OPTION,
	eEXIT_HISCORE,
	eEXIT_GAME,
	eEXIT_EDITOR,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

PROTECTED int MainMenuWinProc(int nWinId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buff[MAXCHARBUFF]={0};

	switch(msg)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		switch(HIWORD(wParam))
		{
		case MID_MAINMENU_PLAY:
			g_exit=eEXIT_PLAY;
			break;
		case MID_MAINMENU_OPTIONS:
			break;
		case MID_MAINMENU_HISCORE:
			g_exit=eEXIT_HISCORE;
			break;
		case MID_MAINMENU_EXIT:
			g_exit=eEXIT_GAME;
			break;
		case MID_MAINMENU_RGTBTN:
			if(g_gameMaps.curMap < g_gameMaps.numMap-1)
				g_gameMaps.curMap++;
			
			sprintf(buff, "%d. %s", g_gameMaps.curMap, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_MAINMENU_TXTBOX), buff);
			break;
		case MID_MAINMENU_LFTBTN:
			if(g_gameMaps.curMap > 0)
				g_gameMaps.curMap--;

			sprintf(buff, "%d. %s", g_gameMaps.curMap, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_MAINMENU_TXTBOX), buff);
			break;
		case MID_MAINMENU_EDITOR:
			g_exit=eEXIT_EDITOR;
			break;
		}
		break;
	}

	return 1;
}

PROTECTED RETCODE MainMenuProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;

		g_gameMaps.curMap = 0;

		GUTStopMusic();

		_GUTBKFXEnterProc(eBKFX_FADEINOUT,0);

		g_win = MenuGetItem(MID_MAINMENU);

		if(!g_win)
		{
			char path[MAXCHARBUFF];

			//load up the menu
			strcpy(path, WINDOWDIR);
			strcat(path, MAINMENUFILE);

			FILE *fp = fopen(path, "rt");
			MenuLoadFromFile (fp);
			fclose(fp);

			g_win = MenuGetItem(MID_MAINMENU);

			WindowChangeSize((HWINDOW)g_win, GFXGetScrnSize().cx, GFXGetScrnSize().cy);
			WindowNewProc((HWINDOW)g_win, MainMenuWinProc);
		}
		else
			MenuShowItem(g_win, HMC_ACTIVE, 1);

		char buff[MAXCHARBUFF];
		sprintf(buff, "%d. %s", g_gameMaps.curMap, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));
		TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_MAINMENU_TXTBOX), buff);

		break;
	case GMSG_UPDATE:
		if(g_exit==eEXIT_NO)
		{
			GUTPlayRandMusic(eMUSIC_RANDOFFSET, 0);

			MenuProc();			 //update the windows

			if(g_exit!=eEXIT_NO)
				_GUTBKFXExitProc(eBKFX_FADEINOUT,0);
		}
		//MenuProc();			 //update the windows
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
			case eEXIT_PLAY:
				g_gameScore = 0;
				g_gameLives = MAXCONTINUE;
				SETPROCTYPE(eGame);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			case eEXIT_OPTION:
				break;
			case eEXIT_HISCORE:
				SETPROCTYPE(eTopTen);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			case eEXIT_EDITOR:
				SETPROCTYPE(eEditor);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			case eEXIT_GAME:
				PostQuitMessage(0);
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