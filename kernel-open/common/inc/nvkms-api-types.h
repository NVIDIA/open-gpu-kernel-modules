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

#if !defined(NVKMS_API_TYPES_H)
#define NVKMS_API_TYPES_H

#include <nvtypes.h>
#include <nvmisc.h>
#include <nvlimits.h>

#define NVKMS_MAX_SUBDEVICES                  NV_MAX_SUBDEVICES
#define NVKMS_MAX_HEADS_PER_DISP              NV_MAX_HEADS

#define NVKMS_LEFT                            0
#define NVKMS_RIGHT                           1
#define NVKMS_MAX_EYES                        2

#define NVKMS_MAIN_LAYER                      0
#define NVKMS_OVERLAY_LAYER                   1
#define NVKMS_MAX_LAYERS_PER_HEAD             8

#define NVKMS_MAX_PLANES_PER_SURFACE          3

#define NVKMS_DP_ADDRESS_STRING_LENGTH        64

#define NVKMS_DEVICE_ID_TEGRA                 0x0000ffff

#define NVKMS_MAX_SUPERFRAME_VIEWS            4

#define NVKMS_LOG2_LUT_ARRAY_SIZE             10
#define NVKMS_LUT_ARRAY_SIZE                  (1 << NVKMS_LOG2_LUT_ARRAY_SIZE)

#define NVKMS_OLUT_FP_NORM_SCALE_DEFAULT      0xffffffff

typedef NvU32 NvKmsDeviceHandle;
typedef NvU32 NvKmsDispHandle;
typedef NvU32 NvKmsConnectorHandle;
typedef NvU32 NvKmsSurfaceHandle;
typedef NvU32 NvKmsFrameLockHandle;
typedef NvU32 NvKmsDeferredRequestFifoHandle;
typedef NvU32 NvKmsSwapGroupHandle;
typedef NvU32 NvKmsVblankSyncObjectHandle;
typedef NvU32 NvKmsVblankSemControlHandle;

struct NvKmsSize {
    NvU16 width;
    NvU16 height;
};

struct NvKmsPoint {
    NvU16 x;
    NvU16 y;
};

struct NvKmsSignedPoint {
    NvS16 x;
    NvS16 y;
};

struct NvKmsRect {
    NvU16 x;
    NvU16 y;
    NvU16 width;
    NvU16 height;
};

/*
 * A 3x3 row-major matrix.
 *
 * The elements are 32-bit single-precision IEEE floating point values.  The
 * floating point bit pattern should be stored in NvU32s to be passed into the
 * kernel.
 */
struct NvKmsMatrix {
    NvU32 m[3][3];
};

typedef enum {
    NVKMS_CONNECTOR_TYPE_DP             = 0,
    NVKMS_CONNECTOR_TYPE_VGA            = 1,
    NVKMS_CONNECTOR_TYPE_DVI_I          = 2,
    NVKMS_CONNECTOR_TYPE_DVI_D          = 3,
    NVKMS_CONNECTOR_TYPE_ADC            = 4,
    NVKMS_CONNECTOR_TYPE_LVDS           = 5,
    NVKMS_CONNECTOR_TYPE_HDMI           = 6,
    NVKMS_CONNECTOR_TYPE_USBC           = 7,
    NVKMS_CONNECTOR_TYPE_DSI            = 8,
    NVKMS_CONNECTOR_TYPE_DP_SERIALIZER  = 9,
    NVKMS_CONNECTOR_TYPE_UNKNOWN        = 10,
    NVKMS_CONNECTOR_TYPE_MAX            = NVKMS_CONNECTOR_TYPE_UNKNOWN,
} NvKmsConnectorType;

