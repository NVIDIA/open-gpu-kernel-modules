/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVKV_H_
#define _NVKV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvmisc.h"
#include "nvstatus.h"
#include "utils/nvassert.h"
#include "nvport/nvport.h"

/*
** NVKV is a simple "key/value" encoding scheme loosly inspired by pushbuffer encoding.  It is designed to efficiently encode
** high level API information into a compact format that is easy decode.
**
** Keys are specified as 16-bit values and will generally be scoped to groups of related RPCs.   For keys that are associated
** with natuarally indexable set of values, a dedicated index field is provided to avoid a requirement for defining a large
** number of sequential keys.
**
** The data associated with any one key may be either a single 32-bit or 64-bit value, or may be an array of 8-bit, 32-bit or
** 64-bit values.
**
** Helper macros are provided to assist with encoding data into the NVKV format.  These are similar to the macros used for
** pushbuffer encoding and follow similar patterns.   Macros that support encoding multiple key/value pairs of the same type
** require that the associated keys are sequential. This is a requirement to use the most efficient encoding that only has
** to encode the first key being sent.
**
**
** General "instruction" format:
**
** 63                                                            32 31   28 27                     16 15                           0
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
** |                    immediate/count[63:32]                     |opcode |       index[27:16]      |       key[15:0]              |
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
**
** 63                                                            32 31   28 27                     16 15                           0
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
** |                   32-bit immediate[63:32]                     | IMM32 |       index[27:16]      |       key[15:0]              |
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
**
** 63                                                            32 31   28 27                     16 15                           0
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
** |                       key count [63:32]                       | SEQ*  |       index[27:16]      |       key[15:0]              |
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
**
** 63                                                            32 31   28 27                     16 15                           0
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
** |                      value count [63:32]                      |ARRAY* |       index[27:16]      |       key[15:0]              |
** .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+.
**
** For the SEQ* and ARRAY* opcodes, the associated values will follow immediately after the instruction with additional padding added
** to ensure instruction boundaries are aligned to 8 bytes.  The padding will be ignored by the decoder, but the standard encoding
** macros provided below will ensure it is zero filled.
**
*/

#define NVKV_KEY                                          15:0            /* 16 bit key (64K unique values) */
#define NVKV_IDX12                                        27:16           /* 12 bit index */
#define NVKV_OPCODE                                       31:28           /* 4 bit opcode */
#define NVKV_COUNT32                                      63:32           /* 32-bit count/size, aliased with... */
#define NVKV_IMM32_VALUE                                  63:32           /* ... 32-bit immediate field */
#define NVKV_VALUE64                                      63:0            /* 64-bit value */

/* NVKV_OPCODE_IMM32 is used to encode a single key with a single 32-bit value stored within the instruction */
#define NVKV_OPCODE_IMM32                                 (0x0)

/* NVKV_OPCODE_SEQ32 encodes a sequential set of keys each with 32-bit values */
#define NVKV_OPCODE_SEQ32                                 (0x1)

/* NVKV_OPCODE_SEQ64 encodes a sequential set of keys each with 64-bit values */
#define NVKV_OPCODE_SEQ64                                 (0x2)

/* NVKV_OPCODE_ARRAY8 encodes a single key containing an array of 8-bit values */
#define NVKV_OPCODE_ARRAY8                                (0x3)

/* NVKV_OPCODE_ARRAY32 encodes a single key containing an array of 32-bit values */
#define NVKV_OPCODE_ARRAY32                               (0x4)

/* NVKV_OPCODE_ARRAY64 encodes a single key containing an array of 64-bit values */
#define NVKV_OPCODE_ARRAY64                               (0x5)

/* Helper macro to encode non-immediate values */
#define NVKV_KEY_COUNT(OpCode, Idx, Key, Count)             \
    (REF_DEF64(NVKV_OPCODE, OpCode) |                       \
     REF_NUM64(NVKV_IDX12, Idx) |                           \
     REF_NUM64(NVKV_KEY, Key) |                             \
     REF_NUM64(NVKV_COUNT32, (NvU64)Count))

/* Helper macro to encode a single key with a single 32-bit immediate value */
#define NVKV_KEY_IMM32(Idx, Key, Imm)                       \
    (REF_DEF64(NVKV_OPCODE, _IMM32) |                       \
     REF_NUM64(NVKV_IDX12, Idx) |                           \
     REF_NUM64(NVKV_KEY, Key) |                             \
     REF_NUM64(NVKV_IMM32_VALUE, (NvU64)Imm))

