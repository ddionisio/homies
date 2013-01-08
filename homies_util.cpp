#include "homies.h"
#include "homies_i.h"

//returns -1 if error occures
//otherwise, children are added
//the ID for each child is ID+(1,2,...n)
PRIVATE int _GUTCreateChild(hOBJ parent, unsigned int ID, 
							 unsigned int numChild, FILE *fp)
{
	hOBJ objChild;
	unsigned int mdlID, objID=ID+1, numSubChild, syncIt;
	char keyFile[MAXCHARBUFF];
	float trans[eMaxPt];

	for(int i = 0; i < numChild; i++, objID++)
	{
		fscanf(fp, "mdlID=%d\n", &mdlID);
		fscanf(fp, "animKey=%s\n", keyFile);
		fscanf(fp, "syncState=%d\n", &syncIt);
		fscanf(fp, "localTrans=%f,%f,%f\n", &trans[eX], &trans[eY], &trans[eZ]);
		fscanf(fp, "numChild=%d\n", &numSubChild);

		objChild = OBJCreate(objID, 
			MDLSearch(mdlID), 0,0,0,eOBJ_ANIMATE,
			keyFile);

		if(!objChild)
			return -1;

		//attach child
		OBJAddChild(parent, objChild, syncIt > 0 ? true : false);

		//go get other children for this child obj.
		if(numSubChild > 0)
		{
			objID = _GUTCreateChild(objChild, objID, numChild, fp);
			
			if(objID < 0) break;
		}
	}

	return objID;
}

//creates object from file
PUBLIC hOBJ GUTCreateObj(unsigned int ID, const char *filename)
{
	hOBJ object=0;

	unsigned int mdlID, numChild;

	//load up file
	FILE *fp = fopen(filename, "rt");

	if(!fp) 
	{ ASSERT_MSG(0, "Unable to load object file", "Error in GUTCreateObj"); return 0; }

	//load up the big daddy
	char keyFile[MAXCHARBUFF];
	float trans[eMaxPt];

	fscanf(fp, "mdlID=%d\n", &mdlID);
	fscanf(fp, "animKey=%s\n", keyFile);
	fscanf(fp, "trans=%f,%f,%f\n", &trans[eX], &trans[eY], &trans[eZ]);
	fscanf(fp, "numChild=%d\n", &numChild);

	if(stricmp(keyFile, "none") == 0)
		memset(keyFile, 0, sizeof(keyFile));

	//get object
	object = OBJCreate(ID, MDLSearch(mdlID), trans[eX],trans[eY],trans[eZ], eOBJ_ANIMATE,
		keyFile);

	//load up the children
	if(object)
		_GUTCreateChild(object, ID, numChild, fp);

	OBJFixOrientation(object);

	fclose(fp);

	return object;
}

