#include "homies.h"
#include "homies_i.h"

#include "homies_type_map.h"
#include "homies_type_cursor.h"

#define GAMEFONTIND 0

typedef struct _game {
	hCAMERA	 cam;		//camera
	hGCURSOR cursor;	//the cursor
	hGMAP	 map;		//the map
	timer	 timeDelay;	//delay for time (sec)
	hFNT     gameFont;	//display for time, score and level
} game;

typedef enum {
	eEXIT_NO,
	eEXIT_MAINMENU,
	eEXIT_VICTORY,
	eEXIT_LOSE,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

static unsigned int g_victoyTxtID;
static int			g_curTime=0;	//current time (decreases until it reaches zero)

static game *g_theGame=0;

static bool g_showCam=false;

PRIVATE void _GameTerm()
{
	GFXEnableLight(false, 0xffffffff);
	LightReset();

	if(g_theGame)
	{
		CameraDestroy(&g_theGame->cam);
		CursorDestroy(&g_theGame->cursor);
		MapDestroy(&g_theGame->map);

		MemFree((void**)&g_theGame); g_theGame=0;
	}
}

PRIVATE RETCODE _GameInit()
{
	_GameTerm();

	GFXEnableLight(true, 0xffffffff);

	if(MemAlloc((void**)&g_theGame, sizeof(game), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate game", "_GameInit"); return RETCODE_FAILURE; }

	TimerInit(&g_theGame->timeDelay, 1, 1);

	g_theGame->map = MapLoad(g_gameMaps.maps[g_gameMaps.curMap].mapFile, &g_curTime);

	g_theGame->cursor = CursorCreate(CURSORFILE);
	CursorSetToMap(g_theGame->cursor, g_theGame->map, 0, 0);

	D3DXVECTOR3 vEye(0,0,0);
	g_theGame->cam = CameraCreate(vEye, 0, 0, 0);

	g_theGame->gameFont = MenuFontGet(GAMEFONTIND);

	return RETCODE_SUCCESS;
}

PRIVATE void _GameUpdateInput()
{
	hHOMIE thisHomie;
	thisHomie = CursorGetHomie(g_theGame->cursor);

	if(InputKbIsReleased(kCodeUp))
	{
		if(!thisHomie)
			CursorMoveToMap(g_theGame->cursor, g_theGame->map, 0,-1);
		else
			HomieMove(thisHomie, eDIR_NORTH, g_theGame->map);
	}
	else if(InputKbIsReleased(kCodeDown))
	{
		if(!thisHomie)
			CursorMoveToMap(g_theGame->cursor, g_theGame->map, 0,1);
		else
			HomieMove(thisHomie, eDIR_SOUTH, g_theGame->map);
	}

	if(InputKbIsReleased(kCodeLeft))
	{
		if(!thisHomie)
			CursorMoveToMap(g_theGame->cursor, g_theGame->map, -1,0);
		else
			HomieMove(thisHomie, eDIR_WEST, g_theGame->map);
	}
	else if(InputKbIsReleased(kCodeRight))
	{
		if(!thisHomie)
			CursorMoveToMap(g_theGame->cursor, g_theGame->map, 1,0);
		else
			HomieMove(thisHomie, eDIR_EAST, g_theGame->map);
	}

	if(InputKbIsReleased(kCodeSpc))
	{
		if(CursorGetHomie(g_theGame->cursor))
			CursorUnPick(g_theGame->cursor, g_theGame->map);
		else
			CursorPickUp(g_theGame->cursor, g_theGame->map);
	}

	if(InputKbIsReleased(kCodeEsc))
		g_exit=eEXIT_LOSE;

	if(InputKbIsReleased(kCodeF12))
		g_showCam = !g_showCam;
}

PRIVATE void _GameDisplay()
{
	GFXClear(0, 0, 0);   //clears screen within rects with given color

	MapDisplay(g_theGame->map);
	CursorDisplay(g_theGame->cursor);

	//display the time, score, level...
	GFXBegin();


	FontPrintf2D(g_theGame->gameFont, 0,0, 0xffff0000, "Time: %d", g_curTime);

	FontPrintf2D(g_theGame->gameFont, 150,0, 0xff00ff00, "Level: %d", g_gameMaps.curMap);

	FontPrintf2D(g_theGame->gameFont, 300,0, 0xff0000ff, "Score: %d", g_gameScore);

	GFXEnd();
}

PROTECTED RETCODE GameProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;
		_GUTBKFXEnterProc(eBKFX_FADEINOUT,0);
		_GameInit();
		break;
	case GMSG_UPDATE:
		if(g_exit==eEXIT_NO)
		{
			GUTPlayRandMusic(eMUSIC_RANDOFFSET, 0);

			if(!MapIsZoomingIn(g_theGame->map))
			{
				_GameUpdateInput();

				if(TimerEllapse(&g_theGame->timeDelay))
				{
					g_curTime--;

					if(g_curTime <= 0)
						g_exit = eEXIT_LOSE;
				}

#ifndef NDEBUG
				MapControlCamera(g_theGame->map);
#endif
			}

			if(MapUpdate(g_theGame->map) == MapGetNumGoal(g_theGame->map)
				&& !CursorGetHomie(g_theGame->cursor))
				g_exit = eEXIT_VICTORY;

			if(g_exit!=eEXIT_NO)
			{
				switch(g_exit)
				{
				case eEXIT_MAINMENU:
					_GUTBKFXExitProc(eBKFX_FADEINOUT,0);
					break;
				case eEXIT_VICTORY:
					g_victoyTxtID = GUTGetVictoryImgID();
					_GUTBKFXExitProc(eBKFX_IMGFADEINOUT, g_victoyTxtID);
					break;
				case eEXIT_LOSE:
					if(g_gameLives > 0)
						_GUTBKFXExitProc(eBKFX_IMGFADEINOUT, LOSEIMGID);
					else
						_GUTBKFXExitProc(eBKFX_IMGFADEINOUT, GAMEOVERIMGID);
					break;
				}
			}
		}
		break;

	case GMSG_DISPLAY:
		_GameDisplay();

		if(g_showCam)
			MapDisplayCamera(g_theGame->map, g_theGame->gameFont, 0, 100);

		if(_GUTBKFXUpdate() == RETCODE_BREAK && g_exit != eEXIT_NO)
		{
			SENDPROCMSG(GMSG_DESTROY, 0, 0);

			switch(g_exit)
			{
			case eEXIT_MAINMENU:
				SETPROCTYPE(eMainMenu);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			case eEXIT_VICTORY:
				SETPROCTYPE(eWin);
				SENDPROCMSG(GMSG_INIT, (WPARAM)g_victoyTxtID, (LPARAM)g_curTime);
				break;
			case eEXIT_LOSE:
				if(g_gameLives > 0)
				{
					SETPROCTYPE(eLose);
					SENDPROCMSG(GMSG_INIT, 0, 0);
				}
				else
				{
					SETPROCTYPE(eGameOver);
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
		_GameTerm();
		break;
	}

	return RETCODE_SUCCESS;
}