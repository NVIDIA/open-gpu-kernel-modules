/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-modepool.h"
#include "nvkms-types.h"
#include "nvkms-dpy.h"
#include "nvkms-hdmi.h"
#include "nvkms-utils.h"
#include "nvkms-3dvision.h"
#include "nvkms-evo.h"
#include "nvkms-ioctl.h"
#include "nvkms-modetimings.h"

#include "nv_mode_timings_utils.h"
#include "nv_vasprintf.h"

#include "nvkms-prealloc.h"

#include "nvkms-api.h"

typedef struct {
    enum NvKmsModeSource source;
    NvBool patchedStereoTimings;
} EvoValidateModeFlags;

static NvBool
ValidateModeIndexEdid(NVDpyEvoPtr pDpyEvo,
                      const struct NvKmsModeValidationParams *pParams,
                      struct NvKmsValidateModeIndexReply *pReply,
                      NVEvoInfoStringPtr pInfoString,
                      const NvU32 requestedModeIndex,
                      NvU32 *pCurrentModeIndex);
static NvBool
ValidateModeIndexVesa(NVDpyEvoPtr pDpyEvo,
                      const struct NvKmsModeValidationParams *pParams,
                      struct NvKmsValidateModeIndexReply *pReply,
                      NVEvoInfoStringPtr pInfoString,
                      const NvU32 requestedModeIndex,
                      NvU32 *pCurrentModeIndex);

static void LogModeValidationEnd(const NVDispEvoRec *pDispEvo,
                                 NVEvoInfoStringPtr pInfoString,
                                 const char *failureReasonFormat, ...)
    __attribute__ ((format (printf, 3, 4)));

static NvBool ConstructModeTimingsMetaData(
    NVDpyEvoRec *pDpyEvo,
    const struct NvKmsModeValidationParams *pParams,
    struct NvKmsMode *pKmsMode,
    EvoValidateModeFlags *pFlags,
    NVT_VIDEO_INFOFRAME_CTRL *pInfoFrameCtrl);

static NvBool ValidateMode(NVDpyEvoPtr pDpyEvo,
                           const struct NvKmsMode *pKmsMode,
                           const EvoValidateModeFlags *flags,
                           const struct NvKmsModeValidationParams *pParams,
                           NVEvoInfoStringPtr pInfoString,
                           struct NvKmsModeValidationValidSyncs *pValidSyncs,
                           struct NvKmsUsageBounds *pModeUsage);

#define NV_MAX_MODE_NAME_LEN 64
#define NV_MAX_MODE_DESCRIPTION_LEN 128

/* A single frequency, at its longest, will have the format: "aaa.bbb" */
#define NV_MAX_FREQUENCY_STRING_LEN 8

/* A range element, at its longest, will have the format: "aaa.bbb-ccc.ddd, " */
#define NV_MAX_RANGE_ELEMENT_STRING_LEN 18
#define NV_MAX_RANGE_STRING_LEN \
    (NV_MAX_RANGE_ELEMENT_STRING_LEN * NVKMS_MAX_VALID_SYNC_RANGES)


void
nvValidateModeIndex(NVDpyEvoPtr pDpyEvo,
                    const struct NvKmsValidateModeIndexRequest *pRequest,
                    struct NvKmsValidateModeIndexReply *pReply)
{
    const struct NvKmsModeValidationParams *pParams = &pRequest->modeValidation;
    const NvU32 requestedModeIndex = pRequest->modeIndex;
    NVEvoInfoStringRec infoString;
    NvU32 currentModeIndex = 0;
    NvBool done;

    nvkms_memset(pReply, 0, sizeof(*pReply));

    nvInitInfoString(&infoString, nvKmsNvU64ToPointer(pRequest->pInfoString),
                     pRequest->infoStringSize);

    done = ValidateModeIndexEdid(pDpyEvo, pParams, pReply, &infoString,
                                 requestedModeIndex, &currentModeIndex);
    if (done) {
        goto out;
    }

    done = ValidateModeIndexVesa(pDpyEvo, pParams, pReply, &infoString,
                                 requestedModeIndex, &currentModeIndex);
    if (done) {
        goto out;
    }

    pReply->end = 1;
    return;

out:
    if (pRequest->infoStringSize > 0) {
        /* Add 1 for the final '\0' */
        nvAssert((infoString.length + 1) <= pRequest->infoStringSize);
        pReply->infoStringLenWritten = infoString.length + 1;
    }
}


void
nvValidateModeEvo(NVDpyEvoPtr pDpyEvo,
                  const struct NvKmsValidateModeRequest *pRequest,
                  struct NvKmsValidateModeReply *pReply)
{
    NVEvoInfoStringRec infoString;
    struct NvKmsMode kmsMode = {
        .timings = pRequest->mode.timings,
    };
    EvoValidateModeFlags evoFlags;
    NVT_VIDEO_INFOFRAME_CTRL dummyInfoFrameCtrl;

    nvkms_memset(pReply, 0, sizeof(*pReply));

    if (!ConstructModeTimingsMetaData(pDpyEvo,
                                      &pRequest->modeValidation,
                                      &kmsMode,
                                      &evoFlags,
                                      &dummyInfoFrameCtrl)) {
        pReply->valid = FALSE;
        return;
    }

    nvInitInfoString(&infoString, nvKmsNvU64ToPointer(pRequest->pInfoString),
                     pRequest->infoStringSize);

    pReply->valid = ValidateMode(pDpyEvo,
                                 &kmsMode,
                                 &evoFlags,
                                 &pRequest->modeValidation,
                                 &infoString,
                                 &pReply->validSyncs,
                                 &pReply->modeUsage);

    if (infoString.length > 0) {
        /* Add 1 for the final '\0' */
        nvAssert((infoString.length + 1) <= pRequest->infoStringSize);
        pReply->infoStringLenWritten = infoString.length + 1;
    }
}


/*!
 * Determine whether this mode is HDMI 3D by checking the HDMI 3D
 * support map parsed from the CEA-861 EDID extension.
 *
 * Currently only frame packed 3D modes are supported, as we rely on
 * Kepler's HW support for this mode.
 *
 * If hdmi 3D is supported, then only one of hdmi3D or hdmi3DAvailable
 * will be returned true, based on if it was requested.
 */
static void GetHdmi3DValue(const NVDpyEvoRec *pDpyEvo,
                           const struct NvKmsModeValidationParams *pParams,
                           const NVT_TIMING *pTiming, 
                           NvBool *hdmi3D,
                           NvBool *hdmi3DAvailable)
{
    /* This should only be used in paths where we have a valid parsed EDID. */

    nvAssert(pDpyEvo->parsedEdid.valid);

    if ((NVT_GET_TIMING_STATUS_TYPE(pTiming->etc.status) ==
         NVT_TYPE_EDID_861ST) &&
        nvDpyEvoSupportsHdmi3D(pDpyEvo)) {

        const NVT_EDID_INFO *pInfo = &pDpyEvo->parsedEdid.info;
        int i;

        for (i = 0; i < pInfo->Hdmi3Dsupport.total; i++) {
            HDMI3DDETAILS hdmi3DMap = pInfo->Hdmi3Dsupport.map[i];
            NvU32 vic = NVT_GET_TIMING_STATUS_SEQ(pTiming->etc.status);
            if ((vic == hdmi3DMap.Vic) &&
                (hdmi3DMap.StereoStructureMask &
                 NVT_HDMI_3D_SUPPORTED_FRAMEPACK_MASK)) {
                *hdmi3D = pParams->stereoMode == NVKMS_STEREO_HDMI_3D;
                *hdmi3DAvailable = pParams->stereoMode != NVKMS_STEREO_HDMI_3D;
                return;
            }
        }
    }

    *hdmi3D = FALSE;
    *hdmi3DAvailable = FALSE;
}

/*
 * DP 1.3 decimated YUV 4:2:0 mode is required if:
 *
 * - The GPU and monitor both support it.
 * - Either the monitor doesn't support RGB 4:4:4 scanout of this mode, or
 *   the user prefers YUV 4:2:0 scanout when possible.
 */
static NvBool DpYuv420Required(const NVDpyEvoRec *pDpyEvo,
                               const struct NvKmsModeValidationParams *pParams,
                               const NVT_TIMING *pTiming)
{
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;
    const NvBool monitorSupports444 =
        IS_BPC_SUPPORTED_COLORFORMAT(pTiming->etc.rgb444.bpcs);

    if (!pDevEvo->caps.supportsDP13) {
        // The GPU doesn't support YUV420.
        return FALSE;
    }

    if (monitorSupports444) {
        // The GPU and monitor both support YUV420 and RGB444; use RGB444
        // by default, but allow the user to prefer YUV420 mode in this
        // decision.
        return pParams->preferYUV420;
    } else {
        // The GPU and monitor both support YUV420, and the monitor doesn't
        // support RGB444, so we have to fall back to YUV420.
        return TRUE;
    }
}

/*
 * Return whether this mode requires SW, HW, or no YUV 4:2:0 compression given
 * this GPU, display, connector type, and user preference.
 */
