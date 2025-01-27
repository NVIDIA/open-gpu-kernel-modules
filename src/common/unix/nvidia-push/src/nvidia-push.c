/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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




#include "nvidia-push-init.h"
#include "nvidia-push-methods.h"
#include "nvidia-push-utils.h"
#include "nvidia-push-priv.h"
#include "nvidia-push-priv-imports.h"

#include "nvos.h"
#include "nv_assert.h"
#include "nvSemaphoreCommon.h"

#include "class/cl2080.h" // NV2080_SUBDEVICE_NOTIFICATION_STATUS_DONE_SUCCESS

#include "class/cla16f.h" // KEPLER_CHANNEL_GPFIFO_B
#include "class/cla26f.h" // KEPLER_CHANNEL_GPFIFO_C
#include "class/clb06f.h" // MAXWELL_CHANNEL_GPFIFO_A
#include "class/clc06f.h" // PASCAL_CHANNEL_GPFIFO_A
#include "class/clc36f.h" // VOLTA_CHANNEL_GPFIFO_A
#include "class/clc46f.h" // TURING_CHANNEL_GPFIFO_A
#include "class/clc56f.h" // AMPERE_CHANNEL_GPFIFO_A
#include "class/clc86f.h" // HOPPER_CHANNEL_GPFIFO_A
#include "class/clc361.h" // VOLTA_USERMODE_A
#include "class/clc96f.h" // BLACKWELL_CHANNEL_GPFIFO_A
#include "class/clca6f.h" // BLACKWELL_CHANNEL_GPFIFO_B
#include "ctrl/ctrl906f.h" // NV906F_CTRL_GET_CLASS_ENGINEID

/*
 * This low-level macro pushes SetSubDevice methods.  Most of the driver should
 * use the higher-level NV_DMA_SET_SUBDEVICE_MASK macro.
 */
#define SetSubDeviceMask(_push_buffer, _segment, _mask)                 \
    do {                                                                \
        ASSERT_DRF_NUM(A16F, _DMA, _SET_SUBDEVICE_MASK_VALUE, _mask);   \
        nvPushHeader(_push_buffer, _segment, 0,                         \
            DRF_DEF(A16F, _DMA, _SEC_OP, _GRP0_USE_TERT) |              \
            DRF_DEF(A16F, _DMA, _TERT_OP, _GRP0_SET_SUB_DEV_MASK) |     \
            DRF_NUM(A16F, _DMA, _SET_SUBDEVICE_MASK_VALUE, _mask));     \
    } while(0)

/*!
 * Check for a channel error on pChannel.
 *
 * Check the channel's error notifier to determine if a channel error
 * occurred.  If an error occurred, call the host driver's
 * nvPushImportChannelErrorOccurred() implementation, which may take
 * steps to recover from the error.
 *
 * \param[in]  pChannel  The GPU channel.
 *
 * \return  Return TRUE if a channel error occurred, FALSE otherwise.
 */
NvBool nvPushCheckChannelError(NvPushChannelPtr pChannel)
{
    unsigned short status;
    NvNotification *pNotifier;

    if (pChannel->ignoreChannelErrors) {
        return FALSE;
    }

    pNotifier =
        nvPushGetNotifierCpuAddress(pChannel, NV_PUSH_ERROR_NOTIFIER_INDEX, 0);

    status = pNotifier->status;

    if (status == 0xFFFF) {
        nvPushImportChannelErrorOccurred(pChannel, pNotifier->info32);
        pChannel->channelErrorOccurred = TRUE;
        return TRUE;
    }

    return FALSE;
}


/*
 * This function dumps the pushbuffer.  The output format is compatible with
 * nvdiss.
 */
static void DumpPB(NvPushChannelPtr pChannel)
{
#if defined(DEBUG)
    const NvPushChannelSegmentRec *segment = &pChannel->main;
    const int columns = 8;
    const NvPushChannelUnion *base = segment->base;
    const NvPushChannelUnion *b =
        (NvPushChannelUnion *)((char *)segment->base + segment->putOffset);
    const NvPushChannelUnion *end = segment->buffer;
    NvU32 i;

    if (!pChannel->logNvDiss) {
        return;
    }

    nvPushImportLogNvDiss(pChannel,
       "***************** Push Buffer Contents: ********************\n");

    for (i = 0; b && b != end; b++, i++) {
        if ((i % columns) == 0) {
            nvPushImportLogNvDiss(pChannel, "%06" NvUPtr_fmtx ":",
                                  (NvUPtr)((b - base) * 4));
        }

        nvPushImportLogNvDiss(pChannel, " %08X", b->u);

        if ((i % columns) == (columns - 1) || (b + 1) == end) {
            nvPushImportLogNvDiss(pChannel, "\n");
        }
    }

    nvPushImportLogNvDiss(pChannel,
        "***************** End Push Buffer Contents: ****************\n");
    nvPushImportLogNvDiss(pChannel,
        "************************************************************\n");
#endif /* DEBUG */
}

static NvU32 ReadProgressTrackerSemaphore(NvPushChannelPtr p, int sd)
{
    volatile NvU32 *ptr = (volatile NvU32 *)p->progressSemaphore.ptr[sd];
    return *ptr;
}

static NvU32 GpFifoReadGet(NvPushChannelPtr p, int sd)
{
    const NvU32 current = ReadProgressTrackerSemaphore(p, sd);
    const NvU32 getDwords =
        DRF_VAL(_PUSH_PROGRESS_TRACKER, _SEMAPHORE, _GET, current);
    return getDwords * 4; /* return get in bytes */
}

