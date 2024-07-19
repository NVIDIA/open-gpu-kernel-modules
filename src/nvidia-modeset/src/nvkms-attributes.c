/*
 * SPDX-FileCopyrightText: Copyright (c) 2013 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-evo.h"
#include "nvkms-types.h"
#include "nvkms-attributes.h"
#include "nvkms-dpy.h"
#include "nvkms-framelock.h"
#include "nvkms-vrr.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvos.h"
#include "nvkms-stereo.h"
#include "nvkms-hdmi.h"

#include <ctrl/ctrl0073/ctrl0073dp.h> // NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_*

/*!
 * Set the current backlight brightness for the given pDpyEvo.
 *
 * \param[in] pDpyEvo     The display device whose backlight brightness
 *                        should be assigned.
 * \param[in] brightness  The backlight brightness value to program
 *
 * \return  TRUE if backlight brightness is available for this pDpyEvo,
 *          otherwise FALSE.
 */
static NvBool DpySetBacklightBrightness(NVDpyEvoRec *pDpyEvo, NvS64 brightness)
{
    NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    if (!pDpyEvo->hasBacklightBrightness) {
        return FALSE;
    }

    if (brightness > NV0073_CTRL_BACKLIGHT_BRIGHTNESS_MAX_VALUE) {
        return FALSE;
    }

    if (brightness < NV0073_CTRL_BACKLIGHT_BRIGHTNESS_MIN_VALUE) {
        return FALSE;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyEvoGetConnectorId(pDpyEvo);
    params.brightness = brightness;
    params.brightnessType = NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_PERCENT100;

    ret = nvRmApiControl(
            nvEvoGlobal.clientHandle,
            pDevEvo->displayCommonHandle,
            NV0073_CTRL_CMD_SPECIFIC_SET_BACKLIGHT_BRIGHTNESS,
            &params, sizeof(params));

    return (ret == NVOS_STATUS_SUCCESS);
}

/*!
 * Query the current backlight brightness for the given pDpyEvo.
 *
 * \param[in] pDpyEvo       The display device whose backlight brightness
 *                          should be queried.
 * \param[out] pBrightness  The backlight brightness value
 *
 * \return  TRUE if backlight brightness is available for this pDpyEvo,
 *          otherwise FALSE.
 */
static NvBool DpyGetBacklightBrightness(const NVDpyEvoRec *pDpyEvo,
                                        NvS64 *pBrightness)
{
    NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyEvoGetConnectorId(pDpyEvo);
    params.brightnessType = NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_PERCENT100;

    ret = nvRmApiControl(
            nvEvoGlobal.clientHandle,
            pDevEvo->displayCommonHandle,
            NV0073_CTRL_CMD_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS,
            &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    nvAssert(params.brightness <= NV0073_CTRL_BACKLIGHT_BRIGHTNESS_MAX_VALUE);

    *pBrightness = params.brightness;

    return TRUE;
}

/*!
 * Populate NvKmsAttributeValidValuesCommonReply for backlight brightness.
 *
 * \param[in]  pDpyEvo       The display device whose backlight brightness
 *                           should be queried.
 * \param[out] pValidValues  The ValidValues structure to populate.
 *
 * \return  TRUE if backlight brightness is available for this pDpy,
 *          otherwise FALSE.
 */
static NvBool DpyGetBacklightBrightnessValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!pDpyEvo->hasBacklightBrightness) {
        return FALSE;
    }

    pValidValues->type = NV_KMS_ATTRIBUTE_TYPE_RANGE;

    pValidValues->u.range.min = NV0073_CTRL_BACKLIGHT_BRIGHTNESS_MIN_VALUE;
    pValidValues->u.range.max = NV0073_CTRL_BACKLIGHT_BRIGHTNESS_MAX_VALUE;

    return TRUE;
}

/*!
 * Query RM for the current scanline of the given pDpyEvo.
 *
 * \param[in] pDpyEvo     The display device whose scanline
 *                        should be queried.
 * \param[out] pScanLine  The scanline value.
 *
 * \return  TRUE if the scanline could be queried for this pDpyEvo,
 *          otherwise FALSE.
 */
static NvBool GetScanLine(const NVDpyEvoRec *pDpyEvo, NvS64 *pScanLine)
{
    NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 head, ret;

    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return FALSE;
    }

    head = nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead);
    nvAssert(head != NV_INVALID_HEAD);

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.head = head;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SYSTEM_GET_SCANLINE,
                         &params, sizeof(params));

    if (ret == NVOS_STATUS_SUCCESS) {
        *pScanLine = params.currentScanline;
        return TRUE;
    }

    return FALSE;
}

/*!
 * Retrieve the current head of the given pDpyEvo.
 *
 * \param[in] pDpyEvo     The display device whose head
 *                        should be queried.
 * \param[out] pHead      The head value.
 *
 * \return  TRUE. If there is no valid head pHead will
 *          return NV_INVALID_HEAD
 */
static NvBool GetHead(const NVDpyEvoRec *pDpyEvo, NvS64 *pHead)
{
    *pHead = (NvS64)pDpyEvo->apiHead;
    return TRUE;
}

