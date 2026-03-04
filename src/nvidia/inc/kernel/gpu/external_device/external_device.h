/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef EXTDEV_H_
#define EXTDEV_H_

/* ------------------------ Includes --------------------------------------- */
#include "Nvcm.h"
#include "gpu/disp/dispsw.h"

/* ------------------------ Types definitions ------------------------------ */
typedef struct DACEXTERNALDEVICE DACEXTERNALDEVICE, *PDACEXTERNALDEVICE;
typedef struct DACEXTERNALDEVICEIFACE DACEXTERNALDEVICEIFACE, *PDACEXTERNALDEVICEIFACE;
typedef struct DACP2060EXTERNALDEVICE DACP2060EXTERNALDEVICE, *PDACP2060EXTERNALDEVICE;

typedef enum _DAC_EXTERNAL_DEVICES
{
    DAC_EXTERNAL_DEVICE_NONE  = 0,
    DAC_EXTERNAL_DEVICE_P2060 = 0x2060, // NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_P2060
    DAC_EXTERNAL_DEVICE_P2061 = 0x2061, // NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_P2060
} DAC_EXTERNAL_DEVICES;

typedef enum _DAC_EXTERNAL_DEVICE_FPGA_REVS
{
    DAC_EXTERNAL_DEVICE_REV_0,
    DAC_EXTERNAL_DEVICE_REV_1,
    DAC_EXTERNAL_DEVICE_REV_2,
    DAC_EXTERNAL_DEVICE_REV_3,
    DAC_EXTERNAL_DEVICE_REV_4,
    DAC_EXTERNAL_DEVICE_REV_5,
    DAC_EXTERNAL_DEVICE_REV_6,
    DAC_EXTERNAL_DEVICE_REV_7,
    DAC_EXTERNAL_DEVICE_REV_MAX,
    DAC_EXTERNAL_DEVICE_REV_NONE,
} DAC_EXTERNAL_DEVICE_REVS;

typedef enum _DAC_EXTDEV_ACTIONS
{
    GET,
    SET,
} DAC_EXTDEV_ACTIONS;

//
// Not super-sure how "ExternalDevice" fits in with the model vs. say an
// external display encoder (tv, digital, DAC, etc).  But perhaps those would
// instantiate one of these guys to become the "CommandChannel".  As in an
// external display encoder would "know about" its associated ExternalDevice
// and use it to communicate... later... for now ExternalDevice is the base
// class for the extension boards.
//

struct DACEXTERNALDEVICEIFACE
{
    NvBool    (*GetDevice) (OBJGPU *, PDACEXTERNALDEVICE); // = 0 ( Pure virtual )
    NvBool    (*Init)      (OBJGPU *, PDACEXTERNALDEVICE); // = 0 ( Pure virtual )
    void      (*Destroy)   (OBJGPU *, PDACEXTERNALDEVICE);
    NvBool    (*Attach)    (OBJGPU *, PDACEXTERNALDEVICE *);
    NvBool    (*Validate)  (OBJGPU *, PDACEXTERNALDEVICE);  // check if the config is valid

    void      (*SetMode)   (OBJGPU *, PDACEXTERNALDEVICE, NvU32);

    NvBool    (*SetupVblankService)(OBJGPU *, PDACEXTERNALDEVICE, NvU32, NvBool);
    void      (*Service)           (OBJGPU *, PDACEXTERNALDEVICE, NvU8, NvU8, NvU8, NvBool);
    NV_STATUS (*Watchdog)     (OBJGPU *, struct OBJTMR *, PDACEXTERNALDEVICE); // OBJTMR routine signature (TIMERPROC).
    NvBool    (*setI2cHandles)(OBJGPU *, DACEXTERNALDEVICE *);
};

struct DACEXTERNALDEVICE
{
    DACEXTERNALDEVICEIFACE *pI;

    NvU32 ReferenceCount;

    NvU8 I2CAddr;
    NvU32 I2CPort;
    NvU32 MaxGpus;

    NvU8 revId;
    DAC_EXTERNAL_DEVICES deviceId;
    DAC_EXTERNAL_DEVICE_REVS deviceRev; //device revision, also known as firmware major version
    NvU8 deviceExRev; //device extended revision, also known as firmware minor version

    struct {
        NvBool   Scheduled;
        NvU32    TimeOut;

        TMR_EVENT *pTimerEvents[NV_MAX_DEVICES];
    } WatchdogControl;
};

typedef PDACEXTERNALDEVICE (*pfextdevConstruct) (OBJGPU *, PDACEXTERNALDEVICE);

/* ------------------------ Macros & Defines ------------------------------- */
#define NV_P2060_MIN_REV 0x2

void                 extdevGetBoundHeadsAndDisplayIds(OBJGPU *, NvU32 *);
PDACEXTERNALDEVICE   extdevConstruct_Base    (OBJGPU *, PDACEXTERNALDEVICE);
void                 extdevDestroy_Base      (OBJGPU *, PDACEXTERNALDEVICE);
NvBool               extdevValidate_Default  (OBJGPU *, PDACEXTERNALDEVICE);
void                 extdevInvalidate_Default(OBJGPU *, PDACEXTERNALDEVICE);

NV_STATUS i2c_extdeviceHelper(OBJGPU *, DACEXTERNALDEVICE *, NvU32, NvU8, NvU8 *,NvBool);
NV_STATUS writeregu008_extdevice(OBJGPU *, PDACEXTERNALDEVICE, NvU8, NvU8);
NV_STATUS writeregu008_extdeviceTargeted(OBJGPU *, PDACEXTERNALDEVICE, NvU8, NvU8);
NV_STATUS readregu008_extdevice(OBJGPU *, PDACEXTERNALDEVICE, NvU8, NvU8*);
NV_STATUS readregu008_extdeviceTargeted(OBJGPU *, PDACEXTERNALDEVICE, NvU8, NvU8*);

void      extdevDestroy  (OBJGPU *);
NV_STATUS extdevServiceWatchdog(OBJGPU *, OBJTMR *, TMR_EVENT *);
NV_STATUS extdevScheduleWatchdog(OBJGPU *, PDACEXTERNALDEVICE);
NV_STATUS extdevCancelWatchdog  (OBJGPU *, PDACEXTERNALDEVICE);
void      extdevGsyncService(OBJGPU *,      NvU8, NvU8, NvU8, NvBool);

#endif
