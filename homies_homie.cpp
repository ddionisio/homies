#include "homies.h"
#include "homies_i.h"

#include "homies_type_homie.h"
#include "homies_type_map.h"

#define HOMIEOBJID	4978496

//destroys a homie
PUBLIC void HomieDestroy(hHOMIE *homie)
{
	if(*homie)
	{
		OBJDestroy(&(*homie)->obj);
		MemFree((void**)homie);
	}
}

//creates a homie
PUBLIC hHOMIE HomieCreate(HOMIE_TYPE type, hGMAP map, int indX, int indY, 
						  const char *objFile)
{
	assert(map->tiles);

	hHOMIE newHomie;

	if(MemAlloc((void**)&newHomie, sizeof(homie), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new homie", "HomieCreate"); return 0; }

	newHomie->type = type;

	strcpy(newHomie->objFilename, objFile);

	newHomie->obj = GUTCreateObj(HOMIEOBJID, newHomie->objFilename); assert(newHomie->obj);

	newHomie->indX = indX; newHomie->indY = indY;

	if(newHomie->indX < 0) newHomie->indX = 0;
	else if(newHomie->indX >= map->indSizeX) newHomie->indX = map->indSizeX-1;

	if(newHomie->indY < 0) newHomie->indY = 0;
	else if(newHomie->indY >= map->indSizeY) newHomie->indY = map->indSizeY-1;

	hTILE tile = &CELL(map->tiles, newHomie->indY, newHomie->indX, map->indSizeX);

	//set to correct location
	float objLoc[eMaxPt]; OBJGetLoc(tile->obj, objLoc);
	gfxBound objBound; OBJGetBounds(tile->obj, &objBound, true);
	gfxBound homieBound; OBJGetBounds(newHomie->obj, &homieBound, true);

	//we are using z as y since the object has been rotated on y-axis
	//by -90
	float yAmt = ((homieBound.max.z-homieBound.min.z)/2)+((objBound.max.z-objBound.min.z)/2);

	OBJSetLoc(newHomie->obj, objLoc[eX], objLoc[eY]+yAmt, objLoc[eZ]);

	//default to south
	OBJSetOrientation(newHomie->obj, 0, 0, GCFGGetHomieSpd());

	SETFLAG(tile->status, TILE_OCCUPIED);

	return newHomie;
}

//move homie
PUBLIC RETCODE HomieMove(hHOMIE homie, eDIR dir, hGMAP map)
{
	if(!TESTFLAGS(homie->status, HOMIE_MOVING) 
		&& !TESTFLAGS(homie->status, HOMIE_STUCK)
		&& !TESTFLAGS(homie->status, HOMIE_DEAD))
	{
		switch(dir)
		{
		case eDIR_SOUTH:
			OBJSetOrientation(homie->obj, 0, 0, GCFGGetHomieSpd());
			break;
		case eDIR_NORTH:
			OBJSetOrientation(homie->obj, 0, 0, -GCFGGetHomieSpd());
			break;
		case eDIR_EAST:
			OBJSetOrientation(homie->obj, -GCFGGetHomieSpd(), 0, 0);
			break;
		case eDIR_WEST:
			OBJSetOrientation(homie->obj, GCFGGetHomieSpd(), 0, 0);
			break;
		}

		OBJSetState(homie->obj, eHOMIESTATE_MOVE);
		SETFLAG(homie->status, HOMIE_MOVING);

		hTILE tile = &CELL(map->tiles, homie->indY, homie->indX, map->indSizeX);
		CLEARFLAG(tile->status, TILE_OCCUPIED);
	}

	return RETCODE_SUCCESS;
}

//stop homie
PUBLIC RETCODE HomieStop(hHOMIE homie, hGMAP map)
{
	CLEARFLAG(homie->status, HOMIE_MOVING);
	CLEARFLAG(homie->status, HOMIE_STUCK);

	//adjust to current tile
	hTILE tile = &CELL(map->tiles, homie->indY, homie->indX, map->indSizeX);

	//check if the tile is one of the arrows
	if(tile->type >= eTILE_ARROW_N && tile->type <= eTILE_ARROW_W)
	{ SETFLAG(homie->status, HOMIE_STUCK); OBJSetState(homie->obj, eHOMIESTATE_STAND); }
	//else if(tile->type == eTILE_FLOOR)
	//	OBJSetState(homie->obj, eHOMIESTATE_STAND);
	else if(tile->type == eTILE_GOAL && tile->homieType == homie->type)
		OBJSetState(homie->obj, eHOMIESTATE_CELEBRATE);
	else
		OBJSetState(homie->obj, eHOMIESTATE_STAND);
	
	float homieLoc[eMaxPt]; OBJGetLoc(homie->obj, homieLoc);
	float tileLoc[eMaxPt]; OBJGetLoc(tile->obj, tileLoc);
	
	OBJSetLoc(homie->obj, tileLoc[eX], homieLoc[eY], tileLoc[eZ]);

	SETFLAG(tile->status, TILE_OCCUPIED);

	return RETCODE_SUCCESS;
}

//update homie
PUBLIC RETCODE HomieUpdate(hHOMIE homie, hGMAP map)
{
	int lastIndX = homie->indX;
	int lastIndY = homie->indY;

	if((TESTFLAGS(homie->status, HOMIE_MOVING) || TESTFLAGS(homie->status, HOMIE_STUCK))
		&& !TESTFLAGS(homie->status, HOMIE_DEAD))
	{
		float orient[eMaxPt]; OBJGetOrientation(homie->obj, orient);
		orient[eX] *= -1;

		int nextIndX = homie->indX + (orient[eX] <= 0 ? (orient[eX] < 0 ? -1 : 0) : 1);
		int nextIndY = homie->indY + (orient[eZ] <= 0 ? (orient[eZ] < 0 ? -1 : 0) : 1);

		hTILE nextTile;

		//check to see if next ind is out of bound
		if(nextIndX < 0 || nextIndX >= map->indSizeX ||
			nextIndY < 0 || nextIndY >= map->indSizeY)
			return HomieStop(homie, map);

		//get next tile
		nextTile = &CELL(map->tiles, nextIndY, nextIndX, map->indSizeX); assert(nextTile);

		//check if occupied
		if(TESTFLAGS(nextTile->status, TILE_OCCUPIED))
			return HomieStop(homie, map);

		//check if next tile is a wall or etc...
		switch(nextTile->type)
		{
		case eTILE_WALL:
			return HomieStop(homie, map);
		case eTILE_BOUNCE:
			//inverse direction
			orient[eZ] *= -1;
			OBJSetOrientation(homie->obj, orient[eX],orient[eY],orient[eZ]);
			homie->indX = nextIndX;
			homie->indY = nextIndY;
			return RETCODE_SUCCESS;
		case eTILE_DOOR:
			if(nextTile->homieType == homie->type) //destroy this door (change to floor)
			{
				float tileLoc[eMaxPt]; OBJGetLoc(nextTile->obj, tileLoc);
				float floorLoc[eMaxPt];

				OBJDestroy(&nextTile->obj);

				//look for a floor tile
				hTILE theTile = MapGetTile(map, eTILE_FLOOR);

				if(theTile)
				{
					OBJGetLoc(theTile->obj, floorLoc);
					nextTile->tileID = theTile->tileID;
					nextTile->obj = OBJDuplicate(theTile->obj, OBJGetID(theTile->obj), tileLoc[eX], floorLoc[eY], tileLoc[eZ]);
				}
				else
				{
					OBJGetLoc(map->floor.obj, floorLoc);
					nextTile->tileID = map->floor.tileID;
					nextTile->obj = OBJDuplicate(map->floor.obj, OBJGetID(map->floor.obj), tileLoc[eX], floorLoc[eY], tileLoc[eZ]);
				}

				nextTile->homieType = map->floor.homieType;
				nextTile->status = map->floor.status;
				nextTile->type = map->floor.type;
			}
			else
				return HomieStop(homie, map);
			break;
		}

		//otherwise, move homie!
		OBJMovLoc(homie->obj, orient[eX], 0, orient[eZ]);
		
		if(TESTFLAGS(homie->status, HOMIE_STUCK))
		{
			SETFLAG(homie->status, HOMIE_MOVING);
			CLEARFLAG(homie->status, HOMIE_STUCK);

			hTILE tile = &CELL(map->tiles, homie->indY, homie->indX, map->indSizeX);
			CLEARFLAG(tile->status, TILE_OCCUPIED);
		}

		float homieLoc[eMaxPt]; OBJGetLoc(homie->obj, homieLoc);
		float tileLoc[eMaxPt]; OBJGetLoc(nextTile->obj, tileLoc);

		//check to see if homie has reached the next tile
		if((orient[eX] > 0 && homieLoc[eX] >= tileLoc[eX]) ||
			(orient[eX] < 0 && homieLoc[eX] <= tileLoc[eX]) ||
			(orient[eZ] > 0 && homieLoc[eZ] >= tileLoc[eZ]) ||
			(orient[eZ] < 0 && homieLoc[eZ] <= tileLoc[eZ]))
		{
			homie->indX = nextIndX;
			homie->indY = nextIndY;

			//determine course of action with specific tile
			if(nextTile->homieType == eHOMIE_NOCOLOR || 
				nextTile->homieType == homie->type)
			{
				switch(nextTile->type)
				{
				case eTILE_ARROW_N:
					OBJSetLoc(homie->obj, tileLoc[eX], homieLoc[eY], tileLoc[eZ]);
					OBJSetOrientation(homie->obj, 0, 0, -GCFGGetHomieSpd());
					return RETCODE_SUCCESS;
				case eTILE_ARROW_S:
					OBJSetLoc(homie->obj, tileLoc[eX], homieLoc[eY], tileLoc[eZ]);
					OBJSetOrientation(homie->obj, 0, 0, GCFGGetHomieSpd());
					return RETCODE_SUCCESS;
				case eTILE_ARROW_E:
					OBJSetLoc(homie->obj, tileLoc[eX], homieLoc[eY], tileLoc[eZ]);
					OBJSetOrientation(homie->obj, -GCFGGetHomieSpd(), 0, 0);
					return RETCODE_SUCCESS;
				case eTILE_ARROW_W:
					OBJSetLoc(homie->obj, tileLoc[eX], homieLoc[eY], tileLoc[eZ]);
					OBJSetOrientation(homie->obj, GCFGGetHomieSpd(), 0, 0);
					return RETCODE_SUCCESS;
				case eTILE_TELEPORT:
					{
						hTILE theTile, lastTile;
						//look for another teleport tile similar to this
						for(int row = 0; row < map->indSizeY; row++)
						{
							for(int col = 0; col < map->indSizeX; col++)
							{
								theTile = &CELL(map->tiles, row, col, map->indSizeX);

								if((col != nextIndX || row != nextIndY) 
									&& !TESTFLAGS(theTile->status, TILE_OCCUPIED)
									&& theTile->type == eTILE_TELEPORT
									&& (theTile->homieType == nextTile->homieType))
								{
									//teleport homie here
									OBJGetLoc(theTile->obj, tileLoc);
									OBJSetLoc(homie->obj, tileLoc[eX], homieLoc[eY], tileLoc[eZ]);

									lastTile = &CELL(map->tiles, lastIndY, lastIndX, map->indSizeX);
									CLEARFLAG(lastTile->status, TILE_OCCUPIED);

									homie->indX = col;
									homie->indY = row;
									return RETCODE_SUCCESS;
								}
							}
						}
					}
					return RETCODE_SUCCESS;
				case eTILE_HOLE:
					{
						CLEARFLAG(homie->status, HOMIE_MOVING);
						SETFLAG(homie->status, HOMIE_DEAD);

						float tileLoc[eMaxPt]; OBJGetLoc(nextTile->obj, tileLoc);
						float floorLoc[eMaxPt];

						OBJDestroy(&nextTile->obj);

						//look for a floor tile
						hTILE theTile = MapGetTile(map, eTILE_FLOOR);

						if(theTile)
						{
							OBJGetLoc(theTile->obj, floorLoc);
							nextTile->tileID = theTile->tileID;
							nextTile->obj = OBJDuplicate(theTile->obj, OBJGetID(theTile->obj), tileLoc[eX], floorLoc[eY], tileLoc[eZ]);
						}
						else
						{
							OBJGetLoc(map->floor.obj, floorLoc);
							nextTile->tileID = map->floor.tileID;
							nextTile->obj = OBJDuplicate(map->floor.obj, OBJGetID(map->floor.obj), tileLoc[eX], floorLoc[eY], tileLoc[eZ]);
						}

						nextTile->homieType = map->floor.homieType;
						nextTile->status = map->floor.status;
						nextTile->type = map->floor.type;
					}
					return RETCODE_SUCCESS;
				}
			}
		}
	}

	return RETCODE_SUCCESS;
}