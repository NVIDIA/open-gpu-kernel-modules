/*
 * Copyright (c) 2015-2022, NVIDIA CORPORATION. All rights reserved.
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

#include "nvidia-drm-conftest.h" /* NV_DRM_AVAILABLE */

#include "nvidia-drm-priv.h"
#include "nvidia-drm-drv.h"
#include "nvidia-drm-fb.h"
#include "nvidia-drm-modeset.h"
#include "nvidia-drm-encoder.h"
#include "nvidia-drm-connector.h"
#include "nvidia-drm-gem.h"
#include "nvidia-drm-crtc.h"
#include "nvidia-drm-fence.h"
#include "nvidia-drm-helper.h"
#include "nvidia-drm-gem-nvkms-memory.h"
#include "nvidia-drm-gem-user-memory.h"
#include "nvidia-drm-gem-dma-buf.h"
#include "nvidia-drm-utils.h"
#include "nv_dpy_id.h"

#if defined(NV_DRM_AVAILABLE)

#include "nvidia-drm-ioctl.h"

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#if defined(NV_DRM_DRM_ATOMIC_UAPI_H_PRESENT)
#include <drm/drm_atomic_uapi.h>
#endif

#include <drm/drm_vblank.h>
#include <drm/drm_file.h>
#include <drm/drm_prime.h>
#include <drm/drm_ioctl.h>

#if defined(NV_LINUX_APERTURE_H_PRESENT)
#include <linux/aperture.h>
#endif

#if defined(NV_DRM_DRM_APERTURE_H_PRESENT)
#include <drm/drm_aperture.h>
#endif

#if defined(NV_DRM_FBDEV_AVAILABLE)
#include <drm/drm_fb_helper.h>
#endif

#if defined(NV_DRM_DRM_CLIENT_SETUP_H_PRESENT)
#include <drm/drm_client_setup.h>
#elif defined(NV_DRM_CLIENTS_DRM_CLIENT_SETUP_H_PRESENT)
#include <drm/clients/drm_client_setup.h>
#endif

#if defined(NV_DRM_DRM_FBDEV_TTM_H_PRESENT)
#include <drm/drm_fbdev_ttm.h>
#elif defined(NV_DRM_DRM_FBDEV_GENERIC_H_PRESENT)
#include <drm/drm_fbdev_generic.h>
#endif

#include <linux/pci.h>
#include <linux/workqueue.h>
#include <linux/sort.h>

/*
 * Commit fcd70cd36b9b ("drm: Split out drm_probe_helper.h")
 * moves a number of helper function definitions from
 * drm/drm_crtc_helper.h to a new drm_probe_helper.h.
 */
#if defined(NV_DRM_DRM_PROBE_HELPER_H_PRESENT)
#include <drm/drm_probe_helper.h>
#endif
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem.h>
#include <drm/drm_auth.h>
#include <drm/drm_atomic_helper.h>

static int nv_drm_revoke_modeset_permission(struct drm_device *dev,
                                            struct drm_file *filep,
                                            NvU32 dpyId);
static int nv_drm_revoke_sub_ownership(struct drm_device *dev);

static struct nv_drm_device *dev_list = NULL;

static const char* nv_get_input_colorspace_name(
    enum nv_drm_input_color_space colorSpace)
{
    switch (colorSpace) {
        case NV_DRM_INPUT_COLOR_SPACE_NONE:
            return "None";
        case NV_DRM_INPUT_COLOR_SPACE_SCRGB_LINEAR:
            return "scRGB Linear FP16";
        case NV_DRM_INPUT_COLOR_SPACE_BT2100_PQ:
            return "BT.2100 PQ";
        default:
            /* We shoudn't hit this */
            WARN_ON("Unsupported input colorspace");
            return "None";
    }
};

static char* nv_get_transfer_function_name(
    enum nv_drm_transfer_function tf)
{
    switch (tf) {
        case NV_DRM_TRANSFER_FUNCTION_LINEAR:
            return "Linear";
        case NV_DRM_TRANSFER_FUNCTION_PQ:
            return "PQ (Perceptual Quantizer)";
        default:
            /* We shoudn't hit this */
            WARN_ON("Unsupported transfer function");
#if defined(fallthrough)
            fallthrough;
#else
            /* Fallthrough */
#endif
        case NV_DRM_TRANSFER_FUNCTION_DEFAULT:
            return "Default";
    }
};

#if defined(NV_DRM_OUTPUT_POLL_CHANGED_PRESENT)
static void nv_drm_output_poll_changed(struct drm_device *dev)
{
    struct drm_connector *connector = NULL;
    struct drm_mode_config *config = &dev->mode_config;
    struct drm_connector_list_iter conn_iter;
    drm_connector_list_iter_begin(dev, &conn_iter);
    /*
     * Here drm_mode_config::mutex has been acquired unconditionally.  The
     * mutex must be held for the duration of a fill_modes() call chain:
     *     connector->funcs->fill_modes()
     *      |-> drm_helper_probe_single_connector_modes()
     *
     * It is easiest to always acquire the mutex for the entire connector
     * loop.
     */
    mutex_lock(&config->mutex);

    drm_for_each_connector_iter(connector, &conn_iter) {

        struct nv_drm_connector *nv_connector = to_nv_connector(connector);

        if (!nv_drm_connector_check_connection_status_dirty_and_clear(
                nv_connector)) {
            continue;
        }

        connector->funcs->fill_modes(
            connector,
            dev->mode_config.max_width, dev->mode_config.max_height);
    }

    mutex_unlock(&config->mutex);
    drm_connector_list_iter_end(&conn_iter);
}
#endif /* NV_DRM_OUTPUT_POLL_CHANGED_PRESENT */

static const struct drm_mode_config_funcs nv_mode_config_funcs = {
    .fb_create = nv_drm_framebuffer_create,

    .atomic_state_alloc = nv_drm_atomic_state_alloc,
    .atomic_state_clear = nv_drm_atomic_state_clear,
    .atomic_state_free  = nv_drm_atomic_state_free,
    .atomic_check  = nv_drm_atomic_check,
    .atomic_commit = nv_drm_atomic_commit,

    #if defined(NV_DRM_OUTPUT_POLL_CHANGED_PRESENT)
    .output_poll_changed = nv_drm_output_poll_changed,
    #endif
};

static void nv_drm_event_callback(const struct NvKmsKapiEvent *event)
{
    struct nv_drm_device *nv_dev = event->privateData;

    mutex_lock(&nv_dev->lock);

    if (!atomic_read(&nv_dev->enable_event_handling)) {
        goto done;
    }

    switch (event->type) {
        case NVKMS_EVENT_TYPE_DPY_CHANGED:
            nv_drm_handle_display_change(
                nv_dev,
                event->u.displayChanged.display);
            break;

        case NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED:
            nv_drm_handle_dynamic_display_connected(
                nv_dev,
                event->u.dynamicDisplayConnected.display);
            break;
        case NVKMS_EVENT_TYPE_FLIP_OCCURRED:
            nv_drm_handle_flip_occurred(
                nv_dev,
                event->u.flipOccurred.head,
                event->u.flipOccurred.layer);
            break;
        default:
            break;
    }

done:

    mutex_unlock(&nv_dev->lock);
}

struct nv_drm_mst_display_info {
    NvKmsKapiDisplay handle;
    NvBool isDpMST;
    char dpAddress[NVKMS_DP_ADDRESS_STRING_LENGTH];
};

/*
 * Helper function to get DpMST display info.
 * dpMSTDisplayInfos is allocated dynamically,
 * so it needs to be freed after finishing the query.
 */
