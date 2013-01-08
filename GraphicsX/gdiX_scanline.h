#ifndef _gdix_scanline_h
#define _gdix_scanline_h

#include "gdix.h"

// The data structure for the edge node used in both Edge Table and
// Active Edge table. It contains edge vertex information as well as
// the incremental values for the scan line algorithm.
typedef struct _EdgeNode {

	float	ymax;			// maximum y value among two vertices
	float	xlow;			// x value of minimum y among vertices
	float	zlow;			// z value of minimum y among vertices

	float	dx, dz;			// incremental x and z for next scan line
	float	DZ;				// incremental z for the same scan line
	
	int		polyID;			// index of polygon in the polygonTable
	int		vtxID1,vtxID2;	// index of 2 vertices of this node

	struct _EdgeNode *next; // pointer to next edge node in table
} EdgeNode;


// The data structure for entries in the Edge Table;
typedef struct _ETEntry {

	int		y;				// the Y value of a scanline;
	EdgeNode	*EdgeNode;	// the first edge node in this table entry;

	struct _ETEntry *next;

}  ETEntry;

//----------------------------------------------------------------------------
// function prototypes:
//----------------------------------------------------------------------------

PUBLIC RETCODE SLDepthTerm();
PUBLIC RETCODE SLDepthClear();
PUBLIC RETCODE SLDepthInit();

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
//		Algorithm: 
//		1.  transform all polygons from the world to projected space;
//
//		2.	for each polygon in the polygon table, do the following:
//
//			2.1 construct its edge table (ET);
//
//			2.2	set y to the value of first non-empty entry in ET;
//
//			2.3	initialize the AET to be empty;
//
//			2.4 repeat the following steps until both AET and ET are empty:
//
//				2.4.1 remove from ET those edges whose ymin = current y value;
//
//				2.4.2 insert them into AET in xmin ascending order;
//
//				2.4.3 remove from AET those edges whose ymax = y;
//					  (remember to release the allocated memory)
//
//				2.4.4 get the color information for each pixel between 
//					  pairs of x coordinates from AET;
//
//				2.4.5 call the z-buffer procedure to draw it in the frame buffer.
//
//				2.4.6 increase the current y value by 1;
//
//				2.4.7 update in AET xmin values for the new y for each node;
//
//		    2.5 release ET;
//
// Return: void
// 
//----------------------------------------------------------------------------
void scanLineConversion(hOBJ obj);

#endif