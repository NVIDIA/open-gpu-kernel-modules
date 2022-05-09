/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"

#include "flcn/flcnable_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "flcn/flcnqueue_nvswitch.h"

/*!
 * @file   flcnqueue_fb.c
 * @brief  Provides all functions specific to FB Queue (non-DMEM queues).
 *
 * Queues are the primary communication mechanism between the RM and various
 * falcon-based engines such as the PMU and Display Falcon.  The RM requests
 * actions by inserting a data packet (command) into a command queue. This
 * generates an interrupt to the falcon which allows it to wake-up and service
 * the request.  Upon completion of the command, the falcon can optionally
 * write an acknowledgment packet (message) into a separate queue designated
 * for RM-bound messages.  CMDs sent by an FB CMD queue must send a
 * response, as that is required to clear that CMD queue element's "in use bit"
 * and, free the DMEM allocation associated with it.
 *
 * For more information on FB Queue see:
  *     PMU FB Queue (RID-70296)
 * For general queue information, see the HDR of flcnqueue.c.
 * For information specific to DMEM queues, see the HDR of flcnqueue_dmem.c
 *
 * Each queue has distinct "head" and "tail" pointers. The "head" pointer is the
 * index of the queue Element where the next write operation will take place;
 * the "tail" marks the index of the queue Element for the next read.  When the
 * head and tail pointers are equal, the queue is empty.  When non-equal, data
 * exists in the queue that needs to be processed.  Queues are always allocated
 * in the Super Surface in FB.
 */

