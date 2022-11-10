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

#include "nvidia-drm-conftest.h" /* NV_DRM_AVAILABLE and NV_DRM_DRM_GEM_H_PRESENT */

#include "nvidia-drm-priv.h"
#include "nvidia-drm-drv.h"
#include "nvidia-drm-fb.h"
#include "nvidia-drm-modeset.h"
#include "nvidia-drm-encoder.h"
#include "nvidia-drm-connector.h"
#include "nvidia-drm-gem.h"
#include "nvidia-drm-crtc.h"
#include "nvidia-drm-prime-fence.h"
#include "nvidia-drm-helper.h"
#include "nvidia-drm-gem-nvkms-memory.h"
#include "nvidia-drm-gem-user-memory.h"
#include "nvidia-drm-gem-dma-buf.h"

#if defined(NV_DRM_AVAILABLE)

#include "nvidia-drm-ioctl.h"

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#if defined(NV_DRM_DRM_VBLANK_H_PRESENT)
#include <drm/drm_vblank.h>
#endif

#if defined(NV_DRM_DRM_FILE_H_PRESENT)
#include <drm/drm_file.h>
#endif

#if defined(NV_DRM_DRM_PRIME_H_PRESENT)
#include <drm/drm_prime.h>
#endif

#if defined(NV_DRM_DRM_IOCTL_H_PRESENT)
#include <drm/drm_ioctl.h>
#endif

#include <linux/pci.h>

/*
 * Commit fcd70cd36b9b ("drm: Split out drm_probe_helper.h")
 * moves a number of helper function definitions from
 * drm/drm_crtc_helper.h to a new drm_probe_helper.h.
 */
#if defined(NV_DRM_DRM_PROBE_HELPER_H_PRESENT)
#include <drm/drm_probe_helper.h>
#endif
#include <drm/drm_crtc_helper.h>

#if defined(NV_DRM_DRM_GEM_H_PRESENT)
#include <drm/drm_gem.h>
#endif

#if defined(NV_DRM_DRM_AUTH_H_PRESENT)
#include <drm/drm_auth.h>
#endif

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
#include <drm/drm_atomic_helper.h>
#endif

static struct nv_drm_device *dev_list = NULL;

static const char* nv_get_input_colorspace_name(
    enum NvKmsInputColorSpace colorSpace)
{
    switch (colorSpace) {
        case NVKMS_INPUT_COLORSPACE_NONE:
            return "None";
        case NVKMS_INPUT_COLORSPACE_SCRGB_LINEAR:
            return "IEC 61966-2-2 linear FP";
        case NVKMS_INPUT_COLORSPACE_BT2100_PQ:
            return "ITU-R BT.2100-PQ YCbCr";
        default:
            /* We shoudn't hit this */
            WARN_ON("Unsupported input colorspace");
            return "None";
    }
};

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

