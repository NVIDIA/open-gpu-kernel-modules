/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080boardobj.finn
//



#include "ctrl/ctrl2080/ctrl2080base.h"
#include "ctrl/ctrl2080/ctrl2080boardobjgrpclasses.h"

/*!
 * @brief   NV20_SUBDEVICE_XX BOARDOBJ-related control commands and parameters.
 *
 * Base structures in RMCTRL equivalent to BOARDOBJ/BOARDOBJGRP in RM. NV2080
 * structs in this file carry info w.r.t BOARDOBJ/BOARDOBJGRP.
 */

/*!
 * @brief Type for representing an index of a BOARDOBJ within a
 * BOARDOBJGRP.  This type can also represent the number of elements
 * within a BOARDOBJGRP or the number of bits in a BOARDOBJGRPMASK.
 */
typedef NvU16 NvBoardObjIdx;

/*!
 * @brief Type for representing an index into a mask element within a
 * BOARDOBJGRPMASK to a @ref NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE.
 */
typedef NvU16 NvBoardObjMaskIdx;

/*!
 * @brief   Primitive type which a BOARDOBJGRPMASK is composed of.
 *
 * For example, a 32 bit mask will have one of these elements and a 256 bit
 * mask will have eight.
 */
typedef NvU32 NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE;

/*!
 * @brief   Min value a single BOARDOBJGRPMASK element can hold.
 *
 * @note    Must be kept in sync with @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_ELEMENT_MIN           NV_U32_MIN

/*!
 * @brief   Max value a single BOARDOBJGRPMASK element can hold.
 *
 * @note    Must be kept in sync with @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_ELEMENT_MAX           NV_U32_MAX

/*!
 * @brief   Number of bits in a the NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE type.
 *
 * This exists to eliminate the assumption that 32-bits is the width of
 * NV2080_CTRL_BOARDOBJGRP_MASK primitive element.
 *
 * @note    Left shift by 3 (multiply by 8) converts the sizeof in bytes to the
 * number of bits in our primitive/essential mask type.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_BIT_SIZE 32

/*!
 * @brief   Value for an invalid Board Object index.
 *
 * This value should only be used directly for input to and output
 * from BOARDOBJ and BOARDOBJGRP code.
 *
 * @note This define should not be referenced directly in any
 * implementing object code.  Instead, each object should define it's
 * own IDX_INVALID macro and alias it to whatever size fits their
 * specific index storage type.
 * For example, many objects still store indexes as NvU8 (because the
 * GRPs are either _E32 or _E255) while others store as NvBoardObjIdx
 * (currently aliased to NvU16), so they should alias to a correct
 * type.
 */
#define NV2080_CTRL_BOARDOBJ_IDX_INVALID                   NV_U16_MAX

/*!
 * @brief   Value for an invalid Board Object index.
 *
 * This value encodes an invalid/unsupported BOARDOBJ index for an
 * 8-bit value.  This should be used within by any legacy appcode
 * implementing BOARDOBJGRP which stores/encodes indexes as 8-bit
 * values.
 *
 * All new groups should use @ref NV2080_CTRL_BOARDOBJ_IDX_INVALID.
 *
 * @note This define should not be referenced directly in any
 * implementing object code.  Instead, each object should define it's
 * own IDX_INVALID macro and alias it to whatever size fits their
 * specific index storage type.
 * For example, many objects still store indexes as NvU8 (because the
 * GRPs are either _E32 or _E255) while others store as NvU16 (for
 * GRPs larger than _E255), so they should alias to a correct type.
 */
#define NV2080_CTRL_BOARDOBJ_IDX_INVALID_8BIT              NV_U8_MAX

