#pragma once
#include "AudioSink.h"

class LoopbackAudioSink sealed : private AudioSink
{
public:
	LoopbackAudioSink();

	// Inherited via AudioSink
public:
	virtual HRESULT SetFormat(WAVEFORMATEX* format) override;
	virtual HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) override;
	virtual HRESULT LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) override;
};

