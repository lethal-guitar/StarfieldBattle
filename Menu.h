#ifndef MENU_H
#define MENU_H

#include "DIManager.h"
#include <ddraw.h>

#define MENU_STEP_Y 100

class GameMenu
{
private:
  LPDIRECTDRAWSURFACE7 lpDDSMenuItemsSource;
  int iNrItems, iCurrentItem;
  BOOL bMenuActive;
  RECT rMenuItemBBox{};
  POINT Position{};

public:
  GameMenu();
  GameMenu(LPDIRECTDRAWSURFACE7 lpDDS, int iItems, RECT rBBox, POINT pPos);

  void DrawMenu(LPDIRECTDRAWSURFACE7 lpDDS);

  int CheckMenu(DirectInputManager& DIM);

  void SetState(BOOL bNewState)
  {
    bMenuActive = bNewState;
    iCurrentItem = 0;
  }
};

#endif
