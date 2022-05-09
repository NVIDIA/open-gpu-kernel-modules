/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_IBMNPU_H_
#define _NV_IBMNPU_H_

#if defined(NVCPU_PPC64LE)

#include "ibmnpu_linux.h"

#define NV_MAX_ATTACHED_IBMNPUS 6

typedef struct nv_npu_numa_info
{
    /*
     * 47-bit NVIDIA 'system physical address': the hypervisor real 56-bit
     * address with NVLink address compression scheme applied.
     */
    NvU64 compr_sys_phys_addr;

    /*
     * 56-bit NVIDIA 'guest physical address'/host virtual address. On
     * unvirtualized systems, applying the NVLink address compression scheme
     * to this address should be the same as compr_sys_phys_addr.
     */
    NvU64 guest_phys_addr;

    /*
     * L1 data cache block size on P9 - needed to manually flush/invalidate the
     * NUMA region from the CPU caches after offlining.
     */
    NvU32 l1d_cache_block_size;
} nv_npu_numa_info_t;

struct nv_ibmnpu_info
{
    NvU8 dev_count;
    NvU8 initialized_dev_count;
    struct pci_dev *devs[NV_MAX_ATTACHED_IBMNPUS];
    ibmnpu_genregs_info_t genregs;
    nv_npu_numa_info_t numa_info;
};

/*
 * TODO: These parameters are specific to Volta/P9 configurations, and may
 *       need to be determined dynamically in the future.
 */
static const NvU32 nv_volta_addr_space_width = 37;
static const NvU32 nv_volta_dma_addr_size = 47;

#endif

void nv_init_ibmnpu_info(nv_state_t *nv);
void nv_destroy_ibmnpu_info(nv_state_t *nv);
int nv_init_ibmnpu_devices(nv_state_t *nv);
void nv_unregister_ibmnpu_devices(nv_state_t *nv);
int nv_get_ibmnpu_chip_id(nv_state_t *nv);
void nv_ibmnpu_cache_flush_numa_region(nv_state_t *nv);

#endif
