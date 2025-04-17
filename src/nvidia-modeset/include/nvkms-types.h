/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_TYPES_H__
#define __NVKMS_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvidia-modeset-os-interface.h"

#include "nvctassert.h"
#include "nv_list.h"

#include <ctrl/ctrl0073/ctrl0073specific.h> /* NV0073_CTRL_SPECIFIC_OR_PROTOCOL_* */
#include <ctrl/ctrl0073/ctrl0073system.h> /* NV0073_CTRL_SYSTEM_CAPS_TBL_SIZE */
#include <ctrl/ctrl0000/ctrl0000gpu.h> /* NV0000_CTRL_GPU_MAX_ATTACHED_GPUS */
#include <ctrl/ctrl0080/ctrl0080unix.h> /* NV0080_CTRL_OS_UNIX_VT_SWITCH_FB_INFO */
#include <ctrl/ctrl2080/ctrl2080gpu.h> /* NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_* */
#include <ctrl/ctrl30f1.h> /* NV30F1_CTRL_MAX_GPUS_PER_GSYNC */
#include <ctrl/ctrl5070/ctrl5070system.h> /* NV5070_CTRL_SYSTEM_CAPS_TBL_SIZE */
#include <ctrl/ctrl0073/ctrl0073dfp.h>

#include "nvkms-api.h"
#include "nvkms-prealloc-types.h"

#include "nvos.h"

#include "nv_common_utils.h"
#include "nv_assert.h"
#include "unix_rm_handle.h"

#include "nvmisc.h"

#include "nvidia-push-init.h"

#include "timing/nvtiming.h"
#include "timing/dpsdp.h"
#include "timing/nvt_dsc_pps.h"
#include "timing/dpsdp.h"
#include "hdmipacket/nvhdmi_frlInterface.h" // HDMI_{SRC,SINK}_CAPS

#include <stddef.h>

#include "nv_smg.h"

#if defined(DEBUG) || defined(DEVELOP)
#define NVKMS_PROCFS_ENABLE 1
#else
#define NVKMS_PROCFS_ENABLE 0
#endif

#define NV_DMA_EVO_PUSH_BUFFER_SIZE         (4 * 1024)
#define NV_DMA_EVO_PUSH_BUFFER_PAD_SIZE     (4 * 12)
#define NV_DMA_EVO_NOTIFIER_SIZE            4096

#define NV_NUM_EVO_LUT_ENTRIES              1025
/*
 * Size of the nvdisplay 3 LUT variable segment size header, in LUT entries
 * (which are 8 bytes each).
 */
#define NV_LUT_VSS_HEADER_SIZE              4

#define NV_EVO_SUBDEV_STACK_SIZE            10

#define NV_DP_READ_EDID_RETRIES             18
#define NV_DP_REREAD_EDID_DELAY_USEC        500 /* in microseconds */

#define NV_EVO_SURFACE_ALIGNMENT            0x1000

/*
 * Prior to nvdisplay 4.0, the final address for all scanout surfaces must be
 * 256B-aligned.
 *
 * For nvdisplay 4.0, the final address for all scanout surfaces must be
 * 512B-aligned for GPU, and 1KB-aligned for Tegra.
 *
 * NVKMS already uses NV_EVO_SURFACE_ALIGNMENT to force 4KB-alignment for the
 * base address of each scanout surface. As such, we're forcing 1KB-alignment
 * for the corresponding ctxdma offsets in order to be compatible with all
 * display architectures.
 */
#define NV_SURFACE_OFFSET_ALIGNMENT_SHIFT   10

#define NVKMS_BLOCK_LINEAR_LOG_GOB_WIDTH    6U    /*    64 bytes (2^6) */
#define NVKMS_BLOCK_LINEAR_GOB_WIDTH        ((NvU32)1 << NVKMS_BLOCK_LINEAR_LOG_GOB_WIDTH)

#define NVKMS_BLOCK_LINEAR_LOG_GOB_HEIGHT   3U    /*    8 rows (2^3) */
#define NVKMS_BLOCK_LINEAR_GOB_HEIGHT       ((NvU32)1 << NVKMS_BLOCK_LINEAR_LOG_GOB_HEIGHT)

#define NV_INVALID_OR                      0xFFFFFFFF

#define NVKMS_RM_HEAP_ID                    0xDCBA

#define NVKMS_MAX_WINDOWS_PER_DISP          32

#define NV_SYNCPT_GLOBAL_TABLE_LENGTH      1024

#define HEAD_MASK_QUERY(_mask, _head) (!!((_mask) & (1 << (_head))))
#define HEAD_MASK_SET(_mask, _head) ((_mask) | (1 << (_head)))
#define HEAD_MASK_UNSET(_mask, _head) ((_mask) & ~(1 << (_head)))

#define NVKMS_COMPOSITION_FOR_MATCH_BITS(__colorKeySelect, __match)            \
    for ((__match) = (((__colorKeySelect) ==                                   \
                         NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE) ? 1 : 0); \
         (__match) <= 1; (__match)++)

typedef struct _NVEvoApiHandlesRec *NVEvoApiHandlesPtr;
typedef struct _NVEvoSubDeviceRec *NVSubDeviceEvoPtr;
typedef struct _NVEvoDevRec *NVDevEvoPtr;
typedef struct _NVDIFRStateEvoRec *NVDIFRStateEvoPtr;
typedef struct _NVDmaBufferEvoRec *NVDmaBufferEvoPtr;
typedef struct _NVEvoChannel *NVEvoChannelPtr;
typedef struct _NVEvoHeadControl *NVEvoHeadControlPtr;
typedef struct _NVEvoCapabilities *NVEvoCapabilitiesPtr;
typedef struct _NVEvoSubDevHeadStateRec *NVEvoSubDevHeadStatePtr;
typedef struct _NVEvoSubDevRec *NVEvoSubDevPtr;
typedef struct _NVEvoColorRec *NVEvoColorPtr;
typedef struct _NVHwModeViewPortEvo *NVHwModeViewPortEvoPtr;
typedef struct _NVHwModeTimingsEvo *NVHwModeTimingsEvoPtr;
typedef struct _NVConnectorEvoRec *NVConnectorEvoPtr;
typedef struct _NVVblankSyncObjectRec *NVVblankSyncObjectPtr;
typedef struct _NVDispHeadStateEvoRec *NVDispHeadStateEvoPtr;
typedef struct _NVDispEvoRec *NVDispEvoPtr;
typedef struct _NVParsedEdidEvoRec *NVParsedEdidEvoPtr;
typedef struct _NVVBlankCallbackRec *NVVBlankCallbackPtr;
typedef struct _NVRgLine1CallbackRec *NVRgLine1CallbackPtr;
typedef struct _NVDpyEvoRec *NVDpyEvoPtr;
typedef struct _NVFrameLockEvo *NVFrameLockEvoPtr;
typedef struct _NVEvoInfoString *NVEvoInfoStringPtr;
typedef struct _NVSurfaceEvoRec NVSurfaceEvoRec, *NVSurfaceEvoPtr;
typedef struct _NVDeferredRequestFifoRec *NVDeferredRequestFifoPtr;
typedef struct _NVSwapGroupRec *NVSwapGroupPtr;
typedef struct _NVEvoModesetUpdateState NVEvoModesetUpdateState;
typedef struct _NVLockGroup NVLockGroup;
typedef struct _NVVblankSemControl *NVVblankSemControlPtr;

/*
 * _NVHs*EvoRec are defined in nvkms-headsurface-priv.h; they are intentionally
 * opaque outside of the nvkms-headsurface code.
 */
typedef struct _NVHsDeviceEvoRec *NVHsDeviceEvoPtr;
typedef struct _NVHsChannelEvoRec *NVHsChannelEvoPtr;
typedef struct _NVHsSurfaceRec *NVHsSurfacePtr;

/* _nv_dplibXXX are defined in dp/nvdp-connector-event-sink.h */
typedef struct _nv_dplibconnector NVDPLibConnectorRec, *NVDPLibConnectorPtr;
typedef struct _nv_dplibdevice NVDPLibDeviceRec, *NVDPLibDevicePtr;
typedef struct __nv_dplibmodesetstate NVDPLibModesetStateRec, *NVDPLibModesetStatePtr;

/* _nv_dplibtimer is defined in nvdp-timer.hpp */
typedef struct _nv_dplibtimer NVDPLibTimerRec, *NVDPLibTimerPtr;

typedef struct _NVEvoApiHandlesRec {
    void **pointers; /* Dynamically allocated array of pointers. */
    NvU32 numPointers; /* Number of elements in pointers array. */
    NvU32 defaultSize;
} NVEvoApiHandlesRec;

typedef struct _NVSurfaceDescriptor
{
    NvU32 ctxDmaHandle;
    NvU32 memAperture;
    NvU64 memOffset;
    NvBool bValid;
} NVSurfaceDescriptor;

typedef struct _NVEvoDma
{
    NvU32 memoryHandle;
    NVSurfaceDescriptor surfaceDesc;

    NvU64 limit;

    /* Whether this is sysmem, or vidmem accessed through a BAR1 mapping. */
    NvBool isBar1Mapping;

    void  *subDeviceAddress[NVKMS_MAX_SUBDEVICES];
} NVEvoDma, *NVEvoDmaPtr;

typedef struct _NVDmaBufferEvoRec
{
    NVEvoDma        dma;

    NvU32           channel_handle;  // handles
    NvU32           num_channels;
    void           *control[NVKMS_MAX_SUBDEVICES];
    NvU32          *base;  // Push buffer start pointer
    NvU32          *buffer;// Push buffer current pointer
    NvU32          *end;   // Push buffer end pointer
    NvU32           offset_max; // Push buffer max offset (in bytes)
    NvU32           put_offset; // Push buffer last kicked off offset
    NvU32           fifo_free_count; // fifo free space (in words)
    NvU32           currentSubDevMask;
    NVDevEvoPtr     pDevEvo;
} NVDmaBufferEvoRec;

/* EVO capabilities */
typedef struct {
    NvBool flipLock;
    NvBool stereo;
    NvBool scanLock;
} NVEvoLockPinCaps;
#define NV_EVO_NUM_LOCK_PIN_CAPS 16

typedef struct {
    NvBool supportsInterlaced;
    NvBool supportsSemiPlanar;
    NvBool supportsPlanar;
    NvBool supportsHVFlip;
    NvBool supportsDSI;
} NVEvoMiscCaps;

static inline NvU8 NVEvoScalerTapsToNum(NVEvoScalerTaps taps)
{
    NvU8 numTaps = 1;

    switch (taps) {
        case NV_EVO_SCALER_8TAPS:
            numTaps = 8;
            break;
        case NV_EVO_SCALER_5TAPS:
            numTaps = 5;
            break;
        case NV_EVO_SCALER_3TAPS:
            numTaps = 3;
            break;
        case NV_EVO_SCALER_2TAPS:
            numTaps = 2;
            break;
        case NV_EVO_SCALER_1TAP:
            numTaps = 1;
            break;
    }

    return numTaps;
}

#define NV_EVO_SCALE_FACTOR_1X  (1 << 10)
#define NV_EVO_SCALE_FACTOR_2X  (2 << 10)
#define NV_EVO_SCALE_FACTOR_3X  (3 << 10)
#define NV_EVO_SCALE_FACTOR_4X  (4 << 10)

typedef struct {
    NvU32 maxPixelsVTaps;
    NvU16 maxVDownscaleFactor; /* Scaled by 1024 */
    NvU16 maxHDownscaleFactor; /* Scaled by 1024 */
} NVEvoScalerTapsCaps;

typedef struct {
    NvBool present;
    NVEvoScalerTapsCaps taps[NV_EVO_SCALER_TAPS_MAX + 1];
} NVEvoScalerCaps;

typedef struct {
    NvBool usable;
    NvU32 maxPClkKHz;
    NvBool supportsHDMIYUV420HW;
    NVEvoScalerCaps scalerCaps;
} NVEvoHeadCaps;
#define NV_EVO_NUM_HEAD_CAPS 8

typedef struct {
    NvBool dualTMDS;
    NvU32 maxTMDSClkKHz;
} NVEvoSorCaps;
#define NV_EVO_NUM_SOR_CAPS 8

typedef struct {
    NvBool usable;
    NvBool csc0MatricesPresent;
    NvBool cscLUTsPresent;
    NvBool csc10MatrixPresent;
    NvBool csc11MatrixPresent;
    NvBool tmoPresent;
    NVEvoScalerCaps scalerCaps;
} NVEvoWindowCaps;
#define NV_EVO_NUM_WINDOW_CAPS 32

/*
 * The GB20X+ gpus support two different tile variants:
 *   1. TYPE_0: This tile variant supports legacy post-composition
 *   features which excludes LTM (Local Tone Mapping) and 3DLUT.
 *   2. TYPE_1: A trimmed configuration that does not support LTM
 *   (Local Tone Mapping), SCALER, VFILTER (used in YUV420
 *   down-sampling), or 3DLUT.
 *
 * TYPE_0 is the top variant of tile configurations, with trimming
 * applied progressively from TYPE_0 to TYPE_N.
 *
 * The function EvoIsModePossibleCA() selects and assigns the correct
 * tile type to the given heads.
 */
typedef enum _NVEvoHwTileType {
    NV_EVO_HW_TILE_TYPE_FIRST = 0,
    NV_EVO_HW_TILE_TYPE_0 =
        NV_EVO_HW_TILE_TYPE_FIRST,
    NV_EVO_HW_TILE_TYPE_1,
    NV_EVO_HW_TILE_TYPE_LAST =
        NV_EVO_HW_TILE_TYPE_1,
} NVEvoHwTileType;

typedef struct {
    NVEvoHwTileType type;
    NvBool usable;
    NvBool supportMultiTile;
} NVEvoHwTileCaps;

typedef struct {
    NvBool supportMultiTile;
} NVEvoHwPhywinCaps;


typedef NvU64 NVEvoChannelMask;

#define NV_EVO_CHANNEL_MASK_CORE                          0:0
#define NV_EVO_CHANNEL_MASK_CORE_ENABLE                     1
#define NV_EVO_CHANNEL_MASK_CORE_DISABLE                    0
#define NV_EVO_CHANNEL_MASK_WINDOW_FIELD                 32:1
#define NV_EVO_CHANNEL_MASK_WINDOW(_n)      (1+(_n)):(1+(_n))
#define NV_EVO_CHANNEL_MASK_WINDOW__SIZE                   32
#define NV_EVO_CHANNEL_MASK_WINDOW_ENABLE                   1
#define NV_EVO_CHANNEL_MASK_WINDOW_DISABLE                  0
#define NV_EVO_CHANNEL_MASK_CURSOR_FIELD                40:33
#define NV_EVO_CHANNEL_MASK_CURSOR(_n)    (33+(_n)):(33+(_n))
#define NV_EVO_CHANNEL_MASK_CURSOR__SIZE                    8
#define NV_EVO_CHANNEL_MASK_CURSOR_ENABLE                   1
#define NV_EVO_CHANNEL_MASK_CURSOR_DISABLE                  0
#define NV_EVO_CHANNEL_MASK_BASE_FIELD                  44:41
#define NV_EVO_CHANNEL_MASK_BASE(_n)      (41+(_n)):(41+(_n))
#define NV_EVO_CHANNEL_MASK_BASE__SIZE                      4
#define NV_EVO_CHANNEL_MASK_BASE_ENABLE                     1
#define NV_EVO_CHANNEL_MASK_BASE_DISABLE                    0
#define NV_EVO_CHANNEL_MASK_OVERLAY_FIELD               48:45
#define NV_EVO_CHANNEL_MASK_OVERLAY(_n)   (45+(_n)):(45+(_n))
#define NV_EVO_CHANNEL_MASK_OVERLAY__SIZE                   4
#define NV_EVO_CHANNEL_MASK_OVERLAY_ENABLE                  1
#define NV_EVO_CHANNEL_MASK_OVERLAY_DISABLE                 0
/* Window Immediate channels get only one bit. */
#define NV_EVO_CHANNEL_MASK_WINDOW_IMM                  49:49
#define NV_EVO_CHANNEL_MASK_WINDOW_IMM_ENABLE               1
#define NV_EVO_CHANNEL_MASK_WINDOW_IMM_DISABLE              0

#define NV_EVO_CHANNEL_MASK_WINDOW_ALL \
    DRF_SHIFTMASK64(NV_EVO_CHANNEL_MASK_WINDOW_FIELD)
#define NV_EVO_CHANNEL_MASK_CURSOR_ALL \
    DRF_SHIFTMASK64(NV_EVO_CHANNEL_MASK_CURSOR_FIELD)
