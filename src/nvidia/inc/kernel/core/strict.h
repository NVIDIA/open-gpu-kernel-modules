/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __STRICT_H__
#define __STRICT_H__

//
// RM_STRICT_SUPPRESS_DEPRECATED_DEFINITIONS_VER_XYZ should be set
// before including any RM internal headers when disabling deprecated
// definitions is desired.
//
// For transition during refactoring, we might introduce new types and
// interfaces and use macros/wrappers to forward the old interface to
// the new one.
//
// Once a callsite is migrated to use the new interface it can use RM
// strict to disable the deprecated definitions to prevent changes from
// reintroducing calls to a deprecated interface within a cleansed
// module.
//
// Controlling disablement of deprecated definitions is versioned. This
// enables us to introduce new deprecated interfaces incrementally.
// Example, ModuleA might scrub to versionX (removal of OBJFB defns) but
// not versionY (removal of legacy CLI types).
//
// Flags to turn off deprecated definitions are intended to be
// temporary, once all modules remove references the deprecated
// definitions and knobs in this header should be deleted.
//
#ifdef RM_STRICT_SUPPRESS_DEPRECATED_DEFINITIONS_VER_JAN_21_2020
#define RM_STRICT_CONFIG_EMIT_DEPRECATED_OBJFB_DEFINITIONS         0
#define RM_STRICT_CONFIG_EMIT_DEPRECATED_CONTEXT_DMA_DEFINITIONS   0
#endif

//
// RM_STRICT_SUPPRESS_PHYSICAL_DEFINITIONS_VER_XYZ should be set before
// including any RM internal headers when disabling "physical" definitions is
// desired.
//
// Physical definitions refers to interfaces/types that are only used by GSP-RM
// and VGPU-host, i.e.: not to be used by VGPU Client or GSP Client
//
#ifdef RM_STRICT_SUPPRESS_PHYSICAL_DEFINITIONS_VER_JAN_21_2020
#define RM_STRICT_CONFIG_EMIT_MEMORY_SYSTEM_DEFINITIONS    0
#endif

//
// Default deprecated and "physical engine" definitions on unless specified
//
#ifndef RM_STRICT_CONFIG_EMIT_DEPRECATED_OBJFB_DEFINITIONS
#define RM_STRICT_CONFIG_EMIT_DEPRECATED_OBJFB_DEFINITIONS   1
#endif

#ifndef RM_STRICT_CONFIG_EMIT_DEPRECATED_CONTEXT_DMA_DEFINITIONS
#define RM_STRICT_CONFIG_EMIT_DEPRECATED_CONTEXT_DMA_DEFINITIONS    1
#endif

#ifndef RM_STRICT_CONFIG_EMIT_MEMORY_SYSTEM_DEFINITIONS
#define RM_STRICT_CONFIG_EMIT_MEMORY_SYSTEM_DEFINITIONS     1
#endif

//
// "Physical engine" definitions not yet included in any version, but available
// for T234X. Should be defined to 0 before including any RM internal headers
// when disabling OBJDISP (and related) definitions is desired.
//
#ifndef RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS
#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     1
#endif

//
// Generate OBJGPU engine accessors (GPU_GET_FOO(pGpu)) for disabled engines.
// These will always return NULL, but will allow the code that references them
// to compile.
//
#ifndef RM_STRICT_CONFIG_EMIT_DISABLED_GPU_ENGINE_ACCESSORS
#define RM_STRICT_CONFIG_EMIT_DISABLED_GPU_ENGINE_ACCESSORS     1
#endif

#endif /* __STRICT_H__ */
