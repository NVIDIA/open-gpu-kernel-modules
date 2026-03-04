/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERNEL_GRAPHICS_CONTEXT_BUFFERS_H
#define KERNEL_GRAPHICS_CONTEXT_BUFFERS_H

#include "utils/nv_enum.h"

/*
 * Global buffer types.  These are shared between contexts
 * each PF/VF context normally.   A GraphicsContext may have
 * a private allocation for security (VPR) or when
 * graphics preemption is enabled.
 *
 * Not all buffer types are supported on every GPU.
 */
#define GR_GLOBALCTX_BUFFER_DEF(x)                                                 \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_BUNDLE_CB,                    0x00000000) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_PAGEPOOL,                     0x00000001) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_ATTRIBUTE_CB,                 0x00000002) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_RTV_CB,                       0x00000003) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_GFXP_POOL,                    0x00000004) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_GFXP_CTRL_BLK,                0x00000005) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_FECS_EVENT,                   0x00000006) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_PRIV_ACCESS_MAP,              0x00000007) \
    NV_ENUM_ENTRY(x, GR_GLOBALCTX_BUFFER_UNRESTRICTED_PRIV_ACCESS_MAP, 0x00000008) \
    NV_ENUM_ENTRY(x, GR_GLOBAL_BUFFER_GLOBAL_PRIV_ACCESS_MAP,          0x00000009)

NV_ENUM_DEF(GR_GLOBALCTX_BUFFER, GR_GLOBALCTX_BUFFER_DEF)
#define GR_GLOBALCTX_BUFFER_COUNT               NV_ENUM_SIZE(GR_GLOBALCTX_BUFFER)


#define GR_CTX_BUFFER_DEF(x)                                 \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_MAIN,     0x00000000) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_ZCULL,    0x00000001) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_PM,       0x00000002) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_PREEMPT,  0x00000003) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_SPILL,    0x00000004) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_BETA_CB,  0x00000005) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_PAGEPOOL, 0x00000006) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_RTV_CB,   0x00000007) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_PATCH,    0x00000008) \
        NV_ENUM_ENTRY(x, GR_CTX_BUFFER_SETUP,    0x00000009)

NV_ENUM_DEF(GR_CTX_BUFFER, GR_CTX_BUFFER_DEF)

#endif // KERNEL_GRAPHICS_CONTEXT_BUFFERS_H
