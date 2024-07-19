/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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



#include "dp/nvdp-connector.h"
#include "dp/nvdp-timer.h"
#include "dp/nvdp-device.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "g_nvkms-evo-states.h"
#include "nvkms-event.h"
#include "nvkms-dpy.h"
#include "nvkms-types.h"
#include "nvkms-evo.h"
#include "nvkms-dma.h"
#include "nvkms-utils.h"
#include "nvkms-private.h"
#include "nvkms-modeset.h"
#include "nvkms-surface.h"
#include "nvkms-vrr.h"

#include "nvkms-push.h"
#include "nvkms-difr.h"

#include "class/cl0005.h" /* NV01_EVENT */

#include <class/cl0070.h> // NV01_MEMORY_VIRTUAL
#include <class/cl0073.h> /* NV04_DISPLAY_COMMON */
#include <class/cl003e.h> /* NV01_MEMORY_SYSTEM */
#include <class/cl0076.h> /* NV01_MEMORY_FRAMEBUFFER_CONSOLE */
#include <class/cl0080.h> /* NV01_DEVICE_0 */
#include <class/cl0040.h> /* NV01_MEMORY_LOCAL_USER */
#include <class/cl2080.h> /* NV20_SUBDEVICE_0 */

#include "class/clc37b.h" /* NVC37B_WINDOW_IMM_CHANNEL_DMA */
#include "class/clc37e.h" /* NVC37E_WINDOW_CHANNEL_DMA */
#include "class/clc57b.h" /* NVC57B_WINDOW_IMM_CHANNEL_DMA */
#include "class/clc57e.h" /* NVC57E_WINDOW_CHANNEL_DMA */
#include "class/clc67b.h" /* NVC67B_WINDOW_IMM_CHANNEL_DMA */
#include "class/clc67e.h" /* NVC67E_WINDOW_CHANNEL_DMA */

#include "class/cl917b.h" /* NV917B_OVERLAY_IMM_CHANNEL_PIO */

#include "class/cl927c.h" /* NV927C_BASE_CHANNEL_DMA */

#include "class/cl917e.h" /* NV917E_OVERLAY_CHANNEL_DMA */

#include <ctrl/ctrl0000/ctrl0000gpu.h> /* NV0000_CTRL_GPU_* */
#include <ctrl/ctrl0002.h> /* NV0002_CTRL_CMD_BIND_CONTEXTDMA */
#include <ctrl/ctrl0073/ctrl0073dfp.h> /* NV0073_CTRL_CMD_DFP_GET_INFO */
#include <ctrl/ctrl0073/ctrl0073dp.h> /* NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID */
#include <ctrl/ctrl0073/ctrl0073specific.h> /* NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO */
#include <ctrl/ctrl0073/ctrl0073system.h> /* NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED */
#include <ctrl/ctrl0080/ctrl0080gpu.h> /* NV0080_CTRL_CMD_GPU_SET_DISPLAY_OWNER */
#include <ctrl/ctrl0080/ctrl0080gr.h> /* NV0080_CTRL_CMD_GR_GET_CAPS_V2 */
#include <ctrl/ctrl0080/ctrl0080unix.h> /* NV0080_CTRL_CMD_OS_UNIX_VT_SWITCH */
#include <ctrl/ctrl2080/ctrl2080bios.h> /* NV2080_CTRL_CMD_BIOS_GET_NBSI */
#include <ctrl/ctrl2080/ctrl2080bus.h> /* NV2080_CTRL_CMD_BUS_GET_INFO */
#include <ctrl/ctrl2080/ctrl2080event.h> /* NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION */
#include <ctrl/ctrl2080/ctrl2080tmr.h> /* NV2080_CTRL_CMD_TIMER_GET_TIME */
#include <ctrl/ctrl2080/ctrl2080unix.h> /* NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT */
#include <ctrl/ctrl5070/ctrl5070chnc.h> /* NV5070_CTRL_CMD_SET_RMFREE_FLAGS */
#include <ctrl/ctrl5070/ctrl5070or.h> /* NV5070_CTRL_CMD_SET_DAC_PWR */
#include <ctrl/ctrl0000/ctrl0000system.h> /* NV0000_CTRL_CMD_SYSTEM_GET_APPROVAL_COOKIE */

#include "nvos.h"

#include "displayport/dpcd.h"

#define NVKMS_SYNCPT_ID_INVALID     (0xFFFFFFFF)

static NvU32 GetLegacyConnectorType(NVDispEvoPtr pDispEvo, NVDpyId dpyId);

static void RmFreeEvoChannel(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel);

static NvBool EngineListCheckOneSubdevice(const NVEvoSubDeviceRec *pSubDevice,
                                          NvU32 engineType)
{
    const NvU32 *engines = pSubDevice->supportedEngines;
    int i;

    for (i = 0; i < pSubDevice->numEngines; i++) {
        if (engines[i] == engineType) {
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool EngineListCheck(const NVDevEvoRec *pDevEvo, NvU32 engineType)
{
    int sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (!EngineListCheckOneSubdevice(pDevEvo->pSubDevices[sd],
                                         engineType)) {
            return FALSE;
        }
    }

    return TRUE;
}

static NvBool QueryGpuCapabilities(NVDevEvoPtr pDevEvo)
{
    NvBool ctxDmaCoherentAllowedDev = FALSE;
    NvBool ctxDmaNonCoherentAllowedDev = FALSE;
    NvU32 ret, sd;

    NV0000_CTRL_GPU_GET_ID_INFO_PARAMS idInfoParams = { 0 };

    pDevEvo->isHeadSurfaceSupported = FALSE;

    if (EngineListCheck(pDevEvo, NV2080_ENGINE_TYPE_GRAPHICS)) {
        NV0080_CTRL_GR_GET_CAPS_V2_PARAMS grCaps = { 0 };

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->deviceHandle,
                             NV0080_CTRL_CMD_GR_GET_CAPS_V2,
                             &grCaps,
                             sizeof(grCaps));

        if (ret != NVOS_STATUS_SUCCESS) {
            return FALSE;
        }

        /* Assume headSurface is supported if there is a graphics engine
         * and headSurface support is included in the NVKMS build.
         */
        pDevEvo->isHeadSurfaceSupported = NVKMS_INCLUDE_HEADSURFACE;
    }

    /* ctxDma{,Non}CoherentAllowed */

    /* simulationType */

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        NV2080_CTRL_GPU_GET_SIMULATION_INFO_PARAMS simParams = { 0 };

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->pSubDevices[sd]->handle,
                             NV2080_CTRL_CMD_GPU_GET_SIMULATION_INFO,
                             &simParams,
                             sizeof(simParams));

        if (ret != NVOS_STATUS_SUCCESS) {
            simParams.type = NV2080_CTRL_GPU_GET_SIMULATION_INFO_TYPE_NONE;
        }
        if (sd == 0) {
            pDevEvo->simulationType = simParams.type;
        }
        nvAssert(pDevEvo->simulationType == simParams.type);
    }

    /* mobile */

    idInfoParams.gpuId = pDevEvo->pSubDevices[0]->gpuId;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         nvEvoGlobal.clientHandle,
                         NV0000_CTRL_CMD_GPU_GET_ID_INFO,
                         &idInfoParams, sizeof(idInfoParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        pDevEvo->mobile = FALSE;
        pDevEvo->isSOCDisplay = FALSE;
    } else {
        pDevEvo->mobile =
            FLD_TEST_DRF(0000, _CTRL_GPU_ID_INFO, _MOBILE, _TRUE,
                         idInfoParams.gpuFlags);

        pDevEvo->isSOCDisplay =
            FLD_TEST_DRF(0000, _CTRL_GPU_ID_INFO, _SOC, _TRUE,
                         idInfoParams.gpuFlags);
    }

    /* TODO: This cap bit should be queried from RM */
    pDevEvo->requiresAllAllocationsInSysmem = pDevEvo->isSOCDisplay;

    /*
     * Prohibit vblank_sem_control if:
     * - on tegra, or
     * - the kernel interface layer says so, or
     * - (RM-based) SLI mosaic is enabled (WAR for bug 4552673, until RM-based
     *   SLI is dropped)
     */
    pDevEvo->supportsVblankSemControl =
        !pDevEvo->isSOCDisplay &&
        nvkms_vblank_sem_control() &&
        !pDevEvo->sli.mosaic;

    /* ctxDma{,Non}CoherentAllowed */

    if (!pDevEvo->isSOCDisplay) {
        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            NV2080_CTRL_BUS_GET_INFO_PARAMS busParams = { 0 };
            struct {
                NV2080_CTRL_BUS_INFO coherentFlags;
                NV2080_CTRL_BUS_INFO nonCoherentFlags;
            } busInfoList = { { 0 } };

            NvBool ctxDmaCoherentAllowed;
            NvBool ctxDmaNonCoherentAllowed;

            busInfoList.coherentFlags.index =
                NV2080_CTRL_BUS_INFO_INDEX_COHERENT_DMA_FLAGS;
            busInfoList.nonCoherentFlags.index =
                NV2080_CTRL_BUS_INFO_INDEX_NONCOHERENT_DMA_FLAGS;

            busParams.busInfoListSize =
                sizeof(busInfoList) / sizeof(busInfoList.coherentFlags);
            busParams.busInfoList = NV_PTR_TO_NvP64(&busInfoList);

            ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                                 pDevEvo->pSubDevices[sd]->handle,
                                 NV2080_CTRL_CMD_BUS_GET_INFO,
                                 &busParams, sizeof(busParams));

            if (ret != NVOS_STATUS_SUCCESS) {
                return FALSE;
            }

            ctxDmaCoherentAllowed =
                FLD_TEST_DRF(2080_CTRL_BUS_INFO, _COHERENT_DMA_FLAGS,
                             _CTXDMA, _TRUE, busInfoList.coherentFlags.data);

            ctxDmaNonCoherentAllowed =
                FLD_TEST_DRF(2080_CTRL_BUS_INFO, _NONCOHERENT_DMA_FLAGS,
                             _CTXDMA, _TRUE, busInfoList.nonCoherentFlags.data);

            if (sd == 0) {
                ctxDmaCoherentAllowedDev = ctxDmaCoherentAllowed;
                ctxDmaNonCoherentAllowedDev = ctxDmaNonCoherentAllowed;
            } else {
                ctxDmaCoherentAllowedDev =
                    ctxDmaCoherentAllowedDev && ctxDmaCoherentAllowed;
                ctxDmaNonCoherentAllowedDev =
                    ctxDmaNonCoherentAllowedDev && ctxDmaNonCoherentAllowed;
            }
        }
        nvAssert(ctxDmaCoherentAllowedDev || ctxDmaNonCoherentAllowedDev);

        if (ctxDmaCoherentAllowedDev) {
            pDevEvo->isoIOCoherencyModes.coherent = TRUE;
            pDevEvo->nisoIOCoherencyModes.coherent = TRUE;
        }

        if (ctxDmaNonCoherentAllowedDev) {
            pDevEvo->isoIOCoherencyModes.noncoherent = TRUE;
            pDevEvo->nisoIOCoherencyModes.noncoherent = TRUE;
        }
    } else {
        /*
         * On SOC display, NISO requests are IO-coherent and ISO
         * requests are non-coherent.
         */
        pDevEvo->isoIOCoherencyModes.noncoherent = TRUE;
        pDevEvo->nisoIOCoherencyModes.coherent = TRUE;
    }

    pDevEvo->supportsSyncpts =
        FALSE;

    return TRUE;
}


static void FreeDisplay(NVDispEvoPtr pDispEvo)
{
    if (pDispEvo == NULL) {
        return;
    }

    nvAssert(pDispEvo->vrrSetTimeoutEventUsageCount == 0);
    nvAssert(pDispEvo->vrrSetTimeoutEventHandle == 0);

#if defined(DEBUG)
    for (NvU32 apiHead = 0;
         apiHead < ARRAY_LEN(pDispEvo->pSwapGroup); apiHead++) {
        nvAssert(pDispEvo->pSwapGroup[apiHead] == NULL);
    }
#endif

    nvAssert(nvListIsEmpty(&pDispEvo->dpyList));

    nvkms_free_ref_ptr(pDispEvo->ref_ptr);

    nvInvalidateRasterLockGroupsEvo();
    nvFree(pDispEvo);
}


static inline NVDispEvoPtr AllocDisplay(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo = nvCalloc(1, sizeof(NVDispEvoRec));

    if (pDispEvo == NULL) {
        goto fail;
    }

    pDispEvo->pDevEvo = pDevEvo;

    nvListInit(&pDispEvo->dpyList);
    nvListInit(&pDispEvo->connectorList);

    pDispEvo->framelock.server = nvInvalidDpyId();
    pDispEvo->framelock.clients = nvEmptyDpyIdList();
    pDispEvo->framelock.currentServerHead = NV_INVALID_HEAD;

    pDispEvo->ref_ptr = nvkms_alloc_ref_ptr(pDispEvo);
    if (!pDispEvo->ref_ptr) {
        goto fail;
    }

    return pDispEvo;

fail:
    FreeDisplay(pDispEvo);

    return NULL;
}


static void FreeDisplays(NVDevEvoPtr pDevEvo)
{
    unsigned int sd;

    for (sd = 0; sd < pDevEvo->nDispEvo; sd++) {
        FreeDisplay(pDevEvo->pDispEvo[sd]);
        pDevEvo->pDispEvo[sd] = NULL;
    }
    pDevEvo->nDispEvo = 0;
}


/*!
 * Allocate the NVDispRecs for the given pDev.
 *
 * \param[in,out]  pDev  The device for which to allocate Displays.
 */
static NvBool AllocDisplays(NVDevEvoPtr pDevEvo)
{
    unsigned int sd;

    nvAssert(pDevEvo->nDispEvo == 0);

    pDevEvo->nDispEvo = pDevEvo->numSubDevices;

    for (sd = 0; sd < pDevEvo->nDispEvo; sd++) {
        NVDispEvoPtr pDispEvo = AllocDisplay(pDevEvo);

        if (pDispEvo == NULL) {
            goto fail;
        }

        pDevEvo->pDispEvo[sd] = pDispEvo;

        pDispEvo->displayOwner = sd;

        pDispEvo->gpuLogIndex = pDevEvo->pSubDevices[sd]->gpuLogIndex;
    }

    return TRUE;

fail:
    FreeDisplays(pDevEvo);
    return FALSE;
}

/*
 * Get the (id) list of all supported display devices for this pDisp.
 */
static NvBool ProbeValidDisplays(NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS getSupportedParams = { 0 };
    NvU32 ret;

    pDispEvo->connectorIds = nvEmptyDpyIdList();
    pDispEvo->displayPortMSTIds = nvEmptyDpyIdList();
    pDispEvo->dynamicDpyIds = nvEmptyDpyIdList();
    pDispEvo->validDisplays = nvEmptyDpyIdList();

    getSupportedParams.subDeviceInstance = pDispEvo->displayOwner;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED,
                         &getSupportedParams, sizeof(getSupportedParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to get supported display device(s)");
    } else {
        NVDpyIdList dpyIdList;
        NVDpyId dpyId;

        // Grab only the static ids from the list.  Dynamic ids are
        // used to communicate with devices that are connected to
        // a connector that has a static id.
        dpyIdList = nvNvU32ToDpyIdList(getSupportedParams.displayMask);

        FOR_ALL_DPY_IDS(dpyId, dpyIdList) {
            NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS getOrInfoParams = { 0 };
            getOrInfoParams.subDeviceInstance = pDispEvo->displayOwner;
            getOrInfoParams.displayId = nvDpyIdToNvU32(dpyId);

            ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                                 pDevEvo->displayCommonHandle,
                                 NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO,
                                 &getOrInfoParams,
                                 sizeof(getOrInfoParams));
            if (ret != NVOS_STATUS_SUCCESS) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                            "Failed to get supported display device(s)");
            } else {
                if (!getOrInfoParams.bIsDispDynamic) {
                    pDispEvo->connectorIds =
                        nvAddDpyIdToDpyIdList(dpyId, pDispEvo->connectorIds);
                }
            }
        }
    }

    pDispEvo->validDisplays = pDispEvo->connectorIds;

    return TRUE;
}

/*!
 * Return TRUE if every pDispEvo on this pDevEvo has an empty validDisplays.
 */
static NvBool NoValidDisplays(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    unsigned int sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        if (!nvDpyIdListIsEmpty(pDispEvo->validDisplays)) {
            return FALSE;
        }
    }

    return TRUE;
}


/*
 * Find the NvKmsConnectorSignalFormat for the pConnectorEvo.
 */
static NvKmsConnectorSignalFormat
GetSignalFormat(const NVConnectorEvoRec *pConnectorEvo)
{
    // SignalFormat represents a weird combination of our OR type and protocol.
    switch (pConnectorEvo->or.type) {
    case NV0073_CTRL_SPECIFIC_OR_TYPE_DAC:
        switch (pConnectorEvo->or.protocol) {
        default:
            nvAssert(!"Unexpected OR protocol for DAC");
            // fall through
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DAC_RGB_CRT:
            return NVKMS_CONNECTOR_SIGNAL_FORMAT_VGA;
        }

    case NV0073_CTRL_SPECIFIC_OR_TYPE_SOR:
        switch (pConnectorEvo->or.protocol) {
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_LVDS_CUSTOM:
            return NVKMS_CONNECTOR_SIGNAL_FORMAT_LVDS;

        default:
            nvAssert(!"Unexpected OR protocol for SOR");
            // fall through
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_A:
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_B:
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DUAL_TMDS:
            return NVKMS_CONNECTOR_SIGNAL_FORMAT_TMDS;

        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A:
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B:
            return NVKMS_CONNECTOR_SIGNAL_FORMAT_DP;
        }

    case NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR:
        switch (pConnectorEvo->or.protocol) {
        default:
            nvAssert(!"Unexpected OR protocol for PIOR");
            // fall through
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_PIOR_EXT_TMDS_ENC:
            return NVKMS_CONNECTOR_SIGNAL_FORMAT_TMDS;
        }

    case NV0073_CTRL_SPECIFIC_OR_TYPE_DSI:
        switch (pConnectorEvo->or.protocol) {
        default:
            nvAssert(!"Unexpected OR protocol for DSI");
            // fall through
        case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DSI:
            return NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI;
        }

    default:
        nvAssert(!"Unexpected OR type");
        return NVKMS_CONNECTOR_SIGNAL_FORMAT_UNKNOWN;
    }

    return NVKMS_CONNECTOR_SIGNAL_FORMAT_UNKNOWN;
}


static NvU32 GetDfpInfo(const NVConnectorEvoRec *pConnectorEvo)
{
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    NV0073_CTRL_DFP_GET_INFO_PARAMS params = { 0 };
    NvU32 ret;

    if (pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
        return 0x0;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_GET_INFO,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR, "Failed to query DFP info");
        return 0x0;
    }

    return params.flags;
}

typedef struct _AllocConnectorDispDataRec {
    NvU32 dfpIndex;
    NvU32 crtIndex;
    NvU32 typeIndices[NVKMS_CONNECTOR_TYPE_MAX + 1];
} AllocConnectorDispDataRec;

/*!
 * Query and setup information for a connector.
 */
