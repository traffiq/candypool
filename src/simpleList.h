#include"./JHLib.h"

typedef struct _objectCreatorCache_t objectCreatorCache_t;

typedef struct  
{
	void** objects;
	uint32_t objectCount;
	uint32_t objectLimit;
	uint32_t stepScaler;
	bool isPreallocated;
	bool doNotFreeRawData;
}simpleList_t;


simpleList_t* simpleList_create(sint32 initialLimit);
void simpleList_create(simpleList_t* simpleList, sint32 initialLimit);
void simpleList_create(simpleList_t* simpleList, sint32 initialLimit, void** rawArray);

void simpleList_free(simpleList_t* simpleList);
void simpleList_add(simpleList_t* simpleList, void* object);
void simpleList_addUnique(simpleList_t* simpleList, void* object);
bool simpleList_addUniqueFeedback(simpleList_t* simpleList, void* object);
bool simpleList_remove(simpleList_t* simpleList, void* object);
void* simpleList_get(simpleList_t* simpleList, sint32 index);


typedef struct  
{
	void** objects;
	uint32_t objectCount;
	uint32_t objectLimit;
	uint32_t stepScaler;
	bool doNotFreeRawData;
}simpleListCached_t;

objectCreatorCache_t* simpleListCached_createCache(uint32_t listCountPerBlock, uint32_t initialEntryCount);
simpleListCached_t* simpleListCached_create(objectCreatorCache_t* objectCreatorCache);

void simpleListCached_free(simpleListCached_t* simpleListCached);
void simpleListCached_add(simpleListCached_t* simpleListCached, void* object);
void simpleListCached_addUnique(simpleListCached_t* simpleListCached, void* object);
bool simpleListCached_addUniqueFeedback(simpleListCached_t* simpleListCached, void* object);
bool simpleListCached_remove(simpleListCached_t* simpleListCached, void* object);
void* simpleListCached_get(simpleListCached_t* simpleListCached, sint32 index);



typedef struct _objectCreatorCache_t
{
	uint32_t currentCacheCount;
	uint32_t currentCacheStep;
	uint32_t currentCacheStepMult;
	uint32_t currentCacheStepAdd;
	uint32_t objectSize;
	uint8* currentEmitPointer;
	uint8* endEmitPointer;
	simpleList_t* dataBlocks;
}objectCreatorCache_t;

objectCreatorCache_t* objectCreatorCache_create(uint32_t objectSize, uint32_t initialEntryCount, uint32_t stepScale, uint32_t stepAdd);
void* objectCreatorCache_getNext(objectCreatorCache_t* objectCreatorCache);
void objectCreatorCache_freeAll(objectCreatorCache_t* objectCreatorCache);
