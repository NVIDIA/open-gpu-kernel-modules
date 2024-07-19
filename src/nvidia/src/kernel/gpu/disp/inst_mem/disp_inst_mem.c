/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**************************** Instmem Routines *****************************\
*                                                                          *
*         Display instance memory object function Definitions.             *
*                                                                          *
\***************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/disp/kern_disp.h"
#include "gpu/disp/disp_channel.h"
#include "gpu/disp/inst_mem/disp_inst_mem.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/mem_mgr/context_dma.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "os/nv_memory_type.h"
#include "os/os.h"
#include "containers/eheap_old.h"

/*!
 * Display Context DMA instance memory is always 2 16B blocks in size on all chips.  There
 * is no HW support for scatter lists.  Instance memory should be naturally aligned.
 */
#define DISPLAY_CONTEXT_DMA_INST_SIZE    2
#define DISPLAY_CONTEXT_DMA_INST_ALIGN   2

#define DISP_INST_MEM_EHEAP_OWNER NvU32_BUILD('i','n','s','t')

/*!
 * A hardware display hash table entry.
 */
typedef struct
{
    NvHandle ht_ObjectHandle;
    NvV32 ht_Context;
} DISP_HW_HASH_TABLE_ENTRY;


/*! @brief Constructor */
NV_STATUS
instmemConstruct_IMPL
(
    DisplayInstanceMemory *pInstMem
)
{
    pInstMem->pInstMem = NULL;
    pInstMem->pAllocedInstMemDesc = NULL;
    pInstMem->pInstMemDesc = NULL;
    pInstMem->pHashTable = NULL;
    pInstMem->pInstHeap = NULL;

    return NV_OK;
}


/*!
 * @brief Instmem destructor
 */
void
instmemDestruct_IMPL
(
    DisplayInstanceMemory *pInstMem
)
{
}

/*! @brief Initialized heap related files in display instance memory */
static NV_STATUS
instmemInitBitmap
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  instMemSize,
    NvU32                  hashTableSize
)
{
    NV_STATUS status = NV_OK;
    NvU64     base, limit;
    NvU64     allocSize, allocOffset;
    NvU32     allocFlags;
    NvU32     freeInstMemBase;
    NvU32     freeInstMemSize;
    NvU32     freeInstMemMax;

    //
    // Locate and size the free instance area. This is the base where
    // allocations should start and size of the allocatable inst mem.
    // Initially hash table is the only entity that's allocated.
    //
    freeInstMemBase = hashTableSize >> 4;
    freeInstMemSize = instMemSize - hashTableSize;
    freeInstMemMax = (freeInstMemSize  / 16) & ~0x07;

    // Allocate the Instmem heap manager
    pInstMem->pInstHeap = portMemAllocNonPaged(sizeof(OBJEHEAP));
    if (pInstMem->pInstHeap == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to allocate instance memory heap manager.\n");
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }
    portMemSet(pInstMem->pInstHeap, 0x00, sizeof(OBJEHEAP));

    NV_PRINTF(LEVEL_INFO, "FB Free Size       = 0x%x\n", freeInstMemSize);
    NV_PRINTF(LEVEL_INFO, "FB Free Inst Base  = 0x%x\n", freeInstMemBase);
    NV_PRINTF(LEVEL_INFO, "FB Free Inst Max   = 0x%x\n",
              freeInstMemMax + freeInstMemBase);

    //
    // Construct the Instmem heap manager - Pre-allocate mgmt structures
    // to avoid dynamic allocation and allow bind/unbind at high IRQL
    // on Windows.  Size to fill hash table + NULL instance.
    //
    base  = freeInstMemBase;
    limit = freeInstMemBase + freeInstMemMax + 1;
    constructObjEHeap(
        pInstMem->pInstHeap,
        base,
        limit,
        0,      // sizeofMemBlock
        pInstMem->nHashTableEntries + 1); // numPreAllocMemStruct

    // Reserve instance 0 as the NULL instance.
    allocSize = 1;
    allocOffset = base;
    allocFlags = NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE;
    status = pInstMem->pInstHeap->eheapAlloc(
                     pInstMem->pInstHeap,       // thisHeap
                     DISP_INST_MEM_EHEAP_OWNER, // owner
                     &allocFlags,               // flags
                     &allocOffset,              // offset
                     &allocSize,                // size
                     1,                         // offsetAlign
                     1,                         // sizeAlign
                     NULL,                      // ppMemBlock
                     NULL,                      // isolation id
                     NULL);                     // callback ownership checker
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "eheapAlloc failed for instance memory heap manager.\n");
        status = NV_ERR_NO_MEMORY;
    }

