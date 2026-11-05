/*
 * Copyright (c) 2015-2026, NVIDIA CORPORATION.  All rights reserved.
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
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _NV_DRM_COMMON_IOCTL_H_
#define _NV_DRM_COMMON_IOCTL_H_

#include <drm/drm.h>

/*
 * We should do our best to keep these values constant. Any change to these will
 * be backwards incompatible with client applications that might be using them
 */
#define DRM_NVIDIA_GET_CRTC_CRC32                   0x00
#define DRM_NVIDIA_GEM_IMPORT_NVKMS_MEMORY          0x01
#define DRM_NVIDIA_GEM_IMPORT_USERSPACE_MEMORY      0x02
#define DRM_NVIDIA_GET_DEV_INFO                     0x03
#define DRM_NVIDIA_FENCE_SUPPORTED                  0x04
#define DRM_NVIDIA_PRIME_FENCE_CONTEXT_CREATE       0x05
#define DRM_NVIDIA_GEM_PRIME_FENCE_ATTACH           0x06
#define DRM_NVIDIA_GET_CLIENT_CAPABILITY            0x08
#define DRM_NVIDIA_GEM_EXPORT_NVKMS_MEMORY          0x09
#define DRM_NVIDIA_GEM_MAP_OFFSET                   0x0a
#define DRM_NVIDIA_GEM_ALLOC_NVKMS_MEMORY           0x0b
#define DRM_NVIDIA_GET_CRTC_CRC32_V2                0x0c
#define DRM_NVIDIA_GEM_EXPORT_DMABUF_MEMORY         0x0d
#define DRM_NVIDIA_GEM_IDENTIFY_OBJECT              0x0e
#define DRM_NVIDIA_DMABUF_SUPPORTED                 0x0f
#define DRM_NVIDIA_GET_DPY_ID_FOR_CONNECTOR_ID      0x10
#define DRM_NVIDIA_GET_CONNECTOR_ID_FOR_DPY_ID      0x11
#define DRM_NVIDIA_GRANT_PERMISSIONS                0x12
#define DRM_NVIDIA_REVOKE_PERMISSIONS               0x13
#define DRM_NVIDIA_SEMSURF_FENCE_CTX_CREATE         0x14
#define DRM_NVIDIA_SEMSURF_FENCE_CREATE             0x15
#define DRM_NVIDIA_SEMSURF_FENCE_WAIT               0x16
#define DRM_NVIDIA_SEMSURF_FENCE_ATTACH             0x17
#define DRM_NVIDIA_GET_DRM_FILE_UNIQUE_ID           0x18
#define DRM_NVIDIA_REGISTER_ROI                     0x19  /* Register ROI  */
#define DRM_NVIDIA_UNREGISTER_ROI                   0x1a  /* Unregister ROI */
#define DRM_NVIDIA_GET_CRTC_ROI_CRCS                0x1b  /* Read CRCs for registered ROIs */
#define DRM_NVIDIA_GET_ROI_CAPABILITIES             0x1c  /* Get ROI related capabilities */
#define DRM_NVIDIA_SEMSURF_EXPORT_TO_SYNCOBJ_POINT  0x1d
#define DRM_NVIDIA_SYNCOBJ_GET_SYNCFD               0x1e


/* Maximum possible telltale CRCs per plane (used for array size)
 * actual value is queried from the plane property  NV_DRM_NUM_PLANE_TELLTALE_CRCS*/
#define NV_DRM_MAX_TELLTALES_PER_PLANE              64

/* Maximum possible ROIs per CRTC (used for array size)
 * actual value is queried from DRM_NVIDIA_GET_ROI_CAPABILITIES */
#define NV_DRM_MAX_ROIS_PER_CRTC                    64

/* IOCTLs */
#define DRM_IOCTL_NVIDIA_GEM_IMPORT_NVKMS_MEMORY                           \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_IMPORT_NVKMS_MEMORY),      \
             struct drm_nvidia_gem_import_nvkms_memory_params)

#define DRM_IOCTL_NVIDIA_GEM_IMPORT_USERSPACE_MEMORY                       \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_IMPORT_USERSPACE_MEMORY),  \
             struct drm_nvidia_gem_import_userspace_memory_params)

#define DRM_IOCTL_NVIDIA_GET_DEV_INFO                                      \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_DEV_INFO),                 \
             struct drm_nvidia_get_dev_info_params)

