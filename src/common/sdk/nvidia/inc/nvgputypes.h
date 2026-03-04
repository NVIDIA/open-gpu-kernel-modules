/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2006 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


 /***************************************************************************\
|*                                                                           *|
|*                         NV GPU Types                                      *|
|*                                                                           *|
|*  This header contains definitions describing NVIDIA's GPU hardware state. *|
|*                                                                           *|
 \***************************************************************************/


#ifndef NVGPUTYPES_INCLUDED
#define NVGPUTYPES_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

 /***************************************************************************\
|*                              NvNotification                               *|
 \***************************************************************************/

/***** NvNotification Structure *****/
/*
 * NV objects return information about method completion to clients via an
 * array of notification structures in main memory.
 *
 * The client sets the status field to NV???_NOTIFICATION_STATUS_IN_PROGRESS.
 * NV fills in the NvNotification[] data structure in the following order:
 * timeStamp, otherInfo32, otherInfo16, and then status.
 */

/* memory data structures */
typedef volatile struct NvNotificationRec {
 struct {                      /*                                   0000-    */
  NvU32 nanoseconds[2];        /* nanoseconds since Jan. 1, 1970       0-   7*/
 } timeStamp;                  /*                                       -0007*/
 NvV32 info32;                 /* info returned depends on method   0008-000b*/
 NvV16 info16;                 /* info returned depends on method   000c-000d*/
 NvV16 status;                 /* user sets bit 15, NV sets status  000e-000f*/
} NvNotification;

 /***************************************************************************\
|*                              NvGpuSemaphore                               *|
 \***************************************************************************/

/***** NvGpuSemaphore Structure *****/
/*
 * NvGpuSemaphore objects are used by the GPU to synchronize multiple
 * command-streams.
 *
 * Please refer to class documentation for details regarding the content of
 * the data[] field.
 */

/* memory data structures */
typedef volatile struct NvGpuSemaphoreRec {
 NvV32 data[2];                /* Payload/Report data               0000-0007*/
 struct {                      /*                                   0008-    */
  NvV32 nanoseconds[2];        /* nanoseconds since Jan. 1, 1970       8-   f*/
 } timeStamp;                  /*                                       -000f*/
} NvGpuSemaphore;

 /***************************************************************************\
|*                            NvGetReport                                    *|
 \***************************************************************************/

/*
 * NV objects, starting with Kelvin, return information such as pixel counts to
 * the user via the NV*_GET_REPORT method.
 *
 * The client fills in the "zero" field to any nonzero value and waits until it
 * becomes zero.  NV fills in the timeStamp, value, and zero fields.
 */
typedef volatile struct NVGetReportRec {
    struct  {                  /*                                   0000-    */
        NvU32 nanoseconds[2];  /* nanoseconds since Jan. 1, 1970       0-   7*/
    } timeStamp;               /*                                       -0007*/
    NvU32 value;               /* info returned depends on method   0008-000b*/
    NvU32 zero;                /* always written to zero            000c-000f*/
} NvGetReport;

 /***************************************************************************\
|*                           NvRcNotification                                *|
 \***************************************************************************/

/*
 * NV robust channel notification information is reported to clients via
 * standard NV01_EVENT objects bound to instance of the NV*_CHANNEL_DMA and
 * NV*_CHANNEL_GPFIFO objects.
 */
typedef struct NvRcNotificationRec {
    struct {
        NvU32 nanoseconds[2];  /* nanoseconds since Jan. 1, 1970       0-   7*/
    } timeStamp;               /*                                       -0007*/
    NvU32 exceptLevel;         /* exception level                   000c-000f*/
    NvU32 exceptType;          /* exception type                    0010-0013*/
} NvRcNotification;

 /***************************************************************************\
|*                              NvSyncPointFence                             *|
 \***************************************************************************/

/***** NvSyncPointFence Structure *****/
/*
 * NvSyncPointFence objects represent a syncpoint event.  The syncPointID
 * identifies the syncpoint register and the value is the value that the
 * register will contain right after the event occurs.
 *
 * If syncPointID contains NV_INVALID_SYNCPOINT_ID then this is an invalid
 * event.  This is often used to indicate an event in the past (i.e. no need to
 * wait).
 *
 * For more info on syncpoints refer to Mobile channel and syncpoint
 * documentation.
 */
typedef struct NvSyncPointFenceRec {
    NvU32   syncPointID;
    NvU32   value;
} NvSyncPointFence;

#define NV_INVALID_SYNCPOINT_ID ((NvU32)-1)

 /***************************************************************************\
|*                                                                           *|
|*  64 bit type definitions for use in interface structures.                 *|
|*                                                                           *|
 \***************************************************************************/

typedef NvU64           NvOffset; /* GPU address                             */

#define NvOffset_HI32(n)  ((NvU32)(((NvU64)(n)) >> 32))
#define NvOffset_LO32(n)  ((NvU32)((NvU64)(n)))

/*
* There are two types of GPU-UUIDs available:
*
*  (1) a SHA-256 based 32 byte ID, formatted as a 64 character
*      hexadecimal string as "GPU-%16x-%08x-%08x-%08x-%024x"; this is
*      deprecated.
*
*  (2) a SHA-1 based 16 byte ID, formatted as a 32 character
*      hexadecimal string as "GPU-%08x-%04x-%04x-%04x-%012x" (the
*      canonical format of a UUID); this is the default.
*/
#define NV_GPU_UUID_SHA1_LEN            (16)
#define NV_GPU_UUID_SHA256_LEN          (32)
#define NV_GPU_UUID_LEN                 NV_GPU_UUID_SHA1_LEN

#ifdef __cplusplus
};
#endif

#endif /* NVGPUTYPES_INCLUDED */