static NvBool GetHwHead(const NVDpyEvoRec *pDpyEvo, NvS64 *pHead)
{
    NvU32 primaryHwHead =
        nvGetPrimaryHwHead(pDpyEvo->pDispEvo, pDpyEvo->apiHead);
    *pHead = (NvS64)primaryHwHead;
    return TRUE;
}

static NvBool DitherConfigurationAllowed(const NVDpyEvoRec *pDpyEvo)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    return pDevEvo->hal->caps.supportedDitheringModes != 0;
}

static void SetDitheringCommon(NVDpyEvoPtr pDpyEvo)
{
    NVEvoUpdateState updateState = { };
    const NVConnectorEvoRec *pConnectorEvo = pDpyEvo->pConnectorEvo;
    NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState;
    NvU32 head;

    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return;
    }
    pApiHeadState = &pDispEvo->apiHeadState[pDpyEvo->apiHead];

    nvAssert((pApiHeadState->hwHeadsMask) != 0x0 &&
             (nvDpyIdIsInDpyIdList(pDpyEvo->id, pApiHeadState->activeDpys)));

    nvChooseDitheringEvo(pConnectorEvo,
                         pApiHeadState->attributes.color.bpc,
                         &pDpyEvo->requestedDithering,
                         &pApiHeadState->attributes.dithering);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        nvSetDitheringEvo(pDispEvo,
                          head,
                          &pApiHeadState->attributes.dithering,
                          &updateState);
    }

    nvEvoUpdateAndKickOff(pDpyEvo->pDispEvo, FALSE, &updateState,
                          TRUE /* releaseElv */);
}

/*!
 * Assigns dithering on all dpys driven by pDpyEvo's head.
 */
static NvBool SetDithering(NVDpyEvoRec *pDpyEvo, NvS64 dithering)
{
    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    switch (dithering) {
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_AUTO:
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_ENABLED:
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DISABLED:
        break;
    default:
        return FALSE;
    }

    pDpyEvo->requestedDithering.state = dithering;

    SetDitheringCommon(pDpyEvo);

    return TRUE;
}

static NvBool GetDithering(const NVDpyEvoRec *pDpyEvo, NvS64 *pDithering)
{
    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    *pDithering = pDpyEvo->requestedDithering.state;

    return TRUE;
}

static NvBool GetDitheringGenericValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    return DitherConfigurationAllowed(pDpyEvo);
}

/*!
 * Assigns ditheringMode on all dpys driven by pDpyEvo's head.
 */
static NvBool SetDitheringMode(NVDpyEvoRec *pDpyEvo, NvS64 ditheringMode)
{
    NVDevEvoPtr pDevEvo = pDpyEvo->pDispEvo->pDevEvo;
    NvU32 mask = (1 << ditheringMode);

    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    if (!(mask & pDevEvo->hal->caps.supportedDitheringModes)) {
        return FALSE;
    }

    switch (ditheringMode) {
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_AUTO:
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_DYNAMIC_2X2:
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_STATIC_2X2:
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_TEMPORAL:
        break;
    default:
        return FALSE;
    }

    pDpyEvo->requestedDithering.mode = ditheringMode;

    SetDitheringCommon(pDpyEvo);

    return TRUE;
}

static NvBool GetDitheringMode(const NVDpyEvoRec *pDpyEvo,
                               NvS64 *pDitheringMode)
{
    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    *pDitheringMode = pDpyEvo->requestedDithering.mode;

    return TRUE;
}

static NvBool GetDitheringModeValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTBITS);

    pValidValues->u.bits.ints =
        pDevEvo->hal->caps.supportedDitheringModes;

    return TRUE;
}

/*!
 * Assigns ditheringDepth on all dpys driven by pDpyEvo's head.
 */
static NvBool SetDitheringDepth(NVDpyEvoRec *pDpyEvo, NvS64 ditheringDepth)
{
    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    switch (ditheringDepth) {
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_AUTO:
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_6_BITS:
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_8_BITS:
        break;
    default:
        return FALSE;
    }

    pDpyEvo->requestedDithering.depth = ditheringDepth;

    SetDitheringCommon(pDpyEvo);

    return TRUE;
}

static NvBool GetDitheringDepth(const NVDpyEvoRec *pDpyEvo,
                                NvS64 *pDitheringDepth)
{
    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    *pDitheringDepth = pDpyEvo->requestedDithering.depth;

    return TRUE;
}

static NvBool GetCurrentDithering(const NVDpyEvoRec *pDpyEvo,
                                  NvS64 *pCurrentDithering)
{
    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    *pCurrentDithering = pDpyEvo->currentAttributes.dithering.enabled;

    return TRUE;
}

static NvBool GetCurrentDitheringMode(const NVDpyEvoRec *pDpyEvo,
                                      NvS64 *pCurrentDitheringMode)
{
    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    *pCurrentDitheringMode =
        pDpyEvo->currentAttributes.dithering.mode;

    return TRUE;
}

