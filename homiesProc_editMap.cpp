#include "homies.h"
#include "homies_i.h"

#include "homies_type_map.h"
#include "homies_type_homie.h"
#include "homies_type_cursor.h"

#include "homies_menu_id.h"

#include "Parser\\parser.h"

#include "menu\\textbox.h"
#include "menu\\editline.h"

#define EDITMAPFONTIND 4

#define TILEGFX_X 532
#define TILEGFX_Y 96
#define TILEGFX_SIZE 32

#define INST_X		0
#define INST_Y		32

#define EDITMAPFILE "editmap.txt"

#define FLOORTILE		1	//index in tile list file

#define LIGHTDEFAULT	"Homies\\Lights\\someLight.txt"

static const char *g_tileNames[eTILE_MAX]={"void(dont use)", "floor",
"wall", "goal", "arrow N", "arrow S", "arrow E", "arrow W", "teleport",
"bounce", "door", "hole(stupid)"};

static const char *g_homieNames[eHOMIE_MAX]={"any(use for tile)", "red",
"green", "blue", "yellow"};

static const char *g_homieOBJFile[eHOMIE_MAX]={0,
"Homies\\Objects\\homieRed.txt", "Homies\\Objects\\homieGreen.txt", 
"Homies\\Objects\\homieBlue.txt", "Homies\\Objects\\homieYellow.txt"};

static HMC g_win=0;

typedef enum {
	eEXIT_NO,
	eEXIT_EXIT,
	eEXIT_MAX
} eEXIT_TYPE;

static eEXIT_TYPE g_exit=eEXIT_NO;

typedef struct _tileStuff {
	int ID;
	hTXT txt;	//used for display
} tileStuff;

typedef struct _editMap {
	hGCURSOR cursor;				//the cursor
	hGMAP	 map;					//the map
	hFNT     gameFont;				//display for instructions...etc.
	bool	 bSaveMapList;			//tells us to update the map list
	char mapFilename[MAXCHARBUFF];	//the map file

	int homieType;			//current homie type selected
	
	int tileObjInd;					//tile obj index
	int tileHomieType;		//current tile color
	int tileType;				//current tile type
	
	tileStuff	*tiles;					//gfx tiles to use (numTile)
	int		numTile;				//number of gfx tile

	int timeMax;
} editMap;

static editMap *g_editMap=0;

PRIVATE void _EditMapTerm()
{
	GFXEnableLight(false, 0xffffffff);
	LightReset();

	if(g_editMap)
	{
		CursorDestroy(&g_editMap->cursor);
		MapDestroy(&g_editMap->map);

		if(g_editMap->tiles)
		{
			for(int i = 0; i < g_editMap->numTile; i++)
				TextureDestroy(&g_editMap->tiles[i].txt);

			MemFree((void**)&g_editMap->tiles);
		}

		MemFree((void**)&g_editMap);
	}
}

