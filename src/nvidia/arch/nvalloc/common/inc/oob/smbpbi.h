/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SMBPBI_H
#define _SMBPBI_H

#include "oob/smbpbi_impl.h"

/*!
 * This file contains definitions for SMBPBI - SMBUS Out-Of-Band (OOB) interface
 */
/*!
 * NV_MSGBOX_CMD() macro below and its derivatives depend on DRF_DEF() and DRF_NUM()
 * macros, that are defined in "nvdrf.h" header file, which is a part of
 * smbus/msgbox SDK. That file needs to be #included before this (smbpbi.h)
 * header file.
 */

/********************************************************************************
 *                                                                              *
 *                             GPU COMMANDS                                     *
 *                                                                              *
 ********************************************************************************/

//
// Encoding:
//       30 29          23           16 15            8 7             0
//     .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
//     |I|C|R|  STATUS |      ARG2     |      ARG1     |     OPCODE    | COMMAND
//     `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//     31     28     24
//

#define NV_MSGBOX_CMD_OPCODE                                           7:0
#define NV_MSGBOX_CMD_OPCODE_NULL_CMD                           0x00000000
#define NV_MSGBOX_CMD_OPCODE_GET_CAP_DWORD                      0x00000001
#define NV_MSGBOX_CMD_OPCODE_GET_TEMP                           0x00000002
#define NV_MSGBOX_CMD_OPCODE_GET_EXT_TEMP                       0x00000003    /* With 8 fractional bits! */
#define NV_MSGBOX_CMD_OPCODE_GET_POWER                          0x00000004
#define NV_MSGBOX_CMD_OPCODE_GET_SYS_ID_DATA                    0x00000005
#define NV_MSGBOX_CMD_OPCODE_GET_ECC_V1                         0x00000006
#define NV_MSGBOX_CMD_OPCODE_GET_ECC_V2                         0x00000007
#define NV_MSGBOX_CMD_OPCODE_GPU_PCONTROL                       0x00000008
#define NV_MSGBOX_CMD_OPCODE_GPU_SYSCONTROL                     0x00000009
#define NV_MSGBOX_CMD_OPCODE_SET_MASTER_CAPS                    0x0000000a
#define NV_MSGBOX_CMD_OPCODE_SET_PRIMARY_CAPS                   0x0000000a // Duplicate value till bug 3190552 is addressed.
#define NV_MSGBOX_CMD_OPCODE_GPU_REQUEST_CPL                    0x0000000b
#define NV_MSGBOX_CMD_OPCODE_GET_ECC_V3                         0x0000000c
#define NV_MSGBOX_CMD_OPCODE_SCRATCH_READ                       0x0000000d
#define NV_MSGBOX_CMD_OPCODE_SCRATCH_WRITE                      0x0000000e
#define NV_MSGBOX_CMD_OPCODE_SCRATCH_COPY                       0x0000000f
#define NV_MSGBOX_CMD_OPCODE_ASYNC_REQUEST                      0x00000010
#define NV_MSGBOX_CMD_OPCODE_REGISTER_ACCESS                    0x00000011
#define NV_MSGBOX_CMD_OPCODE_GET_POWER_CONNECTOR_STATE          0x00000012
#define NV_MSGBOX_CMD_OPCODE_GET_PAGE_RETIREMENT_STATS          0x00000013
#define NV_MSGBOX_CMD_OPCODE_GET_ECC_V4                         0x00000014
#define NV_MSGBOX_CMD_OPCODE_GET_THERM_PARAM                    0x00000015
#define NV_MSGBOX_CMD_OPCODE_GET_ECC_V5                         0x00000016
#define NV_MSGBOX_CMD_OPCODE_ACCESS_WP_MODE                     0x00000017

//
// TODO: Get rid of MISC_GPU_FLAGS once all SMBPBI implementations have made
// the switch to MISC_DEVICE_FLAGS.
//
#define NV_MSGBOX_CMD_OPCODE_GET_MISC_GPU_FLAGS                 0x00000018
#define NV_MSGBOX_CMD_OPCODE_GET_MISC_DEVICE_FLAGS              0x00000018
#define NV_MSGBOX_CMD_OPCODE_GPU_UTIL_COUNTERS                  0x00000019
#define NV_MSGBOX_CMD_OPCODE_GET_NVLINK_INFO                    0x0000001a
#define NV_MSGBOX_CMD_OPCODE_GET_CLOCK_FREQ_INFO                0x0000001b
#define NV_MSGBOX_CMD_OPCODE_BUNDLE_LAUNCH                      0x0000001c
#define NV_MSGBOX_CMD_OPCODE_GET_DRIVER_EVENT_MSG               0x0000001d
#define NV_MSGBOX_CMD_OPCODE_GET_ECC_V6                         0x0000001e
#define NV_MSGBOX_CMD_OPCODE_REMAP_ROW_STATS                    0x00000020
#define NV_MSGBOX_CMD_OPCODE_GET_PCIE_LINK_INFO                 0x00000021
#define NV_MSGBOX_CMD_OPCODE_GET_ENERGY_COUNTER                 0x00000022
#define NV_MSGBOX_CMD_OPCODE_RESERVED_0                          0x00000024
#define NV_MSGBOX_CMD_OPCODE_RESERVED_1                          0x00000025
#define NV_MSGBOX_CMD_OPCODE_GET_POWER_HINT_INFO                0x00000026
#define NV_MSGBOX_CMD_OPCODE_DYNAMIC_SYSTEM_INFORMATION         0x00000027
#define NV_MSGBOX_CMD_OPCODE_GPU_PERFORMANCE_MONITORING         0x00000028

#define NV_MSGBOX_CMD_ARG1                                            15:8
#define NV_MSGBOX_CMD_ARG1_NULL                                 0x00000000
#define NV_MSGBOX_CMD_ARG1_TEMP_GPU_0                           0x00000000
#define NV_MSGBOX_CMD_ARG1_TEMP_NVSWITCH_0                      0x00000000
#define NV_MSGBOX_CMD_ARG1_TEMP_GPU_1                           0x00000001    /* For Gemini boards */
#define NV_MSGBOX_CMD_ARG1_TEMP_BOARD                           0x00000004
#define NV_MSGBOX_CMD_ARG1_TEMP_MEMORY                          0x00000005
#define NV_MSGBOX_CMD_ARG1_TEMP_PWR_SUPPLY                      0x00000006
#define NV_MSGBOX_CMD_ARG1_TEMP_T_LIMIT                         0x00000007
#define NV_MSGBOX_CMD_ARG1_TEMP_NUM_SENSORS                              8
#define NV_MSGBOX_CMD_ARG1_POWER_TOTAL                          0x00000000
#define NV_MSGBOX_CMD_ARG1_SMBPBI_POWER                         0x00000001
#define NV_MSGBOX_CMD_ARG1_POWER_FB                             0x00000002
#define NV_MSGBOX_CMD_ARG1_POWER_MODULE                         0x00000003
/* SysId info type encodings for opcode NV_MSGBOX_CMD_OPCODE_GET_SYS_ID_DATA (0x05) */
#define NV_MSGBOX_CMD_ARG1_BOARD_PART_NUM_V1                    0x00000000
#define NV_MSGBOX_CMD_ARG1_OEM_INFO_V1                          0x00000001
#define NV_MSGBOX_CMD_ARG1_SERIAL_NUM_V1                        0x00000002
#define NV_MSGBOX_CMD_ARG1_MARKETING_NAME_V1                    0x00000003
#define NV_MSGBOX_CMD_ARG1_GPU_PART_NUM_V1                      0x00000004
#define NV_MSGBOX_CMD_ARG1_MEMORY_VENDOR_V1                     0x00000005
#define NV_MSGBOX_CMD_ARG1_MEMORY_PART_NUM_V1                   0x00000006
#define NV_MSGBOX_CMD_ARG1_BUILD_DATE_V1                        0x00000007
#define NV_MSGBOX_CMD_ARG1_FIRMWARE_VER_V1                      0x00000008
#define NV_MSGBOX_CMD_ARG1_VENDOR_ID_V1                         0x00000009
#define NV_MSGBOX_CMD_ARG1_DEV_ID_V1                            0x0000000a
#define NV_MSGBOX_CMD_ARG1_SUB_VENDOR_ID_V1                     0x0000000b
#define NV_MSGBOX_CMD_ARG1_SUB_ID_V1                            0x0000000c
#define NV_MSGBOX_CMD_ARG1_GPU_GUID_V1                          0x0000000d
#define NV_MSGBOX_CMD_ARG1_INFOROM_VER_V1                       0x0000000e
#define NV_MSGBOX_CMD_ARG1_PRODUCT_LENGTH_V1                    0x0000000f
#define NV_MSGBOX_CMD_ARG1_PRODUCT_WIDTH_V1                     0x00000010
#define NV_MSGBOX_CMD_ARG1_PRODUCT_HEIGHT_V1                    0x00000011
#define NV_MSGBOX_CMD_ARG1_PCIE_SPEED_V1                        0x00000012
#define NV_MSGBOX_CMD_ARG1_PCIE_WIDTH_V1                        0x00000013
#define NV_MSGBOX_CMD_ARG1_TGP_LIMIT_V1                         0x00000014
#define NV_MSGBOX_CMD_ARG1_FRU_PART_NUMBER_V1                   0x00000015
#define NV_MSGBOX_CMD_ARG1_MODULE_POWER_LIMIT_V1                0x00000016
#define NV_MSGBOX_CMD_ARG1_MAX_DRAM_CAPACITY_V1                 0x00000017
#define NV_MSGBOX_CMD_ARG1_SYS_ID_DATA_TYPE_MAX                 0x00000017    /* Adjust, when adding new types */
#define NV_MSGBOX_CMD_ARG1_REGISTER_ACCESS_WRITE                0x00000000
#define NV_MSGBOX_CMD_ARG1_REGISTER_ACCESS_READ                 0x00000001
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_TARGET              0x00000000
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_HW_SLOWDN           0x00000001
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_SHUTDN              0x00000002
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_HBM_SLOWDN          0x00000003
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_SW_SLOWDN           0x00000004
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_TARGET_TLIMIT       0x00000005
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_HW_SLOWDN_TLIMIT    0x00000006
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_SHUTDN_TLIMIT       0x00000007
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_HBM_SLOWDN_TLIMIT   0x00000008
#define NV_MSGBOX_CMD_ARG1_THERM_PARAM_TEMP_SW_SLOWDN_TLIMIT    0x00000009
#define NV_MSGBOX_CMD_ARG1_GET_MISC_ECC_ENABLED_STATE           0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_RESET_REQUIRED          0x00000001
#define NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_FLAGS_PAGE_0            0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_FLAGS_PAGE_1            0x00000001
#define NV_MSGBOX_CMD_ARG1_GET_MISC_DEVICE_FLAGS_PAGE_0         0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_MISC_DEVICE_FLAGS_PAGE_1         0x00000001

#define NV_MSGBOX_CMD_ARG1_GPU_UTIL_COUNTERS_CONTEXT_TIME       0x00000000
#define NV_MSGBOX_CMD_ARG1_GPU_UTIL_COUNTERS_SM_TIME            0x00000001
#define NV_MSGBOX_CMD_ARG1_GPU_UTIL_COUNTERS_RESET_COUNTERS     0x000000FF
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK_COUNT           0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK_STATE_V1        0x00000001
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK_BANDWIDTH       0x00000002
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_ERR_COUNTER_REPLAY   0x00000003
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_ERR_COUNTER_RECOVERY 0x00000004
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_ERR_COUNTER_FLIT_CRC 0x00000005
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_ERR_COUNTER_DATA_CRC 0x00000006
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK_STATE_V2        0x00000007
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_SUBLINK_WIDTH        0x00000008
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_THROUGHPUT_DATA_TX   0x00000009
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_THROUGHPUT_DATA_RX   0x0000000a
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_THROUGHPUT_RAW_TX    0x0000000b
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_THROUGHPUT_RAW_RX    0x0000000c
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_TRAINING_ERROR_STATE   0x0000000d
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_RUNTIME_ERROR_STATE    0x0000000e
#define NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_AVAILABILITY         0x0000000f
#define NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_INFO_CURRENT          0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_INFO_MIN              0x00000001
#define NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_INFO_MAX              0x00000002
#define NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_INFO_PAGE_3           0x00000003
#define NV_MSGBOX_CMD_ARG1_GET_SUPPORTED_CLOCK_THROTTLE_REASONS 0x00000004
#define NV_MSGBOX_CMD_ARG1_GET_CURRENT_CLOCK_THROTTLE_REASONS   0x00000005
#define NV_MSGBOX_CMD_ARG1_REMAP_ROWS_RAW_COUNTS                0x00000000
#define NV_MSGBOX_CMD_ARG1_REMAP_ROWS_STATE_FLAGS               0x00000001
#define NV_MSGBOX_CMD_ARG1_REMAP_ROWS_HISTOGRAM                 0x00000002
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_0            0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_1            0x00000001
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_2            0x00000002
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_3            0x00000003
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_4            0x00000004
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_6            0x00000006
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_8            0x00000008
#define NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_9            0x00000009

/* Async requests */
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_TGP_LIMIT_CONTROL_GET     \
                                                                0x00000000
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_TGP_LIMIT_CONTROL_SET     \
                                                                0x00000001
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_TGP_LIMIT_INFO_GET        \
                                                                0x00000002
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_THERMAL_FAN_V1_COUNT_GET           \
                                                                0x00000003
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_THERMAL_FAN_V1_INFO_GET            \
                                                                0x00000004
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_THERMAL_FAN_V1_STATUS_GET          \
                                                                0x00000005
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_OVERCLOCKING_LIMIT_CONTROL_GET     \
                                                                0x00000006
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_OVERCLOCKING_LIMIT_CONTROL_SET     \
                                                                0x00000007
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_ENERGY_COUNTER_STATUS_GET          \
                                                                0x00000008
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_VIOLATION_COUNTERS_STATUS_GET      \
                                                                0x00000009
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_UTILIZATION_RATE_GET               \
                                                                0x0000000a
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_OOB_CLOCK_LIMIT_SET                \
                                                                0x0000000b
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_DEVICE_MODE_CONTROL                \
                                                                0x0000000c
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_TEST_MESSAGE_SEND                  \
                                                                0x0000000e
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_CLOCK_LIMIT_GET                    \
                                                                0x0000000d
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_FAN_CURVE_POINTS_GET_SET           \
                                                                0x0000000f
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_POWER_HINT_GET                     \
                                                                0x00000010
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_CONFIGURE_PROGRAMMABLE_EDPP        \
                                                                0x00000011
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_MODULE_LIMIT_CONTROL_GET  \
                                                                0x00000012
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_MODULE_LIMIT_CONTROL_SET  \
                                                                0x00000013
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_MODULE_LIMIT_INFO_GET     \
                                                                0x00000014
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_MEMORY_CAPACITY_UTILIZATION_GET    \
                                                                0x00000015
#define NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_POLL                   0x000000ff


#define NV_MSGBOX_CMD_ARG1_ECC_V2_SEL                                15:14
#define NV_MSGBOX_CMD_ARG1_ECC_V2_SEL_CNT                       0x00000000    /* counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_SEL_FBA                       0x00000001    /* FB addresses */
/* Counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_DEV                            13:12
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_DEV_FB                    0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_DEV_GR                    0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_FB_SRC                         11:10
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_FB_SRC_LTC                0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_FB_SRC_FB                 0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_GR_SRC                         11:10
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_GR_SRC_L1                 0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_GR_SRC_RF                 0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_TYPE                             9:8
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_TYPE_SBE                  0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V2_CNT_TYPE_DBE                  0x00000001
/* FB addresses */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_FBA_SC                             13:13    /* scope */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_FBA_SC_IND                    0x00000000    /* individual addresses */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_FBA_SC_CUMUL                  0x00000001    /* cumulative counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_FBA_IND_FLD                        12:12    /* field */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_FBA_IND_FLD_CTRS              0x00000000    /* counters: sbe, dbe */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_FBA_IND_FLD_ADDR              0x00000001    /* address value */
#define NV_MSGBOX_CMD_ARG1_ECC_V2_FBA_IND_INDEX                       11:8    /* address array index: 0-13 */

#define NV_MSGBOX_CMD_ARG1_ECC_V3_SEL                                15:14
#define NV_MSGBOX_CMD_ARG1_ECC_V3_SEL_CNT                       0x00000000    /* counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_SEL_FBA                       0x00000001    /* FB addresses */
/* Counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_DEV                            13:12
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_DEV_FB                    0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_DEV_GR                    0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_FB_SRC                         11:10
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_FB_SRC_LTC                0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_FB_SRC_FB                 0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_GR_SRC                         11:10
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_GR_SRC_L1                 0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_GR_SRC_RF                 0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_GR_SRC_TEX                0x00000002
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_TYPE                             9:8
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_TYPE_SBE                  0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V3_CNT_TYPE_DBE                  0x00000001
/* FB addresses */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_FBA_SC                             13:13    /* scope */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_FBA_SC_IND                    0x00000000    /* individual addresses */
// These are not present in V3
//#define NV_MSGBOX_CMD_ARG1_ECC_V3_FBA_SC_CUMUL                  0x00000001    /* cumulative counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_FBA_IND_FLD                        12:12    /* field */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_FBA_IND_FLD_CTRS              0x00000000    /* counters: sbe, dbe */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_FBA_IND_FLD_ADDR              0x00000001    /* address value */
#define NV_MSGBOX_CMD_ARG1_ECC_V3_FBA_IND_INDEX                       11:8    /* address array index: 0-13 */

#define NV_MSGBOX_CMD_ARG1_ECC_V4_SEL                                15:14
#define NV_MSGBOX_CMD_ARG1_ECC_V4_SEL_CNT                       0x00000000    /* counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_SEL_FBA                       0x00000001    /* FB addresses */
/* Counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_DEV                            13:12
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_DEV_FB                    0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_DEV_GR                    0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_FB_SRC                         11:10
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_FB_SRC_LTC                0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_FB_SRC_FB                 0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_GR_SRC                         11:10
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_GR_SRC_SHM                0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_GR_SRC_RF                 0x00000001
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_GR_SRC_TEX                0x00000002
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_TYPE                             9:8
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_TYPE_SBE                  0x00000000
#define NV_MSGBOX_CMD_ARG1_ECC_V4_CNT_TYPE_DBE                  0x00000001
/* FB addresses */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_FBA_SC                             13:13    /* scope */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_FBA_SC_IND                    0x00000000    /* individual addresses */
// These are not present in V4
//#define NV_MSGBOX_CMD_ARG1_ECC_V4_FBA_SC_CUMUL                0x00000001    /* cumulative counters */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_FBA_IND_FLD                        12:12    /* field */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_FBA_IND_FLD_CTRS              0x00000000    /* counters: sbe, dbe */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_FBA_IND_FLD_ADDR              0x00000001    /* address value */
#define NV_MSGBOX_CMD_ARG1_ECC_V4_FBA_IND_INDEX                       11:8    /* address array index: 0-13 */

#define NV_MSGBOX_CMD_ARG1_ECC_V5_SEL                                15:15
#define NV_MSGBOX_CMD_ARG1_ECC_V5_SEL_COUNTS                    0x00000000    /* address/region counts */
#define NV_MSGBOX_CMD_ARG1_ECC_V5_SEL_ERR_BUFFER                0x00000001    /* SRAM error buffer */

#define NV_MSGBOX_CMD_ARG1_ECC_V5_COUNTS_INDEX_LO                     14:8    /* 7 least significant index bits */

#define NV_MSGBOX_CMD_ARG1_ECC_V5_ERR_BUF_INDEX                       11:8    /* buffer entry index */

#define NV_MSGBOX_CMD_ARG1_ACCESS_WP_MODE_ACTION                       8:8
#define NV_MSGBOX_CMD_ARG1_ACCESS_WP_MODE_ACTION_GET            0x00000000
#define NV_MSGBOX_CMD_ARG1_ACCESS_WP_MODE_ACTION_SET            0x00000001

#define NV_MSGBOX_CMD_ARG1_BUNDLE_REQUEST_COUNT                       11:8
#define NV_MSGBOX_CMD_ARG1_BUNDLE_DISP_RULE_COUNT                    15:12

#define NV_MSGBOX_CMD_ARG1_GET_DEM_OLDEST                       0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_DEM_BY_SEQ_NUMBER                0x00000001
#define NV_MSGBOX_CMD_ARG1_GET_DEM_BY_TIMESTAMP                 0x00000002

