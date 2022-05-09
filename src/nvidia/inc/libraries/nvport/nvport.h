/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**
 * @file
 * @brief NvPort global definitions
 */

#ifndef _NVPORT_H_
#define _NVPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note nvport.h may be included through nvrm.h by projects which haven't yet
 * configured their makefiles appropriately. These files don't use any NvPort
 * features, so it's safe to define away this entire file instead of failing
 * the build. This will be removed once NvPort becomes ubiquitous enough.
 */
#if defined(PORT_IS_KERNEL_BUILD)

#include <nvtypes.h>
#include <nvstatus.h>

#if !defined(PORT_IS_KERNEL_BUILD)
#error "PORT_IS_KERNEL_BUILD must be defined to 0 or 1 by makefile"
#endif

#if !defined(PORT_IS_CHECKED_BUILD)
#error "PORT_IS_CHECKED_BUILD must be defined to 0 or 1 by makefile"
#endif

/**
 * @defgroup NVPORT_CORE Core Functions
 * @{
 */

/**
 * @brief Helper macro to test if an extended function is supported
 *
 * Whether an extended function is supported or not is a compile time decision.
 * Every function has an associated define  that will look like this:
 *
 * ~~~{.c}
 * #define portSomeFunction_SUPPORTED SOME_EXPRESSION
 * ~~~
 *
 * That will be evaluated by the preprocessor to either 0 or 1 (not supported
 * or supported).  If it evaluates to 0 then the symbol will not exist and the
 * function cannot be referenced.
 */
#define PORT_IS_FUNC_SUPPORTED(function) function ## _SUPPORTED

/**
 * @brief Helper macro to test if a module is supported. The argument should be
 * a lowercase module name, e.g. @c PORT_IS_MODULE_SUPPORTED(memory)
 *
 * Whether a module is included in the build is decided at compile time.
 * Modules can either not support a given platform or be explicitly disabled
 * through the Makefile.
 *
 * This define will be equal to 1 if the module is supported.
 * If it evaluates to 0 or is not defined, then none of the module's symbols or
 * defines will exist in the build.
 */
#define PORT_IS_MODULE_SUPPORTED(module) PORT_MODULE_ ## module


#if defined(__clang__)
#define PORT_COMPILER_IS_CLANG 1
#define PORT_COMPILER_HAS_INTRINSIC_ATOMICS __has_builtin(__c11_atomic_fetch_add)
#define PORT_COMPILER_HAS_ATTRIBUTE_FORMAT __has_attribute(__format__)
#define PORT_COMPILER_HAS_COUNTER          1
#else
#define PORT_COMPILER_IS_CLANG 0
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define PORT_COMPILER_IS_GCC   1
#define PORT_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define PORT_COMPILER_HAS_INTRINSIC_ATOMICS (PORT_GCC_VERSION >= 40100)
#define PORT_COMPILER_HAS_ATTRIBUTE_FORMAT  (PORT_GCC_VERSION >= 20300)
#define PORT_COMPILER_HAS_COUNTER           (PORT_GCC_VERSION >= 40300)
#define PORT_COMPILER_HAS_INTRINSIC_CPUID   1
#else
#define PORT_COMPILER_IS_GCC   0
#endif

#define PORT_COMPILER_IS_MSVC  0

#if !(PORT_COMPILER_IS_GCC || PORT_COMPILER_IS_CLANG || PORT_COMPILER_IS_MSVC)
#error "Unsupported compiler"
#endif

//
// Need to define an IS_MODS macro that expands to 1 or 0 as defined(NV_MODS)
// is not entirely portable when used within a macro expansion.
//    e.g. this would not always work: #define PORT_IS_MODS defined(NV_MODS)
//
#define PORT_IS_MODS 0

#ifndef PORT_INLINE
/**
 * @brief Qualifier for all inline functions declared by NvPort.
 * Modules will usually define PORT_<MODULE>_INLINE which is either PORT_INLINE
 * or nothing, depending whether the functions are being inlined in that module.
 */
#define PORT_INLINE static NV_INLINE
#endif


/**
 * @def PORT_CHECKED_ONLY(x)
 * @brief Evaluates the argument only if it is a checked build
 */
#if PORT_IS_CHECKED_BUILD
#define PORT_CHECKED_ONLY(x) x
#else
#define PORT_CHECKED_ONLY(x)
#endif

/**
 * @def PORT_KERNEL_ONLY(x)
 * @brief Evaluates the argument only if it is a kernel build
 */
#if PORT_IS_KERNEL_BUILD
#define PORT_KERNEL_ONLY(x) x
#else
#define PORT_KERNEL_ONLY(x)
#endif

#ifndef PORT_INCLUDE_NEW_STYLE_ALIASES
/**
 * @brief Switch to include aliases for objects and methods that conform to the
 * new RM style.
 *
 * This switch will define type and method aliases for object types in NvPort.
 * The current NvPort style object names are PORT_MODULE_OBJECT, while the
 * methods are portModuleObjectMethod(). 
 * The update proposal dictates these to be PortModuleObject and objectMethod.
 *
 * @todo Currently we just alias the new names to the old ones. Once the coding
 * style has been finalized, we should add a deprecation note to the old names,
 * and do a mass search and replace.
 */
#define PORT_INCLUDE_NEW_STYLE_ALIASES 1
#endif // PORT_INCLUDE_NEW_STYLE_ALIASES

/**
 * @brief Suppresses unused variable warnings
 * @param x - Variable or argument name
 *
 * No compilation errors are reported by any compiler when we use
 * the following definition.
 * 
 * #define PORT_UNREFERENCED_VARIABLE(x)  ((void)sizeof(&(x)))
 *
 * But Coverity reports BAD_SIZEOF error with this definition.
 * Adding a Coverity annotation "coverity[bad_sizeof]" near 
 * the definition does not work. The preprocessor ignores all
 * the comments and the Coverity annotation is also ignored
 * as a legal comment. As a result, this annotation never ends
 * up in the source code where this macro is used. Hence, we use
 * two definitions of this macro - one for Coverity and the other
 * for the rest of the targets. 
 * 
 * Coverity does not report any warnings for unused variables.
 * Hence, we do nothing while building for Coverity.
 */
#if !defined(__COVERITY__)
#define PORT_UNREFERENCED_VARIABLE(x)  ((void)sizeof(&(x)))
#else
#define PORT_UNREFERENCED_VARIABLE(x)
#endif

/// @}

#if PORT_IS_MODULE_SUPPORTED(core)
#include "nvport/core.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(example)
#include "nvport/example.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(atomic)
#include "nvport/atomic.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(debug)
#include "nvport/debug.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(util)
#include "nvport/util.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(memory)
#include "nvport/memory.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(sync)
#include "nvport/sync.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(safe)
#include "nvport/safe.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(mmio)
#include "nvport/mmio.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(thread)
#include "nvport/thread.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(time)
#include "nvport/time.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(crypto)
#include "nvport/crypto.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(string)
#include "nvport/string.h"
#endif

#if PORT_IS_MODULE_SUPPORTED(cpu)
#include "nvport/cpu.h"
#endif

#endif // defined(PORT_IS_KERNEL_BUILD)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _NVPORT_H_