static NvBool GetCurrentDitheringDepth(const NVDpyEvoRec *pDpyEvo,
                                       NvS64 *pCurrentDitheringDepth)
{

    if (!DitherConfigurationAllowed(pDpyEvo)) {
        return FALSE;
    }

    *pCurrentDitheringDepth =
        pDpyEvo->currentAttributes.dithering.depth;

    return TRUE;
}

static NvBool DigitalVibranceAvailable(const NVDpyEvoRec *pDpyEvo)
{
    return nvDpyEvoIsActive(pDpyEvo);
}

/*!
 * Assigns dvc on all dpys driven by pDpyEvo's head.
 */
static NvBool SetDigitalVibrance(NVDpyEvoRec *pDpyEvo, NvS64 dvc)
{
    NVEvoUpdateState updateState = { };
    NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState;
    NvU32 head;

    if ((pDpyEvo->apiHead == NV_INVALID_HEAD) ||
            !DigitalVibranceAvailable(pDpyEvo)) {
        return FALSE;
    }
    pApiHeadState = &pDispEvo->apiHeadState[pDpyEvo->apiHead];

    nvAssert((pApiHeadState->hwHeadsMask) != 0x0 &&
             (nvDpyIdIsInDpyIdList(pDpyEvo->id, pApiHeadState->activeDpys)));

    dvc = NV_MAX(dvc, NV_EVO_DVC_MIN);
    dvc = NV_MIN(dvc, NV_EVO_DVC_MAX);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        nvSetDVCEvo(pDispEvo, head, dvc, &updateState);
    }

    nvEvoUpdateAndKickOff(pDpyEvo->pDispEvo, FALSE, &updateState,
                          TRUE /* releaseElv */);

    pApiHeadState->attributes.dvc = dvc;

    return TRUE;
}

static NvBool GetDigitalVibrance(const NVDpyEvoRec *pDpyEvo, NvS64 *pDvc)
{
    if (!DigitalVibranceAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pDvc = pDpyEvo->currentAttributes.dvc;

    return TRUE;
}

static NvBool GetDigitalVibranceValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!DigitalVibranceAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_RANGE);

    pValidValues->u.range.min = NV_EVO_DVC_MIN;
    pValidValues->u.range.max = NV_EVO_DVC_MAX;

    return TRUE;
}

static NvBool ImageSharpeningAvailable(const NVDpyEvoRec *pDpyEvo)
{
    if (!pDpyEvo->pDispEvo->pDevEvo->hal->caps.supportsImageSharpening) {
        return FALSE;
    }

    if (!nvDpyEvoIsActive(pDpyEvo)) {
        return FALSE;
    }

    return pDpyEvo->currentAttributes.imageSharpening.available;
}

/*!
 * Assigns imageSharpening on all dpys driven by pDpyEvo's head.
 */
static NvBool SetImageSharpening(NVDpyEvoRec *pDpyEvo, NvS64 imageSharpening)
{
    NVEvoUpdateState updateState = { };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState;
    NvU32 head;

    if ((pDpyEvo->apiHead == NV_INVALID_HEAD) ||
            !ImageSharpeningAvailable(pDpyEvo)) {
        return FALSE;
    }
    pApiHeadState = &pDispEvo->apiHeadState[pDpyEvo->apiHead];

    nvAssert((pApiHeadState->hwHeadsMask) != 0x0 &&
             (nvDpyIdIsInDpyIdList(pDpyEvo->id, pApiHeadState->activeDpys)));

    imageSharpening = NV_MAX(imageSharpening, NV_EVO_IMAGE_SHARPENING_MIN);
    imageSharpening = NV_MIN(imageSharpening, NV_EVO_IMAGE_SHARPENING_MAX);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        nvSetImageSharpeningEvo(pDispEvo, head, imageSharpening, &updateState);
    }

    nvEvoUpdateAndKickOff(pDispEvo, FALSE, &updateState,
                          TRUE /* releaseElv */);

    pApiHeadState->attributes.imageSharpening.value = imageSharpening;

    return TRUE;
}

static NvBool GetImageSharpening(const NVDpyEvoRec *pDpyEvo,
                                 NvS64 *pImageSharpening)
{
    if (!ImageSharpeningAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pImageSharpening = pDpyEvo->currentAttributes.imageSharpening.value;

    return TRUE;
}

static NvBool GetImageSharpeningValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!ImageSharpeningAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_RANGE);

    pValidValues->u.range.min = NV_EVO_IMAGE_SHARPENING_MIN;
    pValidValues->u.range.max = NV_EVO_IMAGE_SHARPENING_MAX;

    return TRUE;
}

static NvBool GetImageSharpeningAvailable(const NVDpyEvoRec *pDpyEvo,
                                          NvS64 *pImageSharpeningAvailable)
{
    *pImageSharpeningAvailable = ImageSharpeningAvailable(pDpyEvo);

    return TRUE;
}

static NvBool GetImageSharpeningDefault(const NVDpyEvoRec *pDpyEvo,
                                        NvS64 *pImageSharpeningDefault)
{
    if (!nvDpyEvoIsActive(pDpyEvo)) {
        return FALSE;
    }

    *pImageSharpeningDefault = NV_EVO_IMAGE_SHARPENING_DEFAULT;

    return TRUE;
}