exit:
   return status;
}

/*! @brief Initialized hash table related files in display instance memory */
static NV_STATUS
instmemInitHashTable
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  hashTableSize
)
{
    NV_STATUS status = NV_OK;
    NvU32     i;

    pInstMem->nHashTableEntries = hashTableSize / sizeof(DISP_HW_HASH_TABLE_ENTRY);
    pInstMem->hashTableBaseAddr = instmemGetHashTableBaseAddr_HAL(pGpu, pInstMem);

    // Allocate Hash Table structure.
    pInstMem->pHashTable = portMemAllocNonPaged(pInstMem->nHashTableEntries *
                                               sizeof(SW_HASH_TABLE_ENTRY));
    if (pInstMem->pHashTable == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to allocate hash table.\n");
        status = NV_ERR_NO_MEMORY;
        goto exit;
    }

    // Initialize Hash Table.
    for (i = 0; i < pInstMem->nHashTableEntries; i++)
    {
        pInstMem->pHashTable[i].pContextDma = NULL;
    }

exit:
    return status;
}

/*!
 * @brief Save instance memory parameters
 *
 * For dGPU called from mem_mgr initialization with reserved frame buffer memory.  For SOC
 * we dynamically allocate system memory later.
 */
void
instmemSetMemory_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NV_ADDRESS_SPACE       dispInstMemAddrSpace,
    NvU32                  dispInstMemAttr,
    NvU64                  dispInstMemBase,
    NvU32                  dispInstMemSize
)
{
    pInstMem->instMemAddrSpace = dispInstMemAddrSpace;
    pInstMem->instMemAttr = dispInstMemAttr;
    pInstMem->instMemBase = dispInstMemBase;
    pInstMem->instMemSize = dispInstMemSize;
}

/*! @brief Initialize instance memory descriptor */
static NV_STATUS
instmemInitMemDesc
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  instMemSize
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS status = NV_OK;

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        //
        // FB reserved memory logic not be getting called for Tegra system memory scanout.
        // So as InstMem Desc is not getting initialized, currently hardcoding
        // dispInstMemAttr to NV_MEMORY_CACHED this needs to be set based on system configuration/registry parameter.
        //
        instmemSetMemory(pGpu, pInstMem,
                         ADDR_SYSMEM, NV_MEMORY_CACHED,
                         0 /* base */, instMemSize);
    }
    else if (IS_GSP_CLIENT(pGpu))
    {
        // ToDO: Need to respect RM overrides and keep monolithic design same as offload.
        instmemSetMemory(pGpu, pInstMem,
                         ADDR_FBMEM, NV_MEMORY_WRITECOMBINED,
                         0 , instMemSize);
    }

    switch (pInstMem->instMemAddrSpace)
    {
        default:
        case ADDR_FBMEM:
            {
                NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                    memdescCreate(&pInstMem->pInstMemDesc, pGpu,
                                  pInstMem->instMemSize,
                                  DISP_INST_MEM_ALIGN,
                                  NV_TRUE, pInstMem->instMemAddrSpace,
                                  pInstMem->instMemAttr,
                                  MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO),
                    exit);

                memdescDescribe(pInstMem->pInstMemDesc,
                                ADDR_FBMEM,
                                memmgrGetRsvdMemoryBase(pMemoryManager) + pInstMem->instMemBase,
                                pInstMem->instMemSize);
            }
            break;

        case ADDR_SYSMEM:
            {
                //
                // memdescAlloc won't (currently) honor a request for sysmem alloc alignment!  Overallocate
                // and round up the address to work around this.
                //
                // Create a sub-memdesc to the aligned block.  This keeps the alignment calculation local
                // to this function.
                //
                NvU64 base;
                NvU64 offset;
                NvBool bContig = NV_TRUE;

                if (pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
                {
                    //
                    // On Orin, display FE goes through the NISO SMMU to read
                    // from Instance Memory. As such, there's absolutely no
                    // reason why we need a contiguous allocation for Instance
                    // Memory.
                    //
                    bContig = NV_FALSE;
                }

                NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                    memdescCreate(&pInstMem->pAllocedInstMemDesc, pGpu,
                                  instMemSize + (DISP_INST_MEM_ALIGN - RM_PAGE_SIZE),
                                  DISP_INST_MEM_ALIGN,
                                  bContig, pInstMem->instMemAddrSpace,
                                  pInstMem->instMemAttr,
                                  MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO),
                    exit);

                memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_67, 
                                pInstMem->pAllocedInstMemDesc);
                NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, status,
                    exit);

                base = memdescGetPhysAddr(pInstMem->pAllocedInstMemDesc, AT_GPU, 0);
                offset = RM_ALIGN_UP(base, DISP_INST_MEM_ALIGN) - base;

                NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                    memdescCreateSubMem(&pInstMem->pInstMemDesc, pInstMem->pAllocedInstMemDesc,
                                        pGpu,
                                        offset,
                                        instMemSize),
                    exit);
            }
            break;
    }

