/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NVEGPUCONFIG_H_
#define _NVEGPUCONFIG_H_

#include <nvtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#define EGPU_INLINE NV_FORCEINLINE
#else   //!__cplusplus
#if defined(NV_UNIX) || defined(NVCPU_RISCV64) || defined(NV_MODS)
#define EGPU_INLINE static NV_INLINE
#else   //NV_UNIX
#define EGPU_INLINE NV_INLINE
#endif  //NV_UNIX
#endif  //!__cplusplus

#define PARENT_EGPU_BUS_DEVICE_43           0x57A4
#define PARENT_EGPU_BUS_DEVICE_42           0x5786
#define PARENT_EGPU_BUS_DEVICE_41           0x1578
#define PARENT_EGPU_BUS_DEVICE_40           0x1576
#define PARENT_EGPU_BUS_DEVICE_39           0x15C0
#define PARENT_EGPU_BUS_DEVICE_38           0x15D3
#define PARENT_EGPU_BUS_DEVICE_37           0x15DA
#define PARENT_EGPU_BUS_DEVICE_36           0x15EA
#define PARENT_EGPU_BUS_DEVICE_35           0x15E7
#define PARENT_EGPU_BUS_DEVICE_34           0x15EF
#define PARENT_EGPU_BUS_DEVICE_33           0x1133
#define PARENT_EGPU_BUS_DEVICE_32           0x1136

#define PARENT_EGPU_BUS_DEVICE_31           0x8A1D
#define PARENT_EGPU_BUS_DEVICE_30           0x8A1F
#define PARENT_EGPU_BUS_DEVICE_29           0x8A21
#define PARENT_EGPU_BUS_DEVICE_28           0x8A23
#define PARENT_EGPU_BUS_DEVICE_27           0x8A0D
#define PARENT_EGPU_BUS_DEVICE_26           0x8A17


#define PARENT_EGPU_BUS_DEVICE_25           0x9A1B
#define PARENT_EGPU_BUS_DEVICE_24           0x9A1D
#define PARENT_EGPU_BUS_DEVICE_23           0x9A1F
#define PARENT_EGPU_BUS_DEVICE_22           0x9A21
#define PARENT_EGPU_BUS_DEVICE_21           0x9A23
#define PARENT_EGPU_BUS_DEVICE_20           0x9A25
#define PARENT_EGPU_BUS_DEVICE_19           0x9A27
#define PARENT_EGPU_BUS_DEVICE_18           0x9A29
#define PARENT_EGPU_BUS_DEVICE_17           0x9A2B
#define PARENT_EGPU_BUS_DEVICE_16           0x9A2D

#define PARENT_EGPU_BUS_DEVICE_15           0x7EB2
#define PARENT_EGPU_BUS_DEVICE_14           0x7EC2
#define PARENT_EGPU_BUS_DEVICE_13           0x7EC3
#define PARENT_EGPU_BUS_DEVICE_12           0x7EB4
#define PARENT_EGPU_BUS_DEVICE_11           0x7EC4
#define PARENT_EGPU_BUS_DEVICE_10           0x7EB5
#define PARENT_EGPU_BUS_DEVICE_09           0x7EC5
#define PARENT_EGPU_BUS_DEVICE_08           0x7EC6
#define PARENT_EGPU_BUS_DEVICE_07           0x7EC7

#define PARENT_EGPU_BUS_DEVICE_06           0xA73E
#define PARENT_EGPU_BUS_DEVICE_05           0xA76D
#define PARENT_EGPU_BUS_DEVICE_04           0x466E
#define PARENT_EGPU_BUS_DEVICE_03           0x463F
#define PARENT_EGPU_BUS_DEVICE_02           0x462F
#define PARENT_EGPU_BUS_DEVICE_01           0x461F

#define PARENT_EGPU_BUS_DEVICE_02_08        0X156D
#define PARENT_EGPU_BUS_DEVICE_02_07        0x157E
#define PARENT_EGPU_BUS_DEVICE_02_06        0x156B
#define PARENT_EGPU_BUS_DEVICE_02_05        0x1567
#define PARENT_EGPU_BUS_DEVICE_02_04        0x1569
#define PARENT_EGPU_BUS_DEVICE_02_03        0x151B
#define PARENT_EGPU_BUS_DEVICE_02_02        0x1549
#define PARENT_EGPU_BUS_DEVICE_02_01        0x1513

