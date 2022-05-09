/*
 * Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-drm-conftest.h" /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include "nvidia-drm-helper.h"
#include "nvidia-drm-priv.h"
#include "nvidia-drm-connector.h"
#include "nvidia-drm-utils.h"
#include "nvidia-drm-encoder.h"

/*
 * Commit fcd70cd36b9b ("drm: Split out drm_probe_helper.h")
 * moves a number of helper function definitions from
 * drm/drm_crtc_helper.h to a new drm_probe_helper.h.
 */
#if defined(NV_DRM_DRM_PROBE_HELPER_H_PRESENT)
#include <drm/drm_probe_helper.h>
#endif
#include <drm/drm_crtc_helper.h>

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>

static void nv_drm_connector_destroy(struct drm_connector *connector)
{
    struct nv_drm_connector *nv_connector = to_nv_connector(connector);

    drm_connector_unregister(connector);

    drm_connector_cleanup(connector);

    if (nv_connector->edid != NULL) {
        nv_drm_free(nv_connector->edid);
    }

    nv_drm_free(nv_connector);
}

static bool
__nv_drm_detect_encoder(struct NvKmsKapiDynamicDisplayParams *pDetectParams,
                        struct drm_connector *connector,
                        struct drm_encoder *encoder)
{
    struct nv_drm_connector *nv_connector = to_nv_connector(connector);
    struct drm_device *dev = connector->dev;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct nv_drm_encoder *nv_encoder;

    /*
     * DVI-I connectors can drive both digital and analog
     * encoders.  If a digital connection has been forced then
     * skip analog encoders.
     */

    if (connector->connector_type == DRM_MODE_CONNECTOR_DVII &&
        connector->force == DRM_FORCE_ON_DIGITAL &&
        encoder->encoder_type == DRM_MODE_ENCODER_DAC) {
        return false;
    }

    nv_encoder = to_nv_encoder(encoder);

    memset(pDetectParams, 0, sizeof(*pDetectParams));

    pDetectParams->handle = nv_encoder->hDisplay;

    switch (connector->force) {
        case DRM_FORCE_ON:
        case DRM_FORCE_ON_DIGITAL:
            pDetectParams->forceConnected = NV_TRUE;
            break;
        case DRM_FORCE_OFF:
            pDetectParams->forceDisconnected = NV_TRUE;
            break;
        case DRM_FORCE_UNSPECIFIED:
            break;
    }

    if (connector->override_edid) {
        const struct drm_property_blob *edid = connector->edid_blob_ptr;

        if (edid->length <= sizeof(pDetectParams->edid.buffer)) {
            memcpy(pDetectParams->edid.buffer, edid->data, edid->length);
            pDetectParams->edid.bufferSize = edid->length;
            pDetectParams->overrideEdid = NV_TRUE;
        } else {
            WARN_ON(edid->length >
                    sizeof(pDetectParams->edid.buffer));
        }
    }

    if (!nvKms->getDynamicDisplayInfo(nv_dev->pDevice, pDetectParams)) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to detect display state");
        return false;
    }

    if (pDetectParams->connected) {
        if (!pDetectParams->overrideEdid && pDetectParams->edid.bufferSize) {

            if ((nv_connector->edid = nv_drm_calloc(
                        1,
                        pDetectParams->edid.bufferSize)) != NULL) {

                memcpy(nv_connector->edid,
                       pDetectParams->edid.buffer,
                       pDetectParams->edid.bufferSize);
            } else {
                NV_DRM_LOG_ERR("Out of Memory");
            }
        }

        return true;
    }

    return false;
}

static enum drm_connector_status __nv_drm_connector_detect_internal(
    struct drm_connector *connector)
{
    struct drm_device *dev = connector->dev;
    struct nv_drm_connector *nv_connector = to_nv_connector(connector);

    enum drm_connector_status status = connector_status_disconnected;

    struct drm_encoder *detected_encoder = NULL;
    struct nv_drm_encoder *nv_detected_encoder = NULL;
    struct drm_encoder *encoder;

    struct NvKmsKapiDynamicDisplayParams *pDetectParams = NULL;

    BUG_ON(!mutex_is_locked(&dev->mode_config.mutex));

    if (nv_connector->edid != NULL) {
        nv_drm_free(nv_connector->edid);
        nv_connector->edid = NULL;
    }

    if ((pDetectParams = nv_drm_calloc(
                1,
                sizeof(*pDetectParams))) == NULL) {
        WARN_ON(pDetectParams == NULL);
        goto done;
    }

