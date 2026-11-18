/* SPDX-License-Identifier: MIT */
#include <stdio.h>
#include "mapping_reuse/mapping_reuse.h"
#include "nvos.h"
#include "nvsecurityinfo.h"

typedef struct {
    NvBool bKernel;
    NvU32 flags;
} TestMapParams;

#include "mapping_policy.h"

#define SINGLE REUSE_MAPPING_DB_MAP_FLAGS_SINGLE_RANGE

static size_t liveAllocations;
static size_t allocationCount;
static size_t failAllocation;

/* Only the allocator and GPU callbacks are mocked; containers and reuse are real. */
void *_portMemAllocatorAlloc(PORT_MEM_ALLOCATOR *allocator, NvLength size)
{
    void *ptr;
    (void)allocator;
    if (++allocationCount == failAllocation)
        return NULL;
    assert(size != 0);
    ptr = malloc(size);
    assert(ptr != NULL);
    liveAllocations++;
    return ptr;
}

void _portMemAllocatorFree(PORT_MEM_ALLOCATOR *allocator, void *ptr)
{
    (void)allocator;
    if (ptr != NULL)
    {
        assert(liveAllocations != 0);
        liveAllocations--;
        free(ptr);
    }
}

void *portMemSet(void *ptr, NvU8 value, NvLength size)
{
    return memset(ptr, value, size);
}

void *portMemCopy(void *dst, NvLength dstSize, const void *src, NvLength srcSize)
{
    assert(dstSize >= srcSize);
    return memcpy(dst, src, srcSize);
}

typedef struct {
    ReuseMappingDb db;
    PORT_MEM_ALLOCATOR allocator;
    MemoryRange active[16];
    size_t mapCalls;
    size_t unmapCalls;
    size_t failChunk;
    NvU64 chunkSize;
} Fixture;

static void unmapRange(void *global, void *context, MemoryRange range)
{
    Fixture *f = global;
    size_t i;
    (void)context;
    for (i = 0; i < 16; i++)
    {
        if (f->active[i].size != 0 && f->active[i].start == range.start)
        {
            assert(f->active[i].size == range.size);
            f->active[i].size = 0;
            f->unmapCalls++;
            return;
        }
    }
    assert(!"Unmap of an unallocated or already released range");
}

static NV_STATUS mapRange(void *global, void *context, MemoryRange range,
                         NvU64 flags, void *token,
                         ReuseMappingDbAddMappingCallback add)
{
    Fixture *f = global;
    size_t chunk = 0;
    f->mapCalls++;
    if ((flags & REUSE_MAPPING_DB_MAP_FLAGS_SINGLE_RANGE) &&
        range.size > f->chunkSize)
        return NV_ERR_NO_MEMORY;

    while (range.size != 0)
    {
        NV_STATUS status;
        size_t i;
        NvU64 size = NV_MIN(range.size, f->chunkSize);
        if (++chunk == f->failChunk)
            return NV_ERR_NO_MEMORY;
        for (i = 0; i < 16 && f->active[i].size != 0; i++) {}
        assert(i < 16);
        /* Deliberately nonadjacent BAR1 ranges. */
        f->active[i] = mrangeMake(0x1000000 * (i + 1), size);
        status = add(token, range.start, f->active[i].start, size);
        if (status != NV_OK)
        {
            /* Match the GPU callback's ownership on add-callback failure. */
            unmapRange(f, context, f->active[i]);
            return status;
        }
        range.start += size;
        range.size -= size;
    }
    return NV_OK;
}

static void init(Fixture *f)
{
    assert(liveAllocations == 0);
    memset(f, 0, sizeof(*f));
    allocationCount = failAllocation = 0;
    f->chunkSize = 0x10000;
    reusemappingdbInit(&f->db, &f->allocator, f, mapRange, unmapRange, NULL);
}

static void finish(Fixture *f)
{
    size_t i;
    for (i = 0; i < 16; i++)
        assert(f->active[i].size == 0);
    assert(mapCount(&f->db.virtualMap) == 0);
    assert(mapCount(&f->db.allocCtxPhysicalMap) == 0);
    reusemappingdbDestruct(&f->db);
    assert(liveAllocations == 0);
}

static void unmapArea(Fixture *f, void *context, MemoryArea area)
{
    NvU64 i;
    for (i = 0; i < area.numRanges; i++)
        reusemappingdbUnmap(&f->db, context, area.pRanges[i]);
    PORT_FREE(&f->allocator, area.pRanges);
}

static void testMappingContract(void)
{
    TestMapParams params = {0};
    /* Ordinary userspace still receives scatter mappings via its mmap context. */
    assert(rmapiValidateKernelMapping(RS_PRIV_LEVEL_USER, 0, &params.bKernel) == NV_OK);
    assert(mappingFlags(&params) & BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG);

    /* A kernel virtual mapping also requires one contiguous BAR1 range. */
    assert(rmapiValidateKernelMapping(RS_PRIV_LEVEL_KERNEL, 0, &params.bKernel) == NV_OK);
    assert(!(mappingFlags(&params) & BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG));

    /* NVKMS gets a physical address, despite bKernel being false. */
    params.flags = DRF_DEF(OS33, _FLAGS, _MEM_SPACE, _USER);
    assert(rmapiValidateKernelMapping(RS_PRIV_LEVEL_KERNEL, params.flags, &params.bKernel) == NV_OK);
    assert(!params.bKernel);
    assert(!(mappingFlags(&params) & BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG));
    assert(rmapiValidateKernelMapping(RS_PRIV_LEVEL_USER, params.flags, NULL) == NV_ERR_INVALID_FLAGS);
}

