#ifndef _gdix_h
#define _gdix_h

#include "..\common.h"

#include "d3d8types.h"
#include "d3dx8math.h"

//NOTES:
// 1. We are using right-handed coord
// 2. Z-axis is the depth
// 3. vertices are drawn in CW order


//public constants
#define NUMPTFACE		3	//well duh?
#define NUMPTQUAD		4
#define NUMMTXELEM		16

#define DAVIDPI			3.1415926535897932384626433832795f

#define INFINITE_DURATION -1

//public flags
#define FNT_BOLD        1
#define FNT_ITALIC      2

//public enum
typedef enum {
	BPP_32,	 //32-bit
	BPP_24,	 //24-bit
	BPP_16,  //16-bit r5 g6 b5
	BPP_15a, //16-bit r5 g5 b5 a1
	BPP_15,  //15-bit r5 g5 b5
	BPP_8,	 //8-bit color index
	BPP_DOH, //d'oh!
} eBPP;

typedef enum {
	eR,
	eG,
	eB,
	eA,
	eMaxClr
} eColor;

typedef enum {
	eX,
	eY,
	eZ,
	eMaxPt
} ePt;

typedef enum {
	e_11=0, e_21=4, e_31=8,  e_41=12,
    e_12=1, e_22=5, e_32=9,  e_42=13,
    e_13=2, e_23=6, e_33=10, e_43=14,
    e_14=3, e_24=7, e_34=11, e_44=15,
	eMaxMtxElem=16
} eMtx;

typedef enum {
	ePA,
	ePB,
	ePC,
	ePD,
	eMaxPlaneEq
} ePlane;

typedef enum {
	eOBJ_STILL,
	eOBJ_ANIMATE
} eOBJTYPE;

//public typedefs
typedef int	   angle;

//public handles
typedef struct _gfxCamera  *hCAMERA;
typedef struct _gfxTexture *hTXT;
typedef struct _gfxModel   *hMDL;
typedef struct _gfxObject  *hOBJ;
typedef struct _gfxFont	   *hFNT;
typedef struct _gfxSprite  *hSPRITE;
typedef struct _gfxTMap	   *hTMAP;

//public structs
typedef struct _gfxDisplayMode {
	unsigned int width;		//scrn width
	unsigned int height;	//scrn height
	eBPP		 bpp;		//bits per pixel
	unsigned int refresh;	//refresh rate (usu. 0)
} gfxDisplayMode;

typedef struct _gfxSprFrame {
	int firstframe; //Index of first frame
	int lastframe;	//Index of last frame
	int curFrame;	//Index of current frame
} gfxSprFrame;

//bounding stuff for model
typedef struct _gfxBound {
	float center[eMaxPt];			 //center of bounding sphere
	float		radius;				 //the radius
	D3DXVECTOR3 min,max;			 //bounding box min/max
} gfxBound;

//ID for objects
typedef struct _gfxID {
	DWORD ID;
	LONGLONG counter;
} gfxID;

//
//GFX main functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXResize(unsigned int width, unsigned int height);

/////////////////////////////////////
// Name:	GFXSetTextureFilter
// Purpose:	sets texture filter for min/mag
//          base is usually 0.
// Output:	texture filter on base is updated
// Return:	RETCODE_SUCCESS if successful
/////////////////////////////////////
PUBLIC RETCODE GFXSetTextureFilter(unsigned int base, D3DTEXTUREFILTERTYPE minFilter, D3DTEXTUREFILTERTYPE magFilter);

/////////////////////////////////////
// Name:	GFXEnableLight
// Purpose:	enable/disable light
//			also set ambient color
// Output:	light is enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnableLight(bool do_you_want_light, unsigned int ambient);

/////////////////////////////////////
// Name:	GFXIsLightEnabled
// Purpose:	is light enabled?
// Output:	none
// Return:	true if so...
/////////////////////////////////////
PUBLIC bool GFXIsLightEnabled();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXInit(HWND hwnd, gfxDisplayMode *pMode);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXDestroy();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXClear(int numRect, const D3DRECT* pRects, DWORD color);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXDisplayScene();

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXUpdate(const RECT* pSrcRect, const RECT* pDestRect, HWND hwndOverride);

