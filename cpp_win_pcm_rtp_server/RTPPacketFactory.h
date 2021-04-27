#pragma once
#include <WinRTBase.h>
struct RTPPacket {
	unsigned int part_zero;
	unsigned int part_one;
	unsigned int part_two;
};

class RTPPacketFactory
{
public:
	RTPPacketFactory();
	RTPPacket* CreatePacket(BYTE* pData, UINT32* nData);


private:
	void SetHeader(RTPPacket* pPacket);
	void SetPartZero(RTPPacket* pResult);
	void IncrementTimestamp(UINT32 nData);
	void CompressData(BYTE* pData, BYTE* pPacket, UINT32* nData);
	inline void SetBit(RTPPacket* pPacket, int bitNumber) {
		//pPacket->part_zero |= 1 << 31 - bitNumber;
		pPacket->part_zero |= 1  << 1+ bitNumber;
	}
};

