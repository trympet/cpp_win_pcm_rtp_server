#include "RTPAudioStream.h"
#include <winsock.h>
#include <thread>

RTPPacketFactory factory;

SOCKET s;
SOCKET* m_pSocket = NULL;
WSADATA m_wsa;
sockaddr_in m_addr;
sockaddr_in m_other;

const int m_lAddr = sizeof(m_addr);

HRESULT RTPAudioStream::ProvideNextData(BYTE* pData, UINT32 nBytes)
{
	UINT32 nActual = nBytes;
	RTPPacket* pPacket = factory.CreatePacket(pData, &nActual);
	return Send((BYTE*)pPacket, nActual);
}

RTPAudioStream::RTPAudioStream()
{
}

HRESULT RTPAudioStream::Connect(const char* destination, int port)
{
	// Initialize winsock
	InitWinSock();
	m_pSocket = CreateSocket();
	CreateAddress(port, destination);
	//if (bind(s, (sockaddr*)&m_addr, m_lAddr) == SOCKET_ERROR)
	//{
	//	printf("Bind failed with error code : %d", WSAGetLastError());
	//	exit(EXIT_FAILURE);
	//}
	return S_OK;
}

void RTPAudioStream::InitWinSock()
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
}

SOCKET* RTPAudioStream::CreateSocket()
{
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	//if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	//{
	//	printf("Could not create socket : %d", WSAGetLastError());
	//}
	return &s;
}

void RTPAudioStream::CreateAddress(int port, const char* destination)
{
	memset((char*)&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	m_addr.sin_addr.S_un.S_addr = inet_addr(destination);
	//m_addr.sin_family = AF_INET;
	//m_addr.sin_addr.s_addr = INADDR_ANY;
	//m_addr.sin_port = htons(port);
}

HRESULT RTPAudioStream::Send(BYTE* pData, UINT32 nBytes)
{
	//char* emptyBuf = new char[1];
	//int recv_len;
	//int slen = sizeof(m_other);
	//if ((recv_len = recvfrom(s, emptyBuf, 1, 0, (struct sockaddr*)&m_other, &slen) == SOCKET_ERROR))
	//{
	//	printf("recvfrom() failed with error code : %d", WSAGetLastError());
	//	exit(EXIT_FAILURE);
	//}
	//if (m_other.sin_addr.S_un.S_addr == 0) {
	//	return S_OK;
	//}

	//printf("Received packet from %s:%d\n", inet_ntoa(m_other.sin_addr), ntohs(m_other.sin_port));

	if (sendto(s, (const char*)pData, nBytes, 0, (sockaddr*)&m_addr, m_lAddr) == SOCKET_ERROR) {
		return WSAGetLastError();
	}

	return S_OK;
}
