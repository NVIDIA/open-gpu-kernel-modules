/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if !defined(NVKMS_API_H)
#define NVKMS_API_H

/*
 * NVKMS API
 *
 *
 * All file operations described in this header file go through a
 * single device file that has system-wide scope.  The individual
 * ioctl request data structures specify the objects to which the
 * request is targeted.
 *
 *
 * OBJECTS
 *
 * The NVKMS API is organized into several objects:
 *
 * - A device, which corresponds to an RM device.  This can either be
 *   a single GPU, or multiple GPUs linked into SLI.  Each GPU is
 *   called a "subdevice".  The subdevices used by an NVKMS device are
 *   reported in NvKmsAllocDeviceReply::subDeviceMask.
 *
 *   A device is specified by a deviceHandle, returned by
 *   NVKMS_IOCTL_ALLOC_DEVICE.
 *
 * - A disp, which represents an individually programmable display
 *   engine of a GPU.  In SLI Mosaic, there is one disp per physical
 *   GPU.  In all other configurations there is one disp for the
 *   entire device.  A disp is specified by a (deviceHandle,
 *   dispHandle) duple.  A dispHandle is only unique within a single
 *   device: multiple devices may have disps with the same dispHandle
 *   value.
 *
 *   A disp represents one subdevice; disp index N corresponds to subdevice
 *   index N.
 *
 * - A connector, which represents an electrical connection to the
 *   GPU.  E.g., a physical DVI-I connector has two NVKMS connector
 *   objects (a VGA NVKMS connector and a TMDS NVKMS connector).
 *   However, a physical DisplayPort connector has one NVKMS connector
 *   object, even if there is a tree of DisplayPort1.2 Multistream
 *   monitors connected to it.
 *
 *   Connectors are associated with a specific disp.  A connector is
 *   specified by a (deviceHandle, dispHandle, connectorHandle)
 *   triplet.  A connectorHandle is only unique within a single disp:
 *   multiple disps may have connectors with the same connectorHandle
 *   value.
 *
 * - A dpy, which represents a connection of a display device to the
 *   system.  Multiple dpys can map to the same connector in the case
 *   of DisplayPort1.2 MultiStream.  A dpy is specified by a
 *   (deviceHandle, dispHandle, dpyId) triplet.  A dpyId is only
 *   unique within a single disp: multiple disps may have dpys with
 *   the same dpyId value.
 *
 * - A surface, which represents memory to be scanned out.  Surfaces
 *   should be allocated by resman, and then registered and
 *   unregistered with NVKMS.  The NvKmsSurfaceHandle value of 0 is
 *   reserved to mean no surface.
 *
 * NVKMS clients should treat the device, disp, connector, and surface
 * handles as opaque values.  They are specific to the file descriptor
 * through which a client allocated and queried them.  Dpys should
 * also be treated as opaque, though they can be passed between
 * clients.
 *
 * NVKMS clients initialize NVKMS by allocating an NVKMS device.  The
 * device can either be a single GPU, or an SLI group.  It is expected
 * that the client has already attached/linked the GPUs through
 * resman and created a resman device.
 *
 * NVKMS device allocation returns a device handle, the disp handles,
 * and capabilities of the device.
 *
 *
 * MODE VALIDATION
 *
 * When a client requests to set a mode via NVKMS_IOCTL_SET_MODE,
 * NVKMS will validate the mode at that point in time, honoring the
 * NvKmsModeValidationParams specified as part of the request.
 *
 * Clients can use NVKMS_IOCTL_VALIDATE_MODE to test if a mode is valid.
 *
 * Clients can use NVKMS_IOCTL_VALIDATE_MODE_INDEX to get the list of
 * modes that NVKMS currently considers valid for the dpy (modes from
 * the EDID, etc).
 *
 * IMPLEMENTATION NOTE: the same mode validation common code will be
 * used in each of NVKMS_IOCTL_SET_MODE, NVKMS_IOCTL_VALIDATE_MODE,
 * and NVKMS_IOCTL_VALIDATE_MODE_INDEX, but NVKMS won't generally maintain
 * a "mode pool" with an exhaustive list of the allowable modes for a
 * dpy.
 *
 *
 * DYNAMIC DPY HANDLING
 *
 * Dynamic dpys (namely, DisplayPort multistream dpys) share the NVDpyId
 * namespace with non-dynamic dpys on the same disp.  However, dynamic dpys will
 * not be listed in NvKmsQueryDispReply::validDpys.  Instead, dynamic dpys are
 * added and removed from the system dynamically.
 *
 * When a dynamic dpy is first connected, NVKMS will allocate a new NVDpyId for
 * it and generate an NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED event.  When the
 * dynamic dpy is disconnected, NVKMS will generate an
 * NVKMS_EVENT_TYPE_DYNAMIC_DPY_DISCONNECTED event.  Whether the corresponding
 * NVDpyId is immediately freed and made available for subsequent dynamic dpys
 * depends on client behavior.
 *
 * Clients may require that a dynamic NVDpyId persist even after the dynamic dpy
 * is disconnected.  Clients who require this can use
 * NVKMS_IOCTL_DECLARE_DYNAMIC_DPY_INTEREST.  NVKMS will retain the NVDpyId
 * until the dynamic dpy is disconnected and there are no clients who have
 * declared "interest" on the particular dynamic dpy.  While the NVDpyId
 * persists, it will be used for any monitor that is connected at the same
 * dynamic dpy address (i.e., port address, in the case of DP MST).
 *
 *
 * FILE DESCRIPTOR HANDLING
 *
 * With the exception of NVDpyIds, all handles should be assumed to be
 * specific to the current file descriptor on which the ioctls are
 * performed.
 *
 * Multiple devices can be allocated on the same file descriptor.
 * E.g., to drive the display of multiple GPUs.
 *
 * If a file descriptor is closed prematurely, either explicitly by
 * the client or implicitly by the operating system because the client
 * process was terminated, NVKMS will perform an
 * NVKMS_IOCTL_FREE_DEVICE for any devices currently allocated by the
 * client on the closed file descriptor.
 *
 * NVKMS file descriptors are normally used as the first argument of
 * ioctl(2).  However, NVKMS file descriptors are also used for
 * granting surfaces (see NVKMS_IOCTL_GRANT_SURFACE) or permissions
 * (see NVKMS_IOCTL_GRANT_PERMISSIONS).  Any given NVKMS file
 * descriptor can only be used for one of these uses.
 *
 * QUESTIONS:
 *
 * - Is there any reason for errors to be returned through a status field
 *   in the Param structures, rather than the ioctl(2) return value?
 *
 * - Is it too asymmetric that NVKMS_IOCTL_SET_MODE can set a
 *   mode across heads/disps, but other requests (e.g.,
 *   NVKMS_IOCTL_SET_CURSOR_IMAGE) operate on a single head?
 *
 *
 * IOCTL PARAMETER ORGANIZATION
 *
 * For table-driven processing of ioctls, it is useful for all ioctl
 * parameters to follow the same convention:
 *
 *   struct NvKmsFooRequest {
 *       (...)
 *   };
 *
 *   struct NvKmsFooReply {
 *       (...)
 *   };
 *
 *   struct NvKmsFooParams {
 *       struct NvKmsFooRequest request; //! in
 *       struct NvKmsFooReply reply;     //! out
 *   };
 *
 * I.e., all ioctl parameter structures NvKmsFooParams should have
 * "request" and "reply" fields, with types "struct NvKmsFooRequest"
 * and "struct NvKmsFooReply".  C doesn't technically support empty
 * structures, so the convention is to place a "padding" NvU32 in
 * request or reply structures that would otherwise be empty.
 */
#include "nvtypes.h"
#include "nvlimits.h"
#include "nv_mig_types.h"
#include "nv_dpy_id.h"
#include "nv_mode_timings.h"
#include "nvkms-api-types.h"
#include "nvgputypes.h" /* NvGpuSemaphore */
#include "nvkms-format.h"

/*
 * The NVKMS ioctl commands.  See the ioctl parameter declarations
 * later in this header file for an explanation of each ioctl command.
 */
enum NvKmsIoctlCommand {
    NVKMS_IOCTL_ALLOC_DEVICE,
    NVKMS_IOCTL_FREE_DEVICE,
    NVKMS_IOCTL_QUERY_DISP,
    NVKMS_IOCTL_QUERY_CONNECTOR_STATIC_DATA,
    NVKMS_IOCTL_QUERY_CONNECTOR_DYNAMIC_DATA,
    NVKMS_IOCTL_QUERY_DPY_STATIC_DATA,
    NVKMS_IOCTL_QUERY_DPY_DYNAMIC_DATA,
    NVKMS_IOCTL_VALIDATE_MODE_INDEX,
    NVKMS_IOCTL_VALIDATE_MODE,
    NVKMS_IOCTL_SET_MODE,
    NVKMS_IOCTL_SET_CURSOR_IMAGE,
    NVKMS_IOCTL_MOVE_CURSOR,
    NVKMS_IOCTL_SET_LUT,
    NVKMS_IOCTL_CHECK_LUT_NOTIFIER,
    NVKMS_IOCTL_IDLE_BASE_CHANNEL,
    NVKMS_IOCTL_FLIP,
    NVKMS_IOCTL_DECLARE_DYNAMIC_DPY_INTEREST,
    NVKMS_IOCTL_REGISTER_SURFACE,
    NVKMS_IOCTL_UNREGISTER_SURFACE,
    NVKMS_IOCTL_GRANT_SURFACE,
    NVKMS_IOCTL_ACQUIRE_SURFACE,
    NVKMS_IOCTL_RELEASE_SURFACE,
    NVKMS_IOCTL_SET_DPY_ATTRIBUTE,
    NVKMS_IOCTL_GET_DPY_ATTRIBUTE,
    NVKMS_IOCTL_GET_DPY_ATTRIBUTE_VALID_VALUES,
    NVKMS_IOCTL_SET_DISP_ATTRIBUTE,
    NVKMS_IOCTL_GET_DISP_ATTRIBUTE,
    NVKMS_IOCTL_GET_DISP_ATTRIBUTE_VALID_VALUES,
    NVKMS_IOCTL_QUERY_FRAMELOCK,
    NVKMS_IOCTL_SET_FRAMELOCK_ATTRIBUTE,
    NVKMS_IOCTL_GET_FRAMELOCK_ATTRIBUTE,
    NVKMS_IOCTL_GET_FRAMELOCK_ATTRIBUTE_VALID_VALUES,
    NVKMS_IOCTL_GET_NEXT_EVENT,
    NVKMS_IOCTL_DECLARE_EVENT_INTEREST,
    NVKMS_IOCTL_CLEAR_UNICAST_EVENT,
    NVKMS_IOCTL_GET_3DVISION_DONGLE_PARAM_BYTES,
    NVKMS_IOCTL_SET_3DVISION_AEGIS_PARAMS,
    NVKMS_IOCTL_SET_LAYER_POSITION,
    NVKMS_IOCTL_GRAB_OWNERSHIP,
    NVKMS_IOCTL_RELEASE_OWNERSHIP,
    NVKMS_IOCTL_GRANT_PERMISSIONS,
    NVKMS_IOCTL_ACQUIRE_PERMISSIONS,
    NVKMS_IOCTL_REVOKE_PERMISSIONS,
    NVKMS_IOCTL_QUERY_DPY_CRC32,
    NVKMS_IOCTL_REGISTER_DEFERRED_REQUEST_FIFO,
    NVKMS_IOCTL_UNREGISTER_DEFERRED_REQUEST_FIFO,
    NVKMS_IOCTL_ALLOC_SWAP_GROUP,
    NVKMS_IOCTL_FREE_SWAP_GROUP,
    NVKMS_IOCTL_JOIN_SWAP_GROUP,
    NVKMS_IOCTL_LEAVE_SWAP_GROUP,
    NVKMS_IOCTL_SET_SWAP_GROUP_CLIP_LIST,
    NVKMS_IOCTL_GRANT_SWAP_GROUP,
    NVKMS_IOCTL_ACQUIRE_SWAP_GROUP,
    NVKMS_IOCTL_RELEASE_SWAP_GROUP,
    NVKMS_IOCTL_SWITCH_MUX,
    NVKMS_IOCTL_GET_MUX_STATE,
    NVKMS_IOCTL_EXPORT_VRR_SEMAPHORE_SURFACE,
    NVKMS_IOCTL_ENABLE_VBLANK_SYNC_OBJECT,
    NVKMS_IOCTL_DISABLE_VBLANK_SYNC_OBJECT,
    NVKMS_IOCTL_NOTIFY_VBLANK,
    NVKMS_IOCTL_SET_FLIPLOCK_GROUP,
    NVKMS_IOCTL_ENABLE_VBLANK_SEM_CONTROL,
    NVKMS_IOCTL_DISABLE_VBLANK_SEM_CONTROL,
    NVKMS_IOCTL_ACCEL_VBLANK_SEM_CONTROLS,
    NVKMS_IOCTL_VRR_SIGNAL_SEMAPHORE,
    NVKMS_IOCTL_FRAMEBUFFER_CONSOLE_DISABLED,
};


#define NVKMS_NVIDIA_DRIVER_VERSION_STRING_LENGTH                     32
#define NVKMS_MAX_CONNECTORS_PER_DISP                                 16
#define NVKMS_MAX_GPUS_PER_FRAMELOCK                                  4
#define NVKMS_MAX_DEVICE_REGISTRY_KEYS                                16
#define NVKMS_MAX_DEVICE_REGISTRY_KEYNAME_LEN                         32
#define NVKMS_MAX_VBLANK_SYNC_OBJECTS_PER_HEAD                        6


/*
 * There can be at most one SwapGroup per-head, per-disp (and,
 * in the extreme, there is one disp per-GPU).
 */
#define NVKMS_MAX_SWAPGROUPS (NVKMS_MAX_HEADS_PER_DISP * NV_MAX_DEVICES)

#define NVKMS_MAX_VALID_SYNC_RANGES                                   8

#define NVKMS_DPY_NAME_SIZE                                           128
#define NVKMS_GUID_SIZE                                               16
#define NVKMS_3DVISION_DONGLE_PARAM_BYTES                             20
#define NVKMS_GPU_STRING_SIZE                                         80

#define NVKMS_VRR_SEMAPHORE_SURFACE_COUNT                             256
#define NVKMS_VRR_SEMAPHORE_SURFACE_SIZE                              (sizeof(NvU32) * NVKMS_VRR_SEMAPHORE_SURFACE_COUNT)

/*
 * The GUID string has the form:
 * XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
 * Two Xs per byte, plus four dashes and a NUL byte.
 */
#define NVKMS_GUID_STRING_SIZE                  ((NVKMS_GUID_SIZE * 2) + 5)

#define NVKMS_MODE_VALIDATION_MAX_INFO_STRING_LENGTH                  2048
#define NVKMS_EDID_INFO_STRING_LENGTH                                 (32 * 1024)

/*!
 * A base EDID is 128 bytes, with 128 bytes per extension block.  2048
 * should be large enough for any EDID we see.
 */
#define NVKMS_EDID_BUFFER_SIZE                                        2048

/*!
 * Description of modetimings.
 *
 * YUV420 modes require special care since some GPUs do not support YUV420
 * scanout in hardware.  When timings::yuv420Mode is NV_YUV420_SW, NVKMS will
 * set a mode with horizontal values that are half of what are described in
 * NvKmsMode, and not enable any color space conversion.  When clients allocate
 * a surface and populate it with content, the region of interest within the
 * surface should be half the width of the NvKmsMode, and the surface content
 * should be RGB->YUV color space converted, and decimated from 4:4:4 to 4:2:0.
 *
 * The NvKmsMode and viewPortOut, specified by the NVKMS client,
 * should be in "full" horizontal space, but the surface and
 * viewPortIn should be in "half" horizontal space.
 */
struct NvKmsMode {
    NvModeTimings timings;
    char name[32];
};

/*!
 * Mode validation override bit flags, for use in
 * NvKmsModeValidationParams::overrides.
 */
enum NvKmsModeValidationOverrides {
    NVKMS_MODE_VALIDATION_NO_MAX_PCLK_CHECK                  = (1 << 0),
    NVKMS_MODE_VALIDATION_NO_EDID_MAX_PCLK_CHECK             = (1 << 1),
    NVKMS_MODE_VALIDATION_NO_HORIZ_SYNC_CHECK                = (1 << 2),
    NVKMS_MODE_VALIDATION_NO_VERT_REFRESH_CHECK              = (1 << 3),
    NVKMS_MODE_VALIDATION_NO_EDID_DFP_MAX_SIZE_CHECK         = (1 << 4),
    NVKMS_MODE_VALIDATION_NO_EXTENDED_GPU_CAPABILITIES_CHECK = (1 << 5),
    NVKMS_MODE_VALIDATION_OBEY_EDID_CONTRADICTIONS           = (1 << 6),
    NVKMS_MODE_VALIDATION_NO_TOTAL_SIZE_CHECK                = (1 << 7),
    NVKMS_MODE_VALIDATION_NO_DUAL_LINK_DVI_CHECK             = (1 << 8),
    NVKMS_MODE_VALIDATION_NO_DISPLAYPORT_BANDWIDTH_CHECK     = (1 << 9),
    NVKMS_MODE_VALIDATION_ALLOW_NON_3DVISION_MODES           = (1 << 10),
    NVKMS_MODE_VALIDATION_ALLOW_NON_EDID_MODES               = (1 << 11),
    NVKMS_MODE_VALIDATION_ALLOW_NON_HDMI3D_MODES             = (1 << 12),
    NVKMS_MODE_VALIDATION_NO_MAX_SIZE_CHECK                  = (1 << 13),
    NVKMS_MODE_VALIDATION_NO_HDMI2_CHECK                     = (1 << 14),
    NVKMS_MODE_VALIDATION_NO_RRX1K_CHECK                     = (1 << 15),
    NVKMS_MODE_VALIDATION_REQUIRE_BOOT_CLOCKS                = (1 << 16),
    NVKMS_MODE_VALIDATION_ALLOW_DP_INTERLACED                = (1 << 17),
    NVKMS_MODE_VALIDATION_NO_INTERLACED_MODES                = (1 << 18),
    NVKMS_MODE_VALIDATION_MAX_ONE_HARDWARE_HEAD              = (1 << 19),
};

/*!
 * Frequency information used during mode validation (for HorizSync
 * and VertRefresh) can come from several possible sources.  NVKMS
 * selects the frequency information by prioritizing the input sources
 * and then reports the selected source.
 *
 * Without client input, NVKMS will use frequency ranges from the
 * EDID, if available.  If there is no EDID, NVKMS will fall back to
 * builtin conservative defaults.
 *
 * The client can specify frequency ranges that are used instead of
 * anything in the EDID (_CLIENT_BEFORE_EDID), or frequency ranges
 * that are used only if no EDID-reported ranges are available
 * (_CLIENT_AFTER_EDID).
 */
enum NvKmsModeValidationFrequencyRangesSource {
    NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_NONE                  = 0,
    NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CLIENT_BEFORE_EDID    = 1,
    NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_EDID                  = 2,
    NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CLIENT_AFTER_EDID     = 3,
    NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_CONSERVATIVE_DEFAULTS = 4,
};


/*!
 * Mode validation parameters.
 */
struct NvKmsModeValidationFrequencyRanges {
    enum NvKmsModeValidationFrequencyRangesSource source;
    NvU32 numRanges;
    struct {
        NvU32 high;
        NvU32 low;
    } range[NVKMS_MAX_VALID_SYNC_RANGES];
};

