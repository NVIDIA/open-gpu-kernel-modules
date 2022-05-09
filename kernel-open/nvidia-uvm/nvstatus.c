/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvstatus.h"

#if !defined(NV_PRINTF_STRING_SECTION)
#if defined(NVRM) && NVCPU_IS_RISCV64
#define NV_PRINTF_STRING_SECTION         __attribute__ ((section (".logging")))
#else // defined(NVRM) && NVCPU_IS_RISCV64
#define NV_PRINTF_STRING_SECTION
#endif // defined(NVRM) && NVCPU_IS_RISCV64
#endif // !defined(NV_PRINTF_STRING_SECTION)

/*
 * Include nvstatuscodes.h twice.  Once for creating constant strings in the
 * the NV_PRINTF_STRING_SECTION section of the ececutable, and once to build
 * the g_StatusCodeList table.
 */
#undef NV_STATUS_CODE
#undef SDK_NVSTATUSCODES_H
#define NV_STATUS_CODE( name, code, string ) static NV_PRINTF_STRING_SECTION   \
    const char rm_pvt_##name##_str[] = string " [" #name "]";
#include "nvstatuscodes.h"

#undef NV_STATUS_CODE
#undef SDK_NVSTATUSCODES_H
#define NV_STATUS_CODE( name, code, string ) { name, rm_pvt_##name##_str },
static struct NvStatusCodeString
{
    NV_STATUS   statusCode;
    const char *statusString;
} g_StatusCodeList[] = {
   #include "nvstatuscodes.h"
   { 0xffffffff, "Unknown error code!" } // Some compilers don't like the trailing ','
};
#undef NV_STATUS_CODE

/*!
 * @brief Given an NV_STATUS code, returns the corresponding status string.
 *
 * @param[in]   nvStatusIn                  NV_STATUS code for which the string is required
 *
 * @returns     Corresponding status string from the nvstatuscodes.h
 *
 * TODO: Bug 200025711: convert this to an array-indexed lookup, instead of a linear search
 *
*/
const char *nvstatusToString(NV_STATUS nvStatusIn)
{
    static NV_PRINTF_STRING_SECTION const char rm_pvt_UNKNOWN_str[] = "Unknown error code!";
    NvU32 i;
    NvU32 n = ((NvU32)(sizeof(g_StatusCodeList))/(NvU32)(sizeof(g_StatusCodeList[0])));
    for (i = 0; i < n; i++)
    {
        if (g_StatusCodeList[i].statusCode == nvStatusIn)
        {
            return g_StatusCodeList[i].statusString;
        }
    }

    return rm_pvt_UNKNOWN_str;
}
