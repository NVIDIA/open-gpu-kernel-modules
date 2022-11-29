/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_NB_REGS_H_
#define _NV_NB_REGS_H_

#include "nvdevid.h"

typedef struct
{
    NvU32 subsystem_vendor_id;
    NvU32 subsystem_device_id;
    NvU16 gpu_device_id;
} nv_nb_id_t;

typedef struct
{
    NvU32       vendor_id;
    const char *name;
    NvU32       data;
} nv_nb_reg_t;

/*
 * nb_id_table contains the OEM vendor ID, the subsystem ID and the
 * GPU device ID of the notebooks for which we need to enable
 * vendor specific registry keys. nb_reg_table contains the vendor
 * specific registry key values. The initVendorSpecificRegistry()
 * function compares the present notebooks OEM subsystem ID and the
 * GPU device ID with the values present in id_tables. If a match
 * is found, initVendorSpecificRegistry() extracts the vendor
 * ID and sets any associated registry key listed in nb_reg_table.
 */

static nv_nb_id_t nb_id_table[] = {
    { PCI_VENDOR_ID_PC_PARTNER, 0x0620, 0x1284 }, // Acer GT 630
    { PCI_VENDOR_ID_PC_PARTNER, 0x0620, 0x124b }, // Acer GT 640
    { 0,                      0,      0      }
};

static nv_nb_reg_t nb_reg_table[] = {
    { PCI_VENDOR_ID_PC_PARTNER, "RmOverrideSupportChipsetAspm", 2   },
    { 0,                      NULL,                      0   }
};

#endif //_NV_NB_REGS_H_
