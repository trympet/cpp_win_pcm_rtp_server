#include "RTPPacketFactory.h"
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>

#define COMPRESSION_RATIO 2
#define AMPLIFICATION 2

unsigned short seqNumber = 0;
unsigned int timestamp = 0;
unsigned int SSRC = 0;
unsigned long long timesInvoked = 0;
unsigned long long timesInc = 0;

std::vector<std::thread> threads;
std::thread* pClockThread = NULL;

void SetSequenceNumber(RTPPacket* pPacket, unsigned short value) {
	//pPacket->part_zero |= value >> 32;
	pPacket->part_zero |= _byteswap_ushort(value) << 16;
}

void SetTimestamp(RTPPacket* pPacket, unsigned int value) {
	pPacket->part_one = _byteswap_ulong(value);
}

void SetSSRC(RTPPacket* pPacket, unsigned int value) {
	pPacket->part_two = _byteswap_ushort(value);
}

void Clock() {
	while (true) {
		Sleep(1000);
		std::cout << "CREATE: ";
		std::cout << timesInvoked;
		std::cout << " INC: ";
		std::cout << timesInc;
		std::cout << "\n";
		timesInvoked = 0;
		timesInc = 0;
	}
}

RTPPacketFactory::RTPPacketFactory()
{
	threads.push_back(std::thread(&Clock));
	SSRC = rand();
}

RTPPacket* RTPPacketFactory::CreatePacket(BYTE* pData, UINT32* nData) {
#ifdef DEBUG
	timesInvoked += 1;
#endif

	UINT32 packetSize = *nData / COMPRESSION_RATIO + 12;
	void* pPacket = malloc(packetSize);
	if (pPacket == NULL) {
		return NULL;
	}

	memset(pPacket, 0, packetSize);
	RTPPacket* pResult = (struct RTPPacket*)pPacket;
	SetHeader(pResult);

	CompressData(pData, (BYTE*)((intptr_t)pPacket + 12), nData);

	IncrementTimestamp(*nData);

	*nData = packetSize;
	return pResult;
}

void RTPPacketFactory::IncrementTimestamp(UINT32 nData) {
	if (nData == 3840) {
		timestamp += 441 * 1;
	}
	else {
		timestamp += 440 * 2;
	}
}

void RTPPacketFactory::SetHeader(RTPPacket* pPacket) {
	// first dword
	SetPartZero(pPacket);
	// second dword
	SetTimestamp(pPacket, timestamp);
	// third dword
	SetSSRC(pPacket, SSRC);
}


void RTPPacketFactory::SetPartZero(RTPPacket* pResult)
{
	// set version 2
	SetBit(pResult, 6);
	// set payload type 10
	SetBit(pResult, 8);
	SetBit(pResult, 10);
	// Set seq
	SetSequenceNumber(pResult, seqNumber++);
}

void RTPPacketFactory::CompressData(BYTE* pData, BYTE* pPacket, UINT32* nData) {
	float* pDataFloat = (float*)pData;
	unsigned short* pPacketShort = (unsigned short*)pPacket;
	const auto nShort = *nData / (COMPRESSION_RATIO * 2);
	for (size_t i = 0; i < nShort; i++)
	{
		pPacketShort[i] = (short int)(pDataFloat[i] * 32767);
	}

	for (size_t i = 0; i < nShort; i++)
	{
		pPacketShort[i] = _byteswap_ushort(pPacketShort[i] * AMPLIFICATION);

	}
}