static inline
const char *NvKmsConnectorTypeString(const NvKmsConnectorType connectorType)
{
    switch (connectorType) {
    case NVKMS_CONNECTOR_TYPE_DP:              return "DP";
    case NVKMS_CONNECTOR_TYPE_VGA:             return "VGA";
    case NVKMS_CONNECTOR_TYPE_DVI_I:           return "DVI-I";
    case NVKMS_CONNECTOR_TYPE_DVI_D:           return "DVI-D";
    case NVKMS_CONNECTOR_TYPE_ADC:             return "ADC";
    case NVKMS_CONNECTOR_TYPE_LVDS:            return "LVDS";
    case NVKMS_CONNECTOR_TYPE_HDMI:            return "HDMI";
    case NVKMS_CONNECTOR_TYPE_USBC:            return "USB-C";
    case NVKMS_CONNECTOR_TYPE_DSI:             return "DSI";
    case NVKMS_CONNECTOR_TYPE_DP_SERIALIZER:   return "DP-SERIALIZER";
    default: break;
    }
    return "Unknown";
}

typedef enum {
    NVKMS_CONNECTOR_SIGNAL_FORMAT_VGA     = 0,
    NVKMS_CONNECTOR_SIGNAL_FORMAT_LVDS    = 1,
    NVKMS_CONNECTOR_SIGNAL_FORMAT_TMDS    = 2,
    NVKMS_CONNECTOR_SIGNAL_FORMAT_DP      = 3,
    NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI     = 4,
    NVKMS_CONNECTOR_SIGNAL_FORMAT_UNKNOWN = 5,
    NVKMS_CONNECTOR_SIGNAL_FORMAT_MAX     =
      NVKMS_CONNECTOR_SIGNAL_FORMAT_UNKNOWN,
} NvKmsConnectorSignalFormat;

/*!
 * Description of Notifiers and Semaphores (Non-isochronous (NISO) surfaces).
 *
 * When flipping, the client can optionally specify a notifier and/or
 * a semaphore to use with the flip.  The surfaces used for these
 * should be registered with NVKMS to get an NvKmsSurfaceHandle.
 *
 * NvKmsNIsoSurface::offsetInWords indicates the starting location, in
 * 32-bit words, within the surface where EVO should write the
 * notifier or semaphore.  Note that only the first 4096 bytes of a
 * surface can be used by semaphores or notifiers; offsetInWords must
 * allow for the semaphore or notifier to be written within the first
 * 4096 bytes of the surface.  I.e., this must be satisfied:
 *
 *   ((offsetInWords * 4) + elementSizeInBytes) <= 4096
 *
 * Where elementSizeInBytes is:
 *
 *  if NISO_FORMAT_FOUR_WORD*, elementSizeInBytes = 16
 *  if NISO_FORMAT_LEGACY,
 *    if overlay && notifier, elementSizeInBytes = 16
 *    else, elementSizeInBytes = 4
 *
 *  Note that different GPUs support different semaphore and notifier formats.
 *  Check NvKmsAllocDeviceReply::validNIsoFormatMask to determine which are
 *  valid for the given device.
 *
 *  Note also that FOUR_WORD and FOUR_WORD_NVDISPLAY are the same size, but
 *  FOUR_WORD uses a format compatible with display class 907[ce], and
 *  FOUR_WORD_NVDISPLAY uses a format compatible with c37e (actually defined by
 *  the NV_DISP_NOTIFIER definition in clc37d.h).
 */
enum NvKmsNIsoFormat {
    NVKMS_NISO_FORMAT_LEGACY,
    NVKMS_NISO_FORMAT_FOUR_WORD,
    NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY,
};

enum NvKmsEventType {
    NVKMS_EVENT_TYPE_DPY_CHANGED,
    NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED,
    NVKMS_EVENT_TYPE_DYNAMIC_DPY_DISCONNECTED,
    NVKMS_EVENT_TYPE_DPY_ATTRIBUTE_CHANGED,
    NVKMS_EVENT_TYPE_FRAMELOCK_ATTRIBUTE_CHANGED,
    NVKMS_EVENT_TYPE_FLIP_OCCURRED,
};

enum NvKmsFlipResult {
    NV_KMS_FLIP_RESULT_SUCCESS = 0,    /* Success */
    NV_KMS_FLIP_RESULT_INVALID_PARAMS, /* Parameter validation failed */
    NV_KMS_FLIP_RESULT_IN_PROGRESS,    /* Flip would fail because an outstanding
                                          flip containing changes that cannot be
                                          queued is in progress */
};

