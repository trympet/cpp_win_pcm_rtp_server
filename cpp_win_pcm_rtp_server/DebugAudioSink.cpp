#include "DebugAudioSink.h"
#include "LoopbackAudioSink.h"
#include <iostream>
#include "RTPAudioSink.h"

LoopbackAudioSink loopbackSink;
RTPAudioSink rtpSink;


DebugAudioSink::DebugAudioSink()
{
}

HRESULT DebugAudioSink::SetFormat(WAVEFORMATEX* format) {
	return loopbackSink.SetFormat(format);
}

HRESULT DebugAudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone)
{
	//std::cout << "C";
	auto result = rtpSink.CopyData(pData, numFramesAvailable, pDone);
	if (result != S_OK) {
		return result;
	}
	return loopbackSink.CopyData(pData, numFramesAvailable, pDone);
}

HRESULT DebugAudioSink::LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone)
{
	//std::cout << "L";

	//return loopbackSink.LoadData(numFramesAvailable, pData, pDone);
	return S_OK;
}