/*!
 * @brief   Computes the array index of a NV2080_CTRL_BOARDOBJGRP_MASK element
 *          storing requested bit.
 *
 * @note    Designed to be used in conjunction with @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_OFFSET.
 *
 * @param[in]   _bit    Index of a bit within a bit mask.
 *
 * @return  Array index of mask element containing @ref _bit.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_INDEX(_bit)                  \
    ((_bit) / NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_BIT_SIZE)

/*!
 * @brief   Computes bit-position within NV2080_CTRL_BOARDOBJGRP_MASK element
 *          corresponding to requested bit.
 *
 * @note    Designed to be used in conjunction with @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_INDEX.
 *
 * @param[in]   _bit    Index of a bit within a bit mask.
 *
 * @return  Offset (in bits) within a mask element for @ref _bit.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_OFFSET(_bit)                 \
    ((_bit) % NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_BIT_SIZE)

/*!
 * @brief   Computes the size of an array of NV2080_CTRL_BOARDOBJGRP_MASK
 *          elements that can store all mask's bits.
 *
 * @param[in]   _bits   Size of the mask in bits.
 *
 * @return  Number of array elements needed to store @ref _bits number of bits.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_DATA_SIZE(_bits)                          \
    (NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_INDEX((_bits) - 1U) + 1U)


/*!
 * @brief   Number of elements that are in the NV2080_CTRL_BOARDOBJGRP_MASK base
 *          class.
 *
 * @note    "START_SIZE" is used here to represent the size of the mask that
 *          derived classes must build up from. See @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E32, @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E255, @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E512, @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E1024, @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E2048.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_ARRAY_START_SIZE      1U

/*!
 * @brief   Macro used to determine the number of NV2080_CTRL_BOARDOBJGRP_MASK
 *          elements required to extend the base number of elements in a mask,
 *          @ref NV2080_CTRL_BOARDOBJGRP_MASK_ARRAY_START_SIZE.
 *
 * @note    Used in order to avoid dynamic memory allocation and related
 *          code/data waste as well as two levels of indirection while accessing
 *          the data bits stored in an array of @ref
 *          NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_BIT_SIZE sized words. The
 *          NV2080_CTRL_BOARDOBJGRP_MASK super-class array's size should be
 *          zero and actual data should be completely stored in child's array.
 *          Since most compilers reject structures with zero-sized arrays first
 *          element word was moved to the super-class and remaining array
 *          elements to child class.
 *
 * @param[in]   _bits   Total number of bits to be represented in the
 *                      NV2080_CTRL_BOARDOBJGRP_MASK extending mask class.
 *
 * @return  Number of additional mask elements that must be allocated in order
 *          to extend the NV2080_CTRL_BOARDOBJGRP_MASK base class.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_ARRAY_EXTENSION_SIZE(_bits)               \
    (NV2080_CTRL_BOARDOBJGRP_MASK_DATA_SIZE(_bits) -                           \
     (NV2080_CTRL_BOARDOBJGRP_MASK_ARRAY_START_SIZE))

/*!
 * @brief   Macro to set input bit in NV2080_CTRL_BOARDOBJGRP_MASK.
 *
 * @param[in]   _pMask      PBOARDOBJGRPMASK of mask.
 * @param[in]   _bitIdx     Index of the target bit within the mask.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_BIT_SET(_pMask, _bitIdx)                  \
    do {                                                                       \
        (_pMask)->pData[                                                       \
            NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_INDEX(_bitIdx)] |=       \
                NVBIT_TYPE(                                                    \
                    NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_OFFSET(_bitIdx), \
                    NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE);                   \
    } while (NV_FALSE)

/*!
 * @brief   Macro to clear input bit in NV2080_CTRL_BOARDOBJGRP_MASK.
 *
 * @param[in]   _pMask      PBOARDOBJGRPMASK of mask.
 * @param[in]   _bitIdx     Index of the target bit within the mask.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_BIT_CLR(_pMask, _bitIdx)                  \
    do {                                                                       \
        (_pMask)->pData[                                                       \
            NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_INDEX(_bitIdx)] &=       \
                ~NVBIT_TYPE(                                                   \
                    NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_OFFSET(_bitIdx), \
                    NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE);                   \
    } while (NV_FALSE)

/*!
 * @brief   Macro to test input bit in NV2080_CTRL_BOARDOBJGRP_MASK.
 *
 * @param[in]   _pMask      PBOARDOBJGRPMASK of mask.
 * @param[in]   _bitIdx     Index of the target bit within the mask.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_BIT_GET(_pMask, _bitIdx)                  \
    (((_pMask)->pData[NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_INDEX(_bitIdx)]\
      & NVBIT_TYPE(NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_OFFSET(_bitIdx),  \
                   NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE)) != 0U)

/*!
 * @brief   Not to be called directly. Helper macro allowing simple iteration
 *          over bits set in a NV2080_CTRL_BOARDOBJGRP_MASK.
 *
 * @param[in]       _maxObjects
 *     Maximum number of objects/bits in BOARDOJBGRP and its
 *     NV2080_CTRL_BOARDOBJGRP_MASK.
 * @param[in,out]   _index
 *     lvalue that is used as a bit index in the loop (can be declared
 *     as any NvU* or NvS* variable).
 *     CRPTODO - I think we need to revisit this.  Signed types of
 *     size <= sizeof(NvBoardObjIdx) can't work.
 * @param[in]       _pMask
 *     Pointer to NV2080_CTRL_BOARDOBJGRP_MASK over which to iterate.
 *
 * @note CRPTODO - Follow-on CL will add ct_assert that _index has
 * size >= sizeof(NvBoardObjIdx).
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX(_maxObjects,_index,_pMask) \
{                                                                              \
    for ((_index) = 0; (_index) < (_maxObjects); (_index)++)                   \
    {                                                                          \
        if(!NV2080_CTRL_BOARDOBJGRP_MASK_BIT_GET((_pMask), (_index)))          \
        {                                                                      \
            continue;                                                          \
        }
#define NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX_END                        \
    }                                                                          \
}

/*!
 * @brief   Macro allowing simple iteration over bits set in a
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E32.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E32_FOR_EACH_INDEX(_index,_pMask)         \
    NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX(                               \
        NV2080_CTRL_BOARDOBJGRP_E32_MAX_OBJECTS,_index,_pMask)

/*!
 * @brief   Macro allowing simple iteration over bits set in a
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E255.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E255_FOR_EACH_INDEX(_index,_pMask)        \
    NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX(                               \
        NV2080_CTRL_BOARDOBJGRP_E255_MAX_OBJECTS,_index,_pMask)

/*!
 * @brief   Macro allowing simple iteration over bits set in a
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E512.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E512_FOR_EACH_INDEX(_index,_pMask)        \
    NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX(                               \
        NV2080_CTRL_BOARDOBJGRP_E512_MAX_OBJECTS,_index,_pMask)

/*!
 * @brief   Macro allowing simple iteration over bits set in a
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E1024.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E1024_FOR_EACH_INDEX(_index,_pMask)       \
    NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX(                               \
        NV2080_CTRL_BOARDOBJGRP_E1024_MAX_OBJECTS,_index,_pMask)

/*!
 * @brief   Macro allowing simple iteration over bits set in a
 *          NV2080_CTRL_BOARDOBJGRP_MASK_E2048.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E2048_FOR_EACH_INDEX(_index,_pMask)       \
    NV2080_CTRL_BOARDOBJGRP_MASK_FOR_EACH_INDEX(                               \
        NV2080_CTRL_BOARDOBJGRP_E2048_MAX_OBJECTS,_index,_pMask)
/*!
 * @brief   Not to be called directly. Macro to initialize a
 *          NV2080_CTRL_BOARDOBJGRP_MASK to an empty mask.
 *
 * @param[in]   _pMask      NV2080_CTRL_BOARDOBJGRP_MASK to initialize.
 * @param[in]   _bitSize    NvU8 specifying size of the mask in bits.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_INIT(_pMask,_bitSize)                              \
    do {                                                                                \
        NvU32 *_pData = (_pMask)->pData;                                                \
        NvBoardObjIdx _dataCount = NV2080_CTRL_BOARDOBJGRP_MASK_DATA_SIZE(_bitSize);    \
        NvBoardObjIdx _dataIndex;                                                       \
        for (_dataIndex = 0; _dataIndex < _dataCount; _dataIndex++)                     \
        {                                                                               \
            _pData[_dataIndex] = 0U;                                                    \
        }                                                                               \
    } while (NV_FALSE)

/*!
 * @brief   Macro to initialize NV2080_CTRL_BOARDOBJGRP_MASK_E32 to an empty
 *          mask.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_INIT().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_INIT()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E32_INIT(_pMask)                          \
    NV2080_CTRL_BOARDOBJGRP_MASK_INIT(_pMask,                                  \
        NV2080_CTRL_BOARDOBJGRP_E32_MAX_OBJECTS)

/*!
 * @brief   Macro to initialize NV2080_CTRL_BOARDOBJGRP_MASK_E255 to an empty
 *          mask.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_INIT().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_INIT()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E255_INIT(_pMask)                         \
    NV2080_CTRL_BOARDOBJGRP_MASK_INIT(_pMask,                                  \
        NV2080_CTRL_BOARDOBJGRP_E255_MAX_OBJECTS)

/*!
 * @brief   Macro to initialize NV2080_CTRL_BOARDOBJGRP_MASK_E512 to an empty
 *          mask.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_INIT().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_INIT()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E512_INIT(_pMask)                         \
    NV2080_CTRL_BOARDOBJGRP_MASK_INIT(_pMask,                                  \
        NV2080_CTRL_BOARDOBJGRP_E512_MAX_OBJECTS)

/*!
 * @brief   Macro to initialize NV2080_CTRL_BOARDOBJGRP_MASK_E1024 to an empty
 *          mask.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_INIT().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_INIT()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E1024_INIT(_pMask)                        \
    NV2080_CTRL_BOARDOBJGRP_MASK_INIT(_pMask,                                  \
        NV2080_CTRL_BOARDOBJGRP_E1024_MAX_OBJECTS)

/*!
 * @brief   Macro to initialize NV2080_CTRL_BOARDOBJGRP_MASK_E2048 to an empty
 *          mask.
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_INIT().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_INIT()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E2048_INIT(_pMask)                        \
    NV2080_CTRL_BOARDOBJGRP_MASK_INIT(_pMask,                                  \
        NV2080_CTRL_BOARDOBJGRP_E2048_MAX_OBJECTS)

/*!
 * @brief   Not to be called directly. Macro to perform a bitwise AND of a
 *          NV2080_CTRL_BOARDOBJGRP_MASK with another NV2080_CTRL_BOARDOBJGRP_MASK.
 *
 * @param[out]  _pMaskOut    NV2080_CTRL_BOARDOBJGRP_MASK with bitwise and output.
 * @param[in]   _pMask1      NV2080_CTRL_BOARDOBJGRP_MASK to bitwise and.
 * @param[in]   _pMask2      NV2080_CTRL_BOARDOBJGRP_MASK to bitwise and.
 * @param[in]   _bitSize     NvU8 specifying size of the mask in bits.
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_AND(_pMaskOut, _pMask1, _pMask2, _bitSize)         \
    do {                                                                                \
        NvU32 *_pData1   = (_pMask1)->pData;                                            \
        NvU32 *_pData2   = (_pMask2)->pData;                                            \
        NvU32 *_pDataOut = (_pMaskOut)->pData;                                          \
        NvBoardObjIdx _dataCount = NV2080_CTRL_BOARDOBJGRP_MASK_DATA_SIZE(_bitSize);    \
        NvBoardObjIdx _dataIndex;                                                       \
        for (_dataIndex = 0; _dataIndex < _dataCount; _dataIndex++)                     \
        {                                                                               \
            _pDataOut[_dataIndex] = _pData1[_dataIndex] & _pData2[_dataIndex];                                  \
        }                                                                               \
    } while (NV_FALSE)

/*!
 * @brief   Macro to bitwise and a NV2080_CTRL_BOARDOBJGRP_MASK_32 with another NV2080_CTRL_BOARDOBJGRP_MASK_32
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_AND().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_AND()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E32_AND(_pMaskOut, _pMask1, _pMask2)      \
    NV2080_CTRL_BOARDOBJGRP_MASK_AND(_pMaskOut, _pMask1, _pMask2,               \
        NV2080_CTRL_BOARDOBJGRP_E32_MAX_OBJECTS)

/*!
 * @brief   Macro to bitwise and a NV2080_CTRL_BOARDOBJGRP_MASK_255 with another NV2080_CTRL_BOARDOBJGRP_MASK_255
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_AND().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_AND()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E255_AND(_pMaskOut, _pMask1, _pMask2)     \
    NV2080_CTRL_BOARDOBJGRP_MASK_AND(_pMaskOut, _pMask1, _pMask2,               \
        NV2080_CTRL_BOARDOBJGRP_E255_MAX_OBJECTS)

/*!
 * @brief   Macro to bitwise and a NV2080_CTRL_BOARDOBJGRP_MASK_512 with another NV2080_CTRL_BOARDOBJGRP_MASK_512
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_AND().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_AND()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E512_AND(_pMaskOut, _pMask1, _pMask2)     \
    NV2080_CTRL_BOARDOBJGRP_MASK_AND(_pMaskOut, _pMask1, _pMask2,               \
        NV2080_CTRL_BOARDOBJGRP_E512_MAX_OBJECTS)

/*!
 * @brief   Macro to bitwise and a NV2080_CTRL_BOARDOBJGRP_MASK_1024 with another NV2080_CTRL_BOARDOBJGRP_MASK_1024
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_AND().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_AND()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E1024_AND(_pMaskOut, _pMask1, _pMask2)    \
    NV2080_CTRL_BOARDOBJGRP_MASK_AND(_pMaskOut, _pMask1, _pMask2,               \
        NV2080_CTRL_BOARDOBJGRP_E1024_MAX_OBJECTS)

/*!
 * @brief   Macro to bitwise and a NV2080_CTRL_BOARDOBJGRP_MASK_2048 with another NV2080_CTRL_BOARDOBJGRP_MASK_2048
 *
 * Wrapper for @ref NV2080_CTRL_BOARDOBJGRP_MASK_AND().
 *
 * @copydetails NV2080_CTRL_BOARDOBJGRP_MASK_AND()
 */