    nv_drm_connector_for_each_possible_encoder(connector, encoder) {
        if (__nv_drm_detect_encoder(pDetectParams, connector, encoder)) {
            detected_encoder = encoder;
            break;
        }
    } nv_drm_connector_for_each_possible_encoder_end;

    if (detected_encoder == NULL) {
        goto done;
    }

    nv_detected_encoder = to_nv_encoder(detected_encoder);

    status = connector_status_connected;

    nv_connector->nv_detected_encoder = nv_detected_encoder;

    if (nv_connector->type == NVKMS_CONNECTOR_TYPE_DVI_I) {
        drm_object_property_set_value(
            &connector->base,
            dev->mode_config.dvi_i_subconnector_property,
            detected_encoder->encoder_type == DRM_MODE_ENCODER_DAC ?
                DRM_MODE_SUBCONNECTOR_DVIA :
                DRM_MODE_SUBCONNECTOR_DVID);
    }

done:

    nv_drm_free(pDetectParams);

    return status;
}

static void __nv_drm_connector_force(struct drm_connector *connector)
{
    __nv_drm_connector_detect_internal(connector);
}

static enum drm_connector_status
nv_drm_connector_detect(struct drm_connector *connector, bool force)
{
    return __nv_drm_connector_detect_internal(connector);
}

static struct drm_connector_funcs nv_connector_funcs = {
#if defined NV_DRM_ATOMIC_HELPER_CONNECTOR_DPMS_PRESENT
    .dpms                   = drm_atomic_helper_connector_dpms,
#endif
    .destroy                = nv_drm_connector_destroy,
    .reset                  = drm_atomic_helper_connector_reset,
    .force                  = __nv_drm_connector_force,
    .detect                 = nv_drm_connector_detect,
    .fill_modes             = drm_helper_probe_single_connector_modes,
    .atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
    .atomic_destroy_state   = drm_atomic_helper_connector_destroy_state,
};

static int nv_drm_connector_get_modes(struct drm_connector *connector)
{
    struct drm_device *dev = connector->dev;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct nv_drm_connector *nv_connector = to_nv_connector(connector);
    struct nv_drm_encoder *nv_detected_encoder =
                           nv_connector->nv_detected_encoder;
    NvU32 modeIndex = 0;
    int   count = 0;


    if (nv_connector->edid != NULL) {
        nv_drm_connector_update_edid_property(connector, nv_connector->edid);
    }

    while (1) {
        struct drm_display_mode *mode;
        struct NvKmsKapiDisplayMode displayMode;
        NvBool valid = 0;
        NvBool preferredMode = NV_FALSE;
        int ret;

        ret = nvKms->getDisplayMode(nv_dev->pDevice,
                                    nv_detected_encoder->hDisplay,
                                    modeIndex++, &displayMode, &valid,
                                    &preferredMode);

        if (ret < 0) {
            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "Failed to get mode at modeIndex %d of NvKmsKapiDisplay 0x%08x",
                modeIndex, nv_detected_encoder->hDisplay);
            break;
        }

        /* Is end of mode-list */

        if (ret == 0) {
            break;
        }

        /* Ignore invalid modes */

        if (!valid) {
            continue;
        }

        mode = drm_mode_create(connector->dev);

        if (mode == NULL) {
            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "Failed to create mode for NvKmsKapiDisplay 0x%08x",
                nv_detected_encoder->hDisplay);
            continue;
        }

        nvkms_display_mode_to_drm_mode(&displayMode, mode);

        if (preferredMode) {
            mode->type |= DRM_MODE_TYPE_PREFERRED;
        }

        /* Add a mode to a connector's probed_mode list */

        drm_mode_probed_add(connector, mode);

        count++;
    }

    return count;
}

static int nv_drm_connector_mode_valid(struct drm_connector    *connector,
                                       struct drm_display_mode *mode)
{
    struct drm_device *dev = connector->dev;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct nv_drm_encoder *nv_detected_encoder =
                           to_nv_connector(connector)->nv_detected_encoder;
    struct NvKmsKapiDisplayMode displayMode;

    if (nv_detected_encoder == NULL) {
        return MODE_BAD;
    }

    drm_mode_to_nvkms_display_mode(mode, &displayMode);

    if (!nvKms->validateDisplayMode(nv_dev->pDevice,
                                    nv_detected_encoder->hDisplay,
                                    &displayMode)) {
        return MODE_BAD;
    }