typedef enum {
    NV_EVO_SCALER_1TAP      = 0,
    NV_EVO_SCALER_2TAPS     = 1,
    NV_EVO_SCALER_3TAPS     = 2,
    NV_EVO_SCALER_5TAPS     = 3,
    NV_EVO_SCALER_8TAPS     = 4,
    NV_EVO_SCALER_TAPS_MIN  = NV_EVO_SCALER_1TAP,
    NV_EVO_SCALER_TAPS_MAX  = NV_EVO_SCALER_8TAPS,
} NVEvoScalerTaps;

/* This structure describes the scaling bounds for a given layer. */
struct NvKmsScalingUsageBounds {
    /*
     * Maximum vertical downscale factor (scaled by 1024)
     *
     * For example, if the downscale factor is 1.5, then maxVDownscaleFactor
     * would be 1.5 x 1024 = 1536.
     */
    NvU16 maxVDownscaleFactor;

    /*
     * Maximum horizontal downscale factor (scaled by 1024)
     *
     * See the example above for maxVDownscaleFactor.
     */
    NvU16 maxHDownscaleFactor;

    /* Maximum vertical taps allowed */
    NVEvoScalerTaps vTaps;

    /* Whether vertical upscaling is allowed */
    NvBool vUpscalingAllowed;
};

struct NvKmsUsageBounds {
    struct {
        NvBool usable;
        struct NvKmsScalingUsageBounds scaling;
        NvU64 supportedSurfaceMemoryFormats NV_ALIGN_BYTES(8);
    } layer[NVKMS_MAX_LAYERS_PER_HEAD];
};

/*!
 * Per-component arrays of NvU16s describing the LUT; used for both the input
 * LUT and output LUT.
 */
struct NvKmsLutRamps {
    NvU16 red[NVKMS_LUT_ARRAY_SIZE];   /*! in */
    NvU16 green[NVKMS_LUT_ARRAY_SIZE]; /*! in */
    NvU16 blue[NVKMS_LUT_ARRAY_SIZE];  /*! in */
};

/* Datatypes for LUT capabilities */
enum NvKmsLUTFormat {
    /*
     * Normalized fixed-point format mapping [0, 1] to [0x0, 0xFFFF].
     */
    NVKMS_LUT_FORMAT_UNORM16,

    /*
     * Half-precision floating point.
     */
    NVKMS_LUT_FORMAT_FP16,

    /*
     * 14-bit fixed-point format required to work around hardware bug 813188.
     *
     * To convert from UNORM16 to UNORM14_WAR_813188:
     * unorm14_war_813188 = ((unorm16 >> 2) & ~7) + 0x6000
     */
    NVKMS_LUT_FORMAT_UNORM14_WAR_813188
};

enum NvKmsLUTVssSupport {
    NVKMS_LUT_VSS_NOT_SUPPORTED,
    NVKMS_LUT_VSS_SUPPORTED,
    NVKMS_LUT_VSS_REQUIRED,
};

enum NvKmsLUTVssType {
    NVKMS_LUT_VSS_TYPE_NONE,
    NVKMS_LUT_VSS_TYPE_LINEAR,
    NVKMS_LUT_VSS_TYPE_LOGARITHMIC,
};

struct NvKmsLUTCaps {
    /*! Whether this layer or head on this device supports this LUT stage. */
    NvBool supported;

    /*! Whether this LUT supports VSS. */
    enum NvKmsLUTVssSupport vssSupport;

    /*!
     * The type of VSS segmenting this LUT uses.
     */
    enum NvKmsLUTVssType vssType;

    /*!
     * Expected number of VSS segments.
     */
    NvU32 vssSegments;

    /*!
     * Expected number of LUT entries.
     */
    NvU32 lutEntries;

    /*!
     * Format for each of the LUT entries.
     */
    enum NvKmsLUTFormat entryFormat;
};

/* each LUT entry uses this many bytes */
#define NVKMS_LUT_CAPS_LUT_ENTRY_SIZE (4 * sizeof(NvU16))

/* if the LUT surface uses VSS, size of the VSS header */
#define NVKMS_LUT_VSS_HEADER_SIZE (4 * NVKMS_LUT_CAPS_LUT_ENTRY_SIZE)

