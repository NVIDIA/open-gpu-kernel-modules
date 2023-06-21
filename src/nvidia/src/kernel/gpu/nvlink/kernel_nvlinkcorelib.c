/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "os/os.h"
#include "core/hal.h"
#include "core/info_block.h"
#include "core/locks.h"
#include "gpu/gpu.h"

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"

#if defined(INCLUDE_NVLINK_LIB)
static void _knvlinkUpdateRemoteEndUuidInfo(OBJGPU *, KernelNvlink *);
#endif

/*!
 * @brief Initializes NVLink lib - WAR only.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 */
void
knvlinkCoreDriverLoadWar_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
    //
    // All platforms which support NVLINK_CORE should call nvlink_lib_initialize
    // explicitly, if NVLink support is needed. It is not RM's responsibility to
    // initialize NVLink driver. Currently, only windows fails to do the same.
    // Hence, adding this WAR to keep NVLink alive on windows. Also, see
    // nvlinkCoreDriverUnloadWar_IMPL.
    //
    // See Bug 1962411 in order to nuke this WAR for Windows.
    //
#if defined(INCLUDE_NVLINK_LIB)

    if (RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        if (!nvlink_lib_is_initialized())
        {
            nvlink_lib_initialize();
        }
    }

#endif
}

/*!
 * @brief Un-initializes NVLink lib - WAR only.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 */
void
knvlinkCoreDriverUnloadWar_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    if (RMCFG_FEATURE_PLATFORM_WINDOWS)
    {
        nvlink_lib_unload();
    }

#endif
}