/*
 * XXX Solaris compiler has issues with DRM_IO. None of this is supported on
 * Solaris anyway, so just skip it.
 *
 * 'warning: suggest parentheses around arithmetic in operand of |'
 */
#if defined(NV_LINUX) || defined(NV_BSD)
#define DRM_IOCTL_NVIDIA_FENCE_SUPPORTED                         \
    DRM_IO(DRM_COMMAND_BASE + DRM_NVIDIA_FENCE_SUPPORTED)
#define DRM_IOCTL_NVIDIA_DMABUF_SUPPORTED                        \
    DRM_IO(DRM_COMMAND_BASE + DRM_NVIDIA_DMABUF_SUPPORTED)
#else
#define DRM_IOCTL_NVIDIA_FENCE_SUPPORTED 0
#define DRM_IOCTL_NVIDIA_DMABUF_SUPPORTED 0
#endif

#define DRM_IOCTL_NVIDIA_PRIME_FENCE_CONTEXT_CREATE                     \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_PRIME_FENCE_CONTEXT_CREATE),\
             struct drm_nvidia_prime_fence_context_create_params)

#define DRM_IOCTL_NVIDIA_GEM_PRIME_FENCE_ATTACH                         \
    DRM_IOW((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_PRIME_FENCE_ATTACH),     \
            struct drm_nvidia_gem_prime_fence_attach_params)

#define DRM_IOCTL_NVIDIA_GET_CLIENT_CAPABILITY                          \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_CLIENT_CAPABILITY),     \
             struct drm_nvidia_get_client_capability_params)

#define DRM_IOCTL_NVIDIA_GET_CRTC_CRC32                                 \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_CRTC_CRC32),            \
             struct drm_nvidia_get_crtc_crc32_params)

#define DRM_IOCTL_NVIDIA_GET_CRTC_CRC32_V2                              \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_CRTC_CRC32_V2),         \
              struct drm_nvidia_get_crtc_crc32_v2_params)

#define DRM_IOCTL_NVIDIA_GEM_EXPORT_NVKMS_MEMORY                        \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_EXPORT_NVKMS_MEMORY),   \
              struct drm_nvidia_gem_export_nvkms_memory_params)

#define DRM_IOCTL_NVIDIA_GEM_MAP_OFFSET                                 \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_MAP_OFFSET),            \
             struct drm_nvidia_gem_map_offset_params)

#define DRM_IOCTL_NVIDIA_GEM_ALLOC_NVKMS_MEMORY                         \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_ALLOC_NVKMS_MEMORY),    \
              struct drm_nvidia_gem_alloc_nvkms_memory_params)

#define DRM_IOCTL_NVIDIA_GEM_EXPORT_DMABUF_MEMORY                       \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_EXPORT_DMABUF_MEMORY),  \
              struct drm_nvidia_gem_export_dmabuf_memory_params)

#define DRM_IOCTL_NVIDIA_GEM_IDENTIFY_OBJECT                            \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GEM_IDENTIFY_OBJECT),       \
              struct drm_nvidia_gem_identify_object_params)

#define DRM_IOCTL_NVIDIA_GET_DPY_ID_FOR_CONNECTOR_ID                     \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_DPY_ID_FOR_CONNECTOR_ID),\
             struct drm_nvidia_get_dpy_id_for_connector_id_params)

#define DRM_IOCTL_NVIDIA_GET_CONNECTOR_ID_FOR_DPY_ID                     \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_CONNECTOR_ID_FOR_DPY_ID),\
             struct drm_nvidia_get_connector_id_for_dpy_id_params)

#define DRM_IOCTL_NVIDIA_GRANT_PERMISSIONS                              \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GRANT_PERMISSIONS),         \
             struct drm_nvidia_grant_permissions_params)

#define DRM_IOCTL_NVIDIA_REVOKE_PERMISSIONS                             \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_REVOKE_PERMISSIONS),        \
             struct drm_nvidia_revoke_permissions_params)

#define DRM_IOCTL_NVIDIA_SEMSURF_FENCE_CTX_CREATE                       \
    DRM_IOWR((DRM_COMMAND_BASE +                                        \
              DRM_NVIDIA_SEMSURF_FENCE_CTX_CREATE),                     \
              struct drm_nvidia_semsurf_fence_ctx_create_params)

