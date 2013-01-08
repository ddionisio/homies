#include "..\common.h"
#include "i_menu.h"
#include "Menu.h"
#include "Window.h"
#include "Button.h"
#include "CheckBox.h"
#include "TextBox.h"
#include "EditLine.h"
#include "ScrollBar.h"
#include "ListBox.h"
#include "MultiLine.h"




PUBLIC int MenuLoadFromFile ( FILE* pFile ) {

	float x;
	char type[181];
	char pBitmap[181];
	int nId, nX, nY;

	if ( fscanf ( pFile, "Mr. Menu Loader v%f\n", &x ) != 1 ) return 0;

	if ( x != 0.2f ) return 0;

	while ( 1 ) {

		type[0] = fgetc ( pFile );

		if ( type[0] == '~') break;

		if ( type[0] == '*') {

			fscanf ( pFile, "%[^\n]\n", pBitmap );
			continue;
		}

		fscanf ( pFile, "%s", &type[1] );

		switch ( type[0] ) {

		case 'w':
			WindowCreateFromFile ( pFile );
			break;

		case 'b':
			ButtonCreateFromFile ( pFile );
			break;

		case 'c':
			CheckBoxCreateFromFile ( pFile );
			break;

		case 't':
			TextBoxCreateFromFile ( pFile );
			break;

		case 'e':
			EditLineCreateFromFile ( pFile );
			break;

		case 's':
			ScrollBarCreateFromFile ( pFile );
			break;

		case 'l':
			ListBoxCreateFromFile ( pFile );
			break;
			
		case 'm':
			MultiLineCreateFromFile ( pFile );
			break;

		default:
			return 0;
		}
	}
	fgetc ( pFile );
	while ( 1 ) {

		type[0] = fgetc ( pFile );

		if ( type[0] == '~' ) break;

		fscanf ( pFile, "%s", &type[1] );

		switch ( type[0] ) {

		case 'a':
			fscanf ( pFile, "%i\n", &nId );
			MenuAddWindow ( 0, nId );
			break;

		case 'c':
			fscanf ( pFile, "%i %i\n", &nX, &nY );
			MenuAddChild ( nY, nX );
			break;

		default:
			fscanf ( pFile, "\n" );
			break;
		}
	}
	return 1;
}