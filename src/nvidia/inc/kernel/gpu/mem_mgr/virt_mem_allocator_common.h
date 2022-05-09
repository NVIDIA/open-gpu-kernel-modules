/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef VIRT_MEM_ALLOCATOR_COMMON_H
#define VIRT_MEM_ALLOCATOR_COMMON_H

/********************************* DMA Manager *****************************\
*                                                                           *
*       DMA object/engine management.                                       *
*                                                                           *
****************************************************************************/

#include "nvtypes.h"
#include "nvgputypes.h"
#include "nvstatus.h"

typedef struct OBJGPU OBJGPU;
typedef struct ChannelDescendant ChannelDescendant;
typedef struct ContextDma ContextDma;
typedef struct Memory Memory;
typedef struct EVENTNOTIFICATION EVENTNOTIFICATION;
//---------------------------------------------------------------------------
//
//  Memory page defines.
//
//  These correspond to the granularity understood by the hardware
//  for address mapping; the system page size can be larger.
//
//---------------------------------------------------------------------------
#define RM_PAGE_SIZE        4096
#define RM_PAGE_SIZE_64K    (64 * 1024)
#define RM_PAGE_SIZE_128K   (128 * 1024)
#define RM_PAGE_MASK        0x0FFF
#define RM_PAGE_SHIFT       12
#define RM_PAGE_SHIFT_64K   16

// Huge page size is 2 MB
#define RM_PAGE_SHIFT_HUGE 21
#define RM_PAGE_SIZE_HUGE  (1 << RM_PAGE_SHIFT_HUGE)
#define RM_PAGE_MASK_HUGE  ((1 << RM_PAGE_SHIFT_HUGE) - 1)

// 512MB page size
#define RM_PAGE_SHIFT_512M 29
#define RM_PAGE_SIZE_512M  (1 << RM_PAGE_SHIFT_512M)
#define RM_PAGE_MASK_512M  (RM_PAGE_SIZE_512M - 1)

//---------------------------------------------------------------------------
//
//  Memory page attributes.
//
//  These attributes are used by software for page size mapping;
//  Big pages can be of 64/128KB[Fermi/Kepler/Pascal]
//  Huge page is 2 MB[Pascal+]
//  512MB page is Ampere+
//  Default page attribute lets driver decide the optimal page size
//
//---------------------------------------------------------------------------
typedef enum
{
    RM_ATTR_PAGE_SIZE_DEFAULT = 0x0,
    RM_ATTR_PAGE_SIZE_4KB     = 0x1,
    RM_ATTR_PAGE_SIZE_BIG     = 0x2,
    RM_ATTR_PAGE_SIZE_HUGE    = 0x3,
    RM_ATTR_PAGE_SIZE_512MB   = 0x4,
    RM_ATTR_PAGE_SIZE_INVALID = 0x5
}
RM_ATTR_PAGE_SIZE;

//---------------------------------------------------------------------------
//
//  Notification buffer structure.
//
//---------------------------------------------------------------------------
typedef union _def_info_status_buffer
{
    struct
    {
        NvV16 OtherInfo16;
        NvV16 Status;
    } Info16Status_16;

    NvU32 Info16Status_32;

} INFO16_STATUS;

typedef struct _def_notification_buffer
{
    NvU32 TimeLo;
    NvU32 TimeHi;
    NvV32 OtherInfo32;
    INFO16_STATUS Info16Status;
} NOTIFICATION, *PNOTIFICATION;


//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------
void      notifyMethodComplete(OBJGPU*, ChannelDescendant *, NvU32, NvV32, NvU32, NvU16, NV_STATUS);

NV_STATUS notifyFillNotifier               (OBJGPU*, ContextDma *, NvV32, NvV16, NV_STATUS);
NV_STATUS notifyFillNotifierOffset         (OBJGPU*, ContextDma *, NvV32, NvV16, NV_STATUS, NvU64);
NV_STATUS notifyFillNotifierOffsetTimestamp(OBJGPU*, ContextDma *, NvV32, NvV16, NV_STATUS, NvU64, NvU64);
NV_STATUS notifyFillNotifierArray          (OBJGPU*, ContextDma *, NvV32, NvV16, NV_STATUS, NvU32);
NV_STATUS notifyFillNotifierArrayTimestamp (OBJGPU*, ContextDma *, NvV32, NvV16, NV_STATUS, NvU32, NvU64);
void notifyFillNOTIFICATION(OBJGPU       *pGpu,
                            NOTIFICATION *pNotifyBuffer,
                            NvV32         Info32,
                            NvV16         Info16,
                            NV_STATUS     CompletionStatus,
                            NvBool        TimeSupplied,
                            NvU64         Time);
NV_STATUS notifyFillNotifierGPUVA          (OBJGPU*, NvHandle, NvHandle, NvU64, NvV32, NvV16, NV_STATUS, NvU32);
NV_STATUS notifyFillNotifierGPUVATimestamp (OBJGPU*, NvHandle, NvHandle, NvU64, NvV32, NvV16, NV_STATUS, NvU32, NvU64);
NV_STATUS notifyFillNotifierMemory         (OBJGPU*, Memory *, NvV32, NvV16, NV_STATUS, NvU32);
NV_STATUS notifyFillNotifierMemoryTimestamp(OBJGPU*, Memory *, NvV32, NvV16, NV_STATUS, NvU32, NvU64);
void notifyFillNvNotification(OBJGPU         *pGpu,
                              NvNotification *pNotification,
                              NvV32           Info32,
                              NvV16           Info16,
                              NV_STATUS       CompletionStatus,
                              NvBool          TimeSupplied,
                              NvU64           Time);

NV_STATUS semaphoreFillGPUVA         (OBJGPU*, NvHandle, NvHandle, NvU64, NvV32, NvV32, NvBool);
NV_STATUS semaphoreFillGPUVATimestamp(OBJGPU*, NvHandle, NvHandle, NvU64, NvV32, NvV32, NvBool, NvU64);

RM_ATTR_PAGE_SIZE dmaNvos32ToPageSizeAttr(NvU32 attr, NvU32 attr2);

#endif // VIRT_MEM_ALLOCATOR_COMMON_H
