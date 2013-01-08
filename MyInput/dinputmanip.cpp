#include "dinputstuff.h"


/**********************************************************
;
;	Name:		dinput_init
;
;	Purpose:	initializes the direct input object and the three devices...joystick support is minimum
;
;	Input:		The handle to window, the hinstance and the data struct which includes the 3 devices
;				The last three are cooperation for each device (in order: mouse, keyboard, joystick)
;
;	Output:		stuff are put in the data struct
;
;	Return:		TRUE if success...and FALSE for failure
;
**********************************************************/
BOOL dinput_init(HWND hwnd, HINSTANCE hinst, PTrioInput pdiitems, DWORD mouse_coop_flag, DWORD kybrd_coop_flag, DWORD jystck_coop_flag)
{
	LPDIRECTINPUTDEVICE tempdevice;	//temporary input device

	DIPROPDWORD dipdw; //buffer data for mouse

	//
	//	step 1: Create the object.
	//
	if(FAILED(DirectInputCreate(hinst, DIRECTINPUT_VERSION, &(pdiitems->dinputobj), NULL)))
	{ MessageBox(hwnd, "DirectInputCreate failed!!!  DAng it!", "Failure in ddinput_init", MB_OK); return FALSE; }


	//
	// step 2: Create the device object for each and every one of them
	//

	//First we do the mouse!
	if(FAILED(IDirectInput_CreateDevice(pdiitems->dinputobj,&GUID_SysMouse,&tempdevice,NULL)))
	{ MessageBox(hwnd, "Error in making mouse device", "Failure in ddinput_init", MB_OK); return FALSE; }
	else
	{ 
		if(FAILED(IDirectInputDevice_QueryInterface(tempdevice,&IID_IDirectInputDevice2W,&(pdiitems->dmouse))))
		{ MessageBox(hwnd, "Unable to Query interface for Mouse Device!", "Failure in ddinput_init", MB_OK); return FALSE; }
		else
			IDirectInputDevice_Release(tempdevice); 
	}

	//Now let's do keyboard!
	if(FAILED(IDirectInput_CreateDevice(pdiitems->dinputobj,&GUID_SysKeyboard,&tempdevice,NULL)))
	{ MessageBox(hwnd, "Error in making keyboard device", "Failure in ddinput_init", MB_OK); return FALSE; }
	else
	{ 
		if(FAILED(IDirectInputDevice_QueryInterface(tempdevice,&IID_IDirectInputDevice2W,&(pdiitems->dkeyboard))))
		{ MessageBox(hwnd, "Unable to Query interface for Keyboard Device!", "Failure in ddinput_init", MB_OK); return FALSE; }
		else
			IDirectInputDevice_Release(tempdevice); 
	}

	//...and last but not least the joystick
	// although we should enumerate the joysticks available, we are only going to use the primary
	if(FAILED(IDirectInput_CreateDevice(pdiitems->dinputobj,&GUID_Joystick,&tempdevice,NULL)))
	{ MessageBox(hwnd, "Error in making keyboard device", "Failure in ddinput_init", MB_OK); return FALSE; }
	else
	{ 
		if(FAILED(IDirectInputDevice_QueryInterface(tempdevice,&IID_IDirectInputDevice2W,&(pdiitems->djoystick))))
		{ MessageBox(hwnd, "Unable to Query interface for Keyboard Device!", "Failure in ddinput_init", MB_OK); return FALSE; }
		else
			IDirectInputDevice_Release(tempdevice); 
	}

	//
	// step 3: Set the data format
	//

	/*
     *  Set the data format to "mouse format".
     *
     *  A data format specifies which controls on a device we
     *  are interested in, and how they should be reported.
     *
     *  This tells DirectInput that we will be passing a
     *  DIMOUSESTATE structure to IDirectInputDevice::GetDeviceState.
     *
     *  Parameters:
     *
     *      c_dfDIMouse
     *
     *          Predefined data format which describes
     *          a DIMOUSESTATE structure.
     */
    if(FAILED(IDirectInputDevice_SetDataFormat(pdiitems->dmouse, &c_dfDIMouse)))
	{ MessageBox(hwnd, "Unable to set data format for Mouse Device!", "Failure in ddinput_init", MB_OK); return FALSE; }

	/*
     *  Set the data format to "keyboard format".
     *
     *  A data format specifies which controls on a device we
     *  are interested in, and how they should be reported.
     *
     *  This tells DirectInput that we are interested in all keys
     *  on the device, and they should be reported as DirectInput
     *  DIK_* codes.
     *
     *  Parameters:
     *
     *      c_dfDIKeyboard
     *
     *          Predefined data format which describes
     *          an array of 256 bytes, one per scancode.
     */
	if(FAILED(IDirectInputDevice_SetDataFormat(pdiitems->dkeyboard, &c_dfDIKeyboard)))
	{ MessageBox(hwnd, "Unable to set data format for Keyboard!", "Failure in ddinput_init", MB_OK); return FALSE; }


	// DONT FORGET TO FINISH THE REST!!!!!!!!!!
	// DONT FORGET TO FINISH THE REST!!!!!!!!!!
	// DONT FORGET TO FINISH THE REST!!!!!!!!!!

	//
	// step 4: Set the cooperation level
	//

	/*
     *  Set the cooperativity level to let DirectInput know how
     *  this device should interact with the system and with other
     *  DirectInput applications.
     *
     *  Parameters:
     *
     *      DISCL_NONEXCLUSIVE
     *
     *          Retrieve keyboard data when acquired, not interfering
     *          with any other applications which are reading keyboard
     *          data.
     *
     *      DISCL_FOREGROUND
     *
     *          If the user switches away from our application,
     *          automatically release the keyboard back to the system.
     *
     */

	//DWORD mouse_coop_flag, DWORD kybrd_coop_flag, DWORD jystck_coop_flag)

	//Do the mouse
	if(FAILED(IDirectInputDevice_SetCooperativeLevel(pdiitems->dmouse, hwnd, mouse_coop_flag)))//DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
	{ MessageBox(hwnd, "Unable to set cooperative level for Mouse Device!", "Failure in ddinput_init", MB_OK); return FALSE; }

	//Now the keyboard
	if(FAILED(IDirectInputDevice_SetCooperativeLevel(pdiitems->dkeyboard, hwnd, kybrd_coop_flag)))
	{ MessageBox(hwnd, "Unable to set cooperative level for Keyboard!", "Failure in ddinput_init", MB_OK); return FALSE; }

	//
	// step 5: Prepare the buffer
	//
	dipdw.diph.dwSize =		  sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj =		  0;					//nothing for now...
	dipdw.diph.dwHow =		  DIPH_DEVICE;			// entire device
	dipdw.dwData =			  BUFFERSIZE;

	if(FAILED(IDirectInputDevice_SetProperty(pdiitems->dmouse, DIPROP_BUFFERSIZE, &dipdw.diph)))
	{ MessageBox(hwnd, "Unable to set property for Mouse Device!", "Failure in ddinput_init", MB_OK); return FALSE; }

	// DONT FORGET TO FINISH THE REST!!!!!!!!!!
	// DONT FORGET TO FINISH THE REST!!!!!!!!!!
	// DONT FORGET TO FINISH THE REST!!!!!!!!!!

	//
	// step 6: Acquire the devices
	//
	IDirectInputDevice_Acquire(pdiitems->dmouse);
	IDirectInputDevice_Acquire(pdiitems->dkeyboard);

	// DONT FORGET TO FINISH THE REST!!!!!!!!!!
	// DONT FORGET TO FINISH THE REST!!!!!!!!!!
	// DONT FORGET TO FINISH THE REST!!!!!!!!!!

	return TRUE;
}