//creates a bunch of light from file
PUBLIC RETCODE GUTLoadLights(const char *filename)
{
	D3DLIGHT8 lightStruct;

	//load up file
	FILE *fp = fopen(filename, "rt");

	if(fp)
	{
		char buff[MAXCHARBUFF];

		//get num light
		int max;
		fscanf(fp, "numLight=%d\n", &max);

		for(int i = 0; i < max; i++)
		{
			//get light type
			fscanf(fp, "%s\n", buff);

			if(stricmp("point", buff) == 0)
				lightStruct.Type = D3DLIGHT_POINT;
			else if(stricmp("spot", buff) == 0)
				lightStruct.Type = D3DLIGHT_SPOT;
			else if(stricmp("directional", buff) == 0)
				lightStruct.Type = D3DLIGHT_DIRECTIONAL;

			//get diffuse color
			fscanf(fp, "diff: %f,%f,%f,%f\n", 
				&lightStruct.Diffuse.r, &lightStruct.Diffuse.g, &lightStruct.Diffuse.b, &lightStruct.Diffuse.a);

			//get specular color
			fscanf(fp, "spec: %f,%f,%f,%f\n", 
				&lightStruct.Specular.r, &lightStruct.Specular.g, &lightStruct.Specular.b, &lightStruct.Specular.a);

			//get ambient color
			fscanf(fp, "amb: %f,%f,%f,%f\n", 
				&lightStruct.Ambient.r, &lightStruct.Ambient.g, &lightStruct.Ambient.b, &lightStruct.Ambient.a);

			//get position
			fscanf(fp, "pos: %f,%f,%f\n", &lightStruct.Position.x, &lightStruct.Position.y, &lightStruct.Position.z);

			//get direction
			fscanf(fp, "dir: %f,%f,%f\n", &lightStruct.Direction.x, &lightStruct.Direction.y, &lightStruct.Direction.z);

			//get range
			fscanf(fp, "rng: %f\n", &lightStruct.Range);

			//get falloff
			fscanf(fp, "foff: %f\n", &lightStruct.Falloff);

			//get Attenuation 0/1/2
			fscanf(fp, "atten012: %f,%f,%f\n", &lightStruct.Attenuation0, &lightStruct.Attenuation1, &lightStruct.Attenuation2);

			//get inner/outer angle
			fscanf(fp, "inAngle: %f\n", &lightStruct.Theta);
			fscanf(fp, "outAngle: %f\n", &lightStruct.Phi);

			LightAdd(&lightStruct);
		}

		fclose(fp);
		return RETCODE_SUCCESS;
	}

	return RETCODE_FAILURE;
}

//load/search graphical tile
PUBLIC hOBJ GUTTileSearch(unsigned int ID)
{
	hOBJ thisOBJ;
	FILE *fp = fopen(TILELIST, "rt");

	if(fp)
	{
		unsigned int theID;
		char path[MAXCHARBUFF];

		while(!feof(fp))
		{
			fscanf(fp, "%d %[^\n]\n", &theID, path);

			if(theID == ID)
			{
				thisOBJ = GUTCreateObj(theID, path);
				break;
			}
		}

		fclose(fp);
	}

	return thisOBJ;
}

//loads all tile Index to given list
PUBLIC void GUTTileLoadAll(list<int> *tileList)
{
	hOBJ thisOBJ;
	FILE *fp = fopen(TILELIST, "rt");

	if(fp && tileList)
	{
		unsigned int theID;
		char path[MAXCHARBUFF];

		while(!feof(fp))
		{
			fscanf(fp, "%d %[^\n]\n", &theID, path);

			thisOBJ = GUTCreateObj(theID, path);

			if(thisOBJ)
				tileList->push_back(theID);
		}

		fclose(fp);
	}
}

//used to clear the list of maps
PUBLIC void GUTClearMapList()
{
	if(g_gameMaps.maps)
		MemFree((void**)&g_gameMaps.maps);
	g_gameMaps.numMap = 0;
	g_gameMaps.curMap = 0;
}

//used to initialize the list of maps for game
PUBLIC void GUTInitMapList(const char *filename)
{
	FILE *fp = fopen(filename, "rt");

	if(fp)
	{
		GUTClearMapList();

		fscanf(fp, "numMap=%d\n", &g_gameMaps.numMap);

		if(g_gameMaps.numMap > 0)
		{
			if(MemAlloc((void**)&g_gameMaps.maps, sizeof(nmap)*g_gameMaps.numMap, M_ZERO) != RETCODE_SUCCESS)
			{ ASSERT_MSG(0, "Unable to allocate game map list", "GUTInitMapList"); return; }

			for(int i = 0; i < g_gameMaps.numMap; i++)
				fscanf(fp, "%[^\n]\n", g_gameMaps.maps[i].mapFile);
		}

		fclose(fp);
	}
	else
		ASSERT_MSG(0, "Unable to open map list", "GUTInitMapList");
}