/////////////////////////////////////
// Name:	GFXBegin
// Purpose:	readies device to render
//			stuff.
// Output:	shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBegin();

/////////////////////////////////////
// Name:	GFXEnd
// Purpose:	call this after finish
//			with rendering stuff
// Output:	more shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnd();

/////////////////////////////////////
// Name:	GFXBltModeEnable
// Purpose:	use this for blitting
//			textures on 2d raster
//			coord.
// Output:	projection is changed
//			disables depth buffer
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeEnable(bool bPriority);

/////////////////////////////////////
// Name:	GFXIsBltMode
// Purpose:	checks if bltmode is enabled
// Output:	none
// Return:	if true
/////////////////////////////////////
PUBLIC bool GFXIsBltMode();

/////////////////////////////////////
// Name:	GFXBltModeDisable
// Purpose:	disable blt mode
// Output:	changes projection
//			back to perspective
//			enables depth buffer again
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeDisable();

/////////////////////////////////////
// Name:	GFXGetScrnSize
// Purpose:	grabs size of screen
// Output:	none
// Return:	the scrn size
/////////////////////////////////////
PUBLIC SIZE GFXGetScrnSize();

//
//camera functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hCAMERA CameraCreate(const float vEye[eMaxPt], angle yaw, angle pitch, angle roll);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraSetScene(hCAMERA cam);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraMov(hCAMERA cam, const float amt[eMaxPt]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraSet(hCAMERA cam, const float vEye[eMaxPt]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraMovDir(hCAMERA cam, float xAmt, float yAmt, float zAmt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraYawPitchRoll(hCAMERA cam, angle yaw, angle pitch, angle roll);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void CameraDestroy(hCAMERA *cam);

/////////////////////////////////////
// Name:	CameraGetLoc
// Purpose:	grabs camera's eye loc
// Output:	the eye location
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetLoc(hCAMERA cam, float loc[eMaxPt]);

/////////////////////////////////////
// Name:	CameraGetDir
// Purpose:	grabs camera's direction
//			vector
// Output:	the direction vector
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetDir(hCAMERA cam, float dir[eMaxPt]);

/////////////////////////////////////
// Name:	CameraGetYawPitchRoll
// Purpose:	grabs camera's yaw/pitch/roll
// Output:	none
// Return:	none
/////////////////////////////////////
PUBLIC void CameraGetYawPitchRoll(hCAMERA cam, angle *yaw, angle *pitch, angle *roll);

//
//texture functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreate(unsigned int newID, const char *filename, bool clrkey, D3DCOLOR srcClrKey);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureAddRef(hTXT txt);

/////////////////////////////////////
// Name:	TextureSetListFile
// Purpose:	sets the texture list file
// Output:	the texture list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSetListFile(const char *filename);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureSearch(unsigned int ID);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureDestroy(hTXT *txt);