#if !defined(NVKV_ASSERT)
#define NVKV_ASSERT(x) NV_ASSERT(x)
#endif

typedef struct NVKVContext {
    NvU64 *pPtr64;
    NvU64  kvIndex;
    NvU64  kvLimit;
} NVKVContext;

/* Inline helper functions */
static inline NVKVContext nvkvBegin(NvU64 *ptr64, NvU64 kvIndex, NvU64 kvLimit)
{
    NVKVContext ctx;
    ctx.pPtr64 = ptr64;
    ctx.kvIndex = kvIndex;
    ctx.kvLimit = (ptr64 != NULL) ? kvLimit : 0 ;
    return ctx;
}

static inline NvU64 nvkvEnd(NVKVContext *pCtx)
{
    pCtx->pPtr64 = NULL;
    pCtx->kvLimit = 0;
    return pCtx->kvIndex;
}

static inline void nvkvSetData(NVKVContext *pCtx, NvU64 data)
{
    if((pCtx->kvIndex + 1) <= pCtx->kvLimit)
        pCtx->pPtr64[pCtx->kvIndex] = data;
    pCtx->kvIndex += 1;
}

static inline void nvkvSetImm32(NVKVContext *pCtx, NvU64 idx, NvU64 key1, NvU32 data1)
{
    if ((pCtx->kvIndex + 1) <= pCtx->kvLimit)
        pCtx->pPtr64[pCtx->kvIndex] = NVKV_KEY_IMM32(idx, key1, data1);
    pCtx->kvIndex += 1;
}

static inline void nvkvSetSeq32_2U(NVKVContext *pCtx, NvU64 idx, NvU64 key1, NvU32 data1, NvU32 data2, NvU64 count)
{
    if ((pCtx->kvIndex + 2) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_SEQ32, idx, key1, count);
        pCtx->pPtr64[pCtx->kvIndex + 1] = (NvU64)data1 | ((NvU64)data2 << 32);
    }
    pCtx->kvIndex += 2;
}

static inline void nvkvSetSeq32_4U(NVKVContext *pCtx, NvU64 idx, NvU64 key1, NvU32 data1, NvU32 data2, NvU32 data3, NvU32 data4, NvU64 count)
{
    if ((pCtx->kvIndex + 3) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_SEQ32, idx, key1, count);
        pCtx->pPtr64[pCtx->kvIndex + 1] = (NvU64)data1 | ((NvU64)data2 << 32);
        pCtx->pPtr64[pCtx->kvIndex + 2] = (NvU64)data3 | ((NvU64)data4 << 32);
    }
    pCtx->kvIndex += 3;
}

static inline void nvkvSetSeq64_1U(NVKVContext *pCtx, NvU64 idx, NvU64 key1, NvU64 data1)
{
    if ((pCtx->kvIndex + 2) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_SEQ64, idx, key1, 1);
        pCtx->pPtr64[pCtx->kvIndex + 1] = data1;
    }
    pCtx->kvIndex += 2;
}

static inline void nvkvSetSeq64_2U(NVKVContext *pCtx, NvU64 idx, NvU64 key1, NvU64 data1, NvU64 data2)
{
    if ((pCtx->kvIndex + 3) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_SEQ64, idx, key1, 2);
        pCtx->pPtr64[pCtx->kvIndex + 1] = data1;
        pCtx->pPtr64[pCtx->kvIndex + 2] = data2;
    }
    pCtx->kvIndex += 3;
}

static inline void nvkvSetSeq64_3U(NVKVContext *pCtx, NvU64 idx, NvU64 key1, NvU64 data1, NvU64 data2, NvU64 data3)
{
    if ((pCtx->kvIndex + 4) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_SEQ64, idx, key1, 3);
        pCtx->pPtr64[pCtx->kvIndex + 1] = data1;
        pCtx->pPtr64[pCtx->kvIndex + 2] = data2;
        pCtx->pPtr64[pCtx->kvIndex + 3] = data3;
    }
    pCtx->kvIndex += 4;
}