exit:
    // Clean-up is handled by the caller
    return status;
}

/*! @brief Free all memory allocations done for display instance memory */
static void
instmemDestroy
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem
)
{
    // Free up the inst mem descriptors
    memdescDestroy(pInstMem->pInstMemDesc);
    pInstMem->pInstMemDesc = NULL;

    memdescFree(pInstMem->pAllocedInstMemDesc);
    memdescDestroy(pInstMem->pAllocedInstMemDesc);
    pInstMem->pAllocedInstMemDesc = NULL;

    if (pInstMem->pInstHeap != NULL)
    {
        pInstMem->pInstHeap->eheapDestruct(pInstMem->pInstHeap);
        portMemFree(pInstMem->pInstHeap);
        pInstMem->pInstHeap = NULL;
    }

    portMemFree(pInstMem->pHashTable);
    pInstMem->pHashTable = NULL;
}

NV_STATUS
instmemStateInitLocked_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem
)
{
    NV_STATUS status = NV_OK;
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU32     hClient = pGpu->hInternalClient;
    NvU32     hSubdevice = pGpu->hInternalSubdevice;
    NvU32     instMemSize, hashTableSize;
    NV2080_CTRL_INTERNAL_DISPLAY_WRITE_INST_MEM_PARAMS ctrlParams;

    instmemGetSize_HAL(pGpu, pInstMem, &instMemSize, &hashTableSize);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            instmemInitHashTable(pGpu, pInstMem, hashTableSize), exit);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            instmemInitBitmap(pGpu, pInstMem, instMemSize, hashTableSize), exit);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            instmemInitMemDesc(pGpu, pInstMem, instMemSize), exit);

    // Make internal RPC to write the instance memory register
    ctrlParams.instMemAddrSpace = memdescGetAddressSpace(pInstMem->pInstMemDesc);
    ctrlParams.instMemCpuCacheAttr = memdescGetCpuCacheAttrib(pInstMem->pInstMemDesc);
    ctrlParams.instMemPhysAddr = memdescGetPhysAddr(pInstMem->pInstMemDesc, AT_GPU, 0);
    ctrlParams.instMemSize = memdescGetSize(pInstMem->pInstMemDesc);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->Control(pRmApi, hClient, hSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_DISPLAY_WRITE_INST_MEM,
                            &ctrlParams, sizeof(ctrlParams)), exit);

exit:
    if (status != NV_OK)
    {
        instmemDestroy(pGpu, pInstMem);
    }

    return status;
}

void
instmemStateDestroy_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem
)
{
    instmemDestroy(pGpu, pInstMem);
}

