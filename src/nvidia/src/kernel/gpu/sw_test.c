/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/sw_test.h"

NV_STATUS
swtestConstruct_IMPL
(
    SoftwareMethodTest           *pSwTest,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

void
swtestDestruct_IMPL
(
    SoftwareMethodTest           *pSwTest
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pSwTest, ChannelDescendant);

    chandesIsolateOnDestruct(pChannelDescendant);
}

static const METHOD Nv04SoftwareTestMethods[] =
{
    {mthdNoOperation,                   0x0100, 0x0103},
};

NV_STATUS swtestGetSwMethods_IMPL
(
    SoftwareMethodTest *pSwTest,
    const METHOD      **ppMethods,
    NvU32              *pNumMethods
)
{
    *ppMethods = Nv04SoftwareTestMethods;
    *pNumMethods = NV_ARRAY_ELEMENTS(Nv04SoftwareTestMethods);
    return NV_OK;
}

