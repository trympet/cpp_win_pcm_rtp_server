#pragma once
#include "AudioSink.h"

class DebugAudioSink final : public AudioSink
{
public:
	DebugAudioSink();

	// Inherited via AudioSink
	virtual void SignalStart() override;
	virtual HRESULT SetFormat(WAVEFORMATEX* format) override;
	virtual HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) override;
	virtual HRESULT LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) override;
};

