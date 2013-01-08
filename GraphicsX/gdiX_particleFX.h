#ifndef _gdix_particlefx_h
#define _gdix_particlefx_h

#include "gdix.h"

typedef struct _gfxParticleFX *hPARFX;

typedef bool (* COLLISIONFUNC) (const gfxBound *bound, float outPt[eMaxPt]);

typedef enum {
	ePARFX_GAS,		//gas spreading in various directions
	ePARFX_GAS2,	//gas spreading in a radius
	ePARFX_MAX
} ePARFXType;

/**********************************************************
***********************************************************

  The init structs you must fill up for FX

***********************************************************
**********************************************************/

typedef struct _fxGas_init {
	hTXT		gasTxt;				//gas image
	DWORD		clr;				//color
	float		upVec[eMaxPt];		//up vector
	float		size;				//max scale size of each gas before disappearing
	float		minBound[eMaxPt];	//upper-left bound
	float		maxBound[eMaxPt];	//lower right bound
	float		minGrowth,maxGrowth;//growth range min-max rate
	DWORD		maxParticle;		//maximum particle to be displayed
} fxGas_init;

typedef struct _fxGas2_init {
	hTXT		gasTxt;				//gas image
	DWORD		clr;				//color

	float		center[eMaxPt];		//center

	float		size;				//max scale size of each gas before disappearing
	
	float		minVel[eMaxPt];		//minimum generated velocity
	float		maxVel[eMaxPt];		//maximum generated velocity

	float		radius;				//max scale size of each gas before disappearing

	float       normArea[eMaxPt];	//area within radius to consider (0~1)

	float		minGrowth,maxGrowth;//growth range min-max rate
	DWORD		maxParticle;		//maximum particle to be displayed
} fxGas2_init;

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hPARFX PARFXCreate(ePARFXType type, void *initStruct, int maxDuration,
						int delay, hOBJ objAttach, COLLISIONFUNC collision);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void PARFXDestroyAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void PARFXDestroy(hPARFX *parFX);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE PARFXUpdate(hPARFX parFX);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplay(hPARFX parFX);

/////////////////////////////////////
// Name:	PARFXUpdateAll
// Purpose:	updates all particles
//			created.
//			NOTE: This will destroy
//				  any particle that has
//				  expired
// Output:	stuff updated
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE PARFXUpdateAll();

/////////////////////////////////////
// Name:	PARFXDisplayAll
// Purpose:	displays all particles
//			created.
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplayAll();

#endif