//*****************************************************************************
// Function:  isTB3DeviceID
//
// Routine Description:
//
//      Function to match the specified Device ID with the known TB3 BUS's 
//      device IDs.
//
// Arguments:
//
//      deviceID[IN]: Device ID to match with the TB3 Bus
//
// Return Value:
//
//      true:  When the passed Dev ID match with TB3's BUS Device ID
//      false: When the passed Dev ID is not matching with known TB3's 
//             BUS Device ID
//*****************************************************************************
EGPU_INLINE NvBool isTB3DeviceID(NvU16 deviceID)
{
    NvU32   index;
    NvU16   tb3DeviceIDList[]={ PARENT_EGPU_BUS_DEVICE_01,
                                PARENT_EGPU_BUS_DEVICE_02,
                                PARENT_EGPU_BUS_DEVICE_03,
                                PARENT_EGPU_BUS_DEVICE_04,
                                PARENT_EGPU_BUS_DEVICE_05,
                                PARENT_EGPU_BUS_DEVICE_06,
                                PARENT_EGPU_BUS_DEVICE_07,
                                PARENT_EGPU_BUS_DEVICE_08,
                                PARENT_EGPU_BUS_DEVICE_09,
                                PARENT_EGPU_BUS_DEVICE_10,
                                PARENT_EGPU_BUS_DEVICE_11,
								PARENT_EGPU_BUS_DEVICE_12,
                                PARENT_EGPU_BUS_DEVICE_13,
								PARENT_EGPU_BUS_DEVICE_14,
                                PARENT_EGPU_BUS_DEVICE_15,
                                PARENT_EGPU_BUS_DEVICE_16,
                                PARENT_EGPU_BUS_DEVICE_17,
                                PARENT_EGPU_BUS_DEVICE_18,
                                PARENT_EGPU_BUS_DEVICE_19,
                                PARENT_EGPU_BUS_DEVICE_20,
                                PARENT_EGPU_BUS_DEVICE_21,
                                PARENT_EGPU_BUS_DEVICE_22,
                                PARENT_EGPU_BUS_DEVICE_23,
                                PARENT_EGPU_BUS_DEVICE_24,
                                PARENT_EGPU_BUS_DEVICE_25,
                                PARENT_EGPU_BUS_DEVICE_26,
                                PARENT_EGPU_BUS_DEVICE_27,
                                PARENT_EGPU_BUS_DEVICE_28,
                                PARENT_EGPU_BUS_DEVICE_29,
                                PARENT_EGPU_BUS_DEVICE_30,
                                PARENT_EGPU_BUS_DEVICE_31,
                                PARENT_EGPU_BUS_DEVICE_32,
                                PARENT_EGPU_BUS_DEVICE_33,
                                PARENT_EGPU_BUS_DEVICE_34,
                                PARENT_EGPU_BUS_DEVICE_35,
                                PARENT_EGPU_BUS_DEVICE_36,
                                PARENT_EGPU_BUS_DEVICE_37,
                                PARENT_EGPU_BUS_DEVICE_38,
                                PARENT_EGPU_BUS_DEVICE_39,
                                PARENT_EGPU_BUS_DEVICE_40,
                                PARENT_EGPU_BUS_DEVICE_41,
                                PARENT_EGPU_BUS_DEVICE_42,
                                PARENT_EGPU_BUS_DEVICE_43
							};

    for (index = 0; index < (sizeof(tb3DeviceIDList)/sizeof(NvU16)); index++)
    {
        if (deviceID == tb3DeviceIDList[index])
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
} // isTB3DeviceID

//*****************************************************************************
// Function:  isTB2DeviceID
//
// Routine Description:
//
//      Function to match the specified Device ID with the known TB2 BUS's 
//      device IDs.
//
// Arguments:
//
//      deviceID[IN]: Device ID to match with the TB2 Bus
//
// Return Value:
//
//      true:  When the passed Dev ID match with TB2's BUS Device ID
//      false: When the passed Dev ID is not matching with known TB2's 
//             BUS Device ID
//*****************************************************************************
EGPU_INLINE NvBool isTB2DeviceID(NvU16 deviceID)
{
    NvU32   index;
    NvU16   tb2DeviceIDList[]={ PARENT_EGPU_BUS_DEVICE_02_01,
                                PARENT_EGPU_BUS_DEVICE_02_02,
                                PARENT_EGPU_BUS_DEVICE_02_03,
                                PARENT_EGPU_BUS_DEVICE_02_04,
                                PARENT_EGPU_BUS_DEVICE_02_05,
                                PARENT_EGPU_BUS_DEVICE_02_06,
                                PARENT_EGPU_BUS_DEVICE_02_07,
                                PARENT_EGPU_BUS_DEVICE_02_08
                              };
    for (index = 0; index < (sizeof(tb2DeviceIDList)/sizeof(NvU16)); index++)
    {
        if (deviceID == tb2DeviceIDList[index])
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
} // isTB2DeviceID

#ifdef __cplusplus
}
#endif
#endif  //_NVEGPUCONFIG_H_
