/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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



//***************************** Module Header **********************************
//
// This code is linked into the resource manager proper.  It receives the
//    ioctl from the resource manager's customer, unbundles the args and
//    calls the correct resman routines.
//
//******************************************************************************

#include <core/prelude.h>
#include <core/locks.h>
#include <nv.h>
#include <nv_escape.h>
#include <osapi.h>
#include <rmapi/exports.h>
#include <nv-unix-nvos-params-wrappers.h>

#include <nvos.h>
#include <class/cl0000.h> // NV01_ROOT
#include <class/cl0001.h> // NV01_ROOT_NON_PRIV
#include <class/cl0005.h> // NV01_EVENT
#include <class/cl003e.h> // NV01_MEMORY_SYSTEM
#include <class/cl0071.h> // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR

#include "rmapi/client_resource.h"
#include "nvlog/nvlog.h"
#include <nv-ioctl-lockless-diag.h>

#include <ctrl/ctrl00fd.h>

#include <ctrl/ctrl00e0.h>

#define NV_CTL_DEVICE_ONLY(nv)                 \
{                                              \
    if (((nv)->flags & NV_FLAG_CONTROL) == 0)  \
    {                                          \
        rmStatus = NV_ERR_INVALID_ARGUMENT;    \
        goto done;                             \
    }                                          \
}

#define NV_ACTUAL_DEVICE_ONLY(nv)              \
{                                              \
    if (((nv)->flags & NV_FLAG_CONTROL) != 0)  \
    {                                          \
        rmStatus = NV_ERR_INVALID_ARGUMENT;    \
        goto done;                             \
    }                                          \
}

static NV_STATUS RmGetDeviceFd(NVOS54_PARAMETERS *pApi, NvS32 *pFd,
                               NvBool *pSkipDeviceRef)
{
    RMAPI_PARAM_COPY paramCopy;
    void *pKernelParams;
    NvU32 paramSize;
    NV_STATUS status;

    *pFd = -1;
    *pSkipDeviceRef = NV_TRUE;

    switch(pApi->cmd)
    {
        case NV00FD_CTRL_CMD_ATTACH_GPU:
            paramSize = sizeof(NV00FD_CTRL_ATTACH_GPU_PARAMS);
            break;
        case NV00E0_CTRL_CMD_EXPORT_MEM:
            paramSize = sizeof(NV00E0_CTRL_EXPORT_MEM_PARAMS);
            break;
        default:
            return NV_OK;
    }

    RMAPI_PARAM_COPY_INIT(paramCopy, pKernelParams, pApi->params, paramSize, 1);

    status = rmapiParamsAcquire(&paramCopy, NV_TRUE);
    if (status != NV_OK)
        return status;

    switch(pApi->cmd)
    {
        case NV00FD_CTRL_CMD_ATTACH_GPU:
            {
                NV00FD_CTRL_ATTACH_GPU_PARAMS *pAttachGpuParams = pKernelParams;

                *pSkipDeviceRef = NV_FALSE;
                 *pFd = (NvS32)pAttachGpuParams->devDescriptor;
            }
            break;
        case NV00E0_CTRL_CMD_EXPORT_MEM:
            {
                NV00E0_CTRL_EXPORT_MEM_PARAMS *pExportMemParams = pKernelParams;

                // If hParent is client, no need to reference device.
                *pSkipDeviceRef = (pExportMemParams->hParent == pApi->hClient);
                *pFd = (NvS32)pExportMemParams->devDescriptor;
            }
            break;
        default:
            NV_ASSERT(0);
            break;
    }

    NV_ASSERT(rmapiParamsRelease(&paramCopy) == NV_OK);

    return status;
}

