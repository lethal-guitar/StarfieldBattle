#include "Sprites.h"
#include "ArmedSprite.h"
#include "Colors.h"
#include <algorithm>

Sprite::Sprite()
{
  lpDDSSprite = nullptr;
  Looped = Done = TRUE;
  Continue = FALSE;
  FrameWidth = FrameHeight = FrameDelay = Delay = CurrentFrameX =
    CurrentFrameY = FramesX = FramesY = StepX = StepY = 0;
  x = y = 0.0f;
  BBox.left = BBox.right = BBox.top = BBox.bottom = 0;
}

void Sprite::Create(
  LPDIRECTDRAWSURFACE7 lpDDS,
  int FrmWidth,
  int FrmHeight,
  int FrmDelay,
  BOOL IsLooped,
  int StX,
  int StY)
{
  StepX = StX;
  StepY = StY;

  Looped = IsLooped;

  if (Looped == TRUE)
  {
    Continue = TRUE;
  }

  lpDDSSprite = lpDDS;

  if (nullptr == lpDDSSprite)
  {
    Error("Sprite konnte nicht erzeugt werden. Problem: Es wurde keine gültige\
		             Oberfläche übergeben.");
    return;
  }

  FrameWidth = FrmWidth;
  FrameHeight = FrmHeight;

  BBox.right = FrameWidth;
  BBox.bottom = FrameHeight;

  FrameDelay = FrmDelay;

  DDSURFACEDESC2 ddsd;
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  lpDDSSprite->GetSurfaceDesc(&ddsd);

  FramesX = static_cast<int>(ddsd.dwWidth) / FrameWidth;
  FramesY = static_cast<int>(ddsd.dwHeight) / FrameHeight;

  DWORD ColorValue;

  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  lpDDSSprite->Lock(nullptr, &ddsd, DDLOCK_WAIT, nullptr);

  ColorValue = DWORD(*PWORD(ddsd.lpSurface));

  lpDDSSprite->Unlock(nullptr);

  DDCOLORKEY ddck;
  ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = ColorValue;

  lpDDSSprite->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

void Sprite::Draw(LPDIRECTDRAWSURFACE7 lpDDS)
{
  RECT r;

  r.left = CurrentFrameX * FrameWidth;
  r.right = r.left + FrameWidth;
  r.top = CurrentFrameY * FrameHeight;
  r.bottom = r.top + FrameHeight;

  if (FAILED(lpDDS->BltFast(
        DWORD(x),
        DWORD(y),
        lpDDSSprite,
        &r,
        DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT)))
  {
    x = std::clamp(x, 0.0f, float(SCR_WIDTH - FrameWidth));
    y = std::clamp(y, 0.0f, float(SCR_WIDTH - FrameHeight));

    lpDDS->BltFast(
      DWORD(x),
      DWORD(y),
      lpDDSSprite,
      &r,
      DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
  }

  if (Continue == TRUE && FrameDelay != 0)
  {
    if (++Delay >= FrameRate / FrameDelay)
    {
      CurrentFrameX++;
      if (CurrentFrameX == FramesX)
      {
        CurrentFrameX = 0;
        CurrentFrameY++;

        if (CurrentFrameY == FramesY)
        {
          CurrentFrameY = 0;

          if (Looped == FALSE)
          {
            Continue = FALSE;
            Done = TRUE;
          }
        }
      }

      Delay = 0;
    }
  }
}

void Sprite::Bounce()
{
  x += float(float(StepX) / float(FrameRate));
  y += float(float(StepY) / float(FrameRate));

  if ((int(x) + FrameWidth) > SCR_WIDTH || x < 0.0f)
  {
    StepX = -StepX;
    x += float(float(StepX) / float(FrameRate));
  }

  if ((int(y) + FrameHeight) > SCR_HEIGHT || y < 0.0f)
  {
    StepY = -StepY;
    y += float(float(StepY) / float(FrameRate));
  }
}

BOOL Sprite::TestCollision(Sprite* ToTest) const
{
  int rx = (BBox.right - BBox.left) / 2;
  int ry = (BBox.bottom - BBox.top) / 2;

  RECT r = ToTest->GetBBox();

  int rx_ToTest = (r.right - r.left) / 2;
  int ry_ToTest = (r.bottom - r.top) / 2;

  int crx = static_cast<int>(x) + rx;
  int cry = static_cast<int>(y) + ry;

  int crx_ToTest = ToTest->GetXY(FALSE).x + rx_ToTest;
  int cry_ToTest = ToTest->GetXY().y + ry_ToTest;

  int dx = abs(crx_ToTest - crx);
  int dy = abs(cry_ToTest - cry);

  if (dx < rx + rx_ToTest && dy < ry + ry_ToTest)
  {
    return TRUE;
  }

  return FALSE;
}