static enum NvYuv420Mode GetYUV420Value(
    const NVDpyEvoRec *pDpyEvo,
    const struct NvKmsModeValidationParams *pParams,
    const NVT_TIMING *pTiming)
{
    if (!IS_BPC_SUPPORTED_COLORFORMAT(pTiming->etc.yuv420.bpcs) ||
        ((pTiming->HSyncWidth & 1) != 0) ||
        ((pTiming->HFrontPorch & 1) != 0) ||
        ((pTiming->HVisible & 1) != 0) ||
        ((pTiming->HTotal & 1) != 0) ||
        ((pTiming->VVisible & 1) != 0)) {
        // If this mode doesn't support YUV420, then the GPU caps or
        // user preference doesn't matter.
        return NV_YUV420_MODE_NONE;
    } else if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        if (DpYuv420Required(pDpyEvo, pParams, pTiming)) {
            return NV_YUV420_MODE_SW;
        } else {
            return NV_YUV420_MODE_NONE;
        }
    } else if (nvDpyIsHdmiEvo(pDpyEvo)) {
        /*
         * YUV 4:2:0 compression is necessary for HDMI 2.0 4K@60hz modes
         * unless the GPU and display both support HDMI 2.0 4K@60hz
         * uncompressed RGB 4:4:4 (6G mode). A mode validation override
         * may be used to allow RGB 4:4:4 mode if the GPU supports it
         * even if the display doesn't claim support in the EDID.
         */
        if (!nvHdmi204k60HzRGB444Allowed(pDpyEvo, pParams, pTiming) ||
            pParams->preferYUV420) {

            const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
            const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
            // XXX assume the heads have equal capabilities
            // XXX assume the gpus have equal capabilities
            NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[0];
            NVEvoCapabilitiesPtr pEvoCaps = &pEvoSubDev->capabilities;
            NVEvoHeadCaps *pHeadCaps = &pEvoCaps->head[0];

            if (pHeadCaps->supportsHDMIYUV420HW) {
                return NV_YUV420_MODE_HW;
            } else {
                return NV_YUV420_MODE_SW;
            }
        } else {
            return NV_YUV420_MODE_NONE;
        }
    } else {
        return NV_YUV420_MODE_NONE;
    }
}


/*!
 * Scan through the EDID-specified modes, counting each one.  If the
 * count reaches requestedModeIndex, then validate that mode.
 *
 * \param[in]     pDpyEvo  The dpy whose EDID's modes are considered.
 * \param[in]     pParams  The NvKmsModeValidationParams.
 * \param[out]    pReply   The NvKmsValidateModeIndexReply; if we found
 *                         requestedModeIndex, pReply->valid will store if
 *                         the mode was valid.
 * \param[in]     requestedModeIndex  The index of the mode we are looking for.
 * \param[in,out] pCurrentModeIndex   A running total of the number of modes
 *                         we have considered.  This will be incremented
 *                         by the number of modes considered.
 *
 * \return        If we found the mode with index == requestedModeIndex,
 *                return TRUE.  Otherwise, return FALSE.
 */
static NvBool
ValidateModeIndexEdid(NVDpyEvoPtr pDpyEvo,
                      const struct NvKmsModeValidationParams *pParams,
                      struct NvKmsValidateModeIndexReply *pReply,
                      NVEvoInfoStringPtr pInfoString,
                      const NvU32 requestedModeIndex,
                      NvU32 *pCurrentModeIndex)
{
    const char *description;
    int i;
    NvBool is3DVisionStereo = nvIs3DVisionStereoEvo(pParams->stereoMode);

    /* if no EDID, we have nothing to do here */

    if (!pDpyEvo->parsedEdid.valid) {
        return FALSE;
    }

    /* Scan through all EDID modes. */

    for (i = 0; i < pDpyEvo->parsedEdid.info.total_timings; i++) {

        NVT_TIMING timing = pDpyEvo->parsedEdid.info.timing[i];
        EvoValidateModeFlags flags;
        struct NvKmsMode kmsMode = { };
        NvBool hdmi3D = FALSE;

        /* Skip this mode if it was marked invalid by nvtiming. */

        if (timing.etc.status == 0) {
            continue;
        }

        /*
         * If *pCurrentModeIndex matches requestedModeIndex, then
         * validate the mode.  Otherwise, go on to the next mode.
         */
        if (*pCurrentModeIndex != requestedModeIndex) {
            (*pCurrentModeIndex)++;
            continue;
        }

        nvkms_memset(&flags, 0, sizeof(flags));
        flags.source = NvKmsModeSourceEdid;

        /* patch the mode for 3DVision */
        if (is3DVisionStereo &&
            pDpyEvo->stereo3DVision.requiresModetimingPatching &&
            nvPatch3DVisionModeTimingsEvo(&timing, pDpyEvo, pInfoString)) {
            flags.patchedStereoTimings = TRUE;
        }

        if ((NVT_GET_TIMING_STATUS_TYPE(timing.etc.status) ==
             NVT_TYPE_EDID_861ST) &&
            (NVT_GET_CEA_FORMAT(timing.etc.status) > 0) &&
            (timing.etc.name[0] != '\0')) {
            description = (const char *) timing.etc.name;
        } else {
            description = NULL;
        }

        /* convert from the EDID's NVT_TIMING to NvModeTimings */

        NVT_TIMINGtoNvModeTimings(&timing, &kmsMode.timings);

        /*
         * Determine whether this mode is a HDMI 3D by checking the HDMI 3D
         * support map parsed from the CEA-861 EDID extension.
         *
         * Currently only frame packed 3D modes are supported, as we rely on
         * Kepler's HW support for this mode.
         */
        GetHdmi3DValue(pDpyEvo, pParams, &timing, &hdmi3D,
                       &pReply->hdmi3DAvailable);
        nvKmsUpdateNvModeTimingsForHdmi3D(&kmsMode.timings, hdmi3D);

        kmsMode.timings.yuv420Mode = GetYUV420Value(pDpyEvo, pParams, &timing);

        /* validate the mode */

        pReply->valid = ValidateMode(pDpyEvo,
                                     &kmsMode,
                                     &flags,
                                     pParams,
                                     pInfoString,
                                     &pReply->validSyncs,
                                     &pReply->modeUsage);

        /*
         * The client did not request hdmi3D, but this mode supports hdmi3D.
         * Re-validate the mode with hdmi3D enabled.  If that passes, report
         * to the client that the mode could be used with hdmi3D if they choose
         * later.
         */
        if (pReply->valid && pReply->hdmi3DAvailable) {
            /*
             * Use dummy validSyncs and modeUsage so the original result isn't
             * affected.
             *
             * Create a temporary KMS mode so that we can enable hdmi3D in it
             * without perturbing the currently validated mode.
             *
             * Put all of this in a temporary heap allocation, to conserve
             * stack.
             */
            struct workArea {
                struct NvKmsModeValidationValidSyncs stereoValidSyncs;
                struct NvKmsUsageBounds stereoModeUsage;
                struct NvKmsMode stereoKmsMode;
            } *pWorkArea = nvCalloc(1, sizeof(*pWorkArea));

            if (pWorkArea == NULL) {
                pReply->hdmi3DAvailable = FALSE;
            } else {
                pWorkArea->stereoKmsMode = kmsMode;
                nvKmsUpdateNvModeTimingsForHdmi3D(
                    &pWorkArea->stereoKmsMode.timings, TRUE);

                pReply->hdmi3DAvailable =
                    ValidateMode(pDpyEvo,
                                 &pWorkArea->stereoKmsMode,
                                 &flags,
                                 pParams,
                                 pInfoString,
                                 &pWorkArea->stereoValidSyncs,
                                 &pWorkArea->stereoModeUsage);
                nvFree(pWorkArea);
            }
        }

        /*
         * if this is a detailed timing, then flag it as such; this
         * will be used later when searching for the AutoSelect mode
         */

        if (NVT_GET_TIMING_STATUS_TYPE(timing.etc.status) ==
            NVT_TYPE_EDID_DTD) {

            /*
             * if the EDID indicates that the first detailed timing is
             * preferred, then flag it is as such; this will be used
             * later when searching for the AutoSelect mode
             *
             * Note that the sequence number counts from 1
             */

            if ((pDpyEvo->parsedEdid.info.u.feature_ver_1_3.preferred_timing_is_native) &&
                NVT_GET_TIMING_STATUS_SEQ(timing.etc.status) == 1) {

                pReply->preferredMode = TRUE;
            }
        }

        /*
         * If the NVT_TIMING was patched for 3DVision above, then the
         * NvModeTimings generated from it, when passed to
         * nvFindEdidNVT_TIMING() during nvValidateModeForModeset(),
         * won't match the original EDID NVT_TIMING.  Rebuild
         * NvModeTimings based on the original (non-3DVision-patched)
         * NVT_TIMING from the EDID, and return that to the client.
         * When the NvModeTimings is passed to
         * nvValidateModeForModeset(), the 3DVision patching will be
         * performed again.
         */
        if (flags.patchedStereoTimings) {
            enum NvYuv420Mode yuv420Mode = kmsMode.timings.yuv420Mode;
            hdmi3D = kmsMode.timings.hdmi3D;

            NVT_TIMINGtoNvModeTimings(&pDpyEvo->parsedEdid.info.timing[i],
                                      &kmsMode.timings);
            kmsMode.timings.yuv420Mode = yuv420Mode;

            nvKmsUpdateNvModeTimingsForHdmi3D(&kmsMode.timings, hdmi3D);
        }

        pReply->mode.timings = kmsMode.timings;
        pReply->source = NvKmsModeSourceEdid;

        if (description != NULL) {
            nvAssert(nvkms_strlen(description) < sizeof(pReply->description));
            nvkms_strncpy(pReply->description, description,
                          sizeof(pReply->description));
            pReply->description[sizeof(pReply->description) - 1] = '\0';
        }

        nvBuildModeName(kmsMode.timings.hVisible, kmsMode.timings.vVisible,
                        pReply->mode.name, sizeof(pReply->mode.name));
        return TRUE;
    }

    /* No matching mode found. */
    return FALSE;
}