static NvBool ColorSpaceAndRangeAvailable(const NVDpyEvoRec *pDpyEvo)
{
    return ((pDpyEvo->pConnectorEvo->legacyType ==
             NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) &&
            (pDpyEvo->pConnectorEvo->signalFormat !=
             NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI));
}

/*!
 * Send infoFrame with new color{Space,Range}.
 */
static void DpyPostColorSpaceOrRangeSetEvo(NVDpyEvoPtr pDpyEvo)
{
    enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace;
    enum NvKmsDpyAttributeColorBpcValue colorBpc;
    enum NvKmsDpyAttributeColorRangeValue colorRange;
    NVEvoUpdateState updateState = { };
    NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState;
    NvU32 head;

    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return;
    }
    pApiHeadState = &pDispEvo->apiHeadState[pDpyEvo->apiHead];

    nvAssert((pApiHeadState->hwHeadsMask) != 0x0 &&
             (nvDpyIdIsInDpyIdList(pDpyEvo->id, pApiHeadState->activeDpys)));

    /*
     * Choose current colorSpace and colorRange based on the current mode
     * timings and the requested color space and range.
     */
    if (!nvChooseCurrentColorSpaceAndRangeEvo(pDpyEvo,
                                              pApiHeadState->timings.yuv420Mode,
                                              pApiHeadState->attributes.color.colorimetry,
                                              pDpyEvo->requestedColorSpace,
                                              NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN,
                                              pDpyEvo->requestedColorRange,
                                              &colorSpace,
                                              &colorBpc,
                                              &colorRange)) {
        nvAssert(!"Failed to choose current color space and color range");
        return;
    }

    /* For DP, neither color space nor bpc can be changed without a modeset */
    if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo) &&
            ((pApiHeadState->attributes.color.format != colorSpace) ||
             (pApiHeadState->attributes.color.bpc != colorBpc))) {
        return;
    }

    /*
     * Hardware does not support HDMI FRL with YUV422, and it is not possible
     * to downgrade current color bpc on HDMI FRL at this point.
     */
    if ((pApiHeadState->timings.protocol == NVKMS_PROTOCOL_SOR_HDMI_FRL) &&
            ((colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422) ||
             (pApiHeadState->attributes.color.bpc > colorBpc))) {
        return;
    }

    if (nvDpyIsHdmiEvo(pDpyEvo) &&
            (colorBpc > pApiHeadState->attributes.color.bpc)) {
        NVDpyAttributeColor tmpDpyColor = pApiHeadState->attributes.color;

        tmpDpyColor.format = colorSpace;
        tmpDpyColor.range = colorRange;
        tmpDpyColor.bpc = colorBpc;

        /*
         * For HDMI FRL, downgrade the selected color bpc to the current color
         * bpc so that the current color bpc remains unchanged.
         */
        if (pApiHeadState->timings.protocol == NVKMS_PROTOCOL_SOR_HDMI_FRL) {
            tmpDpyColor.bpc = pApiHeadState->attributes.color.bpc;
        } else {
            const NvKmsDpyOutputColorFormatInfo colorFormatsInfo =
                nvDpyGetOutputColorFormatInfo(pDpyEvo);

            while (nvHdmiGetEffectivePixelClockKHz(pDpyEvo,
                                                   &pApiHeadState->timings,
                                                   &tmpDpyColor) >
                       pDpyEvo->maxSingleLinkPixelClockKHz) {

                if(!nvDowngradeColorSpaceAndBpc(&colorFormatsInfo,
                                                &tmpDpyColor)) {
                    return;
                }
            }
        }

        pApiHeadState->attributes.color.format = tmpDpyColor.format;
        pApiHeadState->attributes.color.range = tmpDpyColor.range;
        pApiHeadState->attributes.color.bpc = tmpDpyColor.bpc;
    } else {
        pApiHeadState->attributes.color.format = colorSpace;
        pApiHeadState->attributes.color.range = colorRange;
        pApiHeadState->attributes.color.bpc = colorBpc;
    }

    /* Update hardware's current colorSpace and colorRange */
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        enum nvKmsPixelDepth newPixelDepth =
            nvEvoDpyColorToPixelDepth(&pApiHeadState->attributes.color);

        nvUpdateCurrentHardwareColorSpaceAndRangeEvo(pDispEvo,
                                                     head,
                                                     &pApiHeadState->attributes.color,
                                                     &updateState);

        if (newPixelDepth != pDispEvo->headState[head].pixelDepth) {
            pDispEvo->headState[head].pixelDepth = newPixelDepth;
            nvEvoHeadSetControlOR(pDispEvo, head, &updateState);
        }
    }

    /* Update InfoFrames as needed. */
    nvUpdateInfoFrames(pDpyEvo);

    // Kick off
    nvEvoUpdateAndKickOff(pDispEvo, FALSE, &updateState, TRUE /* releaseElv */);

    // XXX DisplayPort sets color format.
}

