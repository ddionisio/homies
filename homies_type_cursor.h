#ifndef _homies_type_cursor_h
#define _homies_type_cursor_h

#include "homies_types.h"

/********************************************************************
*																	*
*								Flags								*
*																	*
********************************************************************/

#define CURSOR_EMPTY	0
#define CURSOR_PICKUP	1

/********************************************************************
*																	*
*							Enumerated types						*
*																	*
********************************************************************/

/********************************************************************
*																	*
*							Internal types							*
*																	*
********************************************************************/

typedef struct _gcursor {
	hOBJ	   obj;			//graphical representation
	int		   indX,indY;	//current tile location
	hHOMIE	   thisHomie;	//if cursor has picked a homie to move
	FLAGS	   status;		//current status, ex: CURSOR_EMPTY
} gcursor;

/********************************************************************
*																	*
*						Cursor Interface							*
*																	*
********************************************************************/

//creates a cursor
PUBLIC hGCURSOR CursorCreate(const char *objFilename);

//destroys cursor
PUBLIC void CursorDestroy(hGCURSOR *cursor);

//set cursor to map position
PUBLIC RETCODE CursorSetToMap(hGCURSOR cursor, hGMAP map, int indX, int indY);

//move cursor to map position
PUBLIC RETCODE CursorMoveToMap(hGCURSOR cursor, hGMAP map, int indAmtX, int indAmtY);

//displays cursor
PUBLIC RETCODE CursorDisplay(hGCURSOR cursor);

//picks up a homie (true if so)
PUBLIC hHOMIE CursorPickUp(hGCURSOR cursor, hGMAP map);

//tells you if we have a homie on our hand
PUBLIC hHOMIE CursorGetHomie(hGCURSOR cursor);

//un-picks a homie
//only if it's not moving
PUBLIC void CursorUnPick(hGCURSOR cursor, hGMAP map);

//gives you the index cursor location
PUBLIC void CursorGetIndLoc(hGCURSOR cursor, int *indX, int *indY);

#endif