struct NvKmsModeValidationValidSyncs {

    /*! If TRUE, ignore frequency information from the EDID. */
    NvBool ignoreEdidSource;

    /*! values are in Hz */
    struct NvKmsModeValidationFrequencyRanges horizSyncHz;

    /*! values are in 1/1000 Hz */
    struct NvKmsModeValidationFrequencyRanges vertRefreshHz1k;
};

enum NvKmsStereoMode {
    NVKMS_STEREO_DISABLED = 0,
    NVKMS_STEREO_NVIDIA_3D_VISION,
    NVKMS_STEREO_NVIDIA_3D_VISION_PRO,
    NVKMS_STEREO_HDMI_3D,
    NVKMS_STEREO_OTHER,
};

enum NvKmsDscMode {
    NVKMS_DSC_MODE_DEFAULT = 0,
    NVKMS_DSC_MODE_FORCE_ENABLE,
    NVKMS_DSC_MODE_FORCE_DISABLE,
};

struct NvKmsModeValidationParams {
    NvBool verboseModeValidation;
    NvBool moreVerboseModeValidation;

    /*!
     * Normally, if a mode supports both YUV 4:2:0 and RGB 4:4:4,
     * NVKMS will prefer RGB 4:4:4 if both the monitor and the GPU
     * support it.  Use preferYUV420 to override that preference.
     */
    NvBool preferYUV420;

    enum NvKmsStereoMode stereoMode;
    NvU32 overrides;

    struct NvKmsModeValidationValidSyncs validSyncs;

    /*!
     * Normally, NVKMS will determine on its own whether to enable/disable
     * Display Stream Compression (DSC). Use dscMode to force NVKMS to
     * enable/disable DSC, when both the GPU and display supports it.
     */
    enum NvKmsDscMode dscMode;

    /*!
     * When enabled, Display Stream Compression (DSC) has an
     * associated bits/pixel rate, which NVKMS normally computes.
     * Use dscOverrideBitsPerPixelX16 to override the DSC bits/pixel rate.
     * This is in units of 1/16 of a bit per pixel.
     *
     * This target bits/pixel rate should be >= 8.0 and <= 32.0, i.e. the valid
     * bits/pixel values are members of the sequence 8.0, 8.0625, 8.125, ...,
     * 31.9375, 32.0.  You can convert bits/pixel value to
     * the dscOverrideBitsPerPixelX16 as follow:
     *
     *   +------------------+--------------------------------------------+
     *   |  bits_per_pixel  |  dscBitsPerPixelX16 = bits_per_pixel * 16  |
     *   +------------------+--------------------------------------------+
     *   |  8.0             |  128                                       |
     *   |  8.0625          |  129                                       |
     *   |  .               |  .                                         |
     *   |  .               |  .                                         |
     *   |  .               |  .                                         |
     *   |  31.9375         |  511                                       |
     *   |  32.0            |  512                                       |
     *   +------------------+--------------------------------------------+
     *
     * If the specified dscOverrideBitsPerPixelX16 is out of range,
     * then mode validation may fail.
     *
     * When dscOverrideBitsPerPixelX16 is 0, NVKMS compute the rate itself.
     */
    NvU32 dscOverrideBitsPerPixelX16;
};

/*!
 * The list of pixelShift modes.
 */
enum NvKmsPixelShiftMode {
    NVKMS_PIXEL_SHIFT_NONE = 0,
    NVKMS_PIXEL_SHIFT_4K_TOP_LEFT,
    NVKMS_PIXEL_SHIFT_4K_BOTTOM_RIGHT,
    NVKMS_PIXEL_SHIFT_8K,
};

/*!
 * The available resampling methods used when viewport scaling is requested.
 */
enum NvKmsResamplingMethod {
    NVKMS_RESAMPLING_METHOD_BILINEAR = 0,
    NVKMS_RESAMPLING_METHOD_BICUBIC_TRIANGULAR,
    NVKMS_RESAMPLING_METHOD_BICUBIC_BELL_SHAPED,
    NVKMS_RESAMPLING_METHOD_BICUBIC_BSPLINE,
    NVKMS_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_TRIANGULAR,
    NVKMS_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BELL_SHAPED,
    NVKMS_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BSPLINE,
    NVKMS_RESAMPLING_METHOD_NEAREST,
    NVKMS_RESAMPLING_METHOD_DEFAULT = NVKMS_RESAMPLING_METHOD_BILINEAR,
};

enum NvKmsWarpMeshDataType {
    NVKMS_WARP_MESH_DATA_TYPE_TRIANGLES_XYUVRQ,
    NVKMS_WARP_MESH_DATA_TYPE_TRIANGLE_STRIP_XYUVRQ,
};

/*!
 * Description of a cursor image on a single head; this is used by any
 * NVKMS request that needs to specify the cursor image.
 */
struct NvKmsSetCursorImageCommonParams {
    /*! The surface to display in the cursor. */
    NvKmsSurfaceHandle surfaceHandle[NVKMS_MAX_EYES];
    /*!
     * The cursor composition parameters gets read and applied only if the
     * specified cursor surface is not null.
     */
    struct NvKmsCompositionParams cursorCompParams;
};


/*!
 * Description of the cursor position on a single head; this is used
 * by any NVKMS request that needs to specify the cursor position.
 *
 * x,y are relative to the current viewPortIn configured on the head.
 */
struct NvKmsMoveCursorCommonParams {
    NvS16 x; /*! in */
    NvS16 y; /*! in */
};

/*!
 * Description of the main layer LUT on a single head; this is used by any NVKMS
 * request that needs to specify the LUT.
 */
struct NvKmsSetInputLutParams {
    NvBool specified;
    NvU32 depth;        /*! used bits per pixel (8, 15, 16, 24, 30) */

    /*!
     * The first and last elements (inclusive) in the color arrays to
     * use.  Valid values are in the range [0,N], where N is a
     * function of depth:
     *
     * Depth   N
     *  8      256
     *  15     32
     *  16     64
     *  24     256
     *  30     1024
     *
     * 'start' is the first element in the color arrays to use.
     */
    NvU32 start;

    /*!
     * 'end' is the last element (inclusive) in the color arrays to
     * use.  If end == 0, this command will disable the HW LUT for
     * this head.
     *
     * The other fields in this structure, besides 'specified', are ignored if
     * end == 0.
     */
    NvU32 end;

    /*!
     * Pointer to struct NvKmsLutRamps describing the LUT.
     * Elements [start,end] will be used.
     *
     * Each entry in the input LUT has valid values in the range [0, 65535].
     * However, on pre-Turing GPUs only 11 bits are significant; NVKMS will
     * convert values in this range into the appropriate internal format.
     *
     * Use nvKmsPointerToNvU64() to assign pRamps.
     */
    NvU64 pRamps NV_ALIGN_BYTES(8);
};


/*!
 * Description of the output LUT on a single head; this is used by any NVKMS
 * request that needs to specify the LUT.
 *
 * Unlike the input LUT:
 *  - specifying the output LUT updates all values at once.
 *
 * Each entry in the output LUT has valid values in the range [0, 65535].
 * However, only 11 bits are significant; NVKMS will convert values in this
 * range into the appropriate internal format.
 */
struct NvKmsSetOutputLutParams {
    NvBool specified;
    NvBool enabled;

    /*!
     * Pointer to struct NvKmsLutRamps containing the actual LUT data, if
     * required.
     * Use nvKmsPointerToNvU64() to assign pRamps.
     */
    NvU64 pRamps NV_ALIGN_BYTES(8);
};

/*!
 * Description of the LUT on a single head; this is used by any NVKMS
 * request that needs to specify the LUT.
 */
struct NvKmsSetLutCommonParams {
    struct NvKmsSetInputLutParams  input;
    struct NvKmsSetOutputLutParams output;

    NvBool synchronous; /*! block until the LUT update is complete */
};

struct NvKmsNIsoSurface {
    NvKmsSurfaceHandle surfaceHandle;
    enum NvKmsNIsoFormat format;
    NvU16 offsetInWords;
};

struct NvKmsCompletionNotifierDescription {
    struct NvKmsNIsoSurface surface;
    NvBool awaken;
};

struct NvKmsSemaphore {
    struct NvKmsNIsoSurface surface;
    NvU32 value;
};

enum NvKmsSyncptType {
    NVKMS_SYNCPT_TYPE_NONE,
    NVKMS_SYNCPT_TYPE_RAW,
    NVKMS_SYNCPT_TYPE_FD,
};

struct NvKmsSyncpt {
    enum NvKmsSyncptType type;
    union {
        int fd;
        struct {
            NvU32 id;
            NvU32 value;
        } raw;
    } u;
};

struct NvKmsChannelSyncObjects {
    /*
     * If useSyncpt is set to FALSE, clients can provide an acquisition and/or
     * release semaphore via the 'syncObjects.semaphores' struct.
     *
     *    If NvKmsAllocDeviceReply::supportsIndependentAcqRelSemaphore is
     *    FALSE, then 'syncObjects.semaphores.acquire.surface' must be the same
     *    as 'syncObjects.semaphores.release.surface'. In other words, the same
     *    exact semaphore surface must be used for both acquire and release.
     *
     *    If NvKmsAllocDeviceReply::supportsIndependentAcqRelSemaphore is
     *    TRUE, then the client is allowed to provide different semaphore
     *    surfaces for acquire and release.
     *
     * If useSyncpt is set to TRUE, clients can provide a pre-syncpt that they
     * want the display engine to wait on before scanning out from the given
     * buffer, and can specify that they want NVKMS to return a post-syncpt
     * that they can wait on, via the 'syncObjects.syncpts' struct.
     *
     *    The post-syncpt that NVKMS returns will be signaled once the
     *    buffer that was activated by this flip is displaced. As a typical
     *    example:
     *    - Client flips buffer A, and requests a post-syncpt PS.
     *    - Buffer A becomes active at the next frame boundary, and display
     *      starts scanning out buffer A.
     *    - Client flips buffer B.
     *    - Once the UPDATE for the buffer B flip is processed and display
     *      has finished sending the last pixel of buffer A to precomp for
     *      the current frame, post-syncpt PS will get signaled.
     *
     *    Clients can use this option iff
     *    NvKmsAllocDeviceReply::supportsSyncpts is TRUE.
     */
    NvBool useSyncpt;

    union {
        struct {
            struct NvKmsSemaphore acquire;
            struct NvKmsSemaphore release;
        } semaphores;

        struct {
            struct NvKmsSyncpt pre;
            enum NvKmsSyncptType requestedPostType;
        } syncpts;
    } u;
};

/*!
 * Description of how to flip on a single head.
 *
 * viewPortIn::point describes the position of the viewPortIn that
 * should be scaled to the viewPortOut of the head.  The
 * viewPortSizeIn is specified by NvKmsSetModeOneHeadRequest.  Note
 * that viewPortIn::point is in desktop coordinate space, and
 * therefore applies across all layers.
 *
 * For YUV420 modes, the surfaces and position should be in "half"
 * horizontal space.  See the explanation in NvKmsMode.
 *
 * If 'specified' is FALSE for any of the layers, then the current
 * hardware value is used.
 */
struct NvKmsFlipCommonParams {
    
    NvBool allowVrr;

    struct {
        NvBool specified;
        struct NvKmsPoint point;
    } viewPortIn;

    struct {
        struct NvKmsSetCursorImageCommonParams image;
        NvBool imageSpecified;

        struct NvKmsMoveCursorCommonParams position;
        NvBool positionSpecified;
    } cursor;

    /*
     * Set the output transfer function.
     *
     * If output transfer function is HDR and no staticMetadata is specified
     * for the head or layers, flip request will be rejected.
     *
     * If output transfer is set, output lut values specified during modeset
     * will be ignored and output lut will be set with the specified HDR
     * transfer function.
     *
     * If output transfer function is SDR and staticMetadata is enabled,
     * HDR content for that layer will be tonemapped to the SDR output
     * range.
     */
    struct {
        enum NvKmsOutputTf val;
        NvBool specified;
    } tf;

    /*!
     * Describe the LUT to be used with the modeset or flip.
     */
    struct NvKmsSetLutCommonParams lut;

    struct {
        NvBool specified;
        NvBool enabled;
        struct NvKmsLUTSurfaceParams lut;
    } olut;

    struct {
        NvBool specified;
        NvU32 val;
    } olutFpNormScale;

    struct {
        NvBool specified;
        /*!
         * If TRUE, override HDR static metadata for the head, instead of
         * calculating it from HDR layer(s). If FALSE, do not override.
         *
         * Note that “specified” serves to mark the field as being changed in
         * this flip request, rather than as specified for this frame.  So to
         * disable HDR static metadata, set hdrStaticMetadata.specified = TRUE
         * and hdrStaticMetadata.enabled = FALSE.
         */
        NvBool enabled;
        enum NvKmsInfoFrameEOTF eotf;
        struct NvKmsHDRStaticMetadata staticMetadata;
    } hdrInfoFrame;

    struct {
        NvBool specified;
        enum NvKmsOutputColorimetry val;
    } colorimetry;

    struct {
        struct {
            NvKmsSurfaceHandle handle[NVKMS_MAX_EYES];
            struct NvKmsRRParams rrParams;
            NvBool specified;
        } surface;

        /*
         * sizeIn/sizeOut can be used when
         * NvKmsAllocDeviceReply::layerCaps[layer].supportsWindowMode is TRUE.
         */
        struct {
            struct NvKmsSize val;
            NvBool specified;
        } sizeIn;

        struct {
            struct NvKmsSize val;
            NvBool specified;
        } sizeOut;

        /*
         * Set the position of the layer, relative to the upper left
         * corner of the surface. This controls the same state as
         * NVKMS_IOCTL_SET_LAYER_POSITION.
         *
         * This field can be used when
         * NvKmsAllocDeviceReply::layerCaps[layer].supportsWindowMode is TRUE.
         */
        struct {
            struct NvKmsSignedPoint val;
            NvBool specified;
        } outputPosition;

        struct {
            struct NvKmsCompletionNotifierDescription val;
            NvBool specified;
        } completionNotifier;

        struct {
            struct NvKmsChannelSyncObjects val;

            /* If 'specified' is FALSE, then the current hardware value is used. */
            NvBool specified;
        } syncObjects;

        /*
         * If 'maxDownscaleFactors::specified' is true, nvkms will set the
         * max H/V downscale usage bounds to the values specified in
         * 'maxDownscaleFactors::horizontal' and 'maxDownscaleFactors::vertical'.
         *
         * If the 'maxDownscaleFactors::specified' values are within the bounds
         * of 'NvKmsSetModeOneHeadReply::guaranteedUsage', then clients can expect
         * the flip to succeed. If the 'maxDownscaleFactors::specified' values are
         * beyond the bounds of 'NvKmsSetModeOneHeadReply::guaranteedUsage' but
         * within 'NvKmsSetModeOneHeadReply::possibleUsage', then the request may
         * legitimately fail due to insufficient display bandwidth and clients
         * need to be prepared to handle that flip request failure.
         *
         * If 'maxDownscaleFactors::specified' is false, nvkms will calculate max
         * H/V downscale factor by quantizing the range. E.g., max H/V downscale
         * factor supported by HW is 4x for 5-tap and 2x for 2-tap mode. If
         * 5-tap mode is required, the target usage bound that nvkms will
         * attempt to program will either allow up to 2x downscaling, or up to
         * 4x downscaling. If 2-tap mode is required, the target usage bound
         * that NVKMS will attempt to program will allow up to 2x downscaling.
         * Example: to downscale from 4096x2160 -> 2731x864 in 5-tap mode,
         * NVKMS would specify up to 2x for the H downscale bound (required is
         * 1.5x), and up to 4x for the V downscale bound (required is 2.5x).
         */
        struct {
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

            NvBool specified;
        } maxDownscaleFactors;

        NvBool tearing;

        /*
         * When true, we will flip to this buffer whenever the current eye is
         * finished scanning out.  Otherwise, this flip will only execute after
         * both eyes have finished scanout.
         *
         * Note that if this is FALSE and a vsynced stereo flip is requested,
         * the buffers in this flip will be displayed for minPresentInterval*2
         * vblanks, one for each eye.
         *
         * This flag cannot be used for the overlay layer.
         */
        NvBool perEyeStereoFlip;

        /* When non-zero, block the flip until PTIMER >= timeStamp. */
        NvU64 timeStamp NV_ALIGN_BYTES(8);
        NvU8 minPresentInterval;

        /* This field cannot be used for the main layer right now. */
        struct {
            struct NvKmsCompositionParams val;
            NvBool specified;
        } compositionParams;

        struct {
            NvBool specified;
            NvBool enabled;
            struct NvKmsLUTSurfaceParams lut;
        } ilut;

        struct {
            NvBool specified;
            NvBool enabled;
            struct NvKmsLUTSurfaceParams lut;
        } tmo;

        /*
         * Color-space conversion matrix applied to the layer before
         * compositing.
         *
         * If csc::specified is TRUE and csc::useMain is TRUE, then the CSC
         * matrix specified in the main layer is used instead of the one here.
         * If csc::specified is FALSE, then the CSC matrix used from the previous
         * flip is used. csc::useMain must be set to FALSE for the main layer.
         */
        struct {
            NvBool specified;
            NvBool useMain;
            struct NvKmsCscMatrix matrix;
        } csc;

        /*
         * When true, all pending flips and synchronization operations get
         * ignored, and channel flips to given buffer. Notifier and semaphore
         * should not be specified if this flag is true.  This flag does
         * nothing if set true for NVKMS_IOCTL_SET_MODE ioctl.
         *
         * This flag allows client to remove stalled flips and unblock
         * the channel.
         *
         * This flag cannot be used for the overlay layer.
         */
        NvBool skipPendingFlips;

        /*
         * This field can be used when
         * NvKmsAllocDeviceReply::layerCaps[layer].supportsICtCp = TRUE.
         *
         * If staticMetadata is enabled for multiple layers, flip request
         * will be rejected.
         */
        struct {
            NvBool specified;
            /*!
             * If TRUE, enable HDR static metadata. If FALSE, disable it.
             *
             * Note that “specified” serves to mark the field as being changed
             * in this flip request, rather than as specified for this frame.
             * So to disable HDR static metadata, set hdr.specified = TRUE and
             * hdr.staticMetadata.enabled = FALSE.
             */
            NvBool enabled;
            struct NvKmsHDRStaticMetadata staticMetadata;
        } hdr;

        /* Specifies whether the input color range is FULL or LIMITED. */
        struct {
            enum NvKmsInputColorRange val;
            NvBool specified;
        } colorRange;

        /* This field has no effect right now. */
        struct {
            enum NvKmsInputColorSpace val;
            NvBool specified;
        } colorSpace;

        /* Specifies input transfer function to be used */
        struct {
            enum NvKmsInputTf val;
            NvBool specified;
        } tf;

        /* When enabled, explicitly set CSC00 with provided matrix */
        struct {
            struct NvKmsCscMatrix matrix;
            NvBool enabled;
            NvBool specified;
        } csc00Override;

        /* When enabled, explicitly set CSC01 with provided matrix */
        struct {
            struct NvKmsCscMatrix matrix;
            NvBool enabled;
            NvBool specified;
        } csc01Override;

        /* When enabled, explicitly set CSC10 with provided matrix */
        struct {
            struct NvKmsCscMatrix matrix;
            NvBool enabled;
            NvBool specified;
        } csc10Override;

