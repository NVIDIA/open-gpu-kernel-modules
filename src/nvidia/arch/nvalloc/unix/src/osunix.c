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

#include <os/os.h>

void
osInitObjOS(OBJOS *pOS)
{
    pOS->setProperty(pOS, PDB_PROP_OS_ONDEMAND_VBLANK_CONTROL_ENABLE_DEFAULT, NV_TRUE);
    pOS->setProperty(pOS, PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE, NV_TRUE);
    pOS->setProperty(pOS, PDB_PROP_OS_LIMIT_GPU_RESET, NV_TRUE);
}
