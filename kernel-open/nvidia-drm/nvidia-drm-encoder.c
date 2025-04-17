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

#include "nvidia-drm-priv.h"
#include "nvidia-drm-encoder.h"
#include "nvidia-drm-utils.h"
#include "nvidia-drm-connector.h"
#include "nvidia-drm-crtc.h"
#include "nvidia-drm-helper.h"

#include "nvmisc.h"

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

static void nv_drm_encoder_destroy(struct drm_encoder *encoder)
{
    struct nv_drm_encoder *nv_encoder = to_nv_encoder(encoder);

    drm_encoder_cleanup(encoder);

    nv_drm_free(nv_encoder);
}

static const struct drm_encoder_funcs nv_encoder_funcs = {
    .destroy = nv_drm_encoder_destroy,
};

static bool nv_drm_encoder_mode_fixup(struct drm_encoder *encoder,
                                      const struct drm_display_mode *mode,
                                      struct drm_display_mode *adjusted_mode)
{
    return true;
}

static void nv_drm_encoder_prepare(struct drm_encoder *encoder)
{

}

static void nv_drm_encoder_commit(struct drm_encoder *encoder)
{

}

static void nv_drm_encoder_mode_set(struct drm_encoder *encoder,
                                    struct drm_display_mode *mode,
                                    struct drm_display_mode *adjusted_mode)
{

}

static const struct drm_encoder_helper_funcs nv_encoder_helper_funcs = {
    .mode_fixup = nv_drm_encoder_mode_fixup,
    .prepare    = nv_drm_encoder_prepare,
    .commit     = nv_drm_encoder_commit,
    .mode_set   = nv_drm_encoder_mode_set,
};

static uint32_t get_crtc_mask(struct drm_device *dev, uint32_t headMask)
{
    struct drm_crtc *crtc = NULL;
    uint32_t crtc_mask = 0x0;

    nv_drm_for_each_crtc(crtc, dev) {
        struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);

        if (headMask & NVBIT(nv_crtc->head)) {
            crtc_mask |= drm_crtc_mask(crtc);
        }
    }

    return crtc_mask;
}

/*
 * Helper function to create new encoder for given NvKmsKapiDisplay
 * with given signal format.
 */
static struct drm_encoder*
nv_drm_encoder_new(struct drm_device *dev,
                   NvKmsKapiDisplay hDisplay,
                   NvKmsConnectorSignalFormat format,
                   unsigned int crtc_mask)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    struct nv_drm_encoder *nv_encoder = NULL;

    int ret = 0;

    /* Allocate an NVIDIA encoder object */

    nv_encoder = nv_drm_calloc(1, sizeof(*nv_encoder));

    if (nv_encoder == NULL) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to allocate memory for NVIDIA-DRM encoder object");
        return ERR_PTR(-ENOMEM);
    }

    nv_encoder->hDisplay = hDisplay;

    /* Initialize the base encoder object and add it to the drm subsystem */

    ret = drm_encoder_init(dev,
                           &nv_encoder->base, &nv_encoder_funcs,
                           nvkms_connector_signal_to_drm_encoder_signal(format)
#if defined(NV_DRM_ENCODER_INIT_HAS_NAME_ARG)
                           , NULL
#endif
                           );

    if (ret != 0) {
        nv_drm_free(nv_encoder);

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to initialize encoder created from NvKmsKapiDisplay 0x%08x",
            hDisplay);
        return ERR_PTR(ret);
    }

    nv_encoder->base.possible_crtcs = crtc_mask;

    drm_encoder_helper_add(&nv_encoder->base, &nv_encoder_helper_funcs);

    return &nv_encoder->base;
}

/*
 * Add encoder for given NvKmsKapiDisplay
 */
