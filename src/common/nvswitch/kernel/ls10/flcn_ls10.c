/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "common_nvswitch.h"
#include "ls10/ls10.h"
#include "flcn/flcn_nvswitch.h"

#include "nvswitch/ls10/dev_falcon_v4.h"
#include "nvswitch/ls10/dev_riscv_pri.h"

//
// Functions shared with LR10
//

void flcnSetupHal_LR10(PFLCN pFlcn);

static NvU32
_flcnRiscvRegRead_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            offset
)
{
    // Probably should perform some checks on the offset, the device, and the engine descriptor
    return nvswitch_reg_read_32(device, NV_FALCON2_SOE_BASE + offset);
}

static void
_flcnRiscvRegWrite_LS10
(
    nvswitch_device    *device,
    PFLCN               pFlcn,
    NvU32               offset,
    NvU32               data
)
{
    // Probably should perform some checks on the offset, the device, and the engine descriptor
    nvswitch_reg_write_32(device, NV_FALCON2_SOE_BASE + offset, data);
}

/*!
 * @brief Retrieve the size of the falcon data memory.
 *
 * @param[in]  device           nvswitch_device pointer
 * @param[in]  pFlcn            Falcon object pointer
 * @param[in]  bFalconReachable If set, returns size that can be reached by Falcon
 *
 * @return IMEM size in bytes
 */
static NvU32
_flcnDmemSize_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NvU32 data = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_HWCFG3);
    return (DRF_VAL(_PFALCON, _FALCON_HWCFG3, _DMEM_TOTAL_SIZE, data) <<
            FALCON_DMEM_BLKSIZE2);
}

/*
 * @brief Get the destination after masking
 * off all but the OFFSET and BLOCK in IMEM
 *
 * @param[in] dst   Destination in IMEM
 *
 * @returns dst with only OFFSET and BLOCK bits set
 */
static NvU32
_flcnSetImemAddr_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst
)
{
    NVSWITCH_ASSERT(0);
    return 0;
}

/*!
 *
 * @brief Copy contents of pSrc to IMEM
 *
 * @param[in] device        nvswitch_device pointer
 * @param[in] pFlcn         Falcon object pointer
 * @param[in] dst           Destination in IMEM
 * @param[in] pSrc          IMEM contents
 * @param[in] sizeInBytes   Total IMEM size in bytes
 * @param[in] bSecure       NV_TRUE if IMEM is signed
 * @param[in] tag           IMEM tag
 * @param[in] port          PMB port to copy IMEM
 *
 * @returns void
 */
static void
_flcnImemCopyTo_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst,
    NvU8            *pSrc,
    NvU32            sizeBytes,
    NvBool           bSecure,
    NvU32            tag,
    NvU8             port
)
{
}

/*!
 *
 * @brief Mask the DMEM destination to have only the BLK and OFFSET bits set
 *
 * @param[in] dst           Destination in DMEM
 *
 * @returns masked destination value in DMEM
 */

static NvU32
_flcnSetDmemAddr_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst
)
{
    return (dst & (DRF_SHIFTMASK(NV_PFALCON_FALCON_DMEMC_OFFS) |
                   DRF_SHIFTMASK(NV_PFALCON_FALCON_DMEMC_BLK)));
}

/*!
 * Depending on the direction of the copy, copies 'sizeBytes' to/from 'pBuf'
 * from/to DMEM offset 'dmemAddr' using DMEM access port 'port'.
 *
 * @param[in]  device     nvswitch_device pointer
 * @param[in]  pFlcn      Falcon object pointer
 * @param[in]  dmemAddr   The DMEM offset for the copy
 * @param[in]  pBuf       The pointer to the buffer containing the data to copy
 * @param[in]  sizeBytes  The number of bytes to copy
 * @param[in]  port       The DMEM port index to use when accessing DMEM
 * @param[in]  bCopyFrom  Boolean representing the copy direction (to/from DMEM)
 *
 * @return NV_OK                    if the data was successfully copied
 *         NV_ERR_INVALID_ARGUMENT  if the input argument(s) is/are invalid
 */