#define NV_EVO_CHANNEL_MASK_BASE_ALL \
    DRF_SHIFTMASK64(NV_EVO_CHANNEL_MASK_BASE_FIELD)
#define NV_EVO_CHANNEL_MASK_OVERLAY_ALL \
    DRF_SHIFTMASK64(NV_EVO_CHANNEL_MASK_OVERLAY_FIELD)

static inline NvU32 NV_EVO_CHANNEL_MASK_POPCOUNT(NvU64 mask)
{
    // It's tempting to use __builtin_popcountll here, but that depends on
    // intrinsics not available to nvkms in the kernel.
    return nvPopCount64(mask);
}

static inline NvU32 NV_EVO_CHANNEL_MASK_BASE_HEAD_NUMBER(NvU64 mask)
{
    nvAssert(NV_EVO_CHANNEL_MASK_POPCOUNT(mask) == 1);
    return BIT_IDX_64(DRF_VAL64(_EVO, _CHANNEL_MASK, _BASE_FIELD, mask));
}
static inline NvU32 NV_EVO_CHANNEL_MASK_OVERLAY_HEAD_NUMBER(NvU64 mask)
{
    nvAssert(NV_EVO_CHANNEL_MASK_POPCOUNT(mask) == 1);
    return BIT_IDX_64(DRF_VAL64(_EVO, _CHANNEL_MASK, _OVERLAY_FIELD, mask));
}
static inline NvU32 NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(NvU64 mask)
{
    nvAssert(NV_EVO_CHANNEL_MASK_POPCOUNT(mask) == 1);
    return BIT_IDX_64(DRF_VAL64(_EVO, _CHANNEL_MASK, _WINDOW_FIELD, mask));
}

/* EVO structures */

typedef struct {
    struct {
        NVEvoChannelMask channelMask;
        NVEvoChannelMask noCoreInterlockMask;
        /* Each channel in this mask was programmed with a "flip lock
         * qualifying" flip. */
        NVEvoChannelMask flipLockQualifyingMask;
        /* Channels set here are transitioning from NULL ctxdma to non-NULL
         * ctxdma or vice-versa on this update.  Only necessary/valid on Turing
         * (class C5*). */
        NVEvoChannelMask flipTransitionWAR;

        struct {
            NvBool vrrTearing;
        } base[NVKMS_MAX_HEADS_PER_DISP];

        /*
         * Window immediate channels with pending methods are represented
         * here by NV_EVO_CHANNEL_MASK_WINDOW(n) for window immediate
         * channel n.
         */
        NVEvoChannelMask winImmChannelMask;

        /*
         * Each window channel NV_EVO_CHANNEL_MASK_WINDOW(n) needs to
         * be interlocked with its corresponding window immediate channel n.
         */
        NVEvoChannelMask winImmInterlockMask;

    } subdev[NVKMS_MAX_SUBDEVICES];

} NVEvoUpdateState;

struct _NVEvoModesetUpdateState {
    NVEvoUpdateState updateState;
    NVDpyIdList connectorIds;
    const NVDPLibModesetStateRec
        *pDpLibModesetState[NVKMS_MAX_HEADS_PER_DISP];
    NvBool windowMappingChanged;
    struct {
        struct _NVEvoModesetUpdateStateOneLayer {
            struct nvkms_ref_ptr *ref_ptr;
            NvBool changed;
        } layer[NVKMS_MAX_LAYERS_PER_HEAD];
    } flipOccurredEvent[NVKMS_MAX_HEADS_PER_DISP];
};

typedef struct {
    struct {
        NVEvoChannelMask channelMask;
    } subdev[NVKMS_MAX_SUBDEVICES];
} NVEvoIdleChannelState;

typedef struct {
    NvU8 validTimeStampBits;
    NvBool tearingFlips         :1;
    NvBool vrrTearingFlips      :1;
    NvBool perEyeStereoFlips    :1;
} NVEvoChannelCaps;

enum NVEvoImmChannel {
    NV_EVO_IMM_CHANNEL_NONE,
    NV_EVO_IMM_CHANNEL_PIO,
    NV_EVO_IMM_CHANNEL_DMA,
};

typedef struct {
    NvU32 handle;
    void *control[NVKMS_MAX_SUBDEVICES];
} NVEvoPioChannel;

/*! basic syncpt structure used for pre and post syncpt usage */
typedef struct _NVEvoSyncpt {
    /* syncpt is allocated */
    NvBool allocated;
    /*! syncpt id (only useful for post-syncpt) */
    NvU32 id;
    /*! bitmask of channels using this syncpt */
    NVEvoChannelMask channelMask;
    /*! Surface descriptor for the syncpt */
    NVSurfaceDescriptor surfaceDesc;
    /*! handle of syncpt object */
    NvU32 hSyncpt;
    /*! stores syncpt max value */
    NvU32 syncptMaxVal;
} NVEvoSyncpt;

/* Tracks internal state of a vblank sync object. */
typedef struct _NVVblankSyncObjectRec {
    /* Whether the vblank sync object is currently in use by some client. */
    NvBool inUse;

    /* Whether the vblank sync object is enabled or disabled. */
    NvBool enabled;

    /*
     * The index of this Rec inside of the HeadState's vblankSyncObjects array.
     * Also corresponds with the index of the sync object in hardware.
     */
    NvU32 index;

    /*
     * This syncpoint object should be created as part of
     * nvRmSetupEvoCoreChannel().
     */
    NVEvoSyncpt evoSyncpt;
} NVVblankSyncObjectRec;

/* EVO channel, encompassing multiple subdevices and a single pushbuf */
typedef struct _NVEvoChannel {
    /* Pointer to array of per subdev notifier dma structs */
    NVEvoDmaPtr                 notifiersDma;

    NvU32                       hwclass;
    NvU32                       instance;
    NVEvoChannelMask            channelMask; /* only one bit should be set */

    NVDmaBufferEvoRec           pb;

    NVOS10_EVENT_KERNEL_CALLBACK_EX completionNotifierEventCallback;
    const struct nvkms_ref_ptr *completionNotifierEventRefPtr;
    NvU32 completionNotifierEventHandle;

    /*
     * GV100 timestamped flips need a duplicate update which only changes
     * TIMESTAMP_MODE and MIN_PRESENT_INTERVAL fields in SET_PRESENT_CONTROL;
     * to allow updating these fields without changing anything else in
     * SET_PRESENT_CONTROL, normal updates to SET_PRESENT_CONTROL are cached
     * here. (bug 1990958)
     */
    NvU32 oldPresentControl;

    // On Turing, RM wants to be notified when the tearing mode changes.
    NvBool oldTearingMode;

    struct {
        enum NVEvoImmChannel type;
        union {
            NVEvoPioChannel *pio;
            struct _NVEvoChannel *dma;
        } u;
    } imm;

    NVEvoChannelCaps caps;

    NVEvoSyncpt postSyncpt;

    struct {
        NvBool enabled;
        NvBool clientSpecified;
        NvU32 srcMaxLum;
        NvU32 targetMaxLums[NVKMS_MAX_SUBDEVICES];
    } tmoParams;
} NVEvoChannel;

typedef enum {
    NV_EVO_NO_LOCK,
    NV_EVO_FRAME_LOCK,
    NV_EVO_RASTER_LOCK,
} NVEvoLockMode;

typedef enum {
    NV_EVO_LOCK_PIN_ERROR = -1,
    NV_EVO_LOCK_PIN_INTERNAL_0 = 0,
    NV_EVO_LOCK_PIN_0 = 0x20,
} NVEvoLockPin;

typedef struct _NVEvoHeadControl {
    NvBool                      interlaced;
    NVEvoLockMode               clientLock;
    NVEvoLockPin                clientLockPin;
    int                         clientLockoutWindow;
    NVEvoLockMode               serverLock;
    NVEvoLockPin                serverLockPin;
    NvBool                      flipLock;
    NVEvoLockPin                flipLockPin;
    NVEvoLockPin                stereoPin;

    NvBool                      mergeMode;
    NvBool                      setLockOffsetX;

    NvU32                       stallLockPin;
    NvBool                      useStallLockPin;
    NvBool                      crashLockUnstallMode;

    /*
     * Whether or not this GPU is stereo locked.  True if all heads are either
     * frame or raster locked, and all heads are driving non-interlaced modes.
     */
    NvBool                      stereoLocked;

    /*
     * Whether or not this head is driving a HDMI 3D frame packed mode.  Used
     * in headcontrol only on >=GV100.
     */
    NvBool                      hdmi3D;

    /*
     * Whether or not this head is driving a mode requiring the HW YUV420
     * packer.  Used in headcontrol only on >=nvdisplay 4.0.
     */
    NvBool                      hwYuv420;

    /* This isn't actually part of HeadControl, but it's convenient */
    NvU32                       lockChainPosition;
} NVEvoHeadControl;

typedef struct _NVEvoCapabilities {
    NVEvoLockPinCaps            pin[NV_EVO_NUM_LOCK_PIN_CAPS];
    NVEvoMiscCaps               misc;
    NVEvoHeadCaps               head[NV_EVO_NUM_HEAD_CAPS];
    NVEvoSorCaps                sor[NV_EVO_NUM_SOR_CAPS];
    NVEvoWindowCaps             window[NV_EVO_NUM_WINDOW_CAPS];
    NVEvoHwTileCaps             hwTile[8];
    NVEvoHwPhywinCaps           hwPhywin[32];
} NVEvoCapabilities;

typedef struct {
    NVSurfaceEvoPtr pSurfaceEvo;
    enum NvKmsNIsoFormat format;
    NvU16 offsetInWords;
} NVFlipNIsoSurfaceEvoHwState;

typedef struct {
    NVFlipNIsoSurfaceEvoHwState surface;
    NvBool awaken;
} NVFlipCompletionNotifierEvoHwState;

typedef struct {
    NvBool usingSyncpt;
    union {
        struct {
            NVFlipNIsoSurfaceEvoHwState acquireSurface;
            NvU32 acquireValue;
            NVFlipNIsoSurfaceEvoHwState releaseSurface;
            NvU32 releaseValue;
        } semaphores;
        struct {
            NvBool isPreSyncptSpecified;
            NvU32 preSyncpt;
            NvU32 preValue;

            NvBool isPostSyncptSpecified;
            NVSurfaceDescriptor surfaceDesc;
            NvU32 postValue;
        } syncpts;
    } u;
} NVFlipSyncObjectEvoHwState;

typedef struct {
    NVSurfaceEvoPtr pLutSurfaceEvo;
    NvU64 offset;
    NvU32 vssSegments;
    NvU32 lutEntries;
    NvBool fromOverride;
} NVFlipLutHwState;

typedef struct {
    NVSurfaceEvoPtr pSurfaceEvo;
    NvS16 x, y;

    struct NvKmsCompositionParams cursorCompParams;
} NVFlipCursorEvoHwState;

typedef struct {
    NVSurfaceEvoPtr pSurfaceEvo[NVKMS_MAX_EYES];
    NVFlipCompletionNotifierEvoHwState completionNotifier;
    NVFlipSyncObjectEvoHwState syncObject;

    // Non-zero timeStamp value is only allowed if the channel's
    // 'timeStampFlipBits' capability is > 0.
    NvU64 timeStamp;
    NvU8 minPresentInterval;
    // True means immediate or tearing flip.  False means flip-at-vblank.
    NvBool tearing;
    // The tearing mode passed to RM's VRR code via
    // NV_VRR_TRAP_ARGUMENT_MAX_FPS_TEARING.
    NvBool vrrTearing;
    NvBool perEyeStereoFlip;

    struct NvKmsSize sizeIn;
    struct NvKmsSize sizeOut;
    struct NvKmsSignedPoint outputPosition;

    NVEvoScalerTaps hTaps;
    NVEvoScalerTaps vTaps;

    struct NvKmsCscMatrix cscMatrix;

    NVFlipLutHwState inputLut;

    struct NvKmsRRParams rrParams;

    struct NvKmsCompositionParams composition;

    NVFlipLutHwState tmoLut;
    struct {
        struct NvKmsHDRStaticMetadata val;
        NvBool enabled;
    } hdrStaticMetadata;

    enum NvKmsInputColorSpace colorSpace;
    enum NvKmsInputTf tf;
    enum NvKmsInputColorRange colorRange;

    struct {
        NvBool specified;

        /*
         * Maximum vertical downscale factor (scaled by 1024)
         *
         * For example, if the downscale factor is 1.5, then maxVDownscaleFactor
         * would be 1.5 x 1024 = 1536.
         */
        NvU16 vertical;

        /*
         * Maximum horizontal downscale factor (scaled by 1024)
         *
         * See the example above for vertical.
         */
        NvU16 horizontal;
    } maxDownscaleFactors;

    struct {
        struct NvKmsCscMatrix matrix;
        NvBool enabled;
    } csc00Override;

    struct {
        struct NvKmsCscMatrix matrix;
        NvBool enabled;
    } csc01Override;

    struct {
        struct NvKmsCscMatrix matrix;
        NvBool enabled;
    } csc10Override;

    struct {
        struct NvKmsCscMatrix matrix;
        NvBool enabled;
    } csc11Override;
} NVFlipChannelEvoHwState;

typedef struct {
    struct NvKmsPoint viewPortPointIn;
    NVFlipCursorEvoHwState cursor;
    NVFlipChannelEvoHwState layer[NVKMS_MAX_LAYERS_PER_HEAD];
    struct NvKmsUsageBounds usage;
    NvBool disableMidFrameAndDWCFWatermark;
    enum NvKmsOutputTf tf;

    struct {
        NvBool enabled;
        enum NvKmsInfoFrameEOTF eotf;
        struct NvKmsHDRStaticMetadata staticMetadata;
    } hdrInfoFrame;

    NVFlipLutHwState outputLut;
    NvU32 olutFpNormScale;

    NvBool skipLayerPendingFlips[NVKMS_MAX_LAYERS_PER_HEAD];

    struct {
        NvBool viewPortPointIn   : 1;
        NvBool cursorSurface     : 1;
        NvBool cursorPosition    : 1;
        NvBool tf                : 1;
        NvBool hdrStaticMetadata : 1;
        NvBool olut              : 1;

        NvBool layerPosition[NVKMS_MAX_LAYERS_PER_HEAD];
        NvBool layerSyncObjects[NVKMS_MAX_LAYERS_PER_HEAD];
        NvBool layer[NVKMS_MAX_LAYERS_PER_HEAD];
    } dirty;
} NVFlipEvoHwState;

/*
 * XXX Default to NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING to send SDR
 * infoframe for 2 seconds to WAR issue where some displays may remain in HDR
 * mode after head has been previously shut down.
 */
enum NvKmsHDRInfoFrameState {
    NVKMS_HDR_INFOFRAME_STATE_TRANSITIONING = 0,
    NVKMS_HDR_INFOFRAME_STATE_DISABLED      = 1,
    NVKMS_HDR_INFOFRAME_STATE_ENABLED       = 2,
};

typedef struct _NVEvoSubDevHeadStateRec {
    struct NvKmsPoint viewPortPointIn;
    NVFlipCursorEvoHwState cursor;
    NVFlipChannelEvoHwState layer[NVKMS_MAX_LAYERS_PER_HEAD];
    NVFlipLutHwState outputLut;
    NvU32 olutFpNormScale;
    // Current usage bounds programmed into the hardware.
    struct NvKmsUsageBounds usage;
    // Usage bounds required after the last scheduled flip completes.
    struct NvKmsUsageBounds targetUsage;
    // Preallocated usage bounds that will be required for upcoming flips.
    struct NvKmsUsageBounds preallocatedUsage;

    // Current state of MidFrameAndDWCFWatermark programmed into the hardware.
    NvBool disableMidFrameAndDWCFWatermark;
    //
    // State of MidFrameAndDWCFWatermark required after the last scheduled
    // flip completes.
    //
    NvBool targetDisableMidFrameAndDWCFWatermark;
} NVEvoSubDevHeadStateRec;

#define NVKMS_HEAD_SURFACE_MAX_BUFFERS 2

/*
 * HeadSurface state that applies to a single head, but spans across
 * all subdevices.
 */
typedef struct {
    /*
     * The size of the headSurfaces for this head, across all subdevices.
     * headSurface might only use a subset of the surfaces on one or more
     * subdevices in SLI Mosaic.
     */
    struct NvKmsSize size;
    struct NvKmsSize stagingSize;

    /*
     * The surfaces allocated for use by headSurface on this head.
     * Surface allocations are broadcast across subdevices, though
     * headSurface may unicast its rendering to the headSurface
     * surface allocations on specific subdevices.
     */
    struct {
        NVHsSurfacePtr pSurface;
        NVHsSurfacePtr pStagingSurface;
    } surfaces[NVKMS_MAX_EYES][NVKMS_HEAD_SURFACE_MAX_BUFFERS];

    /*
     * The number of surfaces in the NVKMS_HEAD_SURFACE_MAX_BUFFERS dimension of
     * the surfaces[][] array.  Elements [0,surfaceCount-1] in the surfaces[][]
     * array will be populated.
     */
    NvU32 surfaceCount;
} NVHsStateOneHeadAllDisps;

