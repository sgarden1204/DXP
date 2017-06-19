#pragma once

#include<windows.h>
#include<mmsystem.h>
#include<dsound.h>

class Sound
{

public:
	LPDIRECTSOUND8 lpDirectSound;
	LPDIRECTSOUNDBUFFER dsbSound;
	LPDIRECTSOUNDBUFFER TestSound;


	bool running;

public:
	Sound();
	~Sound();

	BOOL LoadWave(LPWSTR filename);
	BOOL DSoundInit(HWND hWnd);

	void ReleaseDSound();
	void PlayWave();
	void StopWave();
	
};

