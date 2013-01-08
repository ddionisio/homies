#include "homies.h"
#include "homies_i.h"

#include "homies_type_cursor.h"
#include "homies_type_map.h"

#define CURSOROBJID 2001

//destroys cursor
PUBLIC void CursorDestroy(hGCURSOR *cursor)
{
	if(*cursor)
	{
		OBJDestroy(&(*cursor)->obj);

		MemFree((void**)cursor);
	}
}

//creates a cursor
PUBLIC hGCURSOR CursorCreate(const char *objFilename)
{
	hGCURSOR newCursor;

	if(MemAlloc((void**)&newCursor, sizeof(gcursor), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new cursor", "CursorCreate"); return 0; }

	newCursor->obj = GUTCreateObj(CURSOROBJID, objFilename);

	return newCursor;
}

//set cursor to map position
PUBLIC RETCODE CursorSetToMap(hGCURSOR cursor, hGMAP map, int indX, int indY)
{
	cursor->indX=indX;
	cursor->indY=indY;

	if(cursor->indX < 0) cursor->indX = 0;
	else if(cursor->indX >= map->indSizeX) cursor->indX = map->indSizeX-1;

	if(cursor->indY < 0) cursor->indY = 0;
	else if(cursor->indY >= map->indSizeY) cursor->indY = map->indSizeY-1;

	hTILE tile = &CELL(map->tiles, cursor->indY, cursor->indX, map->indSizeX);

	float objLoc[eMaxPt]; OBJGetLoc(tile->obj, objLoc);
	gfxBound objBound; OBJGetBounds(tile->obj, &objBound, true);
	gfxBound cursorBound; OBJGetBounds(cursor->obj, &cursorBound, true);

	//we are using z as y since the object has been rotated on y-axis
	//by -90
	float yAmt = ((cursorBound.max.z-cursorBound.min.z)/2)+((objBound.max.z-objBound.min.z)/2);

	OBJSetLoc(cursor->obj, objLoc[eX], objLoc[eY]+yAmt, objLoc[eZ]);

	return RETCODE_SUCCESS;
}

//move cursor to map position
PUBLIC RETCODE CursorMoveToMap(hGCURSOR cursor, hGMAP map, int indAmtX, int indAmtY)
{
	cursor->indX+=indAmtX;
	cursor->indY+=indAmtY;

	if(cursor->indX < 0) cursor->indX = 0;
	else if(cursor->indX >= map->indSizeX) cursor->indX = map->indSizeX-1;

	if(cursor->indY < 0) cursor->indY = 0;
	else if(cursor->indY >= map->indSizeY) cursor->indY = map->indSizeY-1;

	hTILE tile = &CELL(map->tiles, cursor->indY, cursor->indX, map->indSizeX);

	float objLoc[eMaxPt]; OBJGetLoc(tile->obj, objLoc);
	gfxBound objBound; OBJGetBounds(tile->obj, &objBound, true);
	gfxBound cursorBound; OBJGetBounds(cursor->obj, &cursorBound, true);

	//we are using z as y since the object has been rotated on y-axis
	//by -90
	float yAmt = ((cursorBound.max.z-cursorBound.min.z)/2)+((objBound.max.z-objBound.min.z)/2);

	OBJSetLoc(cursor->obj, objLoc[eX], objLoc[eY]+yAmt, objLoc[eZ]);

	return RETCODE_SUCCESS;
}

//displays cursor
PUBLIC RETCODE CursorDisplay(hGCURSOR cursor)
{
	if(!cursor->thisHomie)
	{
		GFXBegin();
		OBJDisplay(cursor->obj);
		GFXEnd();
	}

	return RETCODE_SUCCESS;
}

//picks up a homie (true if so)
PUBLIC hHOMIE CursorPickUp(hGCURSOR cursor, hGMAP map)
{
	for(list<hHOMIE>::iterator i = map->homies->begin(); i != map->homies->end(); i++)
	{
		hHOMIE thisHomie = *i;

		if(thisHomie->indX == cursor->indX 
			&& thisHomie->indY == cursor->indY
			&& !TESTFLAGS(thisHomie->status, HOMIE_MOVING)
			&& !TESTFLAGS(thisHomie->status, HOMIE_DEAD))
		{
			cursor->thisHomie = thisHomie;
			return thisHomie;
		}
	}
	
	return 0;
}

//tells you if we have a homie on our hand
PUBLIC hHOMIE CursorGetHomie(hGCURSOR cursor)
{
	return cursor->thisHomie;
}

//un-picks a homie
//only if it's not moving
PUBLIC void CursorUnPick(hGCURSOR cursor, hGMAP map)
{
	if(cursor->thisHomie)
	{
		CursorSetToMap(cursor, map, cursor->thisHomie->indX, cursor->thisHomie->indY);

		if(!TESTFLAGS(cursor->thisHomie->status, HOMIE_MOVING))
			cursor->thisHomie = 0;
	}
}

//gives you the index cursor location
PUBLIC void CursorGetIndLoc(hGCURSOR cursor, int *indX, int *indY)
{
	if(indX)
		*indX = cursor->indX;
	if(indY)
		*indY = cursor->indY;
}