#include "gdix_particlefx.h"
#include "gdix_particlefx_i.h"
#include "gdix_i.h"

PRIVATE inline void _fxGas2InitParticle(const fxGas2_init *pGasInit, 
										const float center[eMaxPt],
										fxGas2_par *thisPar)
{
	thisPar->curVtx.loc[eX]=center[eX] + pGasInit->radius*_GFXMathRand(-pGasInit->normArea[eX], pGasInit->normArea[eX]);
	thisPar->curVtx.loc[eY]=center[eY] + pGasInit->radius*_GFXMathRand(-pGasInit->normArea[eY], pGasInit->normArea[eY]);
	thisPar->curVtx.loc[eZ]=center[eZ] + pGasInit->radius*_GFXMathRand(-pGasInit->normArea[eZ], pGasInit->normArea[eZ]);
	thisPar->curVtx.scale = 0;
	thisPar->curVtx.clr = pGasInit->clr;

	thisPar->vel[eX]=_GFXMathRand(pGasInit->minVel[eX], pGasInit->minVel[eX]);
	thisPar->vel[eY]=_GFXMathRand(pGasInit->minVel[eY], pGasInit->minVel[eY]);
	thisPar->vel[eZ]=_GFXMathRand(pGasInit->minVel[eZ], pGasInit->minVel[eZ]);
	
	thisPar->scaleRate =_GFXMathRand(pGasInit->minGrowth, pGasInit->maxGrowth);
	thisPar->bDeath = false;
}

PRIVATE inline bool _fxGas2UpdateParticle(fxGas2_par *thisPar, float maxSize)
{
	thisPar->curVtx.loc[eX] += thisPar->vel[eX];
	thisPar->curVtx.loc[eY] += thisPar->vel[eY];
	thisPar->curVtx.loc[eZ] += thisPar->vel[eZ];

	if(!thisPar->bDeath)
	{
		thisPar->curVtx.scale += thisPar->scaleRate;

		if(thisPar->curVtx.scale >= maxSize)
			thisPar->bDeath = true;
	}
	else
	{
		//fade the particle out
		BYTE a=(thisPar->curVtx.clr&0xff000000)>>24;

		a--;

		if(a==0)
			return false;
		else
			thisPar->curVtx.clr = (thisPar->curVtx.clr&0x00ffffff)|(a<<24);
	}

	return true;
}

PROTECTED RETCODE fxGas2Func(hPARFX thisPARFX, DWORD message, LPARAM dumbParam, WPARAM otherParam)
{
	switch(message)
	{
	case PARFXM_CREATE:
		{
			float center[eMaxPt];
			hOBJ obj = OBJQuery(&thisPARFX->objKey);
			fxGas2_init *init = (fxGas2_init *)dumbParam;

			if(!init) return RETCODE_FAILURE;

			//set up the main particle
			if(MemAlloc(&thisPARFX->data, sizeof(fxGas2), M_ZERO) != RETCODE_SUCCESS)
			{ ASSERT_MSG(0, "Unable to allocate gas data", "fxGas2Func"); return RETCODE_FAILURE; }

			fxGas2 *data = (fxGas2 *)thisPARFX->data;

			data->gasTxt	  = init->gasTxt; TextureAddRef(data->gasTxt);
			data->size		  = init->size;
			data->maxParticle = init->maxParticle;

			//translate the bounds if obj is specified
			if(obj)
				OBJGetLoc(obj, center);
			else
				memcpy(center, init->center, sizeof(float)*eMaxPt);

			//allocate the particle gases
			if(MemAlloc((void**)&data->bunchOgas, sizeof(fxGas2_par)*data->maxParticle, M_ZERO) != RETCODE_SUCCESS)
			{ ASSERT_MSG(0, "Unable to allocate gas particles", "fxGas2Func"); return RETCODE_FAILURE; }

			//create vertex buffer
			//_GFXCheckError(HRESULT hr, bool displayMsg, const char *header)
			if(_GFXCheckError(g_p3DDevice->CreateVertexBuffer(sizeof(fxGas_vtx)*data->maxParticle,
				D3DUSAGE_POINTS, FXGASVTXFLAG, D3DPOOL_MANAGED, &data->gasVtx), true, "fxGas2Func"))
				return RETCODE_FAILURE;

			//initialize each gas particle
			for(int i = 0; i < data->maxParticle; i++)
				_fxGas2InitParticle(init, center, &data->bunchOgas[i]);
		}
		break;
	
	case PARFXM_UPDATE:
		{
			fxGas2 *data = (fxGas2 *)thisPARFX->data;

			fxGas_vtx *pGasVtx;

			if(FAILED(data->gasVtx->Lock(0,0, (BYTE**)&pGasVtx, 0)))
			{ ASSERT_MSG(0, "Unable to lock gas particle vtx", "fxGas2Func"); return RETCODE_FAILURE; }

			data->numAlive=0;

			for(int i = 0; i < data->maxParticle; i++)
			{
				if(_fxGas2UpdateParticle(&data->bunchOgas[i], data->size))
				{
					memcpy(&pGasVtx[i], &data->bunchOgas[i].curVtx, sizeof(fxGas_vtx));
					data->numAlive++;
				}
			}

			data->gasVtx->Unlock();

			if(data->numAlive==0)
				return RETCODE_BREAK;
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

			fxGas2 *data = (fxGas2 *)thisPARFX->data;

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

			if(FAILED(g_p3DDevice->DrawPrimitive( D3DPT_POINTLIST, 0, data->numAlive)))
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
			fxGas2 *data = (fxGas2 *)thisPARFX->data;

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