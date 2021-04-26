#pragma once
#include <audiopolicy.h>
#include <mmdeviceapi.h>

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { Exit(); }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }


class SystemAudioStream
{
private:
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
	const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

public:
	SystemAudioStream(IAudioClient* audioClient, REFERENCE_TIME timesPerSecond, REFERENCE_TIME timesPerMilliSecond, DWORD sampleRate);

	UINT32 getBufferFrameCount();

	UINT32 getFramesAvailable();

	BYTE* getDataBuffer();

	DWORD* getPlaybackFlags();

	DWORD getSleepTime();

	bool isAnyCaptureData();

	void LoadPlaybackBuffer();

	void PreparePlaybackBuffer();

	void PrepareCaptureBuffer();

	void StartPlayback();

	void StartCapture();

	void PrepareCaptureFrame();

	void LoadCaptureBuffer();

	void ReleaseCaptureBuffer();

	void ReleasePlaybackBuffer();
};