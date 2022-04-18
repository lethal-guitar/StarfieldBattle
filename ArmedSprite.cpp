#include "ArmedSprite.h"

ArmedSprite::ArmedSprite(void)
{
  ShotSpeed = MaxShots = NrShots = CurrentDelay = ShotDelay = ShotTime = 0;
  ShotSound = NULL;

  ZeroMemory(&ShotData, sizeof(ShotData));
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

  while (NULL != ShotList.GetCurrent())
  {
    Shot = (Sprite*)ShotList.GetCurrent()->GetEntry();

    if (Shot == NULL)
    {
      return;
    }

    if (
      Shot->GetXY().y - ShotData.FrameHeight > 0 &&
      Shot->GetXY().y + ShotData.FrameHeight < SCR_HEIGHT - 10)
    {
      Shot->Move(0, float(ShotSpeed / float(FrameRate)));
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

void ArmedSprite::Fire(void)
{
  if (NrShots < MaxShots && CurrentDelay > ShotDelay)
  {
    Sprite* Shot = new Sprite;
    Shot->Create(
      ShotData.lpDDS,
      ShotData.FrameWidth,
      ShotData.FrameHeight,
      ShotData.FrameDelay,
      ShotData.Looped);

    if (ShotSpeed > 0)
    {
      Shot->SetXY(DWORD(x) + FrameWidth / 2, DWORD(y) + FrameHeight);
    }
    else
    {
      Shot->SetXY(DWORD(x) + FrameWidth / 2, DWORD(y) - ShotData.FrameHeight);
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

  while (NULL != ShotList.GetCurrent())
  {
    Shot = (Sprite*)ShotList.GetCurrent()->GetEntry();

    if (NULL == Shot)
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

void ArmedSprite::ClearShotList(void)
{
  ShotList.FirstKnoten();

  while (NULL != ShotList.GetCurrent())
  {
    ShotList.DeleteKnoten();

    if (ShotList.NextKnoten() == FALSE)
      break;
  }

  // ShotList.Delete();

  NrShots = 0;
}