static NvU32 DpyGetValidColorSpaces(const NVDpyEvoRec *pDpyEvo)
{
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;
    NvU32 val = (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_RGB);

    if ((nvDpyIsHdmiEvo(pDpyEvo) &&
            (pDevEvo->caps.hdmiYCbCr422MaxBpc != 0)) ||
        (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo) &&
            (pDevEvo->caps.dpYCbCr422MaxBpc != 0))) {
        val |= (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr422);
    }

    if (nvDpyIsHdmiEvo(pDpyEvo) ||
            nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        val |= (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr444);
    }

    return val;
}

NvBool nvDpyValidateColorSpace(const NVDpyEvoRec *pDpyEvo, NvS64 value)
{
    NvU32 validMask = DpyGetValidColorSpaces(pDpyEvo);

    if (!ColorSpaceAndRangeAvailable(pDpyEvo) || !(validMask & (1 << value))) {
        return FALSE;
    }

    return TRUE;
}

static NvBool SetRequestedColorSpace(NVDpyEvoRec *pDpyEvo, NvS64 value)
{
    if (!nvDpyValidateColorSpace(pDpyEvo, value)) {
        return FALSE;
    }

    pDpyEvo->requestedColorSpace = value;

    DpyPostColorSpaceOrRangeSetEvo(pDpyEvo);

    return TRUE;
}

static NvBool GetCurrentColorSpace(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->currentAttributes.color.format;

    return TRUE;
}

static NvBool GetRequestedColorSpace(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->requestedColorSpace;

    return TRUE;
}

static NvBool GetCurrentColorSpaceValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTBITS);

    pValidValues->u.bits.ints = DpyGetValidColorSpaces(pDpyEvo);

    /*
     * The current color space may be YUV420 depending on the current mode.
     * Rather than determine whether this pDpy is capable of driving any
     * YUV420 modes, just assume this is always a valid current color space.
     */
    pValidValues->u.bits.ints |=
        (1 << NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420);

    return TRUE;
}

static NvBool GetRequestedColorSpaceValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTBITS);

    pValidValues->u.bits.ints = DpyGetValidColorSpaces(pDpyEvo);

    return TRUE;
}

static NvBool SetRequestedColorRange(NVDpyEvoRec *pDpyEvo, NvS64 value)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    pDpyEvo->requestedColorRange = value;

    DpyPostColorSpaceOrRangeSetEvo(pDpyEvo);

    return TRUE;
}

static NvBool GetCurrentColorRange(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->currentAttributes.color.range;

    return TRUE;
}

static NvBool GetRequestedColorRange(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->requestedColorRange;

    return TRUE;
}

static NvBool GetColorRangeValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!ColorSpaceAndRangeAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTBITS);

    /*
     * The preferred color range may always select between full or limited
     * range, but the actual resulting color range depends on the current
     * color space.  Both color ranges are always valid values for both
     * preferred and current color range attributes.
     */
    pValidValues->u.bits.ints = (1 << NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL) |
                                (1 << NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED);

    return TRUE;
}

static NvBool GetCurrentColorBpc(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    *pValue = pDpyEvo->currentAttributes.color.bpc;
    return TRUE;
}

static NvBool GetColorBpcValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTBITS);

    /* If new enum values are added, update the u.bits.ints assignment. */
    ct_assert(NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_MAX ==
                NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10);

    pValidValues->u.bits.ints =
        NVBIT(NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6) |
        NVBIT(NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8) |
        NVBIT(NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10);

    return TRUE;
}

static NvBool DigitalSignalAvailable(const NVDpyEvoRec *pDpyEvo)
{
    return pDpyEvo->pConnectorEvo->legacyType ==
                NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP;
}

static NvBool GetDigitalSignal(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!DigitalSignalAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->currentAttributes.digitalSignal;

    return TRUE;
}

static NvBool GetDigitalSignalValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!DigitalSignalAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTEGER);

    return TRUE;
}

static NvBool DigitalLinkTypeAvailable(const NVDpyEvoRec *pDpyEvo)
{
    return (nvDpyEvoIsActive(pDpyEvo) &&
            (pDpyEvo->pConnectorEvo->legacyType ==
             NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP));
}

static NvBool GetDigitalLinkType(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!DigitalLinkTypeAvailable(pDpyEvo)) {
        return FALSE;
    }

    if (nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        *pValue = nvRMLaneCountToNvKms(pDpyEvo->dp.laneCount);
    } else {
        enum nvKmsTimingsProtocol protocol;
        const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
        NvU32 head = nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead);

        nvAssert(head != NV_INVALID_HEAD);
        protocol = pDispEvo->headState[head].timings.protocol;
#if defined(DEBUG)
        {
            NvU32 h;
            FOR_EACH_EVO_HW_HEAD(pDispEvo, pDpyEvo->apiHead, h) {
                nvAssert(protocol == pDispEvo->headState[h].timings.protocol);
            }
        }
#endif

        *pValue = (protocol == NVKMS_PROTOCOL_SOR_DUAL_TMDS) ?
            NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_DUAL :
            NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_SINGLE;
    }

    return TRUE;
}

static NvBool GetDigitalLinkTypeValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!DigitalLinkTypeAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTEGER);

    return TRUE;
}

