#include "gdix.h"
#include "gdix_i.h"

#include "gdix_scanline.h"

static float *g_depthBuffer=0;	//allocated to window size

PUBLIC RETCODE SLDepthTerm()
{
	if(g_depthBuffer)
	{
		MemFree((void**)&g_depthBuffer);
		g_depthBuffer=0;
	}

	return RETCODE_SUCCESS;
}

PUBLIC RETCODE SLDepthClear()
{
	int max = g_SCRNSIZE.cx*g_SCRNSIZE.cy, i;

	for(i=0; i < max; i++)
	{
		g_depthBuffer[i]=1000000; //here for now...
	}

	return RETCODE_SUCCESS;
}

PUBLIC RETCODE SLDepthInit()
{
	SLDepthTerm();

	if(MemAlloc((void**)&g_depthBuffer, sizeof(float)*g_SCRNSIZE.cx*g_SCRNSIZE.cy, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate depth buffer", "SLDepthInit"); return RETCODE_FAILURE; }

	SLDepthClear();

	return RETCODE_SUCCESS;
}

// out = m1*m2
PRIVATE void _multMtx(float out[eMaxMtxElem], const float m1[eMaxMtxElem], 
					  const float m2[eMaxMtxElem])
{
	out[e_11] = m1[e_11]*m2[e_11]+m1[e_12]*m2[e_21]+m1[e_13]*m2[e_31]+m1[e_14]*m2[e_41];
	out[e_21] = m1[e_21]*m2[e_11]+m1[e_22]*m2[e_21]+m1[e_23]*m2[e_31]+m1[e_24]*m2[e_41];
	out[e_31] = m1[e_31]*m2[e_11]+m1[e_32]*m2[e_21]+m1[e_33]*m2[e_31]+m1[e_34]*m2[e_41];
	out[e_41] = m1[e_41]*m2[e_11]+m1[e_42]*m2[e_21]+m1[e_43]*m2[e_31]+m1[e_44]*m2[e_41];

	out[e_12] = m1[e_11]*m2[e_12]+m1[e_12]*m2[e_22]+m1[e_13]*m2[e_32]+m1[e_14]*m2[e_42];
	out[e_22] = m1[e_21]*m2[e_12]+m1[e_22]*m2[e_22]+m1[e_23]*m2[e_32]+m1[e_24]*m2[e_42];
	out[e_32] = m1[e_31]*m2[e_12]+m1[e_32]*m2[e_22]+m1[e_33]*m2[e_32]+m1[e_34]*m2[e_42];
	out[e_42] = m1[e_41]*m2[e_12]+m1[e_42]*m2[e_22]+m1[e_43]*m2[e_32]+m1[e_44]*m2[e_42];

	out[e_13] = m1[e_11]*m2[e_13]+m1[e_12]*m2[e_23]+m1[e_13]*m2[e_33]+m1[e_14]*m2[e_43];
	out[e_23] = m1[e_21]*m2[e_13]+m1[e_22]*m2[e_23]+m1[e_23]*m2[e_33]+m1[e_24]*m2[e_43];
	out[e_33] = m1[e_31]*m2[e_13]+m1[e_32]*m2[e_23]+m1[e_33]*m2[e_33]+m1[e_34]*m2[e_43];
	out[e_43] = m1[e_41]*m2[e_13]+m1[e_42]*m2[e_23]+m1[e_43]*m2[e_33]+m1[e_44]*m2[e_43];

	out[e_14] = m1[e_11]*m2[e_14]+m1[e_12]*m2[e_24]+m1[e_13]*m2[e_34]+m1[e_14]*m2[e_44];
	out[e_24] = m1[e_21]*m2[e_14]+m1[e_22]*m2[e_24]+m1[e_23]*m2[e_34]+m1[e_24]*m2[e_44];
	out[e_34] = m1[e_31]*m2[e_14]+m1[e_32]*m2[e_24]+m1[e_33]*m2[e_34]+m1[e_34]*m2[e_44];
	out[e_44] = m1[e_41]*m2[e_14]+m1[e_42]*m2[e_24]+m1[e_43]*m2[e_34]+m1[e_44]*m2[e_44];
}

// out = m*v
PRIVATE void _multVtxMtx(float out[eMaxPt], const float m[eMaxMtxElem], const float v[eMaxPt])
{
	out[eX] = m[e_11]*v[eX]+m[e_12]*v[eY]+m[e_13]*v[eZ]+m[e_14];
	out[eY] = m[e_21]*v[eX]+m[e_22]*v[eY]+m[e_23]*v[eZ]+m[e_24];
	out[eZ] = m[e_31]*v[eX]+m[e_32]*v[eY]+m[e_33]*v[eZ]+m[e_34];
}

//----------------------------------------------------------------------------
// Function Prototype:    transformPolygon(VertexTable	*vtxTable, 
//                                         f64			matrix[4][4]); 
//		
// Parameter Description: 
//		vtxTable:	Pointer to a vertex table;
//		matrix:		A 4x4 array contains a homogenius transformation matrix
//					from the world space to the projected space.
//
// Function description:    
//		The function transforms the geometry of all vertices in a given vertex 
//		table into the projected space specified by argument "matrix[][]".
// 	
// Return: void
// 
//----------------------------------------------------------------------------
PRIVATE void _transformPoints(float *pts[eMaxPt], float coeff[eMaxPlaneEq],
							  const int numPt,
							  const float wrldMtx[eMaxMtxElem],
							  const float camMtx[eMaxMtxElem], 
							  const float projMtx[eMaxMtxElem],
							  const float vpMtx[eMaxMtxElem])
{
	if(pts && numPt >= NUMPTFACE)
	{
		D3DXPLANE coefPl;

		float theMtx[eMaxMtxElem];
		float vtx[eMaxPt];

		_multMtx(theMtx, wrldMtx, camMtx);

		//calculate plane from first three points
		D3DXPlaneFromPoints(&coefPl,
			&D3DXVECTOR3(pts[0][eX], pts[0][eY], pts[0][eZ]),
			&D3DXVECTOR3(pts[1][eX], pts[1][eY], pts[1][eZ]),
			&D3DXVECTOR3(pts[2][eX], pts[2][eY], pts[2][eZ]));

		//translate it
		D3DXPlaneTransform(&coefPl, &coefPl,
			&D3DXMATRIX(theMtx));

		//mult proj
		_multMtx(theMtx, theMtx, projMtx);

		for(int i = 0; i < numPt; i++)
		{
			vtx[eX] = pts[i][eX]; vtx[eY] = pts[i][eY]; vtx[eZ] = pts[i][eZ];

			_multVtxMtx(vtx, theMtx, vtx);
			
			//not sure...
			//vtxTransTable->vertexList[i][X] /= vtxTransTable->vertexList[i][Z];
			//vtxTransTable->vertexList[i][Y] /= vtxTransTable->vertexList[i][Z];}
			
			_multVtxMtx(vtx, vpMtx, vtx);

			pts[i][eX] = vtx[eX]; pts[i][eY] = vtx[eY]; pts[i][eZ] = vtx[eZ];
		}
	}
}

PRIVATE EdgeNode * _ENPushFrontNew(ETEntry *thisET, float ymax, float xlow, float zlow, 
						   float dx, float dz, float DZ, int polyID,
						   int vtxID1, int vtxID2)
{
	EdgeNode *oldNode=thisET->EdgeNode;

	EdgeNode *newNode;//=(EdgeNode*)malloc(sizeof(EdgeNode));

	if(MemAlloc((void**)&newNode, sizeof(EdgeNode), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new node", "_ENPushFrontNew"); return 0; }

	newNode->ymax	= ymax;
	newNode->xlow	= xlow;
	newNode->zlow	= zlow;
	newNode->dx		= dx;
	newNode->dz		= dz;
	newNode->DZ		= DZ;
	newNode->polyID = polyID;
	newNode->next   = oldNode;

	newNode->vtxID1 = vtxID1;
	newNode->vtxID2 = vtxID2;

	thisET->EdgeNode = newNode;

	return newNode;
}

PRIVATE ETEntry	* _ETCreate(int y)
{
	ETEntry *newET;//=(ETEntry*)malloc(sizeof(ETEntry));

	if(MemAlloc((void**)&newET, sizeof(ETEntry), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new ET", "_ETCreate"); return 0; }

	newET->y = y;
	newET->EdgeNode = 0;
	newET->next = 0;

	return newET;
}

//this will create a new ET node and inserted to given yMin
//this will also create the edgenode
//returns the head
PRIVATE ETEntry * _ETInsertNew(ETEntry *head, int yMin,
							  float ymax, float xlow, float zlow, 
						   float dx, float dz, float DZ, int polyID,
						   int vtxID1, int vtxID2)
{
	ETEntry *theHead=head;
	ETEntry *node=theHead, *newNode, *next;

	while(node)
	{
		next = node->next;

		if(((node->y < yMin) && (next && (next->y > yMin))) 
			|| ((node->y < yMin) && !next))
		{
			newNode=_ETCreate(yMin);
			node->next = newNode;
			newNode->next = next;
			node=newNode;
			break;
		}
		else if(node == head && node->y > yMin)
		{
			newNode=_ETCreate(yMin);
			newNode->next=node;
			theHead=newNode;
			node=newNode;
			break;
		}
		else if(node->y == yMin)
			break;

		node = next;
	}

	if(node)
		_ENPushFrontNew(node, ymax, xlow, zlow, dx, dz, DZ, polyID,
		vtxID1, vtxID2);
	else if(!theHead) //that means we haven't init. ET yet
	{
		theHead=_ETCreate(yMin);
		_ENPushFrontNew(theHead, ymax, xlow, zlow, dx, dz, DZ, polyID,
			vtxID1, vtxID2);
	}

	return theHead;
}

PRIVATE void _ETDestroy(ETEntry *thisET)
{
	if(thisET)
	{
		//remove each edge node
		EdgeNode *node=thisET->EdgeNode, *next;

		while(node)
		{
			next = node->next;

			MemFree((void**)&node);

			node = next;
		}

		MemFree((void**)&thisET);
		thisET=0;
	}
}

//returns the head or new head
//destroys given thisET
//note:this will not destroy the Edge nodes
static ETEntry * _ETRemove(ETEntry *thisET, ETEntry *head, ETEntry *prev)
{
	ETEntry *theHead=head;

	if(head && thisET)
	{
		if(!prev || thisET == head) //that means we want to remove the head
			theHead = thisET->next;
		else
			prev->next = thisET->next;

		MemFree((void**)&thisET);
		thisET=0;
	}

	return theHead;
}

//returns true if ET is still not empty
PRIVATE int _ETIsEmpty(ETEntry *thisET)
{
	if(thisET)
	{
		if(thisET->EdgeNode || thisET->next) //that means there is still something...
			return 0;
	}

	return 1;
}

PRIVATE EdgeNode * _AETRemoveStuff(EdgeNode *AET, EdgeNode *newNode, int curY)
{
	EdgeNode *head=AET;
	EdgeNode *checkNode=head, *checkNext, *checkPrev=0;

	while(checkNode)
	{
		checkNext=checkNode->next;

		if(checkNode != newNode)
		{
			//check if new node shares the same vtx with given node
			if((newNode && ((checkNode->vtxID1 == newNode->vtxID1) || (checkNode->vtxID2 == newNode->vtxID2)
				|| (checkNode->vtxID1 == newNode->vtxID2) || (checkNode->vtxID2 == newNode->vtxID1))
				&& curY == (int)checkNode->ymax) || curY > (int)checkNode->ymax)
			{
				if(!checkPrev)
					head=checkNext;
				else
					checkPrev->next=checkNext;

				MemFree((void**)&checkNode);
				checkNode=0;
			}
		}

		checkPrev=checkNode;
		checkNode=checkNext;
	}

	return head;
}

//AET thing
//inserts given edgenode in xmin order to given AET head
//returns head of AET
static EdgeNode * _AETInsert(EdgeNode *AET, EdgeNode *stuff, int curY)
{
	EdgeNode *stuffNode=stuff, *stuffNext;

	EdgeNode *head=AET, *node, *next;

	while(stuffNode)
	{
		stuffNext=stuffNode->next;
		node=head;

		while(node)
		{
			next=node->next;

			if(((node->xlow <= stuffNode->xlow) && (next && (next->xlow >= stuffNode->xlow)))
				|| ((node->xlow <= stuffNode->xlow) && !next))
			{
				node->next = stuffNode;
				stuffNode->next = next;
				break;
			}
			else if(node == head && node->xlow >= stuffNode->xlow)
			{
				head=stuffNode;
				head->next=node;
				break;
			}

			node=next;
		}

		if(!head) //AET is empty
		{
			head=stuffNode;
			head->next=0;
		}
		
		//cull some stuff from AET
		head=_AETRemoveStuff(head, stuffNode, curY);

		stuffNode=stuffNext;
	}

	return head;
}

//----------------------------------------------------------------------------
// Function Prototype:    makeEdgeTable(VertexTable	*vertexTable, 
//                                      u16			polyVtxList[],
//										u16			vertexCount,
//										s16			minY,
//										s16			maxY,
//										ETEntry		**edgeTable); 
//		
// Parameter Description: 
//		vertexTable:  Pointer to a vertex table;
//		polyVtxList:  Polygon vertex list that contains the indices to 
//					  the given vertex table;
//		vertexCount:  The number of vertices in the polygon vertex list;
//		minY:		  The minimum y value of the edge table;
//		maxY:		  The maximum y value of the edge table;
//		edgeTable:	  Pointer to a list of ETEntry structure, containing  
//					  all non-empty entries in the edge table.
//
// Function description:    
//		Given a polygon (in its vertex index list and corresponding vertex
//		table), the function constructs the edge table for the scan-line
//		conversion algorithm. The minimum and maximum scan-line Y values  
//		are specified by the arguments. 
//		
//		The constructed edge table is stored in argument "edgeTable" and 
//		memory blocks are allocated for entries in the table by the function. 
//		It is caller's responsibility to call freeEdgeTable() to release 
//		the memory afterwards.
//
// Return: the number of entries in the constructed edge table;
// 
//----------------------------------------------------------------------------
int makeEdgeTable(float			*vtx[eMaxPt],
				  int			polyID,
				  float			coefs[eMaxPlaneEq],
				  int			vertexCount,
				  int			minY,
				  int			maxY,
				  ETEntry		**edgeTable)
{
	int numEntry=0;

	int maxPt=vertexCount-1, i;
	int ind1,ind2;

	float	ymax,ymin;		// maximum y value among two vertices
	float	xlow;			// x value of minimum y among vertices
	float	zlow;			// z value of minimum y among vertices

	float	dx, dz;			// incremental x and z for next scan line
	float	DZ;				// incremental z for the same scan line

	float 	*pt[2];			//the line segment

	for(i = 0; i < maxPt; i++)
	{
		ind1=i;
		ind2=i+1;

		pt[0] = vtx[ind1];
		pt[1] = vtx[ind2];

		//don't include horizontals
		if(pt[1][eY]!=pt[0][eY])
		{
			ymax = MAX(pt[0][eY],pt[1][eY]);
			ymin = MIN(pt[0][eY],pt[1][eY]);

			xlow = pt[0][eY] < pt[1][eY] ? pt[0][eX] : pt[1][eX];
			zlow = pt[0][eY] < pt[1][eY] ? pt[0][eZ] : pt[1][eZ];

			dx   = (pt[1][eX]-pt[0][eX])/(pt[1][eY]-pt[0][eY]);

			DZ	 = coefs[ePA]/coefs[ePC];

			dz	 = ((coefs[ePA]*dx)+coefs[ePB])/coefs[ePC];

			*edgeTable=_ETInsertNew(*edgeTable, (int)ymin,
							  ymax, xlow, zlow, 
						   dx, dz, DZ, polyID,
						   ind1, ind2);
			numEntry++;
		}
	}

	//insert last
	ind1=maxPt; ind2=0;

	pt[0] = vtx[ind1];
	pt[1] = vtx[ind2];

	//don't include horizontals
	if(pt[1][eY]!=pt[0][eY])
	{
		ymax = MAX(pt[0][eY],pt[1][eY]);
		ymin = MIN(pt[0][eY],pt[1][eY]);

		xlow = pt[0][eY] < pt[1][eY] ? pt[0][eX] : pt[1][eX];
		zlow = pt[0][eY] < pt[1][eY] ? pt[0][eZ] : pt[1][eZ];

		dx   = (pt[1][eX]-pt[0][eX])/(pt[1][eY]-pt[0][eY]);

		DZ	 = coefs[ePA]/coefs[ePC];

		dz	 = ((coefs[ePA]*dx)+coefs[ePB])/coefs[ePC];

		*edgeTable=_ETInsertNew(*edgeTable, (int)ymin,
						  ymax, xlow, zlow, 
					   dx, dz, DZ, polyID,
					   ind1, ind2);
		numEntry++;
	}

	return numEntry;
}


//----------------------------------------------------------------------------
// Function Prototype:    freeEdgeTable(ETEntry  *edgeTable, u16 count);
//		
// Parameter Description: 
//		edgeTable:	  A list of ETEntry structure, containing all non-empty 
//					  entries in the edge table;
//		count:		  The number of entries in the edge table;
//
// Function description:    
//		The function releases the memory allocated for edge node entries 
//		in a given edge table.
//
// Return: void
// 
//----------------------------------------------------------------------------
void freeEdgeTable(ETEntry  *edgeTable, int count)
{
	//remove each edge node
	ETEntry *next;

	while(edgeTable)
	{
		next=edgeTable->next;
		_ETDestroy(edgeTable);
		edgeTable=next;
	}
}

//----------------------------------------------------------------------------
// Function Prototype:    getColorAtPixel(Point3D		location,
//										  u16			polyginID,
//										  PolygonTable  *polygonTable,
//										  ObjColor		pixelColor);
//		
// Parameter Description: 
//		location:	  <x,y,z> coordinates on the given polygon.
//		polygonID:	  the index of the polygon in the polygon table.
//		polygonTable: Pointer to a polygon table;
//		ObjColor:	  the color at the pixel location;
//
// Function description:
//		Given the location of a pixel on a polygon, the function calculates
//		and returns the color (in the RGBA format) by apply the desired 
//		illumination, shading, mapping or shadow models.
//
// Return: void
// 
//----------------------------------------------------------------------------
DWORD getColorAtPixel(float			point[eMaxPt],
					  gfxVtx		*pts,
					  int			ind1,
					  int			ind2,
					  int			ind3)
{
	D3DXVECTOR3 clr(((pts[ind1].color&0x00ff0000)>>16)/((float)255),((pts[ind1].color&0x0000ff00)>>8)/((float)255),(pts[ind1].color&0x000000ff)/((float)255));
	D3DXVECTOR3 lightV(0.57f, 0.57f, 0.57f);

	float scalar = (float)fabs(D3DXVec3Dot(&clr, &lightV));

	DWORD r = clr.x*255*scalar, g = clr.y*255*scalar, b = clr.z*255*scalar;

    return D3DCOLOR_XRGB(r,g,b);
}

//----------------------------------------------------------------------------
// Function Prototype:    scanLineConversion(VertexTable   *vertexTable,
//										     PolygonTable  *polygonTable,
//											 s16		   s_viewPortOrgX,
//											 s16		   s_viewPortOrgY,
//											 s16		   s_viewPortExtX,
//											 s16		   s_viewPortExtY,
//											 f32		   s_viewPlaneLeft,
//											 f32		   s_viewPlaneRight,
//											 f32		   s_viewPlaneBot,
//											 f32		   s_viewPlaneTop,
//											 f64		   matrix[4][4]);
//		
// Parameter Description: 
//		vertexTable:    Pointer to a vertex table;
//		polygonTable:   Pointer to a polygon table;
//
//		s_viewPortOrgX,
//		s_viewPortOrgY,
//		s_viewPortExtX,
//		s_viewPortExtY: These four define the view port space.
//
//		s_viewPlaneLeft: 
//		s_viewPlaneRight: 
//		s_viewPlaneBot: 
//		s_viewPlaneTop: These four define the view plane in projected space.
//
//		matrix:		    A 4x4 array contains a homogenius transformation matrix
//					    from the world space to the projected space.
//
// Function description:
//		The function scan-line converts each polygon in a given polygon table
//		and draw the pixel in the display window.
//
//
// Return: void
// 
//----------------------------------------------------------------------------
void scanLineConversion(hOBJ obj)
{
	float   coef[eMaxPlaneEq];
	float	*pts[eMaxPt];

	if(MemAlloc((void**)&pts, sizeof(float)*eMaxPt*NUMPTFACE, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate pts", "scanLineConversion"); return; }

	int i;
	int numEntry, y;

	ETEntry  *ET=0;
	EdgeNode *AET;

	float wrldMtx[eMaxMtxElem], camMtx[eMaxMtxElem], projMtx[eMaxMtxElem], vpMtx[eMaxMtxElem];

	OBJGetWorldMtx(obj, wrldMtx);
	GFXGetViewMtx(camMtx);
	GFXGetProjMtx(projMtx);
	GFXCreateVPMtx(vpMtx);

	//convert vertices to work space and store it to the transVtxList
	//_transformPolygon(obj, camMtx, projMtx, vpMtx);

	BYTE *ptrInd;
	gfxVtx *ptrV;

	//for each polygon in the obj
	if(SUCCEEDED(obj->curMesh->LockIndexBuffer(0,&ptrInd))
		&& SUCCEEDED(obj->curMesh->LockVertexBuffer(0,(BYTE**)&ptrV)))
	{
		for(i=0; i<obj->theMdl->indCount; i+=NUMPTFACE)
		{
			pts[0][eX] = ptrV[ptrInd[i]].x;
			pts[0][eY] = ptrV[ptrInd[i]].y;
			pts[0][eZ] = ptrV[ptrInd[i]].z;
			pts[1][eX] = ptrV[ptrInd[i+1]].x;
			pts[1][eY] = ptrV[ptrInd[i+1]].y;
			pts[1][eZ] = ptrV[ptrInd[i+1]].z;
			pts[2][eX] = ptrV[ptrInd[i+2]].x;
			pts[2][eY] = ptrV[ptrInd[i+2]].y;
			pts[2][eZ] = ptrV[ptrInd[i+2]].z;

			_transformPoints(pts, coef,
							  NUMPTFACE,
							  wrldMtx, camMtx, 
							  projMtx, vpMtx);

			//make edge table
			numEntry=makeEdgeTable(pts, OBJGetID(obj), coef,
				NUMPTFACE, 0, g_SCRNSIZE.cy, &ET);

	//		assert(ET);

			//set y to first ET
			if(ET)
				y=ET->y;

			//set AET to none
			AET=0;

			do //repeat the following steps until both AET and ET are empty
			{
	//				2.4.1 remove from ET those edges whose ymin = current y value;
	//				2.4.2 insert them into AET in xmin ascending order;
	//				2.4.3 remove from AET those edges whose ymax = y;
	//					  (remember to release the allocated memory)
				if(ET && y==ET->y)
				{
					//this will update the AET with x-min ordered edgenodes
					//it will also remove old edgenodes that are not needed
					AET=_AETInsert(AET, ET->EdgeNode, y);
					ET->EdgeNode=0;
					ET=_ETRemove(ET, ET, 0); //this will update the ET to next
				}
				else
					AET=_AETRemoveStuff(AET, 0, y);

	//				2.4.4 get the color information for each pixel between 
	//					  pairs of x coordinates from AET;
	//				2.4.5 call the z-buffer procedure to draw it in the frame buffer.
				if(AET && (y >= 0 && y < g_SCRNSIZE.cy))
				{
					int x, xp;
					float z;
					float *depth;
					//get pair of lines
					EdgeNode *edge1=AET, *edge2=AET->next;

					while(edge1 && edge2)
					{
						x = edge1->xlow;
						z = edge1->zlow;

						xp=(int)edge2->xlow;
						
						//draw the scanline
						if(x >= 0 && x < g_SCRNSIZE.cx && xp >= 0 && xp < g_SCRNSIZE.cx)
						{
						while(x <= xp)
						{
							if(x >= 0 && x < g_SCRNSIZE.cx)
							{
							depth=&CELL(g_depthBuffer, y, x, g_SCRNSIZE.cx);

							if(*depth > z)
							{
								DWORD clr;
								float pt[eMaxPt]={x,y,z};

								//get color
								clr=getColorAtPixel(pt, ptrV,
									ptrInd[i], ptrInd[i+1], ptrInd[i+2]);

								//set pixel
								GFXDrawPixel(pt[eX], pt[eY], clr);

								*depth = z; //update z buffer
							}
							}
							
							x++;
							z -= edge1->DZ;
						}
						}

	//				2.4.7 update in AET xmin values for the new y for each node;
						edge1->xlow += edge1->dx;
						edge1->zlow -= edge1->dz;
						
						edge2->xlow += edge2->dx;
						edge2->zlow -= edge2->dz;

						//get next pair of lines
						edge1=edge2->next;
						
						if(edge1)
							edge2=edge1->next;
					}
				}

	//				2.4.6 increase the current y value by 1;
				y++;

			} while(!_ETIsEmpty(ET) || AET);

			//destroy edge table
			freeEdgeTable(ET, numEntry);
		}

		obj->curMesh->UnlockIndexBuffer();
		obj->curMesh->UnlockVertexBuffer();
	}

	MemFree((void**)&pts);
}
