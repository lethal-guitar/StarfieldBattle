#include "DIManager.h"

BOOL DirectInputManager::Init(HWND hWnd, HINSTANCE hInstance, int Dev)
{
	lpDI8         = NULL; 
	lpDIDevice    = NULL;
	CurrentDevice = Dev;
	
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpDI8, NULL)))
	{
		return Error("DirectInput-Instanz konnte nicht erzeugt werden");
	}

	switch (CurrentDevice)
	{
	case USE_KEYBOARD:
		return InitKeyboard(hWnd);
		break;
	case USE_MOUSE:
		return InitMouse(hWnd);
		break;
	case USE_JOYSTICK:
		return InitJoystick(hWnd);
		break;
	default:
		CurrentDevice = USE_KEYBOARD;
		return InitKeyboard(hWnd);
		break;
	}

	return FALSE;
}

int DirectInputManager::GetInput(KeyActionMap KeyMap)
{
	switch (CurrentDevice)
	{
	case USE_MOUSE:
		return GetMouseInput();
		break;
	case USE_JOYSTICK:
		return GetJoystickInput();
		break;
	
	case USE_KEYBOARD:
	default:
		return GetKeyboardInput(KeyMap);
		break;
	}
	
	return 0;
}

DirectInputManager::~DirectInputManager()
{
	if (NULL != lpDIDevice)
	{
		lpDIDevice->Unacquire();
		lpDIDevice->Release();
		lpDIDevice = NULL;
	}

	if (NULL != lpDI8)
	{
		lpDI8->Release();
		lpDI8 = NULL;
	}
}

int DirectInputManager::GetKeyboardInput(KeyActionMap KeyMap)
{
	char bKeyboard[256];

	if (FAILED(lpDIDevice->GetDeviceState(sizeof(bKeyboard), (void*)&bKeyboard)))
	{
		lpDIDevice->Acquire();
		return 0;
	}
	
	int ret = 0;

	if (bKeyboard[KeyMap.Key_Left])
	{
		ret += MOVE_LEFT;
	}
	else if (bKeyboard[KeyMap.Key_Right])
	{
		ret += MOVE_RIGHT;
	}

	if (bKeyboard[KeyMap.Key_Up])
	{
		ret += MOVE_UP;
	}
	else if (bKeyboard[KeyMap.Key_Down])
	{
		ret += MOVE_DOWN;
	}

	if (bKeyboard[KeyMap.Key_Fire])
	{
		ret += FIRE;
	}

	return ret;
}

int DirectInputManager::GetMouseInput(void)
{
	short NumElements = 1;
	DIDEVICEOBJECTDATA data;
	ZeroMemory(&data, sizeof(data));

	if (FAILED(lpDIDevice->GetDeviceData(sizeof(data), &data, (DWORD*)&NumElements, 0)))
	{
		lpDIDevice->Acquire();
		return 0;
	}

	int ret = 0;
	
	switch (data.dwOfs)
	{
	case DIMOFS_X:
		ret += (data.dwOfs > 0) ? MOVE_RIGHT : MOVE_LEFT;
		break;
	case DIMOFS_Y:
		ret += (data.dwData > 0) ? MOVE_DOWN : MOVE_UP;
		break;
	case DIMOFS_BUTTON0:
		ret += FIRE;
		break;
	}

	return ret;
}

int DirectInputManager::GetJoystickInput(void)
{
	DIJOYSTATE2 DIJoyState;

	if (FAILED(lpDIDevice->Poll()))
	{
		lpDIDevice->Acquire();
		return 0;
	}

	lpDIDevice->GetDeviceState(sizeof(DIJoyState), &DIJoyState);

    int ret = 0;
	
	if(DIJoyState.lX      < -JOY_TOLERANCE)
    {
        ret += MOVE_LEFT;    
    }
    else if(DIJoyState.lX >  JOY_TOLERANCE)
    {
        ret += MOVE_RIGHT;
    }

    if(DIJoyState.lY      < -JOY_TOLERANCE)
    {
        ret += MOVE_UP;        
    }
    else if(DIJoyState.lY > JOY_TOLERANCE)
    {
        ret += MOVE_DOWN;
    }

	for(int i=0; i<128; i++)
    {
        if(DIJoyState.rgbButtons[i])
        {
            ret += FIRE;
            break;
        }
    }

	return ret;
}