#define NV_MSGBOX_CMD_ARG1_ECC_V6_ERROR_TYPE                          15:8
#define NV_MSGBOX_CMD_ARG1_ECC_V6_ERROR_TYPE_CORRECTABLE_ERROR           0
#define NV_MSGBOX_CMD_ARG1_ECC_V6_ERROR_TYPE_UNCORRECTABLE_ERROR         1
#define NV_MSGBOX_CMD_ARG1_ECC_V6_ERROR_TYPE_ECC_STATE_FLAGS             2

#define NV_MSGBOX_CMD_ARG1_ENERGY_COUNTER_GPU                   0x00000000
#define NV_MSGBOX_CMD_ARG1_ENERGY_COUNTER_MODULE                0x00000003

// Query type of _GET_POWER_HINT_INFO
#define NV_MSGBOX_CMD_ARG1_GET_POWER_HINT_INFO_CLK                0x00000000
#define NV_MSGBOX_CMD_ARG1_GET_POWER_HINT_INFO_TEMP               0x00000001
#define NV_MSGBOX_CMD_ARG1_GET_POWER_HINT_INFO_PROFILES           0x00000002
#define NV_MSGBOX_CMD_ARG1_GET_POWER_HINT_INFO_NUM                         \
            (NV_MSGBOX_CMD_ARG1_GET_POWER_HINT_INFO_PROFILES + 1)

// Arg1 for _GPU_PERFORMANCE_MONITORING
#define NV_MSGBOX_CMD_ARG1_GPM_ACTION                                  15:14
#define NV_MSGBOX_CMD_ARG1_GPM_ACTION_GET_INSTANTANEOUS_METRIC    0x00000000
#define NV_MSGBOX_CMD_ARG1_GPM_ACTION_GET_SNAPSHOT_METRIC         0x00000001
#define NV_MSGBOX_CMD_ARG1_GPM_ACTION_CAPTURE_SNAPSHOT            0x00000002
#define NV_MSGBOX_CMD_ARG1_GPM_ACTION_SET_MULTIPLIER              0x00000003

#define NV_MSGBOX_CMD_ARG1_GPM_METRIC                                   13:8
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_GRAPHICS_ENGINE             0x00000000
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_SM_ACTIVITY                 0x00000001
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_SM_OCCUPANCY                0x00000002
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_TENSOR_CORE_ACTIVITY        0x00000003
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_DRAM_USAGE                  0x00000004
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_FP64_ACTIVITY               0x00000005
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_FP32_ACTIVITY               0x00000006
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_FP16_ACTIVITY               0x00000007
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_PCIE_TX_BANDWIDTH           0x00000008
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_PCIE_RX_BANDWIDTH           0x00000009
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_RAW_TX_BANDWIDTH     0x0000000A
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_DATA_TX_BANDWIDTH    0x0000000B
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_RAW_RX_BANDWIDTH     0x0000000C
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_DATA_RX_BANDWIDTH    0x0000000D
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVDEC_UTILIZATION           0x0000000E
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVJPG_UTILIZATION           0x0000000F
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVOFA_UTILIZATION           0x00000010
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_RAW_TX_BW_PER_LINK   0x00000011
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_DATA_TX_BW_PER_LINK  0x00000012
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_RAW_RX_BW_PER_LINK   0x00000013
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_DATA_RX_BW_PER_LINK  0x00000014
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVDEC_UTIL_PER_INSTANCE     0x00000015
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVJPG_UTIL_PER_INSTANCE     0x00000016
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_INTEGER_UTILIZATION         0x00000017
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_DMMA_UTILIZATION            0x00000018
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_HMMA_UTILIZATION            0x00000019
#define NV_MSGBOX_CMD_ARG1_GPM_METRIC_IMMA_UTILIZATION            0x0000001A

#define NV_MSGBOX_CMD_ARG1_DYN_SYS_INFO_DRIVER_VERSION_V1         0x00000000

#define NV_MSGBOX_CMD_ARG2                                           23:16
#define NV_MSGBOX_CMD_ARG2_NULL                                 0x00000000

#define NV_MSGBOX_CMD_ARG2_ECC_V2_FB_PARTITION                       23:20
#define NV_MSGBOX_CMD_ARG2_ECC_V2_FB_SLICE                           19:16
#define NV_MSGBOX_CMD_ARG2_ECC_V2_GR_GPC                             23:20
#define NV_MSGBOX_CMD_ARG2_ECC_V2_GR_TPC                             19:16

#define NV_MSGBOX_CMD_ARG2_ECC_V3_FB_PARTITION                       23:20
#define NV_MSGBOX_CMD_ARG2_ECC_V3_FB_SLICE                           19:16
#define NV_MSGBOX_CMD_ARG2_ECC_V3_FB_SUBPARTITION                    19:16
#define NV_MSGBOX_CMD_ARG2_ECC_V3_GR_TEX                             23:22
#define NV_MSGBOX_CMD_ARG2_ECC_V3_GR_GPC                             21:19
#define NV_MSGBOX_CMD_ARG2_ECC_V3_GR_TPC                             18:16

#define NV_MSGBOX_CMD_ARG2_ECC_V4_FB_PARTITION                       23:20
#define NV_MSGBOX_CMD_ARG2_ECC_V4_FB_SLICE                           19:16
#define NV_MSGBOX_CMD_ARG2_ECC_V4_FB_SUBPARTITION                    19:16
#define NV_MSGBOX_CMD_ARG2_ECC_V4_GR_TEX                             23:22
#define NV_MSGBOX_CMD_ARG2_ECC_V4_GR_GPC                             21:19
#define NV_MSGBOX_CMD_ARG2_ECC_V4_GR_TPC                             18:16

#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_INDEX_HI                    23:21
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE                        18:16
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_HDR_META                 0x0  /* header and metadata */

#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_ADDR_ADDR                0x1  /* addr: address */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_ADDR_UNCORRECTED_COUNTS  0x2  /* addr: uncorrectedCounts */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_ADDR_CORRECTED_TOTAL     0x3  /* addr: correctedTotal */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_ADDR_CORRECTED_UNIQUE    0x4  /* addr: correctedUnique */

#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_REGN_CORRECTED_TOTAL     0x1  /* region: correctedTotal */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_REGN_CORRECTED_UNIQUE    0x2  /* region: correctedUnique */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_REGN_UNCORRECTED_TOTAL   0x3  /* region: uncorrectedTotal */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_REGN_UNCORRECTED_UNIQUE  0x4  /* region: uncorrectedUnique */

#define NV_MSGBOX_CMD_ARG2_ECC_V5_COUNTS_TYPE_MAX                      0x4

#define NV_MSGBOX_CMD_ARG2_ECC_V5_ERR_BUF_TYPE                       17:16  /* entry element type */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_ERR_BUF_TYPE_ERR_TYPE_META           0x0  /* errorType and metadata */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_ERR_BUF_TYPE_TIME_STAMP              0x1  /* time stamp */
#define NV_MSGBOX_CMD_ARG2_ECC_V5_ERR_BUF_TYPE_ADDR                    0x2  /* address */

#define NV_MSGBOX_CMD_ARG2_ECC_V5_ERR_BUF_TYPE_MAX                     0x2

#define NV_MSGBOX_CMD_ARG2_REG_INDEX_SCRATCH_PAGE               0x00000000
#define NV_MSGBOX_CMD_ARG2_REG_INDEX_EVENTS_PENDING             0x00000001
#define NV_MSGBOX_CMD_ARG2_REG_INDEX_EVENT_MASK                 0x00000002
#define NV_MSGBOX_CMD_ARG2_REG_INDEX_MAX                        0x00000002 /* increase when adding
                                                                              new registers */

#define NV_MSGBOX_CMD_ARG2_NVLINK_INFO_AGGREGATE                0x000000FF

#define NV_MSGBOX_CMD_ARG2_ACCESS_WP_MODE_SET_STATE                  23:16
#define NV_MSGBOX_CMD_ARG2_ACCESS_WP_MODE_SET_STATE_DISABLED    0x0000005A
#define NV_MSGBOX_CMD_ARG2_ACCESS_WP_MODE_SET_STATE_ENABLED     0x000000A5

#define NV_MSGBOX_CMD_ARG2_GET_CLOCK_FREQ_INFO_GPCCLK           0x00000000
#define NV_MSGBOX_CMD_ARG2_GET_CLOCK_FREQ_INFO_MEMCLK           0x00000001

#define NV_MSGBOX_CMD_ARG2_ECC_V6_COUNTER_TYPE                       23:16
#define NV_MSGBOX_CMD_ARG2_ECC_V6_COUNTER_TYPE_SRAM                      0
#define NV_MSGBOX_CMD_ARG2_ECC_V6_COUNTER_TYPE_DRAM                      1

#define NV_MSGBOX_CMD_ARG2_REMAP_ROW_RAW_CNT_COMBINED           0x00000000
#define NV_MSGBOX_CMD_ARG2_REMAP_ROW_RAW_CNT_UNCORR             0x00000001
#define NV_MSGBOX_CMD_ARG2_REMAP_ROW_RAW_CNT_CORR               0x00000002

#define NV_MSGBOX_CMD_ARG2_REMAP_ROWS_STATE_FLAGS_PAGE0         0x00000000

/*!
 * Arg2 for _GET_PCIE_LINK_INFO
 * Arg1 == _GET_PCIE_LINK_INFO_PAGE_8
 * Return TX EQ parameters
 */
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_8_LANE_IDX          3:0
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_8_SPEED_SELECT      5:4
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_8_SPEED_SELECT_GEN_3 \
                                                                0x00000000
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_8_SPEED_SELECT_GEN_4 \
                                                                0x00000001
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_8_SPEED_SELECT_GEN_5 \
                                                                0x00000002

/*!
 * Arg2 for _GET_PCIE_LINK_INFO
 * Arg1 == _GET_PCIE_LINK_INFO_PAGE_9
 * Return RX EQ parameters
 */
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_9_LANE_IDX          3:0
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_9_SPEED_SELECT      5:4
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_9_SPEED_SELECT_GEN_3 \
                                                                0x00000000
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_9_SPEED_SELECT_GEN_4 \
                                                                0x00000001
#define NV_MSGBOX_CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_9_SPEED_SELECT_GEN_5 \
                                                                0x00000002

/*!
 * Arg2 for _GET_POWER_HINT_INFO
 * ARG1 == _GET_POWER_HINT_INFO_CLK
 * Return Min/Max frequency in MHz
 */
#define NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_CLK_GR         0x00000000
#define NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_CLK_MEM        0x00000001

/*!
 * Arg2 for _GET_POWER_HINT_INFO
 * ARG1 == _GET_POWER_HINT_INFO_PROFILES
 * Return bits represents which profiles are available
 */
#define NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_PROFILES_PAGE_0    0x00000000
#define NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_PROFILES_PAGE_1    0x00000001
#define NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_PROFILES_PAGE_2    0x00000002
#define NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_PROFILES_PAGE_3    0x00000003
#define NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_PROFILES_TOTAL_PAGES        \
            (NV_MSGBOX_CMD_ARG2_GET_POWER_HINT_INFO_PROFILES_PAGE_3 + 1)

// Arg2 for _GPU_PERFORMANCE_MONITORING
#define NV_MSGBOX_CMD_ARG2_GPM_PARTITION_AGGREGATE                0x000000FF
#define NV_MSGBOX_CMD_ARG2_GPM_PARTITION_INDEX                         21:16
#define NV_MSGBOX_CMD_ARG2_GPM_CI_METRICS_REQUESTED                    23:23
#define NV_MSGBOX_CMD_ARG2_GPM_CI_METRICS_REQUESTED_YES           0x00000001
#define NV_MSGBOX_CMD_ARG2_GPM_CI_METRICS_REQUESTED_NO            0x00000000

#define NV_MSGBOX_CMD_ARG2_GPM_MULTIPLIER                              23:16
#define NV_MSGBOX_CMD_ARG2_GPM_MULTIPLIER_1X                      0x00000001
#define NV_MSGBOX_CMD_ARG2_GPM_MULTIPLIER_2X                      0x00000002
#define NV_MSGBOX_CMD_ARG2_GPM_MULTIPLIER_5X                      0x00000005
#define NV_MSGBOX_CMD_ARG2_GPM_MULTIPLIER_10X                     0x0000000A
#define NV_MSGBOX_CMD_ARG2_GPM_MULTIPLIER_100X                    0x00000064


#define NV_MSGBOX_CMD_STATUS                                         28:24
#define NV_MSGBOX_CMD_STATUS_NULL                               0x00000000
#define NV_MSGBOX_CMD_STATUS_ERR_REQUEST                        0x00000001
#define NV_MSGBOX_CMD_STATUS_ERR_OPCODE                         0x00000002
#define NV_MSGBOX_CMD_STATUS_ERR_ARG1                           0x00000003
#define NV_MSGBOX_CMD_STATUS_ERR_ARG2                           0x00000004
#define NV_MSGBOX_CMD_STATUS_ERR_DATA                           0x00000005
#define NV_MSGBOX_CMD_STATUS_ERR_MISC                           0x00000006
#define NV_MSGBOX_CMD_STATUS_ERR_I2C_ACCESS                     0x00000007
#define NV_MSGBOX_CMD_STATUS_ERR_NOT_SUPPORTED                  0x00000008
#define NV_MSGBOX_CMD_STATUS_ERR_NOT_AVAILABLE                  0x00000009
#define NV_MSGBOX_CMD_STATUS_ERR_BUSY                           0x0000000a
#define NV_MSGBOX_CMD_STATUS_ERR_AGAIN                          0x0000000b
#define NV_MSGBOX_CMD_STATUS_ERR_SENSOR_DATA                    0x0000000c
#define NV_MSGBOX_CMD_STATUS_ERR_DISPOSITION                    0x0000000d
#define NV_MSGBOX_CMD_STATUS_PARTIAL_FAILURE                    0x0000001b
#define NV_MSGBOX_CMD_STATUS_ACCEPTED                           0x0000001c
#define NV_MSGBOX_CMD_STATUS_INACTIVE                           0x0000001d
#define NV_MSGBOX_CMD_STATUS_READY                              0x0000001e
#define NV_MSGBOX_CMD_STATUS_SUCCESS                            0x0000001f

#define NV_MSGBOX_CMD_RSVD                                           29:29
#define NV_MSGBOX_CMD_RSVD_INIT                                 0x00000000

#define NV_MSGBOX_CMD_COPY_DATA                                      30:30
#define NV_MSGBOX_CMD_COPY_DATA_OFF                             0x00000000
#define NV_MSGBOX_CMD_COPY_DATA_ON                              0x00000001

#define NV_MSGBOX_CMD_EVENT_PENDING                                  30:30
#define NV_MSGBOX_CMD_EVENT_PENDING_OFF                         0x00000000
#define NV_MSGBOX_CMD_EVENT_PENDING_ON                          0x00000001

#define NV_MSGBOX_CMD_INTR                                           31:31
#define NV_MSGBOX_CMD_INTR_NOT_PENDING                          0x00000000
#define NV_MSGBOX_CMD_INTR_PENDING                              0x00000001
#define NV_MSGBOX_CMD_INTR_CLEAR                                0x00000000

/* For individual requests in a bundle */

#define NV_MSGBOX_CMD_ON_BUNDLE_FAILURE                              31:31
#define NV_MSGBOX_CMD_ON_BUNDLE_FAILURE_CONTINUE                0x00000000
#define NV_MSGBOX_CMD_ON_BUNDLE_FAILURE_STOP                    0x00000001

#define NV_MSGBOX_CMD_DATA_COPY                                       23:0
#define NV_MSGBOX_CMD_DATA_COPY_INIT                            0x00000000
#define NV_MSGBOX_CMD_EXT_STATUS                                      23:0

// NV_MSGBOX_CMD_COPY_SIZE_ENCODING used for certain commands to embed size
// indicators in the output when copy-bit is set in request

#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_READ_DATA_OUT                 0:0
#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_READ_DATA_OUT_SET               1
#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_READ_DATA_OUT_NOT_SET           0

#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_READ_EXT_DATA_OUT             1:1
#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_READ_EXT_DATA_OUT_SET           1
#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_READ_EXT_DATA_OUT_NOT_SET       0

#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_DATA                         23:2
#define NV_MSGBOX_CMD_COPY_SIZE_ENCODING_DATA_INIT              0x00000000

/* Response to NV_MSGBOX_CMD_ARG1_REMAP_ROWS_HISTOGRAM */
#define NV_MSGBOX_CMD_REMAP_ROW_HISTOGRAM_NONE_AVAILABILITY                 15:0

/* Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_THROUGHPUT_* */
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_GRANULARITY                       1:0
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_GRANULARITY_KIB            0x00000000
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_GRANULARITY_MIB            0x00000001
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_GRANULARITY_GIB            0x00000002
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_GRANULARITY_TIB            0x00000003
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_NEW_SAMPLE                        2:2
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_NEW_SAMPLE_FALSE           0x00000000
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_NEW_SAMPLE_TRUE            0x00000001
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_INVALID_DELTA                     3:3
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_INVALID_DELTA_FALSE        0x00000000
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_INVALID_DELTA_TRUE         0x00000001
#define NV_MSGBOX_CMD_NVLINK_INFO_THROUGHPUT_DELTA                            23:4

/*
 * Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_TRAINING_ERR_STATE
 *             NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_RUNTIME_ERR_STATE
 */
#define NV_MSGBOX_CMD_NVLINK_INFO_TRAINING_ERROR_COUNT                        11:0
#define NV_MSGBOX_CMD_NVLINK_INFO_RUNTIME_ERROR_COUNT                        23:12
#define NV_MSGBOX_CMD_NVLINK_INFO_ERROR_COUNT_MAX                                \
            DRF_MASK(NV_MSGBOX_CMD_NVLINK_INFO_TRAINING_ERROR_COUNT)

/**
 * Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_AVAILABILITY
 */
#define NV_MSGBOX_CMD_NVLINK_INFO_GET_NVLINK_INFO_AVAILABILTY_PAGE_0  0x00000000

/**
 * This field is populated as response to the following metrics:
 * - NV_MSGBOX_CMD_ARG1_GPM_METRIC_DRAM_USAGE
 * - NV_MSGBOX_CMD_ARG1_GPM_METRIC_PCIE_TX_BANDWIDTH
 * - NV_MSGBOX_CMD_ARG1_GPM_METRIC_PCIE_RX_BANDWIDTH
 * - NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_RAW_TX_BANDWIDTH
 * - NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_DATA_TX_BANDWIDTH
 * - NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_RAW_RX_BANDWIDTH
 * - NV_MSGBOX_CMD_ARG1_GPM_METRIC_NVLINK_DATA_RX_BANDWIDTH
 */
#define NV_MSGBOX_CMD_GPM_DATA_GRANULARITY                                     2:0
#define NV_MSGBOX_CMD_GPM_DATA_GRANULARITY_B                            0x00000000
#define NV_MSGBOX_CMD_GPM_DATA_GRANULARITY_KIB                          0x00000001
#define NV_MSGBOX_CMD_GPM_DATA_GRANULARITY_MIB                          0x00000002
#define NV_MSGBOX_CMD_GPM_DATA_GRANULARITY_GIB                          0x00000003

/* MSGBOX data, capability dword structure */

#define NV_MSGBOX_DATA_REG                                                    31:0
#define NV_MSGBOX_DATA_CAP_COUNT                                                 6

