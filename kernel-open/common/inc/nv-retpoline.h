/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_RETPOLINE_H_
#define _NV_RETPOLINE_H_

#include "cpuopsys.h"

#if (NV_SPECTRE_V2 == 0)
#define NV_RETPOLINE_THUNK NV_SPEC_THUNK
#else
#define NV_RETPOLINE_THUNK NV_NOSPEC_THUNK
#endif

#if defined(NVCPU_X86_64)
#define NV_SPEC_THUNK(REG)                                        \
    __asm__(                                                      \
        ".weak __x86_indirect_thunk_" #REG ";"                    \
        ".type __x86_indirect_thunk_" #REG ", @function;"         \
        "__x86_indirect_thunk_" #REG ":"                          \
        "   .cfi_startproc;"                                      \
        "   jmp *%" #REG ";"                                      \
        "   .cfi_endproc;"                                        \
        ".size __x86_indirect_thunk_" #REG ", .-__x86_indirect_thunk_" #REG)

#define NV_NOSPEC_THUNK(REG)                                      \
    __asm__(                                                      \
        ".weak __x86_indirect_thunk_" #REG ";"                    \
        ".type __x86_indirect_thunk_" #REG ", @function;"         \
        "__x86_indirect_thunk_" #REG ":"                          \
        "   .cfi_startproc;"                                      \
        "   call .Lnv_no_fence_" #REG ";"                         \
        ".Lnv_fence_" #REG ":"                                    \
        "   pause;"                                               \
        "   lfence;"                                              \
        "   jmp .Lnv_fence_" #REG ";"                             \
        ".Lnv_no_fence_" #REG ":"                                 \
        "   mov %" #REG ", (%rsp);"                               \
        "   ret;"                                                 \
        "   .cfi_endproc;"                                        \
        ".size __x86_indirect_thunk_" #REG ", .-__x86_indirect_thunk_" #REG)

    __asm__(".pushsection .text");
    NV_RETPOLINE_THUNK(rax);
    NV_RETPOLINE_THUNK(rbx);
    NV_RETPOLINE_THUNK(rcx);
    NV_RETPOLINE_THUNK(rdx);
    NV_RETPOLINE_THUNK(rsi);
    NV_RETPOLINE_THUNK(rdi);
    NV_RETPOLINE_THUNK(rbp);
    NV_RETPOLINE_THUNK(r8);
    NV_RETPOLINE_THUNK(r9);
    NV_RETPOLINE_THUNK(r10);
    NV_RETPOLINE_THUNK(r11);
    NV_RETPOLINE_THUNK(r12);
    NV_RETPOLINE_THUNK(r13);
    NV_RETPOLINE_THUNK(r14);
    NV_RETPOLINE_THUNK(r15);
    __asm__(".popsection");
#endif

#endif /* _NV_RETPOLINE_H_ */
