/*
 * SPDX-FileCopyrightText: Copyright (c) 2011-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_P2P_H_
#define _NV_P2P_H_

/*
 * NVIDIA P2P Structure Versioning
 *
 * For the nvidia_p2p_*_t structures allocated by the NVIDIA driver, it will
 * set the version field of the structure according to the definition used by
 * the NVIDIA driver. The "major" field of the version is defined as the upper
 * 16 bits, and the "minor" field of the version is defined as the lower 16
 * bits. The version field will always be the first 4 bytes of the structure,
 * and third-party drivers should check the value of this field in structures
 * allocated by the NVIDIA driver to ensure runtime compatibility.
 *
 * In general, version numbers will be incremented as follows:
 * - When a backwards-compatible change is made to the structure layout, the
 *   minor version for that structure will be incremented. Third-party drivers
 *   built against an older minor version will continue to work with the newer
 *   minor version used by the NVIDIA driver, without recompilation.
 * - When a breaking change is made to the structure layout, the major version
 *   will be incremented. Third-party drivers built against an older major
 *   version require at least recompilation and potentially additional updates
 *   to use the new API.
 */
#define NVIDIA_P2P_MAJOR_VERSION_MASK   0xffff0000
#define NVIDIA_P2P_MINOR_VERSION_MASK   0x0000ffff

#define NVIDIA_P2P_MAJOR_VERSION(v) \
    (((v) & NVIDIA_P2P_MAJOR_VERSION_MASK) >> 16)

#define NVIDIA_P2P_MINOR_VERSION(v) \
    (((v) & NVIDIA_P2P_MINOR_VERSION_MASK))

#define NVIDIA_P2P_MAJOR_VERSION_MATCHES(p, v) \
    (NVIDIA_P2P_MAJOR_VERSION((p)->version) == NVIDIA_P2P_MAJOR_VERSION(v))

#define NVIDIA_P2P_VERSION_COMPATIBLE(p, v)    \
    (NVIDIA_P2P_MAJOR_VERSION_MATCHES(p, v) && \
     (NVIDIA_P2P_MINOR_VERSION((p)->version) >= (NVIDIA_P2P_MINOR_VERSION(v))))

enum {
    NVIDIA_P2P_ARCHITECTURE_TESLA = 0,
    NVIDIA_P2P_ARCHITECTURE_FERMI,
    NVIDIA_P2P_ARCHITECTURE_CURRENT = NVIDIA_P2P_ARCHITECTURE_FERMI
};

#define NVIDIA_P2P_PARAMS_VERSION   0x00010001

enum {
    NVIDIA_P2P_PARAMS_ADDRESS_INDEX_GPU = 0,
    NVIDIA_P2P_PARAMS_ADDRESS_INDEX_THIRD_PARTY_DEVICE,
    NVIDIA_P2P_PARAMS_ADDRESS_INDEX_MAX = \
        NVIDIA_P2P_PARAMS_ADDRESS_INDEX_THIRD_PARTY_DEVICE
};

#define NVIDIA_P2P_GPU_UUID_LEN   16

typedef
struct nvidia_p2p_params {
    uint32_t version;
    uint32_t architecture;
    union nvidia_p2p_mailbox_addresses {
        struct {
            uint64_t wmb_addr;
            uint64_t wmb_data;
            uint64_t rreq_addr;
            uint64_t rcomp_addr;
            uint64_t reserved[2];
        } fermi;
    } addresses[NVIDIA_P2P_PARAMS_ADDRESS_INDEX_MAX+1];
} nvidia_p2p_params_t;

/*
 * Macro for users to detect
 * driver support for persistent pages.
 */
#define NVIDIA_P2P_CAP_GET_PAGES_PERSISTENT_API

/*
 * This API is not supported.
 */
int nvidia_p2p_init_mapping(uint64_t p2p_token,
        struct nvidia_p2p_params *params,
        void (*destroy_callback)(void *data),
        void *data);

/*
 * This API is not supported.
 */
int nvidia_p2p_destroy_mapping(uint64_t p2p_token);

enum nvidia_p2p_page_size_type {
    NVIDIA_P2P_PAGE_SIZE_4KB = 0,
    NVIDIA_P2P_PAGE_SIZE_64KB,
    NVIDIA_P2P_PAGE_SIZE_128KB,
    NVIDIA_P2P_PAGE_SIZE_COUNT
};

