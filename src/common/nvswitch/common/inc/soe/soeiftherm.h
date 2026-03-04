/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFTHERM_H_
#define _SOEIFTHERM_H_

#include "nvfixedtypes.h"

/*!
 * @file   soeiftherm.h
 * @brief  SOE Thermal Command Queue
 *          
 *         The Therm unit ID will be used for sending and recieving
 *         Command Messages between driver and Thermal unt of SOE
 */

/* ------------------------ Defines ---------------------------------*/

// Macros for FXP9.5 conversion

#define NV_TSENSE_FXP_9_5_INTEGER            13:4
#define NV_TSENSE_FXP_9_5_FRACTIONAL         4:0

// Convert 32 bit Signed integer or Floating Point value to FXP9.5
#define NV_TSENSE_CONVERT_TO_FXP_9_5(val)  \
       (NvU32) (val *(1 << DRF_SIZE(NV_TSENSE_FXP_9_5_FRACTIONAL)))

// Convert FXP 9.5 to Celsius (Works only for temperatures >= 0)
#define NV_TSENSE_FXP_9_5_TO_CELSIUS(fxp)    \
       (NvU32) (fxp /(1 << DRF_SIZE(NV_TSENSE_FXP_9_5_FRACTIONAL)))

// Convert FXP 9.5 to NvTemp
#define NV_TSENSE_FXP_9_5_SIGN(fxp)  \
    DRF_VAL(_TYPES, _SFXP, _INTEGER_SIGN(9,5), fxp)

#define NV_TSENSE_FXP_9_5_TO_24_8(fxp)                  \
    (NvTemp) ((NV_TSENSE_FXP_9_5_SIGN(fxp) ==           \
             NV_TYPES_SFXP_INTEGER_SIGN_NEGATIVE ?      \
             DRF_SHIFTMASK(31:17) : 0x0) | (fxp << 3))

/*!
 * Macros for NvType <-> Celsius temperature conversion.
 */
#define RM_SOE_CELSIUS_TO_NV_TEMP(cel)                                      \
                                NV_TYPES_S32_TO_SFXP_X_Y(24,8,(cel))
#define RM_SOE_NV_TEMP_TO_CELSIUS_TRUNCED(nvt)                              \
                                NV_TYPES_SFXP_X_Y_TO_S32(24,8,(nvt))
#define RM_SOE_NV_TEMP_TO_CELSIUS_ROUNDED(nvt)                              \
                                NV_TYPES_SFXP_X_Y_TO_S32_ROUNDED(24,8,(nvt))

/*!
 * Thermal Message IDs
 */
enum
{
    RM_SOE_THERM_MSG_ID_SLOWDOWN_STATUS,
    RM_SOE_THERM_MSG_ID_SHUTDOWN_STATUS,
    RM_SOE_THERM_MSG_ID_ACK_FORCE_SLOWDOWN,
};

/*!
 * @brief message for thermal shutdown
 */
typedef struct
{
    NvU8   msgType;
    NvTemp maxTemperature;
    NvTemp overtThreshold;

    struct
    {
        NvBool bTsense;
        NvBool bPmgr;    
    }source;
} RM_SOE_THERM_MSG_SHUTDOWN_STATUS;

/*!
 * @brief message for thermal slowdown
 */
typedef struct
{
    NvU8   msgType;
    NvBool bSlowdown;
    NvTemp maxTemperature;
    NvTemp warnThreshold;
    NvBool bLinksL1Status;

    struct
    {
        NvBool bTsense;
        NvBool bPmgr;    
    }source;
} RM_SOE_THERM_MSG_SLOWDOWN_STATUS;

/*!
 * A simple union of all the Thermal messages.
 * Use the 'msgType' variable to determine the actual type of the message.
 */
typedef union
{
    NvU8  msgType;
    // The following structs are expected to include cmdType as the first member
    RM_SOE_THERM_MSG_SLOWDOWN_STATUS  slowdown;
    RM_SOE_THERM_MSG_SHUTDOWN_STATUS  shutdown;
}RM_SOE_THERM_MSG;

/*!
 * Thermal Command types
 */
enum
{
    RM_SOE_THERM_FORCE_SLOWDOWN,
    RM_SOE_THERM_SEND_MSG_TO_DRIVER,
};

/*!
 * @brief Force Thermal slowdown
 */
typedef struct
{
    NvU8   cmdType;
    NvBool slowdown;
    NvU32  periodUs;
} RM_SOE_THERM_CMD_FORCE_SLOWDOWN;

/*!
 * @brief Send aysncronous message about thermal events.
 */
typedef struct
{
    NvU8   cmdType;
    union
    {
        NvU8 msgType;
        RM_SOE_THERM_MSG_SLOWDOWN_STATUS  slowdown;
        RM_SOE_THERM_MSG_SHUTDOWN_STATUS  shutdown;
    } status;
} RM_SOE_THERM_CMD_SEND_ASYNC_MSG;

/*!
 * A simple union of all the therm commands. Use the 'cmdType' variable to
 * determine the actual type of the command.
 */
typedef union
{
    NvU8  cmdType;
    // The following structs are expected to include cmdType as the first member
    RM_SOE_THERM_CMD_FORCE_SLOWDOWN  slowdown;
    RM_SOE_THERM_CMD_SEND_ASYNC_MSG  msg;
}RM_SOE_THERM_CMD;

#endif  // _SOEIFTHERM_H_