NV_STATUS
instmemStateLoad_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  flags
)
{
    NvBool bPersistent;

    //
    // We keep a persistent mapping to instance memory in two cases:
    //   * Windows issues bind/update/unbind control calls with with BYPASS_LOCK,
    //     so we cannot generate a new BAR2 mapping at control call time.
    //   * System memory backing.
    //
    bPersistent = (pInstMem->instMemAddrSpace == ADDR_SYSMEM);
    if (bPersistent)
    {
        //
        // Windows issues bind/update/unbind control calls with BYPASS_LOCK,
        // so we generate a new BAR2 mapping control call time.
        //
        pInstMem->pInstMem = memdescMapInternal(pGpu, pInstMem->pInstMemDesc,
                                                TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING);
        if (pInstMem->pInstMem == NULL)
            return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

NV_STATUS
instmemStateUnload_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  flags
)
{
    if (pInstMem->pInstMem != NULL)
    {
        memdescUnmapInternal(pGpu, pInstMem->pInstMemDesc, TRANSFER_FLAGS_NONE);
        pInstMem->pInstMem = NULL;
    }

    return NV_OK;
}

/*!
 * @brief  Reserve a chunk of display instance memory (will always be for Context DMAs).
 * @return offset from the base of display instance memory (not base of FB).
 */
static NV_STATUS
_instmemReserveContextDma
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                 *offset
)
{
    NV_STATUS         rmStatus;
    NvU64             allocSize  = DISPLAY_CONTEXT_DMA_INST_SIZE;  // size << 4;
    NvU64             allocOffset;
    NvU32             allocFlags = 0;

    *offset = 0;

    rmStatus = pInstMem->pInstHeap->eheapAlloc(
                     pInstMem->pInstHeap,             // thisHeap
                     DISP_INST_MEM_EHEAP_OWNER,       // owner
                     &allocFlags,                     // flags
                     &allocOffset,                    // offset
                     &allocSize,                      // size
                     DISPLAY_CONTEXT_DMA_INST_ALIGN,  // offsetAlign
                     DISPLAY_CONTEXT_DMA_INST_ALIGN,  // sizeAlign
                     NULL,                            // ppMemBlock
                     NULL,                            // isolation id
                     NULL);                           // callback ownership checker

   // return the allocation offset if successful
   if (rmStatus == NV_OK)
   {
       *offset = (NvU32)allocOffset;
   }
   else
   {
       rmStatus = NV_ERR_NO_MEMORY;
   }

   return rmStatus;
}

/*!
 * @brief  Free display instance memory reserved for Context DMA.
 */
static NV_STATUS
_instmemFreeContextDma
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  offset
)
{
    //
    // If instance is already set to 0, then it has already been freed.  This can
    // happen in some cases when a mode switch is happening while MIDI is playing
    // using the timer alarm notifies.  Ignoring this case can potentially cause a
    // protection fault, so be careful.
    //
    if (offset == 0)
        return NV_OK;

    if (pInstMem->pInstHeap == NULL)
        return NV_OK;

    pInstMem->pInstHeap->eheapFree(
        pInstMem->pInstHeap, // thisHeap
        offset);             // offset

    return NV_OK;
}

static void
_instmemClearHashEntry
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    NvU32                  htEntry
)
{
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    TRANSFER_SURFACE    dest = {0};
    NvU32               entryOffset;
    DISP_HW_HASH_TABLE_ENTRY entry;

    pInstMem->pHashTable[htEntry].pContextDma = NULL;
    pInstMem->pHashTable[htEntry].pDispChannel = NULL;

    //
    // If we found the entry, clear the inst mem copy of the entry
    // Start with offset of base of inst mem
    // Add offset of base of hash table from base of inst mem
    // Add the offset of entry from base of hash table
    //
    entryOffset = pInstMem->hashTableBaseAddr +
                      (sizeof(DISP_HW_HASH_TABLE_ENTRY) * htEntry);

    dest.pMemDesc = pInstMem->pInstMemDesc;
    dest.offset = entryOffset;

    entry.ht_ObjectHandle = 0;
    entry.ht_Context = instmemGenerateHashTableData_HAL(pGpu, pInstMem,
                            0 /* client id */,
                            0 /* NV_UDISP_HASH_TBL_INSTANCE_INVALID */,
                            0 /* dispChannelNum */);

    NV_ASSERT_OK(memmgrMemWrite(pMemoryManager, &dest, &entry, sizeof(entry),
                                TRANSFER_FLAGS_NONE));
}

