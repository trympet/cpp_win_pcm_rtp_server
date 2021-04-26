#pragma once
struct RTPPacket {
	unsigned int part_zero;
	unsigned int part_one;
	unsigned int part_two;
};

class RTPPacketFactory
{
private:
	inline void SetBit(RTPPacket* pPacket, int bitNumber) {
		pPacket->part_one |= 1 << bitNumber;
	}

	inline void SetSequenceNumber(RTPPacket* pPacket, unsigned short value) {
		pPacket->part_one |= value << 16;
	}

public:
	RTPPacket* CreatePacket();
};

