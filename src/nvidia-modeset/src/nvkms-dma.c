/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2013 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <stddef.h>

#include "nvkms-dma.h"
#include "nvkms-utils.h"
#include "nvkms-rmapi.h"
#include "class/cl917d.h" // NV917DDispControlDma, NV917D_DMA_*
#include <ctrl/ctrl0080/ctrl0080dma.h> // NV0080_CTRL_CMD_DMA_FLUSH
#include "nvos.h"

#define NV_DMA_PUSHER_CHASE_PAD 5
#define NV_EVO_NOTIFIER_SHORT_TIMEOUT_USEC 3000000 // 3 seconds

static void EvoCoreKickoff(NVDmaBufferEvoPtr push_buffer, NvU32 putOffset);

void nvDmaKickoffEvo(NVEvoChannelPtr pChannel)
{
    NVDmaBufferEvoPtr p = &pChannel->pb;
    NvU32 putOffset = (NvU32)((char *)p->buffer - (char *)p->base);

    if (p->put_offset == putOffset) {
        return;
    }

    EvoCoreKickoff(p, putOffset);
}

static void EvoCoreKickoff(NVDmaBufferEvoPtr push_buffer, NvU32 putOffset)
{
    NVEvoDmaPtr pDma = &push_buffer->dma;
    int i;

    nvAssert(putOffset % 4 == 0);
    nvAssert(putOffset <= push_buffer->offset_max);

    /* If needed, copy the chunk to be kicked off into each GPU's FB */
    if (pDma->isBar1Mapping) {
        NVDevEvoPtr pDevEvo = push_buffer->pDevEvo;
        int sd;

        NV0080_CTRL_DMA_FLUSH_PARAMS flushParams = { 0 };
        NvU32 ret;

        NvU32 *endAddress;

        if (putOffset < push_buffer->put_offset) {
            /* If we've wrapped, copy to the end of the pushbuffer */
            nvAssert(putOffset == 0);
            endAddress = push_buffer->base + push_buffer->offset_max /
                                             sizeof(NvU32);
        } else {
            endAddress = push_buffer->buffer;
        }

        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            NvU32 startOffset = push_buffer->put_offset / sizeof(NvU32);

            NvU32 *src = push_buffer->base;
            NvU32 *dst = pDma->subDeviceAddress[sd];

            nvAssert(dst != NULL);

            src += startOffset;
            dst += startOffset;
            while (src < endAddress) {
                *dst++ = *src++;
            }
        }

        /*
         * Finally, tell RM to flush so that the data actually lands in FB
         * before telling the GPU to fetch it.
         */
        flushParams.targetUnit = DRF_DEF(0080_CTRL_DMA, _FLUSH_TARGET,
                                         _UNIT_FB, _ENABLE);

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->deviceHandle,
                             NV0080_CTRL_CMD_DMA_FLUSH,
                             &flushParams, sizeof(flushParams));
        if (ret != NVOS_STATUS_SUCCESS) {
            nvAssert(!"NV0080_CTRL_CMD_DMA_FLUSH failed");
        }
    }

#if NVCPU_IS_X86_64
    __asm__ __volatile__ ("sfence\n\t" : : : "memory");
#elif NVCPU_IS_FAMILY_ARM
    __asm__ __volatile__ ("dsb sy\n\t" : : : "memory");
#endif

    /* Kick off all push buffers */
    push_buffer->put_offset = putOffset;
    for (i = 0; i <  push_buffer->num_channels; i++) {
        void *pControl = push_buffer->control[i];
        nvDmaStorePioMethod(pControl, NV917D_PUT, putOffset);
    }
}

/* Read GET from an EVO core channel */
static NvU32 EvoCoreReadGet(NVDmaBufferEvoPtr push_buffer, int sd)
{
    void *pControl = push_buffer->control[sd];
    return nvDmaLoadPioMethod(pControl, NV917D_GET);
}

