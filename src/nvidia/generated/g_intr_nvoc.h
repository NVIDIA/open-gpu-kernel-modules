
#ifndef _G_INTR_NVOC_H_
#define _G_INTR_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2006-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_intr_nvoc.h"

#ifndef INTR_H
#define INTR_H


#include "kernel/gpu/eng_state.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/intr/intr_common.h"
#include "kernel/gpu/intr/intr_service.h"

#include "ctrl/ctrl2080/ctrl2080internal.h"

#include "dev_ctrl_defines.h"
#include "libraries/containers/list.h"
#include "libraries/containers/vector.h"
#include "libraries/nvoc/utility.h"
#include "libraries/utils/nvbitvector.h"


//
// Interrupt Type
//
// Abstraction of the disabled/software/hardware enumeration in NV_PMC_INTR_EN_0_INTA
// !!! This enumeration must exactly match NV_PMC_INTR_EN_0_INTA !!!
//

#define INTERRUPT_TYPE_DISABLED         0
#define INTERRUPT_TYPE_HARDWARE         1
#define INTERRUPT_TYPE_SOFTWARE         2
#define INTERRUPT_TYPE_MULTI            3
#define INTERRUPT_TYPE_MAX              INTERRUPT_TYPE_MULTI

#define INTERRUPT_MASK_DISABLED          0x00000000
#define INTERRUPT_MASK_HARDWARE          0x7fffffff
#define INTERRUPT_MASK_SOFTWARE          0x80000000
#define INTERRUPT_MASK_ENABLED           0xffffffff

/**
 * @brief Each entry corresponds to a top level interrupt
 *
 * This structure will eventually be replaced by #InterruptEntry.
 */
typedef struct
{
    /** MC_ENGINE_IDX* value */
    NvU16 mcEngine;
    /** Bit in top level PMC interrupt registers */
    NvU32 pmcIntrMask;
    /** Interrupt vector in CTRL interrupt tree (Turing+). For non-host driven
     *  engines, this is their single interrupt vector at top level; while for
     *  host driven engines, this is their stalling interrupt vector
     */
    NvU32 intrVector;
    /** Nonstalling interrupt vector in CTRL interrupt tree (Turing+). Only
     *  valid for host driven engines. NV_INTR_VECTOR_INVALID signifies
     *  unavailable
     */
    NvU32 intrVectorNonStall;
} INTR_TABLE_ENTRY;

#define INTR_TABLE_MAX_INTRS_PER_ENTRY       6

MAKE_VECTOR(InterruptTable, INTR_TABLE_ENTRY);

/*!
 * Mapping from leaf level interrupt to conceptual interrupt name.
 *
 * - The interrupt vector is implicit from the tree / index of an array which
 *   contains this struct.
 * - The target is a conceptual name that represents the interrupt identified by
 *   (MC_ENGINE_IDX*, INTR_KIND*) pair.
 * - A service routine may or may not be actually registered to handle the
 *   interrupt.
 * - Multiple physical vectors can map to the same conceptual interrupt.
 */
typedef struct
{
    /*!
     * MC_ENGINE_IDX* value.
     *
     * A value of #MC_ENGINE_IDX_NULL means that the vector corresponding to
     * this entry is unused. Use #interruptEntryIsEmpty to check this.
     */
    NvU16 mcEngine;

    /*!
     * INTR_KIND_* value.
     *
     * This allows multiple logically separate interrupts to map to a service
     * routine via a common mcEngine value.
     */
    INTR_KIND intrKind;

    /*!
     * If the interrupt should be handled.
     *
     * If this is false:
     * - The interrupt may need to be visible for clients, VF, etc (error
     *   containment).
     * - It can be an interrupt to be triggered to notify RM running in a
     *   different environment: doorbell, GSP triggered notifications to CPU.
     * - The interrupt does not need to be serviced. There should be no
     *   corresponding entry in the #intrServiceTable.
     */
    NvBool bService;
} InterruptEntry;

static NV_FORCEINLINE NvBool
interruptEntryIsEmpty(const InterruptEntry *pEntry)
{
    return pEntry->mcEngine == MC_ENGINE_IDX_NULL;
}


//
// Default value for intrStuckThreshold
#define INTR_STUCK_THRESHOLD 1000

#define INTR_TABLE_INIT_KERNEL (1 << 0)
#define INTR_TABLE_INIT_PHYSICAL (1 << 1)

/**
 * @brief This enum specifies the type of DPC node
 *      INTERRUPT_BASED_DPC: DPC queued for an interrupt source
 *      SPECIAL_DPC        : DPC queued within processing of another interrupt
 *                           source
 *
 * Currently only used on Fermi+.
 */
typedef enum
{
    INTERRUPT_BASED_DPC=0,
    SPECIAL_DPC
} DPCTYPE;

/**
 * @brief This is a structure for a node on the DPC Queue
 *          dpctype: Type of DPC for processing
 *          dpcdata: Data required for dpc processing
 *                   This union will contain dpctype specific data
 *          pNext  : Pointer to the next DPC node
 *
 * Currently only used on Fermi+.
 */
typedef struct _DPCNODE
{
    DPCTYPE dpctype;
    union _dpcdata
    {
        MC_ENGINE_BITVECTOR pendingEngines;
    } dpcdata;

    struct _DPCNODE *pNext;
} DPCNODE;

/**
 * @brief This is a structure for the DPC Queue
 *          numEntries: Number of entries currently on DPC queue (debugging purpose)
 *          pFront    : Front pointer for the queue
 *          pRear     : Rear pointer for the queue
 *
 * Currently only used on Fermi+.
 */
typedef struct
{
    NvU32    numEntries;
    DPCNODE *pFront;
    DPCNODE *pRear;
} DPCQUEUE;

// Data related to PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING
typedef struct
{
    NvU32 flags;
    NvU32 cached;                   // Pascal+, to implement intr mask in SW.
    MC_ENGINE_BITVECTOR engMaskUnblocked;
} INTR_MASK;

//
// interrupt mask information used for lazy interrupt disable and interrupt
// masking for locking.
//
typedef struct
{
    NvU32 intrEnable;
    MC_ENGINE_BITVECTOR intrMask;
} INTR_MASK_CTX;


//!
//! List of interrupt trees that RM sees.
//!
//! Kernel RM should determine number of implemented vectors using the actual
//! interrupt table fetched.
//!
typedef enum
{
    INTR_TREE_CPU,
    INTR_TREE_COUNT
} INTR_TREE;


//
// IntrMask Locking Flag Defines
//
#define INTR_MASK_FLAGS_ISR_SKIP_MASK_UPDATE     NVBIT(0)


struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_INTR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct Intr {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct Intr *__nvoc_pbase_Intr;    // intr

    // Vtable with 32 per-object function pointers
    NV_STATUS (*__intrConstructEngine__)(OBJGPU *, struct Intr * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__intrStatePreInitLocked__)(OBJGPU *, struct Intr * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__intrStateInitUnlocked__)(OBJGPU *, struct Intr * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__intrStateInitLocked__)(OBJGPU *, struct Intr * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__intrStateDestroy__)(OBJGPU *, struct Intr * /*this*/);  // virtual override (engstate) base (engstate)
    NvU32 (*__intrDecodeStallIntrEn__)(OBJGPU *, struct Intr * /*this*/, NvU32);  // halified (2 hals) body
    void (*__intrServiceVirtual__)(OBJGPU *, struct Intr * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__intrTriggerPrivDoorbell__)(OBJGPU *, struct Intr * /*this*/, NvU32);  // halified (2 hals) body
    void (*__intrGetLocklessVectorsInRmSubtree__)(OBJGPU *, struct Intr * /*this*/, NvU32 (*)[2]);  // halified (2 hals) body
    void (*__intrSetDisplayInterruptEnable__)(OBJGPU *, struct Intr * /*this*/, NvBool, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    NvU32 (*__intrReadRegTopEnSet__)(OBJGPU *, struct Intr * /*this*/, NvU32, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__intrWriteRegTopEnSet__)(OBJGPU *, struct Intr * /*this*/, NvU32, NvU32, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__intrWriteRegTopEnClear__)(OBJGPU *, struct Intr * /*this*/, NvU32, NvU32, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    NvU32 (*__intrGetNumLeaves__)(OBJGPU *, struct Intr * /*this*/);  // halified (2 hals) body
    NvU32 (*__intrGetLeafSize__)(OBJGPU *, struct Intr * /*this*/);  // halified (2 hals) body
    NvU64 (*__intrGetIntrTopNonStallMask__)(OBJGPU *, struct Intr * /*this*/);  // halified (2 hals) body
    void (*__intrSanityCheckEngineIntrStallVector__)(OBJGPU *, struct Intr * /*this*/, NvU32, NvU16);  // halified (3 hals) body
    void (*__intrSanityCheckEngineIntrNotificationVector__)(OBJGPU *, struct Intr * /*this*/, NvU32, NvU16);  // halified (3 hals) body
    NV_STATUS (*__intrStateLoad__)(OBJGPU *, struct Intr * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__intrStateUnload__)(OBJGPU *, struct Intr * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__intrInitSubtreeMap__)(OBJGPU *, struct Intr * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__intrInitInterruptTable__)(OBJGPU *, struct Intr * /*this*/);  // halified (2 hals)
    NV_STATUS (*__intrSetIntrMask__)(OBJGPU *, struct Intr * /*this*/, union MC_ENGINE_BITVECTOR *, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__intrSetIntrEnInHw__)(OBJGPU *, struct Intr * /*this*/, NvU32, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    NvU32 (*__intrGetIntrEnFromHw__)(OBJGPU *, struct Intr * /*this*/, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__intrInitMissing__)(struct OBJGPU *, struct Intr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__intrStatePreInitUnlocked__)(struct OBJGPU *, struct Intr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__intrStatePreLoad__)(struct OBJGPU *, struct Intr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__intrStatePostLoad__)(struct OBJGPU *, struct Intr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__intrStatePreUnload__)(struct OBJGPU *, struct Intr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__intrStatePostUnload__)(struct OBJGPU *, struct Intr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__intrIsPresent__)(struct OBJGPU *, struct Intr * /*this*/);  // virtual inherited (engstate) base (engstate)

    // 8 PDB properties
    NvBool PDB_PROP_INTR_ENABLE_DETAILED_LOGS;
    NvBool PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC;
    NvBool PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS;
    NvBool PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET;
    NvBool PDB_PROP_INTR_SIMPLIFIED_VBLANK_HANDLING_FOR_CTRL_TREE;
    NvBool PDB_PROP_INTR_DISABLE_PER_INTR_DPC_QUEUEING;
    NvBool PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING;
    NvBool PDB_PROP_INTR_MASK_SUPPORTED;

    // Data members
    NvU32 nonStallPmcIntrMask;
    NvU64 uvmSharedCpuLeafEn;
    NvU64 uvmSharedCpuLeafEnDisableMask;
    NvU32 replayableFaultIntrVector;
    NvU32 accessCntrIntrVector;
    NvU32 displayIntrVector;
    NvU64 intrTopEnMask;
    InterruptTable intrTable;
    IntrServiceRecord intrServiceTable[175];
    InterruptEntry *(vectorToMcIdx[1]);
    NvLength vectorToMcIdxCounts[1];
    NvBool bDefaultNonstallNotify;
    NvBool bUseLegacyVectorAssignment;
    NV2080_INTR_CATEGORY_SUBTREE_MAP subtreeMap[7];
    NvBool bDpcStarted;
    union MC_ENGINE_BITVECTOR pmcIntrPending;
    DPCQUEUE dpcQueue;
    NvU32 intrStuckThreshold;
    INTR_MASK intrMask;
    union MC_ENGINE_BITVECTOR helperEngineMask;
    NvU32 intrEn0;
    NvU32 intrCachedEn0;
    NvU32 intrCachedEnSet;
    NvU32 intrCachedEnClear;
    NvU32 intrEn0Orig;
    NvBool halIntrEnabled;
    NvU32 saveIntrEn0;
};

#ifndef __NVOC_CLASS_Intr_TYPEDEF__
#define __NVOC_CLASS_Intr_TYPEDEF__
typedef struct Intr Intr;
#endif /* __NVOC_CLASS_Intr_TYPEDEF__ */

#ifndef __nvoc_class_id_Intr
#define __nvoc_class_id_Intr 0xc06e44
#endif /* __nvoc_class_id_Intr */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Intr;

#define __staticCast_Intr(pThis) \
    ((pThis)->__nvoc_pbase_Intr)

#ifdef __nvoc_intr_h_disabled
#define __dynamicCast_Intr(pThis) ((Intr*)NULL)
#else //__nvoc_intr_h_disabled
#define __dynamicCast_Intr(pThis) \
    ((Intr*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Intr)))
#endif //__nvoc_intr_h_disabled

// Property macros
#define PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC_BASE_CAST
#define PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC_BASE_NAME PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC
#define PDB_PROP_INTR_SIMPLIFIED_VBLANK_HANDLING_FOR_CTRL_TREE_BASE_CAST
#define PDB_PROP_INTR_SIMPLIFIED_VBLANK_HANDLING_FOR_CTRL_TREE_BASE_NAME PDB_PROP_INTR_SIMPLIFIED_VBLANK_HANDLING_FOR_CTRL_TREE
#define PDB_PROP_INTR_MASK_SUPPORTED_BASE_CAST
#define PDB_PROP_INTR_MASK_SUPPORTED_BASE_NAME PDB_PROP_INTR_MASK_SUPPORTED
#define PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING_BASE_CAST
#define PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING_BASE_NAME PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING
#define PDB_PROP_INTR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_INTR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_INTR_ENABLE_DETAILED_LOGS_BASE_CAST
#define PDB_PROP_INTR_ENABLE_DETAILED_LOGS_BASE_NAME PDB_PROP_INTR_ENABLE_DETAILED_LOGS
#define PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET_BASE_CAST
#define PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET_BASE_NAME PDB_PROP_INTR_ENUMERATIONS_ON_ENGINE_RESET
#define PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS_BASE_CAST
#define PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS_BASE_NAME PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS
#define PDB_PROP_INTR_DISABLE_PER_INTR_DPC_QUEUEING_BASE_CAST
#define PDB_PROP_INTR_DISABLE_PER_INTR_DPC_QUEUEING_BASE_NAME PDB_PROP_INTR_DISABLE_PER_INTR_DPC_QUEUEING

NV_STATUS __nvoc_objCreateDynamic_Intr(Intr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Intr(Intr**, Dynamic*, NvU32);
#define __objCreate_Intr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_Intr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define intrConstructEngine_FNPTR(pIntr) pIntr->__intrConstructEngine__
#define intrConstructEngine(pGpu, pIntr, arg3) intrConstructEngine_DISPATCH(pGpu, pIntr, arg3)
#define intrStatePreInitLocked_FNPTR(pIntr) pIntr->__intrStatePreInitLocked__
#define intrStatePreInitLocked(pGpu, pIntr) intrStatePreInitLocked_DISPATCH(pGpu, pIntr)
#define intrStateInitUnlocked_FNPTR(pIntr) pIntr->__intrStateInitUnlocked__
#define intrStateInitUnlocked(pGpu, pIntr) intrStateInitUnlocked_DISPATCH(pGpu, pIntr)
#define intrStateInitLocked_FNPTR(pIntr) pIntr->__intrStateInitLocked__
#define intrStateInitLocked(pGpu, pIntr) intrStateInitLocked_DISPATCH(pGpu, pIntr)
#define intrStateDestroy_FNPTR(pIntr) pIntr->__intrStateDestroy__
#define intrStateDestroy(pGpu, pIntr) intrStateDestroy_DISPATCH(pGpu, pIntr)
#define intrDecodeStallIntrEn_FNPTR(pIntr) pIntr->__intrDecodeStallIntrEn__
#define intrDecodeStallIntrEn(pGpu, pIntr, arg3) intrDecodeStallIntrEn_DISPATCH(pGpu, pIntr, arg3)
#define intrDecodeStallIntrEn_HAL(pGpu, pIntr, arg3) intrDecodeStallIntrEn_DISPATCH(pGpu, pIntr, arg3)
#define intrServiceVirtual_FNPTR(pIntr) pIntr->__intrServiceVirtual__
#define intrServiceVirtual(pGpu, pIntr) intrServiceVirtual_DISPATCH(pGpu, pIntr)
#define intrServiceVirtual_HAL(pGpu, pIntr) intrServiceVirtual_DISPATCH(pGpu, pIntr)
#define intrTriggerPrivDoorbell_FNPTR(pIntr) pIntr->__intrTriggerPrivDoorbell__
#define intrTriggerPrivDoorbell(pGpu, pIntr, gfid) intrTriggerPrivDoorbell_DISPATCH(pGpu, pIntr, gfid)
#define intrTriggerPrivDoorbell_HAL(pGpu, pIntr, gfid) intrTriggerPrivDoorbell_DISPATCH(pGpu, pIntr, gfid)
#define intrGetLocklessVectorsInRmSubtree_FNPTR(pIntr) pIntr->__intrGetLocklessVectorsInRmSubtree__
#define intrGetLocklessVectorsInRmSubtree(pGpu, pIntr, pInterruptVectors) intrGetLocklessVectorsInRmSubtree_DISPATCH(pGpu, pIntr, pInterruptVectors)
#define intrGetLocklessVectorsInRmSubtree_HAL(pGpu, pIntr, pInterruptVectors) intrGetLocklessVectorsInRmSubtree_DISPATCH(pGpu, pIntr, pInterruptVectors)
#define intrSetDisplayInterruptEnable_FNPTR(pIntr) pIntr->__intrSetDisplayInterruptEnable__
#define intrSetDisplayInterruptEnable(pGpu, pIntr, bEnable, pThreadState) intrSetDisplayInterruptEnable_DISPATCH(pGpu, pIntr, bEnable, pThreadState)
#define intrSetDisplayInterruptEnable_HAL(pGpu, pIntr, bEnable, pThreadState) intrSetDisplayInterruptEnable_DISPATCH(pGpu, pIntr, bEnable, pThreadState)
#define intrReadRegTopEnSet_FNPTR(pIntr) pIntr->__intrReadRegTopEnSet__
#define intrReadRegTopEnSet(pGpu, pIntr, arg3, arg4) intrReadRegTopEnSet_DISPATCH(pGpu, pIntr, arg3, arg4)
#define intrReadRegTopEnSet_HAL(pGpu, pIntr, arg3, arg4) intrReadRegTopEnSet_DISPATCH(pGpu, pIntr, arg3, arg4)
#define intrWriteRegTopEnSet_FNPTR(pIntr) pIntr->__intrWriteRegTopEnSet__
#define intrWriteRegTopEnSet(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegTopEnSet_DISPATCH(pGpu, pIntr, arg3, arg4, arg5)
#define intrWriteRegTopEnSet_HAL(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegTopEnSet_DISPATCH(pGpu, pIntr, arg3, arg4, arg5)
#define intrWriteRegTopEnClear_FNPTR(pIntr) pIntr->__intrWriteRegTopEnClear__
#define intrWriteRegTopEnClear(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegTopEnClear_DISPATCH(pGpu, pIntr, arg3, arg4, arg5)
#define intrWriteRegTopEnClear_HAL(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegTopEnClear_DISPATCH(pGpu, pIntr, arg3, arg4, arg5)
#define intrGetNumLeaves_FNPTR(pIntr) pIntr->__intrGetNumLeaves__
#define intrGetNumLeaves(pGpu, pIntr) intrGetNumLeaves_DISPATCH(pGpu, pIntr)
#define intrGetNumLeaves_HAL(pGpu, pIntr) intrGetNumLeaves_DISPATCH(pGpu, pIntr)
#define intrGetLeafSize_FNPTR(pIntr) pIntr->__intrGetLeafSize__
#define intrGetLeafSize(pGpu, pIntr) intrGetLeafSize_DISPATCH(pGpu, pIntr)
#define intrGetLeafSize_HAL(pGpu, pIntr) intrGetLeafSize_DISPATCH(pGpu, pIntr)
#define intrGetIntrTopNonStallMask_FNPTR(pIntr) pIntr->__intrGetIntrTopNonStallMask__
#define intrGetIntrTopNonStallMask(pGpu, pIntr) intrGetIntrTopNonStallMask_DISPATCH(pGpu, pIntr)
#define intrGetIntrTopNonStallMask_HAL(pGpu, pIntr) intrGetIntrTopNonStallMask_DISPATCH(pGpu, pIntr)
#define intrSanityCheckEngineIntrStallVector_FNPTR(pIntr) pIntr->__intrSanityCheckEngineIntrStallVector__
#define intrSanityCheckEngineIntrStallVector(pGpu, pIntr, vector, mcEngine) intrSanityCheckEngineIntrStallVector_DISPATCH(pGpu, pIntr, vector, mcEngine)
#define intrSanityCheckEngineIntrStallVector_HAL(pGpu, pIntr, vector, mcEngine) intrSanityCheckEngineIntrStallVector_DISPATCH(pGpu, pIntr, vector, mcEngine)
#define intrSanityCheckEngineIntrNotificationVector_FNPTR(pIntr) pIntr->__intrSanityCheckEngineIntrNotificationVector__
#define intrSanityCheckEngineIntrNotificationVector(pGpu, pIntr, vector, mcEngine) intrSanityCheckEngineIntrNotificationVector_DISPATCH(pGpu, pIntr, vector, mcEngine)
#define intrSanityCheckEngineIntrNotificationVector_HAL(pGpu, pIntr, vector, mcEngine) intrSanityCheckEngineIntrNotificationVector_DISPATCH(pGpu, pIntr, vector, mcEngine)
#define intrStateLoad_FNPTR(pIntr) pIntr->__intrStateLoad__
#define intrStateLoad(pGpu, pIntr, arg3) intrStateLoad_DISPATCH(pGpu, pIntr, arg3)
#define intrStateLoad_HAL(pGpu, pIntr, arg3) intrStateLoad_DISPATCH(pGpu, pIntr, arg3)
#define intrStateUnload_FNPTR(pIntr) pIntr->__intrStateUnload__
#define intrStateUnload(pGpu, pIntr, arg3) intrStateUnload_DISPATCH(pGpu, pIntr, arg3)
#define intrStateUnload_HAL(pGpu, pIntr, arg3) intrStateUnload_DISPATCH(pGpu, pIntr, arg3)
#define intrInitSubtreeMap_FNPTR(pIntr) pIntr->__intrInitSubtreeMap__
#define intrInitSubtreeMap(pGpu, pIntr) intrInitSubtreeMap_DISPATCH(pGpu, pIntr)
#define intrInitSubtreeMap_HAL(pGpu, pIntr) intrInitSubtreeMap_DISPATCH(pGpu, pIntr)
#define intrInitInterruptTable_FNPTR(pIntr) pIntr->__intrInitInterruptTable__
#define intrInitInterruptTable(pGpu, pIntr) intrInitInterruptTable_DISPATCH(pGpu, pIntr)
#define intrInitInterruptTable_HAL(pGpu, pIntr) intrInitInterruptTable_DISPATCH(pGpu, pIntr)
#define intrSetIntrMask_FNPTR(pIntr) pIntr->__intrSetIntrMask__
#define intrSetIntrMask(pGpu, pIntr, arg3, arg4) intrSetIntrMask_DISPATCH(pGpu, pIntr, arg3, arg4)
#define intrSetIntrMask_HAL(pGpu, pIntr, arg3, arg4) intrSetIntrMask_DISPATCH(pGpu, pIntr, arg3, arg4)
#define intrSetIntrEnInHw_FNPTR(pIntr) pIntr->__intrSetIntrEnInHw__
#define intrSetIntrEnInHw(pGpu, pIntr, arg3, arg4) intrSetIntrEnInHw_DISPATCH(pGpu, pIntr, arg3, arg4)
#define intrSetIntrEnInHw_HAL(pGpu, pIntr, arg3, arg4) intrSetIntrEnInHw_DISPATCH(pGpu, pIntr, arg3, arg4)
#define intrGetIntrEnFromHw_FNPTR(pIntr) pIntr->__intrGetIntrEnFromHw__
#define intrGetIntrEnFromHw(pGpu, pIntr, arg3) intrGetIntrEnFromHw_DISPATCH(pGpu, pIntr, arg3)
#define intrGetIntrEnFromHw_HAL(pGpu, pIntr, arg3) intrGetIntrEnFromHw_DISPATCH(pGpu, pIntr, arg3)
#define intrInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define intrInitMissing(pGpu, pEngstate) intrInitMissing_DISPATCH(pGpu, pEngstate)
#define intrStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define intrStatePreInitUnlocked(pGpu, pEngstate) intrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define intrStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define intrStatePreLoad(pGpu, pEngstate, arg3) intrStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define intrStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define intrStatePostLoad(pGpu, pEngstate, arg3) intrStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define intrStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define intrStatePreUnload(pGpu, pEngstate, arg3) intrStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define intrStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define intrStatePostUnload(pGpu, pEngstate, arg3) intrStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define intrIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define intrIsPresent(pGpu, pEngstate) intrIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS intrConstructEngine_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, ENGDESCRIPTOR arg3) {
    return pIntr->__intrConstructEngine__(pGpu, pIntr, arg3);
}

static inline NV_STATUS intrStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrStatePreInitLocked__(pGpu, pIntr);
}

static inline NV_STATUS intrStateInitUnlocked_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrStateInitUnlocked__(pGpu, pIntr);
}

