/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_LINUX_H_
#define _NVLINK_LINUX_H_

#include <linux/init.h>        // for entry/exit macros
#include <linux/sched.h>       // for "struct task_struct"
#include <linux/kernel.h>      // for printk priority macros
#include <linux/fs.h>


#define NVLINK_DEVICE_NAME        "nvidia-nvlink"
#define NVLINK_NUM_MINOR_DEVICES  1

/*
 * @Brief : Debug Breakpoint implementation
 *
 * @Description :
 *
 * @returns                 void
 */
static inline void
dbg_breakpoint(void)
{
    /* OS specific breakpoint implemented for NVLink library */
    #if defined(DEBUG)
      #if defined(CONFIG_X86_REMOTE_DEBUG) || defined(CONFIG_KGDB) || defined(CONFIG_XMON)
        #if defined(NVCPU_X86) || defined(NVCPU_X86_64)
            __asm__ __volatile__ ("int $3");
        #elif defined(NVCPU_ARM)
            __asm__ __volatile__ (".word %c0" :: "i" (KGDB_COMPILED_BREAK));
        #elif defined(NVCPU_AARCH64)
            # warning "Need to implement dbg_breakpoint() for aarch64"
        #elif defined(NVCPU_PPC64LE)
            __asm__ __volatile__ ("trap");
        #endif /* NVCPU_X86 || NVCPU_X86_64 */
      #elif defined(CONFIG_KDB)
            KDB_ENTER();
      #endif /* CONFIG_X86_REMOTE_DEBUG || CONFIG_KGDB || CONFIG_XMON */
    #endif /* DEBUG */
}

#endif //_NVLINK_LINUX_H_
