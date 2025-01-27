/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef DAC_P2060_H
#define DAC_P2060_H

/* ------------------------ Includes --------------------------------------- */
#include "gpu/external_device/external_device.h" // DACEXTERNALDEVICE
#include "gpu/external_device/gsync.h" // GSYNCVIDEOMODE, GSYNCSYNCPOLARITY
#include "gpu/disp/kern_disp_max.h"

/* ------------------------ Macros & Defines ------------------------------- */

// Display synchronization interface. (Framelock, Genlock, Swapready, etc)
#define NV_P2060_MAX_ASSOCIATED_GPUS  4
#define NV_P2060_MAX_IFACES_PER_GSYNC 4
#define NV_P2060_MAX_GPUS_PER_IFACE   1
#define NV_P2060_MAX_HEADS_PER_GPU    4
#define NV_P2060_MAX_MOSAIC_SLAVES    3
#define NV_P2060_MAX_MOSAIC_GROUPS    2

#define NV_P2060_IFACE_ONE    0
#define NV_P2060_IFACE_TWO    1
#define NV_P2060_IFACE_THREE  2
#define NV_P2060_IFACE_FOUR   3

#define NV_P2060_SYNC_SKEW_MAX_UNITS_FULL_SUPPORT       65535   // For FPGA with Rev >= 3. Refer Bug 1058215
#define NV_P2060_SYNC_SKEW_MAX_UNITS_LIMITED_SUPPORT    1       // For FPGA with Rev < 3.
#define NV_P2060_SYNC_SKEW_RESOLUTION                   977
#define NV_P2060_START_DELAY_MAX_UNITS                  65535
#define NV_P2060_START_DELAY_RESOLUTION                 7800
#define NV_P2060_SYNC_INTERVAL_MAX_UNITS                7
#define NV_P2061_V204_SYNC_SKEW_RESOLUTION              7          // For 2061 V2.04+
#define NV_P2061_V204_SYNC_SKEW_MAX_UNITS               0xFFFFFF   // For 2061 V2.04+
#define NV_P2061_V204_SYNC_SKEW_INVALID                 (NV_P2061_V204_SYNC_SKEW_MAX_UNITS + 1)

#define NV_P2060_WATCHDOG_COUNT_DOWN_VALUE  60   // 1 minute, assuming watchdog time interval is 1 second.
#define NV_P2060_FRAME_COUNT_TIMER_INTERVAL 5000000000LL // 5 sec

#define NV_P2060_MAX_GPU_FRAME_COUNT           65535
#define NV_P2060_MAX_GSYNC_FRAME_COUNT         16777215 // 2^24.Gsync frame count is a 24 bit register

#define P2061_FW_REV(pExtDev)           ((pExtDev->deviceRev << 8) | (pExtDev->deviceExRev))
/* ------------------------ Types definitions ------------------------------ */

typedef struct EXTDEV_I2C_HANDLES
{
    //Internal handles per GPU

    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    NvHandle hSubscription;
    NvU32    gpuId;
} EXTDEV_I2C_HANDLES;

typedef struct
{
    NvU8               lossRegStatus;
    NvU8               gainRegStatus;
    NvU8               miscRegStatus;
    DACEXTERNALDEVICE *pExtDevice;
}EXTDEV_INTR_DATA;

// note: NV_P2060_MAX_ASSOCIATED_GPUS = NV_P2060_MAX_IFACES_PER_GSYNC * NV_P2060_MAX_GPUS_PER_IFACE

struct DACP2060EXTERNALDEVICE
{
    //Must be at top of struct
    DACEXTERNALDEVICE ExternalDevice;

    // Stuff for supporting the DisplaySync interface
    NvU32 AssociatedCRTCs;  // bit mask of crtcs ids associated.
    GSYNCVIDEOMODE    VideoMode;
    GSYNCSYNCPOLARITY SyncPolarity;
    NvU32 SyncStartDelay;
    NvU32 SyncSkew;
    NvU32 NSync;
    NvU32 HouseSignal;
    NvU32 UseHouseSync;
    NvU32 Master;
    NvU32 Slaves;
    NvU32 EmitTestSignal;
    NvU32 InterlaceMode;
    NvU32 RefreshRate;      // desired frame rate (units of .01Hz)
    NvU32 DebugMask;
    NvU32 gpuAttachMask;
    NvU32 id;
    NvU32 watchdogCountDownValue;
    NvBool isNonFramelockInterruptEnabled;
    NvU32 interruptEnabledInterface;
    NvU32 tSwapRdyHi;  /* Value of SWAP_LOCKOUT_START in accordance to the
                        * time in microseconds for which swap Rdy
                        * lines will remain high.(Provided via a regkey)
                       */
    NvU32 tSwapRdyHiLsrMinTime; /* Value of LSR_MIN_TIME in accordance to the time (in us)
                                 * swap ready line will remain high.(Provided via a regkey)
                                */
    NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS mulDivSettings;   // Cached multiplier-divider settings

