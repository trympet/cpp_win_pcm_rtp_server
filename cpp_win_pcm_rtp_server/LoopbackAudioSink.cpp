#include "LoopbackAudioSink.h"

BYTE* myData;
UINT32 myFrames;
bool done;

HRESULT LoopbackAudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone)
{
	myData = pData;
	myFrames = numFramesAvailable;
	done = false;
	return S_OK;
}

HRESULT LoopbackAudioSink::LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone)
{
	if (myData == NULL) {
		DWORD silent = AUDCLNT_BUFFERFLAGS_SILENT;
		pDone = &silent;
	}
	else if (done || numFramesAvailable == 0) { return S_OK; }
	else {
		memcpy(pData, myData, min(numFramesAvailable, myFrames));
		done = true;
	}
	return S_OK;
}
