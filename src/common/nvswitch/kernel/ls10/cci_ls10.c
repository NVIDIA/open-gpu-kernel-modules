/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "error_nvswitch.h"
#include "export_nvswitch.h"
#include "soe/soe_nvswitch.h"
#include "ls10/soe_ls10.h"
#include "soe/soeifcci.h"
#include "cci/cci_nvswitch.h"
#include "cci/cci_priv_nvswitch.h"
#include "nvswitch/ls10/dev_pmgr.h"
#include "boards_nvswitch.h"
#include "regkey_nvswitch.h"

#include "ls10/cci_ls10.h"

#define CCI_LINK_TRAINING_POLLING_RATE_HZ     1
#define CCI_CDB_PROCESSING_POLLING_RATE_HZ    50

#define OSFP_LANE_MASK_LS10(lane0, lane1) \
    (NVBIT(lane0) | NVBIT(lane1))

#define MUX_I2C_ADDR_PCA9847                 (0x71 << 1)
#define MODULE_ID_TO_MUX_CHANNEL_PCA9847(id) (id | 0x8)

//
// E4840 board
//
NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE nvswitch_i2c_device_list_e4840[] =
{
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
};

const NvU32 nvswitch_i2c_device_list_e4840_size =
    NV_ARRAY_ELEMENTS(nvswitch_i2c_device_list_e4840);

//
// Vulcan board
//
NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE nvswitch_i2c_device_list_vulcan[] =
{
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
};

const NvU32 nvswitch_i2c_device_list_vulcan_size =
    NV_ARRAY_ELEMENTS(nvswitch_i2c_device_list_vulcan);

//
// Kong board
//
NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE nvswitch_i2c_device_list_kong[] =
{
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CC, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
};

const NvU32 nvswitch_i2c_device_list_kong_size =
    NV_ARRAY_ELEMENTS(nvswitch_i2c_device_list_kong);

//
// Goldstone board
//
NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE nvswitch_i2c_device_list_goldstone[] =
{
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
    NVSWITCH_DESCRIBE_I2C_DEVICE(_I2CB, 0xA0, _CMIS4_MODULE,
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL, _PUBLIC) |
        DRF_DEF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL, _PUBLIC)),
};

const NvU32 nvswitch_i2c_device_list_goldstone_size =
    NV_ARRAY_ELEMENTS(nvswitch_i2c_device_list_goldstone);

/*
 * Mapping between osfp, linkId, and osfp-lane-mask for E4840.
 *
 */
NVSWITCH_CCI_MODULE_LINK_LANE_MAP nvswitch_cci_osfp_map_e4840[] =
{
    { 0, 36, OSFP_LANE_MASK_LS10(0, 1) },
    { 0, 37, OSFP_LANE_MASK_LS10(2, 3) },
    { 0, 38, OSFP_LANE_MASK_LS10(4, 5) },
    { 0, 39, OSFP_LANE_MASK_LS10(6, 7) },
    { 1, 40, OSFP_LANE_MASK_LS10(0, 1) },
    { 1, 41, OSFP_LANE_MASK_LS10(2, 3) },
    { 1, 42, OSFP_LANE_MASK_LS10(4, 5) },
    { 1, 43, OSFP_LANE_MASK_LS10(6, 7) },
    { 2, 44, OSFP_LANE_MASK_LS10(0, 1) },
    { 2, 45, OSFP_LANE_MASK_LS10(2, 3) },
    { 2, 46, OSFP_LANE_MASK_LS10(4, 5) },
    { 2, 47, OSFP_LANE_MASK_LS10(6, 7) },
    { 3, 48, OSFP_LANE_MASK_LS10(0, 1) },
    { 3, 49, OSFP_LANE_MASK_LS10(2, 3) },
    { 3, 50, OSFP_LANE_MASK_LS10(4, 5) },
    { 3, 51, OSFP_LANE_MASK_LS10(6, 7) },
    { 4, 52, OSFP_LANE_MASK_LS10(0, 1) },
    { 4, 53, OSFP_LANE_MASK_LS10(2, 3) },
    { 4, 54, OSFP_LANE_MASK_LS10(4, 5) },
    { 4, 55, OSFP_LANE_MASK_LS10(6, 7) },
    { 5, 56, OSFP_LANE_MASK_LS10(0, 1) },
    { 5, 57, OSFP_LANE_MASK_LS10(2, 3) },
    { 5, 58, OSFP_LANE_MASK_LS10(4, 5) },
    { 5, 59, OSFP_LANE_MASK_LS10(6, 7) },
    { 6, 60, OSFP_LANE_MASK_LS10(0, 1) },
    { 6, 61, OSFP_LANE_MASK_LS10(2, 3) },
    { 6, 62, OSFP_LANE_MASK_LS10(4, 5) },
    { 6, 63, OSFP_LANE_MASK_LS10(6, 7) },
};

const NvU32 nvswitch_cci_osfp_map_e4840_size =
    NV_ARRAY_ELEMENTS(nvswitch_cci_osfp_map_e4840);

/*
 * Mapping between osfp, linkId, and osfp-lane-mask for Vulcan.
 *
 */
NVSWITCH_CCI_MODULE_LINK_LANE_MAP nvswitch_cci_osfp_map_vulcan[] =
{
    { 0, 10, OSFP_LANE_MASK_LS10(0, 1) },
    { 0, 11, OSFP_LANE_MASK_LS10(2, 3) },
    { 0, 8,  OSFP_LANE_MASK_LS10(4, 5) },
    { 0, 9,  OSFP_LANE_MASK_LS10(6, 7) },
    { 1, 14, OSFP_LANE_MASK_LS10(0, 1) },
    { 1, 15, OSFP_LANE_MASK_LS10(2, 3) },
    { 1, 12, OSFP_LANE_MASK_LS10(4, 5) },
    { 1, 13, OSFP_LANE_MASK_LS10(6, 7) },
    { 2, 28, OSFP_LANE_MASK_LS10(0, 1) },
    { 2, 29, OSFP_LANE_MASK_LS10(2, 3) },
    { 2, 30, OSFP_LANE_MASK_LS10(4, 5) },
    { 2, 31, OSFP_LANE_MASK_LS10(6, 7) },
    { 3, 24, OSFP_LANE_MASK_LS10(0, 1) },
    { 3, 25, OSFP_LANE_MASK_LS10(2, 3) },
    { 3, 26, OSFP_LANE_MASK_LS10(4, 5) },
    { 3, 27, OSFP_LANE_MASK_LS10(6, 7) },
    { 4, 20, OSFP_LANE_MASK_LS10(0, 1) },
    { 4, 21, OSFP_LANE_MASK_LS10(2, 3) },
    { 4, 22, OSFP_LANE_MASK_LS10(4, 5) },
    { 4, 23, OSFP_LANE_MASK_LS10(6, 7) },
};