/* Subdevice-specific, channel-independent state */
typedef struct _NVEvoSubDevRec {
    NvU32                       subDeviceInstance;

    NVEvoCapabilities           capabilities;

    NVDispEvoPtr                pDispEvo;

    NvU32                       setSwSpareA[NVKMS_MAX_HEADS_PER_DISP];

    NVEvoSubDevHeadStateRec     headState[NVKMS_MAX_HEADS_PER_DISP];
    NVEvoHeadControl            headControl[NVKMS_MAX_HEADS_PER_DISP];
    NVEvoHeadControl            headControlAssy[NVKMS_MAX_HEADS_PER_DISP];
    void                       *cursorPio[NVKMS_MAX_HEADS_PER_DISP];
    NvBool                      (*scanLockState)(NVDispEvoPtr pDispEvo,
                                                 NVEvoSubDevPtr pEvoSubDev,
                                                 NvU32 action,
                                                 /* NV_INVALID_HEAD-terminated
                                                  * array of head indices */
                                                 const NvU32 *pHeads);

    /*
     * EVO state machine refcounter for the number of SLI or proxy framelock
     * clients that are connected to this server.
     */
    NvU32                        frameLockSliProxyClients;

    /*
     * Since we add all active heads as framelock clients whenever we enable
     * the second head as a framelock client, there's no need for EVO state
     * transitions for heads 3 and more.  Instead of those state transitions,
     * we use the frameLockExtraClients ref counter to keep track of heads
     * 3 and greater being added as framelock clients.
     *
     * XXX The state machine currently will naively framelock heads 3 and
     * greater during this state transition, even if they're not capable
     * of being framelocked (for example, when they have very different
     * refresh rates).  Bug 976532
     */
    NvU32                        frameLockExtraClients;

    /*
     * All of the following except the "armed" versions are set by the EVO
     * state machine to the desired HW configuration given the current locking
     * state.
     * The "armed" versions represent the current hardware configuration, used
     * to avoid excess hardware updates.
     */
    NvU32                        frameLockServerMaskArmed;
    NvU32                        frameLockServerMaskAssy;
    NvU32                        frameLockClientMaskArmed;
    NvU32                        frameLockClientMaskAssy;
    NvU32                        frameLockExtRefClkMaskArmed;
    NvU32                        frameLockExtRefClkMaskAssy;
    NvBool                       frameLockHouseSync;

    NvU8                         flipLockPinSetForFrameLockHeadMask;
    NvU8                         flipLockEnabledForFrameLockHeadMask;
    NvU8                         flipLockPinSetForSliHeadMask;
    NvU8                         flipLockEnabledForSliHeadMask;

    NvU32                        flipLockProhibitedHeadMask;

    NvU32                        sliRasterLockServerMask;
    NvU32                        sliRasterLockClientMask;

    NVEvoLockPin                 sliServerLockPin;
    NVEvoLockPin                 sliClientLockPin;
} NVEvoSubDevRec;

typedef struct _NVEvoColorRec {
    NvU16 red;
    NvU16 green;
    NvU16 blue;
} NVEvoColorRec;

typedef struct  {
    NvU16 Red;
    NvU16 Green;
    NvU16 Blue;
    NvU16 Unused;
} NVEvoLutEntryRec;

typedef struct {
    NVEvoLutEntryRec base[NV_LUT_VSS_HEADER_SIZE + NV_NUM_EVO_LUT_ENTRIES];
    // The output LUT requires 8-bit alignment.
    NVEvoLutEntryRec output[NV_LUT_VSS_HEADER_SIZE + NV_NUM_EVO_LUT_ENTRIES]
        __attribute__((aligned(0x100)));
} NVEvoLutDataRec;

typedef struct {
    NvBool supportsDP13                    :1;
    NvBool supportsHDMI20                  :1;
    NvBool supportsYUV2020                 :1;
    NvBool inputLutAppliesToBase           :1;
    NvU8   validNIsoFormatMask;
    NvU32  maxPitchValue;
    int    maxWidthInBytes;
    int    maxWidthInPixels;
    int    maxHeight;
    NvU32  maxRasterWidth;
    NvU32  maxRasterHeight;
    struct NvKmsCompositionCapabilities cursorCompositionCaps;
    NvU16  validLayerRRTransforms;
    struct NvKmsLayerCapabilities layerCaps[NVKMS_MAX_LAYERS_PER_HEAD];
    struct NvKmsLUTCaps olut;
    NvU8 legacyNotifierFormatSizeBytes[NVKMS_MAX_LAYERS_PER_HEAD];
    NvU8 dpYCbCr422MaxBpc;
    NvU8 hdmiYCbCr422MaxBpc;
} NVEvoCapsRec;

typedef struct {
    NvU32  coreChannelClass;
    size_t dmaArmedSize;
    NvU32  dmaArmedOffset;
} NVEvoCoreChannelDmaRec;


typedef struct _NVEvoSubDeviceRec {
    NvU32           handle;
    NvU32           gpuId;
#define NV_INVALID_GPU_LOG_INDEX 0xFF
    NvU8            gpuLogIndex;
    char            gpuString[NVKMS_GPU_STRING_SIZE];

    NvU32           numEngines;
    NvU32          *supportedEngines;

    /* Core channel memory mapping for ARM values */
    void           *pCoreDma;

    /* ISO ctxdma programmed by EVO2 hal, into the overlay channel */
    NvU32 overlayContextDmaIso[NVKMS_MAX_HEADS_PER_DISP];
    enum NvKmsSurfaceMemoryFormat overlaySurfFormat[NVKMS_MAX_HEADS_PER_DISP];

    /* Per head surface programmed into the core channel */
    const NVSurfaceEvoRec *pCoreChannelSurface[NVKMS_MAX_HEADS_PER_DISP];

    /* EVO2 only, TRUE if a valid base surface passed to ->Flip() */
    NvBool isBaseSurfSpecified[NVKMS_MAX_HEADS_PER_DISP];
    enum NvKmsSurfaceMemoryFormat baseSurfFormat[NVKMS_MAX_HEADS_PER_DISP];

    /* EVO2 only, base and output LUT state - prevents unnecessary flip interlocking */
    const NVSurfaceEvoRec *pBaseLutSurface[NVKMS_MAX_HEADS_PER_DISP];
    NvU64 baseLutOffset[NVKMS_MAX_HEADS_PER_DISP];
    const NVSurfaceEvoRec *pOutputLutSurface[NVKMS_MAX_HEADS_PER_DISP];
    NvU64 outputLutOffset[NVKMS_MAX_HEADS_PER_DISP];

    /* Composition parameters considered for hardware programming by EVO2 hal */
    struct {
        NvBool initialized;
        enum NvKmsCompositionColorKeySelect colorKeySelect;
        NVColorKey colorKey;
    } baseComp[NVKMS_MAX_HEADS_PER_DISP], overlayComp[NVKMS_MAX_HEADS_PER_DISP];

} NVEvoSubDeviceRec;

enum NvKmsLUTState {
    NvKmsLUTStateUninitialized = 0,
    NvKmsLUTStateIdentity      = 1,
    NvKmsLUTStatePQ            = 2,
};

/* Device-specific EVO state (subdevice- and channel-independent) */
typedef struct _NVEvoDevRec {

    NvU8                gpuLogIndex;
    NvU32               allocRefCnt; /* number of ALLOC_DEVICE calls */
    NVListRec           devListEntry;

    /* array of gpuIds opened with nvkms_open_gpu() */
    NvU32               openedGpuIds[NV0000_CTRL_GPU_MAX_ATTACHED_GPUS];

    NVUnixRmHandleAllocatorRec handleAllocator;

    struct NvKmsDeviceId deviceId;

    /* MIG subscription state for SMG support */
    struct {
        NvU32 gpuInstSubHandle;
        NvU32 computeInstSubHandle;
    } smg;

    NvU32               deviceHandle;
    struct NvKmsPerOpenDev *pNvKmsOpenDev;

    struct {
        NvPushDeviceRec device;
        NvU32 handlePool[NV_PUSH_DEVICE_HANDLE_POOL_NUM];
    } nvPush;

    /* SLI Info */
    struct {
        NvBool          mosaic;
        struct {
            NvBool      present                     :1;

            /* Current hardware state */
            NvBool      powered                     :1;

            /* Software state tracking needs from hardware */
            NvBool      powerNeededForRasterLock    :1;
        } bridge;
    } sli;

    NvU32               numHeads;
    NvU32               numWindows; /* NVDisplay only. */
    /*
     * 'numHwTiles' and 'numHwPhywins' fields are
     * NVDisplay 5.0+ only.
     */
    NvU32               numHwTiles;
    NvU32               numHwPhywins;

    NvU32               displayHandle;


    /*!
     * modesetOwner points to the pOpenDev of the client that called
     * NVKMS_IOCTL_GRAB_OWNERSHIP.
     */
    const struct NvKmsPerOpenDev *modesetOwner;

    /*!
     * Indicates whether modeset ownership is changed since
     * last modeset.
     */
    NvBool modesetOwnerChanged;

    /*!
     * modesetSubOwner points to the pOpenDev of the client that called
     * NVKMS_IOCTL_ACQUIRE_PERMISSIONS with a file descriptor that grants
     * NV_KMS_PERMISSIONS_TYPE_SUB_OWNER.
     */
    const struct NvKmsPerOpenDev *modesetSubOwner;

    /*!
     * NVEvoDevRec::numSubDevices is the number of GPUs in the SLI
     * device.  This is the number of NVEvoSubDevPtrs in
     * NVEvoDevRec::gpus[] and the number of NVSubDeviceEvoPtr in
     * NVEvoDevRec::pSubDevices.
     *
     * The data structure organization is summarized by the following table:
     *
     *                  NVDevEvoRec::numSubDevices (# of pSubDevs)
     *                  | NVDevEvoRec::nDispEvo (# of pDispEvos)
     *                  | | NVDispEvoRec::numSubDevices (# of sd per disp)
     *                  | | |
     * no SLI           1 1 1
     * SLI Mosaic       N N 1
     */
    NvU32               numSubDevices;
    NVSubDeviceEvoPtr   pSubDevices[NVKMS_MAX_SUBDEVICES];

    NvU32               dispClass;
    NvU32               displayCommonHandle;
    NvU32               rmCtrlHandle;

    unsigned int        nDispEvo;
    NVDispEvoPtr        pDispEvo[NVKMS_MAX_SUBDEVICES];

    NVEvoChannelPtr     base[NVKMS_MAX_HEADS_PER_DISP];
    NVEvoChannelPtr     core;
    NVEvoChannelPtr     overlay[NVKMS_MAX_HEADS_PER_DISP];
    NVEvoChannelPtr     window[NVKMS_MAX_WINDOWS_PER_DISP];

    /* NVDisplay head<->window mapping */
    NvU32 headForWindow[NVKMS_MAX_WINDOWS_PER_DISP];

    struct {
        NVEvoChannelPtr layer[NVKMS_MAX_LAYERS_PER_HEAD];
        NvU32 numLayers;
    } head[NVKMS_MAX_HEADS_PER_DISP];

    /* Pointer to array of subdev structs */
    NVEvoSubDevPtr      gpus;

    NvU32               subDevMaskStack[NV_EVO_SUBDEV_STACK_SIZE];
    NvU32               subDevMaskStackDepth;

    NvU32               cursorHandle[NVKMS_MAX_HEADS_PER_DISP];

    NVDPLibTimerPtr     dpTimer;

    NvU8                capsBits[NV5070_CTRL_SYSTEM_CAPS_TBL_SIZE];
    NvU8                commonCapsBits[NV0073_CTRL_SYSTEM_CAPS_TBL_SIZE];

    NVEvoCapsRec        caps;

    NVEvoCoreChannelDmaRec coreChannelDma;
    NvU32               nvkmsGpuVASpace;

    NvBool              mobile                   : 1;

    /*
     * IO coherency modes that display supports for ISO and NISO memory
     * allocations, respectively.
     */
    NvKmsDispIOCoherencyModes isoIOCoherencyModes;
    NvKmsDispIOCoherencyModes nisoIOCoherencyModes;

    /*
     * Indicates whether the init_no_update methods that were pushed by the
     * hardware during core channel allocation are still pending.
     */
    NvBool              coreInitMethodsPending   : 1;
    /*
     * Indicates that NVKMS restored the console and freeing the core channel
     * should leave the display configuration alone.
     *
     * This should be set to FALSE whenever an update is sent that flips away
     * from the framebuffer console.
     *
     * TODO: Remove this in favor of passing a parameter explicitly to the
     * functions that use it.
     */
    NvBool              skipConsoleRestore       : 1;
    /*
     * Indicates that hotplug events that occur while NVKMS is the modeset owner
     * should trigger console restore modesets.
     */
    NvBool              handleConsoleHotplugs    : 1;
    /*
     * Cached from NvKmsSetModeRequest::allowHeadSurfaceInNvKms when the
     * modeset owner does a modeset.  This is needed so that when non-modeset
     * owners do a partial modeset they don't override this value.
     */
    NvBool              allowHeadSurfaceInNvKms  : 1;

    NvBool              gc6Allowed               : 1;

    /*
     * Indicates whether NVKMS is driving an SOC display device, or an external
     * dGPU device.
     */
    NvBool              isSOCDisplay : 1;

    /*
     * Indicates whether NVKMS is supporting syncpts.
     */
    NvBool              supportsSyncpts : 1;

    /*
     * Indicates whether the display device that NVKMS is driving requires all
     * memory allocations that display will access to come from sysmem.
     *
     * For SOC display devices, this should be set to TRUE since the only
     * memory aperture that they support is sysmem.
     */
    NvBool              requiresAllAllocationsInSysmem : 1;
    /*
     * Indicates whether the device that NVKMS is driving supports headSurface
     * composition.
     *
     * For SOC display devices (e.g., Orin), this should be set to FALSE since
     * there's currently zero nvgpu support, and no Tegra clients should be
     * using headSurface right now.
     */
    NvBool              isHeadSurfaceSupported : 1;

    /*
     * vblank Sem Control requires support in resman; that support is not
     * currently available on Tegra.
     */
    NvBool              supportsVblankSemControl : 1;

    nvkms_timer_handle_t *postFlipIMPTimer;
    nvkms_timer_handle_t *consoleRestoreTimer;

    nvkms_timer_handle_t *lowerDispBandwidthTimer;

    NvU32               simulationType;

    NvU32               numClasses;
    NvU32              *supportedClasses;

    struct {
        /* name[0] == '\0' for unused registryKeys[] array elements. */
        char name[NVKMS_MAX_DEVICE_REGISTRY_KEYNAME_LEN];
        NvU32 value;
    } registryKeys[NVKMS_MAX_DEVICE_REGISTRY_KEYS];

    /* Returns true if the Quadro Sync card connected to this GPU has
     * a firmware version incompatible with this GPU.
     */
    NvBool badFramelockFirmware;

    const struct _nv_evo_hal *hal;
    const struct _nv_evo_cursor_hal *cursorHal;

    /*!
     * ref_ptr to the structure.
     *
     * nvkms_timer_handle_t objects refer to the pDevEvo via references to this,
     * so that timers that fire after the pDevEvo has been freed can detect that
     * case and do nothing.
     */
    struct nvkms_ref_ptr *ref_ptr;

    struct {
        void *handle;
    } hdmiLib;

    struct {
        NvU32 semaphoreHandle;
        void *pSemaphores;
        NvBool enabled;
        NvU32 flipCounter;
    } vrr;

    /*
     * Information about the framebuffer console returned by
     * NV0080_CTRL_CMD_OS_UNIX_VT_GET_FB_INFO.
     */
    NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS vtFbInfo;

    /*
     * Handle referencing the memory reserved by RM that is used by the kernel
     * as the framebuffer console surface.
     */
    NvKmsSurfaceHandle fbConsoleSurfaceHandle;

    NVHsDeviceEvoPtr pHsDevice;

    /* The current headSurface configuration. */
    NVHsStateOneHeadAllDisps apiHeadSurfaceAllDisps[NVKMS_MAX_HEADS_PER_DISP];

    struct NVDevPreallocRec prealloc;

    struct {
        NvU32 handle;
        NVOS10_EVENT_KERNEL_CALLBACK_EX callback;
    } nonStallInterrupt;

    /*
     * Track the LUT with per-head, per-pDisp scope.  The LUT itself
     * is triple buffered.
     *
     * RM surface allocations are broadcast in SLI, so LUT is allocated with
     * per-device scope.  However, writes into the LUT are unicast with
     * per-pDisp scope.
     *
     * The LUT surface in the core channel contains both the base and output
     * LUTs.
     */
    struct {
        struct {
            NVSurfaceEvoPtr      LUT[3];
            struct {
                NvBool           waitForPreviousUpdate;
                NvBool           curBaseLutEnabled;
                NvBool           curOutputLutEnabled;
                NvU8             curLUTIndex;
                nvkms_timer_handle_t *updateTimer;
            } disp[NVKMS_MAX_SUBDEVICES];
        } apiHead[NVKMS_MAX_HEADS_PER_DISP];
        NVSurfaceEvoPtr    defaultLut;
        enum NvKmsLUTState defaultBaseLUTState[NVKMS_MAX_SUBDEVICES];
        enum NvKmsLUTState defaultOutputLUTState[NVKMS_MAX_SUBDEVICES];

        /*
         * Track outstanding LUT notifiers. Each notifier can have any number
         * of apiHeads waiting on it.
         *
         * waitingApiHeadMask tracks all apiHeads waiting on a LUT notifier.
         * stagedApiHeadMask tracks a set of apiHeads that will be tracked by
         * the next LUT notifier kickoff.
         *
         * notifiers is an array of trackers for specific core
         * notifiers. If notifiers[i].waiting is true, then the apiHeads listed
         * in notifiers[i].apiHeadMask are waiting on notifiers[i].notifier.
         */
        struct {
            struct {
                NvU32 waitingApiHeadMask;
                NvU32 stagedApiHeadMask;
                struct {
                    int notifier;
                    NvU32 apiHeadMask;
                    NvBool waiting;
                } notifiers[NVKMS_MAX_HEADS_PER_DISP];
            } sd[NVKMS_MAX_SUBDEVICES];
        } notifierState;
    } lut;

    /*! stores pre-syncpts */
    NVEvoSyncpt *preSyncptTable;
    NvBool *pAllSyncptUsedInCurrentFlip;

    /* DIFR prefetch event handling. */
    NVOS10_EVENT_KERNEL_CALLBACK_EX difrPrefetchCallback;
    NvU32 difrPrefetchEventHandler;

    /* DIFR runtime state. */
    NVDIFRStateEvoPtr pDifrState;

    NvU32 numApiHeads;

    struct {
        NvU32 numLayers;
    } apiHead[NVKMS_MAX_HEADS_PER_DISP];
} NVDevEvoRec;

