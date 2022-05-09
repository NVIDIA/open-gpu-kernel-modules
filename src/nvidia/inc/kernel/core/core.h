/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef __CORE_H__
#define __CORE_H__

#include "core/prelude.h"

/**
 * @brief Global RM initialization
 *
 * The single entrypoint into the RM for all platforms. This will initial cross
 * platform RM subsystems and call into OS specific init as needed.
 *
 * Must be called once and only once before any RM internal functions can be
 * called
 *
 * @return NV_OK if successful, error otherwise
 */
NV_STATUS coreInitializeRm(void);

/**
 * @brief Global RM shutdown
 *
 * Must be called once and only once when a driver is shutting down and no more
 * RM internal functions will be called.
 *
 */
void coreShutdownRm(void);

#endif /* __CORE_H__ */
