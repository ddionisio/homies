#include "homies.h"
#include "homies_i.h"
#include "homies_menu_id.h"

#define GAMEOVERDELAY 5

typedef enum {
	eEXIT_NO,
	eEXIT_OK,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

static hTXT g_gameoverImg=0;
static timer g_timer={0};

PROTECTED RETCODE GameOverProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;
		g_gameoverImg = TextureSearch(GAMEOVERIMGID);
		TextureAddRef(g_gameoverImg);
		TimerInit(&g_timer, 1, GAMEOVERDELAY);
		GUTPlayMusic(eMUSIC_GAMEOVER);
		break;
	case GMSG_UPDATE:
		if(g_exit==eEXIT_NO)
		{
			if(TimerEllapse(&g_timer) || InputKbAnyKeyReleased())
				g_exit=eEXIT_OK;

			if(g_exit!=eEXIT_NO)
				_GUTBKFXExitProc(eBKFX_FADEINOUT,0);
		}
		break;

	case GMSG_DISPLAY:
		GFXClear(0, 0, 0);   //clears screen within rects with given color

		TextureStretchBlt(g_gameoverImg, 0,0, GFXGetScrnSize().cx, GFXGetScrnSize().cy, 0,0,0);

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
		TextureDestroy(&g_gameoverImg);
		break;
	}

	return RETCODE_SUCCESS;
}