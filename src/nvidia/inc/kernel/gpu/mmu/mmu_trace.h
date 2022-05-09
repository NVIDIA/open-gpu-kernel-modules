/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef MMU_TRACE_H
#define MMU_TRACE_H

#include "core/core.h"
#include "mem_mgr/vaspace.h"
#include "mmu/mmu_fmt.h"
#include "ctrl/ctrl83de.h"

#define MMU_INVALID_ADDR (0xf) // All base addresses are aligned and 0xf is unaligned

#define MMU_MAX_ENTRY_SIZE_BYTES 16

/* ------------------------ Types definitions ------------------------------ */

typedef union
{
  NvU8   v8[MMU_MAX_ENTRY_SIZE_BYTES                ];
  NvU32 v32[MMU_MAX_ENTRY_SIZE_BYTES / sizeof(NvU32)];
  NvU64 v64[MMU_MAX_ENTRY_SIZE_BYTES / sizeof(NvU64)];
} MMU_ENTRY;

typedef NvBool (*MmuTraceCbIsPte)(const void *pFmt, const MMU_FMT_LEVEL *pFmtLevel,
                                  const MMU_ENTRY *pEntry, NvBool *pValid);
typedef const void *(*MmuTraceCbGetFmtPde)(const void *pFmt, const MMU_FMT_LEVEL *pFmtLevel,
                                           NvU32 sublevel);
typedef const void *(*MmuTraceCbGetFmtPte)(const void *pFmt);
typedef NvU64 (*MmuTraceCbGetPdePa)(OBJGPU *pGpu, const void *pFmtPde, const MMU_ENTRY *pPde);
typedef NvU64 (*MmuTraceCbGetPtePa)(OBJGPU *pGpu, const void *pFmtPte, const MMU_ENTRY *pPte);
typedef void (*MmuTraceCbPrintPdb)(OBJGPU *pGpu, OBJVASPACE *pVAS, NvU64 va,
                                   NvU64 vaLimit);
typedef void (*MmuTraceCbPrintPde)(OBJGPU *pGpu, const void *pFmt, const MMU_FMT_LEVEL *pFmtLevel,
                                   const MMU_ENTRY *pPde);
typedef void (*MmuTraceCbPrintPt)(OBJGPU *pGpu, const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPde,
                                  const MMU_ENTRY *pPde);
typedef void (*MmuTraceCbPrintPte)(OBJGPU *pGpu, const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPte,
                                   const MMU_ENTRY *pPte, NvU32 index);
typedef NvBool (*MmuTraceCbIsInvalidPdeOk)(OBJGPU *pGpu, const void *pFmt, const void *pFmtPde,
                                           const MMU_ENTRY *pPde, NvU32 sublevel);
typedef NvU32 (*MmuTraceCbPdeAddrSpace)(const void *pFmtPde, const MMU_ENTRY *pPde);
typedef NvU32 (*MmuTraceCbPteAddrSpace)(const void *pFmtPte, const MMU_ENTRY *pPte);
typedef NvU32 (*MmuTraceCbSwToHwLevel)(const void *pFmt, NvU32 level);

typedef enum
{
    MMU_TRACE_MODE_TRACE         = 0,
    MMU_TRACE_MODE_TRACE_VERBOSE = 1,
    MMU_TRACE_MODE_TRANSLATE     = 2,
    MMU_TRACE_MODE_VALIDATE      = 3,
    MMU_TRACE_MODE_DUMP_RANGE    = 4
} MMU_TRACE_MODE, *PMMU_TRACE_MODE;

typedef struct
{
    NvU64                                      pa;
    NvU32                                      aperture;
    NvBool                                     valid;
    NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *pMapParams;
    NvU64                                      validateCount;
} MMU_TRACE_ARG, *PMMU_TRACE_ARG;

typedef struct
{
    MMU_TRACE_MODE                     mode;
    NvU64                              va;
    NvU64                              vaLimit;
    PMMU_TRACE_ARG                     pArg;
} MMU_TRACE_PARAM, *PMMU_TRACE_PARAM;

typedef struct
{
    MmuTraceCbIsPte          isPte;
    MmuTraceCbGetFmtPde      getFmtPde;
    MmuTraceCbGetFmtPte      getFmtPte;
    MmuTraceCbGetPdePa       getPdePa;
    MmuTraceCbGetPtePa       getPtePa;
    MmuTraceCbPrintPdb       printPdb;
    MmuTraceCbPrintPde       printPde;
    MmuTraceCbPrintPt        printPt;
    MmuTraceCbPrintPte       printPte;
    MmuTraceCbIsInvalidPdeOk isInvalidPdeOk;
    MmuTraceCbPdeAddrSpace   pdeAddrSpace;
    MmuTraceCbPteAddrSpace   pteAddrSpace;
    MmuTraceCbSwToHwLevel    swToHwLevel;
} MMU_TRACE_CALLBACKS;

NV_STATUS mmuTrace(OBJGPU *pGpu, OBJVASPACE *pVAS, MMU_TRACE_PARAM *pParams);

#endif // MMU_TRACE_H
