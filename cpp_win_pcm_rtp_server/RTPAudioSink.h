#pragma once
#include "AudioSink.h"
#include "RTPAudioStream.h"

class RTPAudioSink : private AudioSink
{
	// Inherited via AudioSink
private:
	RTPAudioStream m_Stream;

public:
	RTPAudioSink();
	virtual void SignalStart() override;
	virtual HRESULT SetFormat(WAVEFORMATEX* format) override;
	virtual HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) override;
	virtual HRESULT LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) override;
};

