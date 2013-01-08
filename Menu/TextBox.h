#ifndef TEXTBOX_H_NE
#define TEXTBOX_H_NE



typedef struct _TextBox* HTEXTBOX;


//changes the text that the text box displays, the text cannot be a blank
// string, a space is suggested as it will appear to be nothing on the screen
PUBLIC void TextBoxChangeText ( HTEXTBOX hTextBox, char* pNewText );


//changes the text color of the text box text, I suggest cunning use of
// D3DCOLOR_RGBA
PUBLIC void TextBoxChangeTextColor ( HTEXTBOX hTextBox, int nColor );


//this function returns a pointer to the text in the textbox
// this pointer is not owned by the user, but by the box
// changing the information in it could be very very bad
PUBLIC char* TextBoxGetText ( HTEXTBOX hTextBox );


//creates a menu item with the normal info plus all the
// font stuff that it needs
PUBLIC HTEXTBOX TextBoxCreate ( int nId, int nX, int nY, int nWidth, int nHeight, char* pText, int nColor, int nFont );

HMC TextBoxCreateFromFile ( FILE* pFile );

//Frees up all the stuff associated with teh TExtBOx
void TextBoxDestroy ( HMC hMc, int bAll );

#endif