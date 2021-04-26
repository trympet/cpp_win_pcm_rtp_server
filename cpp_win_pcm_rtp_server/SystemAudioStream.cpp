#include "SystemAudioStream.h"

HRESULT hr;

REFERENCE_TIME hnsRequestedDuration;
REFERENCE_TIME hnsActualDuration;
REFERENCE_TIME refTimesPerMilliSecond;

UINT32 bufferFrameCount;
UINT32 numFramesAvailable;
UINT32 numFramesPadding;

IAudioClient* pAudioClient = NULL;

IAudioRenderClient* pRenderClient = NULL;
IAudioCaptureClient* pCaptureClient = NULL;

UINT32 packetLength = 0;
BOOL bDone = FALSE;
BYTE* pData;
DWORD flags;
int sampleRate;

bool firstLoop = true;

void Exit() {
	SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pRenderClient)
		SAFE_RELEASE(pCaptureClient)
		throw hr;
}

void LoadBufferFrameCount() {
	// Get the actual size of the allocated buffer.
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr)
}

void GetCaptureBuffer() {
	// Get the available data in the shared buffer.
	hr = pCaptureClient->GetBuffer(
		&pData,
		&numFramesAvailable,
		&flags, NULL, NULL);
}

void GetPlaybackService() {
	hr = pAudioClient->GetService(
		IID_IAudioRenderClient,
		(void**)&pRenderClient);
	EXIT_ON_ERROR(hr)
}

void GetCaptureService() {
	hr = pAudioClient->GetService(
		IID_IAudioCaptureClient,
		(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr)
}

void SetHnsDuration() {
	// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = (double)hnsRequestedDuration *
		bufferFrameCount / sampleRate;
}

void GetPlaybackBuffer() {
	// Grab the entire buffer for the initial fill operation.
	hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
	EXIT_ON_ERROR(hr)
}

void SystemAudioStream::ReleasePlaybackBuffer() {
	hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
	EXIT_ON_ERROR(hr)
}

void StartAudioClient() {
	hr = pAudioClient->Start();  // Start playing.
	EXIT_ON_ERROR(hr)
}

void GetPlaybackPadding() {
	// See how much buffer space is available.
	hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
	EXIT_ON_ERROR(hr)
}

SystemAudioStream::SystemAudioStream(IAudioClient* audioClient, REFERENCE_TIME timesPerSecond, REFERENCE_TIME timesPerMilliSecond, DWORD nSampleRate)
{
	pAudioClient = audioClient;
	hnsRequestedDuration = timesPerSecond;
	refTimesPerMilliSecond = timesPerMilliSecond;
}

UINT32 SystemAudioStream::getBufferFrameCount()
{
	return bufferFrameCount;
}

UINT32 SystemAudioStream::getFramesAvailable() {
	return numFramesAvailable;
}

BYTE* SystemAudioStream::getDataBuffer()
{
	return pData;;
}

DWORD* SystemAudioStream::getPlaybackFlags()
{
	return &flags;
}

DWORD SystemAudioStream::getSleepTime() {
	return (DWORD)(hnsActualDuration / refTimesPerMilliSecond / 2);
}

bool SystemAudioStream::isAnyCaptureData() {
	return packetLength != 0;
}

void SystemAudioStream::PreparePlaybackBuffer() {
	LoadBufferFrameCount();
	GetPlaybackService();
	GetPlaybackBuffer();
}

void SystemAudioStream::PrepareCaptureBuffer() {
	LoadBufferFrameCount();
	GetCaptureService();
}

void SystemAudioStream::StartPlayback() {
	hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
	EXIT_ON_ERROR(hr)
		SetHnsDuration();
	StartAudioClient();
}

void SystemAudioStream::StartCapture() {
	SetHnsDuration();
	StartAudioClient();
}

void SystemAudioStream::LoadPlaybackBuffer() {
	GetPlaybackPadding();
	numFramesAvailable = bufferFrameCount - numFramesPadding;
	GetPlaybackBuffer();
}

void SystemAudioStream::PrepareCaptureFrame() {
	hr = pCaptureClient->GetNextPacketSize(&packetLength);
	EXIT_ON_ERROR(hr)
}

void SystemAudioStream::LoadCaptureBuffer() {
	// Get the available data in the shared buffer.
	hr = pCaptureClient->GetBuffer(
		&pData,
		&numFramesAvailable,
		&flags, NULL, NULL);
	EXIT_ON_ERROR(hr)
		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
		{
			pData = NULL;  // Tell CopyData to write silence.
		}
}

void SystemAudioStream::ReleaseCaptureBuffer() {
	hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
	EXIT_ON_ERROR(hr)
}
