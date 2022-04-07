#include <ddraw.h>
#include "Engine.h"

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
			lpDDSBackBuffer->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &BlitFX); 
		}

		void Flip(void) { lpDDSPrimary->Flip(NULL, DDFLIP_WAIT); }
		void CopyPrimaryToBackBuffer(void) { lpDDSBackBuffer->BltFast(0, 0, lpDDSPrimary, NULL, DDBLTFAST_WAIT); }

		LPDIRECTDRAWSURFACE7 GetBuffer(void) { return lpDDSBackBuffer; }
		
		LPDIRECTDRAWSURFACE7 CreateSurfaceFromBitmap(LPCTSTR, int, int);
};

#endif