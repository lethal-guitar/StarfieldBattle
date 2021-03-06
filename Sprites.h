#include "DXAManager.h"
#include "Engine.h"

#ifndef SPRITES_H
  #define SPRITES_H

struct SpriteDescription
{
  LPDIRECTDRAWSURFACE7 lpDDS = nullptr;
  int FrameWidth = 0;
  int FrameHeight = 0;
  int FrameDelay = 0;
  BOOL Looped = false;
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
  char Type{};
  RECT BBox{};

public:
  Sprite();

  void Create(LPDIRECTDRAWSURFACE7, int, int, int, BOOL, int = 100, int = 100);
  void SetXY(int PosX, int PosY)
  {
    x = float(PosX);
    y = float(PosY);
  }
  void Move(float MvX, float MvY)
  {
    if ((x + MvX > 0) && (x + float(FrameWidth) + MvX < float(SCR_WIDTH)))
      x += MvX;
    if ((y + MvY > 0) && (y + float(FrameHeight) + MvY < float(SCR_HEIGHT)))
      y += MvY;
  }
  void Draw(LPDIRECTDRAWSURFACE7);
  void Reset()
  {
    if (Continue == FALSE)
    {
      Continue = TRUE;
      Done = FALSE;
    }
  }
  [[nodiscard]] POINT GetXY(BOOL AddHalfSizeX = TRUE) const
  {
    POINT p = {int(x), int(y)};
    if (AddHalfSizeX)
      p.x = int(x) + FrameWidth / 2;
    return p;
  }
  POINT GetSize()
  {
    POINT p = {FrameWidth, FrameHeight};
    return p;
  }
  void Bounce();
  BOOL TestCollision(Sprite* ToTest) const;
  [[nodiscard]] BOOL IsItDone() const { return Done; }

  [[nodiscard]] char GetType() const { return Type; }
  void SetType(char NewType) { Type = NewType; }
  void SetNewBBox(int x1, int y1, int x2, int y2)
  {
    SetRect(&BBox, x1, y1, x2, y2);
  }
  RECT& GetBBox() { return BBox; }
};

class ASprite_NoList : public Sprite
{
private:
  const char* ShotSnd = nullptr;
  int ShotDelay = 0;
  short Time = 0;
  short CurrentShotDelay = 0;

public:
  void SetData(char Type, const char* Sound, int Delay)
  {
    this->Type = Type;
    ShotSnd = Sound;
    ShotDelay = Delay;
  }

  void IncShotDelay()
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
