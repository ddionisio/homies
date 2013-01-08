#include "gdix.h"
#include "gdix_i.h"

//All sprite creation/destruction/manipulation are done here

PRIVATE void _destroySpr(hSPRITE *spr)
{
	if(*spr)
	{
		if((*spr)->frameRects)
			MemFree((void**)&(*spr)->frameRects);
		if((*spr)->states)
			MemFree((void**)&(*spr)->states);

		TextureDestroy(&(*spr)->theTexture);

		MemFree((void**)spr);
	}
}

/////////////////////////////////////
// Name:	SpriteCreate
// Purpose:	creates a sprite with txt.
//			NOTE: states has to be allocated with
//				  size numState.
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
PUBLIC hSPRITE SpriteCreate(hTXT txt, int numFrame, SIZE frameSize, 
							unsigned int delay, int numImgCol, LOCATION *imgOffset,
							int numState, const gfxSprFrame *states)
{
	assert(txt);

	hSPRITE newSpr;

	if(MemAlloc((void**)&newSpr, sizeof(gfxSprite), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new sprite", "Error in SpriteCreate"); return 0; }

	MemSetPattern(newSpr, "GFXSPR");

	//assign stuff
	newSpr->theTexture = txt; TextureAddRef(txt);
	
	newSpr->numFrames = numFrame;
	newSpr->frameSize = frameSize;

	if(numImgCol > 0)
		newSpr->numImgCol = numImgCol;
	else
		newSpr->numImgCol = TextureGetWidth(newSpr->theTexture)/newSpr->frameSize.cx;
	
	if(imgOffset)
		newSpr->imgOffset = *imgOffset;
	
	newSpr->numStates = numState;

	//init timer
	TimerInit(&newSpr->ticker, MILLISECOND, delay);

	/////////////////////////////////////////////////
	//create the frame rects
	if(MemAlloc((void**)&newSpr->frameRects, sizeof(RECT)*newSpr->numFrames, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate frame rects", "Error in SpriteCreate"); _destroySpr(&newSpr); return 0; }

	MemSetPattern(newSpr->frameRects, "SPRFRM");

	//pre-CALIculate the rects
	for(int i = 0; i < newSpr->numFrames; i++)
	{
		newSpr->frameRects[i].left   = i % newSpr->numImgCol * newSpr->frameSize.cx + newSpr->imgOffset.x;
		newSpr->frameRects[i].top    = i / newSpr->numImgCol * newSpr->frameSize.cy + newSpr->imgOffset.y; //offset
		newSpr->frameRects[i].right  = newSpr->frameRects[i].left + newSpr->frameSize.cx; //plus the size of frame
		newSpr->frameRects[i].bottom = newSpr->frameRects[i].top + newSpr->frameSize.cy; //plus size of frame
	}
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	//create states
	if(MemAlloc((void**)&newSpr->states, sizeof(gfxSprFrame)*newSpr->numStates, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate states", "Error in SpriteCreate"); _destroySpr(&newSpr); return 0; }

	MemSetPattern(newSpr->states, "SPRST");

	//just copy it
	memcpy(newSpr->states, states, sizeof(gfxSprFrame)*newSpr->numStates);
	/////////////////////////////////////////////////

	return newSpr;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC hSPRITE SpriteLoad(hTXT txt, int numImageCol, 
							   LOCATION *imgOffset, const char *filename)
{
	SIZE frameSize;
	int delay, numFrames, numStates;
	gfxSprFrame *states;

	//let's load up some stuff
	FILE *theFile = fopen(filename, "rt");

	if(!theFile)
	{ ASSERT_MSG(0, "Unable to open sprite file", "Error in SpriteLoad"); return 0; }

	//get the frame size
	fscanf(theFile, "framesize x=%d y=%d\n", &frameSize.cx, &frameSize.cy);

	//get the animation delay
	fscanf(theFile, "aniDelay=%d\n", &delay);

	//get number of frames
	fscanf(theFile, "numFrames=%d\n", &numFrames);

	//get the number of states
	fscanf(theFile, "numStates=%d\n", &numStates);

	//now let's allocate the states
	if(MemAlloc((void**)&states, sizeof(gfxSprFrame)*numStates, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate states", "Error in SpriteLoad"); fclose(theFile); return 0; }

	//now fill up the states
	for(int i = 0; i < numStates; i++)
		fscanf(theFile, "State first=%d last=%d\n", &states[i].firstframe, &states[i].lastframe);

	//create the sprite!
	hSPRITE newSprite = SpriteCreate(txt, numFrames, frameSize, delay, numImageCol, 
		imgOffset, numStates, states);

	//we don't need states anymore
	MemFree((void**)&states);

	//we don't need the file
	fclose(theFile);

	if(!newSprite)
		ASSERT_MSG(0, "Unable to create sprites", "Error in SpriteLoad");

	return newSprite;
}

/////////////////////////////////////
// Name:	SpriteDestroy
// Purpose:	destroys sprite
// Output:	none
// Return:	none
/////////////////////////////////////
PUBLIC void SpriteDestroy(hSPRITE *spr)
{
	_destroySpr(spr);
	spr=0;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void SpriteUpdateFrame(hSPRITE spr)
{
	assert(spr);

	//do the tick count
	if(TimerEllapse(&spr->ticker))
	{
		int & theFrame = spr->states[spr->curState].curFrame;
		theFrame++;

		if(/*theFrame < Sprite->States[Sprite->CurState].firstframe 
			||*/ theFrame > spr->states[spr->curState].lastframe)
			theFrame = spr->states[spr->curState].firstframe;
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC void SpriteUpdateFrameAllState(hSPRITE spr)
{
	assert(spr);

	//do the tick count
	if(TimerEllapse(&spr->ticker))
	{
		for(int i = 0; i < spr->numStates; i++)
		{
			int & theFrame = spr->states[i].curFrame;
			theFrame++;

			if(/*theFrame < Sprite->States[Sprite->CurState].firstframe 
				||*/ theFrame > spr->states[i].lastframe)
				theFrame = spr->states[i].firstframe;
		}
	}
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int SpriteGetMaxState(hSPRITE spr)
{
	return spr->numStates;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC int SpriteGetCurState(hSPRITE spr)
{
	return spr->curState;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE SpriteSetState(hSPRITE spr, int state)
{
	assert(spr);

	if((state >= SpriteGetMaxState(spr)) || (state < 0))
		return RETCODE_OUTOFBOUND;

	spr->curState = state;

	spr->states[spr->curState].curFrame = spr->states[spr->curState].firstframe;

	return RETCODE_SUCCESS;
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC SIZE SpriteGetSize(hSPRITE spr)
{
	return spr->frameSize;
}

/////////////////////////////////////
// Name:	SpriteBlt
// Purpose:	displays sprite
//			NOTE: GFXBltModeEnable()
//				  must be called first
// Output:	sprite is displayed
// Return:	RETCODE_SUCCESS
/////////////////////////////////////
PUBLIC RETCODE SpriteBlt(hSPRITE spr, const float loc[eMaxPt], const unsigned int *color, float rotate)
{
	return TextureBlt(spr->theTexture, 
		loc[eX], loc[eY], 
		&spr->frameRects[spr->states[spr->curState].curFrame], color, rotate);
}

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PUBLIC RETCODE SpriteStretchBlt(hSPRITE spr, const float loc[eMaxPt],
								float w, float h, const unsigned int *color, float rotate)
{
	return TextureStretchBlt(spr->theTexture, loc[eX], loc[eY], w, h,
		&spr->frameRects[spr->states[spr->curState].curFrame], color, rotate);
}

/////////////////////////////////////
// Name:	SpriteBlt3D
// Purpose:	displays sprite
//			NOTE: GFXBegin()
//				  must be called first
//			This will also set cull mode
//			to none
// Output:	sprite is displayed in 3d
// Return:	RETCODE_SUCCESS
/////////////////////////////////////
PUBLIC RETCODE SpriteBlt3D(hSPRITE spr, const D3DXMATRIX *wrldMtx, 
						   const D3DMATERIAL8 *material,
						   const unsigned int *color)
{
	return TextureBlt3D(spr->theTexture, wrldMtx, 
		&spr->frameRects[spr->states[spr->curState].curFrame], material, color);
}

/////////////////////////////////////
// Name:	SpriteDuplicate
// Purpose:	duplicate a sprite with
//			given sprite
// Output:	none
// Return:	new sprite created
/////////////////////////////////////
PUBLIC hSPRITE SpriteDuplicate(hSPRITE spr)
{
	hSPRITE newSpr;

	if(MemAlloc((void**)&newSpr, sizeof(gfxSprite), M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate new sprite", "Error in SpriteCreate"); return 0; }

	MemSetPattern(newSpr, "GFXDSPR");

	//assign stuff
	newSpr->theTexture = spr->theTexture; TextureAddRef(spr->theTexture);
	
	newSpr->numFrames = spr->numFrames;
	newSpr->frameSize = spr->frameSize;

	newSpr->numImgCol = spr->numImgCol;
	
	newSpr->imgOffset = spr->imgOffset;
	
	newSpr->numStates = spr->numStates;

	//init timer
	TimerInit(&newSpr->ticker, MILLISECOND, spr->ticker.delay);

	/////////////////////////////////////////////////
	//create the frame rects
	if(MemAlloc((void**)&newSpr->frameRects, sizeof(RECT)*newSpr->numFrames, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate frame rects", "Error in SpriteCreate"); _destroySpr(&newSpr); return 0; }

	MemSetPattern(newSpr->frameRects, "DSPRFRM");

	//pre-CALIculate the rects
	for(int i = 0; i < newSpr->numFrames; i++)
	{
		newSpr->frameRects[i].left   = i % newSpr->numImgCol * newSpr->frameSize.cx + newSpr->imgOffset.x;
		newSpr->frameRects[i].top    = i / newSpr->numImgCol * newSpr->frameSize.cy + newSpr->imgOffset.y; //offset
		newSpr->frameRects[i].right  = newSpr->frameRects[i].left + newSpr->frameSize.cx; //plus the size of frame
		newSpr->frameRects[i].bottom = newSpr->frameRects[i].top + newSpr->frameSize.cy; //plus size of frame
	}
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	//create states
	if(MemAlloc((void**)&newSpr->states, sizeof(gfxSprFrame)*newSpr->numStates, M_ZERO) != RETCODE_SUCCESS)
	{ ASSERT_MSG(0, "Unable to allocate states", "Error in SpriteCreate"); _destroySpr(&newSpr); return 0; }

	MemSetPattern(newSpr->states, "DSPRST");

	//just copy it
	memcpy(newSpr->states, spr->states, sizeof(gfxSprFrame)*newSpr->numStates);
	/////////////////////////////////////////////////

	return newSpr;
}
