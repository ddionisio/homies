#include "gdix.h"
#include "gdix_i.h"

#define HEIGHTITER		.5f
#define MIDDLEHEIGHT	.25f

#define GETHEIGHT(h) _GFXMathRand(-(h), (h))

//This is the midpoint fractal terrain generator

//model map generator stuff

PRIVATE float _MapGetAvgCorner(int i, int j, int stride, int size, float *points)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "*", and the four value we want to average are
       "X"s:
           X   .   X

           .   *   .

           X   .   X
       */
    return ((float) (points[((i-stride)*size) + j-stride] +
		     points[((i-stride)*size) + j+stride] +
		     points[((i+stride)*size) + j-stride] +
		     points[((i+stride)*size) + j+stride]) * MIDDLEHEIGHT);
}

PRIVATE float _MapGetAvgDiamond(int i, int j, int stride,
			     int size, int subSize, float *points)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "X", and the four value we want to average are
       "*"s:
           .   *   .

           *   X   *

           .   *   .
       */

    /* In order to support tiled surfaces which meet seamless at the
       edges (that is, they "wrap"), We need to be careful how we
       calculate averages when the i,j diamond center lies on an edge
       of the array. The first four 'if' clauses handle these
       cases. The final 'else' clause handles the general case (in
       which i,j is not on an edge).
     */
    if (i == 0)
	return ((float) (points[(i*size) + j-stride] +
			 points[(i*size) + j+stride] +
			 points[((subSize-stride)*size) + j] +
			 points[((i+stride)*size) + j]) * MIDDLEHEIGHT);
    else if (i == size-1)
	return ((float) (points[(i*size) + j-stride] +
			 points[(i*size) + j+stride] +
			 points[((i-stride)*size) + j] +
			 points[((0+stride)*size) + j]) * MIDDLEHEIGHT);
    else if (j == 0)
	return ((float) (points[((i-stride)*size) + j] +
			 points[((i+stride)*size) + j] +
			 points[(i*size) + j+stride] +
			 points[(i*size) + subSize-stride]) * MIDDLEHEIGHT);
    else if (j == size-1)
	return ((float) (points[((i-stride)*size) + j] +
			 points[((i+stride)*size) + j] +
			 points[(i*size) + j-stride] +
			 points[(i*size) + 0+stride]) * MIDDLEHEIGHT);
    else
	return ((float) (points[((i-stride)*size) + j] +
			 points[((i+stride)*size) + j] +
			 points[(i*size) + j-stride] +
			 points[(i*size) + j+stride]) * MIDDLEHEIGHT);
}

PRIVATE void _MapGeneratePoints(float *points, float height, float r, int size)
{
	int subSize = size;
	size++;

	float hRatio = (float)pow(2.,-r); //height ratio
	float hScale = height*hRatio;

	//generate the first four value
	//let the first four corner have the same value

	int stride = subSize/2;
    points[(0*size)+0] =
      points[(subSize*size)+0] =
        points[(subSize*size)+subSize] =
          points[(0*size)+subSize] = 0.f;

	//now go through each of the 4 subdivisions and create midpoint heights
	//the first iteration would produce the center (the diamond process) then
	//we will take each four corners and randomize the height (the square process)
	//then we go back to (the diamond process) and so on...until stride reaches 0
	int i, j, oddLine;
	while (stride)
	{
		//Do the diamond process
		//Creating the center point
		for(i = stride; i < subSize; i += stride)
		{
			for(j = stride; j < subSize; j += stride)
			{
				points[(i * size) + j] =
					hScale * GETHEIGHT(HEIGHTITER) +
					_MapGetAvgCorner(i, j, stride, size, points);
				j += stride;
			}
			i += stride;
		}

		//Do the square process
		//Split to four regions and generate corner heights
		oddLine = 0;
		for(i = 0; i < subSize; i += stride)
		{
		    oddLine = (oddLine == 0);
			for(j = 0; j < subSize; j += stride)
			{
				if((oddLine) && !j) j += stride;

				/* i and j are setup. Call avgDiamondVals with the
				   current position. It will return the average of the
				   surrounding diamond data points. */
				points[(i * size) + j] =
					hScale * GETHEIGHT(HEIGHTITER) +
					_MapGetAvgDiamond(i, j, stride, size, subSize, points);

				/* To wrap edges seamlessly, copy edge values around
				   to other side of array */
				if (i==0)
					points[(subSize*size) + j] =
						points[(i * size) + j];
				if (j==0)
					points[(i*size) + subSize] =
						points[(i * size) + j];

				j+=stride;
			}
		}

		//reduce height range and move on to other corners
		hScale *= hRatio;
		stride >>= 1;
	}
}

