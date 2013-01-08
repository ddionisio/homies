#ifndef _BITMAPS_H_NE
#define _BITMAPS_H_NE


//get the handle to the bitmap with id number nIndex
//the menu system does NOT use this function, making
//these functions only useful for loading extra
//textures not loaded elsewhere
hFNT MenuBitmapGet ( int nIndex );

//loads up fonts from a file, this is not needed if the
//textures used in the menu can be loaded from somewhere
//else, as the menu does not use MenuBitmapGet, but does
//a TextureSearch to get the need texture handles
int MenuBitmapLoad ( FILE* pFile );

//unloads all the texture handles, this function only
//needs to be called if MenuBitmapLoad succeded
int MenuBitmapUnload ();

#endif