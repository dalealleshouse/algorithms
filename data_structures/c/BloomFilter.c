#include "BloomFilter.h"
#include "include/MemAllocMock.h"

typedef struct BloomFilter {
    size_t n;
    size_t bits;
    size_t bytes;
    void* data;
} BloomFilter;

BloomFilter* BloomFilter_Create(size_t max_n)
{
    (void)max_n;

    BloomFilter* filter = malloc(sizeof(BloomFilter));
    if (filter == NULL) {
        ERROR("BloomFilter", FailedMemoryAllocation);
        return NULL;
    }

    filter->data = calloc(1, 300);
    if (filter->data == NULL) {
        BloomFilter_Destroy(filter);
        ERROR("BloomFilter", FailedMemoryAllocation);
        return NULL;
    }

    filter->n = 0;
    filter->bits = 0;
    filter->bytes = 0;

    return filter;
}

size_t BloomFilter_GetN(const BloomFilter* self)
{
    (void)self;
    return 0;
}

Result BloomFilter_Insert(BloomFilter* self, const char* key)
{
    (void)self;
    (void)key;
    return Success;
}

bool BloomFilter_Lookup(BloomFilter* self, const char* key)
{
    (void)self;
    (void)key;
    return false;
}

void BloomFilter_Destroy(BloomFilter* self) { (void)self; }
