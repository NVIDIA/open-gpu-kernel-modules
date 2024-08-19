/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if !defined(__NVKMS_KAPI_H__)

#include "nvtypes.h"

#include "nv-gpu-info.h"
#include "nvkms-api-types.h"
#include "nvkms-format.h"

#define __NVKMS_KAPI_H__

#define NVKMS_KAPI_MAX_HEADS           4

#define NVKMS_KAPI_MAX_CONNECTORS     16
#define NVKMS_KAPI_MAX_CLONE_DISPLAYS 16

#define NVKMS_KAPI_EDID_BUFFER_SIZE   2048

#define NVKMS_KAPI_MODE_NAME_LEN      32

/**
 * \defgroup Objects
 * @{
 */

struct NvKmsKapiDevice;
struct NvKmsKapiMemory;
struct NvKmsKapiSurface;
struct NvKmsKapiChannelEvent;
struct NvKmsKapiSemaphoreSurface;
struct NvKmsKapiSemaphoreSurfaceCallback;

typedef NvU32 NvKmsKapiConnector;
typedef NvU32 NvKmsKapiDisplay;

/** @} */

/**
 * \defgroup FuncPtrs
 * @{
 */

/*
 * Note: The channel event proc should not call back into NVKMS-KAPI driver.
 * The callback into NVKMS-KAPI from the channel event proc, may cause
 * deadlock.
 */
typedef void NvKmsChannelEventProc(void *dataPtr, NvU32 dataU32);

/*
 * Note: Same as above, this function must not call back into NVKMS-KAPI, nor
 * directly into RM. Doing so could cause deadlocks given the notification
 * function will most likely be called from within RM's interrupt handler
 * callchain.
 */
typedef void NvKmsSemaphoreSurfaceCallbackProc(void *pData);

/** @} */

/**
 * \defgroup Structs
 * @{
 */

struct NvKmsKapiDisplayModeTimings {

    NvU32 refreshRate;
    NvU32 pixelClockHz;
    NvU32 hVisible;
    NvU32 hSyncStart;
    NvU32 hSyncEnd;
    NvU32 hTotal;
    NvU32 hSkew;
    NvU32 vVisible;
    NvU32 vSyncStart;
    NvU32 vSyncEnd;
    NvU32 vTotal;

    struct {

        NvU32 interlaced : 1;
        NvU32 doubleScan : 1;
        NvU32 hSyncPos   : 1;
        NvU32 hSyncNeg   : 1;
        NvU32 vSyncPos   : 1;
        NvU32 vSyncNeg   : 1;

    } flags;

    NvU32 widthMM;
    NvU32 heightMM;

};

struct NvKmsKapiDisplayMode {
    struct NvKmsKapiDisplayModeTimings timings;
    char name[NVKMS_KAPI_MODE_NAME_LEN];
};

#define NVKMS_KAPI_LAYER_MAX 8

#define NVKMS_KAPI_LAYER_INVALID_IDX           0xff
#define NVKMS_KAPI_LAYER_PRIMARY_IDX              0

struct NvKmsKapiDeviceResourcesInfo {

    NvU32 numHeads;
    NvU32 numLayers[NVKMS_KAPI_MAX_HEADS];

    NvU32        numConnectors;
    NvKmsKapiConnector connectorHandles[NVKMS_KAPI_MAX_CONNECTORS];

    struct {
        NvU32 validCursorCompositionModes;
        NvU64 supportedCursorSurfaceMemoryFormats;

        struct {
            NvU64 maxSubmittedOffset;
            NvU64 stride;
        } semsurf;

        struct {
            NvU16 validRRTransforms;
            NvU32 validCompositionModes;
        } layer[NVKMS_KAPI_LAYER_MAX];

        NvU32 minWidthInPixels;
        NvU32 maxWidthInPixels;

        NvU32 minHeightInPixels;
        NvU32 maxHeightInPixels;

        NvU32 maxCursorSizeInPixels;

        NvU32 pitchAlignment;

        NvU32 hasVideoMemory;

        NvU32 numDisplaySemaphores;

        NvU8  genericPageKind;

        NvBool  supportsSyncpts;

        NvBool requiresVrrSemaphores;
    } caps;

    NvU64 supportedSurfaceMemoryFormats[NVKMS_KAPI_LAYER_MAX];
    NvBool supportsICtCp[NVKMS_KAPI_LAYER_MAX];
};

#define NVKMS_KAPI_LAYER_MASK(layerType) (1 << (layerType))

typedef enum NvKmsKapiMappingTypeRec {
    NVKMS_KAPI_MAPPING_TYPE_USER   = 1,
    NVKMS_KAPI_MAPPING_TYPE_KERNEL = 2,
} NvKmsKapiMappingType;

struct NvKmsKapiConnectorInfo {

    NvKmsKapiConnector handle;

    NvU32 physicalIndex;

    NvKmsConnectorSignalFormat signalFormat;
    NvKmsConnectorType         type;

    /*
     * List of connectors, not possible to serve together with this connector
     * because they are competing for same resources.
     */
    NvU32        numIncompatibleConnectors;
    NvKmsKapiConnector incompatibleConnectorHandles[NVKMS_KAPI_MAX_CONNECTORS];

};

struct NvKmsKapiStaticDisplayInfo {

    NvKmsKapiDisplay handle;

    NvKmsKapiConnector connectorHandle;

    /* Set for DisplayPort MST displays (dynamic displays) */
    char dpAddress[NVKMS_DP_ADDRESS_STRING_LENGTH];

    NvBool internal;

    /* List of potential sibling display for cloning */
    NvU32  numPossibleClones;
    NvKmsKapiDisplay possibleCloneHandles[NVKMS_KAPI_MAX_CLONE_DISPLAYS];

    NvU32 headMask;
};

