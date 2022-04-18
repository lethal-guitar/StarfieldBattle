#ifndef FXMANAGER_H
  #define FXMANAGER_H

  #include "DDManager.h"
  #include "DXAManager.h"
  #include "Engine.h"
  #include "List.h"
  #include "Sprites.h"

class FXManager
{
private:
  LPDIRECTDRAWSURFACE7 m_lpDDS;
  Sprite* m_SpriteArray;
  int m_MaxFX, m_FXNr;

public:
  FXManager(void)
  {
    m_lpDDS = NULL;
    m_SpriteArray = NULL;
    m_MaxFX = m_FXNr = 0;
  }
  ~FXManager() { delete[] m_SpriteArray; }

  void Init(int MaxFX, LPDIRECTDRAWSURFACE7 lpDDS);
  void Draw(void);
  void CreateExplosionAt(int x, int y);
};

#endif

extern DirectDrawManager DDM;
extern DirectXAudioManager DMM;
