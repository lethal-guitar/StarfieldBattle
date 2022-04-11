#include <ddraw.h>
#include <time.h>
#include "Engine.h"
#include "Starfield.h"

STARCOLORS StarColors[ MAX_SPEED ];

Star::Star(void)
{
	x = (float)(rand()% SCR_WIDTH  );
	y = (float)(rand()% SCR_HEIGHT );
	
	dy = (float)(rand()% MAX_SPEED  + 1);
	dy /= 10;

	Length = (int)(dy+1);
}

void Star::Move(void)
{
	y += dy / (FrameRate/35);

	if(y >= SCR_HEIGHT - Length)
	{
		x = (float)(rand()% SCR_WIDTH);

		dy = (float)(rand()% MAX_SPEED + 1);
		dy /= 10;

		y = dy;

		Length = (int)(dy+1);
	}
}

void Star::Draw(int Pitch, WORD* wlpSurface)
{
	int index = ((int)y * Pitch + (int)x);

	for (int i=0; i<Length; i++)
	{
		wlpSurface[index - i * Pitch] = CalcColor(StarColors[i].r, StarColors[i].g, StarColors[i].b);
	}
}

Starfield::Starfield(int NrStars, LPDIRECTDRAWSURFACE7 lpDDS)
{
	DeterminePixelFormat(m_lpDDS = lpDDS);
	srand((unsigned)time(NULL));
	
	m_Stars = new Star[m_NrStars = NrStars];

	int rgb = 255;
	
	for (int i = 0; i < MAX_SPEED; i++)
	{
		StarColors[i].r = rgb;
		StarColors[i].g = rgb;
		StarColors[i].b = rgb;
		rgb -= 40;
	}
}

Starfield::~Starfield()
{
	if(NULL != m_Stars)
	{
		delete[ ] m_Stars;
	}
}

void Starfield::Draw(BOOL FirstDraw)
{
	DDSURFACEDESC2 ddsd;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	m_lpDDS->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	WORD* wlpSurface = (WORD*)ddsd.lpSurface;

	int Pitch = ddsd.lPitch / 2;
	
	for (int i = 0; i < m_NrStars; i++)
	{
		if (!FirstDraw)
			m_Stars[i].Move();
		
		m_Stars[i].Draw(Pitch, wlpSurface);
	}

	m_lpDDS->Unlock(NULL);
}