static NvBool AllocConnector(
    NVDispEvoPtr pDispEvo,
    NVDpyId dpyId,
    AllocConnectorDispDataRec *pAllocConnectorDispData)
{
    NVConnectorEvoPtr pConnectorEvo = NULL;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS params = { 0 };
    NvU32 ret;
    NvBool isDP;

    pConnectorEvo = nvCalloc(1, sizeof(*pConnectorEvo));

    if (pConnectorEvo == NULL) {
        return FALSE;
    }

    pConnectorEvo->pDispEvo = pDispEvo;
    pConnectorEvo->displayId = dpyId;
    pConnectorEvo->type = NVKMS_CONNECTOR_TYPE_UNKNOWN;
    pConnectorEvo->physicalIndex = NV_INVALID_CONNECTOR_PHYSICAL_INFORMATION;
    pConnectorEvo->physicalLocation = NV_INVALID_CONNECTOR_PHYSICAL_INFORMATION;
    /* Query the output resource configuration */
    nvRmGetConnectorORInfo(pConnectorEvo, FALSE);

    isDP =
        (pConnectorEvo->or.type ==
         NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) &&
        (pConnectorEvo->or.protocol ==
         NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A ||
         pConnectorEvo->or.protocol ==
         NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B);

    /* Determine the connector type. */

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(dpyId);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_GET_CONNECTOR_DATA,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to determine connector type for connector "
                     NV_DPY_ID_PRINT_FORMAT, nvDpyIdToPrintFormat(dpyId));
        goto fail;
    } else {

        static const struct {
            NvU32 type0073;
            NvKmsConnectorType typeNvKms;
        } connectorTypeTable[] = {
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_EXT,
              NVKMS_CONNECTOR_TYPE_DP },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_USB_C,
              NVKMS_CONNECTOR_TYPE_USBC },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_INT,
              NVKMS_CONNECTOR_TYPE_DP },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_MINI_EXT,
              NVKMS_CONNECTOR_TYPE_DP },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DP_1,
              NVKMS_CONNECTOR_TYPE_DP },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DP_2,
              NVKMS_CONNECTOR_TYPE_DP },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_VGA_15_PIN,
              NVKMS_CONNECTOR_TYPE_VGA },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_I_TV_SVIDEO,
              NVKMS_CONNECTOR_TYPE_DVI_I },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_I_TV_COMPOSITE,
              NVKMS_CONNECTOR_TYPE_DVI_I },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_I,
              NVKMS_CONNECTOR_TYPE_DVI_I },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DVI_D,
              NVKMS_CONNECTOR_TYPE_DVI_D },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_ADC,
              NVKMS_CONNECTOR_TYPE_ADC },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DVI_I_1,
              NVKMS_CONNECTOR_TYPE_DVI_I },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_LFH_DVI_I_2,
              NVKMS_CONNECTOR_TYPE_DVI_I },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_SPWG,
              NVKMS_CONNECTOR_TYPE_LVDS },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_OEM,
              NVKMS_CONNECTOR_TYPE_LVDS },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_HDMI_A,
              NVKMS_CONNECTOR_TYPE_HDMI },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_HDMI_C_MINI,
              NVKMS_CONNECTOR_TYPE_HDMI },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_VIRTUAL_WFD,
              NVKMS_CONNECTOR_TYPE_UNKNOWN },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DSI,
              NVKMS_CONNECTOR_TYPE_DSI },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_DP_SERIALIZER,
              NVKMS_CONNECTOR_TYPE_DP_SERIALIZER },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_STEREO_3PIN_DIN,
              NVKMS_CONNECTOR_TYPE_UNKNOWN },
            { NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_UNKNOWN,
              NVKMS_CONNECTOR_TYPE_UNKNOWN },
        };

        int i, j;

        for (i = 0; i < params.count; i++) {
            for (j = 0; j < ARRAY_LEN(connectorTypeTable); j++) {
                if (connectorTypeTable[j].type0073 == params.data[i].type) {
                    if (pConnectorEvo->type == NVKMS_CONNECTOR_TYPE_UNKNOWN) {
                        pConnectorEvo->type = connectorTypeTable[j].typeNvKms;
                    } else {
                        /*
                         * The only cases where we should see
                         * params.count > 1 (and thus attempt to
                         * assign pConnectorEvo->type multiple times)
                         * should be where all the
                         * NV0073_CTRL_SPECIFIC_CONNECTOR_DATA_TYPE_*
                         * values map to the same NvKmsConnectorType;
                         */
                        nvAssert(pConnectorEvo->type ==
                                 connectorTypeTable[j].typeNvKms);
                    }
                    break;
                }
            }
            if (j == ARRAY_LEN(connectorTypeTable)) {
                nvAssert(!"Unhandled connector type!");
            }

            if (i == 0) {
                pConnectorEvo->physicalIndex = params.data[i].index;
                pConnectorEvo->physicalLocation = params.data[i].location;
            } else {
                nvAssert(pConnectorEvo->physicalIndex == params.data[i].index);
                nvAssert(pConnectorEvo->physicalLocation ==
                         params.data[i].location);
            }
        }

        pConnectorEvo->ddcPartnerDpyIdsList = nvNvU32ToDpyIdList(params.DDCPartners);
    }

    /* If the connector type is unknown, ignore this connector. */
    if (pConnectorEvo->type == NVKMS_CONNECTOR_TYPE_UNKNOWN) {
        nvFree(pConnectorEvo);
        return TRUE;
    }

    /*
     * Ignore connectors that use DP protocol, but don't have a
     * DP-compatible type.
     */
    if (isDP &&
        ((pConnectorEvo->type != NVKMS_CONNECTOR_TYPE_DP) &&
         !nvConnectorIsDPSerializer(pConnectorEvo) &&
         (pConnectorEvo->type != NVKMS_CONNECTOR_TYPE_USBC))) {
        nvFree(pConnectorEvo);
        return TRUE;
    }

    /*
     * Bind connector to the DP lib if DP capable. Serializer
     * connector is not managed by DP lib.
     */
    if (isDP &&
        !nvConnectorIsDPSerializer(pConnectorEvo)) {
        pConnectorEvo->pDpLibConnector = nvDPCreateConnector(pConnectorEvo);
        if (!pConnectorEvo->pDpLibConnector) {
            nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                         "Failed to initialize DisplayPort support for "
                         NV_DPY_ID_PRINT_FORMAT, nvDpyIdToPrintFormat(dpyId));
            goto fail;
        }
    }

    pConnectorEvo->signalFormat = GetSignalFormat(pConnectorEvo);

    pConnectorEvo->dfpInfo = GetDfpInfo(pConnectorEvo);

    /* Assign connector indices. */

    pConnectorEvo->legacyType =
        GetLegacyConnectorType(pDispEvo, pConnectorEvo->displayId);

    switch (pConnectorEvo->legacyType) {
        case NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT:
            pConnectorEvo->legacyTypeIndex =
                pAllocConnectorDispData->crtIndex++;
            break;
        case NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP:
            pConnectorEvo->legacyTypeIndex =
                pAllocConnectorDispData->dfpIndex++;
            break;
        default:
            nvAssert(!"Unknown connector type");
            break;
    }

    nvAssert(pConnectorEvo->type <
             ARRAY_LEN(pAllocConnectorDispData->typeIndices));
    pConnectorEvo->typeIndex =
        pAllocConnectorDispData->typeIndices[pConnectorEvo->type]++;

    nvListAppend(&pConnectorEvo->connectorListEntry, &pDispEvo->connectorList);

    nvkms_snprintf(pConnectorEvo->name, sizeof(pConnectorEvo->name), "%s-%u",
                   NvKmsConnectorTypeString(pConnectorEvo->type),
                   pConnectorEvo->typeIndex);

    return TRUE;

fail:
    nvFree(pConnectorEvo);
    return FALSE;
}


static void FreeConnectors(NVDispEvoPtr pDispEvo)
{
    NVConnectorEvoPtr pConnectorEvo, pConnectorEvoNext;

    nvListForEachEntry_safe(pConnectorEvo, pConnectorEvoNext,
                            &pDispEvo->connectorList, connectorListEntry) {
        // Unbind DP lib from the connector
        nvDPDestroyConnector(pConnectorEvo->pDpLibConnector);
        pConnectorEvo->pDpLibConnector = NULL;
        nvListDel(&pConnectorEvo->connectorListEntry);
        nvFree(pConnectorEvo);
    }
}


/*!
 * Allocate and initialize the connector structs for the given pDisp.
 *
 * NOTE: Each Display ID in pDispEvo->connectorIds (aka the
 * NV0073_CTRL_CMD_SYSTEM_GET_SUPPORTED mask) is a possible display
 * connection to the GPU which is static after boot.
 */
static NvBool AllocConnectors(NVDispEvoPtr pDispEvo)
{
    NVDpyId dpyId;
    NVConnectorEvoPtr pConnectorEvo;
    AllocConnectorDispDataRec allocConnectorDispData = { };

    nvAssert(nvListIsEmpty(&pDispEvo->connectorList));

    if (nvDpyIdListIsEmpty(pDispEvo->connectorIds)) {
        /* Allow boards with no connectors */
        return TRUE;
    }

    /* Allocate the connectors */
    FOR_ALL_DPY_IDS(dpyId, pDispEvo->connectorIds) {
        if (!AllocConnector(pDispEvo, dpyId, &allocConnectorDispData)) {
            goto fail;
        }
    }

    /*
     * Reassign pDispEvo->connectorIds, to exclude any connectors ignored above:
     * AllocConnector() may return TRUE but not actually create a pConnectorEvo
     * for some connectors reported by resman.
     */
    pDispEvo->connectorIds = nvEmptyDpyIdList();
    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        pDispEvo->connectorIds =
            nvAddDpyIdToDpyIdList(pConnectorEvo->displayId,
                                  pDispEvo->connectorIds);
    }

    pDispEvo->validDisplays = pDispEvo->connectorIds;

    return TRUE;

 fail:
    FreeConnectors(pDispEvo);
    return FALSE;
}

static NvBool IsFlexibleWindowMapping(NvU32 windowHeadMask)
{
    return (windowHeadMask ==
            NV0073_CTRL_SPECIFIC_FLEXIBLE_HEAD_WINDOW_ASSIGNMENT);
}

/*!
 * Query the number of heads and save the result in pDevEvo->numHeads.
 * Get window head assignment and save it in pDevEvo->headForWindow[win].
 *
 * Query the number of heads on each pDisp of the pDev and limit to
 * the minimum across all pDisps. Query the headMask on each pDisp and
 * take the intersection across pDisps. Query the window-head assignment
 * and if it is fully flexible, assign WINDOWs (2N) and (2N + 1) to HEAD N.
 * Otherwise, use the queried assignment.
 *
 * Limit the number of heads to the number of bits in the headMask. Ignore
 * the heads which don't have any windows assigned to them and heads which
 * create holes in the headMask. If a head which has assigned windows gets
 * pruned out, assign NV_INVALID_HEAD to those windows.
 *
 * \param[in,out] pDev   This is the device pointer; the pDisps within
 *                       it are used to query per-GPU information.
 *                       The result is written to pDevEvo->numHeads.
 *
 * \return               Return TRUE if numHeads are correctly queried and
 *                       window-head assignment is done.
 *                       Return FALSE if numHeads or window-head assignment
 *                       could not be queried.
 */
static NvBool ProbeHeadCountAndWindowAssignment(NVDevEvoPtr pDevEvo)
{
    NvU32 numHeads = 0, headMask = 0;
    NvU32 headsWithWindowsMask = 0;
    int sd, head, numBits;
    NVDispEvoPtr pDispEvo;
    NvBool first = TRUE;
    NvBool isFlexibleWindowMapping = NV_TRUE;
    NvU32 win;
    NvU32 ret;

    pDevEvo->numHeads = 0;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS numHeadsParams = { 0 };
        NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS headMaskParams = { 0 };
        NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS winHeadAssignParams = { };

        numHeadsParams.subDeviceInstance = sd;
        numHeadsParams.flags = 0;

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_SYSTEM_GET_NUM_HEADS,
                             &numHeadsParams, sizeof(numHeadsParams));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to get the number of heads");
            return FALSE;
        }

        if (numHeadsParams.numHeads == 0) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "No heads found on board!");
            return FALSE;
        }

        if (numHeadsParams.numHeads > NV_MAX_HEADS)
        {
            nvEvoLog(EVO_LOG_WARN,
                     "HW supports %d heads. Limiting to %d heads",
                     numHeadsParams.numHeads, NV_MAX_HEADS);

            numHeadsParams.numHeads = NV_MAX_HEADS;
        }

        if (numHeads == 0) {
            numHeads = numHeadsParams.numHeads;
        } else {
            if (numHeads != numHeadsParams.numHeads) {
                NvU32 minNumHeads =
                    NV_MIN(numHeads, numHeadsParams.numHeads);
                nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                            "Unexpected numbers of heads "
                            "(%d, %d); clamping to %d",
                            numHeads, numHeadsParams.numHeads, minNumHeads);
                numHeads = minNumHeads;
            }
        }

        headMaskParams.subDeviceInstance = sd;

        ret = nvRmApiControl(
                nvEvoGlobal.clientHandle,
                pDevEvo->displayCommonHandle,
                NV0073_CTRL_CMD_SPECIFIC_GET_ALL_HEAD_MASK,
                &headMaskParams, sizeof(headMaskParams));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to get head configuration");
            return FALSE;
        }

        if (headMask == 0) {
            headMask = headMaskParams.headMask;
        } else {
            if (headMask != headMaskParams.headMask) {
                NvU32 intersectedHeadMask =
                    headMask & headMaskParams.headMask;
                nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                            "Unexpected head configurations "
                            "(0x%02x, 0x%02x); limiting to 0x%02x",
                            headMask, headMaskParams.headMask,
                            intersectedHeadMask);
                headMask = intersectedHeadMask;
            }
        }

        winHeadAssignParams.subDeviceInstance = sd;
        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT,
                             &winHeadAssignParams, sizeof(winHeadAssignParams));

        if (ret == NVOS_STATUS_SUCCESS) {
            for (win = 0; win < NVKMS_MAX_WINDOWS_PER_DISP; win++) {
                NvU32 windowHeadMask = winHeadAssignParams.windowHeadMask[win];

                if ((win == 0) && first) {
                    isFlexibleWindowMapping = IsFlexibleWindowMapping(windowHeadMask);
                } else if (isFlexibleWindowMapping) {
                    /*
                     * Currently, if one window is completely flexible, then all are.
                     * In case of fully flexible window mapping, if windowHeadMask is
                     * zero for a window, then that window is not present in HW.
                     */
                    nvAssert(!windowHeadMask || (isFlexibleWindowMapping ==
                             IsFlexibleWindowMapping(windowHeadMask)));
                }

                /*
                 * For custom window mapping, if windowHeadMask is 0, then head
                 * is not assigned to this window. For flexible window mapping,
                 * if windowHeadMask is 0, then the window is not present in HW.
                 */
                if (windowHeadMask == 0) {
                    continue;
                }

                if (isFlexibleWindowMapping) {
                    /*
                     * TODO: For now assign WINDOWs (2N) and (2N + 1) to HEAD N when
                     * completely flexible window assignment is specified by window
                     * head assignment mask.
                     */
                    head = win >> 1;
                    windowHeadMask = NVBIT_TYPE(head, NvU8);
                    nvAssert(head < numHeads);
                } else {
                    // We don't support same window assigned to multiple heads.
                    nvAssert(ONEBITSET(windowHeadMask));

                    head = BIT_IDX_32(windowHeadMask);
                }

                if (first) {
                    pDevEvo->headForWindow[win] = head;
                    headsWithWindowsMask |= windowHeadMask;
                } else {
                    nvAssert(pDevEvo->headForWindow[win] == head);
                }
            }
        } else if (ret != NVOS_STATUS_ERROR_NOT_SUPPORTED) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                             "Failed to get window-head assignment");
            return FALSE;
        } else {
            // Pre-Volta, we don't need to populate pDevEvo->headForWindow[] and
            // each HW head has a window assigned.
            headsWithWindowsMask = headMask;
        }

        if (first) {
            first = FALSE;
        }
    }

    /* Check whether heads which have windows assigned are actually present in HW */
    nvAssert(!(~headMask & headsWithWindowsMask));

    /* Intersect heads present in HW with heads which have windows assigned */
    headMask &= headsWithWindowsMask;

    /* clamp numHeads to the number of bits in headMask */

    numBits = nvPopCount32(headMask);

    /* for now, we only support headMask when it is tightly packed at 0 */

    for (head = 0; head < numBits; head++) {
        if ((headMask & (1 << head)) == 0) {
            NvU32 modifiedHeadMask = (1 << head) - 1;

            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "The head configuration (0x%02x) "
                        "is unexpected; limiting to 0x%02x", headMask,
                   modifiedHeadMask);

            headMask = modifiedHeadMask;
            numBits = head;
            break;
        }
    }

    /* headMask should never increase numHeads */

    if (numBits > numHeads) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                    "The head configuration (0x%02x) "
                    "is inconsistent with the number of heads (%d)",
                    headMask, numHeads);
    } else if (numBits < numHeads) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                    "Correcting number of heads for "
                    "current head configuration (0x%02x)", headMask);
        numHeads = numBits;
    }

    pDevEvo->numHeads = numHeads;

    /*
     * If a head which has assigned windows gets pruned out, assign
     * NV_INVALID_HEAD to those windows.
     */
    for (win = 0; win < NVKMS_MAX_WINDOWS_PER_DISP; win++) {
        if ((pDevEvo->headForWindow[win] == NV_INVALID_HEAD) ||
            (pDevEvo->headForWindow[win] < pDevEvo->numHeads)) {
            continue;
        }
        pDevEvo->headForWindow[win] = NV_INVALID_HEAD;
    }

    return TRUE;
}

/*!
 * Set a pConnectorEvo's software state based on the boot head assignment.
 */
static void MarkConnectorBootHeadActive(NVDispEvoPtr pDispEvo, NvU32 head)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVDpyId displayId, rootPortId;
    NVConnectorEvoPtr pConnectorEvo;
    NVDispHeadStateEvoPtr pHeadState;
    NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS params = { 0 };
    NvU32 ret;

    // Use the first displayId in the boot display list.
    //
    // TODO: What should we do if more than one dpy ID is listed for a boot
    // display?
    nvAssert(nvCountDpyIdsInDpyIdList(pDispEvo->vbiosDpyConfig[head]) == 1);
    displayId = nvNextDpyIdInDpyIdListUnsorted(nvInvalidDpyId(),
                                              pDispEvo->vbiosDpyConfig[head]);

    // The displayId reported by RM could be a dynamic one.  Find the root port
    // for this ID.
    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(displayId);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        return;
    }

    if (params.bIsDispDynamic) {
        rootPortId = nvNvU32ToDpyId(params.rootPortId);
    } else {
        rootPortId = displayId;
    }

    pConnectorEvo = nvGetConnectorFromDisp(pDispEvo, rootPortId);
    if (!pConnectorEvo) {
        return;
    }

    if (NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
            NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED)) {

        nvAssert(params.index != NV_INVALID_OR);
        if (params.index == NV_INVALID_OR) {
            // If RM reported that a head is driving this dpyId, then there
            // should be an SOR assigned.  However, due to a bug in the way
            // PDB_PROP_GPU_DISABLE_VGA_CONSOLE_RESTORATION_ON_RESUME is
            // handled, RM can report an "active" head with no SOR assigned on
            // certain specific GPUs.  If that happens, just treat the head as
            // disabled.  See bug 1692425.
            pDispEvo->vbiosDpyConfig[head] = nvEmptyDpyIdList();
            return;
        } else {
            // Track the SOR assignment for this connector.  See the comment in
            // nvRmGetConnectorORInfo() for why this is deferred until now.
            nvAssert(pConnectorEvo->or.primary == NV_INVALID_OR);
            pConnectorEvo->or.primary = params.index;
        }
    }
    nvAssert(pConnectorEvo->or.primary == params.index);

    pHeadState = &pDispEvo->headState[head];

    nvAssert(!nvHeadIsActive(pDispEvo, head));

    pHeadState->pConnectorEvo = pConnectorEvo;
    pHeadState->activeRmId = nvDpyIdToNvU32(displayId);

    // Track the assigned head.
    pConnectorEvo->or.ownerHeadMask[params.index] |= NVBIT(head);

    nvEvoStateStartNoLock(&pDispEvo->pDevEvo->gpus[pDispEvo->displayOwner]);
}

/*!
 * Query the vbios assignment of heads to display devices, and cache
 * in pDispEvo->vbiosDpyConfig for later use by nvDPResume().
 *
 * \param[in,out] pDisp  This is the GPU display pointer; the result is
 *                       written to pDispEvo->vbiosDpyConfig
 */
static void GetVbiosHeadAssignmentOneDisp(NVDispEvoPtr pDispEvo)
{
    unsigned int head;
    NvU32 ret = NVOS_STATUS_ERROR_GENERIC;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    nvkms_memset(&pDispEvo->vbiosDpyConfig, 0,
                 sizeof(pDispEvo->vbiosDpyConfig));

    /* if there is no display, there is no origDpyConfig */

    nvAssert(pDevEvo->displayCommonHandle != 0);

    /*
     * get the vbios assignment of heads within the GPU, so that
     * later when we do head assignment, we can try to preserve the
     * existing assignment; see bug 208072
     */

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS activeDpysParams = { 0 };

        activeDpysParams.subDeviceInstance = pDispEvo->displayOwner;
        activeDpysParams.head = head;
        /*
         * We want to check for active displays set by any low-level software
         * such as VBIOS, not just those set by an RM client
         */
        activeDpysParams.flags =
            DRF_DEF(0073, _CTRL_SYSTEM_GET_ACTIVE_FLAGS, _CLIENT, _DISABLE);

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_SYSTEM_GET_ACTIVE,
                             &activeDpysParams, sizeof(activeDpysParams));

        if (ret == NVOS_STATUS_SUCCESS) {
            // XXX TODO: If this is a dynamic display ID, it's not necessarily
            // correlated with the NVDpyId we'll assign to a dynamic pDpyEvo
            // later.  We should instead store this as an NvU32 and assign it as
            // the activeRmId for a dynamic pDpyEvo that DPLib reports as being
            // driven by the firmware group.  See bug 1656584.
            pDispEvo->vbiosDpyConfig[head] =
                nvNvU32ToDpyIdList(activeDpysParams.displayId);
            if (activeDpysParams.displayId != 0) {
                MarkConnectorBootHeadActive(pDispEvo, head);
            }
        }

        nvAssert(ret == NVOS_STATUS_SUCCESS);
    }
}

static void GetVbiosHeadAssignment(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        GetVbiosHeadAssignmentOneDisp(pDispEvo);
    }
}

/*!
 * Query the boot display device(s).
 */
static void ProbeBootDisplays(NVDispEvoPtr pDispEvo)
{
    NvU32 ret;
    NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS bootParams = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    pDispEvo->bootDisplays = nvEmptyDpyIdList();

    bootParams.subDeviceInstance = pDispEvo->displayOwner;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SYSTEM_GET_BOOT_DISPLAYS,
                         &bootParams, sizeof(bootParams));

    if (ret == NVOS_STATUS_SUCCESS) {
        pDispEvo->bootDisplays =
            nvNvU32ToDpyIdList(bootParams.bootDisplayMask);
    }
}

/*!
 * Query the 0073 display common object capabilities.
 */
static NvBool ProbeDisplayCommonCaps(NVDevEvoPtr pDevEvo)
{
    NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS capsParams = { };
    NvU32 ret;

    ct_assert(sizeof(pDevEvo->commonCapsBits) == sizeof(capsParams.capsTbl));
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SYSTEM_GET_CAPS_V2,
                         &capsParams, sizeof(capsParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to determine display common capabilities");
        return FALSE;
    }
    nvkms_memcpy(pDevEvo->commonCapsBits, capsParams.capsTbl,
                 sizeof(pDevEvo->commonCapsBits));

    return TRUE;
}