static NvBool DisplayportLinkRateAvailable(const NVDpyEvoRec *pDpyEvo)
{
    return ((pDpyEvo->pConnectorEvo->legacyType ==
             NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) &&
            nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo));
}

static NvBool GetDisplayportLinkRate(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!DisplayportLinkRateAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->dp.linkRate;

    return TRUE;
}

static NvBool GetDisplayportLinkRateValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!DisplayportLinkRateAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTEGER);

    return TRUE;
}

static NvBool GetDisplayportLinkRate10MHz(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (!DisplayportLinkRateAvailable(pDpyEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->dp.linkRate10MHz;

    return TRUE;
}

static NvBool GetDisplayportLinkRate10MHzValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!DisplayportLinkRateAvailable(pDpyEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTEGER);

    return TRUE;
}

static NvBool GetDisplayportConnectorType(const NVDpyEvoRec *pDpyEvo,
                                          NvS64 *pValue)
{
    if (!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->dp.connectorType;

    return TRUE;
}

static NvBool GetDisplayportConnectorTypeValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_INTEGER);

    return TRUE;
}

static NvBool GetDisplayportIsMultistream(const NVDpyEvoRec *pDpyEvo,
                                          NvS64 *pValue)
{
    if (!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        return FALSE;
    }

    *pValue = nvDpyEvoIsDPMST(pDpyEvo);

    return TRUE;
}

static NvBool GetDisplayportIsMultistreamValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_BOOLEAN);

    return TRUE;
}

static NvBool GetDisplayportSinkIsAudioCapable(const NVDpyEvoRec *pDpyEvo,
                                               NvS64 *pValue)
{
    if (!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        return FALSE;
    }

    *pValue = pDpyEvo->dp.sinkIsAudioCapable;

    return TRUE;
}

static NvBool GetDisplayportSinkIsAudioCapableValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    if (!nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo)) {
        return FALSE;
    }

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_BOOLEAN);

    return TRUE;
}

NvS64 nvRMLaneCountToNvKms(NvU32 rmLaneCount)
{
    switch (rmLaneCount) {
    case NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_0:
        // fallthrough
    default:
        nvAssert(!"Unexpected DisplayPort lane configuration!");
        // fallthrough
    case NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_1:
        return NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_SINGLE;
    case NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_2:
        return NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_DUAL;
    case NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_4:
        return NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE_QUAD;
    }
}

static NvBool SetStereoEvo(NVDpyEvoPtr pDpyEvo, NvS64 value)
{
    NvBool enable = !!value;

    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return FALSE;
    }

    return nvSetStereo(pDpyEvo->pDispEvo, pDpyEvo->apiHead, enable);
}

static NvBool GetStereoEvo(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return FALSE;
    }

    *pValue = !!nvGetStereo(pDpyEvo->pDispEvo, pDpyEvo->apiHead);

    return TRUE;
}

static NvBool GetVrrMinRefreshRate(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    NvU32 timeoutMicroseconds;
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    NvU32 head;

    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return FALSE;
    }

    head = nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead);
    nvAssert(head != NV_INVALID_HEAD);
    timeoutMicroseconds =
        pDispEvo->headState[head].timings.vrr.timeoutMicroseconds;
#if defined(DEBUG)
    {
        NvU32 h;
        FOR_EACH_EVO_HW_HEAD(pDispEvo, pDpyEvo->apiHead, h) {
            nvAssert(timeoutMicroseconds ==
                         pDispEvo->headState[h].timings.vrr.timeoutMicroseconds);
        }
    }
#endif

    *pValue = timeoutMicroseconds ? (1000000 / timeoutMicroseconds) : 0;

    return TRUE;
}

static NvBool GetVrrMinRefreshRateValidValues(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsAttributeValidValuesCommonReply *pValidValues)
{
    NvU32 minMinRefreshRate, maxMinRefreshRate;
    const NVHwModeTimingsEvo *pTimings;
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    NvU32 head;

    if (pDpyEvo->apiHead == NV_INVALID_HEAD) {
        return FALSE;
    }

    head = nvGetPrimaryHwHead(pDispEvo, pDpyEvo->apiHead);
    nvAssert(head != NV_INVALID_HEAD);

    pTimings = &pDispEvo->headState[head].timings;

    nvGetDpyMinRefreshRateValidValues(pTimings,
                                      pDpyEvo->vrr.type,
                                      pTimings->vrr.timeoutMicroseconds,
                                      &minMinRefreshRate,
                                      &maxMinRefreshRate);
#if defined(DEBUG)
    {
        NvU32 h;
        FOR_EACH_EVO_HW_HEAD(pDispEvo, pDpyEvo->apiHead, h) {
            NvU32 tmpMinMinRefreshRate, tmpMaxMinRefreshRate;

            pTimings = &pDispEvo->headState[h].timings;

            nvGetDpyMinRefreshRateValidValues(pTimings,
                                              pDpyEvo->vrr.type,
                                              pTimings->vrr.timeoutMicroseconds,
                                              &tmpMinMinRefreshRate,
                                              &tmpMaxMinRefreshRate);

            nvAssert(tmpMinMinRefreshRate == minMinRefreshRate);
            nvAssert(tmpMaxMinRefreshRate == maxMinRefreshRate);
        }
    }
#endif

    nvAssert(pValidValues->type == NV_KMS_ATTRIBUTE_TYPE_RANGE);

    pValidValues->u.range.min = minMinRefreshRate;
    pValidValues->u.range.max = maxMinRefreshRate;

    return TRUE;
}