// Only return errors through pApi->status
static void RmCreateOsDescriptor(NVOS32_PARAMETERS *pApi, API_SECURITY_INFO secInfo)
{
    NV_STATUS rmStatus;
    NvBool writable;
    NvU32 flags = 0;
    NvU64 allocSize, pageCount, *pPteArray = NULL;
    void *pDescriptor, *pPageArray = NULL;

    pDescriptor = NvP64_VALUE(pApi->data.AllocOsDesc.descriptor);
    if (((NvUPtr)pDescriptor & ~os_page_mask) != 0)
    {
        rmStatus = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    // Check to prevent an NvU64 overflow
    if ((pApi->data.AllocOsDesc.limit + 1) == 0)
    {
        rmStatus = NV_ERR_INVALID_LIMIT;
        goto done;
    }

    allocSize = (pApi->data.AllocOsDesc.limit + 1);
    pageCount = (1 + ((allocSize - 1) / os_page_size));

    writable = FLD_TEST_DRF(OS32, _ATTR2, _PROTECTION_USER, _READ_WRITE, pApi->data.AllocOsDesc.attr2);

    flags = FLD_SET_DRF_NUM(_LOCK_USER_PAGES, _FLAGS, _WRITE, writable, flags);
    rmStatus = os_lock_user_pages(pDescriptor, pageCount, &pPageArray, flags);
    if (rmStatus == NV_OK)
    {
        pApi->data.AllocOsDesc.descriptor = (NvP64)(NvUPtr)pPageArray;
        pApi->data.AllocOsDesc.descriptorType = NVOS32_DESCRIPTOR_TYPE_OS_PAGE_ARRAY;
    }
    else if (rmStatus == NV_ERR_INVALID_ADDRESS)
    {
        rmStatus = os_lookup_user_io_memory(pDescriptor, pageCount, &pPteArray);
        if (rmStatus == NV_OK)
        {
            pApi->data.AllocOsDesc.descriptor = (NvP64)(NvUPtr)pPteArray;
            pApi->data.AllocOsDesc.descriptorType = NVOS32_DESCRIPTOR_TYPE_OS_IO_MEMORY;
        }
    }
    if (rmStatus != NV_OK)
        goto done;

    Nv04VidHeapControlWithSecInfo(pApi, secInfo);

    if (pApi->status != NV_OK)
    {
        switch (pApi->data.AllocOsDesc.descriptorType)
        {
            default:
                break;
            case NVOS32_DESCRIPTOR_TYPE_OS_PAGE_ARRAY:
                os_unlock_user_pages(pageCount, pPageArray);
                break;
        }
    }

done:
    if (rmStatus != NV_OK)
        pApi->status = rmStatus;
}

// Only return errors through pApi->status
static void RmAllocOsDescriptor(NVOS02_PARAMETERS *pApi, API_SECURITY_INFO secInfo)
{
    NV_STATUS rmStatus = NV_OK;
    NvU32 flags, attr, attr2;
    NVOS32_PARAMETERS *pVidHeapParams;

    if (!FLD_TEST_DRF(OS02, _FLAGS, _LOCATION, _PCI, pApi->flags) ||
        !FLD_TEST_DRF(OS02, _FLAGS, _MAPPING, _NO_MAP, pApi->flags))
    {
        rmStatus = NV_ERR_INVALID_FLAGS;
        goto done;
    }

    attr = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI);

    if (FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _CACHED, pApi->flags) ||
        FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _WRITE_BACK, pApi->flags))
    {
        attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_BACK, attr);
    }
    else if (FLD_TEST_DRF(OS02, _FLAGS, _COHERENCY, _UNCACHED, pApi->flags))
        attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _UNCACHED, attr);
    else {
        rmStatus = NV_ERR_INVALID_FLAGS;
        goto done;
    }

    if (FLD_TEST_DRF(OS02, _FLAGS, _PHYSICALITY, _CONTIGUOUS, pApi->flags))
        attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS, attr);
    else
        attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS, attr);

    if (FLD_TEST_DRF(OS02, _FLAGS, _GPU_CACHEABLE, _YES, pApi->flags))
        attr2 = DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _YES);
    else
        attr2 = DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _NO);

    pVidHeapParams = portMemAllocNonPaged(sizeof(NVOS32_PARAMETERS));
    if (pVidHeapParams == NULL)
    {
        rmStatus = NV_ERR_NO_MEMORY;
        goto done;
    }
    portMemSet(pVidHeapParams, 0, sizeof(NVOS32_PARAMETERS));

    pVidHeapParams->hRoot = pApi->hRoot;
    pVidHeapParams->hObjectParent = pApi->hObjectParent;
    pVidHeapParams->function = NVOS32_FUNCTION_ALLOC_OS_DESCRIPTOR;

    flags = (NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED |
             NVOS32_ALLOC_FLAGS_MAP_NOT_REQUIRED);

    if (DRF_VAL(OS02, _FLAGS, _ALLOC_USER_READ_ONLY, pApi->flags))
        attr2 = FLD_SET_DRF(OS32, _ATTR2, _PROTECTION_USER, _READ_ONLY, attr2);

    // Currently CPU-RO memory implies GPU-RO as well
    if (DRF_VAL(OS02, _FLAGS, _ALLOC_DEVICE_READ_ONLY, pApi->flags) ||
        DRF_VAL(OS02, _FLAGS, _ALLOC_USER_READ_ONLY, pApi->flags))
        attr2 = FLD_SET_DRF(OS32, _ATTR2, _PROTECTION_DEVICE, _READ_ONLY, attr2);

    pVidHeapParams->data.AllocOsDesc.hMemory = pApi->hObjectNew;
    pVidHeapParams->data.AllocOsDesc.flags = flags;
    pVidHeapParams->data.AllocOsDesc.attr = attr;
    pVidHeapParams->data.AllocOsDesc.attr2 = attr2;
    pVidHeapParams->data.AllocOsDesc.descriptor = pApi->pMemory;
    pVidHeapParams->data.AllocOsDesc.limit = pApi->limit;
    pVidHeapParams->data.AllocOsDesc.descriptorType = NVOS32_DESCRIPTOR_TYPE_VIRTUAL_ADDRESS;

    RmCreateOsDescriptor(pVidHeapParams, secInfo);

    pApi->status = pVidHeapParams->status;

    portMemFree(pVidHeapParams);

