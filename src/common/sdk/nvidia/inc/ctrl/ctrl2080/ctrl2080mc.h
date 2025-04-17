/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080mc.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"
#include "nvcfg_sdk.h"

/* NV20_SUBDEVICE_XX mc control commands and parameters */

/**
 * NV2080_CTRL_CMD_MC_GET_ARCH_INFO
 *
 * This command returns chip architecture information from the
 * master control engine in the specified GPU.
 *
 *   architecture
 *       This parameter specifies the architecture level for the GPU.
 *   implementation
 *       This parameter specifies the implementation of the architecture
 *       for the GPU.
 *   revision
 *       This parameter specifies the revision of the mask used to produce
 *       the GPU.
 *   subRevision
 *       This parameter specific the sub revision of the GPU. Value is one of
 *       NV2080_CTRL_MC_ARCH_INFO_SUBREVISION_*
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_MC_GET_ARCH_INFO (0x20801701) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_MC_INTERFACE_ID << 8) | NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS {
    NvU32 architecture;
    NvU32 implementation;
    NvU32 revision;
    NvU8  subRevision;
} NV2080_CTRL_MC_GET_ARCH_INFO_PARAMS;

/* valid architecture values */

#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_T23X          (0xE0000023)


#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100         (0x00000160)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100         (0x00000170)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100         (0x00000180)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100         (0x00000190)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100         (0x000001A0)

#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB200         (0x000001B0)



/* valid ARCHITECTURE_T23X implementation values */


#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_T234        (0x00000004)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_T234D       (0x00000005)


/* valid ARCHITECTURE_TU10x implementation values */
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_TU100       (0x00000000)


#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_TU102       (0x00000002)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_TU104       (0x00000004)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_TU106       (0x00000006)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_TU116       (0x00000008)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_TU117       (0x00000007)


/* valid ARCHITECTURE_GA10x implementation values */
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA100       (0x00000000)


#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA102       (0x00000002)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA103       (0x00000003)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA104       (0x00000004)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA106       (0x00000006)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA107       (0x00000007)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA10B       (0x0000000B)


/* valid ARCHITECTURE_GH10x implementation values */
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GH100       (0x00000000)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GH100_SOC   (0x00000001)

/* valid ARCHITECTURE_AD10x implementation values */
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD100       (0x00000000)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD000       (0x00000001)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD101       (0x00000001)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD102       (0x00000002)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD103       (0x00000003)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD104       (0x00000004)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD106       (0x00000006)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD107       (0x00000007)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_AD10B       (0x0000000B)
/* valid ARCHITECTURE_GB10x implementation values */
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB100       (0x00000000)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB102       (0x00000002)

#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB110       (0x00000003)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB112       (0x00000004)


/* valid ARCHITECTURE_GB20x implementation values */
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB200       (0x00000000)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB202       (0x00000002)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB203       (0x00000003)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB205       (0x00000005)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB206       (0x00000006)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB207       (0x00000007)


#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GB20B       (0x0000000B)



/* Valid Chip sub revisions */
#define NV2080_CTRL_MC_ARCH_INFO_SUBREVISION_NO_SUBREVISION (0x00000000)
#define NV2080_CTRL_MC_ARCH_INFO_SUBREVISION_P              (0x00000001)
#define NV2080_CTRL_MC_ARCH_INFO_SUBREVISION_Q              (0x00000002)
#define NV2080_CTRL_MC_ARCH_INFO_SUBREVISION_R              (0x00000003)

/*
 * NV2080_CTRL_CMD_MC_SERVICE_INTERRUPTS
 *
 * This command instructs the RM to service interrupts for the specified
 * engine(s).
 *
 *   engines
 *       This parameter specifies which engines should have their interrupts
 *       serviced.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_MC_SERVICE_INTERRUPTS               (0x20801702) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_MC_INTERFACE_ID << 8) | NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MC_ENGINE_ID_GRAPHICS                   0x00000001
#define NV2080_CTRL_MC_ENGINE_ID_ALL                        0xFFFFFFFF

#define NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS {
    NvU32 engines;
} NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS;


/*
 * NV2080_CTRL_CMD_MC_GET_MANUFACTURER
 *
 * This command returns the GPU manufacturer information for the associated
 * subdevice.
 *
 *   manufacturer
 *       This parameter returns the manufacturer value for the GPU.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_MC_GET_MANUFACTURER (0x20801703) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_MC_INTERFACE_ID << 8) | NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS {
    NvU32 manufacturer;
} NV2080_CTRL_MC_GET_MANUFACTURER_PARAMS;



/*
 * NV2080_CTRL_CMD_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP
 *
 * This call will setup RM to either service or ignore the
 * repayable fault interrupt.
 * This is a privileged call that can only be called by the UVM driver
 * when it will take ownership of the repalayable fault interrupt.
 *
 * Possible status values returned are:
 *   NVOS_STATUS_SUCCESS
 *   NVOS_STATUS_ERROR_INVALID_ARGUMENT
 *   NVOS_STATUS_ERROR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP (0x2080170c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_MC_INTERFACE_ID << 8) | NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS {
    NvBool bOwnedByRm;
} NV2080_CTRL_MC_CHANGE_REPLAYABLE_FAULT_OWNERSHIP_PARAMS;

/*
 *  NV2080_CTRL_CMD_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS
 *
 *  This command gets the notification interrupt vectors device for all VGPU engines from Host RM.
 *
 *  Parameters:
 *
 *    entries [out]
 *      A buffer to store up to MAX_ENGINES entries of type
 *      NV2080_CTRL_MC_ENGINE_NOTIFICATION_INTR_VECTOR_ENTRY.
 *
 *    numEntries [out]
 *      Number of populated entries in the provided buffer.
 *
 *  Possible status values returned are:
 *    NV_OK
 */
