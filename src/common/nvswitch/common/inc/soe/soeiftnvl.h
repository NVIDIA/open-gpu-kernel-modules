/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFTNVL_H_
#define _SOEIFTNVL_H_



/*!
 * @file   soeiftnvl.h
 * @brief  SOE TNVL Command Queue
 *
 *         The TNVL unit ID will be used for sending and recieving
 *         Command Messages between driver and TNVL unit of SOE
 */

#define RM_SOE_LIST_LS10_ONLY_ENGINES(_op)     \
    _op(GIN)                                \
    _op(XAL)                                \
    _op(XAL_FUNC)                           \
    _op(XPL)                                \
    _op(XTL)                                \
    _op(XTL_CONFIG)                         \
    _op(UXL)                                \
    _op(GPU_PTOP)                           \
    _op(PMC)                                \
    _op(PBUS)                               \
    _op(ROM2)                               \
    _op(GPIO)                               \
    _op(FSP)                                \
    _op(SYSCTRL)                            \
    _op(CLKS_SYS)                           \
    _op(CLKS_SYSB)                          \
    _op(CLKS_P0)                            \
    _op(SAW_PM)                             \
    _op(PCIE_PM)                            \
    _op(PRT_PRI_HUB)                        \
    _op(PRT_PRI_RS_CTRL)                    \
    _op(SYS_PRI_HUB)                        \
    _op(SYS_PRI_RS_CTRL)                    \
    _op(SYSB_PRI_HUB)                       \
    _op(SYSB_PRI_RS_CTRL)                   \
    _op(PRI_MASTER_RS)                      \
    _op(PTIMER)                             \
    _op(CPR)                                \
    _op(TILEOUT)                            \

#define RM_SOE_LIST_ALL_ENGINES(_op)      \
    _op(XVE)                                \
    _op(SAW)                                \
    _op(SOE)                                \
    _op(SMR)                                \
                                            \
    _op(NPG)                                \
    _op(NPORT)                              \
                                            \
    _op(NVLW)                               \
    _op(MINION)                             \
    _op(NVLIPT)                             \
    _op(NVLIPT_LNK)                         \
    _op(NVLTLC)                             \
    _op(NVLDL)                              \
                                            \
    _op(NXBAR)                              \
    _op(TILE)                               \
                                            \
    _op(NPG_PERFMON)                        \
    _op(NPORT_PERFMON)                      \
                                            \
    _op(NVLW_PERFMON)                       \

#define RM_SOE_ENGINE_ID_LIST(_eng)                \
    RM_SOE_ENGINE_ID_##_eng,

//
// ENGINE_IDs are the complete list of all engines that are supported on
// LS10 architecture(s) that may support them.  Any one architecture may or
// may not understand how to operate on any one specific engine.
// Architectures that share a common ENGINE_ID are not guaranteed to have
// compatible manuals.
//
typedef enum rm_soe_engine_id
{
    RM_SOE_LIST_ALL_ENGINES(RM_SOE_ENGINE_ID_LIST)
    RM_SOE_LIST_LS10_ONLY_ENGINES(RM_SOE_ENGINE_ID_LIST)
    RM_SOE_ENGINE_ID_SIZE,
} RM_SOE_ENGINE_ID;

/*!
 * Commands offered by the SOE Tnvl Interface.
 */
enum
{
    /*
     * Issue register write command
     */
    RM_SOE_TNVL_CMD_ISSUE_REGISTER_WRITE = 0x0,
    /*
     * Issue pre-lock sequence
     */
    RM_SOE_TNVL_CMD_ISSUE_PRE_LOCK_SEQUENCE = 0x1,
    /*
     * Issue engine write command
     */
    RM_SOE_TNVL_CMD_ISSUE_ENGINE_WRITE = 0x2,
};

/*!
 * TNVL queue command payload
 */

typedef struct
{
    NvU8  cmdType;
    NvU32 offset;
    NvU32 data;
} RM_SOE_TNVL_CMD_REGISTER_WRITE;

typedef struct
{
    NvU8  cmdType;
    RM_SOE_ENGINE_ID eng_id;
    NvU32 eng_bcast;
    NvU32 eng_instance;
    NvU32 base;
    NvU32 offset;
    NvU32 data;
} RM_SOE_TNVL_CMD_ENGINE_WRITE;

typedef struct
{
    NvU8  cmdType;
} RM_SOE_TNVL_CMD_PRE_LOCK_SEQUENCE;

typedef union
{
    NvU8 cmdType;
    RM_SOE_TNVL_CMD_REGISTER_WRITE registerWrite;
    RM_SOE_TNVL_CMD_ENGINE_WRITE engineWrite;
    RM_SOE_TNVL_CMD_PRE_LOCK_SEQUENCE preLockSequence;
} RM_SOE_TNVL_CMD;

#endif  // _SOETNVL_H_