#define NV_MSGBOX_DATA_CAP_0                                                     0
#define NV_MSGBOX_DATA_CAP_0_TEMP_GPU_0                                        0:0
#define NV_MSGBOX_DATA_CAP_0_TEMP_GPU_0_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_0_TEMP_GPU_0_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_0_TEMP_NVSWITCH_0                                   0:0
#define NV_MSGBOX_DATA_CAP_0_TEMP_NVSWITCH_0_NOT_AVAILABLE              0x00000000
#define NV_MSGBOX_DATA_CAP_0_TEMP_NVSWITCH_0_AVAILABLE                  0x00000001
#define NV_MSGBOX_DATA_CAP_0_TEMP_GPU_1                                        1:1
#define NV_MSGBOX_DATA_CAP_0_TEMP_GPU_1_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_0_TEMP_GPU_1_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_0_TEMP_BOARD                                        4:4
#define NV_MSGBOX_DATA_CAP_0_TEMP_BOARD_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_0_TEMP_BOARD_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_0_TEMP_MEMORY                                       5:5
#define NV_MSGBOX_DATA_CAP_0_TEMP_MEMORY_NOT_AVAILABLE                  0x00000000
#define NV_MSGBOX_DATA_CAP_0_TEMP_MEMORY_AVAILABLE                      0x00000001
#define NV_MSGBOX_DATA_CAP_0_TEMP_PWR_SUPPLY                                   6:6
#define NV_MSGBOX_DATA_CAP_0_TEMP_PWR_SUPPLY_NOT_AVAILABLE              0x00000000
#define NV_MSGBOX_DATA_CAP_0_TEMP_PWR_SUPPLY_AVAILABLE                  0x00000001
#define NV_MSGBOX_DATA_CAP_0_TEMP_T_LIMIT                                      7:7
#define NV_MSGBOX_DATA_CAP_0_TEMP_T_LIMIT_NOT_AVAILABLE                 0x00000000
#define NV_MSGBOX_DATA_CAP_0_TEMP_T_LIMIT_AVAILABLE                     0x00000001
#define NV_MSGBOX_DATA_CAP_0_EXT_TEMP_BITS                                   11:8
#define NV_MSGBOX_DATA_CAP_0_EXT_TEMP_BITS_ZERO                         0x00000000
#define NV_MSGBOX_DATA_CAP_0_EXT_TEMP_BITS_ADT7473                      0x00000002
#define NV_MSGBOX_DATA_CAP_0_EXT_TEMP_BITS_SFXP11_5                     0x00000005
#define NV_MSGBOX_DATA_CAP_0_EXT_TEMP_BITS_SFXP24_8                     0x00000008
#define NV_MSGBOX_DATA_CAP_0_GET_ENERGY_COUNTER_MODULE                       12:12
#define NV_MSGBOX_DATA_CAP_0_GET_ENERGY_COUNTER_MODULE_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_0_GET_ENERGY_COUNTER_MODULE_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_CONTROL_GET                        13:13
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_CONTROL_GET_NOT_AVAILABLE     0x00000000
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_CONTROL_GET_AVAILABLE         0x00000001
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_CONTROL_SET                        14:14
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_CONTROL_SET_NOT_AVAILABLE     0x00000000
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_CONTROL_SET_AVAILABLE         0x00000001
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_INFO_GET                           15:15
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_INFO_GET_NOT_AVAILABLE        0x00000000
#define NV_MSGBOX_DATA_CAP_0_MODULE_LIMIT_INFO_GET_AVAILABLE            0x00000001
#define NV_MSGBOX_DATA_CAP_0_POWER_TOTAL                                     16:16
#define NV_MSGBOX_DATA_CAP_0_POWER_TOTAL_NOT_AVAILABLE                  0x00000000
#define NV_MSGBOX_DATA_CAP_0_POWER_TOTAL_AVAILABLE                      0x00000001
#define NV_MSGBOX_DATA_CAP_0_GPU_PCONTROL                                    17:17
#define NV_MSGBOX_DATA_CAP_0_GPU_PCONTROL_NOT_AVAILABLE                 0x00000000
#define NV_MSGBOX_DATA_CAP_0_GPU_PCONTROL_AVAILABLE                     0x00000001
#define NV_MSGBOX_DATA_CAP_0_GPU_SYSCONTROL                                  18:18
#define NV_MSGBOX_DATA_CAP_0_GPU_SYSCONTROL_NOT_AVAILABLE               0x00000000
#define NV_MSGBOX_DATA_CAP_0_GPU_SYSCONTROL_AVAILABLE                   0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_BITS                                     28:19  // Adjust when adding new bits
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_TLIMIT_BITS                         23:19  // Adjust when adding new bits
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_BITS                                28:24  // Adjust when adding new bits
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_ACOUSTIC_TLIMIT                            19:19
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_ACOUSTIC_TLIMIT_NOT_AVAILABLE         0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_ACOUSTIC_TLIMIT_AVAILABLE             0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SLOWDN_TLIMIT                              20:20
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SLOWDN_TLIMIT_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SLOWDN_TLIMIT_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SHUTDN_TLIMIT                              21:21
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SHUTDN_TLIMIT_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SHUTDN_TLIMIT_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_MEMORY_TLIMIT                              22:22
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_MEMORY_TLIMIT_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_MEMORY_TLIMIT_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_GPU_SW_SLOWDOWN_TLIMIT                     23:23
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_GPU_SW_SLOWDOWN_TLIMIT_NOT_AVAILABLE  0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_GPU_SW_SLOWDOWN_TLIMIT_AVAILABLE      0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_ACOUSTIC                            24:24
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_ACOUSTIC_NOT_AVAILABLE         0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_ACOUSTIC_AVAILABLE             0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SLOWDN                              25:25
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SLOWDN_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SLOWDN_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SHUTDN                              26:26
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SHUTDN_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_SHUTDN_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_MEMORY                              27:27
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_MEMORY_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_MEMORY_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_GPU_SW_SLOWDOWN                     28:28
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_GPU_SW_SLOWDOWN_NOT_AVAILABLE  0x00000000
#define NV_MSGBOX_DATA_CAP_0_THERMP_TEMP_GPU_SW_SLOWDOWN_AVAILABLE      0x00000001
#define NV_MSGBOX_DATA_CAP_0_GET_FABRIC_STATE_FLAGS                          29:29
#define NV_MSGBOX_DATA_CAP_0_GET_FABRIC_STATE_FLAGS_NOT_AVAILABLE       0x00000000
#define NV_MSGBOX_DATA_CAP_0_GET_FABRIC_STATE_FLAGS_AVAILABLE           0x00000001
#define NV_MSGBOX_DATA_CAP_0_POWER_FB                                        30:30
#define NV_MSGBOX_DATA_CAP_0_POWER_FB_NOT_AVAILABLE                     0x00000000
#define NV_MSGBOX_DATA_CAP_0_POWER_FB_AVAILABLE                         0x00000001
#define NV_MSGBOX_DATA_CAP_0_POWER_MODULE                                    31:31
#define NV_MSGBOX_DATA_CAP_0_POWER_MODULE_NOT_AVAILABLE                 0x00000000
#define NV_MSGBOX_DATA_CAP_0_POWER_MODULE_AVAILABLE                     0x00000001

#define NV_MSGBOX_DATA_CAP_1                                                 1
#define NV_MSGBOX_DATA_CAP_1_BOARD_PART_NUM_V1                             0:0
#define NV_MSGBOX_DATA_CAP_1_BOARD_PART_NUM_V1_NOT_AVAILABLE        0x00000000
#define NV_MSGBOX_DATA_CAP_1_BOARD_PART_NUM_V1_AVAILABLE            0x00000001
#define NV_MSGBOX_DATA_CAP_1_OEM_INFO_V1                                   1:1
#define NV_MSGBOX_DATA_CAP_1_OEM_INFO_V1_NOT_AVAILABLE              0x00000000
#define NV_MSGBOX_DATA_CAP_1_OEM_INFO_V1_AVAILABLE                  0x00000001
#define NV_MSGBOX_DATA_CAP_1_SERIAL_NUM_V1                                 2:2
#define NV_MSGBOX_DATA_CAP_1_SERIAL_NUM_V1_NOT_AVAILABLE            0x00000000
#define NV_MSGBOX_DATA_CAP_1_SERIAL_NUM_V1_AVAILABLE                0x00000001
#define NV_MSGBOX_DATA_CAP_1_MARKETING_NAME_V1                             3:3
#define NV_MSGBOX_DATA_CAP_1_MARKETING_NAME_V1_NOT_AVAILABLE        0x00000000
#define NV_MSGBOX_DATA_CAP_1_MARKETING_NAME_V1_AVAILABLE            0x00000001
#define NV_MSGBOX_DATA_CAP_1_GPU_PART_NUM_V1                               4:4
#define NV_MSGBOX_DATA_CAP_1_GPU_PART_NUM_V1_NOT_AVAILABLE          0x00000000
#define NV_MSGBOX_DATA_CAP_1_GPU_PART_NUM_V1_AVAILABLE              0x00000001
#define NV_MSGBOX_DATA_CAP_1_MEMORY_VENDOR_V1                              5:5
#define NV_MSGBOX_DATA_CAP_1_MEMORY_VENDOR_V1_NOT_AVAILABLE         0x00000000
#define NV_MSGBOX_DATA_CAP_1_MEMORY_VENDOR_V1_AVAILABLE             0x00000001
#define NV_MSGBOX_DATA_CAP_1_MEMORY_PART_NUM_V1                            6:6
#define NV_MSGBOX_DATA_CAP_1_MEMORY_PART_NUM_V1_NOT_AVAILABLE       0x00000000
#define NV_MSGBOX_DATA_CAP_1_MEMORY_PART_NUM_V1_AVAILABLE           0x00000001
#define NV_MSGBOX_DATA_CAP_1_BUILD_DATE_V1                                 7:7
#define NV_MSGBOX_DATA_CAP_1_BUILD_DATE_V1_NOT_AVAILABLE            0x00000000
#define NV_MSGBOX_DATA_CAP_1_BUILD_DATE_V1_AVAILABLE                0x00000001
#define NV_MSGBOX_DATA_CAP_1_FIRMWARE_VER_V1                               8:8
#define NV_MSGBOX_DATA_CAP_1_FIRMWARE_VER_V1_NOT_AVAILABLE          0x00000000
#define NV_MSGBOX_DATA_CAP_1_FIRMWARE_VER_V1_AVAILABLE              0x00000001
#define NV_MSGBOX_DATA_CAP_1_VENDOR_ID_V1                                  9:9
#define NV_MSGBOX_DATA_CAP_1_VENDOR_ID_V1_NOT_AVAILABLE             0x00000000
#define NV_MSGBOX_DATA_CAP_1_VENDOR_ID_V1_AVAILABLE                 0x00000001
#define NV_MSGBOX_DATA_CAP_1_DEV_ID_V1                                   10:10
#define NV_MSGBOX_DATA_CAP_1_DEV_ID_V1_NOT_AVAILABLE                0x00000000
#define NV_MSGBOX_DATA_CAP_1_DEV_ID_V1_AVAILABLE                    0x00000001
#define NV_MSGBOX_DATA_CAP_1_SUB_VENDOR_ID_V1                            11:11
#define NV_MSGBOX_DATA_CAP_1_SUB_VENDOR_ID_V1_NOT_AVAILABLE         0x00000000
#define NV_MSGBOX_DATA_CAP_1_SUB_VENDOR_ID_V1_AVAILABLE             0x00000001
#define NV_MSGBOX_DATA_CAP_1_SUB_ID_V1                                   12:12
#define NV_MSGBOX_DATA_CAP_1_SUB_ID_V1_NOT_AVAILABLE                0x00000000
#define NV_MSGBOX_DATA_CAP_1_SUB_ID_V1_AVAILABLE                    0x00000001
#define NV_MSGBOX_DATA_CAP_1_GPU_GUID_V1                                 13:13
#define NV_MSGBOX_DATA_CAP_1_GPU_GUID_V1_NOT_AVAILABLE              0x00000000
#define NV_MSGBOX_DATA_CAP_1_GPU_GUID_V1_AVAILABLE                  0x00000001
#define NV_MSGBOX_DATA_CAP_1_INFOROM_VER_V1                              14:14
#define NV_MSGBOX_DATA_CAP_1_INFOROM_VER_V1_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_1_INFOROM_VER_V1_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_1_MODULE_POWER_LIMIT_V1                       15:15
#define NV_MSGBOX_DATA_CAP_1_MODULE_POWER_LIMIT_V1_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_1_MODULE_POWER_LIMIT_V1_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_1_ECC_V1                                      16:16
#define NV_MSGBOX_DATA_CAP_1_ECC_V1_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_1_ECC_V1_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_1_ECC_V2                                      17:17
#define NV_MSGBOX_DATA_CAP_1_ECC_V2_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_1_ECC_V2_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_1_ECC_V3                                      18:18
#define NV_MSGBOX_DATA_CAP_1_ECC_V3_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_1_ECC_V3_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_1_RET_PG_CNT                                  19:19
#define NV_MSGBOX_DATA_CAP_1_RET_PG_CNT_NOT_AVAILABLE               0x00000000
#define NV_MSGBOX_DATA_CAP_1_RET_PG_CNT_AVAILABLE                   0x00000001
#define NV_MSGBOX_DATA_CAP_1_ECC_V4                                      20:20
#define NV_MSGBOX_DATA_CAP_1_ECC_V4_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_1_ECC_V4_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_1_ECC_V5                                      21:21
#define NV_MSGBOX_DATA_CAP_1_ECC_V5_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_1_ECC_V5_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_1_ACCESS_WP_MODE                              22:22
#define NV_MSGBOX_DATA_CAP_1_ACCESS_WP_MODE_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_1_ACCESS_WP_MODE_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_1_GET_ECC_ENABLED_STATE                       23:23
#define NV_MSGBOX_DATA_CAP_1_GET_ECC_ENABLED_STATE_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_1_GET_ECC_ENABLED_STATE_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_1_GET_GPU_RESET_REQUIRED                      24:24
#define NV_MSGBOX_DATA_CAP_1_GET_GPU_RESET_REQUIRED_NOT_AVAILABLE   0x00000000
#define NV_MSGBOX_DATA_CAP_1_GET_GPU_RESET_REQUIRED_AVAILABLE       0x00000001
#define NV_MSGBOX_DATA_CAP_1_GPU_UTIL_COUNTERS                           25:25
#define NV_MSGBOX_DATA_CAP_1_GPU_UTIL_COUNTERS_NOT_AVAILABLE        0x00000000
#define NV_MSGBOX_DATA_CAP_1_GPU_UTIL_COUNTERS_AVAILABLE            0x00000001
#define NV_MSGBOX_DATA_CAP_1_NVLINK_INFO_STATE_V1                        26:26
#define NV_MSGBOX_DATA_CAP_1_NVLINK_INFO_STATE_V1_NOT_AVAILABLE     0x00000000
#define NV_MSGBOX_DATA_CAP_1_NVLINK_INFO_STATE_V1_AVAILABLE         0x00000001
#define NV_MSGBOX_DATA_CAP_1_NVLINK_ERROR_COUNTERS                       27:27
#define NV_MSGBOX_DATA_CAP_1_NVLINK_ERROR_COUNTERS_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_1_NVLINK_ERROR_COUNTERS_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_1_CLOCK_FREQ_INFO                             28:28
#define NV_MSGBOX_DATA_CAP_1_CLOCK_FREQ_INFO_NOT_AVAILABLE          0x00000000
#define NV_MSGBOX_DATA_CAP_1_CLOCK_FREQ_INFO_AVAILABLE              0x00000001
#define NV_MSGBOX_DATA_CAP_1_GET_MIG_ENABLED_STATE                       29:29
#define NV_MSGBOX_DATA_CAP_1_GET_MIG_ENABLED_STATE_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_1_GET_MIG_ENABLED_STATE_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_1_ECC_V6                                      30:30
#define NV_MSGBOX_DATA_CAP_1_ECC_V6_NOT_AVAILABLE                   0x00000000
#define NV_MSGBOX_DATA_CAP_1_ECC_V6_AVAILABLE                       0x00000001
#define NV_MSGBOX_DATA_CAP_1_CLOCK_THROTTLE_REASON                       31:31
#define NV_MSGBOX_DATA_CAP_1_CLOCK_THROTTLE_REASON_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_1_CLOCK_THROTTLE_REASON_AVAILABLE        0x00000001

#define NV_MSGBOX_DATA_CAP_2                                                2
#define NV_MSGBOX_DATA_CAP_2_GPU_DRIVER                                   0:0
#define NV_MSGBOX_DATA_CAP_2_GPU_DRIVER_AVAILABLE                  0x00000000
#define NV_MSGBOX_DATA_CAP_2_GPU_DRIVER_NOT_AVAILABLE              0x00000001
#define NV_MSGBOX_DATA_CAP_2_NVSWITCH_DRIVER                              0:0
#define NV_MSGBOX_DATA_CAP_2_NVSWITCH_DRIVER_AVAILABLE             0x00000000
#define NV_MSGBOX_DATA_CAP_2_NVSWITCH_DRIVER_NOT_AVAILABLE         0x00000001
#define NV_MSGBOX_DATA_CAP_2_GPU_REQUEST                                  1:1
#define NV_MSGBOX_DATA_CAP_2_GPU_REQUEST_NOT_AVAILABLE             0x00000000
#define NV_MSGBOX_DATA_CAP_2_GPU_REQUEST_AVAILABLE                 0x00000001
#define NV_MSGBOX_DATA_CAP_2_NUM_SCRATCH_BANKS                            4:2
#define NV_MSGBOX_DATA_CAP_2_NUM_SCRATCH_BANKS_NOT_AVAILABLE       0x00000000
#define NV_MSGBOX_DATA_CAP_2_FAN_V1_CONTROL                               5:5
#define NV_MSGBOX_DATA_CAP_2_FAN_V1_CONTROL_NOT_AVAILABLE          0x00000000
#define NV_MSGBOX_DATA_CAP_2_FAN_V1_CONTROL_AVAILABLE              0x00000001
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_LENGTH_V1                            6:6
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_LENGTH_V1_NOT_AVAILABLE       0x00000000
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_LENGTH_V1_AVAILABLE           0x00000001
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_WIDTH_V1                             7:7
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_WIDTH_V1_NOT_AVAILABLE        0x00000000
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_WIDTH_V1_AVAILABLE            0x00000001
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_HEIGHT_V1                            8:8
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_HEIGHT_V1_NOT_AVAILABLE       0x00000000
#define NV_MSGBOX_DATA_CAP_2_PRODUCT_HEIGHT_V1_AVAILABLE           0x00000001
#define NV_MSGBOX_DATA_CAP_2_PCIE_SPEED_V1                                9:9
#define NV_MSGBOX_DATA_CAP_2_PCIE_SPEED_V1_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_2_PCIE_SPEED_V1_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_2_PCIE_WIDTH_V1                              10:10
#define NV_MSGBOX_DATA_CAP_2_PCIE_WIDTH_V1_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_2_PCIE_WIDTH_V1_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_2_TGP_LIMIT_V1                               11:11
#define NV_MSGBOX_DATA_CAP_2_TGP_LIMIT_V1_NOT_AVAILABLE            0x00000000
#define NV_MSGBOX_DATA_CAP_2_TGP_LIMIT_V1_AVAILABLE                0x00000001
#define NV_MSGBOX_DATA_CAP_2_SCRATCH_PAGE_SIZE                          12:12
#define NV_MSGBOX_DATA_CAP_2_SCRATCH_PAGE_SIZE_1024B               0x00000000
#define NV_MSGBOX_DATA_CAP_2_SCRATCH_PAGE_SIZE_256B                0x00000001
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_STATS                            13:13
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_STATS_NOT_AVAILABLE         0x00000000
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_STATS_AVAILABLE             0x00000001
#define NV_MSGBOX_DATA_CAP_2_GET_PCIE_LINK_INFO                         14:14
#define NV_MSGBOX_DATA_CAP_2_GET_PCIE_LINK_INFO_NOT_AVAILABLE      0x00000000
#define NV_MSGBOX_DATA_CAP_2_GET_PCIE_LINK_INFO_AVAILABLE          0x00000001
#define NV_MSGBOX_DATA_CAP_2_GET_GPU_DRAIN_AND_RESET_RECOMMENDED   15:15
#define NV_MSGBOX_DATA_CAP_2_GET_GPU_DRAIN_AND_RESET_RECOMMENDED_NOT_AVAILABLE   0x00000000
#define NV_MSGBOX_DATA_CAP_2_GET_GPU_DRAIN_AND_RESET_RECOMMENDED_AVAILABLE       0x00000001
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_THROUGHPUT                          16:16
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_THROUGHPUT_NOT_AVAILABLE       0x00000000
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_THROUGHPUT_AVAILABLE           0x00000001
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_LINK_STATE_V2                       17:17
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_LINK_STATE_V2_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_LINK_STATE_V2_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_SUBLINK_WIDTH                       18:18
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_SUBLINK_WIDTH_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_SUBLINK_WIDTH_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_2_GET_ENERGY_COUNTER                              19:19
#define NV_MSGBOX_DATA_CAP_2_GET_ENERGY_COUNTER_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_2_GET_ENERGY_COUNTER_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_PENDING                               20:20
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_PENDING_NOT_AVAILABLE            0x00000000
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_PENDING_AVAILABLE                0x00000001
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_HISTOGRAM                             21:21
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_HISTOGRAM_NOT_AVAILABLE          0x00000000
#define NV_MSGBOX_DATA_CAP_2_REMAP_ROW_HISTOGRAM_AVAILABLE              0x00000001
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_TRAINING_ERROR_STATE                      22:22
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_TRAINING_ERROR_STATE_NOT_AVAILABLE   0x00000000
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_TRAINING_ERROR_STATE_AVAILABLE       0x00000001
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_RUNTIME_ERROR_STATE                       23:23
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_RUNTIME_ERROR_STATE_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_RUNTIME_ERROR_STATE_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_2_GET_PCIE_LINK_TARGET_SPEED                            25:25
#define NV_MSGBOX_DATA_CAP_2_GET_PCIE_LINK_TARGET_SPEED_NOT_AVAILABLE         0x00000000
#define NV_MSGBOX_DATA_CAP_2_GET_PCIE_LINK_TARGET_SPEED_AVAILABLE             0x00000001
#define NV_MSGBOX_DATA_CAP_2_CURRENT_PSTATE                                  26:26
#define NV_MSGBOX_DATA_CAP_2_CURRENT_PSTATE_NOT_AVAILABLE               0x00000000
#define NV_MSGBOX_DATA_CAP_2_CURRENT_PSTATE_AVAILABLE                   0x00000001
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_AVAILABILITY                       27:27
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_AVAILABILITY_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_2_NVLINK_INFO_AVAILABILITY_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_2_FRU_PART_NUMBER_V1                             31:31
#define NV_MSGBOX_DATA_CAP_2_FRU_PART_NUMBER_V1_NOT_AVAILABLE          0x00000000
#define NV_MSGBOX_DATA_CAP_2_FRU_PART_NUMBER_V1_AVAILABLE              0x00000001