PRIVATE RETCODE _MapSetCel(gfxCel *cel, const gfxVtx *theVtx)
{
	cel->numVtx = NUMPTQUAD;

	if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(gfxVtx)*cel->numVtx,
		D3DUSAGE_DYNAMIC, GFXVERTEXFLAG, D3DPOOL_DEFAULT, &cel->vtx),
		true, "Error in _MapSetCel"))
		return RETCODE_FAILURE;

	//fill 'er up
	gfxVtx *pVtx;
	
	D3DXPLANE planeOne, planeTwo;

	if(SUCCEEDED(cel->vtx->Lock(0,0, (BYTE**)&pVtx, D3DLOCK_DISCARD)))
	{
		memcpy(pVtx, theVtx, sizeof(gfxVtx)*cel->numVtx);

		D3DXPlaneFromPoints(&planeOne,
						    &D3DXVECTOR3(pVtx[0].x,pVtx[0].y,pVtx[0].z),
							&D3DXVECTOR3(pVtx[1].x,pVtx[1].y,pVtx[1].z),
							&D3DXVECTOR3(pVtx[2].x,pVtx[2].y,pVtx[2].z));

		D3DXPlaneFromPoints(&planeTwo,
						    &D3DXVECTOR3(pVtx[2].x,pVtx[2].y,pVtx[2].z),
							&D3DXVECTOR3(pVtx[3].x,pVtx[3].y,pVtx[3].z),
							&D3DXVECTOR3(pVtx[0].x,pVtx[0].y,pVtx[0].z));

		cel->plane[0][ePA] = planeOne.a; cel->plane[1][ePA] = planeTwo.a;
		cel->plane[0][ePB] = planeOne.b; cel->plane[1][ePB] = planeTwo.b;
		cel->plane[0][ePC] = planeOne.c; cel->plane[1][ePC] = planeTwo.c;
		cel->plane[0][ePD] = planeOne.d; cel->plane[1][ePD] = planeTwo.d;

		pVtx[2].nX = pVtx[0].nX = (planeOne.a + planeTwo.a)/2; pVtx[1].nX = planeOne.a; pVtx[3].nX = planeTwo.a;
		pVtx[2].nY = pVtx[0].nY = (planeOne.b + planeTwo.b)/2; pVtx[1].nY = planeOne.b; pVtx[3].nY = planeTwo.b;
		pVtx[2].nZ = pVtx[0].nZ = (planeOne.c + planeTwo.c)/2; pVtx[1].nZ = planeOne.c; pVtx[3].nZ = planeTwo.c;
		
		cel->vtx->Unlock();
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTMAP TMapCreateRand(float celSizeX, float celSizeZ, float height,
							int xDir, int zDir, float r, unsigned int numIter,
							hTXT texture, D3DMATERIAL8 *mat)
{
	int size = 1<<numIter;
	int numVtx = (size+1)*(size+1);

	//allocate the points
	float *points;

	//HACK for now...
	if(MemAlloc((void**)&points, sizeof(float)*(numVtx+2), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate points", "Error in MDLGenMap"); return 0; }

	//generate the points!
	_MapGeneratePoints(points, height, r, size);
	
	hTMAP newMap;

	//allocate map
	if(MemAlloc((void**)&newMap, sizeof(gfxTMap), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new terrain map", "Error in MapRandTerrain"); return 0; }
	MemSetPattern(newMap, "GFXTMAP");

	//set up mtx
	_GFXMathMtxLoadIden(&newMap->wrldMtx);

	newMap->celSize[eX] = celSizeX;
	newMap->celSize[eY] = height;
	newMap->celSize[eZ] = celSizeZ;

	newMap->indSize.cy = newMap->indSize.cx = size;

	if(texture)
	{
		newMap->texture = texture;
		TextureAddRef(newMap->texture);
	}

	memcpy(&newMap->material, mat, sizeof(D3DMATERIAL8));
	
	//////////////////////////
	//generate the cels of map
	if(MemAlloc((void**)&newMap->cels, sizeof(gfxCel)*newMap->indSize.cx*newMap->indSize.cy, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate map cels", "Error in MapRandTerrain"); TMapDestroy(&newMap); return 0; }
	MemSetPattern(newMap->cels, "GFXCELS");

	gfxVtx quadVtx[NUMPTQUAD]={0};
	float xFDir = xDir < 0 ? -1 : 1;
	float zFDir = zDir < 0 ? -1 : 1;

	for(int row = 0; row < newMap->indSize.cy; row++)
	{
		for(int col = 0; col < newMap->indSize.cx; col++)
		{
			//create cel with the quad

			/*
			0-----3
			|\    |
			|  \  |
			|    \|
			1-----2
			*/

			int dot = col+(row*(size+1));

			quadVtx[0].x = xDir*newMap->celSize[eX]*col;
			quadVtx[0].y = points[dot];
			quadVtx[0].z = zDir*newMap->celSize[eZ]*row;
			quadVtx[0].s = quadVtx[0].x/TextureGetWidth(newMap->texture);
			quadVtx[0].t = quadVtx[0].z/TextureGetHeight(newMap->texture);
			quadVtx[0].color = 0xffffffff;
			
			quadVtx[1].x = xDir*newMap->celSize[eX]*col;
			quadVtx[1].y = points[dot+size+1];
			quadVtx[1].z = zDir*newMap->celSize[eZ]*(row+1);
			quadVtx[1].s = quadVtx[1].x/TextureGetWidth(newMap->texture);
			quadVtx[1].t = quadVtx[1].z/TextureGetHeight(newMap->texture);
			quadVtx[1].color = 0xffffffff;
			
			quadVtx[2].x = xDir*newMap->celSize[eX]*(col+1);
			quadVtx[2].y = points[dot+size+2];
			quadVtx[2].z = zDir*newMap->celSize[eZ]*(row+1);
			quadVtx[2].s = quadVtx[2].x/TextureGetWidth(newMap->texture);
			quadVtx[2].t = quadVtx[2].z/TextureGetHeight(newMap->texture);
			quadVtx[2].color = 0xffffffff;
			
			quadVtx[3].x = xDir*newMap->celSize[eX]*(col+1);
			quadVtx[3].y = points[dot+1];
			quadVtx[3].z = zDir*newMap->celSize[eZ]*row;
			quadVtx[3].s = quadVtx[3].x/TextureGetWidth(newMap->texture);
			quadVtx[3].t = quadVtx[3].z/TextureGetHeight(newMap->texture);
			quadVtx[3].color = 0xffffffff;

			_MapSetCel(&CELL(newMap->cels, row, col, newMap->indSize.cx), quadVtx);
		}
	}

	////////////////
	//clean up stuff
	MemFree((void**)&points);
	////////////////

	return newMap;
}