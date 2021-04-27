#include "RTPAudioSink.h"
#include "RTPPacketFactory.h"

int sizeFrame = 1;

RTPAudioSink::RTPAudioSink() {
	//m_Stream.Connect("127.0.0.1", 1234);
	m_Stream.Connect("172.18.91.121", 1234);
}

HRESULT RTPAudioSink::SetFormat(WAVEFORMATEX* format)
{
	sizeFrame = format->nChannels * format->wBitsPerSample / 8;
	return S_OK;
}

HRESULT RTPAudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone)
{
	return m_Stream.ProvideNextData(pData, numFramesAvailable * sizeFrame);

	//return S_OK;
}

void RTPAudioSink::SignalStart()
{
}

HRESULT RTPAudioSink::LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone)
{
	return E_NOTIMPL;
}
