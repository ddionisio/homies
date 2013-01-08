#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"


int gnFonts = 0;
hFNT* gpFonts = 0;


//get the handle to the font number nIndex
hFNT MenuFontGet ( int nIndex ) {

	if ( nIndex > gnFonts || nIndex < 0 ) {

		return 0;
	}

	return gpFonts[nIndex];
}


//create all the file specified fonts
int MenuFontLoadInternal ( FILE* pFile ) {

	int x;
	int nSize, nStyle;
	char pName[256];

	for ( x = 0; x < gnFonts; x++ ) {

		//get size, style and name
		if ( fscanf ( pFile, "%i %i %[^\n]\n", &nSize, &nStyle, pName ) != 3 ) {

			assert ( !"Error in Reading Font Information!" );
			break;
		}

		//create the font
		gpFonts[x] = FontCreate ( pName, nSize, nStyle );

		if ( gpFonts[x] == 0 ) {

			assert ( !"Error in Font Create!" );
			break;
		}
	}

	return x;
}


//loads up fonts from a file
int MenuFontLoad ( FILE* pFile ) {

	assert ( pFile );
	assert ( !gpFonts );

	//get the amount of fonts
	if ( fscanf ( pFile, "%i\n", &gnFonts ) != 1 ) {

		assert ( !"Error in Font File!" );
		return 0;
	}

	//allocate handle memory
	if ( MemAlloc ( (void**)&gpFonts, sizeof (*gpFonts) * gnFonts, 0 ) == RETCODE_FAILURE ) {

		assert ( !"Couldn't allocate Font Memory!" );

		gnFonts = 0;
		return 0;
	}

	//create all the fonts
	int nLoaded = MenuFontLoadInternal ( pFile );

	if ( nLoaded != gnFonts ) {

		int x;

		//destroy the created fonts
		for ( x = 0; x < nLoaded; x++ ) {

			FontDestroy ( &gpFonts[x] );
		}

		//free
		MemFree ( (void**) &gpFonts );

		gpFonts = 0;
		gnFonts = 0;

		assert ( !"Could not create Fonts from File!" );
		return 0;
	}

	return 1;
}


//unloads all the fonts and frees their storage
int MenuFontUnload () {

	int x;

	if ( gnFonts == 0 || gpFonts == 0 ) return 0;
	
	for ( x = 0; x < gnFonts; x++ ) {
		
		FontDestroy ( &gpFonts[x] );
	}

	MemFree ( (void**) &gpFonts );

	gpFonts = 0;
	gnFonts = 0;

	return 1;
}