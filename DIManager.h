#include "Engine.h"
#include <dinput.h>

#ifndef DIMANAGER_H
  #define DIMANAGER_H

  #define MOVE_LEFT 1
  #define MOVE_RIGHT 2
  #define MOVE_UP 4
  #define MOVE_DOWN 8
  #define FIRE 16

  #define USE_KEYBOARD 0
  #define USE_MOUSE 1
  #define USE_JOYSTICK 2

  #define JOY_TOLERANCE 333

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
  BOOL InitJoystick(HWND);
  BOOL InitMouse(HWND);

  int GetKeyboardInput(KeyActionMap KeyMap);
  int GetMouseInput(void);
  int GetJoystickInput(void);
  static BOOL CALLBACK
    EnumJoystickCallback(const DIDEVICEINSTANCE* lpDInstance, LPVOID lpContext);


  int CurrentDevice;

public:
  ~DirectInputManager();

  BOOL Init(HWND hWnd, HINSTANCE hInstance, int Dev);
  int GetInput(KeyActionMap KeyMap);

  DWORD GetScanCode(void);
  void Flush(int Device);
};

#endif
