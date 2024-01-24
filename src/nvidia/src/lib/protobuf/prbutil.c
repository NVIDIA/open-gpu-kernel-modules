/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "lib/protobuf/prb_util.h"

#include "os/os.h"

#include "g_regs_pb.h"
#include "g_all_dcl_pb.h"

/*!
 * @brief Encode Gpu registers. Wraps REG_RD32.
 * @param [in] pGpu The Gpu
 * @param [in] offset The offset of the regs to read
 * @param [in] numRegs Number of contiguous regs to encode
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGpuRegs(OBJGPU *pGpu, IoAperture *pAperture, NvU64 offset, NvU32 numRegs,
              PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;
    NV_STATUS statusEnd;
    NvU32     data;

    if (pAperture == NULL)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_GPU_REGS);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, REG_GET_ADDR(pAperture, offset));
    if (status != NV_OK)
        goto done;
    while (numRegs--)
    {
        data = REG_RD32(pAperture, (NvU32)offset);
        status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);
        if (status != NV_OK)
            goto done;
        offset += sizeof(NvU32);
    }

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}


/*!
 * @brief Encode Gpu register immediate.
 *        Use this version when you have the register in hand.
 * @param [in] pGpu The Gpu
 * @param [in] offset The offset of the regs to read
 * @param [in] reg The register to encode
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGpuRegImm(OBJGPU *pGpu, NvU64 offset, NvU32 reg,
                PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;
    NV_STATUS statusEnd;

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_GPU_REGS);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, offset);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, reg);
        goto done;

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Encode Gpu registers, table slice version
 *        Adds a base address to each offset. Useful for register
 *        sets like the tpc registers.
 * @param [in] pGpu The Gpu
 * @param [in] tbl The start of the table
 * @param [in] numEntries  Number of entries in the table
 * @param [in] base Base address for the slice of registers in the table
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGpuRegSliceTbl(OBJGPU *pGpu, IoAperture *pAperture, const PRB_GPU_REG_TABLE *pTbl, NvU32 numEntries, NvU32 base,
                PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status = NV_OK; // Init keeps the Mac compiler quiet
    NvU64     regAddr = 0;     // Ditto
    NvU32     stride = 0;     // Ditto
    NV_STATUS statusEnd;
    NvU32     data;
    NvU32     numRegs;
    NvBool    inMsg = NV_FALSE;
    NvU8      startingDepth = prbEncNestingLevel(pPrbEnc);

    if ((pAperture == NULL) || (pTbl == NULL) || (numEntries == 0))
        return NV_ERR_INVALID_REQUEST;

    //
    // This loop collapses registers that are adjacent in the table to
    // avoid repeatedly encoding the register address and to take advantage of the
    // protobuf packed encoding.
    //
    while(numEntries--)
    {
        if (!inMsg)
        {
            status = prbEncNestedStart(pPrbEnc, fieldDesc);
            if (status != NV_OK)
                goto done;

            inMsg = NV_TRUE;
            status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_GPU_REGS);
            if (status != NV_OK)
                goto done;

            regAddr = pTbl->offset + base;
            status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, REG_GET_ADDR(pAperture, regAddr));
            if (status != NV_OK)
                goto done;
            stride = pTbl->stride;
            if (stride != 4)
            {
                status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_STRIDE, stride);
                if (status != NV_OK)
                    goto done;
            }
        }
        for (numRegs = pTbl->numRegs; numRegs > 0; numRegs--)
        {
            data = REG_RD32(pAperture, (NvU32)regAddr);
            status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);
            if (status != NV_OK)
                goto done;
            regAddr += stride;
        }
        pTbl++;
        if (numEntries > 0 &&
            (regAddr != pTbl->offset + base ||
             stride != pTbl->stride))
        {
            status = prbEncNestedEnd(pPrbEnc);
            if (status != NV_OK)
                goto done;
            inMsg = NV_FALSE;
        }
    }

done:
    // Try to close even if preceding error
    statusEnd = prbEncUnwindNesting(pPrbEnc, startingDepth);

    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Encode Gpu registers, table slice indexed version.
 *        Adds a base address and an index multipled by the table length
 *        to each offset. Useful for register sets like the smc registers.
 * @param [in] pGpu The Gpu
 * @param [in] tbl The start of the table
 * @param [in] numEntries  Number of entries in the table
 * @param [in] base Base address for the slice of registers in the table
 * @param [in] index Index to apply to the ilen table entry
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGpuRegSliceIndexedTbl(OBJGPU *pGpu, IoAperture *pAperture, const PRB_GPU_REG_INDEXED_TABLE *pTbl, NvU32 numEntries,
                            NvU32 base, NvU32 index, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status = NV_OK; // Init keeps the Mac compiler quiet
    NvU64     regAddr = 0;     // Ditto
    NvU32     stride = 0;     // Ditto
    NV_STATUS statusEnd;
    NvU32     data;
    NvU32     numRegs;
    NvBool    inMsg = NV_FALSE;
    NvU8      startingDepth = prbEncNestingLevel(pPrbEnc);

    if (pTbl == NULL || numEntries == 0)
        return NV_ERR_INVALID_REQUEST;

    //
    // This loop collapses registers that are adjacent in the table to
    // avoid repeatedly encoding the register address and to take advantage of the
    // protobuf packed encoding.
    //
    while(numEntries--)
    {
        if (!inMsg)
        {
            status = prbEncNestedStart(pPrbEnc, fieldDesc);
            if (status != NV_OK)
                goto done;

            inMsg = NV_TRUE;
            status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_GPU_REGS);
            if (status != NV_OK)
                goto done;

            regAddr = pTbl->offset + base + (NvU64)index * pTbl->ilen;
            status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, REG_GET_ADDR(pAperture, regAddr));
            if (status != NV_OK)
                goto done;
            stride = pTbl->stride;
            if (stride != 4)
            {
                status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_STRIDE, stride);
                if (status != NV_OK)
                    goto done;
            }
        }
        for (numRegs = pTbl->numRegs; numRegs > 0; numRegs--)
        {
            data = REG_RD32(pAperture, (NvU32)regAddr);
            status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);
            if (status != NV_OK)
                goto done;
            regAddr += stride;
        }
        pTbl++;
        if (numEntries > 0 &&
            (regAddr != pTbl->offset + base + (NvU64)index * pTbl->ilen ||
             stride != pTbl->stride))
        {
            status = prbEncNestedEnd(pPrbEnc);
            if (status != NV_OK)
                goto done;
            inMsg = NV_FALSE;
        }
    }

done:
    // Try to close even if preceding error
    statusEnd = prbEncUnwindNesting(pPrbEnc, startingDepth);

    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Encode Gpu registers, table simplified version
 * @param [in] pGpu The Gpu
 * @param [in] pOffset The start of an array with register offsets
 * @param [in] numEntries  Number of entries in the table
 * @param [in] base Base address for the slice of registers in the table
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGpuRegSliceOffset(OBJGPU *pGpu, IoAperture *pAperture, const NvU32 *pOffset, NvU32 numEntries,
        NvU32 base, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *pFieldDesc)
{
    NV_STATUS status  = NV_OK;  // if numEntries == 0
    NvU32     regAddr = 0;      // Init keeps the Mac compiler quiet
    NvBool    inMsg   = NV_FALSE;
    NvU8      uDepth  = prbEncNestingLevel(pPrbEnc);

    if (pAperture == NULL)
    {
        return NV_ERR_INVALID_REQUEST;
    }

    //
    // This loop collapses registers that are adjacent in the table to
    // avoid repeatedly encoding the register address and to take advantage
    // of the protobuf packed encoding.
    //
    while(numEntries--)
    {
        if (!inMsg)
        {
            status = prbEncNestedStart(pPrbEnc, pFieldDesc);
            if (status != NV_OK)
                goto done;

            inMsg = NV_TRUE;
            status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_GPU_REGS);
            if (status != NV_OK)
                goto done;

            regAddr = *pOffset + base;
            status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, REG_GET_ADDR(pAperture, regAddr));
            if (status != NV_OK)
                goto done;
        }

        status = prbEncAddUInt32(pPrbEnc,REGS_REGSANDMEM_VAL, REG_RD32(pAperture, regAddr));
        if (status != NV_OK)
            goto done;

        pOffset++;
        regAddr += 4;

        if (regAddr != *pOffset + base)
        {
            inMsg = NV_FALSE;

            status = prbEncNestedEnd(pPrbEnc);
            if (status != NV_OK)
                goto done;
        }
    }

done:
    // Try to close even if preceding error
    prbEncUnwindNesting(pPrbEnc, uDepth);
    return status;
}

/*!
 * @brief Encode Gpu registers, table simplified version
 * @param [in] pGpu The Gpu
 * @param [in] pOffset The start of an array with register offsets
 * @param [in] numEntries  Number of entries in the table
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGpuRegOffset(OBJGPU *pGpu, IoAperture *pAperture, const NvU32 *pOffset, NvU32 numEntries,
                PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *pFieldDesc)
{
    return prbEncGpuRegSliceOffset(pGpu, pAperture, pOffset, numEntries, 0, pPrbEnc, pFieldDesc);
}

/*!
 * @brief Encode Gpu registers, table version
 * @param [in] pGpu The Gpu
 * @param [in] tbl The start of the table
 * @param [in] numEntries  Number of entries in the table
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGpuRegTbl(OBJGPU *pGpu, IoAperture *pAperture, const PRB_GPU_REG_TABLE *pTbl, NvU32 numEntries,
                PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    return prbEncGpuRegSliceTbl(pGpu, pAperture, pTbl, numEntries, 0, pPrbEnc, fieldDesc);
}


/*!
 * @brief Encode Instance Memory.
 *
 * @param [in] physAddr    The physical address of the memory, saved as offset.
 * @param [in] pVirtAddr   The virtual address of the memory to MEM_RD32.
 * @param [in] numWords    The number of NvU32 words to read.
 * @param [in] pPrbEnc     The protobuf encoder to use
 * @param [in] fieldDesc   The field we are encoding.
 *                         Must be a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncInstMem(NvU64 physAddr, NvU8 *pVirtAddr, NvU32 numWords,
              PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;
    NV_STATUS statusEnd;
    NvU32     data;

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_INSTANCE);
    if (status != NV_OK)
        goto done;

    status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, physAddr);
    if (status != NV_OK)
        goto done;

    while (numWords--)
    {
        data = MEM_RD32(pVirtAddr);
        status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);
        if (status != NV_OK)
            goto done;
        pVirtAddr += sizeof(NvU32);
    }

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Encode Instance Memory Immediate.
 * Assumes the caller has done the MEM_RD32 call.
 *
 * @param [in] physAddr    The physical address of the memory read
 * @param [in] data        The data at that memory
 * @param [in] pPrbEnc     The protobuf encoder to use
 * @param [in] fieldDesc   The field we are encoding.
 *                         Must be a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncInstMemImm(NvU64 physAddr, NvU32 data,
              PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;
    NV_STATUS statusEnd;

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_INSTANCE);
    if (status != NV_OK)
        goto done;

    status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, physAddr);
    if (status != NV_OK)
        goto done;

    status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);
    if (status != NV_OK)
        goto done;

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Encode Memory. Wraps MEM_RD32.
 * @param [in] offset The offset of the memory to read
 * @param [in] numRegs Number of contiguous 32 bit memory cells to encode
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncMem(NvU64 offset, NvU32 numRegs, PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;
    NV_STATUS statusEnd;
    NvU32     data;

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_SYS_MEM);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, offset);
    if (status != NV_OK)
        goto done;
    while (numRegs--)
    {
        data = MEM_RD32((NvU8 *)(NvUPtr)offset);
        status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);
        if (status != NV_OK)
            goto done;
        offset += sizeof(NvU32);
    }

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Encode pci config registers. Wraps GPU_BUS_CFG_RD32.
 * @param [in] pGpu The Gpu
 * @param [in] index Config Index
 * @param [in] numRegs Number of contiguous regs to encode
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncPciConfigRegs(OBJGPU *pGpu, NvU64 index, NvU32 numRegs,
                    PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;
    NV_STATUS statusEnd;
    NvU32     data;

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_PCI_CONFIG_REGS);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, index);
    if (status != NV_OK)
        goto done;
    while (numRegs--)
    {
        if (NV_OK != GPU_BUS_CFG_RD32(pGpu, (NvU32)index, &data))
            goto done;
        status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);
        if (status != NV_OK)
            goto done;
        index += sizeof(NvU32);
    }

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Encode pci space data. Wraps osPciReadDword.
 * @param [in] handle PCI space handle
 * @param [in] offset Offset
 * @param [in] pPrbEnc The protobuf encoder to use
 * @param [in] fieldDesc The field we are encoding. Must be
 *             a RegsAndMem message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncPciReadDword(void *handle, NvU64 offset,
                   PRB_ENCODER *pPrbEnc, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;
    NV_STATUS statusEnd;
    NvU32     data;

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddEnum(pPrbEnc, REGS_REGSANDMEM_TYPE, REGS_REGSANDMEM_PCI_SPACE);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt64(pPrbEnc, REGS_REGSANDMEM_OFFSET, offset);
    if (status != NV_OK)
        goto done;
    data = osPciReadDword(handle, (NvU32)offset);
    status = prbEncAddUInt32(pPrbEnc, REGS_REGSANDMEM_VAL, data);

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Add a generic exception data record to protobuf
 * @param [in] prbEnc The protobuf encoder
 * @param [in] pGpu The gpu
 * @param [in] chId The channel where the exception occurred
 * @param [in] fieldDesc The field we are encoding. Must be
 *             an ExceptionData message.
 * @returns NV_STATUS
*/

