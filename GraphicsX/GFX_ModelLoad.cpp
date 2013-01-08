#include "gdix.h"
#include "gdix_i.h"

//All model file loading are done here

struct txc {
	float s,t;
};

PRIVATE void _ModelComputeNormPlane(D3DXPLANE *plane, gfxVtx *vtx1, gfxVtx *vtx2, gfxVtx *vtx3)
{
	//create a plane and store it to first index to plane buffer
	D3DXVECTOR3 v1(vtx1->x,vtx1->y,vtx1->z);
	D3DXVECTOR3 v2(vtx2->x,vtx2->y,vtx2->z);
	D3DXVECTOR3 v3(vtx3->x,vtx3->y,vtx3->z);

	D3DXPlaneFromPoints(plane, &v1, &v2, &v3);
	D3DXPlaneNormalize(plane, plane);
}

//my own model file!
PROTECTED RETCODE _ModelLoadDataFromFile(hMDL model, const char *filename)
{
	assert(model);

	RETCODE ret = RETCODE_SUCCESS;

	//data structures
	gfxVtx *vtxBuff=0; //it's gonna be biG!, bIG!!, BIG!!!
	
	txc *txcList=0;		//allocated later (numTxc)
	unsigned short *indList=0;		//allocated later (indNum inside model)
	int numTxc;
	int vtxInd[NUMPTFACE], txtInd[NUMPTFACE];

	int vtxCount, faceCount;

	//ready the file
	FILE *theFile;
	char buff[MAXCHARBUFF];
	bool bLoading = false;

	theFile = fopen(filename, "rt");
	if(!theFile)
	{ ASSERT_MSG(0, "Unable to open file", "Error in _ModelLoadDataFromFile"); return RETCODE_FAILURE; }

	do
	{
		fscanf(theFile, "%s\n", buff);

		if(strcmp(buff, "START") == 0) //start loading stuff if we found the START
			bLoading = true;
		else if(bLoading)
		{
			if(strcmp("[VERTEX]", buff) == 0)
			{
				fscanf(theFile, "numvertex=%d\n", &vtxCount);

				//create the array
				if(MemAlloc((void**)&vtxBuff, sizeof(gfxVtx)*vtxCount, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to allocate vertex buffer", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				//fill 'em up!
				int r,g,b,a;

				for(int i = 0; i < vtxCount; i++)
				{
					fscanf(theFile, "vertex=%f,%f,%f color=%d,%d,%d,%d\n", 
						&vtxBuff[i].x,&vtxBuff[i].y,&vtxBuff[i].z,
						&r,&g,&b,&a);
					vtxBuff[i].color = D3DCOLOR_RGBA(r,g,b,a);

					//get the normalized vector and put it as the normal for lighting
					//D3DXVECTOR3 vect(vtxBuff[i].x, vtxBuff[i].y, vtxBuff[i].z);
					//D3DXVec3Normalize(&vect,&vect);
					//float *nV = (float*)vect;
					//vtxBuff[i].nX = -nV[eX]; vtxBuff[i].nY = -nV[eY]; vtxBuff[i].nZ = -nV[eZ];
				}
			}
			else if(strcmp("[TEXTURE]", buff) == 0)
			{
				fscanf(theFile, "image=%s\n", buff);

				int filtermode;

				fscanf(theFile, "filtermode=%d\n", &filtermode);

				//we will only have one texture for now...
				model->numMaterial = 1;

				//allocate texture array
				if(MemAlloc((void**)&model->textures, sizeof(hTXT)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to allocate textures", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }
				MemSetPattern(model->textures, "MDLTXTS");

				model->textures[0] = TextureCreate(MDLTXTID, buff, false, 0);
				TextureAddRef(model->textures[0]);
				//D3DXCreateTextureFromFile(g_p3DDevice, buff, &model->texture);
				
				if(!model->textures[0])
				{ ASSERT_MSG(0,"Unable to load texture", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				//now get the texture locations
				fscanf(theFile, "numtextureloc=%d\n", &numTxc);

				//create the array
				if(MemAlloc((void**)&txcList, sizeof(txc)*numTxc, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to create texture location list", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				MemSetPattern(txcList, "GFXTXTL");

				//fill 'er up
				for(int i = 0; i < numTxc; i++)
				{
					fscanf(theFile, "texloc=%f,%f\n", &txcList[i].s, &txcList[i].t);
				}
			}
			else if(strcmp("[FACE]", buff) == 0)
			{
				fscanf(theFile, "numface=%d\n", &faceCount);

				//create the BIG @$$ array
				model->indCount = faceCount*NUMPTFACE;
				if(MemAlloc((void**)&indList, sizeof(unsigned short)*model->indCount, M_ZERO) != RETCODE_SUCCESS)
				{ ASSERT_MSG(0,"Unable to allocate index buffer, It's not so HUGE!!!", "error in _ModelLoadDataFromFile"); ret=RETCODE_FAILURE; goto BADMOJO; }

				MemSetPattern(vtxBuff, "GFXINDB");

				//fill 'em up
				for(int i = 0, j = 0; i < faceCount; i++, j+=NUMPTFACE)
				{
					fscanf(theFile, "vertexIndex=%d,%d,%d textureIndex=%d,%d,%d\n", 
						&vtxInd[0], &vtxInd[1], &vtxInd[2],
						&txtInd[0], &txtInd[1], &txtInd[2]);

					indList[j] = vtxInd[0];
					indList[j+1] = vtxInd[1];
					indList[j+2] = vtxInd[2];

					//do some messed up texture assign
					vtxBuff[vtxInd[0]].s = txcList[txtInd[0]].s;
					vtxBuff[vtxInd[0]].t = txcList[txtInd[0]].t;
					vtxBuff[vtxInd[1]].s = txcList[txtInd[1]].s;
					vtxBuff[vtxInd[1]].t = txcList[txtInd[1]].t;
					vtxBuff[vtxInd[2]].s = txcList[txtInd[2]].s;
					vtxBuff[vtxInd[2]].t = txcList[txtInd[2]].t;
				}
			}
			else if(strcmp("END", buff) == 0)
				bLoading = false;
		}
	}while(bLoading);

	//////////////////////////////////////////////////////////////
	//now time to stuff it all in the D3D vertex buffer
	//create the mesh
	_GFXCheckError(D3DXCreateMeshFVF(
		faceCount,
		vtxCount,
		D3DXMESH_MANAGED,
		GFXVERTEXFLAG,
		g_p3DDevice,
		&model->mesh),
	true, "Error in _ModelLoadDataFromFile");
	//////////////////////////////////////////////////////////////

	/////////////////
	//now fill 'er up
	BYTE *ptrVtx;

	if(_GFXCheckError(
		model->mesh->LockVertexBuffer(0, //Flags, nothing special
					   &ptrVtx			 //If successful, this will point to the data in the VB
					   ), 
					   true,
			"Error in _ModelLoadDataFromFile"))
	{ ret=RETCODE_FAILURE; goto BADMOJO; }

	memcpy(ptrVtx, vtxBuff, sizeof(gfxVtx)*vtxCount);

	model->mesh->UnlockVertexBuffer();
	/////////////////

	/////////////////
	//now fill the index buffer up
	BYTE *ptrInd;

	if(_GFXCheckError(
			model->mesh->LockIndexBuffer(0, //Flags, nothing special
					   &ptrInd     //If successful, this will point to the data in the IB
					   ),          
			true,
			"Error in _ModelLoadDataFromFile"))
	{ ret=RETCODE_FAILURE; goto BADMOJO; }

	memcpy(ptrInd, indList, sizeof(unsigned short)*model->indCount);

	model->mesh->UnlockIndexBuffer();
	/////////////////

	/////////////////////////////
	//create the adjacency buffer
	//and do some funky stuff
	D3DXCreateBuffer(sizeof(DWORD)*model->indCount, &model->adjacencyBuffer);

	model->mesh->GenerateAdjacency(0.0f, (DWORD*)model->adjacencyBuffer->GetBufferPointer());

	D3DXValidMesh(model->mesh, (DWORD*)model->adjacencyBuffer->GetBufferPointer());
	//////////////////////////////

	//This outta make lighting easy
	D3DXComputeNormals(model->mesh);

BADMOJO:

	if(theFile) fclose(theFile);
	if(indList) MemFree((void**)&indList);
	if(vtxBuff) MemFree((void**)&vtxBuff);
	if(txcList) MemFree((void**)&txcList);

	return ret;
}

//3ds ascii file textured
PROTECTED RETCODE _ModelLoadDataFromFileASE(hMDL model, const char *filename)
{
	assert(model);

	RETCODE ret = RETCODE_SUCCESS;

	//data structures
	gfxVtx *vtxBuff=0; //it's gonna be biG!, bIG!!, BIG!!!
	txc *txcList=0;		//allocated later (numTxc)
	unsigned short *indList=0;		//allocated later (indNum inside model)
	int numTxc;
	int vtxInd[NUMPTFACE], txtInd[NUMPTFACE];

	int vtxCount, faceCount;

	//ready the file
	FILE *theFile;
	char buff[MAXCHARBUFF];

	theFile = fopen(filename, "rt"); //FileOpenFile(filename, FILEREAD | FILETEXT);
	if(!theFile)
	{ ASSERT_MSG(0,"Unable to open file", "Error in _ModelLoadDataFromFileASE"); return RETCODE_FAILURE; }

	//now let's parse this baby!
	if(ParserSkipString(theFile, '*', ' ', "BITMAP"))
	{
		//let's get the filename
		if(ParserReadStringFile(theFile, buff, MAXCHARBUFF, '"', '"') == RETCODE_FAILURE) //we have reached end of file
		{ ASSERT_MSG(0,"Error reading ASE model file, cannot get BITMAP file!", "Error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

		//we will only have one texture for now...
		model->numMaterial = 1;

		//allocate texture array
		if(MemAlloc((void**)&model->textures, sizeof(hTXT)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to allocate textures", "error in _ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
		MemSetPattern(model->textures, "MDLTXTS");

		model->textures[0] = TextureCreate(MDLTXTID, buff, false, 0);
		TextureAddRef(model->textures[0]);
		//D3DXCreateTextureFromFile(g_p3DDevice, buff, &model->texture);
		
		if(!model->textures[0])
		{ ASSERT_MSG(0,"Unable to load texture", "error in _ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

	//now let's find the num vertices
	if(ParserSkipString(theFile, '*', ' ', "MESH_NUMVERTEX"))
	{
		//get the number
		fscanf(theFile, "%d", &vtxCount);

		//create the array
		if(MemAlloc((void**)&vtxBuff, sizeof(gfxVtx)*vtxCount, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to allocate vertex buffer", "error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

		MemSetPattern(vtxBuff, "GFXVTXB");
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in _ModelLoadDataFromFileASE"); ret = RETCODE_FAILURE; goto BADMOJO2; }

	//now get the num faces
	if(ParserSkipString(theFile, '*', ' ', "MESH_NUMFACES"))
	{
		//get the number
		fscanf(theFile, "%d", &faceCount);

		//create the BIG @$$ array
		model->indCount = faceCount*NUMPTFACE;
		if(MemAlloc((void**)&indList, sizeof(unsigned short)*model->indCount, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to allocate vertex buffer, It's not HUGE!!!", "error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

		MemSetPattern(indList, "GFXINDB");
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

	int i, j, dummy;

	//load up the vertex list
	for(i = 0; i < vtxCount; i++)
	{
		//skip "*MESH_VERTEX "
		if(ParserSkipString(theFile, '*', ' ', "MESH_VERTEX"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, 9) == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file 'dummy'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need
			if(ParserReadDataFile(theFile, &vtxBuff[i].x, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file 'vtx X'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxBuff[i].y, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file 'vtx Y'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxBuff[i].z, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file 'vtx Z'", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
			
			vtxBuff[i].color = 0xFFFFFFFF;

			//get the normalized vector and put it as the normal for lighting
			D3DXVECTOR3 vect(vtxBuff[i].x, vtxBuff[i].y, vtxBuff[i].z);
			D3DXVec3Normalize(&vect,&vect);
			float *nV = (float*)vect;
			vtxBuff[i].nX = -nV[eX]; vtxBuff[i].nY = -nV[eY]; vtxBuff[i].nZ = -nV[eZ];
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file '*MESH_VERTEX '", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}

	//now to fill in the face index
	for(i = 0, j = 0; i < faceCount; i++, j+=NUMPTFACE)
	{
		//skip "*MESH_FACE "
		if(ParserSkipString(theFile, '*', ' ', "MESH_FACE"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, ':') == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need

			//skip this junk A:
			if(ParserReadWordFile(theFile, buff, MAXCHARBUFF, ':') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxInd[0], dataINT, ' ') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//skip this junk B:
			if(ParserReadWordFile(theFile, buff, MAXCHARBUFF, ':') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxInd[1], dataINT, ' ') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//skip this junk C:
			if(ParserReadWordFile(theFile, buff, MAXCHARBUFF, ':') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &vtxInd[2], dataINT, ' ') == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			indList[j] = vtxInd[0];
			indList[j+1] = vtxInd[1];
			indList[j+2] = vtxInd[2];
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}

	//now let's get the number of texture vertices
	if(ParserSkipString(theFile, '*', ' ', "MESH_NUMTVERTEX"))
	{
		//get the number
		fscanf(theFile, "%d", &numTxc);

		//create the array
		if(MemAlloc((void**)&txcList, sizeof(txc)*numTxc, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0,"Unable to create texture location list", "error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

		MemSetPattern(txcList, "GFXTXTL");
	}
	else
	{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

	float junk; //we don't need the depth in texture for now...

	//let's fill 'er up
	for(i = 0; i < numTxc; i++)
	{
		//skip "*MESH_TVERT "
		if(ParserSkipString(theFile, '*', ' ', "MESH_TVERT"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, 9) == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need
			if(ParserReadDataFile(theFile, &txcList[i].s, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &txcList[i].t, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &junk, dataFLOAT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
			
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }
	}

	//let's fill in the face index
	//number of face to have texture will be the number of faces created...for now...
	for(i = 0, j = 0; i < faceCount; i++, j+=NUMPTFACE)
	{
		//skip "*MESH_TFACE "
		if(ParserSkipString(theFile, '*', ' ', "MESH_TFACE"))
		{
			//skip index number
			if(ParserReadDataFile(theFile, &dummy, dataINT, 9) == RETCODE_FAILURE) //we have reached end of file
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			//now let's get those that we need
			if(ParserReadDataFile(theFile, &txtInd[0], dataINT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &txtInd[1], dataINT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			if(ParserReadDataFile(theFile, &txtInd[2], dataINT, 9) == RETCODE_FAILURE)
			{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); ret=RETCODE_FAILURE; goto BADMOJO2; }

			vtxBuff[indList[j]].s = txcList[txtInd[0]].s;
			vtxBuff[indList[j]].t = txcList[txtInd[0]].t;

			vtxBuff[indList[j+1]].s = txcList[txtInd[0]].s;
			vtxBuff[indList[j+1]].t = txcList[txtInd[0]].t;

			vtxBuff[indList[j+2]].s = txcList[txtInd[2]].s;
			vtxBuff[indList[j+2]].t = txcList[txtInd[2]].t;
		}
		else
		{ ASSERT_MSG(0,"Error reading ASE model file", "Error in ModelLoadDataFromFileASE"); fclose(theFile); return RETCODE_FAILURE; }
	}

	//////////////////////////////////////////////////////////////
	//now time to stuff it all in the D3D vertex buffer
	//create the mesh
	_GFXCheckError(D3DXCreateMeshFVF(
		faceCount,
		vtxCount,
		D3DXMESH_MANAGED,
		GFXVERTEXFLAG,
		g_p3DDevice,
		&model->mesh),
	true, "Error in ModelLoadDataFromFileASE");
	//////////////////////////////////////////////////////////////

	/////////////////
	//now fill 'er up
	BYTE *ptrVtx;

	if(_GFXCheckError(
		model->mesh->LockVertexBuffer(0, //Flags, nothing special
					   &ptrVtx			 //If successful, this will point to the data in the VB
					   ), 
					   true,
			"Error in ModelLoadDataFromFileASE"))
	{ ret=RETCODE_FAILURE; goto BADMOJO2; }

	memcpy(ptrVtx, vtxBuff, sizeof(gfxVtx)*vtxCount);

	model->mesh->UnlockVertexBuffer();
	/////////////////

	/////////////////
	//now fill the index buffer up
	BYTE *ptrInd;

	if(_GFXCheckError(
			model->mesh->LockIndexBuffer(0, //Flags, nothing special
					   &ptrInd     //If successful, this will point to the data in the IB
					   ),          
			true,
			"Error in ModelLoadDataFromFileASE"))
	{ ret=RETCODE_FAILURE; goto BADMOJO2; }

	memcpy(ptrInd, indList, sizeof(unsigned short)*model->indCount);

	model->mesh->UnlockIndexBuffer();
	/////////////////

	/////////////////////////////
	//create the adjacency buffer
	//and do some funky stuff
	D3DXCreateBuffer(sizeof(DWORD)*model->indCount, &model->adjacencyBuffer);

	model->mesh->GenerateAdjacency(0.0f, (DWORD*)model->adjacencyBuffer->GetBufferPointer());

	D3DXValidMesh(model->mesh, (DWORD*)model->adjacencyBuffer->GetBufferPointer());
	//////////////////////////////

	//This outta make lighting easy
	D3DXComputeNormals(model->mesh);

BADMOJO2:

	if(theFile) fclose(theFile);
	if(indList) MemFree((void**)&indList);
	if(vtxBuff) MemFree((void**)&vtxBuff);
	if(txcList) MemFree((void**)&txcList);

	return ret;
}

//Direct X-files!  Those bastards made it too easy...
PROTECTED RETCODE _ModelLoadDataFromFileX(hMDL model, const char *filename)
{
	assert(model);

	RETCODE ret = RETCODE_SUCCESS;
	
	char path[DMAXCHARBUFF];

	LPD3DXBUFFER pD3DXMtrlBuffer = 0; //material buffer to be copied later

	//DXUtil_FindMediaFile(path, filename);
	strcpy(path, filename);

	unsigned long numMat;

	if(_GFXCheckError(
		D3DXLoadMeshFromX( path, D3DXMESH_MANAGED, g_p3DDevice, 
                            &model->adjacencyBuffer, &pD3DXMtrlBuffer, 
                            &numMat, &model->mesh),
							true, "Error in ModelLoadDataFromFileASE"))
	{ ret=RETCODE_FAILURE; return ret; }

	model->numMaterial = numMat;

	//allocate both textures and materials
	if(MemAlloc((void**)&model->textures, sizeof(hTXT)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
	{ ret=RETCODE_FAILURE; return ret; }

	if(MemAlloc((void**)&model->materials, sizeof(D3DMATERIAL8)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
	{ ret=RETCODE_FAILURE; return ret; }

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	//load up texture and copy material data
	for(int i = 0; i < model->numMaterial; i++ )
    {
        model->materials[i] = d3dxMaterials[i].MatD3D;
        model->materials[i].Ambient = model->materials[i].Diffuse;

        // Get a path to the texture
		if(d3dxMaterials[i].pTextureFilename)
		{
			//DXUtil_FindMediaFile(path, d3dxMaterials[i].pTextureFilename);

			// Load the texture
			model->textures[i] = TextureCreate(MDLTXTID, d3dxMaterials[i].pTextureFilename, false, 0);
			TextureAddRef(model->textures[i]);

			if(!model->textures[i])
			{ ret=RETCODE_FAILURE; return ret; }
		}
    }

	pD3DXMtrlBuffer->Release();

	//compute normals if specified
	if(model->mesh->GetFVF() & D3DFVF_NORMAL)
		D3DXComputeNormals(model->mesh);

	model->indCount = model->mesh->GetNumFaces()*NUMPTFACE;

	return ret;
}

struct md2Header {
	int magic;				//magic number: 844121161
	int version;			//version: 8
	int skinWidth;			//width of skin
	int skinHeight;			//height of skin
	int frameSize;			//size of each frame of model
	int numSkins;			//number of skin used
	int numVertices;		//number of vertices
	int numTexcoords;		//number of texture coord
	int numTriangles;		//number of triangles (face)
	int numGlCommands;		//some GL optimization for triangle_strip/fan
	int numFrames;			//number of key frame animations
	int offsetSkins;
	int offsetTexcoords;
	int offsetTriangles;
	int offsetFrames;
	int offsetGlCommands;
	int offsetEnd;
};

struct md2Vtx
{
	unsigned char vertex[3];		//Scaled version of the model's 'real' vertex coordinate
	unsigned char lightNormalIndex;	//An index into the table of normals, kept by Quake 2
};

struct md2Frame
{
	float	   scale[3];		//The scale used by the model's 'fake' vertex structure
	float	   translate[3];	//The translation used by the model's 'fake' vertex structure
	char	   name[16];		//The name for the frame
	md2Vtx vertices[1];		//An array of MD2_VERTEX structures
};

struct md2Face
{
   short vertexIndices[3];
   short textureIndices[3];
};

struct md2TxtCoord
{
   short s, t;
};

struct md2GlCommandVtx
{
   //int numVtx;		//0 = terminate, < 0 = tri-fan, > 0 = tri-strip
   float s, t;
   int vertexIndex;
};

typedef char md2SkinFile[64];

#if 0
PRIVATE RETCODE _MD2CreateFrame(LPD3DXMESH *mesh, char *frameName, long *glCommands,
								const BYTE *frames, const md2Face *faces,
								const md2Header &header, int frame)
{
	_GFXCheckError(D3DXCreateMeshFVF(
		header.numTriangles,
		header.numVertices,
		D3DXMESH_MANAGED,
		GFXVERTEXFLAG,
		g_p3DDevice,
		mesh),
	true, "Error in _MD2CreateFrame");

	long *command = glCommands;

	//go to the correct array of vertices, THE FRAME!
	md2Frame *theFrame = (md2Frame *) ((BYTE*)frames+header.frameSize*frame);

	if(frameName)
		strcpy(frameName, theFrame->name);
	
	////////////////////////////////////////////
	//lock vertex buffer and start putting stuff
	gfxVtx *ptrVtx;

	if(_GFXCheckError(
		(*mesh)->LockVertexBuffer(0, //Flags, nothing special
					   (BYTE**)&ptrVtx	 //If successful, this will point to the data in the VB
					   ), 
					   true,
			"Error in _MD2CreateFrame"))
	{ return RETCODE_FAILURE; }

	int count = 0;

	//go through command and check for 0, which is the end of the frame info
	//we have to do this because we need to be as accurate as possible with
	//the texture coordinates, damn D3D
	while(*command != 0)
	{
		//we don't need to worry about the openGL optimization,
		//because we are using D3D
		int max = *command < 0 ? -(*command) : *command;
		count += max;
		command++;

		md2GlCommandVtx *cVtx = (md2GlCommandVtx *)command;

		for(int i = 0; i < max; i++)
		{
			//if(ptrVtx[cVtx->vertexIndex].s == 0)
				ptrVtx[cVtx->vertexIndex].s = cVtx->s;

			//if(ptrVtx[cVtx->vertexIndex].t == 0)
				ptrVtx[cVtx->vertexIndex].t = cVtx->t;

			ptrVtx[cVtx->vertexIndex].x = (theFrame->vertices[cVtx->vertexIndex].vertex[0]
						* theFrame->scale[0]) + theFrame->translate[0];
			ptrVtx[cVtx->vertexIndex].y = (theFrame->vertices[cVtx->vertexIndex].vertex[1]
						* theFrame->scale[1]) + theFrame->translate[1];
			ptrVtx[cVtx->vertexIndex].z = (theFrame->vertices[cVtx->vertexIndex].vertex[2]
						* theFrame->scale[2]) + theFrame->translate[2];

			ptrVtx[cVtx->vertexIndex].color = 0xffffffff;

			command += 3;
			cVtx = (md2GlCommandVtx *)command;
		}
	}
	
	(*mesh)->UnlockVertexBuffer();
	////////////////////////////////////////////

	/////////////////
	//now fill the index buffer up
	unsigned short *ptrInd;

	if(_GFXCheckError(
			(*mesh)->LockIndexBuffer(0, //Flags, nothing special
					   (BYTE**)&ptrInd     //If successful, this will point to the data in the IB
					   ),          
			true,
			"Error in _MD2CreateFrame"))
	{ return RETCODE_FAILURE; }

	for(int i = 0; i < header.numTriangles; i++)
	{
		*ptrInd = faces[i].vertexIndices[0]; ptrInd++;
		*ptrInd = faces[i].vertexIndices[1]; ptrInd++;
		*ptrInd = faces[i].vertexIndices[2]; ptrInd++;
	}

	(*mesh)->UnlockIndexBuffer();
	/////////////////

	//This outta make lighting easy
	D3DXComputeNormals(*mesh);

	return RETCODE_SUCCESS;
}
#endif

PRIVATE RETCODE _MD2CreateFrame(LPD3DXMESH *mesh, char *frameName, const md2TxtCoord *txtcoords,
								const BYTE *frames, const md2Face *faces,
								const md2Header &header, int frame)
{
	_GFXCheckError(D3DXCreateMeshFVF(
		header.numTriangles,
		header.numVertices,
		D3DXMESH_MANAGED,
		GFXVERTEXFLAG,
		g_p3DDevice,
		mesh),
	true, "Error in _MD2CreateFrame");

	//go to the correct array of vertices, THE FRAME!
	md2Frame *theFrame = (md2Frame *) ((BYTE*)frames+header.frameSize*frame);

	if(frameName)
		strcpy(frameName, theFrame->name);
	
	////////////////////////////////////////////
	//lock vertex buffer and start putting stuff
	gfxVtx *ptrVtx;

	if(_GFXCheckError(
		(*mesh)->LockVertexBuffer(0, //Flags, nothing special
					   (BYTE**)&ptrVtx	 //If successful, this will point to the data in the VB
					   ), 
					   true,
			"Error in _MD2CreateFrame"))
	{ return RETCODE_FAILURE; }

	int count = 0;

	//copy all the frame's vertices
	for(int j = 0; j < header.numVertices; j++)
	{
		ptrVtx[j].x = (theFrame->vertices[j].vertex[0]*theFrame->scale[0])
					  + theFrame->translate[0];
		ptrVtx[j].y = (theFrame->vertices[j].vertex[1]*theFrame->scale[1])
					  + theFrame->translate[1];
		ptrVtx[j].z = (theFrame->vertices[j].vertex[2]*theFrame->scale[2])
					  + theFrame->translate[2];

		ptrVtx[j].color = 0xffffffff;
	}

	//we will not lock the vertices yet...
	////////////////////////////////////////////

	/////////////////
	//now fill the index buffer up
	unsigned short *ptrInd;

	if(_GFXCheckError(
			(*mesh)->LockIndexBuffer(0, //Flags, nothing special
					   (BYTE**)&ptrInd     //If successful, this will point to the data in the IB
					   ),          
			true,
			"Error in _MD2CreateFrame"))
	{ return RETCODE_FAILURE; }

	for(int i = 0, k = 0; k < header.numTriangles; k++, i+=3)
	{
		ptrInd[i] = faces[k].vertexIndices[2];
		ptrInd[i+1] = faces[k].vertexIndices[1];
		ptrInd[i+2] = faces[k].vertexIndices[0];

		ptrVtx[faces[k].vertexIndices[2]].s = ((float)txtcoords[faces[k].textureIndices[2]].s)/header.skinWidth;
		ptrVtx[faces[k].vertexIndices[2]].t = ((float)txtcoords[faces[k].textureIndices[2]].t)/header.skinHeight;
		ptrVtx[faces[k].vertexIndices[1]].s = ((float)txtcoords[faces[k].textureIndices[1]].s)/header.skinWidth;
		ptrVtx[faces[k].vertexIndices[1]].t = ((float)txtcoords[faces[k].textureIndices[1]].t)/header.skinHeight;
		ptrVtx[faces[k].vertexIndices[0]].s = ((float)txtcoords[faces[k].textureIndices[0]].s)/header.skinWidth;
		ptrVtx[faces[k].vertexIndices[0]].t = ((float)txtcoords[faces[k].textureIndices[0]].t)/header.skinHeight;
	}

	(*mesh)->UnlockVertexBuffer();
	(*mesh)->UnlockIndexBuffer();
	/////////////////

	//This outta make lighting easy
	D3DXComputeNormals(*mesh);

	return RETCODE_SUCCESS;
}

//md2 loader
PROTECTED RETCODE _ModelLoadDataFromFileMD2(hMDL model, const char *filename)
{
	assert(model);

	RETCODE ret = RETCODE_SUCCESS;

	md2Header header;			   //the md2 header file
	BYTE            *frames=0;     //the frames (used later to be typecast)
	md2Face         *faces=0;      //3 indices per face
	long			*glCommands=0; //stuff
	md2SkinFile		*skins=0;		   //skin files associated with this model
	md2TxtCoord		*txtcoords=0;  //texture coords in pixel
	int i;

	//ready the file
	FILE *theFile;

	theFile = fopen(filename, "rb");
	if(!theFile)
	{ ASSERT_MSG(0, "Unable to open file", "Error in _ModelLoadDataFromFileMD2"); return RETCODE_FAILURE; }

	//load up the header
	fread(&header, sizeof(md2Header), 1, theFile);

	//confirm format
	if(header.magic != 844121161)
	{ ASSERT_MSG(0, "Bad MD2 magic number, invalid file!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	if(header.version != 8)
	{ ASSERT_MSG(0, "Bad MD2 version number, invalid file!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	////////////////
	//load up frames
	if(MemAlloc((void**)&frames, sizeof(BYTE)*header.frameSize*header.numFrames, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate frames!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetFrames, SEEK_SET);
	fread(frames, sizeof(BYTE)*header.frameSize*header.numFrames, 1, theFile);
	////////////////

	/////////////////////
	//load up gl commands
	if(MemAlloc((void**)&glCommands, sizeof(long)*header.numGlCommands, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate gl Commands!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetGlCommands, SEEK_SET);
	fread(glCommands, header.numGlCommands*sizeof(long), 1, theFile);
	/////////////////////

	///////////////////////////
	//load up triangles (faces)
	if(MemAlloc((void**)&faces, sizeof(md2Face)*header.numTriangles, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate triangles!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetTriangles, SEEK_SET);
	fread(faces, sizeof(md2Face)*header.numTriangles, 1, theFile);
	///////////////////////////

	////////////////////////
	//load up the skin files
	if(header.numSkins > 0)
	{
		if(MemAlloc((void**)&skins, sizeof(md2SkinFile)*header.numSkins, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate skin files!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

		fseek(theFile, header.offsetSkins, SEEK_SET);
		fread(skins, sizeof(md2SkinFile)*header.numSkins, 1, theFile);
	}
	///////////////////////////

	///////////////////////////
	//load up txtcoords
	if(MemAlloc((void**)&txtcoords, sizeof(md2TxtCoord)*header.numTexcoords, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate texture coords!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }

	fseek(theFile, header.offsetTexcoords, SEEK_SET);
	fread(txtcoords, sizeof(md2TxtCoord)*header.numTexcoords, 1, theFile);
	///////////////////////////

	/////////////////////////
	//Get some important info
	model->numMaterial = 1;

	//allocate textures
	if(MemAlloc((void**)&model->textures, sizeof(hTXT)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate model textures!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }
	MemSetPattern(model->textures, "MDLTXT");

	//allocate materials
	if(MemAlloc((void**)&model->materials, sizeof(D3DMATERIAL8)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate model materials!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }
	MemSetPattern(model->materials, "MDLMTR");

	//create mesh for main mesh and insert frame 0
	_MD2CreateFrame(&model->mesh, 0, txtcoords, frames, faces, header, 0);
	model->indCount = model->mesh->GetNumFaces()*NUMPTFACE;

	/////////////////////////////
	//create the adjacency buffer
	//and do some funky stuff
#if 0
	D3DXCreateBuffer(sizeof(DWORD)*model->indCount, &model->adjacencyBuffer);

	model->mesh->GenerateAdjacency(0.0f, (DWORD*)model->adjacencyBuffer->GetBufferPointer());

	D3DXValidMesh(model->mesh, (DWORD*)model->adjacencyBuffer->GetBufferPointer());
#endif
	//////////////////////////////

	//We will only create frames if numframes is greater than 1

	if(header.numFrames > 1)
	{
		model->numFrames = header.numFrames;

		//allocate frame meshes
		if(MemAlloc((void**)&model->frames, sizeof(gfxModelFrame)*model->numFrames, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate model frames!", "Error in _ModelLoadDataFromFileMD2"); ret = RETCODE_FAILURE; goto BADMOJO3; }
		MemSetPattern(model->frames, "MDLFRM");

		//create mesh for each frame
		for(i = 0; i < model->numFrames; i++)
		{
			_MD2CreateFrame(&model->frames[i].frameMesh, model->frames[i].name, 
				txtcoords, frames, faces, header, i);

			/////////////////////////////
			//create the adjacency buffer
			//and do some funky stuff
#if 0
			D3DXCreateBuffer(sizeof(DWORD)*model->indCount, &model->frames[i].adjacencyBuffer);

			model->frames[i].frameMesh->GenerateAdjacency(0.0f, (DWORD*)model->frames[i].adjacencyBuffer->GetBufferPointer());

			D3DXValidMesh(model->frames[i].frameMesh, (DWORD*)model->adjacencyBuffer->GetBufferPointer());
#endif
			//////////////////////////////
		}
	}

BADMOJO3:
	if(frames)
		MemFree((void**)&frames);

	if(glCommands)
		MemFree((void**)&glCommands);

	if(faces)
		MemFree((void**)&faces);

	if(skins)
		MemFree((void**)&skins);

	if(txtcoords)
		MemFree((void**)&txtcoords);

	//set model as CCW backface culling
	//SETFLAG(model->status, MDL_BACKFACECCW);

	fclose(theFile);
	return ret;
}