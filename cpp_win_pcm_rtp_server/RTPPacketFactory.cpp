#include "RTPPacketFactory.h"

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

unsigned short currentSequenceNumber = 0;

RTPPacket* RTPPacketFactory:: CreatePacket() {
	RTPPacket result;
	RTPPacket* pResult = &result;
	// set version 2
	RTPPacketFactory::SetBit(pResult, 1);
	// set payload type 10
	SetBit(pResult, 12);
	SetBit(pResult, 14);
	SetSequenceNumber(pResult, currentSequenceNumber++);

	return pResult;
}