typedef
struct nvidia_p2p_page {
    uint64_t physical_address;
    union nvidia_p2p_request_registers {
        struct {
            uint32_t wreqmb_h;
            uint32_t rreqmb_h;
            uint32_t rreqmb_0;
            uint32_t reserved[3];
        } fermi;
    } registers;
} nvidia_p2p_page_t;

#define NVIDIA_P2P_PAGE_TABLE_VERSION   0x00010002

#define NVIDIA_P2P_PAGE_TABLE_VERSION_COMPATIBLE(p) \
    NVIDIA_P2P_VERSION_COMPATIBLE(p, NVIDIA_P2P_PAGE_TABLE_VERSION)

typedef
struct nvidia_p2p_page_table {
    uint32_t version;
    uint32_t page_size; /* enum nvidia_p2p_page_size_type */
    struct nvidia_p2p_page **pages;
    uint32_t entries;
    uint8_t *gpu_uuid;
} nvidia_p2p_page_table_t;

/*
 * @brief
 *   Make the pages underlying a range of GPU virtual memory
 *   accessible to a third-party device.
 *
 *   This API only supports pinned, GPU-resident memory, such as that provided
 *   by cudaMalloc().
 *
 *   This API may sleep.
 *
 * @param[in]     p2p_token
 *   A token that uniquely identifies the P2P mapping.
 * @param[in]     va_space
 *   A GPU virtual address space qualifier.
 * @param[in]     virtual_address
 *   The start address in the specified virtual address space.
 *   Address must be aligned to the 64KB boundary.
 * @param[in]     length
 *   The length of the requested P2P mapping.
 *   Length must be a multiple of 64KB.
 * @param[out]    page_table
 *   A pointer to an array of structures with P2P PTEs.
 * @param[in]     free_callback
 *   A pointer to the function to be invoked when the pages
 *   underlying the virtual address range are freed
 *   implicitly.
 * @param[in]     data
 *   A non-NULL opaque pointer to private data to be passed to the
 *   callback function.
 *
 * @return
 *    0           upon successful completion.
 *   -EINVAL      if an invalid argument was supplied.
 *   -ENOTSUPP    if the requested operation is not supported.
 *   -ENOMEM      if the driver failed to allocate memory or if
 *     insufficient resources were available to complete the operation.
 *   -EIO         if an unknown error occurred.
 */
int nvidia_p2p_get_pages( uint64_t p2p_token, uint32_t va_space,
        uint64_t virtual_address, uint64_t length,
        struct nvidia_p2p_page_table **page_table,
        void (*free_callback)(void *data), void *data);

/*
 * Flags to be used with persistent APIs
 */
#define NVIDIA_P2P_FLAGS_DEFAULT            0
#define NVIDIA_P2P_FLAGS_FORCE_BAR1_MAPPING 1

/*
 * @brief
 *   Pin and make the pages underlying a range of GPU virtual memory
 *   accessible to a third-party device. The pages will persist until
 *   explicitly freed by nvidia_p2p_put_pages_persistent().
 *
 *   Persistent GPU memory mappings are not supported on PowerPC,
 *   MIG-enabled devices and vGPU.
 *
 *   This API only supports pinned, GPU-resident memory, such as that provided
 *   by cudaMalloc().
 *
 *   This API may sleep.
 *
 * @param[in]     virtual_address
 *   The start address in the specified virtual address space.
 *   Address must be aligned to the 64KB boundary.
 * @param[in]     length
 *   The length of the requested P2P mapping.
 *   Length must be a multiple of 64KB.
 * @param[out]    page_table
 *   A pointer to an array of structures with P2P PTEs.
 * @param[in]     flags
 *   NVIDIA_P2P_FLAGS_DEFAULT:
 *     Default value to be used if no specific behavior is expected.
 *   NVIDIA_P2P_FLAGS_FORCE_BAR1_MAPPING:
 *     Force BAR1 mappings on certain coherent platforms,
 *     subject to capability and supported topology.
 *
 * @return
 *    0           upon successful completion.
 *   -EINVAL      if an invalid argument was supplied.
 *   -ENOTSUPP    if the requested operation is not supported.
 *   -ENOMEM      if the driver failed to allocate memory or if
 *     insufficient resources were available to complete the operation.
 *   -EIO         if an unknown error occurred.
 */
