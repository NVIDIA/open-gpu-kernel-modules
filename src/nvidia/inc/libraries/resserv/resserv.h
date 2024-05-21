/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_resserv_nvoc.h"

#ifndef _RESSERV_H_
#define _RESSERV_H_

#include "nvoc/object.h"

#include "containers/list.h"
#include "containers/map.h"
#include "containers/multimap.h"

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvos.h"
#include "nvsecurityinfo.h"
#include "rs_access.h"

#if LOCK_VAL_ENABLED
#include "lockval/lockval.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (RS_STANDALONE)
#include <stddef.h>

#ifndef NV_PRINTF
extern int g_debugLevel;
#define NV_PRINTF(level, format, ...) if (g_debugLevel) { printf(format, ##__VA_ARGS__); }
#endif
#include "utils/nvprintf.h"
#endif

//
// Forward declarations
//
typedef struct RsServer RsServer;
typedef struct RsDomain RsDomain;
typedef struct CLIENT_ENTRY CLIENT_ENTRY;
typedef struct RsResourceDep RsResourceDep;
typedef struct RsResourceRef RsResourceRef;
typedef struct RsInterMapping RsInterMapping;
typedef struct RsCpuMapping RsCpuMapping;

// RS-TODO INTERNAL and EXTERNAL params should be different structures
typedef struct RS_CLIENT_FREE_PARAMS_INTERNAL RS_CLIENT_FREE_PARAMS_INTERNAL;
typedef struct RS_CLIENT_FREE_PARAMS_INTERNAL RS_CLIENT_FREE_PARAMS;
typedef struct RS_RES_ALLOC_PARAMS_INTERNAL RS_RES_ALLOC_PARAMS_INTERNAL;
typedef struct RS_RES_ALLOC_PARAMS_INTERNAL RS_RES_ALLOC_PARAMS;
typedef struct RS_RES_DUP_PARAMS_INTERNAL RS_RES_DUP_PARAMS_INTERNAL;
typedef struct RS_RES_DUP_PARAMS_INTERNAL RS_RES_DUP_PARAMS;
typedef struct RS_RES_SHARE_PARAMS_INTERNAL RS_RES_SHARE_PARAMS_INTERNAL;
typedef struct RS_RES_SHARE_PARAMS_INTERNAL RS_RES_SHARE_PARAMS;
typedef struct RS_RES_ALLOC_PARAMS_INTERNAL RS_CLIENT_ALLOC_PARAMS_INTERNAL;
typedef struct RS_RES_ALLOC_PARAMS_INTERNAL RS_CLIENT_ALLOC_PARAMS;
typedef struct RS_RES_FREE_PARAMS_INTERNAL RS_RES_FREE_PARAMS_INTERNAL;
typedef struct RS_RES_FREE_PARAMS_INTERNAL RS_RES_FREE_PARAMS;
typedef struct RS_RES_CONTROL_PARAMS_INTERNAL RS_RES_CONTROL_PARAMS_INTERNAL;
typedef struct RS_RES_CONTROL_PARAMS_INTERNAL RS_RES_CONTROL_PARAMS;
typedef struct RS_RES_CONTROL_PARAMS_INTERNAL RS_LEGACY_CONTROL_PARAMS;
typedef struct RS_LEGACY_ALLOC_PARAMS RS_LEGACY_ALLOC_PARAMS;
typedef struct RS_LEGACY_FREE_PARAMS RS_LEGACY_FREE_PARAMS;

typedef struct RS_CPU_MAP_PARAMS RS_CPU_MAP_PARAMS;
typedef struct RS_CPU_UNMAP_PARAMS RS_CPU_UNMAP_PARAMS;
typedef struct RS_INTER_MAP_PARAMS RS_INTER_MAP_PARAMS;
typedef struct RS_INTER_UNMAP_PARAMS RS_INTER_UNMAP_PARAMS;

