//made by JOHN ENNEY!
//copyright 1992
//all rights reserved
//patent pending
//paper pumpernickel pie

#ifndef SCROLLBAR_H_NE
#define SCROLLBAR_H_NE



//Handle to a Scroll Bar
typedef struct _ScrollBar* HSCROLLBAR;


//this function is called when a scrollbar has its position changed
// it calls this with hParent as its parent, hScroll as itself, and
// nPos as its new position
typedef void (*SCROLLBARFUNC) ( HMC hParent, HSCROLLBAR hScroll, int nPos );


//this is called to create a scroll bar the easy way
HMC ScrollBarCreateFromFile ( FILE* pFile );

//sets the scroll bar to react with the new data
void ScrollBarSetInfo ( HSCROLLBAR hScroll, int nMaxItems, int nVisibleItems );

//give the scroll bar at new start position
void ScrollBarSetTop ( HSCROLLBAR hScroll, int nTop );

//associate the scroll bar with a parent and its function
void ScrollBarAssociate ( HSCROLLBAR hScroll, HMC hParent, SCROLLBARFUNC pSbf );



//the functions below are only used if you are
//doing more work than you need to

//creates a scroll bar, but it doesn't have any buttons
HSCROLLBAR ScrollBarCreate ( int nId, int nX, int nY, int nWidth, int nHeight, int nButtonHeight, int nBitmap );

//you must call this to add buttons to the scroll bar, if it
// wasn't created from a file
ScrollBarAddButton ( HSCROLLBAR hScroll, HMC hBut );

//destroys the scroll bar
void ScrollBarDestroy ( HMC hMc, int bAll );

#endif