#include "gdix.h"
#include "gdix_i.h"

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hFNT FontCreate(const char *typeName, unsigned int size, unsigned int fmtFlag)
{
	hFNT newFnt;

	if(MemAlloc((void**)&newFnt, sizeof(gfxFont), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new font", "Error in FontCreate"); return 0; }

	lstrcpy( newFnt->strFont, _T(typeName) );

	newFnt->fontSize = size;
	newFnt->fntFormat = fmtFlag;

	newFnt->pD3DFont = new CD3DFont( newFnt->strFont, size, fmtFlag );

	if(FAILED(newFnt->pD3DFont->InitDeviceObjects(g_p3DDevice)))
	{ FontDestroy(&newFnt); return 0; }

	if(FAILED(newFnt->pD3DFont->RestoreDeviceObjects()))
	{ FontDestroy(&newFnt); return 0; }
	
	return newFnt;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf2D(hFNT fnt, float x, float y, unsigned int color, const char *text, ...)
{
	char buff[MAXCHARBUFF];								// Holds Our String

	//do the printf thing
	va_list valist;
	va_start(valist, text);
	_vsnprintf(buff, sizeof(buff), text, valist);
	va_end(valist);

	if(FAILED(fnt->pD3DFont->DrawText(x, y, color, buff)))
		return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE FontPrintf3D(hFNT fnt, D3DXMATRIX *wrldMtx, D3DMATERIAL8 *mtrl, DWORD flags, const char *text, ...)
{
	char buff[MAXCHARBUFF];								// Holds Our String

	//do the printf thing
	va_list valist;
	va_start(valist, text);
	_vsnprintf(buff, sizeof(buff), text, valist);
	va_end(valist);

	g_p3DDevice->SetMaterial(mtrl);
    g_p3DDevice->SetTransform(D3DTS_WORLD, wrldMtx);
    
	if(FAILED(fnt->pD3DFont->Render3DText(buff, flags)))
		return RETCODE_FAILURE;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontDestroy(hFNT *fnt)
{
	assert(fnt);

	if(*fnt)
	{
		if((*fnt)->pD3DFont)
		{
			(*fnt)->pD3DFont->InvalidateDeviceObjects();
			
			(*fnt)->pD3DFont->DeleteDeviceObjects();

			delete (*fnt)->pD3DFont;
			(*fnt)->pD3DFont = 0;
		}

		MemFree((void**)fnt);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetStrSize(hFNT fnt, char *str, SIZE *pSize)
{
	fnt->pD3DFont->GetTextExtent(str, pSize);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void FontGetCharSize(hFNT fnt, char c, SIZE *pSize)
{
	char foo[2] = {c,0};
	fnt->pD3DFont->GetTextExtent(foo, pSize);
}