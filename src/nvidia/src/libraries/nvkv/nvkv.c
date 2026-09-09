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

#include "nvkv/nvkv.h"
#include "utils/nvprintf.h"

NV_STATUS nvkvDecode(NVKVKeyHandler keyHandler, const NvU64 *pKV, const NvU32 maxKVCount, void *const ctx)
{
    NvU64 kvIndex = 0;

    // It's ok/safe to call nvkvDecode with NULL if count is zero.
    NV_ASSERT_OR_RETURN(maxKVCount == 0 || pKV != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(keyHandler != NULL, NV_ERR_INVALID_ARGUMENT);

    while (kvIndex < maxKVCount)
    {
        NvU64 decodeIndex = kvIndex;
        NvU64 kvData = pKV[kvIndex++];  // Always consume first value.

        /* Extract fields common to all opcode types */
        NvU64 key = REF_VAL64(NVKV_KEY, kvData);
        NvU64 opcode = REF_VAL64(NVKV_OPCODE, kvData);
        NvU64 index = REF_VAL64(NVKV_IDX12, kvData);

        switch (opcode)
        {
            case NVKV_OPCODE_IMM32:
            {
                NvU32 value = REF_VAL64(NVKV_IMM32_VALUE, kvData);
                NVKVValue currentValue;
                currentValue.valueType = NVKV_VALUE_TYPE_U32;
                currentValue.valueCount = 1;
                currentValue.valueData.pU32 = &value;
                currentValue.decodeIndex = decodeIndex;
                if (keyHandler(index, key, &currentValue, ctx) == NV_FALSE)
                {
                    return NV_OK;
                }
                break;
            }

            case NVKV_OPCODE_SEQ32:
            {
                NvU64 valueCount = REF_VAL64(NVKV_COUNT32, kvData);
                NvU64 count64 = NV_ALIGN_UP64(valueCount*sizeof(NvU32), sizeof(NvU64)) / sizeof(NvU64);
                NvU64 limit = kvIndex+count64;
                NvU32 valueIndex = 0;

                if (limit > maxKVCount)
                {
                    return NV_ERR_OUT_OF_RANGE;
                }

                const NvU32 *pValue32 = (const NvU32 *)&pKV[kvIndex];
                while (valueIndex < valueCount)
                {
                    NVKVValue currentValue;
                    currentValue.valueType = NVKV_VALUE_TYPE_U32;
                    currentValue.valueCount = 1;
                    currentValue.valueData.pU32 = &pValue32[valueIndex];
                    currentValue.decodeIndex = decodeIndex;
                    if (keyHandler(index, key, &currentValue, ctx) == NV_FALSE)
                    {
                        return NV_OK;
                    }
                    valueIndex++;
                    key++;
                }
                kvIndex += count64;
                break;
            }

            case NVKV_OPCODE_SEQ64:
            {
                NvU64 valueCount = REF_VAL64(NVKV_COUNT32, kvData);
                NvU64 limit = kvIndex+valueCount;
                NvU32 valueIndex = 0;

                if (limit > maxKVCount)
                {
                    return NV_ERR_OUT_OF_RANGE;
                }

                const NvU64 *pValue64 = (const NvU64 *)&pKV[kvIndex];
                while (valueIndex < valueCount)
                {
                    NVKVValue currentValue;
                    currentValue.valueType = NVKV_VALUE_TYPE_U64;
                    currentValue.valueCount = 1;
                    currentValue.valueData.pU64 = &pValue64[valueIndex];
                    currentValue.decodeIndex = decodeIndex;
                    if (keyHandler(index, key, &currentValue, ctx) == NV_FALSE)
                    {
                        return NV_OK;
                    }
                    valueIndex++;
                    key++;
                }
                kvIndex += valueCount;
                break;
            }

            case NVKV_OPCODE_ARRAY8:
            {
                NvU64 valueCount = REF_VAL64(NVKV_COUNT32, kvData);
                NvU64 count64 = NV_ALIGN_UP64(valueCount*sizeof(NvU8), sizeof(NvU64)) / sizeof(NvU64);
                NvU64 limit = kvIndex+count64;

                if (limit > maxKVCount)
                {
                    return NV_ERR_OUT_OF_RANGE;
                }

                // If count is 0 we pass a NULL pointer since the callee should not access memory.
                NVKVValue currentValue;
                currentValue.valueType = NVKV_VALUE_TYPE_U8;
                currentValue.valueCount = (NvU32)valueCount;
                currentValue.valueData.pU8 = valueCount ? (const NvU8 *)&pKV[kvIndex] : NULL;
                currentValue.decodeIndex = decodeIndex;
                if (keyHandler(index, key, &currentValue, ctx) == NV_FALSE)
                {
                    return NV_OK;
                }
                kvIndex += count64;
                break;
            }

            case NVKV_OPCODE_ARRAY32:
            {
                NvU64 valueCount = REF_VAL64(NVKV_COUNT32, kvData);
                NvU64 count64 = NV_ALIGN_UP64(valueCount*sizeof(NvU32), sizeof(NvU64)) / sizeof(NvU64);
                NvU64 limit = kvIndex+count64;

                if (limit > maxKVCount)
                {
                    return NV_ERR_OUT_OF_RANGE;
                }

                // If count is 0 we pass a NULL pointer since the callee should not access memory.
                NVKVValue currentValue;
                currentValue.valueType = NVKV_VALUE_TYPE_U32;
                currentValue.valueCount = (NvU32)valueCount;
                currentValue.valueData.pU32 = valueCount ? (const NvU32 *)&pKV[kvIndex] : NULL;
                currentValue.decodeIndex = decodeIndex;
                if (keyHandler(index, key, &currentValue, ctx) == NV_FALSE)
                {
                    return NV_OK;
                }
                kvIndex += count64;
                break;
            }

            case NVKV_OPCODE_ARRAY64:
            {
                NvU64 valueCount = REF_VAL64(NVKV_COUNT32, kvData);
                NvU64 limit = kvIndex+valueCount;

                if (limit > maxKVCount)
                {
                    return NV_ERR_OUT_OF_RANGE;
                }

                // If count is 0 we pass a NULL pointer since the callee should not access memory.
                NVKVValue currentValue;
                currentValue.valueType = NVKV_VALUE_TYPE_U64;
                currentValue.valueCount = (NvU32)valueCount;
                currentValue.valueData.pU64 = valueCount ? (const NvU64 *)&pKV[kvIndex] : NULL;
                currentValue.decodeIndex = decodeIndex;
                if (keyHandler(index, key, &currentValue, ctx) == NV_FALSE)
                {
                    return NV_OK;
                }
                kvIndex += valueCount;
                break;
            }

            /* If we land here then either the opcode was invalid */
            default:
            {
                NV_PRINTF(LEVEL_ERROR,"nvkvDecode() invalid instruction: %016llx\n",kvData);
                return NV_ERR_INVALID_COMMAND;
            }
        }
    }

    return NV_OK;
}
