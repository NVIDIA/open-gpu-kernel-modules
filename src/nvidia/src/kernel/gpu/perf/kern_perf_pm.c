/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/* ------------------------ Includes --------------------------------------- */
#include "os/os.h"
#include "gpu/subdevice/subdevice.h"
#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/perf/kern_perf_pm.h"

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Functions ------------------------------- */
//
// kPerfPerfmonClientDeviceSet
//
// This routine attempts to acquire (or release) the PerfMon HW
// for all GPUs within the specified device. PM HW mgmt should
// really be a unicast operation but we have a BC interface
// (NV_CFG_RESERVE_PERFMON_HW) that we need to continue to support.
//
// The work here is broken up into two steps:
//
//  (1) loop over all GPUs and check PM HW status
//  (2) if (1) passes then acquire/release the PM HW in a second loop
//  (3) release acquired PM HW if (2) fails
//
NV_STATUS
kPerfPerfmonClientDeviceSet
(
    NvHandle  hClient,
    NvHandle  hDevice,
    NvBool    bReservation,
    NvBool    bClientHandlesGrGating,
    NvBool    bRmHandlesIdleSlow
)
{
    RsResourceRef      *pDeviceRef;
    Subdevice          *pSubdevice;
    OBJGPU             *pGpu;
    RM_API             *pRmApi;
    RmClient           *pClient;
    RsClient           *pRsClient;
    RS_ITERATOR         it;
    NV_STATUS           status = NV_OK;

    pClient = serverutilGetClientUnderLock(hClient);
    if (pClient == NULL)
    {
        return NV_ERR_INVALID_CLIENT;
    }
    pRsClient = staticCast(pClient, RsClient);

    status = clientGetResourceRefByType(pRsClient, hDevice, classId(Device), &pDeviceRef);
    if (status != NV_OK)
    {
        return NV_ERR_INVALID_OBJECT_PARENT;
    }

    //
    // Are we attempting to make a reservation or release one?
    //
    if (bReservation)
    {
        //
        // The Perfmon HW must be *available* on all GPUs for this
        // to work so check that first.
        //
        it = clientRefIter(pRsClient, pDeviceRef, classId(Subdevice), RS_ITERATE_CHILDREN, NV_TRUE);
        while (clientRefIterNext(pRsClient, &it))
        {
            pSubdevice = dynamicCast(it.pResourceRef->pResource, Subdevice);
            pGpu       = gpumgrGetGpuFromSubDeviceInst(pSubdevice->deviceInst, pSubdevice->subDeviceInst);
            pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

            NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS params = {0};
            params.bReservation  = bReservation;

            status = pRmApi->Control(pRmApi,
                                     hClient,
                                     it.pResourceRef->hResource,
                                     NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK,
                                     &params,
                                     sizeof(params));
            if (status != NV_OK)
            {
                goto kClientPerfPerfmonDeviceSet_exit;
            }
        }

        //
        // All available...now claim it using the subdevice interface.
        //
        NvHandle failedReservationHandle = 0;
        it = clientRefIter(pRsClient, pDeviceRef, classId(Subdevice), RS_ITERATE_CHILDREN, NV_TRUE);
        while (clientRefIterNext(pRsClient, &it))
        {
            pSubdevice = dynamicCast(it.pResourceRef->pResource, Subdevice);
            pGpu       = gpumgrGetGpuFromSubDeviceInst(pSubdevice->deviceInst, pSubdevice->subDeviceInst);
            pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

            NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS params = {0};
            params.bReservation           = bReservation;
            params.bClientHandlesGrGating = bClientHandlesGrGating;
            params.bRmHandlesIdleSlow     = bRmHandlesIdleSlow;

            status = pRmApi->Control(pRmApi,
                                     hClient,
                                     it.pResourceRef->hResource,
                                     NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET,
                                     &params,
                                     sizeof(params));
            //
            // If the reservation did not succeed we should release all acquired reservations.
            // This should never happen as the check shpould be always called before reservation.
            //
            if (status != NV_OK)
            {
                failedReservationHandle = it.pResourceRef->hResource;
                break;
            }
        }

        if (status != NV_OK)
        {
            it = clientRefIter(pRsClient, pDeviceRef, classId(Subdevice), RS_ITERATE_CHILDREN, NV_TRUE);
            while (clientRefIterNext(pRsClient, &it))
            {
                if (failedReservationHandle == it.pResourceRef->hResource)
                {
                    goto kClientPerfPerfmonDeviceSet_exit;
                }

                pSubdevice = dynamicCast(it.pResourceRef->pResource, Subdevice);
                pGpu       = gpumgrGetGpuFromSubDeviceInst(pSubdevice->deviceInst, pSubdevice->subDeviceInst);
                pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

                NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS params = {0};
                params.bReservation           = NV_FALSE;
                params.bClientHandlesGrGating = bClientHandlesGrGating;
                params.bRmHandlesIdleSlow     = bRmHandlesIdleSlow;
                //
                // Ignoring the status here intentionaly since no mater if this release failed,
                // we want to try and release perfmon HW for all the GPUs that acquired it.
                //
                NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status,
                                                   LEVEL_ERROR,
                                                   pRmApi->Control(pRmApi,
                                                                   hClient,
                                                                   it.pResourceRef->hResource,
                                                                   NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET,
                                                                   &params,
                                                                   sizeof(params)));
            }
        }
    }
    else
    {
        //
        // The Perfmon HW must be *reserved* on all GPUs for this to work
        // so check that first.
        //
        it = clientRefIter(pRsClient, pDeviceRef, classId(Subdevice), RS_ITERATE_CHILDREN, NV_TRUE);
        while (clientRefIterNext(pRsClient, &it))
        {
            pSubdevice = dynamicCast(it.pResourceRef->pResource, Subdevice);
            pGpu       = gpumgrGetGpuFromSubDeviceInst(pSubdevice->deviceInst, pSubdevice->subDeviceInst);
            pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

            NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK_PARAMS params = {0};
            params.bReservation  = bReservation;

            status = pRmApi->Control(pRmApi,
                                     hClient,
                                     it.pResourceRef->hResource,
                                     NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_CHECK,
                                     &params,
                                     sizeof(params));
            //
            // bailing here under the assumption that we acquired all or none
            //
            if (status != NV_OK)
            {
                goto kClientPerfPerfmonDeviceSet_exit;
            }
        }

        it = clientRefIter(pRsClient, pDeviceRef, classId(Subdevice), RS_ITERATE_CHILDREN, NV_TRUE);
        while (clientRefIterNext(pRsClient, &it))
        {
            pSubdevice = dynamicCast(it.pResourceRef->pResource, Subdevice);
            pGpu       = gpumgrGetGpuFromSubDeviceInst(pSubdevice->deviceInst, pSubdevice->subDeviceInst);
            pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

            NV2080_CTRL_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET_PARAMS params = {0};
            params.bReservation           = bReservation;
            params.bClientHandlesGrGating = bClientHandlesGrGating;
            params.bRmHandlesIdleSlow     = bRmHandlesIdleSlow;

            NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status,
                                               LEVEL_ERROR,
                                               pRmApi->Control(pRmApi,
                                                               hClient,
                                                               it.pResourceRef->hResource,
                                                               NV2080_CTRL_CMD_INTERNAL_PERF_PERFMON_CLIENT_RESERVATION_SET,
                                                               &params,
                                                               sizeof(params)));
        }
    }

