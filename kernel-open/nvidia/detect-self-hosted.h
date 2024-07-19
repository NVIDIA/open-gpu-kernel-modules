/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __DETECT_SELF_HOSTED_H__
#define __DETECT_SELF_HOSTED_H__

// PCI devIds 0x2340-0x237f are for Self-Hosted Hopper
static inline int pci_devid_is_self_hosted_hopper(unsigned short devid)
{
    return devid >= 0x2340 && devid <= 0x237f;
}

// PCI devIds 0x2940-0x297f are for Self-Hosted Blackwell
static inline int pci_devid_is_self_hosted_blackwell(unsigned short devid)
{
    return devid >= 0x2940 && devid <= 0x297f;
}

static inline int pci_devid_is_self_hosted(unsigned short devid)
{
    return pci_devid_is_self_hosted_hopper(devid) ||
           pci_devid_is_self_hosted_blackwell(devid);
}

#endif
