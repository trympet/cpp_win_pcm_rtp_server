#pragma once
#include <audiopolicy.h>
#include "AudioSink.h"

class LoopbackAudioSink : public AudioSink
{
	// Inherited via AudioSink
	virtual HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) override;
	virtual HRESULT LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) override;
};

