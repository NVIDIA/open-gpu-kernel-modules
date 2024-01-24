/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file contains macros and inline functions used to actually program
 * methods.
 */

#ifndef __NVIDIA_PUSH_METHODS_H__
#define __NVIDIA_PUSH_METHODS_H__

#include "nvidia-push-types.h"

#include "class/cla16f.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void __nvPushSetMethodDataSegment(NvPushChannelSegmentPtr s, const NvU32 data)
{
    s->buffer->u = data;
    s->buffer++;
}

static inline void nvPushSetMethodData(NvPushChannelPtr p, const NvU32 data)
{
    __nvPushSetMethodDataSegment(&p->main, data);
}

#if NV_PUSH_ALLOW_FLOAT
static inline void __nvPushSetMethodDataSegmentF(NvPushChannelSegmentPtr s, const float data)
{
    s->buffer->f = data;
    s->buffer++;
}

static inline void nvPushSetMethodDataF(NvPushChannelPtr p, const float data)
{
    __nvPushSetMethodDataSegmentF(&p->main, data);
}
#endif

static inline void __nvPushSetMethodDataSegmentU64(NvPushChannelSegmentPtr s, const NvU64 data)
{
    __nvPushSetMethodDataSegment(s, NvU64_HI32(data));
    __nvPushSetMethodDataSegment(s, NvU64_LO32(data));
}

static inline void nvPushSetMethodDataU64(NvPushChannelPtr p, const NvU64 data)
{
    __nvPushSetMethodDataSegmentU64(&p->main, data);
}

/* Little-endian: least significant bits first. */
static inline void __nvPushSetMethodDataSegmentU64LE(NvPushChannelSegmentPtr s, const NvU64 data)
{
    __nvPushSetMethodDataSegment(s, NvU64_LO32(data));
    __nvPushSetMethodDataSegment(s, NvU64_HI32(data));
}

static inline void nvPushSetMethodDataU64LE(NvPushChannelPtr p, const NvU64 data)
{
    __nvPushSetMethodDataSegmentU64LE(&p->main, data);
}

void __nvPushMoveDWORDS(NvU32* dst, const NvU32* src, int dwords);

static inline void
nvDmaMoveDWORDS(NvPushChannelUnion *dst, const NvU32* src, int dwords)
{
    // The 'dst' argument is an array of NvPushChannelUnion; it is safe
    // to treat this as an array of NvU32, as long as NvU32 and
    // NvPushChannelUnion are the same size.
    ct_assert(sizeof(NvU32) == sizeof(NvPushChannelUnion));
    __nvPushMoveDWORDS((NvU32 *)dst, src, dwords);
}

static inline void nvPushInlineData(NvPushChannelPtr p, const void *data,
                                    size_t dwords)
{
    nvDmaMoveDWORDS(p->main.buffer, (const NvU32 *)data, dwords);
    p->main.buffer += dwords;
}

/*!
 * Return the maximum method count: the maximum number of dwords that can be
 * specified in the nvPushMethod() family of macros.
 */
static inline NvU32 nvPushMaxMethodCount(const NvPushChannelRec *p)
{
    /*
     * The number of methods that can be specified in one NVA16F_DMA_METHOD
     * header is limited by the bit field size of NVA16F_DMA_METHOD_COUNT: 28:16
     * (i.e., maximum representable value 8191).
     */
    const NvU32 maxFromMethodCountMask = DRF_MASK(NVA16F_DMA_METHOD_COUNT);

    /*
     * Further, the method count must be smaller than half the total pushbuffer
     * size minus one, to correctly distinguish empty and full pushbuffers.  See
     * nvPushHeader() for details.
     */
    const NvU32 pushBufferSizeInBytes = p->main.sizeInBytes;
    const NvU32 pushBufferSizeInDWords = pushBufferSizeInBytes / 4;
    const NvU32 pushBufferHalfSizeInDWords = pushBufferSizeInDWords / 2;

    /*
     * Subtract two from pushBufferHalfSizeInDWords:
     *
     * -1 to distinguish pushbuffer empty from full (see above).
     *
     * -1 to be smaller than, rather than equal to, the above constraints.
     */
    const NvU32 maxFromPushBufferSize = pushBufferHalfSizeInDWords - 2;

    return NV_MIN(maxFromMethodCountMask, maxFromPushBufferSize);
}