/* Read GET for all devices and return the minimum or maximum*/
static NvU32 EvoReadGetOffset(NVDmaBufferEvoPtr push_buffer, NvBool minimum)
{
    int i;
    NvU32 get, bestGet = 0;
    NvS32 distanceToPut, minmaxDistanceToPut = (minimum ?
                                                0 :
                                                (push_buffer->dma.limit + 1));

    if (push_buffer->num_channels <= 1) {
        return EvoCoreReadGet(push_buffer, 0);
    }

    for (i =0; i < push_buffer->num_channels; i++) {
        get = EvoCoreReadGet(push_buffer, i);

        /* Compute distance to put, accounting for wraps */
        distanceToPut = push_buffer->put_offset - get;
        if (distanceToPut < 0)
            distanceToPut += push_buffer->dma.limit + 1;

        /* Accumulate the maximum distance to put and the corresponding get. */
        if ((minimum  && (distanceToPut >= minmaxDistanceToPut)) ||
            (!minimum && (distanceToPut <= minmaxDistanceToPut))) {
            minmaxDistanceToPut = distanceToPut;
            bestGet = get;
        }
    }
    return bestGet;
}

NvBool nvEvoPollForEmptyChannel(NVEvoChannelPtr pChannel, NvU32 sd,
                                NvU64 *pStartTime, const NvU32 timeout)
{
    NVDmaBufferEvoPtr push_buffer = &pChannel->pb;

    do {
        if (EvoCoreReadGet(push_buffer, sd) == push_buffer->put_offset) {
            break;
        }

        if (nvExceedsTimeoutUSec(push_buffer->pDevEvo, pStartTime, timeout)) {
            return FALSE;
        }

        nvkms_yield();
   } while (TRUE);

    return TRUE;
}

void nvEvoMakeRoom(NVEvoChannelPtr pChannel, NvU32 count)
{
    NVDmaBufferEvoPtr push_buffer = &pChannel->pb;
    NvU32 getOffset;
    NvU32 putOffset;
    NvU64 startTime = 0;
    const NvU64 timeout = 5000000; /* 5 seconds */

    putOffset = (NvU32) ((char *)push_buffer->buffer -
                         (char *)push_buffer->base);

    if (putOffset >= push_buffer->offset_max) {
        *(push_buffer->buffer) = 0x20000000;
        push_buffer->buffer = push_buffer->base;
        nvDmaKickoffEvo(pChannel);
        putOffset = 0;
    }

    while (1) {
        getOffset = EvoReadGetOffset(push_buffer, TRUE);

        if (putOffset >= getOffset) {
            push_buffer->fifo_free_count =
                (push_buffer->offset_max - putOffset) >> 2;

            if (push_buffer->fifo_free_count <= count) {
                if (getOffset) {
                    *(push_buffer->buffer) = 0x20000000;
                    push_buffer->buffer = push_buffer->base;
                    nvDmaKickoffEvo(pChannel);
                    putOffset = 0;
                }
                else if (putOffset != push_buffer->put_offset) {
                    nvDmaKickoffEvo(pChannel);
                    // Put offset will have changed if a tail was inserted.
                    putOffset = push_buffer->put_offset;
                }
            }
        }
        else {
            getOffset = (getOffset > push_buffer->offset_max) ?
               push_buffer->offset_max : getOffset;

            if ((putOffset + (NV_DMA_PUSHER_CHASE_PAD * 4)) >= getOffset)
                push_buffer->fifo_free_count = 0;
            else
                push_buffer->fifo_free_count =
                   ((getOffset - putOffset) >> 2) - 1;
        }
        if (push_buffer->fifo_free_count > count) {
            break;
        }

        /*
         * If we have been waiting too long, print an error message.  There
         * isn't much we can do as currently structured, so just reset
         * startTime.
         */
        if (nvExceedsTimeoutUSec(push_buffer->pDevEvo, &startTime, timeout)) {
            nvEvoLogDev(push_buffer->pDevEvo, EVO_LOG_ERROR,
                "Error while waiting for GPU progress: "
                "0x%08x:%d %d:%d:%d:%d",
                pChannel->hwclass, pChannel->instance,
                count, push_buffer->fifo_free_count, getOffset, putOffset);
            startTime = 0;
        }

        nvkms_yield();
   }
}

static inline void EvoWriteNotifier(volatile NvU32 *pNotifier, NvU32 value)
{
   /*
     * Note that we don't need to flush to vidmem here; any subsequent GPU
     * write will always be triggered by kicking off pushbuffer methods,
     * which will perform a general FB flush.  This does assume that the
     * pushbuffer and its associated notifier surfaces are either both in
     * sysmem or both in vidmem, however.
     */

    *pNotifier = value;
}

