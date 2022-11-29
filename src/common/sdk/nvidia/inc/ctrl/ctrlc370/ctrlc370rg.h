/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrlc370/ctrlc370rg.finn
//

#include "ctrl/ctrlc370/ctrlc370base.h"
/* C370 is partially derived from 5070 */
#include "ctrl/ctrl5070/ctrl5070rg.h"




/*
 * NVC370_CTRL_CMD_GET_LOCKPINS_CAPS
 *
 * This command returns lockpins for the specified pinset,
 * as well as lockpins' HW capabilities.
 *
 *   pinset [in]
 *     This parameter takes the pinset whose corresponding
 *     lockpin numbers need to be determined. This only affects
 *     the return value for the RaterLock and FlipLock pins.
 *
 *   frameLockPin [out]
 *     This parameter returns the FrameLock pin index.
 *
 *   rasterLockPin [out]
 *     This parameter returns the RasterLock pin index.
 *
 *   flipLockPin [out]
 *     This parameter returns the FlipLock pin index.
 *
 *   stereoPin [out]
 *     This parameter returns the Stereo pin index.
 *
 *   numScanLockPins [out]
 *     This parameter returns the HW capability of ScanLock pins.
 *
 *   numFlipLockPins [out]
 *     This parameter returns the HW capability of FlipLock pins.
 *
 *   numStereoPins [out]
 *     This parameter returns the HW capability of Stereo pins.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NVC370_CTRL_CMD_GET_LOCKPINS_CAPS                  (0xc3700201) /* finn: Evaluated from "(FINN_NVC370_DISPLAY_RG_INTERFACE_ID << 8) | NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS_MESSAGE_ID" */

#define NVC370_CTRL_GET_LOCKPINS_CAPS_FRAME_LOCK_PIN_NONE  (0xffffffff)
#define NVC370_CTRL_GET_LOCKPINS_CAPS_RASTER_LOCK_PIN_NONE (0xffffffff)
#define NVC370_CTRL_GET_LOCKPINS_CAPS_FLIP_LOCK_PIN_NONE   (0xffffffff)
#define NVC370_CTRL_GET_LOCKPINS_CAPS_STEREO_PIN_NONE      (0xffffffff)
#define NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS {
    NVC370_CTRL_CMD_BASE_PARAMS base;
    NvU32                       pinset;
    NvU32                       frameLockPin;
    NvU32                       rasterLockPin;
    NvU32                       flipLockPin;
    NvU32                       stereoPin;
    NvU32                       numScanLockPins;
    NvU32                       numFlipLockPins;
    NvU32                       numStereoPins;
} NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS;

/*
 * NVC370_CTRL_CMD_SET_SWAPRDY_GPIO_WAR
 *
 * This command switches SWAP_READY_OUT GPIO between SW
 * and HW control to WAR bug 200374184
 *
 *   bEnable [in]:
 *     This parameter indicates enable/disable external fliplock
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_GENERIC
 */

#define NVC370_CTRL_CMD_SET_SWAPRDY_GPIO_WAR (0xc3700202) /* finn: Evaluated from "(FINN_NVC370_DISPLAY_RG_INTERFACE_ID << 8) | NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS_MESSAGE_ID" */

#define NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS {
    NVC370_CTRL_CMD_BASE_PARAMS base;
    NvBool                      bEnable;
} NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS;


