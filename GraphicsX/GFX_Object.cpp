#include "gdix.h"
#include "gdix_i.h"

//All object creation/destruction/manipulation are done here

PROTECTED void _destroyObj(hOBJ *obj) //this will destroy the object, will not search list
{
	if(*obj)
	{
		GFXPageRemove(&(*obj)->key);

		//go through object's children and clear those out
		if((*obj)->objNode)
		{
			hOBJ thisObj;
			for(LISTPTR::iterator i = (*obj)->objNode->begin(); i != (*obj)->objNode->end(); ++i)
			{
				thisObj = (hOBJ)(*i);
				_destroyObj(&thisObj);
			}

			delete (*obj)->objNode;
		}

		if((*obj)->theMdl)
			MDLDestroy(&(*obj)->theMdl);

		if((*obj)->states)
			MemFree((void**)&(*obj)->states);

		MemFree((void**)obj);
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJCreate(unsigned int newID, hMDL model, float x, float y, float z, 
					  eOBJTYPE objType, const char *animationfile)
{
	assert(model);

	hOBJ newObj;

	if(MemAlloc((void**)&newObj, sizeof(gfxObject), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new object", "Error in OBJCreate"); return 0; }

	MemSetPattern(newObj, "GFXOBJ");

	newObj->ID = newID;

	//load the animation
	char ext[MAXCHARBUFF]; //file extension

	if(animationfile && animationfile[0] != 0)
	{
		strcpy(ext, GetExtension(animationfile));

		if(stricmp(ext, ".anm") == 0) //key frame animation states
		{
			if(_ObjLoadAnimationKey(newObj, animationfile) == RETCODE_FAILURE)
			{ _destroyObj(&newObj); return 0; }
		}
	}
	
	newObj->theMdl = model;

	//if(model->pBound)
	//	newObj->pBound = BoundCreate ( model->pBound );

	newObj->curMesh = model->mesh;

	if(newObj->theMdl) //add reference
		MDLAddRef(newObj->theMdl);

	//initialize matrices
	_GFXMathMtxLoadIden(&newObj->wrldMtx);

	//set object position
	OBJSetLoc(newObj, x, y, z);

	//set scale to default
	OBJSetScale(newObj, 1, 1, 1);

	//initialize list
	newObj->objNode = new LISTPTR;

	//append object to list
	g_OBJLIST.insert(g_OBJLIST.end(), (unsigned int)newObj);
	newObj->listRef = &g_OBJLIST;

	GFXPageAdd(newObj, &newObj->key);

	return newObj;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJAddRef(hOBJ obj)
{
	obj->ref++;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJSearch(unsigned int ID)
{
	/*hOBJ thisObj;

	for(LISTPTR::iterator i = g_OBJLIST.begin(); i != g_OBJLIST.end(); ++i)
	{
		thisObj = (hOBJ)(*i);

		if(thisObj->ID == ID)
			return thisObj;
	}*/

	return (hOBJ)GFXPageGet(ID, 0);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED void _OBJDestroyAll()
{
	hOBJ thisObj;

	for(LISTPTR::iterator i = g_OBJLIST.begin(); i != g_OBJLIST.end(); ++i)
	{
		thisObj = (hOBJ)(*i);
		_destroyObj(&thisObj);
	}

	for(LISTPTR::iterator j = g_OBJDEACTIVATELIST.begin(); j != g_OBJDEACTIVATELIST.end(); ++j)
	{
		thisObj = (hOBJ)(*j);
		_destroyObj(&thisObj);
	}
	

	g_OBJLIST.clear();
	g_OBJDEACTIVATELIST.clear();
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJDestroy(hOBJ *obj)
{
	if(*obj)
	{
		if((*obj)->ref <= 0)
		{
			//just remove the pointer of object from list
			(*obj)->listRef->remove((unsigned int)(*obj));

			_destroyObj(obj);
		}
		else
			(*obj)->ref--;
	}

	obj=0;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJSetLoc(hOBJ obj, float x, float y, float z)
{
	obj->wrldTrans[eX] = x; obj->wrldTrans[eY] = y; obj->wrldTrans[eZ] = z;

	//set the translation matrix
	D3DXMatrixTranslation(&obj->wrldMtx.mtxTrans, 
		obj->wrldTrans[eX], obj->wrldTrans[eY], obj->wrldTrans[eZ]);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxScale,&obj->wrldMtx.mtxRot);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxTrans);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJMovLoc(hOBJ obj, float x, float y, float z)
{
	obj->wrldTrans[eX] += x; obj->wrldTrans[eY] += y; obj->wrldTrans[eZ] += z;

	//set the translation matrix
	D3DXMatrixTranslation(&obj->wrldMtx.mtxTrans, 
		obj->wrldTrans[eX], obj->wrldTrans[eY], obj->wrldTrans[eZ]);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxScale,&obj->wrldMtx.mtxRot);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxTrans);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJRotX(hOBJ obj, float radian)
{
	obj->wrldRot[eX] += radian;

	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationX(&mtxR, radian);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&obj->wrldMtx.mtxRot,&mtxR);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxScale,&obj->wrldMtx.mtxRot);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxTrans);
}

PUBLIC void OBJRotY(hOBJ obj, float radian)
{
	obj->wrldRot[eY] += radian;

	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationY(&mtxR, radian);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&obj->wrldMtx.mtxRot,&mtxR);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxScale,&obj->wrldMtx.mtxRot);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxTrans);
}

PUBLIC void OBJRotZ(hOBJ obj, float radian)
{
	obj->wrldRot[eZ] += radian;

	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationZ(&mtxR, radian);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&obj->wrldMtx.mtxRot,&mtxR);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxScale,&obj->wrldMtx.mtxRot);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxTrans);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJSetScale(hOBJ obj, float sX, float sY, float sZ)
{
	obj->wrldScl[eX] = sX; obj->wrldScl[eY] = sY; obj->wrldScl[eZ] = sZ;

	//set the translation matrix
	D3DXMatrixScaling(&obj->wrldMtx.mtxScale,
		obj->wrldScl[eX], obj->wrldScl[eY], obj->wrldScl[eZ]);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxScale,&obj->wrldMtx.mtxRot);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxTrans);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJAddScale(hOBJ obj, float sX, float sY, float sZ)
{
	obj->wrldScl[eX] += sX; obj->wrldScl[eY] += sY; obj->wrldScl[eZ] += sZ;

	//set the translation matrix
	D3DXMatrixScaling(&obj->wrldMtx.mtxScale,
		obj->wrldScl[eX], obj->wrldScl[eY], obj->wrldScl[eZ]);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxScale,&obj->wrldMtx.mtxRot);
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxTrans);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJDisplay(hOBJ obj)
{
	assert(obj);
	assert(obj->theMdl);

	//set up culling
	if(TESTFLAGS(obj->theMdl->status, MDL_BACKFACECCW))
		g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	else
		g_p3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//Vertex shaders are a complex topic, but you can do some amazing things with them
    //For this example we're not creating one, so we tell Direct3D that we're just
    //using a plain vertex format.
    g_p3DDevice->SetVertexShader(GFXVERTEXFLAG);

	//push world stack and set this object's world mtx
	g_pWrldStack->Push();
	g_pWrldStack->MultMatrix(&obj->wrldMtx.mtxWrld);

	//do transformations
    g_p3DDevice->SetTransform(D3DTS_WORLD, g_pWrldStack->GetTop());

	//draw the darn thing
	//this will do all sorts of pain in the ass for us, which is awesome!
	// Set and draw each of the materials in the mesh
    for( unsigned int i=0; i<obj->theMdl->numMaterial; i++ )
    {
		if(obj->theMdl->materials)
			g_p3DDevice->SetMaterial( &obj->theMdl->materials[i] );

		if(obj->theMdl->textures)
			if(obj->theMdl->textures[i])
				TextureSet(obj->theMdl->textures[i], 0);

        _GFXCheckError(obj->curMesh->DrawSubset(i),
		true, "Error in OBJDisplay");

		g_p3DDevice->SetTexture(0,0);
    }

	//display this object's children within it's world mtx
	for(LISTPTR::iterator j = obj->objNode->begin(); j != obj->objNode->end(); ++j)
		OBJDisplay((hOBJ)(*j));

	//take this junk out!
	g_pWrldStack->Pop();

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
#ifndef INTERPOLATEANIM
PUBLIC RETCODE OBJUpdateFrame(hOBJ obj)
{
	if(!TESTFLAGS(obj->status, OBJ_PAUSE))
	{
		if(obj->states)
		{
			//do the tick count
			if(TimerEllapse(&obj->ticker))
			{
				int & theFrame = obj->states[obj->curState].curFrame;
				theFrame++;

				if(/*theFrame < Sprite->States[Sprite->CurState].firstframe 
					||*/ theFrame > obj->states[obj->curState].endFrame)
				{
					if(TESTFLAGS(obj->status, OBJ_PAUSE_ON_END))
					{ SETFLAG(obj->status, OBJ_PAUSE); theFrame = obj->states[obj->curState].endFrame; }
					else
						theFrame = obj->states[obj->curState].startFrame;
				}

				obj->curMesh = obj->theMdl->frames[theFrame].frameMesh;
			}
		}

		for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
			OBJUpdateFrame((hOBJ)(*i));
	}

	return RETCODE_SUCCESS;
}
#else
PUBLIC RETCODE OBJUpdateFrame(hOBJ obj)
{
	if(!TESTFLAGS(obj->status, OBJ_PAUSE) || obj->curPercentFrame < 1)
	{
		if(obj->states)
		{
			gfxVtx *ptrVtx, *ptrVtx1, *ptrVtx2;

			if(_GFXCheckError(
		obj->curMesh->LockVertexBuffer(0, //Flags, nothing special
					   (BYTE**)&ptrVtx	 //If successful, this will point to the data in the VB
					   ), 
					   true,
			"OBJUpdateFrame")) { return RETCODE_FAILURE; }

			if(_GFXCheckError(
		obj->theMdl->frames[obj->prevFrame].frameMesh->LockVertexBuffer(0, //Flags, nothing special
					   (BYTE**)&ptrVtx1	 //If successful, this will point to the data in the VB
					   ), 
					   true,
			"OBJUpdateFrame")) { return RETCODE_FAILURE; }

			if(_GFXCheckError(
		obj->theMdl->frames[obj->states[obj->curState].curFrame].frameMesh->LockVertexBuffer(0, //Flags, nothing special
					   (BYTE**)&ptrVtx2	 //If successful, this will point to the data in the VB
					   ), 
					   true,
			"OBJUpdateFrame")) { return RETCODE_FAILURE; }

			obj->curPercentFrame += obj->percentFrameAmt;

			for(int i = 0; i < obj->theMdl->mesh->GetNumVertices(); i++)
			{
				ptrVtx[i].x = ptrVtx1[i].x + ((ptrVtx2[i].x-ptrVtx1[i].x)*obj->curPercentFrame);
				ptrVtx[i].y = ptrVtx1[i].y + ((ptrVtx2[i].y-ptrVtx1[i].y)*obj->curPercentFrame);
				ptrVtx[i].z = ptrVtx1[i].z + ((ptrVtx2[i].z-ptrVtx1[i].z)*obj->curPercentFrame);
			}

			obj->curMesh->UnlockVertexBuffer();
			obj->theMdl->frames[obj->prevFrame].frameMesh->UnlockVertexBuffer();
			obj->theMdl->frames[obj->states[obj->curState].curFrame].frameMesh->UnlockVertexBuffer();

			if(obj->curPercentFrame >= 1)
			{
				obj->prevFrame = obj->states[obj->curState].curFrame;
				int & theFrame = obj->states[obj->curState].curFrame;
				theFrame++;

				if(theFrame > obj->states[obj->curState].endFrame)
				{
					if(TESTFLAGS(obj->status, OBJ_PAUSE_ON_END))
					{ SETFLAG(obj->status, OBJ_PAUSE); theFrame = obj->states[obj->curState].endFrame; }
					else
						theFrame = obj->states[obj->curState].startFrame;
				}
				obj->curPercentFrame=0;
			}
		}

		for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
			OBJUpdateFrame((hOBJ)(*i));
	}

	return RETCODE_SUCCESS;
}
#endif

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int OBJGetCurState(hOBJ obj)
{
	return obj->curState;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJSetState(hOBJ obj, int state)
{
	if((state >= obj->numStates) || (state < 0))
		return RETCODE_OUTOFBOUND;

	if(state == obj->curState)
		return RETCODE_SUCCESS;

	obj->curState = state;

	obj->states[obj->curState].curFrame = obj->states[obj->curState].startFrame;

	obj->percentFrameAmt = obj->states[obj->curState].delay;
	obj->curPercentFrame = 0;

	hOBJ thisObj;
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisObj = (hOBJ)(*i);

		if(TESTFLAGS(thisObj->status, OBJ_SYNCSTATE))
			OBJSetState(thisObj, state);
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	OBJIsStateEnd
// Purpose:	checks to see if the state
//			of an object reached the end
//			frame of current state
// Output:	none
// Return:	true if curframe reached end
/////////////////////////////////////
PUBLIC bool OBJIsStateEnd(hOBJ obj)
{
	if(obj->states[obj->curState].curFrame == obj->states[obj->curState].endFrame)
		return true;
	return false;
}

/////////////////////////////////////
// Name:	OBJPause
// Purpose:	pause / resume  object
//			(true)  (false)
// Output:	Obj flag is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJPause(hOBJ obj, bool bPause)
{
	if(bPause)
		SETFLAG(obj->status, OBJ_PAUSE);
	else
		CLEARFLAG(obj->status, OBJ_PAUSE);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJPauseOnStateEnd(hOBJ obj, bool bPause)
{
	if(bPause)
		SETFLAG(obj->status, OBJ_PAUSE_ON_END);
	else
		CLEARFLAG(obj->status, OBJ_PAUSE_ON_END);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJAddChild(hOBJ obj, hOBJ child, bool syncState)
{
	//remove child from it's refered list
	child->listRef->remove((unsigned int)child);

	obj->objNode->insert(obj->objNode->end(), (unsigned int)child);
	child->listRef = obj->objNode;

	OBJAddRef(child);

	//set flags
	SETFLAG(child->status, OBJ_CHILD);

	if(syncState) //will the child synchronize state with parent?
	{
		SETFLAG(child->status, OBJ_SYNCSTATE);
		child->curState = obj->curState;
	}

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE OBJRemoveChild(hOBJ obj, hOBJ child)
{
	//we will assume that the child was added to the given obj
	//before this function.

	OBJDestroy(&child); //this will just subtract the child ref

	if(child) //does it still exist?  It damn better!
	{
		//remove child from it's refered list
		//in this case, it would be obj's children list
		child->listRef->remove((unsigned int)child);

		//put the child back to the global object list
		g_OBJLIST.insert(g_OBJLIST.end(), (unsigned int)child);
		child->listRef = &g_OBJLIST;
	}

	CLEARFLAG(child->status, OBJ_CHILD);
	CLEARFLAG(child->status, OBJ_SYNCSTATE);

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	OBJGetModel
// Purpose:	grabs model of obj.
// Output:	none
// Return:	the model of obj.
/////////////////////////////////////
PUBLIC hMDL OBJGetModel(hOBJ obj)
{
	return obj->theMdl;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void OBJGetWorldMtx(hOBJ obj, float mtx[eMaxMtxElem])
{
	mtx[e_11]=obj->wrldMtx.mtxWrld._11; mtx[e_12]=obj->wrldMtx.mtxWrld._12; mtx[e_13]=obj->wrldMtx.mtxWrld._13; mtx[e_14]=obj->wrldMtx.mtxWrld._14;
    mtx[e_21]=obj->wrldMtx.mtxWrld._21; mtx[e_22]=obj->wrldMtx.mtxWrld._22; mtx[e_23]=obj->wrldMtx.mtxWrld._23; mtx[e_24]=obj->wrldMtx.mtxWrld._24;
    mtx[e_31]=obj->wrldMtx.mtxWrld._31; mtx[e_32]=obj->wrldMtx.mtxWrld._32; mtx[e_33]=obj->wrldMtx.mtxWrld._33; mtx[e_34]=obj->wrldMtx.mtxWrld._34;
    mtx[e_41]=obj->wrldMtx.mtxWrld._41; mtx[e_42]=obj->wrldMtx.mtxWrld._42; mtx[e_43]=obj->wrldMtx.mtxWrld._43; mtx[e_44]=obj->wrldMtx.mtxWrld._44;
}

/////////////////////////////////////
// Name:	OBJGetLoc
// Purpose:	grabs the location of 
//			the obj.  This can either
//			be local/world.
// Output:	loc is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetLoc(hOBJ obj, float loc[eMaxPt])
{
	memcpy(loc, obj->wrldTrans, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	OBJGetTransMtx
// Purpose:	grabs the obj's translation
//			mtx.
// Output:	none
// Return:	the mtx trans
/////////////////////////////////////
PUBLIC void OBJGetTransMtx(hOBJ obj, float mtx[eMaxMtxElem])
{
	mtx[e_11]=obj->wrldMtx.mtxTrans._11; mtx[e_12]=obj->wrldMtx.mtxTrans._12; mtx[e_13]=obj->wrldMtx.mtxTrans._13; mtx[e_14]=obj->wrldMtx.mtxTrans._14;
    mtx[e_21]=obj->wrldMtx.mtxTrans._21; mtx[e_22]=obj->wrldMtx.mtxTrans._22; mtx[e_23]=obj->wrldMtx.mtxTrans._23; mtx[e_24]=obj->wrldMtx.mtxTrans._24;
    mtx[e_31]=obj->wrldMtx.mtxTrans._31; mtx[e_32]=obj->wrldMtx.mtxTrans._32; mtx[e_33]=obj->wrldMtx.mtxTrans._33; mtx[e_34]=obj->wrldMtx.mtxTrans._34;
    mtx[e_41]=obj->wrldMtx.mtxTrans._41; mtx[e_42]=obj->wrldMtx.mtxTrans._42; mtx[e_43]=obj->wrldMtx.mtxTrans._43; mtx[e_44]=obj->wrldMtx.mtxTrans._44;
}

/////////////////////////////////////
// Name:	OBJGetScale
// Purpose:	grabs the scale value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetScale(hOBJ obj, float scl[eMaxPt])
{
	memcpy(scl, obj->wrldScl, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	OBJGetScaleMtx
// Purpose:	grabs the obj's scale
//			mtx.
// Output:	none
// Return:	the mtx scale
/////////////////////////////////////
PUBLIC void OBJGetScaleMtx(hOBJ obj, float mtx[eMaxMtxElem])
{
	mtx[e_11]=obj->wrldMtx.mtxScale._11; mtx[e_12]=obj->wrldMtx.mtxScale._12; mtx[e_13]=obj->wrldMtx.mtxScale._13; mtx[e_14]=obj->wrldMtx.mtxScale._14;
    mtx[e_21]=obj->wrldMtx.mtxScale._21; mtx[e_22]=obj->wrldMtx.mtxScale._22; mtx[e_23]=obj->wrldMtx.mtxScale._23; mtx[e_24]=obj->wrldMtx.mtxScale._24;
    mtx[e_31]=obj->wrldMtx.mtxScale._31; mtx[e_32]=obj->wrldMtx.mtxScale._32; mtx[e_33]=obj->wrldMtx.mtxScale._33; mtx[e_34]=obj->wrldMtx.mtxScale._34;
    mtx[e_41]=obj->wrldMtx.mtxScale._41; mtx[e_42]=obj->wrldMtx.mtxScale._42; mtx[e_43]=obj->wrldMtx.mtxScale._43; mtx[e_44]=obj->wrldMtx.mtxScale._44;
}

/////////////////////////////////////
// Name:	OBJGetRotate
// Purpose:	grabs the rotate value of 
//			the obj.  This can either
//			be local/world.
// Output:	scl is filled
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetRotate(hOBJ obj, float rot[eMaxPt])
{
	memcpy(rot, obj->wrldRot, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	OBJGetRotateMtx
// Purpose:	grabs the obj's rotate
//			mtx.
// Output:	none
// Return:	the mtx scale
/////////////////////////////////////
PUBLIC void OBJGetRotateMtx(hOBJ obj, float mtx[eMaxMtxElem])
{
	mtx[e_11]=obj->wrldMtx.mtxRot._11; mtx[e_12]=obj->wrldMtx.mtxRot._12; mtx[e_13]=obj->wrldMtx.mtxRot._13; mtx[e_14]=obj->wrldMtx.mtxRot._14;
    mtx[e_21]=obj->wrldMtx.mtxRot._21; mtx[e_22]=obj->wrldMtx.mtxRot._22; mtx[e_23]=obj->wrldMtx.mtxRot._23; mtx[e_24]=obj->wrldMtx.mtxRot._24;
    mtx[e_31]=obj->wrldMtx.mtxRot._31; mtx[e_32]=obj->wrldMtx.mtxRot._32; mtx[e_33]=obj->wrldMtx.mtxRot._33; mtx[e_34]=obj->wrldMtx.mtxRot._34;
    mtx[e_41]=obj->wrldMtx.mtxRot._41; mtx[e_42]=obj->wrldMtx.mtxRot._42; mtx[e_43]=obj->wrldMtx.mtxRot._43; mtx[e_44]=obj->wrldMtx.mtxRot._44;
}

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumFace(hOBJ obj)
{
	return obj->curMesh->GetNumFaces();
}

/////////////////////////////////////
// Name:	OBJGetNumFace
// Purpose:	grabs the obj's number of
//			face(triangle)
// Output:	none
// Return:	the number of face
/////////////////////////////////////
PUBLIC int OBJGetNumVtx(hOBJ obj)
{
	return obj->curMesh->GetNumVertices();
}

/////////////////////////////////////
// Name:	OBJGetNumModel	
// Purpose:	This will give you the num
//			of models associated with
//			the obj.  This will go through
//			the children as well.
// Output:	none
// Return:	num model
/////////////////////////////////////
PUBLIC int OBJGetNumModel(hOBJ obj)
{
	int numMdl = 1;

	hOBJ thisObj;
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisObj = (hOBJ)(*i);
		numMdl += OBJGetNumModel(thisObj);
	}

	return numMdl;
}

/////////////////////////////////////
// Name:	OBJGetBounds
// Purpose:	fills up the bounds array
//			with bound info of obj and
//			all it's children.
//			NOTE:  Assumes that bounds
//			is allocated to the number of
//			models associated w/ object
// Output:	bounds is filled
// Return:	count of bounds filled
/////////////////////////////////////
PUBLIC int OBJGetBounds(hOBJ obj, gfxBound *bounds, bool bGet1stOnly)
{
	//get first bound
	*bounds = MDLGetBound(obj->theMdl);

	bounds->radius *= obj->wrldScl[eX];	//assume equal scaling on all axis

	//add offset to object center if object is a child
	if(TESTFLAGS(obj->status, OBJ_CHILD))
	{
		float local[eMaxPt]; OBJGetLoc(obj, local);

		(*bounds).center[eX] += local[eX];
		(*bounds).center[eY] += local[eY];
		(*bounds).center[eZ] += local[eZ];
		
		(*bounds).min.x += local[eX];
		(*bounds).min.y += local[eY];
		(*bounds).min.z += local[eZ];
		
		(*bounds).max.x += local[eX];
		(*bounds).max.y += local[eY];
		(*bounds).max.z += local[eZ];
	}

	if(bGet1stOnly)
		return 1;

	hOBJ thisObj;
	int count = 1;
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisObj = (hOBJ)(*i);
		count += OBJGetBounds(thisObj, bounds + count);
	}

	return count;
}

/////////////////////////////////////
// Name:	OBJIsIntersectRay
// Purpose:	checks to see if the given
//			object is intersecting a ray
// Output:	pFaceIndex, pU, pV, pDist
// Return:	true if intersect
/////////////////////////////////////
PUBLIC bool OBJIsIntersectRay(hOBJ obj, D3DXVECTOR3 *pRayPos, D3DXVECTOR3 *pRayDir)
{
	//push world stack and set this object's world mtx
	g_pWrldStack->Push();
	g_pWrldStack->MultMatrix(&obj->wrldMtx.mtxTrans);

	//check the base model
	D3DXVECTOR3 ctr=obj->theMdl->bound.center;

	//D3DXMATRIX mtxView;
    //g_p3DDevice->GetTransform( D3DTS_VIEW, &mtxView );
	//D3DXVec3TransformCoord(&ctr, &ctr,  &mtxView);

	D3DXVec3TransformCoord(&ctr, &ctr, g_pWrldStack->GetTop());

	if(D3DXSphereBoundProbe(&ctr, obj->theMdl->bound.radius, pRayPos, pRayDir))
	{ g_pWrldStack->Pop(); return true; }

	//check for children
	hOBJ thisObj;
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisObj = (hOBJ)(*i);
		
		if(OBJIsIntersectRay(thisObj, pRayPos, pRayDir))
			return true;
	}

	//take this junk out!
	g_pWrldStack->Pop();

	return false;
}

/////////////////////////////////////
// Name:	OBJFixOrientation
// Purpose:	fixes obj's orientation
// Output:	object rotated to -90
// Return:	none
/////////////////////////////////////
PUBLIC void OBJFixOrientation(hOBJ obj)
{
	if(TESTFLAGS(obj->theMdl->status, MDL_YDEPTH))
		OBJRotX(obj, D3DXToRadian(-90));
}

/////////////////////////////////////
// Name:	OBJDeactivate
// Purpose:	deactivates obj
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void OBJDeactivate(hOBJ obj)
{
	//remove object from previous list
	obj->listRef->remove((unsigned int)obj);

	//add object to deactivated object list
	g_OBJDEACTIVATELIST.insert(g_OBJDEACTIVATELIST.end(), (unsigned int)obj);
	obj->listRef = &g_OBJDEACTIVATELIST;

	SETFLAG(obj->status, OBJ_DEACTIVATE);
}

/////////////////////////////////////
// Name:	OBJIsDeactive
// Purpose:	check to see if given
//			obj is deactive
// Output:	none
// Return:	true if deactive
/////////////////////////////////////
PUBLIC bool OBJIsDeactivate(hOBJ obj)
{
	return TESTFLAGS(obj->status, OBJ_DEACTIVATE);
}

/////////////////////////////////////
// Name:	OBJDeactivate
// Purpose:	activates obj
// Output:	stuff happens
// Return:	none
/////////////////////////////////////
PUBLIC void OBJActivate(hOBJ obj)
{
	if(OBJIsDeactivate(obj))
	{
		//remove object from previous list
		obj->listRef->remove((unsigned int)obj);

		//add object to deactivated object list
		g_OBJLIST.insert(g_OBJLIST.end(), (unsigned int)obj);
		obj->listRef = &g_OBJLIST;

		CLEARFLAG(obj->status, OBJ_DEACTIVATE);
	}
}

/////////////////////////////////////
// Name:	OBJDuplicate
// Purpose:	duplicates a given object
// Output:	stuff happens
// Return:	new clone object
/////////////////////////////////////
PUBLIC hOBJ OBJDuplicate(hOBJ obj, unsigned int newID, float x, float y, float z)
{
	hOBJ newObj;

	if(MemAlloc((void**)&newObj, sizeof(gfxObject), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new object", "Error in OBJDuplicate"); return 0; }

	MemSetPattern(newObj, "GFXOBJ");

	newObj->ID = newID;

	//copy the animation
	newObj->numStates = obj->numStates;

	if(obj->numStates > 0)
	{
		if(MemAlloc((void**)&newObj->states, sizeof(gfxObjMdlState)*obj->numStates, M_ZERO) != RETCODE_SUCCESS)
		{ ASSERT_MSG(0, "Unable to allocate new object", "Error in OBJDuplicate"); _destroyObj(&newObj); return 0; }

		memcpy(newObj->states, obj->states, sizeof(gfxObjMdlState)*obj->numStates);

		TimerInit(&newObj->ticker, obj->ticker.spd, obj->ticker.delay);
	}
	
	//copy model
	if(obj->theMdl)
	{
		newObj->theMdl = obj->theMdl;

		newObj->curMesh = newObj->theMdl->mesh;

		 //add reference
		MDLAddRef(obj->theMdl);
	}

	//initialize matrices
	memcpy(&newObj->wrldMtx, &obj->wrldMtx, sizeof(newObj->wrldMtx));
	memcpy(&newObj->wrldRot, &obj->wrldRot, sizeof(newObj->wrldRot));
	//_GFXMathMtxLoadIden(&newObj->wrldMtx);
	
	//set object position
	OBJSetLoc(newObj, x, y, z);

	//set scale
	OBJSetScale(newObj, obj->wrldScl[eX], obj->wrldScl[eY], obj->wrldScl[eZ]);

	//initialize list
	newObj->objNode = new LISTPTR;

	//append object to list
	g_OBJLIST.insert(g_OBJLIST.end(), (unsigned int)newObj);
	newObj->listRef = &g_OBJLIST;

	//copy children
	hOBJ thisChildObj, newChildObj;
	float loc[eMaxPt];
	for(LISTPTR::iterator i = obj->objNode->begin(); i != obj->objNode->end(); ++i)
	{
		thisChildObj = (hOBJ)(*i);
		OBJGetLoc(thisChildObj, loc);
		newChildObj = OBJDuplicate(thisChildObj, thisChildObj->ID, loc[eX], loc[eY], loc[eZ]);

		if(newChildObj)
			OBJAddChild(newObj, newChildObj, TESTFLAGS(thisChildObj->status, OBJ_SYNCSTATE));
	}

	GFXPageAdd(newObj, &newObj->key);

	return newObj;
}

/////////////////////////////////////
// Name:	OBJSetOrientation
// Purpose:	set the orient of obj
// Output:	orient is done
// Return:	none
/////////////////////////////////////
PUBLIC void OBJSetOrientation(hOBJ obj, float x, float y, float z)
{

	float radians = obj->wrldRot[eY];

	obj->orient.x = x;
	obj->orient.y = y;
	obj->orient.z = z;

	obj->wrldRot[eY] = z != 0 ? atan ( x/z ) : x >= 0 ? (DAVIDPI/2) : -(DAVIDPI/2);

	if ( z > 0 ) obj->wrldRot[eY] += 3.14f;
	D3DXMATRIX mtxR;

	//set the rotation matrix
	D3DXMatrixRotationY( &mtxR, obj->wrldRot[eY] - radians );
	D3DXMatrixMultiply(&obj->wrldMtx.mtxRot,&obj->wrldMtx.mtxRot,&mtxR);

	//set the world matrix
	D3DXMatrixMultiply(&obj->wrldMtx.mtxWrld,&obj->wrldMtx.mtxRot,&obj->wrldMtx.mtxTrans);
}

/////////////////////////////////////
// Name:	OBJGetOrientation
// Purpose:	get the orient of obj
// Output:	the vec is set
// Return:	none
/////////////////////////////////////
PUBLIC void OBJGetOrientation(hOBJ obj, float vec[eMaxPt])
{
	memcpy(vec, (float*)obj->orient, sizeof(float)*eMaxPt);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC unsigned int OBJGetID(hOBJ obj)
{
	return obj->key.ID;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC gfxID OBJGetKey(hOBJ obj)
{
	return obj->key;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hOBJ OBJQuery(const gfxID *key)
{
	return (hOBJ)GFXPageQuery(key);
}