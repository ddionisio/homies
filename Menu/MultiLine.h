#ifndef _MULTILINE_H_NE
#define _MULTILINE_H_NE


typedef struct _MultiLine* HMULTILINE;


//A multiLine is a text box that can have multiple lines of text
// it can also have a scroll bar


void MultiLineAddText ( HMULTILINE hMulti, char* pText, int nWidth );

//clears the multiline text box
void MultiLineClear ( HMULTILINE hMulti );

HMC MultiLineCreateFromFile ( FILE* pFile );

#endif