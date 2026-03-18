/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _NV_STDDEF_H_
#define _NV_STDDEF_H_

#if defined(NV_KERNEL_INTERFACE_LAYER) && defined(__FreeBSD__)
  #include <sys/stddef.h>   // NULL
#elif defined(NV_KERNEL_INTERFACE_LAYER) && defined(NV_LINUX)
  #include <linux/stddef.h> // NULL
  #include <linux/types.h>  // size_t
  #include <linux/limits.h> // SIZE_MAX,...
#else
  #include <stddef.h>       // NULL
#endif

#ifndef __DECLARE_FLEX_ARRAY
  #ifdef __cplusplus
    #define __DECLARE_FLEX_ARRAY(T, field) T field[0]
  #else
    #define __DECLARE_FLEX_ARRAY(T, field) \
      struct { struct { } __empty_ ## field; T field[]; }
  #endif
#endif

#endif // _NV_STDDEF_H_