static NvU32 GpFifoReadGpGet(NvPushChannelPtr p, int sd)
{
    const NvU32 current = ReadProgressTrackerSemaphore(p, sd);
    const NvU32 gpGetPairs =
        DRF_VAL(_PUSH_PROGRESS_TRACKER, _SEMAPHORE, _GP_GET, current);
    /* return gpGet in gpFifo indices, not pairs of indices */
    return gpGetPairs * 2;
}

/* Read GPGET for all devices and return the minimum */
static NvU32 ReadGpGetOffset(NvPushChannelPtr push_buffer)
{
    unsigned int i;
    NvU32 bestGet = 0;
    NvS32 distanceToPut, maxDistanceToPut = 0;
    const NvPushDeviceRec *pDevice = push_buffer->pDevice;

    if (pDevice->numSubDevices <= 1) {
        bestGet = GpFifoReadGpGet(push_buffer, 0);
    } else {
        for (i = 0; i < pDevice->numSubDevices; i++) {
            NvU32 get = GpFifoReadGpGet(push_buffer, i);

            /* Compute distance to put, accounting for wraps */
            distanceToPut = push_buffer->gpPutOffset - get;
            if (distanceToPut < 0) {
                distanceToPut += push_buffer->numGpFifoEntries;
            }

            nvAssert(distanceToPut >= 0);

            /* Track the maximum distance to put and the corresponding get. */
            if (distanceToPut >= maxDistanceToPut) {
                maxDistanceToPut = distanceToPut;
                bestGet = get;
            }
        }
    }

    /* We should never see an odd index, since we always kick off two entries
     * at a time. */
    nvAssert((bestGet & 1) == 0);

    return bestGet;
}

static void FillGpEntry(NvPushChannelSegmentPtr segment, NvU32 putOffset,
                        NvU32 *gpEntry0, NvU32 *gpEntry1)
{
    const NvU32 length = putOffset - segment->putOffset;
    const NvU64 base = segment->gpuMapOffset + segment->putOffset;
    *gpEntry0 =
        DRF_NUM(A16F, _GP_ENTRY0, _GET, NvU64_LO32(base) >> 2);
    *gpEntry1 =
        DRF_NUM(A16F, _GP_ENTRY1, _GET_HI, NvU64_HI32(base)) |
        DRF_NUM(A16F, _GP_ENTRY1, _LENGTH, length >> 2);

    ASSERT_DRF_NUM(A16F, _GP_ENTRY1, _LENGTH, length >> 2);
    nvAssert(segment->putOffset + length <= segment->sizeInBytes);
    nvAssert(!(segment->putOffset & 0x3));
    nvAssert(!(length & 0x3));
    nvAssert(length != 0);
}

/*!
 * Calculate the amount of space that we need for each kickoff in the progress
 * tracker pushbuffer.
 */
NvU32 __nvPushProgressTrackerEntrySize(const NvPushDeviceRec *pDevice)
{
    NvU32 dwords, size;

    /*
     * The minimum number of methods we need is 4 for pre-Volta (SemaphoreA-D),
     * and 5 for Volta+ (SemAddrHi/Lo, SemPayloadHi/Lo, SemExecute).
     * The pushbuffer encoding consists of one 32-bit header plus one 32-bit
     * data entry for each method.
     */
    dwords = 1;

    if (pDevice->hal.caps.voltaSemMethods) {
        dwords += 5;
    } else {
        dwords += 4;
    }

    /*
     * If SLI is enabled, we may need to ensure that the semaphore is written
     * on all subdevices.  This requires two extra dwords: SET_SUBDEVICE_MASK
     * to a broadcast state, and a second SET_SUBDEVICE_MASK to restore the old
     * state.
     */
    if (pDevice->numSubDevices > 1) {
        dwords += 2;
    }

    size = dwords * sizeof(NvU32);

    /*
     * If the GPU is affected by hardware bug 1667921, then we pad this out to
     * NV_ALIGN_LBDAT_EXTRA_BUG.  (The workaround requires that each entry be a
     * multiple of this size.)
     */
    nvAssert(size <= NV_ALIGN_LBDAT_EXTRA_BUG);
    return pDevice->hostLBoverflowBug1667921 ? NV_ALIGN_LBDAT_EXTRA_BUG : size;
}

/*!
 * This function emits methods into the progress tracker pushbuffer region that
 * will cause host to release a semaphore with payload 'putOffset', then writes
 * a GPFIFO entry to 'gpPointer' that will make host fetch those methods.
 *
 * Normally, for progress tracking purposes, we only need to know if
 * host has read the method, in which case _RELEASE_WFI=DIS is sufficient
 * below.  But, when we need to know if the channel is really idle, host
 * must send WAIT_FOR_IDLE to the downstream engine (_RELEASE_WFI=EN).
 * Use 'progressTrackerWFI' to control this behavior.
 */
