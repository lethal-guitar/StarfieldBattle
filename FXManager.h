#ifndef FXMANAGER_H
#define FXMANAGER_H

#include "DXAManager.h"
#include "DDManager.h"
#include "List.h"
#include "Sprites.h"
#include "Engine.h"

class FXManager
{
	private:
		LPDIRECTDRAWSURFACE7 lpDDS;
		Sprite* SpriteArray;
		int MaxFX, FXNr;
	public:
		FXManager(void) { lpDDS = NULL; SpriteArray = NULL; MaxFX = FXNr = 0; }
		~FXManager() { delete[] SpriteArray; }
		
		void Init(int MaxFX, LPDIRECTDRAWSURFACE7 lpDDS);
		void Draw(void);
		void CreateExplosionAt(int x, int y);
};

#endif

extern DirectDrawManager   DDM;
extern DirectXAudioManager DMM;
