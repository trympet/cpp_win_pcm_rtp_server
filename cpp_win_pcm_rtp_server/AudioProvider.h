#pragma once
#include "AudioSink.h"
class AudioProvider
{
public:
	AudioProvider();
	HRESULT PlayAudioStream(AudioSink* pMySource);
	HRESULT RecordAudioStream(AudioSink* pMySource);
};

