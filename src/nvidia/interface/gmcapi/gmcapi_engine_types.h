/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_ENGINE_TYPES_H
#define GMCAPI_ENGINE_TYPES_H

/* NVGMC_ENGINE_ID_TYPE is only used to represent engine types and not individual
   engine IDs. */
enum NV_ABI_STABLE NVGMC_ENGINE_TYPE
{
      NVGMC_ENGINE_TYPE_NONE          = 0x00,
      NVGMC_ENGINE_TYPE_GR            = 0x01,
      NVGMC_ENGINE_TYPE_COPY          = 0x02,
      NVGMC_ENGINE_TYPE_NVDEC         = 0x03,
      NVGMC_ENGINE_TYPE_NVENC         = 0x04,
      NVGMC_ENGINE_TYPE_VP            = 0x05,
      NVGMC_ENGINE_TYPE_ME            = 0x06,
      NVGMC_ENGINE_TYPE_PPP           = 0x07,
      NVGMC_ENGINE_TYPE_MPEG          = 0x08,
      NVGMC_ENGINE_TYPE_SW            = 0x09,
      NVGMC_ENGINE_TYPE_TSEC          = 0x0A,
      NVGMC_ENGINE_TYPE_VIC           = 0x0B,
      NVGMC_ENGINE_TYPE_MP            = 0x0C,
      NVGMC_ENGINE_TYPE_SEC2          = 0x0D,
      NVGMC_ENGINE_TYPE_HOST          = 0x0E,
      NVGMC_ENGINE_TYPE_DPU           = 0x0F,
      NVGMC_ENGINE_TYPE_PMU           = 0x10,
      NVGMC_ENGINE_TYPE_FBFLCN        = 0x11,
      NVGMC_ENGINE_TYPE_NVJPEG        = 0x12,
      NVGMC_ENGINE_TYPE_OFA           = 0x13,
      NVGMC_ENGINE_TYPE_COUNT
};

/* NVGMC engine IDs are formed by combining the engine type with the engine index
   in the upper 16 bits of a 32-bit value.  This allows engine types and engine
   counts to vary independently while still allowing engine IDs of a given type to
   remain contiguous. */
#define NVGMC_ENGINE_ID_TYPE        15:0
#define NVGMC_ENGINE_ID_INDEX       31:16

#define NVGMC_ENGINE_ID_DEF(_type, _index) (REF_NUM(NVGMC_ENGINE_ID_TYPE, NVGMC_ENGINE_TYPE_ ## _type) | REF_NUM(NVGMC_ENGINE_ID_INDEX, _index))
#define NVGMC_ENGINE_ID_NUM(_type, _index) (REF_NUM(NVGMC_ENGINE_ID_TYPE, _type) | REF_NUM(NVGMC_ENGINE_ID_INDEX, _index))

#endif // GMCAPI_ENGINE_TYPES_H
