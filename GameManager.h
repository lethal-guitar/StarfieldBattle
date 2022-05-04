#ifndef GAMEMANAGER_H
  #define GAMEMANAGER_H

  #include "ArmedSprite.h"
  #include "DDManager.h"
  #include "Engine.h"
  #include "FXManager.h"

  #define MAX_ENEMYS 10
  #define MAX_SHOTS 20
  #define NEWSPR_DELAY 50
  #define SHOTSPEED 5

class SpriteManager
{
private:
  List SpriteList;
  int NrSprites, NrShots, NewSprTime;
  short Times;
  LPDIRECTDRAWSURFACE7 lpDDS;

public:
  SpriteManager(void)
  {
    NrSprites = NrShots = NewSprTime = 0;
    lpDDS = nullptr;
    Times = 0;
  }
  ~SpriteManager() { SpriteList.Delete(); }

  void MoveAndShoot(BOOL Move = TRUE);
  void Init(int, LPDIRECTDRAWSURFACE7);
  void ReInit(int);
  void AddEnemys(int);

  void FireShot(int, int);

  int TestCollisions(ArmedSprite*, int*);
};

typedef ASprite_NoList ASprite;

#endif

extern DirectDrawManager DDM;
extern FXManager FXM;
