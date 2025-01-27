/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/gpu.h"
#include "vgpu/rpc.h"

#include "ctrl/ctrl2080/ctrl2080mc.h"

/**
 * @brief Gets the static interrupt table for a VF via RPC
 *
 * @param pGpu
 * @param pIntr
 * @param pTable The interrupt table to fill in
 * @param pParams
 */
static NV_STATUS
_intrCopyVfStaticInterruptTable
(
    OBJGPU         *pGpu,
    Intr           *pIntr,
    InterruptTable *pTable,
    NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *pParams
)
{
    NvU32 i;

    // Fill in interrupt table with the matching NV2080 engine vectors
    for (i = 0; i < pParams->numEntries; i++)
    {
        INTR_TABLE_ENTRY entry = {0};
        //
        // Translate NV2080_INTR_TYPE back to local MC_ENGINE_IDX type
        // for this driver version
        //
        switch (pParams->entries[i].nv2080IntrType)
        {
            case NV2080_INTR_TYPE_NON_REPLAYABLE_FAULT:
                entry.mcEngine = MC_ENGINE_IDX_NON_REPLAYABLE_FAULT;
                break;
            case NV2080_INTR_TYPE_NON_REPLAYABLE_FAULT_ERROR:
                entry.mcEngine = MC_ENGINE_IDX_NON_REPLAYABLE_FAULT_ERROR;
                break;
            case NV2080_INTR_TYPE_INFO_FAULT:
                entry.mcEngine = MC_ENGINE_IDX_INFO_FAULT;
                break;
            case NV2080_INTR_TYPE_REPLAYABLE_FAULT:
                entry.mcEngine = MC_ENGINE_IDX_REPLAYABLE_FAULT;
                break;
            case NV2080_INTR_TYPE_REPLAYABLE_FAULT_ERROR:
                entry.mcEngine = MC_ENGINE_IDX_REPLAYABLE_FAULT_ERROR;
                break;
            case NV2080_INTR_TYPE_ACCESS_CNTR:
                entry.mcEngine = MC_ENGINE_IDX_ACCESS_CNTR;
                break;
            case NV2080_INTR_TYPE_TMR:
                entry.mcEngine = MC_ENGINE_IDX_TMR;
                break;
            case NV2080_INTR_TYPE_CPU_DOORBELL:
                entry.mcEngine = MC_ENGINE_IDX_CPU_DOORBELL;
                break;
            case NV2080_INTR_TYPE_GR0_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR0_FECS_LOG;
                break;
            case NV2080_INTR_TYPE_GR1_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR1_FECS_LOG;
                break;
            case NV2080_INTR_TYPE_GR2_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR2_FECS_LOG;
                break;
            case NV2080_INTR_TYPE_GR3_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR3_FECS_LOG;
                break;
            case NV2080_INTR_TYPE_GR4_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR4_FECS_LOG;
                break;
            case NV2080_INTR_TYPE_GR5_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR5_FECS_LOG;
                break;
            case NV2080_INTR_TYPE_GR6_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR6_FECS_LOG;
                break;
            case NV2080_INTR_TYPE_GR7_FECS_LOG:
                entry.mcEngine = MC_ENGINE_IDX_GR7_FECS_LOG;
                break;
            default:
                NV_PRINTF(LEVEL_ERROR,
                          "Unknown NV2080_INTR_TYPE 0x%x\n",
                          pParams->entries[i].nv2080IntrType);
                NV_ASSERT_FAILED("Unknown NV2080_INTR_TYPE");
                return NV_ERR_INVALID_STATE;
        }

        entry.pmcIntrMask        = pParams->entries[i].pmcIntrMask;
        entry.intrVector         = pParams->entries[i].intrVectorStall;
        entry.intrVectorNonStall = pParams->entries[i].intrVectorStall;

        NV_ASSERT_OR_RETURN(vectAppend(pTable, &entry) != NULL,
                            NV_ERR_NO_MEMORY);
    }
    return NV_OK;
}

/**
 * @brief Gets the dynamic interrupt table for a VF via RPC
 *
 * @param pGpu
 * @param pIntr
 * @param numEngines
 * @param pTable The interrupt table to fill in, offset to empty entries.
 * @param pParams
 */
