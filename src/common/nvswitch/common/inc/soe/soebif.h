/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEBIF_H_
#define _SOEBIF_H_

/*!
 * @file   soebif.h
 * @brief  SOE BIF Command Queue
 *
 *         The BIF unit ID will be used for sending and recieving
 *         Command Messages between driver and BIF unit of SOE
 */

/*!
 * Commands offered by the SOE Bus Interface.
 */
enum
{
    /*!
     * Update the UPHY EOM(Eye Opening Measurement) parameters.
     */
    RM_SOE_BIF_CMD_UPDATE_EOM,

    /*!
     * This command sends UPHY register's address and lane from the client
     * to the SOE and gets the register value.
     */
    RM_SOE_BIF_CMD_GET_UPHY_DLN_CFG_SPACE,

    /*!
     * Set PCIE link speed
     */
    RM_SOE_BIF_CMD_SET_PCIE_LINK_SPEED,

    /*!
     * Get UPHY EOM(Eye Opening Measurement) status.
     */
    RM_SOE_BIF_CMD_GET_EOM_STATUS,
};

/*!
 * BIF queue command payload
 */
typedef struct
{
    NvU8 cmdType;
    NvU8 mode;
    NvU8 nerrs;
    NvU8 nblks;
    NvU8 berEyeSel;
} RM_SOE_BIF_CMD_EOM;

typedef struct
{
    NvU8 cmdType;
    NvU8 mode;
    NvU8 nerrs;
    NvU8 nblks;
    NvU8 berEyeSel;
    NvU32 laneMask;
    RM_FLCN_U64 dmaHandle;
} RM_SOE_BIF_CMD_EOM_STATUS;

typedef struct
{
    NvU8  cmdType;
    NvU32 regAddress;
    NvU32 laneSelectMask;
} RM_SOE_BIF_CMD_UPHY_DLN_CFG_SPACE;

#define RM_SOE_BIF_LINK_SPEED_INVALID      (0x00)
#define RM_SOE_BIF_LINK_SPEED_GEN1PCIE     (0x01)
#define RM_SOE_BIF_LINK_SPEED_GEN2PCIE     (0x02)
#define RM_SOE_BIF_LINK_SPEED_GEN3PCIE     (0x03)
#define RM_SOE_BIF_LINK_SPEED_GEN4PCIE     (0x04)
#define RM_SOE_BIF_LINK_SPEED_GEN5PCIE     (0x05)

#define RM_SOE_BIF_LINK_WIDTH_INVALID  (0x00)
#define RM_SOE_BIF_LINK_WIDTH_X1       (0x01)
#define RM_SOE_BIF_LINK_WIDTH_X2       (0x02)
#define RM_SOE_BIF_LINK_WIDTH_X4       (0x03)
#define RM_SOE_BIF_LINK_WIDTH_X8       (0x04)
#define RM_SOE_BIF_LINK_WIDTH_X16      (0x05)

// Maximum time to wait for LTSSM to go idle, in ns
#define BIF_LTSSM_IDLE_TIMEOUT_NS          (200 * SOE_INTERVAL_1USEC_IN_NS)
// Maximum time to wait for LTSSM to declare link ready, in ns
#define BIF_LTSSM_LINK_READY_TIMEOUT_NS    (20 * SOE_INTERVAL_1MSEC_IN_NS)
// Maximum time to keep trying to change link speed, in ns
#define BIF_LINK_CHANGE_TIMEOUT_NS         (10 * SOE_INTERVAL_5MSEC_IN_NS)
// Maximum PCIe lanes supported per link is 16 as of PCIe spec 4.0r1.0
#define BIF_MAX_PCIE_LANES   16U

typedef struct
{
    NvU8 cmdType;
    NvU32 linkSpeed;
} RM_SOE_BIF_CMD_PCIE_LINK_SPEED;

typedef union
{
    NvU8 cmdType;
    RM_SOE_BIF_CMD_EOM eomctl;
    RM_SOE_BIF_CMD_UPHY_DLN_CFG_SPACE cfgctl;
    RM_SOE_BIF_CMD_PCIE_LINK_SPEED speedctl;
    RM_SOE_BIF_CMD_EOM_STATUS eomStatus;
} RM_SOE_BIF_CMD;

#endif  // _SOEBIF_H_