static inline void nvkvSetSeq64_4U(NVKVContext *pCtx, NvU64 idx, NvU64 key1, NvU64 data1, NvU64 data2, NvU64 data3, NvU64 data4)
{
    if ((pCtx->kvIndex + 5) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_SEQ64, idx, key1, 4);
        pCtx->pPtr64[pCtx->kvIndex + 1] = data1;
        pCtx->pPtr64[pCtx->kvIndex + 2] = data2;
        pCtx->pPtr64[pCtx->kvIndex + 3] = data3;
        pCtx->pPtr64[pCtx->kvIndex + 4] = data4;
    }
    pCtx->kvIndex += 5;
}

static inline void nvkvSetArray64(NVKVContext *pCtx, NvU64 idx, NvU64 key, const NvU64 *pArray, NvU64 count)
{
    if ((pCtx->kvIndex + 1 + count) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_ARRAY64, idx, key, count);
        portMemCopy(&pCtx->pPtr64[pCtx->kvIndex + 1], (size_t)(count * sizeof(NvU64)), pArray, (size_t)(count * sizeof(NvU64)));
    }
    pCtx->kvIndex += 1 + count;
}

static inline void nvkvSetArray32(NVKVContext *pCtx, NvU64 idx, NvU64 key, const NvU32 *pArray, NvU64 count)
{
    NvU64 size = count * sizeof(NvU32);
    NvU64 count64 = NV_ALIGN_UP64(size, sizeof(NvU64)) / sizeof(NvU64);
    NvU64 padSize = count64 * sizeof(NvU64) - size;
    if ((pCtx->kvIndex + 1 + count64) <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_ARRAY32, idx, key, count);
        NvU32 *pDst = (NvU32 *)&pCtx->pPtr64[pCtx->kvIndex + 1];
        portMemCopy(pDst, (size_t)size, pArray, (size_t)size);
        if (padSize > 0)
            portMemSet(&pDst[count], 0, (size_t)padSize);
    }
    pCtx->kvIndex += 1 + count64;
}

static inline void nvkvSetArray8(NVKVContext *pCtx, NvU64 idx, NvU64 key, const NvU8 *pArray, NvU64 count)
{
    NvU64 size = count * sizeof(NvU8);
    NvU64 count64 = NV_ALIGN_UP64(size, sizeof(NvU64)) / sizeof(NvU64);
    NvU64 padSize = count64 * sizeof(NvU64) - size;
    if (pCtx->kvIndex + 1 + count64 <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_ARRAY8, idx, key, count);
        NvU8 *pDst = (NvU8 *)&pCtx->pPtr64[pCtx->kvIndex + 1];
        portMemCopy(pDst, (size_t)size, pArray, (size_t)size);
        if (padSize > 0)
            portMemSet(&pDst[count], 0, (size_t)padSize);
    }
    pCtx->kvIndex += 1 + count64;
}

static inline void nvkvSetString8(NVKVContext *pCtx, NvU64 idx, NvU64 key, const NvU8 *pString, NvU64 maxLength)
{
    NvU64 stringLength = portStringLengthSafe((const char *)pString, maxLength);
    NvU64 size = stringLength + 1; // +1 for the null terminator we always send (and include in the array count)
    NvU64 count64 = NV_ALIGN_UP64(size, sizeof(NvU64)) / sizeof(NvU64);
    NvU64 zeroPadSize = count64 * sizeof(NvU64) - stringLength;
    if (pCtx->kvIndex + 1 + count64 <= pCtx->kvLimit)
    {
        pCtx->pPtr64[pCtx->kvIndex + 0] = NVKV_KEY_COUNT(_ARRAY8, idx, key, size);
        NvU8 *pDst = (NvU8 *)&pCtx->pPtr64[pCtx->kvIndex + 1];
        // This copies just the string data w/o the null terminator.
        portMemCopy(pDst, (size_t)stringLength, pString, (size_t)stringLength);
        // This writes the null terminator and any required padding to ensure the array is 8-byte aligned.
        portMemSet(&pDst[stringLength], 0, (size_t)zeroPadSize);
    }
    pCtx->kvIndex += 1 + count64;
}

/*
** NVKV_PREFIX_ may optionally be set as a prefix string for all key names passed to the NVKV_SET_* macros.
** If not set then the original token passed to the macros will be used (which may itself also be a macro)
** This is useful to make code more readable when setting a large number of keys with a common prefix.
**
** Note that an underscore will be added between the prefix and the key name provided to the macros.
**
*/