/*
 * The NVHwModeTimingsEvo structure stores all the values necessary to
 * perform a modeset with EVO
 */

typedef struct _NVHwModeViewPortEvo {
    struct {
        /*
         * note that EVO centers ViewPortOut within the active raster,
         * so xAdjust,yAdjust are signed; to position ViewPortOut at
         * 0,0 within active raster:
         *
         * viewPortOut.xAdjust = (activeRaster.w - viewPortOut.w)/2 * -1;
         * viewPortOut.yAdjust = (activeRaster.h - viewPortOut.h)/2 * -1;
         */
        NvS16 xAdjust;
        NvS16 yAdjust;
        NvU16 width;
        NvU16 height;
    } out;

    struct {
        NvU16 width;
        NvU16 height;
    } in;

    NVEvoScalerTaps hTaps;
    NVEvoScalerTaps vTaps;

    // These are the window features that may be possible if the required ISO
    // bw is available at the time that the feature needs to be enabled. By
    // default possibleUsage is set considering that everything is supported
    // by the HW and for dGPU, IMP will scale it as needed.
    struct NvKmsUsageBounds possibleUsage;

    // Guaranteed usage bounds allowed by IMP.  These are never assigned to
    // NVDpyEvoRec::usage or the hardware directly, but rather are used to
    // validate usage bound change requests.
    struct NvKmsUsageBounds guaranteedUsage;
} NVHwModeViewPortEvo;

static inline NvBool nvIsImageSharpeningAvailable(
        const NVHwModeViewPortEvo *pViewPort)
{
    return (pViewPort->out.width != pViewPort->in.width) ||
           (pViewPort->out.height != pViewPort->in.height);
}

enum nvKmsPixelDepth {
    NVKMS_PIXEL_DEPTH_18_444,
    NVKMS_PIXEL_DEPTH_24_444,
    NVKMS_PIXEL_DEPTH_30_444,
    NVKMS_PIXEL_DEPTH_20_422,
    NVKMS_PIXEL_DEPTH_16_422,
};

enum nvKmsTimingsProtocol {
    NVKMS_PROTOCOL_DAC_RGB,

    NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A,
    NVKMS_PROTOCOL_SOR_SINGLE_TMDS_B,
    NVKMS_PROTOCOL_SOR_DUAL_TMDS,
    NVKMS_PROTOCOL_SOR_DP_A,
    NVKMS_PROTOCOL_SOR_DP_B,
    NVKMS_PROTOCOL_SOR_LVDS_CUSTOM,
    NVKMS_PROTOCOL_SOR_HDMI_FRL,

    NVKMS_PROTOCOL_DSI,

    NVKMS_PROTOCOL_PIOR_EXT_TMDS_ENC,
};

enum NVDscInfoEvoType {
    NV_DSC_INFO_EVO_TYPE_DISABLED = 0,
    NV_DSC_INFO_EVO_TYPE_HDMI = 1,
    NV_DSC_INFO_EVO_TYPE_DP = 3,
};

enum NVDscEvoMode {
    NV_DSC_EVO_MODE_SINGLE = 0,
    NV_DSC_EVO_MODE_DUAL = 1,
};

typedef struct _NVDscInfoEvoRec {
    union {
        /* DisplayPort Display Stream Compression */
        struct {
            /*
             * The DSC target bits per pixel (bpp) rate value multiplied by 16 that
             * is being used by the DSC encoder.
             *
             * It maps respectively to {pps4[1:0], pps5[7:0]}.
             */
            NvU32 bitsPerPixelX16;

            /*
             * The DSC picture parameter set (PPS), which the DSC encoder must
             * communicate to the decoder.
             */
            NvU32 pps[DSC_MAX_PPS_SIZE_DWORD];
            enum NVDscEvoMode dscMode;
        } dp;

        struct {
            NvU32 bitsPerPixelX16;
            NvU32 pps[HDMI_DSC_MAX_PPS_SIZE_DWORD];
            NvU32 dscHActiveBytes;
            NvU32 dscHActiveTriBytes;
            NvU32 dscHBlankTriBytes;
            NvU32 dscTBlankToTTotalRatioX1k;
            NvU32 hblankMin;
            enum NVDscEvoMode dscMode;
        } hdmi;
    };

    NvU32 sliceCount;
    NvU32 possibleSliceCountMask;

    enum NVDscInfoEvoType type;
} NVDscInfoEvoRec;

/*
 * This structure defines all of the values necessary to program mode timings
 * on EVO hardware.
 * NOTE: if you add anything to this, consider adding it to
 * RasterLockPossible() in nvkms-evo.c
 */
typedef struct _NVHwModeTimingsEvo {
    struct NvKmsPoint rasterSize;
    struct NvKmsPoint rasterSyncEnd;
    struct NvKmsPoint rasterBlankEnd;
    struct NvKmsPoint rasterBlankStart;
    NvU32 rasterVertBlank2Start;
    NvU32 rasterVertBlank2End;

    NvU32 pixelClock; /* in kHz */
    enum nvKmsTimingsProtocol protocol;
    /*
     * yuv420Mode reflects whether this mode requires YUV 4:2:0 decimation into
     * a half-width output through headsurface (SW YUV420) or >=nvdisplay 4.0 HW
     * CSC (HW YUV420).
     *
     * If a mode requires SW YUV 4:2:0 emulation, the pixelClock and width
     * values in NvModeTimings will still be the full width values specified by
     * the mode parsed from the EDID (e.g. 3840x2160@60), but the pixelClock
     * and width values in NVHwModeTimingsEvo will be the "real" half width
     * values programmed in HW and rendered to through a headSurface transform
     * (e.g. 1920x2160@60).  If a mode requires HW YUV 4:2:0 CSC, the
     * pixelClock and width values in both NvModeTimings and NVHwModeTimingsEvo
     * will be full width, and the decimation to the half width scanout surface
     * is performed in HW.  In both cases, only the full width values should
     * ever be reported to the client.
     */
    enum NvYuv420Mode yuv420Mode;
    /* *SyncPol is TRUE if negative */
    NvBool hSyncPol   : 1;
    NvBool vSyncPol   : 1;
    NvBool interlaced : 1;
    NvBool doubleScan : 1;
    /*
     * hdmi3D reflects whether this mode is a HDMI 3D frame packed mode. True
     * only if the user selected HDMI 3D stereo mode and the GPU supports it.
     * If true, then pixelClock is doubled.
     */
    NvBool hdmi3D     : 1;

    struct {
        /* The vrr type for which this mode is adjusted. */
        enum NvKmsDpyVRRType type;
        NvU32 timeoutMicroseconds;
    } vrr;

    NVHwModeViewPortEvo viewPort;
} NVHwModeTimingsEvo;

static inline NvBool nvIsAdaptiveSyncDpyVrrType(enum NvKmsDpyVRRType type)
{
    return ((type == NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_DEFAULTLISTED) ||
            (type == NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_NON_DEFAULTLISTED));
}

static inline NvBool nvIsGsyncDpyVrrType(enum NvKmsDpyVRRType type)
{
    return (type == NVKMS_DPY_VRR_TYPE_GSYNC);
}

static inline NvU64 nvEvoFrametimeUsFromTimings(const NVHwModeTimingsEvo *pTimings)
{
    NvU64 pixelsPerFrame = pTimings->rasterSize.x * pTimings->rasterSize.y;
    NvU64 pixelsPerSecond = KHzToHz(pTimings->pixelClock);
    NvU64 framesPerSecond = pixelsPerSecond / pixelsPerFrame;

    return 1000000ULL / framesPerSecond;
}

static inline NvU16 nvEvoVisibleWidth(const NVHwModeTimingsEvo *pTimings)
{
    return pTimings->rasterBlankStart.x - pTimings->rasterBlankEnd.x;
}

static inline NvU16 nvEvoVisibleHeight(const NVHwModeTimingsEvo *pTimings)
{
    /* rasterVertBlank2{Start,End} should only be != 0 for interlaced modes. */
    nvAssert(pTimings->interlaced ||
             ((pTimings->rasterVertBlank2Start == 0) &&
              (pTimings->rasterVertBlank2End == 0)));

    return pTimings->rasterBlankStart.y - pTimings->rasterBlankEnd.y +
           pTimings->rasterVertBlank2Start - pTimings->rasterVertBlank2End;
}

/*
 * Calculate BackendSizeHeight, based on this HD or SD quality is
 * defined.
 */
static inline NvBool nvEvoIsHDQualityVideoTimings(
    const NVHwModeTimingsEvo *pTimings)
{
    NvU32 height = nvEvoVisibleHeight(pTimings);

    // as per windows code, nvva uses < 720.
    if (height <= 576) {
        // SD quality: 240, 288, 480, 576
        return FALSE;
    }

    // HD quality: 720, 1080
    return TRUE;
}

static inline struct NvKmsRect nvEvoViewPortOutHwView(
    const NVHwModeTimingsEvo *pTimings)
{
    const NVHwModeViewPortEvo *pViewPort = &pTimings->viewPort;
    const NvU16 hVisible = nvEvoVisibleWidth(pTimings);
    const NvU16 vVisible = nvEvoVisibleHeight(pTimings);
    struct NvKmsRect viewPortOut = { 0 };

    viewPortOut.width = pViewPort->out.width;
    viewPortOut.height = pViewPort->out.height;
    viewPortOut.x = pViewPort->out.xAdjust +
                   (hVisible - pViewPort->out.width) / 2;
    viewPortOut.y = (pViewPort->out.yAdjust +
                   (vVisible - pViewPort->out.height) / 2);

    return viewPortOut;
}

static inline struct NvKmsRect nvEvoViewPortOutClientView(
    const NVHwModeTimingsEvo *pTimings)
{
    struct NvKmsRect viewPortOut = nvEvoViewPortOutHwView(pTimings);

    if (pTimings->doubleScan) {

        nvAssert((viewPortOut.x % 2) == 0);
        viewPortOut.x /= 2;

        nvAssert((viewPortOut.height % 2) == 0);
        viewPortOut.height /= 2;
    }

    return viewPortOut;
}

/*
 * The ELD contains a subset of the digital display device's EDID
 * information related to audio capabilities. The GPU driver sends the
 * ELD to hardware and the audio driver reads it by issuing the ELD
 * command verb.
 */

#define NV_MAX_AUDIO_DEVICE_ENTRIES \
    (NV0073_CTRL_DFP_ELD_AUDIO_CAPS_DEVICE_ENTRY_3 + 1)

typedef enum {
    NV_ELD_PRE_MODESET = 0,
    NV_ELD_POST_MODESET,
    NV_ELD_POWER_ON_RESET,
} NvEldCase;

/* OR indices are per OR-type.  The maximum OR index for each type
 * on each GPU is:
 *
 * Pre-GV10X : 8 SORs, 4 PIORs and 4 Dacs;
 * GV10X     : 8 SORs, 4 PIORs;
 * TU10X+    : 8 SORs;
 */
#define NV_EVO_MAX_ORS 8

/*
 * The scoping of heads, ORs, and dpys relative to connectors can be
 * complicated. Here is how objects are scoped for various configurations:
 *
 *                  #heads  #ORs    #dpys   #NVConnectorEvoRecs
 *  DP 1.1          1       1       1       1
 *  DP-MST          n       1       n       1
 *  DP cloning:     1       1       n       1
 *  2-Heads-1-OR:   2       2       1       1
 */
typedef struct _NVConnectorEvoRec {
    char name[NVKMS_DPY_NAME_SIZE];

    NVDispEvoPtr pDispEvo;

    NVListRec connectorListEntry;

    NvBool detectComplete; /* For sync'ing dpy detection w/ DP lib */
    NVDPLibConnectorPtr pDpLibConnector; // DP Lib
    NvBool dpSerializerEnabled;

    struct {
        NvU8 maxLinkBW;
        NvU8 maxLaneCount;
        NvBool supportsMST;
    } dpSerializerCaps;

    NVDpyId displayId; // RM Display ID
    NvKmsConnectorSignalFormat signalFormat;
    NvKmsConnectorType type;
    NvU32 typeIndex;
    NvU32 legacyType; /* NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_ */
    NvU32 legacyTypeIndex;
    NvU32 physicalIndex;
    NvU32 physicalLocation;

    NvU32 dfpInfo; /* DFP info query through NV0073_CTRL_CMD_DFP_GET_INFO */

    NVDpyIdList ddcPartnerDpyIdsList;

    struct {
        NvU32 type;
        NvU32 protocol; // NV0073_CTRL_SPECIFIC_OR_PROTOCOL_*
        NvU32 location; // NV0073_CTRL_SPECIFIC_OR_LOCATION_*
        NvU32 ditherType;
        NvU32 ditherAlgo;
        /* Hardware heads attached to assigned OR */
        NvU32 ownerHeadMask[NV_EVO_MAX_ORS];
        /* The mask of secondary ORs assigned to this connector */
        NvU32 secondaryMask;
        /* The primary OR assigned to this connector */
        NvU32 primary;
    } or;

    NvEldCase audioDevEldCase[NV_MAX_AUDIO_DEVICE_ENTRIES];

    NvBool isHdmiEnabled;
} NVConnectorEvoRec;

static inline NvU32 nvConnectorGetORMaskEvo(const NVConnectorEvoRec *pConnectorEvo)
{
    if (pConnectorEvo->or.primary != NV_INVALID_OR) {
        return NVBIT(pConnectorEvo->or.primary) | pConnectorEvo->or.secondaryMask;
    }
    return 0x0;
}

static inline NvU32 nvConnectorGetAttachedHeadMaskEvo(
    const NVConnectorEvoRec *pConnectorEvo)
{
    NvU32 headMask = 0x0;
    NvU32 orIndex;

    FOR_EACH_INDEX_IN_MASK(32, orIndex, nvConnectorGetORMaskEvo(pConnectorEvo)) {
        headMask |= pConnectorEvo->or.ownerHeadMask[orIndex];
    } FOR_EACH_INDEX_IN_MASK_END;

    return headMask;
}

static inline
NvBool nvIsConnectorActiveEvo(const NVConnectorEvoRec *pConnectorEvo)
{
    return (pConnectorEvo->or.primary != NV_INVALID_OR) &&
           (pConnectorEvo->or.ownerHeadMask[pConnectorEvo->or.primary] != 0x0);
}