// Forward declarations for structs defined by user
typedef struct RS_RES_MAP_TO_PARAMS RS_RES_MAP_TO_PARAMS;
typedef struct RS_RES_UNMAP_FROM_PARAMS RS_RES_UNMAP_FROM_PARAMS;
typedef struct RS_INTER_MAP_PRIVATE RS_INTER_MAP_PRIVATE;
typedef struct RS_INTER_UNMAP_PRIVATE RS_INTER_UNMAP_PRIVATE;
typedef struct RS_CPU_MAPPING_PRIVATE RS_CPU_MAPPING_PRIVATE;

typedef struct RS_CPU_MAPPING_BACK_REF RS_CPU_MAPPING_BACK_REF;
typedef struct RS_INTER_MAPPING_BACK_REF RS_INTER_MAPPING_BACK_REF;
typedef struct RS_FREE_STACK RS_FREE_STACK;
typedef struct CALL_CONTEXT CALL_CONTEXT;
typedef struct ACCESS_CONTROL ACCESS_CONTROL;
typedef struct RS_ITERATOR RS_ITERATOR;
typedef struct RS_ORDERED_ITERATOR RS_ORDERED_ITERATOR;
typedef struct RS_SHARE_ITERATOR RS_SHARE_ITERATOR;
typedef struct API_STATE API_STATE;
typedef struct RS_LOCK_INFO RS_LOCK_INFO;
typedef struct RS_CONTROL_COOKIE RS_CONTROL_COOKIE;
typedef NV_STATUS RsCtrlFunc(struct RS_RES_CONTROL_PARAMS_INTERNAL*);

class RsClient;
class RsResource;
class RsShared;

MAKE_LIST(RsResourceRefList, RsResourceRef*);
MAKE_LIST(RsResourceList, RsResource*);
MAKE_LIST(RsHandleList, NvHandle);
MAKE_LIST(RsShareList, RS_SHARE_POLICY);
MAKE_MULTIMAP(RsIndex, RsResourceRef*);

typedef NV_STATUS (*CtrlImpl_t)(RsClient*, RsResource*, void*);

typedef void *PUID_TOKEN;

//
// Defines
//

/// Domain handles must start at this base value
#define RS_DOMAIN_HANDLE_BASE           0xD0D00000

/// Client handles must start at this base value
#define RS_CLIENT_HANDLE_BASE           0xC1D00000

/// Internal Client handles start at this base value
#define RS_CLIENT_INTERNAL_HANDLE_BASE  0xC1E00000

/// VF Client handles start at this base value
#define RS_CLIENT_VF_HANDLE_BASE        0xE0000000

/// Get the VF client handle range for gfid
#define RS_CLIENT_GET_VF_HANDLE_BASE(gfid) (RS_CLIENT_VF_HANDLE_BASE + ((gfid) - 1) * RS_CLIENT_HANDLE_MAX)

//
// Print a warning if any client's resource count exceeds this
// threshold. Unless this was intentional, this is likely a client bug.
//
#define RS_CLIENT_RESOURCE_WARNING_THRESHOLD 100000

#define RS_CLIENT_HANDLE_MAX            0x100000 // Must be power of two
#define RS_CLIENT_HANDLE_BUCKET_COUNT   0x400  // 1024
#define RS_CLIENT_HANDLE_BUCKET_MASK    0x3FF


/// The default maximum number of domains a resource server can allocate
#define RS_MAX_DOMAINS_DEFAULT          4096

/// The maximum length of a line of ancestry for resource references
#define RS_MAX_RESOURCE_DEPTH           6

/// RS_LOCK_FLAGS
#define RS_LOCK_FLAGS_NO_TOP_LOCK               NVBIT(0)
#define RS_LOCK_FLAGS_NO_CLIENT_LOCK            NVBIT(1)
#define RS_LOCK_FLAGS_NO_CUSTOM_LOCK_1          NVBIT(2)
#define RS_LOCK_FLAGS_NO_CUSTOM_LOCK_2          NVBIT(3)
#define RS_LOCK_FLAGS_NO_CUSTOM_LOCK_3          NVBIT(4)
#define RS_LOCK_FLAGS_NO_DEPENDANT_SESSION_LOCK NVBIT(5)
#define RS_LOCK_FLAGS_FREE_SESSION_LOCK         NVBIT(6)
#define RS_LOCK_FLAGS_LOW_PRIORITY              NVBIT(7)

