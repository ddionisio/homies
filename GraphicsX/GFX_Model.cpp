#include "gdix.h"
#include "gdix_i.h"

//All model creation/destruction/manipulation are done here

PRIVATE void _destroyMdl(hMDL *model) //this will destroy the model, will not search list
{
	if(*model)
	{
		if((*model)->mesh)
			(*model)->mesh->Release();

		if((*model)->frames)
		{
			for(int i = 0; i < (*model)->numFrames; i++)
			{ 
				if((*model)->frames[i].frameMesh) {(*model)->frames[i].frameMesh->Release();}
				if((*model)->frames[i].adjacencyBuffer) {(*model)->frames[i].adjacencyBuffer->Release();}
			}
			
			MemFree((void**)&(*model)->frames);
		}

		if((*model)->textures)
		{
			for(int i = 0; i < (*model)->numMaterial; i++)
				TextureDestroy(&(*model)->textures[i]);
			
			MemFree((void**)&(*model)->textures);
		}

		if((*model)->adjacencyBuffer)
			(*model)->adjacencyBuffer->Release();

		if((*model)->materials)
			MemFree((void**)&(*model)->materials);

		MemFree((void**)model);
	}
}

PROTECTED void _MDLOptimize(LPD3DXMESH mesh, const LPD3DXBUFFER pAdjacencyBuffer, int numInd, 
						  LPD3DXMESH *optMesh)
{
	HRESULT hr;
	DWORD        *rgdwAdjacencyTemp = 0;
	LPD3DXMESH	 tempMesh;
	DWORD        dw32Bit = mesh->GetOptions() & D3DXMESH_32BIT;

	// allocate a second adjacency buffer to store post attribute sorted adjacency
	if(MemAlloc((void**)&rgdwAdjacencyTemp, sizeof(DWORD)*numInd, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate rgdwAdjacencyTemp", "Error in _MDLOptimize"); goto End; }

    // attribute sort - the un-optimized mesh option
    //          remember the adjacency for the vertex cache optimization
    hr = mesh->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
                                 (DWORD*)pAdjacencyBuffer->GetBufferPointer(),
                                 rgdwAdjacencyTemp, NULL, NULL);
    if( FAILED(hr) )
        goto End;

    // snapshot the attribute sorted mesh, shown as the un-optimized version
    hr = mesh->CloneMeshFVF( dw32Bit|D3DXMESH_MANAGED, mesh->GetFVF(), 
                                      g_p3DDevice, &tempMesh );
    if( FAILED(hr) )
        goto End;

    // actually do the vertex cache optimization
    hr = mesh->OptimizeInplace( D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_VERTEXCACHE,
                                 rgdwAdjacencyTemp,
                                 NULL, NULL, NULL);
    if( FAILED(hr) )
        goto End;

    // snapshot as the optimized mesh
    hr = mesh->CloneMeshFVF( dw32Bit|D3DXMESH_MANAGED, mesh->GetFVF(), 
                                      g_p3DDevice, optMesh );
    if( FAILED(hr) )
        goto End;

End:
	if(rgdwAdjacencyTemp)
		MemFree((void**)&rgdwAdjacencyTemp);
    
	if(tempMesh)
		tempMesh->Release();
}

PRIVATE void _MDLCreateSphereBound(hMDL theMdl)
{
	BYTE *pVtx;
	D3DXVECTOR3 ctr;

	if(SUCCEEDED(theMdl->mesh->LockVertexBuffer(0, &pVtx)))
	{
		D3DXComputeBoundingSphere(pVtx, theMdl->mesh->GetNumVertices(),
			theMdl->mesh->GetFVF(), &ctr, &theMdl->bound.radius);

		memcpy(theMdl->bound.center, (float*)ctr, sizeof(float)*eMaxPt);

		theMdl->mesh->UnlockVertexBuffer();
	}
}

PRIVATE inline void _MDLCenterMesh(float trans[eMaxPt], LPD3DXMESH mesh)
{
	gfxVtx *pVtx;

	if(SUCCEEDED(mesh->LockVertexBuffer(0, (BYTE**)&pVtx)))
	{
		for(int i = 0; i < mesh->GetNumVertices(); i++)
		{
			pVtx[i].x -= trans[eX];
			pVtx[i].y -= trans[eY];
			pVtx[i].z -= trans[eZ];
		}

		mesh->UnlockVertexBuffer();
	}
}

PRIVATE void _MDLCenterModel(hMDL theMdl)
{
	//center vertices in model space

	//set up the base model
	_MDLCenterMesh(theMdl->bound.center, theMdl->mesh);

	//go through the frames and center 'em as well
	for(int i = 0; i < theMdl->numFrames; i++)
		_MDLCenterMesh(theMdl->bound.center, theMdl->frames[i].frameMesh);

	//zero out center since model is centered
	memset(theMdl->bound.center, 0, sizeof(float)*eMaxPt);
}

PRIVATE void _MDLGetBoundBox(hMDL theMdl)
{
	BYTE *pVtx;
	D3DXVECTOR3 ctr;

	if(SUCCEEDED(theMdl->mesh->LockVertexBuffer(0, &pVtx)))
	{
		D3DXComputeBoundingBox(pVtx,
			theMdl->mesh->GetNumVertices(),
			theMdl->mesh->GetFVF(),
			&theMdl->bound.min, &theMdl->bound.max);

		theMdl->mesh->UnlockVertexBuffer();
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hMDL MDLCreate(unsigned int newID, const char *filename)
{
	hMDL newMdl;

	if(MemAlloc((void**)&newMdl, sizeof(gfxModel), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new model", "Error in GFXMdlCreate"); return 0; }

	MemSetPattern(newMdl, "GFXMDL");

	newMdl->ID = newID;

	//load model from file
	char ext[MAXCHARBUFF]; //file extension

	strcpy(ext, GetExtension(filename));

	if(stricmp(ext, ".mdl") == 0) //my version
	{
		if(_ModelLoadDataFromFile(newMdl, filename) == RETCODE_FAILURE)
		{ _destroyMdl(&newMdl); return 0; }
	}
	else if(stricmp(ext, ".ase") == 0) //3ds with texture
	{
		if(_ModelLoadDataFromFileASE(newMdl, filename) == RETCODE_FAILURE)
		{ _destroyMdl(&newMdl); return 0; }
	}
	else if(stricmp(ext, ".X") == 0) //direct X files
	{
		if(_ModelLoadDataFromFileX(newMdl, filename) == RETCODE_FAILURE)
		{ _destroyMdl(&newMdl); return 0; }
	}
	else if(stricmp(ext, ".md2") == 0) //direct X files
	{
		if(_ModelLoadDataFromFileMD2(newMdl, filename) == RETCODE_FAILURE)
		{ _destroyMdl(&newMdl); return 0; }

		SETFLAG(newMdl->status, MDL_YDEPTH);
	}

	////////////////////
	//optimize base mesh
#if 0
	LPD3DXMESH optMesh=0;
	
	_MDLOptimize(newMdl->mesh, newMdl->adjacencyBuffer, newMdl->indCount, &optMesh);

	if(optMesh)
	{
		newMdl->mesh->Release();
		newMdl->mesh = optMesh;
	}

	//go through the frames and optimize 'em!
	for(int i = 0; i < newMdl->numFrames; i++)
	{
		LPD3DXMESH optFMesh=0;

		_MDLOptimize(newMdl->frames[i].frameMesh, 
			newMdl->frames[i].adjacencyBuffer, newMdl->indCount, &optFMesh);

		if(optFMesh)
		{
			newMdl->frames[i].frameMesh->Release();
			newMdl->frames[i].frameMesh = optFMesh;
		}
	}
#endif
	////////////////////

	newMdl->indCount = newMdl->mesh->GetNumFaces()*NUMPTFACE;

	//caliculate the bounding sphere
	//we will only calculate the base mesh
	_MDLCreateSphereBound(newMdl);

	//center and compute plane eq. for model
	if(stricmp(ext, ".X") != 0) //direct X files
	{
		_MDLCenterModel(newMdl);
	}

	//caliculate the bounding box
	//we will only calculate the base mesh
	_MDLGetBoundBox(newMdl);

	//append to list
	g_MDLLIST.insert(g_MDLLIST.end(), (unsigned int)newMdl);

	return newMdl;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLAddRef(hMDL model)
{
	model->ref++;
}

/////////////////////////////////////
// Name:	MDLSetListFile
// Purpose:	sets the model list file
// Output:	the model list file is set
// Return:	none
/////////////////////////////////////
PUBLIC void MDLSetListFile(const char *filename)
{
	//just copy it
	strcpy(g_MDLLISTFILE, filename);
}

PRIVATE hMDL _MDLSearchList(unsigned int ID)
{
	hMDL thisMdl;

	for(LISTPTR::iterator i = g_MDLLIST.begin(); i != g_MDLLIST.end(); ++i)
	{
		unsigned int ptr = *i;
		thisMdl = (hMDL)ptr;

		if(thisMdl->ID == ID)
			return thisMdl;
	}

	return 0;
}

/////////////////////////////////////
// Name:	MDLSearch
// Purpose:	load model via list file 
//			with ID.  You must give the
//			texture list file as well...
// Output:	model is added if not loaded
//			yet
// Return:	the model if ID is found
/////////////////////////////////////
PUBLIC hMDL MDLSearch(unsigned int ID)
{
	hMDL model=_MDLSearchList(ID); //check to see if it exist on the list...

	if(!model)
	{
		char mdlFName[MAXCHARBUFF], useMat;
		unsigned int mdlID;
		//unsigned int bndID;
		int txtID;
		D3DMATERIAL8 mat={0};

		//load up file
		FILE *fp  = fopen(g_MDLLISTFILE, "rt");

		if(!fp)
			return 0;

		while(!feof(fp))
		{
			fscanf(fp, "%d %s img=%d material:%c", &mdlID, mdlFName, &txtID, &useMat);

			if(useMat == 'Y')
				fscanf(fp, " D:%f,%f,%f,%f A:%f,%f,%f,%f S:%f,%f,%f,%f E:%f,%f,%f,%f P:%f",
				&mat.Specular.r,&mat.Specular.g,&mat.Specular.b,&mat.Specular.a,
				&mat.Ambient.r,&mat.Ambient.g,&mat.Ambient.b,&mat.Ambient.a,
				&mat.Specular.r,&mat.Specular.g,&mat.Specular.b,&mat.Specular.a,
				&mat.Emissive.r,&mat.Emissive.g,&mat.Emissive.b,&mat.Emissive.a,
				&mat.Power);
			else
				fscanf(fp, "\n");

			if(mdlID == ID) //does it match???
			{
				//load this puppy up!
				model = MDLCreate(ID, mdlFName);

				if(model)
				{
					if(txtID >= 0) //set texture if ID > 0
						MDLSetTexture(model, 0, TextureSearch(txtID));

//					if ( bndID >= 0 )
						//model->pBound = BoundSearch ( bndID );

					if(useMat == 'Y') //set material if useMat == 'Y'
						MDLSetMaterial(model, 0, mat);
				}

				fclose ( fp );

				break;
			}
		}
	}
	else
		MDLAddRef(model);

	return model;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _MDLDestroyAll()
{
	hMDL thisMdl;

	for(LISTPTR::iterator i = g_MDLLIST.begin(); i != g_MDLLIST.end(); ++i)
	{
		thisMdl = (hMDL)(*i);
		_destroyMdl(&thisMdl);
	}

	g_MDLLIST.clear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void MDLDestroy(hMDL *model)
{
	if(*model)
	{
		if((*model)->ref <= 0)
		{
			//just remove the pointer of model from list
			g_MDLLIST.remove((unsigned int)(*model));

			_destroyMdl(model);
		}
		else
			(*model)->ref--;
	}

	model=0;
}

/////////////////////////////////////
// Name:	MDLGetTexture
// Purpose:	get model's
//			texture within material
//			index
// Output:	none
// Return:	the texture
/////////////////////////////////////
PUBLIC hTXT MDLGetTexture(hMDL model, int materialInd)
{
	if(materialInd < model->numMaterial)
		return model->textures[materialInd];
	return 0;
}

/////////////////////////////////////
// Name:	MDLSetTexture
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetTexture(hMDL model, int materialInd, hTXT texture)
{
	//allocate textures if there is none
	//set it automatically to one
	if(model->numMaterial == 0)
	{
		model->numMaterial = 1;
		if(MemAlloc((void**)&model->textures, sizeof(hTXT)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate model textures", "Error in MDLSetTexture"); return RETCODE_FAILURE; }
		MemSetPattern(model->textures, "MDLTXT");

		model->textures[0] = texture;
		TextureAddRef(model->textures[0]);
	}
	else
	{
		if(materialInd >= model->numMaterial)
		{ ASSERT_MSG(0, "Bad material index", "Error in MDLSetTexture"); return RETCODE_FAILURE; }

		TextureDestroy(&model->textures[materialInd]);

		model->textures[materialInd] = texture;
		TextureAddRef(model->textures[materialInd]);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MDLSetMaterial
// Purpose:	sets/change model's
//			texture within material
//			index
// Output:	model's texture change
// Return:	RETCODE_SUCCESS if success
/////////////////////////////////////
PUBLIC RETCODE MDLSetMaterial(hMDL model, int materialInd, const D3DMATERIAL8 &material)
{
	//allocate materials if there is none
	//set it automatically to one
	if(model->numMaterial == 0)
	{
		model->numMaterial = 1;
		if(MemAlloc((void**)&model->materials, sizeof(D3DMATERIAL8)*model->numMaterial, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate model materials", "Error in MDLSetMaterial"); return RETCODE_FAILURE; }
		MemSetPattern(model->materials, "MDLMTR");

		memcpy(&model->materials[0], &material, sizeof(D3DMATERIAL8));
	}
	else
	{
		if(materialInd >= model->numMaterial)
		{ ASSERT_MSG(0, "Bad material index", "Error in MDLSetMaterial"); return RETCODE_FAILURE; }

		memcpy(&model->materials[materialInd], &material, sizeof(D3DMATERIAL8));
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	MDLGetBound
// Purpose:	grabs model's bound info
// Output:	none
// Return:	the model's bound
/////////////////////////////////////
PUBLIC gfxBound MDLGetBound(hMDL model)
{
	return model->bound;
}

/////////////////////////////////////
// Name:	MDLGetNumVtx
// Purpose:	grabs the number of vtx
//			the model has
// Output:	none
// Return:	number of vtx
/////////////////////////////////////
PUBLIC DWORD MDLGetNumVtx(hMDL model)
{
	return model->mesh->GetNumVertices();
}

/////////////////////////////////////
// Name:	MDLGetVtx
// Purpose:	grabs the vtx of model with
//			given number.
// Output:	none
// Return:	number of vtx copied
/////////////////////////////////////
PUBLIC DWORD MDLGetVtx(hMDL model, float *vtx[eMaxPt], int numVtx)
{
	memset(vtx, 0, sizeof(float)*numVtx*eMaxPt);

	DWORD max = 0;

	gfxVtx *pVtx;

	if(SUCCEEDED(model->mesh->LockVertexBuffer(D3DLOCK_READONLY, (BYTE**)&pVtx)))
	{
		DWORD numMdlVtx = model->mesh->GetNumVertices();
		max = numMdlVtx < numVtx ? numMdlVtx : numVtx;

		for(int i = 0; i < max; i++)
			memcpy(vtx[i], &pVtx[i], sizeof(float)*eMaxPt);

		model->mesh->UnlockVertexBuffer();
	}

	return max;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int MDLGetID(hMDL model)
{
	return model->ID;
}

/////////////////////////////////////
// Name:	MDLSaveToXFile
// Purpose:	this is used for cheating in
//			GEN300, good for terrain!
// Output:	file is created
// Return:	none
/////////////////////////////////////
PUBLIC void MDLSaveToXFile(hMDL model, char *filename)
{
	D3DXMATERIAL *d3dxmats;

	MemAlloc((void**)&d3dxmats, sizeof(D3DXMATERIAL)*model->numMaterial, M_ZERO);

	for(int i = 0; i < model->numMaterial; i++)
	{
		memcpy(&d3dxmats[i].MatD3D, &model->materials[i], sizeof(D3DMATERIAL8));

		if(model->textures[i])
			d3dxmats[i].pTextureFilename = model->textures[i]->filename;
	}

	D3DXSaveMeshToX(
	  filename,
	  model->mesh,
	  (DWORD*)model->adjacencyBuffer->GetBufferPointer(),
	  d3dxmats,
	  model->numMaterial,
	  DXFILEFORMAT_BINARY);

	MemFree((void**)&d3dxmats);
}