/*!
 * Query the variable refresh rate (G-SYNC) capability of a display.
 */
static void ProbeVRRCaps(NVDispEvoPtr pDispEvo)
{
    NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT_PARAMS params = { 0 };
    NvU32 ret;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         nvEvoGlobal.clientHandle,
                         NV0000_CTRL_SYSTEM_GET_VRR_COOKIE_PRESENT,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        return;
    }

    pDispEvo->vrr.hasPlatformCookie = params.bIsPresent;

}


static NvBool ReadDPCDReg(NVConnectorEvoPtr pConnectorEvo,
                          NvU32 dpcdAddr,
                          NvU8 *dpcdData)
{
    NV0073_CTRL_DP_AUXCH_CTRL_PARAMS params = { };
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;

    params.subDeviceInstance = pConnectorEvo->pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);

    params.cmd = DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_TYPE, _AUX);
    params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_REQ_TYPE, _READ);

    params.addr = dpcdAddr;

    /* Requested size is 0-based */
    params.size = 0;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->displayCommonHandle,
                       NV0073_CTRL_CMD_DP_AUXCH_CTRL,
                       &params, sizeof(params)) != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "AUX read failed for DPCD addr 0x%x",
                    dpcdAddr);
        return FALSE;
    }

    if (params.size != 1U) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "AUX read returned 0 bytes for DPCD addr 0x%x",
                    dpcdAddr);
        return FALSE;
    }

    *dpcdData = params.data[0];

    return TRUE;
}

NvBool nvWriteDPCDReg(NVConnectorEvoPtr pConnectorEvo,
                      NvU32 dpcdAddr,
                      NvU8 dpcdData)
{
    NV0073_CTRL_DP_AUXCH_CTRL_PARAMS params = { };
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;

    params.subDeviceInstance = pConnectorEvo->pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);

    params.cmd = DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_TYPE, _AUX);
    params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_REQ_TYPE, _WRITE);

    params.addr = dpcdAddr;
    params.data[0] = dpcdData;

    /* Requested size is 0-based */
    params.size = 0;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->displayCommonHandle,
                       NV0073_CTRL_CMD_DP_AUXCH_CTRL,
                       &params, sizeof(params)) != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "AUX write failed for DPCD addr 0x%x",
                    dpcdAddr);
        return FALSE;
    }

    if (params.size != 1U) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Wrote 0 bytes for DPCD addr 0x%x",
                    dpcdAddr);
        return FALSE;
    }

    return TRUE;
}

static NvBool ReadDPSerializerCaps(NVConnectorEvoPtr pConnectorEvo)
{
    NVDpyIdList oneDpyIdList =
        nvAddDpyIdToEmptyDpyIdList(pConnectorEvo->displayId);
    NVDpyIdList connectedList;
    NvU8 dpcdData = 0;

    /*
     * This call will not only confirm that the DP serializer is connected, but
     * will also power on the corresponding DPAUX pads if the serializer is
     * detected via NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE. The DPAUX pads
     * need to be enabled for the DPCD reads below.
     */
    connectedList = nvRmGetConnectedDpys(pConnectorEvo->pDispEvo, oneDpyIdList);
    if (!nvDpyIdIsInDpyIdList(pConnectorEvo->displayId, connectedList)) {
        nvEvoLogDev(pConnectorEvo->pDispEvo->pDevEvo, EVO_LOG_ERROR,
                    "Serializer connector %s is not currently connected!",
                    pConnectorEvo->name);
        return FALSE;
    }

    if (!ReadDPCDReg(pConnectorEvo, NV_DPCD_MAX_LINK_BANDWIDTH, &dpcdData)) {
        return FALSE;
    }
    pConnectorEvo->dpSerializerCaps.maxLinkBW =
        DRF_VAL(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, dpcdData);

    if (!ReadDPCDReg(pConnectorEvo, NV_DPCD_MAX_LANE_COUNT, &dpcdData)) {
        return FALSE;
    }
    pConnectorEvo->dpSerializerCaps.maxLaneCount =
        DRF_VAL(_DPCD, _MAX_LANE_COUNT, _LANE, dpcdData);

    if (!ReadDPCDReg(pConnectorEvo, NV_DPCD_MSTM, &dpcdData)) {
        return FALSE;
    }
    pConnectorEvo->dpSerializerCaps.supportsMST =
        FLD_TEST_DRF(_DPCD, _MSTM, _CAP, _YES, dpcdData);

    return TRUE;
}

static NvBool AllocDPSerializerDpys(NVConnectorEvoPtr pConnectorEvo)
{
    NvBool supportsMST;
    NvU32 numHeads;
    NvU32 i;

    if (!nvConnectorIsDPSerializer(pConnectorEvo)) {
        return TRUE;
    }

    if (!ReadDPSerializerCaps(pConnectorEvo)) {
        return FALSE;
    }

    supportsMST = pConnectorEvo->dpSerializerCaps.supportsMST;
    numHeads = pConnectorEvo->pDispEvo->pDevEvo->numHeads;
    for (i = 0; i < numHeads && supportsMST; i++) {
        NVDpyEvoPtr pDpyEvo = NULL;
        NvBool dynamicDpyCreated = FALSE;
        char address[5] = { };

        nvkms_snprintf(address, sizeof(address), "0.%d", i + 1);
        pDpyEvo = nvGetDPMSTDpyEvo(pConnectorEvo, address,
                                   &dynamicDpyCreated);
        if ((pDpyEvo == NULL) || !dynamicDpyCreated) {
            return FALSE;
        }

        pDpyEvo->dp.serializerStreamIndex = i;
    }

    return TRUE;
}

/*!
 *
 */
static NvBool AllocDpys(NVDispEvoPtr pDispEvo)
{
    NVConnectorEvoPtr pConnectorEvo;

    // At this point, there should be no DisplayPort multistream devices.
    nvAssert(nvDpyIdListsAreEqual(pDispEvo->validDisplays,
                                  pDispEvo->connectorIds));
    nvAssert(nvDpyIdListIsEmpty(pDispEvo->displayPortMSTIds));
    nvAssert(nvDpyIdListIsEmpty(pDispEvo->dynamicDpyIds));

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        NVDpyEvoPtr pDpyEvo;

        pDpyEvo = nvAllocDpyEvo(pDispEvo, pConnectorEvo,
                                pConnectorEvo->displayId, NULL);

        if (pDpyEvo == NULL) {
            nvAssert(!"Failed to allocate pDpy");
            return FALSE;
        }

        if (!AllocDPSerializerDpys(pConnectorEvo)) {
            nvAssert(!"Failed to allocate non DPLib managed dpys");
            return FALSE;
        }
    }

    return TRUE;
}

static void FreeDpys(NVDispEvoPtr pDispEvo)
{
    NVDpyEvoPtr pDpyEvo, pDpyEvoTmp;

    nvListForEachEntry_safe(pDpyEvo, pDpyEvoTmp,
                            &pDispEvo->dpyList, dpyListEntry) {
        nvFreeDpyEvo(pDispEvo, pDpyEvo);
    }
}


/*!
 * Receive hotplug notification from resman.
 *
 * This function is registered as the kernel callback function from
 * resman when an NV2080_NOTIFIERS_HOTPLUG event is generated.
 *
 * However, this function is called with resman's context (resman locks held,
 * etc).  Schedule deferred work, so that we can process the hotplug event
 * without resman's encumbrances.
 */
static void ReceiveHotplugEvent(void *arg, void *pEventDataVoid, NvU32 hEvent,
                                NvU32 Data, NV_STATUS Status)
{
    (void) nvkms_alloc_timer_with_ref_ptr(
        nvHandleHotplugEventDeferredWork, /* callback */
        arg, /* argument (this is a ref_ptr to a pDispEvo) */
        0,   /* dataU32 */
        0);
}

static void ReceiveDPIRQEvent(void *arg, void *pEventDataVoid, NvU32 hEvent,
                              NvU32 Data, NV_STATUS Status)
{
    // XXX The displayId of the connector that generated the event should be
    // available here somewhere.  We should figure out how to find that and
    // plumb it through to nvHandleDPIRQEventDeferredWork.
    (void) nvkms_alloc_timer_with_ref_ptr(
        nvHandleDPIRQEventDeferredWork, /* callback */
        arg, /* argument (this is a ref_ptr to a pDispEvo) */
        0,   /* dataU32 */
        0);
}

NvBool nvRmRegisterCallback(const NVDevEvoRec *pDevEvo,
                            NVOS10_EVENT_KERNEL_CALLBACK_EX *cb,
                            struct nvkms_ref_ptr *ref_ptr,
                            NvU32 parentHandle,
                            NvU32 eventHandle,
                            Callback5ArgVoidReturn func,
                            NvU32 event)
{
    NV0005_ALLOC_PARAMETERS allocEventParams = { 0 };

    cb->func = func;
    cb->arg = ref_ptr;

    allocEventParams.hParentClient = nvEvoGlobal.clientHandle;
    allocEventParams.hClass        = NV01_EVENT_KERNEL_CALLBACK_EX;
    allocEventParams.notifyIndex   = event;
    allocEventParams.data          = NV_PTR_TO_NvP64(cb);

    return nvRmApiAlloc(nvEvoGlobal.clientHandle,
                        parentHandle,
                        eventHandle,
                        NV01_EVENT_KERNEL_CALLBACK_EX,
                        &allocEventParams)
        == NVOS_STATUS_SUCCESS;
}

static NvBool RegisterDispCallback(NVOS10_EVENT_KERNEL_CALLBACK_EX *cb,
                                   NVDispEvoPtr pDispEvo,
                                   NvU32 handle,
                                   Callback5ArgVoidReturn func,
                                   NvU32 event)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 subDevice = pDevEvo->pSubDevices[pDispEvo->displayOwner]->handle;

    return nvRmRegisterCallback(pDevEvo, cb, pDispEvo->ref_ptr, subDevice,
                                handle, func, event);
}

static void
DifrPrefetchEventDeferredWork(void *dataPtr, NvU32 dataU32)
{
    NVDevEvoPtr pDevEvo = dataPtr;
    size_t l2CacheSize = (size_t)dataU32;
    NvU32 status;

    nvAssert(pDevEvo->pDifrState);

    status = nvDIFRPrefetchSurfaces(pDevEvo->pDifrState, l2CacheSize);
    nvDIFRSendPrefetchResponse(pDevEvo->pDifrState, status);
}

static void DifrPrefetchEvent(void *arg, void *pEventDataVoid,
                              NvU32 hEvent, NvU32 Data, NV_STATUS Status)
{
    Nv2080LpwrDifrPrefetchNotification *notif =
        (Nv2080LpwrDifrPrefetchNotification *)pEventDataVoid;

    (void)nvkms_alloc_timer_with_ref_ptr(
        DifrPrefetchEventDeferredWork, /* callback */
        arg, /* argument (this is a ref_ptr to a pDevEvo) */
        notif->l2CacheSize, /* dataU32 */
        0);  /* timeout: schedule the work immediately */
}

enum NvKmsAllocDeviceStatus nvRmAllocDisplays(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    unsigned int sd;
    enum NvKmsAllocDeviceStatus status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
    NvU32 totalDispNumSubDevices = 0;

    pDevEvo->sli.bridge.present = FALSE;

    if (!QueryGpuCapabilities(pDevEvo)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to query GPU capabilities");
        goto fail;
    }

    if (pDevEvo->supportsSyncpts) {
        pDevEvo->preSyncptTable =
            nvCalloc(1, sizeof(NVEvoSyncpt) * NV_SYNCPT_GLOBAL_TABLE_LENGTH);
        if (pDevEvo->preSyncptTable == NULL) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to allocate memory for pre-syncpt table");
            goto fail;
        }
    }

    if (!AllocDisplays(pDevEvo)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to allocate displays");
        goto fail;
    }

    /* allocate the display common object for this device */

    if (nvRmEvoClassListCheck(pDevEvo, NV04_DISPLAY_COMMON)) {

        pDevEvo->displayCommonHandle =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        if (nvRmApiAlloc(nvEvoGlobal.clientHandle,
                         pDevEvo->deviceHandle,
                         pDevEvo->displayCommonHandle,
                         NV04_DISPLAY_COMMON, NULL)
                != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to initialize the display "
                        "subsystem for the NVIDIA graphics device!");
            goto fail;

        }
    } else {
        /*
         * Not supporting NV04_DISPLAY_COMMON is expected in some
         * configurations: e.g., GF117 (an Optimus-only or "coproc" GPU),
         * emulation netlists.  Fail with "no hardware".
         */
        status = NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
        goto fail;
    }

    if (!ProbeDisplayCommonCaps(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
        goto fail;
    }

    if (!ProbeHeadCountAndWindowAssignment(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
        goto fail;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        if (!ProbeValidDisplays(pDispEvo)) {
            status = NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
            goto fail;
        }

        /* Keep track of connectors per pDisp and bind to DP lib if capable */
        if (!AllocConnectors(pDispEvo)) {
            status = NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
            goto fail;
        }
    }

    /*
     * If there are no valid display devices, fail with "no hardware".
     */
    if (NoValidDisplays(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
        goto fail;
    }

    /*
     * The number of numSubDevices across disps should equal the
     * device's numSubDevices.
     */
    totalDispNumSubDevices = 0;
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        totalDispNumSubDevices++;
    }

    if (totalDispNumSubDevices != pDevEvo->numSubDevices) {
        nvAssert(!"Number of disps' subdevices does not match device's");
    }

    /*
     * Allocate an NV event for each pDispEvo on the corresponding
     * subDevice, tied to the pDevEvo's OS event.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS setEventParams = { };
        NvU32 subDevice, ret;

        subDevice = pDevEvo->pSubDevices[pDispEvo->displayOwner]->handle;

        pDispEvo->hotplugEventHandle =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        if (!RegisterDispCallback(&pDispEvo->rmHotplugCallback, pDispEvo,
                                  pDispEvo->hotplugEventHandle,
                                  ReceiveHotplugEvent,
                                  NV2080_NOTIFIERS_HOTPLUG)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "Failed to register display hotplug event");
        }

        // Enable hotplug notifications from this subdevice.
        setEventParams.event = NV2080_NOTIFIERS_HOTPLUG;
        setEventParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT;
        if ((ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                                  subDevice,
                                  NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                                  &setEventParams,
                                  sizeof(setEventParams)))
                != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "Failed to register display hotplug "
                        "handler: 0x%x\n", ret);
        }
    }

    // Allocate a handler for the DisplayPort "IRQ" event, which is signaled
    // when there's a short interruption in the hotplug detect line.
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS setEventParams = { };
        NvU32 subDevice, ret;

        subDevice = pDevEvo->pSubDevices[pDispEvo->displayOwner]->handle;

        pDispEvo->DPIRQEventHandle =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        if (!RegisterDispCallback(&pDispEvo->rmDPIRQCallback, pDispEvo,
                                  pDispEvo->DPIRQEventHandle, ReceiveDPIRQEvent,
                                  NV2080_NOTIFIERS_DP_IRQ)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "Failed to register DisplayPort interrupt event");
        }

        // Enable DP IRQ notifications from this subdevice.
        setEventParams.event = NV2080_NOTIFIERS_DP_IRQ;
        setEventParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT;
        if ((ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                                  subDevice,
                                  NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                                  &setEventParams,
                                  sizeof(setEventParams)))
                != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "Failed to register DisplayPort interrupt "
                        "handler: 0x%x\n", ret);
        }
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        ProbeBootDisplays(pDispEvo);

        if (!AllocDpys(pDispEvo)) {
            goto fail;
        }

        ProbeVRRCaps(pDispEvo);
    }

    nvAllocVrrEvo(pDevEvo);

    return NVKMS_ALLOC_DEVICE_STATUS_SUCCESS;

fail:
    nvRmDestroyDisplays(pDevEvo);
    return status;
}


void nvRmDestroyDisplays(NVDevEvoPtr pDevEvo)
{
    NvU32 ret;
    NVDispEvoPtr pDispEvo;
    int dispIndex;
    NvS64 tmp;

    nvFreeVrrEvo(pDevEvo);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {

        // Before freeing anything, dump anything left in the RM's DisplayPort
        // AUX channel log.
        if (pDispEvo->dpAuxLoggingEnabled) {
            do {
                ret = nvRmQueryDpAuxLog(pDispEvo, &tmp);
            } while (ret && tmp);
        }

        // Free the DisplayPort IRQ event.
        if (pDispEvo->DPIRQEventHandle != 0) {
            nvRmApiFree(nvEvoGlobal.clientHandle,
                        nvEvoGlobal.clientHandle,
                        pDispEvo->DPIRQEventHandle);
            nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                               pDispEvo->DPIRQEventHandle);
            pDispEvo->DPIRQEventHandle = 0;
        }

        // Free the hotplug event.
        /*
         * XXX I wish I could cancel anything scheduled by
         * ReceiveHotplugEvent() and ReceiveDPIRQEvent() for this pDispEvo...
         */
        if (pDispEvo->hotplugEventHandle != 0) {
            nvRmApiFree(nvEvoGlobal.clientHandle,
                        nvEvoGlobal.clientHandle,
                        pDispEvo->hotplugEventHandle);
            nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                               pDispEvo->hotplugEventHandle);
            pDispEvo->hotplugEventHandle = 0;
        }
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        FreeDpys(pDispEvo);
        FreeConnectors(pDispEvo);
    }

    FreeDisplays(pDevEvo);

    nvFree(pDevEvo->preSyncptTable);
    pDevEvo->preSyncptTable = NULL;

    if (pDevEvo->displayCommonHandle != 0) {
        ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                          pDevEvo->deviceHandle,
                          pDevEvo->displayCommonHandle);
        if (ret != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Free(displayCommonHandle) failed");
        }
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDevEvo->displayCommonHandle);
        pDevEvo->displayCommonHandle = 0;
    }
}


/*!
 * The Allocate a display ID that we use to talk to RM about the dpy(s) on
 * head.
 *
 * \param[in]  pDisp      The display system on which to allocate the ID.
 * \param[in]  dpyList    The list of dpys.
 *
 * \return  The display ID, or 0 on failure.
 */
NvU32 nvRmAllocDisplayId(const NVDispEvoRec *pDispEvo, const NVDpyIdList dpyList)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS params = { 0 };
    const NVDpyEvoRec *pDpyEvo;
    const NVConnectorEvoRec *pConnectorEvo = NULL;
    NvBool isDPMST = NV_FALSE;
    NvU32 ret;

    FOR_ALL_EVO_DPYS(pDpyEvo, dpyList, pDispEvo) {
        if (pConnectorEvo == NULL) {
            /* First DPY from list, assign pConnectorEvo and isDPMST variable */
            pConnectorEvo = pDpyEvo->pConnectorEvo;
            isDPMST = nvDpyEvoIsDPMST(pDpyEvo);
        }

        if (pConnectorEvo != pDpyEvo->pConnectorEvo ||
            isDPMST != nvDpyEvoIsDPMST(pDpyEvo)) {
            return 0;
        }
    }

    nvAssert(nvConnectorUsesDPLib(pConnectorEvo) || !isDPMST);

    if (!isDPMST) {
        /* For non-MST dpy(s), simply return static display ID of connector */
        return nvDpyIdToNvU32(pConnectorEvo->displayId);
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID,
                         &params, sizeof(params));

    if (ret == NVOS_STATUS_SUCCESS) {
        return params.displayIdAssigned;
    } else {
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                     "Failed to allocate display resource.");
    }

    return 0;
}


/*!
 * Send DISPLAY_CHANGE to resman.
 *
 * This should be called before and after each mode change, with the display
 * mask describing the NEW display configuration.
 */
void nvRmBeginEndModeset(NVDispEvoPtr pDispEvo,
                         enum NvKmsBeginEndModeset beginOrEnd,
                         NvU32 mask)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS bracketParams = { };
    NvU32 ret;

    bracketParams.subDeviceInstance = pDispEvo->displayOwner;
    bracketParams.newDevices = mask;
    bracketParams.properties = 0; /* this is currently unused */
    switch (beginOrEnd) {
        case BEGIN_MODESET:
            bracketParams.enable = NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_START;
            break;
        case END_MODESET:
            bracketParams.enable = NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_END;
            break;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_DISPLAY_CHANGE,
                         &bracketParams,
                         sizeof(bracketParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Failed NV0073_CTRL_CMD_SPECIFIC_DISPLAY_CHANGE");
    }
}


/*!
 * Free a RM display ID, if it was allocated dynamically.
 *
 * This function frees a display ID if it was allocated by
 * nvRmAllocDisplayId.  If the display ID is static, this function does
 * nothing.
 *
 * From ctrl0073dp.h: You must not call this function while either the ARM
 * or ASSEMBLY state cache refers to this display-id.  The head must not be
 * attached.
 *
 * \param[in]  pDisp      The display system on which to free the ID.
 * \param[in]  displayId  The display ID to free.
 */
void nvRmFreeDisplayId(const NVDispEvoRec *pDispEvo, NvU32 displayId)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS params = { 0 };
    NVDpyId dpyId = nvNvU32ToDpyId(displayId);
    NvU32 ret;

    /* Do nothing if display ID is static one! */
    if (nvDpyIdIsInDpyIdList(dpyId, pDispEvo->connectorIds)) {
        return;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = displayId;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to relinquish display resource.");
    }
}


/*!
 * Query Resman for the (broad) display device type.
 */
static NvU32 GetLegacyConnectorType(NVDispEvoPtr pDispEvo, NVDpyId dpyId)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS params = { 0 };
    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(dpyId);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_GET_TYPE,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failure getting specific display device type.");
        return NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_UNKNOWN;
    }

    nvAssert((params.displayType == NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT) ||
             (params.displayType == NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP));

    return params.displayType;
}