        /* When enabled, explicitly set CSC11 with provided matrix */
        struct {
            struct NvKmsCscMatrix matrix;
            NvBool enabled;
            NvBool specified;
        } csc11Override;
    } layer[NVKMS_MAX_LAYERS_PER_HEAD];
};

struct NvKmsFlipCommonReplyOneHead {
    struct {
        struct NvKmsSyncpt postSyncpt;
    } layer[NVKMS_MAX_LAYERS_PER_HEAD];
};

/*!
 * NVKMS_IOCTL_ALLOC_DEVICE: Allocate an NVKMS device object.
 *
 * This has the scope of a resman SLI device.
 *
 * Multiple clients can allocate devices (DRM-KMS, multiple X
 * servers).  Clients should configure SLI before initializing NVKMS.
 * NVKMS will query resman for the current SLI topology.
 *
 * The SLI configuration (both the linked SLI device, and the sliMosaic
 * boolean below) will be latched when the specified GPU transitions
 * from zero NVKMS devices allocated to one NVKMS device allocated.
 *
 * The returned information will remain static until the NVKMS device
 * object is freed.
 */

struct NvKmsDeviceId {
    /*!
     * The (primary) GPU for this device; this is used as the value
     * for NV0080_ALLOC_PARAMETERS::deviceId.
     */
    NvU32 rmDeviceId;

    /*!
     * The SMG (MIG) partition ID that this client must subscribe to in
     * N-way SMG mode; or, if not in MIG mode, the value NO_MIG_DEVICE which
     * equals to leaving this field initialized to zero (0).
     */
    MIGDeviceId migDevice;
};

struct NvKmsAllocDeviceRequest {
    /*!
     * Clients should populate versionString with the value of
     * NV_VERSION_STRING from nvUnixVersion.h.  This is used for a
     * version handshake.
     */
    char versionString[NVKMS_NVIDIA_DRIVER_VERSION_STRING_LENGTH];

    /*!
     * The underlying GPU for this device: this may point to a physical GPU
     * or a graphics capable MIG partition (= an SMG device).
     */
    struct NvKmsDeviceId deviceId;

    /*!
     * Whether SLI Mosaic is requested: i.e., multiple disps, one
     * per physical GPU, for the SLI device.
     */
    NvBool sliMosaic;

    /*!
     * When tryInferSliMosaicFromExistingDevice=TRUE, then the above
     * 'sliMosaic' field is ignored and the ALLOC_DEVICE request will
     * inherit the current sliMosaic state of the existing device
     * identified by deviceId.  If there is not an existing device for
     * deviceId, then the ALLOC_DEVICE request will proceed normally, honoring
     * the requested sliMosaic state.
     */
    NvBool tryInferSliMosaicFromExistingDevice;

    /*!
     * NVKMS will use the 3D engine for headSurface.  If clients want to avoid
     * the use of the 3D engine, set no3d = TRUE.  Note this will cause modesets
     * that require headSurface to fail.
     *
     * This flag is only honored when there is not already an existing device
     * for the deviceId.
     */
    NvBool no3d;

    /*!
     * When enableConsoleHotplugHandling is TRUE, NVKMS will start handling
     * hotplug events at the console when no modeset owner is present.
     *
     * If FALSE, console hotplug handling behavior is not changed.
     *
     * This should be set to TRUE for clients that intend to allocate the device
     * but don't intend to become the modeset owner right away. It should be set
     * to FALSE for clients that may take modeset ownership immediately, in
     * order to suppress hotplug handling between the NVKMS_IOCTL_ALLOC_DEVICE
     * and NVKMS_IOCTL_GRAB_OWNERSHIP calls when the calling client is the first
     * to allocate the device.
     *
     * Note that NVKMS_IOCTL_RELEASE_OWNERSHIP also enables console hotplug
     * handling. Once enabled, console hotplug handling remains enabled until
     * the last client frees the device.
     */
    NvBool enableConsoleHotplugHandling;

    struct {
        /* name[0] == '\0' for unused registryKeys[] array elements. */
        char name[NVKMS_MAX_DEVICE_REGISTRY_KEYNAME_LEN];
        NvU32 value;
    } registryKeys[NVKMS_MAX_DEVICE_REGISTRY_KEYS];
};

enum NvKmsAllocDeviceStatus {
    NVKMS_ALLOC_DEVICE_STATUS_SUCCESS,
    NVKMS_ALLOC_DEVICE_STATUS_VERSION_MISMATCH,
    NVKMS_ALLOC_DEVICE_STATUS_BAD_REQUEST,
    NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR,
    NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE,
    NVKMS_ALLOC_DEVICE_STATUS_CORE_CHANNEL_ALLOC_FAILED,
};


struct NvKmsAllocDeviceReply {

    enum NvKmsAllocDeviceStatus status;

    /*!
     * The handle to use when identifying this NVKMS device in
     * subsequent calls.
     */
    NvKmsDeviceHandle deviceHandle;

    /*!
     * A bitmask, indicating the GPUs, one per bit, contained by this
     * device.
     */
    NvU32 subDeviceMask;

    /*! The number of heads on each disp. */
    NvU32 numHeads;

    /*! The number of disps. */
    NvU32 numDisps;

    /*! The handle to identify each disp, in dispHandles[0..numDisps). */
    NvKmsDispHandle dispHandles[NVKMS_MAX_SUBDEVICES];

    /*!
     * Device-wide Capabilities: of the display engine.
     *
     * IMPLEMENTATION NOTE: this is the portion of DispHalRec::caps
     * that can vary between EVO classes.
     */
    NvBool requiresVrrSemaphores;
    NvBool inputLutAppliesToBase;

    /*!
     * Whether the client can allocate and manipulate SwapGroup objects via
     * NVKMS_IOCTL_ALLOC_SWAP_GROUP and friends.
     */
    NvBool supportsSwapGroups;

    /*!
     * Whether NVKMS supports Warp and Blend on this device.
     */
    NvBool supportsWarpAndBlend;

    /*!
     * When nIsoSurfacesInVidmemOnly=TRUE, then only video memory
     * surfaces can be used for the surface in
     * NvKmsCompletionNotifierDescription or NvKmsSemaphore.
     */
    NvBool nIsoSurfacesInVidmemOnly;

    /*
     * When requiresAllAllocationsInSysmem=TRUE, then all memory allocations
     * that will be accessed by display must come from sysmem.
     */
    NvBool requiresAllAllocationsInSysmem;

    /*
     * Whether the device that NVKMS is driving supports headSurface GPU
     * composition.
     */
    NvBool supportsHeadSurface;

    /*!
     * The display engine supports a "legacy" format for notifiers and
     * semaphores (one word for semaphores and base channel notifiers;
     * two words for overlay notifiers).  On newer GPUs, the display
     * engine also supports a similar four word semaphore and notifier
     * format used by graphics.
     *
     * This describes which values are valid for NvKmsNIsoFormat.
     *
     * Iff a particular enum NvKmsNIsoFormat 'value' is supported,
     * then (1 << value) will be set in validNIsoFormatMask.
     */
    NvU8 validNIsoFormatMask;

    NvU32 surfaceAlignment;
    NvU32 maxWidthInBytes;
    NvU32 maxWidthInPixels;
    NvU32 maxHeightInPixels;
    NvU32 maxCursorSize;

    /*!
     * Describes the supported Color Key selects and blending modes for match
     * and nomatch cursor pixels.
     */
    struct NvKmsCompositionCapabilities cursorCompositionCaps;

    /*! The number of layers attached to each head. */
    NvU32 numLayers[NVKMS_MAX_HEADS_PER_DISP];

    /*!
     * Describes supported functionalities for each layer.
     */
    struct NvKmsLayerCapabilities layerCaps[NVKMS_MAX_LAYERS_PER_HEAD];

    /*!
     * Describes supported functionalities for the output LUT on each head
     */
    struct NvKmsLUTCaps olutCaps;

    /*!
     * This bitmask specifies all of the (rotation, reflectionX, reflectionY)
     * combinations that are supported for the main and overlay layers.
     * Each bit in this bitmask is mapped to one combination per the scheme
     * in NvKmsRRParamsToCapBit().
     */
    NvU16 validLayerRRTransforms;

    /*!
     * IO coherency modes that display supports for ISO and NISO memory
     * allocations, respectively.
     */
    NvKmsDispIOCoherencyModes isoIOCoherencyModes;
    NvKmsDispIOCoherencyModes nisoIOCoherencyModes;

    /*!
     * 'displayIsGpuL2Coherent' indicates whether display is coherent with
     * GPU's L2 cache.
     */
    NvBool displayIsGpuL2Coherent;

    /*!
     * 'supportsSyncpts' indicates whether NVKMS supports the use of syncpts
     * for synchronization.
     */
    NvBool supportsSyncpts;

    /*!
     * 'supportsIndependentAcqRelSemaphore' indicates whether HW supports
     * configuring different semaphores for acquire and release for a buffer
     * flip on a given layer.
     */
    NvBool supportsIndependentAcqRelSemaphore;

    /*!
     * 'supportsVblankSyncObjects' indicates whether HW supports raster
     * generator sync objects that signal at vblank.
     */
    NvBool supportsVblankSyncObjects;

    /*!
     * 'supportsVblankSemControl' indicates whether the VBlank Semaphore Control
     * interface:
     *
     *   NVKMS_IOCTL_ENABLE_VBLANK_SEM_CONTROL,
     *   NVKMS_IOCTL_DISABLE_VBLANK_SEM_CONTROL,
     *   NVKMS_IOCTL_ACCEL_VBLANK_SEM_CONTROLS,
     *
     * is supported.
     */
    NvBool supportsVblankSemControl;

    /*!
     * 'supportsInputColorSpace' indicates whether the HW supports setting the
     * input color space.
     */
    NvBool supportsInputColorSpace;

    /*!
     * 'supportsInputColorRange' indicates whether the HW supports setting the
     * input color range.
     */
    NvBool supportsInputColorRange;

    /*! framebuffer console base address and size. */
    NvU64 vtFbBaseAddress;
    NvU64 vtFbSize;
};

struct NvKmsAllocDeviceParams {
    struct NvKmsAllocDeviceRequest request; /*! in */
    struct NvKmsAllocDeviceReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_FREE_DEVICE: Free the NVKMS device object specified by
 * deviceHandle.
 *
 * The underlying device is not actually freed until all callers of
 * NVKMS_IOCTL_ALLOC_DEVICE have freed their reference to the device.
 *
 * When a client calls FREE_DEVICE, any configuration specified by
 * that client will be removed:
 * - Any EDID overrides.
 * - Any interest declared on dynamic dpys.
 * - Any cursor image on any head.
 * - Any custom LUT contents.
 * - Any interest declared on any events.
 *
 * XXX define how FREE_DEVICE interacts with:
 * - concurrent X servers on different VTs
 * - console restore
 */

struct NvKmsFreeDeviceRequest {
    NvKmsDeviceHandle deviceHandle;
};

struct NvKmsFreeDeviceReply {
    NvU32 padding;
};

struct NvKmsFreeDeviceParams {
    struct NvKmsFreeDeviceRequest request; /*! in */
    struct NvKmsFreeDeviceReply reply;     /*!out */
};


/*!
 * NVKMS_IOCTL_QUERY_DISP: Query information about the NVKMS disp
 * object specified by the tuple (deviceHandle, dispHandle).
 *
 * The returned information will remain static until the NVKMS device
 * object is freed.
 */

struct NvKmsQueryDispRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
};

struct NvKmsQueryDispReply {
    /*! The possible dpys for this disp, excluding any dynamic dpys. */
    NVDpyIdList validDpys;

    /*! The dpys that were driven at boot-time, if any. */
    NVDpyIdList bootDpys;

    /*! The dpys that are capable of dynamic mux switching, if any. */
    NVDpyIdList muxDpys;

    /*! The framelock device, if any, connected to this disp. */
    NvKmsFrameLockHandle frameLockHandle;

    /*! The number of connectors on this disp. */
    NvU32 numConnectors;

    /*!
     * The handle to identify each connector, in
     * connectorHandles[0..numConnectors)
     */
    NvKmsConnectorHandle connectorHandles[NVKMS_MAX_CONNECTORS_PER_DISP];

    /*!
     * A string describing one of the the GPUs used by this disp.  The
     * NVKMS log will also print this string to the kernel log.  Users
     * should be able to correlate GPUs between NVKMS and NVKMS
     * clients using this string.
     */
    char gpuString[NVKMS_GPU_STRING_SIZE];
};

struct NvKmsQueryDispParams {
    struct NvKmsQueryDispRequest request; /*! in */
    struct NvKmsQueryDispReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_QUERY_CONNECTOR_STATIC_DATA: Query information about the NVKMS
 * connector object specified by the triplet (deviceHandle, dispHandle,
 * connectorHandle).
 *
 * The returned information will remain static until the NVKMS device
 * object is freed.
 */

struct NvKmsQueryConnectorStaticDataRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvKmsConnectorHandle connectorHandle;
};

struct NvKmsQueryConnectorStaticDataReply {
    NVDpyId dpyId;
    NvBool isDP;
    NvBool isLvds;
    NvBool locationOnChip;
    NvU32 legacyTypeIndex;
    NvKmsConnectorType type;
    NvU32 typeIndex;
    NvKmsConnectorSignalFormat signalFormat;
    NvU32 physicalIndex;
    NvU32 physicalLocation;
};

struct NvKmsQueryConnectorStaticDataParams {
    struct NvKmsQueryConnectorStaticDataRequest request; /*! in */
    struct NvKmsQueryConnectorStaticDataReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_QUERY_CONNECTOR_DYNAMIC_DATA: Query dynamic information about the
 * NVKMS connector object specified by the triplet (deviceHandle, dispHandle,
 * connectorHandle).
 */

struct NvKmsQueryConnectorDynamicDataRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvKmsConnectorHandle connectorHandle;
};

struct NvKmsQueryConnectorDynamicDataReply {
#define NVKMS_DP_DETECT_COMPLETE_POLL_INTERVAL_USEC    100000 /* in microseconds */
#define NVKMS_DP_DETECT_COMPLETE_TIMEOUT_USEC        10000000 /* in microseconds */

    /*
     * For DisplayPort devices, indicates whether the DisplayPort library is
     * finished detecting devices on this connector.  This is set to TRUE for
     * other devices because NVKMS knows as soon as ALLOC_DEVICE is complete
     * whether the device is connected or not.
     */
    NvBool detectComplete;
    /*
     * Contains the list of display IDs for dynamic dpys detected on this
     * connector.
     */
    NVDpyIdList dynamicDpyIdList;
};

struct NvKmsQueryConnectorDynamicDataParams {
    struct NvKmsQueryConnectorDynamicDataRequest request; /*! in */
    struct NvKmsQueryConnectorDynamicDataReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_QUERY_DPY_STATIC_DATA: Query static information about
 * the NVKMS dpy object specified by the triplet (deviceHandle,
 * dispHandle, dpyId).  This information should remain static for the
 * lifetime of the dpy.
 */

struct NvKmsQueryDpyStaticDataRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
};

struct NvKmsQueryDpyStaticDataReply {
    NvKmsConnectorHandle connectorHandle; /*! The connector driving this dpy. */
    NvU32 type;                        /*! NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_ */
    char dpAddress[NVKMS_DP_ADDRESS_STRING_LENGTH];
    NvBool mobileInternal;
    NvBool isDpMST;
    /* Bitmask of valid heads to drive this dpy. */
    NvU32 headMask;
};

struct NvKmsQueryDpyStaticDataParams {
    struct NvKmsQueryDpyStaticDataRequest request; /*! in */
    struct NvKmsQueryDpyStaticDataReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_QUERY_DPY_DYNAMIC_DATA: Query dynamic information about
 * the NVKMS dpy object specified by the triplet (deviceHandle,
 * dispHandle, dpyId).
 *
 * This information should be re-queried after an
 * NVKMS_EVENT_TYPE_DPY_CHANGED event.
 */

struct NvKmsQueryDpyDynamicDataRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;

    NvBool forceConnected;
    NvBool forceDisconnected;
    NvBool overrideEdid;
    NvBool ignoreEdid;
    NvBool ignoreEdidChecksum;
    NvBool allowDVISpecPClkOverride;
    NvBool dpInbandStereoSignaling;
    NvBool disableACPIBrightnessHotkeys;

    /*
     * If overrideEdid is TRUE, then edid::buffer[] contains an EDID
     * to override anything detected.
     */
    struct {
        NvU16 bufferSize;
        NvU8 buffer[NVKMS_EDID_BUFFER_SIZE];
    } edid;
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC attributes. */
enum NvKmsDpyAttributeColorBpcValue {
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN =  0,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6       =  6,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8       =  8,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10      = 10,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_MAX     =
        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10,
};

typedef struct _NvKmsDpyOutputColorFormatInfo {
    struct {
        enum NvKmsDpyAttributeColorBpcValue maxBpc;
        enum NvKmsDpyAttributeColorBpcValue minBpc;
    } rgb444, yuv444, yuv422;
} NvKmsDpyOutputColorFormatInfo;

enum NvKmsDpyVRRType {
    NVKMS_DPY_VRR_TYPE_NONE,
    NVKMS_DPY_VRR_TYPE_GSYNC,
    NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_DEFAULTLISTED,
    NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_NON_DEFAULTLISTED,
};

struct NvKmsQueryDpyDynamicDataReply {
    char name[NVKMS_DPY_NAME_SIZE];

    NvU32 maxPixelClockKHz;
    NvBool connected;
    NvBool isVirtualRealityHeadMountedDisplay;

    struct {
        NvU8 heightInCM; /* vertical screen size */
        NvU8 widthInCM; /* horizontal screen size */
    } physicalDimensions;

    /*!
     * Which VRR type has been selected for this display, either true
     * G-SYNC, Adaptive-Sync defaultlisted, or Adaptive-Sync non-defaultlisted.
     */
    enum NvKmsDpyVRRType vrrType;

    NvBool supportsHDR;

    struct {
        NvBool supported;
        NvBool isDLP;
        NvBool isAegis;
        NvU32  subType; /*! STEREO_PLUG_AND_PLAY_ from nvStereoDisplayDef.h */
    } stereo3DVision;

    struct {
        struct {
            NvBool valid;
            NvU8 buffer[NVKMS_GUID_SIZE];
            char str[NVKMS_GUID_STRING_SIZE];
        } guid;
    } dp;

    struct {
        /*!
         * The size of the EDID in buffer[], or 0 if there is no EDID
         * available in buffer[].
         */
        NvU16 bufferSize;

        /*!
         * Whether NVKMS determined that the EDID is valid.  If the
         * EDID is not valid, there may still be information available
         * in infoString: the infoString will describe why the EDID
         * was deemed invalid.
         */
        NvBool valid;

        /*!
         * The raw EDID bytes.
         */
        NvU8 buffer[NVKMS_EDID_BUFFER_SIZE];

        /*!
         * Parsed information from the EDID.  For the raw EDID bytes,
         * see NvKmsQueryDpyDynamicDataParams::edid::buffer[].
         */
        char infoString[NVKMS_EDID_INFO_STRING_LENGTH];
    } edid;

    NvKmsDpyOutputColorFormatInfo supportedOutputColorFormats;

