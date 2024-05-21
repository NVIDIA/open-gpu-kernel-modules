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
    RM_SOE_CORE_CMD_READ_BIOS_SIZE = 0x0,

    /*!
     * Read the BIOS
     */
    RM_SOE_CORE_CMD_READ_BIOS = 0x1,

    /*!
     * Run DMA self-test
     */
    RM_SOE_CORE_CMD_DMA_SELFTEST = 0x2,

    /*!
     * Perform I2C transaction
     */
    RM_SOE_CORE_CMD_I2C_ACCESS = 0x3,

    /*!
     * Issue NPORT Reset
     */
    RM_SOE_CORE_CMD_ISSUE_NPORT_RESET = 0x4,

    /*!
     * Restore NPORT state
     */
    RM_SOE_CORE_CMD_RESTORE_NPORT_STATE = 0x5,

    /*!
     * Set NPORT TPROD state
     */
    RM_SOE_CORE_CMD_SET_NPORT_TPROD_STATE = 0x6,

    /*!
     * Read VRs
     */
    RM_SOE_CORE_CMD_GET_VOLTAGE_VALUES = 0x7,

    /*!
     * Init PLM2 protected registers
     */
    RM_SOE_CORE_CMD_INIT_L2_STATE = 0x8,

    /*!
     * Read Power
     */
    RM_SOE_CORE_CMD_GET_POWER_VALUES = 0x9,

    /*!
     * Set NPORT interrupts
     */
    RM_SOE_CORE_CMD_SET_NPORT_INTRS = 0xA,

    /*!
     * Set Module LP mode
     */
    RM_SOE_CORE_CMD_SET_MODULE_LP_MODE = 0xB,

    /*!
     * Read from Ports CPLD
     */
    RM_SOE_CORE_CMD_READ_PORTS_CPLD = 0xC,

    /*!
     * Write to Ports CPLD
     */
    RM_SOE_CORE_CMD_WRITE_PORTS_CPLD = 0xD,

    /*!
    * Perform a module onboard phase on behalf of the driver
    */
    RM_SOE_CORE_CMD_PERFORM_MODULE_ONBOARD_PHASE = 0xE,

    /*!
     * Disable NPORT fatal interrupt
     */
    RM_SOE_CORE_CMD_DISABLE_NPORT_FATAL_INTR = 0xF,

    /*
     * Issue Ingress stop
     */
    RM_SOE_CORE_CMD_ISSUE_INGRESS_STOP = 0x10,
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
    NvBool bStop;
} RM_SOE_CORE_CMD_INGRESS_STOP;

typedef struct
{
    NvU8   cmdType;
    NvU8   moduleId;
    NvBool bAssert; 
} RM_SOE_CORE_CMD_SET_LP_MODE;

typedef struct
{
    NvU8   cmdType;
    NvU8   reg;
} RM_SOE_CORE_CMD_READ_CPLD;

typedef struct
{
    NvU8   cmdType;
    NvU8   reg;
    NvU8   dataIn;
} RM_SOE_CORE_CMD_WRITE_CPLD;

typedef struct
{
    NvU8   cmdType;
    NvU8   moduleId; 
    NvU8   onboardPhase;
    NvU8   onboardSubPhase;
    struct {
        NvBool rxDetEnable;
        NvU8   reserved0;
        NvU8   reserved1;
        NvU8   reserved2;
    } attributes;    
    RM_FLCN_U64  linkMask; 
} RM_SOE_CORE_CMD_PERFORM_ONBOARD_PHASE;

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
    RM_SOE_CORE_CMD_SET_LP_MODE setLpMode;
    RM_SOE_CORE_CMD_READ_CPLD readCpld;
    RM_SOE_CORE_CMD_WRITE_CPLD writeCpld;
    RM_SOE_CORE_CMD_PERFORM_ONBOARD_PHASE performOnboardPhase;
    RM_SOE_CORE_CMD_NPORT_FATAL_INTR nportDisableIntr;
    RM_SOE_CORE_CMD_INGRESS_STOP ingressStop;
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

typedef struct
{
    NvU8   msgType;
    NvU8   flcnStatus;
} RM_SOE_CORE_MSG_SET_LP_MODE;

typedef struct
{
    NvU8   msgType;
    NvU8   flcnStatus;
    NvU8   dataOut;
} RM_SOE_CORE_MSG_READ_CPLD;

typedef struct
{
    NvU8   msgType;
    NvU8   flcnStatus;
} RM_SOE_CORE_MSG_WRITE_CPLD;

typedef struct
{
    NvU8   msgType;
    NvU8   flcnStatus;
} RM_SOE_CORE_MSG_PERFORM_ONBOARD_PHASE;

typedef union
{
    NvU8 msgType;
    RM_SOE_CORE_MSG_GET_VOLTAGE getVoltage;
    RM_SOE_CORE_MSG_GET_POWER getPower;
    RM_SOE_CORE_MSG_SET_LP_MODE setLpMode;
    RM_SOE_CORE_MSG_READ_CPLD readCpld;
    RM_SOE_CORE_MSG_WRITE_CPLD writeCpld;
    RM_SOE_CORE_MSG_PERFORM_ONBOARD_PHASE performOnboardPhase;
} RM_SOE_CORE_MSG;
#endif  // _SOECORE_H_
