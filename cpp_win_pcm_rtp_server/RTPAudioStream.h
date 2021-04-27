#pragma once
#include "RTPPacketFactory.h"
#include <WinRTBase.h>
class RTPAudioStream
{
public:
	RTPAudioStream();
public:
	HRESULT Connect(const char* destination, int port);
	HRESULT ProvideNextData(BYTE* pData, UINT32 nBytes);

private:
	SOCKET* CreateSocket();
	void InitWinSock();
	void CreateAddress(int port, const char* destination);
	HRESULT Send(BYTE* data, UINT32 nBytes);
};

