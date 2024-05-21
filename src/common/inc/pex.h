/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef PEX_H
#define PEX_H

#define PEX_REVISION_ID            0x00000002

// subfunction 0 is common use: NV_ACPI_ALL_FUNC_SUPPORT
// #define PEX_FUNC_GETSUPPORTFUNCTION      0x00000000  // Get supported function
#define PEX_FUNC_GETSLOTINFO             0x00000001  // Get PCI Express Slot Information
#define PEX_FUNC_GETSLOTNUMBER           0x00000002  // Get PCI Express Slot Number
#define PEX_FUNC_GETVENDORTOKENID        0x00000003  // Get PCI Express Vendor Specific Token ID strings
#define PEX_FUNC_GETPCIBUSCAPS           0x00000004  // Get PCI Express Root Bus Capabilities
#define PEX_FUNC_IGNOREPCIBOOTCONFIG     0x00000005  // Indication to OS that PCI Boot config can be ignored
#define PEX_FUNC_GETLTRLATENCY           0x00000006  // Get PCI Express Latency Tolerance Reporting Info
#define PEX_FUNC_NAMEPCIDEVICE           0x00000007  // Get name of PCI or PCIE device 
#define PEX_FUNC_SETLTRLATENCY           0x00000008  // Set PCI Express Latency Tolerance Reporting Values
#define PEX_FUNC_AUXPOWERLIMIT           0x0000000A  // Set Aux power limit
#define PEX_FUNC_PEXRST_DELAY            0x0000000B  // Set Pex reset delay


/*
 * 0h: Denied.
 *     Indicates that the platform cannot support the power requested.
 * 1h: Granted.
 *     Indicates that the device is permitted to draw the requested auxiliary power.
 * 2h: Granted.
 *     Indicates that the platform will not remove main power from the slot
 *     while the system is in S0.
 */
#define NV_AUX_POWER_REQUEST_STATUS                                  1:0
#define NV_AUX_POWER_REQUEST_STATUS_DENIED                          0x00
#define NV_AUX_POWER_REQUEST_STATUS_GRANTED_WITHOUT_12V_POWER       0x01
#define NV_AUX_POWER_REQUEST_STATUS_GRANTED_WITH_12V_POWER          0x02

/*
 * Retry, with interval.
 *     Bit 4, is a status bit. If set, it indicates that the platform cannot support
 *     the power requested at this time, but it may be able to do so in the future.
 *     Bits 3:0 contains the waiting time, in seconds, after which request can be made again.
 */
#define NV_AUX_POWER_REQUEST_STATUS_RETRY_LATER                      4:0
#define NV_AUX_POWER_REQUEST_STATUS_RETRY_LATER_STATUS               4:4
#define NV_AUX_POWER_REQUEST_STATUS_RETRY_LATER_STATUS_FALSE         0x0
#define NV_AUX_POWER_REQUEST_STATUS_RETRY_LATER_STATUS_TRUE          0x1
#define NV_AUX_POWER_REQUEST_STATUS_RETRY_LATER_INTERVAL             3:0
#define NV_AUX_POWER_REQUEST_STATUS_RETRY_LATER_INTERVAL_MIN         0x1
#define NV_AUX_POWER_REQUEST_STATUS_RETRY_LATER_INTERVAL_MAX         0xF

#endif // PEX_H