struct NvKmsKapiSyncParams {
    union {
        struct {
            /*!
             * Possible syncpt use case in kapi.
             * For pre-syncpt, use only id and value
             * and for post-syncpt, use only fd.
             */
            NvU32   preSyncptId;
            NvU32   preSyncptValue;
        } syncpt;

        struct {
            NvU32 index;
        } semaphore;
    } u;

    NvBool preSyncptSpecified;
    NvBool postSyncptRequested;
    NvBool semaphoreSpecified;
};

struct NvKmsKapiLayerConfig {
    struct NvKmsKapiSurface *surface;
    struct {
        enum NvKmsCompositionBlendingMode compMode;
        NvU8 surfaceAlpha;
    } compParams;
    struct NvKmsRRParams rrParams;
    struct NvKmsKapiSyncParams syncParams;

    struct {
        struct NvKmsHDRStaticMetadata val;
        NvBool enabled;
    } hdrMetadata;

    enum NvKmsOutputTf tf;

    NvU8 minPresentInterval;
    NvBool tearing;

    NvU16 srcX, srcY;
    NvU16 srcWidth, srcHeight;

    NvS16 dstX, dstY;
    NvU16 dstWidth, dstHeight;

    enum NvKmsInputColorSpace inputColorSpace;
    struct NvKmsCscMatrix csc;
    NvBool cscUseMain;
};

struct NvKmsKapiLayerRequestedConfig {
    struct NvKmsKapiLayerConfig config;
    struct {
        NvBool surfaceChanged     : 1;
        NvBool srcXYChanged       : 1;
        NvBool srcWHChanged       : 1;
        NvBool dstXYChanged       : 1;
        NvBool dstWHChanged       : 1;
        NvBool cscChanged         : 1;
        NvBool tfChanged          : 1;
        NvBool hdrMetadataChanged : 1;
    } flags;
};

struct NvKmsKapiCursorRequestedConfig {
    struct NvKmsKapiSurface *surface;
    struct {
        enum NvKmsCompositionBlendingMode compMode;
        NvU8 surfaceAlpha;
    } compParams;

    NvS16 dstX, dstY;

    struct {
        NvBool surfaceChanged : 1;
        NvBool dstXYChanged   : 1;
    } flags;
};

struct NvKmsKapiHeadModeSetConfig {
    /*
     * DRM distinguishes between the head state "enabled" (the specified
     * configuration for the head is valid, its resources are allocated,
     * etc, but the head may not necessarily be currently driving pixels
     * to its output resource) and the head state "active" (the head is
     * "enabled" _and_ the head is actively driving pixels to its output
     * resource).
     *
     * This distinction is for DPMS:
     *
     *  DPMS On  : enabled=true, active=true
     *  DPMS Off : enabled=true, active=false
     *
     * "Enabled" state is indicated by numDisplays != 0.
     * "Active" state is indicated by bActive == true.
     */
    NvBool bActive;

    NvU32  numDisplays;
    NvKmsKapiDisplay displays[NVKMS_KAPI_MAX_CLONE_DISPLAYS];

    struct NvKmsKapiDisplayMode mode;

    NvBool vrrEnabled;

    struct {
        NvBool enabled;
        enum NvKmsInfoFrameEOTF eotf;
        struct NvKmsHDRStaticMetadata staticMetadata;
    } hdrInfoFrame;

    enum NvKmsOutputColorimetry colorimetry;

    struct {
        struct {
            NvU32 depth;
            NvU32 start;
            NvU32 end;
            struct NvKmsLutRamps *pRamps;
        } input;

        struct {
            NvBool enabled;
            struct NvKmsLutRamps *pRamps;
        } output;
    } lut;
};

struct NvKmsKapiHeadRequestedConfig {
    struct NvKmsKapiHeadModeSetConfig modeSetConfig;
    struct {
        NvBool activeChanged       : 1;
        NvBool displaysChanged     : 1;
        NvBool modeChanged         : 1;
        NvBool hdrInfoFrameChanged : 1;
        NvBool colorimetryChanged  : 1;
        NvBool ilutChanged         : 1;
        NvBool olutChanged         : 1;
    } flags;

    struct NvKmsKapiCursorRequestedConfig cursorRequestedConfig;

    struct NvKmsKapiLayerRequestedConfig
        layerRequestedConfig[NVKMS_KAPI_LAYER_MAX];
};

struct NvKmsKapiRequestedModeSetConfig {
    NvU32 headsMask;
    struct NvKmsKapiHeadRequestedConfig
        headRequestedConfig[NVKMS_KAPI_MAX_HEADS];
};

struct NvKmsKapiLayerReplyConfig {
    int postSyncptFd;
};

struct NvKmsKapiHeadReplyConfig {
    struct NvKmsKapiLayerReplyConfig
        layerReplyConfig[NVKMS_KAPI_LAYER_MAX];
};

struct NvKmsKapiModeSetReplyConfig {
    enum NvKmsFlipResult flipResult;
    NvBool vrrFlip;
    NvS32 vrrSemaphoreIndex;
    struct NvKmsKapiHeadReplyConfig
        headReplyConfig[NVKMS_KAPI_MAX_HEADS];
};

struct NvKmsKapiEventDisplayChanged {
    NvKmsKapiDisplay display;
};

struct NvKmsKapiEventDynamicDisplayConnected {
    NvKmsKapiDisplay display;
};

struct NvKmsKapiEventFlipOccurred {
    NvU32 head;
    NvU32 layer;
};

struct NvKmsKapiDpyCRC32 {
    NvU32 value;
    NvBool supported;
};

struct NvKmsKapiCrcs {
    struct NvKmsKapiDpyCRC32 compositorCrc32;
    struct NvKmsKapiDpyCRC32 rasterGeneratorCrc32;
    struct NvKmsKapiDpyCRC32 outputCrc32;
};

