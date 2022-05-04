#include "Sprites.h"
#include "ArmedSprite.h"
#include "Colors.h"

Sprite::Sprite(void)
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

  FramesX = ddsd.dwWidth / FrameWidth;
  FramesY = ddsd.dwHeight / FrameHeight;

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
    if (x < 0)
    {
      x = 0;
    }

    if (y < 0)
    {
      y = 0;
    }

    if (x + FrameWidth > SCR_WIDTH)
    {
      x = x - (FrameWidth - (SCR_WIDTH - x));
    }

    if (y + FrameHeight > SCR_HEIGHT)
    {
      y = y - (FrameHeight - (SCR_HEIGHT - y));
    }

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

void Sprite::Bounce(void)
{
  x += float(float(StepX) / FrameRate);
  y += float(float(StepY) / FrameRate);

  if ((x + FrameWidth) > SCR_WIDTH || x < 0)
  {
    StepX = -StepX;
    x += float(float(StepX) / FrameRate);
  }

  if ((y + FrameHeight) > SCR_HEIGHT || y < 0)
  {
    StepY = -StepY;
    y += float(float(StepY) / FrameRate);
  }
}

BOOL Sprite::TestCollision(Sprite* ToTest)
{
  int rx = (BBox.right - BBox.left) / 2;
  int ry = (BBox.bottom - BBox.top) / 2;

  RECT r = ToTest->GetBBox();

  int rx_ToTest = (r.right - r.left) / 2;
  int ry_ToTest = (r.bottom - r.top) / 2;

  int crx = (int)x + rx;
  int cry = (int)y + ry;

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
