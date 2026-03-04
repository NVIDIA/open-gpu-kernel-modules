/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _NVIDIA_3D_SHADER_CONSTANTS_H_
#define _NVIDIA_3D_SHADER_CONSTANTS_H_

#if defined(NV3D_BUILD_AS_GLSL)

  #define NV3D_CB_SLOT_FIRST_USER_BINDABLE 0

#else

  /* Shaders always use this slot for compiler-emitted constants.  This
   * assumption is verified at ucode build time. */
  #define NV3D_CB_SLOT_COMPILER 1

  /* Offset between GLSL slot 0 and hardware slot */
  #define NV3D_CB_SLOT_FIRST_USER_BINDABLE 3

#endif

/* This slot is used for most uniforms/constants defined in each shader */
#define NV3D_CB_SLOT_MISC1 (NV3D_CB_SLOT_FIRST_USER_BINDABLE + 0)

/* When needed (Kepler+), shaders always use this constant slot for bindless
 * texture handles. */
#define NV3D_CB_SLOT_BINDLESS_TEXTURE (NV3D_CB_SLOT_FIRST_USER_BINDABLE + 1)


/* Matches __GL_PGM_UNUSED_TEXTURE_UNIT */
#define NV3D_TEX_BINDING_UNUSED 255

#endif /* _NVIDIA_3D_SHADER_CONSTANTS_H_ */
