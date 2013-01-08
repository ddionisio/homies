#include "homies.h"
#include "homies_i.h"

#include "homies_type_map.h"

#define CAMZOOMOUT	1000

//destroys the map
PUBLIC void MapDestroy(hGMAP *map)
{
	if(*map)
	{
		if((*map)->cam)
			CameraDestroy(&(*map)->cam);

		if((*map)->tiles)
		{
			for(int i = 0; i < (*map)->numTiles; i++)
				OBJDestroy(&(*map)->tiles[i].obj);

			MemFree((void**)&(*map)->tiles);
		}

		if((*map)->homies)
		{
			for(list<hHOMIE>::iterator i = (*map)->homies->begin(); i != (*map)->homies->end(); i++)
				HomieDestroy(&(*i));

			delete (*map)->homies;
		}

		OBJDestroy(&(*map)->floor.obj);

		MemFree((void**)map);
	}
}

//creates the map
PUBLIC hGMAP MapLoad(const char *filename, int *timeMax)
{
	hGMAP newMap=0;

	FILE *fp = fopen(filename, "rt");

	if(fp)
	{
		char buff[MAXCHARBUFF];

		if(MemAlloc((void**)&newMap, sizeof(gmap), M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate new map", "MapLoad"); fclose(fp); return 0; }

		while(!feof(fp))
		{
			fscanf(fp, "%s\n", buff);

			if(stricmp(buff, "[CAMERA]") == 0)
			{
				angle yaw, pitch, roll;

				fscanf(fp, "eye=%f,%f,%f yaw=%d pitch=%d roll=%d\n",
					&newMap->camLoc[eX], &newMap->camLoc[eY], &newMap->camLoc[eZ],
					&yaw, &pitch, &roll);

				newMap->camCurY = newMap->camLoc[eY]+CAMZOOMOUT;

				D3DXVECTOR3 vEye(newMap->camLoc[eX], newMap->camCurY, newMap->camLoc[eZ]);

				newMap->cam = CameraCreate(vEye, yaw,pitch,roll);

				if(!newMap->cam)
				{ ASSERT_MSG(0, "Unable to create camera", "MapLoad"); MapDestroy(&newMap); fclose(fp); return 0; }
			}
			else if(stricmp(buff, "[LIGHT]") == 0)
			{
				fscanf(fp, "%s\n", newMap->lightFile);
				LightReset();
				GUTLoadLights(newMap->lightFile);
			}
			else if(stricmp(buff, "[TIME]") == 0 && timeMax)
				fscanf(fp, "%d\n", timeMax);
			else if(stricmp(buff, "[TILES]") == 0)
			{
				int type, tileID, color;

				fscanf(fp, "size=%d,%d\n", &newMap->indSizeX, &newMap->indSizeY);
				newMap->numTiles = newMap->indSizeX*newMap->indSizeY;

				if(MemAlloc((void**)&newMap->tiles, sizeof(tile)*newMap->numTiles, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0, "Unable to allocate tiles", "MapLoad"); MapDestroy(&newMap); fclose(fp); return 0; }

				hTILE thisTile;

				float nextLoc[eMaxPt] = {0,0,0};
				gfxBound objBound;

				for(int y = 0; y < newMap->indSizeY; y++)
				{
					for(int x = 0; x < newMap->indSizeX; x++)
					{
						fscanf(fp, " %d:%d:%d", &type, &tileID, &color);

						thisTile = &CELL(newMap->tiles, y, x, newMap->indSizeX);

						thisTile->type = (TILE_TYPE)type;
						thisTile->homieType = (HOMIE_TYPE)color;
						thisTile->tileID = tileID;
						thisTile->obj = GUTTileSearch(tileID);

						//set tile to correct position
						OBJMovLoc(thisTile->obj, nextLoc[eX], nextLoc[eY], nextLoc[eZ]);

						//move x loc
						OBJGetBounds(thisTile->obj, &objBound, true);

						nextLoc[eX] += (objBound.max.x-objBound.min.x);

						//check to see if it's a goal
						if(thisTile->type == eTILE_GOAL)
							newMap->numGoal++;
					}

					//move z loc
					nextLoc[eX]=0;
					
					thisTile = &CELL(newMap->tiles, y, 0, newMap->indSizeX);

					OBJGetBounds(thisTile->obj, &objBound, true);

					//we are using y as z since the object has been rotated on y-axis
					//by -90
					nextLoc[eZ] += (objBound.max.y-objBound.min.y);

					fscanf(fp, "\n");
				}

				//get floor
				fscanf(fp, " %d:%d:%d", &type, &tileID, &color);
				newMap->floor.type = (TILE_TYPE)type;
				newMap->floor.homieType = (HOMIE_TYPE)color;
				newMap->floor.tileID = tileID;
				newMap->floor.obj = GUTTileSearch(tileID);
			}
			else if(stricmp(buff, "[HOMIES]") == 0)
			{
				int numHomies;
				fscanf(fp, "numHomies=%d\n", &numHomies);

				if(numHomies > 0)
				{
					newMap->homies = new list<hHOMIE>;

					int type, indX, indY;
					char path[MAXCHARBUFF];

					hHOMIE thisHomie;

					for(int i = 0; i < numHomies; i++)
					{
						fscanf(fp, "type=%d\n", &type);
						fscanf(fp, "obj=%s\n", path);
						fscanf(fp, "indLoc=%d,%d\n", &indX, &indY);

						thisHomie = HomieCreate((HOMIE_TYPE)type, newMap, 
							indX, indY, path);

						newMap->homies->push_back(thisHomie);
					}
				}
			}
		}

		fclose(fp);
	}
	else
		ASSERT_MSG(0, "Unable to load map file", "MapLoad");

	return newMap;
}

#define DEFAULT_CAM_X		120
#define DEFAULT_CAM_Y		245
#define DEFAULT_CAM_Z		178
#define DEFAULT_CAM_YAW		0
#define DEFAULT_CAM_PITCH	-65
#define DEFAULT_CAM_ROLL	0

//creates an empty map
PUBLIC hGMAP MapCreate(int tileX, int tileY, int flrGfx, HOMIE_TYPE flrTileHomie,
					   const char *lightFile)
{
	hGMAP newMap=0;

	if(MemAlloc((void**)&newMap, sizeof(gmap), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new map", "MapCreate"); return 0; }

	newMap->indSizeX = tileX; newMap->indSizeY = tileY;
	newMap->numTiles = tileX*tileY;
	
	newMap->floor.type = eTILE_FLOOR;
	newMap->floor.homieType = flrTileHomie;
	newMap->floor.tileID = flrGfx;
	newMap->floor.obj = GUTTileSearch(flrGfx);

	newMap->camLoc[eX]=DEFAULT_CAM_X;
	newMap->camLoc[eY]=DEFAULT_CAM_Y;
	newMap->camLoc[eZ]=DEFAULT_CAM_Z;

	D3DXVECTOR3 vEye(newMap->camLoc[eX], newMap->camCurY, newMap->camLoc[eZ]);

	newMap->cam = CameraCreate(vEye, DEFAULT_CAM_YAW,DEFAULT_CAM_PITCH,DEFAULT_CAM_ROLL);

	if(!newMap->cam)
	{ ASSERT_MSG(0, "Unable to create camera", "MapLoad"); MapDestroy(&newMap); return 0; }

	strcpy(newMap->lightFile, lightFile);
	LightReset();
	GUTLoadLights(newMap->lightFile);

	newMap->homies = new list<hHOMIE>;

	if(MemAlloc((void**)&newMap->tiles, sizeof(tile)*newMap->numTiles, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate map tiles", "MapCreate"); MapDestroy(&newMap); return 0; }

	hTILE thisTile;

	float nextLoc[eMaxPt] = {0,0,0};
	gfxBound objBound;

	for(int y = 0; y < newMap->indSizeY; y++)
	{
		for(int x = 0; x < newMap->indSizeX; x++)
		{
			thisTile = &CELL(newMap->tiles, y, x, newMap->indSizeX);

			//set tile to correct position
			thisTile->type = newMap->floor.type;
			thisTile->homieType = newMap->floor.homieType;
			thisTile->tileID = newMap->floor.tileID;
			thisTile->obj = OBJDuplicate(newMap->floor.obj, 
				OBJGetID(newMap->floor.obj), nextLoc[eX],nextLoc[eY],nextLoc[eZ]);

			//move x loc
			OBJGetBounds(thisTile->obj, &objBound, true);

			nextLoc[eX] += (objBound.max.x-objBound.min.x);
		}

		//move z loc
		nextLoc[eX]=0;
		
		thisTile = &CELL(newMap->tiles, y, 0, newMap->indSizeX);

		OBJGetBounds(thisTile->obj, &objBound, true);

		//we are using y as z since the object has been rotated on y-axis
		//by -90
		nextLoc[eZ] += (objBound.max.y-objBound.min.y);
	}

	return newMap;
}

//displays the map
PUBLIC RETCODE MapDisplay(hGMAP map)
{
	CameraSetScene(map->cam);

	GFXBegin();

	for(int i = 0; i < map->numTiles; i++)
		OBJDisplay(map->tiles[i].obj);

	for(list<hHOMIE>::iterator j = map->homies->begin(); j != map->homies->end(); j++)
		if(!TESTFLAGS((*j)->status, HOMIE_DEAD))
		{ OBJUpdateFrame((*j)->obj); OBJDisplay((*j)->obj); }

	GFXEnd();

	return RETCODE_SUCCESS;
}

//tells us if camera hasn't been zoomed in correctly yet
PUBLIC bool MapIsZoomingIn(hGMAP map)
{
	if(map->camCurY != map->camLoc[eY])
		return true;
	return false;
}

//updates map, returns number of homies on 'home tile'
PUBLIC int MapUpdate(hGMAP map)
{
	//check if we need to zoom-in
	if(MapIsZoomingIn(map))
	{
		map->camCurY -= CAMSPD;

		if(map->camCurY < map->camLoc[eY])
			map->camCurY = map->camLoc[eY];

		float loc[eMaxPt] = {map->camLoc[eX], map->camCurY, map->camLoc[eZ]};
		CameraSet(map->cam, loc);
	}

	int numHome=0;
	hTILE tile;

	for(list<hHOMIE>::iterator i = map->homies->begin(); i != map->homies->end(); i++)
	{
		HomieUpdate(*i, map);

		tile = &CELL(map->tiles, (*i)->indY, (*i)->indX, map->indSizeX);

		if(tile->type == eTILE_GOAL && tile->homieType == (*i)->type)
			numHome++;
	}

	return numHome;
}

//get number of homies
PUBLIC int MapGetNumHomie(hGMAP map)
{
	return map->homies->size();
}

//get the number of goals
PUBLIC int MapGetNumGoal(hGMAP map)
{
	return map->numGoal;
}

//debug purpose
PUBLIC void MapControlCamera(hGMAP map)
{
	/*
	#define kCodeNumpad0			VK_NUMPAD0
#define kCodeNumpad1			VK_NUMPAD1
#define kCodeNumpad2			VK_NUMPAD2
#define kCodeNumpad3			VK_NUMPAD3
#define kCodeNumpad4			VK_NUMPAD4
#define kCodeNumpad5			VK_NUMPAD5
#define kCodeNumpad6			VK_NUMPAD6
#define kCodeNumpad7			VK_NUMPAD7
#define kCodeNumpad8			VK_NUMPAD8
#define kCodeNumpad9			VK_NUMPAD9
#define kCodeNumpadMult		VK_MULTIPLY
#define kCodeNumpadAdd		VK_ADD
#define kCodeNumpadSub		VK_SUBTRACT
#define kCodeNumpadDiv		VK_DIVIDE
#define kCodeNumpadDec		VK_DECIMAL
	*/
	D3DXVECTOR3 movAmt(0,0,0);

	if(InputKbIsPressed(kCodeNumpad8))
		movAmt.z--;
	if(InputKbIsPressed(kCodeNumpad2))
		movAmt.z++;
	if(InputKbIsPressed(kCodeNumpad4))
		movAmt.x--;
	if(InputKbIsPressed(kCodeNumpad6))
		movAmt.x++;
	if(InputKbIsPressed(kCodeNumpad3))
		movAmt.y--;
	if(InputKbIsPressed(kCodeNumpad9))
		movAmt.y++;
	if(InputKbIsPressed(kCodeNumpad7))
		CameraYawPitchRoll(map->cam, 0,1,0);
	if(InputKbIsPressed(kCodeNumpad1))
		CameraYawPitchRoll(map->cam, 0,-1,0);

	map->camLoc[eX] += movAmt.x;
	map->camLoc[eY] += movAmt.y;
	map->camLoc[eZ] += movAmt.z;
	CameraMov(map->cam, movAmt);
}

//debug purpose
PUBLIC void MapDisplayCamera(hGMAP map, hFNT fnt, float x, float y)
{
	angle pitch;

	CameraGetYawPitchRoll(map->cam, 0, &pitch, 0);

	SIZE fntSz; FontGetCharSize(fnt, 'X', &fntSz);

	float curY=y;

	FontPrintf2D(fnt, x, curY, 0xffffffff, "cam loc: %f,%f,%f", map->camLoc[eX],map->camLoc[eY],map->camLoc[eZ]);

	curY+=fntSz.cy;

	FontPrintf2D(fnt, x, curY, 0xffffffff, "pitch: %d", pitch);
}

//set tile
PUBLIC void MapSetTile(hGMAP map, int indX, int indY, int tileGFX,
					   TILE_TYPE tileType, HOMIE_TYPE tileColor)
{
	if(indX >= 0 && indX < map->indSizeX && indY >= 0 && indY < map->indSizeY)
	{
		hTILE tile = &CELL(map->tiles, indY, indX, map->indSizeX);
		
		float objLoc[eMaxPt]; OBJGetLoc(tile->obj, objLoc);
		OBJDestroy(&tile->obj);

		tile->type = tileType;
		tile->homieType = tileColor;
		tile->status = 0;
		tile->tileID = tileGFX;
		tile->obj = GUTTileSearch(tileGFX);

		float newObjLoc[eMaxPt]; OBJGetLoc(tile->obj, newObjLoc);
		OBJSetLoc(tile->obj, objLoc[eX], newObjLoc[eY], objLoc[eZ]);
	}
}

//remove this homie
PUBLIC void MapRemoveHomie(hGMAP map, hHOMIE homie)
{
	hTILE tile = &CELL(map->tiles, homie->indY, homie->indX, map->indSizeX);
	CLEARFLAG(tile->status, TILE_OCCUPIED);

	map->homies->remove(homie);
}

//add a homie
PUBLIC void MapAddHomie(hGMAP map, int indX, int indY, HOMIE_TYPE type,
						const char *objFilename)
{
	if(indX >= 0 && indX < map->indSizeX && indY >= 0 && indY < map->indSizeY)
	{
		hTILE tile = &CELL(map->tiles, indY, indX, map->indSizeX);

		if(!TESTFLAGS(tile->status, TILE_OCCUPIED))
		{
			hHOMIE thisHomie = HomieCreate(type, map, 
							indX, indY, objFilename);

			map->homies->push_back(thisHomie);
		}
	}
}

//saves the map to file
PUBLIC void MapSave(const char *filename, hGMAP map, int timeMax)
{
	FILE *fp = fopen(filename, "wt");

	if(fp)
	{
		fprintf(fp, "[CAMERA]\n");

		angle yaw, pitch, roll;
		CameraGetYawPitchRoll(map->cam, &yaw,&pitch,&roll);

		float camLoc[eMaxPt];
		CameraGetLoc(map->cam, camLoc);

		fprintf(fp, "eye=%f,%f,%f yaw=%d pitch=%d roll=%d\n",
					camLoc[eX], camLoc[eY], camLoc[eZ],
					yaw, pitch, roll);

		fprintf(fp, "[LIGHT]\n");

		fprintf(fp, "%s\n", map->lightFile);

		fprintf(fp, "[TIME]\n");

		fprintf(fp, "%d\n", timeMax);
		
		fprintf(fp, "[TILES]\n");

		fprintf(fp, "size=%d,%d\n", map->indSizeX, map->indSizeY);

		hTILE thisTile;

		for(int y = 0; y < map->indSizeY; y++)
		{
			for(int x = 0; x < map->indSizeX; x++)
			{
				thisTile = &CELL(map->tiles, y, x, map->indSizeX);
				fprintf(fp, " %d:%d:%d", thisTile->type, thisTile->tileID, thisTile->homieType);
			}

			fprintf(fp, "\n");
		}

		fprintf(fp, " %d:%d:%d\n", map->floor.type, map->floor.tileID, map->floor.homieType);
				
		fprintf(fp, "[HOMIES]\n");

		fprintf(fp, "numHomies=%d\n", map->homies->size());

		for(list<hHOMIE>::iterator i = map->homies->begin(); i != map->homies->end(); i++)
		{
			fprintf(fp, "type=%d\n", (*i)->type);
			fprintf(fp, "obj=%s\n", (*i)->objFilename);
			fprintf(fp, "indLoc=%d,%d\n", (*i)->indX, (*i)->indY);
		}

		fclose(fp);
	}
	else
		ASSERT_MSG(0, "Unable to save map to file", "MapSave");
}

PUBLIC hTILE MapGetTile(hGMAP map, TILE_TYPE tileType)
{
	hTILE theTile=0;
	//look for a floor tile
	for(int row = 0; row < map->indSizeY; row++)
	{
		for(int col = 0; col < map->indSizeX; col++)
		{
			theTile = &CELL(map->tiles, row, col, map->indSizeX);

			if(theTile->type == tileType)
			{
				return theTile;
			}
		}
	}

	return 0;
}