static int nv_drm_get_mst_display_infos
(
    struct nv_drm_device *nv_dev,
    NvKmsKapiDisplay hDisplay,
    struct nv_drm_mst_display_info **dpMSTDisplayInfos,
    NvU32 *nDynamicDisplays
)
{
    struct NvKmsKapiStaticDisplayInfo *displayInfo = NULL;
    struct NvKmsKapiStaticDisplayInfo *dynamicDisplayInfo = NULL;
    struct NvKmsKapiConnectorInfo *connectorInfo = NULL;
    struct nv_drm_mst_display_info *displayInfos = NULL;
    NvU32 i = 0;
    int ret = 0;
    NVDpyId dpyId;
    *nDynamicDisplays = 0;

    /* Query NvKmsKapiStaticDisplayInfo and NvKmsKapiConnectorInfo */

    if ((displayInfo = nv_drm_calloc(1, sizeof(*displayInfo))) == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    if ((dynamicDisplayInfo = nv_drm_calloc(1, sizeof(*dynamicDisplayInfo))) == NULL) {
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


    *nDynamicDisplays = nvCountDpyIdsInDpyIdList(connectorInfo->dynamicDpyIdList);

    if (*nDynamicDisplays == 0) {
        goto done;
    }

    if ((displayInfos = nv_drm_calloc(*nDynamicDisplays, sizeof(*displayInfos))) == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    FOR_ALL_DPY_IDS(dpyId, connectorInfo->dynamicDpyIdList) {
        if (!nvKms->getStaticDisplayInfo(nv_dev->pDevice,
                    nvDpyIdToNvU32(dpyId),
                    dynamicDisplayInfo)) {
            ret = -EINVAL;
            nv_drm_free(displayInfos);
            goto done;
        }

        displayInfos[i].handle = dynamicDisplayInfo->handle;
        displayInfos[i].isDpMST = dynamicDisplayInfo->isDpMST;
        memcpy(displayInfos[i].dpAddress, dynamicDisplayInfo->dpAddress, sizeof(dynamicDisplayInfo->dpAddress));

        i++;
    }

    *dpMSTDisplayInfos = displayInfos;

done:

    nv_drm_free(displayInfo);

    nv_drm_free(dynamicDisplayInfo);

    nv_drm_free(connectorInfo);

    return ret;
}

static int nv_drm_disp_cmp (const void *l, const void *r)
{
    struct nv_drm_mst_display_info *l_info = (struct nv_drm_mst_display_info *)l;
    struct nv_drm_mst_display_info *r_info = (struct nv_drm_mst_display_info *)r;

    return strcmp(l_info->dpAddress, r_info->dpAddress);
}

/*
 * Helper function to sort the dpAddress in terms of string.
 * This function is to create DRM connectors ID order deterministically.
 * It's not numerically.
 */
static void nv_drm_sort_dynamic_displays_by_dp_addr
(
    struct nv_drm_mst_display_info *infos,
    int nDynamicDisplays
)
{
    sort(infos, nDynamicDisplays, sizeof(*infos), nv_drm_disp_cmp, NULL);
}


/*
 * Helper function to initialize drm_device::mode_config from
 * NvKmsKapiDevice's resource information.
 */
static void
nv_drm_init_mode_config(struct nv_drm_device *nv_dev,
                        const struct NvKmsKapiDeviceResourcesInfo *pResInfo)
{
    struct drm_device *dev = nv_dev->dev;

    drm_mode_config_init(dev);
    drm_mode_create_dvi_i_properties(dev);

    dev->mode_config.funcs = &nv_mode_config_funcs;

    dev->mode_config.min_width  = pResInfo->caps.minWidthInPixels;
    dev->mode_config.min_height = pResInfo->caps.minHeightInPixels;

    dev->mode_config.max_width  = pResInfo->caps.maxWidthInPixels;
    dev->mode_config.max_height = pResInfo->caps.maxHeightInPixels;

    dev->mode_config.cursor_width  = pResInfo->caps.maxCursorSizeInPixels;
    dev->mode_config.cursor_height = pResInfo->caps.maxCursorSizeInPixels;

    /*
     * NVIDIA GPUs have no preferred depth. Arbitrarily report 24, to be
     * consistent with other DRM drivers.
     */

    dev->mode_config.preferred_depth = 24;
    dev->mode_config.prefer_shadow = 1;

#if defined(NV_DRM_CRTC_STATE_HAS_ASYNC_FLIP) || \
    defined(NV_DRM_CRTC_STATE_HAS_PAGEFLIP_FLAGS)
    dev->mode_config.async_page_flip = true;
#else
    dev->mode_config.async_page_flip = false;
#endif

#if defined(NV_DRM_MODE_CONFIG_HAS_ALLOW_FB_MODIFIERS)
    /* Allow clients to define framebuffer layouts using DRM format modifiers */
    dev->mode_config.allow_fb_modifiers = true;
#endif

    /* Initialize output polling support */

    drm_kms_helper_poll_init(dev);

    /* Disable output polling, because we don't support it yet */

    drm_kms_helper_poll_disable(dev);
}

/*
 * Helper function to enumerate encoders/connectors from NvKmsKapiDevice.
 */
static void nv_drm_enumerate_encoders_and_connectors
(
    struct nv_drm_device *nv_dev
)
{
    struct drm_device *dev = nv_dev->dev;
    NvU32 nDisplays = 0;

    if (!nvKms->getDisplays(nv_dev->pDevice, &nDisplays, NULL)) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to enumurate NvKmsKapiDisplay count");
    }

    if (nDisplays != 0) {
        NvKmsKapiDisplay *hDisplays =
            nv_drm_calloc(nDisplays, sizeof(*hDisplays));

        if (hDisplays != NULL) {
            if (!nvKms->getDisplays(nv_dev->pDevice, &nDisplays, hDisplays)) {
                NV_DRM_DEV_LOG_ERR(
                    nv_dev,
                    "Failed to enumurate NvKmsKapiDisplay handles");
            } else {
                NvU32 i, j;
                NvU32 nDynamicDisplays = 0;

                for (i = 0; i < nDisplays; i++) {
                    struct nv_drm_mst_display_info *displayInfos = NULL;
                    struct drm_encoder *encoder =
                        nv_drm_add_encoder(dev, hDisplays[i]);

                    if (IS_ERR(encoder)) {
                        NV_DRM_DEV_LOG_ERR(
                            nv_dev,
                            "Failed to add connector for NvKmsKapiDisplay 0x%08x",
                            hDisplays[i]);
                    }

                    if (nv_drm_get_mst_display_infos(nv_dev, hDisplays[i],
                            &displayInfos, &nDynamicDisplays)) {
                        NV_DRM_DEV_LOG_ERR(
                                nv_dev,
                                "Failed to get dynamic displays");
                    } else if (nDynamicDisplays) {
                        nv_drm_sort_dynamic_displays_by_dp_addr(displayInfos, nDynamicDisplays);

                        for (j = 0; j < nDynamicDisplays; j++) {
                            if (displayInfos[j].isDpMST) {
                                struct drm_encoder *mst_encoder =
                                    nv_drm_add_encoder(dev, displayInfos[j].handle);

                                NV_DRM_DEV_DEBUG_DRIVER(nv_dev, "found DP MST port display handle %u",
                                        displayInfos[j].handle);

                                if (IS_ERR(mst_encoder)) {
                                    NV_DRM_DEV_LOG_ERR(
                                            nv_dev,
                                            "Failed to add connector for NvKmsKapiDisplay 0x%08x",
                                            displayInfos[j].handle);
                                }
                            }
                        }

                        nv_drm_free(displayInfos);
                    }
                }
            }

            nv_drm_free(hDisplays);
        } else {
            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "Failed to allocate memory for NvKmsKapiDisplay array");
        }
    }
}

/*!
 * 'NV_DRM_OUT_FENCE_PTR' is an atomic per-plane property that clients can use
 * to request an out-fence fd for a particular plane that's being flipped.
 * 'NV_DRM_OUT_FENCE_PTR' does NOT have the same behavior as the standard
 * 'OUT_FENCE_PTR' property - the fd that's returned via 'NV_DRM_OUT_FENCE_PTR'
 * will only be signaled once the buffers in the corresponding flip are flipped
 * away from.
 * In order to use this property, client needs to call set property function
 * with user mode pointer as value. Once driver have post syncpt fd from flip reply,
 * it will copy post syncpt fd at location pointed by user mode pointer.
 */
static int nv_drm_create_properties(struct nv_drm_device *nv_dev)
{
    struct drm_prop_enum_list colorspace_enum_list[3] = { };
    struct drm_prop_enum_list tf_enum_list[NV_DRM_TRANSFER_FUNCTION_MAX] = { };
    int i, len = 0;

    for (i = 0; i < 3; i++) {
        colorspace_enum_list[len].type = i;
        colorspace_enum_list[len].name = nv_get_input_colorspace_name(i);
        len++;
    }

    for (i = 0; i < NV_DRM_TRANSFER_FUNCTION_MAX; i++) {
        tf_enum_list[i].type = i;
        tf_enum_list[i].name = nv_get_transfer_function_name(i);
    }

    if (nv_dev->supportsSyncpts) {
        nv_dev->nv_out_fence_property =
            drm_property_create_range(nv_dev->dev, DRM_MODE_PROP_ATOMIC,
                    "NV_DRM_OUT_FENCE_PTR", 0, U64_MAX);
        if (nv_dev->nv_out_fence_property == NULL) {
            return -ENOMEM;
        }
    }

    nv_dev->nv_input_colorspace_property =
        drm_property_create_enum(nv_dev->dev, 0, "NV_INPUT_COLORSPACE",
                                 colorspace_enum_list, len);
    if (nv_dev->nv_input_colorspace_property == NULL) {
        NV_DRM_LOG_ERR("Failed to create NV_INPUT_COLORSPACE property");
        return -ENOMEM;
    }

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    nv_dev->nv_hdr_output_metadata_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_HDR_STATIC_METADATA", 0);
    if (nv_dev->nv_hdr_output_metadata_property == NULL) {
        return -ENOMEM;
    }
#endif

    nv_dev->nv_plane_lms_ctm_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_PLANE_LMS_CTM", 0);
    if (nv_dev->nv_plane_lms_ctm_property == NULL) {
        return -ENOMEM;
    }

    nv_dev->nv_plane_lms_to_itp_ctm_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_PLANE_LMS_TO_ITP_CTM", 0);
    if (nv_dev->nv_plane_lms_to_itp_ctm_property == NULL) {
        return -ENOMEM;
    }

    nv_dev->nv_plane_itp_to_lms_ctm_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_PLANE_ITP_TO_LMS_CTM", 0);
    if (nv_dev->nv_plane_itp_to_lms_ctm_property == NULL) {
        return -ENOMEM;
    }

    nv_dev->nv_plane_blend_ctm_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_PLANE_BLEND_CTM", 0);
    if (nv_dev->nv_plane_blend_ctm_property == NULL) {
        return -ENOMEM;
    }

    // Degamma TF + LUT + LUT Size + Multiplier

    nv_dev->nv_plane_degamma_tf_property =
        drm_property_create_enum(nv_dev->dev, 0,
            "NV_PLANE_DEGAMMA_TF", tf_enum_list,
            NV_DRM_TRANSFER_FUNCTION_MAX);
    if (nv_dev->nv_plane_degamma_tf_property == NULL) {
        return -ENOMEM;
    }
    nv_dev->nv_plane_degamma_lut_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_PLANE_DEGAMMA_LUT", 0);
    if (nv_dev->nv_plane_degamma_lut_property == NULL) {
        return -ENOMEM;
    }
    nv_dev->nv_plane_degamma_lut_size_property =
        drm_property_create_range(nv_dev->dev, DRM_MODE_PROP_IMMUTABLE,
            "NV_PLANE_DEGAMMA_LUT_SIZE", 0, UINT_MAX);
    if (nv_dev->nv_plane_degamma_lut_size_property == NULL) {
        return -ENOMEM;
    }
    nv_dev->nv_plane_degamma_multiplier_property =
        drm_property_create_range(nv_dev->dev, 0,
            "NV_PLANE_DEGAMMA_MULTIPLIER", 0,
            U64_MAX & ~(((NvU64) 1) << 63)); // No negative values
    if (nv_dev->nv_plane_degamma_multiplier_property == NULL) {
        return -ENOMEM;
    }

    // TMO LUT + LUT Size

    nv_dev->nv_plane_tmo_lut_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_PLANE_TMO_LUT", 0);
    if (nv_dev->nv_plane_tmo_lut_property == NULL) {
        return -ENOMEM;
    }
    nv_dev->nv_plane_tmo_lut_size_property =
        drm_property_create_range(nv_dev->dev, DRM_MODE_PROP_IMMUTABLE,
            "NV_PLANE_TMO_LUT_SIZE", 0, UINT_MAX);
    if (nv_dev->nv_plane_tmo_lut_size_property == NULL) {
        return -ENOMEM;
    }

    // REGAMMA TF + LUT + LUT Size + Divisor

    nv_dev->nv_crtc_regamma_tf_property =
        drm_property_create_enum(nv_dev->dev, 0,
            "NV_CRTC_REGAMMA_TF", tf_enum_list,
            NV_DRM_TRANSFER_FUNCTION_MAX);
    if (nv_dev->nv_crtc_regamma_tf_property == NULL) {
        return -ENOMEM;
    }
    nv_dev->nv_crtc_regamma_lut_property =
        drm_property_create(nv_dev->dev, DRM_MODE_PROP_BLOB,
            "NV_CRTC_REGAMMA_LUT", 0);
    if (nv_dev->nv_crtc_regamma_lut_property == NULL) {
        return -ENOMEM;
    }
    nv_dev->nv_crtc_regamma_lut_size_property =
        drm_property_create_range(nv_dev->dev, DRM_MODE_PROP_IMMUTABLE,
            "NV_CRTC_REGAMMA_LUT_SIZE", 0, UINT_MAX);
    if (nv_dev->nv_crtc_regamma_lut_size_property == NULL) {
        return -ENOMEM;
    }
    // S31.32
    nv_dev->nv_crtc_regamma_divisor_property =
        drm_property_create_range(nv_dev->dev, 0,
            "NV_CRTC_REGAMMA_DIVISOR",
            (((NvU64) 1) << 32), // No values between 0 and 1
            U64_MAX & ~(((NvU64) 1) << 63)); // No negative values
    if (nv_dev->nv_crtc_regamma_divisor_property == NULL) {
        return -ENOMEM;
    }

    return 0;
}

/*
 * We can't just call drm_kms_helper_hotplug_event directly because
 * fbdev_generic may attempt to set a mode from inside the hotplug event
 * handler. Because kapi event handling runs on nvkms_kthread_q, this blocks
 * other event processing including the flip completion notifier expected by
 * nv_drm_atomic_commit.
 *
 * Defer hotplug event handling to a work item so that nvkms_kthread_q can
 * continue processing events while a DRM modeset is in progress.
 */
static void nv_drm_handle_hotplug_event(struct work_struct *work)
{
    struct delayed_work *dwork = to_delayed_work(work);
    struct nv_drm_device *nv_dev =
        container_of(dwork, struct nv_drm_device, hotplug_event_work);

    drm_kms_helper_hotplug_event(nv_dev->dev);
}

static int nv_drm_dev_load(struct drm_device *dev)
{
    struct NvKmsKapiDevice *pDevice;

    struct NvKmsKapiAllocateDeviceParams allocateDeviceParams;
    struct NvKmsKapiDeviceResourcesInfo resInfo;
    NvU64 kind;
    NvU64 gen;
    int i;
    int ret;

    struct nv_drm_device *nv_dev = to_nv_device(dev);

    NV_DRM_DEV_LOG_INFO(nv_dev, "Loading driver");

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return 0;
    }

    /* Allocate NvKmsKapiDevice from GPU ID */

    memset(&allocateDeviceParams, 0, sizeof(allocateDeviceParams));

    allocateDeviceParams.gpuId = nv_dev->gpu_info.gpu_id;
    allocateDeviceParams.migDevice = nv_dev->gpu_mig_device;

    allocateDeviceParams.privateData = nv_dev;
    allocateDeviceParams.eventCallback = nv_drm_event_callback;

    pDevice = nvKms->allocateDevice(&allocateDeviceParams);

    if (pDevice == NULL) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to allocate NvKmsKapiDevice");
        return -ENODEV;
    }

    /* Query information of resources available on device */

    if (!nvKms->getDeviceResourcesInfo(pDevice, &resInfo)) {

        nvKms->freeDevice(pDevice);

        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to query NvKmsKapiDevice resources info");
        return -ENODEV;
    }