    NvU32 syncSkewResolutionInNs; // resolution in ns
    NvU32 syncSkewMax;            // max syncSkew setting in raw units
    NvU32 lastUserSkewSent;       // Remember the last Sync Skew value sent by the user

    struct {
        NvU32  currentFrameCount;              // gpu frame count register value for current user query
        NvU32  previousFrameCount;             // gpu frame count register value for previous user query
        NvU32  totalFrameCount;                // equals to cached gsync frame count = gpu frame count + difference.
        NvU32  numberOfRollbacks;              // Max value of N where (Gsync Frame Count > N * Gpu frame count)
        NvU32  frameTime;                      // Time to render one frame.
        NvU64  lastFrameCounterQueryTime;
        NvS32  initialDifference;              // Difference between Gsync frame count and (numberOfRollbacks * Gpu framecount)
        NvU32  iface;
        NvU32  head;
        NvU32  vActive;                        // Vertical Resolution for which system is framelocked.
        NvBool bReCheck;                       // Enabled to verify initialDifference 1 sec after initialization.
        NvBool enableFrmCmpMatchIntSlave;      // Enable the frmCmpMatchInt for slave, if this bit is set.
        NvBool isFrmCmpMatchIntMasterEnabled;  // To enable frmCmpMatchInt for master when gsync framecount exceeds (2^24 - 1000)

        TMR_EVENT *pTimerEvents[NV_MAX_DEVICES]; // Used for supporting gsyncFrameCountTimerService_P2060
    } FrameCountData;

    struct {
        NvU32   Status1;
        NvU64   lastSyncCheckTime;
        NvU64   lastStereoToggleTime;
    } Snapshot[NV_P2060_MAX_IFACES_PER_GSYNC];

    // These arrays refer to the state of heads with respect to their sync
    // source, and their usage can be kind of confusing. This table
    // describes how they should be set/used:
    //
    // Head[i]   --> Is Head[i] the frame lock master, or a slave
    // SyncSrc   --> Where is the sync timing actually coming from (the
    //               master head or a house sync signal)

    //
    // Head[i] SyncSrc          PM[i] PS[i] PSLS[i]
    // -----------------------------------------+---------------------
    // Master  Head[i]          1     0     0
    // Master  House            1     1     0
    // Slave   Head[!i]         0     0     1
    // Slave   House            0     1     0
    // Slave   External         0     1     0
    // Neither X                0     0     0
    //
    // (the last row represents the case where the head has not been
    // requested to lock).

    struct {
        struct {
            NvU32 Master    [OBJ_MAX_HEADS];
            NvU32 Slaved    [OBJ_MAX_HEADS];
            NvU32 LocalSlave[OBJ_MAX_HEADS];
        } Sync;

        struct {
            NvU32  gpuId;
            NvBool connected;
        } GpuInfo;

        struct {
            NvU32  OrigLsrMinTime[OBJ_MAX_HEADS];
            NvBool saved;
        } DsiFliplock;

        struct {
            NvU32  direction;
            NvU32  mode;
            NvBool saved;
        } RasterSyncGpio;

        NvBool SwapReadyRequested;
        NvBool skipSwapBarrierWar;

        NvU32 lastEventNotified;
        NvU32 gainedSync;           // Set when we gain sync after enabling framelock.

    } Iface[NV_P2060_MAX_IFACES_PER_GSYNC];

    EXTDEV_I2C_HANDLES i2cHandles[NV_P2060_MAX_IFACES_PER_GSYNC];

    struct {
        NvU32  gpuTimingSource;
        NvU32  gpuTimingSlaves[NV_P2060_MAX_MOSAIC_SLAVES];
        NvU32  slaveGpuCount;
        NvBool enabledMosaic;
    } MosaicGroup[NV_P2060_MAX_MOSAIC_GROUPS];
};

