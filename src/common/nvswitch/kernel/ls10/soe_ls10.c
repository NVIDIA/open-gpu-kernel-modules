/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "soe/soe_nvswitch.h"
#include "soe/soe_priv_nvswitch.h"

#include "rmlsfm.h"

#include "nvlink_export.h"

#include "common_nvswitch.h"
#include "ls10/ls10.h"
#include "ls10/soe_ls10.h"

#include "nvswitch/ls10/dev_soe_ip.h"
#include "nvswitch/ls10/dev_soe_ip_addendum.h"
#include "nvswitch/ls10/dev_falcon_v4.h"
#include "nvswitch/ls10/dev_nvlsaw_ip.h"
#include "nvswitch/ls10/dev_nvlsaw_ip_addendum.h"
#include "nvswitch/ls10/dev_riscv_pri.h"
#include "nvswitch/ls10/dev_nport_ip.h"

#include "flcn/flcnable_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"
#include "soe/soeifcmn.h"

/**
 * @brief   Sets pEngDescUc and pEngDescBc to the discovered
 * engine that matches this flcnable instance
 *
 * @param[in]   device       nvswitch_device pointer
 * @param[in]   pSoe         SOE pointer
 * @param[out]  pEngDescUc  pointer to the UniCast Engine
 *       Descriptor
 * @param[out]  pEngDescBc  pointer to the BroadCast Engine
 *       Descriptor
 */
static void
_soeFetchEngines_LS10
(
    nvswitch_device         *device,
    FLCNABLE                *pSoe,
    ENGINE_DESCRIPTOR_TYPE *pEngDescUc,
    ENGINE_DESCRIPTOR_TYPE *pEngDescBc
)
{
    pEngDescUc->initialized = NV_FALSE;
    if (NVSWITCH_ENG_IS_VALID(device, SOE, 0))
    {
        pEngDescUc->base = NVSWITCH_GET_ENG(device, SOE, , 0);
    }
    else
    {
        pEngDescUc->base = 0;
    }

    pEngDescBc->initialized = NV_FALSE;
    pEngDescBc->base = 0;
}

/*
 * @Brief : Send a test command to SOE
 *
 * @param   device  The nvswitch device
 */
static NV_STATUS
_nvswitch_soe_send_test_cmd
(
    nvswitch_device *device
)
{
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    NvU32               cmdSeqDesc;
    NV_STATUS           status;

    FLCN *pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_NULL;
    // sending nothing but a header for UNIT_NULL
    cmd.hdr.size   = RM_FLCN_QUEUE_HDR_SIZE;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&cmd,
                                NULL,   // pMsg             - not used for now
                                NULL,   // pPayload         - not used for now
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);

    NVSWITCH_ASSERT(status == NVL_SUCCESS);
    return status;
}

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
/*!
 * Helper function to dump some registers for debug.
 *
 * @param[in]  device  nvswitch_device pointer
 */
static void
dumpDebugRegisters
(
    nvswitch_device *device
)
{
    FLCN *pFlcn = device->pSoe->pFlcn;

    NvU32 regOS              = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_OS);
    NvU32 regPC              = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_RPC);
    NvU32 regCPUCTL          = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_CPUCTL);
    NvU32 regIDLESTATE       = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IDLESTATE);
    NvU32 regMAILBOX0        = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_MAILBOX0);
    NvU32 regMAILBOX1        = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_MAILBOX1);
    NvU32 regIRQSTAT         = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQSTAT);
    NvU32 regIRQMODE         = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQMODE);
    NvU32 regIRQMASK         = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_IRQMASK);
    NvU32 regIRQDEST         = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_IRQDEST);
    NvU32 regIRQDELEG        = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_IRQDELEG);
    NvU32 regDMACTL          = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMACTL);
    NvU32 regDMATRFCMD       = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMATRFCMD);
    NvU32 regDMATRFBASE      = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMATRFBASE);
    NvU32 regDMATRFMOFFS     = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMATRFMOFFS);
    NvU32 regDMATRFFBOFFS    = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMATRFFBOFFS);
    NvU32 regPRIVERR_STAT    = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_PRIV_ERR_STAT);
    NvU32 regPRIVERR_INFO    = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_PRIV_ERR_INFO);
    NvU32 regPRIVERR_ADDR_HI = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_PRIV_ERR_ADDR_HI);
    NvU32 regPRIVERR_ADDR_LO = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_PRIV_ERR_ADDR);
    NvU32 regHUBERR_STAT     = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_HUB_ERR_STAT);
    NvU32 regBCR_CTRL        = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_BCR_CTRL);
    NvU32 regRESET_PLM       = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_RESET_PRIV_LEVEL_MASK);
    NvU32 regEXE_PLM         = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_EXE_PRIV_LEVEL_MASK);

    NVSWITCH_PRINT(device, ERROR, "Peregrine Registers:\n");    
    NVSWITCH_PRINT(device, ERROR, "OS                    : 0x%08x\n", regOS);
    NVSWITCH_PRINT(device, ERROR, "PC (lo32)             : 0x%08x\n", regPC);
    NVSWITCH_PRINT(device, ERROR, "CPUCTL                : 0x%08x\n", regCPUCTL);
    NVSWITCH_PRINT(device, ERROR, "IDLESTATE             : 0x%08x\n", regIDLESTATE);
    NVSWITCH_PRINT(device, ERROR, "MAILBOX0              : 0x%08x\n", regMAILBOX0);
    NVSWITCH_PRINT(device, ERROR, "MAILBOX1              : 0x%08x\n", regMAILBOX1);
    NVSWITCH_PRINT(device, ERROR, "IRQSTAT               : 0x%08x\n", regIRQSTAT);
    NVSWITCH_PRINT(device, ERROR, "IRQMODE               : 0x%08x\n", regIRQMODE);
    NVSWITCH_PRINT(device, ERROR, "IRQMASK               : 0x%08x\n", regIRQMASK);
    NVSWITCH_PRINT(device, ERROR, "IRQDEST               : 0x%08x\n", regIRQDEST);
    NVSWITCH_PRINT(device, ERROR, "IRQDELEG              : 0x%08x\n", regIRQDELEG);
    NVSWITCH_PRINT(device, ERROR, "DMACTL                : 0x%08x\n", regDMACTL);
    NVSWITCH_PRINT(device, ERROR, "DMATRFCMD             : 0x%08x\n", regDMATRFCMD);
    NVSWITCH_PRINT(device, ERROR, "DMATRFBASE            : 0x%08x\n", regDMATRFBASE);
    NVSWITCH_PRINT(device, ERROR, "DMATRFMOFFS           : 0x%08x\n", regDMATRFMOFFS);
    NVSWITCH_PRINT(device, ERROR, "DMATRFFBOFFS          : 0x%08x\n", regDMATRFFBOFFS);
    NVSWITCH_PRINT(device, ERROR, "PRIVERR_STAT          : 0x%08x\n", regPRIVERR_STAT);
    NVSWITCH_PRINT(device, ERROR, "PRIVERR_INFO          : 0x%08x\n", regPRIVERR_INFO);
    NVSWITCH_PRINT(device, ERROR, "PRIVERR_ADDR          : 0x%08x%08x\n", regPRIVERR_ADDR_HI, regPRIVERR_ADDR_LO);
    NVSWITCH_PRINT(device, ERROR, "HUBERR_STAT           : 0x%08x\n", regHUBERR_STAT);
    NVSWITCH_PRINT(device, ERROR, "BCR_CTRL              : 0x%08x\n", regBCR_CTRL);
    NVSWITCH_PRINT(device, ERROR, "RESET_PLM             : 0x%08x\n", regRESET_PLM);
    NVSWITCH_PRINT(device, ERROR, "EXE_PLM               : 0x%08x\n", regEXE_PLM);
}
#endif  // defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