#define NV2080_CTRL_BOARDOBJGRP_MASK_E2048_AND(_pMaskOut, _pMask1, _pMask2)    \
    NV2080_CTRL_BOARDOBJGRP_MASK_AND(_pMaskOut, _pMask1, _pMask2,               \
        NV2080_CTRL_BOARDOBJGRP_E2048_MAX_OBJECTS)

/*!
 * @brief   Board Object Group Mask base class.
 *
 * Used to unify access to all NV2080_CTRL_BOARDOBJGRP_MASK_E** child classes.
 */
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK {
    /*!
     * @brief   Start with a single element array which is enough to represent
     *          NV2080_CTRL_BOARDOBJGRP_MASK_MASK_ELEMENT_BIT_SIZE bits.
     *
     * @note    Must be the last member of this structure.
     */
    // FINN PORT: The below field is a bit vector!
    // In FINN, bit vectors are arrays of bools and each bool becomes 1 bit when used in an array
    // FINN generates an array of NvU32's on the back end for these bit vectors
    NvU32 pData[1] /* 32 bits */;
} NV2080_CTRL_BOARDOBJGRP_MASK;
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK *PNV2080_CTRL_BOARDOBJGRP_MASK;

/*!
 * @brief   NV2080_CTRL_BOARDOBJGRP_MASK child class capable of storing 32 bits
 *          indexed between 0..31.
 */
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E32 {
    /*!
     * @brief   NV2080_CTRL_BOARDOBJGRP_MASK super-class. Must be the first
     *          member of the structure.
     */
    NV2080_CTRL_BOARDOBJGRP_MASK super;
} NV2080_CTRL_BOARDOBJGRP_MASK_E32;
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E32 *PNV2080_CTRL_BOARDOBJGRP_MASK_E32;