#define DRM_IOCTL_NVIDIA_SEMSURF_FENCE_CREATE                           \
    DRM_IOWR((DRM_COMMAND_BASE +                                        \
              DRM_NVIDIA_SEMSURF_FENCE_CREATE),                         \
              struct drm_nvidia_semsurf_fence_create_params)

#define DRM_IOCTL_NVIDIA_SEMSURF_FENCE_WAIT                             \
    DRM_IOW((DRM_COMMAND_BASE +                                         \
              DRM_NVIDIA_SEMSURF_FENCE_WAIT),                           \
              struct drm_nvidia_semsurf_fence_wait_params)

#define DRM_IOCTL_NVIDIA_SEMSURF_FENCE_ATTACH                           \
    DRM_IOW((DRM_COMMAND_BASE +                                         \
              DRM_NVIDIA_SEMSURF_FENCE_ATTACH),                         \
              struct drm_nvidia_semsurf_fence_attach_params)

#define DRM_IOCTL_NVIDIA_GET_DRM_FILE_UNIQUE_ID                         \
    DRM_IOWR((DRM_COMMAND_BASE +                                        \
              DRM_NVIDIA_GET_DRM_FILE_UNIQUE_ID),                       \
              struct drm_nvidia_get_drm_file_unique_id_params)

#define DRM_IOCTL_NVIDIA_SEMSURF_EXPORT_TO_SYNCOBJ_POINT                \
    DRM_IOW((DRM_COMMAND_BASE +                                         \
             DRM_NVIDIA_SEMSURF_EXPORT_TO_SYNCOBJ_POINT),               \
             struct drm_nvidia_semsurf_export_to_syncobj_point_params)

#define DRM_IOCTL_NVIDIA_SYNCOBJ_GET_SYNCFD                             \
    DRM_IOWR((DRM_COMMAND_BASE +                                        \
              DRM_NVIDIA_SYNCOBJ_GET_SYNCFD),                           \
              struct drm_nvidia_syncobj_get_syncfd_params)

#define DRM_IOCTL_NVIDIA_REGISTER_ROI                                    \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_REGISTER_ROI),               \
             struct drm_nvidia_register_roi_params)

#define DRM_IOCTL_NVIDIA_UNREGISTER_ROI                                 \
    DRM_IOW((DRM_COMMAND_BASE + DRM_NVIDIA_UNREGISTER_ROI),             \
            struct drm_nvidia_unregister_roi_params)

#define DRM_IOCTL_NVIDIA_GET_CRTC_ROI_CRCS                              \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_CRTC_ROI_CRCS),         \
             struct drm_nvidia_read_crc_params)

#define DRM_IOCTL_NVIDIA_GET_ROI_CAPABILITIES                         \
    DRM_IOWR((DRM_COMMAND_BASE + DRM_NVIDIA_GET_ROI_CAPABILITIES),    \
             struct drm_nvidia_get_roi_capabilities_params)

#define NV_DRM_HDMI_VSIF_METADATA_MIN_PAYLOAD_SIZE                     3
#define NV_DRM_HDMI_VSIF_METADATA_MAX_PAYLOAD_SIZE                     27

struct drm_nvidia_gem_import_nvkms_memory_params {
    uint64_t mem_size;           /* IN */

    uint64_t nvkms_params_ptr;   /* IN */
    uint64_t nvkms_params_size;  /* IN */

    uint32_t handle;             /* OUT */

    uint32_t __pad;
};

struct drm_nvidia_gem_import_userspace_memory_params {
    uint64_t size;               /* IN Size of memory in bytes */
    uint64_t address;            /* IN Virtual address of userspace memory */
    uint32_t handle;             /* OUT Handle to gem object */
};

struct drm_nvidia_get_dev_info_params {
    uint32_t gpu_id;             /* OUT */
    uint32_t mig_device;         /* OUT */
    uint32_t primary_index;      /* OUT; the "card%d" value */

    uint32_t supports_alloc;     /* OUT */
    /* The generic_page_kind, page_kind_generation, and sector_layout
     * fields are only valid if supports_alloc is true.
     * See DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D definitions of these. */
    uint32_t generic_page_kind;    /* OUT */
    uint32_t page_kind_generation; /* OUT */
    uint32_t sector_layout;        /* OUT */
    uint32_t supports_sync_fd;     /* OUT */
    uint32_t supports_semsurf;     /* OUT */
};

struct drm_nvidia_prime_fence_context_create_params {
    uint32_t handle;            /* OUT GEM handle to fence context */

