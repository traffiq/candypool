#include"./JHLib.h"



void packetBuffer_init(packetBuffer_t *pb, void *mem, uint32_t limit)
{
	pb->limit = limit;
	pb->cLen = 0;
	pb->index = 0;
	pb->sendBuffer = (uint8*)mem;
}

void packetBuffer_reset(packetBuffer_t *pb)
{
	pb->cLen = 0;
	pb->index = 0;
}

void packetBuffer_addUINT8(packetBuffer_t *pb, uint8 b)
{
	pb->sendBuffer[pb->cLen] = b;
	pb->cLen++;
}

void packetBuffer_addUINT16(packetBuffer_t *pb, uint16 d)
{
	*(uint16*)&(pb->sendBuffer[pb->cLen]) = d;
	pb->cLen += 2;
}

void packetBuffer_adduint32_t(packetBuffer_t *pb, uint32_t d)
{
	*(uint32_t*)&(pb->sendBuffer[pb->cLen]) = d;
	pb->cLen += 4;
}

void packetBuffer_addMemory(packetBuffer_t *pb, unsigned char *d, int len) //Can be optimized
{
	for(int i=0; i<len; i++)
		packetBuffer_addUINT8(pb, d[i]);
}

void packetBuffer_setUINT16(packetBuffer_t *pb, uint32_t offset, uint16 d)
{
	*(uint16*)&(pb->sendBuffer[offset]) = d;
}

void packetBuffer_setuint32_t(packetBuffer_t *pb, uint32_t offset, uint32_t d)
{
	*(uint32_t*)&(pb->sendBuffer[offset]) = d;
}

uint8 packetBuffer_readUINT8(packetBuffer_t *pb)
{
	uint8 v = *(uint8*)(pb->sendBuffer+pb->index);
	pb->index++;
	return v;
}

uint16 packetBuffer_readUINT16(packetBuffer_t *pb)
{
	uint16 v = *(uint16*)(pb->sendBuffer+pb->index);
	pb->index += 2;
	return v;
}

uint32_t packetBuffer_readuint32_t(packetBuffer_t *pb)
{
	uint32_t v = *(uint32_t*)(pb->sendBuffer+pb->index);
	pb->index += 4;
	return v;
}

void packetBuffer_setReadPointer(packetBuffer_t *pb, uint32_t offset)
{
	pb->index = offset;
}


void* packetBuffer_get(packetBuffer_t *pb)
{
	return pb->sendBuffer;
}

uint32_t packetBuffer_length(packetBuffer_t *pb)
{
	return pb->cLen;
}



//void PacketOut_Send(packetBuffer_t *pb, SOCKET s)
//{
//	//Set 4-byte len
//	*(unsigned int*)pb->sendBuffer = pb->cLen - 4;
//	//Send
//	send(s, (char*)pb->sendBuffer, pb->cLen, 0);
//}