/*
 * @Brief : Attach or Detach driver to SOE Queues
 *
 * @param[in] device
 */
static void
_nvswitch_soe_attach_detach_driver_ls10
(
    nvswitch_device *device,
    NvBool bAttach
)
{
    NvU32 val;

    val = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _DRIVER_ATTACH_DETACH);

    if (bAttach)
    {
        val = FLD_SET_DRF(_NVLSAW, _DRIVER_ATTACH_DETACH, _STATUS,
                          _ATTACHED, val);
    }
    else
    {
        val = FLD_SET_DRF(_NVLSAW, _DRIVER_ATTACH_DETACH, _STATUS,
                          _DETACHED, val);
    }

    NVSWITCH_SAW_WR32_LS10(device, _NVLSAW, _DRIVER_ATTACH_DETACH, val);
}

/*
 * @Brief : Returns if SOE is attached to the Queues
 *
 * @param[in] device
 */
static NvBool
_nvswitch_is_soe_attached_ls10
(
    nvswitch_device *device
)
{
    NvU32 val;
    NvBool bSoeAttached;

    val = NVSWITCH_SAW_RD32_LS10(device, _NVLSAW, _SOE_ATTACH_DETACH);
    bSoeAttached = FLD_TEST_DRF(_NVLSAW, _SOE_ATTACH_DETACH, _STATUS, _ATTACHED, val);

    return bSoeAttached;
}

/*
 * @Brief : Backup NPORT state and issue NPORT reset
 *
 * @param[in] device
 * @param[in] nport
 */
NvlStatus
nvswitch_soe_issue_nport_reset_ls10
(
    nvswitch_device *device,
    NvU32 nport
)
{
    FLCN *pFlcn = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    RM_SOE_CORE_CMD_NPORT_RESET *pNportReset;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, NPORT_RESET);

    pNportReset = &cmd.cmd.core.nportReset;
    pNportReset->nport = nport;
    pNportReset->cmdType = RM_SOE_CORE_CMD_ISSUE_NPORT_RESET;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send NPORT RESET command to SOE status 0x%x\n", 
                       __FUNCTION__, status);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : De-Assert NPORT reset and restore NPORT state
 *
 * @param[in] device
 * @param[in] nport
 */
NvlStatus
nvswitch_soe_restore_nport_state_ls10
(
    nvswitch_device *device,
    NvU32 nport
)
{
    FLCN *pFlcn       = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    RM_SOE_CORE_CMD_NPORT_STATE *pNportState;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, NPORT_STATE);

    pNportState = &cmd.cmd.core.nportState;
    pNportState->nport = nport;
    pNportState->cmdType = RM_SOE_CORE_CMD_RESTORE_NPORT_STATE;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send NPORT BACKUP command to SOE status 0x%x\n", 
                       __FUNCTION__, status);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Set NPORT TPROD state in SOE
 *
 * @param[in] device
 * @param[in] nport
 */
NvlStatus
nvswitch_set_nport_tprod_state_ls10
(
    nvswitch_device *device,
    NvU32 nport
)
{
    FLCN *pFlcn       = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    RM_SOE_CORE_CMD_NPORT_TPROD_STATE *nportTprodState;

    if (!NVSWITCH_ENG_IS_VALID(device, NPORT, nport))
    {
         NVSWITCH_PRINT(device, ERROR, "%s: NPORT #%d invalid\n",
                        __FUNCTION__, nport);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, NPORT_TPROD_STATE);

    nportTprodState = &cmd.cmd.core.nportTprodState;
    nportTprodState->nport = nport;
    nportTprodState->cmdType = RM_SOE_CORE_CMD_SET_NPORT_TPROD_STATE;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send NPORT SET_TPROD_STATE command to SOE, status 0x%x\n", 
                       __FUNCTION__, status);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : INIT L2 register state in SOE
 *
 * @param[in] device
 * @param[in] nport
 */
void
nvswitch_soe_init_l2_state_ls10
(
    nvswitch_device *device
)
{
    FLCN            *pFlcn;
    NvU32            cmdSeqDesc = 0;
    NV_STATUS        status;
    RM_FLCN_CMD_SOE  cmd;
    NVSWITCH_TIMEOUT timeout;
    RM_SOE_CORE_CMD_L2_STATE *pL2State;

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO, "%s: SOE is not supported\n",
                       __FUNCTION__);
        return;
    }

    pFlcn       = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, L2_STATE);

    pL2State = &cmd.cmd.core.l2State;
    pL2State->cmdType = RM_SOE_CORE_CMD_INIT_L2_STATE;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send INIT_L2_STATE command to SOE, status 0x%x\n", 
                       __FUNCTION__, status);
    }
}

/*
 * @Brief : Enable/Disable NPORT interrupts
 *
 * @param[in] device
 * @param[in] nport
 */
