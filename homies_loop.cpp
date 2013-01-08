#include "homies.h"
#include "homies_i.h"

//The game loop
PUBLIC RETCODE GameLoop(HWND hwnd)
{
	MSG msg;

	//do set/init proc here
	SETPROCTYPE(eMainMenu);
	SENDPROCMSG(GMSG_INIT, 0, 0);

	while((msg = GetWindowMessage()).message != WM_QUIT)
	{
		//do something
		

		//do update proc here
		if(TimerEllapse(&g_gameTimer))
		{
			InputMouseUpdateLoc(hwnd);
			InputKbUpdate();

			SENDPROCMSG(GMSG_UPDATE, 0, 0);
		}

		//do display proc here
		if(TimerEllapse(&g_gameFrameTimer))
			SENDPROCMSG(GMSG_DISPLAY, 0, 0);
		
		Sleep(1);
	}

	//do terminate proc here
	SENDPROCMSG(GMSG_DESTROY, 0, 0);

	return RETCODE_SUCCESS;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam,
        LPARAM lparam)
{
    switch (message)
    {
        
        case WM_CREATE:
			return 0;

		case WM_MOVE:
            // Our window position has changed, so
            // get the client (drawing) rectangle.
            GetClientRect( hwnd, &G_clientarea);
            // Convert the coordinates from client relative
            // to screen
            ClientToScreen( hwnd, ( LPPOINT )&G_clientarea);
            ClientToScreen( hwnd, ( LPPOINT )&G_clientarea + 1 );
            return 0;

		case WM_LBUTTONDOWN:
			//AppUpdateFrame(SuperApp);
			InputMouseUpdateBtn	(eLeftButton, kButtonDown);
			return 0;

		case WM_LBUTTONUP:
			InputMouseUpdateBtn	(eLeftButton, kButtonUp);
			return 0;

		case WM_RBUTTONDOWN:
			//AppUpdateFrame(SuperApp);
			InputMouseUpdateBtn	(eRightButton, kButtonDown);
			return 0;

		case WM_RBUTTONUP:
			InputMouseUpdateBtn	(eRightButton, kButtonUp);
			return 0;
        
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }

    
    return(DefWindowProc(hwnd, message, wparam, lparam));
}