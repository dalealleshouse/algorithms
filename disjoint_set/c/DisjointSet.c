#include <stdlib.h>

#include "include/MemAllocMock.h"

#include "DisjointSet.h"

static void SetItem_Destory(void* item) { free(item); }

// Find a valid size for the hash table
static size_t _nextPowerOf2(size_t val)
{
    size_t count = 0;

    if (val && !(val & (val - 1)))
        return val;

    while (val != 0) {
        val >>= 1;
        count += 1;
    }

    return 1 << count;
}

Result DisjointSet_Init(DisjointSet* self, const size_t n)
{
    if (self == NULL)
        return NullParameter;

    if (n <= 1)
        return ArgumentOutOfRange;

    self->items = HashTable_Create(_nextPowerOf2(n));
    if (self->items == NULL)
        return DependancyError;

    self->num_sets = 0;

    return Success;
}

Result DisjointSet_MakeSet(DisjointSet* self, const void* rep, SetItem** set)
{
    if (self == NULL || rep == NULL || set == NULL)
        return NullParameter;

    bool exists = HashTable_KeyExists(self->items, &rep, sizeof(void*));
    if (exists)
        return Duplicate;

    SetItem* item = calloc(sizeof(SetItem), 1);
    if (item == NULL)
        return FailedMemoryAllocation;

    item->parent = item;
    item->payload = rep;
    ++self->num_sets;

    Result result = HashTable_Insert(self->items, &rep, sizeof(void*), item);
    if (result != Success) {
        SetItem_Destory(item);
        return result;
    }

    *set = item;
    return Success;
}

Result DisjointSet_FindSet(
    const DisjointSet* self, const void* item, SetItem** set)
{
    if (self == NULL || item == NULL || set == NULL)
        return NullParameter;

    SetItem* _set = HashTable_Find(self->items, &item, sizeof(void*));
    if (_set == NULL)
        return NotFound;

    if (_set != _set->parent) {
        // Path compression - go ahead and set to the immediate parent to
        // shorten the path for future look ups
        Result result
            = DisjointSet_FindSet(self, _set->parent->payload, &_set->parent);
        if (result != Success)
            return result;
    }

    *set = _set->parent;
    return Success;
}

Result DisjointSet_Union(DisjointSet* self, const void* x, const void* y)
{
    if (self == NULL || x == NULL || y == NULL)
        return NullParameter;

    SetItem* x_set = NULL;
    SetItem* y_set = NULL;

    Result result = DisjointSet_FindSet(self, x, &x_set);
    if (result != Success)
        return result;

    result = DisjointSet_FindSet(self, y, &y_set);
    if (result != Success)
        return result;

    if (x_set == y_set)
        return Success;

    if (x_set->rank > y_set->rank)
        y_set->parent = x_set;
    else {
        x_set->parent = y_set;
        if (x_set->rank == y_set->rank)
            ++y_set->rank;
    }

    --self->num_sets;
    return Success;
}

void DisjointSet_Destory(DisjointSet* self)
{
    if (self == NULL)
        return;

    HashTable_Destroy(self->items, SetItem_Destory);
}