/*!
 * Query RM for the current OR properties of the given connector.
 *
 * If 'assertOnly' is TRUE, this function will only assert that the OR
 * configuration has not changed.
 */
void nvRmGetConnectorORInfo(NVConnectorEvoPtr pConnectorEvo, NvBool assertOnly)
{
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS params = { 0 };
    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SPECIFIC_OR_GET_INFO,
                         &params,
                         sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed to determine output resource properties.");

        if (assertOnly) {
            return;
        }
        pConnectorEvo->or.type = NV0073_CTRL_SPECIFIC_OR_TYPE_DAC;
        pConnectorEvo->or.primary = NV_INVALID_OR;
        pConnectorEvo->or.secondaryMask = 0;
        pConnectorEvo->or.protocol =
            NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DAC_RGB_CRT;
        pConnectorEvo->or.ditherType = NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF;
        pConnectorEvo->or.ditherAlgo =
            NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_UNKNOWN;
        pConnectorEvo->or.location = NV0073_CTRL_SPECIFIC_OR_LOCATION_CHIP;

        return;
    }

    if (!assertOnly) {
        pConnectorEvo->or.type = params.type;
        if (NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED) &&
            params.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
            // For the SOR crossbar, RM may report that multiple displayIDs own
            // the same SOR.  For example, it may report SOR 2 for both the
            // DisplayPort and TMDS halves of a physical connector even though
            // they have separate displayIds.
            //
            // All we really need to know is which SOR is assigned to the boot
            // display, so we defer the query to MarkConnectorBootHeadActive().
            pConnectorEvo->or.secondaryMask = 0x0;
            pConnectorEvo->or.primary = NV_INVALID_OR;
        } else {
            pConnectorEvo->or.secondaryMask = 0x0;
            pConnectorEvo->or.primary = params.index;
        }
        pConnectorEvo->or.protocol = params.protocol;
        pConnectorEvo->or.ditherType = params.ditherType;
        pConnectorEvo->or.ditherAlgo = params.ditherAlgo;
        pConnectorEvo->or.location = params.location;
    } else {
        nvAssert(pConnectorEvo->or.type == params.type);
        nvAssert(pConnectorEvo->or.primary == params.index);
        nvAssert(pConnectorEvo->or.protocol == params.protocol);
        nvAssert(pConnectorEvo->or.ditherType == params.ditherType);
        nvAssert(pConnectorEvo->or.ditherAlgo == params.ditherAlgo);
        nvAssert(pConnectorEvo->or.location == params.location);
    }
}

/*!
 * Query connector state, and retry if necessary.
 */
NVDpyIdList nvRmGetConnectedDpys(const NVDispEvoRec *pDispEvo,
                                 NVDpyIdList dpyIdList)
{
    NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS params = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayMask = nvDpyIdListToNvU32(dpyIdList);
    params.flags =
        (DRF_DEF(0073_CTRL_SYSTEM,_GET_CONNECT_STATE_FLAGS,_METHOD,_DEFAULT) |
         DRF_DEF(0073_CTRL_SYSTEM,_GET_CONNECT_STATE_FLAGS,_DDC,_DEFAULT) |
         DRF_DEF(0073_CTRL_SYSTEM,_GET_CONNECT_STATE_FLAGS,_LOAD,_DEFAULT));

    do {
        params.retryTimeMs = 0;
        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_SYSTEM_GET_CONNECT_STATE,
                             &params,
                             sizeof(params));

        if (ret == NVOS_STATUS_ERROR_NOT_READY &&
            params.retryTimeMs == 0) {
            // Work around bug 970351: RM returns a zero retry time on platforms
            // where the display driver is in user space.  Use a conservative
            // default.  This code can be removed once this call is fixed in RM.
            params.retryTimeMs = 20;
        }

        if (params.retryTimeMs > 0) {
            nvkms_usleep(params.retryTimeMs * 1000);
        } else {
            nvkms_yield();
        }
    } while(params.retryTimeMs > 0);

    if (ret == NVOS_STATUS_SUCCESS) {
        return nvNvU32ToDpyIdList(params.displayMask);
    } else {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                     "Failed detecting connected display devices");
        return nvEmptyDpyIdList();
    }
}

/*!
 * Notify the DP library that we are ready to proceed after a suspend/boot, and
 * that it should initialize and start handling events.
 */
NvBool nvRmResumeDP(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    int i;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, i, pDevEvo) {
        NVConnectorEvoPtr pConnectorEvo;
        NVDpyIdList connectedIdsList =
            nvRmGetConnectedDpys(pDispEvo, pDispEvo->connectorIds);

        FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
            NvBool plugged =
                nvDpyIdIsInDpyIdList(pConnectorEvo->displayId, connectedIdsList);

            if (!pConnectorEvo->pDpLibConnector) {
                continue;
            }

            if (!nvDPResume(pConnectorEvo->pDpLibConnector, plugged)) {
                goto failed;
            }
        }
    }

    return TRUE;

failed:
    nvRmPauseDP(pDevEvo);
    return FALSE;
}


void nvRmPauseDP(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    int i;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, i, pDevEvo) {
        NVConnectorEvoPtr pConnectorEvo;

        FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
            if (nvConnectorUsesDPLib(pConnectorEvo)) {
                nvDPPause(pConnectorEvo->pDpLibConnector);
            }
        }
    }
}


/*!
 * This function is called whenever the DPMS level changes; On a CRT,
 * you set the DPMS level by (dis/en)abling the hsync and vsync
 * signals:
 *
 * Hsync  Vsync  Mode
 * =====  =====  ====
 * 1      1      Normal (on).
 * 0      1      Standby -- RGB guns off, power supply on, tube filaments
 *               energized, (screen saver mode).
 * 1      0      Suspend -- RGB guns off, power supply off, tube filaments
 *               energized.
 * 0      0      Power off -- small auxiliary circuit stays on to monitor the
 *               hsync/vsync signals to know when to wake up.
 */
NvBool nvRmSetDpmsEvo(NVDpyEvoPtr pDpyEvo, NvS64 value)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    if (nvDpyUsesDPLib(pDpyEvo)) {
        nvDPDeviceSetPowerState(pDpyEvo,
                                (value == NV_KMS_DPY_ATTRIBUTE_DPMS_ON));
        return TRUE;
    } else if (pDpyEvo->pConnectorEvo->legacyType !=
               NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT) {
        NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS powerParams = { 0 };

        powerParams.subDeviceInstance = pDispEvo->displayOwner;
        powerParams.displayId = nvDpyEvoGetConnectorId(pDpyEvo);

        powerParams.powerState = (value == NV_KMS_DPY_ATTRIBUTE_DPMS_ON) ?
            NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_ON :
            NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_OFF;

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_SPECIFIC_SET_MONITOR_POWER,
                             &powerParams,
                             sizeof(powerParams));

        return (ret == NVOS_STATUS_SUCCESS);
    } else {
        NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;
        NV5070_CTRL_CMD_SET_DAC_PWR_PARAMS powerParams = { { 0 }, 0 };

        powerParams.base.subdeviceIndex = pDispEvo->displayOwner;
        if (pConnectorEvo->or.primary == NV_INVALID_OR) {
            nvAssert(pConnectorEvo->or.primary != NV_INVALID_OR);
            return FALSE;
        }
        powerParams.orNumber = pConnectorEvo->or.primary;

        switch (value) {
        case NV_KMS_DPY_ATTRIBUTE_DPMS_ON:
            powerParams.normalHSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_HSYNC, _ENABLE);
            powerParams.normalVSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_VSYNC, _ENABLE);
            break;
        case NV_KMS_DPY_ATTRIBUTE_DPMS_STANDBY:
            powerParams.normalHSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_HSYNC, _LO);
            powerParams.normalVSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_VSYNC, _ENABLE);
            break;
        case NV_KMS_DPY_ATTRIBUTE_DPMS_SUSPEND:
            powerParams.normalHSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_HSYNC, _ENABLE);
            powerParams.normalVSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_VSYNC, _LO);
            break;
        case NV_KMS_DPY_ATTRIBUTE_DPMS_OFF:
            powerParams.normalHSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_HSYNC, _LO);
            powerParams.normalVSync =
                DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_VSYNC, _LO);
            break;
        default:
            return FALSE;
        }
        // XXX These could probably be disabled too, in the DPMS_OFF case.
        powerParams.normalData =
            DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_DATA, _ENABLE);
        powerParams.normalPower =
            DRF_DEF(5070, _CTRL_CMD_SET_DAC, _PWR_NORMAL_PWR, _ON);

        powerParams.flags =
            DRF_DEF(5070, _CTRL_CMD_SET_DAC_PWR_FLAGS, _SPECIFIED_NORMAL, _YES);

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayHandle,
                             NV5070_CTRL_CMD_SET_DAC_PWR,
                             &powerParams,
                             sizeof(powerParams));

        return (ret == NVOS_STATUS_SUCCESS);
    }
}


NvBool nvRmAllocSysmem(NVDevEvoPtr pDevEvo, NvU32 memoryHandle,
                       NvU32 *ctxDmaFlags, void **ppBase, NvU64 size,
                       NvKmsMemoryIsoType isoType)
{
    NvU32 ret;
    NvBool bufferAllocated = FALSE;
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };
    const NvKmsDispIOCoherencyModes *pIOCoherencyModes;

    memAllocParams.owner = NVKMS_RM_HEAP_ID;

    memAllocParams.attr2 = DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _NO);

    memAllocParams.size = size;

    if (isoType == NVKMS_MEMORY_NISO) {
        memAllocParams.attr2 |= DRF_DEF(OS32, _ATTR2, _NISO_DISPLAY, _YES);

        pIOCoherencyModes = &pDevEvo->nisoIOCoherencyModes;
    } else {
        pIOCoherencyModes = &pDevEvo->isoIOCoherencyModes;
    }

    memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _PCI) |
                          DRF_DEF(OS32, _ATTR, _PHYSICALITY, _NONCONTIGUOUS) |
                          DRF_DEF(OS32, _ATTR, _FORMAT, _PITCH);

    if (pIOCoherencyModes->noncoherent) {
        // Model (3)
        // - allocate USWC system memory
        // - allocate ctx dma with NVOS03_FLAGS_CACHE_SNOOP_DISABLE
        // - to sync CPU and GPU, flush CPU WC buffer

        memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_COMBINE,
                                          memAllocParams.attr);

        ret = nvRmApiAlloc(
                  nvEvoGlobal.clientHandle,
                  pDevEvo->deviceHandle,
                  memoryHandle,
                  NV01_MEMORY_SYSTEM,
                  &memAllocParams);

        if (ret == NVOS_STATUS_SUCCESS) {
            bufferAllocated = TRUE;
            if (ctxDmaFlags) {
                *ctxDmaFlags |= DRF_DEF(OS03, _FLAGS, _CACHE_SNOOP, _DISABLE);
            }
        } else {
            bufferAllocated = FALSE;
        }

    }

    if (!bufferAllocated && pIOCoherencyModes->coherent) {
        // Model (2b): Similar to existing PCI model
        // - allocate cached (or USWC) system memory
        // - allocate ctx DMA with NVOS03_FLAGS_CACHE_SNOOP_ENABLE
        // ...

        memAllocParams.attr = FLD_SET_DRF(OS32, _ATTR, _COHERENCY, _WRITE_BACK,
                                          memAllocParams.attr);

        ret = nvRmApiAlloc(
                  nvEvoGlobal.clientHandle,
                  pDevEvo->deviceHandle,
                  memoryHandle,
                  NV01_MEMORY_SYSTEM,
                  &memAllocParams);

        if (ret == NVOS_STATUS_SUCCESS) {
            bufferAllocated = TRUE;
            if (ctxDmaFlags) {
                *ctxDmaFlags |= DRF_DEF(OS03, _FLAGS, _CACHE_SNOOP, _ENABLE);
            }
        } else {
            bufferAllocated = FALSE;
        }
    }

    if (bufferAllocated) {
        ret = nvRmApiMapMemory(
                  nvEvoGlobal.clientHandle,
                  pDevEvo->deviceHandle,
                  memoryHandle,
                  0, /* offset */
                  size,
                  ppBase,
                  0 /* flags */);

        if (ret != NVOS_STATUS_SUCCESS) {
            nvRmApiFree(nvEvoGlobal.clientHandle,
                        pDevEvo->deviceHandle,
                        memoryHandle);

            bufferAllocated = FALSE;
        }
    }

    return bufferAllocated;
}


/*****************************************************************************/
/* Alloc memory and a context dma, following the rules dictated by the
   DMA coherence flags. */
/*****************************************************************************/

NvBool nvRmAllocEvoDma(NVDevEvoPtr pDevEvo, NVEvoDmaPtr pDma,
                       NvU64 limit, NvU32 ctxDmaFlags, NvU32 subDeviceMask)
{
    NvBool bufferAllocated = FALSE;
    NvU32  memoryHandle = 0;
    void  *pBase = NULL;

    NvBool needBar1Mapping = FALSE;

    NVSurfaceDescriptor surfaceDesc;
    NvU32 localCtxDmaFlags = ctxDmaFlags |
        DRF_DEF(OS03, _FLAGS, _ACCESS, _READ_WRITE) |
        DRF_DEF(OS03, _FLAGS, _HASH_TABLE, _DISABLE);

    NvU32  ret;

    nvkms_memset(pDma, 0, sizeof(*pDma));

    memoryHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    /*
     * On certain GPUs (GF100, GF104) there exists a hardware bug that forces
     * us to put display NISO surfaces (pushbuffer, semaphores, notifiers
     * accessed by EVO) in vidmem instead of sysmem.  See bug 632241 for
     * details.
     */
    if (NV5070_CTRL_SYSTEM_GET_CAP(pDevEvo->capsBits,
            NV5070_CTRL_SYSTEM_CAPS_BUG_644815_DNISO_VIDMEM_ONLY)) {
        NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };

        memAllocParams.owner = NVKMS_RM_HEAP_ID;
        memAllocParams.type = NVOS32_TYPE_DMA;
        memAllocParams.size = limit + 1;
        memAllocParams.attr = DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB) |
                              DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);

        ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                           pDevEvo->deviceHandle,
                           memoryHandle,
                           NV01_MEMORY_LOCAL_USER,
                           &memAllocParams);

        if (ret != NVOS_STATUS_SUCCESS) {
            /* We can't fall back to any of the sysmem options below, due to
             * the nature of the HW bug forcing us to use vidmem. */
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Unable to allocate video memory for display");
            return FALSE;
        }

        limit = memAllocParams.size - 1;

        /* We'll access these surfaces through IFB */
        pBase = NULL;

        bufferAllocated = TRUE;
        needBar1Mapping = TRUE;
    }

    if (!bufferAllocated) {
        /*
         * Setting NVKMS_MEMORY_NISO since nvRmAllocEvoDma() is currently only
         * called to allocate pushbuffer and notifier memory.
         */
        bufferAllocated = nvRmAllocSysmem(pDevEvo, memoryHandle,
                                          &localCtxDmaFlags, &pBase, limit + 1,
                                          NVKMS_MEMORY_NISO);
    }

    if (!bufferAllocated) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, memoryHandle);

        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Unable to allocate DMA memory");

        return FALSE;
    }

    // Create surface descriptor for this allocation.
    ret = pDevEvo->hal->AllocSurfaceDescriptor(pDevEvo, &surfaceDesc, memoryHandle,
                                               localCtxDmaFlags, limit);

    if (ret != NVOS_STATUS_SUCCESS) {
        if (pBase != NULL) {
            nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                               pDevEvo->deviceHandle,
                               memoryHandle,
                               pBase,
                               0);
        }
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle, memoryHandle);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, memoryHandle);

        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to allocate surface descriptor");

        return FALSE;
    }

    pDma->memoryHandle = memoryHandle;

    pDma->surfaceDesc = surfaceDesc;

    pDma->limit = limit;

    if (needBar1Mapping) {
        NvBool result;

        result = nvRmEvoMapVideoMemory(pDevEvo, memoryHandle, limit + 1,
                                       pDma->subDeviceAddress, subDeviceMask);

        if (!result) {
            nvRmFreeEvoDma(pDevEvo, pDma);
            return FALSE;
        }
    } else {
        int sd;

        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            if (((1 << sd) & subDeviceMask) == 0) {
                continue;
            }

            pDma->subDeviceAddress[sd] = pBase;
        }
    }
    pDma->isBar1Mapping = needBar1Mapping;

    return TRUE;
}

void nvRmFreeEvoDma(NVDevEvoPtr pDevEvo, NVEvoDmaPtr pDma)
{
    NvU32 ret;

    pDevEvo->hal->FreeSurfaceDescriptor(pDevEvo,
                                        pDevEvo->deviceHandle,
                                        &pDma->surfaceDesc);

    if (pDma->memoryHandle != 0) {
        if (pDma->isBar1Mapping) {
            nvRmEvoUnMapVideoMemory(pDevEvo, pDma->memoryHandle,
                                    pDma->subDeviceAddress);
        } else {
            int sd = 0;
            NvBool addressMapped = TRUE;

            /* If pDma->subDeviceAddress[sd] is non-NULL for multiple subdevices,
             * assume they are the same. Unmap only one but set all of them to
             * NULL. This matches the logic in nvRmAllocEvoDma().
             */
            for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

                if (addressMapped && pDma->subDeviceAddress[sd] != NULL) {
                    ret = nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                                             pDevEvo->deviceHandle,
                                             pDma->memoryHandle,
                                             pDma->subDeviceAddress[sd],
                                             0);

                    if (ret != NVOS_STATUS_SUCCESS) {
                        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to unmap memory");
                    }

                    addressMapped = FALSE;
                }

                pDma->subDeviceAddress[sd] = NULL;
            }
        }

        ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                          pDevEvo->deviceHandle, pDma->memoryHandle);

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to free DMA memory");
        }

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pDma->memoryHandle);
        pDma->memoryHandle = 0;

        pDma->limit = 0;

        nvkms_memset(pDma->subDeviceAddress, 0, sizeof(pDma->subDeviceAddress));
    }
}

/*****************************************************************************/
/* RmAllocEvoChannel ()
 * Allocates the EVO channel and associated notifier surfaces and ctxdmas.
 * Takes how big the DMA controls are (varies by class of channel) and which
 * class to allocate.
 */
/*****************************************************************************/
static NVEvoChannelPtr
RmAllocEvoChannel(NVDevEvoPtr pDevEvo,
                  NVEvoChannelMask channelMask,
                  NvV32 instance, NvU32 class)
{
    NVEvoChannelPtr pChannel = NULL;
    NVDmaBufferEvoPtr buffer = NULL;
    int sd;
    NvU32 ret;

    /* One 4k page is enough to map PUT and GET */
    const NvU64 dmaControlLen = 0x1000;

    nvAssert(NV_EVO_CHANNEL_MASK_POPCOUNT(channelMask) == 1);

    /* Allocate the channel data structure */
    pChannel = nvCalloc(1, sizeof(*pChannel));

    if (pChannel == NULL) {
        goto fail;
    }

    buffer = &pChannel->pb;

    pChannel->hwclass = class;
    pChannel->instance = instance;
    pChannel->channelMask = channelMask;

    pChannel->notifiersDma = nvCalloc(pDevEvo->numSubDevices, sizeof(NVEvoDma));

    if (pChannel->notifiersDma == NULL) {
        goto fail;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NVEvoDmaPtr pNotifiersDma = &pChannel->notifiersDma[sd];

        void *pDmaDisplayChannel = NULL;

        // Allocation of the notifiers
        if (!nvRmAllocEvoDma(pDevEvo, pNotifiersDma,
                             NV_DMA_EVO_NOTIFIER_SIZE - 1,
                             DRF_DEF(OS03, _FLAGS, _TYPE, _NOTIFIER),
                             1 << sd)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Notifier DMA allocation failed");

            goto fail;
        }

        nvAssert(pNotifiersDma->subDeviceAddress[sd] != NULL);

        // Only allocate memory for one pushbuffer.
        // All subdevices will share (via subdevice mask)
        if (sd == 0) {
            NV50VAIO_CHANNELDMA_ALLOCATION_PARAMETERS ChannelAllocParams = { 0 };

            NvU64 limit = NV_DMA_EVO_PUSH_BUFFER_SIZE - 1;
            NVEvoDmaPtr pDma = &buffer->dma;

            // Allocation of the push buffer
            if (!nvRmAllocEvoDma(pDevEvo, pDma, limit, 0, SUBDEVICE_MASK_ALL)) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                            "Display engine push buffer DMA allocation failed");

                goto fail;
            }

            if (!pDma->isBar1Mapping) {
                buffer->base = pDma->subDeviceAddress[0];
            } else {
                /*
                 * Allocate memory for a shadow copy in sysmem that we'll copy
                 * to vidmem via BAR1 at kickoff time.
                 */
                buffer->base = nvCalloc(buffer->dma.limit + 1, 1);
                if (buffer->base == NULL) {
                    goto fail;
                }
            }

            buffer->channel_handle =
                nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

            // Channel instance (always 0 for CORE - head number otherwise)
            ChannelAllocParams.channelInstance = instance;
            // PB CtxDMA Handle
            ChannelAllocParams.hObjectBuffer   = buffer->dma.surfaceDesc.ctxDmaHandle;
            // Initial offset within the PB
            ChannelAllocParams.offset          = 0;

            ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                               pDevEvo->displayHandle,
                               buffer->channel_handle,
                               class,
                               &ChannelAllocParams);
            if (ret != NVOS_STATUS_SUCCESS) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                            "Display engine push buffer channel allocation failed: 0x%x (%s)",
                            ret, nvstatusToString(ret));

                nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                                   buffer->channel_handle);
                buffer->channel_handle = 0;

                goto fail;
            }
        }

        ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                               pDevEvo->pSubDevices[sd]->handle,
                               buffer->channel_handle,
                               0,
                               dmaControlLen,
                               &pDmaDisplayChannel,
                               0);
        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Display engine push buffer DMA mapping failed: 0x%x (%s)",
                        ret, nvstatusToString(ret));
            goto fail;
        }

        buffer->control[sd] = pDmaDisplayChannel;
    }

    /* Initialize the rest of the required push buffer information */
    buffer->buffer          = buffer->base;
    buffer->end             = (NvU32 *)((char *)buffer->base +
                              NV_DMA_EVO_PUSH_BUFFER_SIZE - 8);

    /*
     * Due to hardware bug 235044, we can not use the last 12 dwords of the
     * core channel pushbuffer.  Adjust offset_max appropriately.
     *
     * This bug is fixed in Volta and newer, so this workaround can be removed
     * when Pascal support is dropped. See bug 3116066.
     */
    buffer->offset_max   = NV_DMA_EVO_PUSH_BUFFER_SIZE -
                           NV_DMA_EVO_PUSH_BUFFER_PAD_SIZE;
    buffer->fifo_free_count = (buffer->offset_max >> 2) - 2;
    buffer->put_offset   = 0;
    buffer->num_channels = pDevEvo->numSubDevices;
    buffer->pDevEvo      = pDevEvo;
    buffer->currentSubDevMask = SUBDEVICE_MASK_ALL;

    pChannel->imm.type = NV_EVO_IMM_CHANNEL_NONE;

    pDevEvo->hal->InitChannel(pDevEvo, pChannel);

    return pChannel;