// NOTE: does not include timings for 848x480, 1280x768, 1360x768,
// 1400x1050, 1440x900, 1680x1050, 1920x1200

static const NvModeTimings VesaModesTable[] = {
    /*
     * { RRx1k, PClkHz;
     *   hVisible, hSyncStart, hSyncEnd, hTotal,
     *   hSkew,
     *   vVisible, vSyncStart, vSyncEnd, vTotal,
     *   { widthMM, heightMM },
     *   interlaced, doubleScan,
     *   hSyncPos, hSyncNeg, vSyncPos, vSyncNeg, hdmi3D, yuv420 },
     */

    // VESA Standard 640x350 @ 85Hz
    { 85080, 31500000,
      640, 672, 736, 832,
      0,
      350, 382, 385, 445,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, FALSE, TRUE, FALSE, FALSE },

    // VESA Standard 640x400 @ 85Hz
    { 85080, 31500000,
      640, 672, 736, 832,
      0,
      400, 401, 404, 445,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 720x400 @ 85Hz
    { 85039, 35500000,
      720, 756, 828, 936,
      0,
      400, 401, 404, 446,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },

    // Industry Standard 640x480 @ 60Hz
    { 59940, 25175000,
      640, 656, 752, 800,
      0,
      480, 490, 492, 525,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, FALSE, TRUE, FALSE, FALSE },

    // VESA Standard 640x480 @ 72Hz
    { 72809, 31500000,
      640, 664, 704, 832,
      0,
      480, 489, 492, 520,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, FALSE, TRUE, FALSE, FALSE },

    // VESA Standard 640x480 @ 75Hz
    { 75000, 31500000,
      640, 656, 720, 840,
      0,
      480, 481, 484, 500,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, FALSE, TRUE, FALSE, FALSE },

    // VESA Standard 640x480 @ 85Hz
    { 85008, 36000000,
      640, 696, 752, 832,
      0,
      480, 481, 484, 509,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, FALSE, TRUE, FALSE, FALSE },

    // VESA Standard 800x600 @ 56Hz
    { 56250, 36000000,
      800, 824, 896, 1024,
      0,
      600, 601, 603, 625,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 800x600 @ 60Hz
    { 60317, 40000000,
      800, 840, 968, 1056,
      0,
      600, 601, 605, 628,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 800x600 @ 72Hz
    { 72188, 50000000,
      800, 856, 976, 1040,
      0,
      600, 637, 643, 666,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 800x600 @ 75Hz
    { 75000, 49500000,
      800, 816, 896, 1056,
      0,
      600, 601, 604, 625,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 800x600 @ 85Hz
    { 85137, 56300000,
      800, 832, 896, 1048,
      0,
      600, 601, 604, 631,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1024x768i @ 87Hz
    { 86958, 44900000,
      1024, 1032, 1208, 1264,
      0,
      768, 768, 776, 817,
      { 0, 0 },
      TRUE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1024x768 @ 60Hz
    { 60004, 65000000,
      1024, 1048, 1184, 1344,
      0,
      768, 771, 777, 806,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, FALSE, TRUE, FALSE, FALSE },

    // VESA Standard 1024x768 @ 70Hz
    { 70069, 75000000,
      1024, 1048, 1184, 1328,
      0,
      768, 771, 777, 806,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, FALSE, TRUE, FALSE, FALSE },

    // VESA Standard 1024x768 @ 75Hz
    { 75029, 78750000,
      1024, 1040, 1136, 1312,
      0,
      768, 769, 772, 800,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1024x768 @ 85Hz
    { 84997, 94500000,
      1024, 1072, 1168, 1376,
      0,
      768, 769, 772, 808,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1152x864 @ 75Hz
    { 75000, 108000000,
      1152, 1216, 1344, 1600,
      0,
      864, 865, 868, 900,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1280x960 @ 60Hz
    { 60000, 108000000,
      1280, 1376, 1488, 1800,
      0,
      960, 961, 964, 1000,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1280x960 @ 85Hz
    { 85002, 148500000,
      1280, 1344, 1504, 1728,
      0,
      960, 961, 964, 1011,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1280x1024 @ 60Hz
    { 60020, 108000000,
      1280, 1328, 1440, 1688,
      0,
      1024, 1025, 1028, 1066,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1280x1024 @ 75Hz
    { 75025, 135000000,
      1280, 1296, 1440, 1688,
      0,
      1024, 1025, 1028, 1066,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1280x1024 @ 85Hz
    { 85024, 157500000,
      1280, 1344, 1504, 1728,
      0,
      1024, 1025, 1028, 1072,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1600x1200 @ 60Hz
    { 60000, 162000000,
      1600, 1664, 1856, 2160,
      0,
      1200, 1201, 1204, 1250,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1600x1200 @ 65Hz
    { 65000, 175500000,
      1600, 1664, 1856, 2160,
      0,
      1200, 1201, 1204, 1250,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1600x1200 @ 70Hz
    { 70000, 189000000,
      1600, 1664, 1856, 2160,
      0,
      1200, 1201, 1204, 1250,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1600x1200 @ 75Hz
    { 75000, 202500000,
      1600, 1664, 1856, 2160,
      0,
      1200, 1201, 1204, 1250,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1600x1200 @ 85Hz
    { 85000, 229500000,
      1600, 1664, 1856, 2160,
      0,
      1200, 1201, 1204, 1250,
      { 0, 0 },
      FALSE, FALSE,
      TRUE, FALSE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1792x1344 @ 60Hz
    { 60014, 204800000,
      1792, 1920, 2120, 2448,
      0,
      1344, 1345, 1348, 1394,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1792x1344 @ 75Hz
    { 74997, 261000000,
      1792, 1888, 2104, 2456,
      0,
      1344, 1345, 1348, 1417,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1856x1392 @ 60Hz
    { 60009, 218300000,
      1856, 1952, 2176, 2528,
      0,
      1392, 1393, 1396, 1439,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1856x1392 @ 75Hz
    { 75000, 288000000,
      1856, 1984, 2208, 2560,
      0,
      1392, 1393, 1396, 1500,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1920x1440 @ 60Hz
    { 60000, 234000000,
      1920, 2048, 2256, 2600,
      0,
      1440, 1441, 1444, 1500,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },

    // VESA Standard 1920x1440 @ 75Hz
    { 75000, 297000000,
      1920, 2064, 2288, 2640,
      0,
      1440, 1441, 1444, 1500,
      { 0, 0 },
      FALSE, FALSE,
      FALSE, TRUE, TRUE, FALSE, FALSE, FALSE },
};


/*!
 * Scan through the VESA Standard modes, counting each one.  If the
 * count reaches requestedModeIndex, then validate that mode.
 *
 * \param[in]     pDpyEvo  The dpy for whom the modes are considered.
 * \param[in]     pParams  The NvKmsModeValidationParams.
 * \param[out]    pReply   The NvKmsValidateModeIndexReply; if we found
 *                         requestedModeIndex, pReply->valid will store if
 *                         the mode was valid.
 * \param[in]     requestedModeIndex  The index of the mode we are looking for.
 * \param[in,out] pCurrentModeIndex   A running total of the number of modes
 *                         we have considered.  This will be incremented
 *                         by the number of modes considered.
 *
 * \return        If we found the mode with index == requestedModeIndex,
 *                return TRUE.  Otherwise, return FALSE.
 */
static NvBool
ValidateModeIndexVesa(NVDpyEvoPtr pDpyEvo,
                      const struct NvKmsModeValidationParams *pParams,
                      struct NvKmsValidateModeIndexReply *pReply,
                      NVEvoInfoStringPtr pInfoString,
                      const NvU32 requestedModeIndex,
                      NvU32 *pCurrentModeIndex)
{
    int i;

    for (i = 0; i < ARRAY_LEN(VesaModesTable); i++) {
        struct NvKmsMode kmsMode = { };
        EvoValidateModeFlags flags;

        /*
         * If *pCurrentModeIndex matches requestedModeIndex, then
         * validate the mode.  Otherwise, go on to the next mode.
         */
        if (*pCurrentModeIndex != requestedModeIndex) {
            (*pCurrentModeIndex)++;
            continue;
        }

        kmsMode.timings = VesaModesTable[i];

        nvkms_memset(&flags, 0, sizeof(flags));
        flags.source = NvKmsModeSourceVesa;

        /* is this mode valid? */
        pReply->valid = ValidateMode(pDpyEvo,
                                     &kmsMode,
                                     &flags,
                                     pParams,
                                     pInfoString,
                                     &pReply->validSyncs,
                                     &pReply->modeUsage);

        pReply->mode.timings = kmsMode.timings;
        pReply->source = NvKmsModeSourceVesa;

        nvBuildModeName(VesaModesTable[i].hVisible,
                        VesaModesTable[i].vVisible,
                        pReply->mode.name, sizeof(pReply->mode.name));
        return TRUE;
    }

    /* No matching mode found. */
    return FALSE;
}


/*!
 * Return if the given NvModeTimings match any entry in VesaModesTable[].
 */
static NvBool IsVesaMode(const NvModeTimings *pModeTimings,
                         const struct NvKmsModeValidationParams *pParams)
{
    int i;

    for (i = 0; i < ARRAY_LEN(VesaModesTable); i++) {
        if (NvModeTimingsMatch(&VesaModesTable[i], pModeTimings,
                               TRUE /* ignoreSizeMM */,
                               ((pParams->overrides &
                                 NVKMS_MODE_VALIDATION_NO_RRX1K_CHECK) != 0x0)
                                /* ignoreRRx1k */)) {
            return TRUE;
        }
    }

    return FALSE;
}


/*!
 * Write to pInfoString with information about the current mode that
 * we are validating; called from the beginning of ValidateMode();
 * LogModeValidationEnd() should be called at the end of
 * ValidateMode() to report whether the mode was validated.
 */

static void LogModeValidationBegin(NVEvoInfoStringPtr pInfoString,
                                   const NvModeTimings *pModeTimings,
                                   const char *modeName)
{
    nvEvoLogInfoString(pInfoString, "%d x %d @ %d Hz%s",
                       pModeTimings->hVisible,
                       pModeTimings->vVisible,
                       NV_U32_KHZ_TO_HZ(pModeTimings->RRx1k),
                       pModeTimings->hdmi3D ? " (HDMI 3D)" : "");

    nvEvoLogModeValidationModeTimings(pInfoString, pModeTimings);
}


/*!
 * Append to pInfoString with any mode validation failure.
 */
static void LogModeValidationEnd(const NVDispEvoRec *pDispEvo,
                                 NVEvoInfoStringPtr pInfoString,
                                 const char *failureReasonFormat, ...)
{
    /* expand any varargs, and print the mode validation result */

    if (failureReasonFormat) {
        char *buf;
        NV_VSNPRINTF(buf, failureReasonFormat);
        nvEvoLogInfoString(pInfoString,
                           "Mode is rejected: %s.",
                           buf ? buf : "Unknown failure");
        nvFree(buf);
    }
}


/*!
 * Print mode timings to the NVEvoInfoStringPtr.
 */
void nvEvoLogModeValidationModeTimings(NVEvoInfoStringPtr
                                       pInfoString,
                                       const NvModeTimings *pModeTimings)
{
    const char *extra;
    NvU32 hdmi3DPixelClock = HzToKHz(pModeTimings->pixelClockHz);

    if (pModeTimings->hdmi3D) {
        hdmi3DPixelClock /= 2;
    }

    nvEvoLogInfoString(pInfoString, "  Pixel Clock      : "
                       NV_FMT_DIV_1000_POINT_2 " MHz%s",
                       NV_VA_DIV_1000_POINT_2(hdmi3DPixelClock),
                       pModeTimings->hdmi3D ? " (HDMI 3D)" : "");

    nvEvoLogInfoString(pInfoString, "  HRes, HSyncStart : %4d, %4d",
                       pModeTimings->hVisible,
                       pModeTimings->hSyncStart);

    nvEvoLogInfoString(pInfoString, "  HSyncEnd, HTotal : %4d, %4d",
                       pModeTimings->hSyncEnd,
                       pModeTimings->hTotal);

    nvEvoLogInfoString(pInfoString, "  VRes, VSyncStart : %4d, %4d",
                       pModeTimings->vVisible,
                       pModeTimings->vSyncStart);

    nvEvoLogInfoString(pInfoString, "  VSyncEnd, VTotal : %4d, %4d",
                       pModeTimings->vSyncEnd,
                       pModeTimings->vTotal);

    nvEvoLogInfoString(pInfoString, "  Sync Polarity    : %s%s%s%s",
                       pModeTimings->hSyncPos ? "+H " : "",
                       pModeTimings->hSyncNeg ? "-H " : "",
                       pModeTimings->vSyncPos ? "+V " : "",
                       pModeTimings->vSyncNeg ? "-V " : "");

    if (pModeTimings->interlaced && pModeTimings->doubleScan) {
        extra = "Interlace DoubleScan";
    } else if (pModeTimings->interlaced) {
        extra = "Interlace";
    } else if (pModeTimings->doubleScan) {
        extra = "DoubleScan";
    } else {
        extra = NULL;
    }

    if (extra) {
        nvEvoLogInfoString(pInfoString, "  Extra            : %s", extra);
    }
}


/*!
 * Adjust the given value by the given percentage, using integer math.
 *
 * The 'percentage' argument is multiplied by 100 by the caller.  E.g.,
 *
 *  percentage=50  ==> 50%
 *  percentage=110 ==> 110%
 *
 * So, divide by 100.0:
 *
 *  value * percentage / 100
 */
static NvU32 Percentage(const NvU32 value, const NvU32 percentage)
{
    return axb_div_c(value, percentage, 100);
}

/*!
 * Write the given frequency to the given buffer.
 *
 * The frequency value is assumed to have been multiplied by 1000,
 * such that 'value % 1000' gives the fractional part, and value/1000
 * gives the integer part.
 *
 * The buffer is assumed to be (at least) NV_MAX_FREQUENCY_STRING_LEN
 * bytes long.
 *
 * Note that to meet the size assumptions made in the
 * NV_MAX_FREQUENCY_STRING_LEN definition, the integer portion of the
 * frquency value is clamped to 3 digits.
 */
static int
FrequencyToString(const NvU32 value, char *buffer)
{
    int n = nvkms_snprintf(buffer, NV_MAX_FREQUENCY_STRING_LEN,
                           "%d.%03d",
                           /* mod 1000, to limit to 3 digits */
                           (value / 1000) % 1000,
                           value % 1000);

    buffer[NV_MAX_FREQUENCY_STRING_LEN - 1] = '\0';

    return n;
}

/*!
 * Write the given NvKmsModeValidationFrequencyRanges to the given buffer.
 */
static void
RangesToString(const struct NvKmsModeValidationFrequencyRanges *pRanges,
               char buffer[NV_MAX_RANGE_STRING_LEN])
{
    char *s;
    int i, n;

    s = buffer;

    for (i = 0; i < pRanges->numRanges; i++) {
        if (pRanges->range[i].high == pRanges->range[i].low) {
            s += FrequencyToString(pRanges->range[i].high, s);
        } else {
            char highString[NV_MAX_FREQUENCY_STRING_LEN];
            char lowString[NV_MAX_FREQUENCY_STRING_LEN];

            FrequencyToString(pRanges->range[i].high, highString);
            FrequencyToString(pRanges->range[i].low, lowString);

            n = buffer + NV_MAX_RANGE_STRING_LEN - s;
            s += nvkms_snprintf(s, n, "%s-%s", lowString, highString);
        }

        if (i < (pRanges->numRanges - 1)) {
            n = buffer + NV_MAX_RANGE_STRING_LEN - s;
            s += nvkms_snprintf(s, n, ", ");
        }
    }

    buffer[NV_MAX_RANGE_STRING_LEN - 1] = '\0';
}

static NvBool ValidateModeTimings(
    NVDpyEvoPtr                             pDpyEvo,
    const struct NvKmsMode                 *pKmsMode,
    const EvoValidateModeFlags             *flags,
    const struct NvKmsModeValidationParams *pParams,
    NVEvoInfoStringPtr                      pInfoString,
    struct NvKmsModeValidationValidSyncs   *pValidSyncs)
{
    int i;
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 overrides = pParams->overrides;
    const NvBool is3DVisionStereo = nvIs3DVisionStereoEvo(pParams->stereoMode);
    const char *modeName = pKmsMode->name;
    const NvModeTimings *pModeTimings = &pKmsMode->timings;
    char localModeName[NV_MAX_MODE_NAME_LEN];

    if (modeName[0] == '\0') {
        nvBuildModeName(pModeTimings->hVisible, pModeTimings->vVisible,
                        localModeName, sizeof(localModeName));
        modeName = localModeName;
    }

    /* Compute the validSyncs to use during validation. */

    *pValidSyncs = pParams->validSyncs;
    nvDpySetValidSyncsEvo(pDpyEvo, pValidSyncs);

    if (pModeTimings->interlaced) {
        NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
        if (!pEvoSubDev->capabilities.misc.supportsInterlaced) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "Interlaced modes are not supported on this GPU");
            return FALSE;
        }
    }

    if ((flags->source != NvKmsModeSourceEdid) &&
        (overrides & NVKMS_MODE_VALIDATION_ALLOW_NON_EDID_MODES) == 0) {

        NvBool continuousFrequency = TRUE;

        /*
         * EDID 1.3 defines the "GTF Supported" flag like this:
         *
         * If this bit is set to 1, the display supports timings based
         * on the GTF standard.
         *
         * We interpret this to mean that if the bit is not set, then
         * the display device only supports modes listed in the EDID.
         */
        if (pDpyEvo->parsedEdid.valid &&
            (pDpyEvo->parsedEdid.info.version == NVT_EDID_VER_1_3)) {
            continuousFrequency =
                pDpyEvo->parsedEdid.info.u.feature_ver_1_3.support_gtf;
        }

        /*
         * EDID 1.4 Release A, Revision 2; Note 5 in section 3.6.4:
         *
         * If bit 0 is set to 0, then the display is non-continuous
         * frequency (multi-mode) and is only specified to accept the
         * video timing formats that are listed in BASE EDID and
         * certain EXTENSION Blocks.
         */
        if (pDpyEvo->parsedEdid.valid &&
            (pDpyEvo->parsedEdid.info.version >= NVT_EDID_VER_1_4)) {
            if (pDpyEvo->parsedEdid.info.input.isDigital) {
                continuousFrequency =
                    pDpyEvo->parsedEdid.info.u.feature_ver_1_4_digital.continuous_frequency;
            } else {
                continuousFrequency =
                    pDpyEvo->parsedEdid.info.u.feature_ver_1_4_analog.continuous_frequency;
            }
        }

        if (!continuousFrequency) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "Only EDID-provided modes are allowed on %s (continuous frequency modes not allowed)",
                pDpyEvo->name);
            return FALSE;
        }

        /*
         * By default, we only allow EDID modes when driving digital
         * protocol.
         */
        if (pDpyEvo->parsedEdid.valid &&
            pDpyEvo->parsedEdid.info.input.isDigital) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "Only EDID-provided modes are allowed on %s",
                pDpyEvo->name);
            return FALSE;
        }
    }

    /* Throw out modes that will break downstream assumptions */

    if ((overrides & NVKMS_MODE_VALIDATION_NO_TOTAL_SIZE_CHECK) == 0) {

        if (pModeTimings->hVisible > pModeTimings->hSyncStart) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "This mode's visible horizontal size (%d) exceeds the horizontal sync start (%d)",
                pModeTimings->hVisible,
                pModeTimings->hSyncStart);
            return FALSE;
        }

        if (pModeTimings->hSyncStart > pModeTimings->hSyncEnd) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "This mode's horizontal sync start (%d) exceeds the horizontal sync end (%d)",
                pModeTimings->hSyncStart,
                pModeTimings->hSyncEnd);
            return FALSE;
        }

        if (pModeTimings->hSyncEnd > pModeTimings->hTotal) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "This mode's horizontal sync end (%d) exceeds the horizontal total size (%d)",
                pModeTimings->hSyncEnd,
                pModeTimings->hTotal);
            return FALSE;
        }

        if (pModeTimings->vVisible > pModeTimings->vSyncStart) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "This mode's visible vertical size (%d) exceeds the vertical sync start (%d)",
                pModeTimings->vVisible,
                pModeTimings->vSyncStart);
            return FALSE;
        }

        if (pModeTimings->vSyncStart > pModeTimings->vSyncEnd) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "This mode's vertical sync start (%d) exceeds the vertical sync end (%d)",
                pModeTimings->vSyncStart,
                pModeTimings->vSyncEnd);
            return FALSE;
        }

        if (pModeTimings->vSyncEnd > pModeTimings->vTotal) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "This mode's vertical sync end (%d) exceeds the vertical total size (%d)",
                pModeTimings->vSyncEnd,
                pModeTimings->vTotal);
            return FALSE;
        }
    }

    /* reject modes with too high pclk */

    if ((overrides & NVKMS_MODE_VALIDATION_NO_MAX_PCLK_CHECK) == 0) {

        NvU32 maxPixelClockKHz = pDpyEvo->maxPixelClockKHz;
        NvU32 realPixelClock = HzToKHz(pModeTimings->pixelClockHz);
        if (pModeTimings->yuv420Mode != NV_YUV420_MODE_NONE) {
            realPixelClock /= 2;
        }

        if (realPixelClock > maxPixelClockKHz) {
            NvU32 hdmi3DPixelClock = realPixelClock;

            if (pModeTimings->hdmi3D) {
                hdmi3DPixelClock /= 2;
            }

            if (is3DVisionStereo &&
                pDpyEvo->stereo3DVision.requiresModetimingPatching &&
                (realPixelClock - maxPixelClockKHz < 5000)) {

                nvAssert(!pModeTimings->hdmi3D);

                nvEvoLogInfoString(pInfoString,
                    "PixelClock (" NV_FMT_DIV_1000_POINT_1 " MHz) is slightly higher than Display Device maximum (" NV_FMT_DIV_1000_POINT_1 " MHz), but is within tolerance for 3D Vision Stereo.",
                    NV_VA_DIV_1000_POINT_1(realPixelClock),
                    NV_VA_DIV_1000_POINT_1(maxPixelClockKHz));

            } else {

                LogModeValidationEnd(pDispEvo, pInfoString,
                    "PixelClock (" NV_FMT_DIV_1000_POINT_1 " MHz%s) too high for Display Device (Max: " NV_FMT_DIV_1000_POINT_1 " MHz)",
                    NV_VA_DIV_1000_POINT_1(hdmi3DPixelClock),
                    pModeTimings->hdmi3D ?
                    ", doubled for HDMI 3D" : "",
                    NV_VA_DIV_1000_POINT_1(maxPixelClockKHz));
                return FALSE;
            }
        }
    }

    /* check against the EDID's max pclk */

    if ((overrides & NVKMS_MODE_VALIDATION_NO_EDID_MAX_PCLK_CHECK) == 0) {

        NvU32 realPixelClock = HzToKHz(pModeTimings->pixelClockHz);
        if (pModeTimings->yuv420Mode != NV_YUV420_MODE_NONE) {
            realPixelClock /= 2;
        }

        if (pDpyEvo->parsedEdid.valid &&
            (pDpyEvo->parsedEdid.limits.max_pclk_10khz != 0) &&
            (realPixelClock >
             (pDpyEvo->parsedEdid.limits.max_pclk_10khz * 10))) {

            NvU32 hdmi3DPixelClock = realPixelClock;
            NvU32 maxPixelClockKHz = pDpyEvo->parsedEdid.limits.max_pclk_10khz * 10;

            if (pModeTimings->hdmi3D) {
                hdmi3DPixelClock /= 2;
            }

            /*
             * If this mode came from the EDID, then something is odd
             * (see bug 336963); print a warning, but continue
             */

            if (is3DVisionStereo &&
                pDpyEvo->stereo3DVision.requiresModetimingPatching &&
                (realPixelClock - maxPixelClockKHz < 5000)) {

                nvAssert(!pModeTimings->hdmi3D);

                nvEvoLogInfoString(pInfoString,
                    "PixelClock (" NV_FMT_DIV_1000_POINT_1 " MHz) is slightly higher than EDID specified maximum (" NV_FMT_DIV_1000_POINT_1 " MHz), but is within tolerance for 3D Vision Stereo.",
                    NV_VA_DIV_1000_POINT_1(realPixelClock),
                    NV_VA_DIV_1000_POINT_1(maxPixelClockKHz));

            } else if ((flags->source == NvKmsModeSourceEdid) &&
                ((overrides &
                  NVKMS_MODE_VALIDATION_OBEY_EDID_CONTRADICTIONS) == 0)) {
                nvEvoLogInfoString(pInfoString,
                    "The EDID for %s contradicts itself: mode \"%s\" is specified in the EDID; "
                    "however, the EDID's reported maximum PixelClock (" NV_FMT_DIV_1000_POINT_1 " MHz) would exclude this mode's PixelClock (" NV_FMT_DIV_1000_POINT_1 " MHz%s); "
                    "ignoring EDID maximum PixelClock check for mode \"%s\".",
                    pDpyEvo->name, modeName,
                    NV_VA_DIV_1000_POINT_1(maxPixelClockKHz),
                    NV_VA_DIV_1000_POINT_1(hdmi3DPixelClock),
                    pModeTimings->hdmi3D ?
                    ", doubled for HDMI 3D" : "",
                    modeName);
            } else {

                LogModeValidationEnd(pDispEvo, pInfoString,
                    "PixelClock (" NV_FMT_DIV_1000_POINT_1 " MHz%s) too high for EDID (EDID Max: " NV_FMT_DIV_1000_POINT_1" MHz)",
                    NV_VA_DIV_1000_POINT_1(hdmi3DPixelClock),
                    pModeTimings->hdmi3D ?
                    ", doubled for HDMI 3D" : "",
                    NV_VA_DIV_1000_POINT_1(maxPixelClockKHz));
                return FALSE;
            }
        }
    }

    /* check the mode against the max size */

    if ((overrides & NVKMS_MODE_VALIDATION_NO_MAX_SIZE_CHECK) == 0) {

        const NvU32 maxHeight = pDevEvo->caps.maxRasterHeight;
        const NvU32 maxWidth = pDevEvo->caps.maxRasterWidth;

        NvU16 realHTotal = pModeTimings->hTotal;
        if (pModeTimings->yuv420Mode == NV_YUV420_MODE_SW) {
            realHTotal /= 2;
        }

        // With YUV420 modes, we want to use the real half-width hTotal
        // for validation, but report the full-width value in the log.
        if ((realHTotal > maxWidth) ||
            (pModeTimings->vTotal > maxHeight)) {

            LogModeValidationEnd(pDispEvo, pInfoString,
                "Mode total size (%u x %u), with visible size (%u x %u), larger than maximum size (%u x %u)",
                pModeTimings->hTotal,
                pModeTimings->vTotal,
                pModeTimings->hVisible,
                pModeTimings->vVisible,
                maxWidth, maxHeight);
            return FALSE;
        }
    }

    /* check against the frequency information */

    if ((overrides & NVKMS_MODE_VALIDATION_NO_HORIZ_SYNC_CHECK) == 0) {
        if (pValidSyncs->horizSyncHz.numRanges > 0) {
            NvU32 hSync = axb_div_c(pModeTimings->pixelClockHz, 1,
                                    pModeTimings->hTotal);

            for (i = 0; i < pValidSyncs->horizSyncHz.numRanges; i++) {
                NvU32 low = pValidSyncs->horizSyncHz.range[i].low;
                NvU32 high = pValidSyncs->horizSyncHz.range[i].high;
                if ((hSync > Percentage(low, 99)) &&
                    (hSync < Percentage(high, 101))) {
                    break;
                }
            }

            /*
             * Now see whether we ran out of sync ranges without
             * finding a match
             */

            if (i == pValidSyncs->horizSyncHz.numRanges) {

                char rangeString[NV_MAX_RANGE_STRING_LEN];
                char hSyncString[NV_MAX_FREQUENCY_STRING_LEN];

                RangesToString(&pValidSyncs->horizSyncHz, rangeString);
                FrequencyToString(hSync, hSyncString);

                /*
                 * If this mode came from the EDID and the valid
                 * HorizSync ranges (which excluded this timing) also
                 * came from the EDID, then something is odd (see bug
                 * 336963); print a warning, but continue.
                 */

                if ((flags->source == NvKmsModeSourceEdid) &&
                    (pValidSyncs->horizSyncHz.source ==
                     NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_EDID) &&
                    ((overrides &
                      NVKMS_MODE_VALIDATION_OBEY_EDID_CONTRADICTIONS) == 0)) {

                    nvEvoLogInfoString(pInfoString,
                        "The EDID for %s contradicts itself: mode \"%s\" is specified in the EDID; "
                        "however, the EDID's valid HorizSync range (%s kHz) would exclude this mode's HorizSync (%s kHz); "
                        "ignoring HorizSync check for mode \"%s\".",
                        pDpyEvo->name, modeName,
                        rangeString, hSyncString, modeName);
                } else {

                    LogModeValidationEnd(pDispEvo, pInfoString,
                        "HorizSync (%s kHz) out of range (%s kHz)", hSyncString, rangeString);
                    return FALSE;
                }
            }
        }
    }

    if ((overrides & NVKMS_MODE_VALIDATION_NO_VERT_REFRESH_CHECK) == 0) {

        if (pValidSyncs->vertRefreshHz1k.numRanges > 0) {

            /*
             * note: we expect RRx1k to be field rate for interlaced
             * modes, (undoubled) frame rate for doublescan modes, and
             * (doubled) frame rate for HDMI 3D modes.
             */
            NvU32 vRefresh = pModeTimings->RRx1k;

            for (i = 0; i < pValidSyncs->vertRefreshHz1k.numRanges; i++) {
                NvU32 low = pValidSyncs->vertRefreshHz1k.range[i].low;
                NvU32 high = pValidSyncs->vertRefreshHz1k.range[i].high;

                if ((vRefresh > Percentage(low, 99)) &&
                    (vRefresh < Percentage(high, 101))) {
                    break;
                }
            }

            /*
             * Now see whether we ran out of refresh ranges without
             * finding a match
             */

            if (i == pValidSyncs->vertRefreshHz1k.numRanges) {

                char rangeString[NV_MAX_RANGE_STRING_LEN];
                char vRefreshString[NV_MAX_FREQUENCY_STRING_LEN];

                if (pModeTimings->hdmi3D) {
                    vRefresh /= 2;
                }

                RangesToString(&pValidSyncs->vertRefreshHz1k,
                               rangeString);
                FrequencyToString(vRefresh, vRefreshString);

                /*
                 * If this mode came from the EDID and the valid
                 * VertRefresh ranges (which excluded this timing)
                 * also came from the EDID, then something is odd (see
                 * bug 336963); print a warning, but continue.
                 */

                if ((flags->source == NvKmsModeSourceEdid) &&
                    (pValidSyncs->vertRefreshHz1k.source ==
                     NVKMS_MODE_VALIDATION_FREQUENCY_RANGE_SOURCE_EDID) &&
                    ((overrides &
                      NVKMS_MODE_VALIDATION_OBEY_EDID_CONTRADICTIONS) == 0)) {

                    nvEvoLogInfoString(pInfoString,
                        "The EDID for %s contradicts itself: mode \"%s\" is specified in the EDID; "
                        "however, the EDID's valid VertRefresh range (%s Hz) would exclude this mode's VertRefresh (%s Hz%s); "
                        "ignoring VertRefresh check for mode \"%s\".",
                        pDpyEvo->name, modeName,
                        rangeString, vRefreshString,
                        pModeTimings->hdmi3D ? ", doubled for HDMI 3D" : "",
                        modeName);
                } else {

                    LogModeValidationEnd(pDispEvo, pInfoString,
                        "VertRefresh (%s Hz%s) out of range (%s Hz)", vRefreshString,
                        pModeTimings->hdmi3D ? ", doubled for HDMI 3D" : "",
                        rangeString);
                    return FALSE;
                }
            }
        }
    }

    /*
     * If 3D Vision Stereo is enabled, and the pDpy requires patched
     * stereo modetimings, and these modetimings are not patched, then
     * reject the mode, unless the mode validation override "AllowNon3DVModes"
     * has been set.
     */

    if ((overrides & NVKMS_MODE_VALIDATION_ALLOW_NON_3DVISION_MODES) == 0) {
        if (is3DVisionStereo &&
            pDpyEvo->stereo3DVision.requiresModetimingPatching &&
            !flags->patchedStereoTimings) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                                 "Mode not compatible with 3D Vision Stereo");
            return FALSE;
        }
    }

    /*
     * If HDMI 3D is enabled and supported, reject non-HDMI 3D modes unless the
     * mode validation override "AllowNonHDMI3DModes" has been set.
     */
    if (((overrides & NVKMS_MODE_VALIDATION_ALLOW_NON_HDMI3D_MODES) == 0) &&
        (pParams->stereoMode == NVKMS_STEREO_HDMI_3D) &&
        nvDpyEvoSupportsHdmi3D(pDpyEvo) &&
        !pModeTimings->hdmi3D) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "Mode not compatible with HDMI 3D");
        return FALSE;
    }

    if (pModeTimings->hdmi3D && pModeTimings->interlaced) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "Interlaced frame packed HDMI 3D modes are not supported.");
        return FALSE;
    }

    if (pModeTimings->interlaced &&
        nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo) &&
        (overrides & NVKMS_MODE_VALIDATION_ALLOW_DP_INTERLACED) == 0) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "Interlaced modes are not supported over DisplayPort");
        return FALSE;
    }

    if (pModeTimings->interlaced &&
        (overrides & NVKMS_MODE_VALIDATION_NO_INTERLACED_MODES)) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "Interlaced modes are not allowed");
        return FALSE;
    }

    if (pModeTimings->interlaced &&
        pParams->stereoMode != NVKMS_STEREO_DISABLED) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "Interlaced modes are not allowed with stereo");
        return FALSE;
    }

    return TRUE;
}

