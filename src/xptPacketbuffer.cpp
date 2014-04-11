#include"global.h"
#include "xptServer.h"
#include "math.h"

/*
 * Changes the size limit of the packetBuffer to the given value
 */
void xptPacketbuffer_changeSizeLimit(xptPacketbuffer_t* pb, uint32_t sizeLimit) {
	fprintf(stderr, "<DEBUG> xptPacketbuffer_changeSizeLimit\n");
	pb->bufferLimit = sizeLimit;
	pb->bufferSize = fmin(pb->bufferSize, pb->bufferLimit);
	pb->parserIndex = fmin(pb->parserIndex, pb->bufferLimit);
	pb->buffer = (uint8_t*)realloc(pb->buffer, pb->bufferLimit);
}

/*
 * Starts writing a new packet
 */
void xptPacketbuffer_beginWritePacket(xptPacketbuffer_t* pb, uint8_t opcode) {
	uint32_t header = (opcode)&0x000000FF;
	*(uint32_t*)pb->buffer = header;
	pb->parserIndex = 4;
}

/*
 * Starts writing a new packet
 */
void xptPacketbuffer_beginReadPacket(xptPacketbuffer_t* pb) {
	pb->parserIndex = 4;
}

/*
 * Returns the amount of readable bytes remaining in the packet
 */
uint32_t xptPacketbuffer_getReadSize(xptPacketbuffer_t* pb) {
	return pb->bufferSize - pb->parserIndex;
}

/*
 * Reads a single float from the packet
 * Returns 0 on error
 */
float xptPacketbuffer_readFloat(xptPacketbuffer_t* pb, bool* error) {
	if ((pb->parserIndex+4) > pb->bufferSize) {
		pb->parserIndex = pb->bufferSize;
		*error = true;return 0;
	}
	float v = *(float*)(pb->buffer+pb->parserIndex);
	pb->parserIndex += 4;
	*error = false;
	return v;
}

/*
* Reads a single uint64 from the packet
* Returns 0 on error
*/
uint64 xptPacketbuffer_readU64(xptPacketbuffer_t* pb, bool* error)
{
	if( (pb->parserIndex+8) > pb->bufferSize )
	{
		pb->parserIndex = pb->bufferSize;
		*error = true;
		return 0;
	}

	uint64 v = *(uint64*)(pb->buffer+pb->parserIndex);
	pb->parserIndex += 8;
	*error = false;
	return v;
}


/*
 * Reads a single uint32_t from the packet
 * Returns 0 on error
 */
uint32_t xptPacketbuffer_readU32(xptPacketbuffer_t* pb, bool* error) {
	if ((pb->parserIndex+4) > pb->bufferSize) {
		pb->parserIndex = pb->bufferSize;
		*error = true;return 0;
	}
	uint32_t v = *(uint32_t*)(pb->buffer+pb->parserIndex);
	pb->parserIndex += 4;
	*error = false;
	return v;
}

/*
 * Reads a single uint16_t from the packet
 * Returns 0 on error
 */
uint16_t xptPacketbuffer_readU16(xptPacketbuffer_t* pb, bool* error) {
	if ((pb->parserIndex+2) > pb->bufferSize) {
		pb->parserIndex = pb->bufferSize;
		*error = true;return 0;
	}
	uint16_t v = *(uint16_t*)(pb->buffer+pb->parserIndex);
	pb->parserIndex += 2;
	*error = false;
	return v;
}

/*
 * Reads a single uint16_t from the packet
 * Returns 0 on error
 */
uint8_t xptPacketbuffer_readU8(xptPacketbuffer_t* pb, bool* error) {
	if ((pb->parserIndex+1) > pb->bufferSize) {
		pb->parserIndex = pb->bufferSize;
		*error = true;return 0;
	}
	uint8_t v = *(uint8_t*)(pb->buffer+pb->parserIndex);
	pb->parserIndex += 1;
	*error = false;
	return v;
}

/* Reads an arbitrary sized byte array from the packetbuffer */
void xptPacketbuffer_readData(xptPacketbuffer_t* pb, uint8_t* data, uint32_t length, bool* error) {
	if (length >= (1024*1024*16)) return;
	if ((pb->parserIndex+length) > pb->bufferSize) {
		pb->parserIndex = pb->bufferSize;
		*error = true;return;
	}
	memcpy(data, (pb->buffer+pb->parserIndex), length);
	pb->parserIndex += length;
	*error = false;
}

/* Writes a single uint64 to the packet */
void xptPacketbuffer_writeU64(xptPacketbuffer_t* pb, bool* error, uint64 v) {
	if ((pb->parserIndex+8) > pb->bufferLimit) { *error = true;return; }
	*(uint64*)(pb->buffer+pb->parserIndex) = v; pb->parserIndex += 8;*error = false;return;
}

/* Writes a single uint32_t to the packet */
void xptPacketbuffer_writeU32(xptPacketbuffer_t* pb, bool* error, uint32_t v) {
	if ((pb->parserIndex+4) > pb->bufferLimit) { *error = true;return; }
	*(uint32_t*)(pb->buffer+pb->parserIndex) = v;pb->parserIndex += 4;*error = false;return;
}

