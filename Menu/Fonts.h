#ifndef _FONTS_H_NE
#define _FONTS_H_NE


//get the handle to the font number nIndex
hFNT MenuFontGet ( int nIndex );

//loads up fonts from a file
int MenuFontLoad ( FILE* pFile );

//unloads all the fonts and frees their storage
int MenuFontUnload ();

#endif