NV_STATUS
prbEncGenExData(PRB_ENCODER *pPrbEnc, OBJGPU *pGpu, NvU32 chId, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS    status;
    NV_STATUS    statusEnd;
    static NvU32 errorSequenceNumber;
    NvU32        sec;
    NvU32        uSec;
    NvU64        curTime;

    status = prbEncNestedStart(pPrbEnc, fieldDesc);
    if (status != NV_OK)
        return status;

    status = prbEncAddUInt32(pPrbEnc, GR_EXCEPTION_EXCEPTIONDATA_GPU_INSTANCE, pGpu->gpuInstance);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt32(pPrbEnc, GR_EXCEPTION_EXCEPTIONDATA_CH_ID, chId);
    if (status != NV_OK)
        goto done;
    status = prbEncAddUInt32(pPrbEnc, GR_EXCEPTION_EXCEPTIONDATA_ERROR_SEQUENCE_NUMBER, errorSequenceNumber++);
    if (status != NV_OK)
        goto done;
    osGetCurrentTime(&sec, &uSec);
    curTime = (NvU64)sec * 1000000 + (NvU64)uSec;
    status = prbEncAddUInt64(pPrbEnc, GR_EXCEPTION_EXCEPTIONDATA_TIME_STAMP, curTime);

done:
    // Try to close even if preceding error
    statusEnd = prbEncNestedEnd(pPrbEnc);
    if (status != NV_OK)
        return status;
    else
        return statusEnd;
}

