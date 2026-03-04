/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Provides GA102+ specific KernelGsp HAL implementations related to
 * execution of Falcon cores.
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/gpu.h"
#include "gpu/falcon/kernel_falcon.h"

#include "published/ampere/ga102/dev_falcon_v4.h"
#include "published/ampere/ga102/dev_falcon_second_pri.h"
#include "published/ampere/ga102/dev_fbif_v4.h"

static GpuWaitConditionFunc s_dmaPollCondFunc;

typedef struct {
    KernelFalcon *pKernelFlcn;
    NvU32 pollMask;
    NvU32 pollValue;
} DmaPollCondData;

static NvBool
s_dmaPollCondFunc
(
    OBJGPU *pGpu,
    void *pVoid
)
{
    DmaPollCondData *pData = (DmaPollCondData *)pVoid;
    return ((kflcnRegRead_HAL(pGpu, pData->pKernelFlcn, NV_PFALCON_FALCON_DMATRFCMD) & pData->pollMask) == pData->pollValue);
}

/*!
 * Poll on either _FULL or _IDLE field of NV_PFALCON_FALCON_DMATRFCMD
 *
 * @param[in]     pGpu            GPU object pointer
 * @param[in]     pKernelFlcn     pKernelFlcn object pointer
 * @param[in]     mode            FLCN_DMA_POLL_QUEUE_NOT_FULL for poll on _FULL; return when _FULL is false
 *                                FLCN_DMA_POLL_ENGINE_IDLE    for poll on _IDLE; return when _IDLE is true
 */
static NV_STATUS
s_dmaPoll_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    FlcnDmaPollMode mode
)
{
    NV_STATUS status;
    DmaPollCondData data;

    data.pKernelFlcn = pKernelFlcn;
    if (mode == FLCN_DMA_POLL_QUEUE_NOT_FULL)
    {
        data.pollMask = DRF_SHIFTMASK(NV_PFALCON_FALCON_DMATRFCMD_FULL);
        data.pollValue = DRF_DEF(_PFALCON, _FALCON_DMATRFCMD, _FULL, _FALSE);
    }
    else
    {
        data.pollMask = DRF_SHIFTMASK(NV_PFALCON_FALCON_DMATRFCMD_IDLE);
        data.pollValue = DRF_DEF(_PFALCON, _FALCON_DMATRFCMD, _IDLE, _TRUE);
    }

    status = gpuTimeoutCondWait(pGpu, s_dmaPollCondFunc, &data, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error while waiting for Falcon DMA; mode: %d, status: 0x%08x\n", mode, status);
        DBG_BREAKPOINT();
        return status;
    }

    return NV_OK;
}

static NV_STATUS
s_dmaTransfer_GA102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 dest,
    NvU32 memOff,
    RmPhysAddr srcPhysAddr,
    NvU32 sizeInBytes,
    NvU32 dmaCmd
)
{
    NV_STATUS status = NV_OK;
    NvU32 data;
    NvU32 bytesXfered = 0;

    // Ensure request queue initially has space or writing base registers will corrupt DMA transfer.
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, s_dmaPoll_GA102(pGpu, pKernelFlcn, FLCN_DMA_POLL_QUEUE_NOT_FULL));

    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMATRFBASE, NvU64_LO32(srcPhysAddr >> 8));
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMATRFBASE1, NvU64_HI32(srcPhysAddr >> 8) & 0x1FF);

    while (bytesXfered < sizeInBytes)
    {
        // Poll for non-full request queue as writing control registers when full will corrupt DMA transfer.
        NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, s_dmaPoll_GA102(pGpu, pKernelFlcn, FLCN_DMA_POLL_QUEUE_NOT_FULL));

        data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMATRFMOFFS, _OFFS, dest, 0);
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMATRFMOFFS, data);

        data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMATRFFBOFFS, _OFFS, memOff, 0);
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMATRFFBOFFS, data);

        // Write the command
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMATRFCMD, dmaCmd);

        bytesXfered += FLCN_BLK_ALIGNMENT;
        dest        += FLCN_BLK_ALIGNMENT;
        memOff      += FLCN_BLK_ALIGNMENT;
    }

    //
    // Poll for completion. GA10x+ does not have TCM tagging so DMA operations to/from TCM should
    // wait for DMA to complete before launching another operation to avoid memory ordering problems.
    //
    NV_CHECK_OK_OR_RETURN(LEVEL_SILENT, s_dmaPoll_GA102(pGpu, pKernelFlcn, FLCN_DMA_POLL_ENGINE_IDLE));

    return status;
}

/*!
 * Execute the HS falcon ucode provided in pFlcnUcode on the falcon engine
 * represented by pKernelFlcn and wait for its completion.
 *
 * For _GA102, pFlcnUcode must be of the BOOT_FROM_HS variant.
 *
 * Note: callers are expected to reset pKernelFlcn before calling this
 * function.

 * @param[in]     pGpu            GPU object pointer
 * @param[in]     pKernelGsp      KernelGsp object pointer
 * @param[in]     pFlcnUcode      Falcon ucode to execute
 * @param[in]     pKernelFlcn     KernelFalcon engine to execute on
 * @param[inout]  pMailbox0       Pointer to value of MAILBOX0 to provide/read (or NULL)
 * @param[inout]  pMailbox0       Pointer to value of MAILBOX1 to provide/read (or NULL)
 */
