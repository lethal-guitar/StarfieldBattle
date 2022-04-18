#include "List.h"
#include "Sprites.h"

#ifndef ARMEDSPRITE_H
  #define ARMEDSPRITE_H

class ArmedSprite : public Sprite
{
private:
  List ShotList;
  int ShotSpeed, MaxShots, NrShots, ShotDelay, CurrentDelay;
  short ShotTime; //, MuzzleFrames;
  SpriteDescription ShotData;
  const char* ShotSound;

public:
  ArmedSprite(void);

  void ClearShotList(void);
  void SetShotData(
    LPDIRECTDRAWSURFACE7,
    int,
    int,
    int,
    BOOL,
    int,
    int,
    const char*,
    int = 0);
  // void SetMuzzleFlash(LPDIRECTDRAWSURFACE7 lpDDS) { lpDDSMuzzle = lpDDS; }
  void DrawShots(LPDIRECTDRAWSURFACE7);
  void Fire(void);

  BOOL TestShotCollision(Sprite* ToTest);
};

#endif

extern DirectXAudioManager DMM;