enum NVDpLibIsModePossibleQueryMode {
    NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_NONE,
    NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_PRE_IMP =
            NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_NONE,
    NV_DP_LIB_IS_MODE_POSSIBLE_QUERY_MODE_POST_IMP,
};

typedef struct _NVDpLibIsModePossibleParamsRec {
    enum NVDpLibIsModePossibleQueryMode queryMode;

    struct {
        NvU32 displayId;
        NVDpyIdList dpyIdList;
        enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace;
        enum NvKmsDpyAttributeColorBpcValue colorBpc;
        const struct NvKmsModeValidationParams *pModeValidationParams;
        const NVHwModeTimingsEvo *pTimings;
        NvBool b2Heads1Or;
        NVDscInfoEvoRec *pDscInfo;
    } head[NV_MAX_HEADS];
} NVDpLibIsModePossibleParamsRec;

typedef struct _NVDpyAttributeCurrentDitheringConfigRec {
    NvBool enabled;
    enum NvKmsDpyAttributeCurrentDitheringDepthValue depth;
    enum NvKmsDpyAttributeCurrentDitheringModeValue mode;
} NVDpyAttributeCurrentDitheringConfig;

typedef struct __NVDpyAttributeColorRec {
    /*
     * For both colorSpace and colorRange, the value for
     * NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_{SPACE,RANGE} sent by the client is
     * stored in NVDpyEvoRec::requestedColor(Space, Range}. The structure stores
     * the actual color space and color range in use.
     *
     * nvChooseCurrentColorSpaceAndRangeEvo() chooses the actual color
     * space, color bpc, and color range, for a dpy.  It sets colorBpc
     * to the max bpc supported by the given dpy.
     *
     * Since YUV444 mode only allows limited color range, changes to the
     * current color space may trigger changes to the current color
     * range (see nvChooseCurrentColorSpaceAndRangeEvo()).
     *
     * For SW YUV420 mode, these values are ignored in
     * HEAD_SET_PROCAMP and applied in the headSurface composite shader.
     *
     * XXX Rename NvKmsDpyAttributeCurrentColorSpaceValue to
     * NvKmsDpyAttributeCurrentFormatValue.
     */
    enum NvKmsDpyAttributeCurrentColorSpaceValue format;
    enum NvKmsDpyAttributeColorBpcValue bpc;
    enum NvKmsDpyAttributeColorRangeValue range;

    enum NvKmsOutputColorimetry colorimetry;
} NVDpyAttributeColor;

typedef struct __NVAttributesSetEvoRec {

#define NV_EVO_DVC_MIN (-1024)
#define NV_EVO_DVC_MAX 1023
#define NV_EVO_DVC_DEFAULT 0

    NvS32 dvc;

    NVDpyAttributeColor color;

    NVDpyAttributeCurrentDitheringConfig dithering;

#define NV_EVO_IMAGE_SHARPENING_MIN 0
#define NV_EVO_IMAGE_SHARPENING_MAX 255
#define NV_EVO_IMAGE_SHARPENING_DEFAULT 127

    struct {
        NvBool available;
        NvU32 value;
    } imageSharpening;

    enum NvKmsDpyAttributeDigitalSignalValue digitalSignal;

    NvU8 numberOfHardwareHeadsUsed;
} NVAttributesSetEvoRec;

#define NV_EVO_DEFAULT_ATTRIBUTES_SET                                     \
    (NVAttributesSetEvoRec) {                                             \
        .dvc =        NV_EVO_DVC_DEFAULT,                                 \
        .color = {                                                        \
            .format = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB,       \
            .range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL,               \
        },                                                                \
        .dithering = {                                                    \
            .enabled = FALSE,                                             \
            .mode    = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_NONE,  \
            .depth   = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_NONE, \
        },                                                                \
        .imageSharpening = {                                              \
            .value = NV_EVO_IMAGE_SHARPENING_DEFAULT,                     \
        },                                                                \
        .numberOfHardwareHeadsUsed = 0,                                   \
    }

typedef struct _NVEldEvoRec {
    NvU32 size;
    NvU8  buffer[NV0073_CTRL_DFP_ELD_AUDIO_CAPS_ELD_BUFFER];
} NVEldEvoRec;

/*
 * This structure stores information about the active per-head audio state.
 */
typedef struct _NVDispHeadAudioStateEvoRec {
    NvU32 maxFreqSupported;
    NVEldEvoRec eld;

    NvBool isAudioOverHdmi : 1;
    NvBool supported       : 1;
    NvBool enabled         : 1;
} NVDispHeadAudioStateEvoRec;

typedef struct _NVDispHeadInfoFrameStateEvoRec {
    NVT_VIDEO_INFOFRAME_CTRL ctrl;
    NvBool hdTimings;
} NVDispHeadInfoFrameStateEvoRec;

typedef enum _NVEvoMergeMode {
    NV_EVO_MERGE_MODE_DISABLED,
    NV_EVO_MERGE_MODE_SETUP,
    NV_EVO_MERGE_MODE_PRIMARY,
    NV_EVO_MERGE_MODE_SECONDARY,
} NVEvoMergeMode;

/*
 * In GB20X+ architecture, the postcomp pipe is now referred to as a "Tile".
 * The term "Head" now refers to just the RG (Raster Generator)  and SF (Serial
 * Frontend). Each Tile can be independently assigned to a Head and multiple
 * Tiles can feed into a single Head. Each Tile will generate one vertical
 * slice of the output viewport
 *
 * When multiple tiles are used, multiple precomp pipelines are used to render
 * a single window in the desktop space. The precomp pipe is now referred to
 * as a "PhyWin" (Physical Window).
 *
 * When a Window channel is attached to a head, the channel must own a number
 * of physical windows equal to the number of tiles owned by that head.
 */
typedef struct _NVHwHeadMultiTileConfigRec {
    /* Mask of tiles feeding into the head */
    NvU32 tilesMask;
    /* Per layer mask of phywins used to render a layer. */
    NvU32 phywinsMask[NVKMS_MAX_LAYERS_PER_HEAD];
} NVHwHeadMultiTileConfigRec;

/*
 * This structure stores information about the active per-head display state.
 */
typedef struct _NVDispHeadStateEvoRec {

    NvU32 displayRate;

    /*! Cached, to preserve across modesets. */
    struct NvKmsModeValidationParams modeValidationParams;

    /*
     * For Turing and newer, enable display composition bypass mode.
     *
     * This is intended to be used by console restore to avoid bug 2168873.
     */
    NvBool bypassComposition        : 1;

    struct {
        NVT_COLOR_FORMAT colorFormat;
        NVT_COLORIMETRY colorimetry;
        NVT_COLOR_RANGE colorRange;
        NvU32 satCos;
    } procAmp;

    /*
     * The activeRmId is the identifier that we use to talk to RM
     * about the display device(s) on this head.  It is zero except
     * when a mode is being driven by this head.  For DP MST, it is the
     * identifier of the displayport library group to which the driven
     * DP device belongs.  Otherwise, it is the identifier of the connector
     * driven by the head.
     */
    NvU32 activeRmId;

    NVHwModeTimingsEvo timings;
    NVConnectorEvoRec *pConnectorEvo; /* NULL if the head is not active */

    HDMI_FRL_CONFIG hdmiFrlConfig;

    NVDscInfoEvoRec dscInfo;

    enum nvKmsPixelDepth pixelDepth;

    NVDispHeadAudioStateEvoRec audio;

    enum NvKmsOutputTf tf;

    struct {
        NvBool enabled;
        enum NvKmsInfoFrameEOTF eotf;
        struct NvKmsHDRStaticMetadata staticMetadata;
    } hdrInfoFrameOverride;

    struct {
        enum NvKmsHDRInfoFrameState state;
        enum NvKmsInfoFrameEOTF eotf;
        struct NvKmsHDRStaticMetadata staticMetadata;
    } hdrInfoFrame;

    struct {
        NVSurfaceEvoPtr pCurrSurface;
        NvBool outputLutEnabled : 1;
        NvBool baseLutEnabled   : 1;
    } lut;

    /*
     * The api head can be mapped onto the N harware heads, a frame presented
     * by the api head gets split horizontally into N sections,
     * 'mergeHeadSection' describe the section presented by this hardware
     * head.
     */
    NvU8 mergeHeadSection;

    NVEvoMergeMode mergeMode;
    NVHwHeadMultiTileConfigRec multiTileConfig;
} NVDispHeadStateEvoRec;

typedef struct _NVDispStereoParamsEvoRec {
    enum NvKmsStereoMode mode;
    NvBool isAegis;
} NVDispStereoParamsEvoRec;

typedef struct _NVDispFlipOccurredEventDataEvoRec {
    NVDispEvoPtr pDispEvo;
    NvU32 apiHead;
    NvU32 layer;
} NVDispFlipOccurredEventDataEvoRec;

typedef struct _NVDispApiHeadStateEvoRec {
    /*
     * The mask of hardware heads mapped onto this api head,
     * set to zero if the api head is not active.
     */
    NvU32 hwHeadsMask;

    NVDpyIdList activeDpys; /* Empty if the head is not active */
    NVAttributesSetEvoRec attributes;

    enum NvKmsOutputTf tf;

    NvBool hdrInfoFrameOverride;
    NvU32 hdrStaticMetadataLayerMask;

    /*
     * Hardware timings which are split across hardware heads.
     *
     * XXX[2Heads1OR] The api-head state does not require to track full
     * hardware timings. Replace 'timings' by minimal per api-head hardware
     * timings information used in code.
     */
    NVHwModeTimingsEvo timings;

    struct {
        NvBool active;
        NvBool pendingCursorMotion;
    } vrr;
    
    NVDispStereoParamsEvoRec stereo;

    struct NvKmsPoint viewPortPointIn;

    NVDispHeadInfoFrameStateEvoRec infoFrame;

    /*
     * Each head can have up to NVKMS_MAX_VBLANK_SYNC_OBJECTS_PER_HEAD
     * programmable Core semaphores.
     *
     * The numVblankSyncObjectsCreated will ideally always be equal to
     * NVKMS_MAX_VBLANK_SYNC_OBJECTS_PER_HEAD, but could be lower if errors
     * occured during syncpt allocation in nvRMSetupEvoCoreChannel().
     */
    NvU8 numVblankSyncObjectsCreated;
    NVVblankSyncObjectRec vblankSyncObjects[NVKMS_MAX_VBLANK_SYNC_OBJECTS_PER_HEAD];

    struct {
        struct nvkms_ref_ptr *ref_ptr;
        NVDispFlipOccurredEventDataEvoRec data;
    } flipOccurredEvent[NVKMS_MAX_LAYERS_PER_HEAD];

    NvU32 rmVBlankCallbackHandle;

    NvBool hs10bpcHint : 1;
} NVDispApiHeadStateEvoRec;

typedef struct _NVDispVblankApiHeadState {

    NvU64 vblankCount;

    /*
     * All entries in vblankCallbackList[0] get called before any entries in
     * vblankCallbackList[1].
     */
    NVListRec vblankCallbackList[2];

    struct {
        NVListRec list;
        NVVBlankCallbackPtr pCallbackPtr;
    } vblankSemControl;

} NVDispVblankApiHeadState;

typedef struct _NVDispEvoRec {
    NvU8       gpuLogIndex;
    NVDevEvoPtr pDevEvo;
    NvU32      hotplugEventHandle;
    NvU32      DPIRQEventHandle;
    NVOS10_EVENT_KERNEL_CALLBACK_EX rmHotplugCallback;
    NVOS10_EVENT_KERNEL_CALLBACK_EX rmDPIRQCallback;

    NVDispHeadStateEvoRec headState[NVKMS_MAX_HEADS_PER_DISP];
    NVDispApiHeadStateEvoRec apiHeadState[NVKMS_MAX_HEADS_PER_DISP];
    NVDispVblankApiHeadState vblankApiHeadState[NVKMS_MAX_HEADS_PER_DISP];

    NVDpyIdList vbiosDpyConfig[NVKMS_MAX_HEADS_PER_DISP];

    NvU32             isoBandwidthKBPS;
    NvU32             dramFloorKBPS;

    /*
     * The list of physical connector display IDs.  This is the union
     * of pConnectorEvo->displayId values, which is also the union of
     * pDpyEvo->id values for non-MST pDpys.
     */
    NVDpyIdList       connectorIds;

    NVListRec         connectorList;

    NvU32             displayOwner;

    NVListRec         dpyList;

    NVDpyIdList       bootDisplays;
    NVDpyIdList       validDisplays;
    NVDpyIdList       connectedDisplays;

    /*
     * displayPortMSTIds is a superset of dynamicDpyIds because not all DP MST
     * dpys are dynamic dpys. For example, the DP MST dpys that are driven by
     * a DP serializer connector are part of a fixed topology, and are static in
     * nature.
     */
    NVDpyIdList       displayPortMSTIds; /* DP MST dpys */
    NVDpyIdList       dynamicDpyIds;

    NVDpyIdList       muxDisplays;

    struct {
        // Indicates whether a VRR cookie was detected
        NvBool hasPlatformCookie;

        nvkms_timer_handle_t *unstallTimer;
    } vrr;

    NVFrameLockEvoPtr pFrameLockEvo;
    struct {
        NVDpyId       server;
        NVDpyIdList   clients;
        NvBool        syncEnabled;   /* GPU is syncing to framelock */
        NvU32         connectorIndex;/* NV30F1_GSYNC_CONNECTOR_* */
        NvU32         currentServerHead; /* used for disabling */
        NvU32         currentClientHeadsMask; /* used for disabling */
        NvBool        currentHouseSync; /* if state machine thinks house sync
                                           is enabled -- used for disabling */

        /* Framelock event-related data */
#define NV_FRAMELOCK_SYNC_LOSS 0
#define NV_FRAMELOCK_SYNC_GAIN 1
#define NV_FRAMELOCK_NUM_EVENTS 2

        struct {
            NvU32     handle;
            NVOS10_EVENT_KERNEL_CALLBACK_EX callback;
        } gsyncEvent[NV_FRAMELOCK_NUM_EVENTS];

    } framelock;

    /* NVDevEvoRec::pHsChannel[] is indexed by the api heads */
    NVHsChannelEvoPtr pHsChannel[NVKMS_MAX_HEADS_PER_DISP];

    /* NVDevEvoRec::pSwapGroup[] is indexed by the api heads */
    NVSwapGroupPtr pSwapGroup[NVKMS_MAX_HEADS_PER_DISP];

    /* If cross-GPU rasterlock is possible with the currently-active
     * configuration */
    NvBool rasterLockPossible;

    /*
     * This points to an *active* lock group (i.e., a set of 1 or more pDisps
     * across which rasterlock -- and possibly fliplock -- is currently
     * enabled), or NULL if no lock group is active on this pDisp.
     */
    NVLockGroup *pLockGroup;

    /*!
     * ref_ptr to the structure.
     *
     * nvkms_timer_handle_t objects refer to the pDispEvo via references to
     * this, so that timers that fire after the pDispEvo has been freed can
     * detect that case and do nothing.
     */
    struct nvkms_ref_ptr *ref_ptr;

    /*
     * Indicates that NV_KMS_DISP_ATTRIBUTE_QUERY_DP_AUX_LOG has been queried at
     * least once on this device. If set, nvRmDestroyDisplays() will flush any
     * remaining AUX log messages to the system log.
     */
    NvBool dpAuxLoggingEnabled;

    struct nvkms_backlight_device *backlightDevice;
} NVDispEvoRec;

static inline NvU32 GetNextHwHead(NvU32 hwHeadsMask, const NvU32 prevHwHead)
{
    NvU32 head;

    if ((hwHeadsMask == 0x0) ||
            ((prevHwHead != NV_INVALID_HEAD) &&
             ((hwHeadsMask &= ~((1 << (prevHwHead + 1)) -1 )) == 0x0))) {
        return NV_INVALID_HEAD;
    }

    head = BIT_IDX_32(LOWESTBIT(hwHeadsMask));

    if (head >= NV_MAX_HEADS) {
        return NV_INVALID_HEAD;
    }

    return head;
}

#define FOR_EACH_EVO_HW_HEAD_IN_MASK(__hwHeadsMask, __hwHead)           \
    for ((__hwHead)  = GetNextHwHead((__hwHeadsMask), NV_INVALID_HEAD); \
         (__hwHead) != NV_INVALID_HEAD;                                 \
         (__hwHead)  = GetNextHwHead((__hwHeadsMask), (__hwHead)))

