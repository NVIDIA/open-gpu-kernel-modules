
#ifndef _G_GPU_DB_NVOC_H_
#define _G_GPU_DB_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#pragma once
#include "g_gpu_db_nvoc.h"

#ifndef GPU_DB_H
#define GPU_DB_H

#include "core/core.h"
#include "nvoc/object.h"
#include "containers/list.h"
#include "gpu/gpu_uuid.h"

typedef struct NBADDR NBADDR;

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************
//
// The GPU database object is used to encapsulate the GPUINFO
//

/*!
 * @brief Compute policy data for a GPU
 *        Saved policy information for a GPU that can be retrieved later
 */
typedef struct GPU_COMPUTE_POLICY_INFO
{
    //
    // Timeslice config for channels/TSG's on a runlist. The timeslice configs
    // are restricted to four levels : default, short, medium and long.
    //
    NvU32 timeslice;
    // Future policies to be added here
} GPU_COMPUTE_POLICY_INFO;

typedef struct
{
    NvU32       domain;
    NvU8        bus;
    NvU8        device;
    NvU8        function;
    NvBool      bValid;
} PCI_PORT_INFO;

#define GPUDB_CLK_PROP_TOP_POLS_COUNT                                          1

/*!
 * @brief Clock Propagation Topology Policies control data
 */
typedef struct
{
    NvU8   chosenIdx[GPUDB_CLK_PROP_TOP_POLS_COUNT];
} GPU_CLK_PROP_TOP_POLS_CONTROL;

typedef struct
{
    NvU8                             uuid[RM_SHA1_GID_SIZE];
    PCI_PORT_INFO                    pciPortInfo;
    PCI_PORT_INFO                    upstreamPciPortInfo;
    GPU_COMPUTE_POLICY_INFO          policyInfo;
    NvBool                           bShutdownState;
    GPU_CLK_PROP_TOP_POLS_CONTROL    clkPropTopPolsControl;
} GPU_INFO_LIST_NODE, *PGPU_INFO_LIST_NODE;

MAKE_LIST(GpuInfoList, GPU_INFO_LIST_NODE);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_DB_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuDb;
struct NVOC_METADATA__Object;


struct GpuDb {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuDb *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct GpuDb *__nvoc_pbase_GpuDb;    // gpudb

    // Data members
    GpuInfoList gpuList;
    PORT_MUTEX *pLock;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuDb {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_GpuDb_TYPEDEF__
#define __NVOC_CLASS_GpuDb_TYPEDEF__
typedef struct GpuDb GpuDb;
#endif /* __NVOC_CLASS_GpuDb_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuDb
#define __nvoc_class_id_GpuDb 0xcdd250
#endif /* __nvoc_class_id_GpuDb */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuDb;

#define __staticCast_GpuDb(pThis) \
    ((pThis)->__nvoc_pbase_GpuDb)

#ifdef __nvoc_gpu_db_h_disabled
#define __dynamicCast_GpuDb(pThis) ((GpuDb*) NULL)
#else //__nvoc_gpu_db_h_disabled
#define __dynamicCast_GpuDb(pThis) \
    ((GpuDb*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuDb)))
#endif //__nvoc_gpu_db_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuDb(GpuDb**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuDb(GpuDb**, Dynamic*, NvU32);
#define __objCreate_GpuDb(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_GpuDb((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS gpudbConstruct_IMPL(struct GpuDb *arg_pGpuDb);

#define __nvoc_gpudbConstruct(arg_pGpuDb) gpudbConstruct_IMPL(arg_pGpuDb)
void gpudbDestruct_IMPL(struct GpuDb *pGpuDb);

#define __nvoc_gpudbDestruct(pGpuDb) gpudbDestruct_IMPL(pGpuDb)
#undef PRIVATE_FIELD


NV_STATUS   gpudbRegisterGpu(const NvU8 *pUuid, const NBADDR *pUpstreamPortPciInfo, NvU64 pciInfo);
NV_STATUS   gpudbSetGpuComputePolicyConfig(const NvU8 *uuid, NvU32 policyType, GPU_COMPUTE_POLICY_INFO *policyInfo);
NV_STATUS   gpudbGetGpuComputePolicyConfigs(const NvU8 *uuid, GPU_COMPUTE_POLICY_INFO *policyInfo);
NV_STATUS   gpudbSetClockPoliciesControl(const NvU8 *uuid, GPU_CLK_PROP_TOP_POLS_CONTROL *pControl);
NV_STATUS   gpudbGetClockPoliciesControl(const NvU8 *uuid, GPU_CLK_PROP_TOP_POLS_CONTROL *pControl);
NV_STATUS   gpudbSetShutdownState(const NvU8 *pUuid);
#endif // GPU_DB_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_DB_NVOC_H_