/// RS_LOCK_STATE
#define RS_LOCK_STATE_TOP_LOCK_ACQUIRED        NVBIT(0)
#define RS_LOCK_STATE_CUSTOM_LOCK_1_ACQUIRED   NVBIT(1)
#define RS_LOCK_STATE_CUSTOM_LOCK_2_ACQUIRED   NVBIT(2)
#define RS_LOCK_STATE_CUSTOM_LOCK_3_ACQUIRED   NVBIT(3)
#define RS_LOCK_STATE_ALLOW_RECURSIVE_RES_LOCK NVBIT(6)
#define RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED     NVBIT(7)
#define RS_LOCK_STATE_SESSION_LOCK_ACQUIRED    NVBIT(8)

/// RS_LOCK_RELEASE
#define RS_LOCK_RELEASE_TOP_LOCK               NVBIT(0)
#define RS_LOCK_RELEASE_CLIENT_LOCK            NVBIT(1)
#define RS_LOCK_RELEASE_CUSTOM_LOCK_1          NVBIT(2)
#define RS_LOCK_RELEASE_CUSTOM_LOCK_2          NVBIT(3)
#define RS_LOCK_RELEASE_CUSTOM_LOCK_3          NVBIT(4)
#define RS_LOCK_RELEASE_SESSION_LOCK           NVBIT(5)

/// API enumerations used for locking knobs
typedef enum
{
    RS_LOCK_CLIENT      =0,
    RS_LOCK_TOP         =1,
    RS_LOCK_RESOURCE    =2,
    RS_LOCK_CUSTOM_3    =3,
} RS_LOCK_ENUM;

typedef enum
{
    RS_API_ALLOC_CLIENT     = 0,
    RS_API_ALLOC_RESOURCE   = 1,
    RS_API_FREE_RESOURCE    = 2,
    RS_API_MAP              = 3,
    RS_API_UNMAP            = 4,
    RS_API_INTER_MAP        = 5,
    RS_API_INTER_UNMAP      = 6,
    RS_API_COPY             = 7,
    RS_API_SHARE            = 8,
    RS_API_CTRL             = 9,
    RS_API_MAX,
} RS_API_ENUM;

NV_STATUS indexAdd(RsIndex *pIndex, NvU32 index, RsResourceRef *pResourceRef);
NV_STATUS indexRemove(RsIndex *pIndex, NvU32 index, RsResourceRef *pResourceRef);

//
// Externs
//
/**
 * NVOC wrapper for constructing resources of a given type
 *
 * @param[in]    pAllocator    Allocator for the resource object
 * @param[in]    pCallContext  Caller context passed to resource constructor
 * @param[inout] pParams       Resource allocation parameters
 * @param[out]   ppResource    New resource object
 */
extern NV_STATUS resservResourceFactory(PORT_MEM_ALLOCATOR *pAllocator, CALL_CONTEXT *pCallContext,
                                        RS_RES_ALLOC_PARAMS_INTERNAL *pParams, RsResource **ppResource);

/**
 * NVOC wrapper for constructing an application-specific client.
 */
extern NV_STATUS resservClientFactory(PORT_MEM_ALLOCATOR *pAllocator, RS_RES_ALLOC_PARAMS_INTERNAL *pParams, RsClient **ppRsClient);

/**
 *  Validate the UID/PID security token of the current user against a client's security token.
 *
 *  This will be obsolete after phase 1.
 *
 *  @param[in]  pClientToken
 *  @param[in]  pCurrentToken
 *
 *  @returns NV_OK if the current user's security token matches the client's security token
 */
extern NV_STATUS osValidateClientTokens(PSECURITY_TOKEN pClientToken, PSECURITY_TOKEN pCurrentToken);