static void testNewMappingFailures(void)
{
    size_t fail;
    /* Context node, pending entry, and result array allocations. */
    for (fail = 1; fail <= 3; fail++)
    {
        Fixture f;
        MemoryArea area = {(void *)1, 99};
        init(&f);
        failAllocation = fail;
        assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, 0x1000), &area,
                                REUSE_MAPPING_DB_MAP_FLAGS_SINGLE_RANGE) == NV_ERR_NO_MEMORY);
        assert(area.numRanges == 0 && area.pRanges == NULL);
        finish(&f);
    }
}

static void testPhysicalAddressExhaustion(void)
{
    Fixture f;
    MemoryArea area;
    TestMapParams params = {NV_FALSE, DRF_DEF(OS33, _FLAGS, _MEM_SPACE, _USER)};
    NvU64 flags = (mappingFlags(&params) & BUS_MAP_FB_FLAGS_ALLOW_DISCONTIG) ?
                  REUSE_MAPPING_DB_MAP_FLAGS_NO_REUSE : SINGLE;
    init(&f);
    /* Enough aggregate space, but no contiguous range for the whole buffer. */
    assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, 2 * f.chunkSize),
                            &area, flags) == NV_ERR_NO_MEMORY);
    assert(area.numRanges == 0 && area.pRanges == NULL);
    assert(liveAllocations == 0);
    /* An exhausted/fragmented request must not poison subsequent mappings. */
    assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, f.chunkSize),
                            &area, flags) == NV_OK);
    unmapArea(&f, &f, area);
    finish(&f);
}

static void testReuseFailure(void)
{
    Fixture f;
    MemoryArea first, second, failed;
    init(&f);
    assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, 0x1000), &first, SINGLE) == NV_OK);
    failAllocation = allocationCount + 1;
    assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, 0x1000), &failed, SINGLE) == NV_ERR_NO_MEMORY);
    assert(failed.numRanges == 0 && failed.pRanges == NULL);
    assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, 0x1000), &second, SINGLE) == NV_OK);
    assert(first.pRanges[0].start == second.pRanges[0].start && f.mapCalls == 1);
    unmapArea(&f, &f, first);
    assert(f.unmapCalls == 0);
    unmapArea(&f, &f, second);
    assert(f.unmapCalls == 1);
    finish(&f);
}

static void testFragmentedMapping(void)
{
    size_t fail;
    for (fail = 0; fail <= 4; fail++)
    {
        Fixture f;
        MemoryArea area;
        NV_STATUS status;
        init(&f);
        failAllocation = fail;
        status = reusemappingdbMap(&f.db, &f, mrangeMake(0, 3 * f.chunkSize), &area,
                                  REUSE_MAPPING_DB_MAP_FLAGS_NO_REUSE);
        if (fail == 0)
        {
            assert(status == NV_OK && area.numRanges == 3);
            assert(area.pRanges[0].start + area.pRanges[0].size != area.pRanges[1].start);
            unmapArea(&f, &f, area);
        }
        else
        {
            assert(status == NV_ERR_NO_MEMORY);
            assert(area.numRanges == 0 && area.pRanges == NULL);
        }
        finish(&f);
    }
}

static void testPartialMappingFailure(void)
{
    Fixture f;
    MemoryArea area;
    init(&f);
    f.failChunk = 2;
    assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, 3 * f.chunkSize), &area,
                            REUSE_MAPPING_DB_MAP_FLAGS_NO_REUSE) == NV_ERR_NO_MEMORY);
    assert(f.unmapCalls == 1);
    finish(&f);
}

static void testContextLifetime(void)
{
    Fixture f;
    size_t iteration;
    init(&f);
    for (iteration = 0; iteration < 100; iteration++)
    {
        MemoryArea first, overlap, failed;
        assert(reusemappingdbMap(&f.db, &f, mrangeMake(0, 0x2000), &first, SINGLE) == NV_OK);
        /* Overlapping, nonidentical ranges are not reused. */
        assert(reusemappingdbMap(&f.db, &f, mrangeMake(0x1000, 0x2000), &overlap, SINGLE) == NV_OK);
        /* Failure must preserve the existing context and live mapping. */
        f.failChunk = 1;
        assert(reusemappingdbMap(&f.db, &f, mrangeMake(0x8000, 0x1000), &failed, SINGLE) == NV_ERR_NO_MEMORY);
        f.failChunk = 0;
        unmapArea(&f, &f, first);
        unmapArea(&f, &f, overlap);
        assert(mapCount(&f.db.allocCtxPhysicalMap) == 0);
        assert(liveAllocations == 0);
    }
    finish(&f);
}

int main(void)
{
    testMappingContract();
    testNewMappingFailures();
    testPhysicalAddressExhaustion();
    testReuseFailure();
    testFragmentedMapping();
    testPartialMappingFailure();
    testContextLifetime();
    puts("BAR1 mapping contract and allocation-failure regressions passed");
    return 0;
}
