#ifndef BUTTON_H_NE
#define BUTTON_H_NE

//Button will only send ButtonDown and ButtonUp messages
// when the button was clicked on that button, but not if
// the button was pressed elsewhere

typedef struct _Button* HBUTTON;

#define BUTTON_UP 0
#define BUTTON_DOWN 1
#define BUTTON_OVER 2


PUBLIC HBUTTON ButtonCreate ( int nId, char* pSprite, int nX, int nY, int nWidth, int nHeight, int nBitmap );
PUBLIC void ButtonDestroy ( HMC hMc, int bAll );

//this function is called from MenuLoadFromFile
// it is given a pointer to button data in a file
// reads the data and creates the button
HMC ButtonCreateFromFile ( FILE* pFile );

//This will set the button's texture color
//David's addition
void ButtonSetImgColor (HBUTTON hButton, unsigned int newC);

//This will change the image and sprite of button
//David's addition
void ButtonChange (HBUTTON hButton, unsigned int newTxtID, const char *newSpriteFile);

#endif