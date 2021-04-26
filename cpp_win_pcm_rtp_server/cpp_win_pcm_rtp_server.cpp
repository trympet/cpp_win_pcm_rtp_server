// cpp_audio_sampler.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <audiopolicy.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <WinRTBase.h>
#include <thread>
#include "AudioProvider.h"
#include "LoopbackAudioSink.h"
#include <list>
#include <stack>
#include <queue>

#define REFTIMES_PER_SEC  1000000
#define REFTIMES_PER_MILLISEC  1000000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);


class MyAudioSink {
private:
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
	std::queue<BYTE> dataStack;
	bool buffering = false;

public:
	MyAudioSink() {
		bufferStart = malloc(bufferSize);
	}

	HRESULT SetFormat(WAVEFORMATEX* format) {
		numChannels = format->nChannels;
		bitDepth = format->wBitsPerSample;
		return S_OK;
	}

	HRESULT CopyData(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) {
		std::cout << "R";
		//copyBuffered(pData, numFramesAvailable * bitDepth * numChannels / 8, pDone);
		copyMemory(pData, numFramesAvailable * bitDepth * numChannels / 8, pDone);
		myData = pData;
		myFrames = numFramesAvailable;
		return S_OK;
	}

	void copyBuffered(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) {
		if (*pData == 0) {
			return;
		}
		for (UINT32 i = 0; i < numFramesAvailable; i++)
		{
			dataStack.push((pData)[i]);
			if (buffering) {
				i += 2 * bitDepth * numChannels / 8;
			}
		}
	}

	void copyMemory(BYTE* pData, UINT32 numFramesAvailable, BOOL* pDone) {
		if (currentWritePointerOffset + numFramesAvailable > bufferSize) {
			currentWritePointerOffset = 0;
		}
		auto pValue = (intptr_t)bufferStart;
		void* actual = (void*)(pValue + currentWritePointerOffset);
		memcpy(actual, pData, numFramesAvailable);
		currentWritePointerOffset += numFramesAvailable;
	}

	HRESULT LoadData(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) {
		std::cout << "P";
		//loadBuffered(numFramesAvailable * bitDepth * numChannels / 8, pData, pDone);
		loadMemory(numFramesAvailable * bitDepth * numChannels / 8, pData, pDone);
		pData = myData;
		//memcpy(pData, myData, min(numFramesAvailable, myFrames) * bitDepth * numChannels / 8);
		return S_OK;
	}

	void loadBuffered(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) {
		auto size = dataStack.size();
		if (dataStack.empty()) {
			pData = NULL;
			//DWORD silent = AUDCLNT_BUFFERFLAGS_SILENT;
			//pDone = &silent;
		}
		else if (numFramesAvailable == 0) { return; }
		else {
			for (size_t i = 0; i < min(numFramesAvailable, size); i++)
			{
				BYTE element = dataStack.front();
				//memcpy(pData + i, &element, 1);
				pData[i] = element;
				dataStack.pop();
				if (dataStack.size() > 100000) {
					buffering = true;
				}
				else {
					buffering = false;
				}
			}
		}
	}

	void loadMemory(UINT32 numFramesAvailable, BYTE* pData, DWORD* pDone) {
		if (lastSeenWritePointerOffset == currentWritePointerOffset) {
			pData = NULL;
			return;
		}

		if (currentWritePointerOffset < currentReadPointerOffset) {
				currentReadPointerOffset = 0;
		}

		if (currentReadPointerOffset + numFramesAvailable > bufferSize) {
			currentReadPointerOffset = 0;
		}
		BYTE* actual = (BYTE*)((intptr_t)bufferStart + currentReadPointerOffset);
		memcpy(pData, actual, numFramesAvailable);
		//pData = actual;
		int nextReadOffset = currentReadPointerOffset + numFramesAvailable;
		currentReadPointerOffset = nextReadOffset;
		//if (newOffset < (intptr_t)bufferStart) {
		//	lastBufferOffset = 0;
		//}
		//else {
		//	lastBufferOffset = newOffset;
		//}
	}
};