#define NV_MSGBOX_DATA_CAP_4                                                     4
#define NV_MSGBOX_DATA_CAP_4_HW_VIOLATION_TIME                                 0:0
#define NV_MSGBOX_DATA_CAP_4_HW_VIOLATION_TIME_NOT_AVAILABLE            0x00000000
#define NV_MSGBOX_DATA_CAP_4_HW_VIOLATION_TIME_AVAILABLE                0x00000001
#define NV_MSGBOX_DATA_CAP_4_SW_VIOLATION_TIME                                 1:1
#define NV_MSGBOX_DATA_CAP_4_SW_VIOLATION_TIME_NOT_AVAILABLE            0x00000000
#define NV_MSGBOX_DATA_CAP_4_SW_VIOLATION_TIME_AVAILABLE                0x00000001
#define NV_MSGBOX_DATA_CAP_4_SW_POWER_VIOLATION_TIME                           2:2
#define NV_MSGBOX_DATA_CAP_4_SW_POWER_VIOLATION_TIME_NOT_AVAILABLE      0x00000000
#define NV_MSGBOX_DATA_CAP_4_SW_POWER_VIOLATION_TIME_AVAILABLE          0x00000001
#define NV_MSGBOX_DATA_CAP_4_SW_THERMAL_VIOLATION_TIME                         3:3
#define NV_MSGBOX_DATA_CAP_4_SW_THERMAL_VIOLATION_TIME_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_DATA_CAP_4_SW_THERMAL_VIOLATION_TIME_AVAILABLE        0x00000001
#define NV_MSGBOX_DATA_CAP_4_UTILIZATION_RATE                                  4:4
#define NV_MSGBOX_DATA_CAP_4_UTILIZATION_RATE_NOT_AVAILABLE             0x00000000
#define NV_MSGBOX_DATA_CAP_4_UTILIZATION_RATE_AVAILABLE                 0x00000001
#define NV_MSGBOX_DATA_CAP_4_GET_DRIVER_EVENT_MSG                              5:5
#define NV_MSGBOX_DATA_CAP_4_GET_DRIVER_EVENT_MSG_NOT_AVAILABLE         0x00000000
#define NV_MSGBOX_DATA_CAP_4_GET_DRIVER_EVENT_MSG_AVAILABLE             0x00000001
#define NV_MSGBOX_DATA_CAP_4_REQUEST_BUNDLING                                  6:6
#define NV_MSGBOX_DATA_CAP_4_REQUEST_BUNDLING_NOT_AVAILABLE             0x00000000
#define NV_MSGBOX_DATA_CAP_4_REQUEST_BUNDLING_AVAILABLE                 0x00000001
#define NV_MSGBOX_DATA_CAP_4_SET_DEVICE_DISABLE                                7:7
#define NV_MSGBOX_DATA_CAP_4_SET_DEVICE_DISABLE_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_DATA_CAP_4_SET_DEVICE_DISABLE_AVAILABLE               0x00000001
#define NV_MSGBOX_DATA_CAP_4_SET_ECC_MODE                                      8:8
#define NV_MSGBOX_DATA_CAP_4_SET_ECC_MODE_NOT_AVAILABLE                 0x00000000
#define NV_MSGBOX_DATA_CAP_4_SET_ECC_MODE_AVAILABLE                     0x00000001
#define NV_MSGBOX_DATA_CAP_4_SET_MIG_MODE                                    10:10
#define NV_MSGBOX_DATA_CAP_4_SET_MIG_MODE_NOT_AVAILABLE                 0x00000000
#define NV_MSGBOX_DATA_CAP_4_SET_MIG_MODE_AVAILABLE                     0x00000001
#define NV_MSGBOX_DATA_CAP_4_FAN_CURVE_POINTS_GET_SET                        11:11
#define NV_MSGBOX_DATA_CAP_4_FAN_CURVE_POINTS_GET_SET_NOT_AVAILABLE     0x00000000
#define NV_MSGBOX_DATA_CAP_4_FAN_CURVE_POINTS_GET_SET_AVAILABLE         0x00000001
#define NV_MSGBOX_DATA_CAP_4_POWER_HINT                                                  22:22
#define NV_MSGBOX_DATA_CAP_4_POWER_HINT_NOT_AVAILABLE                               0x00000000
#define NV_MSGBOX_DATA_CAP_4_POWER_HINT_AVAILABLE                                   0x00000001
#define NV_MSGBOX_DATA_CAP_4_DRIVER_VERSION_V1                                           23:23
#define NV_MSGBOX_DATA_CAP_4_DRIVER_VERSION_V1_NOT_AVAILABLE                        0x00000000
#define NV_MSGBOX_DATA_CAP_4_DRIVER_VERSION_V1_AVAILABLE                            0x00000001
#define NV_MSGBOX_DATA_CAP_4_GPU_PERFORMANCE_MONITORING                                  24:24
#define NV_MSGBOX_DATA_CAP_4_GPU_PERFORMANCE_MONITORING_NOT_AVAILABLE               0x00000000
#define NV_MSGBOX_DATA_CAP_4_GPU_PERFORMANCE_MONITORING_AVAILABLE                   0x00000001
#define NV_MSGBOX_DATA_CAP_4_DEM_BY_SEQNUM_TIMESTAMP                                     26:26
#define NV_MSGBOX_DATA_CAP_4_DEM_BY_SEQNUM_TIMESTAMP_NOT_AVAILABLE                  0x00000000
#define NV_MSGBOX_DATA_CAP_4_DEM_BY_SEQNUM_TIMESTAMP_AVAILABLE                      0x00000001
#define NV_MSGBOX_DATA_CAP_4_CONFIGURE_PROGRAMMABLE_EDPP                                 30:30
#define NV_MSGBOX_DATA_CAP_4_CONFIGURE_PROGRAMMABLE_EDPP_NOT_AVAILABLE              0x00000000
#define NV_MSGBOX_DATA_CAP_4_CONFIGURE_PROGRAMMABLE_EDPP_AVAILABLE                  0x00000001
#define NV_MSGBOX_DATA_CAP_4_MAX_DRAM_CAPACITY_V1                                        31:31
#define NV_MSGBOX_DATA_CAP_4_MAX_DRAM_CAPACITY_V1_NOT_AVAILABLE                     0x00000000
#define NV_MSGBOX_DATA_CAP_4_MAX_DRAM_CAPACITY_V1_AVAILABLE                         0x00000001

#define NV_MSGBOX_DATA_CAP_5                                                                 5
#define NV_MSGBOX_DATA_CAP_5_MEMORY_CAPACITY_UTILIZATION                                   6:6
#define NV_MSGBOX_DATA_CAP_5_MEMORY_CAPACITY_UTILIZATION_NOT_AVAILABLE              0x00000000
#define NV_MSGBOX_DATA_CAP_5_MEMORY_CAPACITY_UTILIZATION_AVAILABLE                  0x00000001

#define NV_MSGBOX_DATA_CAP_5_SRAM_ERROR_THRESHOLD_EXCEEDED                                 9:9
#define NV_MSGBOX_DATA_CAP_5_SRAM_ERROR_THRESHOLD_EXCEEDED_NOT_AVAILABLE            0x00000000
#define NV_MSGBOX_DATA_CAP_5_SRAM_ERROR_THRESHOLD_EXCEEDED_AVAILABLE                0x00000001

/* ECC counters */
#define NV_MSGBOX_DATA_ECC_CNT_16BIT_DBE                             31:16
#define NV_MSGBOX_DATA_ECC_CNT_16BIT_SBE                              16:0
#define NV_MSGBOX_DATA_ECC_CNT_8BIT_DBE                              23:16
#define NV_MSGBOX_DATA_ECC_CNT_8BIT_SBE                                7:0

#define NV_MSGBOX_DATA_ECC_V5_COUNT_HEADER                             7:0

#define NV_MSGBOX_DATA_ECC_V5_COUNT_HDR_ID                             1:0
#define NV_MSGBOX_DATA_ECC_V5_COUNT_HDR_ID_INVAL                         0
#define NV_MSGBOX_DATA_ECC_V5_COUNT_HDR_ID_ADDR                          1
#define NV_MSGBOX_DATA_ECC_V5_COUNT_HDR_ID_REGN                          2

#define NV_MSGBOX_DATA_ECC_V5_COUNT_METADATA                         31:16

#define NV_MSGBOX_DATA_ECC_V5_ERR_BUF_ERR_TYPE                         7:0

#define NV_MSGBOX_DATA_ECC_V5_ERR_BUF_ERR_TYPE_ID                      0:0
#define NV_MSGBOX_DATA_ECC_V5_ERR_BUF_ERR_TYPE_ID_UNCORR                 0
#define NV_MSGBOX_DATA_ECC_V5_ERR_BUF_ERR_TYPE_ID_CORR                   1

#define NV_MSGBOX_DATA_ECC_V5_ERR_BUF_METADATA                       31:16

#define NV_MSGBOX_DATA_ECC_V5_METADATA_STRUCT_ID                     21:16
#define NV_MSGBOX_DATA_ECC_V5_METADATA_STRUCT_ID_LRF                     0
#define NV_MSGBOX_DATA_ECC_V5_METADATA_STRUCT_ID_L1DATA                  1
#define NV_MSGBOX_DATA_ECC_V5_METADATA_STRUCT_ID_L1TAG                   2
#define NV_MSGBOX_DATA_ECC_V5_METADATA_STRUCT_ID_CBU                     3
#define NV_MSGBOX_DATA_ECC_V5_METADATA_STRUCT_ID_LTC                     4
#define NV_MSGBOX_DATA_ECC_V5_METADATA_STRUCT_ID_DRAM                    5

#define NV_MSGBOX_DATA_ECC_V5_METADATA_LOCATION_ID                   26:22
#define NV_MSGBOX_DATA_ECC_V5_METADATA_SUBLOCATION_ID                31:27

/* ECC state flags */
#define NV_MSGBOX_DATA_ECC_V6_STATE_FLAGS                                    31:0

#define NV_MSGBOX_DATA_ECC_V6_STATE_FLAGS_SRAM_ERROR_THRESHOLD_EXCEEDED       0:0
#define NV_MSGBOX_DATA_ECC_V6_STATE_FLAGS_SRAM_ERROR_THRESHOLD_EXCEEDED_FALSE   0
#define NV_MSGBOX_DATA_ECC_V6_STATE_FLAGS_SRAM_ERROR_THRESHOLD_EXCEEDED_TRUE    1

/* NV_MSGBOX_CMD_OPCODE_SCRATCH_COPY src offset argument */
#define NV_MSGBOX_DATA_COPY_SRC_OFFSET                                 7:0

/* NV_MSGBOX_CMD_ARG2_REG_INDEX_SCRATCH_PAGE register layout */
#define NV_MSGBOX_DATA_SCRATCH_PAGE_DST                                7:0
#define NV_MSGBOX_DATA_SCRATCH_PAGE_SRC                               15:8

/* Async request status codes returned in the data register */
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS                            7:0

#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_SUCCESS                         0x00000000
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_CARD_NOT_PRESENT          0x00000001
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_DUAL_LINK_INUSE           0x00000002
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_GENERIC                   0x00000003
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_GPU_NOT_FULL_POWER        0x00000004
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_IN_USE                    0x00000005
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INSUFFICIENT_RESOURCES    0x00000006
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_ACCESS_TYPE       0x00000007
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_ARGUMENT          0x00000008
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_BASE              0x00000009
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_CHANNEL           0x0000000A
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_CLASS             0x0000000B
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_CLIENT            0x0000000C
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_COMMAND           0x0000000D
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_DATA              0x0000000E
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_DEVICE            0x0000000F
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_DMA_SPECIFIER     0x00000010
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_EVENT             0x00000011
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_FLAGS             0x00000012
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_FUNCTION          0x00000013
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_HEAP              0x00000014
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_INDEX             0x00000015
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_LIMIT             0x00000016
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_METHOD            0x00000017
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OBJECT_BUFFER     0x00000018
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OBJECT_ERROR      0x00000019
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OBJECT_HANDLE     0x0000001A
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OBJECT_NEW        0x0000001B
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OBJECT_OLD        0x0000001C
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OBJECT_PARENT     0x0000001D
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OFFSET            0x0000001E
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OWNER             0x0000001F
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_PARAM_STRUCT      0x00000020
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_PARAMETER         0x00000021
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_POINTER           0x00000022
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_REGISTRY_KEY      0x00000023
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_STATE             0x00000024
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_STRING_LENGTH     0x00000025
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_XLATE             0x00000026
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_IRQ_NOT_FIRING            0x00000027
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_MULTIPLE_MEMORY_TYPES     0x00000028
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_NOT_SUPPORTED             0x00000029
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_OPERATING_SYSTEM          0x0000002A
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_PROTECTION_FAULT          0x0000002B
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_TIMEOUT                   0x0000002C
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_TOO_MANY_PRIMARIES        0x0000002D
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_IRQ_EDGE_TRIGGERED        0x0000002E
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_OPERATION         0x0000002F
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_NOT_COMPATIBLE            0x00000030
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_MORE_PROCESSING_REQUIRED  0x00000031
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INSUFFICIENT_PERMISSIONS  0x00000032
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_TIMEOUT_RETRY             0x00000033
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_NOT_READY                 0x00000034
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_GPU_IS_LOST               0x00000035
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_IN_FULLCHIP_RESET         0x00000036
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_LOCK_STATE        0x00000037
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_ADDRESS           0x00000038
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INVALID_IRQ_LEVEL         0x00000039
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_MEMORY_TRAINING_FAILED    0x00000040
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_BUSY_RETRY                0x00000041
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_INSUFFICIENT_POWER        0x00000042
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_OBJECT_NOT_FOUND          0x00000043
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_BUFFER_TOO_SMALL          0x00000044
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_RESET_REQUIRED            0x00000045
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_PRIV_SEC_VIOLATION        0x00000046
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_DEFERRED                        0x00000047
#define NV_MSGBOX_DATA_ASYNC_REQ_STATUS_ERROR_FREQ_NOT_SUPPORTED        0x00000048

/* Response to NV_MSGBOX_CMD_OPCODE_GET_POWER_CONNECTOR_STATE */
#define NV_MSGBOX_DATA_POWER_CONNECTED                                         0:0
#define NV_MSGBOX_DATA_POWER_CONNECTED_SUFFICIENT                       0x00000000
#define NV_MSGBOX_DATA_POWER_CONNECTED_INSUFFICIENT                     0x00000001

/* Response to NV_MSGBOX_CMD_OPCODE_GET_PAGE_RETIREMENT_STATS */
#define NV_MSGBOX_DATA_RETIRED_PAGES_CNT_SBE                                   7:0
#define NV_MSGBOX_DATA_RETIRED_PAGES_CNT_DBE                                  15:8

/*
 * Response to
 * NV_MSGBOX_CMD_ARG1_GET_MISC_ECC_ENABLED_STATE
 * NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_FLAGS_PAGE_0
 */
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_SUPPORTED                             0:0
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_SUPPORTED_OFF                  0x00000000
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_SUPPORTED_ON                   0x00000001
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_CURRENT                               1:1
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_CURRENT_OFF                    0x00000000
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_CURRENT_ON                     0x00000001
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_PENDING                               2:2
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_PENDING_OFF                    0x00000000
#define NV_MSGBOX_DATA_ECC_ENABLED_STATE_PENDING_ON                     0x00000001

/* Response to NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_FLAGS_PAGE_0 */
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_SUPPORTED                             3:3
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_SUPPORTED_OFF                  0x00000000
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_SUPPORTED_ON                   0x00000001
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_CURRENT                               4:4
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_CURRENT_OFF                    0x00000000
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_CURRENT_ON                     0x00000001
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_PENDING                               5:5
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_PENDING_OFF                    0x00000000
#define NV_MSGBOX_DATA_MIG_ENABLED_STATE_PENDING_ON                     0x00000001

/* Response to NV_MSGBOX_CMD_ARG1_GET_MISC_DEVICE_FLAGS_PAGE_0 */
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DRIVER_STATUS                              6:6
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DRIVER_STATUS_NOT_RUNNING           0x00000000
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DRIVER_STATUS_RUNNING               0x00000001
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DEVICE_STATUS                              7:7
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DEVICE_STATUS_ENABLED               0x00000000
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DEVICE_STATUS_DISABLED              0x00000001
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_FABRIC_MANAGER_STATUS                     10:8
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_FABRIC_MANAGER_STATUS_NOT_RUNNING   0x00000000
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_FABRIC_MANAGER_STATUS_RUNNING       0x00000001
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_FABRIC_MANAGER_STATUS_TIMEOUT       0x00000002
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_FABRIC_MANAGER_STATUS_ERROR         0x00000003
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_SOURCE                           13:11
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_SOURCE_NONE                 0x00000000
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_SOURCE_LOCAL_DEVICE         0x00000001
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_SOURCE_PEER_DEVICE          0x00000002
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_SOURCE_IN_BAND              0x00000003
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_SOURCE_OUT_OF_BAND          0x00000004
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_REASON                           16:14
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_REASON_NONE                 0x00000000
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_REASON_CLIENT_OVERRIDE      0x00000001
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_REASON_TRUNK_LINK_FAILED    0x00000002
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_REASON_ACCESS_LINK_FAILED   0x00000003
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DISABLE_REASON_UNSPECIFIED_FAILURE  0x00000004
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DRIVER_RELOAD                            17:17
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DRIVER_RELOAD_NOT_PENDING           0x00000000
#define NV_MSGBOX_DATA_DEVICE_DISABLE_STATE_DRIVER_RELOAD_PENDING               0x00000001

/* Response to NV_MSGBOX_CMD_ARG1_ACCESS_WP_MODE_ACTION_GET */
#define NV_MSGBOX_DATA_ACCESS_WP_MODE_GET_STATE                                7:0
#define NV_MSGBOX_DATA_ACCESS_WP_MODE_GET_STATE_DISABLED                0x0000005A
#define NV_MSGBOX_DATA_ACCESS_WP_MODE_GET_STATE_ENABLED                 0x000000A5

/*
 * Response to
 * NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_RESET_REQUIRED
 * NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_FLAGS_PAGE_1
 */
#define NV_MSGBOX_DATA_GPU_RESET_REQUIRED                                      0:0
#define NV_MSGBOX_DATA_GPU_RESET_REQUIRED_OFF                           0x00000000
#define NV_MSGBOX_DATA_GPU_RESET_REQUIRED_ON                            0x00000001
#define NV_MSGBOX_DATA_GPU_DRAIN_AND_RESET_RECOMMENDED                         1:1
#define NV_MSGBOX_DATA_GPU_DRAIN_AND_RESET_RECOMMENDED_NO               0x00000000
#define NV_MSGBOX_DATA_GPU_DRAIN_AND_RESET_RECOMMENDED_YES              0x00000001