const NvU32 nvswitch_cci_osfp_map_vulcan_size =
    NV_ARRAY_ELEMENTS(nvswitch_cci_osfp_map_vulcan);

/*
 * Mapping between osfp, linkId, and osfp-lane-mask for Kong.
 *
 */
NVSWITCH_CCI_MODULE_LINK_LANE_MAP nvswitch_cci_osfp_map_kong[] =
{
    { 0,  50, OSFP_LANE_MASK_LS10(0, 1) },
    { 0,  49, OSFP_LANE_MASK_LS10(2, 3) },
    { 0,  51, OSFP_LANE_MASK_LS10(4, 5) },
    { 0,  48, OSFP_LANE_MASK_LS10(6, 7) },
    { 1,  59, OSFP_LANE_MASK_LS10(0, 1) },
    { 1,  56, OSFP_LANE_MASK_LS10(2, 3) },
    { 1,  58, OSFP_LANE_MASK_LS10(4, 5) },
    { 1,  57, OSFP_LANE_MASK_LS10(6, 7) },
    { 2,  54, OSFP_LANE_MASK_LS10(0, 1) },
    { 2,  53, OSFP_LANE_MASK_LS10(2, 3) },
    { 2,  55, OSFP_LANE_MASK_LS10(4, 5) },
    { 2,  52, OSFP_LANE_MASK_LS10(6, 7) },
    { 3,  63, OSFP_LANE_MASK_LS10(0, 1) },
    { 3,  60, OSFP_LANE_MASK_LS10(2, 3) },
    { 3,  62, OSFP_LANE_MASK_LS10(4, 5) },
    { 3,  61, OSFP_LANE_MASK_LS10(6, 7) },
    { 4,  47, OSFP_LANE_MASK_LS10(0, 1) },
    { 4,  44, OSFP_LANE_MASK_LS10(2, 3) },
    { 4,  46, OSFP_LANE_MASK_LS10(4, 5) },
    { 4,  45, OSFP_LANE_MASK_LS10(6, 7) },
    { 5,  38, OSFP_LANE_MASK_LS10(0, 1) },
    { 5,  37, OSFP_LANE_MASK_LS10(2, 3) },
    { 5,  39, OSFP_LANE_MASK_LS10(4, 5) },
    { 5,  36, OSFP_LANE_MASK_LS10(6, 7) },
    { 6,  43, OSFP_LANE_MASK_LS10(0, 1) },
    { 6,  40, OSFP_LANE_MASK_LS10(2, 3) },
    { 6,  42, OSFP_LANE_MASK_LS10(4, 5) },
    { 6,  41, OSFP_LANE_MASK_LS10(6, 7) },
    { 7,  34, OSFP_LANE_MASK_LS10(0, 1) },
    { 7,  33, OSFP_LANE_MASK_LS10(2, 3) },
    { 7,  35, OSFP_LANE_MASK_LS10(4, 5) },
    { 7,  32, OSFP_LANE_MASK_LS10(6, 7) },
    { 8,   2, OSFP_LANE_MASK_LS10(0, 1) },
    { 8,   1, OSFP_LANE_MASK_LS10(2, 3) },
    { 8,   3, OSFP_LANE_MASK_LS10(4, 5) },
    { 8,   0, OSFP_LANE_MASK_LS10(6, 7) },
    { 9,  11, OSFP_LANE_MASK_LS10(0, 1) },
    { 9,   8, OSFP_LANE_MASK_LS10(2, 3) },
    { 9,  10, OSFP_LANE_MASK_LS10(4, 5) },
    { 9,   9, OSFP_LANE_MASK_LS10(6, 7) },
    { 10,  6, OSFP_LANE_MASK_LS10(0, 1) },
    { 10,  5, OSFP_LANE_MASK_LS10(2, 3) },
    { 10,  7, OSFP_LANE_MASK_LS10(4, 5) },
    { 10,  4, OSFP_LANE_MASK_LS10(6, 7) },
    { 11, 15, OSFP_LANE_MASK_LS10(0, 1) },
    { 11, 12, OSFP_LANE_MASK_LS10(2, 3) },
    { 11, 14, OSFP_LANE_MASK_LS10(4, 5) },
    { 11, 13, OSFP_LANE_MASK_LS10(6, 7) },
    { 12, 31, OSFP_LANE_MASK_LS10(0, 1) },
    { 12, 28, OSFP_LANE_MASK_LS10(2, 3) },
    { 12, 30, OSFP_LANE_MASK_LS10(4, 5) },
    { 12, 29, OSFP_LANE_MASK_LS10(6, 7) },
    { 13, 22, OSFP_LANE_MASK_LS10(0, 1) },
    { 13, 21, OSFP_LANE_MASK_LS10(2, 3) },
    { 13, 23, OSFP_LANE_MASK_LS10(4, 5) },
    { 13, 20, OSFP_LANE_MASK_LS10(6, 7) },
    { 14, 27, OSFP_LANE_MASK_LS10(0, 1) },
    { 14, 24, OSFP_LANE_MASK_LS10(2, 3) },
    { 14, 26, OSFP_LANE_MASK_LS10(4, 5) },
    { 14, 25, OSFP_LANE_MASK_LS10(6, 7) },
    { 15, 18, OSFP_LANE_MASK_LS10(0, 1) },
    { 15, 17, OSFP_LANE_MASK_LS10(2, 3) },
    { 15, 19, OSFP_LANE_MASK_LS10(4, 5) },
    { 15, 16, OSFP_LANE_MASK_LS10(6, 7) },
};

const NvU32 nvswitch_cci_osfp_map_kong_size =
    NV_ARRAY_ELEMENTS(nvswitch_cci_osfp_map_kong);

/*
 * Mapping between osfp, linkId, and osfp-lane-mask for Goldstone.
 *
 */