int nvidia_p2p_get_pages_persistent(uint64_t virtual_address,
        uint64_t length,
        struct nvidia_p2p_page_table **page_table,
        uint32_t flags);

#define NVIDIA_P2P_DMA_MAPPING_VERSION   0x00020003

#define NVIDIA_P2P_DMA_MAPPING_VERSION_COMPATIBLE(p) \
    NVIDIA_P2P_VERSION_COMPATIBLE(p, NVIDIA_P2P_DMA_MAPPING_VERSION)

struct pci_dev;

typedef
struct nvidia_p2p_dma_mapping {
    uint32_t version;
    enum nvidia_p2p_page_size_type page_size_type;
    uint32_t entries;
    uint64_t *dma_addresses;
    void *private;
    struct pci_dev *pci_dev;
} nvidia_p2p_dma_mapping_t;

/*
 * @brief
 *   Make the physical pages retrieved using nvidia_p2p_get_pages accessible to
 *   a third-party device.
 *
 * @param[in]     peer
 *   The struct pci_dev * of the peer device that needs to DMA to/from the
 *   mapping.
 * @param[in]     page_table
 *   The page table outlining the physical pages underlying the mapping, as
 *   retrieved with nvidia_p2p_get_pages().
 * @param[out]    dma_mapping
 *   The DMA mapping containing the DMA addresses to use on the third-party
 *   device.
 *
 * @return
 *    0           upon successful completion.
 *    -EINVAL     if an invalid argument was supplied.
 *    -ENOTSUPP   if the requested operation is not supported.
 *    -EIO        if an unknown error occurred.
 */
int nvidia_p2p_dma_map_pages(struct pci_dev *peer,
        struct nvidia_p2p_page_table *page_table,
        struct nvidia_p2p_dma_mapping **dma_mapping);

/*
 * @brief
 *   Unmap the physical pages previously mapped to the third-party device by
 *   nvidia_p2p_dma_map_pages().
 *
 * @param[in]     peer
 *   The struct pci_dev * of the peer device that the DMA mapping belongs to.
 * @param[in]     page_table
 *   The page table backing the DMA mapping to be unmapped.
 * @param[in]     dma_mapping
 *   The DMA mapping containing the DMA addresses used by the third-party
 *   device, as retrieved with nvidia_p2p_dma_map_pages(). After this call
 *   returns, neither this struct nor the addresses contained within will be
 *   valid for use by the third-party device.
 *
 * @return
 *    0           upon successful completion.
 *    -EINVAL     if an invalid argument was supplied.
 *    -EIO        if an unknown error occurred.
 */
int nvidia_p2p_dma_unmap_pages(struct pci_dev *peer,
        struct nvidia_p2p_page_table *page_table,
        struct nvidia_p2p_dma_mapping *dma_mapping);

/*
 * @brief
 *   Release a set of pages previously made accessible to
 *   a third-party device.
 *
 *   This API may sleep.
 *
 * @param[in]     p2p_token
 *   A token that uniquely identifies the P2P mapping.
 * @param[in]     va_space
 *   A GPU virtual address space qualifier.
 * @param[in]     virtual_address
 *   The start address in the specified virtual address space.
 * @param[in]     page_table
 *   A pointer to the array of structures with P2P PTEs.
 *
 * @return
 *    0           upon successful completion.
 *   -EINVAL      if an invalid argument was supplied.
 *   -EIO         if an unknown error occurred.
 */
int nvidia_p2p_put_pages(uint64_t p2p_token,
        uint32_t va_space, uint64_t virtual_address,
        struct nvidia_p2p_page_table *page_table);

/*
 * @brief
 *   Release a set of persistent pages previously made accessible to
 *   a third-party device.
 *
 *   This API may sleep.
 *
 * @param[in]     virtual_address
 *   The start address in the specified virtual address space.
 * @param[in]     page_table
 *   A pointer to the array of structures with P2P PTEs.
 * @param[in]     flags
 *   Must be set to zero for now.
 *
 * @return
 *    0           upon successful completion.
 *   -EINVAL      if an invalid argument was supplied.
 *   -EIO         if an unknown error occurred.
 */
int nvidia_p2p_put_pages_persistent(uint64_t virtual_address,
        struct nvidia_p2p_page_table *page_table,
        uint32_t flags);