    struct NvKmsSuperframeInfo superframeInfo;
};

struct NvKmsQueryDpyDynamicDataParams {
    struct NvKmsQueryDpyDynamicDataRequest request; /*! in */
    struct NvKmsQueryDpyDynamicDataReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_VALIDATE_MODE_INDEX: Validate a particular mode from a
 * dpy's candidate modes.
 *
 * NVKMS can consider modes from a dpy's EDID, as well as a
 * variety of builtin modes.
 *
 * This ioctl identifies one of those candidate modes by index.  NVKMS
 * will attempt to validate that candidate mode for the dpy, using the
 * specified mode validation parameters.
 *
 * If the mode index is larger than the list of candidate modes,
 * reply::end will be TRUE.  Otherwise, reply::end will be FALSE, and
 * reply::mode will contain the candidate mode.
 *
 * If the mode is valid, then reply::valid will be TRUE.  Otherwise,
 * reply::valid will be FALSE.  In either case, request::pInfoString[]
 * will contain a description of what happened during mode validation.
 *
 * To query the full modepool, clients should repeatedly call
 * NVKMS_IOCTL_VALIDATE_MODE_INDEX with increasing mode index values,
 * until NVKMS reports end==TRUE.
 *
 * Note that the candidate mode list can change when the dpy changes
 * (reported by the NVKMS_EVENT_TYPE_DPY_CHANGED event).  The client
 * should restart its modepool querying if it receives a DPY_CHANGED
 * event.  The candidate mode list can also change based on the
 * parameters in request::modeValidation.  Clients should not change
 * request::modeValidation while looping over candidate mode indices.
 *
 * Pseudocode example usage pattern:
 *
 *   struct NvKmsModeValidationParams modeValidation = Initialize();
 *
 * retry:
 *   NvU32 modeIndex = 0;
 *
 *   while (1) {
 *       char infoString[INFO_STRING_LENGTH];
 *       memset(&params);
 *       params.request.dpyId = dpyId;
 *       params.request.modeIndex = modeIndex++;
 *       params.request.modeValidation = modeValidation;
 *       params.request.pInfoString = nvKmsPointerToNvU64(infoString);
 *       params.request.infoStringLength = sizeof(infoString);
 *
 *       ioctl(&params);
 *
 *       if (params.reply.end) break;
 *
 *       print(infoString);
 *
 *       if (params.reply.valid) {
 *           AddToModePool(params.reply.mode);
 *       }
 *   }
 *
 *   if (dpyChanged) goto retry;
 *
 */

struct NvKmsValidateModeIndexRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    struct NvKmsModeValidationParams modeValidation;
    NvU32 modeIndex;

    /*
     * Pointer to a string of size 'infoStringSize'.
     * Use nvKmsPointerToNvU64() to assign pInfoString.
     * The maximum size allowed is
     * NVKMS_MODE_VALIDATION_MAX_INFO_STRING_LENGTH.
     */
    NvU32 infoStringSize;
    NvU64 pInfoString NV_ALIGN_BYTES(8);
};

struct NvKmsValidateModeIndexReply {
    NvBool end;
    NvBool valid;

    struct NvKmsMode mode;

    /*! The validSyncs used by NVKMS when validating the mode. */
    struct NvKmsModeValidationValidSyncs validSyncs;

    /*! Whether this mode is marked as "preferred" by the EDID. */
    NvBool preferredMode;

    /*! A text description of the mode. */
    char description[64];

    /*! Where the mode came from. */
    enum NvKmsModeSource {
        NvKmsModeSourceUnknown = 0,
        NvKmsModeSourceEdid    = 1,
        NvKmsModeSourceVesa    = 2,
    } source;

    /* The number of bytes written to 'pInfoString' (from the request) */
    NvU32 infoStringLenWritten;

    /*!
     * These are the usage bounds that may be possible with this mode,
     * assuming that only one head is active. For actual usage bounds,
     * see guaranteedUsage and possibleUsage returned in
     * NvKmsSetModeOneHeadReply.
     */
    struct NvKmsUsageBounds modeUsage;

    /*
     * Whether this mode supports stereo mode hdmi3D, but wasn't
     * requested.
     */
    NvBool hdmi3DAvailable;
};

struct NvKmsValidateModeIndexParams {
    struct NvKmsValidateModeIndexRequest request; /*! in */
    struct NvKmsValidateModeIndexReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_VALIDATE_MODE: Validate an individual mode for the
 * specified dpy.
 *
 * Given the validation parameters, NVKMS will test whether the given
 * mode is currently valid for the specified dpy.
 *
 * If the mode is valid, then reply::valid will be TRUE.  Otherwise,
 * reply::valid will be FALSE.  In either case, reply::infoString[]
 * will contain a description of what happened during mode validation.
 */

struct NvKmsValidateModeRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    struct NvKmsModeValidationParams modeValidation;
    struct NvKmsMode mode;

    /*
     * Pointer to a string of size 'infoStringSize'.
     * Use nvKmsPointerToNvU64() to assign pInfoString.
     * The maximum size allowed is
     * NVKMS_MODE_VALIDATION_MAX_INFO_STRING_LENGTH.
     */
    NvU32 infoStringSize;
    NvU64 pInfoString NV_ALIGN_BYTES(8);
};

struct NvKmsValidateModeReply {
    NvBool valid;

    /*! The validSyncs used by NVKMS when validating the mode. */
    struct NvKmsModeValidationValidSyncs validSyncs;

    /* The number of bytes written to 'pInfoString' (from the request) */
    NvU32 infoStringLenWritten;

