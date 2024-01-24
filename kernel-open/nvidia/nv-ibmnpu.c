/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * nv-ibmnpu.c - interface with the ibmnpu (IBM NVLink Processing Unit) "module"
 */
#include "nv-linux.h"

#if defined(NVCPU_PPC64LE)
#include "nv-ibmnpu.h"
#include "nv-rsync.h"

/*
 * Temporary query to get the L1D cache block size directly from the device
 * tree for the offline cache flush workaround, since the ppc64_caches symbol
 * is unavailable to us.
 */
const NvU32 P9_L1D_CACHE_DEFAULT_BLOCK_SIZE = 0x80;

static NvU32 nv_ibm_get_cpu_l1d_cache_block_size(void)
{
    const __be32 *block_size_prop;

    /*
     * Attempt to look up the block size from device tree. If unavailable, just
     * return the default that we see on these systems.
     */
    struct device_node *cpu = of_find_node_by_type(NULL, "cpu");
    if (!cpu)
    {
        return P9_L1D_CACHE_DEFAULT_BLOCK_SIZE;
    }

    block_size_prop = of_get_property(cpu, "d-cache-block-size", NULL);
    if (!block_size_prop)
    {
        return P9_L1D_CACHE_DEFAULT_BLOCK_SIZE;
    }

    return be32_to_cpu(*block_size_prop);
}

/*
 * GPU device memory can be exposed to the kernel as NUMA node memory via the
 * IBMNPU devices associated with the GPU. The platform firmware will specify
 * the parameters of where the memory lives in the system address space via
 * firmware properties on the IBMNPU devices. These properties specify what
 * memory can be accessed through the IBMNPU device, and the driver can online
 * a GPU device's memory into the range accessible by its associated IBMNPU
 * devices.
 *
 * This function calls over to the IBMNPU driver to query the parameters from
 * firmware, and validates that the resulting parameters are acceptable.
 */
static void nv_init_ibmnpu_numa_info(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_npu_numa_info_t *npu_numa_info = &nvl->npu->numa_info;
    struct pci_dev *npu_dev = nvl->npu->devs[0];
    NvU64 spa, gpa, aper_size;

    /*
     * Terminology:
     * - system physical address (spa): 47-bit NVIDIA physical address, which
     *      is the CPU real address with the NVLink address compression scheme
     *      already applied in firmware.
     * - guest physical address (gpa): 56-bit physical address as seen by the
     *      operating system. This is the base address that we should use for
     *      onlining device memory.
     */
    nvl->numa_info.node_id = ibmnpu_device_get_memory_config(npu_dev, &spa, &gpa,
                                                             &aper_size);
    if (nvl->numa_info.node_id == NUMA_NO_NODE)
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv, "No NUMA memory aperture found\n");
        return;
    }

    /* Validate that the compressed system physical address is not too wide */
    if (spa & (~(BIT_ULL(nv_volta_dma_addr_size) - 1)))
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
            "Invalid NUMA memory system pa 0x%llx"
            " on IBM-NPU device %04x:%02x:%02x.%u\n",
            spa, NV_PCI_DOMAIN_NUMBER(npu_dev), NV_PCI_BUS_NUMBER(npu_dev),
            NV_PCI_SLOT_NUMBER(npu_dev), PCI_FUNC(npu_dev->devfn));
        goto invalid_numa_config;
    }

    /*
     * Validate that the guest physical address is aligned to 128GB.
     * This alignment requirement comes from the Volta address space
     * size on POWER9.
     */
    if (!IS_ALIGNED(gpa, BIT_ULL(nv_volta_addr_space_width)))
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
            "Invalid alignment in NUMA memory guest pa 0x%llx"
            " on IBM-NPU device %04x:%02x:%02x.%u\n",
            gpa, NV_PCI_DOMAIN_NUMBER(npu_dev), NV_PCI_BUS_NUMBER(npu_dev),
            NV_PCI_SLOT_NUMBER(npu_dev), PCI_FUNC(npu_dev->devfn));
        goto invalid_numa_config;
    }

    /* Validate that the aperture can map all of the device's framebuffer */
    if (aper_size < nv->fb->size)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
            "Insufficient NUMA memory aperture size 0x%llx"
            " on IBM-NPU device %04x:%02x:%02x.%u (0x%llx required)\n",
            aper_size, NV_PCI_DOMAIN_NUMBER(npu_dev),
            NV_PCI_BUS_NUMBER(npu_dev), NV_PCI_SLOT_NUMBER(npu_dev),
            PCI_FUNC(npu_dev->devfn), nv->fb->size);
        goto invalid_numa_config;
    }

    npu_numa_info->compr_sys_phys_addr = spa;
    npu_numa_info->guest_phys_addr = gpa;

    if (NVreg_EnableUserNUMAManagement)
    {
        NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_OFFLINE);
    }
    else
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv, "User-mode NUMA onlining disabled.\n");
        nvl->numa_info.node_id = NUMA_NO_NODE;
    }

    NV_DEV_PRINTF(NV_DBG_SETUP, nv, "NUMA memory aperture: "
        "[spa = 0x%llx, gpa = 0x%llx, aper_size = 0x%llx]\n",
        spa, gpa, aper_size);

    /* Get the CPU's L1D cache block size for offlining cache flush */
    npu_numa_info->l1d_cache_block_size = nv_ibm_get_cpu_l1d_cache_block_size();

    return;