NvlStatus
nvswitch_soe_set_nport_interrupts_ls10
(
    nvswitch_device *device,
    NvU32           nport,
    NvBool          bEnable
)
{
    FLCN            *pFlcn;
    NvU32            cmdSeqDesc = 0;
    NV_STATUS        status;
    RM_FLCN_CMD_SOE  cmd;
    NVSWITCH_TIMEOUT timeout;
    RM_SOE_CORE_CMD_NPORT_INTRS *pNportIntrs;

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE is not supported\n",
            __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    pFlcn       = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, NPORT_INTRS);

    pNportIntrs = &cmd.cmd.core.nportIntrs;
    pNportIntrs->cmdType = RM_SOE_CORE_CMD_SET_NPORT_INTRS;
    pNportIntrs->nport = nport;
    pNportIntrs->bEnable = bEnable;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to send SET_NPORT_INTRS command to SOE, status 0x%x\n", 
            __FUNCTION__, status);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Disable NPORT Fatal Interrupt in SOE
 *
 * @param[in] device
 * @param[in] nport
 * @param[in] nportIntrEnable
 * @param[in] nportIntrType
 */
void
nvswitch_soe_disable_nport_fatal_interrupts_ls10
(
    nvswitch_device *device,
    NvU32 nport,
    NvU32 nportIntrEnable,
    NvU8  nportIntrType   
)
{
    FLCN            *pFlcn;
    NvU32            cmdSeqDesc = 0;
    NV_STATUS        status;
    RM_FLCN_CMD_SOE  cmd;
    NVSWITCH_TIMEOUT timeout;
    RM_SOE_CORE_CMD_NPORT_FATAL_INTR *pNportIntrDisable;
    NVSWITCH_GET_BIOS_INFO_PARAMS p = { 0 };

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &p);
    if ((status != NVL_SUCCESS) || ((p.version & SOE_VBIOS_VERSION_MASK) < 
            SOE_VBIOS_REVLOCK_DISABLE_NPORT_FATAL_INTR))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Skipping DISABLE_NPORT_FATAL_INTR command to SOE.  Update firmware "
            "from .%02X to .%02X\n",
            __FUNCTION__, (NvU32)((p.version & SOE_VBIOS_VERSION_MASK) >> 16), 
            SOE_VBIOS_REVLOCK_DISABLE_NPORT_FATAL_INTR);
        return;
    }

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: SOE is not supported\n",
            __FUNCTION__);
        return;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, NPORT_FATAL_INTR);

    pNportIntrDisable = &cmd.cmd.core.nportDisableIntr;
    pNportIntrDisable->cmdType = RM_SOE_CORE_CMD_DISABLE_NPORT_FATAL_INTR;
    pNportIntrDisable->nport   = nport;
    pNportIntrDisable->nportIntrEnable = nportIntrEnable;
    pNportIntrDisable->nportIntrType = nportIntrType;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to send DISABLE_NPORT_FATAL_INTR command to SOE, status 0x%x\n", 
            __FUNCTION__, status);
    }
}

/*
 * @Brief : Issue INGRESS STOP in SOE
 *
 * @param[in] device
 * @param[in] nport
 * @param[in] bStop
 */
NvlStatus
nvswitch_soe_issue_ingress_stop_ls10
(
    nvswitch_device *device,
    NvU32 nport,
    NvBool bStop
)
{
    FLCN *pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    RM_SOE_CORE_CMD_INGRESS_STOP *pIngressStop;
    NVSWITCH_GET_BIOS_INFO_PARAMS params = { 0 };
    NvBool bKeepPolling;
    NvU32 val;

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: SOE is not supported\n",
            __FUNCTION__);
        return NVL_SUCCESS; // -NVL_ERR_NOT_SUPPORTED
    }

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &params);
    if ((status != NVL_SUCCESS) || ((params.version & SOE_VBIOS_VERSION_MASK) < 
            SOE_VBIOS_REVLOCK_ISSUE_INGRESS_STOP))
    {
        return NVL_SUCCESS; // -NVL_ERR_NOT_SUPPORTED
    }

    pFlcn = device->pSoe->pFlcn;
     
    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, INGRESS_STOP);
     
    pIngressStop = &cmd.cmd.core.ingressStop;
    pIngressStop->nport = nport;
    pIngressStop->cmdType = RM_SOE_CORE_CMD_ISSUE_INGRESS_STOP;
    pIngressStop->bStop = bStop;
     
    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to send INGRESS STOP command to SOE status 0x%x\n", 
            __FUNCTION__, status);
        return -NVL_ERR_GENERIC;
    }

    //
    // After asserting INGRESS_STOP, wait until nport is idle
    // to indicate that traffic is drained out.
    //
    if (bStop)
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
        do
        {
            bKeepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

            val = NVSWITCH_NPORT_RD32_LS10(device, nport, _NPORT, _STATUS);

            if (FLD_TEST_DRF(_NPORT, _STATUS, _INGRESS_IDLE, _IDLE, val) &&
                FLD_TEST_DRF(_NPORT, _STATUS, _ROUTE_IDLE,   _IDLE, val))
            {
                break;
            }

            NVSWITCH_NSEC_DELAY(2 * NVSWITCH_INTERVAL_1USEC_IN_NS);

        } while (bKeepPolling);

        if (!FLD_TEST_DRF(_NPORT, _STATUS, _INGRESS_IDLE, _IDLE, val) ||
            !FLD_TEST_DRF(_NPORT, _STATUS, _ROUTE_IDLE,   _IDLE, val))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Traffic failed to drain after ingress stop is asserted, val = 0x%x\n", 
                __FUNCTION__, val);
            return -NVL_ERR_GENERIC;
        }
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Clear Engine Interrupt Counter in SOE
 *
 * @param[in] device
 * @param[in] eng_id
 * @param[in] engineIntrType
 * @param[in] instance
 */