NVSWITCH_CCI_MODULE_LINK_LANE_MAP nvswitch_cci_osfp_map_goldstone[] =
{
    { 0,  50, OSFP_LANE_MASK_LS10(0, 1) },
    { 0,  49, OSFP_LANE_MASK_LS10(2, 3) },
    { 0,  51, OSFP_LANE_MASK_LS10(4, 5) },
    { 0,  48, OSFP_LANE_MASK_LS10(6, 7) },
    { 1,  54, OSFP_LANE_MASK_LS10(0, 1) },
    { 1,  53, OSFP_LANE_MASK_LS10(2, 3) },
    { 1,  55, OSFP_LANE_MASK_LS10(4, 5) },
    { 1,  52, OSFP_LANE_MASK_LS10(6, 7) },
    { 2,  58, OSFP_LANE_MASK_LS10(0, 1) },
    { 2,  57, OSFP_LANE_MASK_LS10(2, 3) },
    { 2,  59, OSFP_LANE_MASK_LS10(4, 5) },
    { 2,  56, OSFP_LANE_MASK_LS10(6, 7) },
    { 3,  62, OSFP_LANE_MASK_LS10(0, 1) },
    { 3,  61, OSFP_LANE_MASK_LS10(2, 3) },
    { 3,  63, OSFP_LANE_MASK_LS10(4, 5) },
    { 3,  60, OSFP_LANE_MASK_LS10(6, 7) },
    { 4,  47, OSFP_LANE_MASK_LS10(0, 1) },
    { 4,  44, OSFP_LANE_MASK_LS10(2, 3) },
    { 4,  46, OSFP_LANE_MASK_LS10(4, 5) },
    { 4,  45, OSFP_LANE_MASK_LS10(6, 7) },
    { 5,  43, OSFP_LANE_MASK_LS10(0, 1) },
    { 5,  40, OSFP_LANE_MASK_LS10(2, 3) },
    { 5,  42, OSFP_LANE_MASK_LS10(4, 5) },
    { 5,  41, OSFP_LANE_MASK_LS10(6, 7) },
    { 6,  39, OSFP_LANE_MASK_LS10(0, 1) },
    { 6,  36, OSFP_LANE_MASK_LS10(2, 3) },
    { 6,  38, OSFP_LANE_MASK_LS10(4, 5) },
    { 6,  37, OSFP_LANE_MASK_LS10(6, 7) },
    { 7,  35, OSFP_LANE_MASK_LS10(0, 1) },
    { 7,  32, OSFP_LANE_MASK_LS10(2, 3) },
    { 7,  34, OSFP_LANE_MASK_LS10(4, 5) },
    { 7,  33, OSFP_LANE_MASK_LS10(6, 7) },
};

const NvU32 nvswitch_cci_osfp_map_goldstone_size =
    NV_ARRAY_ELEMENTS(nvswitch_cci_osfp_map_goldstone);

NvBool
nvswitch_is_cci_supported_ls10
(
    nvswitch_device *device
)
{
    if (FLD_TEST_DRF(_SWITCH_REGKEY, _CCI_CONTROL, _ENABLE, _FALSE,
            device->regkeys.cci_control))
    {
        NVSWITCH_PRINT(device, INFO, "CCI is disabled via regkey.\n");
        return NV_FALSE;
    }

    //
    // Currently, device needs to be initialized before
    // board ID can be obtained from bios
    //
    if (NVSWITCH_IS_DEVICE_INITIALIZED(device) &&
        !cciSupported(device))
    {
        NVSWITCH_PRINT(device, INFO,
                      "%s: CCI is not supported on current board.\n",
                       __FUNCTION__);
        return NV_FALSE;
    }

    return NV_TRUE;
}

void 
nvswitch_cci_setup_gpio_pins_ls10
(
    nvswitch_device *device
)
{
    NvU32 intrCtrl;

    // Disable Ports CPLD interrupts since they are unused
    intrCtrl = NVSWITCH_REG_RD32(device, _GPIO_RM_INTR_MSK, _GPIO_LIST_1);
    intrCtrl = FLD_SET_DRF(_GPIO, _RM_INTR_MSK_GPIO_LIST_1, _GPIO15_RISING, _DISABLED, intrCtrl);
    intrCtrl = FLD_SET_DRF(_GPIO, _RM_INTR_MSK_GPIO_LIST_1, _GPIO15_FALLING, _DISABLED, intrCtrl);
    NVSWITCH_REG_WR32(device, _GPIO_RM_INTR_MSK, _GPIO_LIST_1, intrCtrl);
}

static void 
_nvswitch_cci_optimize_link_ls10
(
    nvswitch_device *device,
    NvU32 linkId
)
{
    NvBool    freeze_maintenance;
    NvBool    restart_training;
    NvBool    nvlink_mode;
    
    freeze_maintenance = NV_FALSE;
    restart_training = NV_FALSE;
    nvlink_mode = NV_TRUE;

    NVSWITCH_PRINT(device, INFO, "%s Recieved optimize link command for link %d\n", __FUNCTION__, linkId);

    // Optimize Tx & Rx (FM = RT = 0 for OptimizeLink command)
    cciConfigureNvlinkModeAsync(device, NVSWITCH_I2C_ACQUIRER_CCI_TRAIN, linkId,
                                freeze_maintenance, restart_training, nvlink_mode);
}

/*!
 * @brief Callback function to recieve CCI messages from SOE.
 */
void
nvswitch_cci_soe_callback_ls10
(
    nvswitch_device *device,
    RM_FLCN_MSG     *pGenMsg,
    void            *pParams,
    NvU32            seqDesc,
    NV_STATUS        status
)
{
    RM_FLCN_MSG_SOE                       *pMsg = (RM_FLCN_MSG_SOE*)pGenMsg;
    RM_SOE_CCI_MSG_ALI_OPTIMIZE_LINK_INFO  aliLinkInfo;
    
    switch (pMsg->msg.cci.msgType)
    {
        case RM_SOE_CCI_MSG_ID_ALI_OPTIMIZE_LINK_INFO:
        {
            aliLinkInfo = pMsg->msg.cci.linkInfo;

            _nvswitch_cci_optimize_link_ls10(device, aliLinkInfo.linkId);
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR, "%s Unknown message ID 0x%x\n", __FUNCTION__, pMsg->msg.cci.msgType);
            NVSWITCH_ASSERT(0);
        }
    }
}

NvlStatus
nvswitch_cci_get_cci_link_mode_ls10
(
    nvswitch_device *device,
    NvU32 linkNumber,
    NvU64 *mode
)
{
    return cciGetLinkMode(device, linkNumber, mode);
}

static void
_nvswitch_update_cages_mask_ls10
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;
    NvU32 idx_i2cdevice;
    NvU32 new_cages;

    new_cages = 0;

    //
    // osfp modules will be enumerated based on the order they are listed in
    // the CMIS modules device list
    // Ex. nvswitch_i2c_device_list_vulcan
    //
    for(idx_i2cdevice = 0; idx_i2cdevice < pCci->osfp_num; idx_i2cdevice++)
    {
        new_cages |= NVBIT(idx_i2cdevice);
    }

    device->pCci->cagesMask |= new_cages;
}

