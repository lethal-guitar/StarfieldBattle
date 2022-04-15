/*************************************************************

  Filename  : Colors.h
  Author    : DJ Cool Killer
  Purpose   : Declarations for the color 
			  calculation functions. 
			  See Colors.cpp for detailed information.

*************************************************************/

#include <ddraw.h>

struct ColorValues
{
	unsigned short ColorDepth;
	unsigned short ShiftRed; 
	unsigned short ShiftGreen; 
	unsigned short ShiftBlue; 
	unsigned short PosRed; 
	unsigned short PosGreen; 
	unsigned short PosBlue;
};

extern void DeterminePixelFormat(LPDIRECTDRAWSURFACE7);
extern int CalcColor(unsigned const int r, unsigned const int g, unsigned const int b);
