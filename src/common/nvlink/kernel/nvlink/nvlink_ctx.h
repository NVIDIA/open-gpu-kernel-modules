/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_CTX_H_
#define _NVLINK_CTX_H_

//
// Link transition times in ms.
// TODO: Review with HW for optimal transition times;
//
#define LINK_TRANSITION_TIME_OFF        1
#define LINK_TRANSITION_TIME_SAFE       5
#define LINK_TRANSITION_TIME_HS         500
#define LINK_TRANSITION_TIMEOUT_IN_MS   2000

typedef struct 
{
    /*
     * Lock for all core lib structures except nvlink_link structures 
     */
    void *topLevelLock;

    /*
     * Head of the device-list
     */
    nvlink_device nv_devicelist_head;

    /*
     * Head of the established intranode nvlink connections list
     */
    nvlink_intranode_conn nv_intraconn_head;

    /*
     * Head of the added internode nvlink connections list
     */
    nvlink_internode_conn nv_interconn_head;

    /*
     * Topology information
     *    registeredEndpoints  : #Endpoints registered in the core library
     *    connectedEndpoints   : #Endpoints whose remote has been determined
     *    notConnectedEndpoints: #Endpoints whose remote has not been determined
     */
    NvU32  registeredEndpoints;
    NvU32  connectedEndpoints;
    NvU32  notConnectedEndpoints;
    NvBool bNewEndpoints;

    /*
     * Endpoint count in different link states
     *    endpointsInSafe  : #Endpoints in SAFE state
     *    endpointsInFail  : #Endpoints that failed to transition to ACTIVE
     *    endpointsInActive: #Endpoints in ACTIVE
     */
    NvU32 endpointsInSafe;
    NvU32 endpointsInFail;
    NvU32 endpointsInActive;

    /*
     * Fabric node id set by ioctl interface. This id will be assigned to each
     * nvlink device during registration and matched for endpoint look-up on 
     * ioctls, which operate on endpoints.
     */
    NvU16 nodeId;
}nvlink_lib_context;

extern nvlink_lib_context nvlinkLibCtx;

#endif //_NVLINK_CTX_H_