/**
 * Response to
 * NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_INFO_PAGE_3
 */
#define NV_MSGBOX_DATA_GET_CLOCK_FREQ_INFO_PAGE_3_CURRENT_PSTATE                 3:0
#define NV_MSGBOX_DATA_GET_CLOCK_FREQ_INFO_PAGE_3_CURRENT_PSTATE_INVALID  0x0000000F

/**
 * Response to
 * NV_MSGBOX_CMD_ARG1_GET_CLOCK_THROTTLE_REASON
 */
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON                                  31:0
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON_NONE                       0x00000000
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON_SW_POWER_CAP               0x00000001
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON_HW_SLOWDOWN                0x00000002
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON_HW_THERMAL_SLOWDOWN        0x00000004
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON_HW_POWER_BREAK_SLOWDOWN    0x00000008
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON_SYNC_BOOST                 0x00000010
#define NV_MSGBOX_DATA_CLOCK_THROTTLE_REASON_SW_THERMAL_SLOWDOWN        0x00000020

/*
 * Number of Nvlink data outputs (dataOut, extData) for
 * NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK queries
 */
#define NV_MSGBOX_DATA_NVLINK_INFO_DATA_WIDTH                                    2

/*
 * Encoding for Invalid data
 */
#define NV_MSGBOX_DATA_NVLINK_INFO_DATA_INVALID                         0xFFFFFFFF

/* Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK_STATE_V1 */
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V1__SIZE                          32
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V1_DOWN                   0x00000000
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V1_UP                     0x00000001
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V1_INVALID                0x000000ff

/* Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK_STATE_V2 */
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2(i)            (3+(i)*4):(0+(i)*4)
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2__SIZE                           8
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2_OFF                    0x00000000
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2_SAFE                   0x00000001
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2_ACTIVE                 0x00000002
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2_ERROR                  0x00000003
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2_L1_LOW_POWER           0x00000004
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2_NVLINK_DISABLED        0x00000005
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_STATE_V2_INVALID                0x000000ff

/* Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_LINK_BANDWIDTH (in Mps) */
#define NV_MSGBOX_DATA_NVLINK_INFO_LINK_BANDWIDTH__SIZE                          1
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_INVALID                         0xFFFFFFFF
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_00000_MBPS                               0
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_16000_MBPS                           16000
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_19200_MBPS                           19200
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_20000_MBPS                           20000
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_25000_MBPS                           25000
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_25781_MBPS                           25781
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_32000_MBPS                           32000
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_40000_MBPS                           40000
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_50000_MBPS                           50000
#define NV_MSGBOX_DATA_NVLINK_LINE_RATE_53125_MBPS                           53125

/* Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_SUBLINK_WIDTH */
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_WIDTH(i)            (5+(i)*6):(0+(i)*6)
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_WIDTH__SIZE                           5
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_WIDTH_INVALID                0x000000ff
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_TX_WIDTH                            2:0
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_TX_WIDTH_0                   0x00000000
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_TX_WIDTH_1                   0x00000001
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_TX_WIDTH_2                   0x00000002
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_TX_WIDTH_4                   0x00000003
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_TX_WIDTH_8                   0x00000004
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_TX_WIDTH_INVALID             0x00000007
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_RX_WIDTH                            5:3
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_RX_WIDTH_0                   0x00000000
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_RX_WIDTH_1                   0x00000001
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_RX_WIDTH_2                   0x00000002
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_RX_WIDTH_4                   0x00000003
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_RX_WIDTH_8                   0x00000004
#define NV_MSGBOX_DATA_NVLINK_INFO_SUBLINK_RX_WIDTH_INVALID             0x00000007

/*
 * Response to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_TRAINING_ERR_STATE
 *             NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_RUNTIME_ERR_STATE
 */
#define NV_MSGBOX_DATA_NVLINK_INFO_ERROR_STATE__SIZE                            32
#define NV_MSGBOX_DATA_NVLINK_INFO_ERROR_STATE_INVALID                  0x000000ff
#define NV_MSGBOX_DATA_NVLINK_INFO_ERROR_STATE_NO_ERROR                 0x00000000
#define NV_MSGBOX_DATA_NVLINK_INFO_ERROR_STATE_ERROR                    0x00000001

/* Respones to NV_MSGBOX_CMD_ARG2_REMAP_ROW_RAW_CNT_COMBINED */
#define NV_MSGBOX_DATA_REMAP_ROW_UNCORR_CNT                             10:0
#define NV_MSGBOX_DATA_REMAP_ROW_UNCORR_CNT_EXCESS                     11:11
#define NV_MSGBOX_DATA_REMAP_ROW_UNCORR_CNT_EXCESS_FALSE                   0
#define NV_MSGBOX_DATA_REMAP_ROW_UNCORR_CNT_EXCESS_TRUE                    1
#define NV_MSGBOX_DATA_REMAP_ROW_CORR_CNT                              22:12
#define NV_MSGBOX_DATA_REMAP_ROW_CORR_CNT_EXCESS                       23:23
#define NV_MSGBOX_DATA_REMAP_ROW_CORR_CNT_EXCESS_FALSE                     0
#define NV_MSGBOX_DATA_REMAP_ROW_CORR_CNT_EXCESS_TRUE                      1

/* Response to NV_MSGBOX_CMD_ARG1_REMAP_ROWS_STATE_FLAGS */
#define NV_MSGBOX_DATA_REMAP_ROW_STATE_FLAGS_PAGE0_FAILED_REMAPPING          0:0
#define NV_MSGBOX_DATA_REMAP_ROW_STATE_FLAGS_PAGE0_FAILED_REMAPPING_FALSE      0
#define NV_MSGBOX_DATA_REMAP_ROW_STATE_FLAGS_PAGE0_FAILED_REMAPPING_TRUE       1
#define NV_MSGBOX_DATA_REMAP_ROW_STATE_FLAGS_PAGE0_PENDING_REMAPPING         1:1
#define NV_MSGBOX_DATA_REMAP_ROW_STATE_FLAGS_PAGE0_PENDING_REMAPPING_FALSE     0
#define NV_MSGBOX_DATA_REMAP_ROW_STATE_FLAGS_PAGE0_PENDING_REMAPPING_TRUE      1

/* Response to NV_MSGBOX_CMD_ARG1_REMAP_ROWS_HISTOGRAM */
#define NV_MSGBOX_DATA_REMAP_ROW_HISTOGRAM_LOW_AVAILABILITY                 15:0
#define NV_MSGBOX_DATA_REMAP_ROW_HISTOGRAM_PARTIAL_AVAILABILITY            31:16

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_0 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_SPEED                    2:0
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_SPEED_UNKNOWN     0x00000000
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_SPEED_2500_MTPS   0x00000001
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_SPEED_5000_MTPS   0x00000002
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_SPEED_8000_MTPS   0x00000003
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_SPEED_16000_MTPS  0x00000004
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_SPEED_32000_MTPS  0x00000005
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH                    6:4
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH_UNKNOWN     0x00000000
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH_X1          0x00000001
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH_X2          0x00000002
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH_X4          0x00000003
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH_X8          0x00000004
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH_X16         0x00000005
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_LINK_WIDTH_X32         0x00000006
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_NONFATAL_ERROR_COUNT         15:8
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_FATAL_ERROR_COUNT           23:16
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_0_UNSUPP_REQ_COUNT            31:24

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_1 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_1_L0_TO_RECOVERY_COUNT         31:0

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_2 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_2_REPLAY_ROLLOVER_COUNT        15:0
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_2_NAKS_RCVD_COUNT             31:16

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_3 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED                    2:0
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED_UNKNOWN     0x00000000
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED_2500_MTPS   0x00000001
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED_5000_MTPS   0x00000002
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED_8000_MTPS   0x00000003
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED_16000_MTPS  0x00000004
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED_32000_MTPS  0x00000005
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_3_TARGET_LINK_SPEED_RESERVED           2:0

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_4 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_4_TX_COUNT                            31:0

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_6 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE                          4:0
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_DETECT            0x00000000
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_POLLING           0x00000001
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_CONFIGURATION     0x00000002
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_RECOVERY          0x00000003
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_RECOVERY_EQZN     0x00000004
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L0                0x00000005
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L0S               0x00000006
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L1                0x00000007
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L1_PLL_PD         0x00000008
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L2                0x00000009
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L1_CPM            0x0000000a
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L1_1              0x0000000b
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_L1_2              0x0000000c
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_HOT_RESET         0x0000000d
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_LOOPBACK          0x0000000e
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_DISABLED          0x0000000f
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_LINK_DOWN         0x00000010
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_LINK_READY        0x00000011
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_LANES_IN_SLEEP    0x00000012
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_6_LTSSM_STATE_ILLEGAL           0x0000001f

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_8 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_8_EQ_TX_LOCAL_PRESET                   3:0
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_8_EQ_TX_LOCAL_USE_PRESET               4:4
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_8_EQ_TX_LOCAL_FS                      10:5
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_8_EQ_TX_LOCAL_LF                     16:11

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_9 */
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_9_EQ_RX_REMOTE_PRESET                  3:0
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_9_EQ_RX_REMOTE_USE_PRESET              4:4
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_9_EQ_RX_REMOTE_FS                     10:5
#define NV_MSGBOX_DATA_PCIE_LINK_INFO_PAGE_9_EQ_RX_REMOTE_LF                    16:11

/*
 * Input for NV_MSGBOX_CMD_OPCODE_GPU_PERFORMANCE_MONITORING. Value is valid
 * only if Arg2 != GPM_PARTITION_AGGREGATE and Arg2.Bit7 == 1
 */
#define NV_MSGBOX_DATA_GPM_COMPUTE_INSTANCE_INDEX                             15:8

/*
 * The following three fields correspond to the data which is interpreted
 * differently depending on GPM Metric specified in Arg1.
 */
#define NV_MSGBOX_DATA_GPM_NVLINK_INDEX                                        7:0
#define NV_MSGBOX_DATA_GPM_NVDEC_INSTANCE                                      7:0
#define NV_MSGBOX_DATA_GPM_NVJPG_INSTANCE                                      7:0

/* MSGBOX Extended Data Register */
#define NV_MSGBOX_EXT_DATA_REG                                            31:0

/* Respones to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_0 */
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_0_CORRECTABLE_ERROR_COUNT  15:0

/* Respones to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_1 */
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_1_REPLAY_COUNT             31:0

/* Respones to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_2 */
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_2_NAKS_SENT_COUNT          15:0

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_4 */
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_4_RX_COUNT                 31:0

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_8 */
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_8_EQ_TX_LOCAL_PRECUR        5:0
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_8_EQ_TX_LOCAL_MAINCUR      11:6
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_8_EQ_TX_LOCAL_POSTCUR     17:12

/* Response to NV_MSGBOX_CMD_ARG1_GET_PCIE_LINK_INFO_PAGE_9 */
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_9_EQ_RX_REMOTE_PRECUR       5:0
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_9_EQ_RX_REMOTE_MAINCUR     11:6
#define NV_MSGBOX_EXT_DATA_PCIE_LINK_INFO_PAGE_9_EQ_RX_REMOTE_POSTCUR    17:12

/* Response to NV_MSGBOX_CMD_ARG1_REMAP_ROWS_HISTOGRAM */
#define NV_MSGBOX_EXT_DATA_REMAP_ROW_HISTOGRAM_MAX_AVAILABILITY            31:16
#define NV_MSGBOX_EXT_DATA_REMAP_ROW_HISTOGRAM_HIGH_AVAILABILITY            15:0

/* SysId info per type sizes */
#define NV_MSGBOX_SYSID_DATA_SIZE_BOARD_PART_NUM_V1             24
#define NV_MSGBOX_SYSID_DATA_SIZE_OEM_INFO_V1                   24
#define NV_MSGBOX_SYSID_DATA_SIZE_SERIAL_NUM_V1                 16
#define NV_MSGBOX_SYSID_DATA_SIZE_MARKETING_NAME_V1             24
#define NV_MSGBOX_SYSID_DATA_SIZE_GPU_PART_NUM_V1               16
#define NV_MSGBOX_SYSID_DATA_SIZE_MEMORY_VENDOR_V1               1
#define NV_MSGBOX_SYSID_DATA_SIZE_MEMORY_PART_NUM_V1            20
#define NV_MSGBOX_SYSID_DATA_SIZE_BUILD_DATE_V1                  4
#define NV_MSGBOX_SYSID_DATA_SIZE_FIRMWARE_VER_V1               14
#define NV_MSGBOX_SYSID_DATA_SIZE_VENDOR_ID_V1                   2
#define NV_MSGBOX_SYSID_DATA_SIZE_DEV_ID_V1                      2
#define NV_MSGBOX_SYSID_DATA_SIZE_SUB_VENDOR_ID_V1               2
#define NV_MSGBOX_SYSID_DATA_SIZE_SUB_ID_V1                      2
#define NV_MSGBOX_SYSID_DATA_SIZE_GPU_GUID_V1                   16
#define NV_MSGBOX_SYSID_DATA_SIZE_INFOROM_VER_V1                16
#define NV_MSGBOX_SYSID_DATA_SIZE_PRODUCT_LENGTH_V1              4
#define NV_MSGBOX_SYSID_DATA_SIZE_PRODUCT_WIDTH_V1               4
#define NV_MSGBOX_SYSID_DATA_SIZE_PRODUCT_HEIGHT_V1              4
#define NV_MSGBOX_SYSID_DATA_SIZE_PCIE_SPEED_V1                  1
#define NV_MSGBOX_SYSID_DATA_SIZE_PCIE_WIDTH_V1                  1
#define NV_MSGBOX_SYSID_DATA_SIZE_TGP_LIMIT_V1                   4
#define NV_MSGBOX_SYSID_DATA_SIZE_FRU_PART_NUMBER_V1            20
#define NV_MSGBOX_SYSID_DATA_SIZE_MODULE_POWER_LIMIT_V1          4
#define NV_MSGBOX_SYSID_DATA_SIZE_MAX_DRAM_CAPACITY_V1           4

/*!
 * Response to NV_MSGBOX_CMD_ARG1_GET_POWER_HINT_INFO_CLK
 * 16-bit frequency in MHz
 */
#define NV_MSGBOX_DATA_POWER_HINT_INFO_CLK_GR_MIN                           15:0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_CLK_GR_MAX                          31:16
#define NV_MSGBOX_DATA_POWER_HINT_INFO_CLK_MEM_MIN                          15:0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_CLK_MEM_MAX                         31:16

/*!
 * Response to NV_MSGBOX_CMD_ARG1_GET_POWER_HINT_INFO_TEMP
 * 16-bit frequency in MHz
 */
#define NV_MSGBOX_DATA_POWER_HINT_INFO_TEMP_MIN                             15:0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_TEMP_MAX                            31:16

/*!
 * Response to _ARG1_GET_POWER_HINT_INFO_PROFILES, 1 page consists of data and
 * ext data, then we can support up to 256 profiles
 * Bit index represents profile ID
 */
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_PERF                           0:0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_PERF_NOT_AVAILABLE               0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_PERF_AVAILABLE                   1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_HIGH_K                         1:1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_HIGH_K_NOT_AVAILABLE             0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_HIGH_K_AVAILABLE                 1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_LOW_K                          2:2
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_LOW_K_NOT_AVAILABLE              0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_DMMA_LOW_K_AVAILABLE                  1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_HMMA                                3:3
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_HMMA_NOT_AVAILABLE                    0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_HMMA_AVAILABLE                        1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_IMMA                                4:4
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_IMMA_NOT_AVAILABLE                    0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_IMMA_AVAILABLE                        1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_SGEMM                               5:5
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_SGEMM_NOT_AVAILABLE                   0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_SGEMM_AVAILABLE                       1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_TRANSFORMER                         6:6
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_TRANSFORMER_NOT_AVAILABLE             0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_TRANSFORMER_AVAILABLE                 1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_0                   7:7
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_0_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_0_AVAILABLE           1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_1                   8:8
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_1_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_1_AVAILABLE           1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_2                   9:9
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_2_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_2_AVAILABLE           1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_3                 10:10
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_3_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_3_AVAILABLE           1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_4                 11:11
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_4_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_4_AVAILABLE           1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_5                 12:12
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_5_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_5_AVAILABLE           1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_6                 13:13
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_6_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_6_AVAILABLE           1
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_7                 14:14
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_7_NOT_AVAILABLE       0
#define NV_MSGBOX_DATA_POWER_HINT_INFO_PROFILES_PAGE_0_CUSTOMER_CUSTOM_7_AVAILABLE           1

#define NV_MSGBOX_DATA_DYN_SYS_INFO_SIZE_DRIVER_VERSION                                     64

/* Event types */
typedef enum
{
    NV_MSGBOX_EVENT_TYPE_SERVER_RESTART_COLD = 0,
    NV_MSGBOX_EVENT_TYPE_GPU_RESET_REQUIRED,
    NV_MSGBOX_EVENT_TYPE_DRIVER_ERROR_MESSAGE_OLDEST,
    NV_MSGBOX_EVENT_TYPE_TGP_LIMIT_SET_SUCCESS,
    NV_MSGBOX_EVENT_TYPE_CLOCK_LIMIT_SET_SUCCESS,
    NV_MSGBOX_EVENT_TYPE_ECC_TOGGLE_SUCCESS,
    NV_MSGBOX_EVENT_TYPE_MIG_TOGGLE_SUCCESS,
    NV_MSGBOX_EVENT_TYPE_SERVER_RESTART_WARM,
    NV_MSGBOX_EVENT_TYPE_DRIVER_ERROR_MESSAGE_NEW,
    NV_MSGBOX_EVENT_TYPE_MODULE_LIMIT_SET_SUCCESS,
    NV_MSGBOX_NUM_EVENTS,                       /* insert new event types before this line */
}   NvMsgboxEventType;

/* Legacy event names for compatipility */
#define NV_MSGBOX_EVENT_TYPE_SERVER_RESTART         NV_MSGBOX_EVENT_TYPE_SERVER_RESTART_COLD
#define NV_MSGBOX_EVENT_TYPE_DRIVER_ERROR_MESSAGE   NV_MSGBOX_EVENT_TYPE_DRIVER_ERROR_MESSAGE_OLDEST

/* Bit mask of all defined events */
#define NV_MSGBOX_EVENT_TYPE__ALL   (NVBIT(NV_MSGBOX_NUM_EVENTS) - 1)

/* ----------------------- Set Master Capabilities ---------------------------- */

//
// SET_MASTER_CAPS
//
// Descripiton:
//     Set/declare SMBPBI Master capabilities.
//
// Common Encoding:
//   7             0               7             0
//  .-+-+-+-+-+-+-+-.             .-+-+-+-+-+-+-+-.
//  |     CLASS     | ARG1        |   CAP_INDEX   | ARG2
//  `-+-+-+-+-+-+-+-'             `-+-+-+-+-+-+-+-'
//
// The SMBPBI Master capabilities are logically grouped into classes (specified
// in arg1). Capabilities are sent 32 at a time (dword index stored in arg2)
// per-class (typically once during system-initialization).
//
// Classes:
//     _GPU_REQ - Class of capability bits corresponding 1:1 with the GPU-
//                Request opcodes the master supports.
//
#define NV_MSGBOX_CMD_SET_MASTER_CAPS_ARG1_CLASS                               7:0
#define NV_MSGBOX_CMD_SET_MASTER_CAPS_ARG1_CLASS_GPU_REQ                0x00000000

#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_COUNT                                 1

#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_0                                  31:0
#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_0_CPU_PCONTROL                      0:0
#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_0_CPU_PCONTROL_NOT_AVAILABLE 0x00000000
#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_0_CPU_PCONTROL_AVAILABLE     0x00000001
#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_0_SYS_PCONTROL                      1:1
#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_0_SYS_PCONTROL_NOT_AVAILABLE 0x00000000
#define NV_MSGBOX_DATA_MASTER_CAPS_GPU_REQ_0_SYS_PCONTROL_AVAILABLE     0x00000001

/* ------------------ GPU Performance Control (PCONTROL) ---------------------- */

//
// GPU_PCONTROL arg1, arg2, and data encoding (fields are relative to zero, not
// relative to their position in the COMMAND/DATA registers).
//

