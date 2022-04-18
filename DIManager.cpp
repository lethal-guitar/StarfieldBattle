#include "DIManager.h"

BOOL DirectInputManager::Init(HWND hWnd, HINSTANCE hInstance, int)
{
  lpDI8 = NULL;
  lpDIDevice = NULL;

  if (FAILED(DirectInput8Create(
        hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&lpDI8,
        NULL)))
  {
    return Error("DirectInput-Instanz konnte nicht erzeugt werden");
  }

  return InitKeyboard(hWnd);
}

int DirectInputManager::GetInput(KeyActionMap KeyMap)
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

  if (FAILED(lpDIDevice->SetCooperativeLevel(
        hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
  {
    return FALSE;
  }

  DIPROPDWORD DIProperties;

  DIProperties.diph.dwSize = sizeof(DIPROPDWORD);
  DIProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  DIProperties.diph.dwObj = 0;
  DIProperties.diph.dwHow = DIPH_DEVICE;
  DIProperties.dwData = 16;

  if (FAILED(lpDIDevice->SetProperty(DIPROP_BUFFERSIZE, &DIProperties.diph)))
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

void DirectInputManager::Flush(int)
{
  DWORD dwTmp = INFINITE;

  lpDIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &dwTmp, 0);
}