// TODO: should be bios driven
static void
_nvswitch_cci_setup_vulcan_config_ls10
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;
    NvU32 nvswitchNum;

    nvswitchNum = nvswitch_read_physical_id(device);

    pCci->osfp_i2c_info = nvswitch_i2c_device_list_vulcan;
    pCci->osfp_map = nvswitch_cci_osfp_map_vulcan; 

    if (nvswitchNum == 0 || nvswitchNum == 3)
    {
        pCci->osfp_num = (nvswitch_i2c_device_list_vulcan_size - 1);
        pCci->osfp_map_size = (nvswitch_cci_osfp_map_vulcan_size - 4);
        pCci->numLinks = 16;
    }
    else
    {
        pCci->osfp_num = nvswitch_i2c_device_list_vulcan_size;
        pCci->osfp_map_size = nvswitch_cci_osfp_map_vulcan_size;
        pCci->numLinks = 20;
    }
}

/*
 * @brief Detect board
 *
 */
static NvlStatus
_nvswitch_cci_detect_board_ls10
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;
    NvlStatus retval = NVL_SUCCESS;
    NvU16 boardId;

    retval = nvswitch_get_board_id(device, &boardId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s Failed to get board ID. rc:%d\n",
                       __FUNCTION__, retval);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    switch (boardId)
    {
        case NVSWITCH_BOARD_LS10_4840_0000_PC0:
        case NVSWITCH_BOARD_LS10_4840_0072_STA:
        {
            pCci->osfp_i2c_info = nvswitch_i2c_device_list_e4840; 
            pCci->osfp_num = nvswitch_i2c_device_list_e4840_size;
            pCci->osfp_map = nvswitch_cci_osfp_map_e4840;
            pCci->osfp_map_size = nvswitch_cci_osfp_map_e4840_size;
            pCci->numLinks = 28;
            _nvswitch_update_cages_mask_ls10(device);
            break;
        }
        case NVSWITCH_BOARD_LS10_5612_0012_895:
        {
            _nvswitch_cci_setup_vulcan_config_ls10(device);
            _nvswitch_update_cages_mask_ls10(device);
            break;
        }
        case NVSWITCH_BOARD_LS10_4697_0000_895:
        {
            pCci->osfp_i2c_info = nvswitch_i2c_device_list_kong; 
            pCci->osfp_num = nvswitch_i2c_device_list_kong_size;
            pCci->osfp_map = nvswitch_cci_osfp_map_kong;
            pCci->osfp_map_size = nvswitch_cci_osfp_map_kong_size;
            pCci->numLinks = 64;
            _nvswitch_update_cages_mask_ls10(device);
            break;
        }
        case NVSWITCH_BOARD_LS10_4262_0000_895:
        {
            pCci->osfp_i2c_info = nvswitch_i2c_device_list_goldstone; 
            pCci->osfp_num = nvswitch_i2c_device_list_goldstone_size;
            pCci->osfp_map = nvswitch_cci_osfp_map_goldstone;
            pCci->osfp_map_size = nvswitch_cci_osfp_map_goldstone_size;
            pCci->numLinks = 32;
            _nvswitch_update_cages_mask_ls10(device);
            break;
        }
        default:
        {
            return -NVL_ERR_NOT_SUPPORTED;
        }
    }

    return NVL_SUCCESS;
}

NvlStatus 
nvswitch_cci_discovery_ls10
(
    nvswitch_device *device
)
{
    NvU64 biosVersion;
    NvlStatus retval;

    retval = _nvswitch_cci_detect_board_ls10(device);
    if (retval != NVL_SUCCESS)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = nvswitch_lib_get_bios_version(device, &biosVersion);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s Get VBIOS version failed.\n",
                        __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (biosVersion < NVSWITCH_CCI_VBIOS_REV_LOCK_VERSION_LS10)
    {
        NVSWITCH_PRINT(device, ERROR, "%s CCI not supported on current VBIOS. Please update to version %s\n",
                        __FUNCTION__, NVSWITCH_CCI_VBIOS_REV_LOCK_VERSION_STRING_LS10);
        return -NVL_ERR_NOT_SUPPORTED;
    }
    
    return NVL_SUCCESS;
}

static void
_nvswitch_detect_presence_cci_devices_ls10
(
    nvswitch_device *device,
    NvU32 *pMaskPresent
)
{
    NvlStatus retval;
    NvU8 val;
    NvU32 maskPresent;

    maskPresent = 0;

    retval = nvswitch_cci_ports_cpld_read_ls10(device, CPLD_MACHXO3_PORTS_PRSNT_REG1, &val);
    if (retval == NVL_SUCCESS)
    {
        maskPresent |= val;
    }

    retval = nvswitch_cci_ports_cpld_read_ls10(device, CPLD_MACHXO3_PORTS_PRSNT_REG2, &val);
    if (retval == NVL_SUCCESS)
    {
        maskPresent |= (val << 8);
    }

    if (pMaskPresent != NULL)
    {
        *pMaskPresent = maskPresent;
    }
}

void 
nvswitch_cci_get_xcvrs_present_ls10
(
    nvswitch_device *device,
    NvU32 *pMaskPresent
)
{
    NvU32 maskPresent;

    maskPresent = 0;

    _nvswitch_detect_presence_cci_devices_ls10(device, &maskPresent);

    if (pMaskPresent != NULL)
    {
        *pMaskPresent = maskPresent;
    }
}

void
nvswitch_cci_set_xcvr_present_ls10
(
    nvswitch_device *device,
    NvU32 maskPresent
)
{
    device->pCci->osfpMaskPresent = maskPresent;
}

void 
nvswitch_cci_destroy_ls10
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;
    
    if (pCci != NULL && pCci->bInitialized)
    {
        cciModuleOnboardShutdown(device);
    }
}

static void
_nvswitch_detect_presence_change_cci_devices_ls10
(
    nvswitch_device *device,
    NvU32 *pMaskPresentChange
)
{
    NvlStatus retval;
    NvU8 val;
    NvU32 maskPresentChange;

    maskPresentChange = 0;

    retval = nvswitch_cci_ports_cpld_read(device, 
                                           CPLD_MACHXO3_PORTS_PRSNT_STATE_CHANGE_REG1,
                                           &val);
    if (retval == NVL_SUCCESS)
    {
        maskPresentChange |= val;

        if (val)
        {
            // Only clear presence change bits that were set
            nvswitch_cci_ports_cpld_write(device, 
                                        CPLD_MACHXO3_PORTS_PRSNT_STATE_CHANGE_REG1,
                                        ~val);
        }
    }

    retval = nvswitch_cci_ports_cpld_read(device, 
                                           CPLD_MACHXO3_PORTS_PRSNT_STATE_CHANGE_REG2,
                                           &val);
    if (retval == NVL_SUCCESS)
    {
        maskPresentChange |= (val << 8);

        if (val)
        {
            // Only clear presence change bits that were set
            nvswitch_cci_ports_cpld_write(device, 
                                        CPLD_MACHXO3_PORTS_PRSNT_STATE_CHANGE_REG2,
                                        ~val);
        }
    }

    if (pMaskPresentChange != NULL)
    {
        *pMaskPresentChange = maskPresentChange;
    }
}

