#include "homies.h"
#include "homies_i.h"

//This is where all the libraries are initialized...

PRIVATE RETCODE _GameCfgExec(gameCfg *pgCfg, HWND *hwnd, HINSTANCE hinst)
{
	//build windows based on flag
	if(TESTFLAGS(pgCfg->status, CFG_FULLSCREEN))
	{
		*hwnd = Create_Window_Popup(WINNAME, WinProc, hinst, NULL, NULL, 
							    0, 0, pgCfg->gfxMode.width+WINOVRLPX, pgCfg->gfxMode.height+WINOVRLPY);
	}
	else
	{
		*hwnd = Create_Window_Overlapped(WINNAME, 0, 0, WinProc, hinst, NULL, NULL,
							    0, 0, pgCfg->gfxMode.width+WINOVRLPX, pgCfg->gfxMode.height+WINOVRLPY);
	}

	//set hwnd and hinst
	pgCfg->config.hWnd = *hwnd; pgCfg->config.hInst = hinst;

	//initialize memory
	if(MemInit(&pgCfg->config) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	//initialize input
	//if(INPXInit(*hwnd, hinst, INPTYPE, INPMAXBUFF) != RETCODE_SUCCESS)
	//	return RETCODE_FAILURE;

	//initialize graphics
	if(TESTFLAGS(pgCfg->status, CFG_FULLSCREEN))
	{
		if(GFXInit(*hwnd, &pgCfg->gfxMode) != RETCODE_SUCCESS)
			return RETCODE_FAILURE;
	}
	else
	{
		if(GFXInit(*hwnd, 0) != RETCODE_SUCCESS)
			return RETCODE_FAILURE;
	}

	//initialize audio
	if (!BASS_Init(-1,44100,0,*hwnd)) {
    // couldn't initialize device, so use no sound
    BASS_Init(-2,44100,0,*hwnd);
	}

	//set up audio's music/sound volume
	BASS_SetGlobalVolumes(pgCfg->aVol, pgCfg->sVol, pgCfg->aVol);

	BASS_Start();

	//set up delay timers
	TimerInit(&g_gameTimer, MILLISECDELAY, pgCfg->delayGame);
	TimerInit(&g_gameFrameTimer, MILLISECDELAY, pgCfg->delayFrame);

	return RETCODE_SUCCESS;
}

//initialize font list for menu
PRIVATE RETCODE _GameInitFontList()
{
	FILE *fp = fopen(MENUFONTS, "rt");

	if(fp)
	{
		if(!MenuFontLoad(fp))
		{ ASSERT_MSG(0, "Error loading menu fonts", "Error in _GameInitFontList"); return RETCODE_FAILURE; }

		fclose(fp);
		return RETCODE_SUCCESS;
	}

	return RETCODE_FAILURE;
}

//initialize game, loads up the config within the given gameDir
PUBLIC RETCODE GameInit(HWND *hwnd, HINSTANCE hinst)
{
	TimerQuery();
	ResetStartTime ( );

	g_gameRandSeed = time(0);
	srand(g_gameRandSeed);

	//load game config from file
	if(_GameCfgLoad(GAMECFG, &g_gameCfg) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to load game config", "Error in GameInit"); return RETCODE_FAILURE; }

	//execute game config
	if(_GameCfgExec(&g_gameCfg, hwnd, hinst) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Error loading configuration", "Error in GameInit"); return RETCODE_FAILURE; }

	//initialize Menu
	if(MenuInit() != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	//set up list files
	TextureSetListFile(TEXTURELIST);
	MDLSetListFile(MODELLIST);

	//set up fonts
	if(_GameInitFontList() != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	//other stuff
	GUTInitMapList(MAPLIST);
	GUTInitVictoryImageList(VICTORYIMGLIST);
	GUTInitScoreList(SCORELIST);
	GUTLoadMusicList(MUSICLIST);

	//done!

	return RETCODE_SUCCESS;
}