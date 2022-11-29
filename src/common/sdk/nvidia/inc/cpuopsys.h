/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*! \brief
 * Define compile time symbols for CPU type and operating system type.
 * This file should only contain preprocessor commands so that
 * there are no dependencies on other files.
 *
 * cpuopsys.h
 *
 * Copyright (c) 2001, Nvidia Corporation.  All rights reserved.
 */

/*!
 * Uniform names are defined for compile time options to distinguish
 * CPU types and Operating systems.
 * Distinctions between CPU and OpSys should be orthogonal.
 *
 * These uniform names have initially been defined by keying off the
 * makefile/build names defined for builds in the OpenGL group.
 * Getting the uniform names defined for other builds may require
 * different qualifications.
 *
 * The file is placed here to allow for the possibility of all driver
 * components using the same naming convention for conditional compilation.
 */

#ifndef CPUOPSYS_H
#define CPUOPSYS_H

/*****************************************************************************/
/* Define all OS/CPU-Chip related symbols */

/* ***** WINDOWS variations */
#if defined(_WIN32) || defined(_WIN16)
#   define NV_WINDOWS

#   if defined(_WIN32_WINNT)
#      define NV_WINDOWS_NT
#   elif defined(_WIN32_WCE)
#      define NV_WINDOWS_CE
#   else
#      define NV_WINDOWS_9X
#   endif
#endif  /* _WIN32 || defined(_WIN16) */

/* ***** Unix variations */
#if defined(__linux__) && !defined(NV_LINUX) && !defined(NV_VMWARE)
#   define NV_LINUX
#endif  /* defined(__linux__) */

#if defined(__VMWARE__) && !defined(NV_VMWARE)
#   define NV_VMWARE
#endif /* defined(__VMWARE__) */

/* SunOS + gcc */
#if defined(__sun__) && defined(__svr4__) && !defined(NV_SUNOS)
#   define NV_SUNOS
#endif /* defined(__sun__) && defined(__svr4__) */

/* SunOS + Sun Compiler (named SunPro, Studio or Forte) */
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#   define NV_SUNPRO_C
#   define NV_SUNOS
#endif /* defined(_SUNPRO_C) || defined(__SUNPRO_CC) */

#if defined(__FreeBSD__) && !defined(NV_BSD)
#   define NV_BSD
#endif /* defined(__FreeBSD__) */

/* XXXar don't define NV_UNIX on MacOSX or vxworks or QNX */
#if (defined(__unix__) || defined(__unix) || defined(__INTEGRITY) ) && !defined(nvmacosx) && !defined(vxworks) && !defined(NV_UNIX) && !defined(__QNX__) && !defined(__QNXNTO__)/* XXX until removed from Makefiles */
#   define NV_UNIX
#endif /* defined(__unix__) */

#if (defined(__QNX__) || defined(__QNXNTO__)) && !defined(NV_QNX)
#   define NV_QNX
#endif

#if (defined(__ANDROID__) || defined(ANDROID)) && !defined(NV_ANDROID)
#   define NV_ANDROID
#endif

#if defined(DceCore) && !defined(NV_DCECORE)
#    define NV_DCECORE
#endif

/* ***** Apple variations */
#if defined(macintosh) || defined(__APPLE__)
#   define NV_MACINTOSH
#   if defined(__MACH__)
#      define NV_MACINTOSH_OSX
#   else
#      define NV_MACINTOSH_OS9
#   endif
#   if defined(__LP64__)
#      define NV_MACINTOSH_64
#   endif
#endif  /* defined(macintosh) */

/* ***** VxWorks */
/* Tornado 2.21 is gcc 2.96 and #defines __vxworks. */
/* Tornado 2.02 is gcc 2.7.2 and doesn't define any OS symbol, so we rely on */
/* the build system #defining vxworks. */
#if defined(__vxworks) || defined(vxworks)
#   define NV_VXWORKS
#endif

/* ***** Integrity OS */
#if defined(__INTEGRITY)
#  if !defined(NV_INTEGRITY)
#    define NV_INTEGRITY
#  endif
#endif

/* ***** Processor type variations */
/* Note: The prefix NV_CPU_* is taken by Nvcm.h */

#if ((defined(_M_IX86) || defined(__i386__) || defined(__i386)) && !defined(NVCPU_X86)) /* XXX until removed from Makefiles */
/* _M_IX86 for windows, __i386__ for Linux (or any x86 using gcc) */
/* __i386 for Studio compiler on Solaris x86 */
#   define NVCPU_X86               /* any IA32 machine (not x86-64) */
#   define NVCPU_MIN_PAGE_SHIFT 12
#endif