//
// Detects if a hot plug event occured. This operation
// will clear the presence change status register
//
void
nvswitch_cci_get_xcvrs_present_change_ls10
(
    nvswitch_device *device,
    NvU32 *pMaskPresentChange
)
{
    NvU32 maskPresentChange;

    maskPresentChange = 0;

    _nvswitch_detect_presence_change_cci_devices_ls10(device, &maskPresentChange);
      
    if (pMaskPresentChange != NULL)
    {
        *pMaskPresentChange = maskPresentChange;
    }
}

#define CPLD_MACHXO3_I2C_ADDR (0xC6)

NvlStatus
nvswitch_cci_ports_cpld_read_ls10
(
    nvswitch_device *device,
    NvU8 reg,
    NvU8 *pData
)
{
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS     i2cIndexed = {0};
    NvlStatus                            status;

    i2cIndexed.port = NVSWITCH_I2C_PORT_I2CA;
    i2cIndexed.bIsRead = NV_TRUE;
    i2cIndexed.address = CPLD_MACHXO3_I2C_ADDR;
    i2cIndexed.flags =
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _START,        _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _RESTART,      _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _STOP,         _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _ADDRESS_MODE, _7BIT) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _HW)         |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _100KHZ) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _DISABLED) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _TRANSACTION_MODE, _NORMAL) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ONE);
    i2cIndexed.index[0] = reg;
    i2cIndexed.messageLength = 1;
    i2cIndexed.acquirer = NVSWITCH_I2C_ACQUIRER_CCI_INITIALIZE;

    status = nvswitch_ctrl_i2c_indexed(device, &i2cIndexed);

    if (status == NVL_SUCCESS)
    {
        *pData = i2cIndexed.message[0];
    }

    return status;
}

NvlStatus
nvswitch_cci_ports_cpld_write_ls10
(
    nvswitch_device *device,
    NvU8 reg,
    NvU8 data
)
{
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS     i2cIndexed = {0};
    NvlStatus                            status;

    i2cIndexed.port = NVSWITCH_I2C_PORT_I2CA;
    i2cIndexed.bIsRead = NV_FALSE;
    i2cIndexed.address = CPLD_MACHXO3_I2C_ADDR;
    i2cIndexed.flags =
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _START,        _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _RESTART,      _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _STOP,         _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _ADDRESS_MODE, _7BIT) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _HW)         |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _100KHZ) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _DISABLED) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _TRANSACTION_MODE, _NORMAL) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ONE);
    i2cIndexed.index[0] = reg;
    i2cIndexed.messageLength = 1;
    i2cIndexed.acquirer = NVSWITCH_I2C_ACQUIRER_CCI_INITIALIZE;

    i2cIndexed.message[0] = data;

    status = nvswitch_ctrl_i2c_indexed(device, &i2cIndexed);

    return status;
}