/*
 * Log to the InfoString with information about this
 * particular ViewPort.
 */

static
void LogViewPort(NVEvoInfoStringPtr pInfoString,
                 const NVHwModeTimingsEvo timings[NVKMS_MAX_HEADS_PER_DISP],
                 const NvU32 numHeads)
{
    NvU32 head;
    char str[64] = { }, *s = NULL;

    nvAssert(numHeads <= 2);

    nvEvoLogInfoString(pInfoString,
            "DualHead Mode: %s", (numHeads > 1) ? "Yes" : "No");

    /* print the viewport name, size, and taps */
    nvkms_memset(str, 0, sizeof(str));
    for (head = 0, s = str; head < numHeads; head++) {
        const struct NvKmsRect viewPortOut =
            nvEvoViewPortOutClientView(&timings[head]);
        size_t n = str + sizeof(str) - s;
        s += nvkms_snprintf(s, n, "%s%dx%d+%d+%d", (s != str) ? ", " : "",
                            viewPortOut.width, viewPortOut.height,
                            viewPortOut.x, viewPortOut.x);
    }
    nvEvoLogInfoString(pInfoString,
               "Viewport                 %s", str);

    nvkms_memset(str, 0, sizeof(str));
    for (head = 0, s = str; head < numHeads; head++) {
        const NVHwModeViewPortEvo *pViewPort = &timings[head].viewPort;
        size_t n = str + sizeof(str) - s;
        s += nvkms_snprintf(s, n, "%s%d", (s != str) ? ", " : "",
                            NVEvoScalerTapsToNum(pViewPort->hTaps));
    }
    nvEvoLogInfoString(pInfoString,
               "  Horizontal Taps        %s", str);

    nvkms_memset(str, 0, sizeof(str));
    for (head = 0, s = str; head < numHeads; head++) {
        const NVHwModeViewPortEvo *pViewPort = &timings[head].viewPort;
        size_t n = str + sizeof(str) - s;
        s += nvkms_snprintf(s, n, "%s%d", (s != str) ? ", " : "",
                            NVEvoScalerTapsToNum(pViewPort->vTaps));
    }
    nvEvoLogInfoString(pInfoString,
               "  Vertical Taps          %s", str);
}

