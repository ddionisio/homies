#include "gdix_particlefx.h"
#include "gdix_particlefx_i.h"
#include "gdix_i.h"

list<hPARFX> g_parFXList;

/**********************************************************
***********************************************************

  The methods for all FX

***********************************************************
**********************************************************/

//
// A bunch of FX functions
//
PRIVATE PAREFFECT ParEffectTable[ePARFX_MAX] = {fxGasFunc, fxGas2Func};

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void PARFXDestroy(hPARFX *parFX)
{
	if(*parFX)
	{
		g_parFXList.remove(*parFX);

		if((*parFX)->Effect)
			(*parFX)->Effect(*parFX, PARFXM_DESTROY, 0, 0);

		if((*parFX)->data)
			MemFree((void**)&(*parFX)->data);

		MemFree((void**)parFX);
	}

	parFX=0;
}

PUBLIC void PARFXDestroyAll()
{
	hPARFX parFX;

	list<hPARFX>::iterator i;

	while(g_parFXList.size() > 0)
	{
		parFX = g_parFXList.back();
		g_parFXList.remove(parFX);
		PARFXDestroy(&parFX);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hPARFX PARFXCreate(ePARFXType type, void *initStruct, int maxDuration,
						int delay, hOBJ objAttach, COLLISIONFUNC collision)
{
	hPARFX newParFX; MemAlloc((void**)&newParFX, sizeof(gfxParticleFX), M_ZERO);

	if(!newParFX) { ASSERT_MSG(0, "Unable to allocate new particle", "PARFXCreate"); return 0; }

	MemSetPattern(newParFX, "PAFX");

	newParFX->type = type;
	newParFX->duration = newParFX->maxDuration = maxDuration;

	newParFX->collision = collision;

	if(objAttach)
		newParFX->objKey = OBJGetKey(objAttach);

	newParFX->Effect = ParEffectTable[type];

	TimerInit(&newParFX->ticker, MILLISECOND, delay);

	if(newParFX->Effect(newParFX, PARFXM_CREATE, (LPARAM)initStruct, 0) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to initialize new particle", "PARFXCreate"); PARFXDestroy(&newParFX); return 0; }

	g_parFXList.push_back(newParFX);

	return newParFX;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE PARFXUpdate(hPARFX parFX)
{
	RETCODE ret=RETCODE_SUCCESS;

	if(parFX->duration == 0) //if duration is over...then return BREAK
		ret = RETCODE_BREAK;
	else if(TimerEllapse(&parFX->ticker))
	{
		if(parFX->duration != INFINITE_DURATION)
			parFX->duration--;

		ret = parFX->Effect(parFX, PARFXM_UPDATE, 0, 0);
	}

	return ret;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplay(hPARFX parFX)
{
	return parFX->Effect(parFX, PARFXM_DISPLAY, 0, 0);
}

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
PUBLIC RETCODE PARFXUpdateAll()
{
	hPARFX parFX;

	list<hPARFX>::iterator i_next;

	for(list<hPARFX>::iterator i = g_parFXList.begin(); i != g_parFXList.end(); i=i_next)
	{
		i_next=i; i_next++;

		parFX = *i;
		
		if(PARFXUpdate(parFX)==RETCODE_BREAK)
			PARFXDestroy(&parFX);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	PARFXDisplayAll
// Purpose:	displays all particles
//			created.
// Output:	stuff displayed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE PARFXDisplayAll()
{
	GFXBegin();

	hPARFX parFX;

	for(list<hPARFX>::iterator i = g_parFXList.begin(); i != g_parFXList.end(); ++i)
	{
		parFX = *i;
		
		PARFXDisplay(parFX);
	}

	GFXEnd();

	return RETCODE_SUCCESS;
}