/*!
 * @brief Checks whether NVLink driver is supported.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreIsDriverSupported_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    if (nvlink_lib_is_initialized())
    {
        return NV_OK;
    }

#endif

    NV_PRINTF(LEVEL_INFO, "NVLink core lib isn't initialized yet!\n");

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Add GPU device to nvlink core
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreAddDevice_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)

    nvlink_device *dev     = NULL;
    char          *drvName = NULL;
    char          *devName = NULL;
    char          *devIdx  = NULL;

    // Return if the device is already registered
    if (pKernelNvlink->pNvlinkDev)
    {
        NV_PRINTF(LEVEL_INFO, "GPU already registered in NVLINK core!\n");

        return status;
    }

    // Set the driver name
    drvName = portMemAllocNonPaged(NVLINK_DRIVER_NAME_LENGTH);
    if (drvName == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet((void *)drvName, 0, NVLINK_DRIVER_NAME_LENGTH);
    portMemCopy(drvName, sizeof(NVLINK_NVIDIA_DRIVER), NVLINK_NVIDIA_DRIVER,
                sizeof(NVLINK_NVIDIA_DRIVER));

    //
    // Set the temporary device name. The actual device name will be updated
    // after PMU state load completes
    //
    devName = portMemAllocNonPaged(NVLINK_DEVICE_NAME_LENGTH);
    if (devName == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate memory for device name\n");

        goto knvlinkCoreAddDevice_exit;
    }
    portMemSet((void *)devName, 0, NVLINK_DEVICE_NAME_LENGTH);
    portMemCopy(devName, sizeof("GPU"), "GPU", sizeof("GPU"));
    devIdx = devName;
    while (*devIdx != '\0') devIdx++;
    knvlinkUtoa((NvU8 *)devIdx,
                NVLINK_DEVICE_NAME_LENGTH - (devIdx - devName),
                gpuGetInstance(pGpu));

    // Allocate memory for the nvlink_device struct
    dev = portMemAllocNonPaged(sizeof(nvlink_device));
    if (dev == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to create nvlink_device struct for GPU\n");

        goto knvlinkCoreAddDevice_exit;
    }
    portMemSet((void *)dev, 0, sizeof(nvlink_device));

    // Initialize values for the nvlink_device struct
    dev->driverName               = drvName;
    dev->deviceName               = devName;
    dev->type                     = NVLINK_DEVICE_TYPE_GPU;
    dev->pciInfo.domain           = gpuGetDomain(pGpu);
    dev->pciInfo.bus              = gpuGetBus(pGpu);
    dev->pciInfo.device           = gpuGetDevice(pGpu);
    dev->pciInfo.function         = 0;
    dev->pciInfo.pciDeviceId      = pGpu->idInfo.PCIDeviceID;
    dev->pciInfo.bars[0].baseAddr = pGpu->pKernelBus->pciBars[0];
    dev->pciInfo.bars[0].barSize  = pGpu->pKernelBus->pciBarSizes[0];
    dev->initialized              = 1;
    dev->enableALI                = pKernelNvlink->bEnableAli;
    dev->numIoctrls               = nvPopCount32(pKernelNvlink->ioctrlMask);
    dev->numActiveLinksPerIoctrl  = knvlinkGetNumActiveLinksPerIoctrl(pGpu, pKernelNvlink);
    dev->numLinksPerIoctrl        = knvlinkGetTotalNumLinksPerIoctrl(pGpu, pKernelNvlink);
    dev->bReducedNvlinkConfig     = knvlinkIsGpuReducedNvlinkConfig_HAL(pGpu, pKernelNvlink);

    // Register the GPU in nvlink core
    if (nvlink_lib_register_device(dev) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to register GPU in NVLINK core!\n");

        goto knvlinkCoreAddDevice_exit;
    }

    NV_PRINTF(LEVEL_INFO, "GPU registered successfully in NVLINK core\n");

    pKernelNvlink->pNvlinkDev = dev;

    return status;

knvlinkCoreAddDevice_exit:

    portMemFree(drvName);

    portMemFree(devName);

    portMemFree(dev);

#endif

    return status;
}

/*!
 * @brief Update GPU UUID in nvlink core
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreUpdateDeviceUUID_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)

    NvU8   *pGidString = NULL;
    char   *devIdx     = NULL;
    NvU32   flags      = 0;
    NvU32   gidStrLen;

    if (pKernelNvlink->pNvlinkDev)
    {
        //
        // SHA1 uuid format is 16 bytes long. Hence, make sure NVLINK_UUID_LEN
        // is sufficient to store SHA1 uuid format.
        //
        ct_assert(NVLINK_UUID_LEN == RM_SHA1_GID_SIZE);
        ct_assert(NVLINK_UUID_LEN == NV_UUID_LEN);

        flags = FLD_SET_DRF_NUM(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE,
                        NV2080_GPU_CMD_GPU_GET_GID_FLAGS_TYPE_SHA1, flags);
        flags = FLD_SET_DRF_NUM(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT,
                        NV2080_GPU_CMD_GPU_GET_GID_FLAGS_FORMAT_BINARY, flags);

        if (!pKernelNvlink->pNvlinkDev->uuid)
        {
            status = gpuGetGidInfo(pGpu, &pGidString, &gidStrLen, flags);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to update GPU UUID\n");

                return status;
            }
            pKernelNvlink->pNvlinkDev->uuid = (NvU8 *)pGidString;

            _knvlinkUpdateRemoteEndUuidInfo(pGpu, pKernelNvlink);

        }

        // PMU state load has completed. Update the device name
        portMemSet((void *)pKernelNvlink->pNvlinkDev->deviceName, 0, NVLINK_DEVICE_NAME_LENGTH);
        gpuGetNameString(pGpu,
                         NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII,
                         pKernelNvlink->pNvlinkDev->deviceName);

        devIdx = pKernelNvlink->pNvlinkDev->deviceName;
        while (*devIdx != '\0') devIdx++;

        NV_ASSERT((devIdx - pKernelNvlink->pNvlinkDev->deviceName) < NVLINK_DEVICE_NAME_LENGTH);

        knvlinkUtoa((NvU8 *)devIdx,
                   NVLINK_DEVICE_NAME_LENGTH - (devIdx - pKernelNvlink->pNvlinkDev->deviceName),
                   gpuGetInstance(pGpu));
    }

#endif

    return status;
}

/*!
 * @brief Add link to nvlink core lib
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 * @param[in]  linkId         Link Number
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreAddLink_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink,
    NvU32          linkId
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)

    nvlink_link *link     = NULL;
    char        *linkName = NULL;
    char        *linkIdx  = NULL;

    // GPU device must be registered
    if (pKernelNvlink->pNvlinkDev == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink device isn't available.\n");

        return NV_ERR_INVALID_STATE;
    }

    // On RTL, by default minion and SW train is disabled
    if (IS_RTLSIM(pGpu) && !pKernelNvlink->bForceEnableCoreLibRtlsims)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Skipping registration of link %d on simulation.\n", linkId);

        return status;
    }

    // Return if the link is already registered
    if (pKernelNvlink->nvlinkLinks[linkId].core_link)
    {
        NV_PRINTF(LEVEL_INFO, "Link %d already registered in NVLINK core!\n",
                  linkId);

        return status;
    }

    // Set the link name
    linkName = portMemAllocNonPaged(NVLINK_LINK_NAME_LENGTH);
    if (linkName == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet((void *)linkName, 0, NVLINK_LINK_NAME_LENGTH);
    portMemCopy(linkName, sizeof("Link"), "Link", sizeof("Link"));
    linkIdx = linkName;
    while (*linkIdx != '\0') linkIdx++;
    knvlinkUtoa((NvU8 *)linkIdx,
                NVLINK_LINK_NAME_LENGTH - (linkIdx - linkName),
                linkId);

    // Allocate memory for the nvlink_link struct
    link = portMemAllocNonPaged(sizeof(nvlink_link));
    if (link == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR, "Failed to create nvlink_link struct\n");

        goto knvlinkCoreAddLink_exit;
    }
    portMemSet((void *)link, 0, sizeof(nvlink_link));

    // Initialize values for the nvlink_link struct
    link->linkName         = linkName;
    link->linkNumber       = linkId;
    link->state            = NVLINK_LINKSTATE_OFF;
    link->tx_sublink_state = NVLINK_SUBLINK_STATE_TX_OFF;
    link->rx_sublink_state = NVLINK_SUBLINK_STATE_RX_OFF;
    link->bRxDetected      = NV_FALSE;
    link->bInitphase5Fails = NV_FALSE;
    link->version          = pKernelNvlink->ipVerNvlink;
    link->dev              = pKernelNvlink->pNvlinkDev;
    link->link_info        = &(pKernelNvlink->nvlinkLinks[linkId]);
    link->link_handlers    = osGetNvlinkLinkCallbacks();

    if (link->link_handlers == NULL)
    {

        NV_PRINTF(LEVEL_ERROR, "link handlers not found\n");

        goto knvlinkCoreAddLink_exit;
    }

    // Register the link in nvlink core
    if (nvlink_lib_register_link(link->dev, link) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to register link %d in NVLINK core!\n",
                  linkId);

        goto knvlinkCoreAddLink_exit;
    }

    pKernelNvlink->nvlinkLinks[linkId].core_link = link;

    NV_PRINTF(LEVEL_INFO,
              "LINK%d: %s registered successfully in NVLINK core\n", linkId,
              linkName);

    return status;

knvlinkCoreAddLink_exit:

    portMemFree(linkName);

    portMemFree(link);

#endif // defined(INCLUDE_NVLINK_LIB)

    return status;
}

/*!
 * @brief Remove link from nvlink core
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 * @param[in]  linkId         Link Number
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreRemoveLink_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink,
    NvU32          linkId
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)

    // GPU device must be registered
    NV_ASSERT(pKernelNvlink->pNvlinkDev != NULL);

    if (pKernelNvlink->nvlinkLinks[linkId].core_link)
    {
        nvlink_lib_unregister_link(pKernelNvlink->nvlinkLinks[linkId].core_link);

        // Update the RM state now that the link is de-registered
        pKernelNvlink->nvlinkLinks[linkId].core_link = NULL;
        pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected = NV_FALSE;
    }

#endif

    return status;
}

/*!
 * @brief Remove GPU device from nvlink core
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink  pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkCoreRemoveDevice_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    OBJGPU       *pGpu1          = NULL;
    KernelNvlink *pKernelNvlink1 = NULL;
    NvU32         gpuMask        = 0;
    NvU32         gpuInstance    = 0;
    NV_STATUS     status         = NV_OK;

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);

    // Clear peer link masks
    while ((pGpu1 = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        if (pGpu1 == pGpu)
            continue;

        pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
        if (pKernelNvlink1 == NULL)
        {
            continue;
        }

        // Set peerLinkMask from pGpu1 to pGpu to 0
        status = knvlinkSetLinkMaskToPeer(pGpu1, pKernelNvlink1, pGpu, 0);
        if (status != NV_OK)
            return status;
    }

    if (pKernelNvlink->pNvlinkDev)
    {
        nvlink_device *dev = pKernelNvlink->pNvlinkDev;
        nvlink_lib_unregister_device(dev);

        portMemFree((NvU8 *)dev->driverName);
        portMemFree((NvU8 *)dev->deviceName);

        if (dev->uuid)
        {
            portMemFree((NvU8 *)dev->uuid);
        }
        portMemFree(dev);
    }

    // Update the RM cache of the core lib device
    pKernelNvlink->pNvlinkDev = NULL;

#endif

    return NV_OK;
}

/*!
 * @brief Return true if a GPU is connected to an NVSwitch. For now, to keep SW
 * simple we make sure that all enabled links are connected to NVSwitch. Note,
 * on NVSwitch systems currently there is no POR for GPU<->GPU direct peers,
 * everything gets routed through NVSwitch.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink  pointer
 */