static NV_STATUS
_flcnDmemTransfer_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dmemAddr,
    NvU8            *pBuf,
    NvU32            sizeBytes,
    NvU8             port,
    NvBool           bCopyFrom
)
{
    NvU32  numWords;
    NvU32  numBytes;
    NvU32 *pData = (NvU32 *)pBuf;
    NvU32  reg32;
    NvU32  i;

    // simply return if the copy-size is zero
    if (sizeBytes == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "Zero-byte copy requested\n");
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    // the DMEM address must be 4-byte aligned
    if (!NV_IS_ALIGNED(dmemAddr, FLCN_DMEM_ACCESS_ALIGNMENT))
    {
        NVSWITCH_PRINT(device, ERROR, "Source not 4-byte aligned. dmemAddr=0x%08x\n", dmemAddr);
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    // calculate the number of words and bytes
    numWords = sizeBytes >> 2;
    numBytes = sizeBytes & NVSWITCH_MASK_BITS(2);

    // mask off all but the OFFSET and BLOCK in DMEM offset
    reg32 = flcnSetDmemAddr_HAL(device, pFlcn, dmemAddr);

    if (bCopyFrom)
    {
        // mark auto-increment on read
        reg32 = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMEMC, _AINCR, 0x1, reg32);
    }
    else
    {
        // mark auto-increment on write
        reg32 = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMEMC, _AINCW, 0x1, reg32);
    }

    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_DMEMC(port), reg32);

    // directly copy as many words as possible
    for (i = 0; i < numWords; i++)
    {
        if (bCopyFrom)
        {
            pData[i] = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMEMD(port));
        }
        else
        {
            flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_DMEMD(port), pData[i]);
        }
    }

    // Check if there are left over bytes to copy
    if (numBytes > 0)
    {
        NvU32 bytesCopied = numWords << 2;

        //
        // Read the contents first. If we're copying to the DMEM, we've set
        // autoincrement on write, so reading does not modify the pointer. We
        // can, thus, do a read/modify/write without needing to worry about the
        // pointer having moved forward. There is no special explanation needed
        // if we're copying from the DMEM since this is the last access to HW
        // in that case.
        //
        reg32 = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMEMD(port));

        if (bCopyFrom)
        {
            // Copy byte-by-byte into the buffer as required
            for (i = 0; i < numBytes; i++)
            {
                pBuf[bytesCopied + i] = ((NvU8 *)&reg32)[i];
            }
        }
        else
        {
            // Modify what we read byte-by-byte before writing to dmem
            for (i = 0; i < numBytes; i++)
            {
                ((NvU8 *)&reg32)[i] = pBuf[bytesCopied + i];
            }
            flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_DMEMD(port), reg32);
        }
    }
    return NVL_SUCCESS;
}

