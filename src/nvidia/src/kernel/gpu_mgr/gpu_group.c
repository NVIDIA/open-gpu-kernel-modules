/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Rotuines ***************************\
*                                                                           *
*         GpuGrp Object Function Definitions.                               *
*                                                                           *
\***************************************************************************/

#include "gpu_mgr/gpu_group.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "core/system.h"
#include "class/cl2080.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "mem_mgr/vaspace.h"
#include "class/cl90f1.h"  // FERMI_VASPACE_A
#include "nvlimits.h"

/*!
 * Creates the gpugrp object.
 *
 * @param[in] pGpuGrp  gpugrp object pointer
 * @param[in] gpuMask  Mask of GPUs corresponding to this gpugrp
 *
 * @returns NV_OK on success, appropriate error on failure.
 */
NV_STATUS
gpugrpCreate_IMPL
(
    OBJGPUGRP *pGpuGrp,
    NvU32      gpuMask
)
{
    pGpuGrp->gpuMask = gpuMask;
    //
    // Add the gpugrp instance to the GPU objects in the mask
    // At boot this call fails and is deferred to GPU post construct.
    // When coming out of SLI this call is succeeding - and postconstruct
    // is not called.
    //
    gpumgrAddDeviceInstanceToGpus(gpuMask);
    return NV_OK;
}

/*!
 * Destroys gpugrp object.
 *
 * It first iterates over the GPUs that belong to this gpugrp
 * object indicated by the gpuMask.
 * Following this it destroys the object.
 *
 * @param[in]  pGpuGrp    gpugrp object pointer
 */
NV_STATUS
gpugrpDestroy_IMPL
(
    OBJGPUGRP *pGpuGrp
)
{
    NV_STATUS rmStatus = NV_ERR_OBJECT_NOT_FOUND;
    OBJGPU   *pGpu = NULL;
    NvU32     gpuIndex = 0;

    // Add the gpugrp instance to the GPU objects in the mask
    while ((pGpu = gpumgrGetNextGpu(pGpuGrp->gpuMask, &gpuIndex)))
    {
        rmStatus = NV_OK;
        pGpu->deviceInstance = NV_MAX_DEVICES;
    }

    // Call the utility routine that does the object deletion.
    objDelete(pGpuGrp);
    return rmStatus;
}

/*!
 * Gets the gpu mask for the gpugrp.
 *
 * @param[in] pGpuGrp  gpugrp object pointer
 *
 * @returns NvU32 gpumask
 */
NvU32
gpugrpGetGpuMask_IMPL(OBJGPUGRP *pGpuGrp)
{
    return pGpuGrp->gpuMask;
}

/*!
 * Sets the gpu mask for the gpugrp.
 *
 * @param[in] pGpuGrp  gpugrp object pointer
 * @param[in] gpuMask  gpumask to set
 *
 */
void
gpugrpSetGpuMask_IMPL(OBJGPUGRP *pGpuGrp, NvU32 gpuMask)
{
    pGpuGrp->gpuMask = gpuMask;
}
/*!
 * Gets the broadcast enabled state
 *
 * @param[in] pGpuGrp  gpugrp object pointer
 *
 * @returns NvBool
 */
NvBool
gpugrpGetBcEnabledState_IMPL(OBJGPUGRP *pGpuGrp)
{
    return pGpuGrp->bcEnabled;
}

/*!
 * Sets the broadcast enable state
 *
 * @param[in] pGpuGrp  gpugrp object pointer
 * @param[in] bcState  Broadcast enable state
 *
 */
void
gpugrpSetBcEnabledState_IMPL(OBJGPUGRP *pGpuGrp, NvBool bcState)
{
    pGpuGrp->bcEnabled = bcState;
}

/*!
 * Sets the parent GPU for the gpugrp
 *
 * @param[in]  pGpuGrp  gpugrp object pointer
 * @param[in]  pGpu     Parent GPU object pointer
 *
 */
void
gpugrpSetParentGpu_IMPL
(
    OBJGPUGRP *pGpuGrp,
    OBJGPU    *pParentGpu
)
{
    pGpuGrp->parentGpu = pParentGpu;
}

/*!
 * Gets the parent GPU for the gpugrp
 *
 * @param[in]  pGpuGrp   gpugrp object pointer
 *
 * @returns GPU pointer
 */
POBJGPU
gpugrpGetParentGpu_IMPL(OBJGPUGRP *pGpuGrp)
{
    return pGpuGrp->parentGpu;
}


/*!
 * @brief gpugrpCreateVASpace - creates the GLobal VASpace for this gpugrp.
 *
 * This is created once per group. So for GPUs in SLI, there is only
 * one of this created.
 *
 * @param[in]   pGpuGrp         GPUGRP object pointer
 * @param[in]   pGpu            Parent GPU object pointer
 * @param[in]   vaspaceClass    VASPACE class to create
 * @param[in]   vaStart         vaspace start
 * @param[in]   vaLimit         vaspace limit
 * @param[in]   vaspaceFlags    VASPACE flags for creation
 * @param[out]  ppGlobalVASpace Global vaspace that is created
 *
 * @return NV_OK on success or appropriate RM_ERR on failure
 *
 */