#if defined(NV_DRM_FBDEV_AVAILABLE)
    /*
     * If fbdev is enabled, take modeset ownership now before other DRM clients
     * can take master (and thus NVKMS ownership).
     */
    if (nv_drm_fbdev_module_param) {
        if (!nvKms->grabOwnership(pDevice)) {
            nvKms->freeDevice(pDevice);
            NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to grab NVKMS modeset ownership");
            return -EBUSY;
        }

        nv_dev->hasFramebufferConsole = NV_TRUE;
    }
#endif

    mutex_lock(&nv_dev->lock);

    /* Set NvKmsKapiDevice */

    nv_dev->pDevice = pDevice;

    nv_dev->pitchAlignment = resInfo.caps.pitchAlignment;

    nv_dev->hasVideoMemory = resInfo.caps.hasVideoMemory;

    nv_dev->genericPageKind = resInfo.caps.genericPageKind;

    // Fermi-Volta use generation 0, Turing+ uses generation 2.
    nv_dev->pageKindGeneration = (nv_dev->genericPageKind == 0x06) ? 2 : 0;

    // Desktop GPUs and mobile GPUs Xavier and later use the same sector layout
    nv_dev->sectorLayout = 1;

    nv_dev->supportsSyncpts = resInfo.caps.supportsSyncpts;

    nv_dev->semsurf_stride = resInfo.caps.semsurf.stride;

    nv_dev->semsurf_max_submitted_offset =
        resInfo.caps.semsurf.maxSubmittedOffset;

    nv_dev->display_semaphores.count =
        resInfo.caps.numDisplaySemaphores;
    nv_dev->display_semaphores.next_index = 0;

    nv_dev->requiresVrrSemaphores = resInfo.caps.requiresVrrSemaphores;

    nv_dev->vtFbBaseAddress = resInfo.vtFbBaseAddress;
    nv_dev->vtFbSize = resInfo.vtFbSize;

    gen = nv_dev->pageKindGeneration;
    kind = nv_dev->genericPageKind;

    for (i = 0; i <= 5; i++) {
        nv_dev->modifiers[i] =
            /*    Log2(block height) ----------------------------------+  *
             *    Page Kind ------------------------------------+      |  *
             *    Gob Height/Page Kind Generation --------+     |      |  *
             *    Sector layout ---------------------+    |     |      |  *
             *    Compression --------------------+  |    |     |      |  *
             *                                    |  |    |     |      |  */
            DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D(0, 1, gen, kind, 5 - i);
    }

    nv_dev->modifiers[i++] = DRM_FORMAT_MOD_LINEAR;
    nv_dev->modifiers[i++] = DRM_FORMAT_MOD_INVALID;

    /* Initialize drm_device::mode_config */

    nv_drm_init_mode_config(nv_dev, &resInfo);

    ret = nv_drm_create_properties(nv_dev);
    if (ret < 0) {
        drm_mode_config_cleanup(dev);
#if defined(NV_DRM_FBDEV_AVAILABLE)
        if (nv_dev->hasFramebufferConsole) {
            nvKms->releaseOwnership(nv_dev->pDevice);
        }
#endif
        nvKms->freeDevice(nv_dev->pDevice);
        return -ENODEV;
    }

    if (!nvKms->declareEventInterest(
            nv_dev->pDevice,
            ((1 << NVKMS_EVENT_TYPE_DPY_CHANGED) |
             (1 << NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED) |
             (1 << NVKMS_EVENT_TYPE_FLIP_OCCURRED)))) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to register event mask");
    }

    /* Add crtcs */

    nv_drm_enumerate_crtcs_and_planes(nv_dev, &resInfo);

    /* Add connectors and encoders */

    nv_drm_enumerate_encoders_and_connectors(nv_dev);