/*!
 * @brief   NV2080_CTRL_BOARDOBJGRP_MASK child class capable of storing 255 bits
 *          indexed between 0..254.
 */
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E255 {
    /*!
     * @brief   NV2080_CTRL_BOARDOBJGRP_MASK super-class. Must be the first
     *          member of the structure.
     */
    NV2080_CTRL_BOARDOBJGRP_MASK super;

    /*!
     * @brief   Continuation of the array of
     *          NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE elements representing the
     *          bit-mask.
     *
     * @note    Must be the second member of the structure.
     */

    // FINN PORT: The below field is a bit vector!
    // In FINN, bit vectors are arrays of bools and each bool becomes 1 bit when used in an array
    // FINN generates an array of NvU32's on the back end for these bit vectors
    NvU32 pDataE255[7] /* 223 bits */;
} NV2080_CTRL_BOARDOBJGRP_MASK_E255;
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E255 *PNV2080_CTRL_BOARDOBJGRP_MASK_E255;

/*!
 * @brief   NV2080_CTRL_BOARDOBJGRP_MASK child class capable of storing 512 bits
 *          indexed between 0..511.
 */
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E512 {
    /*!
     * @brief   NV2080_CTRL_BOARDOBJGRP_MASK super-class. Must be the first
     *          member of the structure.
     */
    NV2080_CTRL_BOARDOBJGRP_MASK super;

    /*!
     * @brief   Continuation of the array of
     *          NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE elements representing the
     *          bit-mask.
     *
     * @note    Must be the second member of the structure.
     */

    // FINN PORT: The below field is a bit vector!
    // In FINN, bit vectors are arrays of bools and each bool becomes 1 bit when used in an array
    // FINN generates an array of NvU32's on the back end for these bit vectors
    NvU32 pDataE512[15] /* 480 bits */;
} NV2080_CTRL_BOARDOBJGRP_MASK_E512;
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E512 *PNV2080_CTRL_BOARDOBJGRP_MASK_E512;