    uint32_t index;             /* IN Index of semaphore to use for fencing */
    uint64_t size;              /* IN Size of semaphore surface in bytes */

    /* Params for importing userspace semaphore surface */
    uint64_t import_mem_nvkms_params_ptr;  /* IN */
    uint64_t import_mem_nvkms_params_size; /* IN */

    /* Params for creating software signaling event */
    uint64_t event_nvkms_params_ptr;  /* IN */
    uint64_t event_nvkms_params_size; /* IN */
};

struct drm_nvidia_gem_prime_fence_attach_params {
    uint32_t handle;                /* IN GEM handle to attach fence to */
    uint32_t fence_context_handle;  /* IN GEM handle to fence context on which fence is run on */
    uint32_t sem_thresh;            /* IN Semaphore value to reach before signal */
    uint32_t __pad;
};

struct drm_nvidia_get_client_capability_params {
    uint64_t capability;    /* IN Client capability enum */
    uint64_t value;         /* OUT Client capability value */
};

/* Struct that stores Crc value and if it is supported by hardware */
struct drm_nvidia_crtc_crc32 {
    uint32_t value; /* Read value, undefined if supported is false */
    uint8_t supported; /* Supported boolean, true if readable by hardware */
    uint8_t __pad0;
    uint16_t __pad1;
};

struct drm_nvidia_crtc_crc32_v2_out {
    struct drm_nvidia_crtc_crc32  compositorCrc32;        /* OUT compositor hardware CRC32 value */
    struct drm_nvidia_crtc_crc32  rasterGeneratorCrc32;   /* OUT raster generator CRC32 value */
    struct drm_nvidia_crtc_crc32  outputCrc32;            /* OUT SF/SOR CRC32 value */
};

struct drm_nvidia_get_crtc_crc32_v2_params {
    uint32_t crtc_id;                             /* IN CRTC identifier */
    struct drm_nvidia_crtc_crc32_v2_out crc32;    /* OUT Crc32 output structure */
};

struct drm_nvidia_get_crtc_crc32_params {
    uint32_t crtc_id;      /* IN CRTC identifier */
    uint32_t crc32;        /* OUT CRC32 value */
};

struct drm_nvidia_gem_export_nvkms_memory_params {
    uint32_t handle;             /* IN */
    uint32_t __pad;

    uint64_t nvkms_params_ptr;   /* IN */
    uint64_t nvkms_params_size;  /* IN */
};

struct drm_nvidia_gem_map_offset_params {
    uint32_t handle;             /* IN Handle to gem object */
    uint32_t __pad;

    uint64_t offset;             /* OUT Fake offset */
};

#define NV_GEM_ALLOC_NO_SCANOUT                     (1 << 0)

struct drm_nvidia_gem_alloc_nvkms_memory_params {
    uint32_t handle;              /* OUT */
    uint8_t  block_linear;        /* IN */
    uint8_t  compressible;        /* IN/OUT */
    uint16_t __pad0;

    uint64_t memory_size;         /* IN */
    uint32_t flags;               /* IN */
    uint32_t __pad1;
};

struct drm_nvidia_gem_export_dmabuf_memory_params {
    uint32_t handle;   /* IN GEM Handle*/
    uint32_t __pad;

    uint64_t nvkms_params_ptr;   /* IN */
    uint64_t nvkms_params_size;  /* IN */
};

typedef enum {
    NV_GEM_OBJECT_NVKMS,
    NV_GEM_OBJECT_DMABUF,
    NV_GEM_OBJECT_USERMEMORY,

    NV_GEM_OBJECT_UNKNOWN = 0x7fffffff  /* Force size of 32-bits. */
} drm_nvidia_gem_object_type;

struct drm_nvidia_gem_identify_object_params {
    uint32_t                    handle;         /* IN GEM handle*/
    drm_nvidia_gem_object_type  object_type;    /* OUT GEM object type */
};

struct drm_nvidia_get_dpy_id_for_connector_id_params {
    uint32_t connectorId; /* IN */
    uint32_t dpyId;       /* OUT */
};

struct drm_nvidia_get_connector_id_for_dpy_id_params {
    uint32_t dpyId;       /* IN */
    uint32_t connectorId; /* OUT */
};

enum drm_nvidia_permissions_type {
    NV_DRM_PERMISSIONS_TYPE_MODESET = 2,
    NV_DRM_PERMISSIONS_TYPE_SUB_OWNER = 3
};

