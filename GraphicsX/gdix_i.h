#ifndef _gdix_i_h
#define _gdix_i_h

#include "gdix.h"
//#include "GFX_Bounds.h"

#include "..\Memory\memory.h"
#include "..\Parser\parser.h"
#include "..\Timer\timer.h"

#include "d3d8.h"
#include "D3dx8tex.h"
#include "D3DFont.h"
#include "d3dx8mesh.h"
#include "d3dx8core.h"

//#include <mmsystem.h>	//for the sake of that damn dxutil
//#include "dxutil.h"		//might as well use it...

#define INTERPOLATEANIM

//constants and macros
#define FOV (D3DX_PI/4)

#define MDLTXTID		 90210		  //It's the constant model texture ID
#define AMBIENT_DEFAULT  0xFFFFFFFF   //default ambient light
#define MILLISECOND		 1000		  //millisecond speed (used in timer)
#define FRAMEMAXCHAR	 16			  //used by MD2
#define SINCOUNT		 4096		  //Used for Sin/Cos table

#define SIN(x) g_sinTable[(((x)*SINCOUNT)/360)&(SINCOUNT-1)]
#define COS(x) g_cosTable[(((x)*SINCOUNT)/360)&(SINCOUNT-1)]

#define GFXVERTEXFLAG (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1) //xyz and color and texture

//this is only used by GFX_Texture
#define GFXTVERTEXFLAG (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1) //xyz and color and texture

#define kDEPTHNEAR		 0.5f
#define kDEPTHFAR		 1000.0f

//flags for GFX
#define GFX_FULLSCREEN	 1			  //are we in fullscreen?
#define GFX_BLTMODE		 2			  //are we in blt mode?
#define GFX_LIGHTDISABLE 4			  //is light disabled?

//flags for models
#define MDL_BACKFACECCW	 1			  //does the model use CCW culling?
#define MDL_YDEPTH		 2			  //Y is depth, we need to rotate X-axis by -90

//flags for Objects
#define OBJ_CHILD			1		  //object is a child
#define OBJ_SYNCSTATE		2		  //object's state is in sync with parent
#define OBJ_DEACTIVATE		4		  //object is deactivated
#define OBJ_PAUSE			8		  //object is paused (no frame update)
#define OBJ_PAUSE_ON_END	16		  //object will pause on state end frame

//flags for Terrain Map
#define TMAP_DRAWLINE		1		  //will draw terrain in wire frame

//typedefs
typedef list<unsigned int> LISTPTR;	  //list of pointers

//internal globals
extern LPDIRECT3D8		 g_pD3D;	  //the mac daddy of d3d
extern LPDIRECT3DDEVICE8 g_p3DDevice; //the main interface object
extern ID3DXMatrixStack *g_pWrldStack;//the world matrix stack
//extern gfxDisplayMode	 g_DISPLAY;	  //our current display mode
extern SIZE				 g_SCRNSIZE;  //the screen size or window size
extern FLAGS			 g_FLAGS;	  //status of graphicsX eg: GFX_FULLSCREEN

extern LISTPTR			 g_TXTLIST;				//list of textures
extern LISTPTR			 g_MDLLIST;				//list of models
extern LISTPTR			 g_OBJLIST;				//list of objects
extern LISTPTR			 g_OBJDEACTIVATELIST;	//list of deactivated objects

extern char				 g_MDLLISTFILE[DMAXCHARBUFF]; //the model list file
extern char				 g_TXTLISTFILE[DMAXCHARBUFF]; //the texture list file

extern LPD3DXSPRITE			   g_pTxtSprite; //global sprite interface
extern LPD3DXMESH			   g_pTxtMesh;	 //global texture flat model

extern DWORD g_numLight;		//number of lights enabled
extern DWORD g_lastLight;		//last index of created lights

extern float g_sinTable[SINCOUNT];
extern float g_cosTable[SINCOUNT];

//internal data structures
typedef struct _gfxCamera {
	D3DXMATRIX mtxCamera;	//camera matrix to apply
	D3DXVECTOR3 vEye;		//the eye position
	D3DXVECTOR3 vTarget;	//the eye target
	D3DXVECTOR3 vDir;		//the eye direction (forward vector)
	D3DXVECTOR3 vUp;		//the 'up' vector (always the same)
	angle aYaw,aPitch,aRoll;//angle in degree
} gfxCamera;