struct NvKmsKapiEvent {
    enum NvKmsEventType type;

    struct NvKmsKapiDevice  *device;

    void *privateData;

    union {
        struct NvKmsKapiEventDisplayChanged displayChanged;
        struct NvKmsKapiEventDynamicDisplayConnected dynamicDisplayConnected;
        struct NvKmsKapiEventFlipOccurred flipOccurred;
    } u;
};

struct NvKmsKapiAllocateDeviceParams {
    /* [IN] GPU ID obtained from enumerateGpus() */
    NvU32 gpuId;

    /* [IN] Private data of device allocator */
    void *privateData;
    /* [IN] Event callback */
    void (*eventCallback)(const struct NvKmsKapiEvent *event);
};

struct NvKmsKapiDynamicDisplayParams {
    /* [IN] Display Handle returned by getDisplays() */
    NvKmsKapiDisplay handle;

    /* [OUT] Connection status */
    NvU32 connected;

    /* [OUT] VRR status */
    NvBool vrrSupported;

    /* [IN/OUT] EDID of connected monitor/ Input to override EDID */
    struct {
        NvU16  bufferSize;
        NvU8   buffer[NVKMS_KAPI_EDID_BUFFER_SIZE];
    } edid;

    /* [IN] Set true to override EDID */
    NvBool overrideEdid;

    /* [IN] Set true to force connected status */
    NvBool forceConnected;

    /* [IN] Set true to force disconnect status */
    NvBool forceDisconnected;
};

struct NvKmsKapiCreateSurfaceParams {

    /* [IN] Parameter of each plane */
    struct {
        /* [IN] Memory allocated for plane, using allocateMemory() */
        struct NvKmsKapiMemory *memory;
        /* [IN] Offsets within the memory object */
        NvU32 offset;
        /* [IN] Byte pitch of plane */
        NvU32 pitch;
    } planes[NVKMS_MAX_PLANES_PER_SURFACE];

    /* [IN] Width of the surface, in pixels */
    NvU32 width;
    /* [IN] Height of the surface, in pixels */
    NvU32 height;

    /* [IN]  The format describing number of planes and their content */
    enum NvKmsSurfaceMemoryFormat format;

    /* [IN] Whether to override the surface objects memory layout parameters
     *      with those provided here.  */
    NvBool explicit_layout;
    /* [IN] Whether the surface layout is block-linear or pitch.  Used only
     *      if explicit_layout is NV_TRUE */
    enum NvKmsSurfaceMemoryLayout layout;
    /* [IN] block-linear block height of surface.  Used only when
     *      explicit_layout is NV_TRUE and layout is
     *      NvKmsSurfaceMemoryLayoutBlockLinear */
    NvU8 log2GobsPerBlockY;
};

enum NvKmsKapiAllocationType {
    NVKMS_KAPI_ALLOCATION_TYPE_SCANOUT = 0,
    NVKMS_KAPI_ALLOCATION_TYPE_NOTIFIER = 1,
    NVKMS_KAPI_ALLOCATION_TYPE_OFFSCREEN = 2,
};

typedef enum NvKmsKapiRegisterWaiterResultRec {
    NVKMS_KAPI_REG_WAITER_FAILED,
    NVKMS_KAPI_REG_WAITER_SUCCESS,
    NVKMS_KAPI_REG_WAITER_ALREADY_SIGNALLED,
} NvKmsKapiRegisterWaiterResult;

typedef void NvKmsKapiSuspendResumeCallbackFunc(NvBool suspend);

struct NvKmsKapiFunctionsTable {

    /*!
     * NVIDIA Driver version string.
     */
    const char *versionString;

    /*!
     * System Information.
     */
    struct {
        /* Availability of write combining support for video memory */
        NvBool bAllowWriteCombining;
    } systemInfo;

    /*!
     * Enumerate the available physical GPUs that can be used with NVKMS.
     *
     * \param [out]  gpuInfo  The information of the enumerated GPUs.
     *                        It is an array of NVIDIA_MAX_GPUS elements.
     *
     * \return  Count of enumerated gpus.
     */
    NvU32 (*enumerateGpus)(nv_gpu_info_t *gpuInfo);

    /*!
     * Allocate an NVK device using which you can query/allocate resources on
     * GPU and do modeset.
     *
     * \param [in]  params  Parameters required for device allocation.
     *
     * \return  An valid device handle on success, NULL on failure.
     */
    struct NvKmsKapiDevice* (*allocateDevice)
    (
        const struct NvKmsKapiAllocateDeviceParams *params
    );

    /*!
     * Frees a device allocated by allocateDevice() and all its resources.
     *
     * \param [in]  device  A device returned by allocateDevice().
     *                      This function is a no-op if device is not valid.
     */
    void (*freeDevice)(struct NvKmsKapiDevice *device);

    /*!
     * Grab ownership of device, ownership is required to do modeset.
     *
     * \param [in]  device  A device returned by allocateDevice().
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*grabOwnership)(struct NvKmsKapiDevice *device);

    /*!
     * Release ownership of device.
     *
     * \param [in]  device  A device returned by allocateDevice().
     */
    void (*releaseOwnership)(struct NvKmsKapiDevice *device);