static void InsertProgressTracker(NvPushChannelPtr p, NvU32 putOffset,
                                  NvU32 gpPutOffset, NvU32 *gpPointer,
                                  NvBool progressTrackerWFI)
{
    const NvPushDeviceRec *pDevice = p->pDevice;
    NvPushChannelSegmentPtr segment = &p->progressTracker;

    /*
     * The progress tracker pushbuffer segment is sized such that we have
     * just enough space to write the methods we need to for every (two)
     * entries in the GPFIFO.
     *
     * We directly calculate the location to begin writing methods from the
     * size of a single entry and gpPutOffset (divided by two because we only
     * write one progress tracker entry for every two GPFIFO entries).
     */
    const NvU32 entrySize = __nvPushProgressTrackerEntrySize(pDevice);
    const NvU32 entry = gpPutOffset >> 1;
    const NvU32 entryOffset = entrySize * entry;
    const NvU32 putOffsetDwords = putOffset / 4;
    const NvU32 payload =
        DRF_NUM(_PUSH_PROGRESS_TRACKER, _SEMAPHORE, _GET, putOffsetDwords) |
        DRF_NUM(_PUSH_PROGRESS_TRACKER, _SEMAPHORE, _GP_GET, gpPutOffset / 2);

    NvU32 restoreSDM = NV_PUSH_SUBDEVICE_MASK_ALL;

    /*
     * PROGRESS_TRACKER_SEMAPHORE_GET above stores dwords, so
     * putOffset had better be a multiple of four.
     */
    nvAssert((putOffset % 4) == 0);

    /*
     * PROGRESS_TRACKER_SEMAPHORE_GP_GET above stores pairs of gpFifo
     * entries, so gpPutOffset had better be even.
     */
    nvAssert((gpPutOffset % 2) == 0);

    segment->freeDwords = entrySize / sizeof(NvU32);
    segment->putOffset = entryOffset;
    segment->buffer = (NvPushChannelUnion *)((char *)segment->base + entryOffset);

    /*
     * __nvPushProgressTrackerEntrySize() contains a calculation of how many
     * methods we need.  This must be kept up-to-date with the actual methods
     * pushed below.
     */
    if (!nvPushSubDeviceMaskEquiv(pDevice, p->currentSubDevMask,
                                  NV_PUSH_SUBDEVICE_MASK_ALL)) {
        restoreSDM = p->currentSubDevMask;
        SetSubDeviceMask(p, progressTracker, NV_PUSH_SUBDEVICE_MASK_ALL);
    }
    if (pDevice->hal.caps.voltaSemMethods) {
        const NvU32 semaphoreOperation =
            DRF_DEF(C36F, _SEM_EXECUTE, _OPERATION, _RELEASE) |
            DRF_DEF(C36F, _SEM_EXECUTE, _PAYLOAD_SIZE, _32BIT) |
            DRF_DEF(C36F, _SEM_EXECUTE, _RELEASE_TIMESTAMP, _DIS) |
            (progressTrackerWFI ?
             DRF_DEF(C36F, _SEM_EXECUTE, _RELEASE_WFI, _EN) :
             DRF_DEF(C36F, _SEM_EXECUTE, _RELEASE_WFI, _DIS));

        __nvPushStart(p, progressTracker, 0, NVC36F_SEM_ADDR_LO, 5, _INC_METHOD);
        __nvPushSetMethodDataSegmentU64LE(segment, p->progressSemaphore.gpuVA);
        __nvPushSetMethodDataSegment(segment, payload);
        __nvPushSetMethodDataSegment(segment, 0);
        __nvPushSetMethodDataSegment(segment, semaphoreOperation);
    } else {
        const NvU32 semaphoreOperation =
            DRF_DEF(A16F, _SEMAPHORED, _OPERATION, _RELEASE) |
            DRF_DEF(A16F, _SEMAPHORED, _RELEASE_SIZE, _4BYTE) |
            (progressTrackerWFI ?
             DRF_DEF(A16F, _SEMAPHORED, _RELEASE_WFI, _EN) :
             DRF_DEF(A16F, _SEMAPHORED, _RELEASE_WFI, _DIS));

        __nvPushStart(p, progressTracker, 0, NVA16F_SEMAPHOREA, 4, _INC_METHOD);
        __nvPushSetMethodDataSegmentU64(segment, p->progressSemaphore.gpuVA);
        __nvPushSetMethodDataSegment(segment, payload);
        __nvPushSetMethodDataSegment(segment, semaphoreOperation);
    }

    if (restoreSDM != NV_PUSH_SUBDEVICE_MASK_ALL) {
        SetSubDeviceMask(p, progressTracker, restoreSDM);
    }

    if (pDevice->hostLBoverflowBug1667921) {
        /* The workaround for bug 1667921 dictates that we must kick off a
         * GPFIFO segment of an exact size.  Pad out with NOPs. */
        while (segment->freeDwords) {
            nvPushImmedValSegment(p, progressTracker, 0, NVA16F_NOP, 0);
        }
    }

    FillGpEntry(segment,
                (NvU32)((char *)segment->buffer - (char *)segment->base),
                &gpPointer[0], &gpPointer[1]);
}

#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
#define NV_CPU_MEMFENCE()   __asm__ __volatile__ ("sfence" : : : "memory")
#elif NVCPU_IS_FAMILY_ARM
#define NV_CPU_MEMFENCE()   __asm__ __volatile__ ("dsb sy\n\t" : : : "memory");
#elif NVCPU_IS_PPC64LE
#define NV_CPU_MEMFENCE()   __asm__ __volatile__ ("lwsync\n\t" : : : "memory")
#else
#define NV_CPU_MEMFENCE()   /* nothing */
#endif

static NvBool nvWriteGpEntry(
    NvPushChannelPtr push_buffer,
    NvU32 putOffset,
    NvBool progressTrackerWFI)
{
    NvU32 gpEntry0, gpEntry1;

    NvU32 nextGpPut;
    NvU32 *gpPointer;
    const NvU32 entriesNeeded = NV_PUSH_NUM_GPFIFO_ENTRIES_PER_KICKOFF;
    NvPushDevicePtr pDevice = push_buffer->pDevice;

    FillGpEntry(&push_buffer->main, putOffset, &gpEntry0, &gpEntry1);

    nextGpPut = (push_buffer->gpPutOffset + entriesNeeded) &
                (push_buffer->numGpFifoEntries - 1);
    gpPointer = &(push_buffer->gpfifo[push_buffer->gpPutOffset*2]);

    nvAssert((nextGpPut % 2) == 0);

    // Wait for a free entry in the buffer
    while (nextGpPut == ReadGpGetOffset(push_buffer)) {
        if (nvPushCheckChannelError(push_buffer)) {
            nvAssert(!"A channel error occurred in nvWriteGpEntry()");
            return FALSE;
        }
    }
    gpPointer[0] = gpEntry0;
    gpPointer[1] = gpEntry1;
    gpPointer += 2;

    InsertProgressTracker(push_buffer, putOffset,
                          push_buffer->gpPutOffset, gpPointer,
                          progressTrackerWFI);

    /* Make sure all CPU writes to writecombined memory get flushed */
    NV_CPU_MEMFENCE();

    pDevice->hal.kickoff(push_buffer, push_buffer->gpPutOffset, nextGpPut);

    push_buffer->gpPutOffset = nextGpPut;

    return TRUE;
}

