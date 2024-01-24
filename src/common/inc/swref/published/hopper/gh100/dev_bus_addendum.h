/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef gh100_dev_nv_bus_addendum_h
#define gh100_dev_nv_bus_addendum_h

/*!
 * @defgroup    FRTS_INSECURE_SCRATCH_REGISTERS
 *
 * Used to communicate the location/size of insecure FRTS
 *
 * @{
 */
#define NV_PBUS_SW_FRTS_INSECURE_ADDR_LO32                          NV_PBUS_SW_SCRATCH(0x3D)

#define NV_PBUS_SW_FRTS_INSECURE_ADDR_HI32                          NV_PBUS_SW_SCRATCH(0x3E)

#define NV_PBUS_SW_FRTS_INSECURE_CONFIG                             NV_PBUS_SW_SCRATCH(0x3F)
#define NV_PBUS_SW_FRTS_INSECURE_CONFIG_SIZE_4K                     15U:0U
#define NV_PBUS_SW_FRTS_INSECURE_CONFIG_SIZE_4K_INVALID             0x0000
#define NV_PBUS_SW_FRTS_INSECURE_CONFIG_SIZE_4K_SHIFT               12U
#define NV_PBUS_SW_FRTS_INSECURE_CONFIG_MEDIA_TYPE                  16U:16U
#define NV_PBUS_SW_FRTS_INSECURE_CONFIG_MEDIA_TYPE_FB               0U
#define NV_PBUS_SW_FRTS_INSECURE_CONFIG_MEDIA_TYPE_SYSMEM           1U
/*!@}*/

#endif // gh100_dev_nv_bus_addendum_h
