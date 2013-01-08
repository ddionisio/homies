#include "gdix_particlefx.h"
#include "gdix_particlefx_i.h"
#include "gdix_i.h"

PRIVATE inline void _fxGasInitParticle(const fxGas *pGas, fxGas_par *thisPar, fxGas_vtx *pGasVtx)
{
	pGasVtx->loc[eX]=_GFXMathRand(pGas->minBound[eX], pGas->maxBound[eX]);
	pGasVtx->loc[eY]=_GFXMathRand(pGas->minBound[eY], pGas->maxBound[eY]);
	pGasVtx->loc[eZ]=_GFXMathRand(pGas->minBound[eZ], pGas->maxBound[eZ]);
	pGasVtx->scale = 0;
	pGasVtx->clr = pGas->clr;
	
	thisPar->scaleRate =_GFXMathRand(pGas->minGrowth, pGas->maxGrowth);
	thisPar->bDeath = false;
}

PRIVATE inline void _fxGasUpdateParticle(const fxGas *pGas, fxGas_par *thisPar, fxGas_vtx *pGasVtx)
{
	pGasVtx->loc[eX] += pGas->upVec[eX];
	pGasVtx->loc[eY] += pGas->upVec[eY];
	pGasVtx->loc[eZ] += pGas->upVec[eZ];

	if(!thisPar->bDeath)
	{
		pGasVtx->scale += thisPar->scaleRate;

		if(pGasVtx->scale >= pGas->size)
			thisPar->bDeath = true;
	}
	else
	{
		//fade the particle out
		BYTE a=(pGasVtx->clr&0xff000000)>>24;

		a--;

		if(a==0)
		{
			_fxGasInitParticle(pGas, thisPar, pGasVtx);
		}
		else
			pGasVtx->clr = (pGasVtx->clr&0x00ffffff)|(a<<24);
	}
}

