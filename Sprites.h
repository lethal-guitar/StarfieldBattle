#include "DXAManager.h"
#include "Engine.h"

#ifndef SPRITES_H
  #define SPRITES_H

struct SpriteDescription
{
  LPDIRECTDRAWSURFACE7 lpDDS;
  int FrameWidth, FrameHeight, FrameDelay;
  BOOL Looped;
};

class Sprite
{
protected:
  LPDIRECTDRAWSURFACE7 lpDDSSprite;
  int FrameWidth, FrameHeight;
  int FrameDelay, Delay, FramesX, FramesY, CurrentFrameX, CurrentFrameY;
  int StepX, StepY;
  float x, y;
  BOOL Looped, Continue, Done;
  char Type;
  RECT BBox;

public:
  Sprite(void);

  void Create(LPDIRECTDRAWSURFACE7, int, int, int, BOOL, int = 100, int = 100);
  void SetXY(int PosX, int PosY)
  {
    x = float(PosX);
    y = float(PosY);
  }
  void Move(float MvX, float MvY)
  {
    if ((x + MvX > 0) && (x + FrameWidth + MvX < SCR_WIDTH))
      x += MvX;
    if ((y + MvY > 0) && (y + FrameHeight + MvY < SCR_HEIGHT))
      y += MvY;
  }
  void Draw(LPDIRECTDRAWSURFACE7);
  void Reset(void)
  {
    if (Continue == FALSE)
    {
      Continue = TRUE;
      Done = FALSE;
    }
  }
  POINT GetXY(BOOL AddHalfSizeX = TRUE)
  {
    POINT p = {int(x), int(y)};
    if (AddHalfSizeX)
      p.x = int(x) + FrameWidth / 2;
    return p;
  }
  POINT GetSize(void)
  {
    POINT p = {FrameWidth, FrameHeight};
    return p;
  }
  void Bounce(void);
  BOOL TestCollision(Sprite* ToTest);
  BOOL IsItDone(void) { return Done; }

  char GetType(void) { return Type; }
  void SetType(char NewType) { Type = NewType; }
  void SetNewBBox(int x1, int y1, int x2, int y2)
  {
    SetRect(&BBox, x1, y1, x2, y2);
  }
  RECT& GetBBox(void) { return BBox; }
};

class ASprite_NoList : public Sprite
{
private:
  char* ShotSnd;
  int ShotDelay;
  short Time, CurrentShotDelay;

public:
  void SetData(char Type, char* Sound, int Delay)
  {
    this->Type = Type;
    ShotSnd = Sound;
    ShotDelay = Delay;
  }

  void IncShotDelay(void)
  {
    if (++Time >= FrameRate / 60)
    {
      Time = 0;
      CurrentShotDelay++;
    }
  }

  BOOL DoFire(DirectXAudioManager* SoundSource)
  {
    if (CurrentShotDelay > ShotDelay)
    {
      SoundSource->PlaySndFX(ShotSnd);
      CurrentShotDelay = 0;

      return TRUE;
    }

    return FALSE;
  }
};

#endif