/*
 * Validate pModeTimings for use on pDpy.  If the mode is valid, use
 * pDev->disp.ConstructHwModeTimings() to assign pHwModeTimings and
 * return TRUE.
 */
static NvBool ValidateMode(NVDpyEvoPtr pDpyEvo,
                           const struct NvKmsMode *pKmsMode,
                           const EvoValidateModeFlags *flags,
                           const struct NvKmsModeValidationParams *pParams,
                           NVEvoInfoStringPtr pInfoString,
                           struct NvKmsModeValidationValidSyncs *pValidSyncs,
                           struct NvKmsUsageBounds *pModeUsage)
{
    const char *modeName = pKmsMode->name;
    const NvModeTimings *pModeTimings = &pKmsMode->timings;
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvBool b2Heads1Or = FALSE;
    char localModeName[NV_MAX_MODE_NAME_LEN];

    NVHwModeTimingsEvo *pTimingsEvo =
        nvPreallocGet(pDevEvo,
                      PREALLOC_TYPE_VALIDATE_MODE_HW_MODE_TIMINGS,
                      sizeof(*pTimingsEvo));
    HDMI_FRL_CONFIG *pHdmiFrlConfig =
        nvPreallocGet(pDevEvo,
                      PREALLOC_TYPE_VALIDATE_MODE_HDMI_FRL_CONFIG,
                      sizeof(*pHdmiFrlConfig));
    NVDscInfoEvoRec *pDscInfo =
         nvPreallocGet(pDevEvo,
                      PREALLOC_TYPE_VALIDATE_MODE_DSC_INFO,
                      sizeof(*pDscInfo));
    NVHwModeTimingsEvo *impOutTimings =
        nvPreallocGet(pDevEvo,
                      PREALLOC_TYPE_VALIDATE_MODE_IMP_OUT_HW_MODE_TIMINGS,
                      sizeof(*impOutTimings) *
                        NVKMS_MAX_HEADS_PER_DISP);
    NvU32 impOutNumHeads = 0x0;
    NvU32 head;
    NvBool ret = FALSE;