BOOL DirectInputManager::InitKeyboard(HWND hWnd)
{
	if (FAILED(lpDI8->CreateDevice(GUID_SysKeyboard, &lpDIDevice, NULL)))
	{
		return FALSE;
	}
	
	if (FAILED(lpDIDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return FALSE;
	}

	if (FAILED(lpDIDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return FALSE;
	}

	DIPROPDWORD DIProperties;
		
	DIProperties.diph.dwSize       = sizeof(DIPROPDWORD);
	DIProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	DIProperties.diph.dwObj        = 0;
	DIProperties.diph.dwHow        = DIPH_DEVICE;
	DIProperties.dwData            = 16;

	if (FAILED(lpDIDevice->SetProperty(DIPROP_BUFFERSIZE, &DIProperties.diph)))
	{
		return FALSE;
	}

	lpDIDevice->Acquire();

	return TRUE;
}

BOOL DirectInputManager::InitMouse(HWND hWnd)
{
	if (FAILED(lpDI8->CreateDevice(GUID_SysMouse, &lpDIDevice, NULL)))
	{
		return FALSE;
	}

	if (FAILED(lpDIDevice->SetDataFormat(&c_dfDIMouse)))
	{
		return FALSE;
	}

	if (FAILED(lpDIDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
	{
		return FALSE;
	}

	DIPROPDWORD DIProperties;

	DIProperties.diph.dwSize       = sizeof(DIPROPDWORD);
	DIProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);

	DIProperties.diph.dwObj        = 0;
	DIProperties.diph.dwHow        = DIPH_DEVICE;

	DIProperties.dwData            = 16;

	if (FAILED(lpDIDevice->SetProperty(DIPROP_BUFFERSIZE, &DIProperties.diph)))
	{
		return FALSE;
	}

	lpDIDevice->Acquire();
	
	return TRUE;
}

BOOL DirectInputManager::InitJoystick(HWND hWnd)
{
    GUID gJoystick;

    if(FAILED(lpDI8->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoystickCallback, &gJoystick , DIEDFL_ATTACHEDONLY)))
    {
       return FALSE;
    }

    if(FAILED(lpDI8->CreateDevice(gJoystick, &lpDIDevice, NULL)))
    {
       return FALSE;
    }
  
    if(FAILED(lpDIDevice->SetDataFormat(&c_dfDIJoystick2)))
    {
        return FALSE;
    }

    if(FAILED(lpDIDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
    {    
        return FALSE;
    }
    
    DIPROPRANGE JoystickRange;
   
	JoystickRange.diph.dwSize       = sizeof(DIPROPRANGE); 
    JoystickRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    JoystickRange.diph.dwObj        = DIJOFS_X;
    JoystickRange.diph.dwHow        = DIPH_BYOFFSET; 
    JoystickRange.lMin              = -1000; 
    JoystickRange.lMax              =  1000; 
   
    if(FAILED(lpDIDevice->SetProperty(DIPROP_RANGE, &JoystickRange.diph)))
    {
        return FALSE;
    }

    JoystickRange.diph.dwObj        = DIJOFS_Y;

    if(FAILED(lpDIDevice->SetProperty(DIPROP_RANGE, &JoystickRange.diph)))
    {
        return FALSE;
    }
    
    lpDIDevice->Acquire();

    return TRUE;
}

DWORD DirectInputManager::GetScanCode(void)
{
	DWORD NumElements = 1;
	DIDEVICEOBJECTDATA data;
	ZeroMemory(&data, sizeof(data));

	if (FAILED(lpDIDevice->GetDeviceData(sizeof(data), &data, &NumElements, 0)))
	{
		lpDIDevice->Acquire();
		return 0;
	}

	if (data.dwData & 0x80)
	{
		return data.dwOfs;
	}

	return 0;
}

void DirectInputManager::Flush(int Device)
{
	if (Device == USE_JOYSTICK)
		return;
	
	DWORD dwTmp = INFINITE;
	
	lpDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &dwTmp, 0);
}


BOOL CALLBACK DirectInputManager::EnumJoystickCallback(const DIDEVICEINSTANCE* lpDInstance, LPVOID lpContext)
{  
	memcpy(lpContext, &lpDInstance->guidInstance, sizeof(lpDInstance->guidInstance));

    return DIENUM_STOP;
} 