kClientPerfPerfmonDeviceSet_exit:
    return status;
}

/*!
 * @brief Reserves HW Performance Monitoring capabilities for clients.
 *
 * This command reserves HW Performance Monitoring capabilities for exclusive
 * use by the requester.
 *
 * The function cannot use "ROUTE_TO_PHYSICAL" directly since the privilege
 * checks cannot be skipped on CPU RM.
 *
 * @param[in]      pSubdevice
 * @param[in,out]  pPerfmonParams
 *
 * @returns NV_OK if the HW Performance monitoring capabilities is sucessfully
 *          acquired by the client.
 */
NV_STATUS
subdeviceCtrlCmdPerfReservePerfmonHw_KERNEL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS *pPerfmonParams
)
{
    OBJGPU   *pGpu   = GPU_RES_GET_GPU(pSubdevice);
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;

    if (gpuIsRmProfilingPrivileged(pGpu) && !osIsAdministrator())
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // Redirect to Physical RM.
    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pSubdevice),
                             RES_GET_HANDLE(pSubdevice),
                             NV2080_CTRL_CMD_PERF_RESERVE_PERFMON_HW,
                             pPerfmonParams,
                             sizeof(*pPerfmonParams));

    return status;
}

/* ------------------------- Private Functions ------------------------------ */