struct drm_nvidia_grant_permissions_params {
    int32_t fd;           /* IN */
    uint32_t dpyId;       /* IN */
    uint32_t type;        /* IN */
};

struct drm_nvidia_revoke_permissions_params {
    uint32_t dpyId;       /* IN */
    uint32_t type;        /* IN */
};

struct drm_nvidia_semsurf_fence_ctx_create_params {
    uint64_t index;             /* IN Index of the desired semaphore in the
                                 * fence context's semaphore surface */

    /* Params for importing userspace semaphore surface */
    uint64_t nvkms_params_ptr;  /* IN */
    uint64_t nvkms_params_size; /* IN */

    uint32_t handle;            /* OUT GEM handle to fence context */
    uint32_t __pad;
};

struct drm_nvidia_semsurf_fence_create_params {
    uint32_t fence_context_handle;  /* IN GEM handle to fence context on which
                                     * fence is run on */

    uint32_t timeout_value_ms;      /* IN Timeout value in ms for the fence
                                     * after which the fence will be signaled
                                     * with its error status set to -ETIMEDOUT.
                                     * Default timeout value is 5000ms */

    uint64_t wait_value;            /* IN Semaphore value to reach before signal */

    int32_t  fd;                    /* OUT sync FD object representing the
                                     * semaphore at the specified index reaching
                                     * a value >= wait_value */
    uint32_t __pad;
};

/*
 * Note there is no provision for timeouts in this ioctl. The kernel
 * documentation asserts timeouts should be handled by fence producers, and
 * that waiters should not second-guess their logic, as it is producers rather
 * than consumers that have better information when it comes to determining a
 * reasonable timeout for a given workload.
 */
struct drm_nvidia_semsurf_fence_wait_params {
    uint32_t fence_context_handle;  /* IN GEM handle to fence context which will
                                     * be used to wait on the sync FD.  Need not
                                     * be the fence context used to create the
                                     * sync FD. */

    int32_t  fd;                    /* IN sync FD object to wait on */

    uint64_t pre_wait_value;        /* IN Wait for the semaphore represented by
                                     * fence_context to reach this value before
                                     * waiting for the sync file. */

    uint64_t post_wait_value;       /* IN Signal the semaphore represented by
                                     * fence_context to this value after waiting
                                     * for the sync file */
};

struct drm_nvidia_semsurf_fence_attach_params {
    uint32_t handle;                /* IN GEM handle of buffer */

    uint32_t fence_context_handle;  /* IN GEM handle of fence context */

    uint32_t timeout_value_ms;      /* IN Timeout value in ms for the fence
                                     * after which the fence will be signaled
                                     * with its error status set to -ETIMEDOUT.
                                     * Default timeout value is 5000ms */

    uint32_t shared;                /* IN If true, fence will reserve shared
                                     * access to the buffer, otherwise it will
                                     * reserve exclusive access */

    uint64_t wait_value;            /* IN Semaphore value to reach before signal */
};

struct drm_nvidia_get_drm_file_unique_id_params {
    uint64_t id;                    /* OUT Unique ID of the DRM file */
};

struct drm_nvidia_semsurf_export_to_syncobj_point_params {
    uint32_t fence_context_handle;  /* IN GEM handle to fence context */
    uint32_t syncobj_handle;        /* IN DRM syncobj handle */
    uint64_t wait_value;            /* IN Semsurf timeline point to export */
    uint64_t syncobj_point;         /* IN DRM syncobj timeline point to export to */
};

struct drm_nvidia_syncobj_get_syncfd_params {
    uint32_t syncobj_handle;        /* IN DRM syncobj timeline handle */
    int32_t  fd;                    /* OUT sync FD for the syncobj timeline point */
    uint64_t syncobj_point;         /* IN DRM syncobj timeline point */
};

/**
 * @brief Parameters for getting ROI capabilities
 *
 * This structure is used with the DRM_NVIDIA_GET_ROI_CAPABILITIES ioctl to
 * get ROI related capabilities.
 *
 * @param[out] max_rois Maximum number of ROIs that can be registered
 */
struct drm_nvidia_get_roi_capabilities_params {
    uint32_t max_registered_rois;     /* OUT Maximum number of ROIs that can be registered */
    uint32_t reserved[7];
};

