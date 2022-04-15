#include <ddraw.h>
#include "Colors.h"

#define MAX_SPEED  60

struct STARCOLORS {
	int r;
	int g;
	int b;
};

class Star
{
	private:
		float x, y, dy;
		int Length;
	public:
		Star(void);
		void Move(void);
		void Draw(int, WORD*);
};

class Starfield
{
	private:
		int   m_NrStars;
		Star* m_Stars;
		LPDIRECTDRAWSURFACE7 m_lpDDS;
	public:
		Starfield(int, LPDIRECTDRAWSURFACE7);
		~Starfield();
		void Draw(BOOL = FALSE);
};