fail:

    RmFreeEvoChannel(pDevEvo, pChannel);

    return NULL;
}

static void FreeImmediateChannelPio(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    NVEvoPioChannel *pPio = pChannel->imm.u.pio;
    int sd;

    nvAssert(pPio != NULL);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        if (!pPio->control[sd]) {
            continue;
        }

        if (nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                               pDevEvo->pSubDevices[sd]->handle,
                               pPio->handle,
                               pPio->control[sd],
                               0)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "Failed to unmap immediate channel");
        }
        pPio->control[sd] = NULL;
    }

    if (pPio->handle) {
        if (nvRmApiFree(nvEvoGlobal.clientHandle,
                        pDevEvo->displayHandle,
                        pPio->handle)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN, "Failed to free immediate channel");
        }
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pPio->handle);
        pPio->handle = 0;
    }

    nvFree(pPio);
    pChannel->imm.u.pio = NULL;
}

static void FreeImmediateChannelDma(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    NVEvoChannelPtr pImmChannel = pChannel->imm.u.dma;

    RmFreeEvoChannel(pDevEvo, pImmChannel);
    pChannel->imm.u.dma = NULL;
}

static void FreeImmediateChannel(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    switch (pChannel->imm.type) {
        case NV_EVO_IMM_CHANNEL_NONE:
            return;
        case NV_EVO_IMM_CHANNEL_PIO:
            FreeImmediateChannelPio(pDevEvo, pChannel);
            break;
        case NV_EVO_IMM_CHANNEL_DMA:
            FreeImmediateChannelDma(pDevEvo, pChannel);
            break;
    }
    pChannel->imm.type = NV_EVO_IMM_CHANNEL_NONE;
}

/*****************************************************************************/
/* RmFreeEvoChannel ()
 * Frees all of the stuff allocated in RmAllocEvoChannel */
/*****************************************************************************/
static void RmFreeEvoChannel(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    int sd;

    if (pChannel == NULL) {
        return;
    }

    FreeImmediateChannel(pDevEvo, pChannel);

    if (pChannel->completionNotifierEventHandle != 0) {

        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pChannel->pb.channel_handle,
                    pChannel->completionNotifierEventHandle);

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pChannel->completionNotifierEventHandle);

        pChannel->completionNotifierEventHandle = 0;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (pChannel->pb.control[sd]) {
            if (nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                                   pDevEvo->pSubDevices[sd]->handle,
                                   pChannel->pb.channel_handle,
                                   pChannel->pb.control[sd],
                                   0) != NVOS_STATUS_SUCCESS) {
                nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                                 "Failed to unmap display engine channel memory");
            }
            pChannel->pb.control[sd] = NULL;
        }
    }

    if (pChannel->pb.channel_handle != 0) {
        // If NVKMS restored the console successfully, tell RM to leave the
        // channels allocated to avoid shutting down the heads we just
        // enabled.
        //
        // On EVO, only leave the core and base channels allocated. The
        // other satellite channels shouldn't be active at the console.
        //
        // On nvdisplay, one or more window channels are also needed. Rather
        // than try to figure out which ones are needed, just leave them all
        // alone.
        const NvBool isCore =
            FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                           pChannel->channelMask);
        const NvBool isBase =
            (pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL) != 0;
        const NvBool isWindow =
            (pChannel->channelMask & NV_EVO_CHANNEL_MASK_WINDOW_ALL) != 0;
        if ((isCore || isBase || isWindow) && pDevEvo->skipConsoleRestore) {
            NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS params = { };

            params.base.subdeviceIndex = pDevEvo->vtFbInfo.subDeviceInstance;
            params.flags = NV5070_CTRL_SET_RMFREE_FLAGS_PRESERVE_HW;

            if (nvRmApiControl(nvEvoGlobal.clientHandle,
                               pDevEvo->displayHandle,
                               NV5070_CTRL_CMD_SET_RMFREE_FLAGS,
                               &params, sizeof(params))
                != NVOS_STATUS_SUCCESS) {
                nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                                 "Failed to set the PRESERVE_HW flag");
            }
        }

        if (nvRmApiFree(nvEvoGlobal.clientHandle,
                        pDevEvo->displayHandle,
                        pChannel->pb.channel_handle)
            != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to tear down display engine channel");
        }
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pChannel->pb.channel_handle);
        pChannel->pb.channel_handle = 0;
    }

    if (pChannel->pb.dma.isBar1Mapping) {
        /* Pushbuffer is in vidmem. Free shadow copy. */
        nvFree(pChannel->pb.base);
        pChannel->pb.base = NULL;
    }

    nvRmFreeEvoDma(pDevEvo, &pChannel->pb.dma);

    if (pChannel->notifiersDma) {
        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            nvRmFreeEvoDma(pDevEvo, &pChannel->notifiersDma[sd]);
        }
    }

    nvFree(pChannel->notifiersDma);
    pChannel->notifiersDma = NULL;

    nvFree(pChannel);
}

static NvBool
AllocImmediateChannelPio(NVDevEvoPtr pDevEvo,
                         NVEvoChannelPtr pChannel,
                         NvU32 class,
                         NvU32 instance,
                         NvU32 mapSize)
{
    NVEvoPioChannel *pPio = NULL;
    NvU32 handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    NV50VAIO_CHANNELPIO_ALLOCATION_PARAMETERS params = { 0 };
    NvU32 sd;

    pPio = nvCalloc(1, sizeof(*pPio));

    if (!pPio) {
        return FALSE;
    }

    pChannel->imm.type = NV_EVO_IMM_CHANNEL_PIO;
    pChannel->imm.u.pio = pPio;

    params.channelInstance = instance;

    if (nvRmApiAlloc(nvEvoGlobal.clientHandle,
                     pDevEvo->displayHandle,
                     handle,
                     class,
                     &params) != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to allocate immediate channel %d", instance);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, handle);
        return FALSE;
    }

    pPio->handle = handle;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        void *pImm = NULL;

        if (nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                             pDevEvo->pSubDevices[sd]->handle,
                             pPio->handle,
                             0,
                             mapSize,
                             &pImm,
                             0) != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to map immediate channel %d/%d",
                        sd, instance);
            return FALSE;
        }

        pPio->control[sd] = pImm;
    }

    return TRUE;
}

static NvBool
AllocImmediateChannelDma(NVDevEvoPtr pDevEvo,
                         NVEvoChannelPtr pChannel,
                         NvU32 immClass)
{
    NVEvoChannelPtr pImmChannel = RmAllocEvoChannel(
        pDevEvo,
        DRF_DEF64(_EVO, _CHANNEL_MASK, _WINDOW_IMM, _ENABLE),
        pChannel->instance, immClass);

    if (!pImmChannel) {
        return FALSE;
    }

    pChannel->imm.type = NV_EVO_IMM_CHANNEL_DMA;
    pChannel->imm.u.dma = pImmChannel;

    return TRUE;
}

NvBool nvRMAllocateBaseChannels(NVDevEvoPtr pDevEvo)
{
    int i;
    NvU32 baseClass = 0;
    NvU32 head;

    static const NvU32 baseChannelDmaClasses[] = {
        NV927C_BASE_CHANNEL_DMA,
    };

    for (i = 0; i < ARRAY_LEN(baseChannelDmaClasses); i++) {
        if (nvRmEvoClassListCheck(pDevEvo, baseChannelDmaClasses[i])) {
            baseClass = baseChannelDmaClasses[i];
            break;
        }
    }

    if (!baseClass) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Unsupported base display class");
        return FALSE;
    }

    for (head = 0; head < pDevEvo->numHeads; head++) {
        pDevEvo->base[head] = RmAllocEvoChannel(
            pDevEvo,
            DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _BASE, head, _ENABLE),
            head, baseClass);

        if (!pDevEvo->base[head]) {
            return FALSE;
        }
    }

    return TRUE;
}

NvBool nvRMAllocateOverlayChannels(NVDevEvoPtr pDevEvo)
{
    NvU32 immMapSize;
    NvU32 head;

    if (!nvRmEvoClassListCheck(pDevEvo,
                               NV917E_OVERLAY_CHANNEL_DMA)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Unsupported overlay display class");
        return FALSE;
    }

    nvAssert(nvRmEvoClassListCheck(pDevEvo, NV917B_OVERLAY_IMM_CHANNEL_PIO));

    /*
     * EvoSetImmPointOut91() will interpret the PIO mapping as a pointer
     * to GK104DispOverlayImmControlPio and access the SetPointOut and
     * Update fields, which is safe as long as SetPointOut and Update are
     * at consistent offsets.
     */
    nvAssert(offsetof(GK104DispOverlayImmControlPio, SetPointsOut) ==
             NV917B_SET_POINTS_OUT(NVKMS_LEFT));
    nvAssert(offsetof(GK104DispOverlayImmControlPio, Update) ==
             NV917B_UPDATE);
    immMapSize =
        NV_MAX(NV917B_SET_POINTS_OUT(NVKMS_LEFT), NV917B_UPDATE) + sizeof(NvV32);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        pDevEvo->overlay[head] = RmAllocEvoChannel(
            pDevEvo,
            DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _OVERLAY, head, _ENABLE),
            head, NV917E_OVERLAY_CHANNEL_DMA);

        if (!pDevEvo->overlay[head]) {
            return FALSE;
        }

        if (!AllocImmediateChannelPio(pDevEvo, pDevEvo->overlay[head],
                                      NV917B_OVERLAY_IMM_CHANNEL_PIO, head, immMapSize)) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * This allocates a syncpt per channel. This syncpt is dedicated
 * to this channel. As NVKMS only supports syncpoints for SOC devices,
 * in which there's only one device/sub-device/disp, sd can be 0.
 */
static NvBool AllocSyncpt(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel,
        NVEvoSyncpt *pEvoSyncptOut)
{
    NvU32 id;
    NvKmsSyncPtOpParams params = { };
    NvBool result;
    NVSurfaceDescriptor surfaceDesc;

    if (!pDevEvo->supportsSyncpts) {
        return FALSE;
    }

    /*! Set syncpt id to invalid to avoid un-intended Free */
    pEvoSyncptOut->id = NVKMS_SYNCPT_ID_INVALID;

    /*
     * HW engine on Orin is called HOST1X, all syncpts are in internal RAM of
     * HOST1X.
     * OP_ALLOC calls into HOST1X driver and allocs a syncpt resource.
     */
    params.alloc.syncpt_name = "nvkms-fence";
    result = nvkms_syncpt_op(NVKMS_SYNCPT_OP_ALLOC, &params);
    if (!result) {
        return FALSE;
    }
    id = params.alloc.id;

    /* Post syncpt max val is tracked locally. Init the value here. */
    params.read_minval.id = id;
    result = nvkms_syncpt_op(NVKMS_SYNCPT_OP_READ_MINVAL, &params);
    if (!result) {
        goto failed;
    }

    result = nvRmEvoAllocAndBindSyncpt(pDevEvo, pChannel, id,
                                       &surfaceDesc,
                                       pEvoSyncptOut);
    if (!result) {
        goto failed;
    }

    /*! Populate syncpt values to return. */
    pEvoSyncptOut->channelMask = pChannel->channelMask;
    pEvoSyncptOut->syncptMaxVal = params.read_minval.minval;

    return TRUE;

failed:
    /*! put back syncpt as operation failed */
    params.put.id = id;
    nvkms_syncpt_op(NVKMS_SYNCPT_OP_PUT, &params);
    return FALSE;
}

static NvBool AllocPostSyncptPerChannel(NVDevEvoPtr pDevEvo,
                                        NVEvoChannelPtr pChannel)
{
    if (!pDevEvo->supportsSyncpts) {
        return TRUE;
    }

    return AllocSyncpt(pDevEvo, pChannel, &pChannel->postSyncpt);
}

NvBool nvRMAllocateWindowChannels(NVDevEvoPtr pDevEvo)
{
    int index;
    NvU32 window, sd;

    static const struct {
        NvU32 windowClass;
        NvU32 immClass;
    } windowChannelClasses[] = {
        { NVC67E_WINDOW_CHANNEL_DMA,
          NVC67B_WINDOW_IMM_CHANNEL_DMA },
        { NVC57E_WINDOW_CHANNEL_DMA,
          NVC57B_WINDOW_IMM_CHANNEL_DMA },
        { NVC37E_WINDOW_CHANNEL_DMA,
          NVC37B_WINDOW_IMM_CHANNEL_DMA },
    }, *c = NULL;

    for (index = 0; index < ARRAY_LEN(windowChannelClasses); index++) {
        if (nvRmEvoClassListCheck(pDevEvo,
                    windowChannelClasses[index].windowClass)) {

            c = &windowChannelClasses[index];

            nvAssert(nvRmEvoClassListCheck(pDevEvo, c->immClass));
            break;
        }
    }

    if (index >= ARRAY_LEN(windowChannelClasses)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Unsupported window display class");
        return FALSE;
    }

    nvAssert(pDevEvo->numWindows <= ARRAY_LEN(pDevEvo->window));
    for (window = 0; window < pDevEvo->numWindows; window++) {
        pDevEvo->window[window] = RmAllocEvoChannel(
            pDevEvo,
            DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE),
            window, c->windowClass);

        if (!pDevEvo->window[window]) {
            return FALSE;
        }

        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            NvU32 ret = pDevEvo->hal->BindSurfaceDescriptor(
                pDevEvo,
                pDevEvo->window[window],
                &pDevEvo->window[window]->notifiersDma[sd].surfaceDesc);
            if (ret != NVOS_STATUS_SUCCESS) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to bind(window channel) display engine notify surface descriptor: 0x%x (%s)",
                        ret, nvstatusToString(ret));
                return FALSE;
            }
        }

        if (!AllocImmediateChannelDma(pDevEvo, pDevEvo->window[window],
                                      c->immClass)) {
            return FALSE;
        }

        if (!AllocPostSyncptPerChannel(pDevEvo,
                                       pDevEvo->window[window])) {
            return FALSE;
        }
    }

    return TRUE;
}

static void EvoFreeCoreChannel(NVDevEvoRec *pDevEvo, NVEvoChannel *pChannel)
{
    NvU32 sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NvU32 ret;

        if (!pDevEvo->pSubDevices[sd]->pCoreDma) {
            continue;
        }

        ret = nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                                 pDevEvo->pSubDevices[sd]->handle,
                                 pChannel->pb.channel_handle,
                                 pDevEvo->pSubDevices[sd]->pCoreDma,
                                 0);

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDevDebug(
                pDevEvo,
                EVO_LOG_ERROR,
                "Failed to unmap NVDisplay core channel memory mapping for ARMed values");
        }
        pDevEvo->pSubDevices[sd]->pCoreDma = NULL;
    }

    RmFreeEvoChannel(pDevEvo, pChannel);
}

static NVEvoChannel* EvoAllocateCoreChannel(NVDevEvoRec *pDevEvo)
{
    NVEvoChannel *pChannel;
    NvU32 sd;

    pChannel =
        RmAllocEvoChannel(pDevEvo,
                          DRF_DEF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE),
                          0,
                          pDevEvo->coreChannelDma.coreChannelClass);

    if (pChannel == NULL) {
        goto failed;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NvU32 ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                                     pDevEvo->pSubDevices[sd]->handle,
                                     pChannel->pb.channel_handle,
                                     pDevEvo->coreChannelDma.dmaArmedOffset,
                                     pDevEvo->coreChannelDma.dmaArmedSize,
                                     (void**)&pDevEvo->pSubDevices[sd]->pCoreDma,
                                     DRF_DEF(OS33, _FLAGS, _ACCESS, _READ_ONLY));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(
                pDevEvo,
                EVO_LOG_ERROR,
                "Core channel memory mapping for ARMed values failed: 0x%x (%s)",
                ret, nvstatusToString(ret));
            goto failed;
        }
    }

    return pChannel;

failed:
    if (pChannel != NULL) {
        EvoFreeCoreChannel(pDevEvo, pChannel);
    }
    return NULL;
}

/* Pre-allocate the vblank syncpts, store in NVDispHeadStateEvoRec. */
void nvRmAllocCoreRGSyncpts(NVDevEvoPtr pDevEvo)
{

    NVDispEvoPtr pDispEvo = NULL;
    NvU32 syncptIdx = 0;

    if (!pDevEvo->supportsSyncpts ||
        !pDevEvo->hal->caps.supportsVblankSyncObjects) {
        return;
    }

    /* If Syncpts are supported, we're on Orin, which only has one display. */
    nvAssert(pDevEvo->nDispEvo == 1);
    pDispEvo = pDevEvo->pDispEvo[0];

    /* Initialize all heads' vblank sync object counts to zero. */
    for (int i = 0; i < pDevEvo->numApiHeads; i++) {
        pDispEvo->apiHeadState[i].numVblankSyncObjectsCreated = 0;
    }

    /* For each core RG syncpt index: */
    for (syncptIdx = 0; syncptIdx < NVKMS_MAX_VBLANK_SYNC_OBJECTS_PER_HEAD;
         syncptIdx++) {
        /* For each Head: */
        for (int i = 0; i < pDevEvo->numApiHeads; i++) {
            NvBool result = FALSE;
            NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[i];

            result =
                AllocSyncpt(pDevEvo, pDevEvo->core,
                            &pApiHeadState->vblankSyncObjects[syncptIdx].evoSyncpt);
            if (!result) {
                /*
                 * Stop trying to allocate more syncpts if none are
                 * available.
                 */
                nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                             "Failed to allocate Core RG Syncpoint at index %d "
                             "on Head %d.", syncptIdx, i);
                return;
            }

            /* Populate the index of the syncpt in the NVVblankSyncObjectRec. */
            pApiHeadState->vblankSyncObjects[syncptIdx].index = syncptIdx;
            /* Update the count. */
            pApiHeadState->numVblankSyncObjectsCreated = syncptIdx + 1;
        }
    }
}

NvBool nvRMSetupEvoCoreChannel(NVDevEvoPtr pDevEvo)
{
    NvU32 sd;

    pDevEvo->core = EvoAllocateCoreChannel(pDevEvo);
    if (!pDevEvo->core) {
        return FALSE;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        // Bind the core notifier surface descriptor
        NvU32 ret =
            pDevEvo->hal->BindSurfaceDescriptor(
                pDevEvo, pDevEvo->core,
                &pDevEvo->core->notifiersDma[sd].surfaceDesc);
        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to bind display engine notify surface descriptor: 0x%x (%s)",
                        ret, nvstatusToString(ret));
            nvRMFreeEvoCoreChannel(pDevEvo);
            return FALSE;
        }
    }

    nvInitEvoSubDevMask(pDevEvo);

    /*
     * XXX NVKMS TODO: Enable core channel event generation; see bug
     * 1671139.
     */

    // Query the VBIOS head assignments.  Note that this has to happen after the
    // core channel is allocated or else RM will return incorrect information
    // about dynamic display IDs it allocates for the boot display on DP MST
    // devices.
    GetVbiosHeadAssignment(pDevEvo);

    return TRUE;
}

void nvRMFreeBaseChannels(NVDevEvoPtr pDevEvo)
{
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        RmFreeEvoChannel(pDevEvo, pDevEvo->base[head]);
        pDevEvo->base[head] = NULL;
    }
}

void nvRMFreeOverlayChannels(NVDevEvoPtr pDevEvo)
{
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        RmFreeEvoChannel(pDevEvo, pDevEvo->overlay[head]);
        pDevEvo->overlay[head] = NULL;
    }
}

void nvRMFreeWindowChannels(NVDevEvoPtr pDevEvo)
{
    NvU32 window;

    for (window = 0; window < pDevEvo->numWindows; window++) {
        nvRmEvoFreeSyncpt(pDevEvo, &pDevEvo->window[window]->postSyncpt);
        RmFreeEvoChannel(pDevEvo, pDevEvo->window[window]);
        pDevEvo->window[window] = NULL;
    }
}