static void nv_drm_output_poll_changed(struct drm_device *dev)
{
    struct drm_connector *connector = NULL;
    struct drm_mode_config *config = &dev->mode_config;
#if defined(NV_DRM_CONNECTOR_LIST_ITER_PRESENT)
    struct drm_connector_list_iter conn_iter;
    nv_drm_connector_list_iter_begin(dev, &conn_iter);
#endif
    /*
     * Here drm_mode_config::mutex has been acquired unconditionally:
     *
     * - In the non-NV_DRM_CONNECTOR_LIST_ITER_PRESENT case, the mutex must
     *   be held for the duration of walking over the connectors.
     *
     * - In the NV_DRM_CONNECTOR_LIST_ITER_PRESENT case, the mutex must be
     *   held for the duration of a fill_modes() call chain:
     *     connector->funcs->fill_modes()
     *      |-> drm_helper_probe_single_connector_modes()
     *
     * It is easiest to always acquire the mutext for the entire connector
     * loop.
     */
    mutex_lock(&config->mutex);

    nv_drm_for_each_connector(connector, &conn_iter, dev) {

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
#if defined(NV_DRM_CONNECTOR_LIST_ITER_PRESENT)
    nv_drm_connector_list_iter_end(&conn_iter);
#endif
}

static struct drm_framebuffer *nv_drm_framebuffer_create(
    struct drm_device *dev,
    struct drm_file *file,
    #if defined(NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_CONST_MODE_CMD_ARG)
    const struct drm_mode_fb_cmd2 *cmd
    #else
    struct drm_mode_fb_cmd2 *cmd
    #endif
)
{
    struct drm_mode_fb_cmd2 local_cmd;
    struct drm_framebuffer *fb;

    local_cmd = *cmd;

    fb = nv_drm_internal_framebuffer_create(
            dev,
            file,
            &local_cmd);

    #if !defined(NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_CONST_MODE_CMD_ARG)
    *cmd = local_cmd;
    #endif

    return fb;
}

static const struct drm_mode_config_funcs nv_mode_config_funcs = {
    .fb_create = nv_drm_framebuffer_create,

    .atomic_state_alloc = nv_drm_atomic_state_alloc,
    .atomic_state_clear = nv_drm_atomic_state_clear,
    .atomic_state_free  = nv_drm_atomic_state_free,
    .atomic_check  = nv_drm_atomic_check,
    .atomic_commit = nv_drm_atomic_commit,

    .output_poll_changed = nv_drm_output_poll_changed,
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

    /* Currently unused. Update when needed. */

    dev->mode_config.fb_base = 0;

#if defined(NV_DRM_CRTC_STATE_HAS_ASYNC_FLIP) || \
    defined(NV_DRM_CRTC_STATE_HAS_PAGEFLIP_FLAGS)
    dev->mode_config.async_page_flip = true;
#else
    dev->mode_config.async_page_flip = false;
#endif

#if defined(NV_DRM_FORMAT_MODIFIERS_PRESENT) && \
    defined(NV_DRM_MODE_CONFIG_HAS_ALLOW_FB_MODIFIERS)
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
                NvU32 i;

                for (i = 0; i < nDisplays; i++) {
                    struct drm_encoder *encoder =
                        nv_drm_add_encoder(dev, hDisplays[i]);

                    if (IS_ERR(encoder)) {
                        NV_DRM_DEV_LOG_ERR(
                            nv_dev,
                            "Failed to add connector for NvKmsKapiDisplay 0x%08x",
                            hDisplays[i]);
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

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

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
    struct drm_prop_enum_list enum_list[3] = { };
    int i, len = 0;

    for (i = 0; i < 3; i++) {
        enum_list[len].type = i;
        enum_list[len].name = nv_get_input_colorspace_name(i);
        len++;
    }

#if defined(NV_LINUX_NVHOST_H_PRESENT) && defined(CONFIG_TEGRA_GRHOST)
    if (!nv_dev->supportsSyncpts) {
        return 0;
    }

    nv_dev->nv_out_fence_property =
        drm_property_create_range(nv_dev->dev, DRM_MODE_PROP_ATOMIC,
            "NV_DRM_OUT_FENCE_PTR", 0, U64_MAX);
    if (nv_dev->nv_out_fence_property == NULL) {
        return -ENOMEM;
    }
#endif

    nv_dev->nv_input_colorspace_property =
        drm_property_create_enum(nv_dev->dev, 0, "NV_INPUT_COLORSPACE",
                                 enum_list, len);
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

    return 0;
}

static int nv_drm_load(struct drm_device *dev, unsigned long flags)
{
#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    struct NvKmsKapiDevice *pDevice;

    struct NvKmsKapiAllocateDeviceParams allocateDeviceParams;
    struct NvKmsKapiDeviceResourcesInfo resInfo;
#endif
#if defined(NV_DRM_FORMAT_MODIFIERS_PRESENT)
    NvU64 kind;
    NvU64 gen;
    int i;
#endif
    int ret;

    struct nv_drm_device *nv_dev = to_nv_device(dev);

    NV_DRM_DEV_LOG_INFO(nv_dev, "Loading driver");

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return 0;
    }

    /* Allocate NvKmsKapiDevice from GPU ID */

    memset(&allocateDeviceParams, 0, sizeof(allocateDeviceParams));

    allocateDeviceParams.gpuId = nv_dev->gpu_info.gpu_id;

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

#if defined(NV_DRM_FORMAT_MODIFIERS_PRESENT)
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
#endif /* defined(NV_DRM_FORMAT_MODIFIERS_PRESENT) */

    /* Initialize drm_device::mode_config */

    nv_drm_init_mode_config(nv_dev, &resInfo);

    ret = nv_drm_create_properties(nv_dev);
    if (ret < 0) {
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

    atomic_set(&nv_dev->enable_event_handling, true);

    init_waitqueue_head(&nv_dev->flip_event_wq);

    mutex_unlock(&nv_dev->lock);

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

    return 0;
}

static void __nv_drm_unload(struct drm_device *dev)
{
#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    struct NvKmsKapiDevice *pDevice = NULL;
#endif

    struct nv_drm_device *nv_dev = to_nv_device(dev);

    NV_DRM_DEV_LOG_INFO(nv_dev, "Unloading driver");

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return;
    }

    mutex_lock(&nv_dev->lock);

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

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */
}

#if defined(NV_DRM_DRIVER_UNLOAD_HAS_INT_RETURN_TYPE)
static int nv_drm_unload(struct drm_device *dev)
{
    __nv_drm_unload(dev);

    return 0;
}
#else
static void nv_drm_unload(struct drm_device *dev)
{
    __nv_drm_unload(dev);
}
#endif

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

static int __nv_drm_master_set(struct drm_device *dev,
                               struct drm_file *file_priv, bool from_open)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    if (!nvKms->grabOwnership(nv_dev->pDevice)) {
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


#if defined(NV_DRM_MASTER_DROP_HAS_FROM_RELEASE_ARG)
static
void nv_drm_master_drop(struct drm_device *dev,
                        struct drm_file *file_priv, bool from_release)
#else
static
void nv_drm_master_drop(struct drm_device *dev, struct drm_file *file_priv)
#endif
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    int err;

    /*
     * After dropping nvkms modeset onwership, it is not guaranteed that
     * drm and nvkms modeset state will remain in sync.  Therefore, disable
     * all outputs and crtcs before dropping nvkms modeset ownership.
     *
     * First disable all active outputs atomically and then disable each crtc one
     * by one, there is not helper function available to disable all crtcs
     * atomically.
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
}
#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#if defined(NV_DRM_BUS_PRESENT) || defined(NV_DRM_DRIVER_HAS_SET_BUSID)
static int nv_drm_pci_set_busid(struct drm_device *dev,
                                struct drm_master *master)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);

    master->unique = nv_drm_asprintf("pci:%04x:%02x:%02x.%d",
                                          nv_dev->gpu_info.pci_info.domain,
                                          nv_dev->gpu_info.pci_info.bus,
                                          nv_dev->gpu_info.pci_info.slot,
                                          nv_dev->gpu_info.pci_info.function);

    if (master->unique == NULL) {
        return -ENOMEM;
    }

    master->unique_len = strlen(master->unique);

    return 0;
}
#endif

static int nv_drm_get_dev_info_ioctl(struct drm_device *dev,
                                     void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_get_dev_info_params *params = data;

    if (dev->primary == NULL) {
        return -ENOENT;
    }

    params->gpu_id = nv_dev->gpu_info.gpu_id;
    params->primary_index = dev->primary->index;
#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    params->generic_page_kind = nv_dev->genericPageKind;
    params->page_kind_generation = nv_dev->pageKindGeneration;
    params->sector_layout = nv_dev->sectorLayout;
#else
    params->generic_page_kind = 0;
    params->page_kind_generation = 0;
    params->sector_layout = 0;
#endif

    return 0;
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

#if defined(NV_DRM_BUS_PRESENT)

#if defined(NV_DRM_BUS_HAS_GET_IRQ)
static int nv_drm_bus_get_irq(struct drm_device *dev)
{
    return 0;
}
#endif

#if defined(NV_DRM_BUS_HAS_GET_NAME)
static const char *nv_drm_bus_get_name(struct drm_device *dev)
{
    return "nvidia-drm";
}
#endif

static struct drm_bus nv_drm_bus = {
#if defined(NV_DRM_BUS_HAS_BUS_TYPE)
    .bus_type     = DRIVER_BUS_PCI,
#endif
#if defined(NV_DRM_BUS_HAS_GET_IRQ)
    .get_irq      = nv_drm_bus_get_irq,
#endif
#if defined(NV_DRM_BUS_HAS_GET_NAME)
    .get_name     = nv_drm_bus_get_name,
#endif
    .set_busid    = nv_drm_pci_set_busid,
};

#endif /* NV_DRM_BUS_PRESENT */

static const struct file_operations nv_drm_fops = {
    .owner          = THIS_MODULE,

    .open           = drm_open,
    .release        = drm_release,
    .unlocked_ioctl = drm_ioctl,
#if defined(CONFIG_COMPAT)
    .compat_ioctl   = drm_compat_ioctl,
#endif

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    .mmap           = nv_drm_mmap,
#endif

    .poll           = drm_poll,
    .read           = drm_read,

    .llseek         = noop_llseek,
};

static const struct drm_ioctl_desc nv_drm_ioctls[] = {
#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_IMPORT_NVKMS_MEMORY,
                      nv_drm_gem_import_nvkms_memory_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_IMPORT_USERSPACE_MEMORY,
                      nv_drm_gem_import_userspace_memory_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_MAP_OFFSET,
                      nv_drm_gem_map_offset_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GET_DEV_INFO,
                      nv_drm_get_dev_info_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),

#if defined(NV_DRM_FENCE_AVAILABLE)
    DRM_IOCTL_DEF_DRV(NVIDIA_FENCE_SUPPORTED,
                      nv_drm_fence_supported_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_FENCE_CONTEXT_CREATE,
                      nv_drm_fence_context_create_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
    DRM_IOCTL_DEF_DRV(NVIDIA_GEM_FENCE_ATTACH,
                      nv_drm_gem_fence_attach_ioctl,
                      DRM_RENDER_ALLOW|DRM_UNLOCKED),
#endif

    DRM_IOCTL_DEF_DRV(NVIDIA_GET_CLIENT_CAPABILITY,
                      nv_drm_get_client_capability_ioctl,
                      0),
#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
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
#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */
};

static struct drm_driver nv_drm_driver = {

    .driver_features        =
#if defined(NV_DRM_DRIVER_PRIME_FLAG_PRESENT)
                               DRIVER_PRIME |
#endif
                               DRIVER_GEM  | DRIVER_RENDER,

#if defined(NV_DRM_DRIVER_HAS_GEM_FREE_OBJECT)
    .gem_free_object        = nv_drm_gem_free,
#endif

    .ioctls                 = nv_drm_ioctls,
    .num_ioctls             = ARRAY_SIZE(nv_drm_ioctls),

    .prime_handle_to_fd     = drm_gem_prime_handle_to_fd,
    .prime_fd_to_handle     = drm_gem_prime_fd_to_handle,
    .gem_prime_import       = nv_drm_gem_prime_import,
    .gem_prime_import_sg_table = nv_drm_gem_prime_import_sg_table,

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_CALLBACKS)
    .gem_prime_export       = drm_gem_prime_export,
    .gem_prime_get_sg_table = nv_drm_gem_prime_get_sg_table,
    .gem_prime_vmap         = nv_drm_gem_prime_vmap,
    .gem_prime_vunmap       = nv_drm_gem_prime_vunmap,

    .gem_vm_ops             = &nv_drm_gem_vma_ops,
#endif

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_RES_OBJ)
    .gem_prime_res_obj      = nv_drm_gem_prime_res_obj,
#endif

#if defined(NV_DRM_DRIVER_HAS_SET_BUSID)
    .set_busid              = nv_drm_pci_set_busid,
#endif

    .load                   = nv_drm_load,
    .unload                 = nv_drm_unload,

    .fops                   = &nv_drm_fops,

#if defined(NV_DRM_BUS_PRESENT)
    .bus                    = &nv_drm_bus,
#endif

    .name                   = "nvidia-drm",

    .desc                   = "NVIDIA DRM driver",
    .date                   = "20160202",

#if defined(NV_DRM_DRIVER_HAS_DEVICE_LIST)
    .device_list            = LIST_HEAD_INIT(nv_drm_driver.device_list),
#elif defined(NV_DRM_DRIVER_HAS_LEGACY_DEV_LIST)
    .legacy_dev_list        = LIST_HEAD_INIT(nv_drm_driver.legacy_dev_list),
#endif
};


/*
 * Update the global nv_drm_driver for the intended features.
 *
 * It defaults to PRIME-only, but is upgraded to atomic modeset if the
 * kernel supports atomic modeset and the 'modeset' kernel module
 * parameter is true.
 */
static void nv_drm_update_drm_driver_features(void)
{
#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

    if (!nv_drm_modeset_module_param) {
        return;
    }

    nv_drm_driver.driver_features |= DRIVER_MODESET | DRIVER_ATOMIC;

    nv_drm_driver.master_set       = nv_drm_master_set;
    nv_drm_driver.master_drop      = nv_drm_master_drop;

    nv_drm_driver.dumb_create      = nv_drm_dumb_create;
    nv_drm_driver.dumb_map_offset  = nv_drm_dumb_map_offset;
    nv_drm_driver.dumb_destroy     = nv_drm_dumb_destroy;
#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */
}



/*
 * Helper function for allocate/register DRM device for given NVIDIA GPU ID.
 */
static void nv_drm_register_drm_device(const nv_gpu_info_t *gpu_info)
{
    struct nv_drm_device *nv_dev = NULL;
    struct drm_device *dev = NULL;
    struct device *device = gpu_info->os_device_ptr;

    DRM_DEBUG(
        "Registering device for NVIDIA GPU ID 0x08%x",
        gpu_info->gpu_id);

    /* Allocate NVIDIA-DRM device */

    nv_dev = nv_drm_calloc(1, sizeof(*nv_dev));

    if (nv_dev == NULL) {
        NV_DRM_LOG_ERR(
            "Failed to allocate memory for NVIDIA-DRM device object");
        return;
    }

    nv_dev->gpu_info = *gpu_info;

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    mutex_init(&nv_dev->lock);
#endif

    /* Allocate DRM device */

    dev = drm_dev_alloc(&nv_drm_driver, device);

    if (dev == NULL) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to allocate device");
        goto failed_drm_alloc;
    }

    dev->dev_private = nv_dev;
    nv_dev->dev = dev;

#if defined(NV_DRM_DEVICE_HAS_PDEV)
    if (device->bus == &pci_bus_type) {
        dev->pdev = to_pci_dev(device);
    }
#endif

    /* Register DRM device to DRM sub-system */

    if (drm_dev_register(dev, 0) != 0) {
        NV_DRM_DEV_LOG_ERR(nv_dev, "Failed to register device");
        goto failed_drm_register;
    }

    /* Add NVIDIA-DRM device into list */

    nv_dev->next = dev_list;
    dev_list = nv_dev;

    return; /* Success */

failed_drm_register:

    nv_drm_dev_free(dev);

failed_drm_alloc:

    nv_drm_free(nv_dev);
}

