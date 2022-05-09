/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/***************************** HW State Routines ***************************\
*                                                                           *
*         Fills in os specific function pointers for the Unix OS object.    *
*                                                                           *
\***************************************************************************/

#include <osfuncs.h>
#include <os/os.h> 

static void initOSSpecificFunctionPointers(OBJOS *);
static void initMiscOSFunctionPointers(OBJOS *);
static void initUnixOSFunctionPointers(OBJOS *);
static void initOSSpecificProperties(OBJOS *);

void
osInitObjOS(OBJOS *pOS)
{
    initOSSpecificFunctionPointers(pOS);
    initOSSpecificProperties(pOS);
}

static void
initOSSpecificFunctionPointers(OBJOS *pOS)
{
    initMiscOSFunctionPointers(pOS);
    initUnixOSFunctionPointers(pOS);
}

static void
initMiscOSFunctionPointers(OBJOS *pOS)
{
    pOS->osQueueWorkItem           = osQueueWorkItem;
    pOS->osQueueWorkItemWithFlags  = osQueueWorkItemWithFlags;
    pOS->osQueueSystemWorkItem     = osQueueSystemWorkItem;
}

static void
initUnixOSFunctionPointers(OBJOS *pOS)
{
#if defined(NVCPU_X86_64)
    pOS->osNv_rdcr4                  = nv_rdcr4;
    pOS->osNv_cpuid                  = nv_cpuid;
#endif

    pOS->osCallACPI_DSM              = osCallACPI_DSM;
    pOS->osCallACPI_DDC              = osCallACPI_DDC;
    pOS->osCallACPI_NVHG_ROM         = osCallACPI_NVHG_ROM;
    pOS->osCallACPI_DOD              = osCallACPI_DOD;
    pOS->osCallACPI_MXDM             = osCallACPI_MXDM;
    pOS->osCallACPI_MXDS             = osCallACPI_MXDS;

    pOS->osDbgBreakpointEnabled      = osDbgBreakpointEnabled;
}

static void
initOSSpecificProperties
(
    OBJOS *pOS
)
{
    pOS->setProperty(pOS, PDB_PROP_OS_ONDEMAND_VBLANK_CONTROL_ENABLE_DEFAULT, NV_TRUE);
    pOS->setProperty(pOS, PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE, NV_TRUE);
    pOS->setProperty(pOS, PDB_PROP_OS_LIMIT_GPU_RESET, NV_TRUE);
}