invalid_numa_config:
    NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
        "NUMA memory aperture disabled due to invalid firmware configuration\n");
    nvl->numa_info.node_id = NUMA_NO_NODE;
}

void nv_init_ibmnpu_info(nv_state_t *nv)
{
#if defined(NV_PNV_PCI_GET_NPU_DEV_PRESENT)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct pci_dev *npu_dev = pnv_pci_get_npu_dev(nvl->pci_dev, 0);
    NvU8 dev_count;

    if (!npu_dev)
    {
        return;
    }

    if (os_alloc_mem((void **)&nvl->npu, sizeof(nv_ibmnpu_info_t)) != NV_OK)
    {
        return;
    }

    os_mem_set(nvl->npu, 0, sizeof(nv_ibmnpu_info_t));

    /* Find any other IBMNPU devices attached to this GPU */
    for (nvl->npu->devs[0] = npu_dev, dev_count = 1;
         dev_count < NV_MAX_ATTACHED_IBMNPUS; dev_count++)
    {
        nvl->npu->devs[dev_count] = pnv_pci_get_npu_dev(nvl->pci_dev, dev_count);
        if (!nvl->npu->devs[dev_count])
        {
            break;
        }
    }

    nvl->npu->dev_count = dev_count;

    /*
     * If we run out of space for IBMNPU devices, NV_MAX_ATTACHED_IBMNPUS will
     * need to be bumped.
     */
    WARN_ON((dev_count == NV_MAX_ATTACHED_IBMNPUS) &&
            pnv_pci_get_npu_dev(nvl->pci_dev, dev_count));

    ibmnpu_device_get_genregs_info(npu_dev, &nvl->npu->genregs);

    if (nvl->npu->genregs.size > 0)
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv,
            "IBM-NPU device %04x:%02x:%02x.%u associated with GPU "
            " has a generation register space 0x%llx-0x%llx\n",
            NV_PCI_DOMAIN_NUMBER(npu_dev), NV_PCI_BUS_NUMBER(npu_dev),
            NV_PCI_SLOT_NUMBER(npu_dev), PCI_FUNC(npu_dev->devfn),
            nvl->npu->genregs.start_addr,
            nvl->npu->genregs.start_addr + nvl->npu->genregs.size - 1);
    }
    else
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv,
            "IBM-NPU device %04x:%02x:%02x.%u associated with GPU "
            "does not support generation registers\n",
            NV_PCI_DOMAIN_NUMBER(npu_dev), NV_PCI_BUS_NUMBER(npu_dev),
            NV_PCI_SLOT_NUMBER(npu_dev), PCI_FUNC(npu_dev->devfn));
    }

    nv_init_ibmnpu_numa_info(nv);
#endif
}

void nv_destroy_ibmnpu_info(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (nvl->npu != NULL)
    {
        os_free_mem(nvl->npu);
        nvl->npu = NULL;
    }
}

int nv_init_ibmnpu_devices(nv_state_t *nv)
{
    NvU8 i;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (!nvl->npu)
    {
        return 0;
    }

    for (i = 0; i < nvl->npu->dev_count; i++)
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv,
            "Initializing IBM-NPU device %04x:%02x:%02x.%u\n",
            NV_PCI_DOMAIN_NUMBER(nvl->npu->devs[i]),
            NV_PCI_BUS_NUMBER(nvl->npu->devs[i]),
            NV_PCI_SLOT_NUMBER(nvl->npu->devs[i]),
            PCI_FUNC(nvl->npu->devs[i]->devfn));

        if (ibmnpu_init_device(nvl->npu->devs[i]) != NVL_SUCCESS)
        {
            nv_unregister_ibmnpu_devices(nv);
            return -EIO;
        }

        nvl->npu->initialized_dev_count++;
    }

    return 0;
}

