#include "homies.h"
#include "homies_i.h"

gameCfg	g_gameCfg={0};				  //the game config
GAMEPROC g_gameProc[eNumGameProc]={MainMenuProc, GameProc, GameOverProc,
GotHiScoreProc, LoseProc, TopTenProc, WinProc, WinAllProc, EditorProc,
EditNewProc, EditMapProc}; //holds all callbacks for the game
eGameProcType g_gameCurProc=eMainMenu;			  //current proc we are using

timer g_gameTimer={0};		//game delay
timer g_gameFrameTimer={0};	//frame delay

list<hOBJ> g_gameTileList;	//list of graphical tiles

mapList g_gameMaps={0};					 //includes all maps
victoryImgList g_gameVictoryImgList={0}; //includes all victory images

nscore g_gameScores[MAXSCORE];	//top-ten people

unsigned int g_gameScore=0;	//the score of current game
unsigned int g_gameLives=0;	//number of lives

hBKFX g_gameBKFX;

musicList g_gameMusic={0};			//contains all music background (for menu, in-game)

unsigned int	g_gameRandSeed=0;		  //the random seed value


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline,
        int ishow)
{
	HWND hwnd;

	if (FindWindow (WINNAME, WINNAME) != NULL)	// Check whether game is already running
		return 0;	// If so, terminate now

	//initialize!
	if(GameInit(&hwnd, hinstance) != RETCODE_SUCCESS)
		goto DEATH;


	//show window
	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);

	//execute game loop
	GameLoop(hwnd);

	//terminate
DEATH:
	GameDestroy();

	return 0;
}