HRESULT PlayAudioStream(MyAudioSink* pMySource)
{
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioRenderClient* pRenderClient = NULL;
	WAVEFORMATEX* pwfx = NULL;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	UINT32 numFramesPadding;
	BYTE* pData;
	DWORD flags = 0;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

		hr = pEnumerator->GetDefaultAudioEndpoint(
			eRender, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

		hr = pDevice->Activate(
			IID_IAudioClient, CLSCTX_ALL,
			NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			0,
			hnsRequestedDuration,
			0,
			pwfx,
			NULL);
	EXIT_ON_ERROR(hr)

		// Tell the audio source which format to use.
		hr = pMySource->SetFormat(pwfx);
	EXIT_ON_ERROR(hr)

		// Get the actual size of the allocated buffer.
		hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->GetService(
			IID_IAudioRenderClient,
			(void**)&pRenderClient);
	EXIT_ON_ERROR(hr)

		// Grab the entire buffer for the initial fill operation.
		hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
	EXIT_ON_ERROR(hr)

		// Load the initial data into the shared buffer.
		hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
	EXIT_ON_ERROR(hr)

		hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
	EXIT_ON_ERROR(hr)

		// Calculate the actual duration of the allocated buffer.
		hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start playing.
	EXIT_ON_ERROR(hr)

		// Each loop fills about half of the shared buffer.
		while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
		{
			//// Sleep for half the buffer duration.
			auto sleepT = (DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);
			Sleep(sleepT);

			// See how much buffer space is available.
			hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
			EXIT_ON_ERROR(hr)

				numFramesAvailable = bufferFrameCount - numFramesPadding;

			// Grab all the available space in the shared buffer.
			hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
			EXIT_ON_ERROR(hr)

				// Get next 1/2-second of data from the audio source.
				hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
			EXIT_ON_ERROR(hr)

				hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
			EXIT_ON_ERROR(hr)
		}

	// Wait for last data in buffer to play before stopping.
	Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

	hr = pAudioClient->Stop();  // Stop playing.
	EXIT_ON_ERROR(hr)

		Exit:
	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pRenderClient)

		return hr;
}

HRESULT RecordAudioStream(MyAudioSink* pMySink)
{
	HRESULT hr;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	REFERENCE_TIME hnsActualDuration;
	UINT32 bufferFrameCount;
	UINT32 numFramesAvailable;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioClient* pAudioClient = NULL;
	IAudioCaptureClient* pCaptureClient = NULL;
	WAVEFORMATEX* pwfx = NULL;
	UINT32 packetLength = 0;
	BOOL bDone = FALSE;
	BYTE* pData;
	DWORD flags;

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr)

		hr = pEnumerator->GetDefaultAudioEndpoint(
			eCapture, eConsole, &pDevice);
	EXIT_ON_ERROR(hr)

		hr = pDevice->Activate(
			IID_IAudioClient, CLSCTX_ALL,
			NULL, (void**)&pAudioClient);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->GetMixFormat(&pwfx);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_SHARED,
			0,
			hnsRequestedDuration,
			0,
			pwfx,
			NULL);
	EXIT_ON_ERROR(hr)

		// Get the size of the allocated buffer.
		hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	EXIT_ON_ERROR(hr)

		hr = pAudioClient->GetService(
			IID_IAudioCaptureClient,
			(void**)&pCaptureClient);
	EXIT_ON_ERROR(hr)

		// Notify the audio sink which format to use.
		hr = pMySink->SetFormat(pwfx);
	EXIT_ON_ERROR(hr)

		// Calculate the actual duration of the allocated buffer.
		hnsActualDuration = (double)REFTIMES_PER_SEC *
		bufferFrameCount / pwfx->nSamplesPerSec;

	hr = pAudioClient->Start();  // Start recording.
	EXIT_ON_ERROR(hr)

		// Each loop fills about half of the shared buffer.
		while (bDone == FALSE)
		{
			// Sleep for half the buffer duration.
			Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

			hr = pCaptureClient->GetNextPacketSize(&packetLength);
			EXIT_ON_ERROR(hr)

				while (packetLength != 0)
				{
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

					// Copy the available capture data to the audio sink.
					hr = pMySink->CopyData(
						pData, numFramesAvailable, &bDone);
					EXIT_ON_ERROR(hr)

						hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
					EXIT_ON_ERROR(hr)

						hr = pCaptureClient->GetNextPacketSize(&packetLength);
					EXIT_ON_ERROR(hr)
				}
		}

	hr = pAudioClient->Stop();  // Stop recording.
	EXIT_ON_ERROR(hr)

		Exit:
	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pEnumerator)
		SAFE_RELEASE(pDevice)
		SAFE_RELEASE(pAudioClient)
		SAFE_RELEASE(pCaptureClient)

		return hr;
}

MyAudioSink sink;

void play() {
	PlayAudioStream(&sink);
}

void rec() {
	RecordAudioStream(&sink);
}

void old() {
	auto playbackThread = std::thread(&rec);
	Sleep(700);
	play();
}

AudioProvider* provider = NULL;
LoopbackAudioSink* loopbackSink = NULL;

void playNew() {
	provider->PlayAudioStream(loopbackSink);
}

void newM() {
	auto m_provider = AudioProvider();
	auto m_loopbackSink = LoopbackAudioSink();
	provider = &m_provider;
	loopbackSink = &m_loopbackSink;
	auto playbackThread = std::thread(&playNew);
	provider->RecordAudioStream(loopbackSink);
}

int main()
{
	std::cout << "Hello World!\n";
	auto hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	old();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
