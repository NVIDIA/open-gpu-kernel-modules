/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RM_STATE_MONITOR_H
#define RM_STATE_MONITOR_H

// run every 100ms to prevent excessive CPU usage
// as RM dump task runs as a higher priority task
// than RM main task
#define RM_STATE_MONITOR_WAIT_PERIOD (100000000) // 100ms
 
typedef enum {
    RM_STATE_MONITOR_HEADER_REQUEST_EXIT,
    RM_STATE_MONITOR_HEADER_REQUEST_RM_STATE,
    RM_STATE_MONITOR_HEADER_RESPONSE
} RM_STATE_MONITOR_HEADER;
 
typedef struct {
    RM_STATE_MONITOR_HEADER header;
} RM_STATE_MONITOR_REQUEST_MESSAGE; 
 
typedef struct {
    NV_STATUS status;
} RM_STATE_MONITOR_RESPONSE_MESSAGE;
 
typedef union {
    RM_STATE_MONITOR_REQUEST_MESSAGE request;
    RM_STATE_MONITOR_RESPONSE_MESSAGE response;
} RM_STATE_MONITOR_MESSAGE;
 
#define RM_STATE_MONITOR_MAX_DPC_QUEUE_ITEMS 16
#define RM_STATE_MONITOR_MAX_WORK_ITEMS 32
#define RM_STATE_MONITOR_MAX_INTERRUPTS 32
#define RM_STATE_MONITOR_MAX_ENINGE_BYTE_ARRAY (4)

typedef struct {
    NvU64 dpcQueue[RM_STATE_MONITOR_MAX_DPC_QUEUE_ITEMS][RM_STATE_MONITOR_MAX_ENINGE_BYTE_ARRAY];
    NvU64 workItemsQueue[RM_STATE_MONITOR_MAX_WORK_ITEMS];
    NvU64 lastDpcExecutionTime; 
    NvU64 totalDpcExecutionTime;
    NvU64 lastWorkItemsExecutionTime;
    NvU64 totalWorkItemsExecutionTime;
    NvU64 interrupts[RM_STATE_MONITOR_MAX_INTERRUPTS]; //TODO: should this get set? will affect perf
} RM_STATE_MONITOR_DATA;
 
typedef struct {
    NvU64 requestSequenceNumber;
    NvU64 responseSequenceNumber; 
    RM_STATE_MONITOR_DATA data;
} RM_STATE_MONITOR_BUFFER_LAYOUT;
 
#endif
