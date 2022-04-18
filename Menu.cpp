#include "Menu.h"

GameMenu::GameMenu(void)
{
  lpDDSMenuItemsSource = NULL;
  iNrItems = iCurrentItem = 0;
  bMenuActive = TRUE;
  ZeroMemory(&rMenuItemBBox, sizeof(rMenuItemBBox));
}

GameMenu::GameMenu(
  LPDIRECTDRAWSURFACE7 lpDDS,
  int iItems,
  RECT rBBox,
  POINT Pos)
{
  lpDDSMenuItemsSource = lpDDS;
  rMenuItemBBox = rBBox;
  Position = Pos;
  iNrItems = iItems;
  iCurrentItem = 0;
  bMenuActive = TRUE;

  DDCOLORKEY ddck;
  ddck.dwColorSpaceLowValue = 0;
  ddck.dwColorSpaceHighValue = 0;

  lpDDSMenuItemsSource->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

void GameMenu::DrawMenu(LPDIRECTDRAWSURFACE7 lpDDS)
{
  RECT r = rMenuItemBBox;

  int iItemHeight = r.bottom;

  for (int i = 0; i < iNrItems; i++)
  {
    if (i > 0 || i == iCurrentItem)
    {
      if (i != iCurrentItem && i - 1 != iCurrentItem)
      {
        r.top += iItemHeight * 2;
        r.bottom += iItemHeight * 2;
      }
      else
      {
        r.top += iItemHeight;
        r.bottom += iItemHeight;
      }
    }

    lpDDS->BltFast(
      Position.x,
      Position.y + (MENU_STEP_Y * i),
      lpDDSMenuItemsSource,
      &r,
      DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

    if (i + 1 == iCurrentItem)
    {
      r.top += iItemHeight * 2;
      r.bottom += iItemHeight * 2;
    }
  }
}

int GameMenu::CheckMenu(DirectInputManager& DIM)
{
  if (bMenuActive)
  {
    DWORD dwKeyCheckValue = DIM.GetScanCode();

    if (dwKeyCheckValue == DIK_RETURN)
      return iCurrentItem;

    if (dwKeyCheckValue == DIK_DOWN)
    {
      if (iCurrentItem >= 0 && iCurrentItem + 1 < iNrItems)
      {
        iCurrentItem++;
      }
      else
      {
        iCurrentItem = 0;
      }
    }
    else if (dwKeyCheckValue == DIK_UP)
    {
      if (iCurrentItem > 0 && iCurrentItem < iNrItems)
      {
        iCurrentItem--;
      }
      else
      {
        iCurrentItem = iNrItems - 1;
      }
    }
  }

  return -1;
}
