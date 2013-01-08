#include "homies.h"
#include "homies_i.h"
#include "homies_menu_id.h"

#include "Parser\\parser.h"

#include "menu\textbox.h"

#define EDITORFILE "editor.txt"

static HMC g_win=0;

typedef enum {
	eEXIT_NO,
	eEXIT_EDIT,
	eEXIT_NEW,
	eEXIT_CANCEL,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

PROTECTED int EditorWinProc(int nWinId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buff[MAXCHARBUFF]={0};

	switch(msg)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		switch(HIWORD(wParam))
		{
		case MID_EDITOR_LFTBTN:
			if(g_gameMaps.curMap > 0)
				g_gameMaps.curMap--;

			sprintf(buff, "%d. %s", g_gameMaps.curMap, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITOR_TXTBOX), buff);
			break;
		case MID_EDITOR_RGTBTN:
			if(g_gameMaps.curMap < g_gameMaps.numMap-1)
				g_gameMaps.curMap++;
			
			sprintf(buff, "%d. %s", g_gameMaps.curMap, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITOR_TXTBOX), buff);
			break;
		case MID_EDITOR_EDIT:
			g_exit=eEXIT_EDIT;
			break;
		case MID_EDITOR_DELETE:
			GUTDeleteCurrentMap();
			GUTSaveMapList(MAPLIST);
			sprintf(buff, "%d. %s", g_gameMaps.curMap, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITOR_TXTBOX), buff);
			break;
		case MID_EDITOR_ADD:
			g_exit=eEXIT_NEW;
			break;
		case MID_EDITOR_CANCEL:
			g_exit=eEXIT_CANCEL;
			break;
		}
		break;
	}

	return 1;
}

PROTECTED RETCODE EditorProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;

		_GUTBKFXEnterProc(eBKFX_FADEINOUT,0);

		g_win = MenuGetItem(MID_EDITOR);

		if(!g_win)
		{
			char path[MAXCHARBUFF];

			//load up the menu
			strcpy(path, WINDOWDIR);
			strcat(path, EDITORFILE);

			FILE *fp = fopen(path, "rt");
			MenuLoadFromFile (fp);
			fclose(fp);

			g_win = MenuGetItem(MID_EDITOR);

			WindowChangeSize((HWINDOW)g_win, GFXGetScrnSize().cx, GFXGetScrnSize().cy);
			WindowNewProc((HWINDOW)g_win, EditorWinProc);
		}
		else
			MenuShowItem(g_win, HMC_ACTIVE, 1);

		char buff[MAXCHARBUFF];
		sprintf(buff, "%d. %s", g_gameMaps.curMap, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));
		TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITOR_TXTBOX), buff);

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

			mapEditInfo mapEdit={0};

			switch(g_exit)
			{
			case eEXIT_EDIT:
				SETPROCTYPE(eEditMap);

				//hardcoded for now...
				strcpy(mapEdit.mapName, GetFilePart(g_gameMaps.maps[g_gameMaps.curMap].mapFile));

				SENDPROCMSG(GMSG_INIT, (WPARAM)&mapEdit, 0);
				break;
			case eEXIT_NEW:
				SETPROCTYPE(eEditNew);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			case eEXIT_CANCEL:
				SETPROCTYPE(eMainMenu);
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