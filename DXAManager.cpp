#include "DXAManager.h"
#include <stdio.h>

BOOL DirectXAudioManager::Init(HWND hWnd, char* SearchPath)
{
	lpDMPerformance        = NULL; 
	lpDMLoader             = NULL; 
	lpDMSegBackgroundMusic = NULL; 
	SndNr                  = 0;

	for (int i = 0; i < 10; i++) 
	{
		lpDMSegSoundFX[i] = NULL; 
		SndFX[i]          = NULL;
	}
			
	if (FAILED(CoInitialize(NULL)))
		return Error("Konnte COM-Bibliothek nicht initialisieren");

	if (FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&lpDMPerformance)))
		return Error("DirectMusic-Performance konnte nicht erzeugt werden");

	if (FAILED(CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8, (void**)&lpDMLoader)))
		return Error("DirectMusic-Loader konnte nicht erzeugt werden");
	
	if (FAILED(lpDMPerformance->InitAudio(NULL, NULL, hWnd, DMUS_APATH_DYNAMIC_STEREO, 32, DMUS_AUDIOF_ALL, NULL)))
		return Error("Performance konnte nicht initialisiert werden");

	WCHAR wstrString[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, SearchPath, -1, wstrString, MAX_PATH);
	
	if (FAILED(lpDMLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wstrString, FALSE)))
		return Error("Suchverzeichnis für Sound-Dateien nicht gefunden (oder interner Programmfehler)"); 

	return TRUE;
}

DirectXAudioManager::~DirectXAudioManager()
{
	for (int i = 0; i < 10; i++)
	{
		if (NULL != lpDMSegSoundFX[i])
		{
			lpDMSegSoundFX[i]->Release();
			lpDMSegSoundFX[i] = NULL;
		}
	}
	
	if (NULL != lpDMSegBackgroundMusic)
	{
		lpDMSegBackgroundMusic->Release();
		lpDMSegBackgroundMusic = NULL;
	}
	
	if (NULL != lpDMPerformance)
	{
		lpDMPerformance->Stop(NULL, NULL, 0, 0);
		lpDMPerformance->CloseDown();
		lpDMPerformance->Release();
		lpDMPerformance = NULL;
	}

	if (NULL != lpDMLoader)
	{
		lpDMLoader->Release();
		lpDMLoader = NULL;
	}

	CoUninitialize();
}

BOOL DirectXAudioManager::AddBGMusic(char* Filename)
{
	WCHAR wstrString[MAX_PATH];

	MultiByteToWideChar(CP_ACP, 0, Filename, -1, wstrString, MAX_PATH);

	if (FAILED(lpDMLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, wstrString, (LPVOID*)&lpDMSegBackgroundMusic)))
	{
		return Error("Musik-Datei nicht gefunden! (oder falsches Format, beschädigte Datei o.a.)");
	}

	if (NULL != lpDMSegBackgroundMusic)
	{
		lpDMSegBackgroundMusic->SetParam(GUID_StandardMIDIFile, 0xFFFFFFFF, DMUS_SEG_ALLTRACKS, 0, NULL);
		lpDMSegBackgroundMusic->Download(lpDMPerformance);
		lpDMSegBackgroundMusic->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
	}

	return TRUE;
}

BOOL DirectXAudioManager::AddSndFX(char* Filename)
{
	WCHAR wstrString[MAX_PATH];

	MultiByteToWideChar(CP_ACP, 0, Filename, -1, wstrString, MAX_PATH);

	if (FAILED(lpDMLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, wstrString, (LPVOID*)&lpDMSegSoundFX[SndNr])))
	{
		return Error("Sound-Datei nicht gefunden! (oder falsches Format, beschädigte Datei o.a.");
	}

	if (NULL != lpDMSegSoundFX[SndNr])
	{
		lpDMSegSoundFX[SndNr]->Download(lpDMPerformance);
				
		SndFX[SndNr] = Filename;

		SndNr++;
	}
	
	return TRUE;

}

void DirectXAudioManager::PlaySndFX(char* Name)
{
	if (NULL != Name)
	{
		int i;
		for (i = 0; i < 10; i++)
		{
			if (strcmp(SndFX[i], Name) == 0)
				break;
		}

		if (i >= 10)
		{
			//char* str = Name;
			//strcat(str, " : Sound-Datei nicht gefunden");
			char str[50];
			sprintf(str, "%s : Sound-Datei nicht gefunden!", Name);

			Error(str);
			return;
		}	
	
		lpDMPerformance->PlaySegmentEx(lpDMSegSoundFX[i], NULL, NULL, DMUS_SEGF_SECONDARY, 0, NULL, NULL, NULL);
	}
}

void DirectXAudioManager::SetGlobalVolume(void)
{
	long Volume = 0;
	
	lpDMPerformance->GetGlobalParam(GUID_PerfMasterVolume, &Volume, sizeof(long));
}
