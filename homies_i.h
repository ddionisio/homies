#ifndef _homies_i_h
#define _homies_i_h

#include "homies_types.h"

/************************
*   FLAGS/TYPES/DEFINES	*
************************/

/************
*   Macros	*
************/

//use this for sending a message to the current proc
//DON'T USE THE GLOBAL PROC!  Unless you know what you are doing...
#define SENDPROCMSG(msg, wP, lP) (g_gameProc[g_gameCurProc](msg,wP,lP))

#define SENDTHISPROCMSG(proc, msg, wP, lP) (proc(msg,wP,lP))

//use this to get the current proc type
#define GETPROCTYPE() (g_curProc)

//use this to change the proc
//make sure you send a GMSG_DESTROY msg to the last game proc 
#define SETPROCTYPE(procType) (g_gameCurProc=(procType))

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam,
        LPARAM lparam);

PROTECTED RETCODE MainMenuProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE GameProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE GameOverProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE GotHiScoreProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE LoseProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE TopTenProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE WinProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE WinAllProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE EditorProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE EditNewProc(DWORD msg, WPARAM wParam, LPARAM lParam);
PROTECTED RETCODE EditMapProc(DWORD msg, WPARAM wParam, LPARAM lParam);

/**************************
*   data types for game   *
**************************/

/*typedef enum {
	MENU_MAIN,			//The main menu
	MENU_MAX
} eMenuType;*/

typedef enum {
	eMUSIC_GAMEOVER,
	eMUSIC_TOPSCORE,
	eMUSIC_END,
	eMUSIC_RANDOFFSET
} eMusicType;

typedef struct _nmap {
	int maxTime;				//max time in seconds
	char mapFile[MAXCHARBUFF];	//map file
} nmap;

typedef struct _mapList {
	int numMap;			//number of maps
	int curMap;			//current map
	nmap *maps;			//the maps (numMap)
} mapList;

typedef struct _victoryImgList {
	int numVictory;
	unsigned int *txtIDs; //texture IDs of victory images (numVictory)
} victoryImgList;

typedef struct _nscore {
	char name[MAXCHARBUFF];
	unsigned int score;
} nscore;

typedef struct _musicList {
	int numMusic;			//number of music
	int curMusic;			//current music playing
	HMUSIC *music;			//array of music(numMusic)
} musicList;

//this is used by editmap proc for creating/editing map
typedef struct _mapEditInfo {
	char mapName[MAXCHARBUFF];
	int row,col; //size, applied only if bCreateNew == true
	bool bCreateNew;	//tells us to append later
} mapEditInfo;

//be careful with 'em...
extern gameCfg	g_gameCfg;				  //the game config
extern GAMEPROC g_gameProc[eNumGameProc]; //holds all callbacks for the game
extern eGameProcType g_gameCurProc;			  //current proc we are using

extern timer g_gameTimer;		//game delay
extern timer g_gameFrameTimer;	//frame delay

extern mapList g_gameMaps;					//includes all maps
extern victoryImgList g_gameVictoryImgList; //includes all victory images

extern unsigned int g_gameScore;	//the score of current game
extern unsigned int g_gameLives;	//number of lives

extern nscore g_gameScores[MAXSCORE];	//top-ten people

extern hBKFX g_gameBKFX;

extern musicList g_gameMusic;			//contains all music background (for menu, in-game)

extern unsigned int	g_gameRandSeed;		  //the random seed value

/****************************************
*   access functions for game config    *
****************************************/
PROTECTED inline int   GCFGGetSVol() { return g_gameCfg.sVol; }
PROTECTED inline int   GCFGGetMVol() { return g_gameCfg.aVol; }
PROTECTED inline float GCFGGetHomieSpd() { return g_gameCfg.homieSpd; }

PROTECTED RETCODE _GameCfgLoad(const char *filename, gameCfg *pgCfg);

//save game cfg
PROTECTED RETCODE _GameCfgSave(const char *filename, gameCfg *pgCfg);

PROTECTED void _GUTBKFXEnterProc(eBKFXType type, unsigned int txtID);
PROTECTED RETCODE _GUTBKFXUpdate();
PROTECTED void _GUTBKFXExitProc(eBKFXType type, unsigned int txtID);

#endif