static void Kickoff(NvPushChannelPtr p, NvBool progressTrackerWFI)
{
    NvU32 putOffset;

    if (!p) {
        return;
    }

    putOffset = (NvU32)((char *)p->main.buffer - (char *)p->main.base);

    if (p->main.putOffset == putOffset) {
        return;
    }

    DumpPB(p);

    if (nvWriteGpEntry(p, putOffset, progressTrackerWFI)) {
        // Change putOffset only if nvWriteGpEntry succeeds.
        // If it fails, it means we went through channel recovery and the
        // recovery process changed putOffset.
        p->main.putOffset = putOffset;
    }
}

void nvPushKickoff(NvPushChannelPtr p)
{
    Kickoff(p, FALSE /* progressTrackerWFI */);
}

/*!
 * Write GP_PUT to USERD.  On GPUs where HOST snoops USERD, this is all we need
 * to do to kick off the channel.
 */
static void UserDKickoff(NvPushChannelPtr push_buffer,
                         NvU32 oldGpPut, NvU32 newGpPut)
{
    /* Kick off all push buffers */
    unsigned int sd;

    for (sd = 0; sd < push_buffer->pDevice->numSubDevices; sd++) {
        KeplerBControlGPFifo *pUserd =
            (KeplerBControlGPFifo *)push_buffer->control[sd];
        pUserd->GPPut = newGpPut;
    }
}

/*!
 * Kick off a channel on GPUs where HOST does not snoop USERD.
 *
 * This is implemented in two steps:
 * 1. Write GP_PUT to USERD;
 * 2. Write the channel's token to HOST's doorbell register.
 */
static void DoorbellKickoff(NvPushChannelPtr pChannel,
                            NvU32 oldGpPut, NvU32 newGpPut)
{
    const NvPushDeviceRec *pDevice = pChannel->pDevice;
    NvU32 sd;

    /* First update GPPUT in USERD. */
    UserDKickoff(pChannel, oldGpPut, newGpPut);

#if NVCPU_IS_PPC64LE
    __asm__ __volatile__ ("sync\n\t" : : : "memory");
#elif NVCPU_IS_FAMILY_ARM
    __asm__ __volatile__ ("dsb sy\n\t" : : : "memory");
#endif

    /* Then ring the doorbells so HOST knows to check for the updated GPPUT. */
    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        volatile NvU32 *doorbell;
        NvU8 *pUserMode = (NvU8 *)pDevice->subDevice[sd].pUserMode;
        NvU32 notifIndex;
        NvNotification *pTokenNotifier = NULL;

        if (pDevice->clientSli) {
            notifIndex = NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1 + sd;
        } else {
            /* RM doesn't maintain a separate token for each subdevice. */
            notifIndex = NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1;
        }
        notifIndex |= NV_PUSH_NOTIFIER_INTERNAL_BIT;

        /* The final parameter 'sd' in nvPushGetNotifierCpuAddress is unused
         * for internal notifiers. */
        pTokenNotifier =
            nvPushGetNotifierCpuAddress(pChannel, notifIndex, 0);

        nvAssert(pUserMode != NULL);

        doorbell = (volatile NvU32 *)(pUserMode +
                                      NVC361_NOTIFY_CHANNEL_PENDING);
        *doorbell = pTokenNotifier->info32;
    }
}

/* Read GET for all devices and return the minimum or maximum*/
NvU32 nvPushReadGetOffset(NvPushChannelPtr push_buffer, NvBool minimum)
{
    unsigned int i;
    NvU32 get, bestGet = 0;
    const NvPushDeviceRec *pDevice = push_buffer->pDevice;
    NvS32 distanceToPut, minmaxDistanceToPut =
        minimum ? 0 : push_buffer->main.sizeInBytes;

    if (pDevice->numSubDevices <= 1) {
        return GpFifoReadGet(push_buffer, 0);
    }

    for (i = 0; i < pDevice->numSubDevices; i++) {
        get = GpFifoReadGet(push_buffer, i);

        /* Compute distance to put, accounting for wraps */
        distanceToPut = push_buffer->main.putOffset - get;
        if (distanceToPut < 0) {
            distanceToPut += push_buffer->main.sizeInBytes;
        }

        /* Accumulate the maximum distance to put and the corresponding get. */
        if ((minimum  && (distanceToPut >= minmaxDistanceToPut)) ||
            (!minimum && (distanceToPut <= minmaxDistanceToPut))) {
            minmaxDistanceToPut = distanceToPut;
            bestGet = get;
        }
    }
    return bestGet;
}

static void WriteGetOffset(NvPushChannelPtr p, NvU32 value)
{
    NvPushDeviceRec *pDevice = p->pDevice;
    unsigned int sd;

    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        volatile NvU32 *ptr = (volatile NvU32 *)(p->progressSemaphore.ptr[sd]);
        *ptr = value;
    }

    NV_CPU_MEMFENCE();
}