NvlStatus
nvswitch_soe_clear_engine_interrupt_counter_ls10
(
    nvswitch_device                        *device,
    RM_SOE_CORE_ENGINE_ID                  eng_id,
    RM_SOE_CORE_ENGINE_INTR_COUNTER_TYPE   engineIntrType,
    NvU32                                  instance
)
{
    FLCN            *pFlcn;
    NvU32            cmdSeqDesc = 0;
    NV_STATUS        status;
    RM_FLCN_CMD_SOE  cmd;
    NVSWITCH_TIMEOUT timeout;
    RM_SOE_CORE_CMD_ENGINE_INTR_COUNTER *pEngineClearIntrCounter;
    NVSWITCH_GET_BIOS_INFO_PARAMS params = { 0 };

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: SOE is not supported\n",
            __FUNCTION__);
        return NVL_SUCCESS; // -NVL_ERR_NOT_SUPPORTED
    }

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &params);
    if ((status != NVL_SUCCESS) || ((params.version & SOE_VBIOS_VERSION_MASK) < 
            SOE_VBIOS_REVLOCK_ENGINE_COUNTERS))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Skipping clearing Engine Interrupt Counter and disabiling NVLW interrupt.  Update firmware "
            "from .%02X to .%02X\n",
            __FUNCTION__, (NvU32)((params.version & SOE_VBIOS_VERSION_MASK) >> 16), 
            SOE_VBIOS_REVLOCK_ENGINE_COUNTERS);

        return NVL_SUCCESS; // -NVL_ERR_NOT_SUPPORTED
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, ENGINE_INTR_COUNTER);

    pEngineClearIntrCounter = &cmd.cmd.core.engineClearIntrCounter;
    pEngineClearIntrCounter->cmdType = RM_SOE_CORE_CMD_ISSUE_ENGINE_INTR_COUNTER_CLEAR;
    pEngineClearIntrCounter->engId = eng_id;
    pEngineClearIntrCounter->engineIntrType = engineIntrType;
    pEngineClearIntrCounter->instance = instance;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to send clear Engine Interrupt Counter command to SOE, status 0x%x\n",
            __FUNCTION__, status);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Perform register writes in SOE during TNVL
 *
 * @param[in] device
 * @param[in] eng_id
 * @param[in] eng_instance
 * @param[in] reg
 * @param[in] data
 */
NvlStatus
nvswitch_soe_update_intr_report_en_ls10
(
    nvswitch_device *device,
    RM_SOE_CORE_ENGINE_ID eng_id,
    NvU32 eng_instance,
    RM_SOE_CORE_ENGINE_REPORT_EN_REGISTER reg,
    NvU32 data
)
{
    FLCN            *pFlcn;
    NvU32            cmdSeqDesc = 0;
    NV_STATUS        status;
    RM_FLCN_CMD_SOE  cmd;
    NVSWITCH_TIMEOUT timeout;
    RM_SOE_CORE_CMD_ERROR_REPORT_EN *pErrorReportEnable;
    NVSWITCH_GET_BIOS_INFO_PARAMS params = { 0 };

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: SOE is not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &params);
    if ((status != NVL_SUCCESS) || ((params.version & SOE_VBIOS_VERSION_MASK) < 
            SOE_VBIOS_REVLOCK_REPORT_EN))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Unable to update REPORT_EN register and disabiling NVLW interrupt.  Update firmware "
            "from .%02x to .%02x\n",
            __FUNCTION__, (NvU32)((params.version & SOE_VBIOS_VERSION_MASK) >> 16), 
            SOE_VBIOS_REVLOCK_REPORT_EN);

        return -NVL_ERR_NOT_SUPPORTED;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, ERROR_REPORT_EN);

    pErrorReportEnable = &cmd.cmd.core.enableErrorReport;
    pErrorReportEnable->cmdType = RM_SOE_CORE_CMD_UPDATE_INTR_REPORT_EN;

    pErrorReportEnable->engId = eng_id;
    pErrorReportEnable->engInstance = eng_instance;
    pErrorReportEnable->reg = reg;
    pErrorReportEnable->data = data;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to update [0x%x] REPORT_EN register through SOE, status 0x%x\n",
            __FUNCTION__, reg, status);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Perform register writes in SOE during TNVL
 *
 * @param[in] device
 * @param[in] offset
 * @param[in] data
 */
NvlStatus
nvswitch_soe_reg_wr_32_ls10
(
    nvswitch_device *device,
    NvU32           offset,
    NvU32           data
)
{
    FLCN *pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    RM_SOE_TNVL_CMD_REGISTER_WRITE *pRegisterWrite;
    NVSWITCH_GET_BIOS_INFO_PARAMS params = { 0 };

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: SOE is not supported\n",
            __FUNCTION__);
        return NVL_SUCCESS; // -NVL_ERR_NOT_SUPPORTED
    }

    if (device->nvlink_device->pciInfo.bars[0].pBar == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: register write failed at offset 0x%x\n",
            __FUNCTION__, offset);
        return -NVL_IO_ERROR;
    }

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &params);
    if ((status != NVL_SUCCESS) || ((params.version & SOE_VBIOS_VERSION_MASK) < 
            SOE_VBIOS_REVLOCK_SOE_PRI_CHECKS))
    {
        nvswitch_os_mem_write32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + offset, data);
        return NVL_SUCCESS;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_TNVL;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(TNVL, REGISTER_WRITE);
 
    pRegisterWrite = &cmd.cmd.tnvl.registerWrite;
    pRegisterWrite->cmdType = RM_SOE_TNVL_CMD_ISSUE_REGISTER_WRITE;
    pRegisterWrite->offset = offset;
    pRegisterWrite->data = data;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to send REGISTER_WRITE command to SOE, offset = 0x%x, data = 0x%x\n", 
            __FUNCTION__, offset, data);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Perform engine writes in SOE during TNVL
 *
 * @param[in] device
 * @param[in] eng_id        NVSWITCH_ENGINE_ID*
 * @param[in] eng_bcast     NVSWITCH_GET_ENG_DESC_TYPE*
 * @param[in] eng_instance  
 * @param[in] base_addr
 * @param[in] offset
 * @param[in] data
 */