PROTECTED RETCODE fxGasFunc(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	switch(message)
	{
	case PARFXM_CREATE:
		{
			hOBJ obj = OBJQuery(&thisPARFX->objKey);
			fxGas_init *init = (fxGas_init *)dumbParam;

			if(!init) return RETCODE_FAILURE;

			//set up the main particle
			if(MemAlloc(&thisPARFX->data, sizeof(fxGas), M_ZERO) != RETCODE_SUCCESS)
			{ ASSERT_MSG(0, "Unable to allocate gas data", "fxGasFunc"); return RETCODE_FAILURE; }

			fxGas *data = (fxGas *)thisPARFX->data;

			data->gasTxt	  = init->gasTxt; TextureAddRef(data->gasTxt);
			data->clr         = init->clr;
			data->size		  = init->size;
			data->minGrowth	  = init->minGrowth;
			data->maxGrowth	  = init->maxGrowth;
			data->maxParticle = init->maxParticle;
			
			memcpy(data->upVec, init->upVec, sizeof(float)*eMaxPt);
			memcpy(data->minBound, init->minBound, sizeof(float)*eMaxPt);
			memcpy(data->maxBound, init->maxBound, sizeof(float)*eMaxPt);

			//translate the bounds if obj is specified
			if(obj)
			{
				float objLoc[eMaxPt]; OBJGetLoc(obj, objLoc);

				data->minBound[eX] += objLoc[eX];
				data->minBound[eY] += objLoc[eY];
				data->minBound[eZ] += objLoc[eZ];

				data->maxBound[eX] += objLoc[eX];
				data->maxBound[eY] += objLoc[eY];
				data->maxBound[eZ] += objLoc[eZ];
			}

			//allocate the particle gases
			if(MemAlloc((void**)&data->bunchOgas, sizeof(fxGas_par)*data->maxParticle, M_ZERO) != RETCODE_SUCCESS)
			{ ASSERT_MSG(0, "Unable to allocate gas particles", "fxGasFunc"); return RETCODE_FAILURE; }

			//create vertex buffer
			//_GFXCheckError(HRESULT hr, bool displayMsg, const char *header)
			if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(fxGas_vtx)*data->maxParticle,
				D3DUSAGE_POINTS, FXGASVTXFLAG, D3DPOOL_MANAGED, &data->gasVtx), true, "fxGasFunc"))
				return RETCODE_FAILURE;

			//initialize each gas particle
			fxGas_vtx *pGasVtx;

			if(FAILED(data->gasVtx->Lock(0,0, (BYTE**)&pGasVtx, 0)))
			{ ASSERT_MSG(0, "Unable to lock gas particle vtx", "fxGasFunc"); return RETCODE_FAILURE; }

			for(int i = 0; i < data->maxParticle; i++)
				_fxGasInitParticle(data, &data->bunchOgas[i], &pGasVtx[i]);

			data->gasVtx->Unlock();
		}
		break;
	
	case PARFXM_UPDATE:
		{
			fxGas *data = (fxGas *)thisPARFX->data;

			fxGas_vtx *pGasVtx;

			if(FAILED(data->gasVtx->Lock(0,0, (BYTE**)&pGasVtx, 0)))
			{ ASSERT_MSG(0, "Unable to lock gas particle vtx", "fxGasFunc"); return RETCODE_FAILURE; }

			for(int i = 0; i < data->maxParticle; i++)
				_fxGasUpdateParticle(data, &data->bunchOgas[i], &pGasVtx[i]);

			data->gasVtx->Unlock();
		}
		break;

	case PARFXM_DISPLAY:
		{
			//push world stack and set this object's world mtx
			g_pWrldStack->Push();
			g_pWrldStack->LoadIdentity();

			//do transformations
			g_p3DDevice->SetTransform(D3DTS_WORLD, g_pWrldStack->GetTop());

			DWORD srcBlend, destBlend;

			fxGas *data = (fxGas *)thisPARFX->data;

			//disable light
			if(!TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE))
				g_p3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

			TextureSet(data->gasTxt, 0);

			g_p3DDevice->GetRenderState( D3DRS_SRCBLEND, &srcBlend );
			g_p3DDevice->GetRenderState( D3DRS_DESTBLEND, &destBlend );

			g_p3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

			g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			g_p3DDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
			g_p3DDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
			g_p3DDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.08f) );
			g_p3DDevice->SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(1000.0f) );
			g_p3DDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
			g_p3DDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
			g_p3DDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

			g_p3DDevice->SetStreamSource( 0, data->gasVtx, sizeof(fxGas_vtx) );
			g_p3DDevice->SetVertexShader( FXGASVTXFLAG );

			if(FAILED(g_p3DDevice->DrawPrimitive( D3DPT_POINTLIST, 0, data->maxParticle)))
			{ ASSERT_MSG(0, "Failed to draw particles", "fxGasFunc"); return RETCODE_FAILURE; }

			g_p3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			g_p3DDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
			g_p3DDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

			g_p3DDevice->SetRenderState( D3DRS_SRCBLEND, srcBlend );
			g_p3DDevice->SetRenderState( D3DRS_DESTBLEND, destBlend );

			//enable light again, if flag is set
			if(!TESTFLAGS(g_FLAGS, GFX_LIGHTDISABLE))
				g_p3DDevice->SetRenderState(D3DRS_LIGHTING,TRUE);

			TextureSet(0, 0);

			//take this junk out!
			g_pWrldStack->Pop();
		}
		break;

	case PARFXM_DESTROY:
		{
			fxGas *data = (fxGas *)thisPARFX->data;

			if(data)
			{
				if(data->bunchOgas)
					MemFree((void**)&data->bunchOgas);

				TextureDestroy(&data->gasTxt);

				if(data->gasVtx)
					data->gasVtx->Release();
			}
		}
		break;
	}

	return RETCODE_SUCCESS;
}