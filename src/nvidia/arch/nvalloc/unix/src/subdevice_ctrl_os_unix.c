/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nv.h>                     // NV device driver interface
#include <nv-priv.h>
#include <os/os.h>
#include <nvos.h>
#include <osapi.h>
#include "gpu/gpu.h"
#include "gpu/gpu_resource.h"
#include "gpu/subdevice/subdevice.h"
#include <diagnostics/journal.h>
#include "gpu/mem_mgr/mem_desc.h"
#include "mem_mgr/mem.h"
#include <nvpcie.h>
#include <core/locks.h>
#include "rmapi/rs_utils.h"
#include "rmapi/client_resource.h"
#include <class/cl0000.h>
#include <class/cl90cd.h>
#include <class/cl0005.h>  // NV01_EVENT 
#include <class/cl003e.h>  // NV01_MEMORY_SYSTEM
#include <class/cl844c.h>  // G84_PERFBUFFER
#include <ctrl/ctrl0000/ctrl0000gpu.h>
#include <ctrl/ctrl0000/ctrl0000unix.h>
#include <ctrl/ctrl2080/ctrl2080gpu.h>
#include <ctrl/ctrl2080/ctrl2080unix.h>

/*!
 * @brief Implements the NV2080_CTRL_CMD_OS_UNIX_VIDMEM_PERSISTENCE_STATUS
 *        RmControl request. It will check if the GPU video memory will be
 *        persistent during system suspend/resume cycle.
 *
 * @param[in]     pSubdevice
 * @param[in,out] pParams
 *
 * @return
 *      NV_OK   Success
 */
NV_STATUS
subdeviceCtrlCmdOsUnixVidmemPersistenceStatus_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_VIDMEM_PERSISTENCE_STATUS_PARAMS *pParams
)
{
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pSubdevice);
    nv_state_t *nv   = NV_GET_NV_STATE(pGpu);
    nv_priv_t  *nvp  = NV_GET_NV_PRIV(nv);

    pParams->bVidmemPersistent = (nv->preserve_vidmem_allocations || nvp->s0ix_pm_enabled);

    return NV_OK;
}

/*!
 * @brief Implements the NV2080_CTRL_CMD_OS_UNIX_UPDATE_TGP_STATUS
 *        RmControl request. It sets restore TGP flag which is used
 *        to restore TGP limits when client is killed.
 *
 * @param[in] pSubdevice
 * @param[in] pParams
 *
 * @return
 *      NV_OK   Success
 */ 
NV_STATUS
subdeviceCtrlCmdOsUnixUpdateTgpStatus_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_OS_UNIX_UPDATE_TGP_STATUS_PARAMS *pParams
)
{
    pSubdevice->bUpdateTGP = pParams->bUpdateTGP;

    return NV_OK;
}
