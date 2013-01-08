#include "..\common.h"
#include "..\Memory\Memory.h"
#include "..\GraphicsX\GdiX.h"

int gnTxt;
hTXT* gpTxt;

//get the handle to the font number nIndex
hTXT MenuBitmapGet ( int nIndex ) {

	if ( nIndex > gnTxt || nIndex < 0 ) {

		return 0;
	}

	return gpTxt[nIndex];
}


//create all the file specified fonts
int MenuBitmapLoadInternal ( FILE* pFile ) {

	int x;
	int nId, bColor, r, g, b;
	bool bC = false;
	char pName[256];

	for ( x = 0; x < gnTxt; x++ ) {

		//get size, style and name
		if ( fscanf ( pFile, "%i\t%i ", &nId, &bColor) != 2 ) {

			assert ( !"Error in Reading Bitmap Information!" );
			break;
		}

		if ( bColor ) {

			bC = true;
			if ( fscanf ( pFile, "%i %i %i ", &r, &g, &b ) != 3 ) {

				assert ( !"Error in Reading Bitmap ColorKey Information!" );
				break;
			}
		}

		if ( fscanf ( pFile, "%[^\n]\n", pName ) != 1 )  {

			assert ( !"Error in Reading Bitmap File Name!" );
			break;
		}

		//create the font
		gpTxt[x] = TextureCreate ( nId, pName, bC, D3DCOLOR_XRGB ( r, g, b ) );

		if ( gpTxt[x] == 0 ) {

			assert ( !"Error in Texture Create!" );
			break;
		}
	}

	return x;
}


//loads up fonts from a file
int MenuBitmapLoad ( FILE* pFile ) {

	assert ( pFile );
	assert ( !gpTxt );

	//get the amount of fonts
	if ( fscanf ( pFile, "%i\n", &gnTxt ) != 1 ) {

		assert ( !"Error in Texture File!" );
		return 0;
	}

	//allocate handle memory
	if ( MemAlloc ( (void**)&gpTxt, sizeof (*gpTxt) * gnTxt, 0 ) == RETCODE_FAILURE ) {

		assert ( !"Couldn't allocate Texture Memory!" );

		gnTxt = 0;
		return 0;
	}

	//create all the fonts
	int nLoaded = MenuBitmapLoadInternal ( pFile );

	if ( nLoaded != gnTxt ) {

		int x;

		//destroy the created fonts
		for ( x = 0; x < nLoaded; x++ ) {

			TextureDestroy ( &gpTxt[x] );
		}

		//free
		MemFree ( (void**) &gpTxt );

		gpTxt = 0;
		gnTxt = 0;

		assert ( !"Could not create Textures from File!" );
		return 0;
	}

	return 1;
}


//unloads all the fonts and frees their storage
int MenuBitmapUnload () {

	int x;

	if ( gnTxt == 0 || gpTxt == 0 ) return 0;
	
	for ( x = 0; x < gnTxt; x++ ) {
		
		TextureDestroy ( &gpTxt[x] );
	}

	MemFree ( (void**) &gpTxt );

	gpTxt = 0;
	gnTxt = 0;

	return 1;
}
