#include "gdix.h"
#include "gdix_i.h"

//this is only used by GFX_Texture
typedef struct _gfxTVtx {
    float x, y, z; //model space vertex position
	float nX, nY, nZ; //normal for lighting
    DWORD color;   //the color!
	float s, t;	   //texture coord
} gfxTVtx;

#define MAXTXTVTX 4

#define TXTVERTEXFLAG (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1) //xyzrhw and color and texture

typedef struct _txtVertex {
    float x, y, z, rhw; //translated vertex position
    DWORD color;   //the color!
	float s, t;	   //texture coord
} txtVertex;

LPD3DXSPRITE		    g_pTxtSprite=0; //global sprite interface
LPD3DXMESH				g_pTxtMesh=0;	//global texture flat model

//all texture creation/manipulation/destruction happens here...

PRIVATE void _destroyTxt(hTXT *txt) //this will destroy the object, will not search list
{
	if(*txt)
	{
		if((*txt)->texture) //destroy frame
			(*txt)->texture->Release();

		if((*txt)->filename)
			MemFree((void**)&(*txt)->filename);

		MemFree((void**)txt);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hTXT TextureCreate(unsigned int newID, const char *filename, bool clrkey, D3DCOLOR srcClrKey)
{
	hTXT newTxt;

	if(MemAlloc((void**)&newTxt, sizeof(gfxTexture), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new texture", "Error in TextureCreate"); return 0; }

	MemSetPattern(newTxt, "GFXTXT");

	newTxt->ID = newID;

	HRESULT hr;

	if(!clrkey)
		hr=D3DXCreateTextureFromFile(g_p3DDevice, filename, &newTxt->texture);
	else
	{
		hr=D3DXCreateTextureFromFileEx(g_p3DDevice, filename,
			D3DX_DEFAULT, D3DX_DEFAULT, //width/height
			D3DX_DEFAULT,				//mip level
			0,							//WTF
			D3DFMT_UNKNOWN,				//pixel format (use file)
			D3DPOOL_MANAGED,			//WTF
			D3DX_DEFAULT,				//filter         (we should let user do this...maybe...)
			D3DX_DEFAULT,				//mip-map filter (we should let user do this...maybe...)
			srcClrKey,					//the color keying
			0,							//src info, whatever the hell that is
			0,							//palette info
			&newTxt->texture);
	}

	_GFXCheckError(hr, true, "Error in TextureCreate");

	D3DSURFACE_DESC d3dsd;
	newTxt->texture->GetLevelDesc(0, &d3dsd);

	newTxt->width = d3dsd.Width;
	newTxt->height = d3dsd.Height;

	//sigh...
	MemAlloc((void**)&newTxt->filename, sizeof(char)*(strlen(filename)+1), M_ZERO);
	strcpy(newTxt->filename, filename);

	//append object to list
	g_TXTLIST.insert(g_TXTLIST.end(), (unsigned int)newTxt);

	return newTxt;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureAddRef(hTXT txt)
{
	if(txt)
		txt->ref++;
}

/////////////////////////////////////
// Name:	TextureSetListFile
// Purpose:	sets the texture list file
// Output:	the texture list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSetListFile(const char *filename)
{
	//just copy it
	strcpy(g_TXTLISTFILE, filename);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PRIVATE hTXT _TextureSearchList(unsigned int ID)
{
	hTXT thisTxt;

	for(LISTPTR::iterator i = g_TXTLIST.begin(); i != g_TXTLIST.end(); ++i)
	{
		thisTxt = (hTXT)(*i);

		if(thisTxt->ID == ID)
			return thisTxt;
	}

	return 0;
}

/////////////////////////////////////
// Name:	TextureSearch
// Purpose:	load image/texture via 
//			list file with ID
//			NOTE: This will first find
//			the ID from the texture
//			list.
// Output:	add texture to list if not
//			yet done
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT TextureSearch(unsigned int ID)
{
	//first, check to see if image already exist
	hTXT texture=_TextureSearchList(ID);

	if(!texture) //search through the file
	{
		char txtFName[MAXCHARBUFF];
		unsigned int txtID, clrKey, r=0, g=0, b=0;

		//load up file
		FILE *fp = fopen(g_TXTLISTFILE, "rt");

		if(!fp)
			return 0;

		while(!feof(fp))
		{
			fscanf(fp, "%d %s %d", &txtID, txtFName, &clrKey);

			if(clrKey == 1)
				fscanf(fp, "%d,%d,%d", &r, &g, &b);
			else
				fscanf(fp, "\n");

			if(txtID == ID) //does it match???
			{
				//load this puppy up!
				texture = TextureCreate(ID, txtFName, clrKey == 1 ? true : false, D3DCOLOR_XRGB(r,g,b));
				break;
			}
		}

		fclose(fp);
	}
	else
		TextureAddRef(texture);

	return texture;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _TextureDestroyAll()
{
	hTXT thisTxt;

	for(LISTPTR::iterator i = g_TXTLIST.begin(); i != g_TXTLIST.end(); ++i)
	{
		thisTxt = (hTXT)(*i);
		_destroyTxt(&thisTxt);
	}

	g_TXTLIST.clear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void TextureDestroy(hTXT *txt)
{
	if(*txt)
	{
		if((*txt)->ref <= 0)
		{
			//just remove the pointer of model from list
			g_TXTLIST.remove((unsigned int)(*txt));

			_destroyTxt(txt);
		}
		else
			(*txt)->ref--;
	}

	txt=0;
}

/////////////////////////////////////
// Name:	TextureSet
// Purpose:	sets texture for texture
//			mapping.  Base is usually
//			0.
// Output:	texture is set
// Return:	none
/////////////////////////////////////
PUBLIC void TextureSet(hTXT txt, unsigned int base)
{
	if(txt)
		g_p3DDevice->SetTexture(base, txt->texture);
	else
		g_p3DDevice->SetTexture(base, 0);
}

PROTECTED void _TextureInitVtxBuff()
{
	D3DXCreateSprite(
		g_p3DDevice,
		&g_pTxtSprite);

	_GFXCheckError(D3DXCreateMeshFVF(
		2,
		NUMPTQUAD,
		D3DXMESH_WRITEONLY,
		GFXTVERTEXFLAG,
		g_p3DDevice,
		&g_pTxtMesh),
	true, "Error in _MD2CreateFrame");

	gfxTVtx *pTVtx;

	if(SUCCEEDED(g_pTxtMesh->LockVertexBuffer(0, (BYTE**)&pTVtx)))
	{
		pTVtx[0].x = 1.0f;  pTVtx[0].y = 0.0f; pTVtx[0].z = 1.0f;  pTVtx[0].color = 0xffffffff;
		pTVtx[1].x = 1.0f;  pTVtx[1].y = 0.0f; pTVtx[1].z = -1.0f; pTVtx[1].color = 0xffffffff;
		pTVtx[2].x = -1.0f; pTVtx[2].y = 0.0f; pTVtx[2].z = -1.0f; pTVtx[2].color = 0xffffffff;
		pTVtx[3].x = -1.0f; pTVtx[3].y = 0.0f; pTVtx[3].z = 1.0f;  pTVtx[3].color = 0xffffffff;

		g_pTxtMesh->UnlockVertexBuffer();
	}

	unsigned short *pTInd;

	if(SUCCEEDED(g_pTxtMesh->LockIndexBuffer(0, (BYTE**)&pTInd)))
	{
		pTInd[0] = 0; pTInd[3] = 2;
		pTInd[1] = 1; pTInd[4] = 3;
		pTInd[2] = 2; pTInd[5] = 0;

		g_pTxtMesh->UnlockIndexBuffer();
	}

	D3DXComputeNormals(g_pTxtMesh);
}

PROTECTED void _TextureTermVtxBuff()
{
	if(g_pTxtSprite)
		g_pTxtSprite->Release();

	if(g_pTxtMesh)
		g_pTxtMesh->Release();

	g_pTxtSprite = 0;
	g_pTxtMesh = 0;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt(hTXT txt, float x, float y, const RECT *srcRect,
								 const unsigned int *color, float rotate)
{
	D3DXVECTOR2 vect(x,y);

	if(rotate != 0)
	{
		float sW, sH;
	
		if(srcRect)
		{ sW = srcRect->right - srcRect->left; sH = srcRect->bottom - srcRect->top; }
		else
		{ sW = txt->width; sH = txt->height; }

		D3DXVECTOR2 rotCenter(sW/2,sH/2);
		g_pTxtSprite->Draw(txt->texture, srcRect, 0, &rotCenter, rotate, &vect, color ? *color : 0xffffffff);
	}
	else
		g_pTxtSprite->Draw(txt->texture, srcRect, 0, 0, 0, &vect, color ? *color : 0xffffffff);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureStretchBlt(hTXT txt, float x, float y, 
								 float w, float h, const RECT *srcRect,
								 const unsigned int *color, float rotate)
{
	float sW, sH;
	
	if(srcRect)
	{ sW = srcRect->right - srcRect->left; sH = srcRect->bottom - srcRect->top; }
	else
	{ sW = txt->width; sH = txt->height; }

	D3DXVECTOR2 vect(x,y);
	D3DXVECTOR2 scale(w/sW,h/sH);
	D3DXVECTOR2 rotCenter(w/2,h/2);

	g_pTxtSprite->Draw(txt->texture, srcRect, &scale, &rotCenter, rotate, &vect, color ? *color : 0xffffffff);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE TextureBlt3D(hTXT txt, const D3DXMATRIX *wrldMtx,
							const RECT* pSrcRect, const D3DMATERIAL8 *material,
							const unsigned int *color)
{
	//set cull to none
	g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	//do transformations
    g_p3DDevice->SetTransform(D3DTS_WORLD, wrldMtx);

	if(material)
		g_p3DDevice->SetMaterial(material);

	TextureSet(txt, 0);

	gfxTVtx *pTVtx;

	if(SUCCEEDED(g_pTxtMesh->LockVertexBuffer(0, (BYTE**)&pTVtx)))
	{
		if(!pSrcRect) //draw entire bitmap
		{
			//Top Left
			pTVtx[0].s = 1;
			pTVtx[0].t = 1;
			//Bottom Left
			pTVtx[1].s = 1;
			pTVtx[1].t = 0;
			//Bottom Right
			pTVtx[2].s = 0;
			pTVtx[2].t = 0;
			//Top Right
			pTVtx[3].s = 0;
			pTVtx[3].t = 1;
		}
		else
		{
			float sX=pSrcRect->left/(float)txt->width;
			float sY=pSrcRect->top/(float)txt->height;
			float eX=pSrcRect->right/(float)txt->width;
			float eY=pSrcRect->bottom/(float)txt->height;

			//Top Left
			pTVtx[0].s = eX;
			pTVtx[0].t = eY;
			//Bottom Left
			pTVtx[1].s = eX;
			pTVtx[1].t = sY;
			//Bottom Right
			pTVtx[2].s = sX;
			pTVtx[2].t = sY;
			//Top Right
			pTVtx[3].s = sX;
			pTVtx[3].t = eY;
			
		}

		if ( color ) {

			pTVtx[0].color = *color;
			pTVtx[1].color = *color;
			pTVtx[2].color = *color;
			pTVtx[3].color = *color;
		}
		else {
			pTVtx[0].color = 0xffffffff;
			pTVtx[1].color = 0xffffffff;
			pTVtx[2].color = 0xffffffff;
			pTVtx[3].color = 0xffffffff;
		}
	}

    _GFXCheckError(g_pTxtMesh->DrawSubset(0),
	true, "Error in TextureBlt3D");

	g_p3DDevice->SetTexture(0,0);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetWidth(hTXT txt)
{
	return txt->width;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int TextureGetHeight(hTXT txt)
{
	return txt->height;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int TextureGetID(hTXT txt)
{
	return txt->ID;
}