struct NvKmsLUTSurfaceParams {
    NvKmsSurfaceHandle surfaceHandle;
    NvU64 offset NV_ALIGN_BYTES(8);
    NvU32 vssSegments;
    NvU32 lutEntries;
};

/*
 * A 3x4 row-major colorspace conversion matrix.
 *
 * The output color C' is the CSC matrix M times the column vector
 * [ R, G, B, 1 ].
 *
 * Each entry in the matrix is a signed 2's-complement fixed-point number with
 * 3 integer bits and 16 fractional bits.
 */
struct NvKmsCscMatrix {
    NvS32 m[3][4];
};

#define NVKMS_IDENTITY_CSC_MATRIX   \
    (struct NvKmsCscMatrix){{       \
        { 0x10000, 0, 0, 0 },       \
        { 0, 0x10000, 0, 0 },       \
        { 0, 0, 0x10000, 0 }        \
    }}

/*!
 * A color key match bit used in the blend equations and one can select the src
 * or dst Color Key when blending. Assert key bit means match, de-assert key
 * bit means nomatch.
 *
 * The src Color Key means using the key bit from the current layer, the dst
 * Color Key means using key bit from the previous layer composition stage. The
 * src or dst key bit will be inherited by blended pixel for the preparation of
 * next blending, as dst Color Key.
 *
 * src: Forward the color key match bit from the current layer pixel to next layer
 * composition stage.
 *
 * dst: Forward the color key match bit from the previous composition stage
 * pixel to next layer composition stage.
 *
 * disable: Forward “1” to the next layer composition stage as the color key.
 */
enum NvKmsCompositionColorKeySelect {
    NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE = 0,
    NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC,
    NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST,
};

#define NVKMS_COMPOSITION_NUMBER_OF_COLOR_KEY_SELECTS 3

/*!
 * Composition modes used for surfaces in general.
 * The various types of composition are:
 *
 * Opaque: source pixels are opaque regardless of alpha,
 * and will occlude the destination pixel.
 *
 * Alpha blending: aka opacity, which could be specified
 * for a surface in its entirety, or on a per-pixel basis.
 *
 * Non-premultiplied: alpha value applies to source pixel,
 * and also counter-weighs the destination pixel.
 * Premultiplied: alpha already applied to source pixel,
 * so it only counter-weighs the destination pixel.
 *
 * Color keying: use a color key structure to decide
 * the criteria for matching and compositing.
 * (See NVColorKey below.)
 */
enum NvKmsCompositionBlendingMode {
    /*!
     * Modes that use no other parameters.
     */
    NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE,

    /*!
     * Mode that ignores both per-pixel alpha provided
     * by client and the surfaceAlpha, makes source pixel
     * totally transparent.
     */
    NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT,

    /*!
     * Modes that use per-pixel alpha provided by client,
     * and the surfaceAlpha must be set to 0.
     */
    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA,
    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA,

    /*!
     * These use both the surface-wide and per-pixel alpha values.
     * surfaceAlpha is treated as numerator ranging from 0 to 255
     * of a fraction whose denominator is 255.
     */
    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA,
    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA,
};

static inline NvBool
NvKmsIsCompositionModeUseAlpha(enum NvKmsCompositionBlendingMode mode)
{
    return mode == NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA ||
           mode == NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA ||
           mode == NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA ||
           mode == NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA;
}

/*!
 * Abstract description of a color key.
 *
 * a, r, g, and b are component values in the same width as the framebuffer
 * values being scanned out.
 *
 * match[ARGB] defines whether that component is considered when matching the
 * color key -- TRUE means that the value of the corresponding component must
 * match the given value for the given pixel to be considered a 'key match';
 * FALSE means that the value of that component is not a key match criterion.
 */
typedef struct {
    NvU16 a, r, g, b;
    NvBool matchA, matchR, matchG, matchB;
} NVColorKey;

/*!
 * Describes the composition parameters for the single layer.
 */
