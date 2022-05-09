/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file   flcndmem_nvswitch.c
 * @brief  FLCN Data-Memory Manager
 *
 * This module is intended to serve as the primary interface between all upper-
 * level Falcon-object layers and the HAL-layer. It provides APIs for accessing
 * the Falcon DMEM (read and write) as well as managing all allocations in the
 * RM-managed region of the Falcon DMEM.
 *
 * DMEM allocations are satisfied out of a carved-out portion of the Falcon
 * DMEM. The location of this region is determined when the Falcon image is
 * built and is communicated to the RM from the Falcon via the INIT message
 * that the Falcon sends upon initialization. Therefore, allocations cannot be
 * satisfied until this message arrives (occurs immediately after STATE_LOAD).
 */

/* ------------------------ Includes --------------------------------------- */
#include "flcn/flcn_nvswitch.h"
#include "common_nvswitch.h"

/* ------------------------ Static Function Prototypes --------------------- */

/* ------------------------ Globals ---------------------------------------- */
/* ------------------------ Public Functions  ------------------------------ */
/* ------------------------ Private Static Functions ----------------------- */
