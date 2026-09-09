/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_PCI_H_
#define _NV_PCI_H_

#include <linux/pci.h>
#include "nv-linux.h"
#include <uapi/linux/pci_regs.h>

#if defined(NV_CXL_CXL_H_PRESENT)
#include <cxl/cxl.h>
#endif

#if defined(NV_CXL_PCI_H_PRESENT)
#include <cxl/pci.h>
#endif

#define NV_GPU_BAR1 1
#define NV_GPU_BAR3 3

int nv_pci_register_driver(void);
void nv_pci_unregister_driver(void);
int nv_pci_count_devices(void);
void nv_pci_wait_for_probe_complete(void);
NvU8 nv_find_pci_capability(struct pci_dev *, NvU8);
int nvidia_dev_get_pci_info(const NvU8 *, struct pci_dev **, NvU64 *, NvU64 *);
NvBool nv_pci_is_valid_topology_for_direct_pci(nv_state_t *, struct pci_dev *);
NvBool nv_pci_has_common_pci_switch(nv_state_t *nv, struct pci_dev *);
void nv_pci_tegra_boost_clocks(struct device *dev);

#define NV_PCI_EXT_CAP_START_OFFSET         0x100
#define NV_PCI_EXT_CAP_END_OFFSET           0x1000
/* Maximum number of iterations to traverse the PCI extended capability chain.
   Each capability is minimum 8 bytes long. */
#define NV_PCI_EXT_CAP_MAX_ITERATIONS       ((NV_PCI_EXT_CAP_END_OFFSET - NV_PCI_EXT_CAP_START_OFFSET) / 8)
#define NV_PCI_EXT_CAP_ID(hdr0)             ((hdr0) & 0xffff)
#define NV_PCI_EXT_CAP_VERSION(hdr0)        (((hdr0) & 0x000f0000) >> 16)
#define NV_PCI_EXT_CAP_NEXT_OFFSET(hdr0)    (((hdr0) & 0xfff00000) >> 20)
#define NV_PCI_EXT_CAP_ID_DVSEC             0x0023
#define NV_PCI_DVSEC_HEADER_1_OFFSET        0x4
#define NV_PCI_DVSEC_VENDOR_ID(hdr1)        ((hdr1) & 0xffff)
#define NV_PCI_DVSEC_REVISION(hdr1)         (((hdr1) & 0x000f0000) >> 16)
#define NV_PCI_DVSEC_LEN(hdr1)              (((hdr1) & 0xfff00000) >> 20)
#define NV_PCI_DVSEC_HEADER_2_OFFSET        0x8
#define NV_PCI_DVSEC_DESIGNATED_ID(hdr2)    ((hdr2) & 0xffff)
#define NV_PCI_DVSEC_NVIDIA_VENDOR_ID       0x10de

/* DVSEC 3 specific constants */
#define NV_PCI_DVSEC_GPU_PDI_DESIGNATED_ID  0x3
#define NV_PCI_DVSEC_GPU_PDI_CAP_LEN        0x14
#define NV_PCI_DVSEC_GPU_PDI_LOW_OFFSET     0xc
#define NV_PCI_DVSEC_GPU_PDI_HIGH_OFFSET    0x10

#define NV_CXL_REGLOC_RBI_COMPONENT 0x1

#define NV_CXL_CM_OFFSET 0x1000
#define NV_CXL_CM_CAP_HDR_OFFSET 0x0
#define NV_CXL_CM_CAP_HDR_ID_MASK GENMASK(15, 0)
#define NV_CM_CAP_HDR_CAP_ID 1
#define NV_CXL_CM_CAP_HDR_ARRAY_SIZE_MASK GENMASK(31, 24)
#define NV_CXL_CM_CAP_PTR_MASK GENMASK(31, 20)
#define NV_CXL_CM_CAP_CAP_ID_HDM 0x5
#define NV_CXL_HDM_DECODER_CAP_OFFSET 0x0
#define NV_CXL_HDM_DECODER0_BASE_LOW_OFFSET(i) (0x20 * (i) + 0x10)
#define NV_CXL_HDM_DECODER0_BASE_HIGH_OFFSET(i) (0x20 * (i) + 0x14)
#define NV_CXL_HDM_DECODER0_SIZE_LOW_OFFSET(i) (0x20 * (i) + 0x18)
#define NV_CXL_HDM_DECODER0_SIZE_HIGH_OFFSET(i) (0x20 * (i) + 0x1c)
#define NV_CXL_HDM_DECODER0_CTRL_OFFSET(i) (0x20 * (i) + 0x20)
#define NV_CXL_HDM_DECODER0_CTRL_COMMITTED BIT(10)

/* CXL r4.0, 8.1.8: Flex Bus DVSEC */
#ifndef PCI_DVSEC_CXL_FLEXBUS_PORT
#define PCI_DVSEC_CXL_FLEXBUS_PORT			         7
#endif
#ifndef PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS
#define  PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS		     0xE
#endif
#ifndef PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS_CACHE
#define   PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS_CACHE    BIT(0)
#endif
#ifndef PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS_MEM
#define   PCI_DVSEC_CXL_FLEXBUS_PORT_STATUS_MEM      BIT(2)
#endif

/* CXL r4.0, 8.1.9: Register Locator DVSEC */
#ifndef PCI_DVSEC_CXL_REG_LOCATOR
#define PCI_DVSEC_CXL_REG_LOCATOR                   8
#endif
#ifndef PCI_DVSEC_CXL_REG_LOCATOR_BLOCK1
#define  PCI_DVSEC_CXL_REG_LOCATOR_BLOCK1           0xC
#endif
#ifndef PCI_DVSEC_CXL_REG_LOCATOR_BIR
#define   PCI_DVSEC_CXL_REG_LOCATOR_BIR		        GENMASK(2, 0)
#endif
#ifndef PCI_DVSEC_CXL_REG_LOCATOR_BLOCK_ID
#define   PCI_DVSEC_CXL_REG_LOCATOR_BLOCK_ID		GENMASK(15, 8)
#endif
#ifndef PCI_DVSEC_CXL_REG_LOCATOR_BLOCK_OFF_LOW
#define   PCI_DVSEC_CXL_REG_LOCATOR_BLOCK_OFF_LOW	GENMASK(31, 16)
#endif

#ifndef PCI_VENDOR_ID_CXL
#define PCI_VENDOR_ID_CXL                           0x1e98
#endif

#endif /* _NV_PCI_H_ */
