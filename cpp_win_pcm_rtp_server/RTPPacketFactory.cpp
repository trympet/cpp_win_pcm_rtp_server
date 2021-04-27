#include "RTPPacketFactory.h"
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>

const int VERSION = 0;
const int VERSION_L = 0;
const int PADDING = 2;
const int PADDING_L = 1;
const int EXTENSION = 3;
const int EXTENSION_L = 1;
const int CC = 4;
const int CC_L = 4;
const int MARKER = 8;
const int MARKER_L = 1;
const int PT = 9;
const int PT_L = 7;
const int SEQ = 16;
const int SEQ_L = 16;

const int SAMPLE_RATE = 48000;

unsigned short seqNumber = 0;
unsigned int timestamp = 0;
unsigned int SSRC = 0;
unsigned int timesInvoked = 0;

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
		//std::this_thread::sleep_for(std::chrono::nanoseconds(200));
		Sleep(1000);
		std::cout << timesInvoked;
		timesInvoked = 0;
		//timestamp += 1;
		//printf("a");
	}
}

RTPPacketFactory::RTPPacketFactory()
{
	//auto clockThread = std::thread(&Clock);
	threads.push_back(std::thread(&Clock));
	SSRC = rand();
}

RTPPacket* RTPPacketFactory::CreatePacket(BYTE* pData, UINT32* nData) {
	timesInvoked += 1;
	UINT32 packetSize = *nData / 2 + 12;
	void* pPacket = malloc(packetSize);
	if (pPacket == NULL) {
		return NULL;
	}

	memset(pPacket, 0, packetSize);

	RTPPacket* pResult = (struct RTPPacket*)pPacket;
	SetPartZero(pResult);
	// Part one
	//timestamp++;
	SetTimestamp(pResult, timestamp);
	// Part two;
	SetSSRC(pResult, SSRC);
	CompressData(pData, (BYTE*)((intptr_t)pPacket + 12), nData);
	//memcpy((void*)((intptr_t)pPacket + 12), pData, *nData);
	timestamp += 444;

	*nData = packetSize;
	return pResult;
}

void RTPPacketFactory::SetPartZero(RTPPacket* pResult)
{
	//// set version 2
	//RTPPacketFactory::SetBit(pResult, 0);
	//// set payload type 10
	//SetBit(pResult, 12);
	//SetBit(pResult, 14);
	// set version 2
	SetBit(pResult, 6);
	// set payload type 10
	SetBit(pResult, 8);
	SetBit(pResult, 10);

	// Set seq
	SetSequenceNumber(pResult, seqNumber++);
}

void RTPPacketFactory::CompressData(BYTE* pData, BYTE* pPacket, UINT32* nData) {
	unsigned short* pDataShort = (unsigned short*)pData;
	unsigned short* pPacketShort = (unsigned short*)pPacket;
	auto nShort = *nData / 4;
	unsigned int* pDataInt = (unsigned int*)pData;
	unsigned int* pPacketInt = (unsigned int*)pPacket;
	auto nInt = *nData / 8;
	unsigned long long* pDataLong = (unsigned long long*)pData;
	unsigned long long* pPacketLong = (unsigned long long*)pPacket;
	auto nLong = *nData / 16;
	for (size_t i = 0; i < nShort; i++)
	{
		pPacketShort[i] = pDataInt[i] / pow(2, 8);
		if (i % 2 == 0) {
			pPacketInt[i / 2] = _byteswap_ulong(pPacketInt[i / 2]);
		}
		//pPacketShort[i * 2] = _byteswap_ushort(pPacketShort[i * 2]);
		//pPacketShort[i * 2 + 1] = _byteswap_ushort(pPacketShort[i * 2 + 1]);
	}

	//unsigned long long* pTest = (unsigned long long*)pData;
	//unsigned int* pTestP = (unsigned int*)pPacket;
	//UINT32 nBytesCompressed = *nData / 8;
	//UINT32 nShortsCompressed = *nData / 2;
	//for (size_t i = 0; i < nBytesCompressed; i++)
	//{
	//	unsigned long long value = pTest[i];
	//	unsigned int compressed = value / (pow(2, 32));
	//	//pPacket[i] = compressed;
	//	pTestP[i] = _byteswap_ulong(compressed);
	//}
}