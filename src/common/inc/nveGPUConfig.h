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

// Surprise removal capable TB3 and TB2 BUS Device ID
#define BUS_DEVICE_ID_TB3_ALPINE_RIDGE_01          0x1578
#define BUS_DEVICE_ID_TB3_02                       0x1576
#define BUS_DEVICE_ID_TB3_03                       0x15C0
#define BUS_DEVICE_ID_TB3_04                       0x15D3
#define BUS_DEVICE_ID_TB3_05                       0x15DA
#define BUS_DEVICE_ID_TB3_06                       0x15EA
#define BUS_DEVICE_ID_TB3_07                       0x15E7
#define BUS_DEVICE_ID_TB3_08                       0x15EF
#define BUS_DEVICE_ID_TB3_09                       0x1133
#define BUS_DEVICE_ID_TB3_10                       0x1136

// IceLake-U TB3 device ids. Below TB3 would be integrated to CPU.
#define BUS_DEVICE_ID_ICELAKE_TB3_01               0x8A1D
#define BUS_DEVICE_ID_ICELAKE_TB3_02               0x8A1F
#define BUS_DEVICE_ID_ICELAKE_TB3_03               0x8A21
#define BUS_DEVICE_ID_ICELAKE_TB3_04               0x8A23
#define BUS_DEVICE_ID_ICELAKE_TB3_05               0x8A0D
#define BUS_DEVICE_ID_ICELAKE_TB3_06               0x8A17

// TigerLake Thunderbolt device ids.
#define BUS_DEVICE_ID_TIGERLAKE_TB3_01             0x9A1B
#define BUS_DEVICE_ID_TIGERLAKE_TB3_02             0x9A1D
#define BUS_DEVICE_ID_TIGERLAKE_TB3_03             0x9A1F
#define BUS_DEVICE_ID_TIGERLAKE_TB3_04             0x9A21
#define BUS_DEVICE_ID_TIGERLAKE_TB3_05             0x9A23
#define BUS_DEVICE_ID_TIGERLAKE_TB3_06             0x9A25
#define BUS_DEVICE_ID_TIGERLAKE_TB3_07             0x9A27
#define BUS_DEVICE_ID_TIGERLAKE_TB3_08             0x9A29
#define BUS_DEVICE_ID_TIGERLAKE_TB3_09             0x9A2B
#define BUS_DEVICE_ID_TIGERLAKE_TB3_10             0x9A2D

// Meteor Lake ThunderBolt Device IDs
#define BUS_DEVICE_ID_METEOR_TB3_01                0x7EB2
#define BUS_DEVICE_ID_METEOR_TB3_02                0x7EC2
#define BUS_DEVICE_ID_METEOR_TB3_03                0x7EC3
#define BUS_DEVICE_ID_METEOR_TB3_04                0x7EB4
#define BUS_DEVICE_ID_METEOR_TB3_05                0x7EC4
#define BUS_DEVICE_ID_METEOR_TB3_06                0x7EB5
#define BUS_DEVICE_ID_METEOR_TB3_07                0x7EC5
#define BUS_DEVICE_ID_METEOR_TB3_08                0x7EC6
#define BUS_DEVICE_ID_METEOR_TB3_09                0x7EC7

// Raptor Lake ThunderBolt Device IDs
#define BUS_DEVICE_ID_RAPTOR_TB3_01                0xA73E
#define BUS_DEVICE_ID_RAPTOR_TB3_02                0xA76D
#define BUS_DEVICE_ID_RAPTOR_TB3_03                0x466E
#define BUS_DEVICE_ID_RAPTOR_TB3_04                0x463F
#define BUS_DEVICE_ID_RAPTOR_TB3_05                0x462F
#define BUS_DEVICE_ID_RAPTOR_TB3_06                0x461F

//#define BUS_DEVICE_ID_TB2_FALCON_RIDGE_DSL5520_01  0X156C   // obsolete
#define BUS_DEVICE_ID_TB2_FALCON_RIDGE_DSL5520_02  0X156D
#define BUS_DEVICE_ID_TB2_03                       0x157E
#define BUS_DEVICE_ID_TB2_04                       0x156B
#define BUS_DEVICE_ID_TB2_05                       0x1567
#define BUS_DEVICE_ID_TB2_06                       0x1569
//#define BUS_DEVICE_ID_TB2_07                       0x1548   // obsolete
#define BUS_DEVICE_ID_TB2_08                       0x151B
#define BUS_DEVICE_ID_TB2_09                       0x1549
#define BUS_DEVICE_ID_TB2_10                       0x1513

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
    NvU16   tb3DeviceIDList[]={ BUS_DEVICE_ID_TB3_ALPINE_RIDGE_01,
                                BUS_DEVICE_ID_TB3_02,
                                BUS_DEVICE_ID_TB3_03,
                                BUS_DEVICE_ID_TB3_04,
                                BUS_DEVICE_ID_TB3_05,
                                BUS_DEVICE_ID_TB3_06,
                                BUS_DEVICE_ID_TB3_07,
                                BUS_DEVICE_ID_TB3_08,
                                BUS_DEVICE_ID_TB3_09,
                                BUS_DEVICE_ID_TB3_10,
                                BUS_DEVICE_ID_ICELAKE_TB3_01,
                                BUS_DEVICE_ID_ICELAKE_TB3_02,
                                BUS_DEVICE_ID_ICELAKE_TB3_03,
                                BUS_DEVICE_ID_ICELAKE_TB3_04,
                                BUS_DEVICE_ID_ICELAKE_TB3_05,
                                BUS_DEVICE_ID_ICELAKE_TB3_06,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_01,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_02,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_03,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_04,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_05,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_06,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_07,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_08,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_09,
                                BUS_DEVICE_ID_TIGERLAKE_TB3_10,
                                BUS_DEVICE_ID_METEOR_TB3_01,
                                BUS_DEVICE_ID_METEOR_TB3_02,
                                BUS_DEVICE_ID_METEOR_TB3_03,
                                BUS_DEVICE_ID_METEOR_TB3_04,
                                BUS_DEVICE_ID_METEOR_TB3_05,
                                BUS_DEVICE_ID_METEOR_TB3_06,
                                BUS_DEVICE_ID_METEOR_TB3_07,
                                BUS_DEVICE_ID_METEOR_TB3_08,
                                BUS_DEVICE_ID_METEOR_TB3_09,
                                BUS_DEVICE_ID_RAPTOR_TB3_01,
                                BUS_DEVICE_ID_RAPTOR_TB3_02,
                                BUS_DEVICE_ID_RAPTOR_TB3_03,
                                BUS_DEVICE_ID_RAPTOR_TB3_04,
                                BUS_DEVICE_ID_RAPTOR_TB3_05,
                                BUS_DEVICE_ID_RAPTOR_TB3_06
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
    NvU16   tb2DeviceIDList[]={ BUS_DEVICE_ID_TB2_FALCON_RIDGE_DSL5520_02,
                                BUS_DEVICE_ID_TB2_03, BUS_DEVICE_ID_TB2_04,
                                BUS_DEVICE_ID_TB2_05, BUS_DEVICE_ID_TB2_06,
                                BUS_DEVICE_ID_TB2_08, BUS_DEVICE_ID_TB2_09,
                                BUS_DEVICE_ID_TB2_10
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
