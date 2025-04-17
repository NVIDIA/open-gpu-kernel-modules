
#ifndef _G_HYPERVISOR_NVOC_H_
#define _G_HYPERVISOR_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_hypervisor_nvoc.h"

#ifndef HYPERVISOR_H
#define HYPERVISOR_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: hypervisor.h                                                      *
*       Defines and structures used for the hypervisor object.              *
\***************************************************************************/

#include "core/core.h"
#include "nvoc/utility.h"
#include "nv-hypervisor.h"
#include "mem_mgr/mem.h"

/* ------------------------ Forward Declarations ---------------------------- */

struct OBJOS;

#ifndef __NVOC_CLASS_OBJOS_TYPEDEF__
#define __NVOC_CLASS_OBJOS_TYPEDEF__
typedef struct OBJOS OBJOS;
#endif /* __NVOC_CLASS_OBJOS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70
#endif /* __nvoc_class_id_OBJOS */



typedef struct HOST_VGPU_DEVICE HOST_VGPU_DEVICE;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_HYPERVISOR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJHYPERVISOR;
struct NVOC_METADATA__Object;


struct OBJHYPERVISOR {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJHYPERVISOR *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJHYPERVISOR *__nvoc_pbase_OBJHYPERVISOR;    // hypervisor

    // 1 PDB property
    NvBool PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED;

