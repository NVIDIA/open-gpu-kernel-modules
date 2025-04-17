/*
 * Copyright (c) 2015-2016, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef _UAPI_NVIDIA_DRM_IOCTL_H_
#define _UAPI_NVIDIA_DRM_IOCTL_H_

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

#endif /* _UAPI_NVIDIA_DRM_IOCTL_H_ */