static NvBool GetNumberOfHardwareHeadsUsed(
    const NVDpyEvoRec *pDpyEvo,
    NvS64 *pNumHwHeadsUsed)
{
    *pNumHwHeadsUsed = pDpyEvo->currentAttributes.numberOfHardwareHeadsUsed;
    return TRUE;
}
static const struct {
    NvBool (*set)(NVDpyEvoPtr pDpyEvo, NvS64 value);
    NvBool (*get)(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue);
    NvBool (*getValidValues)(
        const NVDpyEvoRec *pDpyEvo,
        struct NvKmsAttributeValidValuesCommonReply *pValidValues);
    enum NvKmsAttributeType type;
} DpyAttributesDispatchTable[] = {
    [NV_KMS_DPY_ATTRIBUTE_BACKLIGHT_BRIGHTNESS] = {
        .set            = DpySetBacklightBrightness,
        .get            = DpyGetBacklightBrightness,
        .getValidValues = DpyGetBacklightBrightnessValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_DPY_ATTRIBUTE_SCANLINE] = {
        .set            = NULL,
        .get            = GetScanLine,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_HEAD] = {
        .set            = NULL,
        .get            = GetHead,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_HW_HEAD] = {
        .set            = NULL,
        .get            = GetHwHead,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING] = {
        .set            = SetDithering,
        .get            = GetDithering,
        .getValidValues = GetDitheringGenericValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE] = {
        .set            = SetDitheringMode,
        .get            = GetDitheringMode,
        .getValidValues = GetDitheringModeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH] = {
        .set            = SetDitheringDepth,
        .get            = GetDitheringDepth,
        .getValidValues = GetDitheringGenericValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING] = {
        .set            = NULL,
        .get            = GetCurrentDithering,
        .getValidValues = GetDitheringGenericValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE] = {
        .set            = NULL,
        .get            = GetCurrentDitheringMode,
        .getValidValues = GetDitheringGenericValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH] = {
        .set            = NULL,
        .get            = GetCurrentDitheringDepth,
        .getValidValues = GetDitheringGenericValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_DIGITAL_VIBRANCE] = {
        .set            = SetDigitalVibrance,
        .get            = GetDigitalVibrance,
        .getValidValues = GetDigitalVibranceValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_DPY_ATTRIBUTE_IMAGE_SHARPENING] = {
        .set            = SetImageSharpening,
        .get            = GetImageSharpening,
        .getValidValues = GetImageSharpeningValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_DPY_ATTRIBUTE_IMAGE_SHARPENING_AVAILABLE] = {
        .set            = NULL,
        .get            = GetImageSharpeningAvailable,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DPY_ATTRIBUTE_IMAGE_SHARPENING_DEFAULT] = {
        .set            = NULL,
        .get            = GetImageSharpeningDefault,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE] = {
        .set            = SetRequestedColorSpace,
        .get            = GetRequestedColorSpace,
        .getValidValues = GetRequestedColorSpaceValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE] = {
        .set            = NULL,
        .get            = GetCurrentColorSpace,
        .getValidValues = GetCurrentColorSpaceValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_RANGE] = {
        .set            = SetRequestedColorRange,
        .get            = GetRequestedColorRange,
        .getValidValues = GetColorRangeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_RANGE] = {
        .set            = NULL,
        .get            = GetCurrentColorRange,
        .getValidValues = GetColorRangeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC] = {
        .set            = NULL,
        .get            = GetCurrentColorBpc,
        .getValidValues = GetColorBpcValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_DPY_ATTRIBUTE_DIGITAL_SIGNAL] = {
        .set            = NULL,
        .get            = GetDigitalSignal,
        .getValidValues = GetDigitalSignalValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE] = {
        .set            = NULL,
        .get            = GetDigitalLinkType,
        .getValidValues = GetDigitalLinkTypeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_LINK_RATE] = {
        .set            = NULL,
        .get            = GetDisplayportLinkRate,
        .getValidValues = GetDisplayportLinkRateValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_LINK_RATE_10MHZ] = {
        .set            = NULL,
        .get            = GetDisplayportLinkRate10MHz,
        .getValidValues = GetDisplayportLinkRate10MHzValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE] = {
        .set            = NULL,
        .get            = GetDisplayportConnectorType,
        .getValidValues = GetDisplayportConnectorTypeValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_IS_MULTISTREAM] = {
        .set            = NULL,
        .get            = GetDisplayportIsMultistream,
        .getValidValues = GetDisplayportIsMultistreamValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_SINK_IS_AUDIO_CAPABLE] = {
        .set            = NULL,
        .get            = GetDisplayportSinkIsAudioCapable,
        .getValidValues = GetDisplayportSinkIsAudioCapableValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DPY_ATTRIBUTE_FRAMELOCK_DISPLAY_CONFIG] = {
        .set            = nvSetFrameLockDisplayConfigEvo,
        .get            = nvGetFrameLockDisplayConfigEvo,
        .getValidValues = nvGetFrameLockDisplayConfigValidValuesEvo,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTBITS,
    },
    [NV_KMS_DPY_ATTRIBUTE_RASTER_LOCK] = {
        .set            = NULL,
        .get            = nvQueryRasterLockEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DPY_ATTRIBUTE_UPDATE_FLIPLOCK] = {
        .set            = nvSetFlipLockEvo,
        .get            = nvGetFlipLockEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DPY_ATTRIBUTE_UPDATE_STEREO] = {
        .set            = SetStereoEvo,
        .get            = GetStereoEvo,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_BOOLEAN,
    },
    [NV_KMS_DPY_ATTRIBUTE_DPMS] = {
        .set            = nvRmSetDpmsEvo,
        .get            = NULL,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
    [NV_KMS_DPY_ATTRIBUTE_VRR_MIN_REFRESH_RATE] = {
        .set            = NULL,
        .get            = GetVrrMinRefreshRate,
        .getValidValues = GetVrrMinRefreshRateValidValues,
        .type           = NV_KMS_ATTRIBUTE_TYPE_RANGE,
    },
    [NV_KMS_DPY_ATTRIBUTE_NUMBER_OF_HARDWARE_HEADS_USED] = {
        .set            = NULL,
        .get            = GetNumberOfHardwareHeadsUsed,
        .getValidValues = NULL,
        .type           = NV_KMS_ATTRIBUTE_TYPE_INTEGER,
    },
};

/*!
 * Set pParams->attribute to pParams->value on the given dpy.
 */
NvBool nvSetDpyAttributeEvo(NVDpyEvoPtr pDpyEvo,
                            struct NvKmsSetDpyAttributeParams *pParams)
{
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(DpyAttributesDispatchTable)) {
        return FALSE;
    }

    if (DpyAttributesDispatchTable[index].set == NULL) {
        return FALSE;
    }

    if (!DpyAttributesDispatchTable[index].set(pDpyEvo,
                                               pParams->request.value)) {
        return FALSE;
    }

    if (pDpyEvo->apiHead != NV_INVALID_HEAD) {
        NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
        NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[pDpyEvo->apiHead];
        NVDpyEvoRec *pClonedDpyEvo;

        /*
         * The current attributes state should be consistent across all cloned
         * dpys.
         */
        FOR_ALL_EVO_DPYS(pClonedDpyEvo, pApiHeadState->activeDpys, pDispEvo) {
            nvDpyUpdateCurrentAttributes(pClonedDpyEvo);
        }
    } else {
        nvDpyUpdateCurrentAttributes(pDpyEvo);
    }

    return TRUE;
}

