/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl0100.finn
//



/*
 * The lock stress object (LOCK_STRESS_OBJECT) is a test-only object that exports
 * multiple RM controls which exercise acquiring different combinations of RM locks in
 * order to stress and properly test RM locking specifically. These API's only exercise
 * legal combinations of RM locks that are in use elsewhere in RM but they are intended
 * to be exhaustive in covering all possible combinations of RM locks. The API's
 * themselves, don't do much outside of incrementing/decrementing global integers that
 * can be validated later on within a test executable to ensure that all accesses to
 * these global integers were synchronous.
 * 
 * The lock stress object is not allocatable without the "RmEnableLockStress" registry
 * key being turned on, which is off by default. The creation/destruction of the object
 * does not accomplish anything useful but simply serves as a way to check for the
 * registry key's enablement before allowing executing any of the test API's this object
 * provides.
 */

#define LOCK_STRESS_OBJECT 0x0100U