/*!
 * @brief   NV2080_CTRL_BOARDOBJGRP_MASK child class capable of storing 1024 bits
 *          indexed between 0..1023.
 */
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E1024 {
    /*!
     * @brief   NV2080_CTRL_BOARDOBJGRP_MASK super-class. Must be the first
     *          member of the structure.
     */
    NV2080_CTRL_BOARDOBJGRP_MASK super;

    /*!
     * @brief   Continuation of the array of
     *          NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE elements representing the
     *          bit-mask.
     *
     * @note    Must be the second member of the structure.
     */

    // FINN PORT: The below field is a bit vector!
    // In FINN, bit vectors are arrays of bools and each bool becomes 1 bit when used in an array
    // FINN generates an array of NvU32's on the back end for these bit vectors
    NvU32 pDataE1024[31] /* 992 bits */;
} NV2080_CTRL_BOARDOBJGRP_MASK_E1024;
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E1024 *PNV2080_CTRL_BOARDOBJGRP_MASK_E1024;

/*!
 * @brief   NV2080_CTRL_BOARDOBJGRP_MASK child class capable of storing 2048 bits
 *          indexed between 0..2047.
 */
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E2048 {
    /*!
     * @brief   NV2080_CTRL_BOARDOBJGRP_MASK super-class. Must be the first
     *          member of the structure.
     */
    NV2080_CTRL_BOARDOBJGRP_MASK super;

    /*!
     * @brief   Continuation of the array of
     *          NV2080_CTRL_BOARDOBJGRP_MASK_PRIMITIVE elements representing the
     *          bit-mask.
     *
     * @note    Must be the second member of the structure.
     */

    // FINN PORT: The below field is a bit vector!
    // In FINN, bit vectors are arrays of bools and each bool becomes 1 bit when used in an array
    // FINN generates an array of NvU32's on the back end for these bit vectors
    NvU32 pDataE2048[63] /* 2016 bits */;
} NV2080_CTRL_BOARDOBJGRP_MASK_E2048;
typedef struct NV2080_CTRL_BOARDOBJGRP_MASK_E2048 *PNV2080_CTRL_BOARDOBJGRP_MASK_E2048;