done:
    if (rmStatus != NV_OK)
        pApi->status = rmStatus;
}

ct_assert(NV_OFFSETOF(NVOS21_PARAMETERS, hRoot) == NV_OFFSETOF(NVOS64_PARAMETERS, hRoot));
ct_assert(NV_OFFSETOF(NVOS21_PARAMETERS, hObjectParent) == NV_OFFSETOF(NVOS64_PARAMETERS, hObjectParent));
ct_assert(NV_OFFSETOF(NVOS21_PARAMETERS, hObjectNew) == NV_OFFSETOF(NVOS64_PARAMETERS, hObjectNew));
ct_assert(NV_OFFSETOF(NVOS21_PARAMETERS, hClass) == NV_OFFSETOF(NVOS64_PARAMETERS, hClass));
ct_assert(NV_OFFSETOF(NVOS21_PARAMETERS, pAllocParms) == NV_OFFSETOF(NVOS64_PARAMETERS, pAllocParms));

NV_STATUS RmIoctl(
    nv_state_t  *nv,
    nv_file_private_t *nvfp,
    NvU32        cmd,
    void        *data,
    NvU32        dataSize
)
{
    NV_STATUS            rmStatus = NV_ERR_GENERIC;
    API_SECURITY_INFO    secInfo = { };

    secInfo.privLevel = osIsAdministrator() ? RS_PRIV_LEVEL_USER_ROOT : RS_PRIV_LEVEL_USER;
    secInfo.paramLocation = PARAM_LOCATION_USER;
    secInfo.pProcessToken = NULL;
    secInfo.gpuOsInfo = NULL;
    secInfo.clientOSInfo = nvfp->ctl_nvfp;
    if (secInfo.clientOSInfo == NULL)
        secInfo.clientOSInfo = nvfp;

    switch (cmd)
    {
        case NV_ESC_RM_ALLOC_MEMORY:
        {
            nv_ioctl_nvos02_parameters_with_fd *pApi;
            NVOS02_PARAMETERS *pParms;

            pApi = data;
            pParms = &pApi->params;

            NV_ACTUAL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(nv_ioctl_nvos02_parameters_with_fd))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            if (pParms->hClass == NV01_MEMORY_SYSTEM_OS_DESCRIPTOR)
                RmAllocOsDescriptor(pParms, secInfo);
            else
            {
                NvU32 flags = pParms->flags;

                Nv01AllocMemoryWithSecInfo(pParms, secInfo);

                //
                // If the system memory is going to be mapped immediately,
                // create the mmap context for it now.
                //
                if ((pParms->hClass == NV01_MEMORY_SYSTEM) &&
                    (!FLD_TEST_DRF(OS02, _FLAGS, _ALLOC, _NONE, flags)) &&
                    (!FLD_TEST_DRF(OS02, _FLAGS, _MAPPING, _NO_MAP, flags)) &&
                    (pParms->status == NV_OK))
                {
                    if (rm_create_mmap_context(pParms->hRoot,
                            pParms->hObjectParent, pParms->hObjectNew,
                            pParms->pMemory, pParms->limit + 1, 0,
                            NV_MEMORY_DEFAULT,
                            pApi->fd) != NV_OK)
                    {
                        NV_PRINTF(LEVEL_WARNING,
                                  "could not create mmap context for %p\n",
                                  NvP64_VALUE(pParms->pMemory));
                        rmStatus = NV_ERR_INVALID_ARGUMENT;
                        goto done;
                    }
                }
            }

            break;
        }

        case NV_ESC_RM_ALLOC_OBJECT:
        {
            NVOS05_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS05_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv01AllocObjectWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_ALLOC:
        {
            NVOS21_PARAMETERS *pApi = data;
            NVOS64_PARAMETERS *pApiAccess = data;
            NvBool bAccessApi = (dataSize == sizeof(NVOS64_PARAMETERS));

            if ((dataSize != sizeof(NVOS21_PARAMETERS)) &&
                (dataSize != sizeof(NVOS64_PARAMETERS)))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            switch (pApi->hClass)
            {
                case NV01_ROOT:
                case NV01_ROOT_CLIENT:
                case NV01_ROOT_NON_PRIV:
                {
                    NV_CTL_DEVICE_ONLY(nv);

                    // Force userspace client allocations to be the _CLIENT class.
                    pApi->hClass = NV01_ROOT_CLIENT;
                    break;
                }
                case NV01_EVENT:
                case NV01_EVENT_OS_EVENT:
                case NV01_EVENT_KERNEL_CALLBACK:
                case NV01_EVENT_KERNEL_CALLBACK_EX:
                {
                    break;
                }
                default:
                {
                    NV_CTL_DEVICE_ONLY(nv);
                    break;
                }
            }

            if (!bAccessApi)
            {
                Nv04AllocWithSecInfo(pApi, secInfo);
            }
            else
            {
                Nv04AllocWithAccessSecInfo(pApiAccess, secInfo);
            }

            break;
        }

        case NV_ESC_RM_FREE:
        {
            NVOS00_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS00_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv01FreeWithSecInfo(pApi, secInfo);

            if (pApi->status == NV_OK &&
                pApi->hObjectOld == pApi->hRoot)
            {
                rm_client_free_os_events(pApi->hRoot);
            }

            break;
        }

        case NV_ESC_RM_VID_HEAP_CONTROL:
        {
            NVOS32_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS32_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            if (pApi->function == NVOS32_FUNCTION_ALLOC_OS_DESCRIPTOR)
                RmCreateOsDescriptor(pApi, secInfo);
            else
                Nv04VidHeapControlWithSecInfo(pApi, secInfo);

            break;
        }

        case NV_ESC_RM_I2C_ACCESS:
        {
            NVOS_I2C_ACCESS_PARAMS *pApi = data;

            NV_ACTUAL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS_I2C_ACCESS_PARAMS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04I2CAccessWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_IDLE_CHANNELS:
        {
            NVOS30_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS30_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04IdleChannelsWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_MAP_MEMORY:
        {
            nv_ioctl_nvos33_parameters_with_fd *pApi;
            NVOS33_PARAMETERS *pParms;

            pApi = data;
            pParms = &pApi->params;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(nv_ioctl_nvos33_parameters_with_fd))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            // Don't allow userspace to override the caching type
            pParms->flags = FLD_SET_DRF(OS33, _FLAGS, _CACHING_TYPE, _DEFAULT, pParms->flags);
            Nv04MapMemoryWithSecInfo(pParms, secInfo);

            if (pParms->status == NV_OK)
            {
                pParms->status = rm_create_mmap_context(pParms->hClient,
                                 pParms->hDevice, pParms->hMemory,
                                 pParms->pLinearAddress, pParms->length,
                                 pParms->offset,
                                 DRF_VAL(OS33, _FLAGS, _CACHING_TYPE, pParms->flags),
                                 pApi->fd);
                if (pParms->status != NV_OK)
                {
                    NVOS34_PARAMETERS params;
                    portMemSet(&params, 0, sizeof(NVOS34_PARAMETERS));
                    params.hClient        = pParms->hClient;
                    params.hDevice        = pParms->hDevice;
                    params.hMemory        = pParms->hMemory;
                    params.pLinearAddress = pParms->pLinearAddress;
                    params.flags          = pParms->flags;
                    Nv04UnmapMemoryWithSecInfo(&params, secInfo);
                }
            }
            break;
        }

        case NV_ESC_RM_UNMAP_MEMORY:
        {
            NVOS34_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS34_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04UnmapMemoryWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_ACCESS_REGISTRY:
        {
            NVOS38_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS38_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            pApi->status = rm_access_registry(pApi->hClient,
                                              pApi->hObject,
                                              pApi->AccessType,
                                              pApi->pDevNode,
                                              pApi->DevNodeLength,
                                              pApi->pParmStr,
                                              pApi->ParmStrLength,
                                              pApi->pBinaryData,
                                              &pApi->BinaryDataLength,
                                              &pApi->Data,
                                              &pApi->Entry);
            break;
        }

        case NV_ESC_RM_ALLOC_CONTEXT_DMA2:
        {
            NVOS39_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS39_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04AllocContextDmaWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_BIND_CONTEXT_DMA:
        {
            NVOS49_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS49_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04BindContextDmaWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_MAP_MEMORY_DMA:
        {
            NVOS46_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS46_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04MapMemoryDmaWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_UNMAP_MEMORY_DMA:
        {
            NVOS47_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS47_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04UnmapMemoryDmaWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_DUP_OBJECT:
        {
            NVOS55_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS55_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04DupObjectWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_RM_SHARE:
        {
            NVOS57_PARAMETERS *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS57_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            Nv04ShareWithSecInfo(pApi, secInfo);
            break;
        }

        case NV_ESC_STATUS_CODE:
        {
            nv_state_t *pNv;
            nv_ioctl_status_code_t *pApi = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(nv_ioctl_status_code_t))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            pNv = nv_get_adapter_state(pApi->domain, pApi->bus, pApi->slot);
            if (pNv == NULL)
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            rmStatus = rm_get_adapter_status(pNv, &pApi->status);

            if (rmStatus != NV_OK)
                goto done;

            break;
        }

        case NV_ESC_RM_CONTROL:
        {
            NVOS54_PARAMETERS *pApi = data;
            void *priv = NULL;
            nv_file_private_t *dev_nvfp = NULL;
            NvS32 fd;
            NvBool bSkipDeviceRef;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS54_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            rmStatus = RmGetDeviceFd(pApi, &fd, &bSkipDeviceRef);
            if (rmStatus != NV_OK)
            {
                goto done;
            }

            if (!bSkipDeviceRef)
            {
                dev_nvfp = nv_get_file_private(fd, NV_FALSE, &priv);
                if (dev_nvfp == NULL)
                {
                    rmStatus = NV_ERR_INVALID_DEVICE;
                    goto done;
                }

                // Check to avoid cyclic dependency with NV_ESC_REGISTER_FD
                if (!portAtomicCompareAndSwapU32(&dev_nvfp->register_or_refcount,
                                                 NVFP_TYPE_REFCOUNTED,
                                                 NVFP_TYPE_NONE))
                {
                    // Is this already refcounted...
                    if (dev_nvfp->register_or_refcount != NVFP_TYPE_REFCOUNTED)
                    {
                        nv_put_file_private(priv);
                        rmStatus = NV_ERR_IN_USE;
                        goto done;
                    }
                }

                secInfo.gpuOsInfo = priv;
            }

            Nv04ControlWithSecInfo(pApi, secInfo);

            if ((pApi->status != NV_OK) && (priv != NULL))
            {
                //
                // No need to reset `register_or_refcount` as it might be set
                // for previous successful calls. We let it clear with FD close.
                //
                nv_put_file_private(priv);

                secInfo.gpuOsInfo = NULL;
            }

            break;
        }

        case NV_ESC_RM_UPDATE_DEVICE_MAPPING_INFO:
        {
            NVOS56_PARAMETERS *pApi = data;
            void *pOldCpuAddress;
            void *pNewCpuAddress;

            NV_CTL_DEVICE_ONLY(nv);

            if (dataSize != sizeof(NVOS56_PARAMETERS))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            pOldCpuAddress = NvP64_VALUE(pApi->pOldCpuAddress);
            pNewCpuAddress = NvP64_VALUE(pApi->pNewCpuAddress);

            pApi->status = rm_update_device_mapping_info(pApi->hClient,
                                                         pApi->hDevice,
                                                         pApi->hMemory,
                                                         pOldCpuAddress,
                                                         pNewCpuAddress);
            break;
        }

        case NV_ESC_RM_LOCKLESS_DIAGNOSTIC:
        {
            NV_LOCKLESS_DIAGNOSTIC_PARAMS *pParams = data;

            NV_CTL_DEVICE_ONLY(nv);

            if (!osIsAdministrator())
            {
                rmStatus = NV_ERR_INSUFFICIENT_PERMISSIONS;
                pParams->status = rmStatus;
                goto done;
            }

            switch (pParams->cmd)
            {
                // Do not use NVOC _DISPATCH here as it dereferences NULL RmClientResource*
                case NV0000_CTRL_CMD_NVD_GET_NVLOG_INFO:
                    rmStatus = cliresCtrlCmdNvdGetNvlogInfo_IMPL(NULL, &pParams->params.getNvlogInfo);
                    break;
                case NV0000_CTRL_CMD_NVD_GET_NVLOG_BUFFER_INFO:
                    rmStatus = cliresCtrlCmdNvdGetNvlogBufferInfo_IMPL(NULL, &pParams->params.getNvlogBufferInfo);
                    break;
                case NV0000_CTRL_CMD_NVD_GET_NVLOG:
                    rmStatus = cliresCtrlCmdNvdGetNvlog_IMPL(NULL, &pParams->params.getNvlog);
                    break;
                default:
                    rmStatus = NV_ERR_NOT_SUPPORTED;
                    break;
            }

            pParams->status = rmStatus;
            goto done;
        }

        case NV_ESC_REGISTER_FD:
        {
            nv_ioctl_register_fd_t *params = data;
            void *priv = NULL;
            nv_file_private_t *ctl_nvfp;

            if (dataSize != sizeof(nv_ioctl_register_fd_t))
            {
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            // LOCK: acquire API lock
            rmStatus = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_OSAPI);
            if (rmStatus != NV_OK)
                goto done;

            // If there is already a ctl fd registered on this nvfp, fail.
            if (nvfp->ctl_nvfp != NULL)
            {
                // UNLOCK: release API lock
                rmapiLockRelease();
                rmStatus = NV_ERR_INVALID_STATE;
                goto done;
            }

            //
            // Note that this call is valid for both "actual" devices and ctrl
            // devices.  In particular, NV_ESC_ALLOC_OS_EVENT can be used with
            // both types of devices.
            // But, the ctl_fd passed in should always correspond to a control FD.
            //
            ctl_nvfp = nv_get_file_private(params->ctl_fd,
                                           NV_TRUE, /* require ctl fd */
                                           &priv);
            if (ctl_nvfp == NULL)
            {
                // UNLOCK: release API lock
                rmapiLockRelease();
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            // Disallow self-referential links, and disallow links to FDs that
            // themselves have a link.
            if ((ctl_nvfp == nvfp) || (ctl_nvfp->ctl_nvfp != NULL))
            {
                nv_put_file_private(priv);
                // UNLOCK: release API lock
                rmapiLockRelease();
                rmStatus = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            // Check to avoid cyclic dependency with device refcounting
            if (!portAtomicCompareAndSwapU32(&nvfp->register_or_refcount,
                                             NVFP_TYPE_REGISTERED,
                                             NVFP_TYPE_NONE))
            {
                nv_put_file_private(priv);
                // UNLOCK: release API lock
                rmapiLockRelease();
                rmStatus = NV_ERR_IN_USE;
                goto done;
            }

            //
            // nvfp->ctl_nvfp is read outside the lock, so set it atomically.
            // Note that once set, this can never be removed until the fd
            // associated with nvfp is closed.  We hold on to 'priv' until the
            // fd is closed, too, to ensure that the fd associated with
            // ctl_nvfp remains valid.
            //
            portAtomicSetSize(&nvfp->ctl_nvfp, ctl_nvfp);
            nvfp->ctl_nvfp_priv = priv;

            // UNLOCK: release API lock
            rmapiLockRelease();

            // NOTE: nv_put_file_private(priv) is not called here.  It MUST be
            // called during cleanup of this nvfp.
            rmStatus = NV_OK;
            break;
        }

        default:
        {
            NV_PRINTF(LEVEL_ERROR, "unknown NVRM ioctl command: 0x%x\n", cmd);
            goto done;
        }
    }

    rmStatus = NV_OK;
done:

    return rmStatus;
}