#define NVKV_PREFIX_
#define _NVKV_CAT(a,b)        a##b
#define _NVKV_XCAT(a,b)       _NVKV_CAT(a,b)
#define _NVKV_PREFIX_KEY(a,b,c) _NVKV_XCAT(_NVKV_CAT(a,b),c)
#define NVKV_PREFIX_KEY(_key) _NVKV_PREFIX_KEY(NVKV_PREFIX, _, _key)

/* Handy versions of REF_ macros that use the NVKV_PREFIX_ macro */
#define NVKV_NUM(a, b)      REF_NUM(NVKV_PREFIX_KEY(a), b)
#define NVKV_VAL(a, b)      REF_VAL(NVKV_PREFIX_KEY(a), b)
#define NVKV_DEF(a, b)      (((NVKV_PREFIX_KEY(a ## b))&DRF_MASK(NVKV_PREFIX_KEY(a)))<<DRF_SHIFT(NVKV_PREFIX_KEY(a)))
#define NVKV_SHIFTMASK(a)   DRF_SHIFTMASK(NVKV_PREFIX_KEY(a))

#define NVKV_NUM64(a, b)    REF_NUM64(NVKV_PREFIX_KEY(a), b)
#define NVKV_VAL64(a, b)    REF_VAL64(NVKV_PREFIX_KEY(a), b)
#define NVKV_DEF64(a, b)    (((NVKV_PREFIX_KEY(a ## b))&DRF_MASK64(NVKV_PREFIX_KEY(a)))<<DRF_SHIFT64(NVKV_PREFIX_KEY(a)))
#define NVKV_SHIFTMASK64(a) DRF_SHIFTMASK64(NVKV_PREFIX_KEY(a))

/* Top-level macros: key-sequence assertions stay in macros for compile-time optimization. */

/*
** All of the NVKV_SET_* macros that write to memory must be wrapped in a NVKV_BEGIN() and NVKV_END() pair.
**
** NVKV_BEGIN() sets up a context for encoding.  Arguments:
**
** - ptr64:   pointer to the base of the NvU64 buffer.  May be NULL (encoding still advances index but
**            no data will be written to the buffer.
** - kvIndex: The starting index within the buffer to begin encoding.  This is updated as encoding
**            continues.
** - kvLimit: maximum number of NvU64 values available for encoding.  The encoder functions peform
**            bounds checks against this value and will not write beyond the limit.  If an overflow
**            occurs there is no guarantee for how many NvU64s have been written to and may be less
**            than kvLimit.
**
** The newly initialized context is returned by value.
**
** NVKV_END() clears pPtr64 in the context to NULL and resets kvLimit to 0;  Returns the kvIndex value
**   after encoding.
*/
#define NVKV_BEGIN(ptr64, kvStartIndex, kvLimit) nvkvBegin((ptr64), (kvStartIndex), (kvLimit))

#define NVKV_END(pCtx) nvkvEnd(pCtx)

#define NVKV_SET_SEQ32_1U(pCtx, Idx, Key1, Data1) \
    nvkvSetImm32((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1))

#define NVKV_SET_SEQ32_2U(pCtx, Idx, Key1, Data1, Key2, Data2)                      \
    do {                                                                            \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key2)) - (NVKV_PREFIX_KEY(Key1)) == 1);        \
        nvkvSetSeq32_2U((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1), (Data2), 2); \
    } while(0)

#define NVKV_SET_SEQ32_3U(pCtx, Idx, Key1, Data1, Key2, Data2, Key3, Data3)                         \
    do {                                                                                            \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key2)) - (NVKV_PREFIX_KEY(Key1)) == 1);                        \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key3)) - (NVKV_PREFIX_KEY(Key2)) == 1);                        \
        nvkvSetSeq32_4U((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1), (Data2), (Data3), (0), 3);   \
    } while(0)

#define NVKV_SET_SEQ32_4U(pCtx, Idx, Key1, Data1, Key2, Data2, Key3, Data3, Key4, Data4)                \
    do {                                                                                                \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key2)) - (NVKV_PREFIX_KEY(Key1)) == 1);                            \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key3)) - (NVKV_PREFIX_KEY(Key2)) == 1);                            \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key4)) - (NVKV_PREFIX_KEY(Key3)) == 1);                            \
        nvkvSetSeq32_4U((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1), (Data2), (Data3), (Data4), 4);   \
    } while(0)

