#pragma once

#include<windows.h>
#include<mmsystem.h>

#include<dsound.h>

#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME)

#endif

#define DSVOLUME_TO_DB(volume) ((DWORD)(-30*(100-volume)))

class Sound
{

public:
	LPDIRECTSOUND8 g_lpDS;
	BOOL g_bPlay;

public:
	Sound();
	~Sound();

	BOOL CreateDirectSound(HWND hWnd);
	BOOL LoadWave(LPWSTR lpFileName, LPDIRECTSOUNDBUFFER * lpDSBuffer);
	BOOL SetVolume(LPDIRECTSOUNDBUFFER lpDSBuffer, LONG lVolume);
	BOOL SetPan(LPDIRECTSOUNDBUFFER lpDSBuffer, LONG lPan);

	void DeleteDirectSound();
	void Play(LPDIRECTSOUNDBUFFER lpDSBuffer, BOOL Loop);
	void Stop(LPDIRECTSOUNDBUFFER lpDSBuffer);

};