/* Frees the Core RG Syncpts. */
void nvRmFreeCoreRGSyncpts(NVDevEvoPtr pDevEvo)
{

    NVDispEvoPtr pDispEvo = NULL;

    if (!pDevEvo->supportsSyncpts ||
        !pDevEvo->hal->caps.supportsVblankSyncObjects) {
        return;
    }

    /* We can get here in teardown cases from alloc failures */
    if (pDevEvo->nDispEvo == 0) {
        return;
    }

    /* If Syncpts are supported, we're on Orin, which only has one display. */
    nvAssert(pDevEvo->nDispEvo == 1);
    pDispEvo = pDevEvo->pDispEvo[0];

    /* For each Head: */
    for (int i = 0; i < pDevEvo->numApiHeads; i++) {
        /* Free all core RG syncpts. */
        NVDispApiHeadStateEvoRec *pApiHeadState = &pDispEvo->apiHeadState[i];
        for (int j = 0; j < pApiHeadState->numVblankSyncObjectsCreated; j++) {
            nvAssert(!pApiHeadState->vblankSyncObjects[j].inUse);
            nvRmEvoFreeSyncpt(pDevEvo,
                              &pApiHeadState->vblankSyncObjects[j].evoSyncpt);
        }
        pApiHeadState->numVblankSyncObjectsCreated = 0;
    }
}

void nvRMFreeEvoCoreChannel(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->core != NULL) {
        EvoFreeCoreChannel(pDevEvo, pDevEvo->core);
        pDevEvo->core = NULL;
    }
}

/* Poll for an EVO channel on a particular subdevice to process all its methods */
static NvBool SyncOneEvoChannel(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChan,
    NvU32 sd,
    NvU32 errorToken)
{
    NvBool isMethodPending;
    NvU64 startTime = 0;
    const NvU32 timeout = 2000000; // microseconds

    do {
        if (!pDevEvo->hal->IsChannelMethodPending(pDevEvo, pChan,
                                                  sd, &isMethodPending)) {
            return FALSE;
        }

        if (!isMethodPending) {
            break;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Idling display engine timed out: 0x%08x:%d:%d:%d",
                        pChan->hwclass, pChan->instance,
                        sd, errorToken);
            return FALSE;
        }

        nvkms_yield();

    } while (TRUE);

    return TRUE;
}

/* Sync an EVO channel on all subdevices */
NvBool nvRMSyncEvoChannel(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 errorToken)
{
    NvBool ret = TRUE;

    if (pChannel) {
        NvU32 sd;

        nvDmaKickoffEvo(pChannel);

        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            if (!SyncOneEvoChannel(pDevEvo, pChannel, sd, errorToken)) {
                ret = FALSE;
            }
        }
    }

    return ret;
}


/*
 * Wait for the requested base channel to be idle (no methods pending), and
 * call STOP_BASE if the wait times out.
 *
 * stoppedBase will be TRUE if calling STOP_BASE was necessary and
 * successful.
 */
NvBool nvRMIdleBaseChannel(NVDevEvoPtr pDevEvo, NvU32 head, NvU32 sd,
                           NvBool *stoppedBase)
{
    NVEvoChannelPtr pMainLayerChannel =
        pDevEvo->head[head].layer[NVKMS_MAIN_LAYER];
    NvU64 startTime = 0;
    NvBool idleTimedOut = FALSE;
    const NvU32 timeout = 2000000; // 2 seconds
    NvBool isMethodPending = TRUE;
    NvBool ret = TRUE;

    *stoppedBase = FALSE;

    do {
        if (!pDevEvo->hal->IsChannelMethodPending(pDevEvo,
                                                  pMainLayerChannel,
                                                  sd,
                                                  &isMethodPending)) {
            break;
        }

        if (!isMethodPending) {
            break;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
            idleTimedOut = TRUE;
            break;
        }

        nvkms_yield();

    } while (TRUE);

    if (idleTimedOut) {
        NVEvoIdleChannelState idleChannelState = { };

        idleChannelState.subdev[sd].channelMask |= pMainLayerChannel->channelMask;
        ret = pDevEvo->hal->ForceIdleSatelliteChannel(pDevEvo, &idleChannelState);

        *stoppedBase = ret;
    }

    return ret;
}


NvBool nvRmEvoClassListCheck(const NVDevEvoRec *pDevEvo, NvU32 classID)
{
    const NvU32 *classes = pDevEvo->supportedClasses;

    int i;

    nvAssert(pDevEvo->numClasses > 0);

    for (i = 0; i < pDevEvo->numClasses; i++) {
        if (classes[i] == classID) {
            return TRUE;
        }
    }

    return FALSE;
}

/*!
 * This API used to register syncpt object with RM.
 * It involves ->
 * 1. Allocate a new NV01_MEMORY_SYNCPOINT syncpt object.
 * 2. Allocate a new ctxdma descriptor for the syncpt object.
 * 3. Bind the ctxdma entry to the channel.
 */
NvBool nvRmEvoAllocAndBindSyncpt(
    NVDevEvoRec *pDevEvo,
    NVEvoChannel *pChannel,
    NvU32 id,
    NVSurfaceDescriptor *pSurfaceDesc,
    NVEvoSyncpt *pEvoSyncpt)
{
    return FALSE;
}

void nvRmFreeSyncptHandle(
    NVDevEvoRec *pDevEvo,
    NVEvoSyncpt *pSyncpt)
{
    nvRmApiFree(nvEvoGlobal.clientHandle,
                pDevEvo->deviceHandle,
                pSyncpt->hSyncpt);
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                       pSyncpt->hSyncpt);
    pSyncpt->hSyncpt = 0;

    pDevEvo->hal->FreeSurfaceDescriptor(pDevEvo,
                                        pDevEvo->deviceHandle,
                                        &pSyncpt->surfaceDesc);
    pSyncpt->allocated = FALSE;
}

/*!
 * This API used to unregister syncpt object with given channel.
 * It searches global table, and when finds that for given channel, syncpt
 * is registered, then frees it.
 */
void nvRmEvoFreePreSyncpt(
    NVDevEvoRec *pDevEvo,
    NVEvoChannel *pChannel)
{
    NvU32 i;
    NvBool isChannelIdle = NV_FALSE;

    if (pChannel == NULL) {
        return;
    }

    if (!pDevEvo->supportsSyncpts) {
        return;
    }

    if (pChannel->channelMask == 0) {
        return;
    }

    pDevEvo->hal->IsChannelIdle(
        pDevEvo, pChannel, 0, &isChannelIdle);

    if (isChannelIdle == NV_FALSE) {
        return;
    }

    /*! Find pre-syncpt and free it */
    for (i = 0; i < NV_SYNCPT_GLOBAL_TABLE_LENGTH; i++) {

        pDevEvo->preSyncptTable[i].channelMask &= ~pChannel->channelMask;
        if (pDevEvo->preSyncptTable[i].channelMask == 0 &&
            pDevEvo->preSyncptTable[i].allocated) {

            /*! Free handles */
            nvRmFreeSyncptHandle(pDevEvo, &pDevEvo->preSyncptTable[i]);
        }
    }
}

/*!
 * This API is used to unregister the given syncpt object.
 */
void nvRmEvoFreeSyncpt(
    NVDevEvoRec *pDevEvo,
    NVEvoSyncpt *pEvoSyncpt)
{
    if ((pEvoSyncpt == NULL) || !pDevEvo->supportsSyncpts ||
        (pEvoSyncpt->id == NVKMS_SYNCPT_ID_INVALID)) {
        return;
    }

    /*! Put reference of syncptid from nvhost */
    NvKmsSyncPtOpParams params = { };
    params.put.id = pEvoSyncpt->id;
    nvkms_syncpt_op(NVKMS_SYNCPT_OP_PUT, &params);

    /*! Free handles */
    nvRmFreeSyncptHandle(pDevEvo, pEvoSyncpt);
}

void nvRmEvoUnMapVideoMemory(NVDevEvoPtr pDevEvo, NvU32 memoryHandle,
                             void *subDeviceAddress[NVKMS_MAX_SUBDEVICES])
{
    unsigned int sd;
    NvU32 ret;

    if (memoryHandle == 0) {
        return;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (subDeviceAddress[sd] != NULL) {
            ret = nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                                     pDevEvo->pSubDevices[sd]->handle,
                                     memoryHandle,
                                     subDeviceAddress[sd],
                                     0);

            if (ret != NVOS_STATUS_SUCCESS) {
                nvAssert(!"UnmapMemory() failed");
            }
        }

        subDeviceAddress[sd] = NULL;
    }
}

NvBool nvRmEvoMapVideoMemory(NVDevEvoPtr pDevEvo,
                             NvU32 memoryHandle, NvU64 size,
                             void *subDeviceAddress[NVKMS_MAX_SUBDEVICES],
                             NvU32 subDeviceMask)
{
    NvU32 ret;

    unsigned int sd;

    nvkms_memset(subDeviceAddress, 0, sizeof(void*) * NVKMS_MAX_SUBDEVICES);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        void *address = NULL;

        if (((1 << sd) & subDeviceMask) == 0) {
            continue;
        }

        ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                               pDevEvo->pSubDevices[sd]->handle,
                               memoryHandle,
                               0,
                               size,
                               &address,
                               0);

        if (ret != NVOS_STATUS_SUCCESS) {
            nvRmEvoUnMapVideoMemory(pDevEvo, memoryHandle, subDeviceAddress);
            return FALSE;
        }
        subDeviceAddress[sd] = address;
    }
    return TRUE;
}

static NvBool GetClassList(NVDevEvoPtr pDevEvo)
{
    NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS classListParams = { 0 };
    NvU32 ret;

    classListParams.numClasses = 0;
    classListParams.classList = NvP64_NULL;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->deviceHandle,
                         NV0080_CTRL_CMD_GPU_GET_CLASSLIST,
                         &classListParams, sizeof(classListParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    pDevEvo->supportedClasses =
        nvCalloc(classListParams.numClasses, sizeof(NvU32));

    if (pDevEvo->supportedClasses == NULL) {
        return FALSE;
    }

    classListParams.classList = NV_PTR_TO_NvP64(pDevEvo->supportedClasses);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->deviceHandle,
                         NV0080_CTRL_CMD_GPU_GET_CLASSLIST,
                         &classListParams, sizeof(classListParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvFree(pDevEvo->supportedClasses);
        pDevEvo->supportedClasses = NULL;
        return FALSE;
    }

    pDevEvo->numClasses = classListParams.numClasses;

    return TRUE;
}

static NvBool GetEngineListOneSubDevice(NVDevEvoPtr pDevEvo, NvU32 sd)
{
    NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS engineListParams = { 0 };
    NvU32 ret;
    NVSubDeviceEvoPtr pSubDevice = pDevEvo->pSubDevices[sd];
    size_t length;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pSubDevice->handle,
                         NV2080_CTRL_CMD_GPU_GET_ENGINES_V2,
                         &engineListParams, sizeof(engineListParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    if (engineListParams.engineCount == 0) {
        return TRUE;
    }

    length = engineListParams.engineCount * sizeof(NvU32);

    pSubDevice->supportedEngines = nvAlloc(length);

    if (pSubDevice->supportedEngines == NULL) {
        return FALSE;
    }

    nvkms_memcpy(pSubDevice->supportedEngines,
                 engineListParams.engineList,
                 length);
    pSubDevice->numEngines = engineListParams.engineCount;

    return TRUE;
}

static NvBool GetEngineList(NVDevEvoPtr pDevEvo)
{
    int sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (!GetEngineListOneSubDevice(pDevEvo, sd)) {
            return FALSE;
        }
    }

    return TRUE;
}

static void FreeSubDevice(NVDevEvoPtr pDevEvo, NVSubDeviceEvoPtr pSubDevice)
{
    if (pSubDevice == NULL) {
        return;
    }

    if (pSubDevice->handle != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle,
                    pSubDevice->handle);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pSubDevice->handle);
    }

    if (pSubDevice->gpuString[0] != '\0') {
        nvEvoLogDebug(EVO_LOG_INFO, "Freed %s", pSubDevice->gpuString);
    }

    nvFree(pSubDevice->supportedEngines);

    nvFree(pSubDevice);
}

static NVSubDeviceEvoPtr AllocSubDevice(NVDevEvoPtr pDevEvo, const NvU32 sd)
{
    NV2080_ALLOC_PARAMETERS subdevAllocParams = { 0 };
    NV2080_CTRL_GPU_GET_ID_PARAMS getIdParams = { 0 };
    NV2080_CTRL_GPU_GET_GID_INFO_PARAMS *pGidParams = NULL;
    NV0000_CTRL_GPU_GET_PCI_INFO_PARAMS pciInfoParams = { 0 };
    NvU32 ret;
    const char *uuid;

    NVSubDeviceEvoPtr pSubDevice = nvCalloc(1, sizeof(*pSubDevice));

    if (pSubDevice == NULL) {
        goto failure;
    }

    pSubDevice->handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    subdevAllocParams.subDeviceId = sd;

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       pSubDevice->handle,
                       NV20_SUBDEVICE_0,
                       &subdevAllocParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to initialize subDevice");
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pSubDevice->handle);
        pSubDevice->handle = 0;
        goto failure;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pSubDevice->handle,
                         NV2080_CTRL_CMD_GPU_GET_ID,
                         &getIdParams,
                         sizeof(getIdParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to identify GPU");
        goto failure;
    }

    pSubDevice->gpuId = getIdParams.gpuId;

    /* Query the UUID for the gpuString. */

    pGidParams = nvCalloc(1, sizeof(*pGidParams));

    if (pGidParams == NULL) {
        goto failure;
    }

    pGidParams->flags =
        DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _ASCII) |
        DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pSubDevice->handle,
                         NV2080_CTRL_CMD_GPU_GET_GID_INFO,
                         pGidParams,
                         sizeof(*pGidParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        /* If the query failed, make sure the UUID is cleared out. */
        nvkms_memset(pGidParams, 0, sizeof(*pGidParams));
    }

    /* Query the PCI bus address for the gpuString. */

    pciInfoParams.gpuId = pSubDevice->gpuId;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         nvEvoGlobal.clientHandle,
                         NV0000_CTRL_CMD_GPU_GET_PCI_INFO,
                         &pciInfoParams, sizeof(pciInfoParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        /* If the query failed, make sure the PCI bus address is cleared out. */
        nvkms_memset(&pciInfoParams, 0, sizeof(pciInfoParams));
    }

    pSubDevice->gpuLogIndex = nvGetGpuLogIndex();

    /*
     * Create the gpuString, using this example format:
     * GPU:0 (GPU-af2422f5-2719-29de-567f-ac899cf458c4) @ PCI:0000:01:00.0
     */
    if ((pGidParams->data[0] == '\0') || (pGidParams->length == 0)) {
        uuid = "";
    } else {
        uuid = (const char *) pGidParams->data;
    }

    nvkms_snprintf(pSubDevice->gpuString, sizeof(pSubDevice->gpuString),
                   "GPU:%d (%s) @ PCI:%04x:%02x:%02x.0",
                   pSubDevice->gpuLogIndex, uuid,
                   pciInfoParams.domain,
                   pciInfoParams.bus,
                   pciInfoParams.slot);

    pSubDevice->gpuString[sizeof(pSubDevice->gpuString) - 1] = '\0';

    nvEvoLogDebug(EVO_LOG_INFO, "Allocated %s", pSubDevice->gpuString);
    nvFree(pGidParams);

    return pSubDevice;

failure:
    FreeSubDevice(pDevEvo, pSubDevice);
    nvFree(pGidParams);

    return NULL;
}

static void CloseDevice(NVDevEvoPtr pDevEvo)
{
    NvU32 i;

    for (i = 0; i < ARRAY_LEN(pDevEvo->openedGpuIds); i++) {
        const NvU32 gpuId = pDevEvo->openedGpuIds[i];

        if (gpuId == NV0000_CTRL_GPU_INVALID_ID) {
            break;
        }

        nvkms_close_gpu(gpuId);
        pDevEvo->openedGpuIds[i] = NV0000_CTRL_GPU_INVALID_ID;
    }
}

static NvBool OpenTegraDevice(NVDevEvoPtr pDevEvo)
{
    NV0000_CTRL_GPU_GET_ID_INFO_PARAMS params = { 0 };
    nv_gpu_info_t *gpu_info = NULL;
    NvU32 ret, gpu_count = 0;

    nvAssert(pDevEvo->deviceId == NVKMS_DEVICE_ID_TEGRA);

    gpu_info = nvAlloc(NV_MAX_GPUS * sizeof(*gpu_info));
    if (gpu_info == NULL) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to allocate GPU ids arrays");
        goto fail;
    }

    gpu_count = nvkms_enumerate_gpus(gpu_info);
    if (gpu_count == 0) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "No NVIDIA GPUs found");
        goto fail;
    }

    if (gpu_count != 1) {
        // XXX If the system has both Tegra/iGPU and dGPU, it is not
        // guaranteed to find the Tegra, so fail.
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "More than one NVIDIA GPU found "
                    "in a Tegra configuration where only Tegra is expected.");
        goto fail;
    }

    if (!nvkms_open_gpu(gpu_info[0].gpu_id)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to open GPU");
        goto fail;
    }

    pDevEvo->openedGpuIds[0] = gpu_info[0].gpu_id;
    params.gpuId = gpu_info[0].gpu_id;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         nvEvoGlobal.clientHandle,
                         NV0000_CTRL_CMD_GPU_GET_ID_INFO,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to find GPU ID");
        goto fail;
    }

    pDevEvo->deviceId = params.deviceInstance;

    nvFree(gpu_info);
    return TRUE;

fail:
    nvFree(gpu_info);
    CloseDevice(pDevEvo);
    return FALSE;
}

static NvBool OpenDevice(NVDevEvoPtr pDevEvo)
{
    NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS idParams = { };
    NvU32 ret, i, gpuIdIndex = 0;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         nvEvoGlobal.clientHandle,
                         NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS,
                         &idParams, sizeof(idParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to query attached GPUs");
        goto fail;
    }

    ct_assert(ARRAY_LEN(pDevEvo->openedGpuIds) >= ARRAY_LEN(idParams.gpuIds));

    for (i = 0; i < ARRAY_LEN(idParams.gpuIds); i++) {
        NV0000_CTRL_GPU_GET_ID_INFO_PARAMS params = { 0 };
        const NvU32 gpuId = idParams.gpuIds[i];

        if (gpuId == NV0000_CTRL_GPU_INVALID_ID) {
            break;
        }

        nvAssert(pDevEvo->openedGpuIds[gpuIdIndex] ==
                 NV0000_CTRL_GPU_INVALID_ID);

        params.gpuId = gpuId;

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             nvEvoGlobal.clientHandle,
                             NV0000_CTRL_CMD_GPU_GET_ID_INFO,
                             &params, sizeof(params));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to find GPU ID");
            goto fail;
        }

        if (pDevEvo->deviceId != params.deviceInstance) {
            continue;
        }

        if (!nvkms_open_gpu(gpuId)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to open GPU");
            goto fail;
        }

        pDevEvo->openedGpuIds[gpuIdIndex++] = gpuId;
    }

    return TRUE;

fail:
    CloseDevice(pDevEvo);
    return FALSE;
}

static void FreeGpuVASpace(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->nvkmsGpuVASpace != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle,
                    pDevEvo->nvkmsGpuVASpace);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDevEvo->nvkmsGpuVASpace);
        pDevEvo->nvkmsGpuVASpace = 0;
    }
}

static NvBool AllocGpuVASpace(NVDevEvoPtr pDevEvo)
{
    NvU32 ret;
    NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS memoryVirtualParams = { };

    pDevEvo->nvkmsGpuVASpace =
        nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    memoryVirtualParams.offset = 0;
    memoryVirtualParams.limit = 0;          // no limit on VA space
    memoryVirtualParams.hVASpace = 0;       // client's default VA space

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       pDevEvo->nvkmsGpuVASpace,
                       NV01_MEMORY_VIRTUAL,
                       &memoryVirtualParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDevEvo->nvkmsGpuVASpace);
        pDevEvo->nvkmsGpuVASpace = 0;
        return FALSE;
    }

    return TRUE;
}

static void NonStallInterruptCallback(
    void *arg,
    void *pEventDataVoid,
    NvU32 hEvent,
    NvU32 data,
    NV_STATUS status)
{
    /*
     * We are called within resman's locks.  Schedule a separate callback to
     * execute with the nvkms_lock.
     *
     * XXX It might be nice to use a lighter-weight lock here to check if any
     * requests are pending in any NvKmsDeferredRequestFifo before scheduling
     * nvKmsServiceNonStallInterrupt().
     */

    (void) nvkms_alloc_timer_with_ref_ptr(
        nvKmsServiceNonStallInterrupt, /* callback */
        arg, /* argument (this is a ref_ptr to a pDevEvo) */
        0,   /* dataU32 */
        0);  /* usec */
}

static void UnregisterNonStallInterruptCallback(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->nonStallInterrupt.handle != 0) {
        NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS
            eventNotificationParams = { 0 };

        eventNotificationParams.event = NV2080_NOTIFIERS_FIFO_EVENT_MTHD;
        eventNotificationParams.action =
            NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
        nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->pSubDevices[0]->handle,
                       NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                       &eventNotificationParams,
                       sizeof(eventNotificationParams));

        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->pSubDevices[0]->handle,
                    pDevEvo->nonStallInterrupt.handle);

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDevEvo->nonStallInterrupt.handle);
    }

    pDevEvo->nonStallInterrupt.handle = 0;
}