static NV_STATUS
_intrCopyVfDynamicInterruptTable
(
    OBJGPU         *pGpu,
    Intr           *pIntr,
    NvU32           numEngines,
    InterruptTable *pTable,
    NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS *pParams
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32 i;

    // Fill in interrupt table with the matching NV2080 engine vectors
    for (i = 0; i < numEngines; i++)
    {
        INTR_TABLE_ENTRY entry = {0};
        NvU32 nv2080EngineType;
        RM_ENGINE_TYPE rmEngineType;
        NvU32 engineIdx;
        NvU32 row;

        NV_ASSERT_OK_OR_RETURN(
                kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                    ENGINE_INFO_TYPE_INVALID, i,
                    ENGINE_INFO_TYPE_RM_ENGINE_TYPE, (NvU32 *)&rmEngineType));
        NV_ASSERT_OK_OR_RETURN(
                kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo,
                    ENGINE_INFO_TYPE_INVALID, i,
                    ENGINE_INFO_TYPE_MC, &engineIdx));

        entry.mcEngine = engineIdx;

        // VF does not have any stalling interrupts
        entry.intrVector = NV_INTR_VECTOR_INVALID;

        nv2080EngineType = gpuGetNv2080EngineType(rmEngineType);

        // Find the right row for this engine
        for (row = 0; row < pParams->numEntries; row++)
        {
            if (pParams->entries[row].nv2080EngineType == nv2080EngineType)
            {
                break;
            }
        }
        if (row >= pParams->numEntries)
        {
            // No matching engine received
            return NV_ERR_GENERIC;
        }

        entry.intrVectorNonStall = pParams->entries[row].notificationIntrVector;

        // Engines' nonstall vectors correspond 1 to 1 with their old pmcIntrMask bit
        if ((!pIntr->getProperty(pIntr, PDB_PROP_INTR_HOST_DRIVEN_ENGINES_REMOVED_FROM_PMC)) &&
            (entry.intrVectorNonStall != NV_INTR_VECTOR_INVALID))
        {
            entry.pmcIntrMask = NVBIT(entry.intrVectorNonStall);
        }
        else
        {
            entry.pmcIntrMask = NV_PMC_INTR_INVALID_MASK;
        }

        NV_ASSERT_OR_RETURN(vectAppend(pTable, &entry) != NULL,
                            NV_ERR_NO_MEMORY);
    }

    return NV_OK;
}

NV_STATUS
intrInitInterruptTable_VF
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NvU32             numEngines  = kfifoGetNumEngines_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu));
    VGPU_STATIC_INFO *pVSI        = GPU_GET_STATIC_INFO(pGpu);
    NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS *pStaticParams = &pVSI->mcStaticIntrTable;

    NV_ASSERT_OR_RETURN(vectIsEmpty(&pIntr->intrTable), NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pStaticParams->numEntries <=
                            NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_MAX,
                        NV_ERR_OUT_OF_RANGE);

    NV_ASSERT_OR_RETURN(
        pVSI->mcEngineNotificationIntrVectors.numEntries <=
            NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_MAX_ENGINES,
        NV_ERR_OUT_OF_RANGE);

    NV_ASSERT_OK_OR_RETURN(
        vectReserve(&pIntr->intrTable,
                    pStaticParams->numEntries + pVSI->mcEngineNotificationIntrVectors.numEntries));
    NV_ASSERT_OK_OR_RETURN(_intrCopyVfStaticInterruptTable(pGpu, pIntr,
                                                           &pIntr->intrTable,
                                                           pStaticParams));
    NV_ASSERT_OK_OR_RETURN(_intrCopyVfDynamicInterruptTable(pGpu, pIntr,
                                                            numEngines,
                                                            &pIntr->intrTable,
                                                            &pVSI->mcEngineNotificationIntrVectors));
    vectTrim(&pIntr->intrTable, 0);

    // Bug 3823562 TODO: Fetch this from Host RM
    intrInitSubtreeMap_HAL(pGpu, pIntr);

    return NV_OK;
}