//
//  ARG1/ARG2 Encoding
//
//    7       3 2   0               7             0
//   .-+-+-+-+-+-+-+-.             .-+-+-+-+-+-+-+-.
//   |  TARGET | ACT | ARG1        |0 0 0 0 0 0 0 0| ARG2 (reserved)
//   `-+-+-+-+-+-+-+-'             `-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_ACTION                                 2:0
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_ACTION_GET_INFO                 0x00000000
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_ACTION_GET_LIMIT                0x00000001
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_ACTION_SET_LIMIT                0x00000002
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_ACTION_GET_STATUS               0x00000003
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_TARGET                                 7:3
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_TARGET_VPSTATE                  0x00000000
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_TARGET_POWER_SMBIDX             0x00000001
#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG2_RSVD                                   7:0

#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_SET_ACTION(arg1, action)                 \
    (arg1) = FLD_SET_DRF(_MSGBOX_CMD, _GPU_PCONTROL_ARG1, _ACTION,               \
        action, (arg1))

#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_GET_ACTION(arg1)                         \
    DRF_VAL(_MSGBOX_CMD, _GPU_PCONTROL_ARG1, _ACTION, (arg1))

#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_SET_TARGET(arg1, target)                 \
    (arg1) = FLD_SET_DRF(_MSGBOX_CMD, _GPU_PCONTROL_ARG1, _TARGET,               \
        target, (arg1))

#define NV_MSGBOX_CMD_GPU_PCONTROL_ARG1_GET_TARGET(arg1)                         \
    DRF_VAL(_MSGBOX_CMD, _GPU_PCONTROL_ARG1, _TARGET, (arg1))

//
// Getting GPU vPstate Information
//
// Inputs:
//     ARG1 - {TARGET=VPSTATE, ACTION=GET_INFO}
//     ARG2 - Unused/reserved (must be zero)
//
// Outputs:
//     DATA - Current min/max range of externally available vpstates
//            (vPmin=fastest, vPmax=slowest)
//
// Encoding:
//      31                           16 15            8 7             0
//     .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
//     |0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0|      MIN      |      MAX      | DATA
//     `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_INFO_MAX                   7:0
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_INFO_MIN                  15:8
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_INFO_RSVD                31:16

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_INFO_SET_MAX(data, max)      \
    (data) = FLD_SET_DRF_NUM(_MSGBOX_CMD, _GPU_PCONTROL_DATA,                    \
        _VPSTATE_GET_INFO_MAX, (max), (data))

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_INFO_GET_MAX(data)           \
    DRF_VAL(_MSGBOX_CMD, _GPU_PCONTROL_DATA, _VPSTATE_GET_INFO_MAX, (data))

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_INFO_SET_MIN(data, min)      \
    (data) = FLD_SET_DRF_NUM(_MSGBOX_CMD, _GPU_PCONTROL_DATA,                    \
        _VPSTATE_GET_INFO_MIN, (min), (data))

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_INFO_GET_MIN(data)           \
    DRF_VAL(_MSGBOX_CMD, _GPU_PCONTROL_DATA, _VPSTATE_GET_INFO_MIN, (data))

//
// Getting GPU vPstate Limits
//
// Inputs:
//     ARG1 - {TARGET=VPSTATE, ACTION=GET_LIMIT}
//     ARG2 - Unused/reserved (must be zero)
//
// Outputs:
//     DATA - Last requested vpstate limit/value, zero when no limit has been set.
//
// Encoding:
//      31                                            8 7             0
//     .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
//     |0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0|     VALUE     | DATA
//     `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_LIMIT_VALUE                7:0
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_LIMIT_RSVD                31:8

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_LIMIT_SET_VALUE(data, value) \
    (data) = FLD_SET_DRF_NUM(_MSGBOX_CMD, _GPU_PCONTROL_DATA,                    \
        _VPSTATE_GET_LIMIT_VALUE, (value), (data))

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_LIMIT_GET_VALUE(data)        \
    DRF_VAL(_MSGBOX_CMD, _GPU_PCONTROL_DATA, _VPSTATE_GET_LIMIT_VALUE, (data))

//
// Setting GPU vPstate Limits:
//
// Inputs:
//     ARG1 - {TARGET=VPSTATE, ACTION=SET_LIMIT}
//     ARG2 - Unused/reserved (must be zero)
//     DATA - Desired vpstate limit/value between vPmin and vPmax (inclusive),
//            zero to clear active limits.
// Outputs:
//     none
//
// Encoding:
//      31                                            8 7             0
//     .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
//     |0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0|     VALUE     | DATA
//     `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_SET_LIMIT_VALUE                7:0
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_SET_LIMIT_VALUE_CLEAR   0x00000000
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_SET_LIMIT_VALUE_INIT    0x00000000
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_SET_LIMIT_RSVD                31:8

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_SET_LIMIT_SET_VALUE(data, limit) \
    (data) = FLD_SET_DRF_NUM(_MSGBOX_CMD, _GPU_PCONTROL_DATA,                    \
        _VPSTATE_SET_LIMIT_VALUE, (limit), (data))

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_SET_LIMIT_GET_VALUE(data)        \
    DRF_VAL(_MSGBOX_CMD, _GPU_PCONTROL_DATA, _VPSTATE_SET_LIMIT_VALUE, (data))

//
// Getting GPU vPstate Status
//
// Inputs:
//     ARG1 - {TARGET=VPSTATE, ACTION=GET_STATUS}
//     ARG2 - Unused/reserved (must be zero)
//
// Outputs:
//     DATA - Current vPstate
//
// Encoding:
//      31                                            8 7             0
//     .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
//     |0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0|    CURRENT    | DATA
//     `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_STATUS_CURRENT             7:0
#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_STATUS_RSVD               31:8

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_STATUS_SET_CURRENT(data, curr) \
    (data) = FLD_SET_DRF_NUM(_MSGBOX_CMD, _GPU_PCONTROL_DATA,                      \
        _VPSTATE_GET_STATUS_CURRENT, (curr), (data))

#define NV_MSGBOX_CMD_GPU_PCONTROL_DATA_VPSTATE_GET_STATUS_GET_CURRENT(data)       \
    DRF_VAL(_MSGBOX_CMD, _GPU_PCONTROL_DATA, _VPSTATE_GET_STATUS_CURRENT, (data))


/* ------------------------- GPU System Control ------------------------------- */

//
// GPU_SYSCONTROL
//
// Description:
//     Command sent to the GPU to set/remove a system-imposed operating limit
//     on a specific system-parameter.
//
// Common Encoding:
//      7         2 1 0               7             0
//     .-+-+-+-+-+-+-+-.             .-+-+-+-+-+-+-+-.
//     |   TARGET  |ACT| ARG1        |0 0 0 0 0 0 0 0| ARG2 (reserved)
//     `-+-+-+-+-+-+-+-'             `-+-+-+-+-+-+-+-'
//
// Notes:
//     - TARGET accepts NV0000_CTRL_SYSTEM_PARAM_* definitions
//     - arg1, arg2, and data encoding (fields are relative to zero, not relative
//       to their position in the DATA_OUT register).
//
#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_ACTION                          1:0
#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_ACTION_CLEAR_LIMIT       0x00000000
#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_ACTION_SET_LIMIT         0x00000001
#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_TARGET                          7:2
#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG2_RSVD                            7:0

#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_SET_ACTION(arg1, action)          \
    (arg1) = FLD_SET_DRF(_MSGBOX_CMD, _GPU_SYSCONTROL_ARG1, _ACTION,        \
        action, (arg1))

#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_GET_ACTION(arg1)                  \
    DRF_VAL(_MSGBOX_CMD, _GPU_SYSCONTROL_ARG1, _ACTION, (arg1))

#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_SET_TARGET(arg1, target)          \
    (arg1) = FLD_SET_DRF_NUM(_MSGBOX_CMD, _GPU_SYSCONTROL_ARG1, _TARGET,    \
        target, (arg1))

#define NV_MSGBOX_CMD_GPU_SYSCONTROL_ARG1_GET_TARGET(arg1)                  \
    DRF_VAL(_MSGBOX_CMD, _GPU_SYSCONTROL_ARG1, _TARGET, (arg1))

//
// Setting a System-Parameter Operating-Limit:
//
// Inputs:
//     ARG1 - {TARGET=NV_MSGBOX_SYS_PARAM_*, ACTION=SET_LIMIT}
//     ARG2 - Unused/reserved (must be zero)
//     DATA - Desired limit/value.
//
// Outputs:
//     none
//
// Encoding:
//      31                                                            0
//     .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
//     |                              VALUE                            | DATA
//     `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_CMD_GPU_SYSCONTROL_DATA_SET_LIMIT_VALUE                31:0

#define NV_MSGBOX_CMD_GPU_SYSCONTROL_DATA_SET_LIMIT_SET_VALUE(data, limit)  \
    (data) = FLD_SET_DRF_NUM(_MSGBOX_CMD, _GPU_SYSCONTROL_DATA,             \
        _SET_LIMIT_VALUE, (limit), (data))

#define NV_MSGBOX_CMD_GPU_SYSCONTROL_DATA_PARAM_SET_LIMIT_GET_VALUE(data)   \
    DRF_VAL(_MSGBOX_CMD, _GPU_SYSCONTROL_DATA, _SET_LIMIT_VALUE, (data))

//
// Host side scratch memory buffer
// This memory buffer is allocated by the host CPU in its system memory
// and is used for passing additional arguments and results, that don't
// fit in the command/status  and data registers due to their size.
// This memory can be accessed by the BMC SMBus master through
// dedicated requests.
//
// Page size is various on different SMBPBI servers.
// Capability dword[2] bit 12 shows size in bytes
//
// _1024 --> 0xFF dwords
// _256  --> 0x40 dowrds
//
#define NV_MSGBOX_SCRATCH_PAGE_SIZE_1024_D                                  0xFF
#define NV_MSGBOX_SCRATCH_PAGE_SIZE_256_D                                   0x40

//
// TODO Once switch soc/pmu/oobtest remove the reference to this, this will be
// removed.
//
#define NV_MSGBOX_SCRATCH_PAGE_SIZE             1024
#define NV_MSGBOX_SCRATCH_NUM_PAGES_P              2    // expressed as a power of 2
                                                        // must be >= 2

// This cap code plugs into NV_MSGBOX_DATA_CAP_2_NUM_SCRATCH_BANKS
#define NV_MSGBOX_SCRATCH_NUM_PAGES_CAP_CODE                    \
                    (NV_MSGBOX_SCRATCH_NUM_PAGES_P > 0 ?        \
                        NV_MSGBOX_SCRATCH_NUM_PAGES_P - 1 : 0)

#define NV_MSGBOX_SCRATCH_NUM_PAGES             (1 << NV_MSGBOX_SCRATCH_NUM_PAGES_P)
#define NV_MSGBOX_SCRATCH_BUFFER_SIZE           (NV_MSGBOX_SCRATCH_NUM_PAGES \
                                                * NV_MSGBOX_SCRATCH_PAGE_SIZE)

// Auxiliary definitions related to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_STATE_SPEED
#define NV_MSGBOX_NVLINK_STATE_BITS_PER_NVLINK      4
#define NV_MSGBOX_NVLINK_STATE_NVLINKS_PER_REGISTER (32 / NV_MSGBOX_NVLINK_STATE_BITS_PER_NVLINK)
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLINKS_VOLTA    6
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLINKS_TURING   2
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLIPT_GA100            3
#define NV_MSGBOX_NVLINK_STATE_NUM_LINKS_PER_NVLIPT_NVL30  4
#define NV_MSGBOX_NVLINK_SUBLINK_WIDTH_MAX_NVL30           4
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLINKS_GA100           \
    (NV_MSGBOX_NVLINK_STATE_NUM_NVLIPT_GA100 *             \
     NV_MSGBOX_NVLINK_STATE_NUM_LINKS_PER_NVLIPT_NVL30)
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLIPT_GA10X            1
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLINKS_GA10X           \
    (NV_MSGBOX_NVLINK_STATE_NUM_NVLIPT_GA10X *             \
     NV_MSGBOX_NVLINK_STATE_NUM_LINKS_PER_NVLIPT_NVL30)
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLIPT_LR10             9
#define NV_MSGBOX_NVLINK_STATE_NUM_NVLINKS_LR10            \
    (NV_MSGBOX_NVLINK_STATE_NUM_NVLIPT_LR10 *              \
     NV_MSGBOX_NVLINK_STATE_NUM_LINKS_PER_NVLIPT_NVL30)
