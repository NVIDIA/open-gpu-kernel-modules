/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nv.h>

#include <nv-priv.h>
#include <osapi.h>
#include <core/locks.h>

NV_STATUS
RmPowerManagementTegra(
    OBJGPU *pGpu,
    nv_pm_action_t pmAction
)
{
    //
    // Default to NV_OK. there may cases where resman is loaded, but
    // no devices are allocated (we're still at the console). in these
    // cases, it's fine to let the system do whatever it wants.
    //
    NV_STATUS rmStatus = NV_OK;

    if (pGpu)
    {
        nv_state_t *nv = NV_GET_NV_STATE(pGpu);
        nv_priv_t  *nvp = NV_GET_NV_PRIV(nv);

        switch (pmAction)
        {
            case NV_PM_ACTION_HIBERNATE:
                nvp->pm_state.InHibernate = NV_TRUE;
                pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_TRUE);
                rmStatus = gpuStateUnload(pGpu, GPU_STATE_FLAGS_PM_HIBERNATE |
                                                GPU_STATE_FLAGS_PRESERVING |
                                                GPU_STATE_FLAGS_PM_TRANSITION);
                pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE, NV_TRUE);
                break;

            case NV_PM_ACTION_STANDBY:
                nvp->pm_state.InHibernate = NV_FALSE;
                pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_TRUE);
                rmStatus = gpuStateUnload(pGpu, GPU_STATE_FLAGS_PM_SUSPEND |
                                                GPU_STATE_FLAGS_PRESERVING |
                                                GPU_STATE_FLAGS_PM_TRANSITION);;
                pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_STANDBY, NV_TRUE);
                pGpu->bInD3Cold = NV_TRUE;
                break;

            case NV_PM_ACTION_RESUME:
                pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH, NV_TRUE);

                if (nvp->pm_state.InHibernate)
                {
                    rmStatus = gpuStateLoad(pGpu, GPU_STATE_FLAGS_PM_HIBERNATE |
                                                  GPU_STATE_FLAGS_PRESERVING |
                                                  GPU_STATE_FLAGS_PM_TRANSITION);;
                    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE, NV_FALSE);
                }
                else
                {
                    rmStatus = gpuStateLoad(pGpu, GPU_STATE_FLAGS_PM_SUSPEND |
                                                  GPU_STATE_FLAGS_PRESERVING |
                                                  GPU_STATE_FLAGS_PM_TRANSITION);;
                    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_STANDBY, NV_FALSE);
                    pGpu->bInD3Cold = NV_FALSE;
                }

                pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_FALSE);
                pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH, NV_FALSE);
                break;

            default:
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                break;
        }
    }

    return rmStatus;
}

