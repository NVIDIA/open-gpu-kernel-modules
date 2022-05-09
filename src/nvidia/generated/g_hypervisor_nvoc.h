#ifndef _G_HYPERVISOR_NVOC_H_
#define _G_HYPERVISOR_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

typedef struct OBJHYPERVISOR *POBJHYPERVISOR;

#ifndef __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
#define __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
typedef struct OBJHYPERVISOR OBJHYPERVISOR;
#endif /* __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHYPERVISOR
#define __nvoc_class_id_OBJHYPERVISOR 0x33c1ba
#endif /* __nvoc_class_id_OBJHYPERVISOR */


typedef struct HOST_VGPU_DEVICE HOST_VGPU_DEVICE;

#ifdef NVOC_HYPERVISOR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJHYPERVISOR {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct OBJHYPERVISOR *__nvoc_pbase_OBJHYPERVISOR;
    NvBool bDetected;
    NvBool bIsHVMGuest;
    HYPERVISOR_TYPE type;
    NvBool bIsHypervHost;
    NvBool bIsHypervVgpuSupported;
};

#ifndef __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
#define __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
typedef struct OBJHYPERVISOR OBJHYPERVISOR;
#endif /* __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHYPERVISOR
#define __nvoc_class_id_OBJHYPERVISOR 0x33c1ba
#endif /* __nvoc_class_id_OBJHYPERVISOR */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHYPERVISOR;

#define __staticCast_OBJHYPERVISOR(pThis) \
    ((pThis)->__nvoc_pbase_OBJHYPERVISOR)

#ifdef __nvoc_hypervisor_h_disabled
#define __dynamicCast_OBJHYPERVISOR(pThis) ((OBJHYPERVISOR*)NULL)
#else //__nvoc_hypervisor_h_disabled
#define __dynamicCast_OBJHYPERVISOR(pThis) \
    ((OBJHYPERVISOR*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJHYPERVISOR)))
#endif //__nvoc_hypervisor_h_disabled


NV_STATUS __nvoc_objCreateDynamic_OBJHYPERVISOR(OBJHYPERVISOR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJHYPERVISOR(OBJHYPERVISOR**, Dynamic*, NvU32);
#define __objCreate_OBJHYPERVISOR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJHYPERVISOR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

static inline NvBool hypervisorIsVgxHyper_491d52(void) {
    return ((NvBool)(0 != 0));
}

#define hypervisorIsVgxHyper() hypervisorIsVgxHyper_491d52()
#define hypervisorIsVgxHyper_HAL() hypervisorIsVgxHyper()

static inline NvBool hypervisorCheckForAdminAccess(NvHandle hClient, NvU32 rmCtrlId) {
    return ((NvBool)(0 != 0));
}

static inline NvBool hypervisorCheckForObjectAccess(NvHandle hClient) {
    return ((NvBool)(0 != 0));
}

static inline NvBool hypervisorCheckForGspOffloadAccess(POBJGPU pGpu, NvU32 rmCtrlId) {
    return ((NvBool)(0 != 0));
}

static inline NvBool hypervisorIsType(HYPERVISOR_TYPE hyperType) {
    return ((NvBool)(0 != 0));
}

#undef PRIVATE_FIELD


#endif // HYPERVISOR_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_HYPERVISOR_NVOC_H_
