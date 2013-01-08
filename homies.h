#ifndef _homies_h
#define _homies_h

#include "mywin.h"
#include "homies_types.h"
#include <time.h>
#include <stdarg.h>

//initialize game, loads up the config within the given gameDir
PUBLIC RETCODE GameInit(HWND *hwnd, HINSTANCE hinst);

//terminates game and engine
PUBLIC void GameDestroy();

//The game loop
PUBLIC RETCODE GameLoop(HWND hwnd);

//creates object from file
PUBLIC hOBJ GUTCreateObj(unsigned int ID, const char *filename);

//creates a bunch of light from file
PUBLIC RETCODE GUTLoadLights(const char *filename);

//load/search graphical tile
PUBLIC hOBJ GUTTileSearch(unsigned int ID);

//loads all tile to given list
PUBLIC void GUTTileLoadAll(list<int> *tileList);

//used to initialize the list of maps for game
PUBLIC void GUTInitMapList(const char *filename);

//used to clear the list of maps
PUBLIC void GUTClearMapList();

//used to delete the current map in the map list
//sets current to 0
PUBLIC void GUTDeleteCurrentMap();

//used to insert a map after the current
PUBLIC void GUTAddAfterCurrent(const char *filename);

//used to save the map list
PUBLIC void GUTSaveMapList(const char *filename);

//used to initialize the list of victory image list
PUBLIC void GUTInitVictoryImageList(const char *filename);

//used to clean up the victory image list
PUBLIC void GUTClearVictoryImageList();

//used to get victory txt ID randomly
PUBLIC unsigned int GUTGetVictoryImgID();

//used to load the score list
PUBLIC void GUTInitScoreList(const char *filename);

//used to check if current game score is within top ten
PUBLIC bool GUTCheckScore();

//adds a new person to top-ten with g_gameScore
PUBLIC void GUTAddScore(const char *name);

//displays top ten
PUBLIC void GUTDisplayScoreList(hFNT fnt, float x, float y);

//used to save the score list
PUBLIC void GUTSaveScoreList(const char *filename);

//used to load the music list
PUBLIC void GUTLoadMusicList(const char *filename);

//used to clear the music list
PUBLIC void GUTClearMusicList();

//used to play a certain music index
//will not play anything if ind exceeds num music
PUBLIC void GUTPlayMusic(unsigned int ind);

//used to play a random song within the music list
//this will only change the music if the last music stopped playing
//this is ideally used in update loops
PUBLIC void GUTPlayRandMusic(unsigned int minInd, unsigned int maxInd);

//stops music
PUBLIC void GUTStopMusic();

#endif