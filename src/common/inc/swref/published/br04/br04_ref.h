/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef BR04_REF_H
#define BR04_REF_H

#include "published/br04/dev_br04_xvd.h"
#include "published/br04/dev_br04_xvu.h"

// BR04 can be accessed through a window in GPU register space
#define NV_BR04(i)                      (0x00018000 + (i * 0x00004000))
// Config space access for downstream ports begins at 8Ki, a port every 2Ki
#define NV_BR04_XVD_OFFSET(i)           ((1<<13) + (1<<11)*i)
// Size of total config space and for upstream, downstream ports.
#define NV_BR04_CONFIG_SIZE             (1 << 14)
#define NV_BR04_XVU_CONFIG_SIZE         (1 << 12)
#define NV_BR04_XVD_CONFIG_SIZE         (1 << 11)

// This enumeration is in the order of the _BUFSIZE_ registers; the FLOW_CTL
// registers are in a different order.
typedef enum {
    BR04_PORT_UP0 = 0,
    BR04_PORT_DP0,
    BR04_PORT_DP1,
    BR04_PORT_DP2,
    BR04_PORT_DP3,
    BR04_PORT_MH0,
    NUM_BR04_PORTS
} BR04_PORT;

// Is this a BR04 devid or not
// Based on assumption that XVD and XVU has same deviceID
#define IS_DEVID_BR04(i)                ((i >> 4) == (NV_BR04_XVU_DEV_ID_DEVICE_ID_BR04_0 >> 4))

// Phantom address to use for HGPU P2P transfers
#define HGPU_P2P_PHANTOM_BASE 0xf0f0f0f000000000LL

#endif // BR04_REF_H