static NvlStatus
_nvswitch_cci_reset_mux_ls10
(
    nvswitch_device *device,
    NvU8 i2cPort
)
{
    NvlStatus retval;
    NvU8 regByte;

    retval = nvswitch_cci_ports_cpld_read_ls10(device, CPLD_MACHXO3_CPLD_CTL_REG1, &regByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Assert I2C Mux reset
    if (i2cPort == NVSWITCH_I2C_PORT_I2CB)
    {
        regByte = FLD_SET_REF_NUM(CPLD_MACHXO3_CPLD_CTL_REG1_I2CB_MUX_RESET, 1, regByte);
    }
    else
    {   
        regByte = FLD_SET_REF_NUM(CPLD_MACHXO3_CPLD_CTL_REG1_I2CC_MUX_RESET, 1, regByte);
    }

    retval = nvswitch_cci_ports_cpld_write_ls10(device, CPLD_MACHXO3_CPLD_CTL_REG1, regByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    nvswitch_os_sleep(1);

    // De-assert I2C Mux reset
    if (i2cPort == NVSWITCH_I2C_PORT_I2CB)
    {
        regByte = FLD_SET_REF_NUM(CPLD_MACHXO3_CPLD_CTL_REG1_I2CB_MUX_RESET, 0, regByte);
    }
    else
    {   
        regByte = FLD_SET_REF_NUM(CPLD_MACHXO3_CPLD_CTL_REG1_I2CC_MUX_RESET, 0, regByte);
    }
    
    retval = nvswitch_cci_ports_cpld_write_ls10(device, CPLD_MACHXO3_CPLD_CTL_REG1, regByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_cci_reset_ls10
(
    nvswitch_device *device
)
{
    NvlStatus retval;
    NvU8      regByte;

    // De-assert OSFP reset
    retval = nvswitch_cci_ports_cpld_write_ls10(device, CPLD_MACHXO3_PORTS_RESET_REG1, 0);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    retval = nvswitch_cci_ports_cpld_write_ls10(device, CPLD_MACHXO3_PORTS_RESET_REG2, 0);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Clear latch
    retval = nvswitch_cci_ports_cpld_write_ls10(device, CPLD_MACHXO3_CPLD_STATUS_REG, 0);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Setup control register
    retval = nvswitch_cci_ports_cpld_read_ls10(device, CPLD_MACHXO3_CPLD_CTL_REG1, &regByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Disable watchdog TODO: remove once watchdog implemented
    regByte = FLD_SET_REF_NUM(CPLD_MACHXO3_CPLD_CTL_REG1_WATCHDOG_ENABLE, 0, regByte);

    retval = nvswitch_cci_ports_cpld_write_ls10(device, CPLD_MACHXO3_CPLD_CTL_REG1, regByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }
    
    retval = _nvswitch_cci_reset_mux_ls10(device, NVSWITCH_I2C_PORT_I2CB);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    retval = _nvswitch_cci_reset_mux_ls10(device, NVSWITCH_I2C_PORT_I2CC);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    nvswitch_os_sleep(1000);

    return NVL_SUCCESS;
}

NvlStatus 
nvswitch_cci_reset_ls10
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;

    if ((pCci == NULL) || (!pCci->bSupported))
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: CCI not supported\n",
                __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return _nvswitch_cci_reset_ls10(device);
}

static NvU8
_cciGetXcvrNextLedStateLink_ls10
(
    nvswitch_device *device,
    NvU8            currentLedState,   
    NvU8            linkNum
)
{
    nvlink_link *link;
    NvU64 linkState;

    link = nvswitch_get_link(device, linkNum);

    if ((link == NULL) ||
        (device->hal.nvswitch_corelib_get_dl_link_mode(link, &linkState) != NVL_SUCCESS))
    {
        return CCI_LED_STATE_OFF;
    }

    switch (linkState)
    {
        case NVLINK_LINKSTATE_HS:
        case NVLINK_LINKSTATE_RECOVERY:
        {
            return CCI_LED_STATE_UP_WARM;
        }
        case NVLINK_LINKSTATE_FAULT:
        {
            return CCI_LED_STATE_FAULT;
        }
        default:
        {
            if (currentLedState == CCI_LED_STATE_INITIALIZE)
            {
                return CCI_LED_STATE_INITIALIZE;
            }
            return CCI_LED_STATE_OFF;
        }
    }
}

static NvU8
_cciResolveXcvrLedStates_ls10
(
    NvU8  ledState0,
    NvU8  ledState1
)
{
    // Used to resolve link state discrepancies between partner links
    ct_assert(CCI_LED_STATE_LOCATE < CCI_LED_STATE_FAULT);
    ct_assert(CCI_LED_STATE_FAULT < CCI_LED_STATE_OFF);
    ct_assert(CCI_LED_STATE_OFF < CCI_LED_STATE_INITIALIZE);
    ct_assert(CCI_LED_STATE_INITIALIZE < CCI_LED_STATE_UP_WARM);

    return (ledState0 < ledState1 ? ledState0 : ledState1);

}

static NvU8
_cciGetXcvrNextLedStateLinks_ls10
(
    nvswitch_device *device,
    NvU32            osfp,
    NvU8             currentLedState,   
    NvU64            linkMask
)
{
    NvU8  linkNum;
    NvU8  ledState;
    NvU8  nextLedState;

    nextLedState = CCI_NUM_LED_STATES;

    NVSWITCH_ASSERT(linkMask != 0);
    FOR_EACH_INDEX_IN_MASK(64, linkNum, linkMask)
    {
        ledState = _cciGetXcvrNextLedStateLink_ls10(device, currentLedState, linkNum);
        nextLedState = _cciResolveXcvrLedStates_ls10(nextLedState, ledState);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (nextLedState == CCI_LED_STATE_UP_WARM)
    {
        // Only tells us that one of the links has activity
        if (cciCheckXcvrForLinkTraffic(device, osfp, linkMask))
        {
            nextLedState = CCI_LED_STATE_UP_ACTIVE;
        }
    }

    return nextLedState;
}

/*
 * Determines LED states for both LED A and LED B based on link
 * and module state     
 */
static NvU8
_cciGetXcvrNextLedsState_ls10
(
    nvswitch_device *device,
    NvU32            osfp
)
{
    NvlStatus status;
    NvU64 linkMask;
    NvU64 linkMaskA;
    NvU64 linkMaskB;
    NvU8  linkNum;
    NvU8  laneMask;
    NvU8  ledsNextState;
    NvU8  currentLedsState;
    NvU8  currentLedAState;
    NvU8  currentLedBState;

    ledsNextState = 0;

    if (device->pCci->isFaulty[osfp] || cciModuleOnboardFailed(device, osfp))
    {
        ledsNextState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                                CCI_LED_STATE_FAULT,
                                ledsNextState);
        ledsNextState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                                CCI_LED_STATE_FAULT,
                                ledsNextState);
        return ledsNextState;
    }

    status = cciGetCageMapping(device, osfp, &linkMask, NULL);
    if (status != NVL_SUCCESS)
    {
        ledsNextState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                                CCI_LED_STATE_OFF,
                                ledsNextState);
        ledsNextState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                                CCI_LED_STATE_OFF,
                                ledsNextState);
        return ledsNextState;
    }

    linkMaskA = 0;
    linkMaskB = 0;

    // Filter link pairs between those corresponding to LED A and LED B
    FOR_EACH_INDEX_IN_MASK(64, linkNum, linkMask)
    {
        if (cciGetLaneMask(device, linkNum, &laneMask) != NVL_SUCCESS)
        {
            continue;
        }

        // Lanes 0-3 mapped to LED A, lanes 4-7 mapped to LED B
        if  (laneMask == OSFP_LANE_MASK_LS10(0, 1) ||
             laneMask == OSFP_LANE_MASK_LS10(2, 3))
        {
            linkMaskA |= NVBIT64(linkNum);
        }
        else
        {
            linkMaskB |= NVBIT64(linkNum);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Determine states for LED A and LED B separately
    currentLedsState = device->pCci->xcvrCurrentLedState[osfp];
    currentLedAState = REF_VAL(CCI_LED_STATE_LED_A,
                                currentLedsState);
    currentLedBState = REF_VAL(CCI_LED_STATE_LED_B,
                                currentLedsState);

    ledsNextState = 0;
    ledsNextState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                                    _cciGetXcvrNextLedStateLinks_ls10(device, osfp,
                                                                       currentLedAState,
                                                                       linkMaskA),
                                    ledsNextState);
    ledsNextState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                                    _cciGetXcvrNextLedStateLinks_ls10(device, osfp, 
                                                                       currentLedBState,
                                                                       linkMaskB), 
                                    ledsNextState);

    return ledsNextState;
}

static void
_nvswitch_cci_update_link_state_led_ls10
(
    nvswitch_device *device
)
{
    NvU32 cagesMask;
    NvU8  osfp;
    NvU8  currentLedsState;
    NvU8  currentLedAState;
    NvU8  currentLedBState;
    NvU8  nextLedsState;

    nextLedsState = 0;

    // LEDs are soldered to the carrier PCBA
    if (cciGetXcvrMask(device, &cagesMask, NULL) == NVL_SUCCESS)
    {
        // Loop over all cages and update leds
        FOR_EACH_INDEX_IN_MASK(32, osfp, cagesMask)
        {
            //
            // xcvrCurrentLedState[] is only updated when LED HW state is set
            // currentLedsState represents the state of 2 LEDs for LS10 systems
            //
            currentLedsState = device->pCci->xcvrCurrentLedState[osfp];

            if (cciModulePresent(device, osfp))
            {
                currentLedAState = REF_VAL(CCI_LED_STATE_LED_A,
                                        currentLedsState);
                currentLedBState = REF_VAL(CCI_LED_STATE_LED_B,
                                        currentLedsState);

                nextLedsState = _cciGetXcvrNextLedsState_ls10(device, osfp);

                // Locate state overrides other states
                if (currentLedAState == CCI_LED_STATE_LOCATE)
                {
                    nextLedsState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                                    CCI_LED_STATE_LOCATE,
                                    nextLedsState);
                }

                if (currentLedBState == CCI_LED_STATE_LOCATE)
                {
                    nextLedsState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                                    CCI_LED_STATE_LOCATE,
                                    nextLedsState);
                }
            }
            else
            {
                nextLedsState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                                CCI_LED_STATE_OFF,
                                nextLedsState);
                nextLedsState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                                CCI_LED_STATE_OFF,
                                nextLedsState);
            }

            // This is the next state that the LED will be set to
            cciSetNextXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                                   osfp, nextLedsState);

            // Only update HW if required
            if (currentLedsState != nextLedsState)
            {
                // bSetLocate input param is unused for E4840 implementation
                cciSetXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                                       osfp, NV_FALSE);
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
}

