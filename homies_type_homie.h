#ifndef _homies_type_homie_h
#define _homies_type_homie_h

#include "homies_types.h"

/********************************************************************
*																	*
*								Flags								*
*																	*
********************************************************************/

#define HOMIE_IDLE		0
#define	HOMIE_MOVING	1
#define HOMIE_STUCK		2
#define HOMIE_DEAD		4

/********************************************************************
*																	*
*							Enumerated types						*
*																	*
********************************************************************/

typedef enum {
	eHOMIESTATE_STAND,
	eHOMIESTATE_MOVE,
	eHOMIESTATE_CELEBRATE,
	eHOMIESTATE_MAX
} HOMIE_STATE;

typedef enum {
	eHOMIE_NOCOLOR,		//used by tile to mean any of the colors
	eHOMIE_RED,
	eHOMIE_GREEN,
	eHOMIE_BLUE,
	eHOMIE_YELLOW,
	eHOMIE_MAX
} HOMIE_TYPE;

/********************************************************************
*																	*
*							Internal types							*
*																	*
********************************************************************/

typedef struct _homie {
	HOMIE_TYPE type;		//type of homie
	hOBJ	   obj;			//graphical representation (includes location and orientation)
	char objFilename[MAXCHARBUFF]; //obj filename
	int		   indX,indY;	//current tile location
	FLAGS	   status;		//current status, ex: HOMIE_IDLE
} homie;

/********************************************************************
*																	*
*							Homie Interface							*
*																	*
********************************************************************/

//creates a homie
PUBLIC hHOMIE HomieCreate(HOMIE_TYPE type, hGMAP map, int indX, int indY, 
						  const char *objFile);

//destroys a homie
PUBLIC void HomieDestroy(hHOMIE *homie);

//move homie
PUBLIC RETCODE HomieMove(hHOMIE homie, eDIR dir, hGMAP map);

//stop homie
PUBLIC RETCODE HomieStop(hHOMIE homie, hGMAP map);

//update homie
PUBLIC RETCODE HomieUpdate(hHOMIE homie, hGMAP map);

#endif