#include "gdix_bkgrndfx.h"
#include "gdix_i.h"

//message for FX functions
typedef enum {
	BKFXM_CREATE,
	BKFXM_DOSTUFF,
	BKFXM_DESTROY
} FXmessage;

//signals for FX update
typedef enum {
	BKFXsignal_animate,
	BKFXsignal_noanimate
} BKFXsignal;

/**********************************************************
***********************************************************

  Data structs for the background FXs

***********************************************************
**********************************************************/

//
// background FX main data struct for each type
//

#define GFXCLRVTX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE) //xyzw and color

typedef struct _gfxClrVtx {
    float x, y, z, rhw;
    DWORD color;
} gfxClrVtx;

//fade in/out
typedef struct _fadeinout {
	BYTE r,g,b,a;
	int curA;						//current alpha
	int dir;						//-spd or spd
	IDirect3DVertexBuffer8 *vtx;	//four point vtx
} fadeinout;

typedef struct _imgfadeinout {
	hTXT theImg;	//image to transit to
	BYTE a;
	int curA;		//current alpha
	int dir;		//-spd or spd
} imgfadeinout;

//curtain
typedef struct _curtain {
	hTXT curtainImg;		//curtain to fall down
	unsigned int fadeColor;	//fadeout color after curtain goes down
} curtain;

/**********************************************************
***********************************************************

  The methods for all FX

***********************************************************
**********************************************************/

//fade in/out
PROTECTED RETCODE bkFadeInOutFunc(hBKFX bkFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	switch(message)
	{
	case BKFXM_DOSTUFF:
		{
			fadeinout *thisData = (fadeinout *)bkFX->Data;

			if(dumbParam == BKFXsignal_animate)
			{
				thisData->curA += thisData->dir;

				if(thisData->curA > thisData->a) thisData->curA = thisData->a;
				else if(thisData->curA < 0) thisData->curA = 0;

				gfxClrVtx *vtxData;

				DWORD clr = D3DCOLOR_RGBA(thisData->r, thisData->g, thisData->b, thisData->curA);

				if(SUCCEEDED(thisData->vtx->Lock(0,0,(BYTE**)&vtxData, 0)))
				{
					vtxData[0].color = clr;
					vtxData[1].color = clr;
					vtxData[2].color = clr;
					vtxData[3].color = clr;
 
					thisData->vtx->Unlock();
				}
			}

			GFXBltModeEnable(true);

			g_p3DDevice->SetVertexShader(GFXCLRVTX); //nothing cool for now...

			g_p3DDevice->SetStreamSource(0, thisData->vtx, sizeof(gfxClrVtx));
			g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

			GFXBltModeDisable();

			//check to see if we are over
			if((thisData->dir < 0 && thisData->curA == 0)
			|| (thisData->dir > 0 && thisData->curA == thisData->a))
				return RETCODE_BREAK;

		}
		break;

	case BKFXM_CREATE:
		{
			fadeinout_init *initData = (fadeinout_init *)dumbParam;

			if(!initData)
				return RETCODE_FAILURE;

			fadeinout *newData; MemAlloc((void**)&newData, sizeof(fadeinout), M_ZERO);

			if(!newData)
				return RETCODE_FAILURE;

			newData->r = initData->r; newData->g = initData->g;
			newData->b = initData->b; newData->a = initData->a;
			
			if(initData->bDir) //fade in
			{
				newData->curA = newData->a;
				newData->dir = -initData->spd;
			}
			else //fade out
			{
				newData->curA = 0;
				newData->dir = initData->spd;
			}

			//create flat plane
			SIZE scrnSize = GFXGetScrnSize();

			if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(gfxClrVtx)*NUMPTQUAD,
				D3DUSAGE_WRITEONLY, GFXCLRVTX, D3DPOOL_MANAGED, &newData->vtx),
				true, "bkFadeInOutFunc"))
					return RETCODE_FAILURE;

			gfxClrVtx *vtxData;

			DWORD clr = D3DCOLOR_RGBA(newData->r, newData->g, newData->b, newData->curA);

			if(SUCCEEDED(newData->vtx->Lock(0,0,(BYTE**)&vtxData, 0)))
			{
				vtxData[0].x = 0; vtxData[0].y = 0; vtxData[0].z = 0; vtxData[0].rhw = 1;
				vtxData[0].color = clr;

				vtxData[1].x = 0; vtxData[1].y = scrnSize.cy; vtxData[1].z = 0; vtxData[1].rhw = 1;
				vtxData[1].color = clr;

				vtxData[2].x = scrnSize.cx; vtxData[2].y = scrnSize.cy; vtxData[2].z = 0; vtxData[2].rhw = 1;
				vtxData[2].color = clr;

				vtxData[3].x = scrnSize.cx; vtxData[3].y = 0; vtxData[3].z = 0; vtxData[3].rhw = 1;
				vtxData[3].color = clr;

				newData->vtx->Unlock();
			}

			bkFX->Data = newData;
		}
		break;

	case BKFXM_DESTROY:
		{
			fadeinout *thisData = (fadeinout *)bkFX->Data;

			if(thisData)
				if(thisData->vtx)
					thisData->vtx->Release();
		}
		break;
	}

	return RETCODE_SUCCESS;
}

