#include "gdix.h"
#include "gdix_i.h"

//this is where all math related stuff happens

//return a random double-precision number between 0 and 1
PROTECTED double _GFXMathDRand()
{
	//make sure all bytes of the int are random
	unsigned int i = rand() + (rand() << 15); 
	unsigned int j = rand() + (rand() << 15); 
	double k = (double)i / (double)(unsigned int)(0x3fffffff) + (double)j / (double)(unsigned int)(0x3fffffff) / (double)(unsigned int)(0x3fffffff);

	//return a number between 0 and 1...
	return k;
}

PROTECTED float _GFXMathRand(float a, float b)
{
	int r;
    float	x;
    
	r = rand ();
    x = (float)(r & 0x7fff) /
		(float)0x7fff;
    return (x * (b - a) + a);
}

PROTECTED void _GFXMathMtxLoadIden(gfxWrldMtx *pWMtx)
{
	D3DXMatrixIdentity(&pWMtx->mtxWrld);
	D3DXMatrixIdentity(&pWMtx->mtxScale);
	D3DXMatrixIdentity(&pWMtx->mtxRot);
	D3DXMatrixIdentity(&pWMtx->mtxTrans);
}

//change later so it's not copyrighted
PROTECTED bool _GFXMathIsPowerOf2(int size)
{
    int i, bitcount = 0;

    /* Note this code assumes that (sizeof(int)*8) will yield the
       number of bits in an int. Should be portable to most
       platforms. */
    for (i=0; i<sizeof(int)*8; i++)
	if (size & (1<<i))
	    bitcount++;
    if (bitcount == 1)
	/* One bit. Must be a power of 2. */
	return true;
    else
	/* either size==0, or size not a power of 2. Sorry, Charlie. */
	return false;
}

/////////////////////////////////////
// Name:	GFXMathCreateRayFromLoc
// Purpose:	create a ray from given 
//			screen loc
// Output:	ray pos&dir is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXMathCreateRayFromLoc(const LOCATION *loc,
										D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir)
{
	D3DXMATRIX mtxProj;
    g_p3DDevice->GetTransform( D3DTS_PROJECTION, &mtxProj );

    //Compute the vector of the pick ray in screen space
    D3DXVECTOR3 v;
    v.x =  (((2.0f*loc->x)/g_SCRNSIZE.cx)-1)/mtxProj._11;
    v.y = -(((2.0f*loc->y)/g_SCRNSIZE.cy)-1)/mtxProj._22;
    v.z =  1.0f;

    // Get the inverse view matrix
    D3DXMATRIX mtxView, m;
    g_p3DDevice->GetTransform( D3DTS_VIEW, &mtxView );
    D3DXMatrixInverse( &m, NULL, &mtxView );

    // Transform the screen space pick ray into 3D space
    pRayDir->x = v.x*m._11 + v.y*m._21 + v.z*m._31;
    pRayDir->y = v.x*m._12 + v.y*m._22 + v.z*m._32;
    pRayDir->z = v.x*m._13 + v.y*m._23 + v.z*m._33;
    pRayPos->x = m._41;
    pRayPos->y = m._42;
    pRayPos->z = m._43;
}

/////////////////////////////////////
// Name:	GFXGetProjMtx
// Purpose:	gets the proj mtx
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXGetProjMtx(float mtx[eMaxMtxElem])
{
	D3DMATRIX d3d_mtx;
	g_p3DDevice->GetTransform(D3DTS_PROJECTION, &d3d_mtx);

	mtx[e_11]=d3d_mtx._11; mtx[e_12]=d3d_mtx._12; mtx[e_13]=d3d_mtx._13; mtx[e_14]=d3d_mtx._14;
    mtx[e_21]=d3d_mtx._21; mtx[e_22]=d3d_mtx._22; mtx[e_23]=d3d_mtx._23; mtx[e_24]=d3d_mtx._24;
    mtx[e_31]=d3d_mtx._31; mtx[e_32]=d3d_mtx._32; mtx[e_33]=d3d_mtx._33; mtx[e_34]=d3d_mtx._34;
    mtx[e_41]=d3d_mtx._41; mtx[e_42]=d3d_mtx._42; mtx[e_43]=d3d_mtx._43; mtx[e_44]=d3d_mtx._44;
}

/////////////////////////////////////
// Name:	GFXGetViewMtx
// Purpose:	gets the view mtx
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXGetViewMtx(float mtx[eMaxMtxElem])
{
	D3DMATRIX d3d_mtx;
	g_p3DDevice->GetTransform(D3DTS_VIEW, &d3d_mtx);

	mtx[e_11]=d3d_mtx._11; mtx[e_12]=d3d_mtx._12; mtx[e_13]=d3d_mtx._13; mtx[e_14]=d3d_mtx._14;
    mtx[e_21]=d3d_mtx._21; mtx[e_22]=d3d_mtx._22; mtx[e_23]=d3d_mtx._23; mtx[e_24]=d3d_mtx._24;
    mtx[e_31]=d3d_mtx._31; mtx[e_32]=d3d_mtx._32; mtx[e_33]=d3d_mtx._33; mtx[e_34]=d3d_mtx._34;
    mtx[e_41]=d3d_mtx._41; mtx[e_42]=d3d_mtx._42; mtx[e_43]=d3d_mtx._43; mtx[e_44]=d3d_mtx._44;
}

PUBLIC void GFXCreateVPMtx(float mtx[eMaxMtxElem])
{
	float vW = ((float)g_SCRNSIZE.cx)/2;
	float vH = ((float)g_SCRNSIZE.cy)/2;

	float aspect = vW/vH;

	//float hFov = D3DXToRadian(FOV/2);
	//int fov = FOV/2;

	float sH = sin(FOV/2)/cos(FOV/2);
	float sW = aspect*sH;

	mtx[e_11]=vW/sW; mtx[e_12]=0;      mtx[e_13]=0; mtx[e_14]=vW;
    mtx[e_21]=0;     mtx[e_22]=-vH/sH; mtx[e_23]=0; mtx[e_24]=vH;
    mtx[e_31]=0;     mtx[e_32]=0;      mtx[e_33]=1; mtx[e_34]=0;
    mtx[e_41]=0;     mtx[e_42]=0;      mtx[e_43]=0; mtx[e_44]=1;
	/*mtx[e_11]=g_SCRNSIZE.cx/(g_SCRNSIZE.cy); mtx[e_12]=0;      mtx[e_13]=0; mtx[e_14]=vW;
    mtx[e_21]=0;     mtx[e_22]=-g_SCRNSIZE.cy/(g_SCRNSIZE.cy); mtx[e_23]=0; mtx[e_24]=vH;
    mtx[e_31]=0;     mtx[e_32]=0;      mtx[e_33]=1; mtx[e_34]=0;
    mtx[e_41]=0;     mtx[e_42]=0;      mtx[e_43]=0; mtx[e_44]=1;*/
}