    /*!
     * Grant modeset permissions for a display to fd. Only one (dispIndex, head,
     * display) is currently supported.
     *
     * \param [in]  fd         fd from opening /dev/nvidia-modeset.
     *
     * \param [in]  device     A device returned by allocateDevice().
     *
     * \param [in]  head       head of display.
     *
     * \param [in]  display    The display to grant.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*grantPermissions)
    (
        NvS32 fd,
        struct NvKmsKapiDevice *device,
        NvU32 head,
        NvKmsKapiDisplay display
    );

    /*!
     * Revoke modeset permissions previously granted. Only one (dispIndex,
     * head, display) is currently supported.
     *
     * \param [in]  device     A device returned by allocateDevice().
     *
     * \param [in]  head       head of display.
     *
     * \param [in]  display    The display to revoke.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*revokePermissions)
    (
        struct NvKmsKapiDevice *device,
        NvU32 head,
        NvKmsKapiDisplay display
    );

    /*!
     * Grant modeset sub-owner permissions to fd. This is used by clients to
     * convert drm 'master' permissions into nvkms sub-owner permission.
     *
     * \param [in]  fd         fd from opening /dev/nvidia-modeset.
     *
     * \param [in]  device     A device returned by allocateDevice().
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*grantSubOwnership)
    (
        NvS32 fd,
        struct NvKmsKapiDevice *device
    );

    /*!
     * Revoke sub-owner permissions previously granted.
     *
     * \param [in]  device     A device returned by allocateDevice().
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*revokeSubOwnership)
    (
        struct NvKmsKapiDevice *device
    );

    /*!
     * Registers for notification, via
     * NvKmsKapiAllocateDeviceParams::eventCallback, of the events specified
     * in interestMask.
     *
     * This call does nothing if eventCallback is NULL when NvKmsKapiDevice
     * is allocated.
     *
     * Supported events are DPY_CHANGED and DYNAMIC_DPY_CONNECTED.
     *
     * \param [in]  device        A device returned by allocateDevice().
     *
     * \param [in]  interestMask  A mask of events requested to listen.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*declareEventInterest)
    (
        const struct NvKmsKapiDevice *device,
        const NvU32 interestMask
    );

    /*!
     * Retrieve various static resources like connector, head etc. present on
     * device and capacities.
     *
     * \param [in]      device  A device allocated using allocateDevice().
     *
     * \param [in/out]  info    A pointer to an NvKmsKapiDeviceResourcesInfo
     *                          struct that the call will fill out with number
     *                          of resources and their handles.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getDeviceResourcesInfo)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiDeviceResourcesInfo *info
    );

    /*!
     * Retrieve the number of displays on a device and an array of handles to
     * those displays.
     *
     * \param [in]      device          A device allocated using
     *                                  allocateDevice().
     *
     * \param [in/out]  displayCount    The caller should set this to the size
     *                                  of the displayHandles array it passed
     *                                  in. The function will set it to the
     *                                  number of displays returned, or the
     *                                  total number of displays on the device
     *                                  if displayHandles is NULL or array size
     *                                  of less than number of number of displays.
     *
     * \param [out]     displayHandles  An array of display handles with
     *                                  displayCount entries.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getDisplays)
    (
        struct NvKmsKapiDevice *device,
        NvU32 *numDisplays, NvKmsKapiDisplay *displayHandles
    );

    /*!
     * Retrieve information about a specified connector.
     *
     * \param [in]  device      A device allocated using allocateDevice().
     *
     * \param [in]  connector   Which connector to query, handle return by
     *                          getDeviceResourcesInfo().
     *
     * \param [out] info        A pointer to an NvKmsKapiConnectorInfo struct
     *                          that the call will fill out with information
     *                          about connector.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getConnectorInfo)
    (
        struct NvKmsKapiDevice *device,
        NvKmsKapiConnector connector, struct NvKmsKapiConnectorInfo *info
    );

    /*!
     * Retrieve information about a specified display.
     *
     * \param [in]  device    A device allocated using allocateDevice().
     *
     * \param [in]  display   Which connector to query, handle return by
     *                        getDisplays().
     *
     * \param [out] info      A pointer to an NvKmsKapiStaticDisplayInfo struct
     *                        that the call will fill out with information
     *                        about display.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getStaticDisplayInfo)
    (
        struct NvKmsKapiDevice *device,
        NvKmsKapiDisplay display, struct NvKmsKapiStaticDisplayInfo *info
    );

    /*!
     * Detect/force connection status/EDID of display.
     *
     * \param [in/out]  params    Parameters containing display
     *                            handle, EDID and flags to force connection
     *                            status.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getDynamicDisplayInfo)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiDynamicDisplayParams *params
    );

    /*!
     * Allocate some unformatted video memory of the specified size.
     *
     * This function allocates video memory on the specified GPU.
     * It should be suitable for mapping on the CPU as a pitch
     * linear or block-linear surface.
     *
     * \param [in] device  A device allocated using allocateDevice().
     *
     * \param [in] layout  BlockLinear or Pitch.
     * 
     * \param [in] type    Allocation type.
     *
     * \param [in] size    Size, in bytes, of the memory to allocate.
     *
     * \param [in/out] compressible For input, non-zero if compression
     *                              backing store should be allocated for
     *                              the memory, for output, non-zero if
     *                              compression backing store was
     *                              allocated for the memory.
     *
     * \return An valid memory handle on success, NULL on failure.
     */
    struct NvKmsKapiMemory* (*allocateVideoMemory)
    (
        struct NvKmsKapiDevice *device,
        enum NvKmsSurfaceMemoryLayout layout,
        enum NvKmsKapiAllocationType type,
        NvU64 size,
        NvU8 *compressible
    );

    /*!
     * Allocate some unformatted system memory of the specified size.
     *
     * This function allocates system memory . It should be suitable
     * for mapping on the CPU as a pitch linear or block-linear surface.
     *
     * \param [in] device  A device allocated using allocateDevice().
     *
     * \param [in] layout  BlockLinear or Pitch.
     * 
     * \param [in] type    Allocation type.
     *
     * \param [in] size    Size, in bytes, of the memory to allocate.
     *
     * \param [in/out] compressible For input, non-zero if compression
     *                              backing store should be allocated for
     *                              the memory, for output, non-zero if
     *                              compression backing store was
     *                              allocated for the memory.
     *
     * \return An valid memory handle on success, NULL on failure.
     */
    struct NvKmsKapiMemory* (*allocateSystemMemory)
    (
        struct NvKmsKapiDevice *device,
        enum NvKmsSurfaceMemoryLayout layout,
        enum NvKmsKapiAllocationType type,
        NvU64 size,
        NvU8 *compressible
    );