#if !defined(NV_DRM_CRTC_STATE_HAS_NO_VBLANK)
    drm_vblank_init(dev, dev->mode_config.num_crtc);
#endif

    /*
     * Trigger hot-plug processing, to update connection status of
     * all HPD supported connectors.
     */

    drm_helper_hpd_irq_event(dev);

    /* Enable event handling */

    INIT_DELAYED_WORK(&nv_dev->hotplug_event_work, nv_drm_handle_hotplug_event);
    atomic_set(&nv_dev->enable_event_handling, true);

    init_waitqueue_head(&nv_dev->flip_event_wq);

    mutex_unlock(&nv_dev->lock);

    return 0;
}

static void nv_drm_dev_unload(struct drm_device *dev)
{
    struct NvKmsKapiDevice *pDevice = NULL;

    struct nv_drm_device *nv_dev = to_nv_device(dev);

    NV_DRM_DEV_LOG_INFO(nv_dev, "Unloading driver");

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return;
    }

    /* Release modeset ownership if fbdev is enabled */

#if defined(NV_DRM_FBDEV_AVAILABLE)
    if (nv_dev->hasFramebufferConsole) {
        drm_atomic_helper_shutdown(dev);
        nvKms->releaseOwnership(nv_dev->pDevice);
    }
#endif

    cancel_delayed_work_sync(&nv_dev->hotplug_event_work);
    mutex_lock(&nv_dev->lock);

    WARN_ON(nv_dev->subOwnershipGranted);

    /* Disable event handling */

    atomic_set(&nv_dev->enable_event_handling, false);

    /* Clean up output polling */

    drm_kms_helper_poll_fini(dev);

    /* Clean up mode configuration */

    drm_mode_config_cleanup(dev);

    if (!nvKms->declareEventInterest(nv_dev->pDevice, 0x0)) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to stop event listening");
    }

    /* Unset NvKmsKapiDevice */

    pDevice = nv_dev->pDevice;
    nv_dev->pDevice = NULL;

    mutex_unlock(&nv_dev->lock);

    nvKms->freeDevice(pDevice);
}

static int __nv_drm_master_set(struct drm_device *dev,
                               struct drm_file *file_priv, bool from_open)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    /*
     * If this device is driving a framebuffer, then nvidia-drm already has
     * modeset ownership. Otherwise, grab ownership now.
     */
    if (!nv_dev->hasFramebufferConsole &&
        !nvKms->grabOwnership(nv_dev->pDevice)) {
        return -EINVAL;
    }

    return 0;
}

#if defined(NV_DRM_DRIVER_SET_MASTER_HAS_INT_RETURN_TYPE)
static int nv_drm_master_set(struct drm_device *dev,
                             struct drm_file *file_priv, bool from_open)
{
    return __nv_drm_master_set(dev, file_priv, from_open);
}
#else
static void nv_drm_master_set(struct drm_device *dev,
                              struct drm_file *file_priv, bool from_open)
{
    if (__nv_drm_master_set(dev, file_priv, from_open) != 0) {
        NV_DRM_DEV_LOG_ERR(to_nv_device(dev), "Failed to grab modeset ownership");
    }
}
#endif

static
int nv_drm_reset_input_colorspace(struct drm_device *dev)
{
    struct drm_atomic_state *state;
    struct drm_plane_state *plane_state;
    struct drm_plane *plane;
    struct nv_drm_plane_state *nv_drm_plane_state;
    struct drm_modeset_acquire_ctx ctx;
    int ret = 0;
    bool do_reset = false;
    NvU32 flags = 0;

    state = drm_atomic_state_alloc(dev);
    if (!state)
        return -ENOMEM;

#if defined(DRM_MODESET_ACQUIRE_INTERRUPTIBLE)
    flags |= DRM_MODESET_ACQUIRE_INTERRUPTIBLE;
#endif
    drm_modeset_acquire_init(&ctx, flags);
    state->acquire_ctx = &ctx;

    nv_drm_for_each_plane(plane, dev) {
        plane_state = drm_atomic_get_plane_state(state, plane);
        if (IS_ERR(plane_state)) {
            ret = PTR_ERR(plane_state);
            goto out;
        }

        nv_drm_plane_state = to_nv_drm_plane_state(plane_state);
        if (nv_drm_plane_state) {
            if (nv_drm_plane_state->input_colorspace != NV_DRM_INPUT_COLOR_SPACE_NONE) {
                nv_drm_plane_state->input_colorspace = NV_DRM_INPUT_COLOR_SPACE_NONE;
                do_reset = true;
            }
        }
    }

    if (do_reset) {
        ret = drm_atomic_commit(state);
    }

out:
    drm_atomic_state_put(state);
    drm_modeset_drop_locks(&ctx);
    drm_modeset_acquire_fini(&ctx);

    return ret;
}

static
void nv_drm_master_drop(struct drm_device *dev, struct drm_file *file_priv)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    nv_drm_revoke_modeset_permission(dev, file_priv, 0);
    nv_drm_revoke_sub_ownership(dev);

    if (!nv_dev->hasFramebufferConsole) {
        int err;

        /*
         * After dropping nvkms modeset onwership, it is not guaranteed that drm
         * and nvkms modeset state will remain in sync.  Therefore, disable all
         * outputs and crtcs before dropping nvkms modeset ownership.
         *
         * First disable all active outputs atomically and then disable each
         * crtc one by one, there is not helper function available to disable
         * all crtcs atomically.
         */

        drm_modeset_lock_all(dev);

        if ((err = nv_drm_atomic_helper_disable_all(
                dev,
                dev->mode_config.acquire_ctx)) != 0) {

            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "nv_drm_atomic_helper_disable_all failed with error code %d !",
                err);
        }

        drm_modeset_unlock_all(dev);

        nvKms->releaseOwnership(nv_dev->pDevice);
    } else {
        int err = nv_drm_reset_input_colorspace(dev);
        if (err != 0) {
            NV_DRM_DEV_LOG_WARN(nv_dev,
            "nv_drm_reset_input_colorspace failed with error code: %d !", err);
        }
    }
}

static int nv_drm_get_dev_info_ioctl(struct drm_device *dev,
                                     void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_get_dev_info_params *params = data;

    if (dev->primary == NULL) {
        return -ENOENT;
    }

    params->gpu_id = nv_dev->gpu_info.gpu_id;
    params->mig_device = nv_dev->gpu_mig_device;
    params->primary_index = dev->primary->index;
    params->supports_alloc = false;
    params->generic_page_kind = 0;
    params->page_kind_generation = 0;
    params->sector_layout = 0;
    params->supports_sync_fd = false;
    params->supports_semsurf = false;

    /* Memory allocation and semaphore surfaces are only supported
     * if the modeset = 1 parameter is set */
    if (nv_dev->pDevice != NULL) {
        params->supports_alloc = true;
        params->generic_page_kind = nv_dev->genericPageKind;
        params->page_kind_generation = nv_dev->pageKindGeneration;
        params->sector_layout = nv_dev->sectorLayout;

        if (nv_dev->semsurf_stride != 0) {
            params->supports_semsurf = true;
            params->supports_sync_fd = true;
        }
    }

    return 0;
}

static int nv_drm_get_drm_file_unique_id_ioctl(struct drm_device *dev,
                                               void *data, struct drm_file *filep)
{
    struct drm_nvidia_get_drm_file_unique_id_params *params = data;
    params->id = (u64)(filep->driver_priv);
    return 0;
}

static int nv_drm_dmabuf_supported_ioctl(struct drm_device *dev,
                                         void *data, struct drm_file *filep)
{
    /* check the pDevice since this only gets set if modeset = 1
     * which is a requirement for the dma_buf extension to work
     */
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    return nv_dev->pDevice ? 0 : -EINVAL;
}

static
int nv_drm_get_client_capability_ioctl(struct drm_device *dev,
                                       void *data, struct drm_file *filep)
{
    struct drm_nvidia_get_client_capability_params *params = data;

    switch (params->capability) {
#if defined(DRM_CLIENT_CAP_STEREO_3D)
        case DRM_CLIENT_CAP_STEREO_3D:
            params->value = filep->stereo_allowed;
            break;
#endif
#if defined(DRM_CLIENT_CAP_UNIVERSAL_PLANES)
        case DRM_CLIENT_CAP_UNIVERSAL_PLANES:
            params->value = filep->universal_planes;
            break;
#endif
#if defined(DRM_CLIENT_CAP_ATOMIC)
        case DRM_CLIENT_CAP_ATOMIC:
            params->value = filep->atomic;
            break;
#endif
        default:
            return -EINVAL;
    }

    return 0;
}

static bool nv_drm_connector_is_dpy_id(struct drm_connector *connector,
                                       NvU32 dpyId)
{
    struct nv_drm_connector *nv_connector = to_nv_connector(connector);
    return nv_connector->nv_detected_encoder &&
           nv_connector->nv_detected_encoder->hDisplay == dpyId;
}