NV_STATUS
kgspExecuteHsFalcon_GA102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode *pFlcnUcode,
    KernelFalcon *pKernelFlcn,
    NvU32 *pMailbox0,
    NvU32 *pMailbox1
)
{
    NV_STATUS status;
    KernelGspFlcnUcodeBootFromHs *pUcode;

    NvU32 data = 0;
    NvU32 dmaCmd;

    NV_ASSERT_OR_RETURN(pFlcnUcode != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelFlcn != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pKernelFlcn->bBootFromHs, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pFlcnUcode->bootType == KGSP_FLCN_UCODE_BOOT_FROM_HS, NV_ERR_NOT_SUPPORTED);

    pUcode = &pFlcnUcode->ucodeBootFromHs;

    NV_ASSERT_OR_RETURN(pUcode->pUcodeMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(memdescGetAddressSpace(pUcode->pUcodeMemDesc) == ADDR_SYSMEM,
                        NV_ERR_INVALID_ARGUMENT);

    kflcnDisableCtxReq_HAL(pGpu, pKernelFlcn);

    // Program TRANSCFG to fetch the DMA data
    data = GPU_REG_RD32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_TRANSCFG(0 /* ctxDma */));
    data = FLD_SET_DRF(_PFALCON, _FBIF_TRANSCFG, _TARGET, _COHERENT_SYSMEM, data);
    data = FLD_SET_DRF(_PFALCON, _FBIF_TRANSCFG, _MEM_TYPE, _PHYSICAL, data);
    GPU_REG_WR32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_TRANSCFG(0 /* ctxDma */), data);

    // Prepare DMA command
    dmaCmd = 0;
    dmaCmd = FLD_SET_DRF(_PFALCON, _FALCON_DMATRFCMD, _WRITE, _FALSE, dmaCmd);
    dmaCmd = FLD_SET_DRF(_PFALCON, _FALCON_DMATRFCMD, _SIZE, _256B, dmaCmd);
    dmaCmd = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMATRFCMD, _CTXDMA, 0, dmaCmd);

    // Prepare DMA command for IMEM
    dmaCmd = FLD_SET_DRF(_PFALCON, _FALCON_DMATRFCMD, _IMEM, _TRUE, dmaCmd);
    dmaCmd = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMATRFCMD, _SEC, 0x1, dmaCmd);

    // Perform DMA for IMEM
    {
        RmPhysAddr srcPhysAddr = memdescGetPhysAddr(pUcode->pUcodeMemDesc, AT_GPU, 0);
        srcPhysAddr = srcPhysAddr + pUcode->codeOffset - pUcode->imemVa;

        NV_ASSERT_OK_OR_RETURN(
            s_dmaTransfer_GA102(pGpu, pKernelFlcn,
                                pUcode->imemPa,    // dest
                                pUcode->imemVa,    // memOff
                                srcPhysAddr,       // srcPhysAddr
                                pUcode->imemSize,  // sizeInBytes
                                dmaCmd));
    }

    // Prepare DMA command for DMEM
    dmaCmd = FLD_SET_DRF(_PFALCON, _FALCON_DMATRFCMD, _IMEM, _FALSE, dmaCmd);
    dmaCmd = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMATRFCMD, _SEC, 0x0, dmaCmd);
    if (pUcode->dmemVa != FLCN_DMEM_VA_INVALID)
    {
        dmaCmd = FLD_SET_DRF(_PFALCON, _FALCON_DMATRFCMD, _SET_DMTAG, _TRUE, dmaCmd);
    }

    // Perform DMA for DMEM
    {
        NvU32 memOff = 0;

        RmPhysAddr srcPhysAddr = memdescGetPhysAddr(pUcode->pUcodeMemDesc, AT_GPU, 0);
        srcPhysAddr += pUcode->dataOffset;
        if (pUcode->dmemVa != FLCN_DMEM_VA_INVALID)
        {
            srcPhysAddr -= pUcode->dmemVa;
            memOff = pUcode->dmemVa;
        }

        NV_ASSERT_OK_OR_RETURN(
            s_dmaTransfer_GA102(pGpu, pKernelFlcn,
                                pUcode->dmemPa,    // dest
                                memOff,            // memOff
                                srcPhysAddr,       // srcPhysAddr
                                pUcode->dmemSize,  // sizeInBytes
                                dmaCmd));
    }

    // Program BROM registers for PKC signature validation.
    {
        kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON2_FALCON_BROM_PARAADDR(0), pUcode->hsSigDmemAddr);

        kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON2_FALCON_BROM_ENGIDMASK, pUcode->engineIdMask);

        kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON2_FALCON_BROM_CURR_UCODE_ID,
                               DRF_NUM(_PFALCON2_FALCON, _BROM_CURR_UCODE_ID, _VAL, pUcode->ucodeId));

        kflcnRiscvRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON2_FALCON_MOD_SEL,
                               DRF_NUM(_PFALCON2_FALCON, _MOD_SEL, _ALGO, NV_PFALCON2_FALCON_MOD_SEL_ALGO_RSA3K));
    }

    // Set BOOTVEC to start of secure code.
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_BOOTVEC, pUcode->imemVa);

    // Write mailboxes if requested.
    if (pMailbox0 != NULL)
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX0, *pMailbox0);
    if (pMailbox1 != NULL)
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX1, *pMailbox1);

    // Start CPU now.
    kflcnStartCpu_HAL(pGpu, pKernelFlcn);

    // Wait for completion.
    status = kflcnWaitForHalt_HAL(pGpu, pKernelFlcn, GPU_TIMEOUT_DEFAULT, 0);

    // Read mailboxes if requested.
    if (pMailbox0 != NULL)
        *pMailbox0 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX0);
    if (pMailbox1 != NULL)
        *pMailbox1 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX1);

    return status;
}