    /*!
     * Import some unformatted memory of the specified size.
     *
     * This function accepts a driver-specific parameter structure representing
     * memory allocated elsewhere and imports it to a NVKMS KAPI memory object
     * of the specified size.
     *
     * \param [in] device  A device allocated using allocateDevice().  The
     *                     memory being imported must have been allocated
     *                     against the same physical device this device object
     *                     represents.
     *
     * \param [in] size    Size, in bytes, of the memory being imported.
     *
     * \param [in] nvKmsParamsUser Userspace pointer to driver-specific
     *                             parameters describing the memory object being
     *                             imported.
     *
     * \param [in] nvKmsParamsSize Size of the driver-specific parameter struct.
     *
     * \return A valid memory handle on success, NULL on failure.
     */
    struct NvKmsKapiMemory* (*importMemory)
    (
        struct NvKmsKapiDevice *device, NvU64 size,
        NvU64 nvKmsParamsUser,
        NvU64 nvKmsParamsSize
    );

    /*!
     * Duplicate an existing NVKMS KAPI memory object, taking a reference on the
     * underlying memory.
     *
     * \param [in] device    A device allocated using allocateDevice().  The
     *                       memory being imported need not have been allocated
     *                       against the same physical device this device object
     *                       represents.
     *
     * \param [in] srcDevice The device associated with srcMemory.
     *
     * \param [in] srcMemory The memory object to duplicate.
     *
     * \return A valid memory handle on success, NULL on failure.
     */
    struct NvKmsKapiMemory* (*dupMemory)
    (
        struct NvKmsKapiDevice *device,
        const struct NvKmsKapiDevice *srcDevice,
        const struct NvKmsKapiMemory *srcMemory
    );

    /*!
     * Export the specified memory object to a userspace object handle.
     *
     * This function accepts a driver-specific parameter structure representing
     * a new handle to be assigned to an existing NVKMS KAPI memory object.
     *
     * \param [in] device  A device allocated using allocateDevice().  The
     *                     memory being exported must have been created against
     *                     or imported to the same device object, and the
     *                     destination object handle must be valid for this
     *                     device as well.
     *
     * \param [in] memory  The memory object to export.
     *
     * \param [in] nvKmsParamsUser Userspace pointer to driver-specific
     *                             parameters specifying a handle to add to the
     *                             memory object being exported.
     *
     * \param [in] nvKmsParamsSize Size of the driver-specific parameter struct.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*exportMemory)
    (
        const struct NvKmsKapiDevice *device,
        const struct NvKmsKapiMemory *memory,
        NvU64 nvKmsParamsUser,
        NvU64 nvKmsParamsSize
    );

    /*!
     * Free memory allocated using allocateMemory()
     *
     * \param [in] device  A device allocated using allocateDevice().
     *
     * \param [in] memory  Memory allocated using allocateMemory().
     *
     * \return NV_TRUE on success, NV_FALSE if memory is in use.
     */
    void (*freeMemory)
    (
        struct NvKmsKapiDevice *device, struct NvKmsKapiMemory *memory
    );

    /*!
     * Create MMIO mappings for a memory object allocated using
     * allocateMemory().
     *
     * \param [in]  device           A device allocated using allocateDevice().
     *
     * \param [in]  memory           Memory allocated using allocateMemory()
     *
     * \param [in]  type             Userspace or kernelspace mapping
     *
     * \param [out] ppLinearAddress  The MMIO address where memory object is
     *                               mapped.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*mapMemory)
    (
        const struct NvKmsKapiDevice *device,
        const struct NvKmsKapiMemory *memory, NvKmsKapiMappingType type,
        void **ppLinearAddress
    );

    /*!
     * Destroy MMIO mappings created for a memory object allocated using
     * allocateMemory().
     *
     * \param [in]  device           A device allocated using allocateDevice().
     *
     * \param [in]  memory           Memory allocated using allocateMemory()
     *
     * \param [in]  type             Userspace or kernelspace mapping
     *
     * \param [in]  pLinearAddress   The MMIO address return by mapMemory()
     */
    void (*unmapMemory)
    (
        const struct NvKmsKapiDevice *device,
        const struct NvKmsKapiMemory *memory, NvKmsKapiMappingType type,
        const void *pLinearAddress
    );