struct NvKmsCompositionParams {
    enum NvKmsCompositionColorKeySelect colorKeySelect;
    NVColorKey colorKey;
    /*
     * It is possible to assign different blending mode for match pixels and
     * nomatch pixels. blendingMode[0] is used to blend a pixel with the color key
     * match bit "0", and blendingMode[1] is used to blend a pixel with the color
     * key match bit "1".
     *
     * But because of the hardware restrictions match and nomatch pixels can
     * not use blending mode PREMULT_ALPHA, NON_PREMULT_ALPHA,
     * PREMULT_SURFACE_ALPHA, and NON_PREMULT_SURFACE_ALPHA at once.
     */
    enum NvKmsCompositionBlendingMode blendingMode[2];
    NvU8 surfaceAlpha; /* Applies to all pixels of entire surface */
    /*
     * Defines the composition order. A smaller value moves the layer closer to
     * the top (away from the background). No need to pick consecutive values,
     * requirements are that the value should be different for each of the
     * layers owned by the head and the value for the main layer should be
     * the greatest one.
     *
     * Cursor always remains at the top of all other layers, this parameter
     * has no effect on cursor. NVKMS assigns default depth to each of the
     * supported layers, by default depth of the layer is calculated as
     * (NVKMS_MAX_LAYERS_PER_HEAD - index of the layer). If depth is set to
     * '0' then default depth value will get used.
     */
    NvU8 depth;
};

/*!
 * Describes the composition capabilities supported by the hardware for
 * cursor or layer. It describes supported the color key selects and for each
 * of the supported color key selects it describes supported blending modes
 * for match and nomatch  pixles.
 */
struct NvKmsCompositionCapabilities {

    struct {
        /*
         * A bitmask of the supported blending modes for match and nomatch
         * pixels. It should be the bitwise 'or' of one or more
         * NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_*) values.
         */
        NvU32 supportedBlendModes[2];
    } colorKeySelect[NVKMS_COMPOSITION_NUMBER_OF_COLOR_KEY_SELECTS];

    /*
     * A bitmask of the supported color key selects.
     *
     * It should be the bitwise 'or' of one or more
     * NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_*)
     * values.
     */
    NvU32 supportedColorKeySelects;
};

struct NvKmsLayerCapabilities {
    /*!
     * Whether Layer supports the window mode. If window mode is supported,
     * then clients can set the layer's dimensions so that they're smaller than
     * the viewport, and can also change the output position of the layer to a
     * non-(0, 0) position.
     *
     * NOTE: Dimension changes are currently unsupported for the main layer,
     * and output position changes for the main layer are currently only
     * supported via IOCTL_SET_LAYER_POSITION but not via flips. Support for
     * these is coming soon, via changes to flip code.
     */
    NvBool supportsWindowMode              :1;

    /*!
     * Whether layer supports ICtCp pipe.
     */
    NvBool supportsICtCp                   :1;


    /*!
     * Describes the supported Color Key selects and blending modes for
     * match and nomatch layer pixels.
     */
    struct NvKmsCompositionCapabilities composition;

    /*!
     * Which NvKmsSurfaceMemoryFormat enum values are supported by the NVKMS
     * device on the given scanout surface layer.
     *
     * Iff a particular enum NvKmsSurfaceMemoryFormat 'value' is supported,
     * then (1 << value) will be set in the appropriate bitmask.
     *
     * Note that these bitmasks just report the static SW/HW capabilities,
     * and are a superset of the formats that IMP may allow. Clients are
     * still expected to honor the NvKmsUsageBounds for each head.
     */
    NvU64 supportedSurfaceMemoryFormats NV_ALIGN_BYTES(8);

    /* Capabilities for each LUT stage in the EVO3 precomp pipeline. */
    struct NvKmsLUTCaps ilut;
    struct NvKmsLUTCaps tmo;
};

/*!
 * Surface layouts.
 *
 * BlockLinear is the NVIDIA GPU native tiling format, arranging pixels into
 * blocks or tiles for better locality during common GPU operations.
 *
 * Pitch is the naive "linear" surface layout with pixels laid out sequentially
 * in memory line-by-line, optionally with some padding at the end of each line
 * for alignment purposes.
 */