/**********************************************************
;
;	Name:		dinput_term
;
;	Purpose:	This will destroy the 3 devices and the direct input object
;
;	Input:		The data structure that contians all 3 devices and the direct input object
;
;	Output:		The data is destroyed
;
;	Return:		none
;
**********************************************************/
void dinput_term(PTrioInput pdiitems)
{
	//destroy the direct input object
	if(pdiitems->dinputobj)
		IDirectInput_Release(pdiitems->dinputobj);
	//Then destroy the rest of them!!!
	if(pdiitems->djoystick)
	{
		IDirectInputDevice_Unacquire(pdiitems->djoystick);
		IDirectInputDevice_Release(pdiitems->djoystick);
	}
	if(pdiitems->dkeyboard)
	{
		IDirectInputDevice_Unacquire(pdiitems->dkeyboard);
		IDirectInputDevice_Release(pdiitems->dkeyboard);
	}
	if(pdiitems->dmouse)
	{
		IDirectInputDevice_Unacquire(pdiitems->dmouse);
		IDirectInputDevice_Release(pdiitems->dmouse);
	}
}

/*****************************************************************************************************************
 *****************************************************************************************************************

  The following are device manipulation

 *****************************************************************************************************************
*****************************************************************************************************************/

/**********************************************************
;
;	Name:		diupdate_mouse
;
;	Purpose:	To get the location and button status of the mouse
;
;	Input:		The mouse direct input device and it better damn be it!  As well as the mouse data
;
;	Output:		updates the global mouse data location and button
;
;	Return:		none
;
**********************************************************/
void diupdate_mouse(HWND hwnd, LPDIRECTINPUTDEVICE2 dmouse, PDImouse mouse_data)
{
    if (dmouse) 
	{

		DIDEVICEOBJECTDATA rgdod[BUFFERSIZE]; /* Gobble-di-Gook buffers */
		DWORD			   dwitems = BUFFERSIZE; /*Determines the number of buffers recieved */
        HRESULT hr;

		//flush the double click array
		memset(mouse_data->double_click, 0, sizeof(int) * MaxButtons);

DOITAGAINMAN:
        hr = IDirectInputDevice_GetDeviceData(dmouse, sizeof(DIDEVICEOBJECTDATA), rgdod, &dwitems, 0);
        if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) 
		{
            /*
             *  DirectInput is telling us that the input stream has
             *  been interrupted.  We aren't tracking any state
             *  between polls, so we don't have any special reset
             *  that needs to be done.  We just re-acquire and
             *  try again.
             */
            hr = IDirectInputDevice_Acquire(dmouse);
            if (!FAILED(hr)) 
                goto DOITAGAINMAN;
        }

		//If we succeeded then do this...
        if (!FAILED(hr)) 
		{
			DWORD counter;

			for(counter = 0; counter < dwitems; counter++)
			{
				//If the mouse moved
				if(rgdod[counter].dwOfs == DIMOFS_X)
					mouse_data->location.X += (short)rgdod[counter].dwData;

				else if(rgdod[counter].dwOfs == DIMOFS_Y)
					mouse_data->location.Y += (short)rgdod[counter].dwData;

				//If button is down
				else if(rgdod[counter].dwData & 0x80)
				{
					//double click
					if((rgdod[counter].dwTimeStamp - mouse_data->LastClickTime) <=
						mouse_data->DoubleClickTime)
					{
						//make sure the last one was the same button
						if(mouse_data->LastButtonClicked == rgdod[counter].dwOfs)
							//When you xor it, we get to the right array offset 
							mouse_data->double_click[rgdod[counter].dwOfs ^ 0xF] = TRUE;
					}
					//just a click
					else
						mouse_data->button[rgdod[counter].dwOfs ^ 0xF] = TRUE;
				
					//get the last click time and button pressed...
					mouse_data->LastClickTime = rgdod[counter].dwTimeStamp;
					mouse_data->LastButtonClicked = rgdod[counter].dwOfs;
				}

				//other than that...it was release
				else
					//When you xor it, we get to the right array offset
					mouse_data->button[rgdod[counter].dwOfs ^ 0xF] = FALSE;
			}
        }
		else //we print what kind of an error it was!!!
		{
			if(hr == DIERR_INVALIDPARAM)
				MessageBox(hwnd, "Invalid Param diupdate_mouse", "Just as I thought", MB_OK);
			else if(hr == DIERR_NOTACQUIRED)
				MessageBox(hwnd, "DIERR_NOTACQUIRED diupdate_mouse", "Just as I thought", MB_OK);
			else if(hr == DIERR_NOTBUFFERED)
				MessageBox(hwnd, "DIERR_NOTBUFFERED diupdate_mouse", "Just as I thought", MB_OK);
			else if(hr == DIERR_NOTINITIALIZED)
				MessageBox(hwnd, "DIERR_NOTINITIALIZED diupdate_mouse", "Just as I thought", MB_OK);
		}
    }
}