#define FOR_EACH_EVO_HW_HEAD(__pDispEvo, __apiHead, __hwHead)                          \
    FOR_EACH_EVO_HW_HEAD_IN_MASK((__pDispEvo)->apiHeadState[(__apiHead)].hwHeadsMask,  \
                                 (__hwHead))

static inline NvU32 nvGetPrimaryHwHeadFromMask(const NvU32 hwHeadsMask)
{
    return GetNextHwHead(hwHeadsMask, NV_INVALID_HEAD);
}

static inline NvU32 nvGetPrimaryHwHead(const NVDispEvoRec *pDispEvo,
                                       const NvU32 apiHead)
{
    return (apiHead != NV_INVALID_HEAD) ?
        nvGetPrimaryHwHeadFromMask(pDispEvo->apiHeadState[apiHead].hwHeadsMask) :
            NV_INVALID_HEAD;
}

typedef enum {
    NV_EVO_PASSIVE_DP_DONGLE_UNUSED,
    NV_EVO_PASSIVE_DP_DONGLE_DP2DVI,
    NV_EVO_PASSIVE_DP_DONGLE_DP2HDMI_TYPE_1,
    NV_EVO_PASSIVE_DP_DONGLE_DP2HDMI_TYPE_2,
} NVEvoPassiveDpDongleType;

typedef struct NVEdidRec {
    NvU8 *buffer;
    size_t length;
} NVEdidRec, *NVEdidPtr;

typedef struct _NVParsedEdidEvoRec {
    NvBool               valid;
    NVT_EDID_INFO        info;
    NVT_EDID_RANGE_LIMIT limits;
    char                 monitorName[NVT_EDID_MONITOR_NAME_STRING_LENGTH];
    char                 serialNumberString[NVT_EDID_LDD_PAYLOAD_SIZE+1];
} NVParsedEdidEvoRec;

typedef void (*NVVBlankCallbackProc)(NVDispEvoRec *pDispEvo,
                                     NVVBlankCallbackPtr pCallbackData);

typedef struct _NVVBlankCallbackRec {
    NVListRec vblankCallbackListEntry;
    NVVBlankCallbackProc pCallback;
    void *pUserData;
    NvU32 apiHead;
} NVVBlankCallbackRec;

typedef void (*NVRgLine1CallbackProc)(NVDispEvoRec *pDispEvo,
                                      const NvU32 head,
                                      NVRgLine1CallbackPtr pCallbackData);

typedef struct _NVRgLine1CallbackRec {
    NVRgLine1CallbackProc pCallbackProc;
    void *pUserData;
} NVRgLine1CallbackRec;

typedef struct _NVDpyAttributeRequestedDitheringConfigRec {
    enum NvKmsDpyAttributeRequestedDitheringValue state;
    enum NvKmsDpyAttributeRequestedDitheringDepthValue depth;
    enum NvKmsDpyAttributeRequestedDitheringModeValue mode;
} NVDpyAttributeRequestedDitheringConfig;

typedef struct _NVDpyEvoRec {
    NVListRec dpyListEntry;
    NVDpyId  id;

    char name[NVKMS_DPY_NAME_SIZE];

    NvU32 apiHead;

    struct _NVDispEvoRec *pDispEvo;
    NVConnectorEvoPtr pConnectorEvo;

    NvBool      hasBacklightBrightness   : 1;
    NvBool      internal                 : 1;
    NvBool      allowDVISpecPClkOverride : 1;

    /* whether the connected dpy is HDMI capable */
    NvBool      hdmiCapable              : 1;
    NvBool      isVrHmd                  : 1;

    /*
     * Maximum single link and total allowed pixel clock.  This is first
     * reported by RM through DpyProbeMaxPixelClock, and then potentially
     * overridden by the EVO SOR capabilities for HDMI and DVI through
     * UpdateMaxPixelClock.
     */
    NvU32       maxPixelClockKHz;
    NvU32       maxSingleLinkPixelClockKHz;

    NVEdidRec edid;
    NVParsedEdidEvoRec parsedEdid;

    NVDpyAttributeRequestedDitheringConfig requestedDithering;

    enum NvKmsDpyAttributeRequestedColorSpaceValue requestedColorSpace;
    enum NvKmsDpyAttributeColorRangeValue requestedColorRange;

    NVAttributesSetEvoRec currentAttributes;

    nvkms_timer_handle_t *hdrToSdrTransitionTimer;

    struct {
        char *addressString;
        NVDPLibDevicePtr pDpLibDevice; // DP Lib's notion of the device.
        NvBool inbandStereoSignaling;

        NvU8 laneCount; // NV0073_CTRL_DP_DATA_SET_LANE_COUNT
        NvU8 linkRate; // NV0073_CTRL_DP_DATA_SET_LINK_BW
        NvU32 linkRate10MHz;
        enum NvKmsDpyAttributeDisplayportConnectorTypeValue connectorType;
        NvBool sinkIsAudioCapable;

        struct {
            NvBool valid;
            NvU8 buffer[NVKMS_GUID_SIZE];
            char str[NVKMS_GUID_STRING_SIZE];
        } guid;

        /*
         * When the DP serializer is in MST mode, this field is used to uniquely
         * identify each MST DPY that's connected to the DP serializer.
         *
         * This field is only valid for DP serializer DPYs, and pDpLibDevice
         * must be NULL in this case.
         */
        NvU8 serializerStreamIndex;
    } dp;

    struct {
        HDMI_SRC_CAPS srcCaps;
        HDMI_SINK_CAPS sinkCaps;
    } hdmi;

    struct {
        NvBool ycbcr422Capable;
        NvBool ycbcr444Capable;
    } colorSpaceCaps;

    struct {
        NvBool supported                  : 1;
        NvBool requiresModetimingPatching : 1;
        NvBool isDLP                      : 1;
        NvBool isAegis                    : 1;
        NvBool requiresVbiAdjustment      : 1;
        NvU32  subType;
        int    indexInOverrideTimings;
    } stereo3DVision;

    struct {
        enum NvKmsDpyVRRType type;
    } vrr;
} NVDpyEvoRec;

static inline NvBool nvDpyEvoIsDPMST(const NVDpyEvoRec *pDpyEvo)
{
    return nvDpyIdIsInDpyIdList(pDpyEvo->id,
                                pDpyEvo->pDispEvo->displayPortMSTIds);
}

// Return a pDpy's connector's display ID
static inline NvU32 nvDpyEvoGetConnectorId(const NVDpyEvoRec *pDpyEvo)
{
    NvU32 rmDpyId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);

    // This function shouldn't be used for DP MST dynamic devices.
    nvAssert(!nvDpyEvoIsDPMST(pDpyEvo));
    nvAssert(ONEBITSET(rmDpyId));

    return rmDpyId;
}

static inline
NvBool nvConnectorIsInternal(const NVConnectorEvoRec *pConnectorEvo)
{
    /* For mobile GPUs check for LVDS or embedded DisplayPort signal flag.
     * If found, DFP is internal*/
    return (pConnectorEvo->legacyType ==
            NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) &&
           (((pConnectorEvo->pDispEvo->pDevEvo->mobile) &&
             (FLD_TEST_DRF(0073, _CTRL_DFP_FLAGS, _SIGNAL, _LVDS,
                           pConnectorEvo->dfpInfo))) ||
            (FLD_TEST_DRF(0073, _CTRL_DFP_FLAGS, _SIGNAL, _DSI,
                          pConnectorEvo->dfpInfo)) ||
            (FLD_TEST_DRF(0073, _CTRL_DFP_FLAGS, _EMBEDDED_DISPLAYPORT, _TRUE,
                          pConnectorEvo->dfpInfo)));
}

static inline NvU32 NV_EVO_LOCK_PIN(NvU32 n)
{
    return NV_EVO_LOCK_PIN_0 + n;
}

static inline NvU32 NV_EVO_LOCK_PIN_INTERNAL(NvU32 n)
{
    return NV_EVO_LOCK_PIN_INTERNAL_0 + n;
}

static inline NvBool NV_EVO_LOCK_PIN_IS_INTERNAL(NvU32 n)
{
    ct_assert(NV_IS_UNSIGNED(n) && NV_EVO_LOCK_PIN_INTERNAL_0 == 0);
    return n < NV_EVO_LOCK_PIN_0;
}


/*
 * Utility macro for looping over all the pConnectorsEvo on a pDispEvo.
 */
#define FOR_ALL_EVO_CONNECTORS(_pConnectorEvo, _pDispEvo)               \
    nvListForEachEntry((_pConnectorEvo),                                \
                       &(_pDispEvo)->connectorList, connectorListEntry)

/*
 * Utility macro for declaring a for loop to walk over all the
 * pDispEvos on a particular pDevEvo.
 */
#define FOR_ALL_EVO_DISPLAYS(_pDispEvo, _i, _pDevEvo)          \
    for ((_i) = 0,                                             \
         (_pDispEvo) = (_pDevEvo)->pDispEvo[0];                \
         (_pDispEvo);                                          \
         (_i)++, (_pDispEvo) = ((_i) < (_pDevEvo)->nDispEvo) ? \
         (_pDevEvo)->pDispEvo[(_i)] : NULL)

#define FOR_ALL_EVO_DPYS(_pDpyEvo, _dpyIdList, _pDispEvo)               \
    nvListForEachEntry((_pDpyEvo), &(_pDispEvo)->dpyList, dpyListEntry) \
        if (nvDpyIdIsInDpyIdList((_pDpyEvo)->id, (_dpyIdList)))

#define FOR_ALL_EVO_FRAMELOCKS(_pFrameLockEvo)                     \
    nvListForEachEntry(_pFrameLockEvo, &nvEvoGlobal.frameLockList, \
                       frameLockListEntry)

#define FOR_ALL_EVO_DEVS(_pDevEvo) \
    nvListForEachEntry(_pDevEvo, &nvEvoGlobal.devList, devListEntry)

#define FOR_ALL_EVO_DEVS_SAFE(_pDevEvo, _pDevEvo_tmp) \
    nvListForEachEntry_safe(_pDevEvo, _pDevEvo_tmp, &nvEvoGlobal.devList, devListEntry)

#define FOR_ALL_DEFERRED_REQUEST_FIFOS_IN_SWAP_GROUP(             \
    _pSwapGroup, _pDeferredRequestFifo)                           \
    nvListForEachEntry((_pDeferredRequestFifo),                   \
                       &(_pSwapGroup)->deferredRequestFifoList,   \
                       swapGroup.deferredRequestFifoListEntry)

#define FOR_EACH_SUBDEV_IN_MASK(_sd, _mask) \
    FOR_EACH_INDEX_IN_MASK(32, _sd, _mask)

#define FOR_EACH_SUBDEV_IN_MASK_END \
    FOR_EACH_INDEX_IN_MASK_END

static inline NVDpyEvoPtr nvGetOneArbitraryDpyEvo(NVDpyIdList dpyIdList,
                                                  const NVDispEvoRec *pDispEvo)
{
    NVDpyEvoPtr pDpyEvo;

    nvAssert(nvDpyIdListIsASubSetofDpyIdList(dpyIdList,
                                             pDispEvo->validDisplays));

    FOR_ALL_EVO_DPYS(pDpyEvo, dpyIdList, pDispEvo) {
        return pDpyEvo;
    }

    return NULL;
}


/*
 * Return whether or not the display devices on the connector should
 * be handled by the DP library.
 */
static inline NvBool nvConnectorUsesDPLib(const NVConnectorEvoRec
                                          *pConnectorEvo)
{
    return (pConnectorEvo->pDpLibConnector != NULL);
}

static inline
NvBool nvConnectorIsDPSerializer(const NVConnectorEvoRec *pConnectorEvo)
{
    return (pConnectorEvo->type == NVKMS_CONNECTOR_TYPE_DP_SERIALIZER);
}

/*
 * Return whether or not the display device given is handled by the DP
 * library.
 */
static inline NvBool nvDpyUsesDPLib(const NVDpyEvoRec *pDpyEvo)
{
    return nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo);
}

/*
 * Return whether this dpy is active.  The dpy is active if it is
 * driven by a head.
 */
static inline NvBool nvDpyEvoIsActive(const NVDpyEvoRec *pDpyEvo)
{
    return (pDpyEvo->apiHead != NV_INVALID_HEAD);
}

/*
 * Return true if this dpy reports an EDID supporting HDMI 3D and
 * isn't connected via active DisplayPort.
 */
static inline NvBool nvDpyEvoSupportsHdmi3D(const NVDpyEvoRec *pDpyEvo)
{
    return (pDpyEvo->parsedEdid.valid &&
            pDpyEvo->parsedEdid.info.HDMI3DSupported &&
            !((pDpyEvo->pConnectorEvo->or.protocol ==
               NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A) ||
              (pDpyEvo->pConnectorEvo->or.protocol ==
               NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B)));

}

static inline NvBool nvHeadIsActive(const NVDispEvoRec *pDispEvo,
                                    const NvU32 head)
{
    return (head < ARRAY_LEN(pDispEvo->headState)) &&
           (pDispEvo->headState[head].pConnectorEvo != NULL);
}

static inline NvBool nvApiHeadIsActive(const NVDispEvoRec *pDispEvo,
                                       const NvU32 apiHead)
{
    return (apiHead < ARRAY_LEN(pDispEvo->apiHeadState)) &&
           (!nvDpyIdListIsEmpty(pDispEvo->apiHeadState[apiHead].activeDpys));
}

/*!
 * Return the mask of active heads on this pDispEvo.
 */
static inline NvU32 nvGetActiveHeadMask(const NVDispEvoRec *pDispEvo)
{
    NvU32 head;
    NvU32 headMask = 0;

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        if (nvHeadIsActive(pDispEvo, head)) {
            headMask |= 1 << head;
        }
    }

    return headMask;
}

static inline NvBool nvAllHeadsInactive(const NVDevEvoRec *pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;
    NvU32 head;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            if (nvHeadIsActive(pDispEvo, head)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/*
 * Return the list of dpys that are currently active on the given disp.
 */
static inline NVDpyIdList nvActiveDpysOnDispEvo(const NVDispEvoRec *pDispEvo)
{
    NVDpyIdList dpyIdList = nvEmptyDpyIdList();
    NvU32 apiHead;

    for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        const NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[apiHead];

        dpyIdList = nvAddDpyIdListToDpyIdList(dpyIdList,
                                              pApiHeadState->activeDpys);
    }

    return dpyIdList;
}

static inline NvU32 nvGpuIdOfDispEvo(const NVDispEvoRec *pDispEvo)
{
    nvAssert(pDispEvo->displayOwner < pDispEvo->pDevEvo->numSubDevices);
    return pDispEvo->pDevEvo->pSubDevices[pDispEvo->displayOwner]->gpuId;
}

static inline NvBool nvIsEmulationEvo(const NVDevEvoRec *pDevEvo)
{
    return pDevEvo->simulationType !=
        NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_NONE;
}

static inline NvBool nvIsDfpgaEvo(const NVDevEvoRec *pDevEvo)
{
    return pDevEvo->simulationType ==
        NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_DFPGA;
}

static inline NvBool nvIs3DVisionStereoEvo(const enum NvKmsStereoMode stereo)
{
    return (stereo == NVKMS_STEREO_NVIDIA_3D_VISION ||
            stereo == NVKMS_STEREO_NVIDIA_3D_VISION_PRO);
}

/*
 * Utility macro for iterating over all head bits set in a head bit mask
 */
#define FOR_ALL_HEADS(_head, _headMask)        \
    for((_head) = 0;                           \
        (_headMask) >> (_head);                \
        (_head)++)                             \
        if ((_headMask) & (1 << (_head)))

typedef struct _NVFrameLockEvo {
    NVListRec frameLockListEntry;

    /* array of subdev GPU IDs */
    NvU32  nGpuIds;
    NvU32  gpuIds[NV30F1_CTRL_MAX_GPUS_PER_GSYNC];

    NvU32  gsyncId;
    NvU32  device;               /* RM device handle for this object */

    int    fpgaIdAndRevision;    /* FPGA revId (including firmware version
                                  * and board ID) */

    int    firmwareMajorVersion; /* FPGA firmware major version */
    int    firmwareMinorVersion; /* FPGA firmware minor version */
    NvU32  boardId;              /* NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_* */
    NvU32  caps;                 /* Various capabilities flags */

    NvU32  maxSyncSkew;          /* Max sync skew increment */
    NvU32  syncSkewResolution;   /* In nanoseconds */
    NvU32  maxSyncInterval;      /* Max sync interval */

    NvU32  houseSyncUseable;

    /* House sync mode requested by user */
    enum NvKmsFrameLockAttributeHouseSyncModeValue houseSyncMode;
    NvU32 houseSyncModeValidValues;

    NvBool houseSyncAssy;       /* Current desired state */
    NvBool houseSyncArmed;      /* Current hardware state */

    NvU8   connectedGpuMask;    /* bitmask of GPUs that are connected */
    NvU8   syncReadyGpuMask;    /* bitmask of GPUs that are syncReady */

    NvBool syncReadyLast;       /* Previous NV_CTRL_FRAMELOCK_SYNC_READY
                                 * value changed either from nvctrl or
                                 * the RM, used to avoid resending events
                                 * since RM doesn't trigger a SYNC_READY
                                 * event on framelock disable */

    NvBool videoModeReadOnly;   /* If video mode is read-only */

    NvU32 maxMulDivValue;       /* Max sync multiply/divide value */

    NvBool mulDivSupported;     /* Whether this board supports setting a sync
                                 * multiplier/divider; maxMulDivValue is only
                                 * valid if this is true */

    /* Current device state */
    enum NvKmsFrameLockAttributePolarityValue  polarity;
    NvU32  syncDelay;
    NvU32  syncInterval;
    enum NvKmsFrameLockAttributeVideoModeValue videoMode;
    NvU8 mulDivValue;
    enum NvKmsFrameLockAttributeMulDivModeValue mulDivMode;
    NvBool testMode;

    NVUnixRmHandleAllocatorRec handleAllocator;

} NVFrameLockEvoRec;

/*!
 * The buffer that accumulates a string with information returned to
 * the client.
 */
typedef struct _NVEvoInfoString {
    NvU16 length;      /*! strlen(s); excludes the nul terminator */
    NvU16 totalLength; /*! number of bytes in the buffer pointed to by 's' */
    char *s;           /*! pointer to the buffer to be written to */
} NVEvoInfoStringRec;

enum NvHsMapPermissions {
    NvHsMapPermissionsNone,
    NvHsMapPermissionsReadOnly,
    NvHsMapPermissionsReadWrite,
};

#define NV_HS_BAD_GPU_ADDRESS ((NvU64) -1)

#define NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP ( \
    NVBIT64(NvKmsSurfaceMemoryFormatI8))