    /*!
     * These are the usage bounds that may be possible with this mode,
     * assuming that only one head is active. For actual usage bounds,
     * see guaranteedUsage and possibleUsage returned in
     * NvKmsSetModeOneHeadReply.
     */
    struct NvKmsUsageBounds modeUsage;
};

struct NvKmsValidateModeParams {
    struct NvKmsValidateModeRequest request; /*! in */
    struct NvKmsValidateModeReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_MODE: Perform a modeset.
 *
 * NvKmsSetModeRequest can describe the modetiming configuration
 * across all heads of all disps within the SLI device.
 *
 * The elements in NvKmsSetModeRequest::disp[] correspond to the disps
 * returned in NvKmsAllocDeviceReply::dispHandles[].
 *
 * To only touch certain heads and disps, use the
 * requestedHeadsBitMask and requestedDispsBitMask fields to limit
 * which array elements are honored.
 *
 * If the request is invalid, one or more of the
 * NvKmsSetMode{,OneDisp,OneHead}Reply::status fields will have a
 * non-SUCCESS value.  If the mode set completed successfully, then
 * all {NvKmsSetMode{,OneDisp,OneHead}Reply::status fields should be
 * SUCCESS.
 */

struct NvKmsSetModeHeadSurfaceParams {
    NvBool forceCompositionPipeline;
    NvBool forceFullCompositionPipeline;
    NvBool fakeOverlay;
    NvBool blendAfterWarp;
    NvBool transformSpecified;

    /* Reflect the image along the X axis. */
    NvBool reflectionX;

    /* Reflect the image along the Y axis. */
    NvBool reflectionY;

    /*
     * Rotate the image counter-clockwise in 90 degree increments.
     *
     * Reflection (specified above by ::reflection[XY]) is applied
     * before rotation.  This matches the semantics of RandR.  From:
     *
     *  https://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
     *
     * "Rotation and reflection and how they interact can be confusing. In
     * Randr, the coordinate system is rotated in a counter-clockwise direction
     * relative to the normal orientation. Reflection is along the window system
     * coordinate system, not the physical screen X and Y axis, so that rotation
     * and reflection do not interact. The other way to consider reflection is
     * to is specified in the 'normal' orientation, before rotation, if you find
     * the other way confusing."
     */
    enum NvKmsRotation rotation;
    enum NvKmsPixelShiftMode pixelShift;
    enum NvKmsResamplingMethod resamplingMethod;
    struct NvKmsMatrix transform; /* Only honored if transformSpecified. */

    NvKmsSurfaceHandle blendTexSurfaceHandle;
    NvKmsSurfaceHandle offsetTexSurfaceHandle;

    /*
     * When warpMesh::surfaceHandle is non-zero, it indicates a surface
     * containing warp mesh vertex data.  The surface should:
     *
     * - Have a width multiple of 1024 pixels.
     * - Have a depth of 32.
     * - Contain a binary representation of a list of six-component
     *   vertices. Each of these components is a 32-bit floating point value.
     *
     * The X, Y components should contain normalized vertex coordinates, to be
     * rendered as a triangle list or strip.  The X and Y components' [0,1]
     * range map to the head's ViewportOut X and Y, respectively.
     *
     * The U, V, R, and Q components should contain normalized, projective
     * texture coordinates:
     *
     * U, V: 2D texture coordinate.  U and V components' [0,1] range maps to the
     * display's MetaMode ViewportIn X and Y, respectively.
     *
     * R: unused
     *
     * Q: Used for interpolation purposes.  This is typically the third
     * component of the result of a multiplication by a 3x3 projective transform
     * matrix.
     *
     * warpMesh::vertexCount should contain the amount of vertices stored in the
     * surface.
     *
     * warpMesh::dataType indicates if the vertices describe a triangle list or
     * a triangle strip.  A triangle list must have a vertexCount that is a
     * multiple of 3.
     */
    struct {
        NvKmsSurfaceHandle surfaceHandle;
        NvU32 vertexCount;
        enum NvKmsWarpMeshDataType dataType;
    } warpMesh;
};

#define NVKMS_VRR_MIN_REFRESH_RATE_MAX_VARIANCE 10 // 10hz

enum NvKmsAllowAdaptiveSync {
    NVKMS_ALLOW_ADAPTIVE_SYNC_DISABLED = 0,
    NVKMS_ALLOW_ADAPTIVE_SYNC_DEFAULTLISTED_ONLY,
    NVKMS_ALLOW_ADAPTIVE_SYNC_ALL,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE attribute. */
enum NvKmsDpyAttributeRequestedColorSpaceValue {
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_RGB = 0,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr422 = 1,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr444 = 2,
};

/*!
 * Values for the NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_RANGE and
 * NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_RANGE attributes.
 */
enum NvKmsDpyAttributeColorRangeValue {
    NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL = 0,
    NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED = 1,
};

struct NvKmsSetModeOneHeadRequest {
    /*!
     * The list of dpys to drive with this head; or, empty to disable
     * the head.
     */
    NVDpyIdList dpyIdList;

    /*! The modetimings to set on the head. */
    struct NvKmsMode mode;

    /*! The above mode will be validated, using these validation parameters. */
    struct NvKmsModeValidationParams modeValidationParams;

    /*!
     * The region within the raster timings that should contain an image.
     * This is only used when viewPortOutSpecified is TRUE.  Otherwise, the
     * viewPortOut is inferred from the raster timings.
     *
     * For YUV420 modes, the viewPortOut should be in "full"
     * horizontal space.  See the explanation in NvKmsMode.
     */
    struct NvKmsRect viewPortOut;

    /*!
     * The size, in pixels, that the head will fetch from any surface
     * it scans from.  The viewPortPointIn is specified in
     * NvKmsFlipCommonParams.
     *
     * For YUV420 modes, the viewPortSizeIn should be in "half"
     * horizontal space.  See the explanation in NvKmsMode.
     */
    struct NvKmsSize viewPortSizeIn;

    /*!
     * Describe the surfaces to present on this head.
     */
    struct NvKmsFlipCommonParams flip;

    /*!
     * The headSurface configuration requested, if any.
     */
    struct NvKmsSetModeHeadSurfaceParams headSurface;

    NvBool viewPortOutSpecified; /*! Whether to use viewPortOut. */

    /*!
     * Allow G-SYNC to be enabled on this head if it is supported by the GPU
     * and monitor.
     */
    NvBool allowGsync;

    /*!
     * Whether to allow Adaptive-Sync to be enabled on this head if it is
     * supported by the GPU:
     *
     * NVKMS_ALLOW_ADAPTIVE_SYNC_ALL:
     *     VRR is enabled as long as this monitor supports Adaptive-Sync.
     *
     * NVKMS_ALLOW_ADAPTIVE_SYNC_DEFAULTLISTED_ONLY:
     *     VRR is only enabled on this head if the monitor is on the
     *     Adaptive-Sync defaultlist.
     *
     * NVKMS_ALLOW_ADAPTIVE_SYNC_DISABLED:
     *     VRR is forced to be disabled if this is an Adaptive-Sync monitor.
     */
    enum NvKmsAllowAdaptiveSync allowAdaptiveSync;

    /*!
     * Override the minimum refresh rate for VRR monitors specified by the
     * EDID (0 to not override the EDID-provided value).  Clamped at modeset
     * time to within NVKMS_VRR_MIN_REFRESH_RATE_MAX_VARIANCE of the
     * EDID-specified minimum refresh rate, as long as the minimum is no
     * lower than 1hz and the maximum does not exceed the maximum refresh rate
     * defined by the mode timings.  The current minimum refresh rate and this
     * valid range are exposed through
     * NV_KMS_DPY_ATTRIBUTE_VRR_MIN_REFRESH_RATE.
     *
     * Does not affect G-SYNC monitors, which do not have a minimum refresh
     * rate.
     */
    NvU32 vrrOverrideMinRefreshRate;

    /*!
     * Output colorspace. Valid only when colorSpaceSpecified is true.
     */
    enum NvKmsDpyAttributeRequestedColorSpaceValue colorSpace;
    NvBool colorSpaceSpecified;

    /*!
     * Output color bpc. Valid only when colorBpcSpecified is true.
     */
    enum NvKmsDpyAttributeColorBpcValue colorBpc;
    NvBool colorBpcSpecified;

    /*!
     * Output color range. Valid only when colorRangeSpecified is true.
     */
    enum NvKmsDpyAttributeColorRangeValue colorRange;
    NvBool colorRangeSpecified;
};

struct NvKmsSetModeOneDispRequest {
    /*!
     * The bit mask of which head[] elements to look at on this disp;
     * any other head will use its existing configuration.
     */
    NvU32 requestedHeadsBitMask;
    struct NvKmsSetModeOneHeadRequest head[NVKMS_MAX_HEADS_PER_DISP];
};

struct NvKmsSetModeRequest {
    NvKmsDeviceHandle deviceHandle;

    /*!
     * When a modeset request is made, NVKMS will first perform
     * validation to confirm whether the request can be satisfied.  If
     * the requested configuration cannot be fulfilled, the request
     * returns FALSE.
     *
     * Only the modeset owner can issue a modeset with commit set to TRUE.
     *
     * If 'commit' is FALSE, then the status of validation will be returned.
     *
     * If 'commit' is TRUE, and validation passes, then NVKMS will
     * apply the requested configuration.
     */
    NvBool commit;

    /*!
     * The bitmask of which indices within disp[] describe requested
     * configuration changes.  Any other disps will use their existing
     * configuration.
     */
    NvU32 requestedDispsBitMask;

    /*
     * disp[n] corresponds to the disp named by
     * NvKmsAllocDeviceReply::dispHandles[n].
     */
    struct NvKmsSetModeOneDispRequest disp[NVKMS_MAX_SUBDEVICES];

    /*!
     * Whether to use NVKMS's builtin headSurface support when necessary.
     *
     * XXX NVKMS HEADSURFACE TODO: Make this the default and remove this field.
     */
    NvBool allowHeadSurfaceInNvKms;
};

enum NvKmsSetModeOneHeadStatus {
    NVKMS_SET_MODE_ONE_HEAD_STATUS_SUCCESS = 0,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_MODE = 1,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_DPY = 2,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_CURSOR_IMAGE = 3,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_CURSOR_POSITION = 4,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_LUT = 5,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_FLIP = 6,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_PERMISSIONS = 7,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_INVALID_HEAD_SURFACE = 8,
    NVKMS_SET_MODE_ONE_HEAD_STATUS_UNSUPPORTED_HEAD_SURFACE_COMBO = 9,
};

struct NvKmsSetModeOneHeadReply {
    /*!
     * When the NVKMS_IOCTL_SET_MODE succeeds, then this will be SUCCESS.
     * Otherwise, 'status' will be a non-SUCCESS value for one or more
     * heads and/or one or more disps.
     *
     * Note that a failure could occur for a preexisting head
     * configuration, so this status could be != SUCCESS for a head
     * not listed in NvKmsSetModeOneDispRequest::requestedHeadsBitMask.
     */
    enum NvKmsSetModeOneHeadStatus status;

    NvU32 hwHead;

    /*!
     * The usage bounds that may be possible on this head based on the ISO
     * BW at that point.
     *
     * If a flip request is within the bounds of NvKmsSetModeOneHeadReply::
     * guaranteedUsage, then clients can expect the flip to succeed.
     * If a flip request is beyond the bounds of NvKmsSetModeOneHeadReply::
     * guaranteedUsage but within NvKmsSetModeOneHeadReply::possibleUsage,
     * then the request may legitimately fail due to insufficient display
     * bandwidth and clients need to be prepared to handle that flip
     * request failure.
     */
    struct NvKmsUsageBounds possibleUsage;

    /*!
     * The guaranteed usage bounds usable on this head.
     */
    struct NvKmsUsageBounds guaranteedUsage;

    /*!
     * Whether NVKMS chose to use headSurface on this head.
     */
    NvBool usingHeadSurface;

    /*!
     * Whether NVKMS enabled VRR on this head.
     */
    NvBool vrrEnabled;

    /*!
     * Contains the 'postSyncObject' that the client requested via
     * NvKmsSetModeOneHeadRequest::flip.
     */
    struct NvKmsFlipCommonReplyOneHead flipReply;
};

enum NvKmsSetModeOneDispStatus {
    NVKMS_SET_MODE_ONE_DISP_STATUS_SUCCESS = 0,
    NVKMS_SET_MODE_ONE_DISP_STATUS_INVALID_REQUESTED_HEADS_BITMASK = 1,
    NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_EXTENDED_GPU_CAPABILITIES_CHECK = 2,
    NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_DISPLAY_PORT_BANDWIDTH_CHECK = 3,
    NVKMS_SET_MODE_ONE_DISP_STATUS_INCOMPATIBLE_DPYS = 4,
    NVKMS_SET_MODE_ONE_DISP_STATUS_DUPLICATE_DPYS = 5,
    NVKMS_SET_MODE_ONE_DISP_STATUS_FAILED_TO_ASSIGN_HARDWARE_HEADS = 6,
};

struct NvKmsSetModeOneDispReply {
    /*!
     * When the NVKMS_IOCTL_SET_MODE succeeds, then this will be SUCCESS.
     * Otherwise, 'status' will be a non-SUCCESS value for one or more
     * heads and/or one or more disps.
     *
     * Note that a failure could occur for a preexisting disp
     * configuration, so this status could be != SUCCESS for a disp
     * not listed in NvKmsSetModeRequest::requestedDispsBitMask.
     */
    enum NvKmsSetModeOneDispStatus status;
    struct NvKmsSetModeOneHeadReply head[NVKMS_MAX_HEADS_PER_DISP];
};

enum NvKmsSetModeStatus {
    NVKMS_SET_MODE_STATUS_SUCCESS = 0,
    NVKMS_SET_MODE_STATUS_INVALID_REQUESTED_DISPS_BITMASK = 1,
    NVKMS_SET_MODE_STATUS_NOT_MODESET_OWNER = 2,
};

struct NvKmsSetModeReply {
    enum NvKmsSetModeStatus status;
    struct NvKmsSetModeOneDispReply disp[NVKMS_MAX_SUBDEVICES];
};

struct NvKmsSetModeParams {
    struct NvKmsSetModeRequest request; /*! in */
    struct NvKmsSetModeReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_CURSOR_IMAGE: Set the cursor image for the
 * specified head.
 */

struct NvKmsSetCursorImageRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;

    struct NvKmsSetCursorImageCommonParams common;
};

struct NvKmsSetCursorImageReply {
    NvU32 padding;
};

struct NvKmsSetCursorImageParams {
    struct NvKmsSetCursorImageRequest request; /*! in */
    struct NvKmsSetCursorImageReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_MOVE_CURSOR: Set the cursor position for the specified
 * head.
 *
 * x,y are relative to the current viewPortIn configured on the head.
 */

struct NvKmsMoveCursorRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;

    struct NvKmsMoveCursorCommonParams common;
};

struct NvKmsMoveCursorReply {
    NvU32 padding;
};

struct NvKmsMoveCursorParams {
    struct NvKmsMoveCursorRequest request; /*! in */
    struct NvKmsMoveCursorReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_LUT: Set the LUT contents for the specified head.
 */

struct NvKmsSetLutRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;

    struct NvKmsSetLutCommonParams common;
};

struct NvKmsSetLutReply {
    NvU32 padding;
};

struct NvKmsSetLutParams {
    struct NvKmsSetLutRequest request; /*! in */
    struct NvKmsSetLutReply reply;     /*! out */
};

/*!
 * NVKMS_IOCTL_CHECK_LUT_NOTIFIER: Check or wait on the LUT notifier for the
 * specified apiHead.
 */

struct NvKmsCheckLutNotifierRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;

    NvBool waitForCompletion;
};

struct NvKmsCheckLutNotifierReply {
    NvBool complete;
};

struct NvKmsCheckLutNotifierParams {
    struct NvKmsCheckLutNotifierRequest request; /*! in */
    struct NvKmsCheckLutNotifierReply reply;     /*! out */
};

/*!
 * NVKMS_IOCTL_IDLE_BASE_CHANNEL: Wait for the base channel to be idle on
 * the requested heads on the requested subdevices of a device.
 *
 * Each (head,sd) pair to be idled is described by:
 *
 *   subDevicesPerHead[head] |= NVBIT(sd)
 */

struct NvKmsIdleBaseChannelRequest {
    NvKmsDeviceHandle deviceHandle;
    NvU32 subDevicesPerHead[NVKMS_MAX_HEADS_PER_DISP];
};

struct NvKmsIdleBaseChannelReply {
    /*!
     * If stopping the base channel is necessary due to a timeout, (head,sd)
     * pairs will be described with:
     *
     *   stopSubDevicesPerHead[head] |= NVBIT(sd)
     *
     * indicating that semaphore releases from the stalled channels may not have
     * occurred.
     */
    NvU32 stopSubDevicesPerHead[NVKMS_MAX_HEADS_PER_DISP];
};

struct NvKmsIdleBaseChannelParams {
    struct NvKmsIdleBaseChannelRequest request; /*! in */
    struct NvKmsIdleBaseChannelReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_FLIP: Flip one or more heads on the subdevices of a device.
 *
 * At least one head must be specified in a flip request, and at most
 * NV_MAX_FLIP_REQUEST_HEADS may be specified.
 */

struct NvKmsFlipRequestOneHead {
    NvU32 sd;
    NvU32 head;
    struct NvKmsFlipCommonParams flip;
};

#define NV_MAX_FLIP_REQUEST_HEADS (NV_MAX_SUBDEVICES * NV_MAX_HEADS)

struct NvKmsFlipRequest {
    NvKmsDeviceHandle deviceHandle;

    /* Pointer to an array of length 'numFlipHeads'; each entry in the array is
     * of type 'struct NvKmsFlipRequestOneHead'. */
    NvU64 pFlipHead NV_ALIGN_BYTES(8);
    NvU32 numFlipHeads;

    /*!
     * When a flip request is made, NVKMS will first perform
     * validation to confirm whether the request can be satisfied.  If
     * the requested configuration cannot be fulfilled, the request
     * returns FALSE.
     *
     * If 'commit' is FALSE, then the status of validation will be returned.
     *
     * If 'commit' is TRUE, and validation passes, then NVKMS will
     * apply the requested configuration.
     */
    NvBool commit;

};

enum NvKmsVrrFlipType {
    NV_KMS_VRR_FLIP_NON_VRR = 0,
    NV_KMS_VRR_FLIP_GSYNC,
    NV_KMS_VRR_FLIP_ADAPTIVE_SYNC,
};

struct NvKmsFlipReply {
    /*!
     * If vrrFlipType != NV_KMS_VRR_FLIP_NON_VRR, then VRR was used for the
     * requested flip. In this case, vrrSemaphoreIndex indicates the index
     * into the VRR semaphore surface that the client should release to
     * trigger the flip.
     *
     * A value of -1 indicates that no VRR semaphore release is needed.
     */
    NvS32 vrrSemaphoreIndex;

    /*!
     * Indicates whether the flip was non-VRR, was a VRR flip on one or more
     * G-SYNC displays, or was a VRR flip exclusively on Adaptive-Sync
     * displays.
     */
    enum NvKmsVrrFlipType vrrFlipType;

    /*!
     * Indicates either success or the reason the flip request failed.
     */
    enum NvKmsFlipResult flipResult;

    /*!
     * Entries correspond to the heads specified in
     * NvKmsFlipRequest::pFlipHead, in the same order.
     */
    struct NvKmsFlipCommonReplyOneHead flipHead[NV_MAX_FLIP_REQUEST_HEADS];
};

struct NvKmsFlipParams {
    struct NvKmsFlipRequest request; /*! in */
    struct NvKmsFlipReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_DECLARE_DYNAMIC_DPY_INTEREST: "Dynamic dpy" reference
 * counting.
 *
 * Most dpys have a lifetime equal to the NVKMS device.  However, some
 * dpys are dynamic and are created and destroyed in response to
 * getting connected or disconnected.  DisplayPort MST dpys are dynamic dpys.
 *
 * When a dynamic dpy is disconnected, its NVDpyId will be freed and
 * made available for use by dynamic dpys connected later, unless any
 * client has declared "interest" in the NVDpyId.  The dynamic NVDpyId
 * will persist as long as a client has declared interest on it, and
 * will be reused for newly connected monitors at the same dynamic dpy
 * address (port address, in the case of DP MST dynamic dpys).
 *
 * The 'interest' field selects interest in the dynamic dpy.
 *
 * If the dynamic dpy has already been disconnected (and therefore
 * removed) before the client has declared interest in it, this ioctl
 * will fail.
 *
 * The recommended usage pattern is:
 *
 * - Declare interest in the event types:
 *     NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED
 *     NVKMS_EVENT_TYPE_DYNAMIC_DPY_DISCONNECTED
 *
 * - When a DYNAMIC_DPY_CONNECTED event is received, call
 *     NVKMS_IOCTL_DECLARE_DYNAMIC_DPY_INTEREST
 *   to declare interest on the dpy.  Be sure to check the return
 *   value, in case the dynamic dpy was already removed.  Update any
 *   client bookkeeping, to start tracking the dpy.
 *
 * - When a DYNAMIC_DPY_DISCONNECTED event is received, update any
 *   client bookkeeping, to stop tracking this dynamic dpy.  Call
 *     NVKMS_IOCTL_DECLARE_DYNAMIC_DPY_INTEREST
 *   to remove interest on the dpy.
 */

struct NvKmsDeclareDynamicDpyInterestRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    NvBool interest;
};

struct NvKmsDeclareDynamicDpyInterestReply {
    NvU32 padding;
};

struct NvKmsDeclareDynamicDpyInterestParams {
    struct NvKmsDeclareDynamicDpyInterestRequest request; /*! in */
    struct NvKmsDeclareDynamicDpyInterestReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_{,UN}REGISTER_SURFACE: Register and unregister an
 * RM-allocated surface with NVKMS.
 *
 * A surface must be registered with NVKMS before NVKMS can display
 * it.  Note that NVKMS will create its own RM object for the registered
 * surface.  The surface will not be freed by resman until the surface
 * is unregistered by the client.
 */

struct NvKmsRegisterSurfaceRequest {
    NvKmsDeviceHandle deviceHandle;

    /*!
     * Surfaces can be specified either by file descriptor or by
     * (rmClient, rmObject) tuple.  useFd indicates which is specified
     * in this request.  Userspace clients are required to specify surface by
     * file descriptor.
     */
    NvBool useFd;

    /*!
     * The RM client handle that was used to allocate the surface.
     * NVKMS will use this as the hClientSrc argument to
     * NvRmDupObject().  Only used when useFd is FALSE.
     */
    NvU32 rmClient;

    /*
     * For multi-plane formats, clients are free to use one memory allocation
     * for all planes, or a separate memory allocation per plane:
     * - For the first usecase, 'rmObject'/'fd' and 'rmObjectSizeInBytes'
     *   should be the same for all planes, and each plane should have a
     *   different 'offset'.
     * - For the second usecase, 'rmObject'/'fd' should be different for each
     *   plane.
     *
     * The 'planes' array is indexed as follows:
     * - For RGB and YUV packed formats, 'planes[0]' refers to the single plane
     *   that's used for these formats.
     * - For YUV semi-planar formats, 'planes[0]' refers to the Y-plane and
     *   'planes[1]' refers to the UV-plane.
     * - For YUV planar formats, 'planes[0]' refers to the Y-plane, 'planes[1]'
     *   refers to the U plane, and 'planes[2]' refers to the V plane.
     */
    struct {

        union {
            NvU32 rmObject; /* RM memory handle */
            NvS32 fd;       /* file descriptor describing memory */
        } u;

        /*
         * This byte offset will be added to the base address of the RM memory
         * allocation, and determines the starting address of this plane within
         * that allocation. This offset must be 1KB-aligned.
         */
        NvU64 offset NV_ALIGN_BYTES(8);

        /*
         * If the surface layout is NvKmsSurfaceMemoryLayoutPitch, then
         * 'pitch' should be the pitch of this plane in bytes, and must
         * have an alignment of 256 bytes.  If the surface layout is
         * NvKmsSurfaceMemoryLayoutBlockLinear, then 'pitch' should be the
         * pitch of this plane in _blocks_.  Blocks are always 64 bytes
         * wide.
         */
        NvU32 pitch;

        /*
         * This is the size of the entire RM memory allocation pointed to by
         * rmObject or fd prior to taking the offset into account. This is
         * _not_ always the size of this plane since a single RM memory
         * allocation can contain multiple planes, and we're also not taking
         * the offset into account.
         */
        NvU64 rmObjectSizeInBytes NV_ALIGN_BYTES(8);
    } planes[NVKMS_MAX_PLANES_PER_SURFACE];

    NvU32 widthInPixels;
    NvU32 heightInPixels;

    enum NvKmsSurfaceMemoryLayout layout;
    enum NvKmsSurfaceMemoryFormat format;

    NvBool noDisplayHardwareAccess;

    /*
     * This flag should be set if the surface can potentially be updated
     * directly on the screen after the flip. For example, this is the case
     * if the surface is CPU mapped, accessible by more than one GPU, or in
     * a similar situation. If this flag is set NVKMS knows not to consider
     * the surface content cacheable between flips.
     */
    NvBool noDisplayCaching;

    /*
     * If isoType == NVKMS_MEMORY_NISO, NVKMS will create CPU and GPU mappings
     * for the surface memory.
     */
    NvKmsMemoryIsoType isoType;

    NvU32 log2GobsPerBlockY;
};

struct NvKmsRegisterSurfaceReply {
    NvKmsSurfaceHandle surfaceHandle;
};

struct NvKmsRegisterSurfaceParams {
    struct NvKmsRegisterSurfaceRequest request; /*! in */
    struct NvKmsRegisterSurfaceReply reply;     /*! out */
};

struct NvKmsUnregisterSurfaceRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSurfaceHandle surfaceHandle;
    /*
     * Normally, when a surface is unregistered, nvkms will sync any
     * outstanding flips to ensure the surface is no longer referenced by
     * display hardware before being torn down.
     *
     * To improve performance with GSP firmware, when checking if this sync is
     * necessary a trusted kernel-mode client who knows it is safe to do so
     * may indicate to nvkms that the sync is unneeded.
     */
    NvBool skipSync;
};

struct NvKmsUnregisterSurfaceReply {
    NvU32 padding;
};

struct NvKmsUnregisterSurfaceParams {
    struct NvKmsUnregisterSurfaceRequest request; /*! in */
    struct NvKmsUnregisterSurfaceReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_GRANT_SURFACE:
 * NVKMS_IOCTL_ACQUIRE_SURFACE:
 * NVKMS_IOCTL_RELEASE_SURFACE:
 *
 * An NVKMS client can "grant" a registered surface to another NVKMS
 * client through the following steps:
 *
 * - The granting NVKMS client should open /dev/nvidia-modeset, and
 *   call NVKMS_IOCTL_GRANT_SURFACE to associate an NvKmsSurfaceHandle
 *   with the file descriptor.
 *
 * - The granting NVKMS client should pass the file descriptor over a
 *   UNIX domain socket to one or more clients who should acquire the
 *   surface.
 *
 * - The granting NVKMS client can optionally close the file
 *   descriptor now or later.
 *
 * - Each acquiring client should call NVKMS_IOCTL_ACQUIRE_SURFACE,
 *   and pass in the file descriptor it received.  This returns an
 *   NvKmsSurfaceHandle that the acquiring client can use to refer to
 *   the surface in any other NVKMS API call that takes an
 *   NvKmsSurfaceHandle.
 *
 * - The acquiring clients can optionally close the file descriptor
 *   now or later.
 *
 * - Each acquiring client should call NVKMS_IOCTL_RELEASE_SURFACE to
 *   release it when they are done with the surface.
 *
 * - When the granting client unregisters the surface, it is
 *   "orphaned": NVKMS will flip away from the surface if necessary,
 *   the RM surface allocation is unduped, and the surface is
 *   unregistered from EVO.  But, the acquiring clients will continue
 *   to hold a reference to this orphaned surface until they release
 *   it.
 *
 * Notes:
 *
 * - It is an error to call NVKMS_IOCTL_GRANT_SURFACE more than once
 *   on a /dev/nvidia-modeset file descriptor, or to use a file
 *   descriptor other than one created by opening /dev/nvidia-modeset,
 *   or to use a file descriptor that was previously used as the first
 *   argument to ioctl(2).
 *
 * - The special handling of surfaces when the granting client
 *   unregisters the surface might be a little asymmetric.  However,
 *   this strikes a balance between:
 *
 *   (a) Making sure modesetting NVKMS clients can free memory when
 *   they intend to.
 *
 *   (b) Making sure acquiring clients don't get a stale view of their
 *   surface handle namespace: if the surface were completely
 *   unregistered out from under them, the surface handle could be
 *   recycled without them knowing.  If they later attempted to
 *   release the original surface, they could inadvertently release a
 *   different surface that happened to have the recycled handle.
 *
 * - Do we need an NVKMS_IOCTL_REVOKE_SURFACE?  Or is the
 *   automatic-unregistration-in-acquiring-clients behavior
 *   sufficient?
 */

struct NvKmsGrantSurfaceRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSurfaceHandle surfaceHandle;
    int fd;
};

struct NvKmsGrantSurfaceReply {
    NvU32 padding;
};

struct NvKmsGrantSurfaceParams {
    struct NvKmsGrantSurfaceRequest request; /*! in */
    struct NvKmsGrantSurfaceReply reply;     /*! out */
};

struct NvKmsAcquireSurfaceRequest {
    int fd;
};

struct NvKmsAcquireSurfaceReply {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSurfaceHandle surfaceHandle;
};

struct NvKmsAcquireSurfaceParams {
    struct NvKmsAcquireSurfaceRequest request; /*! in */
    struct NvKmsAcquireSurfaceReply reply;     /*! out */
};

struct NvKmsReleaseSurfaceRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSurfaceHandle surfaceHandle;
};

struct NvKmsReleaseSurfaceReply {
    NvU32 padding;
};

struct NvKmsReleaseSurfaceParams {
    struct NvKmsReleaseSurfaceRequest request; /*! in */
    struct NvKmsReleaseSurfaceReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_DPY_ATTRIBUTE:
 * NVKMS_IOCTL_GET_DPY_ATTRIBUTE:
 * NVKMS_IOCTL_GET_DPY_ATTRIBUTE_VALID_VALUES:
 *
 * Dpys have several attributes that can be queried and set.
 *
 * An attribute has a type (defined by NvKmsAttributeType), read/write
 * permissions, and potentially other descriptions of its valid
 * values.  Use NVKMS_IOCTL_GET_DPY_ATTRIBUTE_VALID_VALUES to get the
 * valid values of an attribute.
 */

enum NvKmsAttributeType {
    NV_KMS_ATTRIBUTE_TYPE_INTEGER = 0,
    NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    NV_KMS_ATTRIBUTE_TYPE_RANGE,
    NV_KMS_ATTRIBUTE_TYPE_BITMASK,
    NV_KMS_ATTRIBUTE_TYPE_DPY_ID,
    NV_KMS_ATTRIBUTE_TYPE_DPY_ID_LIST,
};

enum NvKmsDpyAttribute {
    NV_KMS_DPY_ATTRIBUTE_BACKLIGHT_BRIGHTNESS = 0,
    NV_KMS_DPY_ATTRIBUTE_SCANLINE,
    NV_KMS_DPY_ATTRIBUTE_HW_HEAD,
    NV_KMS_DPY_ATTRIBUTE_HEAD,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_VIBRANCE,
    NV_KMS_DPY_ATTRIBUTE_IMAGE_SHARPENING,
    NV_KMS_DPY_ATTRIBUTE_IMAGE_SHARPENING_AVAILABLE,
    NV_KMS_DPY_ATTRIBUTE_IMAGE_SHARPENING_DEFAULT,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_RANGE,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_RANGE,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_LINK_RATE,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_LINK_RATE_10MHZ,
    NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG,
    /*
     * XXX NVKMS TODO: Delete UPDATE_GLS_FRAMELOCK; this event-only
     * attribute is a kludge to tell GLS about a change in framelock
     * configuration made by NVKMS.  Eventually, NVKMS should manage
     * framelock itself and GLS shouldn't need to be notified.
     *
     * Note that the event data reports two boolean values: enable
     * (bit 0) and server (bit 1).
     */
    NV_KMS_DPY_ATTRIBUTE_UPDATE_GLS_FRAMELOCK,
    NV_KMS_DPY_ATTRIBUTE_RASTER_LOCK,
    NV_KMS_DPY_ATTRIBUTE_UPDATE_FLIPLOCK,
    NV_KMS_DPY_ATTRIBUTE_UPDATE_STEREO,
    NV_KMS_DPY_ATTRIBUTE_DPMS,
    NV_KMS_DPY_ATTRIBUTE_VRR_MIN_REFRESH_RATE,

    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_IS_MULTISTREAM,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_SINK_IS_AUDIO_CAPABLE,

    NV_KMS_DPY_ATTRIBUTE_NUMBER_OF_HARDWARE_HEADS_USED,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING attribute. */
enum NvKmsDpyAttributeRequestedDitheringValue {
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_AUTO = 0,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_ENABLED = 1,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DISABLED = 2,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE attribute. */
enum NvKmsDpyAttributeRequestedDitheringModeValue {
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_AUTO = 0,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_DYNAMIC_2X2 = 1,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_STATIC_2X2 = 2,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_TEMPORAL = 3,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE attribute. */
enum NvKmsDpyAttributeCurrentDitheringModeValue {
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_NONE = 0,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_DYNAMIC_2X2 = 1,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_STATIC_2X2 = 2,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_TEMPORAL = 3,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH attribute. */
enum NvKmsDpyAttributeRequestedDitheringDepthValue {
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_AUTO = 0,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_6_BITS = 1,
    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_8_BITS = 2,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH attribute. */
enum NvKmsDpyAttributeCurrentDitheringDepthValue {
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_NONE = 0,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_6_BITS = 1,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_8_BITS = 2,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE attribute. */
enum NvKmsDpyAttributeCurrentColorSpaceValue {
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB = 0,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422 = 1,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444 = 2,
    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420 = 3,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL attribute. */
enum NvKmsDpyAttributeDigitalSignalValue {
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_LVDS = 0,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_TMDS = 1,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_DISPLAYPORT = 2,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_HDMI_FRL = 3,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL_DSI = 4,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE attribute. */
enum NvKmsDpyAttributeDigitalLinkTypeValue {
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_SINGLE = 0,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_DUAL = 1,
    NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_QUAD = 3,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG attribute. */
enum NvKmsDpyAttributeFrameLockDisplayConfigValue {
    NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_DISABLED = 0,
    NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_CLIENT = 1,
    NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG_SERVER = 2,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_DPMS attribute. */
enum NvKmsDpyAttributeDpmsValue {
    NV_KMS_DPY_ATTRIBUTE_DPMS_ON,
    NV_KMS_DPY_ATTRIBUTE_DPMS_STANDBY,
    NV_KMS_DPY_ATTRIBUTE_DPMS_SUSPEND,
    NV_KMS_DPY_ATTRIBUTE_DPMS_OFF,
};

/*! Values for the NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE attribute */
enum NvKmsDpyAttributeDisplayportConnectorTypeValue {
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_UNKNOWN = 0,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_DISPLAYPORT,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_HDMI,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_DVI,
    NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_VGA,
};

struct NvKmsSetDpyAttributeRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    enum NvKmsDpyAttribute attribute;
    NvS64 value NV_ALIGN_BYTES(8);
};

struct NvKmsSetDpyAttributeReply {
    NvU32 padding;
};

struct NvKmsSetDpyAttributeParams {
    struct NvKmsSetDpyAttributeRequest request; /*! in */
    struct NvKmsSetDpyAttributeReply reply;     /*! out */
};


struct NvKmsGetDpyAttributeRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    enum NvKmsDpyAttribute attribute;
};

struct NvKmsGetDpyAttributeReply {
    NvS64 value NV_ALIGN_BYTES(8);
};

struct NvKmsGetDpyAttributeParams {
    struct NvKmsGetDpyAttributeRequest request; /*! in */
    struct NvKmsGetDpyAttributeReply reply;     /*! out */
};


struct NvKmsAttributeValidValuesCommonReply {
    NvBool readable;
    NvBool writable;
    enum NvKmsAttributeType type;
    union {
        struct {
            NvS64 min NV_ALIGN_BYTES(8);
            NvS64 max NV_ALIGN_BYTES(8);
        } range; /*! Used when type == NV_KMS_ATTRIBUTE_TYPE_RANGE. */
        struct {
            NvU32 ints;
        } bits;  /*! Used when type == NV_KMS_ATTRIBUTE_TYPE_INTBITS. */
    } u;
};

struct NvKmsGetDpyAttributeValidValuesRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    enum NvKmsDpyAttribute attribute;
};

struct NvKmsGetDpyAttributeValidValuesReply {
    struct NvKmsAttributeValidValuesCommonReply common;
};


struct NvKmsGetDpyAttributeValidValuesParams {
    struct NvKmsGetDpyAttributeValidValuesRequest request; /*! in */
    struct NvKmsGetDpyAttributeValidValuesReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_DISP_ATTRIBUTE:
 * NVKMS_IOCTL_GET_DISP_ATTRIBUTE:
 * NVKMS_IOCTL_GET_DISP_ATTRIBUTE_VALID_VALUES:
 */


enum NvKmsDispAttribute {
    NV_KMS_DISP_ATTRIBUTE_FRAMELOCK = 0,
    NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_SYNC,
    NV_KMS_DISP_ATTRIBUTE_GPU_FRAMELOCK_FPGA_REVISION_UNSUPPORTED,
    NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_STEREO_SYNC,
    NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_TIMING,
    NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_TEST_SIGNAL,
    NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_RESET,
    NV_KMS_DISP_ATTRIBUTE_FRAMELOCK_SET_SWAP_BARRIER,
    NV_KMS_DISP_ATTRIBUTE_QUERY_DP_AUX_LOG,
};


struct NvKmsSetDispAttributeRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    enum NvKmsDispAttribute attribute;
    NvS64 value NV_ALIGN_BYTES(8);
};

struct NvKmsSetDispAttributeReply {
    NvU32 padding;
};

struct NvKmsSetDispAttributeParams {
    struct NvKmsSetDispAttributeRequest request; /*! in */
    struct NvKmsSetDispAttributeReply reply;     /*! out */
};


struct NvKmsGetDispAttributeRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    enum NvKmsDispAttribute attribute;
};

struct NvKmsGetDispAttributeReply {
    NvS64 value NV_ALIGN_BYTES(8);
};

struct NvKmsGetDispAttributeParams {
    struct NvKmsGetDispAttributeRequest request; /*! in */
    struct NvKmsGetDispAttributeReply reply;     /*! out */
};


struct NvKmsGetDispAttributeValidValuesRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    enum NvKmsDispAttribute attribute;
};

struct NvKmsGetDispAttributeValidValuesReply {
    struct NvKmsAttributeValidValuesCommonReply common;
};

struct NvKmsGetDispAttributeValidValuesParams {
    struct NvKmsGetDispAttributeValidValuesRequest request; /*! in */
    struct NvKmsGetDispAttributeValidValuesReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_QUERY_FRAMELOCK: Query information about a framelock
 * device.
 */

struct NvKmsQueryFrameLockRequest {
    NvKmsFrameLockHandle frameLockHandle;
};

struct NvKmsQueryFrameLockReply {
    NvU32 gpuIds[NVKMS_MAX_GPUS_PER_FRAMELOCK];
};

struct NvKmsQueryFrameLockParams {
    struct NvKmsQueryFrameLockRequest request; /*! in */
    struct NvKmsQueryFrameLockReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_FRAMELOCK_ATTRIBUTE:
 * NVKMS_IOCTL_GET_FRAMELOCK_ATTRIBUTE:
 * NVKMS_IOCTL_GET_FRAMELOCK_ATTRIBUTE_VALID_VALUES:
 */

enum NvKmsFrameLockAttribute {
    NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY = 0,
    NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_DELAY,
    NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE,
    NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_INTERVAL,
    NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_READY,
    NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE,
    NV_KMS_FRAMELOCK_ATTRIBUTE_FPGA_REVISION,
    NV_KMS_FRAMELOCK_ATTRIBUTE_FIRMWARE_MAJOR_VERSION,
    NV_KMS_FRAMELOCK_ATTRIBUTE_FIRMWARE_MINOR_VERSION,
    NV_KMS_FRAMELOCK_ATTRIBUTE_BOARD_ID,
    NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_DELAY_RESOLUTION,
    NV_KMS_FRAMELOCK_ATTRIBUTE_PORT0_STATUS,
    NV_KMS_FRAMELOCK_ATTRIBUTE_PORT1_STATUS,
    NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_STATUS,
    NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED,
    NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE,
    NV_KMS_FRAMELOCK_ATTRIBUTE_SYNC_RATE_4,
    NV_KMS_FRAMELOCK_ATTRIBUTE_INCOMING_HOUSE_SYNC_RATE,
    NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_VALUE,
    NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE,
};

/*! Values for the NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY attribute. */
enum NvKmsFrameLockAttributePolarityValue {
    NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_RISING_EDGE  = 0x1,
    NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_FALLING_EDGE = 0x2,
    NV_KMS_FRAMELOCK_ATTRIBUTE_POLARITY_BOTH_EDGES   = 0x3,
};

/*! Values for the NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE attribute. */
enum NvKmsFrameLockAttributeHouseSyncModeValue {
    NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_DISABLED = 0,
    NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_INPUT    = 0x1,
    NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_OUTPUT   = 0x2,
};

/*! Values for the NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED attribute. */
enum NvKmsFrameLockAttributeEthernetDetectedValue {
    NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED_NONE  = 0,
    NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED_PORT0 = 0x1,
    NV_KMS_FRAMELOCK_ATTRIBUTE_ETHERNET_DETECTED_PORT1 = 0x2,
};

/*! Values for the NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE attribute. */
enum NvKmsFrameLockAttributeVideoModeValue {
    NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_AUTO      = 0,
    NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_TTL                 = 1,
    NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_BI_LEVEL  = 2,
    NV_KMS_FRAMELOCK_ATTRIBUTE_VIDEO_MODE_COMPOSITE_TRI_LEVEL = 3,
};

/*! Values for the NV_KMS_FRAMELOCK_ATTRIBUTE_PORT[01]_STATUS attributes. */
enum NvKmsFrameLockAttributePortStatusValue {
    NV_KMS_FRAMELOCK_ATTRIBUTE_PORT_STATUS_INPUT = 0,
    NV_KMS_FRAMELOCK_ATTRIBUTE_PORT_STATUS_OUTPUT = 1,
};

/*! Values for the NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE attribute. */
enum NvKmsFrameLockAttributeMulDivModeValue {
    NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE_MULTIPLY = 0,
    NV_KMS_FRAMELOCK_ATTRIBUTE_MULTIPLY_DIVIDE_MODE_DIVIDE = 1,
};

struct NvKmsSetFrameLockAttributeRequest {
    NvKmsFrameLockHandle frameLockHandle;
    enum NvKmsFrameLockAttribute attribute;
    NvS64 value NV_ALIGN_BYTES(8);
};

struct NvKmsSetFrameLockAttributeReply {
    NvU32 padding;
};

struct NvKmsSetFrameLockAttributeParams {
    struct NvKmsSetFrameLockAttributeRequest request; /*! in */
    struct NvKmsSetFrameLockAttributeReply reply;     /*! out */
};


struct NvKmsGetFrameLockAttributeRequest {
    NvKmsFrameLockHandle frameLockHandle;
    enum NvKmsFrameLockAttribute attribute;
};

struct NvKmsGetFrameLockAttributeReply {
    NvS64 value NV_ALIGN_BYTES(8);
};

struct NvKmsGetFrameLockAttributeParams {
    struct NvKmsGetFrameLockAttributeRequest request; /*! in */
    struct NvKmsGetFrameLockAttributeReply reply;     /*! out */
};


struct NvKmsGetFrameLockAttributeValidValuesRequest {
    NvKmsFrameLockHandle frameLockHandle;
    enum NvKmsFrameLockAttribute attribute;
};

struct NvKmsGetFrameLockAttributeValidValuesReply {
    struct NvKmsAttributeValidValuesCommonReply common;
};

struct NvKmsGetFrameLockAttributeValidValuesParams {
    struct NvKmsGetFrameLockAttributeValidValuesRequest request; /*! in */
    struct NvKmsGetFrameLockAttributeValidValuesReply reply;     /*! out */
};



/*!
 * NVKMS_IOCTL_GET_NEXT_EVENT, NVKMS_IOCTL_DECLARE_EVENT_INTEREST:
 * Event handling.
 *
 * Clients should call NVKMS_IOCTL_DECLARE_EVENT_INTEREST to indicate
 * the events in which they are interested.  Then, block on poll(2) or
 * select(2) until there are events available to read on the file
 * descriptor.
 *
 * When events are available, the client should call
 * NVKMS_IOCTL_GET_NEXT_EVENT to get an NvKmsEvent structure, and
 * interpret the union based on eventType.
 *
 * Clients can remove interest for events by calling
 * NVKMS_IOCTL_DECLARE_EVENT_INTEREST again, specifying a new
 * interestMask.
 *
 * Note that there may still be events queued for the client when the
 * client calls NVKMS_IOCTL_DECLARE_EVENT_INTEREST to change its
 * interestMask.  So, clients should be prepared to ignore unexpected
 * events after calling NVKMS_IOCTL_DECLARE_EVENT_INTEREST.
 */



/*!
 * NVKMS_EVENT_TYPE_DPY_CHANGED
 *
 * When a dpy changes, this event will be generated.  The client
 * should call NVKMS_IOCTL_QUERY_DPY_DYNAMIC_DATA to get an updated
 * NvKmsQueryDpyDynamicDataReply.
 */

struct NvKmsEventDpyChanged {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
};


/*!
 * NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED
 *
 * When a dynamic dpy is connected, this event will be generated.
 */

struct NvKmsEventDynamicDpyConnected {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
};


/*!
 * NVKMS_EVENT_TYPE_DYNAMIC_DPY_DISCONNECTED
 *
 * When a dynamic dpy is disconnected, this event will be generated.
 */

struct NvKmsEventDynamicDpyDisconnected {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
};


/*!
 * NVKMS_EVENT_TYPE_DPY_ATTRIBUTE_CHANGED
 *
 * When a dpy attribute changes, this event will be generated.
 */

struct NvKmsEventDpyAttributeChanged {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    enum NvKmsDpyAttribute attribute;
    NvS64 value NV_ALIGN_BYTES(8);
};


/*!
 * NVKMS_EVENT_TYPE_FRAMELOCK_ATTRIBUTE_CHANGED
 *
 * When a framelock attribute changes, this event will be generated.
 */

struct NvKmsEventFrameLockAttributeChanged {
    NvKmsFrameLockHandle frameLockHandle;
    enum NvKmsFrameLockAttribute attribute;
    NvS64 value NV_ALIGN_BYTES(8);
};


/*!
 * NVKMS_EVENT_TYPE_FLIP_OCCURRED
 *
 * When a client requests a flip and specifies a completion notifier
 * with NvKmsCompletionNotifierDescription::awaken == TRUE, this event
 * will be generated. This event is only delivered to clients with
 * flipping permission.
 */

struct NvKmsEventFlipOccurred {
    NvKmsDeviceHandle deviceHandle;
    /* XXX NVKMS TODO: the dispHandle is currently hard-coded to 0. */
    NvKmsDispHandle dispHandle;
    NvU32 head;
    NvU32 layer;
};


struct NvKmsEvent {
    enum NvKmsEventType eventType;
    union {
        struct NvKmsEventDpyChanged dpyChanged;
        struct NvKmsEventDynamicDpyConnected dynamicDpyConnected;
        struct NvKmsEventDynamicDpyDisconnected dynamicDpyDisconnected;
        struct NvKmsEventDpyAttributeChanged dpyAttributeChanged;
        struct NvKmsEventFrameLockAttributeChanged frameLockAttributeChanged;
        struct NvKmsEventFlipOccurred flipOccurred;
    } u;
};


struct NvKmsGetNextEventRequest {
    NvU32 padding;
};

struct NvKmsGetNextEventReply {
    /*!
     * If an event is available, valid = TRUE and the NvKmsEvent
     * contains the event.  If no event is available, valid = FALSE.
     */
    NvBool valid;
    struct NvKmsEvent event;
};

struct NvKmsGetNextEventParams {
    struct NvKmsGetNextEventRequest request; /*! in */
    struct NvKmsGetNextEventReply reply;     /*! out */
};


struct NvKmsDeclareEventInterestRequest {
    /*!
     * Mask of event types, where each event type is indicated by (1
     * << NVKMS_EVENT_TYPE_).
     */
    NvU32 interestMask;
};

struct NvKmsDeclareEventInterestReply {
    NvU32 padding;
};

struct NvKmsDeclareEventInterestParams {
    struct NvKmsDeclareEventInterestRequest request; /*! in */
    struct NvKmsDeclareEventInterestReply reply;     /*! out */
};

/*!
 * NVKMS_IOCTL_CLEAR_UNICAST_EVENT
 *
 * The events generated through NVKMS_IOCTL_DECLARE_EVENT_INTEREST and
 * NVKMS_IOCTL_GET_NEXT_EVENT are most useful for system-wide events which
 * multiple clients may be interested in.  Clients declare their interest in a
 * collection of event types, and when they are notified that some number of
 * events arrived, they have to query the events from the event queue.
 *
 * In contrast, "Unicast Events" are for use in cases where a client is only
 * interested in a particular type of event on a particular object.
 *
 * To use a Unicast Event:
 *
 * - Create an fd through nvKmsOpen().
 *
 * - Do _not_ use the fd for anything else (the first argument to ioctl(2), the
 *   fd in any of the granting APIs such as NvKmsGrantSurfaceParams::request:fd,
 *   etc).
 *
 * - Pass the fd into an API that allows a unicast event.  E.g.,
 *   NvKmsJoinSwapGroupParams::request::member::unicastEvent::fd
 *
 * - Clear the unicast event with NVKMS_IOCTL_CLEAR_UNICAST_EVENT.
 *
 * - Check if the event arrived; if it hasn't, then wait for the event through
 *   poll(2) or select(2).
 */

struct NvKmsClearUnicastEventRequest {
    int unicastEventFd;
};

struct NvKmsClearUnicastEventReply {
    NvU32 padding;
};

struct NvKmsClearUnicastEventParams {
    struct NvKmsClearUnicastEventRequest request; /*! in */
    struct NvKmsClearUnicastEventReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_LAYER_POSITION: Set the position of the layer
 * for the specified heads on the specified disps.  The layer
 * position is in "desktop coordinate space", i.e., relative to the
 * upper left corner of the input viewport.
 *
 * Note that this is only valid if
 * NvKmsAllocDeviceReply::layerCaps[layer].supportsWindowMode is TRUE.
 */
struct NvKmsSetLayerPositionRequest {
    NvKmsDeviceHandle deviceHandle;

    /*!
     * The bitmask of which indices within disp[] describe requested
     * configuration changes.  Any other disps will use their existing
     * configuration.
     */
    NvU32 requestedDispsBitMask;

    struct {
        /*!
         * The bitmask of which head[] elements to look at on this
         * disp; any other head will use its existing configuration.
         */
        NvU32 requestedHeadsBitMask;

        struct {
            struct NvKmsSignedPoint layerPosition[NVKMS_MAX_LAYERS_PER_HEAD];
            /*!
             * The bitmask of which layerPosition[] elements to look at on this
             * head; any other layer will use its existing configuration.
             */
            NvU32 requestedLayerBitMask;
        } head[NVKMS_MAX_HEADS_PER_DISP];

    } disp[NVKMS_MAX_SUBDEVICES];
};

struct NvKmsSetLayerPositionReply {
    NvU32 padding;
};

struct NvKmsSetLayerPositionParams {
    struct NvKmsSetLayerPositionRequest request; /*! in */
    struct NvKmsSetLayerPositionReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_GRAB_OWNERSHIP:
 * NVKMS_IOCTL_RELEASE_OWNERSHIP:
 *
 * NVKMS_IOCTL_GRAB_OWNERSHIP notifies NVKMS that the calling client wants to
 * control modesets on the device, and NVKMS_IOCTL_RELEASE_OWNERSHIP indicates
 * that the modeset ownership should be released and the VT console mode
 * restored.
 *
 * It is not necessary to call NVKMS_IOCTL_RELEASE_OWNERSHIP during shutdown;
 * NVKMS will implicitly clear modeset ownership in nvKmsClose().
 *
 * Releasing modeset ownership enables console hotplug handling. See the
 * explanation in the comment for enableConsoleHotplugHandling above.
 *
 * If modeset ownership is held by nvidia-drm, then NVKMS_IOCTL_GRAB_OWNERSHIP
 * will fail. Clients should open the corresponding DRM device node, acquire
 * 'master' on it, and then use DRM_NVIDIA_GRANT_PERMISSIONS with permission
 * type NV_DRM_PERMISSIONS_TYPE_SUB_OWNER to acquire sub-owner permission.
 */

struct NvKmsGrabOwnershipRequest {
    NvKmsDeviceHandle deviceHandle;
};

struct NvKmsGrabOwnershipReply {
    NvU32 padding;
};

struct NvKmsGrabOwnershipParams {
    struct NvKmsGrabOwnershipRequest request; /*! in */
    struct NvKmsGrabOwnershipReply reply;     /*! out */
};

struct NvKmsReleaseOwnershipRequest {
    NvKmsDeviceHandle deviceHandle;
};

struct NvKmsReleaseOwnershipReply {
    NvU32 padding;
};

struct NvKmsReleaseOwnershipParams {
    struct NvKmsReleaseOwnershipRequest request; /*! in */
    struct NvKmsReleaseOwnershipReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_GRANT_PERMISSIONS:
 * NVKMS_IOCTL_ACQUIRE_PERMISSIONS:
 * NVKMS_IOCTL_REVOKE_PERMISSIONS:
 *
 * By default, only the modeset owning NVKMS client (the one who
 * successfully called NVKMS_IOCTL_GRAB_OWNERSHIP) is allowed to flip
 * or set modes.
 *
 * However, the modeset owner or another NVKMS client with
 * NV_KMS_PERMISSIONS_TYPE_SUB_OWNER permission can grant various
 * permissions to other clients through the following steps:
 *
 * - The modeset owner should open /dev/nvidia-modeset, and call
 *   NVKMS_IOCTL_GRANT_PERMISSIONS to define a set of permissions
 *   associated with the file descriptor.
 *
 * - The modeset owner should pass the file descriptor over a UNIX
 *   domain socket to one or more clients who should acquire these
 *   permissions.
 *
 * - The modeset owner can optionally close the file descriptor now or
 *   later.
 *
 * - The acquiring clients should call NVKMS_IOCTL_ACQUIRE_PERMISSIONS
 *   and pass in the file descriptor they received, to update their
 *   client connection to include the permissions specified by the modeset
 *   owner in the first bullet.
 *
 * - The acquiring clients can optionally close the file descriptor
 *   now or later.
 *
 * - From this point forward, both the modeset owner and the clients
 *   are allowed to perform the actions allowed by the granted
 *   permissions.
 *
 * - The modeset owner can optionally revoke any previously granted
 *   permissions with NVKMS_IOCTL_REVOKE_PERMISSIONS. This can be 
 *   device-scope for all of a type, or just a set of permissions.
 *   Note that _REVOKE_PERMISSIONS to revoke a set of modeset permissions
 *   will cause the revoked heads to be shut down.
 *
 * Notes:
 *
 * - NvKmsPermissions::disp[n] corresponds to the disp named by
 *   NvKmsAllocDeviceReply::dispHandles[n].
 *
 * - It is an error to call NVKMS_IOCTL_GRANT_PERMISSIONS more than
 *   once on a /dev/nvidia-modeset file descriptor, or to use a file
 *   descriptor other than one created by opening /dev/nvidia-modeset,
 *   or to use a file descriptor that was previously used as the first
 *   argument to ioctl(2).
 *
 * - Calling NVKMS_IOCTL_ACQUIRE_PERMISSIONS more than once on the
 *   same NVKMS client will cause the new permissions for that client
 *   to be the union of the previous permissions and the latest
 *   permissions being acquired.
 */

enum NvKmsPermissionsType {
    NV_KMS_PERMISSIONS_TYPE_FLIPPING = 1,
    NV_KMS_PERMISSIONS_TYPE_MODESET = 2,
    NV_KMS_PERMISSIONS_TYPE_SUB_OWNER = 3,
};

struct NvKmsFlipPermissions {
    struct {
        struct {
            /*
             * Bitmask of flippable layers, where each layer is
             * indicated by '1 << layer'.  It is an error for bits
             * above NVKMS_MAX_LAYERS_PER_HEAD to be set.
             *
             * Only applicable when type==FLIPPING.
             */
            NvU8 layerMask;
        } head[NVKMS_MAX_HEADS_PER_DISP];
    } disp[NVKMS_MAX_SUBDEVICES];
};

struct NvKmsModesetPermissions {
    struct {
        struct {
            /*
             * A list of dpys which a particular NVKMS client is
             * allowed to use when performing a modeset on this head.
             *
             * If the NVKMS client is not allowed to set a mode on
             * this head, this list will be empty.
             *
             * If an NVKMS client can drive the head without
             * restrictions, this will be nvAllDpyIdList().
             *
             * Only applicable when type==MODESET.
             */
            NVDpyIdList dpyIdList;
        } head[NVKMS_MAX_HEADS_PER_DISP];
    } disp[NVKMS_MAX_SUBDEVICES];
};

struct NvKmsPermissions {
    enum NvKmsPermissionsType type;
    union {
        struct NvKmsFlipPermissions flip;
        struct NvKmsModesetPermissions modeset;
    };
};

struct NvKmsGrantPermissionsRequest {
    int fd;
    NvKmsDeviceHandle deviceHandle;
    struct NvKmsPermissions permissions;
};

struct NvKmsGrantPermissionsReply {
    NvU32 padding;
};

struct NvKmsGrantPermissionsParams {
    struct NvKmsGrantPermissionsRequest request; /*! in */
    struct NvKmsGrantPermissionsReply reply;     /*! out */
};

struct NvKmsAcquirePermissionsRequest {
    int fd;
};

struct NvKmsAcquirePermissionsReply {
    /*! This client's handle for the device which acquired new permissions */
    NvKmsDeviceHandle deviceHandle;

    /*!
     * The acquired permissions.
     *
     * If permissions::type == FLIPPING, the new combined flipping
     * permissions of the calling client on this device, including
     * prior permissions and permissions added by this operation.
     */
    struct NvKmsPermissions permissions;
};

struct NvKmsAcquirePermissionsParams {
    struct NvKmsAcquirePermissionsRequest request; /*! in */
    struct NvKmsAcquirePermissionsReply reply;     /*! out */
};

struct NvKmsRevokePermissionsRequest {
    NvKmsDeviceHandle deviceHandle;

    /*
     * A bitmask of permission types to be revoked for this device.
     * It should be the bitwise 'or' of any number of
     * NVBIT(NV_KMS_PERMISSIONS_TYPE_*) values.
     */
    NvU32 permissionsTypeBitmask;

    /*
     * If permissionsTypeBitmask is 0, instead revoke only these permissions.
     */
    struct NvKmsPermissions permissions;
};

struct NvKmsRevokePermissionsReply {
    NvU32 padding;
};

struct NvKmsRevokePermissionsParams {
    struct NvKmsRevokePermissionsRequest request; /*! in */
    struct NvKmsRevokePermissionsReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_QUERY_DPY_CRC32
 *
 * Query last CRC32 value from the NVKMS disp head specified by the triplet
 * (deviceHandle, dispHandle, head).
 */

struct NvKmsQueryDpyCRC32Request {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;
};

/*!
 * Generic CRC-structure type to represent CRC value obtained and if
 * hardware architecture supports collection of the CRC type. If
 * the CRC is not supported by hardware, its value is undefined.
 */
struct NvKmsDpyCRC32 {
   /*!
    * Value of the CRC. If it is not supported, value is undefined.
    */
   NvU32 value;

   /*!
    * Boolean which represents if hardware supports CRC collection
    * If this boolean is FALSE, CRC hardware collection is not supported.
    */
   NvBool supported;
};

/*!
 * Reply structure that contains CRC32 values returned from hardware.
 * Supported CRCs obtained are represented by supported boolean in crc struct
 * Note- Crcs that are not supported will not be updated and will remain at 0
 */
struct NvKmsQueryDpyCRC32Reply {
   /*!
    * CRC generated from the Compositor hardware
    */
    struct NvKmsDpyCRC32 compositorCrc32;

    /*!
     * CRC generated from the RG hardware, if head is driving RG/SF.
     * Note that if Dithering is enabled, this CRC will vary across reads
     * from the same frame.
     */
    struct NvKmsDpyCRC32 rasterGeneratorCrc32;

    /*!
     * Crc value generated from the target SF/OR depending on connector's OR type
     * Note that if Dithering is enabled, this CRC will vary across reads
     * from the same frame.
     */
    struct NvKmsDpyCRC32 outputCrc32;

};

struct NvKmsQueryDpyCRC32Params {
    struct NvKmsQueryDpyCRC32Request request; /*! in */
    struct NvKmsQueryDpyCRC32Reply reply;     /*! out */
};

/*!
 * User-space pointers are always passed to NVKMS in an NvU64.
 * This user-space address is eventually passed into the platform's
 * copyin/copyout functions, in a void* argument.
 *
 * This utility function converts from a pointer to an NvU64.
 */

static inline NvU64 nvKmsPointerToNvU64(const void *ptr)
{
    return (NvU64)(NvUPtr)ptr;
}


/*!
 * NVKMS_IOCTL_REGISTER_DEFERRED_REQUEST_FIFO:
 * NVKMS_IOCTL_UNREGISTER_DEFERRED_REQUEST_FIFO:
 *
 * To make a request that is deferred until after a specific point in a client's
 * graphics channel, a client should register a surface with NVKMS as a
 * "deferred request fifo".  The surface is interpreted as having the layout of
 * struct NvKmsDeferredRequestFifo.
 *
 * To make deferred requests, the client should:
 *
 * - Write the NVKMS_DEFERRED_REQUEST_OPCODE for the desired operation to
 *   NvKmsDeferredRequestFifo::request[i], where 'i' is the next available
 *   element in the request[] array.  Repeat as necessary.
 *
 * - Push NV906F_SEMAPHORE[ABCD] methods in its graphics channel to write
 *   '(i + 1) % NVKMS_MAX_DEFERRED_REQUESTS' to
 *   NvKmsDeferredRequestFifo::put.
 *
 * - Push an NV906F_NON_STALL_INTERRUPT method in its graphics channel.
 *
 * NVKMS will be notified of the non-stall interrupt, and scan all clients'
 * deferred request fifos for put != get.  NVKMS will then perform the requests
 * specified in request[get] through request[put-1].  Finally, NVKMS will update
 * get to indicate how much of the fifo it consumed.
 *
 * Wrapping behaves as expected.  In pseudo code:
 *
 *  while (get != put) {
 *      do(request[get]);
 *      get = (get + 1) % NVKMS_MAX_DEFERRED_REQUESTS;
 *  }
 *
 * The only time it is safe for clients to write to get is when get == put and
 * there are no outstanding semaphore releases to gpuPut.
 *
 * The surface used for the deferred request fifo must be:
 *
 * - In system memory (NVKMS will create one device-scoped mapping, not one per
 *   subdevice, as would be needed if the surface were in video memory).
 *
 * - At least as large as sizeof(NvKmsDeferredRequestFifo).
 *
 * Some NVKMS_DEFERRED_REQUESTs may need to write to a semaphore after some
 * operation is performed (e.g., to indicate that a SwapGroup is ready, or that
 * we've reached vblank).  The NVKMS_DEFERRED_REQUEST_SEMAPHORE_INDEX field
 * within the request specifies a semaphore within the
 * NvKmsDeferredRequestFifo::semaphore[] array.  The semantics of that semaphore
 * index are opcode-specific.
 *
 * The opcode and semaphore index are in the low 16-bits of the request.  The
 * upper 16-bits are opcode-specific.
 */

#define NVKMS_MAX_DEFERRED_REQUESTS 128

#define NVKMS_DEFERRED_REQUEST_OPCODE                                      7:0

#define NVKMS_DEFERRED_REQUEST_SEMAPHORE_INDEX                            15:8

#define NVKMS_DEFERRED_REQUEST_OPCODE_NOP                                    0

/*
 * The SWAP_GROUP_READY request means that this NvKmsDeferredRequestFifo is
 * ready for the next swap of the SwapGroup (see NVKMS_IOCTL_JOIN_SWAP_GROUP,
 * below).  NVKMS_DEFERRED_REQUEST_SEMAPHORE_INDEX should specify an element in
 * the semaphore[] array which will be released to
 *
 *   NVKMS_DEFERRED_REQUEST_SEMAPHORE_VALUE_SWAP_GROUP_READY
 *
 * when the SwapGroup actually swaps.
 */
#define NVKMS_DEFERRED_REQUEST_OPCODE_SWAP_GROUP_READY                       1
#define NVKMS_DEFERRED_REQUEST_SEMAPHORE_VALUE_SWAP_GROUP_NOT_READY 0x00000000
#define NVKMS_DEFERRED_REQUEST_SEMAPHORE_VALUE_SWAP_GROUP_READY     0xFFFFFFFF


/*
 * The SWAP_GROUP_READY_PER_EYE_STEREO field indicates whether this deferred
 * request fifo wants the SwapGroup to present new content at every eye boundary
 * (PER_EYE), or present new content only when transitioning from the right eye
 * to the left eye (PER_PAIR).
 */
#define NVKMS_DEFERRED_REQUEST_SWAP_GROUP_READY_PER_EYE_STEREO           16:16
#define NVKMS_DEFERRED_REQUEST_SWAP_GROUP_READY_PER_EYE_STEREO_PER_PAIR      0
#define NVKMS_DEFERRED_REQUEST_SWAP_GROUP_READY_PER_EYE_STEREO_PER_EYE       1


struct NvKmsDeferredRequestFifo {
    NvU32 put;
    NvU32 get;
    NvU32 request[NVKMS_MAX_DEFERRED_REQUESTS];
    NvGpuSemaphore semaphore[NVKMS_MAX_DEFERRED_REQUESTS];
};

struct NvKmsRegisterDeferredRequestFifoRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSurfaceHandle surfaceHandle;
};

struct NvKmsRegisterDeferredRequestFifoReply {
    NvKmsDeferredRequestFifoHandle deferredRequestFifoHandle;
};

struct NvKmsRegisterDeferredRequestFifoParams {
    struct NvKmsRegisterDeferredRequestFifoRequest request; /*! in */
    struct NvKmsRegisterDeferredRequestFifoReply reply;     /*! out */
};

struct NvKmsUnregisterDeferredRequestFifoRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDeferredRequestFifoHandle deferredRequestFifoHandle;
};

struct NvKmsUnregisterDeferredRequestFifoReply {
    NvU32 padding;
};

struct NvKmsUnregisterDeferredRequestFifoParams {
    struct NvKmsUnregisterDeferredRequestFifoRequest request; /*! in */
    struct NvKmsUnregisterDeferredRequestFifoReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_ALLOC_SWAP_GROUP
 * NVKMS_IOCTL_FREE_SWAP_GROUP
 *
 * An NVKMS client creates a SwapGroup by calling NVKMS_IOCTL_ALLOC_SWAP_GROUP
 * and specifying the heads in the SwapGroup with
 * NvKmsAllocSwapGroupRequest::disp[]::headMask.
 *
 * The SwapGroup can be shared with clients through
 * NVKMS_IOCTL_GRANT_SWAP_GROUP, and it is destroyed once all clients that have
 * acquired the swap group through NVKMS_IOCTL_ACQUIRE_SWAP_GROUP have released
 * it through NVKMS_IOCTL_RELEASE_SWAP_GROUP and when the client that created
 * the swap group has called NVKMS_IOCTL_FREE_SWAP_GROUP or freed the device.
 *
 * The SwapGroup allocation is expected to have a long lifetime (e.g., the X
 * driver might call ALLOC_SWAP_GROUP from ScreenInit and FREE_SWAP_GROUP from
 * CloseScreen).  The point of these requests is to define the head topology of
 * the SwapGroup (for X driver purposes, presumably all the heads that are
 * assigned to the X screen).
 *
 * As such:
 *
 * - Not all heads described in the ALLOC_SWAP_GROUP request need to be active
 *   (they can come and go with different modesets).
 *
 * - The SwapGroup persists across modesets.
 *
 * - SwapGroup allocation is expected to be lightweight: the heavyweight
 *   operations like allocating and freeing headSurface resources are done when
 *   the number of SwapGroup members (see {JOIN,LEAVE}_SWAP_GROUP below)
 *   transitions between 0 and 1.
 *
 * Only an NVKMS modeset owner can alloc or free a SwapGroup.
 */

struct NvKmsSwapGroupConfig {
    struct {
        NvU32 headMask;
    } disp[NVKMS_MAX_SUBDEVICES];
};

struct NvKmsAllocSwapGroupRequest {
    NvKmsDeviceHandle deviceHandle;
    struct NvKmsSwapGroupConfig config;
};

struct NvKmsAllocSwapGroupReply {
    NvKmsSwapGroupHandle swapGroupHandle;
};

struct NvKmsAllocSwapGroupParams {
    struct NvKmsAllocSwapGroupRequest request; /*! in */
    struct NvKmsAllocSwapGroupReply reply;     /*! out */
};

struct NvKmsFreeSwapGroupRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSwapGroupHandle swapGroupHandle;
};

struct NvKmsFreeSwapGroupReply {
    NvU32 padding;
};

struct NvKmsFreeSwapGroupParams {
    struct NvKmsFreeSwapGroupRequest request; /*! in */
    struct NvKmsFreeSwapGroupReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_JOIN_SWAP_GROUP
 * NVKMS_IOCTL_LEAVE_SWAP_GROUP
 *
 * Clients can join NvKmsDeferredRequestFifos to SwapGroups using
 * NVKMS_IOCTL_JOIN_SWAP_GROUP, and remove NvKmsDeferredRequestFifos from
 * SwapGroups using NVKMS_IOCTL_LEAVE_SWAP_GROUP (or freeing the
 * NvKmsDeferredRequestFifo, or freeing the device).
 *
 * Once an NvKmsDeferredRequestFifo is joined to a SwapGroup, the SwapGroup will
 * not become ready again until the SwapGroup member sends the
 * NVKMS_DEFERRED_REQUEST_OPCODE_SWAP_GROUP_READY request through their
 * NvKmsDeferredRequestFifo.  The NVKMS_DEFERRED_REQUEST_SEMAPHORE_INDEX
 * specified as part of the request indicates an index into
 * NvKmsDeferredRequestFifo::semaphore[] where NVKMS will write
 *
 *    NVKMS_DEFERRED_REQUEST_SEMAPHORE_VALUE_SWAP_GROUP_READY
 *
 * when the SwapGroup becomes ready.
 *
 * If unicastEvent::specified is TRUE, then unicastEvent::fd will be interpreted
 * as a unicast event file descriptor.  See NVKMS_IOCTL_CLEAR_UNICAST_EVENT for
 * details.  Whenever SWAP_GROUP_READY is written to a semaphore within
 * NvKmsDeferredRequestFifo, the unicastEvent fd will be notified.
 *
 * An NvKmsDeferredRequestFifo can be joined to at most one SwapGroup at a time.
 *
 * If one client uses multiple NvKmsDeferredRequestFifos joined to multiple
 * SwapGroups and wants to synchronize swaps between these fifos, it should
 * bundle all of the (deviceHandle, swapGroupHandle, deferredRequestFifoHandle)
 * tuples into a single join/leave request.
 *
 * If any client joins multiple NvKmsDeferredRequestFifos to multiple
 * SwapGroups, all NVKMS_IOCTL_JOIN_SWAP_GROUP requests must specify the same
 * set of SwapGroups.
 */

struct NvKmsJoinSwapGroupRequestOneMember {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSwapGroupHandle swapGroupHandle;
    NvKmsDeferredRequestFifoHandle deferredRequestFifoHandle;

    struct {
        int fd;
        NvBool specified;
    } unicastEvent;
};

struct NvKmsJoinSwapGroupRequest {
    NvU32 numMembers;
    struct NvKmsJoinSwapGroupRequestOneMember member[NVKMS_MAX_SWAPGROUPS];
};

struct NvKmsJoinSwapGroupReply {
    NvU32 padding;
};

struct NvKmsJoinSwapGroupParams {
    struct NvKmsJoinSwapGroupRequest request; /*! in */
    struct NvKmsJoinSwapGroupReply reply;     /*! out */
};

struct NvKmsLeaveSwapGroupRequestOneMember {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDeferredRequestFifoHandle deferredRequestFifoHandle;
};

struct NvKmsLeaveSwapGroupRequest {
    NvU32 numMembers;
    struct NvKmsLeaveSwapGroupRequestOneMember member[NVKMS_MAX_SWAPGROUPS];
};

struct NvKmsLeaveSwapGroupReply {
    NvU32 padding;
};

struct NvKmsLeaveSwapGroupParams {
    struct NvKmsLeaveSwapGroupRequest request; /*! in */
    struct NvKmsLeaveSwapGroupReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_SET_SWAP_GROUP_CLIP_LIST
 *
 * The X driver needs to define which pixels on-screen are owned by the
 * SwapGroup.  NVKMS will use this to prevent those pixels from updating until
 * all SwapGroup members indicate that they are ready.
 *
 * The clip list is interpreted by NVKMS as relative to the surface specified
 * during a flip or modeset.  The clip list is intersected with the ViewPortIn
 * of the head, described by
 *
 *   NvKmsFlipCommonParams::viewPortIn::point
 *
 * and
 *
 *   NvKmsSetModeOneHeadRequest::viewPortSizeIn
 *
 * The clip list is exclusive.  I.e., each NvKmsRect is a region outside of the
 * SwapGroup.  One surface-sized NvKmsRect would mean that there are no
 * SwapGroup-owned pixels.
 *
 * When no clip list is specified, NVKMS behaves as if there were no
 * SwapGroup-owned pixels.
 *
 * Only an NVKMS modeset owner can set the clip list of a SwapGroup.
 */

struct NvKmsSetSwapGroupClipListRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSwapGroupHandle swapGroupHandle;

    /*! The number of struct NvKmsRects pointed to by pClipList. */
    NvU16 nClips;

    /*!
     * Pointer to an array of struct NvKmsRects describing the inclusive clip
     * list for the SwapGroup.  The NvKmsRects are in desktop coordinate space.
     *
     * Use nvKmsPointerToNvU64() to assign pClipList.
     */
    NvU64 pClipList NV_ALIGN_BYTES(8);
};

struct NvKmsSetSwapGroupClipListReply {
    NvU32 padding;
};

struct NvKmsSetSwapGroupClipListParams {
    struct NvKmsSetSwapGroupClipListRequest request; /*! in */
    struct NvKmsSetSwapGroupClipListReply reply;     /*! out */
};


/*!
 * NVKMS_IOCTL_GRANT_SWAP_GROUP:
 * NVKMS_IOCTL_ACQUIRE_SWAP_GROUP:
 * NVKMS_IOCTL_RELEASE_SWAP_GROUP:
 *
 * An NVKMS client can "grant" a swap group that it has allocated through
 * NVKMS_IOCTL_ALLOC_SWAP_GROUP to another NVKMS client through the following
 * steps:
 *
 * - The granting NVKMS client should open /dev/nvidia-modeset, and call
 *   NVKMS_IOCTL_GRANT_SWAP_GROUP to associate an NvKmsSwapGroupHandle
 *   with the file descriptor.
 *
 * - The granting NVKMS client should pass the file descriptor over a
 *   UNIX domain socket to one or more clients who should acquire the
 *   swap group.
 *
 * - The granting NVKMS client can optionally close the file
 *   descriptor now or later.
 *
 * - Each acquiring client should call NVKMS_IOCTL_ACQUIRE_SWAP_GROUP,
 *   and pass in the file descriptor it received.  This returns an
 *   NvKmsSwapGroupHandle that the acquiring client can use to refer to
 *   the swap group in any other NVKMS API call that takes an
 *   NvKmsSwapGroupHandle.
 *
 * - The acquiring clients can optionally close the file descriptor
 *   now or later.
 *
 * - Each acquiring client should call NVKMS_IOCTL_RELEASE_SWAP_GROUP to
 *   release it when they are done with the swap group.
 */

struct NvKmsGrantSwapGroupRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSwapGroupHandle swapGroupHandle;
    int fd;
};

struct NvKmsGrantSwapGroupReply {
    NvU32 padding;
};

struct NvKmsGrantSwapGroupParams {
    struct NvKmsGrantSwapGroupRequest request; /*! in */
    struct NvKmsGrantSwapGroupReply reply;     /*! out */
};

struct NvKmsAcquireSwapGroupRequest {
    int fd;
};

struct NvKmsAcquireSwapGroupReply {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSwapGroupHandle swapGroupHandle;
};

struct NvKmsAcquireSwapGroupParams {
    struct NvKmsAcquireSwapGroupRequest request; /*! in */
    struct NvKmsAcquireSwapGroupReply reply;     /*! out */
};

struct NvKmsReleaseSwapGroupRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsSwapGroupHandle swapGroupHandle;
};

struct NvKmsReleaseSwapGroupReply {
    NvU32 padding;
};

struct NvKmsReleaseSwapGroupParams {
    struct NvKmsReleaseSwapGroupRequest request; /*! in */
    struct NvKmsReleaseSwapGroupReply reply;     /*! out */
};

/*!
 * NVKMS_IOCTL_SWITCH_MUX:
 *
 * Switch the mux for the given Dpy in the given direction. The mux switch is
 * performed in three stages.
 */

enum NvKmsMuxOperation {
    NVKMS_SWITCH_MUX_PRE,
    NVKMS_SWITCH_MUX,
    NVKMS_SWITCH_MUX_POST,
};

struct NvKmsSwitchMuxRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
    enum NvKmsMuxOperation operation;
    NvMuxState state;
};

struct NvKmsSwitchMuxReply {
    NvU32 padding;
};

struct NvKmsSwitchMuxParams {
    struct NvKmsSwitchMuxRequest request;
    struct NvKmsSwitchMuxReply reply;
};

struct NvKmsGetMuxStateRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NVDpyId dpyId;
};

struct NvKmsGetMuxStateReply {
    NvMuxState state;
};

struct NvKmsGetMuxStateParams {
    struct NvKmsGetMuxStateRequest request;
    struct NvKmsGetMuxStateReply reply;
};

/*!
 * NVKMS_IOCTL_EXPORT_VRR_SEMAPHORE_SURFACE:
 *
 * Export the VRR semaphore surface onto the provided RM 'memFd'.
 * The RM memory FD should be "empty".  An empty FD can be allocated by calling
 * NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD with 'EMPTY_FD' set.
 */

struct NvKmsExportVrrSemaphoreSurfaceRequest {
    NvKmsDeviceHandle deviceHandle;
    int memFd;
};

struct NvKmsExportVrrSemaphoreSurfaceReply {
    NvU32 padding;
};

struct NvKmsExportVrrSemaphoreSurfaceParams {
    struct NvKmsExportVrrSemaphoreSurfaceRequest request;
    struct NvKmsExportVrrSemaphoreSurfaceReply reply;
};

/*!
 * NVKMS_IOCTL_ENABLE_VBLANK_SYNC_OBJECT:
 * NVKMS_IOCTL_DISABLE_VBLANK_SYNC_OBJECT:
 *
 * The NVKMS client can use NVKMS_IOCTL_ENABLE_VBLANK_SYNC_OBJECT to request a
 * vblank syncpt that continuously triggers each time the raster generator
 * reaches the start of vblank. NVKMS will return the syncpt id in
 * 'NvKmsEnableVblankSyncObjectReply::syncptId'.
 *
 * The NVKMS client can use NVKMS_IOCTL_DISABLE_VBLANK_SYNC_OBJECT to disable
 * the vblank syncpt.
 *
 * If a vblank syncpt is currently enabled on a head, and a modeset request is
 * issued to reconfigure that head with a new set of mode timings, NVKMS will
 * automatically reenable the vblank syncpt so it continues to trigger with the
 * new mode timings.
 *
 * Clients can use these IOCTLs only if both NvKmsAllocDeviceReply::
 * supportsVblankSyncObjects and NvKmsAllocDeviceReply::supportsSyncpts are
 * TRUE.
 */

struct NvKmsEnableVblankSyncObjectRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;
};

struct NvKmsEnableVblankSyncObjectReply {
    /*
     * Clients should explicitly disable the vblank sync object to consume the
     * handle.
     */
    NvKmsVblankSyncObjectHandle vblankHandle;

    NvU32 syncptId;
};

struct NvKmsEnableVblankSyncObjectParams {
    struct NvKmsEnableVblankSyncObjectRequest request; /*! in */
    struct NvKmsEnableVblankSyncObjectReply reply;     /*! out */
};

struct NvKmsDisableVblankSyncObjectRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;

    /* This handle is received in NVKMS_IOCTL_ENABLE_VBLANK_SYNC_OBJECT. */
    NvKmsVblankSyncObjectHandle vblankHandle;
};

struct NvKmsDisableVblankSyncObjectReply {
    NvU32 padding;
};

struct NvKmsDisableVblankSyncObjectParams {
    struct NvKmsDisableVblankSyncObjectRequest request; /*! in */
    struct NvKmsDisableVblankSyncObjectReply reply;     /*! out */
};

/*!
 * NVKMS_IOCTL_NOTIFY_VBLANK:
 *
 * Register a unicast event fd to be notified when the next vblank event occurs
 * on the specified head. This is a one-shot notification, and in order to be
 * notified of subsequent vblank events the caller must clear and re-register
 * the unicast event fd.
 */

struct NvKmsNotifyVblankRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 head;

    struct {
        int fd;
    } unicastEvent;
};

struct NvKmsNotifyVblankReply {
    NvU32 padding;
};

struct NvKmsNotifyVblankParams {
    struct NvKmsNotifyVblankRequest request; /*! in */
    struct NvKmsNotifyVblankReply reply;     /*! out */
};

/*!
 * NVKMS_IOCTL_SET_FLIPLOCK_GROUP:
 *
 * This ioctl specifies a set of active heads on which fliplock is allowed.
 * The heads can span devices.
 * The requested state for this group will be maintained until:
 * a) A subsequent SetFlipLockGroup ioctl that specifies any of the heads
 * b) A subsequent ModeSet ioctl that specifies any of the heads
 * If either of those occurs, the requested state will be destroyed for *all*
 * of the heads.
 *
 * Note that a successful call with 'enable = TRUE' only indicates that the
 * request to enable fliplock is registered, not that fliplock was actually
 * enabled.  Fliplock may not be enabled due to incompatible modetimings, for
 * example.
 */

struct NvKmsSetFlipLockGroupOneDev {
    NvKmsDeviceHandle deviceHandle;

    NvU32 requestedDispsBitMask;
    struct {
        NvU32 requestedHeadsBitMask;
    } disp[NVKMS_MAX_SUBDEVICES];
};

struct NvKmsSetFlipLockGroupRequest {
    NvBool enable;
    struct NvKmsSetFlipLockGroupOneDev dev[NV_MAX_SUBDEVICES];
};

struct NvKmsSetFlipLockGroupReply {
    NvU32 padding;
};

struct NvKmsSetFlipLockGroupParams {
    struct NvKmsSetFlipLockGroupRequest request; /*! in */
    struct NvKmsSetFlipLockGroupReply reply;     /*! out */
};

/*
 * NVKMS_IOCTL_ENABLE_VBLANK_SEM_CONTROL
 * NVKMS_IOCTL_DISABLE_VBLANK_SEM_CONTROL
 * NVKMS_IOCTL_ACCEL_VBLANK_SEM_CONTROLS
 *
 * The VBlank Semaphore Control API ("VBlank Sem Control") allows clients to
 * register for a semaphore release to be performed on the specified system
 * memory.
 *
 * One or more clients may register a memory allocation + offset by specifying
 * an NvKmsSurfaceHandle and offset within that surface.  Until the
 * vblank_sem_control is disabled, during each vblank on all enabled heads,
 * nvkms will interpret the specified memory location as an
 * NvKmsVblankSemControlData data structure.  Each enabled head will inspect the
 * corresponding NvKmsVblankSemControlDataOneHead at
 * NvKmsVblankSemControlData::head[head].
 *
 * NvKmsEnableVblankSemControlRequest::surfaceOffset must be a multiple of 8, so
 * that GPU semaphore releases can write to 8-byte fields within
 * NvKmsVblankSemControlDataOneHead with natural alignment.
 *
 * During vblank, the NvKmsVblankSemControlDataOneHead::requestCounter field
 * will be read, and the following pseudocode will be performed:
 *
 *   swapInterval      = DRF_VAL(data->flags)
 *
 *   if (data->requestCounter == prevRequestCounter)
 *       return
 *
 *   if (currentVblankCount < (prevVBlankCount + swapInterval))
 *       return
 *
 *   data->vblankCount    = currentVblankCount
 *   data->semaphore      = data->requestCounter
 *
 *   prevRequestCounter   = data->requestCounter
 *   previousVblankCount  = currentVblankCount
 *
 * I.e., if the client-described conditions are met, nvkms will write
 * NvKmsVblankSemControlDataOneHead::semaphore to the client-requested
 * 'requestCounter' along with the vblankCount.
 *
 * The intent is for clients to use semaphore releases to write:
 *
 *   NvKmsVblankSemControlDataOneHead::swapInterval
 *   NvKmsVblankSemControlDataOneHead::requestCounter
 *
 * and then perform a semaphore acquire on
 * NvKmsVblankSemControlDataOneHead::semaphore >= requestCounter (using the
 * ACQ_GEQ semaphore operation).  This will block any following methods in the
 * client's channel (e.g., a blit) until the requested conditions are met.  Note
 * the ::requestCounter should be written last, because the change in value of
 * ::requestCounter is what causes nvkms, during a vblank callback, to inspect
 * the other fields.
 *
 * Additionally, clients should use the CPU (not semaphore releases in their
 * channel) to write the field
 * NvKmsVblankSemControlDataOneHead::requestCounterAccel at the same time that
 * they enqueue the semaphore release to write to
 * NvKmsVblankSemControlDataOneHead::requestCounter.  ::requestCounterAccel will
 * be used by nvkms to "accelerate" the vblank sem control by copying the value
 * from ::requestCounterAccel to ::semaphore.  This will be done when the vblank
 * sem control is disabled, and when a client calls
 * NVKMS_IOCTL_ACCEL_VBLANK_SEM_CONTROLS.  It is important for nvkms to have
 * access to the value in ::requestCounterAccel, and not just ::requestCounter.
 * The latter is only the last value released so far by the client's channel
 * (further releases to ::requestCounter may still be inflight, perhaps blocked
 * on pending semaphore acquires).  The former should be the most recent value
 * enqueued in the channel.  This is also why it is important for clients to
 * acquire with ACQ_GEQ (greater-than-or-equal-to), rather than just ACQUIRE.
 *
 * The same NvKmsSurfaceHandle (with different surfaceOffsets) may be used by
 * multiple VBlank Sem Controls.
 *
 * It is the responsibility of the nvkms client(s) to coordinate at modeset
 * time: the mapping of nvkms apiHeads to underlying hwHeads may change during a
 * modeset, such that a registered vblank sem control will no longer receive
 * vblank callbacks if the head is shutdown.  Before a modeset shuts down a
 * head, nvkms clients should ensure that all in-flight semaphore acquires are
 * satisfied.
 *
 * NVKMS_IOCTL_ACCEL_VBLANK_SEM_CONTROLS can be used, specifying a particular
 * set of heads, to set all vblank sem controls on those heads to have their
 * semaphore set to the value in their respective
 * NvKmsVblankSemControlDataOneHead::requestCounterAccel fields.
 *
 * These ioctls are only available when
 * NvKmsAllocDeviceReply::supportsVblankSemControl is true.
 */

struct NvKmsEnableVblankSemControlRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvKmsSurfaceHandle surfaceHandle;
    NvU64 surfaceOffset NV_ALIGN_BYTES(8);
};

struct NvKmsEnableVblankSemControlReply {
    NvKmsVblankSemControlHandle vblankSemControlHandle;
};

struct NvKmsEnableVblankSemControlParams {
    struct NvKmsEnableVblankSemControlRequest request;
    struct NvKmsEnableVblankSemControlReply reply;
};

struct NvKmsDisableVblankSemControlRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvKmsVblankSemControlHandle vblankSemControlHandle;
};

struct NvKmsDisableVblankSemControlReply {
    NvU32 padding;
};

struct NvKmsDisableVblankSemControlParams {
    struct NvKmsDisableVblankSemControlRequest request;
    struct NvKmsDisableVblankSemControlReply reply;
};

struct NvKmsAccelVblankSemControlsRequest {
    NvKmsDeviceHandle deviceHandle;
    NvKmsDispHandle dispHandle;
    NvU32 headMask;
};

struct NvKmsAccelVblankSemControlsReply {
    NvU32 padding;
};

struct NvKmsAccelVblankSemControlsParams {
    struct NvKmsAccelVblankSemControlsRequest request;
    struct NvKmsAccelVblankSemControlsReply reply;
};

/*!
 * NVKMS_IOCTL_VRR_SIGNAL_SEMAPHORE
 *
 * This IOCTL is used to signal a semaphore from VRR semaphore surface.
 * It should be invoked after flip if needed. If device does not supports
 * VRR semaphores, then this is a no-op action for compatibility.
 */
struct NvKmsVrrSignalSemaphoreRequest {
    NvKmsDeviceHandle deviceHandle;
    NvS32 vrrSemaphoreIndex;
};

struct NvKmsVrrSignalSemaphoreReply {
    NvU32 padding;
};

struct NvKmsVrrSignalSemaphoreParams {
    struct NvKmsVrrSignalSemaphoreRequest request; /*! in */
    struct NvKmsVrrSignalSemaphoreReply reply;     /*! out */
};

/*
 * NVKMS_IOCTL_FRAMEBUFFER_CONSOLE_DISABLED
 *
 * Notify NVKMS that the calling client has disabled the framebuffer console.
 * NVKMS will free the framebuffer console reserved memory and disable
 * NVKMS-based console restore.
 *
 * This IOCTL can only be used by kernel-mode clients.
 */

struct NvKmsFramebufferConsoleDisabledRequest {
    NvKmsDeviceHandle deviceHandle;
};

struct NvKmsFramebufferConsoleDisabledReply {
    NvU32 padding;
};

struct NvKmsFramebufferConsoleDisabledParams {
    struct NvKmsFramebufferConsoleDisabledRequest request;
    struct NvKmsFramebufferConsoleDisabledReply reply;
};

#endif /* NVKMS_API_H */
