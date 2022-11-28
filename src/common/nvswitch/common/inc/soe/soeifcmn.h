/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _GSOEIFCMN_H_
#define _GSOEIFCMN_H_

/*!
 * @file   soeifcmn.h
 * @brief  SOE Interface - Common Definitions
 *
 * Structures, attributes, or other properties common to ALL commands and
 * messages are defined here.
 */

#include "nvtypes.h"

/*!
 * Convenience macros for determining the size of body for a command or message:
 */
#define RM_SOE_CMD_BODY_SIZE(u,t)      sizeof(RM_SOE_##u##_CMD_##t)
#define RM_SOE_MSG_BODY_SIZE(u,t)      sizeof(RM_SOE_##u##_MSG_##t)

/*!
 * Convenience macros for determining the size of a command or message:
 */
#define RM_SOE_CMD_SIZE(u,t)  \
    (RM_FLCN_QUEUE_HDR_SIZE + RM_SOE_CMD_BODY_SIZE(u,t))

#define RM_SOE_MSG_SIZE(u,t)  \
    (RM_FLCN_QUEUE_HDR_SIZE + RM_SOE_MSG_BODY_SIZE(u,t))

/*!
 * Convenience macros for determining the type of a command or message
 * (intended to be used symmetrically with the CMD and MSG _SIZE macros):
 */
#define RM_SOE_CMD_TYPE(u,t)  RM_SOE_##u##_CMD_ID_##t
#define RM_SOE_MSG_TYPE(u,t)  RM_SOE_##u##_MSG_ID_##t

/*!
 * Define the maximum number of command sequences that can be in flight at
 * any given time.  This is dictated by the width of the sequence number
 * id ('seqNumId') stored in each sequence packet (currently 8-bits).
 */
#define RM_SOE_MAX_NUM_SEQUENCES                 256

/*!
 * Compares an unit id against the values in the unit_id enumeration and
 * verifies that the id is valid.  It is expected that the id is specified
 * as an unsigned integer.
 */
#define  RM_SOE_UNITID_IS_VALID(id)    (((id) <= RM_SOE_UNIT_END))

/*!
 * Defines the size of the surface/buffer that will be allocated to store
 * debug spew from the SOE ucode application when falcon-trace is enabled.
 */
#define RM_SOE_DEBUG_SURFACE_SIZE                               (32*1024)

/*!
 * SOE's frame-buffer interface block has several slots/indices which can
 * be bound to support DMA to various surfaces in memory. This is an
 * enumeration that gives name to each index based on type of memory-aperture
 * the index is used to access.
 *
 * LR10 IAS mentions SOE supporting only SYSMEM coherent transfers, but non-coherent
 * SYSMEM trasnfers have been seen to work in emulation. Hence, adding non-coherent
 * aperture as well for now.
 *
 */

#define SOE_DMAIDX_PHYS_SYS_COH_FN0         0
#define SOE_DMAIDX_PHYS_SYS_NCOH_FN0        1

// TODO: Find a way to share this enum with NVSwitch Driver too
/*!
 * Defines a generic type that may be used to convey status information.
 */