struct drm_encoder*
nv_drm_add_encoder(struct drm_device *dev, NvKmsKapiDisplay hDisplay)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    struct NvKmsKapiStaticDisplayInfo *displayInfo = NULL;
    struct NvKmsKapiConnectorInfo *connectorInfo = NULL;

    struct drm_encoder *encoder = NULL;
    struct nv_drm_encoder *nv_encoder = NULL;

    struct drm_connector *connector = NULL;

    int ret = 0;

    /* Query NvKmsKapiStaticDisplayInfo and NvKmsKapiConnectorInfo */

    if ((displayInfo = nv_drm_calloc(1, sizeof(*displayInfo))) == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    if (!nvKms->getStaticDisplayInfo(nv_dev->pDevice, hDisplay, displayInfo)) {
        ret = -EINVAL;
        goto done;
    }

    connectorInfo = nvkms_get_connector_info(nv_dev->pDevice,
                                             displayInfo->connectorHandle);

    if (IS_ERR(connectorInfo)) {
        ret = PTR_ERR(connectorInfo);
        goto done;
    }

    /* Create and add drm encoder */

    encoder = nv_drm_encoder_new(dev,
                                 displayInfo->handle,
                                 connectorInfo->signalFormat,
                                 get_crtc_mask(dev, displayInfo->headMask));

    if (IS_ERR(encoder)) {
        ret = PTR_ERR(encoder);
        goto done;
    }

    /* Get connector from respective physical index */

    connector =
        nv_drm_get_connector(dev,
                                 connectorInfo->physicalIndex,
                                 connectorInfo->type,
                                 displayInfo->internal, displayInfo->dpAddress);

    if (IS_ERR(connector)) {
        ret = PTR_ERR(connector);
        goto failed_connector_encoder_attach;
    }

    /* Attach encoder and connector */

    ret = nv_drm_connector_attach_encoder(connector, encoder);

    if (ret != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to attach encoder created from NvKmsKapiDisplay 0x%08x "
            "to connector",
            hDisplay);
        goto failed_connector_encoder_attach;
    }

    nv_encoder = to_nv_encoder(encoder);

    mutex_lock(&dev->mode_config.mutex);

    nv_encoder->nv_connector = to_nv_connector(connector);

    nv_drm_connector_mark_connection_status_dirty(nv_encoder->nv_connector);

    mutex_unlock(&dev->mode_config.mutex);

    goto done;

failed_connector_encoder_attach:

    drm_encoder_cleanup(encoder);

    nv_drm_free(encoder);

done:

    nv_drm_free(displayInfo);

    nv_drm_free(connectorInfo);

    return ret != 0 ? ERR_PTR(ret) : encoder;
}

static inline struct nv_drm_encoder*
get_nv_encoder_from_nvkms_display(struct drm_device *dev,
                                  NvKmsKapiDisplay hDisplay)
{
    struct drm_encoder *encoder;

    nv_drm_for_each_encoder(encoder, dev) {
        struct nv_drm_encoder *nv_encoder = to_nv_encoder(encoder);

        if (nv_encoder->hDisplay == hDisplay) {
            return nv_encoder;
        }
    }

    return NULL;
}

void nv_drm_handle_display_change(struct nv_drm_device *nv_dev,
                                  NvKmsKapiDisplay hDisplay)
{
    struct drm_device *dev = nv_dev->dev;
    struct nv_drm_encoder *nv_encoder = NULL;

    mutex_lock(&dev->mode_config.mutex);

    nv_encoder = get_nv_encoder_from_nvkms_display(dev, hDisplay);

    mutex_unlock(&dev->mode_config.mutex);

    if (nv_encoder == NULL) {
        return;
    }

    nv_drm_connector_mark_connection_status_dirty(nv_encoder->nv_connector);

    schedule_delayed_work(&nv_dev->hotplug_event_work, 0);
}

void nv_drm_handle_dynamic_display_connected(struct nv_drm_device *nv_dev,
                                             NvKmsKapiDisplay hDisplay)
{
    struct drm_device *dev = nv_dev->dev;

    struct drm_encoder *encoder = NULL;
    struct nv_drm_encoder *nv_encoder = NULL;

    /*
     * Look for an existing encoder with the same hDisplay and
     * use it if available.
     */

    nv_encoder = get_nv_encoder_from_nvkms_display(dev, hDisplay);

    if (nv_encoder != NULL) {
        NV_DRM_DEV_LOG_INFO(
            nv_dev,
            "Encoder with NvKmsKapiDisplay 0x%08x already exists.",
            hDisplay);
        return;
    }

    encoder = nv_drm_add_encoder(dev, hDisplay);

    if (IS_ERR(encoder)) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to add encoder for NvKmsKapiDisplay 0x%08x",
            hDisplay);
        return;
    }

    /*
     * On some kernels, DRM has the notion of a "primary group" that
     * tracks the global mode setting state for the device.
     *
     * On kernels where DRM has a primary group, we need to reinitialize
     * after adding encoders and connectors.
     */
#if defined(NV_DRM_REINIT_PRIMARY_MODE_GROUP_PRESENT)
    drm_reinit_primary_mode_group(dev);
#endif

    schedule_delayed_work(&nv_dev->hotplug_event_work, 0);
}
#endif
