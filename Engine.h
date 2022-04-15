// --- Engine.h ---
//
// This file contains constants and declarations used by the whole program.

#ifndef ENGINE_H
#define ENGINE_H

#include <ddraw.h>

#define SCR_WIDTH   800
#define SCR_HEIGHT  600
#define COLOR_DEPTH  16

#define PLAYER_IS_HIT      1
#define PLAYER_HAS_SCORED  2

#define MSECS(a) (1000 / (a))

//struct FrameRateVars 
//{ 
//	LONGLONG CurrentTime, NextFrame, Frequency; 
//};

struct HiScoreEntry 
{
	char  Name[50];
	int   Score;
};


#endif

//extern HWND hWnd;
extern int Error(char*);

extern short FrameRate;
