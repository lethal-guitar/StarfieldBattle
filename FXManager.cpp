#include "FXManager.h"
#include <cstdio>

static LPDIRECTDRAWSURFACE7 lpDDSExplosion;

void FXManager::Init(int MaxFX, LPDIRECTDRAWSURFACE7 lpDDS)
{
  lpDDSExplosion =
    DDM.CreateSurfaceFromBitmap("Sprites\\Explosion.bmp", 640, 240);

  if (nullptr == lpDDS)
  {
    Error("FXManager: Es wurde keine gültige Oberfläche übergeben");
  }

  this->m_lpDDS = lpDDS;

  m_FXNr = 0;

  m_SpriteArray = new Sprite[this->m_MaxFX = MaxFX];

  for (int i = 0; i < MaxFX; i++)
  {
    m_SpriteArray[i].Create(lpDDSExplosion, 80, 80, MSECS(30), FALSE);
  }
}

void FXManager::CreateExplosionAt(int x, int y)
{
  if (m_FXNr < m_MaxFX)
  {
    int i;
    for (i = 0;;)
    {
      if (m_SpriteArray[i].IsItDone() == TRUE)
      {
        goto CreateIt;
      }

      i++;

      if (i >= m_MaxFX)
      {
        goto CreateIt;
      }
    }

  CreateIt:

    m_FXNr++;

    DMM.PlaySndFX("Explosion.wav");

    m_SpriteArray[i].SetXY(x, y);
    m_SpriteArray[i].Reset();
  }
}

void FXManager::Draw()
{
  for (int i = 0; i < m_MaxFX; i++)
  {
    m_SpriteArray[i].Draw(m_lpDDS);

    if (m_SpriteArray[i].IsItDone() == TRUE)
    {
      if (m_FXNr > 0)
      {
        m_FXNr--;
      }
    }
  }
}