/*
 * Enumerate NVIDIA GPUs and allocate/register DRM device for each of them.
 */
int nv_drm_probe_devices(void)
{
    nv_gpu_info_t *gpu_info = NULL;
    NvU32 gpu_count = 0;
    NvU32 i;

    int ret = 0;

    nv_drm_update_drm_driver_features();

    /* Enumerate NVIDIA GPUs */

    gpu_info = nv_drm_calloc(NV_MAX_GPUS, sizeof(*gpu_info));

    if (gpu_info == NULL) {
        ret = -ENOMEM;

        NV_DRM_LOG_ERR("Failed to allocate gpu ids arrays");
        goto done;
    }

    gpu_count = nvKms->enumerateGpus(gpu_info);

    if (gpu_count == 0) {
        NV_DRM_LOG_INFO("Not found NVIDIA GPUs");
        goto done;
    }

    WARN_ON(gpu_count > NV_MAX_GPUS);

    /* Register DRM device for each NVIDIA GPU */

    for (i = 0; i < gpu_count; i++) {
        nv_drm_register_drm_device(&gpu_info[i]);
    }

done:

    nv_drm_free(gpu_info);

    return ret;
}

/*
 * Unregister all NVIDIA DRM devices.
 */
void nv_drm_remove_devices(void)
{
    while (dev_list != NULL) {
        struct nv_drm_device *next = dev_list->next;

        drm_dev_unregister(dev_list->dev);
        nv_drm_dev_free(dev_list->dev);

        nv_drm_free(dev_list);

        dev_list = next;
    }
}

#endif /* NV_DRM_AVAILABLE */