static NV_STATUS
_instmemRemoveHashEntry
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma,
    DispChannel           *pDispChannel
)
{
    NvU32 htEntry;

    for (htEntry = 0; htEntry < pInstMem->nHashTableEntries; htEntry++)
    {
        if ( (pInstMem->pHashTable[htEntry].pContextDma == pContextDma) &&
             (pInstMem->pHashTable[htEntry].pDispChannel == pDispChannel))
        {
            _instmemClearHashEntry(pGpu, pInstMem, htEntry);
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_STATE;
}

static NV_STATUS
_instmemAddHashEntry
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma,
    DispChannel           *pDispChannel,
    NvU32                  offset
)
{
    KernelDisplay     *pKernelDisplay  = GPU_GET_KERNEL_DISPLAY(pGpu);
    MemoryManager     *pMemoryManager  = GPU_GET_MEMORY_MANAGER(pGpu);
    NvHandle           hClient         = RES_GET_CLIENT_HANDLE(pContextDma);
    NvU32              entryOffset, dispChannelNum;
    NvU32              Limit, i, Entry;
    NvHandle           handle          = RES_GET_HANDLE(pContextDma);
    NvU32              hash;
    NV_STATUS          status;
    TRANSFER_SURFACE   dest = {0};
    DISP_HW_HASH_TABLE_ENTRY entry;

    status = kdispGetChannelNum_HAL(pKernelDisplay, pDispChannel->DispClass, pDispChannel->InstanceNumber, &dispChannelNum);
    if (status != NV_OK)
        return status;

    // Query HAL for starting entry for this <handle,chid> pair.
    instmemHashFunc_HAL(pGpu, pInstMem, hClient, RES_GET_HANDLE(pContextDma), dispChannelNum, &hash);

    //
    // Since all the ctx dmas are 32 byte aligned, we don't need to
    // store offsets in bytes. We store "which 32 byte chunk" does the
    // ctx dma reside in. So, right shift the whole thing by 5 after
    // left shifting by 4 (need to left shift by 4 since internally we
    // track offsets in 16 byte chunks
    //
    offset >>= (5 - 4); // offset <<= 4; followed by offset >>= 5

    if (offset == 0)  //NV_UDISP_HASH_TBL_INSTANCE_INVALID
    {
        NV_PRINTF(LEVEL_ERROR, "Instance pointer is invalid!!\n");
        return (NV_ERR_GENERIC);
    }

    //
    // Make sure instance memory pointer is valid as well. That is,
    // it's within the mask range of possible instance values
    //
    NV_ASSERT(instmemIsValid_HAL(pGpu, pInstMem, offset));

    // Make sure that hash is valid as well.
    NV_ASSERT(hash < pInstMem->nHashTableEntries);

    //
    // Search table for free slot.
    //
    // Here's the Old way that we did this - Allows for arbitrary sized hash tables
    //
    // Limit = hash + pDispHalPvtInfo->pPram[ChID].nHashTableEntries; // loop over whole table
    // Entry = hash;
    // while(Entry < Limit)
    // {
    //     if (pDispHalPvtInfo->pPram[ChID].pHashTable[Entry].Object == NULL)
    //         break;
    //
    //     //
    //     // if we just checked the last entry and have more entries
    //     // to check for empty, wrap search back to beginning of table
    //     //
    //     if (Entry == (pDispHalPvtInfo->pPram[ChID].nHashTableEntries-1) &&
    //         ((Entry + 1) < Limit))
    //     {
    //         Limit = Limit - Entry - 1; // -1 since we count the one we just checked
    //         Entry = 0;
    //         continue;
    //     }
    //
    //     Entry++;
    // }
    //
    // But since we know that this hash table is always 512 in size, let's go ahead
    // and make this assumption to make the loops faster. Or even better, lets just
    // make sure that the Hash Depth is a power of 2.  That way, we can use
    // nHashTableEntries - 1 as the mask of what entries are valid - and this allows for any
    // nHashTableEntries that is a power of 2.
    //
    NV_ASSERT(!(pInstMem->nHashTableEntries & (pInstMem->nHashTableEntries - 1)));

    Limit = hash + pInstMem->nHashTableEntries; // loop over whole table
    Entry = hash;
    for (i = hash; i < Limit; i++)
    {
        // Mask off high bits of i since we loop the hash table.
        Entry = i & (pInstMem->nHashTableEntries - 1);
        if (pInstMem->pHashTable[Entry].pContextDma == NULL)
            break;
    }

    if (pInstMem->pHashTable[Entry].pContextDma != NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Display Hash table is FULL!!\n");
        return NV_ERR_TOO_MANY_PRIMARIES;
    }

    entryOffset = pInstMem->hashTableBaseAddr +
                  (Entry * sizeof(DISP_HW_HASH_TABLE_ENTRY));

    // Add object to the Hash Table.
    pInstMem->pHashTable[Entry].pContextDma  = pContextDma;
    pInstMem->pHashTable[Entry].pDispChannel = pDispChannel;

    dest.pMemDesc = pInstMem->pInstMemDesc;
    dest.offset = entryOffset;

    entry.ht_ObjectHandle = handle;

    // Note that we have full 32 bit client id at this point and we only need to tell hw the lower 14 bits
    entry.ht_Context = instmemGenerateHashTableData_HAL(
                            pGpu,
                            pInstMem,
                            hClient,
                            offset,
                            dispChannelNum);

    NV_ASSERT_OK_OR_RETURN(memmgrMemWrite(pMemoryManager, &dest, &entry, sizeof(entry),
                                          TRANSFER_FLAGS_NONE));

    return NV_OK;
}

/*!
 * @brief Is the this ContextDma bound to this DispChannel
 */
static NV_STATUS
_instmemProbeHashEntry
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma,
    DispChannel           *pDispChannel
)
{
    KernelDisplay *pKernelDisplay  = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32          dispChannelNum;
    NV_STATUS      status;
    NvU32          hash = 0;
    NvU32          limit;
    NvU32          i;

    status = kdispGetChannelNum_HAL(pKernelDisplay, pDispChannel->DispClass, pDispChannel->InstanceNumber, &dispChannelNum);
    if (status == NV_OK)
    {
        instmemHashFunc_HAL(pGpu, pInstMem,
                            RES_GET_CLIENT_HANDLE(pContextDma),
                            RES_GET_HANDLE(pContextDma),
                            dispChannelNum, &hash);
    }

    // Hash table must be a power of 2 currently
    NV_ASSERT(!(pInstMem->nHashTableEntries & (pInstMem->nHashTableEntries - 1)));

    limit = hash + pInstMem->nHashTableEntries; // loop over whole table

    for (i = hash; i < limit; i++)
    {
        NvU32 htEntry = i & (pInstMem->nHashTableEntries - 1);

        if ((pInstMem->pHashTable[htEntry].pDispChannel == pDispChannel) &&
            (pInstMem->pHashTable[htEntry].pContextDma == pContextDma))
        {
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief Bind the ContextDma to the given Display Channel
 */
NV_STATUS
instmemBindContextDma_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma,
    DispChannel           *pDispChannel
)
{
    NvU32     gpuSubDevInst;
    NV_STATUS status;

    gpuSubDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    // Production SW requires each context is bound only once
    status = _instmemProbeHashEntry(pGpu, pInstMem, pContextDma, pDispChannel);
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "The ctx dma (0x%x) has already been bound\n",
                  RES_GET_HANDLE(pContextDma));
        status = NV_ERR_STATE_IN_USE;
        goto exit;
    }

    if (pContextDma->InstRefCount[gpuSubDevInst] == 0)
    {
        // Reserve inst mem space for this ctx dma
        status = _instmemReserveContextDma(pGpu, pInstMem, &(pContextDma->Instance[gpuSubDevInst]));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to alloc space in disp inst mem for ctx dma 0x%x\n",
                      RES_GET_HANDLE(pContextDma));
            goto exit;
        }

        // Call into HAL to write inst mem with the ctx dma info
        status = instmemCommitContextDma_HAL(pGpu, pInstMem, pContextDma);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to commit ctx dma (0x%x) to inst mem\n",
                      RES_GET_HANDLE(pContextDma));
            _instmemFreeContextDma(pGpu, pInstMem, pContextDma->Instance[gpuSubDevInst]);
            pContextDma->Instance[gpuSubDevInst] = 0;
            goto exit;
        }
    }

    //
    // Now add the hash table entry for this ctx dma
    // We loop around this call instead of looping at MEM_WR level because we
    // also want to propagate the SW hash table.
    //
    status = _instmemAddHashEntry(pGpu, pInstMem,
                                  pContextDma,
                                  pDispChannel,
                                  pContextDma->Instance[gpuSubDevInst]);
    if (status != NV_OK)
    {
        if (pContextDma->InstRefCount[gpuSubDevInst] == 0)
        {
            instmemDecommitContextDma_HAL(pGpu, pInstMem, pContextDma);
            _instmemFreeContextDma(pGpu, pInstMem, pContextDma->Instance[gpuSubDevInst]);
            pContextDma->Instance[gpuSubDevInst] = 0;
        }
        goto exit;
    }

    // We have one more reference to the context DMA in instance memory now.
    pContextDma->InstRefCount[gpuSubDevInst]++;