/*!
 * @brief Wrap a message and queue it to the RC error list
 * @param [in] prbEnc The protobuf encoder
 * @param [in] pErrorHeader The error block to queue it to
 * @returns NV_STATUS
*/

NV_STATUS
prbWrapAndQueue
(
    PRB_ENCODER *pPrbEnc,
    RMERRORHEADER *pErrorHeader,
    RMCD_ERROR_BLOCK **ppErrorBlock
)
{
    NvU32             len;
    NvU8              *pBuff;
    RMCD_ERROR_BLOCK *pCurrErrorBlock;
    RMCD_ERROR_BLOCK *pOldErrorBlock = NULL;
    RMCD_ERROR_BLOCK *pNewErrorBlock = NULL;

    prbEncNestedEnd(pPrbEnc);
    len = prbEncFinish(pPrbEnc, (void **)&pBuff);

    if (pBuff)
    {
        pNewErrorBlock = portMemAllocNonPaged(sizeof(RMCD_ERROR_BLOCK));
        if (NULL != pNewErrorBlock)
        {
            pNewErrorBlock->pBlock = pBuff;
            pNewErrorBlock->blockSize = len;
            pNewErrorBlock->pNext = NULL;

            for (pCurrErrorBlock = pErrorHeader->pErrorBlock; pCurrErrorBlock != NULL;
                 pCurrErrorBlock = pCurrErrorBlock->pNext)
            {
                pOldErrorBlock = pCurrErrorBlock;
            }

            if (pOldErrorBlock)
            {
                pOldErrorBlock->pNext = pNewErrorBlock;
            }
            else
            {
                pErrorHeader->pErrorBlock = pNewErrorBlock;
            }
        }
        else
        {
            portMemFree(pNewErrorBlock);
            portMemFree(pBuff);
            pNewErrorBlock = NULL;
        }
    }
    if (ppErrorBlock != NULL)
    {
        *ppErrorBlock = pNewErrorBlock;
    }
    return NV_OK;
}

