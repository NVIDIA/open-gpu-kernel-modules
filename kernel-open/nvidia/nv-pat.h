/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _NV_PAT_H_
#define _NV_PAT_H_

#include "nv-linux.h"


#if defined(NV_ENABLE_PAT_SUPPORT)
extern int nv_init_pat_support(nvidia_stack_t *sp);
extern void nv_teardown_pat_support(void);
extern int nv_enable_pat_support(void);
extern void nv_disable_pat_support(void);
#else
static inline int nv_init_pat_support(nvidia_stack_t *sp)
{
    (void)sp;
    return 0;
}

static inline void nv_teardown_pat_support(void)
{
    return;
}

static inline int nv_enable_pat_support(void)
{
    return 1;
}

static inline void nv_disable_pat_support(void)
{
    return;
}
#endif

#endif /* _NV_PAT_H_ */