static int nv_drm_get_dpy_id_for_connector_id_ioctl(struct drm_device *dev,
                                                    void *data,
                                                    struct drm_file *filep)
{
    struct drm_nvidia_get_dpy_id_for_connector_id_params *params = data;
    struct drm_connector *connector;
    struct nv_drm_connector *nv_connector;
    int ret = 0;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    // Importantly, drm_connector_lookup will only return the
    // connector if we are master, a lessee with the connector, or not master at
    // all. It will return NULL if we are a lessee with other connectors.
    connector = drm_connector_lookup(dev, filep, params->connectorId);

    if (!connector) {
        return -EINVAL;
    }

    nv_connector = to_nv_connector(connector);
    if (!nv_connector) {
        ret = -EINVAL;
        goto done;
    }

    if (!nv_connector->nv_detected_encoder) {
        ret = -EINVAL;
        goto done;
    }

    params->dpyId = nv_connector->nv_detected_encoder->hDisplay;

done:
    drm_connector_put(connector);
    return ret;
}

static int nv_drm_get_connector_id_for_dpy_id_ioctl(struct drm_device *dev,
                                                    void *data,
                                                    struct drm_file *filep)
{
    struct drm_nvidia_get_connector_id_for_dpy_id_params *params = data;
    struct drm_connector *connector;
    int ret = -EINVAL;
    struct drm_connector_list_iter conn_iter;
    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    /* Lookup for existing connector with same dpyId */
    drm_connector_list_iter_begin(dev, &conn_iter);
    drm_for_each_connector_iter(connector, &conn_iter) {
        if (nv_drm_connector_is_dpy_id(connector, params->dpyId)) {
            params->connectorId = connector->base.id;
            ret = 0;
            break;
        }
    }
    drm_connector_list_iter_end(&conn_iter);

    return ret;
}

static NvU32 nv_drm_get_head_bit_from_connector(struct drm_connector *connector)
{
    struct nv_drm_connector *nv_connector = to_nv_connector(connector);

    if (connector->state && connector->state->crtc) {
        struct nv_drm_crtc *nv_crtc = to_nv_crtc(connector->state->crtc);
        return NVBIT(nv_crtc->head);
    } else if (nv_connector->nv_detected_encoder &&
               nv_connector->nv_detected_encoder->base.crtc) {
        struct nv_drm_crtc *nv_crtc =
            to_nv_crtc(nv_connector->nv_detected_encoder->base.crtc);
        return NVBIT(nv_crtc->head);
    }

    return 0;
}

static int nv_drm_grant_modeset_permission(struct drm_device *dev,
                                           struct drm_nvidia_grant_permissions_params *params,
                                           struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct nv_drm_connector *target_nv_connector = NULL;
    struct nv_drm_crtc *target_nv_crtc = NULL;
    struct drm_connector *connector, *target_connector = NULL;
    struct drm_crtc *crtc;
    NvU32 head = 0, freeHeadBits, targetHeadBit, possible_crtcs;
    int ret = 0;
    struct drm_connector_list_iter conn_iter;
#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    struct drm_modeset_acquire_ctx ctx;
    DRM_MODESET_LOCK_ALL_BEGIN(dev, ctx, DRM_MODESET_ACQUIRE_INTERRUPTIBLE,
                               ret);
#else
    mutex_lock(&dev->mode_config.mutex);
#endif

    /* Get the connector for the dpyId. */
    drm_connector_list_iter_begin(dev, &conn_iter);
    drm_for_each_connector_iter(connector, &conn_iter) {
        if (nv_drm_connector_is_dpy_id(connector, params->dpyId)) {
            target_connector =
                drm_connector_lookup(dev, filep, connector->base.id);
            break;
        }
    }
    drm_connector_list_iter_end(&conn_iter);

    // Importantly, drm_connector_lookup/drm_crtc_find (with filep) will only
    // return the object if we are master, a lessee with the object, or not
    // master at all. It will return NULL if we are a lessee with other objects.
    if (!target_connector) {
        ret = -EINVAL;
        goto done;
    }
    target_nv_connector = to_nv_connector(target_connector);
    possible_crtcs =
        target_nv_connector->nv_detected_encoder->base.possible_crtcs;

    /* Target connector must not be previously granted. */
    if (target_nv_connector->modeset_permission_filep) {
        ret = -EINVAL;
        goto done;
    }

    /* Add all heads that are owned and not already granted. */
    freeHeadBits = 0;
    nv_drm_for_each_crtc(crtc, dev) {
        struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);
        if (drm_crtc_find(dev, filep, crtc->base.id) &&
            !nv_crtc->modeset_permission_filep &&
            (drm_crtc_mask(crtc) & possible_crtcs)) {
            freeHeadBits |= NVBIT(nv_crtc->head);
        }
    }

    targetHeadBit = nv_drm_get_head_bit_from_connector(target_connector);
    if (targetHeadBit & freeHeadBits) {
        /* If a crtc is already being used by this connector, use it. */
        freeHeadBits = targetHeadBit;
    } else {
        /* Otherwise, remove heads that are in use by other connectors. */
        drm_connector_list_iter_begin(dev, &conn_iter);
        drm_for_each_connector_iter(connector, &conn_iter) {
            freeHeadBits &= ~nv_drm_get_head_bit_from_connector(connector);
        }
        drm_connector_list_iter_end(&conn_iter);
    }

    /* Fail if no heads are available. */
    if (!freeHeadBits) {
        ret = -EINVAL;
        goto done;
    }

    /*
     * Loop through the crtc again and find a matching head.
     * Record the filep that is using the crtc and the connector.
     */
    nv_drm_for_each_crtc(crtc, dev) {
        struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);
        if (freeHeadBits & NVBIT(nv_crtc->head)) {
            target_nv_crtc = nv_crtc;
            head = nv_crtc->head;
            break;
        }
    }

    if (!nvKms->grantPermissions(params->fd, nv_dev->pDevice, head,
                                 params->dpyId)) {
        ret = -EINVAL;
        goto done;
    }

    target_nv_connector->modeset_permission_crtc = target_nv_crtc;
    target_nv_connector->modeset_permission_filep = filep;
    target_nv_crtc->modeset_permission_filep = filep;

done:
    if (target_connector) {
        drm_connector_put(target_connector);
    }

#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    DRM_MODESET_LOCK_ALL_END(dev, ctx, ret);
#else
    mutex_unlock(&dev->mode_config.mutex);
#endif

    return ret;
}

static int nv_drm_grant_sub_ownership(struct drm_device *dev,
                                      struct drm_nvidia_grant_permissions_params *params)
{
    int ret = -EINVAL;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_modeset_acquire_ctx *pctx;
#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    struct drm_modeset_acquire_ctx ctx;
    DRM_MODESET_LOCK_ALL_BEGIN(dev, ctx, DRM_MODESET_ACQUIRE_INTERRUPTIBLE,
                                ret);
    pctx = &ctx;
#else
    mutex_lock(&dev->mode_config.mutex);
    pctx = dev->mode_config.acquire_ctx;
#endif

    if (nv_dev->subOwnershipGranted ||
        !nvKms->grantSubOwnership(params->fd, nv_dev->pDevice)) {
        goto done;
    }

    /*
     * When creating an ownership grant, shut down all heads and disable flip
     * notifications.
     */
    ret = nv_drm_atomic_helper_disable_all(dev, pctx);
    if (ret != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "nv_drm_atomic_helper_disable_all failed with error code %d!",
            ret);
    }

    atomic_set(&nv_dev->enable_event_handling, false);
    nv_dev->subOwnershipGranted = NV_TRUE;

    ret = 0;

done:
#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    DRM_MODESET_LOCK_ALL_END(dev, ctx, ret);
#else
    mutex_unlock(&dev->mode_config.mutex);
#endif
    return 0;
}

static int nv_drm_grant_permission_ioctl(struct drm_device *dev, void *data,
                                         struct drm_file *filep)
{
    struct drm_nvidia_grant_permissions_params *params = data;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    if (params->type == NV_DRM_PERMISSIONS_TYPE_MODESET) {
        return nv_drm_grant_modeset_permission(dev, params, filep);
    } else if (params->type == NV_DRM_PERMISSIONS_TYPE_SUB_OWNER) {
        return nv_drm_grant_sub_ownership(dev, params);
    }

    return -EINVAL;
}

static int
nv_drm_atomic_disable_connector(struct drm_atomic_state *state,
                                struct nv_drm_connector *nv_connector)
{
    struct drm_crtc_state *crtc_state;
    struct drm_connector_state *connector_state;
    int ret = 0;

    if (nv_connector->modeset_permission_crtc) {
        crtc_state = drm_atomic_get_crtc_state(
            state, &nv_connector->modeset_permission_crtc->base);
        if (!crtc_state) {
            return -EINVAL;
        }

        crtc_state->active = false;
        ret = drm_atomic_set_mode_prop_for_crtc(crtc_state, NULL);
        if (ret < 0) {
            return ret;
        }
    }

    connector_state = drm_atomic_get_connector_state(state, &nv_connector->base);
    if (!connector_state) {
        return -EINVAL;
    }

