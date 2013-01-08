#ifndef CHECKBOX_H_NE
#define CHECKBOX_H_NE


typedef struct _CheckBox* HCHECKBOX;

PUBLIC HCHECKBOX CheckBoxCreate ( int nId, int nX, int nY, int nWidth, int nHeight, int bChecked, int nBitmap, int nCheckMap );
PUBLIC void CheckBoxDestroy ( HMC hMc, int bAll );

PUBLIC int CheckBoxIsChecked ( HCHECKBOX hCheckBox );

//set check state
PUBLIC void CheckBoxSetCheck ( HCHECKBOX hCheckBox, bool bSet );

//takes a check box handle and returns if the box is checked or not
HMC CheckBoxCreateFromFile ( FILE* pFile );

#endif