#include "Engine.h"
#include <dinput.h>

#ifndef DIMANAGER_H
  #define DIMANAGER_H

  #define MOVE_LEFT 1
  #define MOVE_RIGHT 2
  #define MOVE_UP 4
  #define MOVE_DOWN 8
  #define FIRE 16

struct KeyActionMap
{
  int Key_Left, Key_Right, Key_Up, Key_Down, Key_Fire;
};

class DirectInputManager
{
private:
  LPDIRECTINPUT8 lpDI8;
  LPDIRECTINPUTDEVICE8 lpDIDevice;

  BOOL InitKeyboard(HWND);

public:
  ~DirectInputManager();

  BOOL Init(HWND hWnd, HINSTANCE hInstance);
  int GetInput(KeyActionMap KeyMap);

  DWORD GetScanCode(void);
  void Flush();
};

#endif