typedef enum _SOE_STATUS
{
    // Let's not use the default value as any status/error
    SOE_UNINITIALIZED                                                   =     (0x00000000),
    SOE_OK                                                              =     (0x00000001),
    SOE_BOOT_BINARY_STARTED_NOT_IN_HS_YET                               =     (0x00000002),
    SOE_BOOT_BINARY_STARTED_IN_HS                                       =     (0x00000003),
    SOE_BOOT_BINARY_LS_VERIFIED                                         =     (0x00000004),
    SOE_BOOT_BINARY_LS_ENTRY                                            =     (0x00000005),
    SOE_RTOS_STARTED                                                    =     (0x00000006),
    SOE_BOOTSTRAP_SUCCESS                                               =     (0x00000007),


    // Reserve some constants for future
    SOE_ERROR_UNKNOWN                                                   =     (0x0000000A),
    SOE_ERROR_UNREACHABLE_CODE                                          =     (0x0000000B),
    SOE_ERROR_HS_FATAL_ALIGN_FAIL1                                      =     (0x0000000C),
    SOE_ERROR_HS_FATAL_ALIGN_FAIL2                                      =     (0x0000000D),
    SOE_ERROR_HS_FATAL_ALIGN_FAIL3                                      =     (0x0000000E),
    SOE_ERROR_HS_FATAL_ALIGN_FAIL4                                      =     (0x0000000F),
    SOE_ERROR_HS_FATAL_SIGFAIL_IMEM                                     =     (0x00000010),
    SOE_ERROR_HS_FATAL_SIGFAIL_DMEM                                     =     (0x00000011),
    SOE_ERROR_HS_FATAL_REV_HS_MISMATCH                                  =     (0x00000012),
    SOE_ERROR_HS_FATAL_FALCON_EXCEPTION_HS                              =     (0x00000013),
    SOE_ERROR_HS_RAND_GEN_FAILED                                        =     (0x00000014),
    SOE_ERROR_LS_BOOT_FAIL_AUTH                                         =     (0x00000015),
    SOE_ERROR_HS_STACK_CHECK_FAILED                                     =     (0x00000016),
    SOE_ERROR_LS_STACK_CHECK_SETUP_FAILED                               =     (0x00000017),
    SOE_ERROR_LS_STACK_CHECK_FAILED                                     =     (0x00000018),
    SOE_ERROR_LS_FATAL_DISCOVERY_FAILED                                 =     (0x00000019),

} SOE_STATUS;

// MAILBOX1 is utilized because in HS mode, priv lockdown is in effect and access to other registers may not be allowed
#define SOE_STATUS_SCRATCH_REGISTER  NV_CSOE_FALCON_MAILBOX1

//
// SOE Timeout checking
//
#define SOE_INTERVAL_1USEC_IN_NS     1000LL
#define SOE_INTERVAL_50USEC_IN_NS    50000LL
#define SOE_INTERVAL_1MSEC_IN_NS     1000000LL
#define SOE_INTERVAL_5MSEC_IN_NS     5000000LL
#define SOE_INTERVAL_1SEC_IN_NS      1000000000LL

//
// SOE DMA min and max transfer sizes
//
#define SOE_DMA_MAX_SIZE            256
#define SOE_DMA_MIN_SIZE            4

/*!
 * SOE's frame-buffer interface block has several slots/indices which can
 * be bound to support DMA to various surfaces in memory. This is an
 * enumeration that gives name to each index based on type of memory-aperture
 * the index is used to access.
 *
 * Pre-Turing, RM_SOE_DMAIDX_PHYS_VID_FN0 == RM_SOE_DMAIDX_GUEST_PHYS_VID_BOUND.
 * From Turing, engine context is stored in GPA, requiring a separate aperture.
 *
 * Traditionally, video falcons have used the 6th index for ucode, and we will
 * continue to use that to allow legacy ucode to work seamlessly.
 *
 * Note: DO NOT CHANGE THE VALUE OF RM_SOE_DMAIDX_UCODE. That value is used by
 * both the legacy SOE ucode, which assumes that it will use index 6, and by
 * SOE RTOS. Changing it will break legacy SOE ucode, unless it is updated to
 * reflect the new value.
 */
enum
{
    RM_SOE_DMAIDX_GUEST_PHYS_VID_BOUND      = 0,
    RM_SOE_DMAIDX_VIRT                      = 1,
    RM_SOE_DMAIDX_PHYS_VID_FN0              = 2,
    RM_SOE_DMAIDX_PHYS_SYS_COH_FN0          = 3,
    RM_SOE_DMAIDX_PHYS_SYS_NCOH_FN0         = 4,
    RM_SOE_DMAIDX_GUEST_PHYS_SYS_COH_BOUND  = 5,
    RM_SOE_DMAIDX_UCODE                     = 6,
    RM_SOE_DMAIDX_GUEST_PHYS_SYS_NCOH_BOUND = 7
};

/*!
 * SOE Debug buffer size
 * Creating a copy of SOE_DMESG_BUFFER_SIZE in memmap.h
 * soe/memmap.h is conflicting with sec2/memmap.h and cannot be used in the driver
 */
#define SOE_DEBUG_BUFFER_SIZE 0x1000

#endif  // _GSOEIFCMN_H_