#define NV_MSGBOX_NVLINK_STATE_NUM_REGISTERS(arch)  \
    ((NV_MSGBOX_NVLINK_STATE_NUM_NVLINKS##arch + NV_MSGBOX_NVLINK_STATE_NVLINKS_PER_REGISTER - 1) \
    / NV_MSGBOX_NVLINK_STATE_NVLINKS_PER_REGISTER)

// Auxiliary definitions related to NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_THROUGHPUT_*
#define NV_MSGBOX_NVLINK_THROUGHPUT_COUNTER_INDEX_DATA_TX         0x00000000
#define NV_MSGBOX_NVLINK_THROUGHPUT_COUNTER_INDEX_DATA_RX         0x00000001
#define NV_MSGBOX_NVLINK_THROUGHPUT_COUNTER_INDEX_RAW_TX          0x00000002
#define NV_MSGBOX_NVLINK_THROUGHPUT_COUNTER_INDEX_RAW_RX          0x00000003
#define NV_MSGBOX_NVLINK_THROUGHPUT_COUNTER_INDEX_MAX             0x00000004

#if !defined(NV_MSGBOX_NO_PARAM_STRUCTS)
//
// Parameter structures for asynchronous requests
// passed in scratch space
//

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_TGP_LIMIT_CONTROL_GET,
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_TGP_LIMIT_CONTROL_SET,
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_MODULE_LIMIT_CONTROL_GET and
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_MODULE_LIMIT_CONTROL_SET
 */
typedef struct
{
    NvU32  flags;
    /*!
     * If flags:_PERSIST is _ON, make limitCurrInput persistent,
     * surviving the driver reload and the system reboot.
     *
     * If flags:_CLEAR is _ON, it will clear the TGP limit. The
     * persistence still depends on persist flag.
     */
#define NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS_FLAGS_PERSIST              0:0
#define NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS_FLAGS_PERSIST_OFF   0x00000000
#define NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS_FLAGS_PERSIST_ON    0x00000001
#define NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS_FLAGS_CLEAR                1:1
#define NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS_FLAGS_CLEAR_OFF     0x00000000
#define NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS_FLAGS_CLEAR_ON      0x00000001

    /*!
     * Current total GPU power limit value to enforce, requested by the
     * SMBPBI client, expressed in milliwatts.
     * Must always be within range imposed by the current policy.
     * 0xffffffff is returned, if the limit has not been set by the SMBPBI client.
     */
    NvU32  limitCurrInput;

    /*!
     * Currently arbitrated total GPU power limit value after taking
     * into account limits, requested by all clients, expressed
     * in milliwatts.
     */
    NvU32  limitCurrOutput;
} NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_TGP_LIMIT_INFO_GET and
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_MODULE_LIMIT_INFO_GET
 */
typedef struct
{
    /*!
     * Current total GPU power limit lower and upper bounds and the
     * default setting, expressed in milliwatts.
     * These constraints must be observed, when the limit
     * is being set with either
     * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_TGP_LIMIT_CONTROL_SET or
     * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_PMGR_PWR_MODULE_LIMIT_CONTROL_SET.
     */
    NvU32  limitMin;
    NvU32  limitMax;
    NvU32  limitDefault;
} NV_MSGBOX_PMGR_PWR_POWER_LIMIT_INFO_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_THERMAL_FAN_V1_COUNT_GET
 */
typedef struct
{
    NvU32   fanCount;   //<! number of fans on the board
} NV_MSGBOX_THERMAL_FAN_V1_COUNT_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_THERMAL_FAN_V1_INFO_GET
 */
typedef struct
{
    NvU32   fanIndex;       //<! fan ID to be filled by the caller
    NvU32   fanProperties;  //<! fan properties, detailed below

//! Fan level metric
#define NV_MSGBOX_FAN_V1_PROP_LEVEL_METRIC                        0:0
#define NV_MSGBOX_FAN_V1_PROP_LEVEL_METRIC_PWM             0x00000000
#define NV_MSGBOX_FAN_V1_PROP_LEVEL_METRIC_RPM             0x00000001

//! Fan tachometer availability
#define NV_MSGBOX_FAN_V1_PROP_TACH                                1:1
#define NV_MSGBOX_FAN_V1_PROP_TACH_NOT_AVAILABLE           0x00000000
#define NV_MSGBOX_FAN_V1_PROP_TACH_AVAILABLE               0x00000001

//! Fan tachometer bounds (min/max) availability
#define NV_MSGBOX_FAN_V1_PROP_TACH_BOUNDS                         2:2
#define NV_MSGBOX_FAN_V1_PROP_TACH_BOUNDS_NOT_AVAILABLE    0x00000000
#define NV_MSGBOX_FAN_V1_PROP_TACH_BOUNDS_AVAILABLE        0x00000001

    NvU16   fanLevelMin;    //<! minimum fan level
    NvU16   fanLevelMax;    //<! maximum fan level
    NvU16   fanTachMin;     //<! tachometer minimum
    NvU16   fanTachMax;     //<! tachometer maximum
} NV_MSGBOX_THERMAL_FAN_V1_INFO_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_THERMAL_FAN_V1_STATUS_GET
 */
typedef struct
{
        NvU32   fanIndex;           //<! fan ID to be filled by the caller
        /*!
         * Current fan level and speed
         */
        NvU8  levelCurr;            //<! current fan level, 0 if n/a
        NvU16 speedCurr;            //<! current fan speed (RPM), 0 if no tach

} NV_MSGBOX_THERMAL_FAN_V1_STATUS_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_OVERCLOCKING_LIMIT_CONTROL
 */
typedef struct
{
        NvU32   limitType;                  //<! Argument representing VPSTATE/CLOCK

//! CLOCK/VPSTATE types - add types below
#define NV_MSGBOX_OVERCLOCKING_LIMIT_TYPE_MAX_CUSTOMER_BOOST_VPSTATE 0x00000001

        NvU32   maxClkLimitMHz;             //<! Clock limit

} NV_MSGBOX_OVERCLOCKING_LIMIT_CONTROL_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_ENERGY_COUNTER_STATUS_GET
 */
typedef struct
{
        NV_DECLARE_ALIGNED(NvU64 energyCountermJ, 8);   //<! Energy counter value

} NV_MSGBOX_ENERGY_COUNTER_STATUS_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_VIOLATION_COUNTERS_STATUS_GET
 */
typedef struct
{
                NvU32 policyMask;
        /*!
         * Argument representing policies whose violation times
         * are being requested/provided
         */

//! NVBIT OFFSET <> POLICY - add types below
#define NV_MSGBOX_VIOLATION_COUNTERS_TYPE_ALL_SUPPORTED_POLICIES    0xFFFFFFFF  //-1 represents All Supported policies
#define NV_MSGBOX_VIOLATION_COUNTERS_TYPE_HW_VIOLATION              0
#define NV_MSGBOX_VIOLATION_COUNTERS_TYPE_SW_GLOBAL_VIOLATION       1
#define NV_MSGBOX_VIOLATION_COUNTERS_TYPE_SW_POWER_VIOLATION        2
#define NV_MSGBOX_VIOLATION_COUNTERS_TYPE_SW_THERMAL_VIOLATION      3

        NV_DECLARE_ALIGNED(NvU64 violationTime[32], 8); //<! Violation counter value

} NV_MSGBOX_VIOLATION_COUNTERS_STATUS_PARAMS;

//! Number of clock domains to track for gathering utilization data.
#define NV_MSGBOX_UTILIZATION_RATE_CLOCK_DOMAIN_COUNT               2

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_UTILIZATION_RATE_GET
 */
typedef struct
{
    NvU32 gpuUtil;   //<! Percent of sample that the Graphics/SM unit remains busy.
    NvU32 memUtil;   //<! Percent of sample that the memory unit remains busy.

} NV_MSGBOX_UTILIZATION_RATE_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_OOB_CLOCK_LIMIT_SET
 *
 * Masks of NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_<XYZ>.
 */
typedef struct
{
    NvU32 flags;
    /*!
     * If flags: _CLEAR is _ON, it will clear the limits. Otherwise,
     * set the clock limits.
     *
     * If flags: _PERSIST is _ON, set/clear limits persistently,
     * surviving the driver reload and the system reboot.
     *
     */
#define NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_PERSIST              0:0
#define NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_PERSIST_OFF   0x00000000
#define NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_PERSIST_ON    0x00000001
#define NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_CLEAR                1:1
#define NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_CLEAR_OFF     0x00000000
#define NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_CLEAR_ON      0x00000001

    /*!
     * Graphics clock limit in MHz
     */
    NvU32 clkMinFreqMHz;
    NvU32 clkMaxFreqMHz;
} NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_MEMORY_CAPACITY_UTILIZATION_GET
 */
typedef struct
{
    NvU32 capacityReservedMiB; //<! Device memory (in MiB) reserved for system use.
    NvU32 capacityUsedMiB;     //<! Allocated device memory (in MiB).
} NV_MSGBOX_MEMORY_CAPACITY_UTILIZATION_PARAMS;

/*
 * Parameters for NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_TEST_MESSAGE_SEND
 */
#define NV_MSGBOX_TEST_MESSAGE_SEND_STRING_SIZE_MAX 32
/*
 * If the message starts with this character, the message
 * will not be sent to the kernel.
 */
#define NV_MSGBOX_TEST_MESSAGE_SEND_SILENT_CHAR     '@'
typedef struct
{
    NvU8    string[NV_MSGBOX_TEST_MESSAGE_SEND_STRING_SIZE_MAX];
} NV_MSGBOX_TEST_MESSAGE_SEND_PARAMS;

/*
 * Individual request structure definition for bundled requests
 */
typedef struct
{
    NvU32   cmdStatus;  //<! Command/Status
    NvU32   dataIn;     //<! Data In
    NvU32   dataOut;    //<! Data Out
    NvU32   extData;    //<! Extended Data
} NV_MSGBOX_IND_REQUEST;

/*!
 * Driver Event Message record
 */
typedef struct
{
    NvU8    recordSize;     //<! in 32-bit words
    NvU8    xidId;          //<! event type Id (Xid)
    NvU8    flags;          //<! bit 0: some DEMs after this one
                            //<!        have been lost
                            //<! bit 1: the text message has been
                            //<! truncated
    NvU8    xidIdExt;       //<! event type Id (Xid) extension
    NvU32   seqNumber;      //<! record sequential number
    NvU32   timeStamp;      //<! seconds since the epoch UTC

    /*
     * Some Windows builds include this file, and older Microsoft
     * compilers won't accept a zero size array in this context:
     * "C4200 nonstandard extension used: zero-sized array in struct/union",
     * despite this being a legitimate C99.
     * We need to find a way to fix this, if we ever decide to
     * implement DEM/Xid logging for Windows.
     */
    NvU8    textMessage[];  //<! NUL terminated

} NV_MSGBOX_DEM_RECORD;
#define NV_MSGBOX_DEM_RECORD_FLAGS_OVFL                 0:0
#define NV_MSGBOX_DEM_RECORD_FLAGS_OVFL_NOT_SET  0x00000000
#define NV_MSGBOX_DEM_RECORD_FLAGS_OVFL_SET      0x00000001
#define NV_MSGBOX_DEM_RECORD_FLAGS_TRUNC                1:1
#define NV_MSGBOX_DEM_RECORD_FLAGS_TRUNC_NOT_SET 0x00000000
#define NV_MSGBOX_DEM_RECORD_FLAGS_TRUNC_SET     0x00000001

/*!
 * Result Disposition Rule word layout
 */
#define NV_MSGBOX_DISP_RULE_REQ_IDX                   2:0
#define NV_MSGBOX_DISP_RULE_SRC_REG                   4:3
#define NV_MSGBOX_DISP_RULE_SRC_REG_STATUS     0x00000000
#define NV_MSGBOX_DISP_RULE_SRC_REG_DATA       0x00000001
#define NV_MSGBOX_DISP_RULE_SRC_REG_EXT_DATA   0x00000002
#define NV_MSGBOX_DISP_RULE_SRC_REG_MAX        0x00000002
#define NV_MSGBOX_DISP_RULE_SRC_RIGHT_BIT             9:5
#define NV_MSGBOX_DISP_RULE_SRC_WIDTH               14:10
#define NV_MSGBOX_DISP_RULE_DST_REG                 16:15
#define NV_MSGBOX_DISP_RULE_DST_REG_STATUS     0x00000000
#define NV_MSGBOX_DISP_RULE_DST_REG_DATA       0x00000001
#define NV_MSGBOX_DISP_RULE_DST_REG_EXT_DATA   0x00000002
#define NV_MSGBOX_DISP_RULE_DST_REG_MAX        0x00000002
#define NV_MSGBOX_DISP_RULE_DST_RIGHT_BIT           21:17

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_CLOCK_LIMIT_GET
 * Masks of NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS_FLAGS_<XYZ>.
 */
typedef struct {
    /*!
     * Graphics clock limit in MHz
     * if there is no cap, return
     * NV_MSGBOX_CLOCK_LIMIT_GET_PARAMS_CLOCK_LIMIT_NOT_SET
     */
    NvU16 oobClientMin;
    NvU16 oobClientMax;
    NvU16 enforcedMin;
    NvU16 enforcedMax;
#define NV_MSGBOX_CLOCK_LIMIT_GET_PARAMS_CLOCK_LIMIT_NOT_SET      0xffff
} NV_MSGBOX_CLOCK_LIMIT_GET_PARAMS;

/*!
 * This structure is used to hold parameters for
 * NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_DEVICE_MODE_CONTROL
 */
typedef struct
{
    NvU16    modeType;  //<! Mode type
#define NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS_MODE_TYPE_ECC           0x00000000
#define NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS_MODE_TYPE_MIG           0x00000001
#define NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS_MODE_TYPE_DISABLE       0x00000002

    NvU16    flags;     //<! flags
#define NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS_FLAGS_CONTROL                  0:0
#define NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS_FLAGS_CONTROL_CLEAR     0x00000000
#define NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS_FLAGS_CONTROL_SET       0x00000001

} NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS;

//
//  This is the maximum number of curve points this API will be able to carry.
//  With the understanding that the maximum number of points currently
//  implemented in the RM is 3, we are leaving 2 additional slots as a room
//  for possible future growth, so that we don't have to spin the API definition
//  in case we ever want to implement more than 3 points.
//
#define NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_MAX_POINTS    5

typedef struct
{
    NvS16   temperature;        //<! degrees C, signed 16 bit value

    union   {
        NvU16   level;          //<! Fan PWM setting in the range 0..100
                                //<! 0:       no rotation
                                //<! 100:     full speed
        NvU16   rpmValue;       //<! tach speed in RPM
    }       fanSetting;
} NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINT;

#define NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_ACTION_GET            0
#define NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_ACTION_GET_DEFAULT    1
#define NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_ACTION_SET            2
typedef struct
{
    NvU8    action;             //<! [in]
                                //<! NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_ACTION_GET:
                                //<!      get currently active point values
                                //<! NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_ACTION_GET_DEFAULT:
                                //<!      get VBIOS default point values
                                //<! NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_ACTION_SET:
                                //<!      set new active point values

    NvU8    fanIndex;           //<! [in]

    NvU8    numEffectivePoints; //<! [out] for fanGetCurvePoints. This is the
                                //<!  number of points actually implemented
                                //<!  and filled in the array below
                                //<! [in] for fanSetCurvePoints. This is how
                                //<!  many points the caller is sending to
                                //<!  the API. Must not exceed what was
                                //<!  returned by fanGetCurvePoints.

    NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINT
            curvePoints[NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_MAX_POINTS];
                                //<! [out] for fanGetCurvePoints.
                                //<! [in] for fanSetCurvePoints.
} NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_PARAMS;

#define NV_MSGBOX_POWER_HINT_PROFILES_ID_DMMA_PERF                             0
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_DMMA_HIGH_K                           1
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_DMMA_LOW_K                            2
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_HMMA                                  3
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_IMMA                                  4
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_SGEMM                                 5
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_TRANSFORMER                           6
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_0                     7
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_1                     8
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_2                     9
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_3                    10
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_4                    11
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_5                    12
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_6                    13
#define NV_MSGBOX_POWER_HINT_PROFILES_ID_CUSTOMER_CUSTOM_7                    14
/*!
 * This param is used for NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_POWER_HINT_GET
 */
typedef struct
{
    /*!
     * [out] Power Hint in mW
     */
    NvU32  powermW;
    /*!
     * [in] Graphics Clock in MHz
     */
    NvU16  grClkMHz;
    /*!
     * [in] Memory Clock
     */
    NvU16  memClkMHz;
    /*!
     * [in] GPU Temperature
     */
    NvS16  temperature;
    /*!
     * [in] Profile ID
     */
    NvU8   profile;
    // new param starting from here.
} NV_MSGBOX_POWER_HINT_PARAMS;

/*!
 * This param is used for NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_CONFIGURE_PROGRAMMABLE_EDPP
 */
typedef struct
{
    /*!
     * [out] Power Hint in mW
     */
    NvU32  data;
} NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS;

#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA                      31:0
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_OPERATION            1:0
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_OPERATION_GET        0x0
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_OPERATION_SET        0x1
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_OPERATION_CLEAR      0x2

#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_MODE                 3:2
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_MODE_PERSISTENT      0x0
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_MODE_ONESHOT         0x1

#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_LIMIT_CURRENT         10:4
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_LIMIT_RATED          17:11
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_LIMIT_MAXIMUM        24:18
#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_DATA_LIMIT_MINIMUM        31:25

#define NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS_LIMIT_NOT_SET             0x7F

/*!
 * @brief Union of all possible parameter struct. Used to determine the maximum
 * amount of space parameter blocks can take.
 */
typedef union {
    NV_MSGBOX_PMGR_PWR_POWER_LIMIT_CONTROL_PARAMS tgpLimitControl;
    NV_MSGBOX_THERMAL_FAN_V1_COUNT_PARAMS fanCountV1Get;
    NV_MSGBOX_THERMAL_FAN_V1_INFO_PARAMS fanCountV1Info;
    NV_MSGBOX_THERMAL_FAN_V1_STATUS_PARAMS fanCountV1Status;
    NV_MSGBOX_OVERCLOCKING_LIMIT_CONTROL_PARAMS overclockingLimitControl;
    NV_MSGBOX_ENERGY_COUNTER_STATUS_PARAMS energyCounterStatus;
    NV_MSGBOX_VIOLATION_COUNTERS_STATUS_PARAMS violationCountersStatus;
    NV_MSGBOX_UTILIZATION_RATE_PARAMS utilizationRate;
    NV_MSGBOX_OOB_CLOCK_LIMIT_CTRL_PARAMS oobClockLimitCtrlParams;
    NV_MSGBOX_DEVICE_MODE_CONTROL_PARAMS deviceModeControlParams;
    NV_MSGBOX_TEST_MESSAGE_SEND_PARAMS testMessageSend;
    NV_MSGBOX_CLOCK_LIMIT_GET_PARAMS clockLimitGet;
    NV_MSGBOX_THERMAL_FAN_V3_FAN_CURVE_POINTS_PARAMS fanCurvePointsV3;
    NV_MSGBOX_POWER_HINT_PARAMS powerHintParams;
    NV_MSGBOX_PROGRAMMABLE_EDPP_PARAMS programmableEdppParams;
} NV_MSGBOX_ASYNC_REQ_PARAMS_UNION;

#endif  // !NV_MSGBOX_NO_PARAM_STRUCTS

/* Utility command constructor macros */

#define NV_MSGBOX_CMD(opcode, arg1, arg2)               \
        (                                               \
            DRF_DEF(_MSGBOX, _CMD, _OPCODE, opcode) |   \
            DRF_NUM(_MSGBOX, _CMD, _ARG1, (arg1))   |   \
            DRF_NUM(_MSGBOX, _CMD, _ARG2, (arg2))   |   \
            DRF_DEF(_MSGBOX, _CMD, _STATUS, _NULL)  |   \
            DRF_DEF(_MSGBOX, _CMD, _RSVD, _INIT)    |   \
            DRF_DEF(_MSGBOX, _CMD, _INTR, _PENDING)     \
        )
#define NV_MSGBOX_CMD_NULL_CMD()                NV_MSGBOX_CMD(_NULL_CMD, 0, 0)
#define NV_MSGBOX_CMD_GET_CAP_DWORD(index)      NV_MSGBOX_CMD(_GET_CAP_DWORD, (index), 0)
#define NV_MSGBOX_CMD_GET_TEMP(source)          NV_MSGBOX_CMD(_GET_TEMP, (source), 0)
#define NV_MSGBOX_CMD_GET_EXT_TEMP(source)      NV_MSGBOX_CMD(_GET_EXT_TEMP, (source), 0)
#define NV_MSGBOX_CMD_GET_POWER(source)         NV_MSGBOX_CMD(_GET_POWER, (source), 0)
#define NV_MSGBOX_CMD_GET_SYS_ID_DATA(type, offset) \
                                                NV_MSGBOX_CMD(_GET_SYS_ID_DATA, (type), (offset) / 4)
#define NV_MSGBOX_CMD_SCRATCH_READ(off)         NV_MSGBOX_CMD(_SCRATCH_READ, (off), 0)
#define NV_MSGBOX_CMD_SCRATCH_WRITE(off, cnt)   NV_MSGBOX_CMD(_SCRATCH_WRITE, (off), (cnt) - 1)
#define NV_MSGBOX_CMD_SCRATCH_COPY(off, cnt)    NV_MSGBOX_CMD(_SCRATCH_COPY, (off), (cnt) - 1)
#define NV_MSGBOX_CMD_ASYNC_REQUEST(req_type, scratch_offset)   \
                                                NV_MSGBOX_CMD(_ASYNC_REQUEST, (req_type), (scratch_offset) / 4)
#define NV_MSGBOX_CMD_ASYNC_REQUEST_POLL(req_id)                                                    \
                                                NV_MSGBOX_CMD(_ASYNC_REQUEST,                       \
                                                            NV_MSGBOX_CMD_ARG1_ASYNC_REQUEST_POLL,  \
                                                            (req_id))
#define NV_MSGBOX_CMD_REGISTER_READ(addr)       NV_MSGBOX_CMD(_REGISTER_ACCESS,                         \
                                                            NV_MSGBOX_CMD_ARG1_REGISTER_ACCESS_READ,    \
                                                            (addr))
#define NV_MSGBOX_CMD_REGISTER_WRITE(addr)      NV_MSGBOX_CMD(_REGISTER_ACCESS,                         \
                                                            NV_MSGBOX_CMD_ARG1_REGISTER_ACCESS_WRITE,   \
                                                            (addr))
#define NV_MSGBOX_CMD_GET_THERM_PARAM(param)    NV_MSGBOX_CMD(_GET_THERM_PARAM,                         \
                                                            NV_MSGBOX_CMD_ARG1_THERM_PARAM##param, 0)

#define NV_MSGBOX_CMD_ACCESS_WP_MODE(action, state)                                                     \
                                                NV_MSGBOX_CMD(_ACCESS_WP_MODE, (action), (state))

#define NV_MSGBOX_CMD_GET_WP_MODE()                                                                     \
                    NV_MSGBOX_CMD_ACCESS_WP_MODE(NV_MSGBOX_CMD_ARG1_ACCESS_WP_MODE_ACTION_GET, 0)

#define NV_MSGBOX_CMD_SET_WP_MODE(state)                                                                \
                    NV_MSGBOX_CMD_ACCESS_WP_MODE(NV_MSGBOX_CMD_ARG1_ACCESS_WP_MODE_ACTION_SET, (state))

#define NV_MSGBOX_CMD_GET_MISC_GPU_FLAGS(subtype, arg2)                                                 \
                                                NV_MSGBOX_CMD(_GET_MISC_GPU_FLAGS, (subtype), (arg2))

#define NV_MSGBOX_CMD_GET_ECC_ENABLED_STATE()                                                           \
                    NV_MSGBOX_CMD_GET_MISC_GPU_FLAGS(NV_MSGBOX_CMD_ARG1_GET_MISC_ECC_ENABLED_STATE, 0)

#define NV_MSGBOX_CMD_GET_GPU_RESET_REQUIRED()                                                          \
                    NV_MSGBOX_CMD_GET_MISC_GPU_FLAGS(NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_RESET_REQUIRED, 0)

#define NV_MSGBOX_CMD_GET_MISC_GPU_FLAGS_PAGE_0()                                                       \
                    NV_MSGBOX_CMD_GET_MISC_GPU_FLAGS(NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_FLAGS_PAGE_0, 0)

#define NV_MSGBOX_CMD_GET_MISC_GPU_FLAGS_PAGE_1()                                                       \
                    NV_MSGBOX_CMD_GET_MISC_GPU_FLAGS(NV_MSGBOX_CMD_ARG1_GET_MISC_GPU_FLAGS_PAGE_1, 0)

#define NV_MSGBOX_CMD_GET_NVLINK_INFO(subtype, arg2)                                                    \
                                                NV_MSGBOX_CMD(_GET_NVLINK_INFO, (subtype), (arg2))

#define NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT(errtype, link_index)                                      \
                                                NV_MSGBOX_CMD(_GET_NVLINK_INFO,                         \
                                                NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO_ERR_COUNTER##errtype,\
                                                (link_index))

#define NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_STATE(errtype, link_page)                                        \
                                                NV_MSGBOX_CMD(_GET_NVLINK_INFO,                            \
                                                NV_MSGBOX_CMD_ARG1_GET_NVLINK_INFO##errtype##_ERROR_STATE, \
                                                (link_page))

#define NV_MSGBOX_CMD_GET_GPU_UTIL_COUNTERS(subtype)                                                    \
                                                NV_MSGBOX_CMD(_GPU_UTIL_COUNTERS, (subtype), 0)

#define NV_MSGBOX_CMD_GET_GPU_UTIL_COUNTERS_CONTEXT_TIME()                                              \
                    NV_MSGBOX_CMD_GET_GPU_UTIL_COUNTERS(NV_MSGBOX_CMD_ARG1_GPU_UTIL_COUNTERS_CONTEXT_TIME)

#define NV_MSGBOX_CMD_GET_GPU_UTIL_COUNTERS_SM_TIME()                                                   \
                    NV_MSGBOX_CMD_GET_GPU_UTIL_COUNTERS(NV_MSGBOX_CMD_ARG1_GPU_UTIL_COUNTERS_SM_TIME)

#define NV_MSGBOX_CMD_GET_GPU_UTIL_COUNTERS_RESET_COUNTERS()                                            \
                    NV_MSGBOX_CMD_GET_GPU_UTIL_COUNTERS(NV_MSGBOX_CMD_ARG1_GPU_UTIL_COUNTERS_RESET_COUNTERS)

#define NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT_REPLAY(link_index)                                        \
                                        NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT(_REPLAY, (link_index))

#define NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT_RECOVERY(link_index)                                      \
                                        NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT(_RECOVERY, (link_index))

#define NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT_FLIT_CRC(link_index)                                      \
                                        NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT(_FLIT_CRC, (link_index))

#define NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT_DATA_CRC(link_index)                                      \
                                        NV_MSGBOX_CMD_GET_NVLINK_INFO_ERR_CNT(_DATA_CRC, (link_index))

#define NV_MSGBOX_CMD_GET_CLOCK_FREQ_INFO(subtype, clkdomain)                                           \
                                        NV_MSGBOX_CMD(_GET_CLOCK_FREQ_INFO, (subtype), (clkdomain))

#define NV_MSGBOX_CMD_GET_CLOCK_FREQ_INFO_GPC_CURRENT()                                                     \
                    NV_MSGBOX_CMD_GET_CLOCK_FREQ_RANGE(NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_RANGE_GPC_CURRENT, \
                                                       NV_MSGBOX_CMD_ARG2_GET_CLOCK_FREQ_INFO_GPCCLK)

#define NV_MSGBOX_CMD_GET_CLOCK_FREQ_INFO_GPC_MIN()                                                     \
                    NV_MSGBOX_CMD_GET_CLOCK_FREQ_RANGE(NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_INFO_GPC_MIN,  \
                                                       NV_MSGBOX_CMD_ARG2_GET_CLOCK_FREQ_INFO_GPCCLK)

