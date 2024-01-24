/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "base.h"
#include "libraries/utils/nvprintf.h"

static bool g_libspdmAssertHit = false;

/*
 * Checking and clear libspdm assert status.
 * Any assert in libspdm we need to return error and stop processing.
 * Returns true if no assert detected, false if assert detected.
 */
bool
nvspdm_check_and_clear_libspdm_assert(void)
{
    bool bAssertHit = g_libspdmAssertHit;

    // Clear assert status before returning.
    g_libspdmAssertHit = false;

    // Function is successful on no assert, failing on assert.
    return !bAssertHit;
}

void
libspdm_debug_assert
(
    const char *file_name,
    size_t      line_number,
    const char *description
)
{
    NV_PRINTF (LEVEL_ERROR, "libspdm assertion hit!\n");
    g_libspdmAssertHit = NV_TRUE;
    DBG_BREAKPOINT();
}

void
libspdm_debug_print
(
    size_t      error_level,
    const char *format,
    ...
)
{
    return;
}
