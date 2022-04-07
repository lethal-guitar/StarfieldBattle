#include <stdio.h>
#include "FXManager.h"

static LPDIRECTDRAWSURFACE7 lpDDSExplosion;

void FXManager::Init(int MaxFX, LPDIRECTDRAWSURFACE7 lpDDS)
{
	lpDDSExplosion = DDM.CreateSurfaceFromBitmap("Sprites\\Explosion.bmp", 640, 240);
	
	if (NULL == lpDDS)
	{
		Error("FXManager: Es wurde keine gültige Oberfläche übergeben");
	}

	this->lpDDS = lpDDS;

	FXNr = 0;

	SpriteArray = new Sprite[this->MaxFX = MaxFX];
	
	for (int i = 0; i < MaxFX; i++)
	{
		SpriteArray[i].Create(lpDDSExplosion, 80, 80, MSECS(30), FALSE); 
	}
}

void FXManager::CreateExplosionAt(int x, int y)
{
	if (FXNr < MaxFX)
	{
		for (int i = 0; ;)
		{
			if (SpriteArray[i].IsItDone() == TRUE)
			{
				goto CreateIt;
			}

			i++;
			
			if (i >= MaxFX)
			{				
				goto CreateIt;
			}
		}

		CreateIt:

		FXNr++;

		DMM.PlaySndFX("Explosion.wav");
		
		SpriteArray[i].SetXY(x, y);
		SpriteArray[i].Reset();
	}
}

void FXManager::Draw(void)
{
	for (int i = 0; i < MaxFX; i++)
	{
		SpriteArray[i].Draw(lpDDS);

		if (SpriteArray[i].IsItDone() == TRUE)
		{
			if (FXNr > 0)
			{
				FXNr--;
			}
		}
	}

}