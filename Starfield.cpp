#include "Starfield.h"
#include "Engine.h"
#include <ctime>
#include <ddraw.h>

STARCOLORS StarColors[MAX_SPEED];

Star::Star()
{
  x = static_cast<float>(rand() % SCR_WIDTH);
  y = static_cast<float>(rand() % SCR_HEIGHT);

  dy = static_cast<float>(rand() % MAX_SPEED + 1);
  dy /= 10;

  Length = static_cast<int>(dy + 1);
}

void Star::Move()
{
  y += dy / (float(FrameRate) / 35.0f);

  if (y >= SCR_HEIGHT - Length)
  {
    x = static_cast<float>(rand() % SCR_WIDTH);

    dy = static_cast<float>(rand() % MAX_SPEED + 1);
    dy /= 10;

    y = dy;

    Length = static_cast<int>(dy + 1);
  }
}

void Star::Draw(int Pitch, WORD* wlpSurface) const
{
  int index = (static_cast<int>(y) * Pitch + static_cast<int>(x));

  for (int i = 0; i < Length; i++)
  {
    wlpSurface[index - i * Pitch] =
      CalcColor(StarColors[i].r, StarColors[i].g, StarColors[i].b);
  }
}

Starfield::Starfield(int NrStars, LPDIRECTDRAWSURFACE7 lpDDS)
{
  DeterminePixelFormat(m_lpDDS = lpDDS);
  srand(static_cast<unsigned>(time(nullptr)));

  m_Stars = new Star[m_NrStars = NrStars];

  int rgb = 255;

  for (auto& StarColor : StarColors)
  {
    StarColor.r = rgb;
    StarColor.g = rgb;
    StarColor.b = rgb;
    rgb -= 40;
  }
}

Starfield::~Starfield()
{


  delete[] m_Stars;
}

void Starfield::Draw(BOOL FirstDraw)
{
  DDSURFACEDESC2 ddsd;

  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  m_lpDDS->Lock(nullptr, &ddsd, DDLOCK_WAIT, nullptr);

  WORD* wlpSurface = static_cast<WORD*>(ddsd.lpSurface);

  int Pitch = ddsd.lPitch / 2;

  for (int i = 0; i < m_NrStars; i++)
  {
    if (!FirstDraw)
      m_Stars[i].Move();

    m_Stars[i].Draw(Pitch, wlpSurface);
  }

  m_lpDDS->Unlock(nullptr);
}