    return drm_atomic_set_crtc_for_connector(connector_state, NULL);
}

static int nv_drm_revoke_modeset_permission(struct drm_device *dev,
                                            struct drm_file *filep, NvU32 dpyId)
{
    struct drm_modeset_acquire_ctx *pctx;
    struct drm_atomic_state *state;
    struct drm_connector *connector;
    struct drm_crtc *crtc;
    int ret = 0;
    struct drm_connector_list_iter conn_iter;
#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    struct drm_modeset_acquire_ctx ctx;
    DRM_MODESET_LOCK_ALL_BEGIN(dev, ctx, DRM_MODESET_ACQUIRE_INTERRUPTIBLE,
                               ret);
    pctx = &ctx;
#else
    mutex_lock(&dev->mode_config.mutex);
    pctx = dev->mode_config.acquire_ctx;
#endif

    state = drm_atomic_state_alloc(dev);
    if (!state) {
        ret = -ENOMEM;
        goto done;
    }
    state->acquire_ctx = pctx;

    /*
     * If dpyId is set, only revoke those specific resources. Otherwise,
     * it is from closing the file so revoke all resources for that filep.
     */
    drm_connector_list_iter_begin(dev, &conn_iter);
    drm_for_each_connector_iter(connector, &conn_iter) {
        struct nv_drm_connector *nv_connector = to_nv_connector(connector);
        if (nv_connector->modeset_permission_filep == filep &&
            (!dpyId || nv_drm_connector_is_dpy_id(connector, dpyId))) {
            ret = nv_drm_atomic_disable_connector(state, nv_connector);
            if (ret < 0) {
                goto done;
            }

            // Continue trying to revoke as much as possible.
            nv_drm_connector_revoke_permissions(dev, nv_connector);
        }
    }
    drm_connector_list_iter_end(&conn_iter);

    nv_drm_for_each_crtc(crtc, dev) {
        struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);
        if (nv_crtc->modeset_permission_filep == filep && !dpyId) {
            nv_crtc->modeset_permission_filep = NULL;
        }
    }

    ret = drm_atomic_commit(state);
done:
    drm_atomic_state_put(state);

#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    DRM_MODESET_LOCK_ALL_END(dev, ctx, ret);
#else
    mutex_unlock(&dev->mode_config.mutex);
#endif

    return ret;
}

static int nv_drm_revoke_sub_ownership(struct drm_device *dev)
{
    int ret = -EINVAL;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    struct drm_modeset_acquire_ctx ctx;
    DRM_MODESET_LOCK_ALL_BEGIN(dev, ctx, DRM_MODESET_ACQUIRE_INTERRUPTIBLE,
                               ret);
#else
    mutex_lock(&dev->mode_config.mutex);
#endif

    if (!nv_dev->subOwnershipGranted) {
        goto done;
    }

    if (!nvKms->revokeSubOwnership(nv_dev->pDevice)) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to revoke sub-ownership from NVKMS");
        goto done;
    }

    nv_dev->subOwnershipGranted = NV_FALSE;
    atomic_set(&nv_dev->enable_event_handling, true);
    ret = 0;

done:
#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    DRM_MODESET_LOCK_ALL_END(dev, ctx, ret);
#else
    mutex_unlock(&dev->mode_config.mutex);
#endif
    return ret;
}

static int nv_drm_revoke_permission_ioctl(struct drm_device *dev, void *data,
                                          struct drm_file *filep)
{
    struct drm_nvidia_revoke_permissions_params *params = data;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    if (params->type == NV_DRM_PERMISSIONS_TYPE_MODESET) {
        if (!params->dpyId) {
            return -EINVAL;
        }
        return nv_drm_revoke_modeset_permission(dev, filep, params->dpyId);
    } else if (params->type == NV_DRM_PERMISSIONS_TYPE_SUB_OWNER) {
        return nv_drm_revoke_sub_ownership(dev);
    }

    return -EINVAL;
}

static void nv_drm_postclose(struct drm_device *dev, struct drm_file *filep)
{
    /*
     * Some systems like android can reach here without initializing the
     * device, so check for that.
     */
    if (dev->mode_config.num_crtc > 0 &&
        dev->mode_config.crtc_list.next != NULL &&
        dev->mode_config.crtc_list.prev != NULL &&
        dev->mode_config.num_connector > 0 &&
        dev->mode_config.connector_list.next != NULL &&
        dev->mode_config.connector_list.prev != NULL) {
        nv_drm_revoke_modeset_permission(dev, filep, 0);
    }
}

static int nv_drm_open(struct drm_device *dev, struct drm_file *filep)
{
    _Static_assert(sizeof(filep->driver_priv) >= sizeof(u64),
                   "filep->driver_priv can not hold an u64");
    static atomic64_t id = ATOMIC_INIT(0);

    filep->driver_priv = (void *)atomic64_inc_return(&id);

    return 0;
}

static struct drm_master *nv_drm_find_lessee(struct drm_master *master,
                                             int lessee_id)
{
    int object;
    void *entry;

    while (master->lessor != NULL) {
        master = master->lessor;
    }

    idr_for_each_entry(&master->lessee_idr, entry, object)
    {
        if (object == lessee_id) {
            return entry;
        }
    }

    return NULL;
}

static void nv_drm_get_revoked_objects(struct drm_device *dev,
                                       struct drm_file *filep, unsigned int cmd,
                                       unsigned long arg, int **objects,
                                       int *objects_count)
{
    unsigned int ioc_size;
    struct drm_mode_revoke_lease revoke_lease;
    struct drm_master *lessor, *lessee;
    void *entry;
    int *objs;
    int obj, obj_count, obj_i;

    ioc_size = _IOC_SIZE(cmd);
    if (ioc_size > sizeof(revoke_lease)) {
        return;
    }

    if (copy_from_user(&revoke_lease, (void __user *)arg, ioc_size) != 0) {
        return;
    }

    lessor = nv_drm_file_get_master(filep);
    if (lessor == NULL) {
        return;
    }

    mutex_lock(&dev->mode_config.idr_mutex);
    lessee = nv_drm_find_lessee(lessor, revoke_lease.lessee_id);

    if (lessee == NULL) {
        goto done;
    }

    obj_count = 0;
    idr_for_each_entry(&lessee->leases, entry, obj) {
        ++obj_count;
    }
    if (obj_count == 0) {
        goto done;
    }

    objs = nv_drm_calloc(obj_count, sizeof(int));
    if (objs == NULL) {
        goto done;
    }

    obj_i = 0;
    idr_for_each_entry(&lessee->leases, entry, obj) {
        objs[obj_i++] = obj;
    }
    *objects = objs;
    *objects_count = obj_count;

done:
    mutex_unlock(&dev->mode_config.idr_mutex);
    drm_master_put(&lessor);
}

static bool nv_drm_is_in_objects(int object, int *objects, int objects_count)
{
    int i;
    for (i = 0; i < objects_count; ++i) {
        if (objects[i] == object) {
            return true;
        }
    }
    return false;
}

static void nv_drm_finish_revoking_objects(struct drm_device *dev,
                                           struct drm_file *filep, int *objects,
                                           int objects_count)
{
    struct drm_connector *connector;
    struct drm_crtc *crtc;
    struct drm_connector_list_iter conn_iter;
#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    int ret = 0;
    struct drm_modeset_acquire_ctx ctx;
    DRM_MODESET_LOCK_ALL_BEGIN(dev, ctx, DRM_MODESET_ACQUIRE_INTERRUPTIBLE,
                               ret);
#else
    mutex_lock(&dev->mode_config.mutex);
#endif

    drm_connector_list_iter_begin(dev, &conn_iter);
    drm_for_each_connector_iter(connector, &conn_iter) {
        struct nv_drm_connector *nv_connector = to_nv_connector(connector);
        if (nv_connector->modeset_permission_filep &&
            nv_drm_is_in_objects(connector->base.id, objects, objects_count)) {
            nv_drm_connector_revoke_permissions(dev, nv_connector);
        }
    }
    drm_connector_list_iter_end(&conn_iter);

    nv_drm_for_each_crtc(crtc, dev) {
        struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);
        if (nv_crtc->modeset_permission_filep &&
            nv_drm_is_in_objects(crtc->base.id, objects, objects_count)) {
            nv_crtc->modeset_permission_filep = NULL;
        }
    }

#if NV_DRM_MODESET_LOCK_ALL_END_ARGUMENT_COUNT == 3
    DRM_MODESET_LOCK_ALL_END(dev, ctx, ret);
#else
    mutex_unlock(&dev->mode_config.mutex);
#endif
}

/*
 * Wrapper around drm_ioctl to hook in to upstream ioctl.
 *
 * Currently used to add additional handling to REVOKE_LEASE.
 */
static long nv_drm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    long retcode;

    struct drm_file *file_priv = filp->private_data;
    struct drm_device *dev = file_priv->minor->dev;
    int *objects = NULL;
    int objects_count = 0;

    if (cmd == DRM_IOCTL_MODE_REVOKE_LEASE) {
        // Save the revoked objects before revoking.
        nv_drm_get_revoked_objects(dev, file_priv, cmd, arg, &objects,
                                   &objects_count);
    }

    retcode = drm_ioctl(filp, cmd, arg);

    if (cmd == DRM_IOCTL_MODE_REVOKE_LEASE && objects) {
        if (retcode == 0) {
            // If revoking was successful, finish revoking the objects.
            nv_drm_finish_revoking_objects(dev, file_priv, objects,
                                           objects_count);
        }
        nv_drm_free(objects);
    }

    return retcode;
}

