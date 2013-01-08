#include "homies.h"
#include "homies_i.h"

PROTECTED RETCODE _GameCfgLoad(const char *filename, gameCfg *pgCfg)
{
	FILE *fp = fopen(filename, "rt");
	char buff[MAXCHARBUFF];

	if(!fp)
		return RETCODE_FAILURE;

	while(!feof(fp))
	{
		fscanf(fp, "%s\n", buff);

		if(stricmp(buff, "[MEMORY]") == 0)
		{
			fscanf(fp, "poolSize=%d\n", &pgCfg->config.PoolSize);
		}
		else if(stricmp(buff, "[GRAPHICS]") == 0)
		{
			int bpp, fullscrn;

			fscanf(fp, "fullScreen=%d\n", &fullscrn);

			if(fullscrn)
				SETFLAG(pgCfg->status, CFG_FULLSCREEN);

			fscanf(fp, "vidMode=%dx%dx%dx%d\n", &pgCfg->gfxMode.width,
				&pgCfg->gfxMode.height, &bpp, &pgCfg->gfxMode.refresh);

			switch(bpp)
			{
			case 32:
				pgCfg->gfxMode.bpp = BPP_32; break;
			case 24:
				pgCfg->gfxMode.bpp = BPP_24; break;
			case 16:
				pgCfg->gfxMode.bpp = BPP_16; break;
			case 8:
				pgCfg->gfxMode.bpp = BPP_8; break;
			}
		}
		else if(stricmp(buff, "[AUDIO]") == 0)
		{
			fscanf(fp, "soundVol=%d\n", &pgCfg->sVol);
			fscanf(fp, "musicVol=%d\n", &pgCfg->aVol);
		}
		else if(stricmp(buff, "[GAME]") == 0)
		{
			fscanf(fp, "gameDelay=%d\n", &pgCfg->delayGame);
			fscanf(fp, "frameDelay=%d\n", &pgCfg->delayFrame);
			fscanf(fp, "homieSpd=%f\n", &pgCfg->homieSpd);
		}
	}

	fclose(fp);

	return RETCODE_SUCCESS;
}

//save game cfg
PROTECTED RETCODE _GameCfgSave(const char *filename, gameCfg *pgCfg)
{
	FILE *fp = fopen(filename, "wt");

	if(!fp)
		return RETCODE_FAILURE;

	fprintf(fp, "[MEMORY]\n");
	fprintf(fp, "poolSize=%d\n", pgCfg->config.PoolSize);

	fprintf(fp, "[GRAPHICS]\n");

	if(TESTFLAGS(pgCfg->status,CFG_FULLSCREEN))
		fprintf(fp, "fullScreen=1\n");
	else
		fprintf(fp, "fullScreen=0\n");

	int bpp;

	switch(pgCfg->gfxMode.bpp)
	{
	case BPP_32:
		bpp = 32; break;
	case BPP_24:
		bpp = 24; break;
	case BPP_16:
		bpp = 16; break;
	case BPP_8:
		bpp = 8; break;
	}

	fprintf(fp, "vidMode=%dx%dx%dx%d\n", pgCfg->gfxMode.width,
			pgCfg->gfxMode.height, bpp, pgCfg->gfxMode.refresh);

	fprintf(fp, "[AUDIO]\n");
	
	fprintf(fp, "soundVol=%d\n", pgCfg->sVol);
	fprintf(fp, "musicVol=%d\n", pgCfg->aVol);

	fprintf(fp, "[GAME]\n");

	fprintf(fp, "gameDelay=%d\n", pgCfg->delayGame);
	fprintf(fp, "frameDelay=%d\n", pgCfg->delayFrame);
	fprintf(fp, "homieSpd=%f\n", pgCfg->homieSpd);

	fclose(fp);

	return RETCODE_SUCCESS;
}