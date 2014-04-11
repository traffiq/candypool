
typedef struct  
{
	void* objects;
	uint32_t objectSize;
	uint32_t objectCount;
	uint32_t objectLimit;
	bool isPreallocated;
}customBuffer_t;

customBuffer_t* customBuffer_create(sint32 initialLimit, uint32_t objectSize);
void customBuffer_free(customBuffer_t* customBuffer);
void customBuffer_add(customBuffer_t* customBuffer, void* data);
void customBuffer_add(customBuffer_t* customBuffer, void* data, uint32_t count);
void customBuffer_insert(customBuffer_t* customBuffer, sint32 insertIndex, void* data);
void customBuffer_remove(customBuffer_t* customBuffer, uint32_t removeIndex);
uint32_t customBuffer_generateHash(customBuffer_t* customBuffer);

void* customBuffer_get(customBuffer_t* customBuffer, sint32 index);

customBuffer_t* customBuffer_duplicate(customBuffer_t* customBufferSource);