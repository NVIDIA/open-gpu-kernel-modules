/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "gpu/uvm/uvm.h"

#include "blackwell/gb100/dev_vm.h"

NvU32
uvmGetRegOffsetAccessCntrBufferPut_GB100
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 offsets[] = { NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER0_NOTIFY_BUFFER_PUT, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER1_NOTIFY_BUFFER_PUT };

    NV_ASSERT_OR_RETURN(accessCounterIndex < NV_ARRAY_ELEMENTS(offsets), 0);
    return offsets[accessCounterIndex];
}

NvU32
uvmGetRegOffsetAccessCntrBufferGet_GB100
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 offsets[] = { NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER0_NOTIFY_BUFFER_GET, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER1_NOTIFY_BUFFER_GET };

    NV_ASSERT_OR_RETURN(accessCounterIndex < NV_ARRAY_ELEMENTS(offsets), 0);
    return offsets[accessCounterIndex];
}

NvU32
uvmGetRegOffsetAccessCntrBufferHi_GB100
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 offsets[] = { NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER0_NOTIFY_BUFFER_HI, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER1_NOTIFY_BUFFER_HI };

    NV_ASSERT_OR_RETURN(accessCounterIndex < NV_ARRAY_ELEMENTS(offsets), 0);
    return offsets[accessCounterIndex];
}

NvU32
uvmGetRegOffsetAccessCntrBufferLo_GB100
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 offsets[] = { NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER0_NOTIFY_BUFFER_LO, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER1_NOTIFY_BUFFER_LO };

    NV_ASSERT_OR_RETURN(accessCounterIndex < NV_ARRAY_ELEMENTS(offsets), 0);
    return offsets[accessCounterIndex];
}

NvU32
uvmGetRegOffsetAccessCntrBufferConfig_GB100
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 offsets[] = { NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER0_CONFIG, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER1_CONFIG };

    NV_ASSERT_OR_RETURN(accessCounterIndex < NV_ARRAY_ELEMENTS(offsets), 0);
    return offsets[accessCounterIndex];
}

NvU32
uvmGetRegOffsetAccessCntrBufferInfo_GB100
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 offsets[] = { NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER0_NOTIFY_BUFFER_INFO, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER1_NOTIFY_BUFFER_INFO };

    NV_ASSERT_OR_RETURN(accessCounterIndex < NV_ARRAY_ELEMENTS(offsets), 0);
    return offsets[accessCounterIndex];
}

NvU32
uvmGetRegOffsetAccessCntrBufferSize_GB100
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 offsets[] = { NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER0_NOTIFY_BUFFER_SIZE, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER1_NOTIFY_BUFFER_SIZE };

    NV_ASSERT_OR_RETURN(accessCounterIndex < NV_ARRAY_ELEMENTS(offsets), 0);
    return offsets[accessCounterIndex];
}