#define NVKV_SET_SEQ64_1U(pCtx, Idx, Key1, Data1)                                   \
    do {                                                                            \
        nvkvSetSeq64_1U((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1));             \
    } while(0)

#define NVKV_SET_SEQ64_2U(pCtx, Idx, Key1, Data1, Key2, Data2)                      \
    do {                                                                            \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key2)) - (NVKV_PREFIX_KEY(Key1)) == 1);        \
        nvkvSetSeq64_2U((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1), (Data2));    \
    } while(0)

#define NVKV_SET_SEQ64_3U(pCtx, Idx, Key1, Data1, Key2, Data2, Key3, Data3)                 \
    do {                                                                                    \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key2)) - (NVKV_PREFIX_KEY(Key1)) == 1);                \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key3)) - (NVKV_PREFIX_KEY(Key2)) == 1);                \
        nvkvSetSeq64_3U((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1), (Data2), (Data3));   \
    } while(0)

#define NVKV_SET_SEQ64_4U(pCtx, Idx, Key1, Data1, Key2, Data2, Key3, Data3, Key4, Data4)            \
    do {                                                                                            \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key2)) - (NVKV_PREFIX_KEY(Key1)) == 1);                        \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key3)) - (NVKV_PREFIX_KEY(Key2)) == 1);                        \
        NVKV_ASSERT((NVKV_PREFIX_KEY(Key4)) - (NVKV_PREFIX_KEY(Key3)) == 1);                        \
        nvkvSetSeq64_4U((pCtx), (Idx), NVKV_PREFIX_KEY(Key1), (Data1), (Data2), (Data3), (Data4));  \
    } while(0)

#define NVKV_SET_ARRAY64(pCtx, Idx, Key, Array, Count) \
    nvkvSetArray64((pCtx), (Idx), NVKV_PREFIX_KEY(Key), (Array), (Count))

#define NVKV_SET_ARRAY32(pCtx, Idx, Key, Array, Count) \
    nvkvSetArray32((pCtx), (Idx), NVKV_PREFIX_KEY(Key), (Array), (Count))

#define NVKV_SET_ARRAY8(pCtx, Idx, Key, Array, Count) \
    nvkvSetArray8((pCtx), (Idx), NVKV_PREFIX_KEY(Key), (Array), (Count))

#define NVKV_SET_STRING8(pCtx, Idx, Key, StringVar) \
    nvkvSetString8((pCtx), (Idx), NVKV_PREFIX_KEY(Key), ((NvU8 *)StringVar), sizeof(StringVar))

#define NVKV_SET_STRING8_LEN(pCtx, Idx, Key, StringVar, Length) \
    nvkvSetString8((pCtx), (Idx), NVKV_PREFIX_KEY(Key), ((NvU8 *)StringVar), (Length))

#define NVKV_SET_DATA(pCtx, Data) nvkvSetData((pCtx), (Data))

/* Set by value (ARRAY8 of sizeof(Value) bytes). */
#define NVKV_SET_VALUE(pCtx, Idx, Key, Value) \
    nvkvSetArray8((pCtx), (Idx), NVKV_PREFIX_KEY(Key), (const NvU8 *)&(Value), sizeof(Value))

enum
{
    NVKV_VALUE_TYPE_U8  = 0,
    NVKV_VALUE_TYPE_U32 = 2,
    NVKV_VALUE_TYPE_U64 = 3,
};

/*
** The NVKVValue structure is used to pass the value data, count, and type to the key handler function. The
** callback function may not assume that the data pointers are valid beyond the lifetime of the callback function,
** or are within the bounds of the array passed to nvkvDecode().
*/
typedef struct NVKVValue
{
    union
    {
        const NvU8  *pU8;
        const NvU32 *pU32;
        const NvU64 *pU64;
    } valueData;
    NvU32 valueCount;                // Number of values associated with the key. For NVKV_OPCODE_IMM32 and
                                     // NVKV_OPCODE_SEQ32/64, this will be 1. For NVKV_OPCODE_ARRAY8/32/64, this
                                     // will be the number of elements in the array.  If the count is 0, then
                                     // the valueData pointer will be NULL.
    NvU32 valueType;                 // NVKV_VALUE_TYPE_U8, NVKV_VALUE_TYPE_U32, NVKV_VALUE_TYPE_U64.
    NvU64 decodeIndex;               // Index of the current instruction being decoded, helpful for error reporting.
} NVKVValue;