exit:

    return status;
}

/*!
 * @brief Remove reference to an instance allocation.  Free after last reference.
 */
void
_instmemRemoveReference
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma
)
{
    NvU32 gpuSubDevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    NV_ASSERT(pContextDma->InstRefCount[gpuSubDevInst]);
    if (pContextDma->InstRefCount[gpuSubDevInst])
    {
        pContextDma->InstRefCount[gpuSubDevInst]--;

        // Remove DMA object if this is the last binding
        if (pContextDma->InstRefCount[gpuSubDevInst] == 0)
        {
            instmemDecommitContextDma_HAL(pGpu, pInstMem, pContextDma);
            _instmemFreeContextDma(pGpu, pInstMem, pContextDma->Instance[gpuSubDevInst]);
            pContextDma->Instance[gpuSubDevInst] = 0;
        }
    }
}

/*!
 * @brief Unbind the ContextDma from the given Display Channel
 */
NV_STATUS
instmemUnbindContextDma_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma,
    DispChannel           *pDispChannel
)
{
    NV_STATUS status;

    // If ContextDma is not bound to this subdevice, there is no bookkeeping to do
    status = _instmemRemoveHashEntry(pGpu, pInstMem, pContextDma, pDispChannel);
    if (status == NV_OK)
    {
        _instmemRemoveReference(pGpu, pInstMem, pContextDma);
    }

    return status;
}