/**
 * @brief Rectangle structure for ROI
 *
 * This structure is used to define a rectangle region of interest.
 *
 * @param[in] x X coordinate of the top-left corner
 * @param[in] y Y coordinate of the top-left corner
 * @param[in] width Width of the ROI
 * @param[in] height Height of the ROI
 */
struct drm_nvidia_roi_rect {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};

/**
 * @brief Parameters for registering a Region of Interest (ROI)
 *
 * This structure is used to register an ROI region in the Region RAM which can
 * then be referenced by its region handle when configuring a window to enable CRC.
 * The region handle is treated as a separate resource and must be registered
 * outside of the regular commit cycle.
 *
 * @param[in] rect Rectangle defining the ROI coordinates
 * @param[out] region_handle Unique handle for the registered ROI
 */
struct drm_nvidia_register_roi_params {
    struct drm_nvidia_roi_rect rect;   /* IN */
    uint64_t region_handle;            /* OUT */
};

/**
 * @brief Parameters for unregistering a Region of Interest (ROI)
 *
 * This structure is used to unregister a previously registered ROI region.
 * Note: A region handle that is actively in use (configured in the CRC enablement
 * property) cannot be unregistered. Only a handle that was previously registered
 * by this client can be unregistered.
 *
 * @param[in] region_handle Handle of the ROI to unregister
 */
struct drm_nvidia_unregister_roi_params {
    uint64_t region_handle;          /* IN */
};

/**
 * @brief Per-plane telltale CRC configuration
 *
 * This structure defines the configuration for telltale CRC on a per-plane basis.
 * Each plane can have multiple telltale CRC regions configured, with each region
 * referencing a previously registered ROI via its region handle.
 *
 * @param[in] region_handle Handle of registered ROI to use
 * @param[in] golden_crc Expected CRC value for safety interrupts (0 if not configured)
 */
struct drm_nvidia_telltale_per_plane_config {
    uint64_t region_handle;         /* IN */
    uint64_t golden_crc;            /* IN */
};

/* NV_DRM_NUM_PLANE_TELLTALE_CRCS is a plane property that can be read to get
 * the max number of telltale CRCs that can be enabled on a plane */

/**
 * @brief Blob property structure for NV_DRM_PLANE_TELLTALES
 *
 * This structure is used as the content for the NV_DRM_PLANE_TELLTALES blob property
 * that is set on a plane before atomic commit. It contains an array of telltale
 * configurations, one for each telltale CRC region that should be enabled on the plane.
 * The number of configurations should not exceed the value read from the
 * NV_DRM_NUM_PLANE_TELLTALE_CRCS plane property.
 * This property is expected to be set in the regular atomic commit operation.
 *
 * @param[in] telltale_configs Array of telltale CRC configurations
 */
struct drm_nvidia_plane_telltales {
    struct drm_nvidia_telltale_per_plane_config \
        telltale_configs[NV_DRM_MAX_TELLTALES_PER_PLANE]; /* IN */
};

/**
 * Maximum subframes in the NV_DRM_CRTC_SUPERFRAME_CONFIG blob (array size). This
 * value should remain stable for UAPI compatibility.
 */
#define NV_DRM_MAX_SUBFRAMES_IN_SUPERFRAME 16U

/**
 * @brief Per-plane asymmetric superframe mode
 *
 * Used with the atomic range property NV_DRM_PLANE_ASYMMETRIC_SUPERFRAME_MODE
 * (values 0..2). Selects whether this plane applies HW or SW asymmetric
 * padding, or neither. Use NV_DRM_PLANE_ASYMMETRIC_SUPERFRAME_MODE_NO_PADDING
 * when this plane does not use asymmetric padding (default); use HW_PADDING or
 * SW_PADDING when the CRTC superframe layout requires it for this plane.
 *
 * Prefer HW_PADDING whenever display HW can match the serializer's asymmetric
 * layout; use SW_PADDING when it cannot (e.g. a SerDes-specific superframe
 * format the display hardware does not emit natively). If NV_DRM_CRTC_SUPERFRAME_CONFIG
 * sets sw_superframe_required for a subframe, use SW padding: compositor
 * supplies the padded plane (often an alpha-capable format), and this property
 * must match the prepared buffer on commit. Use HW_PADDING otherwise so the
 * pipeline applies padding_ratio_* from the blob.
 *
 * Clients must set this property on commit so the driver's plane programming
 * matches how the buffer was prepared.
 */