static inline NV_STATUS intrStateInitLocked_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrStateInitLocked__(pGpu, pIntr);
}

static inline void intrStateDestroy_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    pIntr->__intrStateDestroy__(pGpu, pIntr);
}

static inline NvU32 intrDecodeStallIntrEn_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3) {
    return pIntr->__intrDecodeStallIntrEn__(pGpu, pIntr, arg3);
}

static inline void intrServiceVirtual_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    pIntr->__intrServiceVirtual__(pGpu, pIntr);
}

static inline NV_STATUS intrTriggerPrivDoorbell_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid) {
    return pIntr->__intrTriggerPrivDoorbell__(pGpu, pIntr, gfid);
}

static inline void intrGetLocklessVectorsInRmSubtree_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 (*pInterruptVectors)[2]) {
    pIntr->__intrGetLocklessVectorsInRmSubtree__(pGpu, pIntr, pInterruptVectors);
}

static inline void intrSetDisplayInterruptEnable_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvBool bEnable, struct THREAD_STATE_NODE *pThreadState) {
    pIntr->__intrSetDisplayInterruptEnable__(pGpu, pIntr, bEnable, pThreadState);
}

static inline NvU32 intrReadRegTopEnSet_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4) {
    return pIntr->__intrReadRegTopEnSet__(pGpu, pIntr, arg3, arg4);
}

static inline void intrWriteRegTopEnSet_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5) {
    pIntr->__intrWriteRegTopEnSet__(pGpu, pIntr, arg3, arg4, arg5);
}

static inline void intrWriteRegTopEnClear_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5) {
    pIntr->__intrWriteRegTopEnClear__(pGpu, pIntr, arg3, arg4, arg5);
}

static inline NvU32 intrGetNumLeaves_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrGetNumLeaves__(pGpu, pIntr);
}

static inline NvU32 intrGetLeafSize_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrGetLeafSize__(pGpu, pIntr);
}

static inline NvU64 intrGetIntrTopNonStallMask_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrGetIntrTopNonStallMask__(pGpu, pIntr);
}

static inline void intrSanityCheckEngineIntrStallVector_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine) {
    pIntr->__intrSanityCheckEngineIntrStallVector__(pGpu, pIntr, vector, mcEngine);
}

static inline void intrSanityCheckEngineIntrNotificationVector_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine) {
    pIntr->__intrSanityCheckEngineIntrNotificationVector__(pGpu, pIntr, vector, mcEngine);
}

static inline NV_STATUS intrStateLoad_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3) {
    return pIntr->__intrStateLoad__(pGpu, pIntr, arg3);
}

static inline NV_STATUS intrStateUnload_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3) {
    return pIntr->__intrStateUnload__(pGpu, pIntr, arg3);
}

static inline NV_STATUS intrInitSubtreeMap_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrInitSubtreeMap__(pGpu, pIntr);
}

static inline NV_STATUS intrInitInterruptTable_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr) {
    return pIntr->__intrInitInterruptTable__(pGpu, pIntr);
}

static inline NV_STATUS intrSetIntrMask_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    return pIntr->__intrSetIntrMask__(pGpu, pIntr, arg3, arg4);
}

static inline void intrSetIntrEnInHw_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4) {
    pIntr->__intrSetIntrEnInHw__(pGpu, pIntr, arg3, arg4);
}

static inline NvU32 intrGetIntrEnFromHw_DISPATCH(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3) {
    return pIntr->__intrGetIntrEnFromHw__(pGpu, pIntr, arg3);
}