/**
 * @brief The key handler function is called for each key/value pair in the encoded KV array.
 *
 * @param[in] index  The key's index
 * @param[in] key    The key
 * @param[in] pValue A pointer to the NVKVValue structure describing the current values associated with
 *                   the key.
 * @param[in] ctx    Generic context argument passed from nvkvDecode()
 *
 * @return Return NV_TRUE to continue decoding, NV_FALSE to halt decoding.
 */
typedef NvBool (*NVKVKeyHandler)(const NvU64 keyIndex, const NvU64 key, const NVKVValue *pValue, void *ctx);

/**
 * @brief Decode NVKV encoded buffer, invoking the key handler function for each key/value pair.
 *
 * @param[in] keyHandler The function to invoke for each key/value pair.
 * @param[in] pKV        The NVKV encoded buffer to decode.  The buffer must be 8 byte aligned.
 * @param[in] maxKVCount The maximum number of NvU64 elements in the pKV buffer.
 * @param[in] ctx        A pointer to the context to pass to the key handler function.  It is passed through
 *                       to the handler function without modification and is never dereferenced (may be NULL).
 *
 * @return NV_OK if the buffer was decoded successfully, NV_ERR_OUT_OF_RANGE if the encoded instructions
 *         would have required accessing memory beyond the end of the buffer, NV_ERR_INVALID_COMMAND if an
 *         invalid/unsupported opcode is encountered.   Returns NV_ERR_INVALID_ARGUMENT if the key handler is
 *         NULL or if maxKVCount is greater than zero and pKV is NULL.
 */
extern NV_STATUS nvkvDecode(NVKVKeyHandler keyHandler, const NvU64 *pKV, const NvU32 maxKVCount, void * const ctx);

typedef struct NVKVDecodeContext {
    NvU64  keyIndex;
    NvU64  key;
    const NVKVValue *pValue;
} NVKVDecodeContext;

/* Inline helper functions */
static inline NVKVDecodeContext nvkvDecodeInitContext(NvU64 keyIndex, NvU64 key, const NVKVValue *pValue)
{
    NVKVDecodeContext ctx;
    ctx.keyIndex = keyIndex;
    ctx.key = key;
    ctx.pValue = pValue;
    return ctx;
}

/*
** Helper macros for use in implementing key handler functions.  There is no hard requirement
** to use them, but they can help eliminate a lot of boilerplate code and makes decoding almost
** symmetric with encoding.
*/
#define NVKV_DECODE_INIT(keyIndex, key, pValue) nvkvDecodeInitContext(keyIndex, key, pValue)

#define NVKV_DECODE_SWITCH(pCtx)                                    \
    do {                                                            \
        switch((pCtx)->key) {

#define NVKV_DECODE_SWITCH_END(pCtx)                                \
        }                                                           \
        return NV_TRUE;                                             \
    } while (0)

#define NVKV_DECODE_SWITCH_END_CHECK_OK(pCtx, status)               \
        }                                                           \
        if(status != NV_OK) {                                       \
            continue;                                               \
        }                                                           \
        return NV_TRUE;                                             \
    } while (0)

#define _NVKV_VALIDATE_SINGLE_VALUE(pCtx, ValueType)                \
    if ((pCtx)->keyIndex != 0 ||                                    \
        (pCtx)->pValue->valueCount != 1 ||                          \
        (pCtx)->pValue->valueType != NVKV_VALUE_TYPE_##ValueType) { \
        continue;                                                   \
    }

#define _NVKV_VALIDATE_INDEXED_VALUE(pCtx, ValueType, MaxIdx)       \
    if ((pCtx)->keyIndex >= MaxIdx ||                               \
        (pCtx)->pValue->valueCount != 1 ||                          \
        (pCtx)->pValue->valueType != NVKV_VALUE_TYPE_##ValueType) { \
        continue;                                                   \
    }