/*!
 * @brief Allocate and startup a DCL message
 * @param [in] prbEnc The protobuf encoder
 * @param [in] len The number of bytes to allocate
 * @param [in] fieldDesc The dcl field to set up
 * @returns NV_STATUS
*/

NV_STATUS
prbSetupDclMsg(PRB_ENCODER *pPrbEnc, NvU32 len, const PRB_FIELD_DESC *fieldDesc)
{
    NV_STATUS status;

    status = prbEncStartAlloc(pPrbEnc, DCL_DCLMSG, len, NULL);
    if (status == NV_OK)
    {
        status = prbEncNestedStart(pPrbEnc, fieldDesc);
        if (status != NV_OK)
        {
            prbFreeAllocatedBuffer(pPrbEnc);
        }

    }
    return status;
}

/*!
 * @brief Unwinds the nesting to the level specified.
 *
 * For best results, use @ref prbEncNestingLevel to obtain the current nesting
 * level before making calls to @ref prbEncNestedStart.
 *
 * @param [in] prbEnc The protobuf encoder
 * @param [in] pErrorHeader The error block to queue it to
 *
 * @returns NV_STATUS if successfully unwound; detailed error code otherwise.
*/

NV_STATUS
prbEncUnwindNesting(PRB_ENCODER *pPrbEnc, NvU8 level)
{
    NV_STATUS status = NV_OK;

    while (pPrbEnc->depth > level)
    {
        status = prbEncNestedEnd(pPrbEnc);
        if (NV_OK != status)
        {
            break;
        }
    }

    return status;
}

