#ifndef LISTBOX_H_NE
#define LISTBOX_H_NE



typedef struct _ListBox* HLISTBOX;

//create from a file, easy way
HMC ListBoxCreateFromFile ( FILE* pFile );

//returns the 0 based index of the selected item
//returns -1 if no item is selected
int ListBoxGetSelected ( HLISTBOX hList );


//adds another line of text to the list box
int ListBoxAddItem ( HLISTBOX hList, char* pText, int nPos );

//remove an item from the list box by 0 based index
//if the index doesn't exist, the function fails
int ListBoxDeleteItem ( HLISTBOX hList, int nPos );

//removes all the items from the list box
void ListBoxClear ( HLISTBOX hList );


//creates an empty list box with no scroll bar or items
HLISTBOX ListBoxCreate ( int nId, int nX, int nY, int nWidth, int nHeight, int nBitmap, int nScrollBar, int nFont, int nColor, int nHighColor );

//change the offsets for the text displayed in the list box
PUBLIC void ListBoxSetOffsets ( HLISTBOX hList, int nXOffset, int nYOffset, int nTextHeight );

//destroy the list box and all its stuff
void ListBoxDestroy ( HMC hMc, int bAll );

//returns number of item in the list
PUBLIC int ListBoxGetNumItem(HLISTBOX hList);

//returns the string of given ind
//NULL if ind is invalid
PUBLIC char * ListBoxGetItemString(HLISTBOX hList, int ind);

#endif