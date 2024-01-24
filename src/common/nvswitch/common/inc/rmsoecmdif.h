/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RMSOECMDIF_H_
#define _RMSOECMDIF_H_

/*!
 * @file   rmsoecmdif.h
 * @brief  Top-level header-file that defines the command/message interfaces
 *         that may be used to communicate with SOE.
 *
 * This header does not directly define specific attributes, structure, or
 * other properties for specific commands or messages. Instead, it includes all
 * specific unit- header-files and then defines the top-level command and
 * message structures, RM_SOE_COMMAND and RM_SOE_MESSAGE (respectively).
 */



#include "flcnifcmn.h"

#include "soe/soebif.h"
#include "soe/soeifcmn.h"
#include "soe/soeifsoe.h"
#include "soe/soeiftest.h"
#include "soe/soeiftherm.h"
#include "soe/soeifspi.h"
#include "soe/soeififr.h"
#include "soe/soeifsmbpbi.h"
#include "soe/soeifcore.h"
#include "soe/soeifchnmgmt.h"
#include "soe/soeifcci.h"
#include "soe/soeifheartbeat.h"

/*!
 * Defines the structure that must be populated to send any arbitrary command
 * to SOE's command queue.  Each command packet will contain a command
 * header ('hdr') describing various command attributes as well as the command
 * data itself ('cmd').
 */
typedef struct
{
    RM_FLCN_QUEUE_HDR         hdr;
    union
    {
        RM_SOE_TEST_CMD      soeTest;
        RM_SOE_THERM_CMD     therm;
        RM_SOE_SPI_CMD       spi;
        RM_SOE_IFR_CMD       ifr;
        RM_SOE_SMBPBI_CMD    smbpbiCmd;
        RM_SOE_BIF_CMD       bif;
        RM_SOE_CORE_CMD      core;
        RM_SOE_CHNMGMT_CMD   chnmgmt;
        RM_SOE_CCI_CMD       cci;
    } cmd;
} RM_FLCN_CMD_SOE,
*PRM_FLCN_CMD_SOE;

typedef  RM_FLCN_CMD_SOE   RM_SOE_COMMAND;
typedef  PRM_FLCN_CMD_SOE  PRM_SOE_COMMAND;

/*!
 * Defines the structure that represents all messages sent from SOE to
 * the RM.  Similar to the command structure, the message structure also
 * contains a message header ('hdr') that describes the message as well as
 * the actual message data ('msg').
 */
typedef struct
{
    RM_FLCN_QUEUE_HDR         hdr;
    union
    {
        RM_SOE_TEST_MSG      soeTest;
        RM_SOE_THERM_MSG     soeTherm;
        RM_FLCN_MSG_SOE_INIT init;
        RM_SOE_CHNMGMT_MSG   chnmgmt;
        RM_SOE_CORE_MSG      core;   
        RM_SOE_CCI_MSG       cci;
        RM_SOE_HEARTBEAT_MSG heartbeat;
    } msg;
} RM_FLCN_MSG_SOE,
*PRM_FLCN_MSG_SOE;

/*!
 * Production task-identifiers:
 *
 * @note    TASK_ID__IDLE must remain zero.
 * @note    TASK_ID__END must mark the end of the valid IDs.
 *
 * @note    Do NOT compact following defines!
 * @note    Do NOT recycle deleted/unused IDs!
 */
#define RM_SOE_TASK_ID__IDLE        0x00
#define RM_SOE_TASK_ID_CMDMGMT      0x01
#define RM_SOE_TASK_ID_WKRTHD       0x02
#define RM_SOE_TASK_ID_THERM        0x03
#define RM_SOE_TASK_ID_RESERVED     0x04
#define RM_SOE_TASK_ID_SPI          0x05
#define RM_SOE_TASK_ID_RSVD6        0x06
#define RM_SOE_TASK_ID_SMBPBI       0x07
#define RM_SOE_TASK_ID_BIF          0x08
#define RM_SOE_TASK_ID_CORE         0x09
#define RM_SOE_TASK_ID_IFR          0x0A
#define RM_SOE_TASK_ID_CHNMGMT      0x0B
#define RM_SOE_TASK_ID_RMMSG        0x0C
#define RM_SOE_TASK_ID_CCI          0x0D
#define RM_SOE_TASK_ID_FSPMGMT      0x0E
#define RM_SOE_TASK_ID_HEARTBEAT    0x0F
// Add new task ID here...
#define RM_SOE_TASK_ID__END         0x10

/*!
 * Unit-identifiers:
 *
 * Notes:
 *     UNIT_END must mark the end of the valid IDs.
 */
#define  RM_SOE_UNIT_REWIND          RM_FLCN_UNIT_ID_REWIND
#define  RM_SOE_UNIT_INIT            (0x01)
#define  RM_SOE_UNIT_NULL            (0x02)
#define  RM_SOE_UNIT_TEST            (0x03)
#define  RM_SOE_UNIT_UNLOAD          (0x04)
#define  RM_SOE_UNIT_THERM           (0x05)
#define  RM_SOE_UNIT_RESERVED        (0x06)
#define  RM_SOE_UNIT_SPI             (0x07)
#define  RM_SOE_UNIT_RSVD8           (0x08)
#define  RM_SOE_UNIT_SMBPBI          (0x09)
#define  RM_SOE_UNIT_BIF             (0x0A)
#define  RM_SOE_UNIT_CORE            (0x0B)
#define  RM_SOE_UNIT_IFR             (0x0C)
#define  RM_SOE_UNIT_CHNMGMT         (0x0D)
#define  RM_SOE_UNIT_CCI             (0x0E)
#define  RM_SOE_UNIT_HEARTBEAT       (0x0F)
// Add new unit ID here...
#define  RM_SOE_UNIT_END             (0x10)

#endif // _RMSOECMDIF_H_

