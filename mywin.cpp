#include "mywin.h"

RECT G_clientarea;
char G_filepath[MAXCHARBUFF];

/**********************************************************
;
;	Name:		Create_Window_Overlapped
;
;	Purpose:	Just so that I don't have to cut and paste.
;
;	Input:		title of window, the Window Function Callback,
;				the hinstance handle, the menu handler if you want, 
;				data to be passed to WM_CREATE, x & y position 
;				and dimension.
;
;	Output:		a lot of stuff happens
;
;	Return:		The handle of the window, if you want...better to use G_hwnd instead.
;
**********************************************************/
HWND Create_Window_Overlapped(char *name, char *menu_name, DWORD customStyle, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey)
{
	WNDCLASS wndclass={0};
    
    wndclass.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WinCallBack;
    wndclass.hInstance = hinstance ? hinstance : GetModuleHandle(NULL);
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = menu_name;
    wndclass.lpszClassName = name;

    
    RegisterClass(&wndclass);

	return CreateWindow(name, 
            name, 
            customStyle > 0 ? customStyle : WS_OVERLAPPEDWINDOW,// | extraflags,//(WS_THICKFRAME | WS_MAXIMIZEBOX),
            x, // X position.
            y, // Y position.
            sizex, // width.
            sizey, // height.
            NULL, //it's not a child, so it's NULL
            hmenu, //if we are going to put menu and stuff
            hinstance,
            data);
}

/**********************************************************
;
;	Name:		Create_Window_Popup
;
;	Purpose:	Just so that I don't have to cut and paste.
;
;	Input:		title of window, the Window Function Callback,
;				the hinstance handle, the menu handler if you want, 
;				data to be passed to WM_CREATE, x & y position 
;				and dimension.
;
;	Output:		a lot of stuff happens
;
;	Return:		The handle of the window, if you want...better to use G_hwnd instead.
;
**********************************************************/
HWND Create_Window_Popup(char *name, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey)
{
	WNDCLASS wndclass={0};
    
    wndclass.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WinCallBack;
    wndclass.hInstance = hinstance ? hinstance : GetModuleHandle(NULL);
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszClassName = name;

    
    RegisterClass(&wndclass);

	return CreateWindow(name, 
            name, 
            WS_POPUP,
            x, // X position.
            y, // Y position.
            sizex, // width.
            sizey, // height.
            NULL, //it's not a child, so it's NULL
            hmenu, //if we are going to put menu and stuff
            hinstance,
            data);
}

/**********************************************************
;
;	Name:		WindowDestroy
;
;	Purpose:	Destroys given window
;
;	Input:		the HWND, HINSTANCE and it's class name
;
;	Output:		destroys window, don't forget to set HWND and
;				HINSTANCE to NULL after this function.
;
;	Return:		none
;
**********************************************************/
void WindowDestroy(HWND hwnd, HINSTANCE hinst, const char *className)
{
	if(hwnd && hinst)
	{
		DestroyWindow(hwnd);

		UnregisterClass(className,hinst);
	}
}

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
BOOL WindowChangeMode()
{
	//insert code
	return TRUE;
}

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
/*UINT GetWindowMessage()
{
	MSG msg;
	while(1)
	{
		if (!(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)))
			{
			//do something
			}
		else
		{
			if (msg.message == WM_QUIT)
			{
				return WM_QUIT;
			}
			else
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
				return msg.message;
			}
		}
	}
}*/

MSG GetWindowMessage()
{
	MSG msg;
	msg.message = NOTHINGATALL;
	
	while(PeekMessage(&msg,NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return msg;
			}
			else
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
		}

	return msg;
}

/**********************************************************
;
;	Name:		LoadBox
;
;	Purpose:	A simple load dialog box is opened and will return the pathname
;
;	Input:		HWND, max char, filter ie. *.exe, DefExt ie. exe, title of load box,
;				flags.
;
;	Output:		load box is shown
;
;	Return:		Full pathname of the file
;
**********************************************************/
char * LoadBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags)
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));	//instead of filling in all data structure of ofn, we do this

	memset(G_filepath, 0, sizeof(char)*MAXCHARBUFF);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = 0;
	ofn.lpstrFile = G_filepath;
	ofn.nMaxFile = sizeof(G_filepath) - 1;	//minus null
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = NULL;	//minus null
	ofn.lpstrFilter = filter;
	ofn.lpstrDefExt = DefExt;
	ofn.lpstrTitle = title;
	ofn.Flags = flags;

	GetOpenFileName(&ofn);

	strcpy(G_filepath, ofn.lpstrFile);

	return G_filepath;
}

/**********************************************************
;
;	Name:		SaveBox
;
;	Purpose:	A simple save dialog box is opened and will return the pathname
;
;	Input:		HWND, max char, filter ie. *.exe, DefExt ie. exe, title of load box,
;				flags.
;
;	Output:		load box is shown
;
;	Return:		Full pathname of the file
;
**********************************************************/
char * SaveBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags)
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));	//instead of filling in all data structure of ofn, we do this

	memset(G_filepath, 0, sizeof(char)*MAXCHARBUFF);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = 0;
	ofn.lpstrFile = G_filepath;
	ofn.nMaxFile = sizeof(G_filepath) - 1;	//minus null
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = NULL;	//minus null
	ofn.lpstrFilter = filter;
	ofn.lpstrDefExt = DefExt;
	ofn.lpstrTitle = title;
	ofn.Flags = flags;

	GetSaveFileName(&ofn);

	strcpy(G_filepath, ofn.lpstrFile);

	return G_filepath;
}

/**********************************************************
;
;	Name:		GrabAllText
;
;	Purpose:	allocates a big string buffer and stores all
;				text from a file to the big string buffer
;				DONT FORGET TO FREE THE BUFFER AFTER USE
;
;	Input:		the filename
;
;	Output:		none
;
;	Return:		NULL if there was a failure and something if successful
;
**********************************************************/
char * GrabAllText(char *filename)
{
	fpos_t len;
	char *buf;
	FILE *fp;
	fp = fopen(filename, "rb");
	if(!fp)
		return NULL;
	
	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &len);
	fseek(fp, 0, SEEK_SET);
	buf = (char*)malloc((int)len * sizeof(int));
	fread(buf, (int) len, 1, fp);
	buf[len] = 0;
	return buf;
}

/**********************************************************
;
;	Name:	Random	
;
;	Purpose:	makes a random number between a given range
;
;	Input:		min and max number
;
;	Output:		none
;
;	Return:		the generated number
;
**********************************************************/
int Random(int a1, int a2)
{
	int tempnum = a2 - a1 + 1;
	int tempnum2;

	tempnum2 = a2 - (rand()%tempnum);

	return tempnum2;
}