    // Data members
    NvBool bDetected;
    NvBool bIsHVMGuest;
    HYPERVISOR_TYPE type;
    NvBool bIsHypervHost;
    NvBool bIsHypervVgpuSupported;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJHYPERVISOR {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
#define __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
typedef struct OBJHYPERVISOR OBJHYPERVISOR;
#endif /* __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHYPERVISOR
#define __nvoc_class_id_OBJHYPERVISOR 0x33c1ba
#endif /* __nvoc_class_id_OBJHYPERVISOR */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHYPERVISOR;

#define __staticCast_OBJHYPERVISOR(pThis) \
    ((pThis)->__nvoc_pbase_OBJHYPERVISOR)

#ifdef __nvoc_hypervisor_h_disabled
#define __dynamicCast_OBJHYPERVISOR(pThis) ((OBJHYPERVISOR*) NULL)
#else //__nvoc_hypervisor_h_disabled
#define __dynamicCast_OBJHYPERVISOR(pThis) \
    ((OBJHYPERVISOR*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJHYPERVISOR)))
#endif //__nvoc_hypervisor_h_disabled

// Property macros
#define PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED_BASE_CAST
#define PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED_BASE_NAME PDB_PROP_HYPERVISOR_DRIVERVM_ENABLED

NV_STATUS __nvoc_objCreateDynamic_OBJHYPERVISOR(OBJHYPERVISOR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJHYPERVISOR(OBJHYPERVISOR**, Dynamic*, NvU32);
#define __objCreate_OBJHYPERVISOR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJHYPERVISOR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NvBool hypervisorIsVgxHyper_IMPL(void);


#define hypervisorIsVgxHyper() hypervisorIsVgxHyper_IMPL()
#define hypervisorIsVgxHyper_HAL() hypervisorIsVgxHyper()

NV_STATUS hypervisorInjectInterrupt_IMPL(struct OBJHYPERVISOR *arg1, VGPU_NS_INTR *arg2);


#ifdef __nvoc_hypervisor_h_disabled
static inline NV_STATUS hypervisorInjectInterrupt(struct OBJHYPERVISOR *arg1, VGPU_NS_INTR *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJHYPERVISOR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_hypervisor_h_disabled
#define hypervisorInjectInterrupt(arg1, arg2) hypervisorInjectInterrupt_IMPL(arg1, arg2)
#endif //__nvoc_hypervisor_h_disabled

#define hypervisorInjectInterrupt_HAL(arg1, arg2) hypervisorInjectInterrupt(arg1, arg2)

void hypervisorSetHypervVgpuSupported_IMPL(struct OBJHYPERVISOR *arg1);


#ifdef __nvoc_hypervisor_h_disabled
static inline void hypervisorSetHypervVgpuSupported(struct OBJHYPERVISOR *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJHYPERVISOR was disabled!");
}
#else //__nvoc_hypervisor_h_disabled
#define hypervisorSetHypervVgpuSupported(arg1) hypervisorSetHypervVgpuSupported_IMPL(arg1)
#endif //__nvoc_hypervisor_h_disabled

#define hypervisorSetHypervVgpuSupported_HAL(arg1) hypervisorSetHypervVgpuSupported(arg1)

NvBool hypervisorCheckForObjectAccess_IMPL(NvHandle hClient);

#define hypervisorCheckForObjectAccess(hClient) hypervisorCheckForObjectAccess_IMPL(hClient)
NvBool hypervisorIsType_IMPL(HYPERVISOR_TYPE hyperType);

#define hypervisorIsType(hyperType) hypervisorIsType_IMPL(hyperType)
NV_STATUS hypervisorConstruct_IMPL(struct OBJHYPERVISOR *arg_);

#define __nvoc_hypervisorConstruct(arg_) hypervisorConstruct_IMPL(arg_)
void hypervisorDestruct_IMPL(struct OBJHYPERVISOR *arg1);

#define __nvoc_hypervisorDestruct(arg1) hypervisorDestruct_IMPL(arg1)
NV_STATUS hypervisorDetection_IMPL(struct OBJHYPERVISOR *arg1, struct OBJOS *arg2);

#ifdef __nvoc_hypervisor_h_disabled
static inline NV_STATUS hypervisorDetection(struct OBJHYPERVISOR *arg1, struct OBJOS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJHYPERVISOR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_hypervisor_h_disabled
#define hypervisorDetection(arg1, arg2) hypervisorDetection_IMPL(arg1, arg2)
#endif //__nvoc_hypervisor_h_disabled

NvBool hypervisorPcieP2pDetection_IMPL(struct OBJHYPERVISOR *arg1, NvU32 arg2);

#ifdef __nvoc_hypervisor_h_disabled
static inline NvBool hypervisorPcieP2pDetection(struct OBJHYPERVISOR *arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJHYPERVISOR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_hypervisor_h_disabled
#define hypervisorPcieP2pDetection(arg1, arg2) hypervisorPcieP2pDetection_IMPL(arg1, arg2)
#endif //__nvoc_hypervisor_h_disabled

HYPERVISOR_TYPE hypervisorGetHypervisorType_IMPL(struct OBJHYPERVISOR *arg1);

#ifdef __nvoc_hypervisor_h_disabled
static inline HYPERVISOR_TYPE hypervisorGetHypervisorType(struct OBJHYPERVISOR *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJHYPERVISOR was disabled!");
    HYPERVISOR_TYPE ret;
    portMemSet(&ret, 0, sizeof(HYPERVISOR_TYPE));
    return ret;
}
#else //__nvoc_hypervisor_h_disabled
#define hypervisorGetHypervisorType(arg1) hypervisorGetHypervisorType_IMPL(arg1)
#endif //__nvoc_hypervisor_h_disabled

void hypervisorSetHypervisorType_IMPL(struct OBJHYPERVISOR *pHypervisor, HYPERVISOR_TYPE type);

#ifdef __nvoc_hypervisor_h_disabled
static inline void hypervisorSetHypervisorType(struct OBJHYPERVISOR *pHypervisor, HYPERVISOR_TYPE type) {
    NV_ASSERT_FAILED_PRECOMP("OBJHYPERVISOR was disabled!");
}
#else //__nvoc_hypervisor_h_disabled
#define hypervisorSetHypervisorType(pHypervisor, type) hypervisorSetHypervisorType_IMPL(pHypervisor, type)
#endif //__nvoc_hypervisor_h_disabled

#undef PRIVATE_FIELD


#ifdef HAVE_DESIGNATED_INITIALIZERS
#define SFINIT(f, v) f = v
#else
#define SFINIT(f, v) v
#endif

// OPS for different hypervisor operations at runtime

//
// A hypervisor specific function for post detection (after hypervisor type
// has been identified). As the post detection, there could be hypervisor
// specific requirement to detect root/parent/child partition.
//
typedef NV_STATUS (*HypervisorPostDetection)(struct OBJOS *, NvBool *);

//
// A hypervisor specific function to identify if a hypervisor
// is allowed to run GPUs present in the mask (in: param) under
// PCI-E P2P configuration
//
typedef NvBool (*HypervisorIsPcieP2PSupported)(NvU32);

typedef struct _hypervisor_ops
{
    const char *hypervisorName;
    const char *hypervisorSig;
    HypervisorPostDetection hypervisorPostDetection;
    HypervisorIsPcieP2PSupported hypervisorIsPcieP2PSupported;
} HYPERVISOR_OPS, *PHYPERVISOR_OPS;

extern HYPERVISOR_OPS kvmHypervisorOps;
extern HYPERVISOR_OPS xenHypervisorOps;
extern HYPERVISOR_OPS hypervHypervisorOps;
extern HYPERVISOR_OPS vmwareHypervisorOps;

#endif // HYPERVISOR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HYPERVISOR_NVOC_H_