typedef struct _gfxVtx {
    float x, y, z; //model space vertex position
	float nX, nY, nZ; //normal for lighting
    DWORD color;   //the color!
	float s, t;	   //texture coord
} gfxVtx;

typedef struct _gfxTexture {
	unsigned int ID;				//Identification
	char *filename;					//sigh...
	LPDIRECT3DTEXTURE8 texture;		//It's the d3d texture!
	int width;						//width of texture
	int height;						//height of texture

	int ref;						 //number of things refering to this
} gfxTexture;

typedef struct _gfxModelFrame {
	LPD3DXMESH	 frameMesh;			 //key frame mesh
	LPD3DXBUFFER adjacencyBuffer;	 //used for optimizing
	char		 name[FRAMEMAXCHAR]; //name of frame (usually used by MD2)
	float		 *planes[eMaxPlaneEq];	//plane equations per triangle
} gfxModelFrame;

typedef struct _gfxModel {
	unsigned int ID;				 //Identification
	//unsigned int vertexCount;		 //number of vertices
	//unsigned int faceCount;			 //number of triangles (faces)
	unsigned int indCount;			 //number of indices (faceCount*NUMPTFACE)
	unsigned int numMaterial;		 //number of materials for this model

	unsigned int baseInd;			 //zero

	LPD3DXMESH	 mesh;				 //The mesh of this model, vtx and stuff (can be used for current frame)
	
	gfxModelFrame *frames;			 //if model is animated, contains key frame meshes (numFrame)
	unsigned int  numFrames;		 //number of animation frame meshes

	hTXT		 *textures;			 //array of textures (numMaterial)
	D3DMATERIAL8 *materials;		 //array of materials (numMaterial)
	LPD3DXBUFFER adjacencyBuffer;	 //used for optimizing

	gfxBound	 bound;				 //boundary of model

	int ref;						 //number of things refering to this

	FLAGS status;					 //status of model
} gfxModel;

typedef struct _gfxWrldMtx {
	D3DXMATRIX   mtxTrans;			 //Translation matrix
	D3DXMATRIX   mtxRot;			 //Rotation matrix
	D3DXMATRIX	 mtxScale;			 //Scale matrix
	D3DXMATRIX   mtxWrld;			 //World matrix (scale * rot * trans)
} gfxWrldMtx;

typedef struct _gfxObjMdlState {
	int startFrame;	//starting frame
	int curFrame;	//current frame
	int endFrame;	//ending frame
	float delay;	//0~1 speed of anim.
	//int maxFrame;	//number of frames
} gfxObjMdlState;

typedef struct _gfxObject {
	unsigned int ID;				 //Identification
	gfxID		 key;

	hMDL	     theMdl;			 //Model this object use
	LPD3DXMESH	 curMesh;			 //current mesh to be displayed (it's just a pointer)
	
	float		 curPercentFrame;	 //amount of frame finished (0~1)
	float		 percentFrameAmt;	 //amount of frame increase

	timer		  ticker;			 //delay n' stuff 'bout time

	gfxObjMdlState *states;			 //states (numState)
	unsigned int numStates;			 //number of states
	unsigned int curState;			 //current state
	unsigned int prevFrame;			 //previous frame, used by interpolation...

	float        wrldTrans[eMaxPt];	 //world translation
	float		 wrldRot[eMaxPt];	 //world rotation (RADIANS: along x,y,z)
	float		 wrldScl[eMaxPt];	 //world scale    (DEFAULT: x=1,y=1,z=1)

	gfxWrldMtx   wrldMtx;			 //The object world matrix (world loc/rot)

	int ref;						 //number of things refering to this

	LISTPTR		 *objNode;			 //list of children attached to this object
	LISTPTR		 *listRef;			 //pointer to list this object is in

	D3DXVECTOR3  orient;			 //orientation of object

	FLAGS status;					 //object flags
} gfxObject;

typedef struct _gfxFont {
	CD3DFont*     pD3DFont;				//The D3D Font
	TCHAR         strFont[MAXCHARBUFF];	//The font type name
	unsigned int  fontSize;				//Size of font
	unsigned int  fntFormat;			//format of font: BOLD, ITALIC
} gfxFont;