    const NvKmsDpyOutputColorFormatInfo supportedColorFormats =
        nvDpyGetOutputColorFormatInfo(pDpyEvo);
    NVDpyAttributeColor dpyColor;

    if (modeName[0] == '\0') {
        nvBuildModeName(pModeTimings->hVisible, pModeTimings->vVisible,
                        localModeName, sizeof(localModeName));
        modeName = localModeName;
    }

    /* Initialize the EVO hwModeTimings structure */

    nvkms_memset(pTimingsEvo, 0, sizeof(*pTimingsEvo));
    nvkms_memset(pHdmiFrlConfig, 0, sizeof(*pHdmiFrlConfig));
    nvkms_memset(pDscInfo, 0, sizeof(*pDscInfo));
    nvkms_memset(impOutTimings, 0, sizeof(*impOutTimings) * NVKMS_MAX_HEADS_PER_DISP);

    /* begin logging of ModeValidation for this mode */

    LogModeValidationBegin(pInfoString, pModeTimings, modeName);

    if (!ValidateModeTimings(pDpyEvo, pKmsMode, flags, pParams,
                             pInfoString, pValidSyncs)) {
        goto done;
    }

    if (pTimingsEvo->yuv420Mode != NV_YUV420_MODE_NONE) {
        dpyColor.format = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420;
        dpyColor.bpc = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
        dpyColor.range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED;
        dpyColor.colorimetry = NVKMS_OUTPUT_COLORIMETRY_DEFAULT;
    } else if (!nvGetDefaultDpyColor(&supportedColorFormats, &dpyColor)) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "Failed to get default color space and Bpc");
        goto done;
    }

    /*
     * we made it past the rest of mode validation; now construct the
     * hw modetimings to use for this mode; we do this here so that we
     * can report any failures as part of the mode validation
     * reporting.
     *
     * XXX For certain modes like doublescan, interlaced, and YUV 4:2:0
     * emulated mode, the timings stored in the pTimingsEvo constructed
     * here are different than the timings in pModeTimings used for validation
     * earlier in this function.
     *
     * In certain cases (like pclk validation for YUV 4:2:0 modes, which store
     * a doubled pclk in pModeTimings and the real pclk in pTimingsEvo) we
     * want to use the pTimingsEvo value for validation in this function.
     * It may make sense to restructure this function so pTimingsEvo
     * construction happens earlier, then the pTimingsEvo values are used
     * for the remaining validation.
     */

    if (!nvConstructHwModeTimingsEvo(pDpyEvo,
                                     pKmsMode,
                                     NULL, /* pViewPortSizeIn */
                                     NULL, /* pViewPortOut */
                                     &dpyColor,
                                     pTimingsEvo,
                                     pParams,
                                     pInfoString)) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "Unable to construct hardware-specific mode "
                             "timings");
        goto done;
    }

    b2Heads1Or = nvEvoUse2Heads1OR(pDpyEvo, pTimingsEvo, pParams);

    if (nvDpyIsHdmiEvo(pDpyEvo)) {
        if (!nvHdmiFrlQueryConfig(pDpyEvo,
                                  &pKmsMode->timings,
                                  pTimingsEvo,
                                  &dpyColor,
                                  b2Heads1Or,
                                  pParams,
                                  pHdmiFrlConfig,
                                  pDscInfo)) {
            LogModeValidationEnd(pDispEvo, pInfoString,
                "Unable to determine HDMI 2.1 Fixed Rate Link configuration.");
            goto done;
        }
    } else {
        if (!nvDPValidateModeEvo(pDpyEvo, pTimingsEvo, &dpyColor, b2Heads1Or,
                                 pDscInfo, pParams)) {
            LogModeValidationEnd(pDispEvo,
                                 pInfoString, "DP Bandwidth check failed");
            goto done;
        }
    }

    /*
     * Check ViewPortIn dimensions and ensure valid h/vTaps can be assigned.
     */
    if (!nvValidateHwModeTimingsViewPort(pDevEvo,
                                         /* XXX assume the gpus have equal capabilities */
                                         &pDevEvo->gpus[0].capabilities.head[0].scalerCaps,
                                         pTimingsEvo, pInfoString)) {
        goto done;
    }


    /* Run the raster timings through IMP checking. */
    if (!nvConstructHwModeTimingsImpCheckEvo(pDpyEvo->pConnectorEvo,
                                             pTimingsEvo,
                                             (pDscInfo->type !=
                                                NV_DSC_INFO_EVO_TYPE_DISABLED),
                                             b2Heads1Or,
                                             &dpyColor,
                                             pParams,
                                             impOutTimings,
                                             &impOutNumHeads,
                                             pInfoString)) {
        LogModeValidationEnd(pDispEvo, pInfoString,
                             "GPU extended capability check failed");
        goto done;
    }

    nvAssert(impOutNumHeads > 0);

    /* Log modevalidation information about the viewport. */

    LogViewPort(pInfoString, impOutTimings, impOutNumHeads);

    /*
     * Copy out the usage bounds that passed validation; note we intersect
     * the usage bounds across the hardware heads that would be used with
     * this apiHead, accumulating the results in pModeUsage.
     */
    for (head = 0; head < impOutNumHeads; head++) {
        if (head == 0) {
            *pModeUsage = impOutTimings[0].viewPort.possibleUsage;
        } else {
            struct NvKmsUsageBounds *pTmpUsageBounds =
                nvPreallocGet(pDevEvo,
                    PREALLOC_TYPE_VALIDATE_MODE_TMP_USAGE_BOUNDS,
                    sizeof(*pTmpUsageBounds));

            nvIntersectUsageBounds(pModeUsage,
                                   &impOutTimings[head].viewPort.possibleUsage,
                                   pTmpUsageBounds);
            *pModeUsage = *pTmpUsageBounds;

            nvPreallocRelease(pDevEvo, PREALLOC_TYPE_VALIDATE_MODE_TMP_USAGE_BOUNDS);
        }
    }

    /* Whew, if we got this far, the mode is valid. */

    LogModeValidationEnd(pDispEvo, pInfoString, NULL);

    ret = TRUE;

