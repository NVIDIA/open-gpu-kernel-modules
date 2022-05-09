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
#ifndef _NVACPITYPES_H_
#define _NVACPITYPES_H_

typedef enum _ACPI_DSM_FUNCTION
{
    ACPI_DSM_FUNCTION_NBSI  = 0,
    ACPI_DSM_FUNCTION_NVHG,
    ACPI_DSM_FUNCTION_MXM,
    ACPI_DSM_FUNCTION_NBCI,
    ACPI_DSM_FUNCTION_NVOP,
    ACPI_DSM_FUNCTION_PCFG,
    ACPI_DSM_FUNCTION_GPS_2X,
    ACPI_DSM_FUNCTION_JT,
    ACPI_DSM_FUNCTION_PEX,
    ACPI_DSM_FUNCTION_NVPCF_2X,
    ACPI_DSM_FUNCTION_GPS,
    ACPI_DSM_FUNCTION_NVPCF,
    // insert new DSM Functions here
    ACPI_DSM_FUNCTION_COUNT,
    ACPI_DSM_FUNCTION_CURRENT,  // pseudo option to select currently available GUID which supports the subfunction.
    ACPI_DSM_FUNCTION_INVALID = 0xFF
} ACPI_DSM_FUNCTION;

#endif // _NVACPITYPES_H_