NvBool
knvlinkIsGpuConnectedToNvswitch_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
    NvBool bConnected = NV_FALSE;

#if defined(INCLUDE_NVLINK_LIB)

    NvU32 i;
    KNVLINK_CONN_INFO remoteEndInfo;

    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
    {
        remoteEndInfo = pKernelNvlink->nvlinkLinks[i].remoteEndInfo;

        if (remoteEndInfo.bConnected &&
            remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_NVSWITCH)
        {
            bConnected = NV_TRUE;
        }
        else if (remoteEndInfo.bConnected)
        {
            // We don't support a mix of direct connect and switch links
            return NV_FALSE;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

#endif

    return bConnected;
}

/*!
 * @brief Return true if a link is connected.
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink  pointer
 * @param[in]  linkId        Link identifier
 */
NvBool
knvlinkIsLinkConnected_IMPL
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink,
    NvU32          linkId
)
{
#if defined(INCLUDE_NVLINK_LIB)

    //
    // For forced configs, we might not have connectivity information,
    // return true
    //
    if (knvlinkIsForcedConfig(pGpu, pKernelNvlink) ||
        pKernelNvlink->pLinkConnection)
    {
        return NV_TRUE;
    }

    if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected)
    {
        return NV_TRUE;
    }

#endif

    return NV_FALSE;
}