done:
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_VALIDATE_MODE_HW_MODE_TIMINGS);
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_VALIDATE_MODE_HDMI_FRL_CONFIG);
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_VALIDATE_MODE_DSC_INFO);
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_VALIDATE_MODE_IMP_OUT_HW_MODE_TIMINGS);

    return ret;
}


/*!
 * Return whether the given NVT_TIMING and NvModeTimings match.
 */
static NvBool NVT_TIMINGmatchesNvModeTimings
(
    const NVT_TIMING *pTiming,
    const NvModeTimings *pModeTimings,
    const struct NvKmsModeValidationParams *pParams
)
{
    NvModeTimings tmpModeTimings;

    NVT_TIMINGtoNvModeTimings(pTiming, &tmpModeTimings);

    return NvModeTimingsMatch(&tmpModeTimings, pModeTimings,
                              TRUE /* ignoreSizeMM */,
                              ((pParams->overrides &
                                NVKMS_MODE_VALIDATION_NO_RRX1K_CHECK) != 0x0)
                               /* ignoreRRx1k */);
}


/*!
 * Find the NVT_TIMING from the dpy's EDID that matches the pModeTimings.
 */
const NVT_TIMING *nvFindEdidNVT_TIMING
(
    const NVDpyEvoRec *pDpyEvo,
    const NvModeTimings *pModeTimings,
    const struct NvKmsModeValidationParams *pParams
)
{
    NvModeTimings tmpModeTimings;
    int i;

    if (!pDpyEvo->parsedEdid.valid) {
        return NULL;
    }

    tmpModeTimings = *pModeTimings;

    /*
     * Revert any modeTimings modifications that were done for hdmi3D
     * in ValidateModeIndexEdid(), so that the modeTimings can be
     * compared with the NVT_TIMINGs in the parsed EDID.
     */
    nvKmsUpdateNvModeTimingsForHdmi3D(&tmpModeTimings, FALSE);

    /*
     * The NVT_TIMINGs we compare against below won't have hdmi3D or
     * yuv420 set; clear those flags in tmpModeTimings so that we can
     * do a more meaningful comparison.
     */
    tmpModeTimings.yuv420Mode = NV_YUV420_MODE_NONE;

    for (i = 0; i < pDpyEvo->parsedEdid.info.total_timings; i++) {
        const NVT_TIMING *pTiming = &pDpyEvo->parsedEdid.info.timing[i];
        if (NVT_TIMINGmatchesNvModeTimings(pTiming, &tmpModeTimings, pParams) &&
            /*
             * Only consider the mode a match if the yuv420
             * configuration of pTiming would match pModeTimings.
             */
            (pModeTimings->yuv420Mode ==
             GetYUV420Value(pDpyEvo, pParams, pTiming))) {
            return pTiming;
        }
    }

    return NULL;
}

