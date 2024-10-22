/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RESOURCE_LIST_REQUIRED_INCLUDES_H
#define RESOURCE_LIST_REQUIRED_INCLUDES_H 1

//
// This file must be included wherever resource_list.h is included. It provides
// declarations for types that resource_list.h may depend on while honoring
// RMCFG at the same time.
//
// We cannot include the required files right above the RS_ENTRY definitions in
// resource_list.h because resource_list.h may be included in places that don't
// allow some declarations (example typedefs in an enum).
//



//
// CORERM-3604
// A lot of declarations are in nvos.h
// These can be assumed to not require RMCFG. These should be eventually split
// into individual files and nvos.h should be deprecated.
//
#include "nvos.h"


#include "alloc/alloc_channel.h"

#include "class/clb0b5sw.h"

#include "alloc/alloc_access_counter_buffer.h"

#endif /* ifndef RESOURCE_LIST_REQUIRED_INCLUDES_H */
