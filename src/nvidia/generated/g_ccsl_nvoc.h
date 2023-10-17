#ifndef _G_CCSL_NVOC_H_
#define _G_CCSL_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_ccsl_nvoc.h"

#ifndef CCSL_H
#define CCSL_H

#include "nvstatus.h"
#include "nvmisc.h"
#include "kernel/gpu/conf_compute/conf_compute.h"
#include "kernel/gpu/conf_compute/ccsl_context.h"

#define CCSL_DIR_HOST_TO_DEVICE 0
#define CCSL_DIR_DEVICE_TO_HOST 1

/****************************************************************************\
 *                                                                           *
 *      CCSL module header.                                                  *
 *                                                                           *
 ****************************************************************************/
#ifdef NVOC_CCSL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Ccsl {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Ccsl *__nvoc_pbase_Ccsl;
};

#ifndef __NVOC_CLASS_Ccsl_TYPEDEF__
#define __NVOC_CLASS_Ccsl_TYPEDEF__
typedef struct Ccsl Ccsl;
#endif /* __NVOC_CLASS_Ccsl_TYPEDEF__ */

#ifndef __nvoc_class_id_Ccsl
#define __nvoc_class_id_Ccsl 0x9bf1a1
#endif /* __nvoc_class_id_Ccsl */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Ccsl;

#define __staticCast_Ccsl(pThis) \
    ((pThis)->__nvoc_pbase_Ccsl)

#ifdef __nvoc_ccsl_h_disabled
#define __dynamicCast_Ccsl(pThis) ((Ccsl*)NULL)
#else //__nvoc_ccsl_h_disabled
#define __dynamicCast_Ccsl(pThis) \
    ((Ccsl*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Ccsl)))
#endif //__nvoc_ccsl_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Ccsl(Ccsl**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Ccsl(Ccsl**, Dynamic*, NvU32);
#define __objCreate_Ccsl(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_Ccsl((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

NV_STATUS ccslContextInitViaChannel_IMPL(pCcslContext *ppCtx, NvHandle hClient, NvHandle hChannel);


#define ccslContextInitViaChannel(ppCtx, hClient, hChannel) ccslContextInitViaChannel_IMPL(ppCtx, hClient, hChannel)
#define ccslContextInitViaChannel_HAL(ppCtx, hClient, hChannel) ccslContextInitViaChannel(ppCtx, hClient, hChannel)

NV_STATUS ccslContextInitViaKeyId_KERNEL(struct ConfidentialCompute *pConfCompute, pCcslContext *ppCtx, NvU32 globalKeyId);


#define ccslContextInitViaKeyId(pConfCompute, ppCtx, globalKeyId) ccslContextInitViaKeyId_KERNEL(pConfCompute, ppCtx, globalKeyId)
#define ccslContextInitViaKeyId_HAL(pConfCompute, ppCtx, globalKeyId) ccslContextInitViaKeyId(pConfCompute, ppCtx, globalKeyId)

NV_STATUS ccslRotateIv_IMPL(pCcslContext ctx, NvU8 direction);


#define ccslRotateIv(ctx, direction) ccslRotateIv_IMPL(ctx, direction)
#define ccslRotateIv_HAL(ctx, direction) ccslRotateIv(ctx, direction)

NV_STATUS ccslEncryptWithIv_IMPL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, NvU8 *encryptIv, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, NvU8 *authTagBuffer);


#define ccslEncryptWithIv(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncryptWithIv_IMPL(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslEncryptWithIv_HAL(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncryptWithIv(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslEncrypt_KERNEL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, NvU8 *authTagBuffer);


#define ccslEncrypt(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncrypt_KERNEL(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslEncrypt_HAL(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncrypt(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslDecrypt_KERNEL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, const NvU8 *decryptIv, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, const NvU8 *authTagBuffer);


#define ccslDecrypt(ctx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslDecrypt_KERNEL(ctx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslDecrypt_HAL(ctx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslDecrypt(ctx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslSign_IMPL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, NvU8 *authTagBuffer);


#define ccslSign(ctx, bufferSize, inputBuffer, authTagBuffer) ccslSign_IMPL(ctx, bufferSize, inputBuffer, authTagBuffer)
#define ccslSign_HAL(ctx, bufferSize, inputBuffer, authTagBuffer) ccslSign(ctx, bufferSize, inputBuffer, authTagBuffer)

NV_STATUS ccslQueryMessagePool_IMPL(pCcslContext ctx, NvU8 direction, NvU64 *messageNum);


#define ccslQueryMessagePool(ctx, direction, messageNum) ccslQueryMessagePool_IMPL(ctx, direction, messageNum)
#define ccslQueryMessagePool_HAL(ctx, direction, messageNum) ccslQueryMessagePool(ctx, direction, messageNum)

NV_STATUS ccslIncrementIv_IMPL(pCcslContext pCtx, NvU8 direction, NvU64 increment, NvU8 *iv);


#define ccslIncrementIv(pCtx, direction, increment, iv) ccslIncrementIv_IMPL(pCtx, direction, increment, iv)
#define ccslIncrementIv_HAL(pCtx, direction, increment, iv) ccslIncrementIv(pCtx, direction, increment, iv)

void ccslContextClear_IMPL(pCcslContext ctx);

#define ccslContextClear(ctx) ccslContextClear_IMPL(ctx)
NV_STATUS ccslIncrementCounter_IMPL(pCcslContext pCtx, NvU8 *ctr, NvU64 increment);

#define ccslIncrementCounter(pCtx, ctr, increment) ccslIncrementCounter_IMPL(pCtx, ctr, increment)
#undef PRIVATE_FIELD

#ifndef NVOC_CCSL_H_PRIVATE_ACCESS_ALLOWED
#undef ccslIncrementCounter
NV_STATUS NVOC_PRIVATE_FUNCTION(ccslIncrementCounter)(pCcslContext pCtx, NvU8 *ctr, NvU64 increment);

#endif // NVOC_CCSL_H_PRIVATE_ACCESS_ALLOWED


#endif // CCSL_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CCSL_NVOC_H_
