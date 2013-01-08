#include "gdix.h"
#include "gdix_i.h"

//All map manipulation, destruction are done here

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TMapDisplay(hTMAP map, int cBeginCol, int cBeginRow, int cEndCol, int cEndRow)
{
	//assume that user has called GFXBegin()
	g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	g_p3DDevice->SetVertexShader(GFXVERTEXFLAG); //nothing cool for now...

	//do transformations
    g_p3DDevice->SetTransform(D3DTS_WORLD, &map->wrldMtx.mtxWrld);

	//set up texture/material
	if(map->texture)
		TextureSet(map->texture, 0);

	g_p3DDevice->SetMaterial(&map->material);

	//go through given begin and end col/row
	//render 'em
	if(cEndCol == 0) cEndCol = map->indSize.cx-1;
	if(cEndRow == 0) cEndRow = map->indSize.cy-1;

	D3DPRIMITIVETYPE type;

	if(TESTFLAGS(map->status, TMAP_DRAWLINE))
		type=D3DPT_LINESTRIP;
	else
		type=D3DPT_TRIANGLEFAN;

	for(int row = cBeginRow; row <= cEndRow; row++)
	{
		for(int col = cBeginCol; col <= cEndCol; col++)
		{
			gfxCel *cel = &CELL(map->cels, row, col, map->indSize.cx);

			g_p3DDevice->SetStreamSource(0, cel->vtx, sizeof(gfxVtx));
			g_p3DDevice->DrawPrimitive(type, 0, 2);

			if(cel->subTxt)
			{
				TextureSet(cel->subTxt, 0);
				g_p3DDevice->DrawPrimitive(type, 0, 2);
				TextureSet(map->texture, 0);
			}
		}
	}

	g_p3DDevice->SetTexture(0,0);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TMapDestroy(hTMAP *map)
{
	if(*map)
	{
		if((*map)->cels)
		{
			int max = (*map)->indSize.cx*(*map)->indSize.cy;
			for(int i = 0; i < max; i++)
			{
				if((*map)->cels[i].vtx)
					(*map)->cels[i].vtx->Release();

				if((*map)->cels[i].subTxt) //hopefully this will just decrease ref...
					TextureDestroy(&(*map)->cels[i].subTxt);
			}

			MemFree((void**)&(*map)->cels);
		}

		if((*map)->texture) //hopefully this will just decrease ref...
			TextureDestroy(&(*map)->texture);

		MemFree((void**)map);
	}

	map = 0;
}

/////////////////////////////////////
// Name:	TMapGetSize
// Purpose:	grabs the cel size of the
//			map.  eX/eZ is what you want
// Output:	size is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetSize(hTMAP map, float size[eMaxPt])
{
	memcpy(size, map->celSize, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	TMapGetCelVtx
// Purpose:	gets the quad vtx of the
//			specified cel of map
// Output:	vtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetCelVtx(hTMAP map, int ind, float vtx[NUMPTQUAD][eMaxPt])
{
	gfxVtx *pVtx;

	if(SUCCEEDED(map->cels[ind].vtx->Lock(0,0,(BYTE**)&pVtx, 0)))
	{
		vtx[0][eX] = pVtx[0].x; vtx[0][eY] = pVtx[0].y; vtx[0][eZ] = pVtx[0].z;
		vtx[1][eX] = pVtx[1].x; vtx[1][eY] = pVtx[1].y; vtx[1][eZ] = pVtx[1].z;
		vtx[2][eX] = pVtx[2].x; vtx[2][eY] = pVtx[2].y; vtx[2][eZ] = pVtx[2].z;
		vtx[3][eX] = pVtx[3].x; vtx[3][eY] = pVtx[3].y; vtx[3][eZ] = pVtx[3].z;

		map->cels[ind].vtx->Unlock();
	}
}

/////////////////////////////////////
// Name:	TMapClearCelTxt
// Purpose:	clear cel texture
// Output:	cel texture removed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapClearCelTxt(hTMAP map, int ind)
{
	if(map->cels[ind].subTxt)
		TextureDestroy(&map->cels[ind].subTxt);

	map->cels[ind].subTxt = 0;
}

/////////////////////////////////////
// Name:	TMapSetCelTxt
// Purpose:	set cel's texture
// Output:	cel's texture is set
// Return:	none
/////////////////////////////////////
PUBLIC void TMapSetCelTxt(hTMAP map, int ind, hTXT txt)
{
	TMapClearCelTxt(map, ind);

	map->cels[ind].subTxt = txt;
	TextureAddRef(txt);
}

/////////////////////////////////////
// Name:	TMapSetRenderType
// Purpose:	sets map to render in
//			lines for now...
// Output:	map status changed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapSetRenderType(hTMAP map, unsigned int passInZeroForNow)
{
	SETFLAG(map->status, TMAP_DRAWLINE);
}

/////////////////////////////////////
// Name:	TMapResetRenderType
// Purpose:	sets map to normal render
// Output:	map status changed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapResetRenderType(hTMAP map)
{
	CLEARFLAG(map->status, TMAP_DRAWLINE);
}

/////////////////////////////////////
// Name:	TMapGetInd
// Purpose:	grabs the index from given
//			loc.
// Output:	none
// Return:	index within map (ind<0 for outofbound)
/////////////////////////////////////
PUBLIC int TMapGetInd(hTMAP map, float Point [eMaxPt], float dirX, float dirZ)
{
	int col = Point[eX]/(dirX*map->celSize[eX]);
	int row = Point[eZ]/(dirZ*map->celSize[eZ]);

	if(col >= map->indSize.cx || row >= map->indSize.cy)
		return -1;

	return (row * map->indSize.cx) + col;
}

/////////////////////////////////////
// Name:	TMapGetPlaneEq
// Purpose:	grabs the two planes of
//			a cel
// Output:	pl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetPlaneEq(hTMAP map, int ind, float pl[2][eMaxPlaneEq])
{
	memcpy(pl, map->cels[ind].plane, sizeof(float)*2*eMaxPlaneEq);
}

/////////////////////////////////////
// Name:	TMapGetIndSize
// Purpose:	grabs the index size of
//			map (row,col)
// Output:	none
// Return:	index size
/////////////////////////////////////
PUBLIC SIZE TMapGetIndSize(hTMAP map)
{
	return map->indSize;
}

/////////////////////////////////////
// Name:	TMapGetTexture
// Purpose:	grabs the terrain texture
// Output:	none
// Return:	terrain texture
/////////////////////////////////////
PUBLIC hTXT TMapGetTexture(hTMAP map)
{
	return map->texture;
}

/////////////////////////////////////
// Name:	TMapSetTxtSprRand
// Purpose:	makes your map look cool
// Output:	map texture is changed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE TMapSetTxtSprRand(hTMAP map, hSPRITE spr)
{
	TextureDestroy(&map->texture);

	map->texture = spr->theTexture; TextureAddRef(map->texture);

	SIZE txtSize = {TextureGetWidth(map->texture), TextureGetHeight(map->texture)};

	gfxCel *thisCel;
	gfxVtx *pVtx;

	for(int row = 0; row < map->indSize.cy; row++)
	{
		for(int col = 0; col < map->indSize.cx; col++)
		{
			thisCel = &CELL(map->cels, row, col, map->indSize.cx);

			if(SUCCEEDED(thisCel->vtx->Lock(0,0, (BYTE**)&pVtx, D3DLOCK_DISCARD)))
			{
				int frameInd = rand()%spr->numFrames;

				/*
				0-----3
				|\    |
				|  \  |
				|    \|
				1-----2
				*/

				pVtx[0].s = ((float)spr->frameRects[frameInd].left)/txtSize.cx;
				pVtx[0].t = ((float)spr->frameRects[frameInd].top)/txtSize.cy;
				
				pVtx[1].s = ((float)spr->frameRects[frameInd].left)/txtSize.cx;
				pVtx[1].t = ((float)spr->frameRects[frameInd].bottom)/txtSize.cy;
				
				pVtx[2].s = ((float)spr->frameRects[frameInd].right)/txtSize.cx;
				pVtx[2].t = ((float)spr->frameRects[frameInd].bottom)/txtSize.cy;
				
				pVtx[3].s = ((float)spr->frameRects[frameInd].right)/txtSize.cx;
				pVtx[3].t = ((float)spr->frameRects[frameInd].top)/txtSize.cy;

				thisCel->vtx->Unlock();
			}
		}
	}

	return RETCODE_SUCCESS;
}