/*!
 * Get the value of pParams->attribute on the given dpy.
 */
NvBool nvGetDpyAttributeEvo(const NVDpyEvoRec *pDpyEvo,
                            struct NvKmsGetDpyAttributeParams *pParams)
{
    NvU32 index = pParams->request.attribute;

    if (index >= ARRAY_LEN(DpyAttributesDispatchTable)) {
        return FALSE;
    }

    if (DpyAttributesDispatchTable[index].get == NULL) {
        return FALSE;
    }

    return DpyAttributesDispatchTable[index].get(pDpyEvo,
                                                 &pParams->reply.value);
}

/*!
 * Get the valid values of pParams->attribute on the given dpy.
 */
NvBool nvGetDpyAttributeValidValuesEvo(
    const NVDpyEvoRec *pDpyEvo,
    struct NvKmsGetDpyAttributeValidValuesParams *pParams)
{
    NvU32 index = pParams->request.attribute;
    struct NvKmsAttributeValidValuesCommonReply *pReply =
        &pParams->reply.common;

    if (index >= ARRAY_LEN(DpyAttributesDispatchTable)) {
        return FALSE;
    }

    nvkms_memset(pReply, 0, sizeof(*pReply));

    pReply->readable = (DpyAttributesDispatchTable[index].get != NULL);
    pReply->writable = (DpyAttributesDispatchTable[index].set != NULL);

    pReply->type = DpyAttributesDispatchTable[index].type;

    /*
     * The getValidValues function provides three important things:
     * - If type==Range, then assigns reply::u::range.
     * - If type==IntBits, then assigns reply::u:bits::ints.
     * - If the attribute is not currently available, returns FALSE.
     * If the getValidValues function is NULL, assume the attribute is
     * available.  The type must not be something that requires assigning
     * to reply::u.
     */
    if (DpyAttributesDispatchTable[index].getValidValues == NULL) {
        nvAssert(pReply->type != NV_KMS_ATTRIBUTE_TYPE_INTBITS);
        nvAssert(pReply->type != NV_KMS_ATTRIBUTE_TYPE_RANGE);
        return TRUE;
    }

    return DpyAttributesDispatchTable[index].getValidValues(pDpyEvo, pReply);
}
