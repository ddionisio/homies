#ifndef _homies_types_h
#define _homies_types_h

#include "common.h"
#include "memory\memory.h"
#include "graphicsX\gdiX.h"
#include "graphicsX\gdiX_bkgrndFX.h"
//#include "inputx\inputx.h"
#include "MyInput\input.h"
#include "timer\timer.h"
#include "menu\menu.h"
#include "menu\menuloader.h"
#include "menu\fonts.h"
#include "menu\window.h"

#include "bass.h"

//constants!
#define WINNAME			"Supper Time" //windows class name

#define INPTYPE			eNonExcForeground
#define INPMAXBUFF		16
#define WINOVRLPX		8			  //window overlapped x thing
#define WINOVRLPY		27			  //window overlapped y thing

#define MILLISECDELAY	1000		  //number of millisecond per second

#define MAXCONTINUE		3

#define MAXSCORE		10

#define CAMSPD			10

//constant directories
#define MODELDIR		"Homies\\Models\\"
#define TEXTUREDIR		"Homies\\Textures\\"
#define SPRITEDIR		"Homies\\Sprites\\"
#define SOUNDDIR		"Homies\\Sounds\\"
#define MUSICDIR		"Homies\\Music\\"
#define WINDOWDIR		"Homies\\Windows\\"
#define OBJECTDIR		"Homies\\Objects\\"	  //this includes 3d objs and key frames
#define MAPDIR			"Homies\\Maps\\"

//constant files
#define GAMECFG			"Homies\\game.cfg"
#define TEXTURELIST		"Homies\\texture.lst"
#define MODELLIST		"Homies\\model.lst"
#define MENUFONTS		"Homies\\MenuFonts.lst"
#define TILELIST		"Homies\\tile.lst"
#define MAPLIST			"Homies\\map.lst"
#define VICTORYIMGLIST	"Homies\\victoryImg.lst"
#define SCORELIST		"Homies\\score.lst"
#define MUSICLIST		"Homies\\music.lst"
#define CURSORFILE		"Homies\\Objects\\hand.txt"

#define LOSEIMGID		27
#define GAMEOVERIMGID	25

//config flags
#define CFG_FULLSCREEN	1

//enums
typedef enum {
	eMainMenu,			//the main menu screen
	eGame,				//the actual game
	eGameOver,			//GAME OVER!
	eGotHiScore,		//You made it within the top-ten
	eLose,				//you lose
	eTopTen,			//top ten
	eWin,				//you won!?
	eWinAll,			//you won every level
	eEditor,
	eEditNew,
	eEditMap,
	eNumGameProc
} eGameProcType;

typedef enum {
	GMSG_INIT,			//inclusive data proc initialization
	GMSG_UPDATE,		//game update for proc (includes input)
	GMSG_DISPLAY,		//game display everything
	GMSG_SAVE,			//when saving is desired
	GMSG_LOAD,			//when loading is desired
	GMSG_PAUSE,			//pause is called... wparam == 1 means pause, 0 is un-pause
	GMSG_DESTROY		//inclusive data proc destruction
} eGameMsg;

typedef enum {
	eDIR_SOUTH,
	eDIR_NORTH,
	eDIR_EAST,
	eDIR_WEST,
	eDIR_MAX
} eDIR;

//function pointer defines
typedef RETCODE (* GAMEPROC) (DWORD msg, WPARAM wParam, LPARAM lParam);

/********************************************************************
*																	*
*							Handles									*
*																	*
********************************************************************/

typedef struct _homie	*hHOMIE;
typedef struct _tile	*hTILE;
typedef struct _gmap	*hGMAP;
typedef struct _gcursor	*hGCURSOR;

/********************************************************************
*																	*
*							Structures								*
*																	*
********************************************************************/

typedef struct _gameCfg {
	CONFIG			config;		//config defined in common.h
	gfxDisplayMode  gfxMode;	//graphics config
	FLAGS			status;		//state stuff, eg: CFG_FULLSCREEN
	int				sVol;		//sound volume
	int				aVol;		//aurial (music) volume

	int	  delayGame;			//game delay
	int	  delayFrame;			//frame delay

	//insert more
	float homieSpd;				//speed of homie
} gameCfg, *PgameCfg;

#endif