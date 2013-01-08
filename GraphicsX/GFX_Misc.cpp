#include "gdix.h"
#include "gdix_i.h"

PUBLIC void GFXHideCursor (void)
{
	ShowCursor (FALSE);
}

PUBLIC void GFXShowCursor (void)
{
	ShowCursor (TRUE);
}

/////////////////////////////////////
// Name:	GFXDrawBoxXZ
// Purpose:	draws a box from xz plane
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawBoxXZ(float xMin, float zMin, float xMax, float zMax, float h, unsigned int color)
{
	D3DXMATRIX mtx;
	g_p3DDevice->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&mtx));

	gfxVtx vtx[NUMPTQUAD*2]={0};

	//line one
	vtx[0].x = xMin; vtx[0].y = h; vtx[0].z = zMin;
	vtx[0].nX = 0; vtx[0].nY = 1; vtx[0].nZ = 0;
	vtx[0].color = color;

	vtx[1].x = xMin; vtx[1].y = h; vtx[1].z = zMax;
	vtx[1].nX = 0; vtx[1].nY = 1; vtx[1].nZ = 0;
	vtx[1].color = color;

	//line two
	vtx[2] = vtx[1];

	vtx[3].x = xMax; vtx[3].y = h; vtx[3].z = zMax;
	vtx[3].nX = 0; vtx[3].nY = 1; vtx[3].nZ = 0;
	vtx[3].color = color;

	//line three
	vtx[4] = vtx[3];

	vtx[5].x = xMax; vtx[5].y = h; vtx[5].z = zMin;
	vtx[5].nX = 0; vtx[5].nY = 1; vtx[5].nZ = 0;
	vtx[5].color = color;

	//line four
	vtx[6] = vtx[5];
	vtx[7] = vtx[0];

	g_p3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, NUMPTQUAD, vtx, sizeof(gfxVtx));
}

#define PIXELFORMAT (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct pixelVtx {
	float	x,y,z,rhw;
	DWORD	clr;
};

/////////////////////////////////////
// Name:	GFXDrawPixel
// Purpose:	draws a 2d pixel
// Output:	yay
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawPixel(float x, float y, unsigned int clr)
{
	pixelVtx pt;
	pt.x=x; pt.y=y; pt.z=1.0f; pt.rhw=1.0f; pt.clr=clr;

	g_p3DDevice->SetVertexShader(PIXELFORMAT);

	g_p3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, &pt, sizeof(pixelVtx));
}