#ifndef _G_HOST_ENG_NVOC_H_
#define _G_HOST_ENG_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_host_eng_nvoc.h"

#ifndef HOST_ENG_H
#define HOST_ENG_H

/*!
 * @file   host_eng.h
 * @brief  Provides definitions for all OBJHOSTENG data structures and interfaces.
 */

#include "core/core.h"
#include "gpu/gpu_timeout.h"

#include "kernel/gpu/fifo/kernel_channel.h"

typedef struct OBJHOSTENG *POBJHOSTENG;

/*!
 * Interface class for all Hosteng modules.
 */
#ifdef NVOC_HOST_ENG_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJHOSTENG {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJHOSTENG *__nvoc_pbase_OBJHOSTENG;
    NV_STATUS (*__hostengHaltEngine__)(struct OBJGPU *, struct OBJHOSTENG *);
    NV_STATUS (*__hostengHaltAndReset__)(struct OBJGPU *, struct OBJHOSTENG *, RMTIMEOUT *);
    NV_STATUS (*__hostengReset__)(struct OBJGPU *, struct OBJHOSTENG *, NvBool, struct KernelChannel *, struct KernelChannel **);
};

#ifndef __NVOC_CLASS_OBJHOSTENG_TYPEDEF__
#define __NVOC_CLASS_OBJHOSTENG_TYPEDEF__
typedef struct OBJHOSTENG OBJHOSTENG;
#endif /* __NVOC_CLASS_OBJHOSTENG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHOSTENG
#define __nvoc_class_id_OBJHOSTENG 0xb356e7
#endif /* __nvoc_class_id_OBJHOSTENG */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJHOSTENG;

#define __staticCast_OBJHOSTENG(pThis) \
    ((pThis)->__nvoc_pbase_OBJHOSTENG)

#ifdef __nvoc_host_eng_h_disabled
#define __dynamicCast_OBJHOSTENG(pThis) ((OBJHOSTENG*)NULL)
#else //__nvoc_host_eng_h_disabled
#define __dynamicCast_OBJHOSTENG(pThis) \
    ((OBJHOSTENG*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJHOSTENG)))
#endif //__nvoc_host_eng_h_disabled


NV_STATUS __nvoc_objCreateDynamic_OBJHOSTENG(OBJHOSTENG**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJHOSTENG(OBJHOSTENG**, Dynamic*, NvU32);
#define __objCreate_OBJHOSTENG(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJHOSTENG((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define hostengHaltEngine(pGpu, pHosteng) hostengHaltEngine_DISPATCH(pGpu, pHosteng)
#define hostengHaltAndReset(pGpu, pHosteng, pRmTimeout) hostengHaltAndReset_DISPATCH(pGpu, pHosteng, pRmTimeout)
#define hostengReset(pGpu, pHosteng, bReload, pKernelChannel, ppCurrentKernelChannel) hostengReset_DISPATCH(pGpu, pHosteng, bReload, pKernelChannel, ppCurrentKernelChannel)
NV_STATUS hostengHaltEngine_IMPL(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng);

static inline NV_STATUS hostengHaltEngine_DISPATCH(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng) {
    return pHosteng->__hostengHaltEngine__(pGpu, pHosteng);
}

NV_STATUS hostengHaltAndReset_IMPL(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng, RMTIMEOUT *pRmTimeout);

static inline NV_STATUS hostengHaltAndReset_DISPATCH(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng, RMTIMEOUT *pRmTimeout) {
    return pHosteng->__hostengHaltAndReset__(pGpu, pHosteng, pRmTimeout);
}

NV_STATUS hostengReset_IMPL(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng, NvBool bReload, struct KernelChannel *pKernelChannel, struct KernelChannel **ppCurrentKernelChannel);

static inline NV_STATUS hostengReset_DISPATCH(struct OBJGPU *pGpu, struct OBJHOSTENG *pHosteng, NvBool bReload, struct KernelChannel *pKernelChannel, struct KernelChannel **ppCurrentKernelChannel) {
    return pHosteng->__hostengReset__(pGpu, pHosteng, bReload, pKernelChannel, ppCurrentKernelChannel);
}

#undef PRIVATE_FIELD


#endif // HOST_ENG_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_HOST_ENG_NVOC_H_
