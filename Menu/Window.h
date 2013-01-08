
#ifndef WINDOW_H_NE
#define WINDOW_H_NE

typedef struct _WINDOW* HWINDOW;

//msg are all windows defined messages
// the ID of the object is stored in the upper 16 bits of wParam
// WM_MOUSEMOVE  when the mouse moves and an object uses that info this is sent
// WM_LBUTTONDOWN  when the mouse is clicked on an object
// WM_LBUTTONUP    when the mouse is un-clicked on an object
typedef int (*WINDOWPROC) ( int nWinId, UINT msg, WPARAM wParam, LPARAM lParam );

PUBLIC HWINDOW WindowCreate ( int nId, WINDOWPROC WinProc, int nBitmap, int nX, int nY, int nWidth, int nHeight );
PUBLIC void WindowDestroy ( HMC hMc, int bAll );

HMC WindowCreateFromFile ( FILE* pFile );

//attempts to add the given object to the given window, if the
// handle is not NULL, nId is ignored, if the handle is NULL, the
// function attempts to find the object associated with the id, if
// it cannot find the object, the function will fail
int WindowAddChild ( HMC hParent, HMC hChild, int nId);


PUBLIC RETCODE WindowRemoveChild ( HWINDOW hWindow, HMC hMc );


PUBLIC WINDOWPROC WindowNewProc ( HWINDOW hWindow, WINDOWPROC WindowProc );


//move the item to the new x and y coordinates
PUBLIC void WindowChangeSize ( HWINDOW hWindow, int newWidth, int newHeight );

//get the items x position
PUBLIC int WindowGetItemWidth ( HWINDOW hWindow );

//get the items y position
PUBLIC int WindowGetItemHeight ( HWINDOW hWindow );

//change the item's size by the specified amounts
PUBLIC void WindowSizeItem ( HWINDOW hWindow, int dWidth, int dHeight );

#endif