/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080spdm.finn
//



/*************************** SPDM COMMANDS ************************************/

#include "cc_drv.h"

/*!
 * @brief SPDM Command Types
 *
 */
#define RM_GSP_SPDM_CMD_ID_CC_INIT           (0x1)
#define RM_GSP_SPDM_CMD_ID_CC_DEINIT         (0x2)
#define RM_GSP_SPDM_CMD_ID_CC_CTRL           (0x3)
#define RM_GSP_SPDM_CMD_ID_CC_INIT_RM_DATA   (0x4)
#define RM_GSP_SPDM_CMD_ID_CC_HEARTBEAT_CTRL (0x5)
#define RM_GSP_SPDM_CMD_ID_FIPS_SELFTEST     (0x6)


#define RM_GSP_SPDM_CMD_ID_INVALID_COMMAND   (0xFF)



#define RSVD7_SIZE                           16

#define RSVD8_SIZE                           2

/*!
 * Guest RM provides INIT context
 */
typedef struct RM_GSP_SPDM_CMD_CC_INIT {
    // Command must be first as this struct is the part of union
    NvU8 cmdType;
} RM_GSP_SPDM_CMD_CC_INIT;
typedef struct RM_GSP_SPDM_CMD_CC_INIT *PRM_GSP_SPDM_CMD_CC_INIT;

/*!
 * Guest RM provides INIT context
 */
typedef struct RM_GSP_SPDM_CMD_CC_DEINIT {
    // Command must be first as this struct is the part of union
    NvU8 cmdType;
} RM_GSP_SPDM_CMD_CC_DEINIT;
typedef struct RM_GSP_SPDM_CMD_CC_DEINIT *PRM_GSP_SPDM_CMD_CC_DEINIT;

/*!
 * RM provides the SPDM request info to GSP
 */
typedef struct RM_GSP_SPDM_CMD_CC_CTRL {
    // Command must be first as this struct is the part of union
    NvU8 cmdType;
} RM_GSP_SPDM_CMD_CC_CTRL;
typedef struct RM_GSP_SPDM_CMD_CC_CTRL *PRM_GSP_SPDM_CMD_CC_CTRL;

typedef struct RM_GSP_SPDM_CMD_CC_INIT_RM_DATA {
    // Command must be first as this struct is the part of union
    NvU8  cmdType;

    NvU32 rsvd0[2];

    NvU32 rsvd1;

    char  rsvd2[9];

    char  rsvd3[5];

    char  rsvd4[5];

    char  rsvd5[5];

    char  rsvd6[2];

    char  rsvd7[RSVD7_SIZE];

    NvU32 rsvd8[RSVD8_SIZE];
} RM_GSP_SPDM_CMD_CC_INIT_RM_DATA;
typedef struct RM_GSP_SPDM_CMD_CC_INIT_RM_DATA *PRM_GSP_SPDM_CMD_CC_INIT_RM_DATA;

typedef struct RM_GSP_SPDM_CMD_CC_HEARTBEAT_CTRL {
    // Command must be first as this struct is the part of union
    NvU8   cmdType;

    // Whether to enable or disable heartbeats
    NvBool bEnable;
} RM_GSP_SPDM_CMD_CC_HEARTBEAT_CTRL;
typedef struct RM_GSP_SPDM_CMD_CC_HEARTBEAT_CTRL *PRM_GSP_SPDM_CMD_CC_HEARTBEAT_CTRL;



/*! 
 * HCC FIPS Self-test.
 */
#define CE_FIPS_SELF_TEST_DATA_SIZE     16
#define CE_FIPS_SELF_TEST_AUTH_TAG_SIZE 16
#define CE_FIPS_SELF_TEST_IV_SIZE       12