static void
_flcnDbgInfoCaptureRiscvPcTrace_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NvU32 ctl, ridx, widx, bufferSize;
    NvBool bWasFull;
    NvU64 bios_version;

    // Only supported on riscv
    if (!UPROC_ENG_ARCH_FALCON_RISCV(pFlcn))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: is not supported on falcon\n",
            __FUNCTION__);

        NVSWITCH_ASSERT(0);
        return;
    }

    ctl = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACECTL);
    if (ctl == 0)
    {
          NVSWITCH_PRINT(device, ERROR, "Trace buffer is disabled.\n");
          return;
    }


    bWasFull = FLD_TEST_DRF_NUM(_PRISCV_RISCV, _TRACECTL,_FULL, 1, ctl);

    if (bWasFull)
    {
        NVSWITCH_PRINT(device, INFO, "%s: Trace buffer full. Entries may have been lost.\n", __FUNCTION__);
    }

    // Reset and disable buffer, we don't need it during dump (and if core is running)
    flcnRiscvRegWrite_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACECTL, 0);

    widx = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACE_WTIDX);
    widx = DRF_VAL(_PRISCV_RISCV, _TRACE_WTIDX, _WTIDX, widx);

    ridx = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACE_RDIDX);
    bufferSize = DRF_VAL(_PRISCV_RISCV, _TRACE_RDIDX, _MAXIDX, ridx);
    ridx = DRF_VAL(_PRISCV_RISCV, _TRACE_RDIDX, _RDIDX, ridx);

    if (bufferSize > 0)
    {
        NvU32 entry;

        switch (DRF_VAL(_PRISCV_RISCV, _TRACECTL, _MODE, ctl))
        {
            case NV_PRISCV_RISCV_TRACECTL_MODE_FULL:
                NVSWITCH_PRINT(device, ERROR, "Tracebuffer is in full mode.\n");
                break;
            case NV_PRISCV_RISCV_TRACECTL_MODE_REDUCED:
                NVSWITCH_PRINT(device, ERROR, "Tracebuffer is in reduced mode.\n");
                break;
            case NV_PRISCV_RISCV_TRACECTL_MODE_STACK:
                NVSWITCH_PRINT(device, ERROR, "Tracebuffer is in stack mode.\n");
                break;
            default:
                NVSWITCH_PRINT(device, ERROR, "Tracebuffer is in unknown mode.\n");
        }

        NVSWITCH_PRINT(device, ERROR, "Entries (most recent first):\n");

        ridx = widx;
        for (entry = 0; entry < bufferSize; entry++)
        {
            NvU64 pc;

            ridx = ridx > 0 ? ridx - 1 : bufferSize - 1;
            flcnRiscvRegWrite_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACE_RDIDX,
                                  DRF_NUM(_PRISCV_RISCV, _TRACE_RDIDX, _RDIDX, ridx));

            pc = flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACEPC_HI);
            pc = (pc << 32) | flcnRiscvRegRead_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACEPC_LO);

            // Non-mod2 values are invalid here, so stop (this likely indicates an init-marker val)
            if (NvU64_LO32(pc) % 2U != 0U)
            {
                break;
            }

            NVSWITCH_PRINT_SXID_NO_BBX(device, NVSWITCH_ERR_HW_SOE_HALT, "SOE HALT data[%d] = 0x%16llx\n", entry, pc);
        }

        // Print VBIOS version at the end
        nvswitch_lib_get_bios_version(device, &bios_version);
        NVSWITCH_PRINT_SXID_NO_BBX(device, NVSWITCH_ERR_HW_SOE_HALT,
            "SOE HALT data[%d] = 0x%16llx\n", entry, bios_version);
    }

    // reset trace buffer
    flcnRiscvRegWrite_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACE_RDIDX, 0);
    flcnRiscvRegWrite_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACE_WTIDX, 0);

    // Clear full and empty bits
    ctl = FLD_SET_DRF_NUM(_PRISCV_RISCV, _TRACECTL, _FULL, 0, ctl);
    ctl = FLD_SET_DRF_NUM(_PRISCV_RISCV, _TRACECTL, _EMPTY, 0, ctl);
    flcnRiscvRegWrite_HAL(device, pFlcn, NV_PRISCV_RISCV_TRACECTL, ctl);
}

static NV_STATUS
_flcnDebugBufferInit_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            debugBufferMaxSize,
    NvU32            writeRegAddr,
    NvU32            readRegAddr
)
{
    return NVL_SUCCESS;
}

static NV_STATUS
_flcnDebugBufferDestroy_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    return NVL_SUCCESS;
}

static NV_STATUS
_flcnDebugBufferDisplay_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    return NVL_SUCCESS;
}

static NvBool
_flcnDebugBufferIsEmpty_LS10
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    return NV_TRUE;
}