#if defined(NV_LINUX) && defined(__ia64__)
#   define NVCPU_IA64_LINUX        /* any IA64 for Linux opsys */
#endif
#if defined(NVCPU_IA64_WINDOWS) || defined(NVCPU_IA64_LINUX) || defined(IA64)
#   define NVCPU_IA64              /* any IA64 for any opsys */
#endif

#if (defined(NV_MACINTOSH) && !(defined(__i386__) || defined(__x86_64__)))  || defined(__PPC__) || defined(__ppc)
#    if defined(__powerpc64__) && defined(__LITTLE_ENDIAN__)
#       ifndef NVCPU_PPC64LE
#           define NVCPU_PPC64LE           /* PPC 64-bit little endian */
#       endif
#    else
#       ifndef NVCPU_PPC
#           define NVCPU_PPC               /* any non-PPC64LE PowerPC architecture */
#       endif
#       ifndef NV_BIG_ENDIAN
#           define NV_BIG_ENDIAN
#       endif
#    endif
#    define NVCPU_FAMILY_PPC
#endif

#if defined(__x86_64) || defined(AMD64) || defined(_M_AMD64)
#    define NVCPU_X86_64           /* any x86-64 for any opsys */
#endif

#if defined(NVCPU_X86) || defined(NVCPU_X86_64)
#    define NVCPU_FAMILY_X86
#endif

#if defined(__riscv) && (__riscv_xlen==64)
#    define NVCPU_RISCV64
#    if defined(__nvriscv)
#       define NVCPU_NVRISCV64
#    endif
#endif

#if defined(__arm__) || defined(_M_ARM)
/*
 * 32-bit instruction set on, e.g., ARMv7 or AArch32 execution state
 * on ARMv8
 */
#   define NVCPU_ARM
#   define NVCPU_MIN_PAGE_SHIFT 12
#endif

#if defined(__aarch64__) || defined(__ARM64__) || defined(_M_ARM64)
#   define NVCPU_AARCH64           /* 64-bit A64 instruction set on ARMv8 */
#   define NVCPU_MIN_PAGE_SHIFT 12
#endif

#if defined(NVCPU_ARM) || defined(NVCPU_AARCH64)
#   define NVCPU_FAMILY_ARM
#endif

#if defined(__SH4__)
#   ifndef NVCPU_SH4
#   define NVCPU_SH4               /* Renesas (formerly Hitachi) SH4 */
#   endif
#   if   defined NV_WINDOWS_CE
#       define NVCPU_MIN_PAGE_SHIFT 12
#   endif
#endif

/* For Xtensa processors */
#if defined(__XTENSA__)
# define NVCPU_XTENSA
# if defined(__XTENSA_EB__)
#  define NV_BIG_ENDIAN
# endif
#endif


/*
 * Other flavors of CPU type should be determined at run-time.
 * For example, an x86 architecture with/without SSE.
 * If it can compile, then there's no need for a compile time option.
 * For some current GCC limitations, these may be fixed by using the Intel
 * compiler for certain files in a Linux build.
 */

/* The minimum page size can be determined from the minimum page shift */
#if defined(NVCPU_MIN_PAGE_SHIFT)
#define NVCPU_MIN_PAGE_SIZE (1 << NVCPU_MIN_PAGE_SHIFT)
#endif

#if defined(NVCPU_IA64) || defined(NVCPU_X86_64) || \
    defined(NV_MACINTOSH_64) || defined(NVCPU_AARCH64) || \
    defined(NVCPU_PPC64LE) || defined(NVCPU_RISCV64)
#   define NV_64_BITS          /* all architectures where pointers are 64 bits */
#else
/* we assume 32 bits. I don't see a need for NV_16_BITS. */
#endif

/* For verification-only features not intended to be included in normal drivers */
#if defined(ENABLE_VERIF_FEATURES)
#define NV_VERIF_FEATURES
#endif

/*
 * New, safer family of #define's -- these ones use 0 vs. 1 rather than
 * defined/!defined.  This is advantageous because if you make a typo,
 * say misspelled ENDIAN:
 *
 *   #if NVCPU_IS_BIG_ENDAIN
 *
 * ...some compilers can give you a warning telling you that you screwed up.
 * The compiler can also give you a warning if you forget to #include
 * "cpuopsys.h" in your code before the point where you try to use these
 * conditionals.
 *
 * Also, the names have been prefixed in more cases with "CPU" or "OS" for
 * increased clarity.  You can tell the names apart from the old ones because
 * they all use "_IS_" in the name.
 *
 * Finally, these can be used in "if" statements and not just in #if's.  For
 * example:
 *
 *   if (NVCPU_IS_BIG_ENDIAN) x = Swap32(x);
 *
 * Maybe some day in the far-off future these can replace the old #define's.
 */

#define NV_IS_MODS 0