void nv_unregister_ibmnpu_devices(nv_state_t *nv)
{
    NvU8 i;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (!nvl->npu)
    {
        return;
    }

    for (i = 0; i < nvl->npu->initialized_dev_count; i++)
    {
        NV_DEV_PRINTF(NV_DBG_SETUP, nv,
            "Unregistering IBM-NPU device %04x:%02x:%02x.%u\n",
            NV_PCI_DOMAIN_NUMBER(nvl->npu->devs[i]),
            NV_PCI_BUS_NUMBER(nvl->npu->devs[i]),
            NV_PCI_SLOT_NUMBER(nvl->npu->devs[i]),
            PCI_FUNC(nvl->npu->devs[i]->devfn));

        ibmnpu_unregister_device(nvl->npu->devs[i]);
    }

    nvl->npu->initialized_dev_count = 0;
}

NV_STATUS NV_API_CALL nv_get_ibmnpu_genreg_info(nv_state_t *nv, NvU64 *addr,
                                                NvU64 *size, void **device)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (nvl->npu == NULL || nvl->npu->genregs.size == 0)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (addr)
    {
        *addr = nvl->npu->genregs.start_addr;
    }

    if (size)
    {
        *size = nvl->npu->genregs.size;
    }

    if (device)
    {
        *device = (void*)nvl->npu->devs[0];
    }

    return NV_OK;
}

NV_STATUS NV_API_CALL nv_get_ibmnpu_relaxed_ordering_mode(nv_state_t *nv,
                                                          NvBool *mode)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (nvl->npu == NULL || nvl->npu->genregs.size == 0)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    *mode = nv_get_rsync_relaxed_ordering_mode(nv);

    return NV_OK;
}

void NV_API_CALL nv_wait_for_ibmnpu_rsync(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (nvl->npu == NULL || nvl->npu->genregs.size == 0)
    {
        return;
    }

    nv_wait_for_rsync(nv);
}

int nv_get_ibmnpu_chip_id(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (nvl->npu == NULL)
    {
        return -1;
    }

    return ibmnpu_device_get_chip_id(nvl->npu->devs[0]);
}

void NV_API_CALL nv_ibmnpu_cache_flush_range(nv_state_t *nv, NvU64 cpu_virtual, NvU64 size)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU64 offset, cbsize;

    /*
     * The range is commonly an ioremap()ed mapping of the GPU's ATS range and
     * needs to be compared against the created mappings. Alternatively, kernel
     * page tables can be dumped through sysfs if CONFIG_PPC_PTDUMP is enabled.
     */
    NV_DEV_PRINTF(NV_DBG_INFO, nv,
            "Flushing CPU virtual range [0x%llx, 0x%llx)\n",
            cpu_virtual, cpu_virtual + size);

    cbsize = nvl->npu->numa_info.l1d_cache_block_size;

    asm volatile("sync; isync" ::: "memory");

    /* Force eviction of any cache lines from the NUMA-onlined region. */
    for (offset = 0; offset < size; offset += cbsize)
    {
        asm volatile("dcbf %0,%1" :: "r" (cpu_virtual), "r" (offset) : "memory");

        /* Reschedule if necessary to avoid lockup warnings */
        cond_resched();
    }

    asm volatile("sync; isync" ::: "memory");
}

#else

void nv_init_ibmnpu_info(nv_state_t *nv)
{
}

void nv_destroy_ibmnpu_info(nv_state_t *nv)
{
}

int nv_init_ibmnpu_devices(nv_state_t *nv)
{
    return 0;
}

void nv_unregister_ibmnpu_devices(nv_state_t *nv)
{
}

NV_STATUS NV_API_CALL nv_get_ibmnpu_genreg_info(nv_state_t *nv, NvU64 *addr,
                                                NvU64 *size, void **device)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL nv_get_ibmnpu_relaxed_ordering_mode(nv_state_t *nv,
                                                          NvBool *mode)
{
    return NV_ERR_NOT_SUPPORTED;
}

void NV_API_CALL nv_wait_for_ibmnpu_rsync(nv_state_t *nv)
{
}

int nv_get_ibmnpu_chip_id(nv_state_t *nv)
{
    return -1;
}

void NV_API_CALL nv_ibmnpu_cache_flush_range(nv_state_t *nv, NvU64 virtual, NvU64 size)
{
}

void nv_ibmnpu_cache_flush_numa_region(nv_state_t *nv)
{
}

#endif