enum nv_drm_plane_asymmetric_superframe_mode {
    /*! This plane does not use HW or SW asymmetric padding (default). */
    NV_DRM_PLANE_ASYMMETRIC_SUPERFRAME_MODE_NO_PADDING = 0,
    /*!
     * HW padding: the display precomp path for this plane inserts or generates
     * pad lines per the CRTC superframe blob (padding_ratio_*). The plane surface
     * is typically active content only; the pipeline expands to the common
     * padded height for the serializer.
     */
    NV_DRM_PLANE_ASYMMETRIC_SUPERFRAME_MODE_HW_PADDING = 1,
    /*!
     * SW padding: use when display HW cannot emit the serializer's asymmetric
     * superframe format (see sw_superframe_required in the CRTC superframe
     * blob). Compositor builds the padded plane in memory; prefer HW padding
     * when supported (see brief above).
     */
    NV_DRM_PLANE_ASYMMETRIC_SUPERFRAME_MODE_SW_PADDING = 2,
};

/**
 * @brief One subframe entry within NV_DRM_CRTC_SUPERFRAME_CONFIG
 *
 * Describes geometry and padding metadata for a single logical subframe within
 * the CRTC's full superframe viewport. Symmetric layouts report
 * padding_required and padding_ratio_* as zero / unused.
 *
 * Layout: pixel box (four u32), HW padding ratios (three u32), flags (two u8),
 * explicit alignment padding, then uint64 growth space.
 *
 * @param[out] x_pos Left (X) origin of this subframe in full-viewport coordinates
 * @param[out] y_pos Top (Y) origin of this subframe in full-viewport coordinates
 * @param[out] width Width in pixels of this subframe's content region
 * @param[out] height Height in pixels of this subframe's content region (before padding)
 * @param[out] padding_ratio_input_lines When padding_required, input line count for the HW padding ratio (zero if unused)
 * @param[out] padding_ratio_output_lines When padding_required, output line count for the HW padding ratio (zero if unused)
 * @param[out] padding_ratio_start_count Zero for fixed-ratio padding; non-zero for dynamic fractional padding counter seed
 * @param[out] padding_required Non-zero if asymmetric padding applies to this subframe; zero if symmetric
 * @param[out] sw_superframe_required Non-zero if this subframe must be constructed with SW superframe only
 * @param[out] _reserved_align Structure padding; must be zero; pads to 8-byte alignment for reserved[]
 * @param[out] reserved Reserved for UAPI growth; zero
 */
struct drm_nvidia_subframe_config {
    uint32_t x_pos;                        /* OUT */
    uint32_t y_pos;                        /* OUT */
    uint32_t width;                        /* OUT */
    uint32_t height;                       /* OUT */
    uint32_t padding_ratio_input_lines;    /* OUT */
    uint32_t padding_ratio_output_lines;   /* OUT */
    uint32_t padding_ratio_start_count;    /* OUT */
    uint8_t  padding_required;             /* OUT */
    uint8_t  sw_superframe_required;       /* OUT */
    uint8_t  _reserved_align[2];           /* OUT */
    uint64_t reserved[4];                  /* OUT */
};

/**
 * @brief Blob property structure for NV_DRM_CRTC_SUPERFRAME_CONFIG
 *
 * Read-only blob on the CRTC reporting the platform superframe layout for that
 * CRTC (symmetric and asymmetric). UMD uses this to position planes and to
 * interpret padding fields; num_subframes must not exceed
 * NV_DRM_MAX_SUBFRAMES_IN_SUPERFRAME.
 *
 * Layout: num_subframes, a single reserved u32 (zero), subframes array, then
 * top-level reserved u64 words for UAPI growth.
 *
 * @param[out] num_subframes Number of valid entries in the subframes array
 * @param[out] _reserved Must be zero; reserved for UAPI growth and 8-byte alignment before subframes[]
 * @param[out] subframes Per-subframe geometry and padding metadata (see drm_nvidia_subframe_config)
 * @param[out] reserved Reserved for UAPI growth; zero
 */
struct drm_nvidia_superframe_config {
    uint32_t num_subframes;          /* OUT */
    uint32_t _reserved;              /* OUT */
    struct drm_nvidia_subframe_config \
        subframes[NV_DRM_MAX_SUBFRAMES_IN_SUPERFRAME]; /* OUT */
    uint64_t reserved[4];            /* OUT */
};

