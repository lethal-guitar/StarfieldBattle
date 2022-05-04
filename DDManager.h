#include "Engine.h"
#include <ddraw.h>

#ifndef DIRECTDRAWMANAGER_H
  #define DIRECTDRAWMANAGER_H

  #define MAX_SURFACES 15

class DirectDrawManager
{
private:
  LPDIRECTDRAW7 lpDD7;
  LPDIRECTDRAWSURFACE7 lpDDSPrimary;
  LPDIRECTDRAWSURFACE7 lpDDSBackBuffer;

  LPDIRECTDRAWSURFACE7 SurfacesToRelease[MAX_SURFACES];

  DDBLTFX BlitFX;

public:
  ~DirectDrawManager();

  BOOL Init(HWND hWnd);

  void ClearScreen(int Color)
  {
    BlitFX.dwFillColor = Color;
    lpDDSBackBuffer->Blt(
      nullptr, nullptr, nullptr, DDBLT_COLORFILL | DDBLT_WAIT, &BlitFX);
  }

  void Flip() { lpDDSPrimary->Flip(nullptr, DDFLIP_WAIT); }
  void CopyPrimaryToBackBuffer()
  {
    lpDDSBackBuffer->BltFast(0, 0, lpDDSPrimary, nullptr, DDBLTFAST_WAIT);
  }

  LPDIRECTDRAWSURFACE7 GetBuffer() { return lpDDSBackBuffer; }

  LPDIRECTDRAWSURFACE7 CreateSurfaceFromBitmap(LPCTSTR, int, int);
};

#endif
