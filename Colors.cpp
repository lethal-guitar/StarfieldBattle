/*************************************************************

  Filename	 : Colors.cpp
  Author     : DJ Cool Killer
  Purpose    : This is the code for the
                           DeterminePixelFormat(LPDIRECTDRAWSURFACE)
                           and CalcColor(int, int, int) functions.
                           They are used to calculate the correct color
                           values for a given color depth.

*************************************************************/

#include "Colors.h"

static ColorValues ClVals;

// Must be called before you can use CalcColor()
void DeterminePixelFormat(LPDIRECTDRAWSURFACE7 lpDDS)
{
  DDPIXELFORMAT ddpf;
  ddpf.dwSize = sizeof(ddpf);
  lpDDS->GetPixelFormat(&ddpf);

  ClVals.ColorDepth = (unsigned short)ddpf.dwRGBBitCount;

  while ((ddpf.dwRBitMask & 1) == 0)
  {
    ddpf.dwRBitMask >>= 1;
    ClVals.PosRed++;
  }

  if (ddpf.dwRBitMask == 31)
    ClVals.ShiftRed = 3;
  else
    ClVals.ShiftRed = 2;

  while ((ddpf.dwGBitMask & 1) == 0)
  {
    ddpf.dwGBitMask >>= 1;
    ClVals.PosGreen++;
  }

  if (ddpf.dwGBitMask == 31)
    ClVals.ShiftGreen = 3;
  else
    ClVals.ShiftGreen = 2;

  ClVals.PosBlue = 0;

  if (ddpf.dwBBitMask == 31)
    ClVals.ShiftBlue = 3;
  else
    ClVals.ShiftBlue = 2;
}

int CalcColor(unsigned const int r, unsigned const int g, unsigned const int b)
{
  if ((r < 255) && (g < 255) && (b < 255))
  {
    switch (ClVals.ColorDepth)
    {
      case 32:
      case 24:
        return static_cast<int>((r << 16) + (g << 8) + b);
      case 16:
        return static_cast<int>(
          (((r) >> ClVals.ShiftRed) << ClVals.PosRed) +
          (((g) >> ClVals.ShiftGreen) << ClVals.PosGreen) +
          (((b) >> ClVals.ShiftBlue) << ClVals.PosBlue));
    }
  }

  return 0;
}
