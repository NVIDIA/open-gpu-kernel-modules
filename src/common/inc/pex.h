/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#endif // PEX_H