/*!
 * @brief   Macro to provide the BOARDOBJ type for a given (UNIT, CLASS, TYPE)
 *          combination.
 *
 * @details For arguments (FOO, BAR, BAZ), this macro will return
 *          NV2080_CTRL_FOO_BAR_TYPE_BAZ
 *
 * @param[in]   _unit   The unit.
 * @param[in]   _class  The class.
 * @param[in]   _type   The type.
 *
 * @return  BOARDOBJ object type identifier.
 */
#define NV2080_CTRL_BOARDOBJ_TYPE(_unit, _class, _type)                        \
    NV2080_CTRL_##_unit##_##_class##_TYPE_##_type



/*!
 * @brief   Base structure which describes a BOARDOBJ CLIENT_INFO in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO {
    /*!
     * @brief   BOARDOBJ type.
     *
     * This should be a unique value within the class that the BOARDOBJ belongs.
     */
    NvU8 type;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO;

/*!
 * @brief   Base structure which describes a BOARDOBJ CLIENT_STATUS in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS {
    /*!
     * @brief   BOARDOBJ type.
     *
     * This should be a unique value within the class that the BOARDOBJ belongs.
     */
    NvU8 type;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS;

/*!
 * @brief   Base structure which describes a BOARDOBJ CLIENT_CONTROL in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL {
    /*!
     * @brief   BOARDOBJ type.
     *
     * This should be a unique value within the class that the BOARDOBJ belongs.
     */
    NvU8 type;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL;

/*!
 * @brief   Base structure which describes a BOARDOBJ CLIENT_READINGS in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS {
    /*!
     * @brief   BOARDOBJ type.
     *
     * This should be a unique value within the class that the BOARDOBJ belongs.
     */
    NvU8 type;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS;