// These macros verify that the values used in the methods fits
// into the defined ranges.
#define ASSERT_DRF_DEF(d, r, f, n) \
    nvAssert(!(~DRF_MASK(NV ## d ## r ## f) & (NV ## d ## r ## f ## n)))
#define ASSERT_DRF_NUM(d, r, f, n) \
    nvAssert(!(~DRF_MASK(NV ## d ## r ## f) & (n)))

#if defined(DEBUG)
#include "class/clc36f.h"    /* VOLTA_CHANNEL_GPFIFO_A */

/*
 * When pushing GPFIFO methods (NVA16F_SEMAPHORE[ABCD]), all four
 * methods must be pushed together.  If the four methods are not
 * pushed together, nvidia-push might wrap, injecting its progress
 * tracking semaphore release methods in the middle, and perturb the
 * NVA16F_SEMAPHOREA_OFFSET_UPPER and NVA16F_SEMAPHOREB_OFFSET_LOWER
 * channel state.
 *
 * Return whether the methods described by the arguments include some,
 * but not all, of A, B, C, and D.  I.e., if the range starts at B, C,
 * or D, or if the range ends at A, B, or C.
 *
 * Perform a similar check for Volta+ semaphore methods
 * NVC36F_SEM_ADDR_LO..NVC36F_SEM_EXECUTE.  Note that we always check for both
 * sets of methods, regardless of the GPU we're actually running on.  This is
 * okay since:
 * a) the NVC36F_SEM_ADDR_LO..NVC36F_SEM_EXECUTE method offsets were not used
 *    for anything from (a16f..c36f].
 * b) the SEMAPHORE[ABCD] methods still exist on the newer classes (they
 *    haven't been reused for anything else)
 */
static inline NvBool __nvPushStartSplitsSemaphore(
    NvU32 method,
    NvU32 count,
    NvU32 secOp)
{
    ct_assert(NVA16F_SEMAPHOREA < NVA16F_SEMAPHORED);
    ct_assert(NVC36F_SEM_ADDR_LO < NVC36F_SEM_EXECUTE);

    /*
     * compute start and end as inclusive; if not incrementing, we
     * assume end==start
     */
    const NvU32 start = method;
    const NvU32 end = (secOp == NVA16F_DMA_SEC_OP_INC_METHOD) ?
        (method + ((count - 1) * 4)) : method;

    return ((start >  NVA16F_SEMAPHOREA)  && (start <= NVA16F_SEMAPHORED))  ||
           ((end   >= NVA16F_SEMAPHOREA)  && (end   <  NVA16F_SEMAPHORED))  ||
           ((start >  NVC36F_SEM_ADDR_LO) && (start <= NVC36F_SEM_EXECUTE)) ||
           ((end   >= NVC36F_SEM_ADDR_LO) && (end   <  NVC36F_SEM_EXECUTE));
}
#endif /* DEBUG */

/*
 * Note that _count+1 must be less than half the total pushbuffer size.  This is
 * required by GPFIFO because we can't reliably tell when we can write all the
 * way to the end of the pushbuffer if we wrap (see bug 232454).  This
 * assumption ensures that there will be enough space once GET reaches PUT.
 */
#define nvPushHeader(_push_buffer, _segment, _count, _header) do {    \
    NvPushChannelSegmentPtr _pSegment = &(_push_buffer)->_segment;    \
    nvAssert(((_count)+1) < ((_pSegment)->sizeInBytes / 8));          \
    if ((_pSegment)->freeDwords < ((_count)+1))                       \
        __nvPushMakeRoom((_push_buffer), (_count) + 1);               \
    __nvPushSetMethodDataSegment((_pSegment), (_header));             \
    (_pSegment)->freeDwords -= ((_count)+1);                          \
} while(0)

#define __nvPushStart(_push_buffer, _segment, _subch, _offset, _count, _opcode) \
{                                                                     \
    nvAssert(!__nvPushStartSplitsSemaphore(                           \
                                      (_offset),                      \
                                      (_count),                       \
                                      NVA16F_DMA_SEC_OP ## _opcode)); \
    ASSERT_DRF_DEF(A16F, _DMA, _SEC_OP, _opcode);                     \
    ASSERT_DRF_NUM(A16F, _DMA, _METHOD_COUNT, _count);                \
    ASSERT_DRF_NUM(A16F, _DMA, _METHOD_SUBCHANNEL, _subch);           \
    ASSERT_DRF_NUM(A16F, _DMA, _METHOD_ADDRESS, (_offset) >> 2);      \
    nvPushHeader((_push_buffer), _segment, (_count),                  \
        DRF_DEF(A16F, _DMA, _SEC_OP,               _opcode)  |        \
        DRF_NUM(A16F, _DMA, _METHOD_COUNT,         _count)   |        \
        DRF_NUM(A16F, _DMA, _METHOD_SUBCHANNEL,    _subch)   |        \
        DRF_NUM(A16F, _DMA, _METHOD_ADDRESS,    (_offset) >> 2));     \
}

// The GPU can encode a 13-bit constant method/data pair in a single DWORD.
#define nvPushImmedValSegment(_push_buffer, _segment, _subch, _offset, _data) { \
    ASSERT_DRF_NUM(A16F, _DMA, _IMMD_DATA, _data);                    \
    ASSERT_DRF_NUM(A16F, _DMA, _METHOD_SUBCHANNEL, _subch);           \
    ASSERT_DRF_NUM(A16F, _DMA, _METHOD_ADDRESS, (_offset) >> 2);      \
    if ((_push_buffer)->_segment.freeDwords < 1)                      \
        __nvPushMakeRoom((_push_buffer), 1);                          \
    __nvPushSetMethodDataSegment(&(_push_buffer)->_segment,           \
        DRF_DEF(A16F, _DMA, _SEC_OP,     _IMMD_DATA_METHOD)  |        \
        DRF_NUM(A16F, _DMA, _IMMD_DATA,             _data)   |        \
        DRF_NUM(A16F, _DMA, _METHOD_SUBCHANNEL,    _subch)   |        \
        DRF_NUM(A16F, _DMA, _METHOD_ADDRESS,    (_offset) >> 2));     \
    (_push_buffer)->_segment.freeDwords--;                            \
}

#define nvPushImmedVal(_push_buffer, _subch, _offset, _data) \
    nvPushImmedValSegment(_push_buffer, main, _subch, _offset, _data)

#define nvPushImmed(_push_buffer, _subch, _offset, _val) \
    nvPushImmedVal(_push_buffer, _subch, _offset, _offset##_V_##_val)

// Method headers.
#define nvPushMethod(_push_buffer, _subch, _offset, _count) \
    __nvPushStart(_push_buffer, main, _subch, _offset, _count, _INC_METHOD)
#define nvPushMethodNoIncr(_push_buffer, _subch, _offset, _count) \
    __nvPushStart(_push_buffer, main, _subch, _offset, _count, _NON_INC_METHOD)
#define nvPushMethodOneIncr(_push_buffer, _subch, _offset, _count) \
    __nvPushStart(_push_buffer, main, _subch, _offset, _count, _ONE_INC)

#ifdef __cplusplus
};
#endif

#endif /* __NVIDIA_PUSH_METHODS_H__ */