/**********************************************************
;
;	Name:		diupdate_keyboard
;
;	Purpose:	Updates the 256 array keys passed in...
;
;	Input:		hwnd for error stuff, keyboard direct input device and of course the keys[256]
;				...it better damn be 256!!!
;
;	Output:		the keys array is updated
;
;	Return:		none
;
**********************************************************/
void diupdate_keyboard(HWND hwnd, LPDIRECTINPUTDEVICE2 dkeyboard, BYTE *keys)
{
	BYTE akeys[256];
	if (dkeyboard) 
	{

        HRESULT hr;

		
DOITAGAINMAN:
		//Fill up the array with the data
        hr = IDirectInputDevice_GetDeviceState(dkeyboard, sizeof(akeys), &akeys);
        if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
            /*
             *  DirectInput is telling us that the input stream has
             *  been interrupted.  We aren't tracking any state
             *  between polls, so we don't have any special reset
             *  that needs to be done.  We just re-acquire and
             *  try again.
             */
            hr = IDirectInputDevice_Acquire(dkeyboard);
            if (!FAILED(hr)) 
                goto DOITAGAINMAN;
        }
		else //we print what kind of an error it was!!!
		{
			if(hr == DIERR_INVALIDPARAM)
				MessageBox(hwnd, "Invalid Param diupdate_keyboard", "Just as I thought", MB_OK);
			else if(hr == DIERR_NOTACQUIRED)
				MessageBox(hwnd, "DIERR_NOTACQUIRED diupdate_keyboard", "Just as I thought", MB_OK);
			else if(hr == DIERR_NOTBUFFERED)
				MessageBox(hwnd, "DIERR_NOTBUFFERED diupdate_keyboard", "Just as I thought", MB_OK);
			else if(hr == DIERR_NOTINITIALIZED)
				MessageBox(hwnd, "DIERR_NOTINITIALIZED diupdate_keyboard", "Just as I thought", MB_OK);
		}

		memcpy(keys, akeys, sizeof(akeys));
    }
}