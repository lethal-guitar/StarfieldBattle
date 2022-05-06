#include "ArmedSprite.h"

ArmedSprite::ArmedSprite()
{
  ShotSpeed = MaxShots = NrShots = CurrentDelay = ShotDelay = ShotTime = 0;
  ShotSound = nullptr;
}

void ArmedSprite::SetShotData(
  LPDIRECTDRAWSURFACE7 lpDDS,
  int FrameWidth,
  int FrameHeight,
  int FrameDelay,
  BOOL Looped,
  int Max,
  int Speed,
  const char* Sound,
  int Delay)
{
  ShotData.lpDDS = lpDDS;
  ShotData.FrameWidth = FrameWidth;
  ShotData.FrameHeight = FrameHeight;
  ShotData.FrameDelay = FrameDelay;
  ShotData.Looped = Looped;

  ShotSpeed = Speed;
  MaxShots = Max;
  ShotSound = Sound;
  CurrentDelay = ShotDelay = Delay;
}

void ArmedSprite::DrawShots(LPDIRECTDRAWSURFACE7 lpDDS)
{
  if (++ShotTime >= FrameRate / 60)
  {
    CurrentDelay++;
    ShotTime = 0;
  }

  Sprite* Shot;
  ShotList.FirstKnoten();

  while (nullptr != ShotList.GetCurrent())
  {
    Shot = static_cast<Sprite*>(ShotList.GetCurrent()->GetEntry());

    if (Shot == nullptr)
    {
      return;
    }

    if (
      Shot->GetXY().y - ShotData.FrameHeight > 0 &&
      Shot->GetXY().y + ShotData.FrameHeight < SCR_HEIGHT - 10)
    {
      Shot->Move(0, float(ShotSpeed) / float(FrameRate));
      Shot->Draw(lpDDS);
    }
    else
    {
      ShotList.DeleteKnoten();
      NrShots--;
    }

    if (ShotList.NextKnoten() == FALSE)
    {
      break;
    }
  }
}

void ArmedSprite::Fire()
{
  if (NrShots < MaxShots && CurrentDelay > ShotDelay)
  {
    auto* Shot = new Sprite;
    Shot->Create(
      ShotData.lpDDS,
      ShotData.FrameWidth,
      ShotData.FrameHeight,
      ShotData.FrameDelay,
      ShotData.Looped);

    if (ShotSpeed > 0)
    {
      Shot->SetXY(int(x) + FrameWidth / 2, int(y) + FrameHeight);
    }
    else
    {
      Shot->SetXY(int(x) + FrameWidth / 2, int(y) - ShotData.FrameHeight);
    }

    ShotList.Add((void*)Shot);
    NrShots++;
    CurrentDelay = 0;

    DMM.PlaySndFX(ShotSound);
  }
}

BOOL ArmedSprite::TestShotCollision(Sprite* ToTest)
{
  Sprite* Shot;
  ShotList.FirstKnoten();

  while (nullptr != ShotList.GetCurrent())
  {
    Shot = static_cast<Sprite*>(ShotList.GetCurrent()->GetEntry());

    if (nullptr == Shot)
    {
      Error("Interner Fehler: TestShotCollision: Kein gültiger Zeiger");
      return FALSE;
    }

    if (Shot->TestCollision(ToTest) == TRUE)
    {
      ShotList.DeleteKnoten();
      NrShots--;

      return TRUE;
    }

    if (ShotList.NextKnoten() == FALSE)
      break;
  }

  return FALSE;
}

void ArmedSprite::ClearShotList()
{
  ShotList.FirstKnoten();

  while (nullptr != ShotList.GetCurrent())
  {
    ShotList.DeleteKnoten();

    if (ShotList.NextKnoten() == FALSE)
      break;
  }

  // ShotList.Delete();

  NrShots = 0;
}