enum NvKmsSurfaceMemoryLayout {
    NvKmsSurfaceMemoryLayoutBlockLinear = 0,
    NvKmsSurfaceMemoryLayoutPitch = 1,
};

static inline const char *NvKmsSurfaceMemoryLayoutToString(
    enum NvKmsSurfaceMemoryLayout layout)
{
    switch (layout) {
        default:
            return "Unknown";
        case NvKmsSurfaceMemoryLayoutBlockLinear:
            return "BlockLinear";
        case NvKmsSurfaceMemoryLayoutPitch:
            return "Pitch";
    }
}

typedef enum {
    MUX_STATE_GET = 0,
    MUX_STATE_INTEGRATED = 1,
    MUX_STATE_DISCRETE = 2,
    MUX_STATE_UNKNOWN = 3,
} NvMuxState;

enum NvKmsRotation {
    NVKMS_ROTATION_0   = 0,
    NVKMS_ROTATION_90  = 1,
    NVKMS_ROTATION_180 = 2,
    NVKMS_ROTATION_270 = 3,
    NVKMS_ROTATION_MIN = NVKMS_ROTATION_0,
    NVKMS_ROTATION_MAX = NVKMS_ROTATION_270,
};

struct NvKmsRRParams {
    enum NvKmsRotation rotation;
    NvBool reflectionX;
    NvBool reflectionY;
};

/*!
 * Convert each possible NvKmsRRParams to a unique integer [0..15],
 * so that we can describe possible NvKmsRRParams with an NvU16 bitmask.
 * 
 * E.g.
 * rotation = 0, reflectionX = F, reflectionY = F == 0|0|0 == 0
 * ...
 * rotation = 270, reflectionX = T, reflectionY = T == 3|4|8 == 15
 */
static inline NvU8 NvKmsRRParamsToCapBit(const struct NvKmsRRParams *rrParams)
{
    NvU8 bitPosition = (NvU8)rrParams->rotation;
    if (rrParams->reflectionX) {
        bitPosition |= NVBIT(2);
    }
    if (rrParams->reflectionY) {
        bitPosition |= NVBIT(3);
    }
    return bitPosition;
}

/*
 * NVKMS_MEMORY_ISO is used to tag surface memory that will be accessed via
 * display's isochronous interface. Examples of this type of memory are pixel
 * data and LUT entries.
 *
 * NVKMS_MEMORY_NISO is used to tag surface memory that will be accessed via
 * display's non-isochronous interface. Examples of this type of memory are
 * semaphores and notifiers.
 */
typedef enum {
    NVKMS_MEMORY_ISO = 0,
    NVKMS_MEMORY_NISO = 1,
} NvKmsMemoryIsoType;

typedef struct {
    NvBool coherent;
    NvBool noncoherent;
} NvKmsDispIOCoherencyModes;

enum NvKmsInputColorRange {
    /*
     * If DEFAULT is provided, driver will assume full range for RGB formats
     * and limited range for YUV formats.
     */
    NVKMS_INPUT_COLOR_RANGE_DEFAULT = 0,

    NVKMS_INPUT_COLOR_RANGE_LIMITED = 1,

    NVKMS_INPUT_COLOR_RANGE_FULL = 2,
};

enum NvKmsInputColorSpace {
    /* Unknown colorspace */
    NVKMS_INPUT_COLOR_SPACE_NONE = 0,

    NVKMS_INPUT_COLOR_SPACE_BT601 = 1,
    NVKMS_INPUT_COLOR_SPACE_BT709 = 2,
    NVKMS_INPUT_COLOR_SPACE_BT2020 = 3,
    NVKMS_INPUT_COLOR_SPACE_BT2100 = NVKMS_INPUT_COLOR_SPACE_BT2020,

    NVKMS_INPUT_COLOR_SPACE_SCRGB = 4
};

enum NvKmsInputTf {
    NVKMS_INPUT_TF_LINEAR = 0,
    NVKMS_INPUT_TF_PQ = 1
};

enum NvKmsOutputColorimetry {
    NVKMS_OUTPUT_COLORIMETRY_DEFAULT = 0,

    NVKMS_OUTPUT_COLORIMETRY_BT2100 = 1,
};

