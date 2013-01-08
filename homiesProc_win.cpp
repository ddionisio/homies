#include "homies.h"
#include "homies_i.h"
#include "homies_menu_id.h"

#define WINDELAY 10

#define WINFNTIND	3

typedef enum {
	eEXIT_NO,
	eEXIT_OK,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

static int g_timeAmt=0;

static hTXT g_bkgrnd=0;

static hFNT	g_fnt=0;

static timer g_delay={0};
static timer g_counterDelay={0};

PROTECTED RETCODE WinProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;
		g_timeAmt = (int)lParam;
		g_bkgrnd = TextureSearch((unsigned int)wParam);
		TextureAddRef(g_bkgrnd);

		g_fnt=MenuFontGet(WINFNTIND);

		TimerInit(&g_delay, 1, WINDELAY);
		TimerInit(&g_counterDelay, 1000, 10);
		break;

	case GMSG_UPDATE:
		if(g_exit==eEXIT_NO)
		{
			if(TimerEllapse(&g_counterDelay))
			{
				if(g_timeAmt > 0)
				{
					g_gameScore++;
					g_timeAmt--;
				}
			}

			if(TimerEllapse(&g_delay) || InputKbAnyKeyReleased())
			{
				g_gameScore += g_timeAmt;
				g_timeAmt=0;
				g_exit=eEXIT_OK;
			}

			if(g_exit!=eEXIT_NO)
				_GUTBKFXExitProc(eBKFX_FADEINOUT,0);
		}
		break;

	case GMSG_DISPLAY:
		{
			GFXClear(0, 0, 0);   //clears screen within rects with given color

			char buff[MAXCHARBUFF];

			SIZE fntSz;

			float x=GFXGetScrnSize().cx/2;
			float y=GFXGetScrnSize().cy/4;

			GFXBltModeEnable(false);
			
			TextureStretchBlt(g_bkgrnd, 0,0, GFXGetScrnSize().cx, GFXGetScrnSize().cy, 0,0,0);

			sprintf(buff, "Time: %d", g_timeAmt);
			FontGetStrSize(g_fnt, buff, &fntSz);
			FontPrintf2D(g_fnt, x-(fntSz.cx/2), y, 0x7fffffff, buff);

			sprintf(buff, "Score: %d", g_gameScore);
			FontGetStrSize(g_fnt, buff, &fntSz);
			FontPrintf2D(g_fnt, x-(fntSz.cx/2), y+fntSz.cy, 0x7fffffff, buff);
			GFXBltModeDisable();

			if(_GUTBKFXUpdate() == RETCODE_BREAK && g_exit != eEXIT_NO)
			{
				SENDPROCMSG(GMSG_DESTROY, 0, 0);

				g_gameMaps.curMap++;

				switch(g_exit)
				{
				case eEXIT_OK:
					//max level
					if(g_gameMaps.curMap >= g_gameMaps.numMap)
					{
						//we finished all the maps!
						SETPROCTYPE(eWinAll);
						SENDPROCMSG(GMSG_INIT, 0, 0);
					}
					else
					{
						SETPROCTYPE(eGame);
						SENDPROCMSG(GMSG_INIT, 0, 0);
					}
					break;
				}
			}
			
			GFXUpdate(0, 0, 0);  //update frame on display
		}
		break;

	case GMSG_LOAD:
		break;

	case GMSG_DESTROY:
		TextureDestroy(&g_bkgrnd);
		break;
	}

	return RETCODE_SUCCESS;
}