/*!
 * @brief   Base structure which describes a CLIENT_INFO BOARDOBJ_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_INTERFACE;

/*!
 * @brief   Base structure which describes a CLIENT_STATUS BOARDOBJ_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_INTERFACE;

/*!
 * @brief   Base structure which describes a CLIENT_CONTROL BOARDOBJ_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_INTERFACE;

/*!
 * @brief   Base structure which describes a CLIENT_READINGS BOARDOBJ_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_INTERFACE;

/*!
 * @brief   Structure representing CLIENT_INFO for a BOARDOBJ_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL {
    /*!
     * Super class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_INFO for a BOARDOBJ_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_INFO for a
 *          BOARDOBJ_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_INFO_IFACE_MODEL_PMU_INIT_1X;

/*!
 * @brief   Structure representing CLIENT_STATUS for a BOARDOBJ_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL {
    /*!
     * Super class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_STATUS for a BOARDOBJ_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_STATUS for a
 *          BOARDOBJ_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_STATUS_IFACE_MODEL_PMU_INIT_1X;

/*!
 * @brief   Structure representing CLIENT_CONTROL for a BOARDOBJ_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL {
    /*!
     * Super class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_CONTROL for a BOARDOBJ_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_CONTROL for a
 *          BOARDOBJ_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_CONTROL_IFACE_MODEL_PMU_INIT_1X;

/*!
 * @brief   Structure representing CLIENT_READINGS for a BOARDOBJ_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL {
    /*!
     * Super class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_READINGS for a BOARDOBJ_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_READINGS for a
 *          BOARDOBJ_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJ_CLIENT_READINGS_IFACE_MODEL_PMU_INIT_1X;

/*!
 * CLIENT_INFO data for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO {
    /*!
     * @brief   Concrete class type of this BOARDOBJGRP
     */
    NvU8 classType;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO;

/*!
 * CLIENT_INFO SUPER_ALIAS version for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_SUPER_ALIAS {
    /*!
     * Base class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO super;

    NV2080_CTRL_BOARDOBJGRP_MASK                 objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_SUPER_ALIAS;

/*!
 * CLIENT_STATUS data for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS {
    /*!
     * @brief   Concrete class type of this BOARDOBJGRP
     */
    NvU8 classType;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS;

/*!
 * CLIENT_STATUS SUPER_ALIAS version for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_SUPER_ALIAS {
    /*!
     * Base class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS super;

    NV2080_CTRL_BOARDOBJGRP_MASK                   objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_SUPER_ALIAS;

/*!
 * CLIENT_CONTROL data for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL {
    /*!
     * @brief   Concrete class type of this BOARDOBJGRP
     */
    NvU8 classType;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL;

/*!
 * CLIENT_CONTROL SUPER_ALIAS version for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_SUPER_ALIAS {
    /*!
     * Base class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL super;

    NV2080_CTRL_BOARDOBJGRP_MASK                    objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_SUPER_ALIAS;

/*!
 * CLIENT_READINGS data for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS {
    /*!
     * @brief   Concrete class type of this BOARDOBJGRP
     */
    NvU8   classType;

    /*!
     * Whether the CLIENT_READINGS have been initialized.
     * This should be set to false on any initial call into the API
     */
    NvBool bInitialized;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS;