/* Write the EVO core notifier at the given offset to the given value. */
void nvWriteEvoCoreNotifier(
    const NVDispEvoRec *pDispEvo,
    NvU32 offset,
    NvU32 value)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    NVEvoDmaPtr pSubChannel = &pDevEvo->core->notifiersDma[sd];
    volatile NvU32 *pNotifiers = pSubChannel->subDeviceAddress[sd];

    EvoWriteNotifier(pNotifiers + offset, value);
}

static NvBool EvoCheckNotifier(const NVDispEvoRec *pDispEvo,
                               NvU32 offset, NvU32 done_base_bit,
                               NvU32 done_extent_bit, NvU32 done_value,
                               NvBool wait)
{
    const NvU32 sd = pDispEvo->displayOwner;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoDmaPtr pSubChannel = &pDevEvo->core->notifiersDma[sd];
    NVDmaBufferEvoPtr p = &pDevEvo->core->pb;
    volatile NvU32 *pNotifier;
    NvU64 startTime = 0;

    pNotifier = pSubChannel->subDeviceAddress[sd];

    nvAssert(pNotifier != NULL);
    pNotifier += offset;

    // While the completion notifier is not set to done_true
    do {
        const NvU32 val = *pNotifier;
        const NvU32 done_mask = DRF_SHIFTMASK(done_extent_bit:done_base_bit);
        const NvU32 done_val = done_value << done_base_bit;

        if ((val & done_mask) == done_val) {
            return TRUE;
        }

        if (!wait) {
            return FALSE;
        }

        if (nvExceedsTimeoutUSec(
                pDevEvo,
                &startTime,
                NV_EVO_NOTIFIER_SHORT_TIMEOUT_USEC) &&
            (p->put_offset == EvoCoreReadGet(p, sd)))
        {
            nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                         "Lost display notification (%d:0x%08x); "
                         "continuing.", sd, val);
            EvoWriteNotifier(pNotifier, done_value << done_base_bit);
            return TRUE;
        }

        nvkms_yield();
    } while (TRUE);
}

/*
 * Used by NV_EVO_WAIT_FOR_NOTIFIER() and NV_EVO_WAIT_FOR_CAPS_NOTIFIER()
 */
void nvEvoWaitForCoreNotifier(const NVDispEvoRec *pDispEvo, NvU32 offset,
                              NvU32 done_base_bit, NvU32 done_extent_bit,
                              NvU32 done_value)
{
    EvoCheckNotifier(pDispEvo, offset,
                     done_base_bit, done_extent_bit, done_value, TRUE);
}

/*
 * Used by the EVO HAL IsNotifierComplete functions.  Returns TRUE if the
 * notifier is complete.
 */
NvBool nvEvoIsCoreNotifierComplete(NVDispEvoPtr pDispEvo, NvU32 offset,
                                   NvU32 done_base_bit, NvU32 done_extent_bit,
                                   NvU32 done_value)
{
    return EvoCheckNotifier(pDispEvo,
                            offset, done_base_bit, done_extent_bit,
                            done_value, FALSE);
}

void nvEvoSetSubdeviceMask(NVEvoChannelPtr pChannel, NvU32 mask)
{
    NVDmaBufferEvoPtr p = &pChannel->pb;

    nvAssert(!nvDmaSubDevMaskMatchesCurrent(pChannel, mask));

    p->currentSubDevMask = mask;

    ASSERT_DRF_NUM(917D, _DMA, _SET_SUBDEVICE_MASK_VALUE, mask);

    if (p->fifo_free_count <= 1) {
        nvEvoMakeRoom(pChannel, 1);
    }

    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(917D, _DMA, _OPCODE, _SET_SUBDEVICE_MASK) |
        DRF_NUM(917D, _DMA, _SET_SUBDEVICE_MASK_VALUE, mask));
    p->fifo_free_count -= 1;
}

/*!
 * Reads CRC values from the notifier.
 *
 * This function will attempt to read in the first 'entry_count' CRC notifier
 * entries that HW generated. The actual number of entries that are read may
 * be less.
 *
 * \param[in]  pCRC32Notifier   Pointer to the CRC notifier memory.
 * \param[in]  entry_stride     Stride of a single CRC notifier entry
 * \param[in]  entry_count      Expected count of notifier entries to read
 * \param[in]  status_offset    Offset for Status flags header in CRC notifier
 * \param[in]  field_count      Number of fields to read from each CRC notifier
 *                              entry.
 * \param[in]  flag_count       Number of flags to read from the Status Header
 * \param[in out] field_info    Specifies the offset/base/extent info for each field.
 *                              Each 'field_info' contains an output array for
 *                              storing 'entry_count' field values.
 * \param[in]     flag_info     Specifies the base/extent info for each flag.
 *                              Each 'flag_info' contains a 'flag_type' for
 *                              addressing error cases related to the flags.
 *
 * \return  Returns the MIN(count, entry_count) of successfully
 *          read entries.
 */
