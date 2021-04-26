#include "DebugAudioSink.h"
#include "LoopbackAudioSink.h"
#include <iostream>

LoopbackAudioSink loopbackSink;

DebugAudioSink::DebugAudioSink()
{
}

HRESULT DebugAudioSink::SetFormat(WAVEFORMATEX* format) {
	return loopbackSink.SetFormat(format);
}

HRESULT DebugAudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone)
{
	std::cout << "C";

	return loopbackSink.CopyData(pData, numFramesAvailable, pDone);
}

HRESULT DebugAudioSink::LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone)
{
	std::cout << "L";

	return loopbackSink.LoadData(numFramesAvailable, pData, pDone);
}
