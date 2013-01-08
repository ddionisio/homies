#ifndef _gdix_particlefx_i_h
#define _gdix_particlefx_i_h

#include "gdix_particlefx.h"

//message for FX functions
typedef enum {
	PARFXM_CREATE,
	PARFXM_UPDATE,
	PARFXM_DISPLAY,
	PARFXM_DESTROY
} FXmessage;

//signals for FX update
typedef enum {
	PARFXsignal_animate,
	PARFXsignal_noanimate
} PARFXsignal;

// Helper function to stuff a FLOAT into a DWORD argument
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

/**********************************************************
***********************************************************

  Data structs for Particle FXs

***********************************************************
**********************************************************/

/**********************************************************
	FX GAS
**********************************************************/
#define FXGASVTXFLAG (D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE)

typedef struct _fxGas_vtx {
	float		loc[eMaxPt];
	float		scale;
	DWORD		clr;				//color
} fxGas_vtx;

typedef struct _fxGas_par {
	float		scaleRate;
	//float		scale;
	bool		bDeath;				//DEATH!!
} fxGas_par;

typedef struct _fxGas {
	hTXT		gasTxt;				//gas image
	DWORD		clr;				//color
	float		upVec[eMaxPt];		//up vector
	float		size;				//max scale size of each gas before disappearing
	float		minBound[eMaxPt];	//upper-left bound
	float		maxBound[eMaxPt];	//lower right bound
	float		minGrowth,maxGrowth;//growth range min-max rate
	DWORD		maxParticle;		//maximum particle to be displayed
	fxGas_par	*bunchOgas;			//a bunch of gas
	IDirect3DVertexBuffer8 *gasVtx; //a bunch of gas vtx corresponding to bunchOgas
} fxGas;

/**********************************************************
	FX GAS 2
**********************************************************/
typedef struct _fxGas2_par {
	fxGas_vtx	curVtx;
	float		scaleRate;
	float		vel[eMaxPt];		//velocity
	bool		bDeath;				//DEATH!!
} fxGas2_par;

typedef struct _fxGas2 {
	hTXT		gasTxt;				//gas image

	fxGas2_par	*bunchOgas;			//a bunch of gas
	IDirect3DVertexBuffer8 *gasVtx; //a bunch of gas vtx corresponding to bunchOgas

	float		size;				//max scale size of each gas before disappearing

	DWORD       numAlive;			//number of gas still alive
	DWORD		maxParticle;		//maximum particles
} fxGas2;

//func.
PROTECTED RETCODE fxGasFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);
PROTECTED RETCODE fxGas2Func(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);

#endif