#define NV2080_CTRL_CMD_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS         (0x2080170d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_MC_INTERFACE_ID << 8) | NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_MAX_ENGINES 256

typedef struct NV2080_CTRL_MC_ENGINE_NOTIFICATION_INTR_VECTOR_ENTRY {
    NvU32 nv2080EngineType;
    NvU32 notificationIntrVector;
} NV2080_CTRL_MC_ENGINE_NOTIFICATION_INTR_VECTOR_ENTRY;

#define NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS {
    NvU32                                                numEntries;
    NV2080_CTRL_MC_ENGINE_NOTIFICATION_INTR_VECTOR_ENTRY entries[NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_MAX_ENGINES];
} NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS;

/*
 *  NV2080_CTRL_CMD_MC_GET_STATIC_INTR_TABLE
 *
 *  This command gets the static interrupts needed by VGPU from Host RM.
 *
 *  Parameters:
 *
 *    entries [out]
 *      A buffer to store up to MAX_ENGINES entries of type
 *      NV2080_CTRL_MC_STATIC_INTR_ENTRY.
 *
 *    numEntries [out]
 *      Number of populated entries in the provided buffer.
 *
 *  Possible status values returned are:
 *    NV_OK
 */
#define NV2080_CTRL_CMD_MC_GET_STATIC_INTR_TABLE    (0x2080170e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_MC_INTERFACE_ID << 8) | NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_MAX    32

// Interface defines for static MC_ENGINE_IDX defines
#define NV2080_INTR_TYPE_NULL                       (0x00000000)
#define NV2080_INTR_TYPE_NON_REPLAYABLE_FAULT       (0x00000001)
#define NV2080_INTR_TYPE_NON_REPLAYABLE_FAULT_ERROR (0x00000002)
#define NV2080_INTR_TYPE_INFO_FAULT                 (0x00000003)
#define NV2080_INTR_TYPE_REPLAYABLE_FAULT           (0x00000004)
#define NV2080_INTR_TYPE_REPLAYABLE_FAULT_ERROR     (0x00000005)
#define NV2080_INTR_TYPE_ACCESS_CNTR                (0x00000006)
#define NV2080_INTR_TYPE_TMR                        (0x00000007)
#define NV2080_INTR_TYPE_CPU_DOORBELL               (0x00000008)
#define NV2080_INTR_TYPE_GR0_FECS_LOG               (0x00000009)
#define NV2080_INTR_TYPE_GR1_FECS_LOG               (0x0000000A)
#define NV2080_INTR_TYPE_GR2_FECS_LOG               (0x0000000B)
#define NV2080_INTR_TYPE_GR3_FECS_LOG               (0x0000000C)
#define NV2080_INTR_TYPE_GR4_FECS_LOG               (0x0000000D)
#define NV2080_INTR_TYPE_GR5_FECS_LOG               (0x0000000E)
#define NV2080_INTR_TYPE_GR6_FECS_LOG               (0x0000000F)
#define NV2080_INTR_TYPE_GR7_FECS_LOG               (0x00000010)

typedef struct NV2080_CTRL_MC_STATIC_INTR_ENTRY {
    NvU32 nv2080IntrType;
    NvU32 pmcIntrMask;
    NvU32 intrVectorStall;
    NvU32 intrVectorNonStall;
} NV2080_CTRL_MC_STATIC_INTR_ENTRY;

#define NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS {
    NvU32                            numEntries;
    NV2080_CTRL_MC_STATIC_INTR_ENTRY entries[NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_MAX];
} NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS;

/* _ctrl2080mc_h_ */
