
#ifndef _G_VIRT_MEM_MGR_NVOC_H_
#define _G_VIRT_MEM_MGR_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_virt_mem_mgr_nvoc.h"

#ifndef VIRT_MEM_MGR_H
#define VIRT_MEM_MGR_H

/**************** Resource Manager Defines and Structures ******************\
*       Defines and structures used for Virtual Memory Management Object.   *
\***************************************************************************/

#include "mem_mgr/vaspace.h"

typedef struct OBJVMM *POBJVMM;

#ifndef __NVOC_CLASS_OBJVMM_TYPEDEF__
#define __NVOC_CLASS_OBJVMM_TYPEDEF__
typedef struct OBJVMM OBJVMM;
#endif /* __NVOC_CLASS_OBJVMM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVMM
#define __nvoc_class_id_OBJVMM 0xa030ab
#endif /* __nvoc_class_id_OBJVMM */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VIRT_MEM_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJVMM {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJVMM *__nvoc_pbase_OBJVMM;    // vmm
};

#ifndef __NVOC_CLASS_OBJVMM_TYPEDEF__
#define __NVOC_CLASS_OBJVMM_TYPEDEF__
typedef struct OBJVMM OBJVMM;
#endif /* __NVOC_CLASS_OBJVMM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVMM
#define __nvoc_class_id_OBJVMM 0xa030ab
#endif /* __nvoc_class_id_OBJVMM */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVMM;

#define __staticCast_OBJVMM(pThis) \
    ((pThis)->__nvoc_pbase_OBJVMM)

#ifdef __nvoc_virt_mem_mgr_h_disabled
#define __dynamicCast_OBJVMM(pThis) ((OBJVMM*)NULL)
#else //__nvoc_virt_mem_mgr_h_disabled
#define __dynamicCast_OBJVMM(pThis) \
    ((OBJVMM*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJVMM)))
#endif //__nvoc_virt_mem_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJVMM(OBJVMM**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJVMM(OBJVMM**, Dynamic*, NvU32);
#define __objCreate_OBJVMM(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJVMM((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS vmmCreateVaspace_IMPL(struct OBJVMM *pVmm, NvU32 _class, NvU32 vaspaceId, NvU32 gpuMask, NvU64 vaStart, NvU64 vaLimit, NvU64 vaInternalStart, NvU64 vaInternalEnd, struct OBJVASPACE *pPteSpaceMap, NvU32 flags, struct OBJVASPACE **ppVAS);

#ifdef __nvoc_virt_mem_mgr_h_disabled
static inline NV_STATUS vmmCreateVaspace(struct OBJVMM *pVmm, NvU32 _class, NvU32 vaspaceId, NvU32 gpuMask, NvU64 vaStart, NvU64 vaLimit, NvU64 vaInternalStart, NvU64 vaInternalEnd, struct OBJVASPACE *pPteSpaceMap, NvU32 flags, struct OBJVASPACE **ppVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJVMM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_mgr_h_disabled
#define vmmCreateVaspace(pVmm, _class, vaspaceId, gpuMask, vaStart, vaLimit, vaInternalStart, vaInternalEnd, pPteSpaceMap, flags, ppVAS) vmmCreateVaspace_IMPL(pVmm, _class, vaspaceId, gpuMask, vaStart, vaLimit, vaInternalStart, vaInternalEnd, pPteSpaceMap, flags, ppVAS)
#endif //__nvoc_virt_mem_mgr_h_disabled

void vmmDestroyVaspace_IMPL(struct OBJVMM *pVmm, struct OBJVASPACE *pVAS);

#ifdef __nvoc_virt_mem_mgr_h_disabled
static inline void vmmDestroyVaspace(struct OBJVMM *pVmm, struct OBJVASPACE *pVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJVMM was disabled!");
}
#else //__nvoc_virt_mem_mgr_h_disabled
#define vmmDestroyVaspace(pVmm, pVAS) vmmDestroyVaspace_IMPL(pVmm, pVAS)
#endif //__nvoc_virt_mem_mgr_h_disabled

NV_STATUS vmmGetVaspaceFromId_IMPL(struct OBJVMM *pVmm, NvU32 vaspaceId, NvU32 classId, struct OBJVASPACE **ppVAS);

#ifdef __nvoc_virt_mem_mgr_h_disabled
static inline NV_STATUS vmmGetVaspaceFromId(struct OBJVMM *pVmm, NvU32 vaspaceId, NvU32 classId, struct OBJVASPACE **ppVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJVMM was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_mgr_h_disabled
#define vmmGetVaspaceFromId(pVmm, vaspaceId, classId, ppVAS) vmmGetVaspaceFromId_IMPL(pVmm, vaspaceId, classId, ppVAS)
#endif //__nvoc_virt_mem_mgr_h_disabled

#undef PRIVATE_FIELD


#endif // VIRT_MEM_MGR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIRT_MEM_MGR_NVOC_H_