#define NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP ( \
    NVBIT64(NvKmsSurfaceMemoryFormatA1R5G5B5) | \
    NVBIT64(NvKmsSurfaceMemoryFormatX1R5G5B5) | \
    NVBIT64(NvKmsSurfaceMemoryFormatR5G6B5))

#define NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP ( \
    NVBIT64(NvKmsSurfaceMemoryFormatA8R8G8B8) | \
    NVBIT64(NvKmsSurfaceMemoryFormatX8R8G8B8) | \
    NVBIT64(NvKmsSurfaceMemoryFormatA2B10G10R10) | \
    NVBIT64(NvKmsSurfaceMemoryFormatX2B10G10R10) | \
    NVBIT64(NvKmsSurfaceMemoryFormatA8B8G8R8) | \
    NVBIT64(NvKmsSurfaceMemoryFormatX8B8G8R8))

#define NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP ( \
    NVBIT64(NvKmsSurfaceMemoryFormatRF16GF16BF16AF16) | \
    NVBIT64(NvKmsSurfaceMemoryFormatRF16GF16BF16XF16) | \
    NVBIT64(NvKmsSurfaceMemoryFormatR16G16B16A16))

#define NVKMS_SURFACE_MEMORY_FORMATS_YUV_PACKED422 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422) | \
    NVBIT64(NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422))

#define NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP420 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___U8V8_N420) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___V8U8_N420))

#define NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP422 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___U8V8_N422) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___V8U8_N422))

#define NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP444 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___U8V8_N444) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___V8U8_N444))

#define NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP420 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY10___U10V10_N420) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY10___V10U10_N420) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY12___U12V12_N420) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY12___V12U12_N420))

#define NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP422 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY10___U10V10_N422) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY10___V10U10_N422) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY12___U12V12_N422) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY12___V12U12_N422))

#define NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP444 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY10___U10V10_N444) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY10___V10U10_N444) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY12___U12V12_N444) | \
    NVBIT64(NvKmsSurfaceMemoryFormatY12___V12U12_N444))

#define NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR444 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___U8___V8_N444))

#define NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR420 ( \
    NVBIT64(NvKmsSurfaceMemoryFormatY8___U8___V8_N420))

struct _NVSurfaceEvoRec {
    /*
     * By default, all NVSurfaceEvoRecs will have requireDisplayHardwareAccess
     * as TRUE and on chips where ctxDma is supported, a ctxDma is allocated
     * and placed in the display engine hash table for each plane.
     *
     * But, if the client specified the noDisplayHardwareAccess flag,
     * requireDisplayHardwareAccess will be FALSE, and ctxDma will be 0 for
     * all planes.
     *
     * requireDisplayHardwareAccess is used to remember what the client
     * requested, so that we correctly honor noDisplayHardwareAccess across
     * FreeSurfaceCtxDmasForAllOpens() /
     * AllocSurfaceCtxDmasForAllOpens() cycles.
     */
    NvBool requireDisplayHardwareAccess;

    struct {
        NvU32 rmHandle;
        NVSurfaceDescriptor surfaceDesc;
        NvU32 pitch;
        NvU64 offset;
        NvU64 rmObjectSizeInBytes;
    } planes[NVKMS_MAX_PLANES_PER_SURFACE];

    struct {
        const struct NvKmsPerOpenDev *pOpenDev;
        NvKmsSurfaceHandle surfaceHandle;
    } owner;

    NvU32 widthInPixels;
    NvU32 heightInPixels;

    NvU32 log2GobsPerBlockY;

    /*
     * GPU virtual address of the surface, in NVKMS's VA space for use by
     * headSurface.
     */
    NvU64 gpuAddress;

    /*
     * HeadSurface needs a CPU mapping of surfaces containing semaphores.
     */
    void *cpuAddress[NVKMS_MAX_SUBDEVICES];

    enum NvKmsSurfaceMemoryLayout layout;
    enum NvKmsSurfaceMemoryFormat format;

    NvKmsMemoryIsoType isoType;

    /*
     * A surface has two reference counts:
     *
     * - rmRefCnt indicates how many uses of the surface reference
     *   NVSurfaceEvoRec::planes[]::rmHandle (the surface owner who registered
     *   the surface, EVO currently displaying the surface, an open
     *   surface grant file descriptor).
     *
     * - structRefCnt indicates how many uses of the surface reference
     *   the NVSurfaceEvoRec.  In addition to the rmRefCnt uses, this
     *   will also count NVKMS clients who acquired the surface
     *   through GRANT_SURFACE/ACQUIRE_SURFACE.
     *
     * When a client registers a surface, both reference counts will
     * be initialized to 1.  The RM surface for each plane will be unduped when
     * rmRefCnt reaches zero.  The NVSurfaceEvoRec structure will be
     * freed when structRefCnt reaches zero.
     *
     * In most cases, one of the following will be true:
     * (rmRefCnt == 0) && (structRefCnt == 0)
     * (rmRefCnt != 0) && (structRefCnt != 0)
     * The only exception is when the owner of the surface unregisters it while
     * other clients still have references to it; in that case, the rmRefCnt
     * can drop to zero while structRefCnt is still non-zero.
     *
     * If rmRefCnt reaches zero before structRefCnt, the surface is
     * "orphaned": it still exists in ACQUIRE_SURFACE clients' handle
     * namespaces and/or granted FDs, but is not usable in subsequent API
     * requests (e.g., to flip, specify cursor image, etc).
     *
     * Described in a table:
     *
     * ACTION                                         rmRefCnt  structRefCnt
     *   a) NVKMS_IOCTL_REGISTER_SURFACE               =1        =1
     *   b) flip to surface                            +1        +1
     *   c) NVKMS_IOCTL_GRANT_SURFACE(grantFd)         n/a       +1
     *   d) NVKMS_IOCTL_ACQUIRE_SURFACE                n/a       +1
     *   e) NVKMS_IOCTL_UNREGISTER_SURFACE             -1        -1
     *   f) flip away from surface                     -1        -1
     *   g) close(grantFd)                             n/a       -1
     *   h) NVKMS_IOCTL_RELEASE_SURFACE                n/a       -1
     *   i) ..._REGISTER_DEFERRED_REQUEST_FIFO         +1        +1
     *   j) ..._UNREGISTER_DEFERRED_REQUEST_FIFO       -1        -1
     *
     * (e) complements (a)
     * (f) complements (b)
     * (g) complements (c)
     * (h) complements (d)
     * (j) complements (i)
     */
    NvU64 rmRefCnt;
    NvU64 structRefCnt;

#if NVKMS_PROCFS_ENABLE
    NvBool procFsFlag;
#endif

    /*
     * Disallow DIFR if display caching is forbidden. This will be set for
     * CPU accessible surfaces.
     */
    NvBool noDisplayCaching;

    /* Keep track of prefetched surfaces. */
    NvU32 difrLastPrefetchPass;
};

typedef struct _NVDeferredRequestFifoRec {
    NVSurfaceEvoPtr pSurfaceEvo;
    struct NvKmsDeferredRequestFifo *fifo;

    /* A deferred request fifo may be joined to a swapGroup. */
    struct {
        NVSwapGroupPtr pSwapGroup;
        NVListRec deferredRequestFifoListEntry;
        NvBool ready;
        NvBool perEyeStereo;
        NvBool pendingJoined;
        NvBool pendingReady;
        NvU32 semaphoreIndex;
        struct NvKmsPerOpen *pOpenUnicastEvent;
    } swapGroup;
} NVDeferredRequestFifoRec;

typedef struct _NVSwapGroupRec {
    NVListRec deferredRequestFifoList;
    NvBool zombie;
    NvBool pendingFlip;
    NvU32 nMembers;
    NvU32 nMembersReady;
    NvU32 nMembersPendingJoined;

    NvU16 nClips;
    struct NvKmsRect *pClipList;
    NvBool swapGroupIsFullscreen;

    NvU64 refCnt;
} NVSwapGroupRec;

typedef struct {
    NvU32 clientHandle;
    nvRMContext rmSmgContext;

    NVListRec devList;
    NVListRec frameLockList;

#if defined(DEBUG)
    NVListRec debugMemoryAllocationList;
#endif

    struct NvKmsPerOpen *nvKmsPerOpen;
} NVEvoGlobal;

extern NVEvoGlobal nvEvoGlobal;

/*
 * These enums are used during IMP validation:
 * - NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE means that no changes will be made to
 *   the current display bandwidth values.
 * - NV_EVO_REALLOCATE_BANDWIDTH_MODE_PRE means that NVKMS will increase the
 *   current display bandwidth values if required by IMP. This is typically
 *   specified pre-modeset/flip.
 * - NV_EVO_REALLOCATE_BANDWIDTH_MODE_POST means that NVKMS may potentially
 *   decrease the current display bandwidth values to match the current display
 *   configuration. This is typically specified post-modeset/flip.
 */
typedef enum {
    NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE = 0,
    NV_EVO_REALLOCATE_BANDWIDTH_MODE_PRE  = 1,
    NV_EVO_REALLOCATE_BANDWIDTH_MODE_POST = 2,
} NVEvoReallocateBandwidthMode;

typedef struct {
    struct {
        /* pTimings == NULL => this head is disabled */
        const NVHwModeTimingsEvo *pTimings;
        NvBool enableDsc;
        NvBool b2Heads1Or;
        enum nvKmsPixelDepth pixelDepth;
        const struct NvKmsUsageBounds *pUsage;
        NvU32 displayId;
        NvU32 orIndex;
        NvU8 orType; /* NV0073_CTRL_SPECIFIC_OR_TYPE_* */
        NvU32 dscSliceCount;
        NvU32 possibleDscSliceCountMask;
        NVHwHeadMultiTileConfigRec multiTileConfig;
        NvBool modesetRequested : 1;
    } head[NVKMS_MAX_HEADS_PER_DISP];

    NvBool requireBootClocks;
    NVEvoReallocateBandwidthMode reallocBandwidth;
} NVEvoIsModePossibleDispInput;

typedef struct {
    NvBool possible;
    NvU32 minRequiredBandwidthKBPS;
    NvU32 floorBandwidthKBPS;
    struct {
        NvU32 dscSliceCount;
        NVHwHeadMultiTileConfigRec multiTileConfig;
    } head[NVKMS_MAX_HEADS_PER_DISP];
} NVEvoIsModePossibleDispOutput;

/* CRC-query specific defines */
/*!
 * Structure that defines information about where a single variable is stored in
 * the CRC32NotifierEntry structure
 */
typedef struct _CRC32NotifierEntryRec {
    NvU32 field_offset;                       /* Var's offset from start of CRC32Notifier Struct */
    NvU32 field_base_bit;                     /* LSB bit index for variable in entry */
    NvU32 field_extent_bit;                   /* MSB bit index for variable in entry */
    struct NvKmsDpyCRC32 *field_frame_values; /* Array to store read field values across frames */
} CRC32NotifierEntryRec;

/*!
 * Internally identifies flag read from CRC32Notifier's Status for error-checking
 */
enum CRC32NotifierFlagType {
    NVEvoCrc32NotifierFlagCount,
    NVEvoCrc32NotifierFlagCrcOverflow,
};

/*!
 * Structure that defines information about where a single flag is stored in
 * the Status of the CRC32NotifierEntry structure
 */
typedef struct _CRC32NotifierEntryFlags {
    NvU32 flag_base_bit;                      /* LSB bit index for flag in entry */
    NvU32 flag_extent_bit;                    /* MSB bit index for flag in entry */
    enum CRC32NotifierFlagType flag_type;     /* Type of error-checking to perform on flag */
} CRC32NotifierEntryFlags;

/*!
 * Internal Crc32NotifierRead structure used to collect multiple frames of CRC
 * data from a QueryCRC32 call. Arrays should be allocated to match
 * entry_count frames.
 */
typedef struct _CRC32NotifierCrcOut {
  /*!
   * Array of CRCs generated from the Compositor hardware
   */
    struct NvKmsDpyCRC32 *compositorCrc32;

    /*!
     * CRCs generated from the RG hardware, if head is driving RG/SF.
     */
    struct NvKmsDpyCRC32 *rasterGeneratorCrc32;

    /*!
     * Crc values generated from the target SF/OR depending on connector's OR type
     */
    struct NvKmsDpyCRC32 *outputCrc32;

} CRC32NotifierCrcOut;

typedef enum {
    NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME,
    NV_EVO_INFOFRAME_TRANSMIT_CONTROL_INIT =
        NV_EVO_INFOFRAME_TRANSMIT_CONTROL_EVERY_FRAME,
    NV_EVO_INFOFRAME_TRANSMIT_CONTROL_SINGLE_FRAME,
} NvEvoInfoFrameTransmitControl;

