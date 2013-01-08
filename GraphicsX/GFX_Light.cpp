#include "gdix.h"
#include "gdix_i.h"

/////////////////////////////////////
// Name:	LightIsEnable
// Purpose:	checks to see if given
//			light is enabled
// Output:	none
// Return:	TRUE if enabled
/////////////////////////////////////
PUBLIC BOOL LightIsEnable(DWORD ind)
{
	BOOL bEnable; g_p3DDevice->GetLightEnable(ind, &bEnable);
	return bEnable;
}

/////////////////////////////////////
// Name:	LightEnable
// Purpose:	enables/disables light
// Output:	enable/disable light
// Return:	none
/////////////////////////////////////
PUBLIC void LightEnable(DWORD ind, BOOL enable)
{
	if(ind < g_lastLight)
		g_p3DDevice->LightEnable(ind, enable);
}

/////////////////////////////////////
// Name:	LightSet
// Purpose:	set light
// Output:	light is set
// Return:	none
/////////////////////////////////////
PUBLIC void LightSet(DWORD ind, D3DLIGHT8 *lightStruct)
{
	if(lightStruct && ind < g_lastLight)
		g_p3DDevice->SetLight(ind, lightStruct);
}

/////////////////////////////////////
// Name:	LightAdd
// Purpose:	adds new light to GFX
// Output:	increment number of light
// Return:	index of light (use this as
//			an ID to light)
/////////////////////////////////////
PUBLIC DWORD LightAdd(D3DLIGHT8 *lightStruct)
{
	DWORD newInd = g_lastLight;

	if(lightStruct)
		g_p3DDevice->SetLight(newInd, lightStruct);

	g_numLight++;
	g_lastLight++;
	
	LightEnable(newInd, TRUE);
	
	return newInd;
}

/////////////////////////////////////
// Name:	LightRemove
// Purpose:	removes light
// Output:	decrease number of light
//			disables given light ind
// Return:	none
/////////////////////////////////////
PUBLIC void LightRemove(DWORD ind)
{
	LightEnable(ind, FALSE);
	g_numLight--;
}

/////////////////////////////////////
// Name:	LightGetData
// Purpose:	grabs given light's data
// Output:	lightStruct is filled
// Return:	none
/////////////////////////////////////
PUBLIC void LightGetData(DWORD ind, D3DLIGHT8 *lightStruct)
{
	g_p3DDevice->GetLight(ind, lightStruct);
}

/////////////////////////////////////
// Name:	LightGetLastInd
// Purpose:	grabs the last index light
// Output:	none
// Return:	the last light index
/////////////////////////////////////
PUBLIC DWORD LightGetLastInd()
{
	return g_lastLight-1;
}

/////////////////////////////////////
// Name:	LightReset
// Purpose:	disables all light and
//			resets number of lights
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void LightReset()
{
	for(int i = 0; i < g_lastLight; i++)
		LightEnable(i, FALSE);

	g_numLight = g_lastLight = 0;
}