    return MODE_OK;
}

static struct drm_encoder*
nv_drm_connector_best_encoder(struct drm_connector *connector)
{
    struct nv_drm_connector *nv_connector = to_nv_connector(connector);

    if (nv_connector->nv_detected_encoder != NULL) {
        return &nv_connector->nv_detected_encoder->base;
    }

    return NULL;
}

static const struct drm_connector_helper_funcs nv_connector_helper_funcs = {
    .get_modes    = nv_drm_connector_get_modes,
    .mode_valid   = nv_drm_connector_mode_valid,
    .best_encoder = nv_drm_connector_best_encoder,
};

static struct drm_connector*
nv_drm_connector_new(struct drm_device *dev,
                     NvU32 physicalIndex, NvKmsConnectorType type,
                     NvBool internal,
                     char dpAddress[NVKMS_DP_ADDRESS_STRING_LENGTH])
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct nv_drm_connector *nv_connector = NULL;
    int ret = -ENOMEM;

    if ((nv_connector = nv_drm_calloc(1, sizeof(*nv_connector))) == NULL) {
        goto failed;
    }

    if ((nv_connector->base.state =
            nv_drm_calloc(1, sizeof(*nv_connector->base.state))) == NULL) {
        goto failed_state_alloc;
    }
    nv_connector->base.state->connector = &nv_connector->base;

    nv_connector->physicalIndex = physicalIndex;
    nv_connector->type     = type;
    nv_connector->internal = internal;

    strcpy(nv_connector->dpAddress, dpAddress);

    ret = drm_connector_init(
        dev,
        &nv_connector->base, &nv_connector_funcs,
        nvkms_connector_type_to_drm_connector_type(type, internal));

    if (ret != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to initialize connector created from physical index %u",
            nv_connector->physicalIndex);
        goto failed_connector_init;
    }

    drm_connector_helper_add(&nv_connector->base, &nv_connector_helper_funcs);

    nv_connector->base.polled = DRM_CONNECTOR_POLL_HPD;

    if (nv_connector->type == NVKMS_CONNECTOR_TYPE_VGA) {
        nv_connector->base.polled =
            DRM_CONNECTOR_POLL_CONNECT | DRM_CONNECTOR_POLL_DISCONNECT;
    }

    /* Register connector with DRM subsystem */

    ret = drm_connector_register(&nv_connector->base);

    if (ret != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to register connector created from physical index %u",
            nv_connector->physicalIndex);
        goto failed_connector_register;
    }

    return &nv_connector->base;

failed_connector_register:
    drm_connector_cleanup(&nv_connector->base);

failed_connector_init:
    nv_drm_free(nv_connector->base.state);

failed_state_alloc:
    nv_drm_free(nv_connector);

failed:
    return ERR_PTR(ret);
}

/*
 * Get connector with given physical index one exists. Otherwise, create and
 * return a new connector.
 */
struct drm_connector*
nv_drm_get_connector(struct drm_device *dev,
                     NvU32 physicalIndex, NvKmsConnectorType type,
                     NvBool internal,
                     char dpAddress[NVKMS_DP_ADDRESS_STRING_LENGTH])
{
    struct drm_connector *connector = NULL;
#if defined(NV_DRM_CONNECTOR_LIST_ITER_PRESENT)
    struct drm_connector_list_iter conn_iter;
    nv_drm_connector_list_iter_begin(dev, &conn_iter);
#else
    struct drm_mode_config *config = &dev->mode_config;
    mutex_lock(&config->mutex);
#endif

    /* Lookup for existing connector with same physical index */
    nv_drm_for_each_connector(connector, &conn_iter, dev) {
        struct nv_drm_connector *nv_connector = to_nv_connector(connector);

        if (nv_connector->physicalIndex == physicalIndex) {
            BUG_ON(nv_connector->type != type ||
                   nv_connector->internal != internal);

            if (strcmp(nv_connector->dpAddress, dpAddress) == 0) {
                goto done;
            }
        }
    }
    connector = NULL;

done:
#if defined(NV_DRM_CONNECTOR_LIST_ITER_PRESENT)
    nv_drm_connector_list_iter_end(&conn_iter);
#else
    mutex_unlock(&config->mutex);
#endif

    if (!connector) {
        connector = nv_drm_connector_new(dev,
                                         physicalIndex, type, internal,
                                         dpAddress);
    }

    return connector;
}

#endif