    /*!
     * Create a formatted surface from an NvKmsKapiMemory object.
     *
     * \param [in]  device  A device allocated using allocateDevice().
     *
     * \param [in]  params  Parameters to the surface creation.
     *
     * \return  An valid surface handle on success.  NULL on failure.
     */
    struct NvKmsKapiSurface* (*createSurface)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiCreateSurfaceParams *params
    );

    /*!
     * Destroy a surface created by createSurface().
     *
     * \param [in]  device   A device allocated using allocateDevice().
     *
     * \param [in]  surface  A surface created using createSurface()
     */
    void (*destroySurface)
    (
        struct NvKmsKapiDevice *device, struct NvKmsKapiSurface *surface
    );

    /*!
     * Enumerate the mode timings available on a given display.
     *
     * \param [in]   device     A device allocated using allocateDevice().
     *
     * \param [in]   display    A display handle returned by  getDisplays().
     *
     * \param [in]   modeIndex  A mode index (Any integer >= 0).
     *
     * \param [out]  mode       A pointer to an NvKmsKapiDisplayMode struct that
     *                          the call will fill out with mode-timings of mode
     *                          at index modeIndex.
     *
     * \param [out]  valid      Returns TRUE in this param if mode-timings of
     *                          mode at index modeIndex are valid on display.
     *
     * \param [out]  preferredMode  Returns TRUE if this mode is marked as
     *                              "preferred" by the EDID.
     *
     * \return Value >= 1 if more modes are available, 0 if no more modes are
     *         available, and Value < 0 on failure.
     */
    int (*getDisplayMode)
    (
        struct NvKmsKapiDevice *device,
        NvKmsKapiDisplay display, NvU32 modeIndex,
        struct NvKmsKapiDisplayMode *mode, NvBool *valid,
        NvBool *preferredMode
    );

    /*!
     * Validate given mode timings available on a given display.
     *
     * \param [in]  device   A device allocated using allocateDevice().
     *
     * \param [in]  display  A display handle returned by  getDisplays().
     *
     * \param [in]  mode     A pointer to an NvKmsKapiDisplayMode struct that
     *                       filled with mode-timings to validate.
     *
     * \return NV_TRUE if mode-timings are valid, NV_FALSE on failure.
     */
    NvBool (*validateDisplayMode)
    (
        struct NvKmsKapiDevice *device,
        NvKmsKapiDisplay display, const struct NvKmsKapiDisplayMode *mode
    );

    /*!
     * Apply a mode configuration to the device.
     *
     * Client can describe damaged part of configuration but still it is must
     * to describe entire configuration.
     *
     * \param [in]  device            A device allocated using allocateDevice().
     *
     * \param [in]  requestedConfig   Parameters describing a device-wide
     *                                display configuration.
     *
     * \param [in]  commit            If set to 0 them call will only validate
     *                                mode configuration, will not apply it.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*applyModeSetConfig)
    (
        struct NvKmsKapiDevice *device,
        const struct NvKmsKapiRequestedModeSetConfig *requestedConfig,
        struct NvKmsKapiModeSetReplyConfig *replyConfig,
        const NvBool commit
    );

    /*!
     * Return status of flip.
     *
     * \param  [in]  device   A device allocated using allocateDevice().
     *
     * \param  [in]  head     A head returned by getDeviceResourcesInfo().
     *
     * \param  [in]  layer    A layer index.
     *
     * \param  [out] pending  Return TRUE if head has pending flip for
     *                        given layer.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getFlipPendingStatus)
    (
        const struct NvKmsKapiDevice *device,
        const NvU32 head,
        const NvU32 layer,
        NvBool *pending
    );

    /*!
     * Allocate an event callback.
     *
     * \param [in]  device          A device allocated using allocateDevice().
     *
     * \param [in]  proc            Function pointer to call when triggered.
     *
     * \param [in]  data            Argument to pass into function.
     *
     * \param [in] nvKmsParamsUser  Userspace pointer to driver-specific
     *                              parameters describing the event callback
     *                              being created.
     *
     * \param [in] nvKmsParamsSize  Size of the driver-specific parameter struct.
     *
     * \return struct NvKmsKapiChannelEvent* on success, NULL on failure.
     */
    struct NvKmsKapiChannelEvent* (*allocateChannelEvent)
    (
        struct NvKmsKapiDevice *device,
        NvKmsChannelEventProc *proc,
        void *data,
        NvU64 nvKmsParamsUser,
        NvU64 nvKmsParamsSize
    );

    /*!
     * Free an event callback.
     *
     * \param [in]  device  A device allocated using allocateDevice().
     *
     * \param [in]  cb      struct NvKmsKapiChannelEvent* returned from
     *                      allocateChannelEvent()
     */
    void (*freeChannelEvent)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiChannelEvent *cb
    );

    /*!
     * Get 32-bit CRC value for the last contents presented on the specified
     * head.
     *
     * \param [in]  device  A device allocated using allocateDevice().
     *
     * \param [in]  head    A head returned by getDeviceResourcesInfo().
     *
     * \param [out] crc32   The CRC32 generated from the content currently
     *                      presented onto the given head
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getCRC32)
    (
        struct NvKmsKapiDevice *device,
        NvU32 head,
        struct NvKmsKapiCrcs *crc32
    );

     /*!
     * Get the list allocation pages corresponding to the specified memory object.
     *
     * \param [in]  device  A device allocated using allocateDevice().
     *
     * \param [in]  memory  The memory object for which we need to find the
     *                      list of allocation pages and number of pages.
     *
     * \param [out] pPages  A pointer to the list of NvU64 pointers. Caller
     *                      should free pPages on success using freeMemoryPages().
     *
     * \param [out] pNumPages It gives the total number of NvU64 pointers
     *                        returned in pPages.
     *
     * \return NV_TRUE on success, NV_FALSE on failure.
     */
    NvBool (*getMemoryPages)
    (
        const struct NvKmsKapiDevice *device,
        const struct NvKmsKapiMemory *memory,
        NvU64 **pPages,
        NvU32 *pNumPages
    );

    /*!
     * Free the list of allocation pages returned by getMemoryPages()
     *
     * \param [in] pPages  A list of NvU64 pointers allocated by getMemoryPages().
     *
     */
    void (*freeMemoryPages)
    (
        NvU64 *pPages
    );

     /*!
     * Check if this memory object can be scanned out for display.
     *
     * \param [in]  device  A device allocated using allocateDevice().
     *
     * \param [in]  memory  The memory object to check for display support.
     *
     * \return NV_TRUE if this memory can be displayed, NV_FALSE if not.
     */
    NvBool (*isMemoryValidForDisplay)
    (
        const struct NvKmsKapiDevice *device,
        const struct NvKmsKapiMemory *memory
    );

    /*
     * Import SGT as a memory handle.
     *
     * \param [in] device  A device allocated using allocateDevice().
     *
     * \param [in] sgt     SGT pointer.
     * \param [in] gem     GEM pointer that pinned SGT, to be refcounted.
     *
     * \param [in] limit   Size, in bytes, of the memory backed by the SGT.
     *
     * \return A valid memory handle on success, NULL on failure.
     */
    struct NvKmsKapiMemory*
    (*getSystemMemoryHandleFromSgt)(struct NvKmsKapiDevice *device,
                                    NvP64 sgt,
                                    NvP64 gem,
                                    NvU32 limit);

    /*
     * Import dma-buf in the memory handle.
     *
     * \param [in] device  A device allocated using allocateDevice().
     *
     * \param [in] dmaBuf  DMA-BUF pointer.
     *
     * \param [in] limit   Size, in bytes, of the dma-buf.
     *
     * \return An valid memory handle on success, NULL on failure.
     */
    struct NvKmsKapiMemory*
    (*getSystemMemoryHandleFromDmaBuf)(struct NvKmsKapiDevice *device,
                                       NvP64 dmaBuf,
                                       NvU32 limit);

    /*!
     * Import a semaphore surface allocated elsewhere to NVKMS and return a
     * handle to the new object.
     *
     * \param [in] device            A device allocated using allocateDevice().
     *
     * \param [in] nvKmsParamsUser   Userspace pointer to driver-specific
     *                               parameters describing the semaphore
     *                               surface being imported.
     *
     * \param [in] nvKmsParamsSize   Size of the driver-specific parameter
     *                               struct.
     *
     * \param [out] pSemaphoreMap    Returns a CPU mapping of the semaphore
     *                               surface's semaphore memory to the client.
     *
     * \param [out] pMaxSubmittedMap Returns a CPU mapping of the semaphore
     *                               surface's semaphore memory to the client.
     *
     * \return struct NvKmsKapiSemaphoreSurface* on success, NULL on failure.
     */
    struct NvKmsKapiSemaphoreSurface* (*importSemaphoreSurface)
    (
         struct NvKmsKapiDevice *device,
         NvU64 nvKmsParamsUser,
         NvU64 nvKmsParamsSize,
         void **pSemaphoreMap,
         void **pMaxSubmittedMap
    );

    /*!
     * Free an imported semaphore surface.
     *
     * \param [in]  device              The device passed to
     *                                  importSemaphoreSurface() when creating
     *                                  semaphoreSurface.
     *
     * \param [in]  semaphoreSurface    A semaphore surface returned by
     *                                  importSemaphoreSurface().
     */
    void (*freeSemaphoreSurface)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiSemaphoreSurface *semaphoreSurface
    );

    /*!
     * Register a callback to be called when a semaphore reaches a value.
     *
     * The callback will be called when the semaphore at index in
     * semaphoreSurface reaches the value wait_value.  The callback will
     * be called at most once and is automatically unregistered when called.
     * It may also be unregistered (i.e., cancelled) explicitly using the
     * unregisterSemaphoreSurfaceCallback() function. To avoid leaking the
     * memory used to track the registered callback, callers must ensure one
     * of these methods of unregistration is used for every successful
     * callback registration that returns a non-NULL pCallbackHandle.
     *
     * \param [in]  device              The device passed to
     *                                  importSemaphoreSurface() when creating
     *                                  semaphoreSurface.
     *
     * \param [in]  semaphoreSurface    A semaphore surface returned by
     *                                  importSemaphoreSurface().
     *
     * \param [in]  pCallback           A pointer to the function to call when
     *                                  the specified value is reached. NULL
     *                                  means no callback.
     *
     * \param [in]  pData               Arbitrary data to be passed back to the
     *                                  callback as its sole parameter.
     *
     * \param [in]  index               The index of the semaphore within
     *                                  semaphoreSurface.
     *
     * \param [in]  wait_value          The value the semaphore must reach or
     *                                  exceed before the callback is called.
     *
     * \param [in]  new_value           The value the semaphore will be set to
     *                                  when it reaches or exceeds <wait_value>.
     *                                  0 means do not update the value.
     *
     * \param [out] pCallbackHandle     On success, the value pointed to will
     *                                  contain an opaque handle to the
     *                                  registered callback that may be used to
     *                                  cancel it if needed. Unused if pCallback
     *                                  is NULL.
     *
     * \return NVKMS_KAPI_REG_WAITER_SUCCESS if the waiter was registered or if
     *         no callback was requested and the semaphore at <index> has
     *         already reached or exceeded <wait_value>
     *
     *         NVKMS_KAPI_REG_WAITER_ALREADY_SIGNALLED if a callback was
     *         requested and the semaphore at <index> has already reached or
     *         exceeded <wait_value>
     *
     *         NVKMS_KAPI_REG_WAITER_FAILED if waiter registration failed.
     */
    NvKmsKapiRegisterWaiterResult
    (*registerSemaphoreSurfaceCallback)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
        NvKmsSemaphoreSurfaceCallbackProc *pCallback,
        void *pData,
        NvU64 index,
        NvU64 wait_value,
        NvU64 new_value,
        struct NvKmsKapiSemaphoreSurfaceCallback **pCallbackHandle
    );

    /*!
     * Unregister a callback registered via registerSemaphoreSurfaceCallback()
     *
     * If the callback has not yet been called, this function will cancel the
     * callback and free its associated resources.
     *
     * Note this function treats the callback handle as a pointer. While this
     * function does not dereference that pointer itself, the underlying call
     * to RM does within a properly guarded critical section that first ensures
     * it is not in the process of being used within a callback. This means
     * the callstack must take into consideration that pointers are not in
     * general unique handles if they may have been freed, since a subsequent
     * malloc could return the same pointer value at that point. This callchain
     * avoids that by leveraging the behavior of the underlying RM APIs:
     *
     * 1) A callback handle is referenced relative to its corresponding
     *    (semaphore surface, index, wait_value) tuple here and within RM. It
     *    is not a valid handle outside of that scope.
     *
     * 2) A callback can not be registered against an already-reached value
     *    for a given semaphore surface index.
     *
     * 3) A given callback handle can not be registered twice against the same
     *    (semaphore surface, index, wait_value) tuple, so unregistration will
     *    never race with registration at the RM level, and would only race at
     *    a higher level if used incorrectly. Since this is kernel code, we
     *    can safely assume there won't be malicious clients purposely misuing
     *    the API, but the burden is placed on the caller to ensure its usage
     *    does not lead to races at higher levels.
     *
     * These factors considered together ensure any valid registered handle is
     * either still in the relevant waiter list and refers to the same event/
     * callback as when it was registered, or has been removed from the list
     * as part of a critical section that also destroys the list itself and
     * makes future lookups in that list impossible, and hence eliminates the
     * chance of comparing a stale handle with a new handle of the same value
     * as part of a lookup.
     *
     * \param [in]  device              The device passed to
     *                                  importSemaphoreSurface() when creating
     *                                  semaphoreSurface.
     *
     * \param [in]  semaphoreSurface    The semaphore surface passed to
     *                                  registerSemaphoreSurfaceCallback() when
     *                                  registering the callback.
     *
     * \param [in]  index               The index passed to
     *                                  registerSemaphoreSurfaceCallback() when
     *                                  registering the callback.
     *
     * \param [in]  wait_value          The wait_value passed to
     *                                  registerSemaphoreSurfaceCallback() when
     *                                  registering the callback.
     *
     * \param [in]  callbackHandle      The callback handle returned by
     *                                  registerSemaphoreSurfaceCallback().
     */
    NvBool
    (*unregisterSemaphoreSurfaceCallback)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
        NvU64 index,
        NvU64 wait_value,
        struct NvKmsKapiSemaphoreSurfaceCallback *callbackHandle
    );

    /*!
     * Update the value of a semaphore surface from the CPU.
     *
     * Update the semaphore value at the specified index from the CPU, then
     * wake up any pending CPU waiters associated with that index that are
     * waiting on it reaching a value <= the new value.
     */
    NvBool
    (*setSemaphoreSurfaceValue)
    (
        struct NvKmsKapiDevice *device,
        struct NvKmsKapiSemaphoreSurface *semaphoreSurface,
        NvU64 index,
        NvU64 new_value
    );

    /*!
     * Set the callback function for suspending and resuming the display system.
     */
    void
    (*setSuspendResumeCallback)
    (
        NvKmsKapiSuspendResumeCallbackFunc *function
    );

    /*!
     * Immediately initialize the specified display semaphore to the pending state.
     *
     * Must be called prior to applying a mode set that utilizes the specified
     * display semaphore for synchronization.
     *
     * \param [in] device         The device which will utilize the semaphore.
     *
     * \param [in] semaphoreIndex Index of the desired semaphore within the
     *                            NVKMS semaphore pool. Must be less than
     *                            NvKmsKapiDeviceResourcesInfo::caps::numDisplaySemaphores
     *                            for the specified device.
     */
    NvBool
    (*tryInitDisplaySemaphore)
    (
        struct NvKmsKapiDevice *device,
        NvU32 semaphoreIndex
    );

    /*!
     * Immediately set the specified display semaphore to the displayable state.
     *
     * Must be called after \ref tryInitDisplaySemaphore to indicate a mode
     * configuration change that utilizes the specified display semaphore for
     * synchronization may proceed.
     *
     * \param [in] device         The device which will utilize the semaphore.
     *
     * \param [in] semaphoreIndex Index of the desired semaphore within the
     *                            NVKMS semaphore pool. Must be less than
     *                            NvKmsKapiDeviceResourcesInfo::caps::numDisplaySemaphores
     *                            for the specified device.
     */
    void
    (*signalDisplaySemaphore)
    (
        struct NvKmsKapiDevice *device,
        NvU32 semaphoreIndex
    );

    /*!
     * Immediately cancel use of a display semaphore by resetting its value to
     * its initial state.
     *
     * This can be used by clients to restore a semaphore to a consistent state
     * when they have prepared it for use by previously calling
     * \ref tryInitDisplaySemaphore() on it, but are then prevented from
     * submitting the associated hardware operations to consume it due to the
     * subsequent failure of some software or hardware operation.
     *
     * \param [in] device         The device which will utilize the semaphore.
     *
     * \param [in] semaphoreIndex Index of the desired semaphore within the
     *                            NVKMS semaphore pool. Must be less than
     *                            NvKmsKapiDeviceResourcesInfo::caps::numDisplaySemaphores
     *                            for the specified device.
     */
    void
    (*cancelDisplaySemaphore)
    (
        struct NvKmsKapiDevice *device,
        NvU32 semaphoreIndex
    );

    /*!
     * Signal the VRR semaphore at the specified index from the CPU.
     * If device does not support VRR semaphores, this is a no-op.
     * Returns true if signal is success or no-op, otherwise returns false.
     *
     * \param [in]  device  A device allocated using allocateDevice().
     *
     * \param [in]  index   The VRR semaphore index to be signalled.
     */
    NvBool
    (*signalVrrSemaphore)
    (
        struct NvKmsKapiDevice *device,
        NvS32 index
    );
};

/** @} */

/**
 * \defgroup Functions
 * @{
 */

NvBool nvKmsKapiGetFunctionsTable
(
    struct NvKmsKapiFunctionsTable *funcsTable
);

/** @} */

#endif /* defined(__NVKMS_KAPI_H__) */