static NvBool IdleChannel(NvPushChannelPtr p, NvBool progressTrackerWFI,
                          NvU32 timeoutMSec)
{
    NvU64 baseTime, currentTime;

    /*
     * Write a channel NOP, kick off the pushbuffer, and wait for the
     * pushbuffer to drain.  It is important for the NOP to be written
     * here: this ensures the kickoff won't be optimized away, as it
     * otherwise would if the host driver called:
     *
     *   nvPushKickoff(pChannel);
     *   nvPushIdleChannel(pChannel);
     *
     * The path nvPushIdleChannel() => IdleChannel() => Kickoff() will
     * issue a WFI, which is important to ensure the channel is really
     * idle.
     */
    nvPushHeader(p, main, 0, NVA16F_DMA_NOP);

    Kickoff(p, progressTrackerWFI);

    for (baseTime = currentTime = nvPushImportGetMilliSeconds(p->pDevice);
         TRUE; currentTime = nvPushImportGetMilliSeconds(p->pDevice)) {

        nvAssert(p->main.putOffset != 0);

        if (nvPushReadGetOffset(p, TRUE) == p->main.putOffset) {
            return TRUE;
        }

        if (currentTime > (baseTime + timeoutMSec) &&
            !p->noTimeout) {
            return FALSE;
        }
    }
}

/*
 * Idle channel with the requested timeout, but don't print an error when it
 * times out.  This should be used for verifying expected timeouts on
 * idlechannels in testing.
 */
NvBool nvPushIdleChannelTest(NvPushChannelPtr pChannel, NvU32 timeoutMSec)
{
    return IdleChannel(pChannel, TRUE /* progressTrackerWFI */, timeoutMSec);
}

NvBool nvPushIdleChannel(NvPushChannelPtr pChannel)
{
    NvBool ret;

    ret = IdleChannel(pChannel, TRUE /* progressTrackerWFI */,
                      NV_PUSH_NOTIFIER_SHORT_TIMEOUT);

    if (!ret) {
        nvPushImportLogError(pChannel->pDevice, "Failed to idle DMA.");
    }

    return ret;
}

NvBool __nvPushTestPushBuffer(NvPushChannelPtr p)
{
    NvBool ret;

    /*
     * Immediately after allocating the pushbuffer, push a channel NOP and
     * babysit the channel until it's consumed as a quick sanity check.
     
     * Note we use a full long timeout (10 seconds) when performing this
     * sanity test.  In normal operation, idling will happen very quickly.
     * However, when the GPU is under heavy load in stress tests, it can
     * take much longer to idle the channel.
     */
    WriteGetOffset(p, 0);

    ret = IdleChannel(p, FALSE /* progressTrackerWFI */,
                      NV_PUSH_NOTIFIER_LONG_TIMEOUT);

    if (!ret) {
        nvPushImportLogError(p->pDevice, "Failed to initialize DMA.");
    }

    return ret;
}

void __nvPushMakeRoom(NvPushChannelPtr push_buffer, NvU32 count)
{
    NvU32 getOffset;
    NvU32 putOffset;
    NvBool fenceToEnd = FALSE;

    putOffset = (NvU32) ((char *)push_buffer->main.buffer -
                         (char *)push_buffer->main.base);

    nvAssert(putOffset <= push_buffer->main.sizeInBytes);
    nvAssert((count << 2) <= push_buffer->main.sizeInBytes);

    if (putOffset != push_buffer->main.putOffset) {
        nvPushKickoff(push_buffer);
    }
    nvAssert(putOffset == push_buffer->main.putOffset);

    while (count >= push_buffer->main.freeDwords) {
        if (nvPushCheckChannelError(push_buffer)) {
            nvAssert(!"A channel error occurred in __nvPushMakeRoom()");
            // Unlike with non-gpfifo channels, RC recovery can't reset GET to
            // 0 so we need to continue as if we just started waiting for space.
            return __nvPushMakeRoom(push_buffer, count);
        }

        getOffset = nvPushReadGetOffset(push_buffer, TRUE);
        nvAssert(getOffset <= push_buffer->main.sizeInBytes);

        if (getOffset > putOffset) {
            // We previously wrapped.  The space between PUT and GET is
            // available.
            push_buffer->main.freeDwords = ((getOffset - putOffset) >> 2) - 1;

            // TODO: If still not enough room, call DelayRegisterReadsCaller
            // here.
        } else if(!fenceToEnd) {
            // GET wrapped, so we can write all the way to the end of the
            // pushbuffer.
            fenceToEnd = TRUE;
            push_buffer->main.freeDwords =
                (push_buffer->main.sizeInBytes - putOffset) >> 2;
        } else {
            // getOffset is behind putOffset and there wasn't enough space
            // between putOffset and the end of the pushbuffer.  Wrap to the
            // beginning and wait for GET to advance far enough.

            nvAssert((putOffset >> 2) > count);

            // Record where the last method was written so that RC recovery can
            // know where to wrap.
            nvPushImportPushbufferWrapped(push_buffer);

            // We can't write putOffset to 0 while getOffset is 0
            // otherwise we could fool ourselves into thinking a full
            // pushbuffer is empty
            if (getOffset) {
                // XXX NOTE: While it would be nice to be able to decide that we
                // can write to the whole pushbuffer when getOffset == putOffset, we
                // can fall into the trap of writing the same amount of data to the
                // pushbuffer twice and not being able to tell whether GET has
                // wrapped all the way around, or hasn't moved at all.

                push_buffer->main.putOffset = 0;
                push_buffer->main.buffer = push_buffer->main.base;
                push_buffer->main.freeDwords = (getOffset >> 2) - 1;
            }
        }

        if (nvPushCheckChannelError(push_buffer)) {
            nvAssert(!"A channel error was recovered when waiting for room in push buffer");
            return __nvPushMakeRoom(push_buffer, count);
        }
    }

#if defined(DEBUG)
    {
        const NvU32 freeBytes = push_buffer->main.freeDwords * 4;
        const NvU8 *curPtr =
            ((NvU8 *)push_buffer->main.buffer) + freeBytes;
        const NvU8 *endPtr =
            ((NvU8 *)push_buffer->main.base) + push_buffer->main.sizeInBytes;

        nvAssert(freeBytes <= push_buffer->main.sizeInBytes);
        nvAssert(curPtr <= endPtr);
    }
#endif /* DEBUG */
}

