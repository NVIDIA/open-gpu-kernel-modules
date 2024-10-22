/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015,2020,2022,2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file mmu_walk_sparse.c
 * @brief Describes the structures and interfaces used to sparsify N level page tables
 */

/* ------------------------ Includes --------------------------------------- */
#if defined(SRT_BUILD)
#include "shrdebug.h"
#endif
#include "mmu_walk_private.h"

/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Inline Functions ------------------------------- */
/* ------------------------ Public Functions  ------------------------------ */

NV_STATUS
mmuWalkSparsify
(
    MMU_WALK       *pWalk,
    const NvU64     vaLo,
    const NvU64     vaHi,
    const NvBool    bUseStagingBuffer
)
{
    NV_STATUS    status   = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != pWalk, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(vaLo <= vaHi, NV_ERR_INVALID_ARGUMENT);

    // Acquire the root. Call unconditionally to account for change of size
    status = mmuWalkRootAcquire(pWalk, vaLo, vaHi, NV_FALSE);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // Set field to indicate whether to use staging buffer.
    pWalk->bUseStagingBuffer = bUseStagingBuffer;

    // Sparsify
    status = mmuWalkProcessPdes(pWalk,
                                &g_opParamsSparsify,
                                &pWalk->root, pWalk->root.pInstances,
                                vaLo, vaHi);

    if (NV_OK != status)
    {
        NV_STATUS    unmapStatus;

        NV_PRINTF(LEVEL_ERROR,
                  "Failed to sparsify VA Range 0x%llx to 0x%llx. Status = 0x%08x\n",
                  vaLo, vaHi, status);
        NV_ASSERT(0);

        // Mapping failed, unwind by unmapping the VA range
        unmapStatus = mmuWalkUnmap(pWalk,
                                   vaLo,
                                   vaHi);
        if (NV_OK != unmapStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unmap failed with status = 0x%08x\n",
                      unmapStatus);
            NV_ASSERT(NV_OK == unmapStatus);
        }
    }

    // Reset staging buffer usage status
    pWalk->bUseStagingBuffer = NV_FALSE;

    return status;
}

NV_STATUS
mmuWalkUnsparsify
(
    MMU_WALK             *pWalk,
    const NvU64           vaLo,
    const NvU64           vaHi
)
{
    return NV_ERR_NOT_SUPPORTED;
}

static const MMU_FILL_TARGET g_fillTargetSparse =
{
    MMU_ENTRY_STATE_SPARSE,
    MMU_WALK_FILL_SPARSE,
};

const MMU_WALK_OP_PARAMS g_opParamsSparsify =
{
    &g_fillTargetSparse,
    mmuWalkFill,
    mmuWalkFillSelectSubLevel,
    NV_TRUE,  // bFill
    NV_FALSE, // bRelease
    NV_FALSE, // bIgnoreSubLevelConflicts
};

