
typedef struct _stream_t stream_t;

typedef struct  
{
	uint32_t (__fastcall *readData)(void *object, void *buffer, uint32_t len);
	uint32_t (__fastcall *writeData)(void *object, void *buffer, uint32_t len);
	uint32_t (__fastcall *getSize)(void *object);
	void (__fastcall *setSize)(void *object, uint32_t size);
	uint32_t (__fastcall *getSeek)(void *object);
	void (__fastcall *setSeek)(void *object, sint32 seek, bool relative);
	void (__fastcall *initStream)(void *object, stream_t *stream);
	void (__fastcall *destroyStream)(void *object, stream_t *stream);
	// general settings
	bool allowCaching;
}streamSettings_t;

typedef struct _stream_t
{
	void *object;
	//_stream_t *substream;
	streamSettings_t *settings;
	// bit access ( write )
	uint8 bitBuffer[8];
	uint8 bitIndex;
	// bit access ( read )
	uint8 bitReadBuffer[8];
	uint8 bitReadBufferState;
	uint8 bitReadIndex;
}stream_t;


stream_t*	stream_create	(streamSettings_t *settings, void *object);
void		stream_destroy	(stream_t *stream);

// stream reading

char stream_readS8(stream_t *stream);
short stream_readS16(stream_t *stream);
int stream_readS32(stream_t *stream);
uint8 stream_readU8(stream_t *stream);
uint16 stream_readU16(stream_t *stream);
uint32_t stream_readU32(stream_t *stream);
unsigned long long stream_readU64(stream_t *stream);
float stream_readFloat(stream_t *stream);
uint32_t stream_readData(stream_t *stream, void *data, int len);
// stream writing
void stream_writeS8(stream_t *stream, char value);
void stream_writeS16(stream_t *stream, short value);
void stream_writeS32(stream_t *stream, int value);
void stream_writeU8(stream_t *stream, uint8 value);
void stream_writeU16(stream_t *stream, uint16 value);
void stream_writeU32(stream_t *stream, uint32_t value);
void stream_writeFloat(stream_t *stream, float value);
uint32_t stream_writeData(stream_t *stream, void *data, int len);
// stream other
void stream_setSeek(stream_t *stream, uint32_t seek);
uint32_t stream_getSeek(stream_t *stream);
uint32_t stream_getSize(stream_t *stream);
void stream_setSize(stream_t *stream, uint32_t size);
void stream_skipData(stream_t *stream, int len);
uint32_t stream_copy(stream_t* dest, stream_t* source, uint32_t length);

// bit operations
void stream_writeBits(stream_t* stream, uint8* bitData, uint32_t bitCount);
void stream_readBits(stream_t* stream, uint8* bitData, uint32_t bitCount);



/* stream ex */

stream_t* streamEx_fromMemoryRange(void *mem, uint32_t memoryLimit);
stream_t* streamEx_fromDynamicMemoryRange(uint32_t memoryLimit);
stream_t* streamEx_createSubstream(stream_t* mainstream, sint32 startOffset, sint32 size);

// misc
void* streamEx_map(stream_t* stream, sint32* size);
sint32 streamEx_readStringNT(stream_t* stream, char* str, uint32_t strSize);