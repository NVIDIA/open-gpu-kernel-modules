/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RMIFVIDENG_H
#define RMIFVIDENG_H

/*!
 * @file   rmifvideng.h
 * @brief  RM interface for video engine
 */

/*!
 * Video engine's frame-buffer interface block has several slots/indices which can
 * be bound to support DMA to various surfaces in memory. This is an
 * enumeration that gives name to each index based on type of memory-aperture
 * the index is used to access.
 *
 * Traditionally, video falcons have used the 6th index for ucode, and we will
 * continue to use that to allow legacy ucode to work seamlessly.
 *
 * Note: DO NOT CHANGE THE VALUE OF RM_VIDENG_DMAIDX_UCODE. That value is used by
 * both the legacy video ucode, which assumes that it will use index 6, and by
 * msdecos. Changing it will break legacy video ucode, unless it is updated to
 * reflect the new value.
 */
typedef enum _RM_VIDENG_DMAIDX_TYPE
{
    RM_VIDENG_DMAIDX_RSVD0         = 0,
    RM_VIDENG_DMAIDX_VIRT          = 1,
    RM_VIDENG_DMAIDX_PHYS_VID      = 2,
    RM_VIDENG_DMAIDX_PHYS_SYS_COH  = 3,
    RM_VIDENG_DMAIDX_PHYS_SYS_NCOH = 4,
    RM_VIDENG_DMAIDX_RSVD1         = 5,
    RM_VIDENG_DMAIDX_UCODE         = 6,
    RM_VIDENG_DMAIDX_END           = 7
} RM_VIDENG_DMAIDX_TYPE;
#endif // RMIFVIDENG_H
