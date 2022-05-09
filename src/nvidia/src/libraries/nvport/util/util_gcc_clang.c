/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief Util functions implementations using gcc compiler intrinsics
 */

#include "nvport/nvport.h"

#if portUtilExGetStackTrace_SUPPORTED
NV_NOINLINE NvUPtr
portUtilExGetStackTrace
(
    NvU32 level
)
{
    switch (level)
    {
        case 0: return (__builtin_frame_address(0) != 0) ?
                        (NvUPtr)__builtin_return_address(0) : (NvUPtr)0;
        case 1: return (__builtin_frame_address(1) != 0) ?
                        (NvUPtr)__builtin_return_address(1) : (NvUPtr)0;
        case 2: return (__builtin_frame_address(2) != 0) ?
                        (NvUPtr)__builtin_return_address(2) : (NvUPtr)0;
        case 3: return (__builtin_frame_address(3) != 0) ?
                        (NvUPtr)__builtin_return_address(3) : (NvUPtr)0;
        case 4: return (__builtin_frame_address(4) != 0) ?
                        (NvUPtr)__builtin_return_address(4) : (NvUPtr)0;
        case 5: return (__builtin_frame_address(5) != 0) ?
                        (NvUPtr)__builtin_return_address(5) : (NvUPtr)0;
        case 6: return (__builtin_frame_address(6) != 0) ?
                        (NvUPtr)__builtin_return_address(6) : (NvUPtr)0;
        case 7: return (__builtin_frame_address(7) != 0) ?
                        (NvUPtr)__builtin_return_address(7) : (NvUPtr)0;
        case 8: return (__builtin_frame_address(8) != 0) ?
                        (NvUPtr)__builtin_return_address(8) : (NvUPtr)0;
        case 9: return (__builtin_frame_address(9) != 0) ?
                        (NvUPtr)__builtin_return_address(9) : (NvUPtr)0;
        case 10: return (__builtin_frame_address(10) != 0) ?
                        (NvUPtr)__builtin_return_address(10) : (NvUPtr)0;
        case 11: return (__builtin_frame_address(11) != 0) ?
                        (NvUPtr)__builtin_return_address(11) : (NvUPtr)0;
        case 12: return (__builtin_frame_address(12) != 0) ?
                        (NvUPtr)__builtin_return_address(12) : (NvUPtr)0;
        case 13: return (__builtin_frame_address(13) != 0) ?
                        (NvUPtr)__builtin_return_address(13) : (NvUPtr)0;
        case 14: return (__builtin_frame_address(14) != 0) ?
                        (NvUPtr)__builtin_return_address(14) : (NvUPtr)0;
        case 15: return (__builtin_frame_address(15) != 0) ?
                        (NvUPtr)__builtin_return_address(15) : (NvUPtr)0;
    }
    return 0;
}
#endif

NV_NOINLINE NvUPtr portUtilGetIPAddress()
{
    return portUtilGetReturnAddress();
}