static const struct file_operations nv_drm_fops = {
    .owner          = THIS_MODULE,

    .open           = drm_open,
    .release        = drm_release,
    .unlocked_ioctl = nv_drm_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl   = drm_compat_ioctl,
#endif

    .mmap           = nv_drm_mmap,

    .poll           = drm_poll,
    .read           = drm_read,

    .llseek         = noop_llseek,

#if defined(FOP_UNSIGNED_OFFSET)
    .fop_flags   = FOP_UNSIGNED_OFFSET,
#endif
};

static const struct drm_ioctl_desc nv_drm_ioctls[] = {
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_IMPORT_NVKMS_MEMORY,
                      nv_drm_gem_import_nvkms_memory_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_IMPORT_USERSPACE_MEMORY,
                      nv_drm_gem_import_userspace_memory_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_MAP_OFFSET,
                      nv_drm_gem_map_offset_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GET_DEV_INFO,
                      nv_drm_get_dev_info_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GET_DRM_FILE_UNIQUE_ID,
                      nv_drm_get_drm_file_unique_id_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),

    DRM_IOCTL_DEF_DRV(NVIDIA_FENCE_SUPPORTED,
                      nv_drm_fence_supported_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_PRIME_FENCE_CONTEXT_CREATE,
                      nv_drm_prime_fence_context_create_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_PRIME_FENCE_ATTACH,
                      nv_drm_gem_prime_fence_attach_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_SEMSURF_FENCE_CTX_CREATE,
                      nv_drm_semsurf_fence_ctx_create_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_SEMSURF_FENCE_CREATE,
                      nv_drm_semsurf_fence_create_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_SEMSURF_FENCE_WAIT,
                      nv_drm_semsurf_fence_wait_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_SEMSURF_FENCE_ATTACH,
                      nv_drm_semsurf_fence_attach_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),

    /*
     * DRM_UNLOCKED is implicit for all non-legacy DRM driver IOCTLs since Linux
     * v4.10 commit fa5386459f06 "drm: Used DRM_LEGACY for all legacy functions"
     * (Linux v4.4 commit ea487835e887 "drm: Enforce unlocked ioctl operation
     * for kms driver ioctls" previously did it only for drivers that set the
     * DRM_MODESET flag), so this will race with SET_CLIENT_CAP. Linux v4.11
     * commit dcf727ab5d17 "drm: setclientcap doesn't need the drm BKL" also
     * removed locking from SET_CLIENT_CAP so there is no use attempting to lock
     * manually. The latter commit acknowledges that this can expose userspace
     * to inconsistent behavior when racing with itself, but accepts that risk.
     */
    DRM_IOCTL_DEF_DRV(NVIDIA_GET_CLIENT_CAPABILITY,
                      nv_drm_get_client_capability_ioctl,
                      0),

    DRM_IOCTL_DEF_DRV(NVIDIA_GET_CRTC_CRC32,
                      nv_drm_get_crtc_crc32_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GET_CRTC_CRC32_V2,
                      nv_drm_get_crtc_crc32_v2_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_EXPORT_NVKMS_MEMORY,
                      nv_drm_gem_export_nvkms_memory_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_ALLOC_NVKMS_MEMORY,
                      nv_drm_gem_alloc_nvkms_memory_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_EXPORT_DMABUF_MEMORY,
                      nv_drm_gem_export_dmabuf_memory_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_IDENTIFY_OBJECT,
                      nv_drm_gem_identify_object_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_DMABUF_SUPPORTED,
                      nv_drm_dmabuf_supported_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GET_DPY_ID_FOR_CONNECTOR_ID,
                      nv_drm_get_dpy_id_for_connector_id_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GET_CONNECTOR_ID_FOR_DPY_ID,
                      nv_drm_get_connector_id_for_dpy_id_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GRANT_PERMISSIONS,
                      nv_drm_grant_permission_ioctl,
                      DRM_UNLOCKED|DRM_MASTER),
    DRM_IOCTL_DEF_DRV(NVIDIA_REVOKE_PERMISSIONS,
                      nv_drm_revoke_permission_ioctl,
                      DRM_UNLOCKED|DRM_MASTER),
};

static struct drm_driver nv_drm_driver = {

    .driver_features        =
#if defined(NV_DRM_DRIVER_PRIME_FLAG_PRESENT)
                               DRIVER_PRIME |
#endif
#if defined(NV_DRM_SYNCOBJ_FEATURES_PRESENT)
                               DRIVER_SYNCOBJ | DRIVER_SYNCOBJ_TIMELINE |
#endif
                               DRIVER_GEM  | DRIVER_RENDER,

#if defined(NV_DRM_DRIVER_HAS_GEM_FREE_OBJECT)
    .gem_free_object        = nv_drm_gem_free,
#endif

    .ioctls                 = nv_drm_ioctls,
    .num_ioctls             = ARRAY_SIZE(nv_drm_ioctls),

/*
 * Linux kernel v6.6 commit 6b85aa68d9d5 ("drm: Enable PRIME import/export for
 * all drivers") made drm_gem_prime_handle_to_fd() /
 * drm_gem_prime_fd_to_handle() the default when .prime_handle_to_fd /
 * .prime_fd_to_handle are unspecified, respectively.
 *
 * Linux kernel v6.6 commit 71a7974ac701 ("drm/prime: Unexport helpers for
 * fd/handle conversion") unexports drm_gem_prime_handle_to_fd() and
 * drm_gem_prime_fd_to_handle(). However, because of the aforementioned commit,
 * it's fine to just skip specifying them in this case.
 *
 * Linux kernel v6.7 commit 0514f63cfff3 ("Revert "drm/prime: Unexport helpers
 * for fd/handle conversion"") exported the helpers again, but left the default
 * behavior intact. Nonetheless, it does not hurt to specify them.
 */
#if NV_IS_EXPORT_SYMBOL_PRESENT_drm_gem_prime_handle_to_fd
    .prime_handle_to_fd     = drm_gem_prime_handle_to_fd,
#endif
#if NV_IS_EXPORT_SYMBOL_PRESENT_drm_gem_prime_fd_to_handle
    .prime_fd_to_handle     = drm_gem_prime_fd_to_handle,
#endif

    .gem_prime_import       = nv_drm_gem_prime_import,
    .gem_prime_import_sg_table = nv_drm_gem_prime_import_sg_table,

/*
 * Linux kernel v5.0 commit 7698799f95 ("drm/prime: Add drm_gem_prime_mmap()")
 * added drm_gem_prime_mmap().
 *
 * Linux kernel v6.6 commit 0adec22702d4 ("drm: Remove struct
 * drm_driver.gem_prime_mmap") removed .gem_prime_mmap, but replaced it with a
 * direct call to drm_gem_prime_mmap().
 *
 * TODO: Support .gem_prime_mmap on Linux < v5.0 using internal implementation.
 */
#if defined(NV_DRM_GEM_PRIME_MMAP_PRESENT) && \
    defined(NV_DRM_DRIVER_HAS_GEM_PRIME_MMAP)
    .gem_prime_mmap         = drm_gem_prime_mmap,
#endif

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_CALLBACKS)
    .gem_prime_export       = drm_gem_prime_export,
    .gem_prime_get_sg_table = nv_drm_gem_prime_get_sg_table,
    .gem_prime_vmap         = nv_drm_gem_prime_vmap,
    .gem_prime_vunmap       = nv_drm_gem_prime_vunmap,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
    .gem_prime_mmap         = drm_gem_prime_mmap,
#endif
    .gem_vm_ops             = &nv_drm_gem_vma_ops,
#endif

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_RES_OBJ)
    .gem_prime_res_obj      = nv_drm_gem_prime_res_obj,
#endif

    .postclose              = nv_drm_postclose,
    .open                   = nv_drm_open,

    .fops                   = &nv_drm_fops,

    .name                   = "nvidia-drm",

    .desc                   = "NVIDIA DRM driver",

#if defined(NV_DRM_DRIVER_HAS_DATE)
    .date                   = "20160202",
#endif

#if defined(NV_DRM_DRIVER_HAS_LEGACY_DEV_LIST)
    .legacy_dev_list        = LIST_HEAD_INIT(nv_drm_driver.legacy_dev_list),
#endif
// XXX implement nvidia-drm's own .fbdev_probe callback that uses NVKMS kapi directly
#if defined(NV_DRM_FBDEV_AVAILABLE) && defined(DRM_FBDEV_TTM_DRIVER_OPS)
    DRM_FBDEV_TTM_DRIVER_OPS,
#endif
};


/*
 * Update the global nv_drm_driver for the intended features.
 *
 * It defaults to PRIME-only, but is upgraded to atomic modeset if the
 * kernel supports atomic modeset and the 'modeset' kernel module
 * parameter is true.
 */
