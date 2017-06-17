#include "Sound.h"



Sound::Sound()
{
	g_lpDS = NULL;
	g_bPlay = FALSE;
}


Sound::~Sound()
{
}

BOOL Sound::CreateDirectSound(HWND hWnd)
{
	if (DirectSoundCreate8(NULL, &g_lpDS, NULL) != DS_OK)
		return FALSE;

	if (g_lpDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL) != DS_OK)
		return FALSE;

	return TRUE;
}

void Sound::DeleteDirectSound()
{
	g_lpDS->Release();
	g_lpDS = NULL;
}

BOOL Sound::LoadWave(LPWSTR lpFileName, LPDIRECTSOUNDBUFFER* lpDSBuffer)
{
	HMMIO hmmio;
	MMCKINFO ckInRIFF,ckIn;
	PCMWAVEFORMAT pcmWaveFormat;
	WAVEFORMATEX* pWaveFormat;

	hmmio = mmioOpen(lpFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmio == NULL) return FALSE;

	ckInRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if ((mmioDescend(hmmio, &ckInRIFF, NULL, MMIO_FINDRIFF)) != 0)
	{
		mmioClose(hmmio, 0);
		return FALSE;
	}

	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmio,&ckIn, &ckInRIFF, MMIO_FINDCHUNK) != 0)
	{
		mmioClose(hmmio, 0);
		return FALSE;
	}

	if (mmioRead(hmmio, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
	{
		mmioClose(hmmio, 0);
	}
	
	pWaveFormat = new WAVEFORMATEX;

	memcpy(pWaveFormat, &pcmWaveFormat, sizeof(pcmWaveFormat));
	pWaveFormat->cbSize = 0;

	if (mmioAscend(hmmio, &ckIn, 0))
	{
		mmioClose(hmmio, 0);
		return FALSE;
	}

	BYTE * pData = NULL;
	pData = new BYTE[ckIn.cksize];
	mmioRead(hmmio, (LPSTR)pData, ckIn.cksize);

	mmioClose(hmmio, 0);

	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLDEFAULT | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes = ckIn.cksize;
	dsbd.lpwfxFormat = pWaveFormat;

	if (g_lpDS->CreateSoundBuffer(&dsbd, lpDSBuffer, NULL) != DS_OK)
		return FALSE;

	VOID *pBuff1 = NULL;
	VOID *pBuff2 = NULL;
	DWORD dwLength;
	DWORD dwLength2;

	if ((*lpDSBuffer)->Lock(0, dsbd.dwBufferBytes, &pBuff1, &dwLength, &pBuff2, &dwLength2, 0L) != DS_OK)
	{
		(*lpDSBuffer)->Release();
		(*lpDSBuffer) = NULL;
		return FALSE;
	}

	memcpy(pBuff1, pData, dwLength);
	memcpy(pBuff2, (pData + dwLength), dwLength2);

	(*lpDSBuffer)->Unlock(pBuff1, dwLength, pBuff2, dwLength2);
	pBuff1 = pBuff2 = NULL;

	delete[] pData;
	delete[] pWaveFormat;

	return TRUE;

}

void Sound::Play(LPDIRECTSOUNDBUFFER lpDSBuffer, BOOL Loop)
{
	if (lpDSBuffer == NULL) return;

	if (!lpDSBuffer->Play(0, 0, (Loop) ? 1 : 0)) return;

	g_bPlay = TRUE;
}

void Sound::Stop(LPDIRECTSOUNDBUFFER lpDSBuffer)
{
	if (lpDSBuffer == NULL) return;

	lpDSBuffer->Stop();

	g_bPlay = FALSE;
	lpDSBuffer->SetCurrentPosition(0L);
}

BOOL Sound::SetVolume(LPDIRECTSOUNDBUFFER lpDSBuffer, LONG lVolume)
{
	if (lpDSBuffer->SetVolume(lVolume))
		return FALSE;

	return TRUE;
}

BOOL Sound::SetPan(LPDIRECTSOUNDBUFFER lpDSBuffer, LONG lPan)
{
	if (lpDSBuffer->SetPan(lPan) != DS_OK)
		return FALSE;

	return TRUE;
}