/////////////////////////////////////
// Name:	TextureSet
// Purpose:	sets texture for texture
//			mapping.  Base is usually
//			0.
// Output:	texture is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSet(hTXT txt, unsigned int base);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt(hTXT txt, float x, float y, const RECT *srcRect,
								 const unsigned int *color, float rotate);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureStretchBlt(hTXT txt, float x, float y, 
								 float w, float h, const RECT *srcRect,
								 const unsigned int *color, float rotate);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt3D(hTXT txt, const D3DXMATRIX *wrldMtx,
							const RECT* pSrcRect, const D3DMATERIAL8 *material,
							const unsigned int *color);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetWidth(hTXT txt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetHeight(hTXT txt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int TextureGetID(hTXT txt);

//
//model functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLCreate(unsigned int newID, const char *filename);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLGenMap(unsigned int newID, float mapSizeX, float mapSizeZ, float height, float r,
				 unsigned int numIter, hTXT texture, D3DMATERIAL8 *mat);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLAddRef(hMDL model);

/////////////////////////////////////
// Name:	MDLSetListFile
// Purpose:	sets the model list file
// Output:	the model list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLSetListFile(const char *filename);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLSearch(unsigned int ID);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLDestroy(hMDL *model);

/////////////////////////////////////
// Name:	MDLGetTexture
// Purpose:	get model's
//			texture within material
//			index
// Output:	none
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT MDLGetTexture(hMDL model, int materialInd);

/////////////////////////////////////
// Name:	MDLSetTexture
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetTexture(hMDL model, int materialInd, hTXT texture);

/////////////////////////////////////
// Name:	MDLSetMaterial
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetMaterial(hMDL model, int materialInd, const D3DMATERIAL8 &material);

/////////////////////////////////////
// Name:	MDLGetBound
// Purpose:	grabs model's bound info
// Output:	none
// Return:	the model's bound
/////////////////////////////////////
PUBLIC gfxBound MDLGetBound(hMDL model);

/////////////////////////////////////
// Name:	MDLGetNumVtx
// Purpose:	grabs the number of vtx
//			the model has
// Output:	none
// Return:	number of vtx
/////////////////////////////////////
PUBLIC DWORD MDLGetNumVtx(hMDL model);

/////////////////////////////////////
// Name:	MDLGetVtx
// Purpose:	grabs the vtx of model with
//			given number.
// Output:	none
// Return:	number of vtx copied
/////////////////////////////////////
PUBLIC DWORD MDLGetVtx(hMDL model, float *vtx[eMaxPt], int numVtx);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int MDLGetID(hMDL model);

/////////////////////////////////////
// Name:	MDLSaveToXFile
// Purpose:	this is used for cheating in
//			GEN300, good for terrain!
// Output:	file is created
// Return:	none
/////////////////////////////////////
PUBLIC void MDLSaveToXFile(hMDL model, char *filename);

//
//object functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJCreate(unsigned int newID, hMDL model, float x, float y, float z, 
					  eOBJTYPE objType, const char *animationfile);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJAddRef(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJSearch(unsigned int ID);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJDestroy(hOBJ *obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJSetLoc(hOBJ obj, float x, float y, float z);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJMovLoc(hOBJ obj, float x, float y, float z);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJRotX(hOBJ obj, float radian);
PUBLIC void OBJRotY(hOBJ obj, float radian);
PUBLIC void OBJRotZ(hOBJ obj, float radian);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJSetScale(hOBJ obj, float sX, float sY, float sZ);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJAddScale(hOBJ obj, float sX, float sY, float sZ);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJDisplay(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJUpdateFrame(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int OBJGetCurState(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJSetState(hOBJ obj, int state);

/////////////////////////////////////
// Name:	OBJIsStateEnd
// Purpose:	checks to see if the state
//			of an object reached the end
//			frame of current state
// Output:	none
// Return:	true if curframe reached end
/////////////////////////////////////
PUBLIC bool OBJIsStateEnd(hOBJ obj);

/////////////////////////////////////
// Name:	OBJPause
// Purpose:	pause / resume  object
//			(true)  (false)
// Output:	Obj flag is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJPause(hOBJ obj, bool bPause);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJPauseOnStateEnd(hOBJ obj, bool bPause);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJAddChild(hOBJ obj, hOBJ child, bool syncState);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJRemoveChild(hOBJ obj, hOBJ child);

/////////////////////////////////////
// Name:	OBJGetModel
// Purpose:	grabs model of obj.
// Output:	none
// Return:	the model of obj.
/////////////////////////////////////
PUBLIC hMDL OBJGetModel(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJGetWorldMtx(hOBJ obj, float mtx[eMaxMtxElem]);

/////////////////////////////////////
// Name:	OBJGetLoc
// Purpose:	grabs the location of 
//			the obj.  This can either
//			be local/world.
// Output:	loc is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetLoc(hOBJ obj, float loc[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetTransMtx
// Purpose:	grabs the obj's translation
//			mtx.
// Output:	none
// Return:	the mtx trans
/////////////////////////////////////
PUBLIC void OBJGetTransMtx(hOBJ obj, float mtx[eMaxMtxElem]);

/////////////////////////////////////
// Name:	OBJGetScale
// Purpose:	grabs the scale value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetScale(hOBJ obj, float scl[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetScaleMtx
// Purpose:	grabs the obj's scale
//			mtx.
// Output:	none
// Return:	the mtx scale
/////////////////////////////////////
PUBLIC void OBJGetScaleMtx(hOBJ obj, float mtx[eMaxMtxElem]);

/////////////////////////////////////
// Name:	OBJGetRotate
// Purpose:	grabs the rotate value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetRotate(hOBJ obj, float rot[eMaxPt]);

/////////////////////////////////////
// Name:	OBJGetRotateMtx
// Purpose:	grabs the obj's rotate
//			mtx.
// Output:	none
// Return:	the mtx scale
/////////////////////////////////////
PUBLIC void OBJGetRotateMtx(hOBJ obj, float mtx[eMaxMtxElem]);

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumFace(hOBJ obj);

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumVtx(hOBJ obj);

/////////////////////////////////////
// Name:	OBJGetNumModel	
// Purpose:	This will give you the num
//			of models associated with
//			the obj.  This will go through
//			the children as well.
// Output:	none
// Return:	num model
/////////////////////////////////////
PUBLIC int OBJGetNumModel(hOBJ obj);

/////////////////////////////////////
// Name:	OBJGetBounds
// Purpose:	fills up the bounds array
//			with bound info of obj and
//			all it's children.
//			NOTE:  Assumes that bounds
//			is allocated to the number of
//			models associated w/ object
// Output:	bounds is filled
// Return:	count of bounds filled
/////////////////////////////////////
PUBLIC int OBJGetBounds(hOBJ obj, gfxBound *bounds, bool bGet1stOnly=false);

/////////////////////////////////////
// Name:	OBJIsIntersectRay
// Purpose:	checks to see if the given
//			object is intersecting a ray
// Output:	pFaceIndex, pU, pV, pDist
// Return:	true if intersect
/////////////////////////////////////
PUBLIC bool OBJIsIntersectRay(hOBJ obj, D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir);

/////////////////////////////////////
// Name:	OBJFixOrientation
// Purpose:	fixes obj's orientation
// Output:	object rotated to -90
// Return:	none
/////////////////////////////////////
PUBLIC void OBJFixOrientation(hOBJ obj);

/////////////////////////////////////
// Name:	OBJDeactivate
// Purpose:	deactivates obj
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void OBJDeactivate(hOBJ obj);

/////////////////////////////////////
// Name:	OBJIsDeactivate
// Purpose:	check to see if given
//			obj is deactivate
// Output:	none
// Return:	true if deactivate
/////////////////////////////////////
PUBLIC bool OBJIsDeactivate(hOBJ obj);

/////////////////////////////////////
// Name:	OBJDeactivate
// Purpose:	activates obj
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void OBJActivate(hOBJ obj);

/////////////////////////////////////
// Name:	OBJDuplicate
// Purpose:	duplicates a given object
// Output:	stuff happens
// Return:	new clone object
/////////////////////////////////////
PUBLIC hOBJ OBJDuplicate(hOBJ obj, unsigned int newID, float x, float y, float z);

/////////////////////////////////////
// Name:	OBJSetOrientation
// Purpose:	set the orient of obj
// Output:	orient is done
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetOrientation(hOBJ obj, float x, float y, float z);

/////////////////////////////////////
// Name:	OBJGetOrientation
// Purpose:	get the orient of obj
// Output:	the vec is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetOrientation(hOBJ obj, float vec[eMaxPt]);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int OBJGetID(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC gfxID OBJGetKey(hOBJ obj);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJQuery(const gfxID *key);

//
//font functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hFNT FontCreate(const char *typeName, unsigned int size, unsigned int fmtFlag);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf2D(hFNT fnt, float x, float y, unsigned int color, const char *text, ...);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf3D(hFNT fnt, D3DXMATRIX *wrldMtx, D3DMATERIAL8 *mtrl, 
							DWORD flags, const char *text, ...);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontDestroy(hFNT *fnt);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetStrSize(hFNT fnt, char *str, SIZE *pSize);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetCharSize(hFNT fnt, char c, SIZE *pSize);

//
//sprite functions
//

/////////////////////////////////////
// Name:	SpriteCreate
// Purpose:	creates a sprite with txt.
//			NOTE: states has to be allocated with
//				  size numState.
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
PUBLIC hSPRITE SpriteCreate(hTXT txt, int numFrame, SIZE frameSize, 
							unsigned int delay, int numImgCol, LOCATION *imgOffset,
							int numState, const gfxSprFrame *states);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hSPRITE SpriteLoad(hTXT txt, int numImageCol, LOCATION *imgOffset,
						  const char *filename);

/////////////////////////////////////
// Name:	SpriteDestroy
// Purpose:	destroys sprite
// Output:	none
// Return:	none
/////////////////////////////////////
PUBLIC void SpriteDestroy(hSPRITE *spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void SpriteUpdateFrame(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void SpriteUpdateFrameAllState(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int SpriteGetMaxState(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int SpriteGetCurState(hSPRITE spr);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE SpriteSetState(hSPRITE spr, int state);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC SIZE SpriteGetSize(hSPRITE spr);

/////////////////////////////////////
// Name:	SpriteBlt
// Purpose:	displays sprite
//			NOTE: GFXBltModeEnable()
//				  must be called first
// Output:	sprite is displayed
// Return:	RETCODE_SUCCESS
/////////////////////////////////////
PUBLIC RETCODE SpriteBlt(hSPRITE spr, const float loc[eMaxPt], const unsigned int *color, float rotate);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE SpriteStretchBlt(hSPRITE spr, const float loc[eMaxPt],
								float w, float h, const unsigned int *color, float rotate);

/////////////////////////////////////
// Name:	SpriteBlt3D
// Purpose:	displays sprite
//			NOTE: GFXBegin()
//				  must be called first
//			This will also set cull mode
//			to none
// Output:	sprite is displayed in 3d
// Return:	RETCODE_SUCCESS
/////////////////////////////////////
PUBLIC RETCODE SpriteBlt3D(hSPRITE spr, const D3DXMATRIX *wrldMtx, 
						   const D3DMATERIAL8 *material,
						   const unsigned int *color);

/////////////////////////////////////
// Name:	SpriteDuplicate
// Purpose:	duplicate a sprite with
//			given sprite
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
PUBLIC hSPRITE SpriteDuplicate(hSPRITE spr);

//
//map functions
//

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTMAP TMapCreateRand(float celSizeX, float celSizeZ, float height,
							int xDir, int zDir, float r, unsigned int numIter,
							hTXT texture, D3DMATERIAL8 *mat);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TMapDisplay(hTMAP map, int cBeginCol, int cBeginRow,
						   int cEndCol, int cEndRow);

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TMapDestroy(hTMAP *map);

/////////////////////////////////////
// Name:	TMapGetSize
// Purpose:	grabs the cel size of the
//			map.  eX/eZ is what you want
// Output:	size is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetSize(hTMAP map, float size[eMaxPt]);

/////////////////////////////////////
// Name:	TMapGetCelVtx
// Purpose:	gets the quad vtx of the
//			specified cel of map
// Output:	vtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetCelVtx(hTMAP map, int ind, float vtx[NUMPTQUAD][eMaxPt]);

/////////////////////////////////////
// Name:	TMapClearCelTxt
// Purpose:	clear cel texture
// Output:	cel texture removed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapClearCelTxt(hTMAP map, int ind);

/////////////////////////////////////
// Name:	TMapSetCelTxt
// Purpose:	set cel's texture
// Output:	cel's texture is set
// Return:	none
/////////////////////////////////////
PUBLIC void TMapSetCelTxt(hTMAP map, int ind, hTXT txt);

/////////////////////////////////////
// Name:	TMapSetRenderType
// Purpose:	sets map to render in
//			lines for now...
// Output:	map status changed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapSetRenderType(hTMAP map, unsigned int passInZeroForNow);

/////////////////////////////////////
// Name:	TMapResetRenderType
// Purpose:	sets map to normal render
// Output:	map status changed
// Return:	none
/////////////////////////////////////
PUBLIC void TMapResetRenderType(hTMAP map);

/////////////////////////////////////
// Name:	TMapGetInd
// Purpose:	grabs the index from given
//			loc.
// Output:	none
// Return:	index within map (ind<0 for outofbound)
/////////////////////////////////////
PUBLIC int TMapGetInd(hTMAP map, float Point [eMaxPt], float dirX, float dirZ);

/////////////////////////////////////
// Name:	TMapGetPlaneEq
// Purpose:	grabs the two planes of
//			a cel
// Output:	pl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void TMapGetPlaneEq(hTMAP map, int ind, float pl[2][eMaxPlaneEq]);

/////////////////////////////////////
// Name:	TMapGetIndSize
// Purpose:	grabs the index size of
//			map (row,col)
// Output:	none
// Return:	index size
/////////////////////////////////////
PUBLIC SIZE TMapGetIndSize(hTMAP map);

/////////////////////////////////////
// Name:	TMapGetTexture
// Purpose:	grabs the terrain texture
// Output:	none
// Return:	terrain texture
/////////////////////////////////////
PUBLIC hTXT TMapGetTexture(hTMAP map);

/////////////////////////////////////
// Name:	TMapSetTxtSprRand
// Purpose:	makes your map look cool
// Output:	map texture is changed
// Return:	success
/////////////////////////////////////
PUBLIC RETCODE TMapSetTxtSprRand(hTMAP map, hSPRITE spr);

//
// Light functions
//

/////////////////////////////////////
// Name:	LightIsEnable
// Purpose:	checks to see if given
//			light is enabled
// Output:	none
// Return:	TRUE if enabled
/////////////////////////////////////
PUBLIC BOOL LightIsEnable(DWORD ind);

/////////////////////////////////////
// Name:	LightEnable
// Purpose:	enables/disables light
// Output:	enable/disable light
// Return:	none
/////////////////////////////////////
PUBLIC void LightEnable(DWORD ind, BOOL enable);

/////////////////////////////////////
// Name:	LightSet
// Purpose:	set light
// Output:	light is set
// Return:	none
/////////////////////////////////////
PUBLIC void LightSet(DWORD ind, D3DLIGHT8 *lightStruct);

/////////////////////////////////////
// Name:	LightAdd
// Purpose:	adds new light to GFX
// Output:	increment number of light
// Return:	index of light (use this as
//			an ID to light)
/////////////////////////////////////
PUBLIC DWORD LightAdd(D3DLIGHT8 *lightStruct);

/////////////////////////////////////
// Name:	LightRemove
// Purpose:	removes light
// Output:	decrease number of light
//			disables given light ind
// Return:	none
/////////////////////////////////////
PUBLIC void LightRemove(DWORD ind);

/////////////////////////////////////
// Name:	LightGetData
// Purpose:	grabs given light's data
// Output:	lightStruct is filled
// Return:	none
/////////////////////////////////////
PUBLIC void LightGetData(DWORD ind, D3DLIGHT8 *lightStruct);

/////////////////////////////////////
// Name:	LightGetLastInd
// Purpose:	grabs the last index light
// Output:	none
// Return:	the last light index
/////////////////////////////////////
PUBLIC DWORD LightGetLastInd();

/////////////////////////////////////
// Name:	LightReset
// Purpose:	disables all light and
//			resets number of lights
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void LightReset();

//
// Math stuff
//

/////////////////////////////////////
// Name:	GFXMathCreateRayFromLoc
// Purpose:	create a ray from given 
//			screen loc
// Output:	ray pos&dir is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXMathCreateRayFromLoc(const LOCATION *loc,
										D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir);


PUBLIC void GFXHideCursor (void);
PUBLIC void GFXShowCursor (void);

/////////////////////////////////////
// Name:	GFXDrawBoxXZ
// Purpose:	draws a box from xz plane
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawBoxXZ(float xMin, float zMin, float xMax, float zMax, 
						 float h, unsigned int color);

/////////////////////////////////////
// Name:	GFXDrawPixel
// Purpose:	draws a 2d pixel
// Output:	yay
// Return:	none
/////////////////////////////////////
PUBLIC void GFXDrawPixel(float x, float y, unsigned int clr);

/////////////////////////////////////
// Name:	GFXGetProjMtx
// Purpose:	gets the proj mtx
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXGetProjMtx(float mtx[eMaxMtxElem]);

/////////////////////////////////////
// Name:	GFXGetViewMtx
// Purpose:	gets the view mtx
// Output:	pMtx is filled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXGetViewMtx(float mtx[eMaxMtxElem]);

PUBLIC void GFXCreateVPMtx(float mtx[eMaxMtxElem]);

#endif