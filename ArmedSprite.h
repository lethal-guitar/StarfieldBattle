#include "Sprites.h"
#include "List.h"

#ifndef ARMEDSPRITE_H
#define ARMEDSPRITE_H

class ArmedSprite : public Sprite
{
	private:
		List ShotList;
		int ShotSpeed, MaxShots, NrShots, ShotDelay, CurrentDelay;
		short ShotTime;//, MuzzleFrames;
		SpriteDescription ShotData;
		char* ShotSound;
		//LPDIRECTDRAWSURFACE7 lpDDSMuzzle;
		BOOL ShowMuzzle;
	public:
		ArmedSprite(void);
		
		void ClearShotList(void);
		void SetShotData(LPDIRECTDRAWSURFACE7, int, int, int, BOOL, int, int, char*, int = 0);
		//void SetMuzzleFlash(LPDIRECTDRAWSURFACE7 lpDDS) { lpDDSMuzzle = lpDDS; }
		void DrawShots(LPDIRECTDRAWSURFACE7);
		void Draw(LPDIRECTDRAWSURFACE7);
		void Fire(void);
		
		BOOL AreThereShots(void) { if (NrShots > 0) return TRUE; else return FALSE; }

		BOOL TestShotCollision(Sprite* ToTest);

		List&  GetShotList(void)  { return ShotList; }
};

#endif

extern DirectXAudioManager DMM;
