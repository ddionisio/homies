#include "gdix.h"
#include "gdix_i.h"

#pragma comment(lib,"d3d8.lib") //no linking needed
#pragma comment(lib,"d3dx8.lib")
//#pragma comment(lib,"winmm.lib")

LPDIRECT3D8		  g_pD3D = 0;
LPDIRECT3DDEVICE8 g_p3DDevice = 0;
ID3DXMatrixStack *g_pWrldStack = 0;//the world matrix stack
//gfxDisplayMode	  g_DISPLAY = {0};
SIZE			  g_SCRNSIZE = {0};  //the screen size or window size
FLAGS			  g_FLAGS = 0;

LISTPTR			  g_TXTLIST;			//list of textures
LISTPTR			  g_MDLLIST;			//list of models
LISTPTR			  g_OBJLIST;			//list of objects
LISTPTR			  g_OBJDEACTIVATELIST;	//list of deactivated objects

char			  g_MDLLISTFILE[DMAXCHARBUFF]={0}; //the model list file
char			  g_TXTLISTFILE[DMAXCHARBUFF]={0}; //the texture list file

DWORD g_numLight=0;			//number of lights enabled
DWORD g_lastLight=0;		//last index of created lights

float g_sinTable[SINCOUNT]={0};
float g_cosTable[SINCOUNT]={0};

PRIVATE void _GFXInitSinTable()
{
	for(int i = 0; i < SINCOUNT; i++)
	{
		g_sinTable[i] = float(sin(((2*D3DX_PI)/SINCOUNT)*i));
		g_cosTable[i] = float(cos(((2*D3DX_PI)/SINCOUNT)*i));
	}
}

/////////////////////////////////////
// Name:	_GFXGetD3dFormat
// Purpose:	grabs D3DFORMAT of pMode
// Output:	none
// Return:	the D3DFORMAT
/////////////////////////////////////
PROTECTED D3DFORMAT _GFXGetD3dFormat(gfxDisplayMode *pMode)
{
	if(pMode)
	{
		switch(pMode->bpp) //get d3dformat, screw the rest...
		{
		case BPP_32:
			//return D3DFMT_A8R8G8B8;
			return D3DFMT_X8R8G8B8;
		case BPP_24:
			return D3DFMT_R8G8B8;
		case BPP_16:
			return D3DFMT_R5G6B5;
		case BPP_15a:
			return D3DFMT_A1R5G5B5;
		case BPP_15:
			return D3DFMT_X1R5G5B5;
		case BPP_8:
			return D3DFMT_P8;
		case BPP_DOH:
			//get format of choice...
			return D3DFMT_UNKNOWN;
		}
	}

	return D3DFMT_UNKNOWN;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC bool GFXIsDepthFormatExisting(D3DFORMAT DepthFormat, D3DFORMAT AdapterFormat)
{
    HRESULT hr = g_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                          D3DDEVTYPE_HAL,
                                          AdapterFormat,
                                          D3DUSAGE_DEPTHSTENCIL,
                                          D3DRTYPE_SURFACE,
                                          DepthFormat);

    return SUCCEEDED( hr );
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXResize(unsigned int width, unsigned int height)
{
	//set up the projection matrix
	D3DXMATRIX mtxProj;

	g_SCRNSIZE.cx = width;
	g_SCRNSIZE.cy = height;

	D3DXMatrixPerspectiveFovRH(&mtxProj, //Result Matrix
                              FOV,		 //Use 45 degree
                              ((float)g_SCRNSIZE.cx / (float)g_SCRNSIZE.cy),     //Aspect ratio
                              kDEPTHNEAR,     //Near view plane
                              kDEPTHFAR);	  // Far view plane

   //set up the projection matrix, we won't be doing anything else...
   g_p3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProj);
}

