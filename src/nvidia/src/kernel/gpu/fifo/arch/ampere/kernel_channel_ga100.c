/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_channel.h"
#include "published/ampere/ga100/dev_ram.h"

/**
 * @brief Verify that the given userd physical address is of the correct size
 *
 * @param[in] pKernelChannel    KernelChannel pointer
 * @param[in] userdAddrLo       low USERD physical address bits
 * @param[in] userdAddrHi       high USERD physical address bits
 *
 * @returns NV_TRUE if the given userd physical address is of the correct size
            NV_FALSE otherwise
 */
NvBool
kchannelIsUserdAddrSizeValid_GA100
(
    KernelChannel *pKernelChannel,
    NvU32          userdAddrLo,
    NvU32          userdAddrHi
)
{
    return ((userdAddrLo & SF_MASK(NV_RAMRL_ENTRY_CHAN_USERD_PTR_LO)) == userdAddrLo) &&
           ((userdAddrHi & SF_MASK(NV_RAMRL_ENTRY_CHAN_USERD_PTR_HI_HW)) == userdAddrHi);
}