/**
 * @brief ROI CRC data structure
 *
 * This structure contains the CRC value for a specific ROI region. It is used
 * as part of the array returned by the DRM_NVIDIA_GET_CRTC_ROI_CRCS ioctl to
 * provide CRC values for all active ROI regions on a CRTC.
 *
 * @param[out] region_handle Handle of the ROI region
 * @param[out] crc Computed CRC value for this ROI
 * @param[in/out] reserved Reserved fields for future use
 */
struct drm_nvidia_roi_crc {
    uint64_t region_handle;         /* OUT */
    uint64_t crc;                   /* OUT */
    uint64_t reserved[4];
};

/**
 * @brief Parameters for reading CRC data from CRTC
 *
 * This structure is used with the DRM_NVIDIA_GET_CRTC_ROI_CRCS ioctl to read
 * the CRC values for all registered ROI regions that are currently active on
 * the specified CRTC. The ioctl will populate the roi_crcs array with CRC data
 * for each active region and set num_collected_crcs to indicate how many valid
 * entries are in the array.
 * Please note that only the last collected CRC is returned per active region handle.
 *
 * @param[in] crtc_id CRTC identifier to read CRCs from
 * @param[out] num_collected_crcs Number of valid CRC entries returned
 * @param[in/out] reserved Reserved fields for future use
 * @param[out] roi_crcs Array of ROI CRC data
 */
struct drm_nvidia_read_crc_params {
    int32_t crtc_id;                /* IN */
    int32_t num_collected_crcs;     /* OUT */
    struct drm_nvidia_roi_crc roi_crcs[NV_DRM_MAX_ROIS_PER_CRTC]; /* OUT */
    uint64_t reserved[4];
};

/**
 * @brief Named transfer function enum
 *
 * This enum defines the named transfer functions that can be used to set
 * the degamma and regamma transfer functions properties.
 */
 enum nv_drm_transfer_function {
    NV_DRM_TRANSFER_FUNCTION_DEFAULT,
    NV_DRM_TRANSFER_FUNCTION_LINEAR,
    NV_DRM_TRANSFER_FUNCTION_PQ,
    /*
     * nvidia-drm only supports the transfer function types defined above. The
     * below transfer functions are currently only supported by tegradisp-drm,
     * and not by nvidia-drm. Do not define a transfer function above
     * NV_DRM_TRANSFER_FUNCTION_SRGB without updating
     * NV_DRM_TRANSFER_FUNCTION_MAX.
     */
    NV_DRM_TRANSFER_FUNCTION_SRGB,
    NV_DRM_TRANSFER_FUNCTION_SMPTE_170M,
    NV_DRM_TRANSFER_FUNCTION_TEGRA_MAX,
    NV_DRM_TRANSFER_FUNCTION_MAX = NV_DRM_TRANSFER_FUNCTION_SRGB,
};


/**
 * @brief Struct for HDMI VSIF blob property
 *
 * This structure defines the content of the NV_HDMI_VSIF_METADATA blob property.
 * The payload is a 27 byte array where the first three bytes are the vendor OUI,
 * and the remaining bytes are the infoframe content.
 */
struct drm_nvidia_hdmi_vsif_metadata {
    uint8_t payload[NV_DRM_HDMI_VSIF_METADATA_MAX_PAYLOAD_SIZE];
};

/**
 * @brief Per-plane scaling capabilities blob data
 *
 * This structure defines the content of the NV_DRM_PLANE_SCALING_CAPS
 * immutable blob property. Each scale factor is represented as a 1024-based
 * fixed-point value, where 1024 is 1.0x.
 *
 * Downscale factors use source size divided by destination size. Upscale
 * factors use destination size divided by source size.
 *
 * @param[out] max_h_downscale Max horizontal downscale factor.
 * @param[out] max_v_downscale Max vertical downscale factor.
 * @param[out] max_h_upscale Max horizontal upscale factor.
 * @param[out] max_v_upscale Max vertical upscale factor.
 * @param[out] reserved Reserved for future expansion. Must be zero.
 */
struct drm_nvidia_plane_scaling_caps {
    __u32 max_h_downscale;  /* OUT */
    __u32 max_v_downscale;  /* OUT */
    __u32 max_h_upscale;    /* OUT */
    __u32 max_v_upscale;    /* OUT */
    __u32 reserved[4];      /* OUT: reserved, must be zero */
};

#endif /* _NV_DRM_COMMON_IOCTL_H_ */