/////////////////////////////////////
// Name:	GFXSetTextureFilter
// Purpose:	sets texture filter for min/mag
//          base is usually 0.
// Output:	texture filter on base is updated
// Return:	RETCODE_SUCCESS if successful
/////////////////////////////////////
PUBLIC RETCODE GFXSetTextureFilter(unsigned int base, D3DTEXTUREFILTERTYPE minFilter, D3DTEXTUREFILTERTYPE magFilter)
{
	g_p3DDevice->SetTextureStageState(base,D3DTSS_MAGFILTER,magFilter);
	g_p3DDevice->SetTextureStageState(base,D3DTSS_MINFILTER,minFilter);
	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	GFXEnableLight
// Purpose:	enable/disable light
//			also set ambient color
// Output:	light is enabled/disabled
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnableLight(bool do_you_want_light, unsigned int ambient)
{
	//set light
	if(do_you_want_light)
	{
		g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
		g_p3DDevice->SetRenderState(D3DRS_AMBIENT,ambient); //hard-coded for now...
		CLEARFLAG(g_FLAGS, GFX_LIGHTDISABLE);
	}
	else
	{
		g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		SETFLAG(g_FLAGS, GFX_LIGHTDISABLE);
	}
}

/////////////////////////////////////
// Name:	GFXIsLightEnabled
// Purpose:	is light enabled?
// Output:	none
// Return:	true if so...
/////////////////////////////////////
PUBLIC bool GFXIsLightEnabled()
{
	return !TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXInit(HWND hwnd, gfxDisplayMode *pMode)
{
	//quickly initialize the sin table first
	_GFXInitSinTable();

	//the very first thing to do
	//initialize the mac daddy
	g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);

	if(!g_pD3D) //make sure it's successfully created
	{ ASSERT_MSG(0, "Unable to create 3D Device", "Error in GFXInit"); return RETCODE_FAILURE; }

	D3DPRESENT_PARAMETERS d3dParam={0}; 

	d3dParam.SwapEffect     = D3DSWAPEFFECT_DISCARD; //sounds cool...
    d3dParam.hDeviceWindow  = hwnd;					 //our HWND
    d3dParam.BackBufferCount= 1;					 //use only 1

	if(pMode)
	{
		SETFLAG(g_FLAGS, GFX_FULLSCREEN); //set up the flag

		d3dParam.Windowed          = FALSE;
		d3dParam.BackBufferWidth   = pMode->width;
		d3dParam.BackBufferHeight  = pMode->height;
		d3dParam.BackBufferFormat  = _GFXGetD3dFormat(pMode);

		g_SCRNSIZE.cx = pMode->width;
		g_SCRNSIZE.cy = pMode->height;
	}
	else
	{
		D3DDISPLAYMODE		  d3dDisplayMode={0};
		//Get desktop display mode.
	    if(_GFXCheckError(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3dDisplayMode), true, "Error in GFXInit"))
			return RETCODE_FAILURE;

		d3dParam.Windowed          = TRUE;
		d3dParam.BackBufferFormat  = d3dDisplayMode.Format;

		//get client area
		RECT winRect; GetClientRect(hwnd, &winRect);
		g_SCRNSIZE.cx = winRect.right-winRect.left;
		g_SCRNSIZE.cy = winRect.bottom-winRect.top;
	}

	d3dParam.EnableAutoDepthStencil = TRUE;			 //depth/stencil buffering

	if(GFXIsDepthFormatExisting(D3DFMT_D32, d3dParam.BackBufferFormat))
		d3dParam.AutoDepthStencilFormat = D3DFMT_D32;
	else if(GFXIsDepthFormatExisting(D3DFMT_D16, d3dParam.BackBufferFormat))
		d3dParam.AutoDepthStencilFormat = D3DFMT_D16;

	//create the device interface
	if(_GFXCheckError(
		g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,			   //primary for now...
                          D3DDEVTYPE_HAL,					   //use hardware
                          hwnd,								   //the window
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING, //software for now...
						  //D3DCREATE_HARDWARE_VERTEXPROCESSING,
                          &d3dParam,
                          &g_p3DDevice), true, "Error in GFXInit"))
						  return RETCODE_FAILURE;

	//create the world stack
	D3DXCreateMatrixStack(0, &g_pWrldStack);
	g_pWrldStack->LoadIdentity();

	//set light
	GFXEnableLight(false, AMBIENT_DEFAULT);

	//set up projection
	GFXResize(g_SCRNSIZE.cx, g_SCRNSIZE.cy);

	//set up depth buffering
	g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	g_p3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	g_p3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE , TRUE);
	g_p3DDevice->SetRenderState(D3DRS_ALPHAFUNC , D3DCMP_GREATER  );

	/////////////////////////////////////////////////////////////////////////////////
	//set up base texture rendering/blending
	//this tells us to set texture color operation as modulate: COLORARG1 * COLORARG2
	g_p3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	g_p3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE); //use texture
    g_p3DDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT); //use vtx color

	GFXSetTextureFilter(0,D3DTEXF_LINEAR,D3DTEXF_LINEAR); //this'll set the texture filtering

	g_p3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	g_p3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE); //use texture
    g_p3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_CURRENT); //use vtx color

	//g_p3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
    //g_p3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	/////////////////////////////////////////////////////////////////////////////////

	//set up alpha blending for renderer
	g_p3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA); 
    g_p3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

    g_p3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

	//create texture vertex buffer, for blitting texture
	_TextureInitVtxBuff();
	
	//that's all...
	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXDestroy()
{
	PARFXDestroyAll();
	_OBJDestroyAll();
	_MDLDestroyAll();
	_TextureDestroyAll();

	_TextureTermVtxBuff();

	if(g_pWrldStack)
	{ g_pWrldStack->Release(); g_pWrldStack=0;}
	if(g_p3DDevice)
	{g_p3DDevice->Release(); g_p3DDevice=0;}
	if(g_pD3D)
	{g_pD3D->Release(); g_pD3D=0;}

	GFXPageClear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void GFXClear(int numRect, const D3DRECT* pRects, DWORD color)
{
	g_p3DDevice->Clear(numRect,
                       pRects,
                       D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, //what to clear, no depth buffer for now...
                       color, //the color
                       1.0f,  //depth buffer value to clear
                       0 );   //stencil buffer, if I know what it's for
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXDisplayScene()
{
	if(SUCCEEDED(g_p3DDevice->BeginScene()))
	{
		//display all object
		hOBJ thisObj;

		for(LISTPTR::iterator i = g_OBJLIST.begin(); i != g_OBJLIST.end(); ++i)
		{
			thisObj = (hOBJ)(*i);
			
			OBJUpdateFrame(thisObj);
			OBJDisplay(thisObj);
		}

		g_p3DDevice->EndScene();
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE GFXUpdate(const RECT* pSrcRect, const RECT* pDestRect, HWND hwndOverride)
{
	if(_GFXCheckError(
		g_p3DDevice->Present(pSrcRect, pDestRect, hwndOverride, 0),
		true, "Error in GFXUpdate"))
		return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	GFXBegin
// Purpose:	readies device to render
//			stuff.
// Output:	shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBegin()
{
	g_p3DDevice->BeginScene();
}

/////////////////////////////////////
// Name:	GFXEnd
// Purpose:	call this after finish
//			with rendering stuff
// Output:	more shit happens
// Return:	none
/////////////////////////////////////
PUBLIC void GFXEnd()
{
	g_p3DDevice->EndScene();
}

/////////////////////////////////////
// Name:	GFXBltModeEnable
// Purpose:	use this for blitting
//			textures on 2d raster
//			coord.
// Output:	projection is changed
//			disables depth buffer
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeEnable(bool bPriority)
{
	//begin paint
   g_pTxtSprite->Begin();

   //disable depth buffering
   if(!bPriority)
		g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
   else
	   g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

   //disable light
   if(!TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE))
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

   //disable culling
   g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

   //set flag
   SETFLAG(g_FLAGS, GFX_BLTMODE);
}

/////////////////////////////////////
// Name:	GFXIsBltMode
// Purpose:	checks if bltmode is enabled
// Output:	none
// Return:	if true
/////////////////////////////////////
PUBLIC bool GFXIsBltMode()
{
	return TESTFLAGS(g_FLAGS, GFX_BLTMODE);
}

/////////////////////////////////////
// Name:	GFXBltModeDisable
// Purpose:	disable blt mode
// Output:	changes projection
//			back to perspective
//			enables depth buffer again
// Return:	none
/////////////////////////////////////
PUBLIC void GFXBltModeDisable()
{
	//end paint
   g_pTxtSprite->End();

	//enable depth buffering
   g_p3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

   //enable light again, if flag is set
   if(!TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE))
	   g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

   //clear flag
   CLEARFLAG(g_FLAGS, GFX_BLTMODE);

   //enable culling
	g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

/////////////////////////////////////
// Name:	GFXGetScrnSize
// Purpose:	grabs size of screen
// Output:	none
// Return:	the scrn size
/////////////////////////////////////
PUBLIC SIZE GFXGetScrnSize()
{
	return g_SCRNSIZE;
}
