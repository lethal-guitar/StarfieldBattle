#include "Engine.h"
#include <dmusici.h>

#ifndef DXAMANAGER_H
  #define DXAMANAGER_H

class DirectXAudioManager
{
private:
  IDirectMusicPerformance8* lpDMPerformance;
  IDirectMusicLoader8* lpDMLoader;
  IDirectMusicSegment8* lpDMSegBackgroundMusic;
  IDirectMusicSegment8* lpDMSegSoundFX[10];

  const char* SndFX[10];
  int SndNr;

public:
  ~DirectXAudioManager();

  BOOL Init(HWND, const char*);

  BOOL AddBGMusic(const char* Filename);
  BOOL AddSndFX(const char* Filename);

  void SetGlobalVolume();

  void PlayBGMusic()
  {
    lpDMPerformance->PlaySegmentEx(
      lpDMSegBackgroundMusic,
      nullptr,
      nullptr,
      DMUS_SEGF_DEFAULT,
      0,
      nullptr,
      nullptr,
      nullptr);
  }
  void PlaySndFX(const char* Name);
};

#endif
