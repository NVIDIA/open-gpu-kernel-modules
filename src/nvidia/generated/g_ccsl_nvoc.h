
#ifndef _G_CCSL_NVOC_H_
#define _G_CCSL_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CCSL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI
struct NVOC_METADATA__Ccsl;


struct Ccsl {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Ccsl *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Ancestor object pointers for `staticCast` feature
    struct Ccsl *__nvoc_pbase_Ccsl;    // ccsl
};


// Metadata with per-class RTTI
struct NVOC_METADATA__Ccsl {
    const struct NVOC_RTTI rtti;
};

#ifndef __NVOC_CLASS_Ccsl_TYPEDEF__
#define __NVOC_CLASS_Ccsl_TYPEDEF__
typedef struct Ccsl Ccsl;
#endif /* __NVOC_CLASS_Ccsl_TYPEDEF__ */

#ifndef __nvoc_class_id_Ccsl
#define __nvoc_class_id_Ccsl 0x9bf1a1
#endif /* __nvoc_class_id_Ccsl */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Ccsl;

#define __staticCast_Ccsl(pThis) \
    ((pThis)->__nvoc_pbase_Ccsl)

#ifdef __nvoc_ccsl_h_disabled
#define __dynamicCast_Ccsl(pThis) ((Ccsl*) NULL)
#else //__nvoc_ccsl_h_disabled
#define __dynamicCast_Ccsl(pThis) \
    ((Ccsl*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Ccsl)))
#endif //__nvoc_ccsl_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Ccsl(Ccsl**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Ccsl(Ccsl**, Dynamic*, NvU32);
#define __objCreate_Ccsl(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_Ccsl((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS ccslContextInitViaChannel_IMPL(pCcslContext *ppCtx, NvHandle hClient, NvHandle hSubdevice, NvHandle hChannel);


#define ccslContextInitViaChannel(ppCtx, hClient, hSubdevice, hChannel) ccslContextInitViaChannel_IMPL(ppCtx, hClient, hSubdevice, hChannel)
#define ccslContextInitViaChannel_HAL(ppCtx, hClient, hSubdevice, hChannel) ccslContextInitViaChannel(ppCtx, hClient, hSubdevice, hChannel)

NV_STATUS ccslContextInitViaKeyId_KERNEL(struct ConfidentialCompute *pConfCompute, pCcslContext *ppCtx, NvU32 globalKeyId);


#define ccslContextInitViaKeyId(pConfCompute, ppCtx, globalKeyId) ccslContextInitViaKeyId_KERNEL(pConfCompute, ppCtx, globalKeyId)
#define ccslContextInitViaKeyId_HAL(pConfCompute, ppCtx, globalKeyId) ccslContextInitViaKeyId(pConfCompute, ppCtx, globalKeyId)

NV_STATUS ccslContextUpdate_KERNEL(pCcslContext ctx);


#define ccslContextUpdate(ctx) ccslContextUpdate_KERNEL(ctx)
#define ccslContextUpdate_HAL(ctx) ccslContextUpdate(ctx)

NV_STATUS ccslRotateIv_IMPL(pCcslContext ctx, NvU8 direction);


#define ccslRotateIv(ctx, direction) ccslRotateIv_IMPL(ctx, direction)
#define ccslRotateIv_HAL(ctx, direction) ccslRotateIv(ctx, direction)

NV_STATUS ccslEncryptWithIv_IMPL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, NvU8 *encryptIv, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, NvU8 *authTagBuffer);


#define ccslEncryptWithIv(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncryptWithIv_IMPL(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslEncryptWithIv_HAL(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncryptWithIv(ctx, bufferSize, inputBuffer, encryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslEncrypt_KERNEL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, NvU8 *authTagBuffer);


#define ccslEncrypt(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncrypt_KERNEL(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslEncrypt_HAL(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncrypt(ctx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslEncryptWithRotationChecks_KERNEL(pCcslContext pCtx, NvU32 bufferSize, const NvU8 *inputBuffer, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, NvU8 *authTagBuffer);


#define ccslEncryptWithRotationChecks(pCtx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncryptWithRotationChecks_KERNEL(pCtx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslEncryptWithRotationChecks_HAL(pCtx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslEncryptWithRotationChecks(pCtx, bufferSize, inputBuffer, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslDecrypt_KERNEL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, const NvU8 *decryptIv, NvU32 keyRotationId, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, const NvU8 *authTagBuffer);


#define ccslDecrypt(ctx, bufferSize, inputBuffer, decryptIv, keyRotationId, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslDecrypt_KERNEL(ctx, bufferSize, inputBuffer, decryptIv, keyRotationId, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslDecrypt_HAL(ctx, bufferSize, inputBuffer, decryptIv, keyRotationId, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslDecrypt(ctx, bufferSize, inputBuffer, decryptIv, keyRotationId, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslDecryptWithRotationChecks_KERNEL(pCcslContext pCtx, NvU32 bufferSize, const NvU8 *inputBuffer, const NvU8 *decryptIv, const NvU8 *aadBuffer, NvU32 aadSize, NvU8 *outputBuffer, const NvU8 *authTagBuffer);


#define ccslDecryptWithRotationChecks(pCtx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslDecryptWithRotationChecks_KERNEL(pCtx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)
#define ccslDecryptWithRotationChecks_HAL(pCtx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer) ccslDecryptWithRotationChecks(pCtx, bufferSize, inputBuffer, decryptIv, aadBuffer, aadSize, outputBuffer, authTagBuffer)

NV_STATUS ccslSign_IMPL(pCcslContext ctx, NvU32 bufferSize, const NvU8 *inputBuffer, NvU8 *authTagBuffer);


#define ccslSign(ctx, bufferSize, inputBuffer, authTagBuffer) ccslSign_IMPL(ctx, bufferSize, inputBuffer, authTagBuffer)
#define ccslSign_HAL(ctx, bufferSize, inputBuffer, authTagBuffer) ccslSign(ctx, bufferSize, inputBuffer, authTagBuffer)

NV_STATUS ccslQueryMessagePool_IMPL(pCcslContext ctx, NvU8 direction, NvU64 *messageNum);


#define ccslQueryMessagePool(ctx, direction, messageNum) ccslQueryMessagePool_IMPL(ctx, direction, messageNum)
#define ccslQueryMessagePool_HAL(ctx, direction, messageNum) ccslQueryMessagePool(ctx, direction, messageNum)

NV_STATUS ccslIncrementIv_IMPL(pCcslContext pCtx, NvU8 direction, NvU64 increment, NvU8 *iv);


#define ccslIncrementIv(pCtx, direction, increment, iv) ccslIncrementIv_IMPL(pCtx, direction, increment, iv)
#define ccslIncrementIv_HAL(pCtx, direction, increment, iv) ccslIncrementIv(pCtx, direction, increment, iv)

NV_STATUS ccslLogEncryption_IMPL(pCcslContext pCtx, NvU8 direction, NvU32 bufferSize);


#define ccslLogEncryption(pCtx, direction, bufferSize) ccslLogEncryption_IMPL(pCtx, direction, bufferSize)
#define ccslLogEncryption_HAL(pCtx, direction, bufferSize) ccslLogEncryption(pCtx, direction, bufferSize)

static inline void ccslUpdateRotationThreshold_b3696a(NvU64 threshold) {
    return;
}


#define ccslUpdateRotationThreshold(threshold) ccslUpdateRotationThreshold_b3696a(threshold)
#define ccslUpdateRotationThreshold_HAL(threshold) ccslUpdateRotationThreshold(threshold)

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