/*
 * This function intializes a notifier to IN_PROGRESS on all subdevices.
 */
void nvPushInitWaitForNotifier(
    NvPushChannelPtr pChannel,
    NvU32 notifierIndex,
    NvU32 subdeviceMask)
{
    unsigned int sd;

    for (sd = 0; sd < pChannel->pDevice->numSubDevices; sd++) {

        NvNotification *pNotifier;

        if (!(subdeviceMask & (1 << sd))) {
            continue;

        }

        pNotifier = nvPushGetNotifierCpuAddress(pChannel, notifierIndex, sd);
        pNotifier->status = NV2080_SUBDEVICE_NOTIFICATION_STATUS_IN_PROGRESS;
    }
}

/*
 * This function waits for a notifier.  It does the following:
 *
 * - has a short and long timeout
 *
 * - if the short timeout was exceeded then check if PUT == GET. If PUT
 *   == GET then it is almost certain that the notifier really should
 *   have been delivered and something is wrong. We should just stop
 *   waiting for the notifier.
 *
 * - if the short timeout was exceeded and PUT != GET then trying
 *   rewriting PUT in hopes of getting the hardware back on track.
 *   (note: this doesn't actually do anything on GPFIFO channels)
 *
 * - if the PUT != GET and the long timeout is exceeded the chip must
 *   be hung or our channel encountered an error.
 *
 * - For either timeout, in a debug build print a diagnostic message
 *   to indicate what went wrong.
 *
 * This function always prints out a terse message so that we can
 * diagnose problems from the field.  In debug builds we print out
 * additional information.
 */

void nvPushWaitForNotifier(
    NvPushChannelPtr p,
    NvU32 notifyIndex,
    NvU32 subdeviceMask,
    NvBool yield,
    NvPushImportEvent *pEvent,
    int id)
{
    NvU64 newtime, short_timeout_value = 0, long_timeout_value = 0;
    NvBool short_timeout, long_timeout, shortTimeOutDone = FALSE;
    NvU32 getOffset;
    unsigned int sd;
    NvNotification * pNotify;
    int timeout = 0;
    NvBool must_wait_for_event = (pEvent != NULL);
    NvPushDevicePtr pDevice = p->pDevice;

    for (sd = 0; sd < pDevice->numSubDevices; sd++) {
        if (!(subdeviceMask & (1 << sd)))
            continue;

        pNotify = nvPushGetNotifierCpuAddress(p, notifyIndex, sd);

        // Give a chance not to enter the while loop if notifier is
        // already ready.
        while ((must_wait_for_event) ||
               (pNotify->status != NV2080_SUBDEVICE_NOTIFICATION_STATUS_DONE_SUCCESS)) {

            newtime = nvPushImportGetMilliSeconds(pDevice);

            if (!short_timeout_value) {
                short_timeout_value = newtime + NV_PUSH_NOTIFIER_SHORT_TIMEOUT;
                long_timeout_value = newtime + NV_PUSH_NOTIFIER_LONG_TIMEOUT;
            }

            if (must_wait_for_event && pEvent) {
                if (timeout == 0) {
                    timeout = NV_PUSH_NOTIFIER_SHORT_TIMEOUT;
                } else {
                    timeout = NV_PUSH_NOTIFIER_LONG_TIMEOUT-NV_PUSH_NOTIFIER_SHORT_TIMEOUT;
                    must_wait_for_event = FALSE;
                }

                if (nvPushImportWaitForEvent(pDevice, pEvent, timeout)) {
                    // At this point we won't wait for OS events
                    // anymore, but still for the notifier.
                    must_wait_for_event = FALSE;
                    continue;
                }
                // We timed out or an error occurred
            }

            short_timeout = (newtime > short_timeout_value);
            long_timeout = (newtime > long_timeout_value);

            if (p->noTimeout) {
                short_timeout = FALSE;
                long_timeout = FALSE;
            }

            if (nvPushCheckChannelError(p)) {
                nvAssert(!"A channel error was recovered when waiting for a notifier; returning");
                return;
            }

            if (short_timeout || long_timeout) {

                getOffset = GpFifoReadGet(p, sd);
                if (p->main.putOffset == getOffset) {

                    /*
                     * If PUT == GET then it is almost certain that the
                     * notifier really should have been delivered and
                     * something is wrong. We should just stop waiting for
                     * the notifier.  
                     */

                    nvPushImportLogError(pDevice,
                        "WAIT (0, %d, 0x%04x, 0x%08x, 0x%08x)",
                        id, pNotify->status, getOffset, p->main.putOffset);
                    nvAssert(!"PUT == GET, but notifier has not completed; returning!");

                    /* Set status to done in case it is read */
                    pNotify->status = NV2080_SUBDEVICE_NOTIFICATION_STATUS_DONE_SUCCESS;

                    continue;
                }

                if (long_timeout) {

                    nvPushImportLogError(pDevice,
                        "WAIT (1, %d, 0x%04x, 0x%08x, 0x%08x)",
                        id, pNotify->status, getOffset, p->main.putOffset);
                    nvAssert(!"Long timeout exceeded; PUT != GET; returning!");

                    /* Set status to done in case it is read */
                    pNotify->status = NV2080_SUBDEVICE_NOTIFICATION_STATUS_DONE_SUCCESS;

                    return;
                }

                if (!shortTimeOutDone) {
                    nvPushImportLogError(pDevice,
                        "WAIT (2, %d, 0x%04x, 0x%08x, 0x%08x)",
                         id, pNotify->status, getOffset, p->main.putOffset);
                    nvAssert(!"Short timeout exceeded; PUT != GET");

                    /* Once is enough */
                    shortTimeOutDone = TRUE;
                }
            }

            /* optionally yield */
            if (!pEvent && yield) {
                nvPushImportYield(pDevice);
            }
        }
    }

    if (pEvent) {
        /* Empty the event FIFO */
        nvPushImportEmptyEventFifo(pDevice, pEvent);
    }
}