#define NVKV_CASE(pCtx, Key, ...)                                   \
    case NVKV_PREFIX_KEY(Key): {                                    \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_VAR(pCtx, ValueType, Key, Var, ...)               \
    case NVKV_PREFIX_KEY(Key): {                                    \
        _NVKV_VALIDATE_SINGLE_VALUE(pCtx, ValueType);               \
        Var = (pCtx)->pValue->valueData.p##ValueType[0];            \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_LOCAL(pCtx, ValueType, Key, Var, ...)             \
    case NVKV_PREFIX_KEY(Key): {                                    \
        _NVKV_VALIDATE_SINGLE_VALUE(pCtx, ValueType);               \
        Nv ## ValueType Var = (pCtx)->pValue->valueData.p##ValueType[0]; \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_MAX(pCtx, ValueType, Key, Var, MaxValue, ...)     \
    case NVKV_PREFIX_KEY(Key): {                                    \
        _NVKV_VALIDATE_SINGLE_VALUE(pCtx, ValueType);               \
        if((pCtx)->pValue->valueData.p##ValueType[0] > MaxValue) {  \
            continue;                                               \
        }                                                           \
        Var = (pCtx)->pValue->valueData.p##ValueType[0];            \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_IDX(pCtx, ValueType, Key, Var, MaxIdx, ...)       \
    case NVKV_PREFIX_KEY(Key): {                                    \
        _NVKV_VALIDATE_INDEXED_VALUE(pCtx, ValueType, MaxIdx);      \
        Var = (pCtx)->pValue->valueData.p##ValueType[0];            \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_IDX_LOCAL(pCtx, ValueType, Key, Var, MaxIdx, ...) \
    case NVKV_PREFIX_KEY(Key): {                                    \
        _NVKV_VALIDATE_INDEXED_VALUE(pCtx, ValueType, MaxIdx);      \
        Nv ## ValueType Var = (pCtx)->pValue->valueData.p##ValueType[0];\
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_ARRAY8(pCtx, Key, Var, ...)                       \
    case NVKV_PREFIX_KEY(Key): {                                    \
        if ((pCtx)->keyIndex != 0 ||                                \
            (pCtx)->pValue->valueCount > sizeof(Var) ||             \
            (pCtx)->pValue->valueType != NVKV_VALUE_TYPE_U8) {      \
            continue;                                               \
        }                                                           \
        portMemCopy(Var, sizeof(Var),                               \
                    (pCtx)->pValue->valueData.pU8,                  \
                    (pCtx)->pValue->valueCount);                    \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_ARRAY32(pCtx, Key, Var, ...)                      \
    case NVKV_PREFIX_KEY(Key): {                                    \
        if ((pCtx)->keyIndex != 0 ||                                \
            (pCtx)->pValue->valueCount > sizeof(Var) / sizeof(NvU32) || \
            (pCtx)->pValue->valueType != NVKV_VALUE_TYPE_U32) {     \
            continue;                                               \
        }                                                           \
        portMemCopy(Var, sizeof(Var),                               \
                    (pCtx)->pValue->valueData.pU32,                 \
                    (pCtx)->pValue->valueCount * sizeof(NvU32));    \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_ARRAY64(pCtx, Key, Var, ...)                      \
    case NVKV_PREFIX_KEY(Key): {                                    \
        if ((pCtx)->keyIndex != 0 ||                                \
            (pCtx)->pValue->valueCount > sizeof(Var) / sizeof(NvU64) || \
            (pCtx)->pValue->valueType != NVKV_VALUE_TYPE_U64) {     \
            continue;                                               \
        }                                                           \
        portMemCopy(Var, sizeof(Var),                               \
                    (pCtx)->pValue->valueData.pU64,                 \
                    (pCtx)->pValue->valueCount * sizeof(NvU64));    \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#define NVKV_CASE_STRING8(pCtx, Key, Var, ...)                      \
    case NVKV_PREFIX_KEY(Key): {                                    \
        if ((pCtx)->keyIndex != 0 ||                                \
            (pCtx)->pValue->valueCount < 1 ||                       \
            (pCtx)->pValue->valueType != NVKV_VALUE_TYPE_U8) {      \
            continue;                                               \
        }                                                           \
        portStringCopy((char *)Var, sizeof(Var),                    \
                       (const char *)(pCtx)->pValue->valueData.pU8, \
                       (pCtx)->pValue->valueCount);                 \
        __VA_ARGS__;                                                \
        break;                                                      \
    }

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _NVKV_H_
