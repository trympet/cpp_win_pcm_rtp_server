#include "AudioProvider.h"
#include "SystemAudioStream.h"

#define REFTIMES_PER_SEC  1
#define REFTIMES_PER_MILLISEC  10000

HRESULT hr_provider;

IMMDeviceEnumerator* pEnumerator = NULL;
IMMDevice* pDevice = NULL;
IAudioClient* pAudioClient_provider = NULL;
WAVEFORMATEX* pwfx = NULL;


void CreateEnumerator() {
	hr_provider = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
}

void GetAudioEndpoint() {
	hr_provider = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDevice);
}

void PrepareAudioClient() {
	hr_provider = pDevice->Activate(
		IID_IAudioClient, CLSCTX_ALL,
		NULL, (void**)&pAudioClient_provider);
}

void SetAudioFormat() {
	hr_provider = pAudioClient_provider->GetMixFormat(&pwfx);
}

void InitializeAudioClient() {
	hr_provider = pAudioClient_provider->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		0,
		REFTIMES_PER_SEC,
		0,
		pwfx,
		NULL);
}
AudioSink* pAudioSource;

void LoadIntoSource(SystemAudioStream* stream) {
	pAudioSource->LoadData(stream->getFramesAvailable(), stream->getDataBuffer(), stream->getPlaybackFlags());
}

void CopyIntoSource(SystemAudioStream* stream) {
	BOOL f = false;
	pAudioSource->CopyData(stream->getDataBuffer(), stream->getFramesAvailable(), &f);
}

AudioProvider::AudioProvider() {
	CreateEnumerator();
	GetAudioEndpoint();
	PrepareAudioClient();
	SetAudioFormat();
	InitializeAudioClient();
}

HRESULT AudioProvider::PlayAudioStream(AudioSink* pMySource)
{
	pAudioSource = pMySource;
	SystemAudioStream playbackStream = SystemAudioStream(pAudioClient_provider, REFTIMES_PER_SEC, REFTIMES_PER_MILLISEC, pwfx->nSamplesPerSec);
	playbackStream.PreparePlaybackBuffer();
	LoadIntoSource(&playbackStream);
	playbackStream.StartPlayback();
	while (true) {
		Sleep(playbackStream.getSleepTime());
		playbackStream.LoadPlaybackBuffer();
		LoadIntoSource(&playbackStream);
		playbackStream.ReleasePlaybackBuffer();
	}
}

HRESULT AudioProvider::RecordAudioStream(AudioSink* pMySource)
{
	pAudioSource = pMySource;
	SystemAudioStream captureStream = SystemAudioStream(pAudioClient_provider, REFTIMES_PER_SEC, REFTIMES_PER_MILLISEC, pwfx->nSamplesPerSec);
	captureStream.PrepareCaptureBuffer();
	captureStream.StartCapture();
	while (true) {
		Sleep(captureStream.getSleepTime());
		captureStream.PrepareCaptureFrame();
		while (captureStream.isAnyCaptureData()) {
			captureStream.LoadCaptureBuffer();
			CopyIntoSource(&captureStream);
			captureStream.ReleaseCaptureBuffer();
			captureStream.PrepareCaptureFrame();
		}
	}
}
