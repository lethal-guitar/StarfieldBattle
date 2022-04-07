#include <dmusici.h>
#include "Engine.h"

#ifndef DXAMANAGER_H
#define DXAMANAGER_H

class DirectXAudioManager
{
	private:
		IDirectMusicPerformance8* lpDMPerformance;
		IDirectMusicLoader8*      lpDMLoader;
		IDirectMusicSegment8*     lpDMSegBackgroundMusic;
		IDirectMusicSegment8*     lpDMSegSoundFX[10];

		char* SndFX[10];
		int   SndNr;

	public:
		~DirectXAudioManager();

		BOOL Init(HWND, char*);

		BOOL AddBGMusic(char* Filename);
		BOOL AddSndFX(char* Filename);

		void SetGlobalVolume(void);

		void PlayBGMusic(void) { lpDMPerformance->PlaySegmentEx(lpDMSegBackgroundMusic, NULL, NULL, DMUS_SEGF_DEFAULT, 0, NULL, NULL, NULL); }
		void PlaySndFX(char* Name);
};

#endif