enum NvKmsOutputTf {
    /*
     * NVKMS itself won't apply any OETF (clients are still
     * free to provide a custom OLUT)
     */
    NVKMS_OUTPUT_TF_NONE = 0,
    NVKMS_OUTPUT_TF_TRADITIONAL_GAMMA_SDR = 1,
    NVKMS_OUTPUT_TF_PQ = 2,
};

/*!
 * EOTF Data Byte 1 as per CTA-861-G spec.
 * This is expected to match exactly with the spec.
 */
enum NvKmsInfoFrameEOTF {
    NVKMS_INFOFRAME_EOTF_SDR_GAMMA = 0,
    NVKMS_INFOFRAME_EOTF_HDR_GAMMA = 1,
    NVKMS_INFOFRAME_EOTF_ST2084 = 2,
    NVKMS_INFOFRAME_EOTF_HLG = 3,
};

/*!
 * HDR Static Metadata Type1 Descriptor as per CEA-861.3 spec.
 * This is expected to match exactly with the spec.
 */
struct NvKmsHDRStaticMetadata {
    /*!
     * Color primaries of the data.
     * These are coded as unsigned 16-bit values in units of 0.00002,
     * where 0x0000 represents zero and 0xC350 represents 1.0000.
     */
    struct {
        NvU16 x, y;
    } displayPrimaries[3];

    /*!
     * White point of colorspace data.
     * These are coded as unsigned  16-bit values in units of 0.00002,
     * where 0x0000 represents zero and 0xC350 represents 1.0000.
     */
    struct {
        NvU16 x, y;
    } whitePoint;

    /**
     * Maximum mastering display luminance.
     * This value is coded as an unsigned 16-bit value in units of 1 cd/m2,
     * where 0x0001 represents 1 cd/m2 and 0xFFFF represents 65535 cd/m2.
     */
    NvU16 maxDisplayMasteringLuminance;

    /*!
     * Minimum mastering display luminance.
     * This value is coded as an unsigned 16-bit value in units of
     * 0.0001 cd/m2, where 0x0001 represents 0.0001 cd/m2 and 0xFFFF
     * represents 6.5535 cd/m2.
     */
    NvU16 minDisplayMasteringLuminance;

    /*!
     * Maximum content light level.
     * This value is coded as an unsigned 16-bit value in units of 1 cd/m2,
     * where 0x0001 represents 1 cd/m2 and 0xFFFF represents 65535 cd/m2.
     */
    NvU16 maxCLL;

    /*!
     * Maximum frame-average light level.
     * This value is coded as an unsigned 16-bit value in units of 1 cd/m2,
     * where 0x0001 represents 1 cd/m2 and 0xFFFF represents 65535 cd/m2.
     */
    NvU16 maxFALL;
};

/*!
 * A superframe is made of two or more video streams that are combined in
 * a specific way. A DP serializer (an external device connected to a Tegra
 * ARM SOC over DP or HDMI) can receive a video stream comprising multiple
 * videos combined into a single frame and then split it into multiple
 * video streams. The following structure describes the number of views
 * and dimensions of each view inside a superframe.
 */
struct NvKmsSuperframeInfo {
    NvU8 numViews;
    struct {
        /* x offset inside superframe at which this view starts */
        NvU16 x;

        /* y offset inside superframe at which this view starts */
        NvU16 y;

        /* Horizontal active width in pixels for this view */
        NvU16 width;

        /* Vertical active height in lines for this view */
        NvU16 height;
    } view[NVKMS_MAX_SUPERFRAME_VIEWS];
};

/* Fields within NvKmsVblankSemControlDataOneHead::flags */
#define NVKMS_VBLANK_SEM_CONTROL_SWAP_INTERVAL          15:0

struct NvKmsVblankSemControlDataOneHead {
    NvU32 requestCounterAccel;
    NvU32 requestCounter;
    NvU32 flags;

    NvU32 semaphore;
    NvU64 vblankCount NV_ALIGN_BYTES(8);
};

struct NvKmsVblankSemControlData {
    struct NvKmsVblankSemControlDataOneHead head[NV_MAX_HEADS];
};

#endif /* NVKMS_API_TYPES_H */