/**
 *  Get the security token of the current user for the UID/PID security model.
 *
 *  This will be obsolete after phase 1.
 */
extern PSECURITY_TOKEN osGetSecurityToken(void);

/**
 * TLS entry id for call contexts. All servers will use the same id.
 */
#define TLS_ENTRY_ID_RESSERV_CALL_CONTEXT TLS_ENTRY_ID_RESSERV_1

//
// Structs
//
struct RS_FREE_STACK
{
    RS_FREE_STACK *pPrev;
    RsResourceRef *pResourceRef;
};

struct CALL_CONTEXT
{
    RsServer *pServer;              ///< The resource server instance that owns the client
    RsClient *pClient;              ///< Client that was the target of the call
    RsResourceRef *pResourceRef;    ///< Reference that was the target of the call
    RsResourceRef *pContextRef;     ///< Reference that may be used to provide more context [optional]
    RS_LOCK_INFO  *pLockInfo;       ///< Saved locking context information for the call
    API_SECURITY_INFO secInfo;
    RS_RES_CONTROL_PARAMS_INTERNAL  *pControlParams; ///< parameters of the call [optional]

    void *pSerializedParams;        ///< Serialized version of the params
    void *pDeserializedParams;      ///< Deserialized version of the params
    NvU32 serializedSize;           ///< Serialized size
    NvU32 deserializedSize;         ///< Deserialized size
    NvBool bReserialize;            ///< Reserialize before calling into GSP
    NvBool bLocalSerialization;     ///< Serialized internally
};

typedef enum {
    RS_ITERATE_CHILDREN,    ///< Iterate over a RsResourceRef's children
    RS_ITERATE_DESCENDANTS, ///< Iterate over a RsResourceRef's children, grandchildren, etc. (unspecified order)
    RS_ITERATE_CACHED,      ///< Iterate over a RsResourceRef's cache
    RS_ITERATE_DEPENDANTS,  ///< Iterate over a RsResourceRef's dependants
} RS_ITER_TYPE;

typedef enum
{
    LOCK_ACCESS_READ,
    LOCK_ACCESS_WRITE,
} LOCK_ACCESS_TYPE;



/**
 * Access control information. This information will be filled out by the user
 * of the Resource Server when allocating a client or resource.
 */
struct ACCESS_CONTROL
{
    /**
     * The privilege level of this access control
     */
    RS_PRIV_LEVEL privilegeLevel;

    /**
     * Opaque pointer for storing a security token
     */
    PSECURITY_TOKEN pSecurityToken;
};

//
// Utility wrappers for locking validator
//
#if LOCK_VAL_ENABLED
#define RS_LOCK_VALIDATOR_INIT(lock, lockClass, inst) \
    do { NV_ASSERT_OK(lockvalLockInit((lock), (lockClass), (inst))); } while(0)

