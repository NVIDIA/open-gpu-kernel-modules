/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

// Included libSPDM copyright, as file is NV-authored but uses libSPDM code.

#include "base.h"
#include "library/debuglib.h"
#include "hal/library/memlib.h"
#include "nvport/nvport.h"      // NvPort Header

/**
 * Fills a target buffer with a byte value, and returns the target buffer.
 *
 * This function fills length bytes of buffer with value, and returns buffer.
 *
 * If length is greater than (MAX_ADDRESS - buffer + 1), then LIBSPDM_ASSERT().
 *
 * @param  buffer    The memory to set.
 * @param  length    The number of bytes to set.
 * @param  value     The value with which to fill length bytes of buffer.
 *
 *
 **/
void libspdm_set_mem(void *buffer, size_t length, uint8_t value)
{
    portMemSet(buffer, value, length);
}


/**
 * Copies bytes from a source buffer to a destination buffer.
 *
 * This function copies "src_len" bytes from "src_buf" to "dst_buf".
 *
 * Asserts and returns a non-zero value if any of the following are true:
 *   1) "src_buf" or "dst_buf" are NULL.
 *   2) "src_len" or "dst_len" is greater than (SIZE_MAX >> 1).
 *   3) "src_len" is greater than "dst_len".
 *   4) "src_buf" and "dst_buf" overlap.
 *
 * If any of these cases fail, a non-zero value is returned. Additionally if
 * "dst_buf" points to a non-NULL value and "dst_len" is valid, then "dst_len"
 * bytes of "dst_buf" are zeroed.
 *
 * This function follows the C11 cppreference description of memcpy_s.
 * https://en.cppreference.com/w/c/string/byte/memcpy
 * The cppreferece description does NOT allow the source or destination
 * buffers to be NULL.
 *
 * This function differs from the Microsoft and Safeclib memcpy_s implementations
 * in that the Microsoft and Safeclib implementations allow for NULL source and
 * destinations pointers when the number of bytes to copy (src_len) is zero.
 *
 * In addition the Microsoft and Safeclib memcpy_s functions return different
 * negative values on error. For best support, clients should generally check
 * against zero for success or failure.
 *
 * @param    dst_buf   Destination buffer to copy to.
 * @param    dst_len   Maximum length in bytes of the destination buffer.
 * @param    src_buf   Source buffer to copy from.
 * @param    src_len   The number of bytes to copy from the source buffer.
 *
 **/
void libspdm_copy_mem(void *dst_buf, size_t dst_len,
                      const void *src_buf, size_t src_len)
{
    volatile void *pRet = NULL;

    /* Check for case where "dst" or "dst_len" may be invalid.
     * Do not zero "dst" in this case. */
    if (dst_buf == NULL || dst_len > (SIZE_MAX >> 1)) {
        LIBSPDM_ASSERT(0);
        return;
    }

    /* Gaurd against invalid source. Zero "dst" in this case. */
    if (src_buf == NULL) {
        LIBSPDM_ASSERT(0);
        return;
    }

    /* Guard against invalid lengths. Zero "dst" in these cases. */
    if (src_len > dst_len ||
        src_len > (SIZE_MAX >> 1)) {
        LIBSPDM_ASSERT(0);
        return;
    }

    pRet = portMemCopy(dst_buf, dst_len, src_buf, src_len);

    if (pRet == NULL)
    {
        LIBSPDM_ASSERT(0);
        return;
    }

    return;
}

/**
 * Fills a target buffer with zeros, and returns the target buffer.
 *
 * This function fills length bytes of buffer with zeros, and returns buffer.
 *
 * If length > 0 and buffer is NULL, then LIBSPDM_ASSERT().
 * If length is greater than (MAX_ADDRESS - buffer + 1), then LIBSPDM_ASSERT().
 *
 * @param  buffer      The pointer to the target buffer to fill with zeros.
 * @param  length      The number of bytes in buffer to fill with zeros.
 *
 **/
void libspdm_zero_mem(void *buffer, size_t length)
{
    portMemSet(buffer, 0, length);
}
