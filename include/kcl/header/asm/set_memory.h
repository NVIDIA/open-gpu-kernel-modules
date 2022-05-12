/* SPDX-License-Identifier: MIT */
#ifndef _KCL_HEADER__ASM_SET_MEMORY_H_H_
#define _KCL_HEADER__ASM_SET_MEMORY_H_H_

#if defined(HAVE_ASM_SET_MEMORY_H)
#include_next <asm/set_memory.h>
#else
#include <asm/cacheflush.h>
#endif

#endif