NvlStatus
nvswitch_soe_eng_wr_32_ls10
(
    nvswitch_device     *device,
    NVSWITCH_ENGINE_ID  eng_id,
    NvU32               eng_bcast,
    NvU32               eng_instance,
    NvU32               base_addr,
    NvU32               offset,
    NvU32               data
)
{
    FLCN *pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    RM_SOE_TNVL_CMD_ENGINE_WRITE *pEngineWrite;
    NVSWITCH_GET_BIOS_INFO_PARAMS params = { 0 };

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: SOE is not supported\n",
            __FUNCTION__);
        return NVL_SUCCESS; // -NVL_ERR_NOT_SUPPORTED
    }

    if (device->nvlink_device->pciInfo.bars[0].pBar == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: register write failed at offset 0x%x\n",
            __FUNCTION__, offset);
        return -NVL_IO_ERROR;
    }

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &params);
    if ((status != NVL_SUCCESS) || ((params.version & SOE_VBIOS_VERSION_MASK) < 
            SOE_VBIOS_REVLOCK_SOE_PRI_CHECKS))
    {
        nvswitch_os_mem_write32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + offset, data);
        return NVL_SUCCESS;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_TNVL;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(TNVL, ENGINE_WRITE);

    pEngineWrite = &cmd.cmd.tnvl.engineWrite;
    pEngineWrite->cmdType = RM_SOE_TNVL_CMD_ISSUE_ENGINE_WRITE;
    pEngineWrite->eng_id = eng_id;
    pEngineWrite->eng_bcast = eng_bcast;
    pEngineWrite->eng_instance = eng_instance;
    pEngineWrite->base = base_addr;
    pEngineWrite->offset = offset;
    pEngineWrite->data = data;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to send ENGINE_WRITE command to SOE, offset = 0x%x, data = 0x%x\n", 
            __FUNCTION__, offset, data);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Init sequence for SOE FSP RISCV image
 *
 * The driver assumes SOE is already booted by FSP.
 * Driver checks for SOE state and  handshakes with a test command.
 * If FSP or SOE fails to boot, driver fails and quits.
 *
 * @param[in] nvswitch device
 */
NvlStatus
nvswitch_init_soe_ls10
(
    nvswitch_device *device
)
{
    NvlStatus status;
    NvU32 data;
    FLCN *pFlcn = device->pSoe->pFlcn;

    //
    // Check if SOE has halted unexpectedly.
    //
    // The explicit check is required because the interrupts
    // are not yet enabled as the device is still initializing.
    //
    if (soeIsCpuHalted_HAL(device, ((PSOE)pFlcn->pFlcnable)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE halted\n",
            __FUNCTION__);
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_BOOTSTRAP,
            "SOE init Failed(0)\n");
        status = -NVL_ERR_INVALID_STATE;
        goto nvswitch_init_soe_fail;
    }

    // Check for SOE BOOT SUCCESS
    data = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_MAILBOX1);
    if (data != SOE_BOOTSTRAP_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE boot failed\n",
            __FUNCTION__);
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_BOOTSTRAP,
            "SOE init failed(1)\n");
        status = -NVL_ERR_INVALID_STATE;
        goto nvswitch_init_soe_fail;
    }

    // Register SOE callbacks
    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        status = nvswitch_soe_register_event_callbacks(device);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_COMMAND_QUEUE,
                "Failed to register SOE events\n");
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_BOOTSTRAP,
                "SOE init failed(2)\n");
            return status;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "Skipping registering SOE callbacks since TNVL is enabled\n");
    }

    // Sanity the command and message queues as a final check
    if (_nvswitch_soe_send_test_cmd(device) != NV_OK)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_BOOTSTRAP,
            "SOE init failed(4)\n");
        status = -NVL_ERR_INVALID_STATE;
        goto nvswitch_init_soe_fail;
    }

    NVSWITCH_PRINT(device, SETUP,
                   "%s: SOE successfully bootstrapped.\n",
                   __FUNCTION__);

    return NVL_SUCCESS;

nvswitch_init_soe_fail :
    // Log any failures SOE may have had during bootstrap
    (void)soeService_HAL(device, ((PSOE)pFlcn->pFlcnable));
#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
        dumpDebugRegisters(device);
#endif // defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

    flcnDbgInfoCapturePcTrace_HAL(device, pFlcn);
    NVSWITCH_ASSERT(0);
    return status;
}

/*
 * @Brief : Shutdown SOE during driver unload
 *
 * @param[in] device Bootstrap SOE on this device
 */
NvlStatus
nvswitch_unload_soe_ls10
(
    nvswitch_device *device
)
{
    // Detach driver from SOE Queues
    _nvswitch_soe_attach_detach_driver_ls10(device, NV_FALSE);

    return NVL_SUCCESS;
}

/*!
 * @brief : Register callback functions for events and messages from SOE
 *
 * @param[in] device nvswitch_device pointer
 */
NvlStatus
nvswitch_soe_register_event_callbacks_ls10
(
    nvswitch_device *device
)
{

    NV_STATUS status;
    PFLCN     pFlcn = device->pSoe->pFlcn;
    PSOE      pSoe  = (PSOE)device->pSoe;

    // Register Thermal callback funcion
    status = flcnQueueEventRegister(
                 device, pFlcn,
                 RM_SOE_UNIT_THERM,
                 NULL,
                 nvswitch_therm_soe_callback_ls10,
                 NULL,
                 &pSoe->thermEvtDesc);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to register thermal event handler.\n",
            __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    // Register CCI callback funcion
    status = flcnQueueEventRegister(
                device, pFlcn,
                RM_SOE_UNIT_CCI,
                NULL,
                nvswitch_cci_soe_callback_ls10,
                NULL,
                &pSoe->cciEvtDesc);

    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to register CCI event handler.\n",
            __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    // Register Heartbeat callback funcion
    status = flcnQueueEventRegister(
                device, pFlcn,
                RM_SOE_UNIT_HEARTBEAT,
                NULL,
                nvswitch_heartbeat_soe_callback_ls10,
                NULL,
                &pSoe->heartbeatEvtDesc);

    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to register Heartbeat event handler.\n",
            __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

void
nvswitch_soe_unregister_events_ls10
(
    nvswitch_device *device
)
{
    PFLCN pFlcn = device->pSoe->pFlcn;
    PSOE   pSoe  = (PSOE)device->pSoe;
    NV_STATUS status;

    // un-register thermal callback funcion
    status = flcnQueueEventUnregister(device, pFlcn,
                                      pSoe->thermEvtDesc);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to un-register thermal event handler.\n",
            __FUNCTION__);
    }
    // un-register thermal callback funcion
    status = flcnQueueEventUnregister(device, pFlcn,
                                      pSoe->cciEvtDesc);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to un-register cci event handler.\n",
            __FUNCTION__);
    }

    // un-register heartbeat callback funcion
    status = flcnQueueEventUnregister(device, pFlcn,
                                      pSoe->heartbeatEvtDesc);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to un-register heartbeat event handler.\n",
            __FUNCTION__);
    }
}

/*!
 * @brief Determine if the SOE RISCV CPU is halted
 *
 * @param[in] device         nvswitch_device  pointer
 * @param[in] pSoe           SOE  pointer
 *
 * @return NvBool reflecting the SOE Riscv CPU halted state
 */