PDACEXTERNALDEVICE extdevConstruct_P2060   (OBJGPU *, PDACEXTERNALDEVICE);
NvBool    gsyncAttachExternalDevice_P2060  (OBJGPU *, PDACEXTERNALDEVICE*);
void      extdevDestroy_P2060              (OBJGPU *, PDACEXTERNALDEVICE);
NvBool    extdevGetDevice_P2060            (OBJGPU *, PDACEXTERNALDEVICE);
NvBool    extdevInit_P2060                 (OBJGPU *, PDACEXTERNALDEVICE);
void      extdevDestroy_P2060              (OBJGPU *, PDACEXTERNALDEVICE);
void      extdevService_P2060              (OBJGPU *, PDACEXTERNALDEVICE, NvU8, NvU8, NvU8, NvBool);
NV_STATUS extdevWatchdog_P2060             (OBJGPU *, OBJTMR *, PDACEXTERNALDEVICE); // OBJTMR routine signature (TIMERPROC).
NvBool    extdevSaveI2cHandles_P2060       (OBJGPU *, DACEXTERNALDEVICE *);
NV_STATUS gsyncFindGpuHandleLocation       (DACEXTERNALDEVICE *, NvU32 , NvU32 *);

// P2060 hal ifaces

NvBool    gsyncGpuCanBeMaster_P2060          (OBJGPU *, PDACEXTERNALDEVICE);
NV_STATUS gsyncGetSyncPolarity_P2060         (OBJGPU *, PDACEXTERNALDEVICE, GSYNCSYNCPOLARITY *);
NV_STATUS gsyncSetSyncPolarity_P2060         (OBJGPU *, PDACEXTERNALDEVICE, GSYNCSYNCPOLARITY);
NV_STATUS gsyncGetVideoMode_P2060            (OBJGPU *, PDACEXTERNALDEVICE, GSYNCVIDEOMODE *);
NV_STATUS gsyncSetVideoMode_P2060            (OBJGPU *, PDACEXTERNALDEVICE, GSYNCVIDEOMODE);
NV_STATUS gsyncGetNSync_P2060                (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetNSync_P2060                (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncGetSyncSkew_P2060             (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetSyncSkew_P2060             (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncGetUseHouse_P2060             (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetUseHouse_P2060             (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncGetSyncStartDelay_P2060       (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetSyncStartDelay_P2060       (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncRefSignal_P2060               (OBJGPU *, PDACEXTERNALDEVICE, REFTYPE, GSYNCSYNCSIGNAL, NvBool bRate, NvU32 *);
NV_STATUS gsyncRefMaster_P2060               (OBJGPU *, OBJGSYNC *, REFTYPE, NvU32 *DisplayMask,
                                              NvU32 *Refresh, NvBool retainMaster,
                                              NvBool skipSwapBarrierWar);
NV_STATUS gsyncRefSlaves_P2060               (OBJGPU *, PDACEXTERNALDEVICE, REFTYPE, NvU32 *DisplayMask_s, NvU32 *Refresh);
NV_STATUS gsyncGetCplStatus_P2060            (OBJGPU *, PDACEXTERNALDEVICE, GSYNCSTATUS, NvU32 *);
NV_STATUS gsyncGetEmitTestSignal_P2060       (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetEmitTestSignal_P2060       (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncGetInterlaceMode_P2060        (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetInterlaceMode_P2060        (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncRefSwapBarrier_P2060          (OBJGPU *, PDACEXTERNALDEVICE, REFTYPE, NvBool *);
NV_STATUS gsyncGetWatchdog_P2060             (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetWatchdog_P2060             (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncGetRevision_P2060             (OBJGPU *, OBJGSYNC *, GSYNCCAPSPARAMS *);
NV_STATUS gsyncOptimizeTimingParameters_P2060(OBJGPU *, GSYNCTIMINGPARAMS *);
NV_STATUS gsyncGetStereoLockMode_P2060       (OBJGPU *, PDACEXTERNALDEVICE, NvU32 *);
NV_STATUS gsyncSetStereoLockMode_P2060       (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncSetMosaic_P2060               (OBJGPU *, PDACEXTERNALDEVICE, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *);
NV_STATUS gsyncConfigFlashGsync_P2060        (OBJGPU *, PDACEXTERNALDEVICE, NvU32);
NV_STATUS gsyncGetMulDiv_P2060               (OBJGPU *, DACEXTERNALDEVICE *, NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *);
NV_STATUS gsyncSetMulDiv_P2060               (OBJGPU *, DACEXTERNALDEVICE *, NV30F1_CTRL_GSYNC_MULTIPLY_DIVIDE_SETTINGS *);
NvBool    gsyncSupportsLargeSyncSkew_P2060   (DACEXTERNALDEVICE *);

#endif
