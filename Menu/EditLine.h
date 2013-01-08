#ifndef EDITLINE_H_NE
#define EDITLINE_H_NE


typedef struct _EditLine* HEDITLINE;


//clears the text from the edit line
PUBLIC void EditLineClear ( HEDITLINE hEditLine );

//tries to add the character to the edit line, returns 0 on failure
PUBLIC int EditLineAddChar ( HEDITLINE hEditLine, char cChar );

//removes the last character from the string, returns 0 on failure
PUBLIC int EditLineDeleteChar ( HEDITLINE hEditLine );

//tries to add the character to the edit line, returns 0 on failure
//truncates string if edit line is full
PUBLIC int EditLineSetString ( HEDITLINE hEditLine, const char *str);

//copys the edit line's string into pString, assumes it is large enough
PUBLIC void EditLineGetString ( HEDITLINE hEdit, char* pString );

//returns the length of the string, does not include the null terminator
PUBLIC int EditLineGetStringLength ( HEDITLINE hEdit );

//creates a menu item with the normal info plus all the font stuff that it needs
PUBLIC HEDITLINE EditLineCreate ( int nId, int nX, int nY, int nWidth, int nHeight,
								int bText, char* pText, int nColor, char* pFont, int nSize,
								int nStyle, int nMaxChars );


HMC EditLineCreateFromFile ( FILE* pFile );

//Frees up all the stuff associated with teh editline
void EditLineDestroy ( HMC hMc, int bAll );

#endif