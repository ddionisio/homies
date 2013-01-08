#ifndef I_MENU_H_NE
#define I_MENU_H_NE



#define		HMC_INVIS		0x00
#define		HMC_ACTIVE		0x01
#define		HMC_INACTIVE	0x02


typedef struct MenuChild MC;
typedef struct MenuChild* HMC;

typedef void (*MENUDRAWFUNC) ( HMC hMc, int x, int y );
typedef int (*MENUMOUSEPROCFUNC) ( HMC hMc, UINT msg, int x, int y, WPARAM wParam );
typedef int (*MENUKEYBOARDFUNC) (HMC hMc, UINT msg, LPARAM lParam );
typedef void (*MENUDESTROYFUNC) ( HMC hMc, int bAll );
typedef int (*MENUADDCHILD) ( HMC hParent, HMC hChild, int nID );
typedef void (*MENUSHOWFUNC) ( HMC hMc, int bShow, int bAll );

struct _MenuFuncTable {

	MENUDRAWFUNC MenuDrawFunc;
	MENUMOUSEPROCFUNC MenuMouseProcFunc;
	MENUKEYBOARDFUNC MenuKeyboardFunc;
	MENUDESTROYFUNC MenuDestroyFunc;
	MENUSHOWFUNC MenuShowFunc;
	MENUADDCHILD MenuAddChild;
};

struct MenuChild {

	int nType;
	int nId;

	MenuChild* psParent;

	int nStatus;
	int x, y, z;
	int nWidth, nHeight, nDepth;

	struct _MenuFuncTable* psFuncTable;
};


typedef struct _MenuNode MenuNode;

struct _MenuNode {

	HMC pItem;
	MenuNode* pNext;
	MenuNode* pPrev;
};

//protected
void MenuCreateItem ( HMC hMc );

//searches through the list of objects for the menu item
// with the given ID.  returns NULL if the item cannot
// be found.
HMC MenuGetItem ( int nId );

//when an object is destroyed, it calls this function so
// it can be removed from the global list
void MenuDestroyItem ( HMC hMc );

//tells the menu to draw the specified object and set coordinates
void MenuDrawItem ( HMC hMc, int x, int y );


#endif