typedef struct RM_GSP_SPDM_CMD_FIPS_SELFTEST {
    NvU8   cmdType;
    NvU8   isEnc;
    CC_KMB kmb;
    NvU8   text[CE_FIPS_SELF_TEST_DATA_SIZE];
    NvU8   authTag[CE_FIPS_SELF_TEST_AUTH_TAG_SIZE];
} RM_GSP_SPDM_CMD_FIPS_SELFTEST;
typedef struct RM_GSP_SPDM_CMD_FIPS_SELFTEST *PRM_GSP_SPDM_CMD_FIPS_SELFTEST;



/*!
 * NOTE : Do not include structure members that have alignment requirement >= 8 to avoid alignment directives 
 * getting added in FINN generated structures / unions as RM_GSP_SPDM_CMD / RM_GSP_SPDM_MSG are pragma packed in 
 * other structures like RM_FLCN_CMD_GSP / RM_FLCN_MSG_GSP and pragma pack does not produce consistent behavior 
 * when paired with alignment directives on Linux and Windows. 
 */

/*!
 * A union of all SPDM Commands.
 */
typedef union RM_GSP_SPDM_CMD {
    NvU8                              cmdType;
    RM_GSP_SPDM_CMD_CC_INIT           ccInit;
    RM_GSP_SPDM_CMD_CC_DEINIT         ccDeinit;
    RM_GSP_SPDM_CMD_CC_CTRL           ccCtrl;
    RM_GSP_SPDM_CMD_CC_INIT_RM_DATA   rmDataInitCmd;
    RM_GSP_SPDM_CMD_CC_HEARTBEAT_CTRL ccHeartbeatCtrl;


    RM_GSP_SPDM_CMD_FIPS_SELFTEST     ccFipsTest;

} RM_GSP_SPDM_CMD;
typedef union RM_GSP_SPDM_CMD *PRM_GSP_SPDM_CMD;

/***************************** SPDM MESSAGES  *********************************/

/*!
 * SPDM Message Status
 */

/*!
 * Returns the status for program CE keys to RM
 */
#define RM_GSP_SPDM_MSG_ID_CC_INIT           (0x1)
#define RM_GSP_SPDM_MSG_ID_CC_DEINIT         (0x2)
#define RM_GSP_SPDM_MSG_ID_CC_CTRL           (0x3)
#define RM_GSP_SPDM_MSG_ID_CC_INIT_RM_DATA   (0x4)
#define RM_GSP_SPDM_MSG_ID_CC_HEARTBEAT_CTRL (0x5)
#define RM_GSP_SPDM_MSG_ID_FIPS_SELFTEST     (0x6)



/*!
 * Returns the Error Status for Invalid Command
 */
#define RM_GSP_SPDM_MSG_ID_INVALID_COMMAND   (0xFF)

/*!
 * NOTE : Do not include structure members that have alignment requirement >= 8 to avoid alignment directives 
 * getting added in FINN generated structures / unions as RM_GSP_SPDM_CMD / RM_GSP_SPDM_MSG are pragma packed in 
 * other structures like RM_FLCN_CMD_GSP / RM_FLCN_MSG_GSP and pragma pack does not produce consistent behavior 
 * when paired with alignment directives on Linux and Windows. 
 */

/*!
 * SPDM message structure.
 */
typedef struct RM_GSP_SPDM_MSG {
    NvU8  msgType;

    // status returned from GSP message infrastructure.
    NvU32 status;

    NvU32 rsvd1;

    NvU32 rsvd2;

    NvU32 rsvd3;

    NvU32 rsvd4;

    NvU32 rsvd5;
} RM_GSP_SPDM_MSG;
typedef struct RM_GSP_SPDM_MSG *PRM_GSP_SPDM_MSG;

/*
 * NV2080_CTRL_CMD_INTERNAL_SPDM_PARTITION
 *
 * This command does a partition switch to SPDM partition
 *
 */
#define NV2080_CTRL_INTERNAL_SPDM_PARTITION (0x20800ad9) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_INTERNAL_INTERFACE_ID << 8) | NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS_MESSAGE_ID (0xD9U)

typedef struct NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS {
    NvU8            index;
    RM_GSP_SPDM_CMD cmd;
    RM_GSP_SPDM_MSG msg;
} NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS;



