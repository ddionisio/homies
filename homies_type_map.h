#ifndef _homies_type_map_h
#define _homies_type_map_h

#include "homies_types.h"
#include "homies_type_homie.h"

/********************************************************************
*																	*
*								Flags								*
*																	*
********************************************************************/

#define	TILE_OCCUPIED	1

/********************************************************************
*																	*
*							Enumerated types						*
*																	*
********************************************************************/

typedef enum {
	eTILE_VOID,
	eTILE_FLOOR,
	eTILE_WALL,
	eTILE_GOAL,

	//do not add in-between these arrow types!
	eTILE_ARROW_N,
	eTILE_ARROW_S,
	eTILE_ARROW_E,
	eTILE_ARROW_W,

	eTILE_TELEPORT,
	eTILE_BOUNCE,
	eTILE_DOOR,
	eTILE_HOLE,
	eTILE_MAX
} TILE_TYPE;

/********************************************************************
*																	*
*							Internal types							*
*																	*
********************************************************************/

typedef struct _tile {
	TILE_TYPE	type;		//type of tile
	int tileID;				//the tile ID
	HOMIE_TYPE	homieType;	//which homie can activate this tile
	hOBJ		obj;		//graphical representation
	FLAGS		status;		//current status, ex: TILE_OCCUPIED
} tile;

typedef struct _gmap {
	tile	*tiles;				//the tiles of the map (numTiles)
	tile	floor;				//used by tiles that can change to floor
	int		numTiles;			//number of tile
	int		numGoal;			//number of goals
	int		indSizeX,indSizeY;	//index size
	hCAMERA cam;				//camera
	float camLoc[eMaxPt];		//camera loc.
	float camCurY;				//current camera Y
	list<hHOMIE> *homies;		//the homies
	char lightFile[MAXCHARBUFF];//light file
} gmap;

/********************************************************************
*																	*
*							Map Interface							*
*																	*
********************************************************************/

//creates the map
PUBLIC hGMAP MapLoad(const char *filename, int *timeMax);

//creates an empty map
PUBLIC hGMAP MapCreate(int tileX, int tileY, int flrGfx, HOMIE_TYPE flrTileHomie,
					   const char *lightFile);

//destroys the map
PUBLIC void MapDestroy(hGMAP *map);

//displays the map
PUBLIC RETCODE MapDisplay(hGMAP map);

//tells us if camera hasn't been zoomed in correctly yet
PUBLIC bool MapIsZoomingIn(hGMAP map);

//updates map, returns number of homies on 'home tile'
PUBLIC int MapUpdate(hGMAP map);

//get number of homies
PUBLIC int MapGetNumHomie(hGMAP map);

//get the number of goals
PUBLIC int MapGetNumGoal(hGMAP map);

//debug purpose
PUBLIC void MapControlCamera(hGMAP map);

//debug purpose
PUBLIC void MapDisplayCamera(hGMAP map, hFNT fnt, float x, float y);

//map edit purpose

//set tile
PUBLIC void MapSetTile(hGMAP map, int indX, int indY, int tileGFX,
					   TILE_TYPE tileType, HOMIE_TYPE tileColor);

//remove this homie
PUBLIC void MapRemoveHomie(hGMAP map, hHOMIE homie);

//add a homie
PUBLIC void MapAddHomie(hGMAP map, int indX, int indY, HOMIE_TYPE type,
						const char *objFilename);

//saves the map to file
PUBLIC void MapSave(const char *filename, hGMAP map, int timeMax);

//get tile
PUBLIC hTILE MapGetTile(hGMAP map, TILE_TYPE tileType);

#endif