void nv_drm_update_drm_driver_features(void)
{
    if (!nv_drm_modeset_module_param) {
        return;
    }

    nv_drm_driver.driver_features |= DRIVER_MODESET | DRIVER_ATOMIC;

    nv_drm_driver.master_set       = nv_drm_master_set;
    nv_drm_driver.master_drop      = nv_drm_master_drop;

    nv_drm_driver.dumb_create      = nv_drm_dumb_create;
    nv_drm_driver.dumb_map_offset  = nv_drm_dumb_map_offset;
#if defined(NV_DRM_DRIVER_HAS_DUMB_DESTROY)
    nv_drm_driver.dumb_destroy     = nv_drm_dumb_destroy;
#endif /* NV_DRM_DRIVER_HAS_DUMB_DESTROY */
}



/*
 * Helper function for allocate/register DRM device for given NVIDIA GPU ID.
 */
void nv_drm_register_drm_device(const struct NvKmsKapiGpuInfo *gpu_info)
{
    struct nv_drm_device *nv_dev = NULL;
    struct drm_device *dev = NULL;
    struct device *device = gpu_info->gpuInfo.os_device_ptr;
    bool bus_is_pci;

    DRM_DEBUG(
        "Registering device for NVIDIA GPU ID 0x08%x",
        gpu_info->gpuInfo.gpu_id);

    /* Allocate NVIDIA-DRM device */

    nv_dev = nv_drm_calloc(1, sizeof(*nv_dev));

    if (nv_dev == NULL) {
        NV_DRM_LOG_ERR(
            "Failed to allocate memory for NVIDIA-DRM device object");
        return;
    }

    nv_dev->gpu_info = gpu_info->gpuInfo;
    nv_dev->gpu_mig_device = gpu_info->migDevice;

    mutex_init(&nv_dev->lock);

    /* Allocate DRM device */

    dev = drm_dev_alloc(&nv_drm_driver, device);

    if (dev == NULL) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to allocate device");
        goto failed_drm_alloc;
    }

    dev->dev_private = nv_dev;
    nv_dev->dev = dev;

    bus_is_pci =
#if defined(NV_LINUX)
        device->bus == &pci_bus_type;
#elif defined(NV_BSD)
        devclass_find("pci");
#endif

#if defined(NV_DRM_DEVICE_HAS_PDEV)
    if (bus_is_pci) {
        dev->pdev = to_pci_dev(device);
    }
#endif

    /* Load DRM device before registering it */
    if (nv_drm_dev_load(dev) != 0) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to load device");
        goto failed_drm_load;
    }

    /* Register DRM device to DRM sub-system */

    if (drm_dev_register(dev, 0) != 0) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to register device");
        goto failed_drm_register;
    }

#if defined(NV_DRM_FBDEV_AVAILABLE)
    if (nv_drm_fbdev_module_param &&
        drm_core_check_feature(dev, DRIVER_MODESET)) {

        if (bus_is_pci) {
            struct pci_dev *pdev = to_pci_dev(device);

#if defined(NV_DRM_APERTURE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS_PRESENT)

#if defined(NV_DRM_APERTURE_REMOVE_CONFLICTING_PCI_FRAMEBUFFERS_HAS_DRIVER_ARG)
            drm_aperture_remove_conflicting_pci_framebuffers(pdev, &nv_drm_driver);
#else
            drm_aperture_remove_conflicting_pci_framebuffers(pdev, nv_drm_driver.name);
#endif

#elif defined(NV_APERTURE_REMOVE_CONFLICTING_PCI_DEVICES_PRESENT)
            aperture_remove_conflicting_pci_devices(pdev, nv_drm_driver.name);
#endif
            nvKms->framebufferConsoleDisabled(nv_dev->pDevice);
        } else {
            resource_size_t base = (resource_size_t) nv_dev->vtFbBaseAddress;
            resource_size_t size = (resource_size_t) nv_dev->vtFbSize;

            if (base > 0 && size > 0) {
#if defined(NV_DRM_APERTURE_REMOVE_CONFLICTING_FRAMEBUFFERS_PRESENT)

#if defined(NV_DRM_APERTURE_REMOVE_CONFLICTING_FRAMEBUFFERS_HAS_DRIVER_ARG)
                drm_aperture_remove_conflicting_framebuffers(base, size, false, &nv_drm_driver);
#elif defined(NV_DRM_APERTURE_REMOVE_CONFLICTING_FRAMEBUFFERS_HAS_NO_PRIMARY_ARG)
                drm_aperture_remove_conflicting_framebuffers(base, size, &nv_drm_driver);
#else
                drm_aperture_remove_conflicting_framebuffers(base, size, false, nv_drm_driver.name);
#endif

#elif defined(NV_APERTURE_REMOVE_CONFLICTING_DEVICES_PRESENT)
                aperture_remove_conflicting_devices(base, size, nv_drm_driver.name);
#endif
            } else {
                NV_DRM_DEV_LOG_INFO(nv_dev, "Invalid framebuffer console info");
            }
        }
        #if defined(NV_DRM_CLIENT_AVAILABLE)
        drm_client_setup(dev, NULL);
        #elif defined(NV_DRM_FBDEV_TTM_AVAILABLE)
        drm_fbdev_ttm_setup(dev, 32);
        #elif defined(NV_DRM_FBDEV_GENERIC_AVAILABLE)
        drm_fbdev_generic_setup(dev, 32);
        #endif
    }
#endif /* defined(NV_DRM_FBDEV_AVAILABLE) */

    /* Add NVIDIA-DRM device into list */

    nv_dev->next = dev_list;
    dev_list = nv_dev;

    return; /* Success */

failed_drm_register:

    nv_drm_dev_unload(dev);

failed_drm_load:

    drm_dev_put(dev);

failed_drm_alloc:

    nv_drm_free(nv_dev);
}

/*
 * Enumerate NVIDIA GPUs and allocate/register DRM device for each of them.
 */
#if defined(NV_LINUX)
int nv_drm_probe_devices(void)
{
    NvU32 gpu_count;

    nv_drm_update_drm_driver_features();

    /* Register DRM device for each NVIDIA GPU available via NVKMS. */
    gpu_count = nvKms->enumerateGpus(nv_drm_register_drm_device);

    if (gpu_count == 0) {
        NV_DRM_LOG_INFO("No NVIDIA GPUs found");
    }

    return 0;
}
#endif

/*
 * Unregister all NVIDIA DRM devices.
 */
void nv_drm_remove_devices(void)
{
    while (dev_list != NULL) {
        struct nv_drm_device *next = dev_list->next;
        struct drm_device *dev = dev_list->dev;

        drm_dev_unregister(dev);
        nv_drm_dev_unload(dev);
        drm_dev_put(dev);

        nv_drm_free(dev_list);

        dev_list = next;
    }
}

/*
 * Handle system suspend and resume.
 *
 * Normally, a DRM driver would use drm_mode_config_helper_suspend() to save the
 * current state on suspend and drm_mode_config_helper_resume() to restore it
 * after resume. This works for upstream drivers because user-mode tasks are
 * frozen before the suspend hook is called.
 *
 * In the case of nvidia-drm, the suspend hook is also called when 'suspend' is
 * written to /proc/driver/nvidia/suspend, before user-mode tasks are frozen.
 * However, we don't actually need to save and restore the display state because
 * the driver requires a VT switch to an unused VT before suspending and a
 * switch back to the application (or fbdev console) on resume. The DRM client
 * (or fbdev helper functions) will restore the appropriate mode on resume.
 *
 */
void nv_drm_suspend_resume(NvBool suspend)
{
    static DEFINE_MUTEX(nv_drm_suspend_mutex);
    static NvU32 nv_drm_suspend_count = 0;
    struct nv_drm_device *nv_dev;

    mutex_lock(&nv_drm_suspend_mutex);

    /*
     * Count the number of times the driver is asked to suspend. Suspend all DRM
     * devices on the first suspend call and resume them on the last resume
     * call.  This is necessary because the kernel may call nvkms_suspend()
     * simultaneously for each GPU, but NVKMS itself also suspends all GPUs on
     * the first call.
     */
    if (suspend) {
        if (nv_drm_suspend_count++ > 0) {
            goto done;
        }
    } else {
        BUG_ON(nv_drm_suspend_count == 0);

        if (--nv_drm_suspend_count > 0) {
            goto done;
        }
    }

    nv_dev = dev_list;

    /*
     * NVKMS shuts down all heads on suspend. Update DRM state accordingly.
     */
    for (nv_dev = dev_list; nv_dev; nv_dev = nv_dev->next) {
        struct drm_device *dev = nv_dev->dev;

        if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
            continue;
        }

        if (suspend) {
            drm_kms_helper_poll_disable(dev);
#if defined(NV_DRM_FBDEV_AVAILABLE)
            drm_fb_helper_set_suspend_unlocked(dev->fb_helper, 1);
#endif
            drm_mode_config_reset(dev);
        } else {
#if defined(NV_DRM_FBDEV_AVAILABLE)
            drm_fb_helper_set_suspend_unlocked(dev->fb_helper, 0);
#endif
            drm_kms_helper_poll_enable(dev);
        }
    }

done:
    mutex_unlock(&nv_drm_suspend_mutex);
}

#endif /* NV_DRM_AVAILABLE */
