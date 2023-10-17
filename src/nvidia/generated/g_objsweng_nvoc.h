#ifndef _G_OBJSWENG_NVOC_H_
#define _G_OBJSWENG_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** Modular includes *****************************\
*                                                                           *
*   OBJSWENG implements the 'Software Engine' from the GPU host             *
*   perspective. That is the software component that emulates a GPU host    *
*   engine by implementing SW methods in the driver.                        *
*                                                                           *
****************************************************************************/

#include "g_objsweng_nvoc.h"

#ifndef _OBJSWENG_H_
#define _OBJSWENG_H_

#include "core/core.h"
#include "gpu/eng_state.h"

typedef struct OBJSWENG *POBJSWENG;

#ifdef NVOC_OBJSWENG_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJSWENG {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct OBJSWENG *__nvoc_pbase_OBJSWENG;
    NV_STATUS (*__swengConstructEngine__)(struct OBJGPU *, struct OBJSWENG *, ENGDESCRIPTOR);
    NV_STATUS (*__swengStateLoad__)(POBJGPU, struct OBJSWENG *, NvU32);
    NV_STATUS (*__swengStateUnload__)(POBJGPU, struct OBJSWENG *, NvU32);
    NV_STATUS (*__swengStateInitLocked__)(POBJGPU, struct OBJSWENG *);
    NV_STATUS (*__swengStatePreLoad__)(POBJGPU, struct OBJSWENG *, NvU32);
    NV_STATUS (*__swengStatePostUnload__)(POBJGPU, struct OBJSWENG *, NvU32);
    void (*__swengStateDestroy__)(POBJGPU, struct OBJSWENG *);
    NV_STATUS (*__swengStatePreUnload__)(POBJGPU, struct OBJSWENG *, NvU32);
    NV_STATUS (*__swengStateInitUnlocked__)(POBJGPU, struct OBJSWENG *);
    void (*__swengInitMissing__)(POBJGPU, struct OBJSWENG *);
    NV_STATUS (*__swengStatePreInitLocked__)(POBJGPU, struct OBJSWENG *);
    NV_STATUS (*__swengStatePreInitUnlocked__)(POBJGPU, struct OBJSWENG *);
    NV_STATUS (*__swengStatePostLoad__)(POBJGPU, struct OBJSWENG *, NvU32);
    NvBool (*__swengIsPresent__)(POBJGPU, struct OBJSWENG *);
};

#ifndef __NVOC_CLASS_OBJSWENG_TYPEDEF__
#define __NVOC_CLASS_OBJSWENG_TYPEDEF__
typedef struct OBJSWENG OBJSWENG;
#endif /* __NVOC_CLASS_OBJSWENG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSWENG
#define __nvoc_class_id_OBJSWENG 0x95a6f5
#endif /* __nvoc_class_id_OBJSWENG */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSWENG;

#define __staticCast_OBJSWENG(pThis) \
    ((pThis)->__nvoc_pbase_OBJSWENG)

#ifdef __nvoc_objsweng_h_disabled
#define __dynamicCast_OBJSWENG(pThis) ((OBJSWENG*)NULL)
#else //__nvoc_objsweng_h_disabled
#define __dynamicCast_OBJSWENG(pThis) \
    ((OBJSWENG*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJSWENG)))
#endif //__nvoc_objsweng_h_disabled

#define PDB_PROP_SWENG_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_SWENG_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJSWENG(OBJSWENG**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJSWENG(OBJSWENG**, Dynamic*, NvU32);
#define __objCreate_OBJSWENG(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJSWENG((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define swengConstructEngine(pGpu, pSweng, arg0) swengConstructEngine_DISPATCH(pGpu, pSweng, arg0)
#define swengStateLoad(pGpu, pEngstate, arg0) swengStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define swengStateUnload(pGpu, pEngstate, arg0) swengStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define swengStateInitLocked(pGpu, pEngstate) swengStateInitLocked_DISPATCH(pGpu, pEngstate)
#define swengStatePreLoad(pGpu, pEngstate, arg0) swengStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define swengStatePostUnload(pGpu, pEngstate, arg0) swengStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define swengStateDestroy(pGpu, pEngstate) swengStateDestroy_DISPATCH(pGpu, pEngstate)
#define swengStatePreUnload(pGpu, pEngstate, arg0) swengStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define swengStateInitUnlocked(pGpu, pEngstate) swengStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swengInitMissing(pGpu, pEngstate) swengInitMissing_DISPATCH(pGpu, pEngstate)
#define swengStatePreInitLocked(pGpu, pEngstate) swengStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define swengStatePreInitUnlocked(pGpu, pEngstate) swengStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swengStatePostLoad(pGpu, pEngstate, arg0) swengStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define swengIsPresent(pGpu, pEngstate) swengIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS swengConstructEngine_IMPL(struct OBJGPU *pGpu, struct OBJSWENG *pSweng, ENGDESCRIPTOR arg0);

static inline NV_STATUS swengConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pSweng, ENGDESCRIPTOR arg0) {
    return pSweng->__swengConstructEngine__(pGpu, pSweng, arg0);
}

static inline NV_STATUS swengStateLoad_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return pEngstate->__swengStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swengStateUnload_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return pEngstate->__swengStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swengStateInitLocked_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__swengStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePreLoad_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return pEngstate->__swengStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swengStatePostUnload_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return pEngstate->__swengStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void swengStateDestroy_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    pEngstate->__swengStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePreUnload_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return pEngstate->__swengStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swengStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__swengStateInitUnlocked__(pGpu, pEngstate);
}

static inline void swengInitMissing_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    pEngstate->__swengInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__swengStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__swengStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePostLoad_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate, NvU32 arg0) {
    return pEngstate->__swengStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool swengIsPresent_DISPATCH(POBJGPU pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__swengIsPresent__(pGpu, pEngstate);
}

#undef PRIVATE_FIELD


#endif // _OBJSWENG_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OBJSWENG_NVOC_H_
