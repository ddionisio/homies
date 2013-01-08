#include "gdix.h"
#include "gdix_i.h"

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
PROTECTED bool _GFXCheckError(HRESULT hr, bool displayMsg, const char *header)
{
	switch(hr)
	{
	case D3D_OK:
		return false;
	case D3DERR_CONFLICTINGRENDERSTATE:
		if(displayMsg) ASSERT_MSG(0,header, "The currently set render states cannot be used together.");
		return true;
	case D3DERR_CONFLICTINGTEXTUREFILTER:
		if(displayMsg) ASSERT_MSG(0,header, "The current texture filters cannot be used together.");
		return true;
	case D3DERR_CONFLICTINGTEXTUREPALETTE:
		if(displayMsg) ASSERT_MSG(0,header, "The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palletized textures simultaneously enabled also share the same palette.");
		return true;
	case D3DERR_DEVICELOST:
		if(displayMsg) ASSERT_MSG(0,header, "The device is lost and cannot be restored at the current time, so rendering is not possible.");
		return true;
	case D3DERR_DEVICENOTRESET:
		if(displayMsg) ASSERT_MSG(0,header, "The device cannot be reset.");
		return true;
	case D3DERR_DRIVERINTERNALERROR:
		if(displayMsg) ASSERT_MSG(0,header, "Internal driver error.");
		return true;
	case D3DERR_INVALIDCALL:
		if(displayMsg) ASSERT_MSG(0,header, "The method call is invalid. For example, a method's parameter may have an invalid value.");
		return true;
	case D3DERR_INVALIDDEVICE:
		if(displayMsg) ASSERT_MSG(0,header, "The requested device type is not valid.");
		return true;
	case D3DERR_MOREDATA:
		if(displayMsg) ASSERT_MSG(0,header, "There is more data available than the specified buffer size can hold.");
		return true;
	case D3DERR_NOTAVAILABLE:
		if(displayMsg) ASSERT_MSG(0,header, "This device does not support the queried technique.");
		return true;
	case D3DERR_NOTFOUND:
		if(displayMsg) ASSERT_MSG(0,header, "The requested item was not found.");
		return true;
	case D3DERR_OUTOFVIDEOMEMORY:
		if(displayMsg) ASSERT_MSG(0,header, "Direct3D does not have enough display memory to perform the operation.");
		return true;
	case D3DERR_TOOMANYOPERATIONS:
		if(displayMsg) ASSERT_MSG(0,header, "The application is requesting more texture-filtering operations than the device supports.");
		return true;
	case D3DERR_UNSUPPORTEDALPHAARG:
		if(displayMsg) ASSERT_MSG(0,header, "The device does not support a specified texture-blending argument for the alpha channel.");
		return true;
	case D3DERR_UNSUPPORTEDALPHAOPERATION:
		if(displayMsg) ASSERT_MSG(0,header, "The device does not support a specified texture-blending operation for the alpha channel.");
		return true;
	case D3DERR_UNSUPPORTEDCOLORARG:
		if(displayMsg) ASSERT_MSG(0,header, "The device does not support a specified texture-blending argument for color values.");
		return true;
	case D3DERR_UNSUPPORTEDCOLOROPERATION:
		if(displayMsg) ASSERT_MSG(0,header, "The device does not support a specified texture-blending operation for color values.");
		return true;
	case D3DERR_UNSUPPORTEDFACTORVALUE:
		if(displayMsg) ASSERT_MSG(0,header, "The device does not support the specified texture factor value.");
		return true;
	case D3DERR_UNSUPPORTEDTEXTUREFILTER:
		if(displayMsg) ASSERT_MSG(0,header, "The device does not support the specified texture filter.");
		return true;
	case D3DERR_WRONGTEXTUREFORMAT:
		if(displayMsg) ASSERT_MSG(0,header, "The pixel format of the texture surface is not valid.");
		return true;
	case E_FAIL:
		if(displayMsg) ASSERT_MSG(0,header, "An undetermined error occurred inside the Direct3D subsystem.");
		return true;
	case E_INVALIDARG:
		if(displayMsg) ASSERT_MSG(0,header, "An invalid parameter was passed to the returning function");
		return true;
	/*case E_INVALIDCALL:
		if(displayMsg) ASSERT_MSG(0,header, "The method call is invalid. For example, a method's parameter may have an invalid value.");
		return false;*/
	case E_OUTOFMEMORY:
		if(displayMsg) ASSERT_MSG(0,header, "Direct3D could not allocate sufficient memory to complete the call.");
		return true;
	}

	return false;
}
 