#if defined(INCLUDE_NVLINK_LIB)

/*!
 * @brief Update the UUID for the remote device
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink  pointer
 */
static void
_knvlinkUpdateRemoteEndUuidInfo
(
    OBJGPU        *pGpu,
    KernelNvlink  *pKernelNvlink
)
{
    OBJGPU       *pGpu1          = NULL;
    KernelNvlink *pKernelNvlink1 = NULL;
    unsigned      remoteLinkId;
    unsigned      i, j;

    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
    {
        if (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bConnected &&
            (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_GPU))
        {
            pGpu1 = NULL;

            // Get the remote OBJGPU and Nvlink
            for (j = 0; j < NV_MAX_DEVICES; j++)
            {
                pGpu1 = gpumgrGetGpu(j);

                if (pGpu1 &&
                    (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.domain   == gpuGetDomain(pGpu1) &&
                     pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bus      == gpuGetBus(pGpu1)    &&
                     pKernelNvlink->nvlinkLinks[i].remoteEndInfo.device   == gpuGetDevice(pGpu1) &&
                     pKernelNvlink->nvlinkLinks[i].remoteEndInfo.function == 0))
                {
                    pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);

                    if (pKernelNvlink1)
                    {
                        remoteLinkId = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.linkNumber;

                        // Set the PCI information for remote end
                        portMemCopy(pKernelNvlink1->nvlinkLinks[remoteLinkId].remoteEndInfo.devUuid,
                                    NV_UUID_LEN,
                                    &pGpu->gpuUuid.uuid[0],
                                    NV_UUID_LEN);
                    }
                    break;
                }
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;
}

#endif