void
nvswitch_cci_update_link_state_led_ls10
(
    nvswitch_device *device
)
{
    _nvswitch_cci_update_link_state_led_ls10(device);   
}

/*
 *  Returns the CPLD register value assigned to a particular CCI LED state
 *  confluence page ID: 1011518154
 */
static NvU8
_cciGetXcvrLedStateRegVal_ls10
(
    NvU8 ledState
)
{
    switch (ledState)
    {
        case CCI_LED_STATE_OFF:
        {
            return CPLD_MACHXO3_LED_STATE_REG_LED_STATE_OFF;
        }
        case CCI_LED_STATE_UP_WARM:
        {
            return CPLD_MACHXO3_LED_STATE_REG_LED_STATE_GREEN;
        } 
        case CCI_LED_STATE_INITIALIZE:
        {
            return CPLD_MACHXO3_LED_STATE_REG_LED_STATE_3HZ_AMBER;
        } 
        case CCI_LED_STATE_UP_ACTIVE:
        {
            return CPLD_MACHXO3_LED_STATE_REG_LED_STATE_3HZ_GREEN;
        }
        case CCI_LED_STATE_FAULT:
        {
            return CPLD_MACHXO3_LED_STATE_REG_LED_STATE_6HZ_AMBER;
        } 
        case CCI_LED_STATE_LOCATE:
        {
            return CPLD_MACHXO3_LED_STATE_REG_LED_STATE_6HZ_AMBER_GREEN;
        }
        default:
        {
            NVSWITCH_ASSERT(0);
            return CPLD_MACHXO3_LED_STATE_REG_LED_STATE_OFF;
        }
    }
}

/*
 *  
 */
static NvlStatus
_cciSetXcvrLedState_ls10
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp
)
{
    NvlStatus retval;
    NvU8 ledAState;
    NvU8 ledBState;
    NvU8 nextLedState;
    NvU8 regVal;

    nextLedState = device->pCci->xcvrNextLedState[osfp];
    ledAState = REF_VAL(CCI_LED_STATE_LED_A, nextLedState);
    ledBState = REF_VAL(CCI_LED_STATE_LED_B, nextLedState);

    regVal = 0;
    regVal = FLD_SET_REF_NUM(CPLD_MACHXO3_LED_STATE_REG_LED_A,
                             _cciGetXcvrLedStateRegVal_ls10(ledAState),
                             regVal);
    regVal = FLD_SET_REF_NUM(CPLD_MACHXO3_LED_STATE_REG_LED_B,
                             _cciGetXcvrLedStateRegVal_ls10(ledBState),
                             regVal);
    
    // Set states for both LED A and LED B
    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_LED_STATE_REG(osfp), regVal);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // save CCI HW state
    device->pCci->xcvrCurrentLedState[osfp] = REF_NUM(CCI_LED_STATE_LED_A, ledAState) |
                                              REF_NUM(CCI_LED_STATE_LED_B, ledBState);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_cci_set_xcvr_led_state_ls10
(
    nvswitch_device *device,
    NvU32 client, 
    NvU32 osfp, 
    NvBool bSetLocate
)
{
    return _cciSetXcvrLedState_ls10(device, client, osfp);
}

static NvlStatus
_nvswitch_cci_setup_link_mask_ls10
(
    nvswitch_device *device
)
{
    NvU64 linkTrainDisableMask;
    nvlink_link *link;
    NvU8 linkId;

    cciGetAllLinks(device, &device->pCci->linkMask);

    linkTrainDisableMask = 0;

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
        linkTrainDisableMask = (NvU64)device->regkeys.cci_link_train_disable_mask | 
                               ((NvU64)device->regkeys.cci_link_train_disable_mask2 << 32);
#endif
    device->pCci->linkMask = device->pCci->linkMask & ~linkTrainDisableMask;

    // Mark links as CCI managed for use by the NVLink library
    FOR_EACH_INDEX_IN_MASK(64, linkId, device->pCci->linkMask)
    {
        link = nvswitch_get_link(device, linkId);
        if (link == NULL)
        {
            return -NVL_ERR_GENERIC;
        }
        link->bCciManaged = NV_TRUE;
    }
    FOR_EACH_INDEX_IN_MASK_END;                
    
    NVSWITCH_PRINT(device, INFO,
         "%s: Initial CCI link mask 0x%llx\n", __FUNCTION__,
            device->pCci->linkMask);

    return NVL_SUCCESS;
}

static void
_nvswitch_cci_link_training_callback_ls10
(
    nvswitch_device         *device
)
{
    nvswitch_cci_update_link_state_led(device);
    cciModulesOnboardCallback(device);
}

static void
_nvswitch_cci_cdb_processing_callback_ls10
(
    nvswitch_device         *device
)
{
    //
    // TODO: Should be performed in SWGEN0 interrupt handler
    //       Bug 3660933  
    //
    if (nvswitch_is_soe_supported(device))
    {
        soeService_HAL(device, ((PSOE)device->pSoe));
        cciProcessCDBCallback(device);
    }
}