#define RS_SPINLOCK_ACQUIRE(lock)                            do \
{                                                               \
    NV_ASSERT_OK(lockvalPreAcquire((validator)));               \
    portSyncSpinlockAcquire((lock))  ;                          \
    lockvalPostAcquire((validator), LOCK_VAL_SPINLOCK);         \

#define RS_RWLOCK_ACQUIRE_READ(lock, validator)              do \
{                                                               \
    NV_ASSERT_OK(lockvalPreAcquire((validator)));               \
    portSyncRwLockAcquireRead((lock));                          \
    lockvalPostAcquire((validator), LOCK_VAL_RLOCK);            \
} while(0)

#define RS_RWLOCK_ACQUIRE_WRITE(lock, validator)             do \
{                                                               \
    NV_ASSERT_OK(lockvalPreAcquire((validator)));               \
    portSyncRwLockAcquireWrite((lock));                         \
    lockvalPostAcquire((validator), LOCK_VAL_WLOCK);            \
} while(0)

#define RS_SPINLOCK_RELEASE_EXT(lock, validator, bOutOfOrder)                                                       do \
{                                                                                                                      \
    void *pLockValTlsEntry, *pReleasedLockNode;                                                                        \
    if (bOutOfOrder)                                                                                                   \
        NV_ASSERT_OK(lockvalReleaseOutOfOrder((validator), LOCK_VAL_SPINLOCK, &pLockValTlsEntry, &pReleasedLockNode)); \
    else                                                                                                               \
        NV_ASSERT_OK(lockvalRelease((validator), LOCK_VAL_SPINLOCK, &pLockValTlsEntry, &pReleasedLockNode));           \
    portSyncSpinlockRelease((lock));                                                                                   \
    lockvalMemoryRelease(pLockValTlsEntry, pReleasedLockNode);                                                         \
} while(0)

#define RS_RWLOCK_RELEASE_READ_EXT(lock, validator, bOutOfOrder)                                                 do \
{                                                                                                                   \
    void *pLockValTlsEntry, *pReleasedLockNode;                                                                     \
    if (bOutOfOrder)                                                                                                \
        NV_ASSERT_OK(lockvalReleaseOutOfOrder((validator), LOCK_VAL_RLOCK, &pLockValTlsEntry, &pReleasedLockNode)); \
    else                                                                                                            \
        NV_ASSERT_OK(lockvalRelease((validator), LOCK_VAL_RLOCK, &pLockValTlsEntry, &pReleasedLockNode));           \
    portSyncRwLockReleaseRead((lock));                                                                              \
    lockvalMemoryRelease(pLockValTlsEntry, pReleasedLockNode);                                                      \
} while(0)

#define RS_RWLOCK_RELEASE_WRITE_EXT(lock, validator, bOutOfOrder)                                                do \
{                                                                                                                   \
    void *pLockValTlsEntry, *pReleasedLockNode;                                                                     \
    if (bOutOfOrder)                                                                                                \
        NV_ASSERT_OK(lockvalReleaseOutOfOrder((validator), LOCK_VAL_WLOCK, &pLockValTlsEntry, &pReleasedLockNode)); \
    else                                                                                                            \
        NV_ASSERT_OK(lockvalRelease((validator), LOCK_VAL_WLOCK, &pLockValTlsEntry, &pReleasedLockNode));           \
    portSyncRwLockReleaseWrite((lock));                                                                             \
    lockvalMemoryRelease(pLockValTlsEntry, pReleasedLockNode);                                                      \
} while(0)

#else
#define RS_LOCK_VALIDATOR_INIT(lock, lockClass, inst)
#define RS_SPINLOCK_ACQUIRE(lock, validator)                        do { portSyncSpinlockAcquire((lock)); } while(0)
#define RS_RWLOCK_ACQUIRE_READ(lock, validator)                     do { portSyncRwLockAcquireRead((lock)); } while(0)
#define RS_RWLOCK_ACQUIRE_WRITE(lock, validator)                    do { portSyncRwLockAcquireWrite((lock)); } while(0)
#define RS_SPINLOCK_RELEASE_EXT(lock, validator, bOutOfOrder)       do { portSyncSpinlockRelease((lock)); } while(0)
#define RS_RWLOCK_RELEASE_READ_EXT(lock, validator, bOutOfOrder)    do { portSyncRwLockReleaseRead((lock)); } while(0)
#define RS_RWLOCK_RELEASE_WRITE_EXT(lock, validator, bOutOfOrder)   do { portSyncRwLockReleaseWrite((lock)); } while(0)
#endif

#define RS_SPINLOCK_RELEASE(lock, validator)       RS_SPINLOCK_RELEASE_EXT(lock, validator, NV_FALSE)
#define RS_RWLOCK_RELEASE_READ(lock, validator)    RS_RWLOCK_RELEASE_READ_EXT(lock, validator, NV_FALSE)
#define RS_RWLOCK_RELEASE_WRITE(lock, validator)   RS_RWLOCK_RELEASE_WRITE_EXT(lock, validator, NV_FALSE)


#ifdef __cplusplus
}
#endif

#endif