typedef struct _gfxSprite {
	hTXT		  theTexture;			//handle to texture this sprite use
	timer		  ticker;				//delay n' stuff 'bout time
	int numFrames;						//Number of state frames
	int curState;						//Index of current state
	int numStates;						//Number of sprite states
	SIZE frameSize;						//The size of each frame
	int numImgCol;						//Mainly used for duplication
	LOCATION imgOffset;					//Mainly used for duplication
	RECT *frameRects;					//Array of rects for each frames
	gfxSprFrame *states;				//Array of sprite states
} gfxSprite;

typedef struct _gfxCel {
	IDirect3DVertexBuffer8 *vtx;		//Vertex buffer
	int numVtx;							//usually 4
	hTXT subTxt;						//overlay texture
	float plane[2][eMaxPlaneEq];		//two plane equations
} gfxCel;

//Terrain Map
typedef struct _gfxTMap {
	SIZE indSize;						//Size of array (col*row)
	float celSize[eMaxPt];				//Size of each cel
	float wrldTrans[eMaxPt];			//world position
	gfxWrldMtx   wrldMtx;				//The world matrix (ignore rotation!)
	gfxCel *cels;						//array of cel (col*row)
	hTXT texture;						//Texture this map uses
	D3DMATERIAL8 material;				//The material
	FLAGS status;						//map flags
} gfxTMap;

typedef RETCODE (* BKEFFECT) (struct _gfxBkFX *thisBKFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);

typedef struct _gfxBkFX {
	int Type;		  //Specifies effect type
	void *Data;		  //Context to which effect is applied
	BKEFFECT Effect;  //Callback function for effect
	int Duration;	  //Current duration counter of effect
	int MaxDuration;  //Maximum duration of effect
	timer ticker;	  //the timer for animation
	FLAGS Status;	  //Indicates status of effect
} gfxBkFX;

typedef RETCODE (* PAREFFECT) (struct _gfxParticleFX *thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam);

typedef struct _gfxParticleFX {
	int type;			//specifies particle type
	void *data;			//contect to which effect is applied
	PAREFFECT Effect;   //Callback function for effect
	int duration;		//current duration counter of effect
	int maxDuration;	//max duration of effect
	timer ticker;		//timer for mov't/animation
	FLAGS status;		//indicates status of effect

	gfxID objKey;		//reference to object attached
	bool (* collision) (const gfxBound *bound, float outPt[eMaxPt]); //collision callback

} gfxParticleFX;

//internal functions

/////////////////////////////////////
// Name:	_GFXGetD3dFormat
// Purpose:	grabs D3DFORMAT of pMode
// Output:	none
// Return:	the D3DFORMAT
/////////////////////////////////////
PROTECTED D3DFORMAT _GFXGetD3dFormat(gfxDisplayMode *pMode);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED bool _GFXCheckError(HRESULT hr, bool displayMsg, const char *header);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _TextureDestroyAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _MDLDestroyAll();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _OBJDestroyAll();

//my own model file!
PROTECTED RETCODE _ModelLoadDataFromFile(hMDL model, const char *filename);
//3ds ascii file textured
PROTECTED RETCODE _ModelLoadDataFromFileASE(hMDL model, const char *filename);
//Direct X-files!  Those bastards made it too easy...
PROTECTED RETCODE _ModelLoadDataFromFileX(hMDL model, const char *filename);
//md2 loader
PROTECTED RETCODE _ModelLoadDataFromFileMD2(hMDL model, const char *filename);

//key frame loading
PROTECTED RETCODE _ObjLoadAnimationKey(hOBJ obj, const char *animationfile);

PROTECTED void _MDLOptimize(LPD3DXMESH mesh, const LPD3DXBUFFER pAdjacencyBuffer, int numInd, 
						  LPD3DXMESH *optMesh);

//texture vtx stuff
PROTECTED void _TextureInitVtxBuff();
PROTECTED void _TextureTermVtxBuff();

extern PUBLIC void PARFXDestroyAll();

//math stuff
//return a random double-precision number between 0 and 1
PROTECTED double _GFXMathDRand();
PROTECTED float _GFXMathRand(float a, float b);
PROTECTED void _GFXMathMtxLoadIden(gfxWrldMtx *pWMtx);
PROTECTED bool _GFXMathIsPowerOf2(int size);

//pager stuff
PROTECTED void GFXPageClear();
PROTECTED void GFXPageAdd(void *item, gfxID *pID);
PROTECTED void GFXPageRemove(gfxID *pID);
PROTECTED void * GFXPageGet(DWORD ID, LONGLONG *pCounter);
PROTECTED void * GFXPageQuery(const gfxID *key);

#endif