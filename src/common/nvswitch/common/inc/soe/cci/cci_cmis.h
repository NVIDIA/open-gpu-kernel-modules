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

#ifndef _CCI_CMIS_H_
#define _CCI_CMIS_H_

// Based on CMIS 4.0/5.0 specification

// Page 0h
#define CMIS_REV_BYTE                               1
#define CMIS_REV_BYTE_WHOLE                         7:4
#define CMIS_REV_BYTE_DECIMAL                       3:0
#define CMIS_CHARACTERISTICS_BYTE                   2
#define CMIS_CHARACTERISTICS_BYTE_FLAT_MEM          7:7
#define CMIS_MODULE_FLAGS_CUSTOM_BYTE               13
#define CMIS_OSFP_TEMPERATURE_BYTE                  14
#define CMIS_CDB_BLOCK_STATUS_BYTE(block)           ((block) + 37)
#define CMIS_CDB_BLOCK_STATUS_BYTE_BUSY             7:7
#define CMIS_CDB_BLOCK_STATUS_BYTE_BUSY_FALSE       0
#define CMIS_CDB_BLOCK_STATUS_BYTE_BUSY_TRUE        1
#define CMIS_ACTIVE_FW_MAJOR_REVISION               39
#define CMIS_ACTIVE_FW_MINOR_REVISION               40
#define CMIS_BANK_PAGE                              126
#define CMIS_MIT_BYTE                               212
#define CMIS_CHECKSUM_BYTE                          222
#define CMIS_CHECKSUM_LENGTH                        94

// Media Interface Technology Table
#define CMIS_MIT_BYTE_COPPER                                           0xA
#define CMIS_MIT_BYTE_COPPER_UNEQUALIZED                               0xA
#define CMIS_MIT_BYTE_COPPER_PASSIVE_EQUALIZED                         0xB
#define CMIS_MIT_BYTE_COPPER_NEAR_FAR_END_LIMITING_ACTIVE_EQUALIZED    0xC
#define CMIS_MIT_BYTE_COPPER_FAR_END_LIMITING_ACTIVE_EQUALIZED         0xD
#define CMIS_MIT_BYTE_COPPER_NEAR_END_LIMITING_ACTIVE_EQUALIZED        0xE
#define CMIS_MIT_BYTE_COPPER_LINEAR_ACTIVE_EQUALIZED                   0xF
#define CMIS_MIT_BYTE_RESERVED                                         0x10

// Page 01h
#define CMIS_SI_CONTROLS                                   161
#define CMIS_SI_CONTROLS_CDR_IMPLEMENTED                   0:0
#define CMIS_DATA_PATH_MAX_DURATION_BYTE                   144
#define CMIS_DATA_PATH_MAX_DURATION_BYTE_DEINIT            7:4
#define CMIS_DATA_PATH_MAX_DURATION_BYTE_INIT              3:0
#define CMIS_MODULE_POWER_MAX_DURATION_BYTE                167
#define CMIS_MODULE_POWER_MAX_DURATION_BYTE_DOWN           7:4
#define CMIS_MODULE_POWER_MAX_DURATION_BYTE_UP             3:0
#define CMIS_DATA_PATH_TX_MAX_DURATION_BYTE                168
#define CMIS_DATA_PATH_TX_MAX_DURATION_BYTE_OFF            7:4
#define CMIS_DATA_PATH_TX_MAX_DURATION_BYTE_ON             3:0

// Page 02h
#define CMIS_OSFP_OVERT_LIMIT_BYTE          128
#define CMIS_OSFP_WARN_LIMIT_BYTE           132


//
// CMIS 4.0: Table 8-29 State Duration Encoding
//           Stores time in ms
//           Times > 60s will be considered errors
//
static const NvU64 dur_en_map[] = {1, 5, 10, 50, 
                                   100, 500, 1000, 5*1000,
                                   10*1000, 60*1000, 0, 0,
                                   0, 0, 0, 0};
#define CMIS_MAX_DURATION_EN_TO_MS_MAP(en)                 (dur_en_map[en])                        

// Page 10h
#define CMIS_DATA_PATH_CONTROL_BYTE           128
#define CMIS_STAGE_CONTROL_SET_0              143
#define CMIS_STAGE_CONTROL_SET_0_SIZE         35
#define CMIS_STAGE_CONTROL_SET_0_APP_SEL      145

// Page 11h
#define CMIS_DATA_PATH_STATE                     128
// Used to determine which byte(128-131) encodes given lane's datapath state
#define CMIS_DATA_PATH_STATE_LANE_BYTE_MAP(lane) (lane/2)
#define CMIS_DATA_PATH_STATE_SIZE                4
#define CMIS_DATA_PATH_STATE_BYTE_LANE_0         3:0
#define CMIS_DATA_PATH_STATE_BYTE_LANE_1         7:4
#define CMIS_DATA_PATH_CONFIG_ERROR_CODES        202
#define CMIS_ACTIVE_CONTROL_SET                  206
#define CMIS_ACTIVE_CONTROL_SET_SIZE             29 

// Datapath states
#define CMIS_DATA_PATH_STATE_LANE_DEACTIVATED       0x1 
#define CMIS_DATA_PATH_CONFIG_ALL_LANES_ACCEPTED    0x11111111

// CDB command payload size
#define CMIS_CDB_LPL_MAX_SIZE 120

//
// CMIS 4.0: Table 9-16 CDB Command 0100h: Get firmware Info
//           Index of information in LPL   
//
#define CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS                      0
#define CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_A_RUNNING      0:0
#define CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_A_COMMITTED    1:1
#define CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_A_EMPTY        2:2
#define CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_B_RUNNING      4:4
#define CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_B_COMMITTED    5:5
#define CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_B_EMPTY        6:6                        
#define CMIS_CDB_GET_FW_INFO_LPL_INFO_BLOCK                           1
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_MAJOR_BYTE                   2
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_MINOR_BYTE                   3
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_BUILD                        4
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_BUILD_SIZE                   2
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_EXTRA_STRING                 6
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_EXTRA_STRING_SIZE            32
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_MAJOR_BYTE                   38
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_MINOR_BYTE                   39
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_BUILD                        40
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_BUILD_SIZE                   2
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_EXTRA_STRING                 42
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_EXTRA_STRING_SIZE            32
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_MAJOR_BYTE        74
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_MINOR_BYTE        75
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_BUILD             76
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_BUILD_SIZE        2
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_EXTRA_STRING      78
#define CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_EXTRA_STRING_SIZE 32

#endif //_CCI_CMIS_H_
