/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/*
 * GSP MESSAGE QUEUE
 */

#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

typedef struct _message_queue_info MESSAGE_QUEUE_INFO;

// CPU-side calls
NV_STATUS GspMsgQueueInit(OBJGPU *pGpu, MESSAGE_QUEUE_INFO **ppMQI);
NV_STATUS GspStatusQueueInit(OBJGPU *pGpu, MESSAGE_QUEUE_INFO **ppMQI);
void GspMsgQueueCleanup(MESSAGE_QUEUE_INFO **ppMQI);
NV_STATUS GspMsgQueueSendCommand(MESSAGE_QUEUE_INFO *pMQI, OBJGPU *pGpu);
NV_STATUS GspMsgQueueReceiveStatus(MESSAGE_QUEUE_INFO *pMQI);

#endif // _MESSAGE_QUEUE_H_