NvlStatus 
nvswitch_cci_setup_onboard_ls10
(
    nvswitch_device *device
)
{
    NvlStatus status;
    
    // Update Link Mask
    status = _nvswitch_cci_setup_link_mask_ls10(device);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    // Initializes module onboard state machine
    cciModulesOnboardInit(device);
    
    status = cciRegisterCallback(device, NVSWITCH_CCI_CALLBACK_LINK_STATE_UPDATE,
                                    _nvswitch_cci_link_training_callback_ls10,
                                    CCI_LINK_TRAINING_POLLING_RATE_HZ);
    if (status != NVL_SUCCESS)
    {
        return status;
    }
    
    status = cciRegisterCallback(device, NVSWITCH_CCI_CALLBACK_CDB,
                                    _nvswitch_cci_cdb_processing_callback_ls10,
                                    CCI_CDB_PROCESSING_POLLING_RATE_HZ);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return status;
}

NvlStatus
nvswitch_cci_setup_module_path_ls10
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp
)
{
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS i2cIndexed = { 0 };
    NvU8 channel;
    NvlStatus retval;

    NVSWITCH_ASSERT(osfp < device->pCci->osfp_num);
    channel = MODULE_ID_TO_MUX_CHANNEL_PCA9847(osfp);

    if (device->pCci->isFaulty[osfp])
    {
        return -NVL_ERR_GENERIC;
    }

    i2cIndexed.bIsRead = NV_FALSE;
    i2cIndexed.port = device->pCci->osfp_i2c_info[osfp].i2cPortLogical;
    i2cIndexed.address = MUX_I2C_ADDR_PCA9847;
    i2cIndexed.acquirer = client;
    i2cIndexed.flags =
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _START, _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _STOP, _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _ADDRESS_MODE, _7BIT) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ONE) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _HW) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _400KHZ) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _TRANSACTION_MODE, _NORMAL);
    i2cIndexed.flags |= DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _DISABLED);
    i2cIndexed.flags |= DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _RESTART, _NONE);
    i2cIndexed.index[0] = channel;
    i2cIndexed.messageLength = 1;
    i2cIndexed.message[0] = channel;

    retval = nvswitch_ctrl_i2c_indexed(device, &i2cIndexed);
    if (retval != NVL_SUCCESS)
    {
        // Reset mux and try again
        _nvswitch_cci_reset_mux_ls10(device, i2cIndexed.port);

        retval = nvswitch_ctrl_i2c_indexed(device, &i2cIndexed);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Setting mux port %d addr 0x%x to channel %d failed. retval %d\n",
                __FUNCTION__, i2cIndexed.port, i2cIndexed.address, channel, retval);            
            return retval;
        }
    }
    
    return NVL_SUCCESS;
}

static NvlStatus
_cci_process_cmd_ls10
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp,
    NvU32            addr,
    NvU32            length,
    NvU8            *pValArray,
    NvBool           bRead,
    NvBool           bBlk
)
{
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS i2cIndexed = { 0 };
    NvlStatus retval;
    PCCI pCci = device->pCci;

    i2cIndexed.bIsRead = bRead;
    i2cIndexed.port = pCci->osfp_i2c_info[osfp].i2cPortLogical;
    i2cIndexed.address = (NvU16) pCci->osfp_i2c_info[osfp].i2cAddress;
    i2cIndexed.messageLength = length;
    i2cIndexed.acquirer = client;
    i2cIndexed.flags =
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _START, _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _STOP, _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _ADDRESS_MODE, _7BIT) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ONE) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _HW) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _400KHZ) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _TRANSACTION_MODE, _NORMAL);
    i2cIndexed.flags |= bBlk ?
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _ENABLED) :
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _DISABLED);
    i2cIndexed.flags |= bRead ?
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _RESTART, _SEND) :
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _RESTART, _NONE);
    i2cIndexed.index[0] = addr;

    if (!bRead)
    {
        nvswitch_os_memcpy(i2cIndexed.message, pValArray, length);
    }

    retval = nvswitch_ctrl_i2c_indexed(device, &i2cIndexed);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, MMIO,
            "%s: I2C command to osfp[%d](addr : 0x%x, port : %d) failed\n",
            __FUNCTION__, osfp,
            pCci->osfp_i2c_info[osfp].i2cAddress,
            pCci->osfp_i2c_info[osfp].i2cPortLogical);
        return retval;
    }

    if (bRead)
    {
        nvswitch_os_memcpy(pValArray, i2cIndexed.message, length);
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_cci_module_access_cmd_ls10
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp,
    NvU32            addr,
    NvU32            length,
    NvU8            *pValArray,
    NvBool           bRead,
    NvBool           bBlk
)
{
    NvlStatus status;
    NVSWITCH_TIMEOUT timeout;

    nvswitch_timeout_create(5 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    do
    {
        status = _cci_process_cmd_ls10(device, client, osfp, addr, length, pValArray, bRead, bBlk);
        if (status == NVL_SUCCESS)
        {
            return status;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for CMIS access to complete! rc = %d\n",
                __FUNCTION__, status);

            NVSWITCH_PRINT(device, ERROR,
                "%s: Access to register 0x%x failed on osfp[%d](addr : 0x%x, port : %d)\n",
                __FUNCTION__, addr, osfp,
                device->pCci->osfp_i2c_info[osfp].i2cAddress,
                device->pCci->osfp_i2c_info[osfp].i2cPortLogical);

            // Mark as faulty
            device->pCci->isFaulty[osfp] = NV_TRUE;

            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE,
                           "Module %d access error\n", osfp);

            return -NVL_IO_ERROR;
        }

        nvswitch_os_sleep(10);
    } while (NV_TRUE);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_cci_get_xcvr_mask_ls10
(
    nvswitch_device *device,
    NvU32           *pMaskAll,
    NvU32           *pMaskPresent
)
{
    if (pMaskAll != NULL)
    {
        *pMaskAll = device->pCci->cagesMask;
    }

    if (pMaskPresent != NULL)
    {
        device->pCci->modulesMask = device->pCci->osfpMaskPresent;
        *pMaskPresent = device->pCci->modulesMask;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_cci_apply_control_set_values_ls10
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           moduleMask
)
{
    return -NVL_ERR_NOT_IMPLEMENTED;
}

NvlStatus
nvswitch_cci_get_xcvr_led_state_ls10
(
    nvswitch_device *device,
    NvU32 client, 
    NvU32 osfp, 
    NvU8 *pLedState
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_reset_links_ls10
(
    nvswitch_device *device,
    NvU64 linkMask
)
{
    // Links resets are managed by the onboard state machine for CCI managed links
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_cmis_cage_bezel_marking_ls10
(
    nvswitch_device *device,
    NvU8 cageIndex,
    char *pBezelMarking
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_get_grading_values_ls10
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           linkId,
    NvU8            *laneMask,
    NVSWITCH_CCI_GRADING_VALUES *pGrading
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}
