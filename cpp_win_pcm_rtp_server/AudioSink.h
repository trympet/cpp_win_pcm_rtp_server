#pragma once
#include <audiopolicy.h>
class AudioSink
{
public:
	virtual void SignalStart();
	virtual HRESULT SetFormat(WAVEFORMATEX* format);
	virtual HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) = 0;
	virtual HRESULT LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) = 0;
};

