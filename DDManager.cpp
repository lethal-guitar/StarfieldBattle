#include "DDManager.h"

BOOL DirectDrawManager::Init(HWND hWnd)
{
  lpDD7 = nullptr;
  lpDDSPrimary = nullptr;
  lpDDSBackBuffer = nullptr;

  for (int i = 0; i < MAX_SURFACES; i++)
  {
    SurfacesToRelease[i] = nullptr;
  }

  ZeroMemory(&BlitFX, sizeof(BlitFX));
  BlitFX.dwSize = sizeof(BlitFX);

  if (FAILED(
        DirectDrawCreateEx(nullptr, (void**)&lpDD7, IID_IDirectDraw7, nullptr)))
    return Error("DirectDraw-Objekt konnte nicht angelegt werden");

  if (FAILED(lpDD7->SetCooperativeLevel(
        hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT)))
    return Error("Kooperationsebene konnte nicht festgelegt werden");

  if (FAILED(lpDD7->SetDisplayMode(SCR_WIDTH, SCR_HEIGHT, COLOR_DEPTH, 0, 0)))
    return Error("Bildschirmmodus konnte nicht gesetzt werden");

  DDSURFACEDESC2 ddsd;

  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
    DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;

  ddsd.dwBackBufferCount = 1;

  if (FAILED(lpDD7->CreateSurface(&ddsd, &lpDDSPrimary, nullptr)))
    return Error("Primäre Oberfläche konnte nicht erzeugt werden");

  ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

  if (FAILED(lpDDSPrimary->GetAttachedSurface(&ddsd.ddsCaps, &lpDDSBackBuffer)))
    return Error(
      "BackBuffer konnte nicht mit der Hauptoberfläche verbunden werden");

  return TRUE;
}

DirectDrawManager::~DirectDrawManager()
{
  for (int i = 0; i < MAX_SURFACES; i++)
  {
    if (nullptr != SurfacesToRelease[i])
    {
      SurfacesToRelease[i]->Release();
      SurfacesToRelease[i] = nullptr;
    }
  }

  if (nullptr != lpDDSBackBuffer)
  {
    lpDDSBackBuffer->Release();
    lpDDSBackBuffer = nullptr;
  }

  if (nullptr != lpDDSPrimary)
  {
    lpDDSPrimary->Release();
    lpDDSPrimary = nullptr;
  }

  if (nullptr != lpDD7)
  {
    lpDD7->Release();
    lpDD7 = nullptr;
  }
}

LPDIRECTDRAWSURFACE7 DirectDrawManager::CreateSurfaceFromBitmap(
  LPCTSTR File,
  int dWidth,
  int dHeight)
{
  HDC hBmDC, hSDC;
  HBITMAP hBm;

  DDSURFACEDESC2 ddsd;
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  LPDIRECTDRAWSURFACE7 lpDDSurface;

  hBm = (HBITMAP)LoadImage(
    nullptr, File, IMAGE_BITMAP, dWidth, dHeight, LR_LOADFROMFILE);

  if (nullptr == hBm)
    return nullptr;

  ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  ddsd.dwWidth = dWidth;
  ddsd.dwHeight = dHeight;

  if (FAILED(lpDD7->CreateSurface(&ddsd, &lpDDSurface, nullptr)))
    return nullptr;

  if (FAILED(lpDDSurface->GetDC(&hSDC)))
  {
    lpDDSurface->Release();
    return nullptr;
  }

  hBmDC = CreateCompatibleDC(hSDC);

  SelectObject(hBmDC, hBm);

  if (FAILED(BitBlt(hSDC, 0, 0, dWidth, dHeight, hBmDC, 0, 0, SRCCOPY)))
  {
    lpDDSurface->Release();
    return nullptr;
  }

  lpDDSurface->ReleaseDC(hSDC);

  DeleteDC(hBmDC);
  DeleteObject(hBm);

  int i;
  for (i = 0; i < MAX_SURFACES; i++)
  {
    if (nullptr == SurfacesToRelease[i])
    {
      break;
    }
  }

  if (nullptr != SurfacesToRelease[i])
  {
    Error(
      "DirectDraw: Oberfläche kann nicht hinzugefügt werden, Maximale Anzahl Oberflächen erreicht");
    lpDDSurface->Release();
    return nullptr;
  }

  SurfacesToRelease[i] = lpDDSurface;

  return lpDDSurface;
}