static NvBool RegisterNonStallInterruptCallback(NVDevEvoPtr pDevEvo)
{
    NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS eventNotificationParams = { 0 };

    pDevEvo->nonStallInterrupt.handle =
        nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (!nvRmRegisterCallback(pDevEvo,
                              &pDevEvo->nonStallInterrupt.callback,
                              pDevEvo->ref_ptr,
                              pDevEvo->pSubDevices[0]->handle,
                              pDevEvo->nonStallInterrupt.handle,
                              NonStallInterruptCallback,
                              NV2080_NOTIFIERS_FIFO_EVENT_MTHD |
                              NV01_EVENT_NONSTALL_INTR)) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to register nonstall interrupt callback");
        goto failure_free_handle;
    }

    // Setup event notification
    eventNotificationParams.event = NV2080_NOTIFIERS_FIFO_EVENT_MTHD;
    eventNotificationParams.action =
        NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->pSubDevices[0]->handle,
                       NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                       &eventNotificationParams,
                       sizeof(eventNotificationParams))
        != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to set nonstall interrupt notification");
        goto failure_free_callback_and_handle;
    }

    return TRUE;

failure_free_callback_and_handle:
    nvRmApiFree(nvEvoGlobal.clientHandle,
                pDevEvo->pSubDevices[0]->handle,
                pDevEvo->nonStallInterrupt.handle);
failure_free_handle:
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                       pDevEvo->nonStallInterrupt.handle);
    pDevEvo->nonStallInterrupt.handle = 0;
    return FALSE;
}

NvBool nvRmAllocDeviceEvo(NVDevEvoPtr pDevEvo,
                          const struct NvKmsAllocDeviceRequest *pRequest)
{
    NV0080_ALLOC_PARAMETERS allocParams = { 0 };
    NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS getNumSubDevicesParams = { 0 };
    NvU32 ret, sd;

    if (nvEvoGlobal.clientHandle == 0) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Client handle not initialized");
        goto failure;
    }

    /*
     * RM deviceIds should be within [0,NV_MAX_DEVICES); check
     * that the client provided a value in range, and add one when
     * using deviceId as the per-device unique identifier in the
     * RM handle allocator: the identifier is expected to be != 0.
     */

    if ((pRequest->deviceId >= NV_MAX_DEVICES) &&
        (pRequest->deviceId != NVKMS_DEVICE_ID_TEGRA)) {
        goto failure;
    }

    pDevEvo->dpTimer = nvDPAllocTimer(pDevEvo);
    if (!pDevEvo->dpTimer) {
        goto failure;
    }

    if (!nvInitUnixRmHandleAllocator(
            &pDevEvo->handleAllocator,
            nvEvoGlobal.clientHandle,
            NVKMS_RM_HANDLE_SPACE_DEVICE(pRequest->deviceId))) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to initialize handles");
        goto failure;
    }

    pDevEvo->deviceHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    pDevEvo->deviceId = pRequest->deviceId;
    pDevEvo->sli.mosaic = pRequest->sliMosaic;

    if (pRequest->deviceId == NVKMS_DEVICE_ID_TEGRA) {
        /*
         * On Tegra, NVKMS client is not desktop RM client, so
         * enumerate and open first GPU.
         */
        if (!OpenTegraDevice(pDevEvo)) {
            goto failure;
        }

        pDevEvo->usesTegraDevice = TRUE;
    } else if (!OpenDevice(pDevEvo)) {
        goto failure;
    }

    allocParams.deviceId = pDevEvo->deviceId;

    /* Give NVKMS a private GPU virtual address space. */
    allocParams.hClientShare = nvEvoGlobal.clientHandle;

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       NV01_DEVICE_0,
                       &allocParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to initialize device");
        goto failure;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->deviceHandle,
                         NV0080_CTRL_CMD_GPU_GET_NUM_SUBDEVICES,
                         &getNumSubDevicesParams,
                         sizeof(getNumSubDevicesParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to determine number of GPUs");
        goto failure;
    }

    ct_assert(NVKMS_MAX_SUBDEVICES == NV_MAX_SUBDEVICES);
    if ((getNumSubDevicesParams.numSubDevices == 0) ||
        (getNumSubDevicesParams.numSubDevices >
         ARRAY_LEN(pDevEvo->pSubDevices))) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Unsupported number of GPUs: %d",
                    getNumSubDevicesParams.numSubDevices);
        goto failure;
    }

    pDevEvo->numSubDevices = getNumSubDevicesParams.numSubDevices;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        pDevEvo->pSubDevices[sd] = AllocSubDevice(pDevEvo, sd);
        if (pDevEvo->pSubDevices[sd] == NULL) {
            goto failure;
        }
    }

    pDevEvo->gpuLogIndex = pDevEvo->pSubDevices[0]->gpuLogIndex;

    if (!GetClassList(pDevEvo) || !GetEngineList(pDevEvo)) {
        goto failure;
    }

    if (!RegisterNonStallInterruptCallback(pDevEvo)) {
        goto failure;
    }

    if (!AllocGpuVASpace(pDevEvo)) {
        goto failure;
    }

    if (!nvAllocNvPushDevice(pDevEvo)) {
        goto failure;
    }

    return TRUE;

failure:
    nvRmFreeDeviceEvo(pDevEvo);
    return FALSE;
}

void nvRmFreeDeviceEvo(NVDevEvoPtr pDevEvo)
{
    NvU32 sd;

    nvFreeNvPushDevice(pDevEvo);

    FreeGpuVASpace(pDevEvo);

    UnregisterNonStallInterruptCallback(pDevEvo);

    nvFree(pDevEvo->supportedClasses);
    pDevEvo->supportedClasses = NULL;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        FreeSubDevice(pDevEvo, pDevEvo->pSubDevices[sd]);
        pDevEvo->pSubDevices[sd] = NULL;
    }

    if (pDevEvo->deviceHandle != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pDevEvo->deviceHandle);
        pDevEvo->deviceHandle = 0;
    }

    nvTearDownUnixRmHandleAllocator(&pDevEvo->handleAllocator);

    nvDPFreeTimer(pDevEvo->dpTimer);
    pDevEvo->dpTimer = NULL;

    CloseDevice(pDevEvo);
}

/*
 * Set up DIFR notifier listener to drive framebuffer prefetching once the
 * h/w gets idle enough.
 */
NvBool nvRmRegisterDIFREventHandler(NVDevEvoPtr pDevEvo)
{
    pDevEvo->difrPrefetchEventHandler =
        nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (pDevEvo->difrPrefetchEventHandler != 0) {
        NvBool registered;

        /*
         * Allocate event callback.
         */
        registered = nvRmRegisterCallback(
            pDevEvo,
            &pDevEvo->difrPrefetchCallback,
            pDevEvo->ref_ptr,
            pDevEvo->pSubDevices[0]->handle,
            pDevEvo->difrPrefetchEventHandler,
            DifrPrefetchEvent,
            NV2080_NOTIFIERS_LPWR_DIFR_PREFETCH_REQUEST);

        /*
         * Configure event notification.
         */
        if (registered) {
            NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS prefetchEventParams = { 0 };

            prefetchEventParams.event = NV2080_NOTIFIERS_LPWR_DIFR_PREFETCH_REQUEST;
            prefetchEventParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT;

            if (nvRmApiControl(nvEvoGlobal.clientHandle,
                               pDevEvo->pSubDevices[0]->handle,
                               NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                               &prefetchEventParams,
                               sizeof(prefetchEventParams))
                == NVOS_STATUS_SUCCESS) {
                return TRUE;

            }
        }
        nvRmUnregisterDIFREventHandler(pDevEvo);
    }
    return FALSE;
}

void nvRmUnregisterDIFREventHandler(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->difrPrefetchEventHandler != 0) {
        NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS prefetchEventParams = { 0 };

        prefetchEventParams.event = NV2080_NOTIFIERS_LPWR_DIFR_PREFETCH_REQUEST;
        prefetchEventParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;

        nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->pSubDevices[0]->handle,
                       NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                       &prefetchEventParams,
                       sizeof(prefetchEventParams));

        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->pSubDevices[0]->handle,
                    pDevEvo->difrPrefetchEventHandler);

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDevEvo->difrPrefetchEventHandler);
        pDevEvo->difrPrefetchEventHandler = 0;
    }
}


/*!
 * Determine whether all the dpys in the dpyIdList can be activated together.
 *
 * \param[in]   pDispEvo         The disp on which we search for a head.
 * \param[in]   dpyIdList        The connectors to test.
 *
 * \return      Return TRUE if all dpys can be driven simultaneously.
 */
NvBool nvRmIsPossibleToActivateDpyIdList(NVDispEvoPtr pDispEvo,
                                         const NVDpyIdList dpyIdList)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS mapParams = { 0 };
    NvU32 ret = 0;

    /* Trivially accept an empty dpyIdList. */

    if (nvDpyIdListIsEmpty(dpyIdList)) {
        return TRUE;
    }

    /* don't even try if EVO isn't initialized (e.g. during a VT switch) */

    if (!pDevEvo->gpus) {
        return FALSE;
    }

    /* build a mask of all the displays to use */

    mapParams.subDeviceInstance = pDispEvo->displayOwner;

    mapParams.displayMask = nvDpyIdListToNvU32(dpyIdList);

    /* ask RM for the head routing */

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SYSTEM_GET_HEAD_ROUTING_MAP,
                         &mapParams,
                         sizeof(mapParams));

    if ((ret != NVOS_STATUS_SUCCESS) || (mapParams.displayMask == 0)) {
        char *dpyIdListStr = nvGetDpyIdListStringEvo(pDispEvo, dpyIdList);
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "The requested configuration of display devices "
                    "(%s) is not supported on this GPU.",
                    nvSafeString(dpyIdListStr, "unknown"));
        nvFree(dpyIdListStr);

        return FALSE;
    }

    /* make sure we got everything we asked for */

    if (mapParams.displayMask != nvDpyIdListToNvU32(dpyIdList)) {
        char *requestedDpyIdListStr;
        char *returnedDpyIdListStr;

        requestedDpyIdListStr =
            nvGetDpyIdListStringEvo(pDispEvo, dpyIdList);

        returnedDpyIdListStr =
            nvGetDpyIdListStringEvo(pDispEvo,
                                    nvNvU32ToDpyIdList(mapParams.displayMask));

        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "The requested configuration of display devices "
                    "(%s) is not supported on this GPU; "
                    "%s is recommended, instead.",
                    nvSafeString(requestedDpyIdListStr, "unknown"),
                    nvSafeString(returnedDpyIdListStr, "unknown"));

        nvFree(requestedDpyIdListStr);
        nvFree(returnedDpyIdListStr);

        return FALSE;
    }

    return TRUE;
}


/*!
 * Tell the RM to save or restore the console VT state.
 *
 * \param[in]   cmd    indicate RM about the action.
 *
 * \return      TRUE on success, FALSE on failure.
 */
NvBool nvRmVTSwitch(NVDevEvoPtr pDevEvo, NvU32 cmd)
{
    NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS params = { 0 };
    NvU32 ret;

    params.cmd = cmd;
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->deviceHandle,
                         NV0080_CTRL_CMD_OS_UNIX_VT_SWITCH,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    return TRUE;
}

NvBool nvRmGetVTFBInfo(NVDevEvoPtr pDevEvo)
{
    NvU32 ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                     pDevEvo->deviceHandle,
                     NV0080_CTRL_CMD_OS_UNIX_VT_GET_FB_INFO,
                     &pDevEvo->vtFbInfo, sizeof(pDevEvo->vtFbInfo));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    return TRUE;
}

/*!
 * Import the current framebuffer console memory, for later use with NVKMS-based
 * console restore.
 *
 * Note this relies on pDevEvo->fbInfo populated by nvRmVTSwitch().
 *
 * There are several cases in which NVKMS cannot perform console restore:
 *
 * - Anything other than linear frame buffer consoles (i.e., VGA text modes,
 *   Non-linear or paletted graphical modes, etc).  For those, resman cannot
 *   query the framebuffer dimensions from the kernel,
 *   NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_SAVE_VT_STATE returns empty fbInfo
 *   params, and consequently pDevEvo->fbInfo.width == 0.
 *
 * - Linear frame buffer console with an unaligned pitch.  In this case,
 *   nvEvoRegisterSurface() will fail: it has to ensure the surface registration
 *   satisfies the EVO method interface requirement that PITCH surfaces are
 *   multiples of 256 bytes.  Consequently, pDevEvo->fbConsoleSurfaceHandle will
 *   be 0.
 *
 * - Depth 8 frame buffer consoles: these are color index, and cannot be
 *   supported by NVKMS console restore because they require the VGA palette,
 *   which exists in special RAM in the VGA core, so we can't name it with a
 *   ctxdma that we can feed into EVO's LUT.  The pFbInfo->depth switch below
 *   will reject depth 8.
 */
void nvRmImportFbConsoleMemory(NVDevEvoPtr pDevEvo)
{
    NvU32 ret;
    struct NvKmsRegisterSurfaceParams registration = { };
    const NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS *pFbInfo = &pDevEvo->vtFbInfo;
    NvHandle hMemory;

    nvAssert(pDevEvo->fbConsoleSurfaceHandle == 0);

    if (pFbInfo->width == 0) {
        // No console memory to map.
        return;
    }

    switch (pFbInfo->depth) {
    case 15:
        registration.request.format = NvKmsSurfaceMemoryFormatX1R5G5B5;
        break;
    case 16:
        registration.request.format = NvKmsSurfaceMemoryFormatR5G6B5;
        break;
    case 32:
        // That's a lie, it's really depth 24. Fall through.
    case 24:
        registration.request.format = NvKmsSurfaceMemoryFormatX8R8G8B8;
        break;
    default:
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_WARN,
                         "Unsupported framebuffer console depth %d",
                         pFbInfo->depth);
        return;
    }

    hMemory = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    if (hMemory == 0) {
        return;
    }

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       hMemory,
                       NV01_MEMORY_FRAMEBUFFER_CONSOLE,
                       NULL);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_WARN,
                         "Failed to map framebuffer console memory");
        goto done;
    }

    registration.request.useFd = FALSE;
    registration.request.rmClient = nvEvoGlobal.clientHandle;
    registration.request.widthInPixels = pFbInfo->width;
    registration.request.heightInPixels = pFbInfo->height;
    registration.request.layout = NvKmsSurfaceMemoryLayoutPitch;

    registration.request.planes[0].u.rmObject = hMemory;
    registration.request.planes[0].pitch = pFbInfo->pitch;
    registration.request.planes[0].rmObjectSizeInBytes =
        (NvU64) pFbInfo->height * (NvU64) pFbInfo->pitch;

    nvEvoRegisterSurface(pDevEvo, pDevEvo->pNvKmsOpenDev, &registration,
                         NvHsMapPermissionsNone);

    pDevEvo->fbConsoleSurfaceHandle = registration.reply.surfaceHandle;

    // nvEvoRegisterSurface dups the handle, so we can free the one we just
    // imported.
    nvRmApiFree(nvEvoGlobal.clientHandle,
                nvEvoGlobal.clientHandle,
                hMemory);
done:
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, hMemory);
}

static void LogAuxPacket(const NVDispEvoRec *pDispEvo, const DPAUXPACKET *pkt)
{
    const char *req, *rep;
    char str[DP_MAX_MSG_SIZE * 3 + 1];
    char *p = str;
    int i;

    switch (DRF_VAL(_DP, _AUXLOGGER, _REQUEST_TYPE, pkt->auxEvents)) {
        case NV_DP_AUXLOGGER_REQUEST_TYPE_AUXWR:
            req = "auxwr";
            break;
        case NV_DP_AUXLOGGER_REQUEST_TYPE_AUXRD:
            req = "auxrd";
            break;
        case NV_DP_AUXLOGGER_REQUEST_TYPE_MOTWR:
            // MOT is "middle of transaction", which is just another type of i2c
            // access.
            req = "motwr";
            break;
        case NV_DP_AUXLOGGER_REQUEST_TYPE_I2CWR:
            req = "i2cwr";
            break;
        case NV_DP_AUXLOGGER_REQUEST_TYPE_MOTRD:
            req = "motrd";
            break;
        case NV_DP_AUXLOGGER_REQUEST_TYPE_I2CRD:
            req = "i2crd";
            break;
        default:
            // Only log I2C and AUX transactions.
            return;
    }

    switch (DRF_VAL(_DP, _AUXLOGGER, _REPLY_TYPE, pkt->auxEvents)) {
        case NV_DP_AUXLOGGER_REPLY_TYPE_NULL:
            rep = "none";
            break;
        case NV_DP_AUXLOGGER_REPLY_TYPE_SB_ACK:
            rep = "sb_ack";
            break;
        case NV_DP_AUXLOGGER_REPLY_TYPE_RETRY:
            rep = "retry";
            break;
        case NV_DP_AUXLOGGER_REPLY_TYPE_TIMEOUT:
            rep = "timeout";
            break;
        case NV_DP_AUXLOGGER_REPLY_TYPE_DEFER:
            rep = "defer";
            break;
        case NV_DP_AUXLOGGER_REPLY_TYPE_DEFER_TO:
            rep = "defer_to";
            break;
        case NV_DP_AUXLOGGER_REPLY_TYPE_ACK:
            rep = "ack";
            break;
        case NV_DP_AUXLOGGER_REPLY_TYPE_ERROR:
            rep = "error";
            break;
        default:
        case NV_DP_AUXLOGGER_REPLY_TYPE_UNKNOWN:
            rep = "unknown";
            break;
    }

    for (i = 0; i < pkt->auxMessageReplySize; i++) {
        p += nvkms_snprintf(p, str + sizeof(str) - p, "%02x ",
                            pkt->auxPacket[i]);
    }

    nvAssert(p < str + sizeof(str));
    *p = '\0';

    nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                 "%04u: port %u @ 0x%05x: [%10u] %s %2u, [%10u] %-8s %s",
                 pkt->auxCount, pkt->auxOutPort, pkt->auxPortAddress,
                 pkt->auxRequestTimeStamp, req,
                 pkt->auxMessageReqSize,
                 pkt->auxReplyTimeStamp, rep,
                 str);
}

/*!
 * This "attribute" queries the RM DisplayPort AUX channel log and dumps it to
 * the kernel log. It returns a value of TRUE if any RM AUX transactions were
 * logged, and FALSE otherwise.
 *
 * This attribute is intended to be queried in a loop as long as it reads TRUE.
 *
 * \return TRUE if the query succeeded (even if no events were logged).
 * \return FALSE if the query failed.
 */
NvBool nvRmQueryDpAuxLog(NVDispEvoRec *pDispEvo, NvS64 *pValue)
{
    NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS *pParams =
        nvCalloc(sizeof(*pParams), 1);
    NvU32 status;
    int i;
    NvBool ret = FALSE;

    pDispEvo->dpAuxLoggingEnabled = TRUE;
    *pValue = FALSE;

    if (!pParams) {
        return FALSE;
    }

    pParams->subDeviceInstance = pDispEvo->displayOwner;
    pParams->dpAuxBufferReadSize = MAX_LOGS_PER_POLL;

    status = nvRmApiControl(nvEvoGlobal.clientHandle,
                            pDispEvo->pDevEvo->displayCommonHandle,
                            NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA,
                            pParams, sizeof(*pParams));
    if (status != NVOS_STATUS_SUCCESS) {
        goto done;
    }

    nvAssert(pParams->dpNumMessagesRead <= MAX_LOGS_PER_POLL);
    for (i = 0; i < pParams->dpNumMessagesRead; i++) {
        const DPAUXPACKET *pkt = &pParams->dpAuxBuffer[i];

        switch (DRF_VAL(_DP, _AUXLOGGER, _EVENT_TYPE, pkt->auxEvents)) {
            case NV_DP_AUXLOGGER_EVENT_TYPE_AUX:
                LogAuxPacket(pDispEvo, pkt);
                break;
            case NV_DP_AUXLOGGER_EVENT_TYPE_HOT_PLUG:
                nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                             "%04u: port %u [%10u] hotplug",
                             pkt->auxCount, pkt->auxOutPort,
                             pkt->auxRequestTimeStamp);
                break;
            case NV_DP_AUXLOGGER_EVENT_TYPE_HOT_UNPLUG:
                nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                             "%04u: port %u [%10u] unplug",
                             pkt->auxCount, pkt->auxOutPort,
                             pkt->auxRequestTimeStamp);
                break;
            case NV_DP_AUXLOGGER_EVENT_TYPE_IRQ:
                nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                             "%04u: port %u [%10u] irq",
                             pkt->auxCount, pkt->auxOutPort,
                             pkt->auxRequestTimeStamp);
                break;
            default:
                nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                             "%04u: port %u [%10u] unknown event",
                             pkt->auxCount, pkt->auxOutPort,
                             pkt->auxRequestTimeStamp);
                break;
        }

        *pValue = TRUE;
    }

    ret = TRUE;

done:
    nvFree(pParams);
    return ret;
}

/*!
 * Return the GPU's current PTIMER, or 0 if the query fails.
 */
NvU64 nvRmGetGpuTime(NVDevEvoPtr pDevEvo)
{
    const NvU32 sd = 0;
    NV2080_CTRL_TIMER_GET_TIME_PARAMS params;

    NvU32 ret;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->pSubDevices[sd]->handle,
                         NV2080_CTRL_CMD_TIMER_GET_TIME,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDebug(EVO_LOG_ERROR, "Failed to query GPU time, ret = %d", ret);
        return 0;
    }

    return params.time_nsec;
}