/*!
 * CLIENT_READINGS SUPER_ALIAS version for @ref BOARDOBJGRP base class
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_SUPER_ALIAS {
    /*!
     * Base class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS super;

    NV2080_CTRL_BOARDOBJGRP_MASK                     objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_SUPER_ALIAS;

/*!
 * CLIENT_INFO for @ref BOARDOBJGRP_E32
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E32 {
    /*!
     * Base class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E32             objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E32;

/*!
 * CLIENT_STATUS for @ref BOARDOBJGRP_E32
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E32 {
    /*!
     * Base class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E32               objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E32;

/*!
 * CLIENT_CONTROL for @ref BOARDOBJGRP_E32
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E32 {
    /*!
     * Base class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E32                objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E32;

/*!
 * CLIENT_READINGS for @ref BOARDOBJGRP_E32
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E32 {
    /*!
     * Base class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E32                 objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E32;

/*!
 * CLIENT_INFO for @ref BOARDOBJGRP_E255
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E255 {
    /*!
     * Base class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E255            objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E255;

/*!
 * CLIENT_STATUS for @ref BOARDOBJGRP_E255
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E255 {
    /*!
     * Base class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E255              objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E255;

/*!
 * CLIENT_CONTROL for @ref BOARDOBJGRP_E255
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E255 {
    /*!
     * Base class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E255               objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E255;

/*!
 * CLIENT_READINGS for @ref BOARDOBJGRP_E255
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E255 {
    /*!
     * Base class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E255                objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E255;

/*!
 * CLIENT_INFO for @ref BOARDOBJGRP_E512
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E512 {
    /*!
     * Base class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E512            objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E512;

/*!
 * CLIENT_STATUS for @ref BOARDOBJGRP_E512
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E512 {
    /*!
     * Base class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E512              objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E512;

/*!
 * CLIENT_CONTROL for @ref BOARDOBJGRP_E512
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E512 {
    /*!
     * Base class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E512               objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E512;

/*!
 * CLIENT_READINGS for @ref BOARDOBJGRP_E512
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E512 {
    /*!
     * Base class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E512                objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E512;

/*!
 * CLIENT_INFO for @ref BOARDOBJGRP_E1024
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E1024 {
    /*!
     * Base class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E1024           objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E1024;

/*!
 * CLIENT_STATUS for @ref BOARDOBJGRP_E1024
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E1024 {
    /*!
     * Base class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E1024             objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E1024;

/*!
 * CLIENT_CONTROL for @ref BOARDOBJGRP_E1024
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E1024 {
    /*!
     * Base class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E1024              objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E1024;

/*!
 * CLIENT_READINGS for @ref BOARDOBJGRP_E1024
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E1024 {
    /*!
     * Base class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E1024               objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E1024;

/*!
 * CLIENT_INFO for @ref BOARDOBJGRP_E2048
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E2048 {
    /*!
     * Base class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E2048           objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_E2048;

/*!
 * CLIENT_STATUS for @ref BOARDOBJGRP_E2048
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E2048 {
    /*!
     * Base class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E2048             objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_E2048;

/*!
 * CLIENT_CONTROL for @ref BOARDOBJGRP_E2048
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E2048 {
    /*!
     * Base class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E2048              objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_E2048;

/*!
 * CLIENT_READINGS for @ref BOARDOBJGRP_E2048
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E2048 {
    /*!
     * Base class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS super;

    /*!
     * Mask of valid @ref BOARDOBJ objects within the payload
     */
    NV2080_CTRL_BOARDOBJGRP_MASK_E2048               objMask;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_E2048;

/*!
 * @brief   Structure representing CLIENT_INFO for a BOARDOBJGRP_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_INTERFACE;

/*!
 * @brief   Structure representing CLIENT_INFO for a BOARDOBJGRP_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL {
    /*!
     * Super class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_INFO for a BOARDOBJGRP_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_INFO for a
 *          BOARDOBJGRP_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_INFO
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_INFO_IFACE_MODEL_PMU_INIT_1X;

/*!
 * @brief   Structure representing CLIENT_STATUS for a BOARDOBJGRP_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_INTERFACE;

/*!
 * @brief   Structure representing CLIENT_STATUS for a BOARDOBJGRP_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL {
    /*!
     * Super class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_STATUS for a BOARDOBJGRP_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_STATUS for a
 *          BOARDOBJGRP_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_STATUS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_STATUS_IFACE_MODEL_PMU_INIT_1X;

/*!
 * @brief   Structure representing CLIENT_CONTROL for a BOARDOBJGRP_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_INTERFACE;

/*!
 * @brief   Structure representing CLIENT_CONTROL for a BOARDOBJGRP_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL {
    /*!
     * Super class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_CONTROL for a BOARDOBJGRP_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_CONTROL for a
 *          BOARDOBJGRP_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_CONTROL
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_CONTROL_IFACE_MODEL_PMU_INIT_1X;

/*!
 * @brief   Structure representing CLIENT_READINGS for a BOARDOBJGRP_INTERFACE in RMCTRL.
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_INTERFACE {
    /*!
     * @brief   Reserved for future use cases.
     */
    NvU8 rsvd;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_INTERFACE;

/*!
 * @brief   Structure representing CLIENT_READINGS for a BOARDOBJGRP_IFACE_MODEL interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL {
    /*!
     * Super class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_INTERFACE super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL;

/*!
 * @brief   Structure representing CLIENT_READINGS for a BOARDOBJGRP_IFACE_MODEL_20
 *          interface
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL_20 {
    /*!
     * Super class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL_20;

/*!
 * @brief   Structure representing CLIENT_READINGS for a
 *          BOARDOBJGRP_IFACE_MODEL_PMU_INIT_1X interface
 *
 */
typedef struct NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL_PMU_INIT_1X {
    /*!
     * Super class CLIENT_READINGS
     */
    NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL_20 super;
} NV2080_CTRL_BOARDOBJ_BOARDOBJGRP_CLIENT_READINGS_IFACE_MODEL_PMU_INIT_1X;
