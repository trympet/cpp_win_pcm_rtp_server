#include "DebugAudioSink.h"

DebugAudioSink::DebugAudioSink()
{
}

HRESULT DebugAudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone)
{
    return E_NOTIMPL;
}

HRESULT DebugAudioSink::LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone)
{
    return E_NOTIMPL;
}