//image fade in/out
PROTECTED RETCODE bkImgFadeInOutFunc(hBKFX bkFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	switch(message)
	{
	case BKFXM_DOSTUFF:
		{
			imgfadeinout *thisData = (imgfadeinout *)bkFX->Data;

			if(dumbParam == BKFXsignal_animate)
			{
				thisData->curA += thisData->dir;

				if(thisData->curA > thisData->a) thisData->curA = thisData->a;
				else if(thisData->curA < 0) thisData->curA = 0;
			}

			SIZE sSize = GFXGetScrnSize();
			unsigned int clr = D3DCOLOR_RGBA(255, 255, 255, thisData->curA);

			GFXBltModeEnable(true);
			TextureStretchBlt(thisData->theImg, 0, 0, sSize.cx,sSize.cy,0,&clr,0);
			GFXBltModeDisable();

			//check to see if we are over
			if((thisData->dir < 0 && thisData->curA == 0)
			|| (thisData->dir > 0 && thisData->curA == thisData->a))
				return RETCODE_BREAK;

		}
		break;

	case BKFXM_CREATE:
		{
			imgfadeinout_init *initData = (imgfadeinout_init *)dumbParam;

			if(!initData)
				return RETCODE_FAILURE;

			imgfadeinout *newData; MemAlloc((void**)&newData, sizeof(imgfadeinout), M_ZERO);

			if(!newData)
				return RETCODE_FAILURE;

			newData->a = initData->a;

			if(initData->bDir) //fade in
			{
				newData->curA = newData->a;
				newData->dir = -initData->spd;
			}
			else //fade out
			{
				newData->curA = 0;
				newData->dir = initData->spd;
			}

			//add ref to texture
			newData->theImg = initData->theImg;
			TextureAddRef(newData->theImg);

			bkFX->Data = newData;
		}
		break;

	case BKFXM_DESTROY:
		{
			imgfadeinout *thisData = (imgfadeinout *)bkFX->Data;

			if(thisData)
				TextureDestroy(&thisData->theImg);
		}
		break;
	}

	return RETCODE_SUCCESS;
}

//
// A bunch of FX functions
//
PRIVATE BKEFFECT BKEffectTable[eBKFX_MAX] = {bkFadeInOutFunc,bkImgFadeInOutFunc};

/////////////////////////////////////
// Name:	BKFXDestroy
// Purpose:	destroys given bkFX
// Output:	stuff destroyed
// Return:	none
/////////////////////////////////////
PUBLIC void BKFXDestroy(hBKFX *bkFX)
{
	if(*bkFX)
	{
		if((*bkFX)->Effect)
			(*bkFX)->Effect(*bkFX, BKFXM_DESTROY, 0, 0);

		if((*bkFX)->Data)
			MemFree((void**)&(*bkFX)->Data);

		MemFree((void**)bkFX);
	}

	bkFX=0;
}

/////////////////////////////////////
// Name:	BKFXCreate
// Purpose:	creates a background FX
// Output:	none
// Return:	the new background FX
/////////////////////////////////////
PUBLIC hBKFX BKFXCreate(eBKFXType type, void *initStruct, int maxDuration,
						int delay)
{
	hBKFX newBkFX; MemAlloc((void**)&newBkFX, sizeof(gfxBkFX), M_ZERO);

	if(!newBkFX) { ASSERT_MSG(0, "Unable to allocate new background FX", "BKFXCreate"); return 0; }

	MemSetPattern(newBkFX, "BKFX");

	newBkFX->Type = type;
	newBkFX->MaxDuration = maxDuration;
	newBkFX->Duration = maxDuration;

	newBkFX->Effect = BKEffectTable[type];

	TimerInit(&newBkFX->ticker, MILLISECOND, delay);

	if(newBkFX->Effect(newBkFX, BKFXM_CREATE, (LPARAM)initStruct, 0) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new background FX", "BKFXCreate"); BKFXDestroy(&newBkFX); return 0; }

	return newBkFX;
}

/////////////////////////////////////
// Name:	BKFXUpdate
// Purpose:	updates and displays the
//			background FX
// Output:	stuff displayed
// Return:	RETCODE_BREAK if duration
//			is over or FX expired
/////////////////////////////////////
PUBLIC RETCODE BKFXUpdate(hBKFX bkFX)
{
	RETCODE ret=RETCODE_SUCCESS;

	if(bkFX->Duration == 0) //if duration is over...then return BREAK
		ret = RETCODE_BREAK;
	else if(TimerEllapse(&bkFX->ticker))
	{
		ret = bkFX->Effect(bkFX, BKFXM_DOSTUFF, BKFXsignal_animate, 0);

		if(bkFX->Duration != INFINITE_DURATION)
			bkFX->Duration--;
	}
	else //just display the FX with out animation
		ret = bkFX->Effect(bkFX, BKFXM_DOSTUFF, BKFXsignal_noanimate, 0);

	return ret;
}