/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _MAPPING_LIST_H_
#define _MAPPING_LIST_H_

#include <nvlimits.h>
#include "containers/btree.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"
#include "rmapi/resource.h"

typedef struct VirtualMemory VirtualMemory;
typedef struct Memory Memory;

// ****************************************************************************
//                          Type definitions
// ****************************************************************************

// dma information definitions
typedef struct _def_client_dma_mapping_info CLI_DMA_MAPPING_INFO, *PCLI_DMA_MAPPING_INFO;
typedef struct _def_client_dma_mapping_info_iterator CLI_DMA_MAPPING_INFO_ITERATOR, *PCLI_DMA_MAPPING_INFO_ITERATOR;

// mapping information definitions
typedef struct _def_client_dma_alloc_map_info           CLI_DMA_ALLOC_MAP_INFO;

//
// DMA memory mapping XXX keep around since needed by mapping.c
// We need to figure out what to do with this
// RS-TODO gradually remove this with inter-mapping cleanup
//
struct _def_client_dma_mapping_info
{
    NvU64                 DmaOffset;
    void*                 KernelVAddr[NV_MAX_SUBDEVICES];   // Kernel's virtual address, if required
    void*                 KernelPriv;                       // Token required to unmap the kernel mapping
    NvU64                 FbAperture[NV_MAX_SUBDEVICES];    // GPU aperture addresses, if required
    NvU64                 FbApertureLen[NV_MAX_SUBDEVICES]; // GPU aperture mapped lengths
    MEMORY_DESCRIPTOR    *pMemDesc;                         // Subregion to be mapped
    NvU32                 Flags;
    NvBool                bP2P;
    NvU32                 gpuMask;
    NvU64                 mapPageSize;                      // Page size at which the memory is mapped.
    ADDRESS_TRANSLATION   addressTranslation;
    MEMORY_DESCRIPTOR    *pBar1P2PVirtMemDesc;              // The peer GPU mapped BAR1 region
    MEMORY_DESCRIPTOR    *pBar1P2PPhysMemDesc;              // The peer GPU vidmem sub region
    CLI_DMA_MAPPING_INFO *pNext;
};

//
// iterator object to enum CLI_DMA_MAPPING_INFO from 'pDmaMappingList'
//
struct _def_client_dma_mapping_info_iterator
{
    PNODE          pDmaMappingList;        // list of pDmaMappings
    PNODE          pNextDmaMapping;        // next pDmaMapping while iterating over the DmaOffsets
};

//
// DMA allocMapping
//
struct _def_client_dma_alloc_map_info
{
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo;
    struct VirtualMemory        *pVirtualMemory;
    struct Memory               *pMemory;
};

// ****************************************************************************
//                          Function definitions
// ****************************************************************************

// Client Memory Mappings
//
// CliUpdateMemoryMappingInfo - Fill in RsCpuMapping fields for system memory mappings
//
static inline NV_STATUS
CliUpdateMemoryMappingInfo
(
    RsCpuMapping *pCpuMapping,
    NvBool        bKernel,
    NvP64         cpuAddress,
    NvP64         priv,
    NvU64         cpuMapLength,
    NvU32         flags
)
{
    if (pCpuMapping == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pCpuMapping->pPrivate->bKernel = bKernel;
    pCpuMapping->length = cpuMapLength;
    pCpuMapping->flags = flags;
    pCpuMapping->processId = osGetCurrentProcess();
    pCpuMapping->pLinearAddress = cpuAddress;
    pCpuMapping->pPrivate->pPriv = priv;
    pCpuMapping->pPrivate->gpuAddress = -1;
    pCpuMapping->pPrivate->gpuMapLength = -1;

    return NV_OK;
}

// ****************************************************************************
//                              Device Memory Mappings
// ****************************************************************************

//
// CliUpdateDeviceMemoryMapping - Fill in RsCpuMapping fields for device memory mappings
//
static inline NV_STATUS
CliUpdateDeviceMemoryMapping
(
    RsCpuMapping *pCpuMapping,
    NvBool   bKernel,
    NvP64    priv,
    NvP64    cpuAddress,
    NvU64    cpuMapLength,
    NvU64    gpuAddress,
    NvU64    gpuMapLength,
    NvU32    flags
)
{
    if (pCpuMapping == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pCpuMapping->pPrivate->bKernel = bKernel;
    pCpuMapping->length = cpuMapLength;
    pCpuMapping->flags = flags;
    pCpuMapping->processId = osGetCurrentProcess();
    pCpuMapping->pLinearAddress = cpuAddress;
    pCpuMapping->pPrivate->pPriv = priv;
    pCpuMapping->pPrivate->gpuAddress = gpuAddress;
    pCpuMapping->pPrivate->gpuMapLength = gpuMapLength;

    return NV_OK;
}

RsCpuMapping       *CliFindMappingInClient          (NvHandle, NvHandle, NvP64);

// DMA Mappings
NV_STATUS           intermapCreateDmaMapping        (RsClient *, VirtualMemory *, PCLI_DMA_MAPPING_INFO *, NvU32);
NV_STATUS           intermapRegisterDmaMapping      (RsClient *, VirtualMemory *, PCLI_DMA_MAPPING_INFO,  NvU64, NvU32);
NV_STATUS           intermapDelDmaMapping           (RsClient *, VirtualMemory *, NvU64, NvU32);
void                intermapFreeDmaMapping          (PCLI_DMA_MAPPING_INFO);
CLI_DMA_MAPPING_INFO *intermapGetDmaMapping         (VirtualMemory *pVirtualMemory, NvU64 dmaOffset, NvU32 gpuMask);

NvBool              CliGetDmaMappingInfo            (RsClient *, NvHandle, NvHandle, NvU64, NvU32, PCLI_DMA_MAPPING_INFO*);
void                CliGetDmaMappingIterator        (PCLI_DMA_MAPPING_INFO *, PCLI_DMA_MAPPING_INFO_ITERATOR, PNODE pDmaMappingList);
void                CliGetDmaMappingNext            (PCLI_DMA_MAPPING_INFO *, PCLI_DMA_MAPPING_INFO_ITERATOR);

#endif