/*
 * @brief
 *    Free a third-party P2P page table. (This function is a no-op.)
 *
 * @param[in]     page_table
 *   A pointer to the array of structures with P2P PTEs.
 *
 * @return
 *    0           upon successful completion.
 *   -EINVAL      if an invalid argument was supplied.
 */
int nvidia_p2p_free_page_table(struct nvidia_p2p_page_table *page_table);

/*
 * @brief
 *   Free a third-party P2P DMA mapping. (This function is a no-op.)
 *
 * @param[in]     dma_mapping
 *   A pointer to the DMA mapping structure.
 *
 * @return
 *    0           upon successful completion.
 *    -EINVAL     if an invalid argument was supplied.
 */
int nvidia_p2p_free_dma_mapping(struct nvidia_p2p_dma_mapping *dma_mapping);

#define NVIDIA_P2P_RSYNC_DRIVER_VERSION   0x00010001

#define NVIDIA_P2P_RSYNC_DRIVER_VERSION_COMPATIBLE(p) \
    NVIDIA_P2P_VERSION_COMPATIBLE(p, NVIDIA_P2P_RSYNC_DRIVER_VERSION)

typedef
struct nvidia_p2p_rsync_driver {
    uint32_t version;
    int (*get_relaxed_ordering_mode)(int *mode, void *data);
    void (*put_relaxed_ordering_mode)(int mode, void *data);
    void (*wait_for_rsync)(struct pci_dev *gpu, void *data);
} nvidia_p2p_rsync_driver_t;

/*
 * @brief
 *   Registers the rsync driver.
 *
 * @param[in]     driver
 *   A pointer to the rsync driver structure. The NVIDIA driver would use,
 *
 *   get_relaxed_ordering_mode to obtain a reference to the current relaxed
 *   ordering mode (treated as a boolean) from the rsync driver.
 *
 *   put_relaxed_ordering_mode to release a reference to the current relaxed
 *   ordering mode back to the rsync driver. The NVIDIA driver will call this
 *   function once for each successful call to get_relaxed_ordering_mode, and
 *   the relaxed ordering mode must not change until the last reference is
 *   released.
 *
 *   wait_for_rsync to call into the rsync module to issue RSYNC. This callback
 *   can't sleep or re-schedule as it may arrive under spinlocks.
 * @param[in]     data
 *   A pointer to the rsync driver's private data.
 *
 * @Returns
 *   0            upon successful completion.
 *   -EINVAL      parameters are incorrect.
 *   -EBUSY       if a module is already registered or GPU devices are in use.
 */
int nvidia_p2p_register_rsync_driver(nvidia_p2p_rsync_driver_t *driver,
                                     void *data);

/*
 * @brief
 *   Unregisters the rsync driver.
 *
 * @param[in]     driver
 *   A pointer to the rsync driver structure.
 * @param[in]     data
 *   A pointer to the rsync driver's private data.
 */
void nvidia_p2p_unregister_rsync_driver(nvidia_p2p_rsync_driver_t *driver,
                                        void *data);

#define NVIDIA_P2P_RSYNC_REG_INFO_VERSION   0x00020001

#define NVIDIA_P2P_RSYNC_REG_INFO_VERSION_COMPATIBLE(p) \
    NVIDIA_P2P_VERSION_COMPATIBLE(p, NVIDIA_P2P_RSYNC_REG_INFO_VERSION)

typedef struct nvidia_p2p_rsync_reg {
    void *ptr;
    size_t size;
    struct pci_dev *ibmnpu;
    struct pci_dev *gpu;
    uint32_t cluster_id;
    uint32_t socket_id;
} nvidia_p2p_rsync_reg_t;

typedef struct nvidia_p2p_rsync_reg_info {
    uint32_t version;
    nvidia_p2p_rsync_reg_t *regs;
    size_t entries;
} nvidia_p2p_rsync_reg_info_t;

/*
 * @brief
 *   This interface is no longer supported and will always return an error.  It
 *   is left in place (for now) to allow third-party callers to build without
 *   any errors.
 *
 * @Returns
 *   -ENODEV
 */
int nvidia_p2p_get_rsync_registers(nvidia_p2p_rsync_reg_info_t **reg_info);

/*
 * @brief
 *   This interface is no longer supported.  It is left in place (for now) to
 *   allow third-party callers to build without any errors.
 */
void nvidia_p2p_put_rsync_registers(nvidia_p2p_rsync_reg_info_t *reg_info);

#endif /* _NV_P2P_H_ */