/* Writes a single uint16_t to the packet */
void xptPacketbuffer_writeU16(xptPacketbuffer_t* pb, bool* error, uint16_t v) {
	if ((pb->parserIndex+2) > pb->bufferLimit) { *error = true;return; }
	*(uint16_t*)(pb->buffer+pb->parserIndex) = v;pb->parserIndex += 2;*error = false;return;
}

/* Writes a single uint8_t to the packet */
void xptPacketbuffer_writeU8(xptPacketbuffer_t* pb, bool* error, uint8_t v) {
	if ((pb->parserIndex+1) > pb->bufferLimit) { *error = true;return; }
	*(uint8_t*)(pb->buffer+pb->parserIndex) = v;pb->parserIndex += 1;*error = false;return;
}

/* Writes an arbitrary sized byte array from the packetbuffer */
void xptPacketbuffer_writeData(xptPacketbuffer_t* pb, uint8_t* data, uint32_t length, bool* error) {
	if (length >= (1024*1024*16)) return;
	if ((pb->parserIndex+length) > pb->bufferLimit) { *error = true;return; }
	memcpy((pb->buffer+pb->parserIndex), data, length);
	pb->parserIndex += length;*error = false;
}

/* Writes a single float to the packet */
void xptPacketbuffer_writeFloat(xptPacketbuffer_t* pb, bool* error, float v) {
	if ((pb->parserIndex+4) > pb->bufferLimit) { *error = true;return; }
	*(float*)(pb->buffer+pb->parserIndex) = v;pb->parserIndex += 4;*error = false;return;
}

/* Finalizes the packet for sending by setting the correct length in the header */
void xptPacketbuffer_finalizeWritePacket(xptPacketbuffer_t* pb) {
	uint32_t opcode = (*(uint32_t*)pb->buffer)&0x000000FF;
	uint32_t dataSize = pb->parserIndex - 4;
	uint32_t header = ((opcode)&0x000000FF) | ((dataSize<<8)&0xFFFFFF00);
	*(uint32_t*)pb->buffer = header;
}

/*
 * Writes a string
 * Depending on the maxStringLength value, there is a 1 or 2 byte prefix byte that indicates the length of the string
 */
void xptPacketbuffer_writeString(xptPacketbuffer_t* pb, const char* stringData, uint32_t maxStringLength, bool* error) {
	if (maxStringLength == 0 || maxStringLength > 0xFFFF) {
		fprintf(stderr, "xptPacketbuffer_writeData(): Invalid maxStringLength\n");
		// strings of max length 0 or longer than 2^16-1 are not supported
		*error = true;return;
	}
	maxStringLength--; // -1 since we count in the '\0' at the end
	// get size of the string + length prefix
	uint32_t stringLength = (uint32_t)strlen(stringData);
	if (stringLength > maxStringLength) {
		fprintf(stderr, "xptPacketbuffer_writeData(): String is longer than maxStringLength\n");
		// string longer than max length
		*error = true;return;
	}
	uint32_t fullLength = stringLength;
	if (maxStringLength <= 0xFF)
		fullLength++;
	else
		fullLength += 2;
	// packet buffer large enough?
	if ((pb->parserIndex+fullLength) > pb->bufferLimit) {
		fprintf(stderr, "xptPacketbuffer_writeData(): Packetbuffer too small\n");
		*error = true;return;
	}
	// write length prefix
	if (maxStringLength <= 0xFF) {
		*(uint8_t*)(pb->buffer+pb->parserIndex) = (uint8_t)stringLength;
		pb->parserIndex += 1;
	} else if (maxStringLength <= 0xFFFF) {
		*(uint16_t*)(pb->buffer+pb->parserIndex) = (uint16_t)stringLength;
		pb->parserIndex += 2;
	}
	// write string data
	memcpy((pb->buffer+pb->parserIndex), stringData, stringLength);
	pb->parserIndex += stringLength;
	*error = false;
}

/*
 * Reads a string
 * Its important that the maxStringLength parameter has the same value as the maxStringLength parameter of xptPacketbuffer_writeData() that was used to write the string
 */
void xptPacketbuffer_readString(xptPacketbuffer_t* pb, char* stringData, uint32_t maxStringLength, bool* error) {
	if (maxStringLength == 0 || maxStringLength > 0xFFFF) {
		fprintf(stderr, "xptPacketbuffer_readString(): Invalid maxStringLength\n");
		// strings of max length 0 or longer than 2^16-1 are not supported
		*error = true;
		return;
	}
	maxStringLength--; // -1 since we count in the '\0' at the end
	// get size of the string + length prefix
	uint32_t stringLength = 0;
	if (maxStringLength <= 0xFF) {
		stringLength = (uint32_t)xptPacketbuffer_readU8(pb, error);
		if (*error) return;
	} else {
		stringLength = (uint32_t)xptPacketbuffer_readU16(pb, error);
		if (*error) return;
	}
	// string length valid?
	if (stringLength >= maxStringLength) {
		fprintf(stderr, "xptPacketbuffer_readString(): Stringlength invalid\n");
		*error = true;return;	
	}
	// packet data large enough?
	if ((pb->parserIndex+stringLength) > pb->bufferSize) {
		fprintf(stderr, "xptPacketbuffer_readString(): End of data in string\n");
		*error = true;return;
	}
	// write string data
	memcpy(stringData, (pb->buffer+pb->parserIndex), stringLength);
	stringData[stringLength] = '\0';
	pb->parserIndex += stringLength;
	*error = false;
}