typedef const struct _nv_evo_hal {
    void (*SetRasterParams)     (NVDevEvoPtr pDevEvo, int head,
                                 const NVHwModeTimingsEvo *pTimings,
                                 const NvU8 tilePosition,
                                 const NVDscInfoEvoRec *pDscInfo,
                                 const NVEvoColorRec *pOverscanColor,
                                 NVEvoUpdateState *updateState);
    void (*SetProcAmp)          (NVDispEvoPtr pDispEvo, const NvU32 head,
                                 NVEvoUpdateState *updateState);
    void (*SetHeadControl)      (NVDevEvoPtr, int sd, int head,
                                 NVEvoUpdateState *updateState);
    void (*SetHeadRefClk)       (NVDevEvoPtr, int head, NvBool external,
                                 NVEvoUpdateState *updateState);
    void (*HeadSetControlOR)    (NVDevEvoPtr pDevEvo,
                                 const int head,
                                 const NVHwModeTimingsEvo *pTimings,
                                 const enum nvKmsPixelDepth pixelDepth,
                                 const NvBool colorSpaceOverride,
                                 NVEvoUpdateState *updateState);
    void (*ORSetControl)        (NVDevEvoPtr pDevEvo,
                                 const NVConnectorEvoRec *pConnectorEvo,
                                 const enum nvKmsTimingsProtocol protocol,
                                 const NvU32 orIndex,
                                 const NvU32 headMask,
                                 NVEvoUpdateState *updateState);
    void (*HeadSetDisplayId)    (NVDevEvoPtr pDevEvo,
                                 const NvU32 head, const NvU32 displayId,
                                 NVEvoUpdateState *updateState);
    NvBool (*SetUsageBounds)    (NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                 const struct NvKmsUsageBounds *pUsage,
                                 NVEvoUpdateState *updateState);
    void (*Update)              (NVDevEvoPtr,
                                 const NVEvoUpdateState *updateState,
                                 NvBool releaseElv);
    void (*IsModePossible)      (NVDispEvoPtr,
                                 const NVEvoIsModePossibleDispInput *,
                                 NVEvoIsModePossibleDispOutput *);
    void (*PrePostIMP)          (NVDispEvoPtr, NvBool isPre);
    void (*SetNotifier)         (NVDevEvoRec *pDevEvo,
                                 const NvBool notify,
                                 const NvBool awaken,
                                 const NvU32 notifier,
                                 NVEvoUpdateState *updateState);
    NvBool (*GetCapabilities)   (NVDevEvoPtr);
    void (*Flip)                (NVDevEvoPtr pDevEvo,
                                 NVEvoChannelPtr pChannel,
                                 const NVFlipChannelEvoHwState *pHwState,
                                 NVEvoUpdateState *updateState,
                                 NvBool bypassComposition);
    void (*FlipTransitionWAR)   (NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                 const NVEvoSubDevHeadStateRec *pSdHeadState,
                                 const NVFlipEvoHwState *pFlipState,
                                 NVEvoUpdateState *updateState);
    void (*FillLUTSurface)      (NVEvoLutEntryRec *pLUTBuffer,
                                 const NvU16 *red,
                                 const NvU16 *green,
                                 const NvU16 *blue,
                                 int nColorMapEntries, int depth);
    void (*SetOutputLut)        (NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                 const NVFlipLutHwState *pOutputLut,
                                 NvU32 fpNormScale,
                                 NVEvoUpdateState *updateState,
                                 NvBool bypassComposition);
    void (*SetOutputScaler)     (const NVDispEvoRec *pDispEvo, const NvU32 head,
                                 const NvU32 imageSharpeningValue,
                                 NVEvoUpdateState *updateState);
    void (*SetViewportPointIn)  (NVDevEvoPtr pDevEvo, const int head,
                                 NvU16 x, NvU16 y,
                                 NVEvoUpdateState *updateState);
    void (*SetViewportInOut)    (NVDevEvoPtr pDevEvo, const int head,
                                 const NVHwModeViewPortEvo *pViewPortOutMin,
                                 const NVHwModeViewPortEvo *pViewPortOut,
                                 const NVHwModeViewPortEvo *pViewPortOutMax,
                                 NVEvoUpdateState *updateState);
    void (*SetCursorImage)      (NVDevEvoPtr pDevEvo, const int head,
                                 const NVSurfaceEvoRec *,
                                 NVEvoUpdateState *updateState,
                                 const struct NvKmsCompositionParams *pCursorCompParams);
    NvBool (*ValidateCursorSurface)(const NVDevEvoRec *pDevEvo,
                                    const NVSurfaceEvoRec *pSurfaceEvo);
    NvBool (*ValidateWindowFormat)(const enum NvKmsSurfaceMemoryFormat format,
                                   const struct NvKmsRect *sourceFetchRect,
                                   NvU32 *hwFormatOut);
    void (*InitCompNotifier)    (const NVDispEvoRec *pDispEvo, int idx);
    NvBool (*IsCompNotifierComplete) (NVDispEvoPtr pDispEvo, int idx);
    void (*WaitForCompNotifier) (const NVDispEvoRec *pDispEvo, int idx);
    void (*SetDither)           (NVDispEvoPtr pDispEvo, const int head,
                                 const NvBool enabled, const NvU32 type,
                                 const NvU32 algo,
                                 NVEvoUpdateState *updateState);
    void (*SetStallLock)        (NVDispEvoPtr pDispEvo, const int head,
                                 NvBool enable, NVEvoUpdateState *updateState);
    void (*SetDisplayRate)      (NVDispEvoPtr pDispEvo, const int head,
                                 NvBool enable,
                                 NVEvoUpdateState *updateState,
                                 NvU32 timeoutMicroseconds);
    void (*InitChannel)         (NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel);
    void (*InitDefaultLut)      (NVDevEvoPtr pDevEvo);
    void (*InitWindowMapping)   (const NVDispEvoRec *pDispEvo,
                                 NVEvoModesetUpdateState *pModesetUpdateState);
    NvBool (*IsChannelIdle)     (NVDevEvoPtr, NVEvoChannelPtr, NvU32 sd,
                                 NvBool *result);
    NvBool (*IsChannelMethodPending)(NVDevEvoPtr, NVEvoChannelPtr, NvU32 sd,
                                     NvBool *result);
    NvBool (*ForceIdleSatelliteChannel)(NVDevEvoPtr,
                                        const NVEvoIdleChannelState *idleChannelState);
    NvBool (*ForceIdleSatelliteChannelIgnoreLock)(NVDevEvoPtr,
                                                  const NVEvoIdleChannelState *idleChannelState);

    void (*AccelerateChannel)(NVDevEvoPtr pDevEvo,
                              NVEvoChannelPtr pChannel,
                              const NvU32 sd,
                              const NvBool trashPendingMethods,
                              const NvBool unblockMethodsInExecutation,
                              NvU32 *pOldAccelerators);

    void (*ResetChannelAccelerators)(NVDevEvoPtr pDevEvo,
                                     NVEvoChannelPtr pChannel,
                                     const NvU32 sd,
                                     const NvBool trashPendingMethods,
                                     const NvBool unblockMethodsInExecutation,
                                     NvU32 oldAccelerators);

    NvBool (*AllocRmCtrlObject) (NVDevEvoPtr);
    void (*FreeRmCtrlObject)    (NVDevEvoPtr);
    void  (*SetImmPointOut)     (NVDevEvoPtr, NVEvoChannelPtr, NvU32 sd,
                                 NVEvoUpdateState *updateState,
                                 NvU16 x, NvU16 y);
    void (*StartCRC32Capture)   (NVDevEvoPtr       pDevEvo,
                                 NVEvoDmaPtr       pDma,
                                 NVConnectorEvoPtr pConnectorEvo,
                                 const enum nvKmsTimingsProtocol protocol,
                                 const NvU32       orIndex,
                                 NvU32             head,
                                 NvU32             sd,
                                 NVEvoUpdateState *updateState /* out */);
    void (*StopCRC32Capture)    (NVDevEvoPtr       pDevEvo,
                                 NvU32             head,
                                 NVEvoUpdateState *updateState /* out */);
    NvBool (*QueryCRC32)        (NVDevEvoPtr       pDevEvo,
                                 NVEvoDmaPtr       pDma,
                                 NvU32             sd,
                                 NvU32             entry_count,
                                 CRC32NotifierCrcOut *crc32 /* out */,
                                 NvU32            *numCRC32    /* out */);
    void (*GetScanLine)         (const NVDispEvoRec *pDispEvo,
                                 const NvU32 head,
                                 NvU16 *pScanLine,
                                 NvBool *pInBlankingPeriod);
    void (*ConfigureVblankSyncObject) (NVDevEvoPtr pDevEvo,
                                       NvU16 rasterLine,
                                       NvU32 head,
                                       NvU32 semaphoreIndex,
                                       const NVSurfaceDescriptor *pSurfaceDesc,
                                       NVEvoUpdateState* pUpdateState);

    void (*SetDscParams)        (const NVDispEvoRec *pDispEvo,
                                 const NvU32 head,
                                 const NVDscInfoEvoRec *pDscInfo,
                                 const enum nvKmsPixelDepth pixelDepth);

    void (*EnableMidFrameAndDWCFWatermark)(NVDevEvoPtr pDevEvo,
                                           NvU32 sd,
                                           NvU32 head,
                                           NvBool enable,
                                           NVEvoUpdateState *pUpdateState);

    NvU32 (*GetActiveViewportOffset)(NVDispEvoRec *pDispEvo, NvU32 head);

    void (*ClearSurfaceUsage)   (NVDevEvoPtr pDevEvo,
                                 NVSurfaceEvoPtr pSurfaceEvo);

    NvBool (*ComputeWindowScalingTaps)(const NVDevEvoRec *pDevEvo,
                                       const NVEvoChannel *pChannel,
                                       NVFlipChannelEvoHwState *pHwState);

    const NVEvoScalerCaps* (*GetWindowScalingCaps)(const NVDevEvoRec *pDevEvo);

    void (*SetMergeMode)(const NVDispEvoRec *pDispEvo,
                         const NvU32 head,
                         const NVEvoMergeMode mode,
                         NVEvoUpdateState* pUpdateState);
    void (*SendHdmiInfoFrame)(const NVDispEvoRec *pDispEvo,
                              const NvU32 head,
                              const NvEvoInfoFrameTransmitControl transmitCtrl,
                              const NVT_INFOFRAME_HEADER *pInfoFrameHeader,
                              const NvU32 infoFrameSize,
                              NvBool needChecksum);
    void (*DisableHdmiInfoFrame)(const NVDispEvoRec *pDispEvo,
                                 const NvU32 head,
                                 const NvU8 nvtInfoFrameType);
    void (*SendDpInfoFrameSdp)(const NVDispEvoRec *pDispEvo,
                               const NvU32 head,
                               const NvEvoInfoFrameTransmitControl transmitCtrl,
                               const DPSDP_DESCRIPTOR *sdp);
    void (*SetDpVscSdp)(const NVDispEvoRec *pDispEvo,
                        const NvU32 head,
                        const DPSDP_DP_VSC_SDP_DESCRIPTOR *sdp,
                        NVEvoUpdateState *pUpdateState);
    void (*InitHwHeadMultiTileConfig)(NVDevEvoRec *pDevEvo);
    void (*SetMultiTileConfig)(const NVDispEvoRec *pDispEvo,
                               const NvU32 head,
                               const NVHwModeTimingsEvo *pTimings,
                               const NVDscInfoEvoRec *pDscInfo,
                               const NVHwHeadMultiTileConfigRec *pMultiTileConfig,
                               NVEvoModesetUpdateState *pModesetUpdateState);

    NvU32 (*AllocSurfaceDescriptor) (NVDevEvoPtr pDevEvo,
                                     NVSurfaceDescriptor *pSurfaceDesc,
                                     NvU32 memoryHandle,
                                     NvU32 localCtxDmaFlags,
                                     NvU64 limit);

    void (*FreeSurfaceDescriptor) (NVDevEvoPtr pDevEvo,
                                   NvU32 deviceHandle,
                                   NVSurfaceDescriptor *pSurfaceDesc);

    NvU32 (*BindSurfaceDescriptor) (NVDevEvoPtr pDevEvo,
                                    NVEvoChannelPtr pChannel,
                                    NVSurfaceDescriptor *pSurfaceDesc);

    void (*SetTmoLutSurfaceAddress) (const NVDevEvoRec *pDevEvo,
                                     NVEvoChannelPtr pChannel,
                                     const NVSurfaceDescriptor *pSurfaceDesc,
                                     NvU32 offset);

    void (*SetILUTSurfaceAddress) (const NVDevEvoRec *pDevEvo,
                                   NVEvoChannelPtr pChannel,
                                   const NVSurfaceDescriptor *pSurfaceDesc,
                                   NvU32 offset);

    void (*SetISOSurfaceAddress) (const NVDevEvoRec *pDevEvo,
                                  NVEvoChannelPtr pChannel,
                                  const NVSurfaceDescriptor *pSurfaceDesc,
                                  NvU32 offset,
                                  NvU32 ctxDmaIdx,
                                  NvBool isBlocklinear);

    void (*SetCoreNotifierSurfaceAddressAndControl) (const NVDevEvoRec *pDevEvo,
                                                     NVEvoChannelPtr pChannel,
                                                     const NVSurfaceDescriptor *pSurfaceDesc,
                                                     NvU32 notifierOffset,
                                                     NvU32 ctrlVal);

    void (*SetWinNotifierSurfaceAddressAndControl) (const NVDevEvoRec *pDevEvo,
                                                    NVEvoChannelPtr pChannel,
                                                    const NVSurfaceDescriptor *pSurfaceDesc,
                                                    NvU32 notifierOffset,
                                                    NvU32 ctrlVal);

    void (*SetSemaphoreSurfaceAddressAndControl) (const NVDevEvoRec *pDevEvo,
                                                  NVEvoChannelPtr pChannel,
                                                  const NVSurfaceDescriptor *pSurfaceDesc,
                                                  NvU32 semaphoreOffset,
                                                  NvU32 ctrlVal);

    void (*SetAcqSemaphoreSurfaceAddressAndControl) (const NVDevEvoRec *pDevEvo,
                                                     NVEvoChannelPtr pChannel,
                                                     const NVSurfaceDescriptor *pSurfaceDesc,
                                                     NvU32 semaphoreOffset,
                                                     NvU32 ctrlVal);

    struct {
        NvU32 supportsNonInterlockedUsageBoundsUpdate   :1;
        NvU32 supportsDisplayRate                       :1;
        NvU32 supportsFlipLockRGStatus                  :1;
        NvU32 needDefaultLutSurface                     :1;
        NvU32 hasUnorm16OLUT                            :1;
        NvU32 supportsImageSharpening                   :1;
        NvU32 supportsHDMIVRR                           :1;
        NvU32 supportsCoreChannelSurface                :1;
        NvU32 supportsHDMIFRL                           :1;
        NvU32 supportsSetStorageMemoryLayout            :1;
        NvU32 supportsIndependentAcqRelSemaphore        :1;
        NvU32 supportsCoreLut                           :1;
        NvU32 supportsSynchronizedOverlayPositionUpdate :1;
        NvU32 supportsVblankSyncObjects                 :1;
        NvU32 requiresScalingTapsInBothDimensions       :1;
        NvU32 supportsMergeMode                         :1;
        NvU32 supportsHDMI10BPC                         :1;
        NvU32 supportsDPAudio192KHz                     :1;
        NvU32 supportsInputColorSpace                   :1;
        NvU32 supportsInputColorRange                   :1;

        NvU32 supportedDitheringModes;
        size_t impStructSize;
        NVEvoScalerTaps minScalerTaps;
        NvU64 xEmulatedSurfaceMemoryFormats;
    } caps;
} NVEvoHAL, *NVEvoHALPtr;

typedef const struct _nv_evo_cursor_hal {
    NvU32 klass;

    void (*MoveCursor)          (NVDevEvoPtr, NvU32 sd, NvU32 head,
                                 NvS16 x, NvS16 y);
    void (*ReleaseElv)          (NVDevEvoPtr, NvU32 sd, NvU32 head);

    struct {
        NvU16 maxSize;
    } caps;
} NVEvoCursorHAL, *NVEvoCursorHALPtr;

NvU32 nvEvoGetHeadSetStoragePitchValue(const NVDevEvoRec *pDevEvo,
                                       enum NvKmsSurfaceMemoryLayout layout,
                                       NvU32 pitch);

NvBool nvEvoGetHeadSetControlCursorValue90(const NVDevEvoRec *pDevEvo,
                                           const NVSurfaceEvoRec *pSurfaceEvo,
                                           NvU32 *pValue);

static inline NvBool nvEvoScalingUsageBoundsEqual(
    const struct NvKmsScalingUsageBounds *a,
    const struct NvKmsScalingUsageBounds *b)
{
    return (a->maxVDownscaleFactor == b->maxVDownscaleFactor) &&
           (a->maxHDownscaleFactor == b->maxHDownscaleFactor) &&
           (a->vTaps == b->vTaps) &&
           (a->vUpscalingAllowed == b->vUpscalingAllowed);
}

static inline NvBool
nvEvoLayerUsageBoundsEqual(const struct NvKmsUsageBounds *a,
                           const struct NvKmsUsageBounds *b,
                           const NvU32 layer)
{
    return (a->layer[layer].usable == b->layer[layer].usable) &&
           (a->layer[layer].supportedSurfaceMemoryFormats ==
                b->layer[layer].supportedSurfaceMemoryFormats) &&
           nvEvoScalingUsageBoundsEqual(&a->layer[layer].scaling,
                                        &b->layer[layer].scaling);
}

static inline void nvAssignHwHeadsMaskApiHeadState(
    NVDispApiHeadStateEvoRec *pApiHeadState,
    const NvU32 hwHeadsMask)
{
    pApiHeadState->hwHeadsMask = hwHeadsMask;
    pApiHeadState->attributes.numberOfHardwareHeadsUsed =
        nvPopCount32(hwHeadsMask);
}

typedef struct _NVVblankSemControlHeadEntryRec {
    NVListRec                                listEntry;
    NvU32                                    previousRequestCounter;
    NvU64                                    previousVblankCount;
    struct NvKmsVblankSemControlDataOneHead *pDataOneHead;
} NVVblankSemControlHeadEntry;

typedef struct _NVVblankSemControl {
    NvU32 dispIndex;
    NvU32 apiHeadMask;
    NVSurfaceEvoRec *pSurfaceEvo;
    NVVblankSemControlHeadEntry headEntry[NV_MAX_HEADS];
} NVVblankSemControl;

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_TYPES_H__ */
