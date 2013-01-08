#include "gdix.h"
#include "gdix_i.h"

//this is a temporary file, for testing stuff

gfxVertex gTriangle[] = {
	{ 0, 1, 10, D3DCOLOR_XRGB(255,0,0)},
	{ 1,-1, 10, D3DCOLOR_XRGB(0,255,0)},
	{-1,-1, 10, D3DCOLOR_XRGB(0,0,255)}
};

IDirect3DVertexBuffer8 *gTri=0;

PROTECTED void _GFXInitTri()
{
	//create the vertex buffer, like we do with surface handles...
	_GFXCheckError(
		g_p3DDevice->CreateVertexBuffer(sizeof(gfxVertex)*3,
									D3DUSAGE_WRITEONLY,
									GFXVERTEXFLAG,
									D3DPOOL_MANAGED,
									&gTri),
		true,
		"Error in _GFXInitTri");

	//put stuff inside our newly created vertex buffer

	BYTE *ptrVtx; //kinda like surface pointer, but it's a bunch of vertices

	//lock vertex buffer
	_GFXCheckError(
		gTri->Lock(0, //Offset, we want to start at the beginning
                   0, //SizeToLock, 0 means lock the whole thing
                   &ptrVtx, //If successful, this will point to the data in the VB
                   0),  //Flags, nothing special
		true,
		"Error in _GFXInitTri");

	//copy 'em!
	memcpy(ptrVtx, gTriangle, sizeof(gTriangle));

	//done, so unlock now!
	gTri->Unlock();
}

PROTECTED void _GFXDrawTri()
{
	//Vertex shaders are a complex topic, but you can do some amazing things with them
      //For this example we're not creating one, so we tell Direct3D that we're just
      //using a plain vertex format.
      g_p3DDevice->SetVertexShader(GFXVERTEXFLAG);

      //D3D's rendering functions read from streams.  Here we tell D3D that the
      //VB we created for our triangle is the stream it should read from.
      g_p3DDevice->SetStreamSource(0,gTri,sizeof(gfxVertex));

	  //After all that setup, actually drawing the triangle is pretty easy.
      //We tell it what we're giving it (a Triangle List), where it should
      //start reading (0, the beginning), and how many triangles we're drawing(1)
      g_p3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,1);
}

PROTECTED void _GFXTermTri()
{
	if(gTri)
	{
      gTri->Release();
      gTri=0;
    }
}