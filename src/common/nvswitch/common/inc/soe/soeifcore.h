/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOECORE_H_
#define _SOECORE_H_

#define SOE_CORE_BIOS_SIZE_LS10                0x100000          // 1 MB

/*!
 * @file   soeifutil.h
 * @brief  SOE CORE Command Queue
 *
 *         The CORE unit ID will be used for sending and recieving
 *         Command Messages between driver and CORE unit of SOE
 */

/*!
 * Commands offered by the SOE utility Interface.
 */
enum
{
    /*!
     * Read the BIOS Size
     */
    RM_SOE_CORE_CMD_READ_BIOS_SIZE,

    /*!
     * Read the BIOS
     */
    RM_SOE_CORE_CMD_READ_BIOS,

    /*!
     * Run DMA self-test
     */
    RM_SOE_CORE_CMD_DMA_SELFTEST,

    /*!
     * Perform I2C transaction
     */
    RM_SOE_CORE_CMD_I2C_ACCESS,

    /*!
     * Issue NPORT Reset
     */
    RM_SOE_CORE_CMD_ISSUE_NPORT_RESET,

    /*!
     * Restore NPORT state
     */
    RM_SOE_CORE_CMD_RESTORE_NPORT_STATE,

    /*!
     * Set NPORT TPROD state
     */
    RM_SOE_CORE_CMD_SET_NPORT_TPROD_STATE,

    /*!
     * Read VRs
     */
    RM_SOE_CORE_CMD_GET_VOLTAGE_VALUES,

    /*!
     * Init PLM2 protected registers
     */
    RM_SOE_CORE_CMD_INIT_L2_STATE,

    /*!
     * Read Power
     */
    RM_SOE_CORE_CMD_GET_POWER_VALUES,

    /*!
     * Set NPORT interrupts
     */
    RM_SOE_CORE_CMD_SET_NPORT_INTRS,

    /*!
     * Disable NPORT fatal interrupt
     */
    RM_SOE_CORE_CMD_DISABLE_NPORT_FATAL_INTR,
};

// Timeout for SOE reset callback function
#define SOE_UNLOAD_CALLBACK_TIMEOUT_US 10000 // 10ms

#define SOE_DMA_TEST_BUF_SIZE       512

#define SOE_DMA_TEST_INIT_PATTERN   0xab
#define SOE_DMA_TEST_XFER_PATTERN   0xcd

#define RM_SOE_DMA_READ_TEST_SUBCMD    0x00
#define RM_SOE_DMA_WRITE_TEST_SUBCMD   0x01

#define SOE_I2C_DMA_BUF_SIZE            512
#define SOE_I2C_STATUS_INDEX            (SOE_I2C_DMA_BUF_SIZE - 1)

/*!
 * CORE queue command payload
 */
typedef struct
{
    NvU8 cmdType;
    RM_FLCN_U64 dmaHandle;
    NvU32 offset;
    NvU32 sizeInBytes;
} RM_SOE_CORE_CMD_BIOS;

typedef struct
{
    NvU8        cmdType;
    NvU8        subCmdType;
    RM_FLCN_U64 dmaHandle;
    NvU8        dataPattern;
    NvU16       xferSize;
} RM_SOE_CORE_CMD_DMA_TEST;

typedef struct
{
    NvU8        cmdType;
    RM_FLCN_U64 dmaHandle;
    NvU16       xferSize;
} RM_SOE_CORE_CMD_I2C;

typedef struct
{
    NvU8   cmdType;
    NvU32  nport;
} RM_SOE_CORE_CMD_NPORT_RESET;

typedef struct
{
    NvU8   cmdType;
    NvU32  nport;
} RM_SOE_CORE_CMD_NPORT_STATE;

typedef struct
{
    NvU8   cmdType;
    NvU32  nport;
} RM_SOE_CORE_CMD_NPORT_TPROD_STATE;

typedef struct
{
    NvU8   cmdType;
} RM_SOE_CORE_CMD_GET_VOLTAGE;

typedef struct
{
    NvU8   cmdType;
} RM_SOE_CORE_CMD_L2_STATE;

typedef struct
{
    NvU8   cmdType;
} RM_SOE_CORE_CMD_GET_POWER;

typedef struct
{
    NvU8   cmdType;
    NvU32  nport;
    NvBool bEnable;
} RM_SOE_CORE_CMD_NPORT_INTRS;

typedef struct
{
    NvU8   cmdType;
    NvU32  nport;
    NvU32  nportIntrEnable;
    NvU8   nportIntrType;
} RM_SOE_CORE_CMD_NPORT_FATAL_INTR;

/*!
 * NPORT Interrupt Types 
 */
enum
{
    RM_SOE_CORE_NPORT_ROUTE_INTERRUPT,
    RM_SOE_CORE_NPORT_INGRESS_INTERRUPT,
    RM_SOE_CORE_NPORT_EGRESS_0_INTERRUPT,
    RM_SOE_CORE_NPORT_EGRESS_1_INTERRUPT,
    RM_SOE_CORE_NPORT_TSTATE_INTERRUPT,
    RM_SOE_CORE_NPORT_SOURCETRACK_INTERRUPT,
    RM_SOE_CORE_NPORT_MULTICAST_INTERRUPT,
    RM_SOE_CORE_NPORT_REDUCTION_INTERRUPT
};

typedef union
{
    NvU8 cmdType;
    RM_SOE_CORE_CMD_BIOS bios;
    RM_SOE_CORE_CMD_DMA_TEST dma_test;
    RM_SOE_CORE_CMD_I2C i2c;
    RM_SOE_CORE_CMD_NPORT_RESET nportReset;
    RM_SOE_CORE_CMD_NPORT_STATE nportState;
    RM_SOE_CORE_CMD_NPORT_TPROD_STATE nportTprodState;
    RM_SOE_CORE_CMD_GET_VOLTAGE getVoltage;
    RM_SOE_CORE_CMD_L2_STATE l2State;
    RM_SOE_CORE_CMD_GET_POWER getPower;
    RM_SOE_CORE_CMD_NPORT_INTRS nportIntrs;
    RM_SOE_CORE_CMD_NPORT_FATAL_INTR nportDisableIntr;
} RM_SOE_CORE_CMD;

typedef struct
{
    NvU8   msgType;
    NvU8   flcnStatus;
    NvU32  vdd_mv;
    NvU32  dvdd_mv;
    NvU32  hvdd_mv;
} RM_SOE_CORE_MSG_GET_VOLTAGE;

typedef struct
{
    NvU8   msgType;
    NvU8   flcnStatus;
    NvU32  vdd_w;
    NvU32  dvdd_w;
    NvU32  hvdd_w;
} RM_SOE_CORE_MSG_GET_POWER;

typedef union
{
    NvU8 msgType;
    RM_SOE_CORE_MSG_GET_VOLTAGE getVoltage;
    RM_SOE_CORE_MSG_GET_POWER getPower;
} RM_SOE_CORE_MSG;
#endif  // _SOECORE_H_