static NvBool
_soeIsCpuHalted_LS10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    NvU32 data;

    data = NVSWITCH_SOE_RD32_LS10(device, 0, _SOE_RISCV, _CPUCTL);
    return FLD_TEST_DRF(_PFALCON, _FALCON_CPUCTL, _HALTED, _TRUE, data);
}

static NvU32
_soeIntrStatus_LS10
(
    nvswitch_device *device
)
{
    NvU32 irq, mask, dest;

    irq =  NVSWITCH_SOE_RD32_LS10(device, 0, _SOE_FALCON, _IRQSTAT);
    mask = NVSWITCH_SOE_RD32_LS10(device, 0, _SOE_RISCV,  _IRQMASK);
    dest = NVSWITCH_SOE_RD32_LS10(device, 0, _SOE_RISCV,  _IRQDEST);

    return (irq & mask & dest);
}

/*!
 * @brief Top level service routine
 *
 * @param[in] device         nvswitch_device  pointer
 * @param[in] pSoe           SOE  pointer
 *
 * @return 32-bit interrupt status AFTER all known interrupt-sources were
 *         serviced.
 */
static NvU32
_soeService_LS10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    NvBool  bRecheckMsgQ    = NV_FALSE;
    NvU32   clearBits       = 0;
    NvU32   intrStatus;
    PFLCN   pFlcn  = ENG_GET_FLCN(pSoe);

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Get the IRQ status and mask the sources not directed to host.
    intrStatus =  _soeIntrStatus_LS10(device);

    // Exit if there is nothing to do
    if (intrStatus == 0)
    {
       return 0;
    }

    // Service pending interrupts
    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _WDTMR, _TRUE))
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_WATCHDOG,
            "SOE Watchdog error\n");
        NVSWITCH_PRINT(device, INFO,
                    "%s: Watchdog timer fired. We do not support this "
                    "yet.\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);

        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _WDTMR, _SET);
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _EXTERR, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _EXTERR, _SET);

        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_EXTERR, "SOE EXTERR\n");
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _HALT, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _HALT, _SET);

        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_HALT, "SOE HALTED\n");
        soeServiceHalt_HAL(device, pSoe);
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _SWGEN0, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _SWGEN0, _SET);

        NVSWITCH_PRINT(device, MMIO,
                    "%s: Received a message from SOE via SWGEN0\n",
                    __FUNCTION__);
        soeProcessMessages_HAL(device, pSoe);
        bRecheckMsgQ = NV_TRUE;
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _SWGEN1, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _SWGEN1, _SET);

        NVSWITCH_PRINT(device, INFO,
                    "%s: Received a SWGEN1 interrupt\n",
                    __FUNCTION__);
    }

    // Clear any sources that were serviced and get the new status.
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQSCLR, clearBits);

    // Re-read interrupt status before retriggering to return correct value
    intrStatus =  _soeIntrStatus_LS10(device);

    //
    // If we just processed a SWGEN0 message queue interrupt, peek
    // into the message queue and see if any messages were missed the last time
    // the queue was purged (above). If it is not empty, re-generate SWGEN0
    // (since it is now cleared) and exit. As long as an interrupt is pending,
    // this function will be re-entered and the message(s) will be processed.
    //
    if (bRecheckMsgQ)
    {
        PFALCON_QUEUE_INFO      pQueueInfo;
        FLCNQUEUE              *pMsgQ;

        pQueueInfo = pFlcn->pQueueInfo;

        NVSWITCH_ASSERT(pQueueInfo != NULL);
        NVSWITCH_ASSERT(pQueueInfo->pQueues != NULL);

        pMsgQ = &pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID];

        if (!pMsgQ->isEmpty(device, pFlcn, pMsgQ))
        {
           // It is not necessary to RMW IRQSSET (zeros are ignored)
           flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQSSET,
                            DRF_DEF(_PFALCON, _FALCON_IRQSSET, _SWGEN0, _SET));
        }
    }

    flcnIntrRetrigger_HAL(device, pFlcn);

    return intrStatus;
}

/*!
 * Called by soeService to handle a SOE halt. This function will dump the
 * current status of SOE and then trap the CPU for further inspection for a
 * debug build.
 *
 * @param[in]  device  nvswitch_device pointer
 * @param[in]  pSoe    SOE object pointer
 */
static void
_soeServiceHalt_LS10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    PFLCN    pFlcn = ENG_GET_FLCN(pSoe);

    NVSWITCH_PRINT(device, ERROR,
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                "!!                   ** SOE HALTED **                !!\n"
                "!! Please file a bug with the following information. !!\n"
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
    dumpDebugRegisters(device);
#endif // defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

    flcnDbgInfoCaptureRiscvPcTrace_HAL(device, pFlcn);
    NVSWITCH_ASSERT(0);
}

/*!
 * Use the SOE INIT Message to construct, initialize or update SOE Queues.
 *
 * @param[in]      device nvswitch_device pointer
 * @param[in]      pSoe   SOE object pointer
 *
 * @return 'NV_OK' upon successful creation of all SOE Queues
 */
