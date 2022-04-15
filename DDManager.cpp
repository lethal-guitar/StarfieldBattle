#include "DDManager.h"

BOOL DirectDrawManager::Init(HWND hWnd)
{
	lpDD7		    = NULL;
	lpDDSPrimary    = NULL;
	lpDDSBackBuffer = NULL;

	for (int i = 0; i < MAX_SURFACES; i++)
	{
		SurfacesToRelease[i] = NULL;
	}

	ZeroMemory(&BlitFX, sizeof(BlitFX));
	BlitFX.dwSize = sizeof(BlitFX);
	
	if (NULL == lpDD7)
	{
		if (FAILED (DirectDrawCreateEx(NULL, (void**)&lpDD7, IID_IDirectDraw7, NULL)))
			return Error("DirectDraw-Objekt konnte nicht angelegt werden");

		if (FAILED (lpDD7->SetCooperativeLevel(hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT )))
			return Error("Kooperationsebene konnte nicht festgelegt werden");

		if (FAILED (lpDD7->SetDisplayMode(SCR_WIDTH, SCR_HEIGHT, COLOR_DEPTH, 0, 0)))
			return Error("Bildschirmmodus konnte nicht gesetzt werden");
			
		DDSURFACEDESC2 ddsd;
	
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
		
		ddsd.dwBackBufferCount = 1;
	
		if (FAILED (lpDD7->CreateSurface(&ddsd, &lpDDSPrimary, NULL)))
			return Error("Primäre Oberfläche konnte nicht erzeugt werden");
		
		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (FAILED (lpDDSPrimary->GetAttachedSurface(&ddsd.ddsCaps, &lpDDSBackBuffer)))
			return Error("BackBuffer konnte nicht mit der Hauptoberfläche verbunden werden");	
	
	}
	else
	{
		return Error("DirectDraw ist bereits initialisiert");
	}

	return TRUE;
}

DirectDrawManager::~DirectDrawManager()
{
	for (int i = 0; i < MAX_SURFACES; i++)
	{
		if (NULL != SurfacesToRelease[i])
		{
			SurfacesToRelease[i]->Release();
			SurfacesToRelease[i] = NULL;
		}
	}
	
	if (NULL != lpDDSBackBuffer)
	{
		lpDDSBackBuffer->Release();
		lpDDSBackBuffer = NULL;
	}
	
	if (NULL != lpDDSPrimary)
	{
		lpDDSPrimary->Release();
		lpDDSPrimary = NULL;
	}
	
	if (NULL != lpDD7)
	{
		lpDD7->Release();
		lpDD7 = NULL;
	}
}

LPDIRECTDRAWSURFACE7 DirectDrawManager::CreateSurfaceFromBitmap(LPCTSTR File, int dWidth, int dHeight)
{
	HDC hBmDC, hSDC;
	HBITMAP hBm;

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	LPDIRECTDRAWSURFACE7 lpDDSurface;

	hBm = (HBITMAP)LoadImage(NULL, File, IMAGE_BITMAP, dWidth, dHeight, LR_LOADFROMFILE);

	if (NULL == hBm)
		return NULL;
	
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth  = dWidth;
	ddsd.dwHeight =	dHeight;

	if (FAILED(lpDD7->CreateSurface(&ddsd, &lpDDSurface, NULL)))
		return NULL;
	
	if (FAILED(lpDDSurface->GetDC(&hSDC)))
	{
		lpDDSurface->Release();
		return NULL;
	}

	hBmDC = CreateCompatibleDC(hSDC);

	SelectObject(hBmDC, hBm);

	if (FAILED(BitBlt(hSDC, 0, 0, dWidth, dHeight, hBmDC, 0, 0, SRCCOPY)))
	{
		lpDDSurface->Release();
		return NULL;
	}

	lpDDSurface->ReleaseDC(hSDC);
	
	DeleteDC(hBmDC);
	DeleteObject(hBm);

	int i;
	for (i = 0; i < MAX_SURFACES; i++)
	{
		if (NULL == SurfacesToRelease[i])
		{
			break;
		}
	}

	if (NULL != SurfacesToRelease[i])
	{
		Error("DirectDraw: Oberfläche kann nicht hinzugefügt werden, Maximale Anzahl Oberflächen erreicht");
		lpDDSurface->Release();
		return NULL;
	}

	SurfacesToRelease[i] = lpDDSurface;
	
	return lpDDSurface;
}