NvBool nvRmSetGc6Allowed(NVDevEvoPtr pDevEvo, NvBool allowed)
{
    NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_PARAMS params = { };
    NvU32 sd;

    if (allowed == pDevEvo->gc6Allowed) {
        return TRUE;
    }

    params.action = allowed ? NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_DEC :
                              NV2080_CTRL_OS_UNIX_GC6_BLOCKER_REFCNT_INC;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NvU32 ret = nvRmApiControl(
                        nvEvoGlobal.clientHandle,
                        pDevEvo->pSubDevices[sd]->handle,
                        NV2080_CTRL_CMD_OS_UNIX_GC6_BLOCKER_REFCNT,
                        &params, sizeof(params));
        if (ret != NVOS_STATUS_SUCCESS) {
            // XXX This is catastrophic, is there a good way to unravel?
            nvEvoLogDevDebug(
                pDevEvo, EVO_LOG_ERROR,
                "Failed to modify GC6 blocker refcount, sd = %d, ret = %x",
                sd, ret);
            return FALSE;
        }
    }

    pDevEvo->gc6Allowed = allowed;

    /*
     * If we are just now disallowing GC6, it's possible that we previously
     * entered GC6 and invalidated display channel state. Re-initialize it here
     * to ensure that future modesets are successful.
     */
    if (!allowed && pDevEvo->core) {
        NvU32 channelIdx;

        pDevEvo->hal->InitChannel(pDevEvo, pDevEvo->core);
        pDevEvo->coreInitMethodsPending = TRUE;

        for (channelIdx = 0; channelIdx < pDevEvo->numHeads; channelIdx++) {
            // XXX We should InitChannel() for all per-head channels when coming
            // out of GC6.
            pDevEvo->hal->InitChannel(
                pDevEvo, pDevEvo->head[channelIdx].layer[NVKMS_MAIN_LAYER]);
        }
    }

    return TRUE;
}

typedef struct _NVRmRgLine1CallbackRec {
    NVRgLine1CallbackRec base;
    struct nvkms_ref_ptr *ref_ptr;
    NvU32 rmHandle;
    NVDispEvoRec *pDispEvo;
    NvU32 head;
} NVRmRgLine1CallbackRec;

static void RGLine1ServiceInterrupt(void *dataPtr, NvU32 dataU32)
{
     NVRmRgLine1CallbackRec *pRmCallback = (NVRmRgLine1CallbackRec*)dataPtr;
     pRmCallback->base.pCallbackProc(pRmCallback->pDispEvo, pRmCallback->head,
                                     &pRmCallback->base);
}

/*!
 * Receive RG line 1 interrupt notification from resman.
 *
 * This function is registered as the kernel callback function from resman when
 * the RG line 1 interrupt is generated.
 *
 * This function is called within resman's context, so we schedule a zero timer
 * callback to process the swapgroup check and release without holding the
 * resman lock.
 */
static void RGLine1InterruptCallback(NvU32 rgIntrLine, void *param1,
                                      NvBool bIsIrqlIsr /* unused */)
{
    (void) nvkms_alloc_timer_with_ref_ptr(
        RGLine1ServiceInterrupt, /* callback */
        param1, /* argument (this is a ref_ptr to a NVRmRgLine1CallbackRec*) */
        0,  /* dataU32 */
        0); /* usec */
}

/*!
 * Register an RM callback function for the RG line 1 interrupt.
 *
 * \param[in]   pDispEvo          The display on which to allocate the callback
 *
 * \param[in]   head              The head on which to allocate the callback
 *
 * \param[in]   pCallback         The callback function pointer to be registered
 *
 * \return      Pointer to callback object on success, NULL on failure. This same
 *              pointer must be used to unregister the callback.
 */
NVRgLine1CallbackPtr
nvRmAddRgLine1Callback(NVDispEvoRec *pDispEvo,
                       NvU32 head,
                       NVRgLine1CallbackProc pCallbackProc,
                       void *pUserData)
{
    NV0092_RG_LINE_CALLBACK_ALLOCATION_PARAMETERS rgLineParams = { 0 };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    NvU32 ret;
    NVRmRgLine1CallbackRec *pRmCallback = nvCalloc(1, sizeof(*pRmCallback));

    if (pRmCallback == NULL) {
        goto failed;
    }

    pRmCallback->ref_ptr = nvkms_alloc_ref_ptr(pRmCallback);
    if (pRmCallback->ref_ptr == NULL) {
        goto failed;
    }
    pRmCallback->base.pCallbackProc = pCallbackProc;
    pRmCallback->base.pUserData = pUserData;
    pRmCallback->rmHandle = handle;
    pRmCallback->pDispEvo = pDispEvo;
    pRmCallback->head = head;

    rgLineParams.subDeviceInstance = pDispEvo->displayOwner;
    rgLineParams.head = head;
    rgLineParams.rgLineNum = 1;
    rgLineParams.pCallbkFn = RGLine1InterruptCallback;
    rgLineParams.pCallbkParams = pRmCallback->ref_ptr;

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->displayCommonHandle,
                       handle,
                       NV0092_RG_LINE_CALLBACK,
                       &rgLineParams);

    if (ret == NVOS_STATUS_SUCCESS) {
        return &pRmCallback->base;
    }

    nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                     "Failed to enable RG line interrupt, ret: %d", ret);
    /* fall through */

failed:
    if (pRmCallback != NULL) {
        nvkms_free_ref_ptr(pRmCallback->ref_ptr);
        nvFree(pRmCallback);
    }

    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, handle);

    return NULL;
}

/*!
 * Unregister an RM callback function previously registered with
 * nvRmAddRgLine1Callback.
 *
 * \param[in]  pDispEvo    The display on which to unregister the
 *                         callback
 *
 * \param[in]  pCallback   Pointer to the previously allocated
 *                         callback object
 */
void nvRmRemoveRgLine1Callback(const NVDispEvoRec *pDispEvo,
                               NVRgLine1CallbackPtr pCallback)
{
    NVRmRgLine1CallbackRec *pRmCallback;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    if (pCallback == NULL) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to disable RG line interrupt, obj pointer NULL");
        return;
    }
    pRmCallback = nv_container_of(pCallback, NVRmRgLine1CallbackRec, base);

    ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                      pDevEvo->displayCommonHandle,
                      pRmCallback->rmHandle);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to disable RG line interrupt, ret: %d", ret);
    }

    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pRmCallback->rmHandle);
    nvkms_free_ref_ptr(pRmCallback->ref_ptr);
    nvFree(pRmCallback);
}

/*!
 * Register an RM callback function for the VBlankinterrupt.
 *
 * \param[in]   pDispEvo          The display on which to allocate the callback
 *
 * \param[in]   head              The head on which to allocate the callback
 *
 * \param[in]   pCallback         The callback function pointer to be registered
 *
 * \return      Handle to callback object on success, 0 on failure. This same
 *              handle must be used to unregister the callback.
 */
NvU32 nvRmAddVBlankCallback(
    const NVDispEvoRec *pDispEvo,
    NvU32 head,
    OSVBLANKCALLBACKPROC pCallback,
    void *pParam2)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    NvU32 ret;
    NvU32 handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS params = {
        .pProc       = pCallback,
        .LogicalHead = head,
        .pParm1      = pDispEvo->ref_ptr,
        .pParm2      = pParam2,
    };

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->pSubDevices[sd]->handle,
                       handle,
                       NV9010_VBLANK_CALLBACK,
                       &params);

    if (ret == NVOS_STATUS_SUCCESS) {
        return handle;
    } else {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to enable VBlank callback, ret: %d", ret);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, handle);
        return 0;
    }
}

/*!
 * Unregister an RM callback function previously registered with
 * nvRmAddVBlankCallback.
 *
 * \param[in]  pDispEvo                 The display on which to unregister the
 *                                      callback
 *
 * \param[in]  callbackObjectHandle     Handle to the previously allocated
 *                                      callback object
 */
void nvRmRemoveVBlankCallback(const NVDispEvoRec *pDispEvo,
                              NvU32 callbackObjectHandle)
{
    const NvU32 sd = pDispEvo->displayOwner;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    if (callbackObjectHandle == 0) {
        // already removed
        return;
    }

    ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                      pDevEvo->pSubDevices[sd]->handle,
                      callbackObjectHandle);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to disable VBlank callback, ret: %d", ret);
    }

    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, callbackObjectHandle);
}

/*!
 * Initialize the dynamic display mux on supported systems.
 *
 * \param[in] pDpyEvo    The dpy on which to initialize the mux.
 */
static void MuxInit(const NVDpyEvoRec *pDpyEvo)
{
    NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    NvU32 ret;

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyEvoGetConnectorId(pDpyEvo);

    if (pDpyEvo->internal) {
        /* Attempt to get the EDID from ACPI. This is required for internal
         * displays only, as the internal mux initialization requires data
         * from the internal panel's EDID, while the external mux can be
         * initialized in the absence of a display, in which case there is
         * obviously no EDID present. The EDID read is done via ACPI, in
         * order to accommodate mux initialization while the internal panel
         * is disconnected from the GPU. */

        /* Map with hard-coded data for systems known to support dynamic mux
         * switching. This is a poor-man's alternative to the WDDM driver's
         * CDisplayMgr::NVInitializeACPIToDeviceMaskMap() */
        NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS acpiMap = {
            .mapTable = {
                {.acpiId = 0x8001a420, .displayId = 0x1000, .dodIndex = 0},
            }
        };
        NVEdidRec edid = { };
        NVParsedEdidEvoRec *pParsedEdid = NULL;
        NVEvoInfoStringRec infoString;

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_SPECIFIC_SET_ACPI_ID_MAPPING,
                             &acpiMap, sizeof(acpiMap));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDebug(EVO_LOG_ERROR, "Failed to set ACPI ID map.");
            return;
        }

        nvInitInfoString(&infoString, NULL, 0);

        /* Retrieve the internal panel's EDID from ACPI */
        if (!nvDpyReadAndParseEdidEvo(pDpyEvo, NULL,
                                      NVKMS_EDID_READ_MODE_ACPI,
                                      &edid, &pParsedEdid,
                                      &infoString)) {
            /* EDID read is expected to fail on non-dynamic-mux systems. */
            goto edid_done;
        }

        if (edid.length == 0 || pParsedEdid == NULL || !pParsedEdid->valid) {
            goto edid_done;
        }

        params.manfId = pParsedEdid->info.manuf_id;
        params.productId = pParsedEdid->info.product_id;

edid_done:
        nvFree(edid.buffer);
        nvFree(pParsedEdid);

        /* Internal mux initialization will fail without manfId/productId */
        if (!params.manfId || !params.productId) {
            return;
        }
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_INIT_MUX_DATA,
                         &params,
                         sizeof(params));

    if (ret == NVOS_STATUS_SUCCESS) {
        pDispEvo->muxDisplays = nvAddDpyIdToDpyIdList(pDpyEvo->id,
                                                      pDispEvo->muxDisplays);
    } else {
        nvEvoLogDebug(EVO_LOG_ERROR, "Failed to initialize mux on %s.",
                      pDpyEvo->name);
    }
}

static NVDpyIdList GetValidMuxDpys(NVDispEvoPtr pDispEvo)
{
    NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS params = { 0 };

    params.subDeviceInstance = pDispEvo->displayOwner;

    nvRmApiControl(nvEvoGlobal.clientHandle,
                   pDispEvo->pDevEvo->displayCommonHandle,
                   NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX,
                   &params, sizeof(params));

    return nvNvU32ToDpyIdList(params.muxDisplayMask);
}

void nvRmMuxInit(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    int i;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, i, pDevEvo) {
        NVDpyIdList validMuxDpys = GetValidMuxDpys(pDispEvo);
        NVDpyEvoPtr pDpyEvo;

        FOR_ALL_EVO_DPYS(pDpyEvo, validMuxDpys, pDispEvo) {
            MuxInit(pDpyEvo);
        }
    }
}

/*!
 * Perform mux pre-switch operations
 *
 * \param[in] pDpyEvo             The Dpy of the target mux
 * \param[in] state               The target mux state
 *
 * \return TRUE on success; FALSE on failure
 */
NvBool nvRmMuxPre(const NVDpyEvoRec *pDpyEvo, NvMuxState state)
{
    NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo;
    NVDevEvoPtr pDevEvo;
    NvU32 ret;

    pDispEvo = pDpyEvo->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    if (!nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->muxDisplays)) {
        return FALSE;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);
    params.flags = DRF_DEF(0073_CTRL_DFP, _DISP_MUX_FLAGS, _SR_ENTER_SKIP, _NO);

    if (state == MUX_STATE_DISCRETE) {
        params.flags = NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_IGPU_TO_DGPU;
    } else if (state == MUX_STATE_INTEGRATED) {
        params.flags = NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_DGPU_TO_IGPU;
    } else {
        return FALSE;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS,
                         &params, sizeof(params));

    nvEvoLogDebug(EVO_LOG_INFO, "RmMuxPre status %d", ret);

    return ret == NVOS_STATUS_SUCCESS;
}

/*!
 * Perform mux switch operation
 *
 * \param[in] pDpyEvo    The Dpy of the target mux
 * \param[in] state      The target mux state
 *
 * \return TRUE on success; FALSE on failure
 */
NvBool nvRmMuxSwitch(const NVDpyEvoRec *pDpyEvo, NvMuxState state)
{
    NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo;
    NVDevEvoPtr pDevEvo;
    NvU32 ret;

    pDispEvo = pDpyEvo->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    if (!nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->muxDisplays)) {
        return FALSE;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);

    if (state == MUX_STATE_DISCRETE) {
        params.flags = NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_IGPU_TO_DGPU;
    } else if (state == MUX_STATE_INTEGRATED) {
        params.flags = NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_DGPU_TO_IGPU;
    } else {
        return FALSE;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX,
                         &params, sizeof(params));

    nvEvoLogDebug(EVO_LOG_INFO, "RmMuxSwitch status %d", ret);

    /*
     * Force link training after waiting for the DP AUX link to settle.
     * The delay duration comes from DFP_MUX_AUX_SETTLE_DELAY_MS_DEFAULT
     * in drivers/resman/kernel/inc/dfpmux.h.
     */
    nvkms_usleep(100000);

    if (pDpyEvo->internal && state == MUX_STATE_DISCRETE) {
        nvAssert(nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo));
        nvDPNotifyShortPulse(pDpyEvo->pConnectorEvo->pDpLibConnector);
        nvDPFireExpiredTimers(pDevEvo);
    }

    return ret == NVOS_STATUS_SUCCESS;
}

/*!
 * Perform mux post-switch operations
 *
 * \param[in] pDpyEvo                The Dpy of the target mux
 * \param[in] state                  The target mux state
 *
 * \return TRUE on success; FALSE on failure
 */
NvBool nvRmMuxPost(const NVDpyEvoRec *pDpyEvo, NvMuxState state)
{
    NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo;
    NVDevEvoPtr pDevEvo;
    NvU32 ret;

    pDispEvo = pDpyEvo->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    if (!nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->muxDisplays)) {
        return FALSE;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);
    params.flags = DRF_DEF(0073_CTRL_DFP, _DISP_MUX_FLAGS, _SR_ENTER_SKIP, _NO);

    if (state == MUX_STATE_DISCRETE) {
        params.flags = NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_IGPU_TO_DGPU;
    } else if (state == MUX_STATE_INTEGRATED) {
        params.flags = NV0073_CTRL_DFP_DISP_MUX_FLAGS_SWITCH_TYPE_DGPU_TO_IGPU;
    } else {
        return FALSE;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS,
                         &params, sizeof(params));

    nvEvoLogDebug(EVO_LOG_INFO, "RmMuxPost status %d", ret);

    return ret == NVOS_STATUS_SUCCESS;
}

/*!
 * Query the current state of a dynamic mux
 *
 * \param[in] pDpyEvo    The Dpy of the target mux whose state is to be queried
 *
 * \return Mux state (either MUX_STATE_INTEGRATED or MUX_STATE_DISCRETE) on
 * success; MUX_STATE_UNKNOWN on failure.
 */
NvMuxState nvRmMuxState(const NVDpyEvoRec *pDpyEvo)
{
    NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS params = { 0 };
    NVDispEvoPtr pDispEvo;
    NVDevEvoPtr pDevEvo;

    pDispEvo = pDpyEvo->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    if (!nvDpyIdIsInDpyIdList(pDpyEvo->id, pDispEvo->muxDisplays)) {
        return MUX_STATE_UNKNOWN;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = nvDpyIdToNvU32(pDpyEvo->pConnectorEvo->displayId);

    if (NVOS_STATUS_SUCCESS == nvRmApiControl(nvEvoGlobal.clientHandle,
                                    pDevEvo->displayCommonHandle,
                                    NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS,
                                    &params, sizeof(params))) {
        if (FLD_TEST_DRF(0073_CTRL_DFP, _DISP_MUX, _STATE, _INTEGRATED_GPU,
            params.muxStatus)) {
            return MUX_STATE_INTEGRATED;
        }
        if (FLD_TEST_DRF(0073_CTRL_DFP, _DISP_MUX, _STATE, _DISCRETE_GPU,
            params.muxStatus)) {
            return MUX_STATE_DISCRETE;
        }
    }

    return MUX_STATE_UNKNOWN;
}

void nvRmRegisterBacklight(NVDispEvoRec *pDispEvo)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS dispParams = { 0 };
    NvU32 displayMask, displayId;
    NvU32 brightness;

    nvAssert(pDispEvo->backlightDevice == NULL);

    dispParams.subDeviceInstance = pDispEvo->displayOwner;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->displayCommonHandle,
                       NV0073_CTRL_CMD_SYSTEM_GET_INTERNAL_DISPLAYS,
                       &dispParams, sizeof(dispParams)) != NV_OK) {
        return;
    }

    /* Find a display with a backlight */
    displayMask = dispParams.availableInternalDisplaysMask;
    for (; displayMask; displayMask &= ~LOWESTBIT(displayMask))
    {
        NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS params = { 0 };
        NV_STATUS status;

        displayId = LOWESTBIT(displayMask);

        params.subDeviceInstance = pDispEvo->displayOwner;
        params.displayId         = displayId;
        params.brightnessType = NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_TYPE_PERCENT100;

        status = nvRmApiControl(nvEvoGlobal.clientHandle,
                                pDevEvo->displayCommonHandle,
                                NV0073_CTRL_CMD_SPECIFIC_GET_BACKLIGHT_BRIGHTNESS,
                                &params, sizeof(params));

        if (status == NV_OK)
        {
            brightness = params.brightness;
            break;
        }
    }

    if (displayMask == 0)
    {
        /* No internal display has backlight */
        return;
    }

    pDispEvo->backlightDevice = nvkms_register_backlight(
        pDevEvo->pSubDevices[pDispEvo->displayOwner]->gpuId,
        displayId, pDispEvo,
        brightness);
}

void nvRmUnregisterBacklight(NVDispEvoRec *pDispEvo)
{
    if (pDispEvo->backlightDevice != NULL) {
        nvkms_unregister_backlight(pDispEvo->backlightDevice);
    }
    pDispEvo->backlightDevice = NULL;
}

NvU32 nvRmAllocAndBindSurfaceDescriptor(
    NVDevEvoPtr pDevEvo,
    NvU32 hMemory,
    const enum NvKmsSurfaceMemoryLayout layout,
    NvU64 limit,
    NVSurfaceDescriptor *pSurfaceDesc)
{
    NVSurfaceDescriptor surfaceDesc;
    NvU32 flags = DRF_DEF(OS03, _FLAGS, _HASH_TABLE, _DISABLE);
    NvU32 head, layer;
    NvU32 ret;

    switch (layout) {
        case NvKmsSurfaceMemoryLayoutBlockLinear:
            flags |= DRF_DEF(OS03, _FLAGS, _PTE_KIND, _BL);
            break;
        case NvKmsSurfaceMemoryLayoutPitch:
            flags |= DRF_DEF(OS03, _FLAGS, _PTE_KIND, _PITCH);
            break;
    }

     /* Each surface to be displayed needs its own surface descriptor */
    nvAssert(pDevEvo->displayHandle != 0);
    nvAssert(pDevEvo->core);
    nvAssert(pDevEvo->core->pb.channel_handle);
    nvAssert(hMemory);
    nvAssert(limit);

    ret =
        pDevEvo->hal->AllocSurfaceDescriptor(pDevEvo, &surfaceDesc,
                                             hMemory, flags, limit);

    if (ret != NVOS_STATUS_SUCCESS) {
        return ret;
    }

    ret =
        pDevEvo->hal->BindSurfaceDescriptor(pDevEvo,
                                            pDevEvo->core,
                                            &surfaceDesc);
    if (ret != NVOS_STATUS_SUCCESS) {
        goto free_this_handle_and_fail;
    }

    for (head = 0; head < pDevEvo->numHeads; head++) {
        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            if (pDevEvo->head[head].layer[layer]) {
                 nvAssert(pDevEvo->head[head].layer[layer]->pb.channel_handle);

                 ret = pDevEvo->hal->BindSurfaceDescriptor(pDevEvo,
                         pDevEvo->head[head].layer[layer],
                         &surfaceDesc);
                 if (ret != NVOS_STATUS_SUCCESS) {
                     goto free_this_handle_and_fail;
                 }
            }
        }
    }

    *pSurfaceDesc = surfaceDesc;

    return NVOS_STATUS_SUCCESS;

free_this_handle_and_fail:
    pDevEvo->hal->FreeSurfaceDescriptor(pDevEvo,
                                        nvEvoGlobal.clientHandle,
                                        &surfaceDesc);
    return ret;
}
