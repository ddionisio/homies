#include "homies.h"
#include "homies_i.h"

#include "resource.h"

#define MAXBIT	2

static const char *g_bitName[MAXBIT]={"16-bit", "32-bit"};

static gameCfg g_gameCfg={0};


//
// Dialog Procs.
//
BOOL CALLBACK HomieOptDlgProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LONG index;
	char buff[MAXCHARBUFF]={0};
	BOOL trans;

	switch(message)
	{
	case WM_INITDIALOG:		//if we're starting up...
		{
			_GameCfgLoad(GAMECFG, &g_gameCfg);

			eBPP bit=BPP_16;

			index = SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_ADDSTRING, 0, (LONG)(LPSTR)g_bitName[0]);	
			SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_SETITEMDATA, index, ( LPARAM )bit);

			bit=BPP_32;
			index = SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_ADDSTRING, 0, (LONG)(LPSTR)g_bitName[1]);
			SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_SETITEMDATA, index, ( LPARAM )bit);

			switch(g_gameCfg.gfxMode.bpp)
			{
			case BPP_16:
				SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_SELECTSTRING, 1, (LONG)(LPSTR)g_bitName[0]);
				break;
			case BPP_32:
				SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_SELECTSTRING, 1, (LONG)(LPSTR)g_bitName[1]);
				break;
			}

			SetDlgItemInt(hwnd, ID_SOUND_VOL, g_gameCfg.sVol, FALSE);
			SetDlgItemInt(hwnd, ID_MUSIC_VOL, g_gameCfg.aVol, FALSE);

			if(TESTFLAGS(g_gameCfg.status, CFG_FULLSCREEN))
				CheckDlgButton(hwnd, ID_FULLSCRN, BST_CHECKED);
			else
				CheckDlgButton(hwnd, ID_FULLSCRN, BST_UNCHECKED);

			ShowWindow(hwnd,SW_SHOW);
			return FALSE;
		}
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case ID_EXIT:
			g_gameCfg.sVol = GetDlgItemInt(hwnd,  ID_SOUND_VOL, &trans, FALSE);
			g_gameCfg.aVol = GetDlgItemInt(hwnd,  ID_MUSIC_VOL, &trans, FALSE);

			index = SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_GETCURSEL, 0, 0);
			g_gameCfg.gfxMode.bpp = (eBPP)SendMessage(GetDlgItem(hwnd, ID_BITLIST), LB_GETITEMDATA, index, 0);

			if(IsDlgButtonChecked(hwnd, ID_FULLSCRN) == BST_CHECKED)
				SETFLAG(g_gameCfg.status, CFG_FULLSCREEN);
			else
				CLEARFLAG(g_gameCfg.status, CFG_FULLSCREEN);

			_GameCfgSave(GAMECFG, &g_gameCfg);

			EndDialog(hwnd, TRUE);
			return TRUE;
		}
		return FALSE;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline,
        int ishow)
{
	HWND hwnd=0;

	TimerQuery();
	ResetStartTime ( );

	srand(time(0));

	DialogBox(hinstance, MAKEINTRESOURCE(ID_HOMIES_SETUP), 0, HomieOptDlgProc);

	return 0;
}