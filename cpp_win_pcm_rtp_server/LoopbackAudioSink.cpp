#include "LoopbackAudioSink.h"

const size_t bufferSize = 8000000;
//const size_t bufferSize = 400000;

void* bufferStart;
size_t currentWritePointerOffset = 0;
size_t currentReadPointerOffset = 0;
size_t lastSeenWritePointerOffset = 0;

unsigned int numChannels = 2;
unsigned int bitDepth = 32;
unsigned int frameSize = 1;

LoopbackAudioSink::LoopbackAudioSink() {
	bufferStart = malloc(bufferSize);
}

bool OverrunImminent(size_t offset, UINT32 accessByteCount) {
	return offset + accessByteCount > bufferSize;
}

bool IsBufferStale() {
	return lastSeenWritePointerOffset == currentWritePointerOffset;
}
HRESULT LoopbackAudioSink::SetFormat(WAVEFORMATEX* format) {
	numChannels = format->nChannels;
	bitDepth = format->wBitsPerSample;
	frameSize = numChannels * bitDepth / 8;
	return S_OK;
}

HRESULT LoopbackAudioSink::CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone)
{
	UINT32 byteCount = numFramesAvailable * frameSize;

	if (OverrunImminent(currentWritePointerOffset, byteCount)) {
		currentWritePointerOffset = 0;
	}
	intptr_t pValue = (intptr_t)bufferStart;
	void* actual = (void*)(pValue + currentWritePointerOffset);
	memcpy(actual, pData, byteCount);
	currentWritePointerOffset += byteCount;
	return S_OK;
}

HRESULT LoopbackAudioSink::LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone)
{
	UINT32 byteCount = numFramesAvailable * frameSize;

	//if (IsBufferStale()) {
	//	pData = NULL;
	//	return S_OK;
	//}
	lastSeenWritePointerOffset = currentWritePointerOffset;

	if (OverrunImminent(currentReadPointerOffset, byteCount)) {
		currentReadPointerOffset = 0;
	}

	if (currentWritePointerOffset < currentReadPointerOffset) {
		currentReadPointerOffset = currentWritePointerOffset;
	}

	BYTE* actual = (BYTE*)((intptr_t)bufferStart + currentReadPointerOffset);
	memcpy(pData, actual, byteCount);
	currentReadPointerOffset += byteCount;
	return S_OK;
}
