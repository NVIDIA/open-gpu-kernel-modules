/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "platform/sli/sli.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_desc.h"

/*!
 *  Returns the max context size
 *
 *  @returns NvU64
 */
NvU64
memmgrGetMaxContextSize_GM200
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU64  size = 0;

    //
    // This function's original purpose was to estimate how much heap memory RM
    // needs to keep in reserve from Windows LDDM driver to pass WHQL MaxContexts
    // test.  This estimation is done after heap init before KMD allocates a
    // kernel-managed chunk.
    // UVM & PMA similarly require RM to estimate how much heap memory RM needs
    // to reserve for page tables, contexts, etc.  This estimation is used during
    // heap init to divide the FB into internal heap and external PMA managed
    // spaces.
    //

    if (RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL))
        {
            // KMD in WDDM mode
           // 640KB per context and WHQL_TEST_MAX_CONTEXTS(100) contexts
           size = 640 * 1024 * WHQL_TEST_MAX_CONTEXTS;
            // Additional 50MB in case of SLI
            if (IsSLIEnabled(pGpu))
            {
                size += (50 * 1024 * 1024);
            }
        }
        else
        {
            // KMD in TCC mode
            //
            // Reserve enough memory for a moderate number of page tables.
            size = 48 * 1024 * 1024;
        }
    }
    else if (RMCFG_FEATURE_PLATFORM_MODS)
    {
        // TODO: Remove the PMA check after enabling on all chips.
        if (memmgrIsPmaInitialized(pMemoryManager) &&
            !memmgrAreClientPageTablesPmaManaged(pMemoryManager))
        {
            // Reserve enough memory for a moderate context size.
            size = 32 * 1024 * 1024;
        }
        else
        {
             // Reserve 16M -- MODS doesn't need RM to reserve excessive memory
            size = 16 * 1024 * 1024;
        }
    }
    else
    {
        if (memmgrIsPmaEnabled(pMemoryManager) &&
            memmgrIsPmaSupportedOnPlatform(pMemoryManager))
        {
            //
            // We need to estimate the reserved memory needs before PMA is initialized
            // Reserve enough memory for a moderate number of page tables
            //
            size = 32 * 1024 * 1024;
        }
        else
        {
            // Non-specific platform -- non-specific reserved memory requirements
            size = 0;
        }
    }

    // Reserve enough memory for CeUtils 
    size += (7*1024*1024);

    return size;
}
