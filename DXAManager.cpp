#include "DXAManager.h"
#include <stdio.h>

BOOL DirectXAudioManager::Init(HWND hWnd, const char* SearchPath)
{
  lpDMPerformance = nullptr;
  lpDMLoader = nullptr;
  lpDMSegBackgroundMusic = nullptr;
  SndNr = 0;

  for (int i = 0; i < 10; i++)
  {
    lpDMSegSoundFX[i] = nullptr;
    SndFX[i] = nullptr;
  }

  if (FAILED(CoInitialize(nullptr)))
    return Error("Konnte COM-Bibliothek nicht initialisieren");

  if (FAILED(CoCreateInstance(
        CLSID_DirectMusicPerformance,
        nullptr,
        CLSCTX_INPROC,
        IID_IDirectMusicPerformance8,
        (void**)&lpDMPerformance)))
    return Error("DirectMusic-Performance konnte nicht erzeugt werden");

  if (FAILED(CoCreateInstance(
        CLSID_DirectMusicLoader,
        nullptr,
        CLSCTX_INPROC,
        IID_IDirectMusicLoader8,
        (void**)&lpDMLoader)))
    return Error("DirectMusic-Loader konnte nicht erzeugt werden");

  if (FAILED(lpDMPerformance->InitAudio(
        nullptr,
        nullptr,
        hWnd,
        DMUS_APATH_DYNAMIC_STEREO,
        32,
        DMUS_AUDIOF_ALL,
        nullptr)))
    return Error("Performance konnte nicht initialisiert werden");

  WCHAR wstrString[MAX_PATH];
  MultiByteToWideChar(CP_ACP, 0, SearchPath, -1, wstrString, MAX_PATH);

  if (FAILED(lpDMLoader->SetSearchDirectory(
        GUID_DirectMusicAllTypes, wstrString, FALSE)))
    return Error(
      "Suchverzeichnis für Sound-Dateien nicht gefunden (oder interner Programmfehler)");

  return TRUE;
}

DirectXAudioManager::~DirectXAudioManager()
{
  for (int i = 0; i < 10; i++)
  {
    if (nullptr != lpDMSegSoundFX[i])
    {
      lpDMSegSoundFX[i]->Release();
      lpDMSegSoundFX[i] = nullptr;
    }
  }

  if (nullptr != lpDMSegBackgroundMusic)
  {
    lpDMSegBackgroundMusic->Release();
    lpDMSegBackgroundMusic = nullptr;
  }

  if (nullptr != lpDMPerformance)
  {
    lpDMPerformance->Stop(nullptr, nullptr, 0, 0);
    lpDMPerformance->CloseDown();
    lpDMPerformance->Release();
    lpDMPerformance = nullptr;
  }

  if (nullptr != lpDMLoader)
  {
    lpDMLoader->Release();
    lpDMLoader = nullptr;
  }

  CoUninitialize();
}

BOOL DirectXAudioManager::AddBGMusic(const char* Filename)
{
  WCHAR wstrString[MAX_PATH];

  MultiByteToWideChar(CP_ACP, 0, Filename, -1, wstrString, MAX_PATH);

  if (FAILED(lpDMLoader->LoadObjectFromFile(
        CLSID_DirectMusicSegment,
        IID_IDirectMusicSegment8,
        wstrString,
        (LPVOID*)&lpDMSegBackgroundMusic)))
  {
    return Error(
      "Musik-Datei nicht gefunden! (oder falsches Format, beschädigte Datei o.a.)");
  }

  if (nullptr != lpDMSegBackgroundMusic)
  {
    lpDMSegBackgroundMusic->SetParam(
      GUID_StandardMIDIFile, 0xFFFFFFFF, DMUS_SEG_ALLTRACKS, 0, nullptr);
    lpDMSegBackgroundMusic->Download(lpDMPerformance);
    lpDMSegBackgroundMusic->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
  }

  return TRUE;
}

BOOL DirectXAudioManager::AddSndFX(const char* Filename)
{
  WCHAR wstrString[MAX_PATH];

  MultiByteToWideChar(CP_ACP, 0, Filename, -1, wstrString, MAX_PATH);

  if (FAILED(lpDMLoader->LoadObjectFromFile(
        CLSID_DirectMusicSegment,
        IID_IDirectMusicSegment8,
        wstrString,
        (LPVOID*)&lpDMSegSoundFX[SndNr])))
  {
    return Error(
      "Sound-Datei nicht gefunden! (oder falsches Format, beschädigte Datei o.a.");
  }

  if (nullptr != lpDMSegSoundFX[SndNr])
  {
    lpDMSegSoundFX[SndNr]->Download(lpDMPerformance);

    SndFX[SndNr] = Filename;

    SndNr++;
  }

  return TRUE;
}

void DirectXAudioManager::PlaySndFX(const char* Name)
{
  if (nullptr != Name)
  {
    int i;
    for (i = 0; i < 10; i++)
    {
      if (strcmp(SndFX[i], Name) == 0)
        break;
    }

    if (i >= 10)
    {
      // char* str = Name;
      // strcat(str, " : Sound-Datei nicht gefunden");
      char str[50];
      sprintf(str, "%s : Sound-Datei nicht gefunden!", Name);

      Error(str);
      return;
    }

    lpDMPerformance->PlaySegmentEx(
      lpDMSegSoundFX[i],
      nullptr,
      nullptr,
      DMUS_SEGF_SECONDARY,
      0,
      nullptr,
      nullptr,
      nullptr);
  }
}

void DirectXAudioManager::SetGlobalVolume()
{
  long Volume = 0;

  lpDMPerformance->GetGlobalParam(GUID_PerfMasterVolume, &Volume, sizeof(long));
}