/*!
 * @brief Unbind the ContextDma from all Display channels on the given context
 */
void
instmemUnbindContextDmaFromAllChannels_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    ContextDma            *pContextDma
)
{
    NvU32 htEntry;

    // Check all entries in the hash table
    for (htEntry = 0; htEntry < pInstMem->nHashTableEntries; htEntry++)
    {
        if (pInstMem->pHashTable[htEntry].pContextDma == pContextDma)
        {
            _instmemClearHashEntry(pGpu, pInstMem, htEntry);
            _instmemRemoveReference(pGpu, pInstMem, pContextDma);
        }
    }

}

/*!
 * @brief Unbind the ContextDma from all Display channels on the given context
 */
void
instmemUnbindDispChannelContextDmas_IMPL
(
    OBJGPU                *pGpu,
    DisplayInstanceMemory *pInstMem,
    DispChannel           *pDispChannel
)
{
    NvU32 htEntry;

    // Check all entries in the hash table
    for (htEntry = 0; htEntry < pInstMem->nHashTableEntries; htEntry++)
    {
        if (pInstMem->pHashTable[htEntry].pDispChannel == pDispChannel)
        {
            ContextDma *pContextDma = pInstMem->pHashTable[htEntry].pContextDma;

            _instmemClearHashEntry(pGpu, pInstMem, htEntry);
            _instmemRemoveReference(pGpu, pInstMem, pContextDma);
        }
    }

}