NV_STATUS
gpugrpCreateGlobalVASpace_IMPL
(
    OBJGPUGRP   *pGpuGrp,
    OBJGPU      *pGpu,
    NvU32        vaspaceClass,
    NvU64        vaStart,
    NvU64        vaLimit,
    NvU32        vaspaceFlags,
    OBJVASPACE **ppGlobalVASpace
)
{
    NV_STATUS rmStatus;
    NvU32      gpuMask              = pGpuGrp->gpuMask;
    OBJSYS    *pSys                 = SYS_GET_INSTANCE();
    OBJVMM    *pVmm                 = SYS_GET_VMM(pSys);
    NvBool     bcState              = gpumgrGetBcEnabledStatus(pGpu);

    NV_ASSERT_OR_RETURN(ppGlobalVASpace != NULL, NV_ERR_INVALID_ARGUMENT);
    *ppGlobalVASpace = NULL;

    gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);
    vaspaceFlags |= VASPACE_FLAGS_ENABLE_VMM;
    rmStatus = vmmCreateVaspace(pVmm, vaspaceClass, 0x0, gpuMask, vaStart,
                                      vaLimit, 0, 0, NULL, vaspaceFlags, ppGlobalVASpace);
    gpumgrSetBcEnabledStatus(pGpu, bcState);
    if (NV_OK != rmStatus)
    {
        *ppGlobalVASpace = NULL;
        return rmStatus;
    }
    pGpuGrp->pGlobalVASpace = (*ppGlobalVASpace);

    return rmStatus;
}

/*!
 * @brief gpugrpDestroyVASpace - Destroys the gpugrp global vaspace
 *
 * @param[in]   pGpuGrp  GPUGRP object pointer
 * @param[in]   pGpu     Parent GPU object pointer
 *
 * @return NV_OK on success or appropriate RM_ERR on failure
 *
 */
NV_STATUS
gpugrpDestroyGlobalVASpace_IMPL(OBJGPUGRP *pGpuGrp, OBJGPU *pGpu)
{
    NV_STATUS  rmStatus = NV_OK;
    OBJSYS    *pSys     = SYS_GET_INSTANCE();
    OBJVMM    *pVmm     = SYS_GET_VMM(pSys);
    NvBool     bcState  = gpumgrGetBcEnabledStatus(pGpu);

    // Nothing to destroy, bail out early
    if (pGpuGrp->pGlobalVASpace == NULL)
        return rmStatus;

    gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);
    vmmDestroyVaspace(pVmm, pGpuGrp->pGlobalVASpace);
    gpumgrSetBcEnabledStatus(pGpu, bcState);
    pGpuGrp->pGlobalVASpace = NULL;
    return rmStatus;
}


/*!
 * @brief gpugrpGetVASpace - retrieves the group global vaspace
 *
 * @param[in]   pGpuGrp         GPUGRP object pointer
 * @param[out]  ppGlobalVASpace Global vaspace for this GPUGRP
 *
 * @return NV_OK on success
 *         NV_ERR_INVALID_ARGUMENT on NULL pointer parameter
 *         NV_ERR_OBJECT_NOT_FOUND if there is no device vaspace
 */
NV_STATUS
gpugrpGetGlobalVASpace_IMPL(OBJGPUGRP *pGpuGrp, OBJVASPACE **ppVASpace)
{
    NV_ASSERT_OR_RETURN(ppVASpace != NULL, NV_ERR_INVALID_ARGUMENT);

    if (pGpuGrp->pGlobalVASpace == NULL)
    {
        *ppVASpace = NULL;
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    *ppVASpace = pGpuGrp->pGlobalVASpace;
    return NV_OK;
}


/*!
 * @brief gpugrpGetGpuFromSubDeviceInstance - retrieves the pGpu associated to
 *        a GPU group and a subdevice instance.
 *
 * @param[in]   pGpuGrp         GPUGRP object pointer
 * @param[in]   subDeviceInst   GPU sundevice Instance
 * @param[out]  ppGpu           POBJGPU* pointer
 *
 * @return NV_OK on success
 *         NV_ERR_INVALID_ARGUMENT on NULL pointer parameter
 *         NV_ERR_OBJECT_NOT_FOUND if there is no GPU for the input parameters
 */
NV_STATUS
gpugrpGetGpuFromSubDeviceInstance_IMPL(OBJGPUGRP *pGpuGrp, NvU32 subDeviceInst, OBJGPU **ppGpu)
{
    OBJGPU     *pGpu        = NULL;
    NvU32       gpuInst     = 0;
    NvU32       gpuMask;

    NV_ASSERT_OR_RETURN(pGpuGrp != NULL, NV_ERR_INVALID_ARGUMENT);

    *ppGpu = NULL;

    gpuMask = gpugrpGetGpuMask(pGpuGrp);

    // check for single GPU case
    if (gpumgrGetSubDeviceCount(gpuMask) == 1)
    {
        *ppGpu = gpumgrGetNextGpu(gpuMask, &gpuInst);
    }
    else
    {
        while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInst)) != NULL)
        {
            if (gpumgrGetSubDeviceInstanceFromGpu(pGpu) == subDeviceInst)
            {
                *ppGpu = pGpu;
                break;
            }
        }
    }
    return (*ppGpu == NULL ? NV_ERR_OBJECT_NOT_FOUND : NV_OK);
}