NvU32 nvEvoReadCRC32Notifier(volatile NvU32 *pCRC32Notifier,
                             NvU32 entry_stride,
                             NvU32 entry_count,
                             NvU32 status_offset,
                             NvU32 field_count,
                             NvU32 flag_count,
                             const CRC32NotifierEntryRec *field_info,
                             const CRC32NotifierEntryFlags *flag_info)
{
    NvU32 count = 0;
    NvU32 i, j, k;

    nvAssert(pCRC32Notifier != NULL);

    // Iterate over flags (unique at start of the CRC32Notifier Struct)
    for (k = 0; k < flag_count; k++) {
        CRC32NotifierEntryFlags info = flag_info[k];
        volatile NvU32 *pFlag = pCRC32Notifier + status_offset;
        NvU32 flag_mask =
              DRF_SHIFTMASK((info.flag_extent_bit):(info.flag_base_bit));
        NvU32 flag = (*pFlag & flag_mask) >> info.flag_base_bit;

        switch (info.flag_type)
        {
            case NVEvoCrc32NotifierFlagCount:
                count = flag;
                // entry_count is max of each field_frame_values[i] array
                if (count > entry_count) {
                    nvEvoLog(EVO_LOG_WARN, "Too many CRC32 generated entries "
                             "(%d expected; %d found)", entry_count, count);
                    count = entry_count;
                }
                break;

            case NVEvoCrc32NotifierFlagCrcOverflow:
                if (flag) {
                    count = 0;
                    nvEvoLog(EVO_LOG_ERROR, "CRC Overflow occured, "
                             "CRC value unable to be processed fast enough.\n"
                             "Failing flag index in status_info array: %d",
                              k);

                    return count;
                }
                break;
        }
    }

    // Iterate over each collection of fields, for count pairs of values
    for (i = 0; i < count; i++) {
        for (j = 0; j < field_count; j++) {
            CRC32NotifierEntryRec info = field_info[j];
            volatile NvU32 *pEntry = pCRC32Notifier + info.field_offset;
            NvU32 field_mask =
                DRF_SHIFTMASK((info.field_extent_bit):(info.field_base_bit));

            info.field_frame_values[i].value =
                (*pEntry & field_mask) >> info.field_base_bit;
            info.field_frame_values[i].supported = TRUE;
        }
        pCRC32Notifier += entry_stride;
    }

    return count;
}

void nvEvoResetCRC32Notifier(volatile NvU32 *pCRC32Notifier,
                             NvU32 offset,
                             NvU32 reset_base_bit,
                             NvU32 reset_value)
{
    const NvU32 reset_val = reset_value << reset_base_bit;

    nvAssert(pCRC32Notifier != NULL);
    pCRC32Notifier += offset;

    EvoWriteNotifier(pCRC32Notifier, reset_val);
}

NvBool nvEvoWaitForCRC32Notifier(const NVDevEvoPtr pDevEvo,
                                 volatile NvU32 *pCRC32Notifier,
                                 NvU32 offset,
                                 NvU32 done_base_bit,
                                 NvU32 done_extent_bit,
                                 NvU32 done_value)
{
    const NvU32 done_mask = DRF_SHIFTMASK(done_extent_bit:done_base_bit);
    const NvU32 done_val = done_value << done_base_bit;
    NvU64 startTime = 0;

    nvAssert(pCRC32Notifier != NULL);
    pCRC32Notifier += offset;

    do {
        const NvU32 status = *pCRC32Notifier;

        if ((status & done_mask) == done_val) {
            return TRUE;
        }

        if (nvExceedsTimeoutUSec(
                pDevEvo,
                &startTime,
                NV_EVO_NOTIFIER_SHORT_TIMEOUT_USEC)) {
            return FALSE;
        }

        nvkms_yield();

    } while (TRUE);

    return FALSE;
}