PRIVATE void _EditMapInit(mapEditInfo *pMapEdit)
{
	_EditMapTerm();

	GFXEnableLight(true, 0xffffffff);

	if(MemAlloc((void**)&g_editMap, sizeof(editMap), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate edit map" , "_EditMapInit"); return; }

	g_editMap->bSaveMapList = pMapEdit->bCreateNew;
	g_editMap->tileType = eTILE_FLOOR;
	g_editMap->homieType = eHOMIE_RED;

	//get map filename
	strcpy(g_editMap->mapFilename, MAPDIR);
	strcat(g_editMap->mapFilename, pMapEdit->mapName);

	//check if file exists
	FILE *fp = fopen(g_editMap->mapFilename, "rt");

	if(fp)
	{
		fclose(fp);

		//open the map
		g_editMap->map = MapLoad(g_editMap->mapFilename, &g_editMap->timeMax);
	}
	else
	{
		//create new map
		g_editMap->map = MapCreate(pMapEdit->col, pMapEdit->row, FLOORTILE, eHOMIE_NOCOLOR,
					   LIGHTDEFAULT);
	}

	//load up the objects for all tiles
	//this is crazy, but who cares
	list<int> objTiles;
	GUTTileLoadAll(&objTiles);

	if(objTiles.size() > 0) //it better not be!
	{
		g_editMap->numTile = objTiles.size();
		if(MemAlloc((void**)&g_editMap->tiles, sizeof(tileStuff)*g_editMap->numTile, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate tiles", "_EditMapInit"); _EditMapTerm(); return; }

		int j=0;
		for(list<int>::iterator i = objTiles.begin(); i != objTiles.end(); i++, j++)
		{
			g_editMap->tiles[j].ID = *i;
			hOBJ theOBJ = GUTTileSearch(g_editMap->tiles[j].ID);
			g_editMap->tiles[j].txt = MDLGetTexture(OBJGetModel(theOBJ), 0);
			TextureAddRef(g_editMap->tiles[j].txt);
			OBJDestroy(&theOBJ);
		}
	}

	//create cursor
	g_editMap->cursor = CursorCreate(CURSORFILE);
	CursorSetToMap(g_editMap->cursor, g_editMap->map, 0, 0);

	//get font
	g_editMap->gameFont = MenuFontGet(EDITMAPFONTIND);
}

PRIVATE void _EditMapUpdateInput()
{
	hHOMIE thisHomie;
	thisHomie = CursorGetHomie(g_editMap->cursor);

	if(InputKbIsReleased(kCodeUp))
	{
		if(!thisHomie)
			CursorMoveToMap(g_editMap->cursor, g_editMap->map, 0,-1);
		else
			HomieMove(thisHomie, eDIR_NORTH, g_editMap->map);
	}
	else if(InputKbIsReleased(kCodeDown))
	{
		if(!thisHomie)
			CursorMoveToMap(g_editMap->cursor, g_editMap->map, 0,1);
		else
			HomieMove(thisHomie, eDIR_SOUTH, g_editMap->map);
	}

	if(InputKbIsReleased(kCodeLeft))
	{
		if(!thisHomie)
			CursorMoveToMap(g_editMap->cursor, g_editMap->map, -1,0);
		else
			HomieMove(thisHomie, eDIR_WEST, g_editMap->map);
	}
	else if(InputKbIsReleased(kCodeRight))
	{
		if(!thisHomie)
			CursorMoveToMap(g_editMap->cursor, g_editMap->map, 1,0);
		else
			HomieMove(thisHomie, eDIR_EAST, g_editMap->map);
	}

	if(InputKbIsReleased(kCodeSpc))
	{
		if(CursorGetHomie(g_editMap->cursor))
			CursorUnPick(g_editMap->cursor, g_editMap->map);
		else
			CursorPickUp(g_editMap->cursor, g_editMap->map);
	}

	if(!thisHomie)
	{
		int indX,indY;
		CursorGetIndLoc(g_editMap->cursor, &indX, &indY);

		if(InputKbIsReleased(kCodeQ)) //set tile
		{
			MapSetTile(g_editMap->map, indX, indY, g_editMap->tiles[g_editMap->tileObjInd].ID,
					   (TILE_TYPE)g_editMap->tileType, (HOMIE_TYPE)g_editMap->tileHomieType);
		}

		if(InputKbIsReleased(kCodeW)) //add/remove homie
		{
			hHOMIE homie = CursorPickUp(g_editMap->cursor, g_editMap->map);
			CursorUnPick(g_editMap->cursor, g_editMap->map);

			if(homie) //remove homie
				MapRemoveHomie(g_editMap->map, homie);
			else //add a new homie
				MapAddHomie(g_editMap->map, indX, indY, (HOMIE_TYPE)g_editMap->homieType,
						g_homieOBJFile[g_editMap->homieType]);
		}
	}
}

PRIVATE void _EditMapDisplay()
{
	GFXClear(0, 0, 0);   //clears screen within rects with given color

	MapDisplay(g_editMap->map);
	CursorDisplay(g_editMap->cursor);

	GFXBltModeEnable(false);
	MenuDraw();			 //draw all menus

	//draw tile gfx
	TextureStretchBlt(g_editMap->tiles[g_editMap->tileObjInd].txt,
		TILEGFX_X, TILEGFX_Y, TILEGFX_SIZE, TILEGFX_SIZE,
		0, 0, 0);

	GFXBltModeDisable();

	//display instructions...
	GFXBegin();
	FontPrintf2D(g_editMap->gameFont, INST_X,INST_Y, 0x7fffffff, "Q: Set Tile W: add/remove Homie");
	GFXEnd();
}

PROTECTED int EditMapWinProc(int nWinId, UINT msg, WPARAM wParam, LPARAM lParam)
{
	char buff[MAXCHARBUFF];

	switch(msg)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		switch(HIWORD(wParam))
		{
		case MID_EDITMAP_SAVE:
			EditLineGetString((HEDITLINE)MenuGetItem(MID_EDITMAP_ELINETIME), buff);
			sscanf(buff, "%d", &g_editMap->timeMax);

			if(g_editMap->bSaveMapList)
			{
				GUTAddAfterCurrent(GetFilePart(g_editMap->mapFilename));
				GUTSaveMapList(MAPLIST);
				g_editMap->bSaveMapList = false;
			}

			MapSave(g_editMap->mapFilename, g_editMap->map, g_editMap->timeMax);
			break;
		case MID_EDITMAP_EXIT:
			g_exit=eEXIT_EXIT;
			break;
		case MID_EDITMAP_LFTTTYPE:
			if(g_editMap->tileType > eTILE_FLOOR)
			{
				g_editMap->tileType--;
				strcpy(buff,g_tileNames[g_editMap->tileType]);
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTTYPEDESC), buff);
			}
			break;
		case MID_EDITMAP_RGTTTYPE:
			if(g_editMap->tileType < eTILE_MAX-1)
			{
				g_editMap->tileType++;
				strcpy(buff,g_tileNames[g_editMap->tileType]);
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTTYPEDESC), buff);
			}
			break;
		case MID_EDITMAP_LFTTGFX:
			if(g_editMap->tileObjInd > 0)
				g_editMap->tileObjInd--;
			break;
		case MID_EDITMAP_RGTTGFX:
			if(g_editMap->tileObjInd < g_editMap->numTile-1)
				g_editMap->tileObjInd++;
			break;
		case MID_EDITMAP_LFTTCLR:
			if(g_editMap->tileHomieType > eHOMIE_NOCOLOR)
			{
				g_editMap->tileHomieType--;
				strcpy(buff,g_homieNames[g_editMap->tileHomieType]);
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTTCLRTYP), buff);
			}
			break;
		case MID_EDITMAP_RGTTCLR:
			if(g_editMap->tileHomieType < eHOMIE_MAX-1)
			{
				g_editMap->tileHomieType++;
				strcpy(buff,g_homieNames[g_editMap->tileHomieType]);
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTTCLRTYP), buff);
			}
			break;
		case MID_EDITMAP_LFTHOMIE:
			if(g_editMap->homieType > eHOMIE_RED)
			{
				g_editMap->homieType--;
				strcpy(buff,g_homieNames[g_editMap->homieType]);
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTHOMIETYP), buff);
			}
			break;
		case MID_EDITMAP_RGTHOMIE:
			if(g_editMap->homieType < eHOMIE_MAX-1)
			{
				g_editMap->homieType++;
				strcpy(buff,g_homieNames[g_editMap->homieType]);
				TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTHOMIETYP), buff);
			}
			break;
		}
		break;
	}

	return 1;
}