//
// Store pointers to ucode header and data.
// Preload ucode from registry if available.
//
NV_STATUS
_flcnConstruct_LS10
(
    nvswitch_device    *device,
    PFLCN               pFlcn
)
{
    NV_STATUS          status;
    PFLCNABLE          pFlcnable = pFlcn->pFlcnable;
    PFALCON_QUEUE_INFO pQueueInfo;
    pFlcn->bConstructed         = NV_TRUE;

    // Set arch to Riscv
    pFlcn->engArch = NV_UPROC_ENGINE_ARCH_FALCON_RISCV;

    // Allocate the memory for Queue Data Structure if needed.
    if (pFlcn->bQueuesEnabled)
    {
        pQueueInfo = pFlcn->pQueueInfo = nvswitch_os_malloc(sizeof(*pQueueInfo));
        if (pQueueInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            NVSWITCH_ASSERT(0);
            goto _flcnConstruct_LR10_fail;
        }
        nvswitch_os_memset(pQueueInfo, 0, sizeof(FALCON_QUEUE_INFO));
        // Assert if Number of Queues are zero
        NVSWITCH_ASSERT(pFlcn->numQueues != 0);
        pQueueInfo->pQueues = nvswitch_os_malloc(sizeof(FLCNQUEUE) * pFlcn->numQueues);
        if (pQueueInfo->pQueues == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            NVSWITCH_ASSERT(0);
            goto _flcnConstruct_LR10_fail;
        }
        nvswitch_os_memset(pQueueInfo->pQueues, 0, sizeof(FLCNQUEUE) * pFlcn->numQueues);
        // Sequences can be optional
        if (pFlcn->numSequences != 0)
        {
            if ((pFlcn->numSequences - 1) > ((NvU32)NV_U8_MAX))
            {
                status = NV_ERR_OUT_OF_RANGE;
                NVSWITCH_PRINT(device, ERROR,
                          "Max numSequences index = %d cannot fit into byte\n",
                          (pFlcn->numSequences - 1));
                NVSWITCH_ASSERT(0);
                goto _flcnConstruct_LR10_fail;
            }
            flcnQueueSeqInfoStateInit(device, pFlcn);
        }
    }
    // DEBUG
    NVSWITCH_PRINT(device, INFO, "Falcon: %s\n", flcnGetName_HAL(device, pFlcn));
    NVSWITCH_ASSERT(pFlcnable != NULL);
    flcnableGetExternalConfig(device, pFlcnable, &pFlcn->extConfig);
    return NV_OK;
_flcnConstruct_LR10_fail:
    // call flcnDestruct to free the memory allocated in this construct function
    flcnDestruct_HAL(device, pFlcn);
    return status;
}

/**
 * @brief   set hal function pointers for functions defined in
 *          LS10 (i.e. this file)
 *
 * this function has to be at the end of the file so that all the
 * other functions are already defined.
 *
 * @param[in] pFlcn   The flcn for which to set hals
 */
void
flcnSetupHal_LS10
(
    PFLCN            pFlcn
)
{
    flcn_hal *pHal = pFlcn->pHal;

    flcnSetupHal_LR10(pFlcn);
    pHal->riscvRegRead                  =  _flcnRiscvRegRead_LS10;
    pHal->riscvRegWrite                 =  _flcnRiscvRegWrite_LS10;
    pHal->dmemTransfer                  =  _flcnDmemTransfer_LS10;
    pHal->setDmemAddr                   =  _flcnSetDmemAddr_LS10;
    pHal->imemCopyTo                    =  _flcnImemCopyTo_LS10;
    pHal->setImemAddr                   =  _flcnSetImemAddr_LS10;
    pHal->dmemSize                      =  _flcnDmemSize_LS10;
    pHal->dbgInfoCaptureRiscvPcTrace    =  _flcnDbgInfoCaptureRiscvPcTrace_LS10;
    pHal->debugBufferInit               =  _flcnDebugBufferInit_LS10;
    pHal->debugBufferDestroy            =  _flcnDebugBufferDestroy_LS10;
    pHal->debugBufferDisplay            =  _flcnDebugBufferDisplay_LS10;
    pHal->debugBufferIsEmpty            =  _flcnDebugBufferIsEmpty_LS10;
    pHal->construct                     = _flcnConstruct_LS10;
}
