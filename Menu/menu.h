#ifndef MENU_H_NE
#define MENU_H_NE



#ifndef I_MENU_H_NE
#define I_MENU_H_NE


typedef struct MenuChild* HMC;

#endif


#define		HMC_INVIS		0x00
#define		HMC_ACTIVE		0x01
#define		HMC_INACTIVE	0x02


//initializes global menu variables to empty
PUBLIC RETCODE MenuInit ();

//send it a file pointer, and it will create menu items
PUBLIC int MenuLoadFromFile ( FILE* pFile );

//this will call tell a visible objects to draw
PUBLIC void MenuDraw ( );


//move the item to the new x and y coordinates
PUBLIC void MenuChangePos ( HMC hMc, int newX, int newY );

//get the items x position
PUBLIC int MenuGetItemX ( HMC hMc );

//get the items y position
PUBLIC int MenuGetItemY ( HMC hMc );

//move the item by the specified amounts
PUBLIC void MenuMoveItem ( HMC hMc, int dX, int dY );


//frees global list of menu's, windows themselves still
// need to be freed manually as of this release
// if bAll is non-zero, this function will call the
// destroy function on all children, telling them to
// destroy their children as well 
PUBLIC RETCODE MenuTerm ( int bAll );


//remove indicated window
PUBLIC RETCODE MenuRemoveWindow ( HMC hMc );

//Create menu node and place at beginning of list with pointer
// to the menu object, if hMc is not NULL, nId is ignored, if
// hMc is NULL, the function attempts to find the object in
// an internal list
PUBLIC RETCODE MenuAddWindow ( HMC hMc, int nId );

PUBLIC RETCODE MenuAddChild ( int nParent, int nChild );

//sets the windows show value to true of false based on bShow
// if bAll is true, then all of the items children will also
// have there show value changed
PUBLIC void MenuShowItem ( HMC hMc, int bShow, int bAll );


//returns whether the item is shown or not
PUBLIC int MenuGetShow ( HMC hMc );


HMC MenuGetItem ( int nId );


PUBLIC RETCODE MenuProc ( );

#endif