PROTECTED RETCODE EditMapProc(DWORD msg, WPARAM wParam, LPARAM lParam)
{
	char buff[MAXCHARBUFF]={0};

	switch(msg)
	{
	case GMSG_INIT:
		g_exit=eEXIT_NO;

		_GUTBKFXEnterProc(eBKFX_FADEINOUT,0);

		g_win = MenuGetItem(MID_EDITMAP);

		if(!g_win)
		{
			char path[MAXCHARBUFF];

			//load up the menu
			strcpy(path, WINDOWDIR);
			strcat(path, EDITMAPFILE);

			FILE *fp = fopen(path, "rt");
			MenuLoadFromFile (fp);
			fclose(fp);

			g_win = MenuGetItem(MID_EDITMAP);

			WindowChangeSize((HWINDOW)g_win, GFXGetScrnSize().cx, GFXGetScrnSize().cy);
			WindowNewProc((HWINDOW)g_win, EditMapWinProc);
		}
		else
			MenuShowItem(g_win, HMC_ACTIVE, 1);

		_EditMapInit((mapEditInfo *)wParam);

		//set up the windows...
		strcpy(buff,g_tileNames[g_editMap->tileType]);
		TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTTYPEDESC), buff);

		strcpy(buff,g_homieNames[g_editMap->tileHomieType]);
		TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTTCLRTYP), buff);

		strcpy(buff,g_homieNames[g_editMap->homieType]);
		TextBoxChangeText((HTEXTBOX)MenuGetItem(MID_EDITMAP_TXTHOMIETYP), buff);

		sprintf(buff, "%d", g_editMap->timeMax);
		EditLineSetString((HEDITLINE)MenuGetItem(MID_EDITMAP_ELINETIME), buff);

		break;

	case GMSG_UPDATE:
		if(g_exit==eEXIT_NO)
		{
			GUTPlayRandMusic(eMUSIC_RANDOFFSET, 0);

			MenuProc();			 //update the windows

			if(!MapIsZoomingIn(g_editMap->map))
			{
				_EditMapUpdateInput();

				MapControlCamera(g_editMap->map);
			}

			MapUpdate(g_editMap->map);

			if(g_exit!=eEXIT_NO)
				_GUTBKFXExitProc(eBKFX_FADEINOUT,0);
		}
		break;

	case GMSG_DISPLAY:
		_EditMapDisplay();

		if(_GUTBKFXUpdate() == RETCODE_BREAK && g_exit != eEXIT_NO)
		{
			SENDPROCMSG(GMSG_DESTROY, 0, 0);

			switch(g_exit)
			{
			case eEXIT_EXIT:
				SETPROCTYPE(eEditor);
				SENDPROCMSG(GMSG_INIT, 0, 0);
				break;
			}
		}
		
		GFXUpdate(0, 0, 0);  //update frame on display
		break;

	case GMSG_LOAD:
		break;

	case GMSG_DESTROY:
		if(g_win)
			MenuShowItem(g_win, HMC_INVIS, 1);
		_EditMapTerm();
		break;
	}

	return RETCODE_SUCCESS;
}