// Decode a method header, returning the method count.  Return FALSE if the
// value is not recognizable as a method header.
NvBool nvPushDecodeMethod(NvU32 header, NvU32 *count)
{
    switch (DRF_VAL(A16F, _DMA, _SEC_OP, header)) {
        case NVA16F_DMA_SEC_OP_IMMD_DATA_METHOD:
            *count = 0;
            return TRUE;
        case NVA16F_DMA_SEC_OP_INC_METHOD:
        case NVA16F_DMA_SEC_OP_NON_INC_METHOD:
        case NVA16F_DMA_SEC_OP_ONE_INC:
            *count = DRF_VAL(A16F, _DMA, _METHOD_COUNT, header);
            return TRUE;
        default:
            // Not a recognized method header!
            return FALSE;
    }
}

static NvU32 GetSetObjectHandle(NvPushChannelPtr pChannel, NvU32 handle,
                                int deviceIndex)
{
    NvPushDevicePtr pDevice = pChannel->pDevice;
    NV906F_CTRL_GET_CLASS_ENGINEID_PARAMS params = { 0 };
    NvU32 ret;

    // AModel has a bug where it requires an object handle instead of a class
    // and engine ID.
    if (nvPushIsAModel(pDevice)) {
        return handle;
    }

    // Query RM for the class and engine ID of this handle.
    params.hObject = handle;
    ret = nvPushImportRmApiControl(pDevice,
                                   pChannel->channelHandle[deviceIndex],
                                   NV906F_CTRL_GET_CLASS_ENGINEID,
                                   &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvPushImportLogError(pDevice, "Failed to query object info.");
        return 0;
    }

#if defined(DEBUG)
    // Print debug spew mapping the SetObject ID to a class number for
    // nvdiss
    if (params.classID && pChannel->logNvDiss) {
        nvPushImportLogNvDiss(pChannel,
                              "SetObjectId: class 0x%x, objectID 0x%x\n",
                              params.classID, params.classEngineID);
    }
#endif /* DEBUG */

    return params.classEngineID;
}

// Issue a SET_OBJECT method on the specified subchannel.
void nvPushSetObject(NvPushChannelPtr p, NvU32 subch, NvU32 *object)
{
    const NvPushDeviceRec *pDevice = p->pDevice;
    const NvU32 oldSubDevMask = p->currentSubDevMask;
    int deviceIndex;

    for (deviceIndex = 0;
         deviceIndex < __nvPushGetNumDevices(pDevice);
         deviceIndex++) {
        if (pDevice->clientSli) {
            const NvU32 thisSDM = 1 << deviceIndex;
            if ((thisSDM & oldSubDevMask) == 0) {
                continue;
            }
            nvPushSetSubdeviceMask(p, thisSDM);
        }
        nvPushMethod(p, subch, NVA16F_SET_OBJECT, 1);
        nvPushSetMethodData(p,
            GetSetObjectHandle(p, object[deviceIndex], deviceIndex));
    }
    nvPushSetSubdeviceMask(p, oldSubDevMask);
}

void nvPushSetSubdeviceMask(NvPushChannelPtr p, NvU32 mask)
{
    if (nvPushSubDeviceMaskEquiv(p->pDevice, p->currentSubDevMask, mask)) {
        return;
    }
    p->currentSubDevMask = mask;

    SetSubDeviceMask(p, main, mask);
}

static void KeplerReleaseTimelineSemaphore(
    NvPushChannelPtr p,
    void *cpuAddress,
    NvU64 gpuAddress,
    NvU64 val)
{
    NvReportSemaphore32 *report = (NvReportSemaphore32 *)cpuAddress;

    // Must be done before submitting the semaphore release to ensure the maximum
    // known-submitted value is never less than the semaphore's current value.
    NvTimeSemFermiSetMaxSubmittedVal(&report->timer, val);

    nvPushMethod(p, 0, NVA16F_SEMAPHOREA, 4);
    nvPushSetMethodDataU64(p, gpuAddress);    // NVA16F_SEMAPHOREB
    nvPushSetMethodData(p, val);              // NVA16F_SEMAPHOREC
    nvPushSetMethodData(p,                    // NVA16F_SEMAPHORED
        DRF_DEF(A16F, _SEMAPHORED, _OPERATION, _RELEASE) |
        DRF_DEF(A16F, _SEMAPHORED, _RELEASE_SIZE, _4BYTE));

    nvPushMethod(p, 0, NVA16F_NON_STALL_INTERRUPT, 1);
    nvPushSetMethodData(p, 0);
}

static void KeplerAcquireTimelineSemaphore(
    NvPushChannelPtr p,
    NvU64 gpuAddress,
    NvU64 val)
{
    nvPushMethod(p, 0, NVA16F_SEMAPHOREA, 4);
    nvPushSetMethodDataU64(p, gpuAddress);    // NVA16F_SEMAPHOREB
    nvPushSetMethodData(p, val);              // NVA16F_SEMAPHOREC
    nvPushSetMethodData(p,                    // NVA16F_SEMAPHORED
        DRF_DEF(A16F, _SEMAPHORED, _ACQUIRE_SWITCH, _ENABLED) |
        DRF_DEF(A16F, _SEMAPHORED, _OPERATION, _ACQ_GEQ));
}

