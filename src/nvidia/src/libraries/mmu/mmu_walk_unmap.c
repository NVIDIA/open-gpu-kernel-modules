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
 * @file mmu_walk_unmap.c
 * @brief Describes the structures and interfaces used to map N level page tables
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
mmuWalkUnmap
(
    MMU_WALK       *pWalk,
    const NvU64     vaLo,
    const NvU64     vaHi
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != pWalk, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(vaLo <= vaHi, NV_ERR_INVALID_ARGUMENT);

    // Unmap starting from root if it exists.
    if (NULL != pWalk->root.pInstances)
    {

        status = mmuWalkProcessPdes(pWalk, &g_opParamsUnmap,
                                    &pWalk->root, pWalk->root.pInstances, vaLo, vaHi);

        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to unmap VA Range 0x%llx to 0x%llx. Status = 0x%08x\n",
                      vaLo, vaHi, status);
            NV_ASSERT(0);
        }

        // Release the root.
        mmuWalkRootRelease(pWalk);
    }

    return status;
}

static const MMU_FILL_TARGET g_fillTargetUnmap =
{
    MMU_ENTRY_STATE_INVALID,
    MMU_WALK_FILL_INVALID,
};

const MMU_WALK_OP_PARAMS g_opParamsUnmap =
{
    &g_fillTargetUnmap,
    mmuWalkFill,
    mmuWalkFillSelectSubLevel,
    NV_TRUE,  // bFill
    NV_FALSE, // bRelease
    NV_FALSE, // bIgnoreSubLevelConflicts
};