//used to delete the current map in the map list
//sets current to 0
PUBLIC void GUTDeleteCurrentMap()
{
	if(g_gameMaps.numMap > 1)
	{
		nmap *newMapList;
		int newMax = g_gameMaps.numMap-1;

		if(MemAlloc((void**)&newMapList, sizeof(nmap)*(newMax), M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate new map stuff", "GUTDeleteCurrentMap"); return; }

		for(int i = 0, j = 0; j < newMax; i++)
		{
			if(i != g_gameMaps.curMap)
			{
				memcpy(&newMapList[j], &g_gameMaps.maps[i], sizeof(nmap));
				j++;
			}
		}

		int curMap = g_gameMaps.curMap;
		GUTClearMapList();

		g_gameMaps.maps = newMapList;
		g_gameMaps.numMap = newMax;

		if(curMap >= g_gameMaps.numMap)
			g_gameMaps.curMap = g_gameMaps.numMap-1;
		else
			g_gameMaps.curMap = curMap;
	}
}

//used to insert a map after the current
PUBLIC void GUTAddAfterCurrent(const char *filename)
{
	nmap *newMapList;
	int newMax = g_gameMaps.numMap+1;

	if(MemAlloc((void**)&newMapList, sizeof(nmap)*(newMax), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new map stuff", "GUTDeleteCurrentMap"); return; }

	for(int i = 0, j = 0; j < newMax; i++, j++)
	{
		if(i == g_gameMaps.curMap)
		{
			if(g_gameMaps.numMap > 1)
			{
				memcpy(&newMapList[j], &g_gameMaps.maps[i], sizeof(nmap));
				j++;
			}

			strcpy(newMapList[j].mapFile, MAPDIR);
			strcat(newMapList[j].mapFile, filename);
		}
		else
			memcpy(&newMapList[j], &g_gameMaps.maps[i], sizeof(nmap));
	}

	GUTClearMapList();

	g_gameMaps.maps = newMapList;
	g_gameMaps.numMap = newMax;
	g_gameMaps.curMap = 0;
}

//used to save the map list
PUBLIC void GUTSaveMapList(const char *filename)
{
	FILE *fp = fopen(filename, "wt");

	if(fp)
	{
		fprintf(fp, "numMap=%d\n", g_gameMaps.numMap);

		for(int i = 0; i < g_gameMaps.numMap; i++)
			fprintf(fp, "%s\n", g_gameMaps.maps[i].mapFile);

		fclose(fp);
	}
	else
		ASSERT_MSG(0, "Unable to open map list", "GUTInitMapList");
}

//used to clean up the victory image list
PUBLIC void GUTClearVictoryImageList()
{
	if(g_gameVictoryImgList.txtIDs)
		MemFree((void**)&g_gameVictoryImgList.txtIDs);
}

//used to initialize the list of victory image list
PUBLIC void GUTInitVictoryImageList(const char *filename)
{
	FILE *fp = fopen(filename, "rt");

	if(fp)
	{
		GUTClearVictoryImageList();

		fscanf(fp, "numImg=%d\n", &g_gameVictoryImgList.numVictory);

		if(g_gameVictoryImgList.numVictory > 0)
		{
			if(MemAlloc((void**)&g_gameVictoryImgList.txtIDs, sizeof(unsigned int)*g_gameVictoryImgList.numVictory, M_ZERO) != RETCODE_SUCCESS)
			{ ASSERT_MSG(0, "Unable to allocate bunch of textures", "GUTInitVictoryImageList"); return; }

			for(int i = 0; i < g_gameVictoryImgList.numVictory; i++)
				fscanf(fp, "%d\n", &g_gameVictoryImgList.txtIDs[i]);
		}

		fclose(fp);
	}
}

//used to get victory txt ID randomly
PUBLIC unsigned int GUTGetVictoryImgID()
{
	return g_gameVictoryImgList.txtIDs[rand()%g_gameVictoryImgList.numVictory];
}

//used to load the score list
PUBLIC void GUTInitScoreList(const char *filename)
{
	FILE *fp = fopen(filename, "rt");

	if(fp)
	{
		for(int i = 0; i < MAXSCORE; i++)
			fscanf(fp, "%d %[^\n]\n", &g_gameScores[i].score, g_gameScores[i].name);

		fclose(fp);
	}
}

//used to check if current game score is within top ten
PUBLIC bool GUTCheckScore()
{
	for(int i = 0; i < MAXSCORE; i++)
		if(g_gameScore > g_gameScores[i].score)
			return true;

	return false;
}

//adds a new person to top-ten with g_gameScore
PUBLIC void GUTAddScore(const char *name)
{
	bool bAdded=false;
	nscore newScores[MAXSCORE];

	for(int i = 0, j = 0; j < MAXSCORE; i++, j++)
	{
		if(g_gameScore > g_gameScores[i].score && !bAdded)
		{
			newScores[j].score = g_gameScore;
			strcpy(newScores[j].name, name);
			bAdded=true;
			i--;
		}
		else
		{
			newScores[j].score = g_gameScores[i].score;
			strcpy(newScores[j].name, g_gameScores[i].name);
		}
	}

	memcpy(g_gameScores, newScores, sizeof(nscore)*MAXSCORE);
}

#define XSCOREOFFSET 420
#define XNAMEOFFSET	50

//displays top ten
PUBLIC void GUTDisplayScoreList(hFNT fnt, float x, float y)
{
	SIZE fsz; FontGetCharSize(fnt, 'X', &fsz);

	float thisX=x;
	float thisY=y;

	unsigned int clr[4] = {0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00};
	unsigned int thisClr;

	for(int i = 0; i < MAXSCORE; i++)
	{
		thisClr = clr[i%4];
		FontPrintf2D(fnt, thisX, thisY, thisClr, "%d.", i+1);
		FontPrintf2D(fnt, thisX+XNAMEOFFSET, thisY, thisClr, "%s", g_gameScores[i].name);
		FontPrintf2D(fnt, thisX+XSCOREOFFSET, thisY, thisClr, "%d", g_gameScores[i].score);

		thisY+=fsz.cy;
	}
}

//used to save the score list
PUBLIC void GUTSaveScoreList(const char *filename)
{
	FILE *fp = fopen(filename, "wt");

	if(fp)
	{
		for(int i = 0; i < MAXSCORE; i++)
			fprintf(fp, "%d %s\n", g_gameScores[i].score, g_gameScores[i].name);

		fclose(fp);
	}
}

//used to clear the music list
PUBLIC void GUTClearMusicList()
{
	if(g_gameMusic.music)
	{
		for(int i = 0; i < g_gameMusic.numMusic; i++)
			BASS_MusicFree(g_gameMusic.music[i]);

		MemFree((void**)&g_gameMusic.music);
		g_gameMusic.numMusic=0;
		g_gameMusic.curMusic=0;
	}
}

//used to load the music list
PUBLIC void GUTLoadMusicList(const char *filename)
{
	FILE *fp = fopen(filename, "rt");

	char path[DMAXCHARBUFF], buff[MAXCHARBUFF];

	if(fp)
	{
		GUTClearMusicList();

		fscanf(fp, "numMusic=%d\n", &g_gameMusic.numMusic);

		if(g_gameMusic.numMusic)
		{
			if(MemAlloc((void**)&g_gameMusic.music, sizeof(HMUSIC)*g_gameMusic.numMusic, M_ZERO) != RETCODE_SUCCESS)
			{ ASSERT_MSG(0, "Unable to allocate music list", "GUTLoadMusicList"); return; }

			for(int i = 0; i < g_gameMusic.numMusic; i++)
			{
				fscanf(fp, "%[^\n]\n", buff);
				strcpy(path, MUSICDIR);
				strcat(path, buff);

				g_gameMusic.music[i] = 
					BASS_MusicLoad(FALSE, path, 0, 0, 0);//BASS_MUSIC_CALCLEN);
			}
		}

		fclose(fp);
	}
}

//used to play a certain music index
//will not play anything if ind exceeds num music
PUBLIC void GUTPlayMusic(unsigned int ind)
{
	if(ind < g_gameMusic.numMusic)
	{
		BASS_ChannelStop(g_gameMusic.music[g_gameMusic.curMusic]);

		g_gameMusic.curMusic=ind;
		BASS_MusicPlay(g_gameMusic.music[ind]);
	}
}

//used to play a random song within the music list
//this will only change the music if the last music stopped playing
//this is ideally used in update loops
PUBLIC void GUTPlayRandMusic(unsigned int minInd, unsigned int maxInd)
{
	if(maxInd < minInd)
		maxInd = g_gameMusic.numMusic-1;

	if(minInd < g_gameMusic.numMusic && maxInd < g_gameMusic.numMusic
		&& BASS_ChannelIsActive(g_gameMusic.music[g_gameMusic.curMusic])==0)
	{
		g_gameMusic.curMusic = Random(minInd, maxInd);
		BASS_MusicPlay(g_gameMusic.music[g_gameMusic.curMusic]);
	}
}

//stops music
PUBLIC void GUTStopMusic()
{
	BASS_ChannelStop(g_gameMusic.music[g_gameMusic.curMusic]);
}


//hard code all the way!
PROTECTED void _GUTBKFXEnterProc(eBKFXType type, unsigned int txtID)
{
	if(g_gameBKFX)
		BKFXDestroy(&g_gameBKFX);

	switch(type)
	{
	case eBKFX_FADEINOUT:
		{
			fadeinout_init fadeinout;

			fadeinout.bDir = 1;
			fadeinout.r = 0;
			fadeinout.g = 0;
			fadeinout.b = 0;
			fadeinout.a = (BYTE)255;
			fadeinout.spd = (BYTE)10;

			g_gameBKFX = BKFXCreate(eBKFX_FADEINOUT, &fadeinout, INFINITE_DURATION, 1);
		}
		break;

	case eBKFX_IMGFADEINOUT:
		{
			imgfadeinout_init imgfadeinout;

			imgfadeinout.bDir = 1;
			imgfadeinout.a = (BYTE)255;
			imgfadeinout.spd = (BYTE)10;
			imgfadeinout.theImg = TextureSearch(txtID);

			g_gameBKFX = BKFXCreate(eBKFX_IMGFADEINOUT, &imgfadeinout, INFINITE_DURATION, 1);
		}
		break;
	}
}

PROTECTED RETCODE _GUTBKFXUpdate()
{
	if(g_gameBKFX)
	{
		RETCODE ret = BKFXUpdate(g_gameBKFX);

		if(ret == RETCODE_FAILURE || ret == RETCODE_BREAK)
		{
			BKFXDestroy(&g_gameBKFX);
			g_gameBKFX=0;

			return RETCODE_BREAK;
		}
	}

	return RETCODE_SUCCESS;
}

PROTECTED void _GUTBKFXExitProc(eBKFXType type, unsigned int txtID)
{
	if(g_gameBKFX)
		BKFXDestroy(&g_gameBKFX);

	switch(type)
	{
	case eBKFX_FADEINOUT:
		{
			fadeinout_init fadeinout;

			fadeinout.bDir = 0;
			fadeinout.r = 0;
			fadeinout.g = 0;
			fadeinout.b = 0;
			fadeinout.a = (BYTE)255;
			fadeinout.spd = (BYTE)10;

			g_gameBKFX = BKFXCreate(eBKFX_FADEINOUT, &fadeinout, INFINITE_DURATION, 1);
		}
		break;

	case eBKFX_IMGFADEINOUT:
		{
			imgfadeinout_init imgfadeinout;

			imgfadeinout.bDir = 0;
			imgfadeinout.a = (BYTE)255;
			imgfadeinout.spd = (BYTE)10;
			imgfadeinout.theImg = TextureSearch(txtID);

			g_gameBKFX = BKFXCreate(eBKFX_IMGFADEINOUT, &imgfadeinout, INFINITE_DURATION, 1);
		}
		break;
	}
}