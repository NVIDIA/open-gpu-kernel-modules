
#ifndef _G_KEYSTORE_NVOC_H_
#define _G_KEYSTORE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_keystore_nvoc.h"

#ifndef KEYSTORE_H
#define KEYSTORE_H

#include "gpu/gpu.h"
#include "nvport/nvport.h"

/****************************************************************************\
 *                                                                           *
 *      Keystore module header.                                              *
 *                                                                           *
 ****************************************************************************/

#define KEYSTORE_AES_256_GCM_KEY_SIZE_BYTES   (32)
#define KEYSTORE_AES_256_GCM_IV_SIZE_BYTES    (12)

#define KEYSTORE_HMAC_SHA256_KEY_SIZE_BYTES   (32)
#define KEYSTORE_HMAC_SHA256_NONCE_SIZE_BYTES (32)

#define KEYSTORE_HKDF_SHA256_KEY_SIZE_BYTES   (32)

#define KEYSTORE_KEY_SIZE_BYTES_MAX           (32)

#define KEYSTORE_MAX_KEYS                     (128)

typedef enum
{
    KEYSTORE_USAGE_AES_256_GCM,
    KEYSTORE_USAGE_HMAC_SHA256,
    KEYSTORE_USAGE_HKDF_SHA256,
    KEYSTORE_USAGE_INVALID,
} KEY_USAGE;

#define KEYSTORE_USAGE_IS_VALID(usage) ((usage) < KEYSTORE_USAGE_INVALID)

#define KEY_USAGE_IS_HKDF(usage) ((usage) >= KEYSTORE_USAGE_HKDF_SHA256 && (usage) < KEYSTORE_USAGE_INVALID)

#define KEYSTORE_USAGE_TO_KEY_SIZE_BYTES(usage) \
    ((usage) == KEYSTORE_USAGE_AES_256_GCM ? KEYSTORE_AES_256_GCM_KEY_SIZE_BYTES : \
    (usage) == KEYSTORE_USAGE_HMAC_SHA256 ? KEYSTORE_HMAC_SHA256_KEY_SIZE_BYTES : \
    (usage) == KEYSTORE_USAGE_HKDF_SHA256 ? KEYSTORE_HKDF_SHA256_KEY_SIZE_BYTES : 0)

typedef struct
{
    NvU8 iv[KEYSTORE_AES_256_GCM_IV_SIZE_BYTES];
    NvU8 ivMask[KEYSTORE_AES_256_GCM_IV_SIZE_BYTES];
} AES_256_GCM_DATA;

typedef struct
{
    NvU8 nonce[KEYSTORE_HMAC_SHA256_NONCE_SIZE_BYTES];
} HMAC_SHA256_DATA;

typedef struct
{
    KEY_USAGE usage;
    NvU8 key[KEYSTORE_KEY_SIZE_BYTES_MAX];
    NvLength keySizeBytes;
    union
    {
        AES_256_GCM_DATA aes256gcm;
        HMAC_SHA256_DATA hmacsha256;
    } data;
} KEYSTORE_KEY;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KEYSTORE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Keystore;
struct NVOC_METADATA__Object;


struct Keystore {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Keystore *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct Keystore *__nvoc_pbase_Keystore;    // keystore