static NV_STATUS
_soeUpdateInitMsgQueuesInfo
(
    nvswitch_device  *device,
    PSOE              pSoe
)
{
    FLCNQUEUE                *pQueue;
    PFLCN                     pFlcn = ENG_GET_FLCN(pSoe);
    PFALCON_QUEUE_INFO        pQueueInfo;
    NvU32                     queueOffset;
    NvU16                     queueSize;
    NvU8                      queuePhyId;
    NvU8                      queueLogId;
    NV_STATUS                 status;

    //
    // No command should be longer than half the queue size.
    // See _soeQueueCmdValidate_IMPL().
    //
    ct_assert(sizeof(RM_FLCN_CMD_SOE) <= (SOE_CMD_QUEUE_LENGTH / 2));

    NVSWITCH_ASSERT(pFlcn != NULL);

    pQueueInfo = pFlcn->pQueueInfo;
    NVSWITCH_ASSERT(pQueueInfo != NULL);

    // Construct DMEM for CMDQ
    queueOffset = SOE_EMEM_CHANNEL_CMDQ_OFFSET;
    queueSize   = SOE_CMD_QUEUE_LENGTH;
    queuePhyId  = 0;
    queueLogId  = SOE_RM_CMDQ_LOG_ID;
    pQueue = &pQueueInfo->pQueues[queueLogId];
    status = flcnQueueConstruct_dmem_nvswitch(
                 device,
                 pFlcn,
                 &pQueue,                  // ppQueue
                 queueLogId,               // Logical ID of the queue
                 queuePhyId,               // Physical ID of the queue
                 queueOffset,              // offset
                 queueSize,                // size
                 RM_FLCN_QUEUE_HDR_SIZE);  // cmdHdrSize
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error constructing SOE CmdQueue (status="
            "0x%08x).\n", __FUNCTION__, status);
        NVSWITCH_ASSERT(0);
        return status;
    }

    // Construct DMEM for MSGQ
    queueOffset = SOE_EMEM_CHANNEL_MSGQ_OFFSET;
    queueSize   = SOE_MSG_QUEUE_LENGTH;
    queuePhyId  = 0;
    queueLogId  = SOE_RM_MSGQ_LOG_ID;
    pQueue = &pQueueInfo->pQueues[queueLogId];
    status = flcnQueueConstruct_dmem_nvswitch(
                 device,
                 pFlcn,
                 &pQueue,                  // ppQueue
                 queueLogId,               // Logical ID of the queue
                 queuePhyId,               // Physical ID of the queue
                 queueOffset,              // offset
                 queueSize,                // size
                 RM_FLCN_QUEUE_HDR_SIZE);  // cmdHdrSize
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error constructing SOE MsgQueue (status="
            "0x%08x).\n", __FUNCTION__, status);
        NVSWITCH_ASSERT(0);
        return status;
    }

    pFlcn->bOSReady = NV_TRUE;

    return NV_OK;
}

static NV_STATUS
_soeWaitForInitAck_LS10
(
    nvswitch_device    *device,
    PSOE               pSoe
)
{
    NV_STATUS status;
    PFLCN     pFlcn  = ENG_GET_FLCN(pSoe);

    if (!_nvswitch_is_soe_attached_ls10(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s SOE is not to attached!\n",
            __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return NV_ERR_NOT_READY;
    }

    // Update InitMsg Queue info before sending any commands
    if (!pFlcn->bOSReady)
    {
        status = _soeUpdateInitMsgQueuesInfo(device, pSoe);
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s Failed to attach driver!\n",
                __FUNCTION__);
                NVSWITCH_ASSERT(0);
                return status;
        }

        // Attach driver to SOE Queues
        _nvswitch_soe_attach_detach_driver_ls10(device, NV_TRUE);
    }

    return NV_OK;
}

/*!
 * Purges all the messages from the SOE's message queue.  Each message will
 * be analyzed, clients will be notified of status, and events will be routed
 * to all registered event listeners.
 *
 * @param[in]  device nvswitch_device pointer
 * @param[in]  pSoe   SOE object pointer
 *
 * @return 'NV_OK' if the message queue was successfully purged.
 */
static NV_STATUS
_soeProcessMessages_LS10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    RM_FLCN_MSG_SOE  soeMessage;
    NV_STATUS        status;
    PFLCN            pFlcn  = ENG_GET_FLCN(pSoe);

    // Update InitMsg Queue info before recieving any messages
    if (!pFlcn->bOSReady)
    {
        status = _soeUpdateInitMsgQueuesInfo(device, pSoe);
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s Failed to attach driver!\n",
                __FUNCTION__);
                NVSWITCH_ASSERT(0);
                return status;
        }

        // Attach driver to SOE Queues
        _nvswitch_soe_attach_detach_driver_ls10(device, NV_TRUE);
    }


    // keep processing messages until no more exist in the message queue
    while (NV_OK == (status = flcnQueueReadData(
                                     device,
                                     pFlcn,
                                     SOE_RM_MSGQ_LOG_ID,
                                     (RM_FLCN_MSG *)&soeMessage, NV_TRUE)))
    {
        NVSWITCH_PRINT(device, MMIO,
                    "%s: unitId=0x%02x, size=0x%02x, ctrlFlags=0x%02x, " \
                    "seqNumId=0x%02x\n",
                    __FUNCTION__,
                    soeMessage.hdr.unitId,
                    soeMessage.hdr.size,
                    soeMessage.hdr.ctrlFlags,
                    soeMessage.hdr.seqNumId);

        // check to see if the message is a reply or an event.
        if ((soeMessage.hdr.ctrlFlags &= RM_FLCN_QUEUE_HDR_FLAGS_EVENT) != 0)
        {
            flcnQueueEventHandle(device, pFlcn, (RM_FLCN_MSG *)&soeMessage, NV_OK);
        }
        // the message is a response from a previously queued command
        else
        {
            flcnQueueResponseHandle(device, pFlcn, (RM_FLCN_MSG *)&soeMessage);
        }
    }

    //
    // Status NV_ERR_NOT_READY implies, Queue is empty.
    // Log the message in other error cases.
    //
    if (status != NV_ERR_NOT_READY)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: unexpected error while purging message queue (status=0x%x).\n",
            __FUNCTION__, (status));
    }

    return status;
}

static NV_STATUS
_soeHandleInitEvent_LS10
(
    nvswitch_device  *device,
    PFLCNABLE         pSoe,
    RM_FLCN_MSG      *pGenMsg
)
{
    NVSWITCH_PRINT(device, ERROR,
        "%s: Init handle event not Supported!\n",
        __FUNCTION__);
    NVSWITCH_ASSERT(0);
    return NV_ERR_GENERIC;
}

static NvlStatus
_soeI2CAccessSend
(
    nvswitch_device *device,
    void            *cpuAddr,
    NvU64           dmaHandle,
    NvU16           xferSize
)
{
    FLCN *pFlcn       = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    RM_SOE_CORE_CMD_I2C *pI2cCmd;
    NVSWITCH_TIMEOUT    timeout;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, I2C);

    pI2cCmd = &cmd.cmd.core.i2c;
    RM_FLCN_U64_PACK(&pI2cCmd->dmaHandle, &dmaHandle);
    pI2cCmd->xferSize    = xferSize;
    pI2cCmd->cmdType = RM_SOE_CORE_CMD_I2C_ACCESS;

    cmdSeqDesc = 0;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send I2C command to SOE status 0x%x\n", 
                       __FUNCTION__, status);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

