/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NVPORT_CORE_H_
#define _NVPORT_CORE_H_

/**
 * @defgroup NVPORT_CORE Core Functions
 *
 * @brief These are core NvPort functions present in all configurations.
 * @{
 */
/**
 * @brief Global initialization
 *
 * Must be called once and only once before any NvPort functions can be called
 *
 * If this function returns an error then calling any NvPort function will result
 * in undefined behavior.
 *
 *
 * @return NV_OK if successful, error otherwise
 */
NV_STATUS portInitialize(void);

/**
 * @brief Global shutdown
 *
 * Must be called once and only once when a driver is shutting down and no more
 * NvPort functions will be called.
 *
 */
void portShutdown(void);

/**
 * @brief Returns if NvPort is initialized
 *
 * This function can be called at any time.  It returns if @ref portInitialize
 * has been called (and @ref portShutdown has not).
 */
NvBool portIsInitialized(void);

/// @}

#endif // _NVPORT_CORE_H_
