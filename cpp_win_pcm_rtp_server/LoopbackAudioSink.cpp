#include "LoopbackAudioSink.h"

//const size_t bufferSize = 8000000;
const size_t bufferSize = 400000;

void* bufferStart;
size_t currentWritePointerOffset = 0;
size_t currentReadPointerOffset = 0;

size_t lastSeenWritePointerOffset = 0;

unsigned int numChannels = 2;
unsigned int bitDepth = 32;
BYTE* myData = NULL;
UINT32 myFrames = 0;
bool buffering = false;

LoopbackAudioSink::LoopbackAudioSink() {
	bufferStart = malloc(bufferSize);
}

HRESULT SetFormat(WAVEFORMATEX* format) {
	numChannels = format->nChannels;
	bitDepth = format->wBitsPerSample;
	return S_OK;
}

bool OverrunImminent(UINT32 accessByteCount) {
	return currentReadPointerOffset + accessByteCount > bufferSize;
}

HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) {
	if (OverrunImminent(numFramesAvailable)) {
		currentWritePointerOffset = 0;
	}
	auto pValue = (intptr_t)bufferStart;
	void* actual = (void*)(pValue + currentWritePointerOffset);
	memcpy(actual, pData, numFramesAvailable);
	currentWritePointerOffset += numFramesAvailable;
	return S_OK;
}

bool IsBufferStale() {
	return lastSeenWritePointerOffset == currentWritePointerOffset;
}

bool IsBufferSaturated(UINT32 numFramesAvailable) {
	return currentWritePointerOffset < currentReadPointerOffset
		|| OverrunImminent(numFramesAvailable);
}

HRESULT LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) {
	if (IsBufferStale()) {
		pData = NULL;
		return;
	}

	if (IsBufferSaturated(numFramesAvailable)) {
		currentReadPointerOffset = 0;
	}

	BYTE* actual = (BYTE*)((intptr_t)bufferStart + currentReadPointerOffset);
	memcpy(pData, actual, numFramesAvailable);
	currentReadPointerOffset = currentReadPointerOffset + numFramesAvailable;
	return S_OK;
};