static NvlStatus 
_soeI2cFlcnStatusToNvlStatus
(
    NvU8 flcnStatus
)
{
    switch (flcnStatus)
    {
        case FLCN_OK: 
            return NVL_SUCCESS;
        case FLCN_ERR_INVALID_ARGUMENT: 
            return -NVL_BAD_ARGS;
        case FLCN_ERR_OBJECT_NOT_FOUND: 
            return -NVL_NOT_FOUND;
        case FLCN_ERROR: 
            return -NVL_ERR_GENERIC;
        case FLCN_ERR_INVALID_STATE: 
            return -NVL_ERR_INVALID_STATE;
        case FLCN_ERR_MORE_PROCESSING_REQUIRED: 
            return -NVL_MORE_PROCESSING_REQUIRED;
        case FLCN_ERR_TIMEOUT: 
            return -NVL_IO_ERROR;
        case FLCN_ERR_I2C_BUSY: 
            return -NVL_ERR_STATE_IN_USE;
        case FLCN_ERR_NOT_SUPPORTED: 
            return -NVL_ERR_NOT_SUPPORTED;
        default:
            return -NVL_ERR_GENERIC;
    }
}

static NvlStatus
_soeI2CAccess_LS10
(
    nvswitch_device  *device,
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams
)
{
    NvlStatus        ret;
    NvU8             flcnRet;
    PNVSWITCH_OBJI2C pI2c;
    void             *pCpuAddr;
    NvU64            dmaHandle;

    if (pParams == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    pI2c = device->pI2c;
    
    if (pI2c == NULL || !pI2c->soeI2CSupported)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pCpuAddr = pI2c->pCpuAddr;
    dmaHandle = pI2c->dmaHandle;

    ret = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
                                              SOE_I2C_DMA_BUF_SIZE,
                                              NVSWITCH_DMA_DIR_FROM_SYSMEM);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: nvswitch_os_sync_dma_region_for_cpu returned %d\n",
                       __FUNCTION__,  ret);
        return ret; 
    }

    // Required for error reporting from SOE to driver
    ct_assert(sizeof(NVSWITCH_CTRL_I2C_INDEXED_PARAMS) < SOE_I2C_STATUS_INDEX);

    // Copy I2C struct into buffer
    nvswitch_os_memcpy(pCpuAddr, pParams, sizeof(NVSWITCH_CTRL_I2C_INDEXED_PARAMS));

    ret = nvswitch_os_sync_dma_region_for_device(device->os_handle, dmaHandle,
                                                 SOE_I2C_DMA_BUF_SIZE,
                                                 NVSWITCH_DMA_DIR_FROM_SYSMEM);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: nvswitch_os_sync_dma_region_for_device returned %d\n",
                       __FUNCTION__,  ret);
        return ret;
    }

    // Send I2C access command to SOE
    ret = _soeI2CAccessSend(device, pCpuAddr, dmaHandle, SOE_I2C_DMA_BUF_SIZE);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: _soeI2CAccessSend returned %d\n",
                       __FUNCTION__,  ret);
        return ret;
    }

    // Get result
    ret = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
                                              SOE_I2C_DMA_BUF_SIZE,
                                              NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: nvswitch_os_sync_dma_region_for_cpu returned %d\n",
                       __FUNCTION__, ret);
        return ret;
    }

    nvswitch_os_memcpy(pParams, pCpuAddr, sizeof(NVSWITCH_CTRL_I2C_INDEXED_PARAMS));

    // Return value of the I2C operation that was performed in SOE   
    flcnRet = ((NvU8*)pCpuAddr)[SOE_I2C_STATUS_INDEX];
    ret = _soeI2cFlcnStatusToNvlStatus(flcnRet);

    return ret;
}

/*
 * @Brief : Send TNVL Pre Lock command to SOE
 *
 * @param[in] device
 */
NvlStatus
nvswitch_send_tnvl_prelock_cmd_ls10
(
    nvswitch_device *device
)
{
    FLCN            *pFlcn;
    NvU32            cmdSeqDesc = 0;
    NV_STATUS        status;
    RM_FLCN_CMD_SOE  cmd;
    NVSWITCH_TIMEOUT timeout;
    RM_SOE_TNVL_CMD_PRE_LOCK_SEQUENCE *pTnvlPreLock;
    NVSWITCH_GET_BIOS_INFO_PARAMS params = { 0 };

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO, "%s: SOE is not supported\n",
                       __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = device->hal.nvswitch_ctrl_get_bios_info(device, &params);
    if ((status != NVL_SUCCESS) || ((params.version & SOE_VBIOS_VERSION_MASK) <
            SOE_VBIOS_REVLOCK_TNVL_PRELOCK_COMMAND))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Skipping TNVL_CMD_PRE_LOCK_SEQUENCE command to SOE.  Update firmware "
            "from .%02X to .%02X\n",
            __FUNCTION__, (NvU32)((params.version & SOE_VBIOS_VERSION_MASK) >> 16), 
            SOE_VBIOS_REVLOCK_TNVL_PRELOCK_COMMAND);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pFlcn       = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_TNVL;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(TNVL, PRE_LOCK_SEQUENCE);

    pTnvlPreLock = &cmd.cmd.tnvl.preLockSequence;
    pTnvlPreLock->cmdType = RM_SOE_TNVL_CMD_ISSUE_PRE_LOCK_SEQUENCE;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      NULL,                 // pMsg
                                      NULL,                 // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send PRE_LOCK_SEQUENCE command to SOE, status 0x%x\n", 
                       __FUNCTION__, status);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/**
 * @brief   set hal function pointers for functions defined in LR10 (i.e. this file)
 *
 * this function has to be at the end of the file so that all the
 * other functions are already defined.
 *
 * @param[in] pFlcnable   The flcnable for which to set hals
 */
void
soeSetupHal_LS10
(
    SOE *pSoe
)
{
    soe_hal *pHal = pSoe->base.pHal;
    flcnable_hal *pParentHal = (flcnable_hal *)pHal;

    soeSetupHal_LR10(pSoe);

    pParentHal->fetchEngines = _soeFetchEngines_LS10;
    pParentHal->handleInitEvent = _soeHandleInitEvent_LS10;

    pHal->isCpuHalted        = _soeIsCpuHalted_LS10;
    pHal->service            = _soeService_LS10;
    pHal->serviceHalt        = _soeServiceHalt_LS10;
    pHal->processMessages    = _soeProcessMessages_LS10;
    pHal->waitForInitAck     = _soeWaitForInitAck_LS10;
    pHal->i2cAccess          = _soeI2CAccess_LS10;
}
