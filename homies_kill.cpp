#include "homies.h"
#include "homies_i.h"

//terminates game and engine
PUBLIC void GameDestroy()
{
	//destroy all other stuff
	GUTClearMapList();
	GUTClearVictoryImageList();
	BKFXDestroy(&g_gameBKFX);

	GUTClearMusicList();

	GUTSaveScoreList(SCORELIST);

	//destroy all engines
	MenuFontUnload();
	MenuTerm(1);
	GFXDestroy();
	BASS_Free();
	//INPXDestroy();
	MemTerm("memdebug.txt");
}