#define NV_MSGBOX_CMD_GET_CLOCK_FREQ_INFO_GPC_MAX()                                                     \
                    NV_MSGBOX_CMD_GET_CLOCK_FREQ_RANGE(NV_MSGBOX_CMD_ARG1_GET_CLOCK_FREQ_INFO_GPC_MAX,  \
                                                       NV_MSGBOX_CMD_ARG2_GET_CLOCK_FREQ_INFO_GPCCLK)

#define NV_MSGBOX_CMD_GET_DRIVER_EVENT_MSG(ptr) NV_MSGBOX_CMD(_GET_DRIVER_EVENT_MSG, 0, (ptr))

//
// ECC V2
//
#define NV_MSGBOX_CMD_ECC_V2_CNT_FB(src, type, partition, slice)             \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V2, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_SEL, _CNT)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_CNT_DEV, _FB)              | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_CNT_FB_SRC, src)           | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_CNT_TYPE, type)            | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_SLICE, (slice))           \
        )

#define NV_MSGBOX_CMD_ECC_V2_CNT_GR(src, type, gpc, tpc)           \
        (                                                          \
            NV_MSGBOX_CMD(_GET_ECC_V2, 0, 0)                     | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_SEL, _CNT)       | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_CNT_DEV, _GR)    | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_CNT_GR_SRC, src) | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_CNT_TYPE, type)  | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_GR_GPC, (gpc))   | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_GR_TPC, (tpc))     \
        )

#define NV_MSGBOX_CMD_ECC_V2_FBA_CUMUL(partition, slice)                     \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V2, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_SEL, _FBA)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_FBA_SC, _CUMUL)            | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_SLICE, (slice))           \
        )

#define NV_MSGBOX_CMD_ECC_V2_FBA_IND_ADDR(partition, slice, index)           \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V2, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_SEL, _FBA)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_FBA_SC, _IND)              | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_FBA_IND_FLD, _ADDR)        | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V2_FBA_IND_INDEX, (index))    | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_SLICE, (slice))           \
        )

#define NV_MSGBOX_CMD_ECC_V2_FBA_IND_CTRS(partition, slice, index)           \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V2, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_SEL, _FBA)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_FBA_SC, _IND)              | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V2_FBA_IND_FLD, _CTRS)        | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V2_FBA_IND_INDEX, (index))    | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V2_FB_SLICE, (slice))           \
        )

//
// ECC V3
//
#define NV_MSGBOX_CMD_ECC_V3_CNT_FB_LTC(type, partition, slice)              \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V3, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_SEL, _CNT)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_DEV, _FB)              | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_FB_SRC, _LTC)          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_TYPE, type)            | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_SLICE, (slice))           \
        )

#define NV_MSGBOX_CMD_ECC_V3_CNT_FB_FB(type, partition, subpart)             \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V3, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_SEL, _CNT)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_DEV, _FB)              | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_FB_SRC, _FB)           | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_TYPE, type)            | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_SUBPARTITION, (subpart))  \
        )

#define NV_MSGBOX_CMD_ECC_V3_CNT_GR(src, type, gpc, tpc)           \
        (                                                          \
            NV_MSGBOX_CMD(_GET_ECC_V3, 0, 0)                     | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_SEL, _CNT)       | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_DEV, _GR)    | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_GR_SRC, src) | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_TYPE, type)  | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_GR_GPC, (gpc))   | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_GR_TPC, (tpc))     \
        )

#define NV_MSGBOX_CMD_ECC_V3_CNT_GR_TEX(type, gpc, tpc, tex)          \
        (                                                             \
            NV_MSGBOX_CMD(_GET_ECC_V3, 0, 0)                        | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_SEL, _CNT)          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_DEV, _GR)       | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_GR_SRC, _TEX)   | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_CNT_TYPE, type)     | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_GR_GPC, (gpc))      | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_GR_TPC, (tpc))      | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_GR_TEX, (tex))        \
        )

#define NV_MSGBOX_CMD_ECC_V3_FBA_IND_ADDR(partition, subpart, index)          \
        (                                                                     \
            NV_MSGBOX_CMD(_GET_ECC_V3, 0, 0)                                | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_SEL, _FBA)                  | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_FBA_SC, _IND)               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_FBA_IND_FLD, _ADDR)         | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V3_FBA_IND_INDEX, (index))     | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_PARTITION, (partition))  | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_SUBPARTITION, (subpart))   \
        )

#define NV_MSGBOX_CMD_ECC_V3_FBA_IND_CTRS(partition, subpart, index)          \
        (                                                                     \
            NV_MSGBOX_CMD(_GET_ECC_V3, 0, 0)                                | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_SEL, _FBA)                  | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_FBA_SC, _IND)               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V3_FBA_IND_FLD, _CTRS)         | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V3_FBA_IND_INDEX, (index))     | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_PARTITION, (partition))  | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V3_FB_SUBPARTITION, (subpart))   \
        )

//
// ECC V4
//
#define NV_MSGBOX_CMD_ECC_V4_CNT_FB_LTC(type, partition, slice)              \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V4, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_SEL, _CNT)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_DEV, _FB)              | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_FB_SRC, _LTC)          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_TYPE, type)            | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_SLICE, (slice))           \
        )

#define NV_MSGBOX_CMD_ECC_V4_CNT_FB_FB(type, partition, subpart)             \
        (                                                                    \
            NV_MSGBOX_CMD(_GET_ECC_V4, 0, 0)                               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_SEL, _CNT)                 | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_DEV, _FB)              | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_FB_SRC, _FB)           | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_TYPE, type)            | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_PARTITION, (partition)) | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_SUBPARTITION, (subpart))  \
        )

#define NV_MSGBOX_CMD_ECC_V4_CNT_GR(src, type, gpc, tpc)           \
        (                                                          \
            NV_MSGBOX_CMD(_GET_ECC_V4, 0, 0)                     | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_SEL, _CNT)       | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_DEV, _GR)    | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_GR_SRC, src) | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_TYPE, type)  | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_GR_GPC, (gpc))   | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_GR_TPC, (tpc))     \
        )

#define NV_MSGBOX_CMD_ECC_V4_CNT_GR_TEX(type, gpc, tpc, tex)          \
        (                                                             \
            NV_MSGBOX_CMD(_GET_ECC_V4, 0, 0)                        | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_SEL, _CNT)          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_DEV, _GR)       | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_GR_SRC, _TEX)   | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_CNT_TYPE, type)     | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_GR_GPC, (gpc))      | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_GR_TPC, (tpc))      | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_GR_TEX, (tex))        \
        )

#define NV_MSGBOX_CMD_ECC_V4_FBA_IND_ADDR(partition, subpart, index)          \
        (                                                                     \
            NV_MSGBOX_CMD(_GET_ECC_V4, 0, 0)                                | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_SEL, _FBA)                  | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_FBA_SC, _IND)               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_FBA_IND_FLD, _ADDR)         | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V4_FBA_IND_INDEX, (index))     | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_PARTITION, (partition))  | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_SUBPARTITION, (subpart))   \
        )

#define NV_MSGBOX_CMD_ECC_V4_FBA_IND_CTRS(partition, subpart, index)          \
        (                                                                     \
            NV_MSGBOX_CMD(_GET_ECC_V4, 0, 0)                                | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_SEL, _FBA)                  | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_FBA_SC, _IND)               | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V4_FBA_IND_FLD, _CTRS)         | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V4_FBA_IND_INDEX, (index))     | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_PARTITION, (partition))  | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V4_FB_SUBPARTITION, (subpart))   \
        )

//
// ECC V5
//
#define NV_MSGBOX_CMD_ECC_V5_CNT(index)                                               \
        (                                                                             \
            NV_MSGBOX_CMD(_GET_ECC_V5, 0, 0)                                        | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V5_SEL, _COUNTS)                       | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V5_COUNTS_INDEX_LO, (index))           | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_INDEX_HI, (index) >> 7)        \
        )
#define NV_MSGBOX_ECC_V5_COUNTS_NUM_ENTRIES 600

#define NV_MSGBOX_CMD_ECC_V5_CNT_HDR_META(index)                                      \
        (                                                                             \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                         | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _HDR_META)               \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_REGN_CORRECTED_TOTAL(index)                           \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _REGN_CORRECTED_TOTAL)    \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_REGN_CORRECTED_UNIQUE(index)                          \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _REGN_CORRECTED_UNIQUE)   \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_REGN_UNCORRECTED_TOTAL(index)                         \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _REGN_UNCORRECTED_TOTAL)  \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_REGN_UNCORRECTED_UNIQUE(index)                        \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _REGN_UNCORRECTED_UNIQUE) \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_ADDR_ADDR(index)                                      \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _ADDR_ADDR)               \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_ADDR_UNCORRECTED_COUNTS(index)                        \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _ADDR_UNCORRECTED_COUNTS) \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_ADDR_CORRECTED_TOTAL(index)                           \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _ADDR_CORRECTED_TOTAL)    \
        )

#define NV_MSGBOX_CMD_ECC_V5_CNT_ADDR_CORRECTED_UNIQUE(index)                          \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_CNT(index)                                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_COUNTS_TYPE, _ADDR_CORRECTED_UNIQUE)   \
        )

#define NV_MSGBOX_CMD_ECC_V5_ERR_BUF(index)                                            \
        (                                                                              \
            NV_MSGBOX_CMD(_GET_ECC_V5, 0, 0)                                         | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_ECC_V5_SEL, _ERR_BUFFER)                    | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_ECC_V5_ERR_BUF_INDEX, (index))                \
        )
#define NV_MSGBOX_ECC_V5_ERR_BUF_NUM_ENTRIES 16

#define NV_MSGBOX_CMD_ECC_V5_ERR_BUF_ERR_TYPE_META(index)                              \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_ERR_BUF(index)                                      | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_ERR_BUF_TYPE, _ERR_TYPE_META)          \
        )

#define NV_MSGBOX_CMD_ECC_V5_ERR_BUF_TIME_STAMP(index)                                 \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_ERR_BUF(index)                                      | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_ERR_BUF_TYPE, _TIME_STAMP)             \
        )

#define NV_MSGBOX_CMD_ECC_V5_ERR_BUF_ADDR(index)                                       \
        (                                                                              \
            NV_MSGBOX_CMD_ECC_V5_ERR_BUF(index)                                      | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2_ECC_V5_ERR_BUF_TYPE, _ADDR)                   \
        )

#define NV_MSGBOX_CMD_SET_COPY_DATA(cmd)                                               \
        (                                                                              \
            FLD_SET_DRF(_MSGBOX, _CMD, _COPY_DATA, _ON, (cmd))                         \
        )

#define NV_MSGBOX_CMD_GET_PCIE_LINK_INFO_1(page)                                       \
        (                                                                              \
            NV_MSGBOX_CMD(_GET_PCIE_LINK_INFO,                                         \
                          (page),                                                      \
                          0)                                                           \
        )

// We use the fact here that for pages ## 8 and 9 Arg2 definitions are similar
#define NV_MSGBOX_CMD_GET_PCIE_LINK_INFO_2(page, lane, speed)                          \
        (                                                                              \
            NV_MSGBOX_CMD(_GET_PCIE_LINK_INFO,                                         \
                          (page),                                                      \
                          DRF_NUM(_MSGBOX, _CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_8,        \
                                  _LANE_IDX, (lane))                                 | \
                          DRF_NUM(_MSGBOX, _CMD_ARG2_GET_PCIE_LINK_INFO_PAGE_8,        \
                                  _SPEED_SELECT, (speed)))                             \
        )

#define NV_MSGBOX_CMD_GPM_GET_METRIC(type, metric, req_ci_metrics, partition)          \
        (                                                                              \
            NV_MSGBOX_CMD(_GPU_PERFORMANCE_MONITORING, 0, 0)                         | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_GPM_ACTION, type)                           | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_GPM_METRIC, metric)                         | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_GPM_CI_METRICS_REQUESTED, req_ci_metrics)   | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_GPM_PARTITION_INDEX, partition)               \
        )

#define NV_MSGBOX_CMD_GPM_GET_METRIC_AGGREGATE(type, metric) \
        (                                                                               \
            NV_MSGBOX_CMD(_GPU_PERFORMANCE_MONITORING, 0, 0)                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_GPM_ACTION, type)                            | \
            DRF_NUM(_MSGBOX, _CMD, _ARG1_GPM_METRIC, metric)                          | \
            DRF_DEF(_MSGBOX, _CMD, _ARG2, _GPM_PARTITION_AGGREGATE)                     \
        )

#define NV_MSGBOX_DATA_IN_GPM(instance_id, ci_index)                                   \
        (                                                                              \
            DRF_NUM(_MSGBOX, _DATA, _GPM_NVDEC_INSTANCE, instance_id)                | \
            DRF_NUM(_MSGBOX, _DATA, _GPM_COMPUTE_INSTANCE_INDEX, ci_index)             \
        )

#define NV_MSGBOX_CMD_GPM_SET_MULTIPLIER(multiplier)                                   \
        (                                                                              \
            NV_MSGBOX_CMD(_GPU_PERFORMANCE_MONITORING, 0, 0)                         | \
            DRF_DEF(_MSGBOX, _CMD, _ARG1_GPM_ACTION, _SET_MULTIPLIER)                | \
            DRF_NUM(_MSGBOX, _CMD, _ARG2_GPM_MULTIPLIER, (multiplier))                 \
        )

#define NV_MSGBOX_GET_CMD_OPCODE(cmd)           DRF_VAL(_MSGBOX, _CMD, _OPCODE, (cmd))
#define NV_MSGBOX_GET_CMD_ARG1(cmd)             DRF_VAL(_MSGBOX, _CMD, _ARG1  , (cmd))
#define NV_MSGBOX_GET_CMD_ARG2(cmd)             DRF_VAL(_MSGBOX, _CMD, _ARG2  , (cmd))
#define NV_MSGBOX_GET_CMD_STATUS(cmd)           DRF_VAL(_MSGBOX, _CMD, _STATUS, (cmd))
#define NV_MSGBOX_CMD_IS_COPY_DATA_SET(cmd)                                    \
                        FLD_TEST_DRF(_MSGBOX, _CMD, _COPY_DATA, _ON, (cmd))

#define NV_MSGBOX_CAP_IS_AVAILABLE(caps, idx, name)                            \
    !(FLD_TEST_DRF(_MSGBOX, _DATA_CAP_##idx, name, _NOT_AVAILABLE, (caps)[idx]))

#define NV_MSGBOX_CAP_SET_AVAILABLE(caps, idx, name)                           \
    (caps)[idx] = FLD_SET_DRF(_MSGBOX, _DATA_CAP_##idx, name, _AVAILABLE,      \
        (caps)[idx])

/********************************************************************************
 *                                                                              *
 *                             GPU REQUESTS                                     *
 *                                                                              *
 ********************************************************************************/

//
// Encoding:
//      31           24 23           16 15            8 7             0
//     .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
//     |   REQUEST ID  |      ARG2     |      ARG1     |     OPCODE    | DATA_OUT
//     `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_REQ_OPCODE                                                   7:0
#define NV_MSGBOX_REQ_OPCODE_CPU_PCONTROL                               0x00000000
#define NV_MSGBOX_REQ_OPCODE_SYS_PCONTROL                               0x00000001
#define NV_MSGBOX_REQ_ARG1                                                    15:8
#define NV_MSGBOX_REQ_ARG2                                                   23:16
#define NV_MSGBOX_REQ_REQUEST_ID                                             31:24
#define NV_MSGBOX_REQ(opcode, arg1, arg2, requestId)                             \
   (                                                                             \
       DRF_NUM(_MSGBOX, _REQ, _OPCODE    , (opcode))    |                        \
       DRF_NUM(_MSGBOX, _REQ, _ARG1      , (arg1))      |                        \
       DRF_NUM(_MSGBOX, _REQ, _ARG2      , (arg2))      |                        \
       DRF_NUM(_MSGBOX, _REQ, _REQUEST_ID, (requestId))                          \
   )

/* ---------------- CPU Performance Control (PCONTROL) ------------------------ */

//
// CPU_PCONTROL
//
// Description:
//     GPU-Request used to retrieve information and control various aspects of
//     CPU performance.
//
// Common Encoding:
//      7       3 2   0
//     .-+-+-+-+-+-+-+-.
//     |  TARGET | ACT | ARG1
//     `-+-+-+-+-+-+-+-'
//
// Notes:
//     arg1, arg2, and data encoding (fields are relative to zero, not relative
//     to their position in the DATA_OUT register).
//
#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_ACTION                                 2:0
#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_ACTION_RSVD1                    0x00000000
#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_ACTION_RSVD2                    0x00000001
#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_ACTION_SET_LIMIT                0x00000002
#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_TARGET                                 7:3
#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_TARGET_PSTATE                   0x00000000

#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_SET_ACTION(arg1, action)                 \
    (arg1) = FLD_SET_DRF(_MSGBOX_REQ, _CPU_PCONTROL_ARG1, _ACTION,               \
        action, (arg1))

#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_GET_ACTION(arg1)                         \
    DRF_VAL(_MSGBOX_REQ, _CPU_PCONTROL_ARG1, _ACTION, (arg1))

#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_SET_TARGET(arg1, target)                 \
    (arg1) = FLD_SET_DRF(_MSGBOX_REQ, _CPU_PCONTROL_ARG1, _TARGET,               \
        target, (arg1))

#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG1_GET_TARGET(arg1)                         \
    DRF_VAL(_MSGBOX_REQ, _CPU_PCONTROL_ARG1, _TARGET, (arg1))

//
// Setting CPU Pstate Limits:
//
// Inputs:
//     ARG1 - {TARGET=PSTATE, ACTION=SET_LIMIT}
//     ARG2 - Stores desired CPU Pstate limit
//
// Outputs:
//     none
//
// Encoding:
//      7       3 2   0               7             0
//     .-+-+-+-+-+-+-+-.             .-+-+-+-+-+-+-+-.
//     |  TARGET | ACT | ARG1        |     VALUE     | ARG2
//     `-+-+-+-+-+-+-+-'             `-+-+-+-+-+-+-+-'
//
#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG2_PSTATE_SET_LIMIT_VALUE                 7:0

#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG2_PSTATE_SET_LIMIT_SET_VALUE(arg2, limit)  \
    (arg2) = FLD_SET_DRF_NUM(_MSGBOX_REQ, _CPU_PCONTROL_ARG2,                    \
        _PSTATE_SET_LIMIT_VALUE, (limit), (arg2))

#define NV_MSGBOX_REQ_CPU_PCONTROL_ARG2_PSTATE_SET_LIMIT_GET_VALUE(arg2)         \
    DRF_VAL(_MSGBOX_REQ, _CPU_PCONTROL_ARG2, _PSTATE_SET_LIMIT_VALUE, (arg2))

/* ----------------------- System Performance Control ------------------------- */

//
// SYS_PCONTROL
//
// Description:
//     GPU-Request used to retrieve information and control various aspects of
//     system-performance.
//
// Common Encoding:
//      7       3 2   0               7             0
//     .-+-+-+-+-+-+-+-.             .-+-+-+-+-+-+-+-.
//     |  TARGET | ACT | ARG1        |0 0 0 0 0 0 0 0| ARG2 (reserved)
//     `-+-+-+-+-+-+-+-'             `-+-+-+-+-+-+-+-'
//
// Notes:
//     - TARGET accepts NV0000_CTRL_SYSTEM_PARAM_* definitions
//     - arg1, arg2, and data encoding (fields are relative to zero, not relative
//       to their position in the DATA_OUT register).
//
#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_ACTION                              2:0
#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_ACTION_GET_STATUS            0x00000000
#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_ACTION_GET_LIMIT             0x00000001
#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_TARGET                              7:3
#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG2_RSVD                                7:0

#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_SET_ACTION(arg1, action)              \
    (arg1) = FLD_SET_DRF(_MSGBOX_REQ, _SYS_PCONTROL_ARG1, _ACTION,            \
        action, (arg1))

#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_GET_ACTION(arg1)                      \
    DRF_VAL(_MSGBOX_REQ, _SYS_PCONTROL_ARG1, _ACTION, (arg1))

#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_SET_TARGET(arg1, target)              \
    (arg1) = FLD_SET_DRF_NUM(_MSGBOX_REQ, _SYS_PCONTROL_ARG1, _TARGET,        \
        (target), (arg1))

#define NV_MSGBOX_REQ_SYS_PCONTROL_ARG1_GET_TARGET(arg1)                      \
    DRF_VAL(_MSGBOX_REQ, _SYS_PCONTROL_ARG1, _TARGET, (arg1))

#endif /* _SMBPBI_H */