    // Data members
    NvBool PRIVATE_FIELD(m_bInitialized);
    KEYSTORE_KEY *PRIVATE_FIELD(m_keys)[128];
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Keystore {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_Keystore
#define __nvoc_class_id_Keystore 0x474273u
typedef struct Keystore Keystore;
#endif /* __nvoc_class_id_Keystore */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Keystore;

#define __staticCast_Keystore(pThis) \
    ((pThis)->__nvoc_pbase_Keystore)

#ifdef __nvoc_keystore_h_disabled
#define __dynamicCast_Keystore(pThis) ((Keystore*) NULL)
#else //__nvoc_keystore_h_disabled
#define __dynamicCast_Keystore(pThis) \
    ((Keystore*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Keystore)))
#endif //__nvoc_keystore_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Keystore(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Keystore(Keystore**, Dynamic*, NvU32);
#define __objCreate_Keystore(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_Keystore((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
NV_STATUS keystoreConstruct_IMPL(struct Keystore *pKeystore);
#define __nvoc_keystoreConstruct(pKeystore) keystoreConstruct_IMPL(pKeystore)

void keystoreDestruct_IMPL(struct Keystore *pKeystore);
#define __nvoc_keystoreDestruct(pKeystore) keystoreDestruct_IMPL(pKeystore)

NV_STATUS keystoreSetMasterKey_IMPL(struct Keystore *pKeystore, NvU32 keyId, NvU8 *pKey, NvLength keySizeBytes, KEY_USAGE usage);
#ifdef __nvoc_keystore_h_disabled
static inline NV_STATUS keystoreSetMasterKey(struct Keystore *pKeystore, NvU32 keyId, NvU8 *pKey, NvLength keySizeBytes, KEY_USAGE usage) {
    NV_ASSERT_FAILED_PRECOMP("Keystore was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_keystore_h_disabled
#define keystoreSetMasterKey(pKeystore, keyId, pKey, keySizeBytes, usage) keystoreSetMasterKey_IMPL(pKeystore, keyId, pKey, keySizeBytes, usage)
#endif // __nvoc_keystore_h_disabled

NV_STATUS keystoreDeriveKey_IMPL(struct Keystore *pKeystore, NvU32 srcKeyId, NvU32 dstKeyId, const void *pInfo, NvLength infoSizeBytes, KEY_USAGE usage);
#ifdef __nvoc_keystore_h_disabled
static inline NV_STATUS keystoreDeriveKey(struct Keystore *pKeystore, NvU32 srcKeyId, NvU32 dstKeyId, const void *pInfo, NvLength infoSizeBytes, KEY_USAGE usage) {
    NV_ASSERT_FAILED_PRECOMP("Keystore was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_keystore_h_disabled
#define keystoreDeriveKey(pKeystore, srcKeyId, dstKeyId, pInfo, infoSizeBytes, usage) keystoreDeriveKey_IMPL(pKeystore, srcKeyId, dstKeyId, pInfo, infoSizeBytes, usage)
#endif // __nvoc_keystore_h_disabled

NV_STATUS keystoreSetData_IMPL(struct Keystore *pKeystore, NvU32 keyId, const void *pData, NvLength dataSizeBytes);
#ifdef __nvoc_keystore_h_disabled
static inline NV_STATUS keystoreSetData(struct Keystore *pKeystore, NvU32 keyId, const void *pData, NvLength dataSizeBytes) {
    NV_ASSERT_FAILED_PRECOMP("Keystore was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_keystore_h_disabled
#define keystoreSetData(pKeystore, keyId, pData, dataSizeBytes) keystoreSetData_IMPL(pKeystore, keyId, pData, dataSizeBytes)
#endif // __nvoc_keystore_h_disabled

NV_STATUS keystoreAllocKey_IMPL(struct Keystore *pKeystore, NvU32 keyId);
#ifdef __nvoc_keystore_h_disabled
static inline NV_STATUS keystoreAllocKey(struct Keystore *pKeystore, NvU32 keyId) {
    NV_ASSERT_FAILED_PRECOMP("Keystore was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_keystore_h_disabled
#define keystoreAllocKey(pKeystore, keyId) keystoreAllocKey_IMPL(pKeystore, keyId)
#endif // __nvoc_keystore_h_disabled

NV_STATUS keystoreGetKey_IMPL(struct Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey);
#ifdef __nvoc_keystore_h_disabled
static inline NV_STATUS keystoreGetKey(struct Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey) {
    NV_ASSERT_FAILED_PRECOMP("Keystore was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_keystore_h_disabled
#define keystoreGetKey(pKeystore, keyId, ppKey) keystoreGetKey_IMPL(pKeystore, keyId, ppKey)
#endif // __nvoc_keystore_h_disabled

NV_STATUS keystoreGetOrAllocKey_IMPL(struct Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey, NvBool *pIsNewKey);
#ifdef __nvoc_keystore_h_disabled
static inline NV_STATUS keystoreGetOrAllocKey(struct Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey, NvBool *pIsNewKey) {
    NV_ASSERT_FAILED_PRECOMP("Keystore was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_keystore_h_disabled
#define keystoreGetOrAllocKey(pKeystore, keyId, ppKey, pIsNewKey) keystoreGetOrAllocKey_IMPL(pKeystore, keyId, ppKey, pIsNewKey)
#endif // __nvoc_keystore_h_disabled

void keystoreFreeKey_IMPL(struct Keystore *pKeystore, NvU32 keyId);
#ifdef __nvoc_keystore_h_disabled
static inline void keystoreFreeKey(struct Keystore *pKeystore, NvU32 keyId) {
    NV_ASSERT_FAILED_PRECOMP("Keystore was disabled!");
}
#else // __nvoc_keystore_h_disabled
#define keystoreFreeKey(pKeystore, keyId) keystoreFreeKey_IMPL(pKeystore, keyId)
#endif // __nvoc_keystore_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD

#ifndef NVOC_KEYSTORE_H_PRIVATE_ACCESS_ALLOWED
#ifndef __nvoc_keystore_h_disabled
#undef keystoreAllocKey
NV_STATUS NVOC_PRIVATE_FUNCTION(keystoreAllocKey)(struct Keystore *pKeystore, NvU32 keyId);
#endif //__nvoc_keystore_h_disabled

#ifndef __nvoc_keystore_h_disabled
#undef keystoreGetKey
NV_STATUS NVOC_PRIVATE_FUNCTION(keystoreGetKey)(struct Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey);
#endif //__nvoc_keystore_h_disabled

#ifndef __nvoc_keystore_h_disabled
#undef keystoreGetOrAllocKey
NV_STATUS NVOC_PRIVATE_FUNCTION(keystoreGetOrAllocKey)(struct Keystore *pKeystore, NvU32 keyId, KEYSTORE_KEY **ppKey, NvBool *pIsNewKey);
#endif //__nvoc_keystore_h_disabled

#ifndef __nvoc_keystore_h_disabled
#undef keystoreFreeKey
void NVOC_PRIVATE_FUNCTION(keystoreFreeKey)(struct Keystore *pKeystore, NvU32 keyId);
#endif //__nvoc_keystore_h_disabled

#endif // NVOC_KEYSTORE_H_PRIVATE_ACCESS_ALLOWED



#endif // KEYSTORE_H
#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KEYSTORE_NVOC_H_
