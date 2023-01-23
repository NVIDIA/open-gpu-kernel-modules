/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NVIDIA_PUSH_PRIV_H__
#define __NVIDIA_PUSH_PRIV_H__

#include "nvmisc.h" // NV_ALIGN_UP
#include "class/cla16f.h" // NVA16F_GP_ENTRY__SIZE

/*
 * Push buffer constants
 * "The pushbuffer" consists of several regions packed into a single memory
 * allocation.  In order, they are:
 * 1. The "main" pushbuffer.  Most of the driver pushes methods here;
 * 2. GPFIFO entries;
 * 3. The "progress tracker" pushbuffer.  This is used by the DMA kickoff code
 *    as a reserved area to put semaphore release methods, which we use to
 *    track HOST's progress fetching the pushbuffer.  We also use this to
 *    workaround hardware bug 1667921.
 */

/* Offset of the GPFIFO entries: entry (2) above. */
static inline NvU32 __nvPushGpFifoOffset(const NvPushChannelRec *pChannel)
{
    nvAssert(pChannel->main.sizeInBytes != 0);
    return NV_ALIGN_UP(pChannel->main.sizeInBytes, NVA16F_GP_ENTRY__SIZE);
}

/*
 * We need to align each set of methods in the progress tracker pushbuffer to
 * 128 bytes so that we avoid HW bug 1667921 (on chips that are affected).
 * This is used for both the start of the GPFIFO segment _and_ the size (for
 * each GPFIFO entry).
 */
#define NV_ALIGN_LBDAT_EXTRA_BUG         128
/*
 * Offset of the progress tracker pushbuffer: entry (3) above.
 *
 * Note that we always use the appropriate alignment to WAR the LBDAT_EXTRA bug
 * for the offset.  Although this is only necessary on some chips, it's simpler
 * to always use this alignment.
 */
static inline NvU32 __nvPushProgressTrackerOffset(
    const NvPushChannelRec *pChannel)
{
    const NvU32 gpFifoOffset = __nvPushGpFifoOffset(pChannel);
    const NvU32 gpFifoLength =
        pChannel->numGpFifoEntries * NVA16F_GP_ENTRY__SIZE;

    nvAssert(gpFifoLength != 0);

    return NV_ALIGN_UP(gpFifoOffset + gpFifoLength, NV_ALIGN_LBDAT_EXTRA_BUG);
}

/* We always write two GPFIFO entries: one for the main pushbuffer, and one
 * for the progress tracker pushbuffer. */
#define NV_PUSH_NUM_GPFIFO_ENTRIES_PER_KICKOFF 2

/*
 * Encoding for the progress tracker semaphore payload.
 * _GET stores dwords, rather than bytes.
 * _GP_GET stores the number of "pairs" of gpFifo entries.
 */
#define NV_PUSH_PROGRESS_TRACKER_SEMAPHORE_GET      17:0
#define NV_PUSH_PROGRESS_TRACKER_SEMAPHORE_GP_GET   31:18

/*
 * The number of 0080 RM devices for the given NvPushDevice.
 * This is 1 for RM SLI and numSubDevices for client SLI.
 */
static inline int
__nvPushGetNumDevices(const NvPushDeviceRec *pDevice)
{
    if (pDevice->clientSli) {
        return pDevice->numSubDevices;
    }
    return 1;
}

/*
 * The 0080 RM device index for the given subdevice index.
 * This is 0 for RM SLI, and the subdevice index for client SLI.
 */
static inline int
__nvPushGetDeviceIndex(const NvPushDeviceRec *pDevice, int sd)
{
    if (pDevice->clientSli) {
        return sd;
    }
    return 0;
}

NvU32 __nvPushProgressTrackerEntrySize(const NvPushDeviceRec *pDevice);

NvBool __nvPushTestPushBuffer(NvPushChannelPtr p);

NvBool __nvPushGetHal(
    const NvPushAllocDeviceParams *pParams,
    NvU32 channelClass,
    NvPushHal *pHal);

#endif /* __NVIDIA_PUSH_PRIV_H__ */
