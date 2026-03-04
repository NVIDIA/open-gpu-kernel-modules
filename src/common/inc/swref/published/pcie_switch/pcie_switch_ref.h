/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef PCIE_SWITCH_REF_H
#define PCIE_SWITCH_REF_H
#include "published/br03/dev_br03_xvd.h"
#include "published/br03/dev_br03_xvu.h"
#include "published/br04/br04_ref.h"
#include "nvdevid.h"

//
// This file has the vendor and device IDs of all supported PCIe switches
// and any new PCIe switch can be added to this allowlist.
//

#define IS_SUPPORTED_PCIE_SWITCH(vendorId, deviceId)                                        \
            (((vendorId == PCI_VENDOR_ID_NVIDIA) && IS_DEVID_SUPPORTED_NVIDIA(deviceId)) || \
             ((vendorId == PCI_VENDOR_ID_PLX) && IS_DEVID_SUPPORTED_PLX(deviceId)) ||       \
             ((vendorId == PCI_VENDOR_ID_PMC) && IS_DEVID_SUPPORTED_PMC(deviceId)) ||       \
             ((vendorId == PCI_VENDOR_ID_MELLANOX) && IS_DEVID_SUPPORTED_MELLANOX(deviceId)))

// Accept NVIDIA devices BR03, BR04
#define PCI_VENDOR_ID_NVIDIA                0x10DE
#define IS_DEVID_SUPPORTED_NVIDIA(devId)                                \
                        (IS_DEVID_BR04(devId) ||                        \
                         (devId == NV_BR03_XVU_DEV_ID_DEVICE_ID_BR03))

//
// Accept all 87xx and 97xx PLX bridges as supported - these are all
// currently Gen3 peer-to-peer-capable switches.
//
#define PCI_VENDOR_ID_PLX                   0x10B5
#define IS_DEVID_SUPPORTED_PLX(devId)                       \
                        (((devId & 0xFF00) == 0x8700) ||    \
                         ((devId & 0xFF00) == 0x9700))

// Accept all 85xx and 40xx PMC-Sierra bridges as supported
#define PCI_VENDOR_ID_PMC                   0x11F8
#define IS_DEVID_SUPPORTED_PMC(devId)                       \
                        (((devId & 0xFF00) == 0x8500) ||    \
                         ((devId & 0xFF00) == 0x4000))

// Accept Mellanox CX6 PCIe bridge 0x1976 as supported for A16
#define IS_DEVID_SUPPORTED_MELLANOX(devId) (devId == 0x1976)

#endif // PCIE_SWITCH_REF_H