/*!
 * Construct mode-timing's meta data required for mode validation
 * logic. This meta data involves EvoValidateModeFlags, patched stereo
 * vision timings, etc.
 *
 * \param[in]       pDpyEvo         The dpy for whom the mode is considered.
 * \param[in]       pParams         The NvKmsModeValidationParams.
 * \param[in/out]   pKmsMode        The NVKMS mode to be considered.
 * \param[out]      pFlags          The EvoValidateModeFlags
 * \param[out]      pInfoFrameCtrl  InfoFrame control
 *
 * \return  Return TRUE on success with patched mode timings,
 *          EvoValidateModeFlags and infoFrame controls etc.; otherwise
 *          returns FALSE.
 */
static NvBool ConstructModeTimingsMetaData(
    NVDpyEvoRec *pDpyEvo,
    const struct NvKmsModeValidationParams *pParams,
    struct NvKmsMode *pKmsMode,
    EvoValidateModeFlags *pFlags,
    NVT_VIDEO_INFOFRAME_CTRL *pInfoFrameCtrl)
{
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    EvoValidateModeFlags flags = { 0 };
    NVT_VIDEO_INFOFRAME_CTRL infoFrameCtrl;
    NvModeTimings modeTimings = pKmsMode->timings;
    const NVT_TIMING *pTiming;

    nvkms_memset(&infoFrameCtrl, NVT_INFOFRAME_CTRL_DONTCARE,
                 sizeof(infoFrameCtrl));

    flags.source = NvKmsModeSourceUnknown;

    /* Is this an EDID mode? */
    pTiming = nvFindEdidNVT_TIMING(pDpyEvo, &modeTimings, pParams);

    if (pTiming != NULL) {
        NVT_TIMING timing = *pTiming;
        const NvBool is3DVisionStereo =
            nvIs3DVisionStereoEvo(pParams->stereoMode);

        flags.source = NvKmsModeSourceEdid;

        /* Patch the mode for 3DVision. */
        if (is3DVisionStereo &&
            pDpyEvo->stereo3DVision.requiresModetimingPatching &&
            nvPatch3DVisionModeTimingsEvo(&timing, pDpyEvo,
                                          &dummyInfoString)) {
            flags.patchedStereoTimings = TRUE;

            /*
             * Replace the client's modeTimings with the version
             * patched for 3DVision stereo.
             */
            NVT_TIMINGtoNvModeTimings(&timing, &modeTimings);

            /* Restore the yuv420 and hdmi3D flags from the client's mode. */
            modeTimings.yuv420Mode = pKmsMode->timings.yuv420Mode;

            /* Re-apply adjustments for hdmi3D. */
            nvKmsUpdateNvModeTimingsForHdmi3D(&modeTimings, pKmsMode->timings.hdmi3D);

        }

        /* Validate yuv420. */
        if (modeTimings.yuv420Mode !=
            GetYUV420Value(pDpyEvo, pParams, &timing)) {
            return FALSE;
        }

        /* Validate hdmi3D. */
        NvBool hdmi3D = FALSE;
        NvBool hdmi3DAvailable = FALSE;
        GetHdmi3DValue(pDpyEvo, pParams, &timing, &hdmi3D, &hdmi3DAvailable);
        if ((modeTimings.hdmi3D != hdmi3D) && !hdmi3DAvailable) {
            return FALSE;
        }

        if (pParams->stereoMode == NVKMS_STEREO_HDMI_3D) {
            if (!nvDpyEvoSupportsHdmi3D(pDpyEvo)) {
                nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                       "HDMI 3D mode is selected, but "
                       "HDMI 3D is not supported by %s; HDMI 3D may not function "
                       "properly. This might happen if no EDID is available for "
                       "%s, if the display is not connected over HDMI, or if the "
                       "display does not support HDMI 3D.", pDpyEvo->name,
                       pDpyEvo->name);
            } else if (!modeTimings.hdmi3D) {
                nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                       "HDMI 3D mode is selected, but the "
                       "currently selected mode is incompatible with HDMI 3D. "
                       "HDMI 3D will be disabled.");
            }
        }

        /*
         * Compute the infoFrame control; this will be assigned to
         * pTimingsEvo after ValidateMode has written to it.
         */
        if (nvDpyIsHdmiEvo(pDpyEvo)) {
            NvTiming_ConstructVideoInfoframeCtrl(&timing, &infoFrameCtrl);
        }

        goto done;
    }

    /* Otherwise, is this a VESA mode? */

    if (IsVesaMode(&modeTimings, pParams)) {
        flags.source = NvKmsModeSourceVesa;
        goto done;
    }

    /*
     * Otherwise, this must be a user-specified mode; no metadata changes
     * are needed.
     */

done:
    *pFlags = flags;
    *pInfoFrameCtrl = infoFrameCtrl;
    pKmsMode->timings = modeTimings;

    return TRUE;
}

/*!
 * Validate the NvKmsMode.
 *
 * \param[in]   pDpyEvo      The dpy for whom the mode is considered.
 * \param[in]   pParams      The NvKmsModeValidationParams.
 * \param[in]   pKmsMode     The mode to be considered.
 * \param[out]  pTimingsEvo  The EVO mode timings to be programmed in hardware.
 *
 * \return  If the mode is valid, return TRUE and populate pTimingsEvo.
 *          If the mode is not valid, return FALSE.
 */
NvBool nvValidateModeForModeset(NVDpyEvoRec *pDpyEvo,
                                const struct NvKmsModeValidationParams *pParams,
                                const struct NvKmsMode *pKmsMode,
                                const struct NvKmsSize *pViewPortSizeIn,
                                const struct NvKmsRect *pViewPortOut,
                                NVDpyAttributeColor *pDpyColor,
                                NVHwModeTimingsEvo *pTimingsEvo,
                                NVT_VIDEO_INFOFRAME_CTRL *pInfoFrameCtrl)
{
    EvoValidateModeFlags flags;
    struct NvKmsMode kmsMode = *pKmsMode;
    NVT_VIDEO_INFOFRAME_CTRL infoFrameCtrl;
    struct NvKmsModeValidationValidSyncs dummyValidSyncs;

    nvkms_memset(pTimingsEvo, 0, sizeof(*pTimingsEvo));

    if (!ConstructModeTimingsMetaData(pDpyEvo,
                                      pParams,
                                      &kmsMode,
                                      &flags,
                                      &infoFrameCtrl)) {
        return FALSE;
    }

    if (!ValidateModeTimings(pDpyEvo,
                             pKmsMode,
                             &flags,
                             pParams,
                             &dummyInfoString,
                             &dummyValidSyncs)) {
        return FALSE;
    }

    if (!nvConstructHwModeTimingsEvo(pDpyEvo,
                                     &kmsMode,
                                     pViewPortSizeIn,
                                     pViewPortOut,
                                     pDpyColor,
                                     pTimingsEvo,
                                     pParams,
                                     &dummyInfoString)) {
        return FALSE;
    }

    if (pInfoFrameCtrl != NULL) {
        *pInfoFrameCtrl = infoFrameCtrl;
    }

    return TRUE;
}