static inline void intrInitMissing_DISPATCH(struct OBJGPU *pGpu, struct Intr *pEngstate) {
    pEngstate->__intrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS intrStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct Intr *pEngstate) {
    return pEngstate->__intrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS intrStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return pEngstate->__intrStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS intrStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return pEngstate->__intrStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS intrStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return pEngstate->__intrStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS intrStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct Intr *pEngstate, NvU32 arg3) {
    return pEngstate->__intrStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool intrIsPresent_DISPATCH(struct OBJGPU *pGpu, struct Intr *pEngstate) {
    return pEngstate->__intrIsPresent__(pGpu, pEngstate);
}

NV_STATUS intrCheckFecsEventbufferPending_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrCheckFecsEventbufferPending(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrCheckFecsEventbufferPending(pGpu, pIntr, arg3, arg4) intrCheckFecsEventbufferPending_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrCheckFecsEventbufferPending_HAL(pGpu, pIntr, arg3, arg4) intrCheckFecsEventbufferPending(pGpu, pIntr, arg3, arg4)

NV_STATUS intrCheckAndServiceFecsEventbuffer_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrCheckAndServiceFecsEventbuffer(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrCheckAndServiceFecsEventbuffer(pGpu, pIntr, arg3, arg4) intrCheckAndServiceFecsEventbuffer_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrCheckAndServiceFecsEventbuffer_HAL(pGpu, pIntr, arg3, arg4) intrCheckAndServiceFecsEventbuffer(pGpu, pIntr, arg3, arg4)

static inline NV_STATUS intrStateDestroyPhysical_56cd7a(OBJGPU *pGpu, struct Intr *pIntr) {
    return NV_OK;
}

NV_STATUS intrStateDestroyPhysical_IMPL(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrStateDestroyPhysical(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrStateDestroyPhysical(pGpu, pIntr) intrStateDestroyPhysical_56cd7a(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrStateDestroyPhysical_HAL(pGpu, pIntr) intrStateDestroyPhysical(pGpu, pIntr)

static inline void intrSetInterruptMaskBug1470153War_b3696a(OBJGPU *pGpu, struct Intr *pIntr) {
    return;
}


#ifdef __nvoc_intr_h_disabled
static inline void intrSetInterruptMaskBug1470153War(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSetInterruptMaskBug1470153War(pGpu, pIntr) intrSetInterruptMaskBug1470153War_b3696a(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrSetInterruptMaskBug1470153War_HAL(pGpu, pIntr) intrSetInterruptMaskBug1470153War(pGpu, pIntr)

NV_STATUS intrGetPendingNonStall_TU102(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetPendingNonStall(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetPendingNonStall(pGpu, pIntr, arg3, arg4) intrGetPendingNonStall_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetPendingNonStall_HAL(pGpu, pIntr, arg3, arg4) intrGetPendingNonStall(pGpu, pIntr, arg3, arg4)

NV_STATUS intrServiceNonStall_TU102(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrServiceNonStall(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrServiceNonStall(pGpu, pIntr, arg3, arg4) intrServiceNonStall_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrServiceNonStall_HAL(pGpu, pIntr, arg3, arg4) intrServiceNonStall(pGpu, pIntr, arg3, arg4)

NvU32 intrGetNonStallEnable_TU102(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrGetNonStallEnable(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetNonStallEnable(pGpu, pIntr, arg3) intrGetNonStallEnable_TU102(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

#define intrGetNonStallEnable_HAL(pGpu, pIntr, arg3) intrGetNonStallEnable(pGpu, pIntr, arg3)

void intrDisableNonStall_TU102(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_intr_h_disabled
static inline void intrDisableNonStall(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrDisableNonStall(pGpu, pIntr, arg3) intrDisableNonStall_TU102(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

#define intrDisableNonStall_HAL(pGpu, pIntr, arg3) intrDisableNonStall(pGpu, pIntr, arg3)

void intrRestoreNonStall_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline void intrRestoreNonStall(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrRestoreNonStall(pGpu, pIntr, arg3, arg4) intrRestoreNonStall_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrRestoreNonStall_HAL(pGpu, pIntr, arg3, arg4) intrRestoreNonStall(pGpu, pIntr, arg3, arg4)

void intrGetStallInterruptMode_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *pIntrmode, NvBool *pPending);


#ifdef __nvoc_intr_h_disabled
static inline void intrGetStallInterruptMode(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *pIntrmode, NvBool *pPending) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrGetStallInterruptMode(pGpu, pIntr, pIntrmode, pPending) intrGetStallInterruptMode_TU102(pGpu, pIntr, pIntrmode, pPending)
#endif //__nvoc_intr_h_disabled

#define intrGetStallInterruptMode_HAL(pGpu, pIntr, pIntrmode, pPending) intrGetStallInterruptMode(pGpu, pIntr, pIntrmode, pPending)

void intrEncodeStallIntrEn_GP100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrEn, NvU32 *pIntrEnSet, NvU32 *pIntrEnClear);


#ifdef __nvoc_intr_h_disabled
static inline void intrEncodeStallIntrEn(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrEn, NvU32 *pIntrEnSet, NvU32 *pIntrEnClear) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrEncodeStallIntrEn(pGpu, pIntr, intrEn, pIntrEnSet, pIntrEnClear) intrEncodeStallIntrEn_GP100(pGpu, pIntr, intrEn, pIntrEnSet, pIntrEnClear)
#endif //__nvoc_intr_h_disabled

#define intrEncodeStallIntrEn_HAL(pGpu, pIntr, intrEn, pIntrEnSet, pIntrEnClear) intrEncodeStallIntrEn(pGpu, pIntr, intrEn, pIntrEnSet, pIntrEnClear)

NV_STATUS intrCheckAndServiceNonReplayableFault_TU102(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrCheckAndServiceNonReplayableFault(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrCheckAndServiceNonReplayableFault(pGpu, pIntr, arg3) intrCheckAndServiceNonReplayableFault_TU102(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

#define intrCheckAndServiceNonReplayableFault_HAL(pGpu, pIntr, arg3) intrCheckAndServiceNonReplayableFault(pGpu, pIntr, arg3)

static inline NvU32 intrGetStallBaseVector_4a4dee(OBJGPU *pGpu, struct Intr *pIntr) {
    return 0;
}

NvU32 intrGetStallBaseVector_TU102(OBJGPU *pGpu, struct Intr *pIntr);

static inline NvU32 intrGetStallBaseVector_c067f9(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}


#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrGetStallBaseVector(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetStallBaseVector(pGpu, pIntr) intrGetStallBaseVector_4a4dee(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrGetStallBaseVector_HAL(pGpu, pIntr) intrGetStallBaseVector(pGpu, pIntr)

void intrEnableLeaf_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrVector);


#ifdef __nvoc_intr_h_disabled
static inline void intrEnableLeaf(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrVector) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrEnableLeaf(pGpu, pIntr, intrVector) intrEnableLeaf_TU102(pGpu, pIntr, intrVector)
#endif //__nvoc_intr_h_disabled

#define intrEnableLeaf_HAL(pGpu, pIntr, intrVector) intrEnableLeaf(pGpu, pIntr, intrVector)

void intrDisableLeaf_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrVector);


#ifdef __nvoc_intr_h_disabled
static inline void intrDisableLeaf(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrVector) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrDisableLeaf(pGpu, pIntr, intrVector) intrDisableLeaf_TU102(pGpu, pIntr, intrVector)
#endif //__nvoc_intr_h_disabled

#define intrDisableLeaf_HAL(pGpu, pIntr, intrVector) intrDisableLeaf(pGpu, pIntr, intrVector)

void intrEnableTopNonstall_TU102(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *pThreadState);


#ifdef __nvoc_intr_h_disabled
static inline void intrEnableTopNonstall(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrEnableTopNonstall(pGpu, pIntr, pThreadState) intrEnableTopNonstall_TU102(pGpu, pIntr, pThreadState)
#endif //__nvoc_intr_h_disabled

#define intrEnableTopNonstall_HAL(pGpu, pIntr, pThreadState) intrEnableTopNonstall(pGpu, pIntr, pThreadState)

void intrDisableTopNonstall_TU102(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *pThreadState);


#ifdef __nvoc_intr_h_disabled
static inline void intrDisableTopNonstall(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrDisableTopNonstall(pGpu, pIntr, pThreadState) intrDisableTopNonstall_TU102(pGpu, pIntr, pThreadState)
#endif //__nvoc_intr_h_disabled

#define intrDisableTopNonstall_HAL(pGpu, pIntr, pThreadState) intrDisableTopNonstall(pGpu, pIntr, pThreadState)

void intrSetStall_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrType, struct THREAD_STATE_NODE *pThreadState);


#ifdef __nvoc_intr_h_disabled
static inline void intrSetStall(OBJGPU *pGpu, struct Intr *pIntr, NvU32 intrType, struct THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSetStall(pGpu, pIntr, intrType, pThreadState) intrSetStall_TU102(pGpu, pIntr, intrType, pThreadState)
#endif //__nvoc_intr_h_disabled

#define intrSetStall_HAL(pGpu, pIntr, intrType, pThreadState) intrSetStall(pGpu, pIntr, intrType, pThreadState)

void intrClearLeafVector_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, struct THREAD_STATE_NODE *pThreadState);


#ifdef __nvoc_intr_h_disabled
static inline void intrClearLeafVector(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, struct THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrClearLeafVector(pGpu, pIntr, vector, pThreadState) intrClearLeafVector_TU102(pGpu, pIntr, vector, pThreadState)
#endif //__nvoc_intr_h_disabled

#define intrClearLeafVector_HAL(pGpu, pIntr, vector, pThreadState) intrClearLeafVector(pGpu, pIntr, vector, pThreadState)

NvBool intrIsVectorPending_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, struct THREAD_STATE_NODE *pThreadState);


#ifdef __nvoc_intr_h_disabled
static inline NvBool intrIsVectorPending(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, struct THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_FALSE;
}
#else //__nvoc_intr_h_disabled
#define intrIsVectorPending(pGpu, pIntr, vector, pThreadState) intrIsVectorPending_TU102(pGpu, pIntr, vector, pThreadState)
#endif //__nvoc_intr_h_disabled

#define intrIsVectorPending_HAL(pGpu, pIntr, vector, pThreadState) intrIsVectorPending(pGpu, pIntr, vector, pThreadState)

NV_STATUS intrSetStallSWIntr_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrSetStallSWIntr(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrSetStallSWIntr(pGpu, pIntr) intrSetStallSWIntr_TU102(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrSetStallSWIntr_HAL(pGpu, pIntr) intrSetStallSWIntr(pGpu, pIntr)

NV_STATUS intrClearStallSWIntr_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrClearStallSWIntr(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrClearStallSWIntr(pGpu, pIntr) intrClearStallSWIntr_TU102(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrClearStallSWIntr_HAL(pGpu, pIntr) intrClearStallSWIntr(pGpu, pIntr)

void intrEnableStallSWIntr_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline void intrEnableStallSWIntr(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrEnableStallSWIntr(pGpu, pIntr) intrEnableStallSWIntr_TU102(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrEnableStallSWIntr_HAL(pGpu, pIntr) intrEnableStallSWIntr(pGpu, pIntr)

void intrDisableStallSWIntr_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline void intrDisableStallSWIntr(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrDisableStallSWIntr(pGpu, pIntr) intrDisableStallSWIntr_TU102(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrDisableStallSWIntr_HAL(pGpu, pIntr) intrDisableStallSWIntr(pGpu, pIntr)

static inline void intrResetIntrRegistersForVF_b3696a(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid) {
    return;
}

void intrResetIntrRegistersForVF_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid);


#ifdef __nvoc_intr_h_disabled
static inline void intrResetIntrRegistersForVF(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrResetIntrRegistersForVF(pGpu, pIntr, gfid) intrResetIntrRegistersForVF_b3696a(pGpu, pIntr, gfid)
#endif //__nvoc_intr_h_disabled

#define intrResetIntrRegistersForVF_HAL(pGpu, pIntr, gfid) intrResetIntrRegistersForVF(pGpu, pIntr, gfid)

static inline NV_STATUS intrSaveIntrRegValue_46f6a7(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 *arg4, NvU32 *arg5) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS intrSaveIntrRegValue_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 *arg4, NvU32 *arg5);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrSaveIntrRegValue(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 *arg4, NvU32 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrSaveIntrRegValue(pGpu, pIntr, arg3, arg4, arg5) intrSaveIntrRegValue_46f6a7(pGpu, pIntr, arg3, arg4, arg5)
#endif //__nvoc_intr_h_disabled

#define intrSaveIntrRegValue_HAL(pGpu, pIntr, arg3, arg4, arg5) intrSaveIntrRegValue(pGpu, pIntr, arg3, arg4, arg5)

static inline NV_STATUS intrRestoreIntrRegValue_46f6a7(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, NvU32 *arg5) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS intrRestoreIntrRegValue_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, NvU32 *arg5);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrRestoreIntrRegValue(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, NvU32 *arg5) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrRestoreIntrRegValue(pGpu, pIntr, arg3, arg4, arg5) intrRestoreIntrRegValue_46f6a7(pGpu, pIntr, arg3, arg4, arg5)
#endif //__nvoc_intr_h_disabled

#define intrRestoreIntrRegValue_HAL(pGpu, pIntr, arg3, arg4, arg5) intrRestoreIntrRegValue(pGpu, pIntr, arg3, arg4, arg5)

static inline NV_STATUS intrTriggerCpuDoorbellForVF_46f6a7(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS intrTriggerCpuDoorbellForVF_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrTriggerCpuDoorbellForVF(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrTriggerCpuDoorbellForVF(pGpu, pIntr, gfid) intrTriggerCpuDoorbellForVF_46f6a7(pGpu, pIntr, gfid)
#endif //__nvoc_intr_h_disabled

#define intrTriggerCpuDoorbellForVF_HAL(pGpu, pIntr, gfid) intrTriggerCpuDoorbellForVF(pGpu, pIntr, gfid)

void intrRetriggerTopLevel_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline void intrRetriggerTopLevel(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrRetriggerTopLevel(pGpu, pIntr) intrRetriggerTopLevel_TU102(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrRetriggerTopLevel_HAL(pGpu, pIntr) intrRetriggerTopLevel(pGpu, pIntr)

NV_STATUS intrGetLeafStatus_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetLeafStatus(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetLeafStatus(pGpu, pIntr, arg3, arg4) intrGetLeafStatus_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetLeafStatus_HAL(pGpu, pIntr, arg3, arg4) intrGetLeafStatus(pGpu, pIntr, arg3, arg4)

NV_STATUS intrGetPendingDisplayIntr_TU102(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *pEngines, struct THREAD_STATE_NODE *pThreadState);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetPendingDisplayIntr(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *pEngines, struct THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetPendingDisplayIntr(pGpu, pIntr, pEngines, pThreadState) intrGetPendingDisplayIntr_TU102(pGpu, pIntr, pEngines, pThreadState)
#endif //__nvoc_intr_h_disabled

#define intrGetPendingDisplayIntr_HAL(pGpu, pIntr, pEngines, pThreadState) intrGetPendingDisplayIntr(pGpu, pIntr, pEngines, pThreadState)

void intrDumpState_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline void intrDumpState(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrDumpState(pGpu, pIntr) intrDumpState_TU102(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrDumpState_HAL(pGpu, pIntr) intrDumpState(pGpu, pIntr)

NV_STATUS intrCacheIntrFields_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrCacheIntrFields(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrCacheIntrFields(pGpu, pIntr) intrCacheIntrFields_TU102(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrCacheIntrFields_HAL(pGpu, pIntr) intrCacheIntrFields(pGpu, pIntr)

NvU32 intrReadRegLeafEnSet_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

NvU32 intrReadRegLeafEnSet_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrReadRegLeafEnSet(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrReadRegLeafEnSet(pGpu, pIntr, arg3, arg4) intrReadRegLeafEnSet_CPU_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrReadRegLeafEnSet_HAL(pGpu, pIntr, arg3, arg4) intrReadRegLeafEnSet(pGpu, pIntr, arg3, arg4)

NvU32 intrReadRegLeaf_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

NvU32 intrReadRegLeaf_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrReadRegLeaf(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrReadRegLeaf(pGpu, pIntr, arg3, arg4) intrReadRegLeaf_CPU_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrReadRegLeaf_HAL(pGpu, pIntr, arg3, arg4) intrReadRegLeaf(pGpu, pIntr, arg3, arg4)

NvU32 intrReadRegTop_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

NvU32 intrReadRegTop_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrReadRegTop(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrReadRegTop(pGpu, pIntr, arg3, arg4) intrReadRegTop_CPU_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrReadRegTop_HAL(pGpu, pIntr, arg3, arg4) intrReadRegTop(pGpu, pIntr, arg3, arg4)

void intrWriteRegLeafEnSet_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegLeafEnSet_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);


#ifdef __nvoc_intr_h_disabled
static inline void intrWriteRegLeafEnSet(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrWriteRegLeafEnSet(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegLeafEnSet_CPU_TU102(pGpu, pIntr, arg3, arg4, arg5)
#endif //__nvoc_intr_h_disabled

#define intrWriteRegLeafEnSet_HAL(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegLeafEnSet(pGpu, pIntr, arg3, arg4, arg5)

void intrWriteRegLeafEnClear_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegLeafEnClear_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);


#ifdef __nvoc_intr_h_disabled
static inline void intrWriteRegLeafEnClear(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrWriteRegLeafEnClear(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegLeafEnClear_CPU_TU102(pGpu, pIntr, arg3, arg4, arg5)
#endif //__nvoc_intr_h_disabled

#define intrWriteRegLeafEnClear_HAL(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegLeafEnClear(pGpu, pIntr, arg3, arg4, arg5)

void intrWriteRegLeaf_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegLeaf_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);


#ifdef __nvoc_intr_h_disabled
static inline void intrWriteRegLeaf(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrWriteRegLeaf(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegLeaf_CPU_TU102(pGpu, pIntr, arg3, arg4, arg5)
#endif //__nvoc_intr_h_disabled

#define intrWriteRegLeaf_HAL(pGpu, pIntr, arg3, arg4, arg5) intrWriteRegLeaf(pGpu, pIntr, arg3, arg4, arg5)

static inline NV_STATUS intrRouteInterruptsToSystemFirmware_56cd7a(OBJGPU *pGpu, struct Intr *pIntr, NvBool bEnable) {
    return NV_OK;
}

NV_STATUS intrRouteInterruptsToSystemFirmware_GH100(OBJGPU *pGpu, struct Intr *pIntr, NvBool bEnable);

NV_STATUS intrRouteInterruptsToSystemFirmware_GB100(OBJGPU *pGpu, struct Intr *pIntr, NvBool bEnable);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrRouteInterruptsToSystemFirmware(OBJGPU *pGpu, struct Intr *pIntr, NvBool bEnable) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrRouteInterruptsToSystemFirmware(pGpu, pIntr, bEnable) intrRouteInterruptsToSystemFirmware_56cd7a(pGpu, pIntr, bEnable)
#endif //__nvoc_intr_h_disabled

#define intrRouteInterruptsToSystemFirmware_HAL(pGpu, pIntr, bEnable) intrRouteInterruptsToSystemFirmware(pGpu, pIntr, bEnable)

static inline NV_STATUS intrInitDynamicInterruptTable_5baef9(OBJGPU *pGpu, struct Intr *pIntr, struct OBJFIFO *arg3, InterruptTable *arg4, NvU32 initFlags) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrInitDynamicInterruptTable(OBJGPU *pGpu, struct Intr *pIntr, struct OBJFIFO *arg3, InterruptTable *arg4, NvU32 initFlags) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrInitDynamicInterruptTable(pGpu, pIntr, arg3, arg4, initFlags) intrInitDynamicInterruptTable_5baef9(pGpu, pIntr, arg3, arg4, initFlags)
#endif //__nvoc_intr_h_disabled

#define intrInitDynamicInterruptTable_HAL(pGpu, pIntr, arg3, arg4, initFlags) intrInitDynamicInterruptTable(pGpu, pIntr, arg3, arg4, initFlags)

static inline NV_STATUS intrInitAnyInterruptTable_5baef9(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable, NvU32 initFlags) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrInitAnyInterruptTable(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable, NvU32 initFlags) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrInitAnyInterruptTable(pGpu, pIntr, pIntrTable, initFlags) intrInitAnyInterruptTable_5baef9(pGpu, pIntr, pIntrTable, initFlags)
#endif //__nvoc_intr_h_disabled

#define intrInitAnyInterruptTable_HAL(pGpu, pIntr, pIntrTable, initFlags) intrInitAnyInterruptTable(pGpu, pIntr, pIntrTable, initFlags)

NV_STATUS intrGetInterruptTable_IMPL(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable **ppIntrTable);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetInterruptTable(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable **ppIntrTable) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetInterruptTable(pGpu, pIntr, ppIntrTable) intrGetInterruptTable_IMPL(pGpu, pIntr, ppIntrTable)
#endif //__nvoc_intr_h_disabled

#define intrGetInterruptTable_HAL(pGpu, pIntr, ppIntrTable) intrGetInterruptTable(pGpu, pIntr, ppIntrTable)

NV_STATUS intrDestroyInterruptTable_IMPL(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrDestroyInterruptTable(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrDestroyInterruptTable(pGpu, pIntr) intrDestroyInterruptTable_IMPL(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrDestroyInterruptTable_HAL(pGpu, pIntr) intrDestroyInterruptTable(pGpu, pIntr)

static inline NV_STATUS intrGetStaticVFmcEngines_5baef9(OBJGPU *pGpu, struct Intr *pIntr, NvU16 **ppMcEngines, NvU32 *pCount) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS intrGetStaticVFmcEngines_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU16 **ppMcEngines, NvU32 *pCount);

NV_STATUS intrGetStaticVFmcEngines_GA100(OBJGPU *pGpu, struct Intr *pIntr, NvU16 **ppMcEngines, NvU32 *pCount);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetStaticVFmcEngines(OBJGPU *pGpu, struct Intr *pIntr, NvU16 **ppMcEngines, NvU32 *pCount) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetStaticVFmcEngines(pGpu, pIntr, ppMcEngines, pCount) intrGetStaticVFmcEngines_5baef9(pGpu, pIntr, ppMcEngines, pCount)
#endif //__nvoc_intr_h_disabled

#define intrGetStaticVFmcEngines_HAL(pGpu, pIntr, ppMcEngines, pCount) intrGetStaticVFmcEngines(pGpu, pIntr, ppMcEngines, pCount)

static inline NV_STATUS intrGetStaticInterruptTable_5baef9(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pTable, NvU32 initFlags) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS intrGetStaticInterruptTable_TU102(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pTable, NvU32 initFlags);

NV_STATUS intrGetStaticInterruptTable_GA100(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pTable, NvU32 initFlags);

NV_STATUS intrGetStaticInterruptTable_GA102(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pTable, NvU32 initFlags);

NV_STATUS intrGetStaticInterruptTable_GH100(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pTable, NvU32 initFlags);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetStaticInterruptTable(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pTable, NvU32 initFlags) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetStaticInterruptTable(pGpu, pIntr, pTable, initFlags) intrGetStaticInterruptTable_5baef9(pGpu, pIntr, pTable, initFlags)
#endif //__nvoc_intr_h_disabled

#define intrGetStaticInterruptTable_HAL(pGpu, pIntr, pTable, initFlags) intrGetStaticInterruptTable(pGpu, pIntr, pTable, initFlags)

static inline NV_STATUS intrInitGPUHostInterruptTable_5baef9(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable, NvU32 initFlags) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS intrInitGPUHostInterruptTable_GM107(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable, NvU32 initFlags);

NV_STATUS intrInitGPUHostInterruptTable_GA100(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable, NvU32 initFlags);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrInitGPUHostInterruptTable(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable, NvU32 initFlags) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrInitGPUHostInterruptTable(pGpu, pIntr, pIntrTable, initFlags) intrInitGPUHostInterruptTable_5baef9(pGpu, pIntr, pIntrTable, initFlags)
#endif //__nvoc_intr_h_disabled

#define intrInitGPUHostInterruptTable_HAL(pGpu, pIntr, pIntrTable, initFlags) intrInitGPUHostInterruptTable(pGpu, pIntr, pIntrTable, initFlags)

static inline NV_STATUS intrInitEngineSchedInterruptTable_5baef9(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrInitEngineSchedInterruptTable(OBJGPU *pGpu, struct Intr *pIntr, InterruptTable *pIntrTable) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrInitEngineSchedInterruptTable(pGpu, pIntr, pIntrTable) intrInitEngineSchedInterruptTable_5baef9(pGpu, pIntr, pIntrTable)
#endif //__nvoc_intr_h_disabled

#define intrInitEngineSchedInterruptTable_HAL(pGpu, pIntr, pIntrTable) intrInitEngineSchedInterruptTable(pGpu, pIntr, pIntrTable)

void intrServiceStall_IMPL(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline void intrServiceStall(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrServiceStall(pGpu, pIntr) intrServiceStall_IMPL(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrServiceStall_HAL(pGpu, pIntr) intrServiceStall(pGpu, pIntr)

void intrServiceStallList_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool arg4);


#ifdef __nvoc_intr_h_disabled
static inline void intrServiceStallList(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrServiceStallList(pGpu, pIntr, arg3, arg4) intrServiceStallList_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrServiceStallList_HAL(pGpu, pIntr, arg3, arg4) intrServiceStallList(pGpu, pIntr, arg3, arg4)

void intrServiceStallSingle_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU16 arg3, NvBool arg4);


#ifdef __nvoc_intr_h_disabled
static inline void intrServiceStallSingle(OBJGPU *pGpu, struct Intr *pIntr, NvU16 arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrServiceStallSingle(pGpu, pIntr, arg3, arg4) intrServiceStallSingle_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrServiceStallSingle_HAL(pGpu, pIntr, arg3, arg4) intrServiceStallSingle(pGpu, pIntr, arg3, arg4)

void intrProcessDPCQueue_IMPL(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline void intrProcessDPCQueue(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrProcessDPCQueue(pGpu, pIntr) intrProcessDPCQueue_IMPL(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrProcessDPCQueue_HAL(pGpu, pIntr) intrProcessDPCQueue(pGpu, pIntr)

NV_STATUS intrGetIntrMask_GP100(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetIntrMask(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetIntrMask(pGpu, pIntr, arg3, arg4) intrGetIntrMask_GP100(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetIntrMask_HAL(pGpu, pIntr, arg3, arg4) intrGetIntrMask(pGpu, pIntr, arg3, arg4)

static inline NV_STATUS intrGetEccIntrMaskOffset_5baef9(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS intrGetEccIntrMaskOffset_GP100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4);

static inline NV_STATUS intrGetEccIntrMaskOffset_46f6a7(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetEccIntrMaskOffset(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetEccIntrMaskOffset(pGpu, pIntr, arg3, arg4) intrGetEccIntrMaskOffset_5baef9(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetEccIntrMaskOffset_HAL(pGpu, pIntr, arg3, arg4) intrGetEccIntrMaskOffset(pGpu, pIntr, arg3, arg4)

static inline NV_STATUS intrGetNvlinkIntrMaskOffset_5baef9(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS intrGetNvlinkIntrMaskOffset_GP100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4);

static inline NV_STATUS intrGetNvlinkIntrMaskOffset_46f6a7(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetNvlinkIntrMaskOffset(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetNvlinkIntrMaskOffset(pGpu, pIntr, arg3, arg4) intrGetNvlinkIntrMaskOffset_5baef9(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetNvlinkIntrMaskOffset_HAL(pGpu, pIntr, arg3, arg4) intrGetNvlinkIntrMaskOffset(pGpu, pIntr, arg3, arg4)

static inline NvBool intrRequiresPossibleErrorNotifier_491d52(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *pEngines) {
    return ((NvBool)(0 != 0));
}

NvBool intrRequiresPossibleErrorNotifier_TU102(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *pEngines);

NvBool intrRequiresPossibleErrorNotifier_GA100(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *pEngines);

NvBool intrRequiresPossibleErrorNotifier_GH100(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *pEngines);


#ifdef __nvoc_intr_h_disabled
static inline NvBool intrRequiresPossibleErrorNotifier(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *pEngines) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_FALSE;
}
#else //__nvoc_intr_h_disabled
#define intrRequiresPossibleErrorNotifier(pGpu, pIntr, pEngines) intrRequiresPossibleErrorNotifier_491d52(pGpu, pIntr, pEngines)
#endif //__nvoc_intr_h_disabled

#define intrRequiresPossibleErrorNotifier_HAL(pGpu, pIntr, pEngines) intrRequiresPossibleErrorNotifier(pGpu, pIntr, pEngines)

static inline NvU32 intrReadErrCont_4a4dee(OBJGPU *pGpu, struct Intr *pIntr) {
    return 0;
}

NvU32 intrReadErrCont_TU102(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrReadErrCont(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrReadErrCont(pGpu, pIntr) intrReadErrCont_4a4dee(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrReadErrCont_HAL(pGpu, pIntr) intrReadErrCont(pGpu, pIntr)

NV_STATUS intrGetPendingStall_GP100(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetPendingStall(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetPendingStall(pGpu, pIntr, arg3, arg4) intrGetPendingStall_GP100(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetPendingStall_HAL(pGpu, pIntr, arg3, arg4) intrGetPendingStall(pGpu, pIntr, arg3, arg4)

NV_STATUS intrGetPendingStallEngines_TU102(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetPendingStallEngines(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetPendingStallEngines(pGpu, pIntr, arg3, arg4) intrGetPendingStallEngines_TU102(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetPendingStallEngines_HAL(pGpu, pIntr, arg3, arg4) intrGetPendingStallEngines(pGpu, pIntr, arg3, arg4)

NvBool intrIsIntrEnabled_IMPL(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_intr_h_disabled
static inline NvBool intrIsIntrEnabled(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_FALSE;
}
#else //__nvoc_intr_h_disabled
#define intrIsIntrEnabled(pGpu, pIntr, arg3) intrIsIntrEnabled_IMPL(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

#define intrIsIntrEnabled_HAL(pGpu, pIntr, arg3) intrIsIntrEnabled(pGpu, pIntr, arg3)

static inline void intrSetHubLeafIntr_b3696a(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 *arg4, NvU32 *arg5, struct THREAD_STATE_NODE *arg6) {
    return;
}


#ifdef __nvoc_intr_h_disabled
static inline void intrSetHubLeafIntr(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 *arg4, NvU32 *arg5, struct THREAD_STATE_NODE *arg6) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSetHubLeafIntr(pGpu, pIntr, arg3, arg4, arg5, arg6) intrSetHubLeafIntr_b3696a(pGpu, pIntr, arg3, arg4, arg5, arg6)
#endif //__nvoc_intr_h_disabled

#define intrSetHubLeafIntr_HAL(pGpu, pIntr, arg3, arg4, arg5, arg6) intrSetHubLeafIntr(pGpu, pIntr, arg3, arg4, arg5, arg6)

void intrGetHubLeafIntrPending_STUB(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);


#ifdef __nvoc_intr_h_disabled
static inline void intrGetHubLeafIntrPending(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrGetHubLeafIntrPending(pGpu, pIntr, arg3, arg4) intrGetHubLeafIntrPending_STUB(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#define intrGetHubLeafIntrPending_HAL(pGpu, pIntr, arg3, arg4) intrGetHubLeafIntrPending(pGpu, pIntr, arg3, arg4)

NV_STATUS intrRefetchInterruptTable_IMPL(OBJGPU *pGpu, struct Intr *pIntr);


#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrRefetchInterruptTable(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrRefetchInterruptTable(pGpu, pIntr) intrRefetchInterruptTable_IMPL(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

#define intrRefetchInterruptTable_HAL(pGpu, pIntr) intrRefetchInterruptTable(pGpu, pIntr)

NV_STATUS intrConstructEngine_IMPL(OBJGPU *pGpu, struct Intr *pIntr, ENGDESCRIPTOR arg3);

NV_STATUS intrStatePreInitLocked_IMPL(OBJGPU *pGpu, struct Intr *pIntr);

NV_STATUS intrStateInitUnlocked_IMPL(OBJGPU *pGpu, struct Intr *pIntr);

NV_STATUS intrStateInitLocked_IMPL(OBJGPU *pGpu, struct Intr *pIntr);

void intrStateDestroy_IMPL(OBJGPU *pGpu, struct Intr *pIntr);

NvU32 intrDecodeStallIntrEn_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3);

static inline NvU32 intrDecodeStallIntrEn_4a4dee(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3) {
    return 0;
}

static inline void intrServiceVirtual_f2d351(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_PRECOMP(0);
}

void intrServiceVirtual_TU102(OBJGPU *pGpu, struct Intr *pIntr);

static inline NV_STATUS intrTriggerPrivDoorbell_5baef9(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS intrTriggerPrivDoorbell_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 gfid);

void intrGetLocklessVectorsInRmSubtree_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 (*pInterruptVectors)[2]);

void intrGetLocklessVectorsInRmSubtree_GA100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 (*pInterruptVectors)[2]);

void intrSetDisplayInterruptEnable_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvBool bEnable, struct THREAD_STATE_NODE *pThreadState);

static inline void intrSetDisplayInterruptEnable_b3696a(OBJGPU *pGpu, struct Intr *pIntr, NvBool bEnable, struct THREAD_STATE_NODE *pThreadState) {
    return;
}

NvU32 intrReadRegTopEnSet_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

NvU32 intrReadRegTopEnSet_CPU_GA102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

NvU32 intrReadRegTopEnSet_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

NvU32 intrReadRegTopEnSet_GSP_GA102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

void intrWriteRegTopEnSet_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegTopEnSet_CPU_GA102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegTopEnSet_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegTopEnSet_GSP_GA102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegTopEnClear_CPU_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegTopEnClear_CPU_GA102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegTopEnClear_GSP_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

void intrWriteRegTopEnClear_GSP_GA102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);

NvU32 intrGetNumLeaves_TU102(OBJGPU *pGpu, struct Intr *pIntr);

NvU32 intrGetNumLeaves_GH100(OBJGPU *pGpu, struct Intr *pIntr);

NvU32 intrGetLeafSize_TU102(OBJGPU *pGpu, struct Intr *pIntr);

NvU32 intrGetLeafSize_GH100(OBJGPU *pGpu, struct Intr *pIntr);

NvU64 intrGetIntrTopNonStallMask_TU102(OBJGPU *pGpu, struct Intr *pIntr);

NvU64 intrGetIntrTopNonStallMask_GH100(OBJGPU *pGpu, struct Intr *pIntr);

void intrSanityCheckEngineIntrStallVector_GA100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine);

void intrSanityCheckEngineIntrStallVector_GH100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine);

static inline void intrSanityCheckEngineIntrStallVector_b3696a(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine) {
    return;
}

void intrSanityCheckEngineIntrNotificationVector_GA100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine);

void intrSanityCheckEngineIntrNotificationVector_GH100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine);

static inline void intrSanityCheckEngineIntrNotificationVector_b3696a(OBJGPU *pGpu, struct Intr *pIntr, NvU32 vector, NvU16 mcEngine) {
    return;
}

NV_STATUS intrStateLoad_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3);

NV_STATUS intrStateUnload_TU102(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3);

static inline NV_STATUS intrInitSubtreeMap_395e98(OBJGPU *pGpu, struct Intr *pIntr) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS intrInitSubtreeMap_TU102(OBJGPU *pGpu, struct Intr *pIntr);

NV_STATUS intrInitSubtreeMap_GH100(OBJGPU *pGpu, struct Intr *pIntr);

NV_STATUS intrInitInterruptTable_VF(OBJGPU *pGpu, struct Intr *pIntr);

NV_STATUS intrInitInterruptTable_KERNEL(OBJGPU *pGpu, struct Intr *pIntr);

NV_STATUS intrSetIntrMask_GP100(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);

static inline NV_STATUS intrSetIntrMask_46f6a7(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

void intrSetIntrEnInHw_GP100(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4);

static inline void intrSetIntrEnInHw_d44104(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, struct THREAD_STATE_NODE *arg4) {
    return;
}

NvU32 intrGetIntrEnFromHw_GP100(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3);

static inline NvU32 intrGetIntrEnFromHw_b2b553(OBJGPU *pGpu, struct Intr *pIntr, struct THREAD_STATE_NODE *arg3) {
    return 0;
}

void intrDestruct_IMPL(struct Intr *pIntr);

#define __nvoc_intrDestruct(pIntr) intrDestruct_IMPL(pIntr)
NV_STATUS intrServiceNonStallBottomHalf_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);

#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrServiceNonStallBottomHalf(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrServiceNonStallBottomHalf(pGpu, pIntr, arg3, arg4) intrServiceNonStallBottomHalf_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

NV_STATUS intrServiceNotificationRecords_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU16 mcEngineIdx, struct THREAD_STATE_NODE *arg4);

#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrServiceNotificationRecords(OBJGPU *pGpu, struct Intr *pIntr, NvU16 mcEngineIdx, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrServiceNotificationRecords(pGpu, pIntr, mcEngineIdx, arg4) intrServiceNotificationRecords_IMPL(pGpu, pIntr, mcEngineIdx, arg4)
#endif //__nvoc_intr_h_disabled

NvU64 intrGetUvmSharedLeafEnDisableMask_IMPL(OBJGPU *pGpu, struct Intr *pIntr);

#ifdef __nvoc_intr_h_disabled
static inline NvU64 intrGetUvmSharedLeafEnDisableMask(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetUvmSharedLeafEnDisableMask(pGpu, pIntr) intrGetUvmSharedLeafEnDisableMask_IMPL(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

NvU64 intrGetIntrTopLegacyStallMask_IMPL(struct Intr *pIntr);

#ifdef __nvoc_intr_h_disabled
static inline NvU64 intrGetIntrTopLegacyStallMask(struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetIntrTopLegacyStallMask(pIntr) intrGetIntrTopLegacyStallMask_IMPL(pIntr)
#endif //__nvoc_intr_h_disabled

NvU64 intrGetIntrTopLockedMask_IMPL(OBJGPU *pGpu, struct Intr *pIntr);

#ifdef __nvoc_intr_h_disabled
static inline NvU64 intrGetIntrTopLockedMask(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetIntrTopLockedMask(pGpu, pIntr) intrGetIntrTopLockedMask_IMPL(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

NV_STATUS intrGetSubtreeRange_IMPL(struct Intr *pIntr, NV2080_INTR_CATEGORY category, NV2080_INTR_CATEGORY_SUBTREE_MAP *pRange);

#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetSubtreeRange(struct Intr *pIntr, NV2080_INTR_CATEGORY category, NV2080_INTR_CATEGORY_SUBTREE_MAP *pRange) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetSubtreeRange(pIntr, category, pRange) intrGetSubtreeRange_IMPL(pIntr, category, pRange)
#endif //__nvoc_intr_h_disabled

NvU64 intrGetIntrTopCategoryMask_IMPL(struct Intr *pIntr, NV2080_INTR_CATEGORY category);

#ifdef __nvoc_intr_h_disabled
static inline NvU64 intrGetIntrTopCategoryMask(struct Intr *pIntr, NV2080_INTR_CATEGORY category) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetIntrTopCategoryMask(pIntr, category) intrGetIntrTopCategoryMask_IMPL(pIntr, category)
#endif //__nvoc_intr_h_disabled

NV_STATUS intrSetInterruptEntry_IMPL(struct Intr *pIntr, INTR_TREE tree, NvU32 vector, const InterruptEntry *pEntry);

#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrSetInterruptEntry(struct Intr *pIntr, INTR_TREE tree, NvU32 vector, const InterruptEntry *pEntry) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrSetInterruptEntry(pIntr, tree, vector, pEntry) intrSetInterruptEntry_IMPL(pIntr, tree, vector, pEntry)
#endif //__nvoc_intr_h_disabled

void intrServiceStallListAllGpusCond_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool arg4);

#ifdef __nvoc_intr_h_disabled
static inline void intrServiceStallListAllGpusCond(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrServiceStallListAllGpusCond(pGpu, pIntr, arg3, arg4) intrServiceStallListAllGpusCond_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

void intrServiceStallListDevice_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool arg4);

#ifdef __nvoc_intr_h_disabled
static inline void intrServiceStallListDevice(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, NvBool arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrServiceStallListDevice(pGpu, pIntr, arg3, arg4) intrServiceStallListDevice_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

NvU32 intrServiceInterruptRecords_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU16 arg3, NvBool *arg4);

#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrServiceInterruptRecords(OBJGPU *pGpu, struct Intr *pIntr, NvU16 arg3, NvBool *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrServiceInterruptRecords(pGpu, pIntr, arg3, arg4) intrServiceInterruptRecords_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

void intrQueueDpc_IMPL(OBJGPU *pGpu, struct Intr *pIntr, DPCQUEUE *arg3, DPCNODE *arg4);

#ifdef __nvoc_intr_h_disabled
static inline void intrQueueDpc(OBJGPU *pGpu, struct Intr *pIntr, DPCQUEUE *arg3, DPCNODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrQueueDpc(pGpu, pIntr, arg3, arg4) intrQueueDpc_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

DPCNODE *intrDequeueDpc_IMPL(OBJGPU *pGpu, struct Intr *pIntr, DPCQUEUE *arg3);

#ifdef __nvoc_intr_h_disabled
static inline DPCNODE *intrDequeueDpc(OBJGPU *pGpu, struct Intr *pIntr, DPCQUEUE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NULL;
}
#else //__nvoc_intr_h_disabled
#define intrDequeueDpc(pGpu, pIntr, arg3) intrDequeueDpc_IMPL(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

NvBool intrIsDpcQueueEmpty_IMPL(OBJGPU *pGpu, struct Intr *pIntr, DPCQUEUE *arg3);

#ifdef __nvoc_intr_h_disabled
static inline NvBool intrIsDpcQueueEmpty(OBJGPU *pGpu, struct Intr *pIntr, DPCQUEUE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_FALSE;
}
#else //__nvoc_intr_h_disabled
#define intrIsDpcQueueEmpty(pGpu, pIntr, arg3) intrIsDpcQueueEmpty_IMPL(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

void intrQueueInterruptBasedDpc_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU16 arg3);

#ifdef __nvoc_intr_h_disabled
static inline void intrQueueInterruptBasedDpc(OBJGPU *pGpu, struct Intr *pIntr, NvU16 arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrQueueInterruptBasedDpc(pGpu, pIntr, arg3) intrQueueInterruptBasedDpc_IMPL(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

NvU32 intrConvertEngineMaskToPmcIntrMask_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3);

#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrConvertEngineMaskToPmcIntrMask(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrConvertEngineMaskToPmcIntrMask(pGpu, pIntr, arg3) intrConvertEngineMaskToPmcIntrMask_IMPL(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

void intrConvertPmcIntrMaskToEngineMask_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, union MC_ENGINE_BITVECTOR *arg4);

#ifdef __nvoc_intr_h_disabled
static inline void intrConvertPmcIntrMaskToEngineMask(OBJGPU *pGpu, struct Intr *pIntr, NvU32 arg3, union MC_ENGINE_BITVECTOR *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrConvertPmcIntrMaskToEngineMask(pGpu, pIntr, arg3, arg4) intrConvertPmcIntrMaskToEngineMask_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

INTR_TABLE_ENTRY *intrGetInterruptTableEntryFromEngineId_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU16 mcEngineId, NvBool bNonStall);

#ifdef __nvoc_intr_h_disabled
static inline INTR_TABLE_ENTRY *intrGetInterruptTableEntryFromEngineId(OBJGPU *pGpu, struct Intr *pIntr, NvU16 mcEngineId, NvBool bNonStall) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NULL;
}
#else //__nvoc_intr_h_disabled
#define intrGetInterruptTableEntryFromEngineId(pGpu, pIntr, mcEngineId, bNonStall) intrGetInterruptTableEntryFromEngineId_IMPL(pGpu, pIntr, mcEngineId, bNonStall)
#endif //__nvoc_intr_h_disabled

NvU32 intrGetVectorFromEngineId_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU16 mcEngineId, NvBool bNonStall);

#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrGetVectorFromEngineId(OBJGPU *pGpu, struct Intr *pIntr, NvU16 mcEngineId, NvBool bNonStall) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetVectorFromEngineId(pGpu, pIntr, mcEngineId, bNonStall) intrGetVectorFromEngineId_IMPL(pGpu, pIntr, mcEngineId, bNonStall)
#endif //__nvoc_intr_h_disabled

NV_STATUS intrGetSmallestNotificationVector_IMPL(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3);

#ifdef __nvoc_intr_h_disabled
static inline NV_STATUS intrGetSmallestNotificationVector(OBJGPU *pGpu, struct Intr *pIntr, NvU32 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intr_h_disabled
#define intrGetSmallestNotificationVector(pGpu, pIntr, arg3) intrGetSmallestNotificationVector_IMPL(pGpu, pIntr, arg3)
#endif //__nvoc_intr_h_disabled

void intrSetIntrMaskUnblocked_IMPL(struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg2);

#ifdef __nvoc_intr_h_disabled
static inline void intrSetIntrMaskUnblocked(struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSetIntrMaskUnblocked(pIntr, arg2) intrSetIntrMaskUnblocked_IMPL(pIntr, arg2)
#endif //__nvoc_intr_h_disabled

void intrGetIntrMaskUnblocked_IMPL(struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg2);

#ifdef __nvoc_intr_h_disabled
static inline void intrGetIntrMaskUnblocked(struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg2) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrGetIntrMaskUnblocked(pIntr, arg2) intrGetIntrMaskUnblocked_IMPL(pIntr, arg2)
#endif //__nvoc_intr_h_disabled

void intrSetIntrMaskFlags_IMPL(struct Intr *pIntr, NvU32 arg2);

#ifdef __nvoc_intr_h_disabled
static inline void intrSetIntrMaskFlags(struct Intr *pIntr, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSetIntrMaskFlags(pIntr, arg2) intrSetIntrMaskFlags_IMPL(pIntr, arg2)
#endif //__nvoc_intr_h_disabled

NvU32 intrGetIntrMaskFlags_IMPL(struct Intr *pIntr);

#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrGetIntrMaskFlags(struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetIntrMaskFlags(pIntr) intrGetIntrMaskFlags_IMPL(pIntr)
#endif //__nvoc_intr_h_disabled

void intrSetDefaultIntrEn_IMPL(struct Intr *pIntr, NvU32 arg2);

#ifdef __nvoc_intr_h_disabled
static inline void intrSetDefaultIntrEn(struct Intr *pIntr, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSetDefaultIntrEn(pIntr, arg2) intrSetDefaultIntrEn_IMPL(pIntr, arg2)
#endif //__nvoc_intr_h_disabled

NvU32 intrGetDefaultIntrEn_IMPL(struct Intr *pIntr);

#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrGetDefaultIntrEn(struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetDefaultIntrEn(pIntr) intrGetDefaultIntrEn_IMPL(pIntr)
#endif //__nvoc_intr_h_disabled

void intrSetIntrEn_IMPL(struct Intr *pIntr, NvU32 arg2);

#ifdef __nvoc_intr_h_disabled
static inline void intrSetIntrEn(struct Intr *pIntr, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSetIntrEn(pIntr, arg2) intrSetIntrEn_IMPL(pIntr, arg2)
#endif //__nvoc_intr_h_disabled

NvU32 intrGetIntrEn_IMPL(struct Intr *pIntr);

#ifdef __nvoc_intr_h_disabled
static inline NvU32 intrGetIntrEn(struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
    return 0;
}
#else //__nvoc_intr_h_disabled
#define intrGetIntrEn(pIntr) intrGetIntrEn_IMPL(pIntr)
#endif //__nvoc_intr_h_disabled

void intrSaveIntrEn0FromHw_IMPL(OBJGPU *pGpu, struct Intr *pIntr);

#ifdef __nvoc_intr_h_disabled
static inline void intrSaveIntrEn0FromHw(OBJGPU *pGpu, struct Intr *pIntr) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrSaveIntrEn0FromHw(pGpu, pIntr) intrSaveIntrEn0FromHw_IMPL(pGpu, pIntr)
#endif //__nvoc_intr_h_disabled

void intrGetGmmuInterrupts_IMPL(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4);

#ifdef __nvoc_intr_h_disabled
static inline void intrGetGmmuInterrupts(OBJGPU *pGpu, struct Intr *pIntr, union MC_ENGINE_BITVECTOR *arg3, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("Intr was disabled!");
}
#else //__nvoc_intr_h_disabled
#define intrGetGmmuInterrupts(pGpu, pIntr, arg3, arg4) intrGetGmmuInterrupts_IMPL(pGpu, pIntr, arg3, arg4)
#endif //__nvoc_intr_h_disabled

#undef PRIVATE_FIELD


// This mask is used for interrupts that should be masked off in the PMC tree
#define NV_PMC_INTR_INVALID_MASK (0)

#endif // INTR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_INTR_NVOC_H_