static void VoltaReleaseTimelineSemaphore(
    NvPushChannelPtr p,
    void *cpuAddress,
    NvU64 gpuAddress,
    NvU64 val)
{
    nvPushMethod(p, 0, NVC36F_SEM_ADDR_LO, 5);
    nvPushSetMethodDataU64LE(p, gpuAddress);        // NVC36F_SEM_ADDR_LO/HI
    nvPushSetMethodDataU64LE(p, val);               // NVC36F_SEM_PAYLOAD_LO/HI
    nvPushSetMethodData(p,                          // NVC36F_SEM_EXECUTE
        DRF_DEF(C36F, _SEM_EXECUTE, _OPERATION, _RELEASE) |
        DRF_DEF(C36F, _SEM_EXECUTE, _RELEASE_WFI, _EN) |
        DRF_DEF(C36F, _SEM_EXECUTE, _PAYLOAD_SIZE, _64BIT) |
        DRF_DEF(C36F, _SEM_EXECUTE, _RELEASE_TIMESTAMP, _EN));

    nvPushMethod(p, 0, NVC36F_NON_STALL_INTERRUPT, 1);
    nvPushSetMethodData(p, 0);
}

static void VoltaAcquireTimelineSemaphore(
    NvPushChannelPtr p,
    NvU64 gpuAddress,
    NvU64 val)
{
    nvPushMethod(p, 0, NVC36F_SEM_ADDR_LO, 5);
    nvPushSetMethodDataU64LE(p, gpuAddress);        // NVC36F_SEM_ADDR_LO/HI
    nvPushSetMethodDataU64LE(p, val);               // NVC36F_SEM_PAYLOAD_LO/HI
    nvPushSetMethodData(p,                          // NVC36F_SEM_EXECUTE
        DRF_DEF(C36F, _SEM_EXECUTE, _OPERATION, _ACQ_STRICT_GEQ) |
        DRF_DEF(C36F, _SEM_EXECUTE, _ACQUIRE_SWITCH_TSG, _EN) |
        DRF_DEF(C36F, _SEM_EXECUTE, _PAYLOAD_SIZE, _64BIT));
}

void nvPushReleaseTimelineSemaphore(
    NvPushChannelPtr p,
    void *cpuAddress,
    NvU64 gpuAddress,
    NvU64 val)
{
    NvPushDevicePtr pDevice = p->pDevice;
    pDevice->hal.releaseTimelineSemaphore(p, cpuAddress, gpuAddress, val);
}

void nvPushAcquireTimelineSemaphore(
    NvPushChannelPtr p,
    NvU64 gpuAddress,
    NvU64 val)
{
    NvPushDevicePtr pDevice = p->pDevice;
    pDevice->hal.acquireTimelineSemaphore(p, gpuAddress, val);
}

NvBool __nvPushGetHal(
    const NvPushAllocDeviceParams *pParams,
    NvU32 channelClass,
    NvPushHal *pHal)
{
    switch (channelClass) {
        case BLACKWELL_CHANNEL_GPFIFO_B:
            // backwards compatible
            // fall through
        case BLACKWELL_CHANNEL_GPFIFO_A:
            // backwards compatible with Hopper
            // fall through
        case HOPPER_CHANNEL_GPFIFO_A:
            pHal->caps.extendedBase = TRUE;
            // otherwise backwards compatible with the Volta DMA HAL
            // fall through
        case AMPERE_CHANNEL_GPFIFO_A:
            // backwards compatible with the Volta DMA HAL
            // fall through
        case TURING_CHANNEL_GPFIFO_A:
            // backwards compatible with the Volta DMA HAL
            // fall through
        case VOLTA_CHANNEL_GPFIFO_A:
            pHal->kickoff = DoorbellKickoff;
            pHal->caps.clientAllocatesUserD = TRUE;
            pHal->caps.allocateDoubleSizeGpFifo = FALSE;
            pHal->caps.voltaSemMethods = TRUE;
            pHal->releaseTimelineSemaphore = VoltaReleaseTimelineSemaphore;
            pHal->acquireTimelineSemaphore = VoltaAcquireTimelineSemaphore;
            break;
        case PASCAL_CHANNEL_GPFIFO_A:
            // backwards compatible with the Kepler DMA HAL
            // fall through
        case MAXWELL_CHANNEL_GPFIFO_A:
            // backwards compatible with the Kepler DMA HAL
            // fall through
        case KEPLER_CHANNEL_GPFIFO_B:
            pHal->kickoff = UserDKickoff;
            pHal->caps.clientAllocatesUserD = FALSE;
            pHal->caps.allocateDoubleSizeGpFifo = FALSE;
            pHal->releaseTimelineSemaphore = KeplerReleaseTimelineSemaphore;
            pHal->acquireTimelineSemaphore = KeplerAcquireTimelineSemaphore;
            break;
        default:
            nvAssert(!"There's no DMA HAL for this channel class");
    }

    if (pParams->amodel.config != NV_AMODEL_NONE) {
        pHal->kickoff = NULL;
    } else if (pParams->isTegra) {
        pHal->kickoff = NULL;
    }

    return !!(pHal->kickoff);
}

void __nvPushMoveDWORDS(NvU32* dst, const NvU32* src, int dwords)
{
    while (dwords & ~0x03) {
        *dst = *src;
        *(dst + 1) = *(src + 1);
        *(dst + 2) = *(src + 2);
        *(dst + 3) = *(src + 3);
        src += 4;
        dst += 4;
        dwords -= 4;
    }
    if (!dwords) return;
    *dst = *src;
    if (dwords == 1) return;
    *(dst + 1) = *(src + 1);
    if (dwords == 2) return;
    *(dst + 2) = *(src + 2);
}