#define NVOS_IS_WINDOWS 0
#if defined(NV_WINDOWS_CE)
#define NVOS_IS_WINDOWS_CE 1
#else
#define NVOS_IS_WINDOWS_CE 0
#endif
#if defined(NV_LINUX)
#define NVOS_IS_LINUX 1
#else
#define NVOS_IS_LINUX 0
#endif
#if defined(NV_UNIX)
#define NVOS_IS_UNIX 1
#else
#define NVOS_IS_UNIX 0
#endif
#if defined(NV_BSD)
#define NVOS_IS_FREEBSD 1
#else
#define NVOS_IS_FREEBSD 0
#endif
#if defined(NV_SUNOS)
#define NVOS_IS_SOLARIS 1
#else
#define NVOS_IS_SOLARIS 0
#endif
#define NVOS_IS_VMWARE 0
#if defined(NV_QNX)
#define NVOS_IS_QNX 1
#else
#define NVOS_IS_QNX 0
#endif
#if defined(NV_ANDROID)
#define NVOS_IS_ANDROID 1
#else
#define NVOS_IS_ANDROID 0
#endif
#if defined(NV_MACINTOSH)
#define NVOS_IS_MACINTOSH 1
#else
#define NVOS_IS_MACINTOSH 0
#endif
#if defined(NV_VXWORKS)
#define NVOS_IS_VXWORKS 1
#else
#define NVOS_IS_VXWORKS 0
#endif
#if defined(NV_LIBOS)
#define NVOS_IS_LIBOS 1
#else
#define NVOS_IS_LIBOS 0
#endif
#if defined(NV_INTEGRITY)
#define NVOS_IS_INTEGRITY 1
#else
#define NVOS_IS_INTEGRITY 0
#endif

#if defined(NVCPU_X86)
#define NVCPU_IS_X86 1
#else
#define NVCPU_IS_X86 0
#endif
#if defined(NVCPU_RISCV64)
#define NVCPU_IS_RISCV64 1
#else
#define NVCPU_IS_RISCV64 0
#endif
#if defined(NVCPU_NVRISCV64)
#define NVCPU_IS_NVRISCV64 1
#else
#define NVCPU_IS_NVRISCV64 0
#endif
#if defined(NVCPU_IA64)
#define NVCPU_IS_IA64 1
#else
#define NVCPU_IS_IA64 0
#endif
#if defined(NVCPU_X86_64)
#define NVCPU_IS_X86_64 1
#else
#define NVCPU_IS_X86_64 0
#endif
#if defined(NVCPU_FAMILY_X86)
#define NVCPU_IS_FAMILY_X86 1
#else
#define NVCPU_IS_FAMILY_X86 0
#endif
#if defined(NVCPU_PPC)
#define NVCPU_IS_PPC 1
#else
#define NVCPU_IS_PPC 0
#endif
#if defined(NVCPU_PPC64LE)
#define NVCPU_IS_PPC64LE 1
#else
#define NVCPU_IS_PPC64LE 0
#endif
#if defined(NVCPU_FAMILY_PPC)
#define NVCPU_IS_FAMILY_PPC 1
#else
#define NVCPU_IS_FAMILY_PPC 0
#endif
#if defined(NVCPU_ARM)
#define NVCPU_IS_ARM 1
#else
#define NVCPU_IS_ARM 0
#endif
#if defined(NVCPU_AARCH64)
#define NVCPU_IS_AARCH64 1
#else
#define NVCPU_IS_AARCH64 0
#endif
#if defined(NVCPU_FAMILY_ARM)
#define NVCPU_IS_FAMILY_ARM 1
#else
#define NVCPU_IS_FAMILY_ARM 0
#endif
#if defined(NVCPU_SH4)
#define NVCPU_IS_SH4 1
#else
#define NVCPU_IS_SH4 0
#endif
#if defined(NVCPU_XTENSA)
#define NVCPU_IS_XTENSA 1
#else
#define NVCPU_IS_XTENSA 0
#endif
#if defined(NV_BIG_ENDIAN)
#define NVCPU_IS_BIG_ENDIAN 1
#else
#define NVCPU_IS_BIG_ENDIAN 0
#endif
#if defined(NV_64_BITS)
#define NVCPU_IS_64_BITS 1
#else
#define NVCPU_IS_64_BITS 0
#endif
#if defined(NVCPU_FAMILY_ARM)
#define NVCPU_IS_PCIE_CACHE_COHERENT 0
#else
#define NVCPU_IS_PCIE_CACHE_COHERENT 1
#endif
#if defined(NV_DCECORE)
#define NVOS_IS_DCECORE 1
#else
#define NVOS_IS_DCECORE 0
#endif
/*****************************************************************************/

#endif /* CPUOPSYS_H */
