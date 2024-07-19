/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080bus.finn
//

#include "nvcfg_sdk.h"
#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX bus control commands and parameters */

/**
 * NV2080_CTRL_CMD_BUS_GET_PCI_INFO
 *
 * This command returns PCI bus identifier information for the specified GPU.
 *
 *   pciDeviceId
 *       This parameter specifies the internal PCI device and vendor
 *       identifiers for the GPU.
 *   pciSubSystemId
 *       This parameter specifies the internal PCI subsystem identifier for
 *       the GPU.
 *   pciRevisionId
 *       This parameter specifies the internal PCI device-specific revision
 *       identifier for the GPU.
 *   pciExtDeviceId
 *       This parameter specifies the external PCI device identifier for
 *       the GPU.  It contains only the 16-bit device identifier.  This
 *       value is identical to the device identifier portion of
 *       pciDeviceId since non-transparent bridges are no longer supported.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_BUS_GET_PCI_INFO (0x20801801) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS {
    NvU32 pciDeviceId;
    NvU32 pciSubSystemId;
    NvU32 pciRevisionId;
    NvU32 pciExtDeviceId;
} NV2080_CTRL_BUS_GET_PCI_INFO_PARAMS;

/*
 * NV2080_CTRL_BUS_INFO
 *
 * This structure represents a single 32bit bus engine value.  Clients
 * request a particular bus engine value by specifying a unique bus
 * information index.
 *
 * Legal bus information index values are:
 *   NV2080_CTRL_BUS_INFO_INDEX_TYPE
 *     This index is used to request the bus type of the GPU.
 *     Legal return values for this index are:
 *       NV2080_CTRL_BUS_INFO_TYPE_PCI
 *       NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS
 *       NV2080_CTRL_BUS_INFO_TYPE_FPCI
 *   NV2080_CTRL_BUS_INFO_INDEX_INTLINE
 *     This index is used to request the interrupt line (or irq) assignment
 *     for the GPU.  The return value is system-dependent.
 *   NV2080_CTRL_BUS_INFO_INDEX_CAPS
 *     This index is used to request the bus engine capabilities for the GPU.
 *     The return value is specified as a mask of capabilities.
 *     Legal return values for this index are:
 *       NV2080_CTRL_BUS_INFO_CAPS_NEED_IO_FLUSH
 *       NV2080_CTRL_BUS_INFO_CAPS_CHIP_INTEGRATED
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CAPS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CAPS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CAPS
 *     These indices are used to request PCI Express link-specific
 *     capabilities values.  A value of zero is returned for non-PCIE GPUs.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CTRL_STATUS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CTRL_STATUS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CTRL_STATUS
 *     These indices are used to request PCI Express link-specific
 *     control status values.  A value of zero is returned for non-PCIE GPUs.
 *   NV2080_CTRL_BUS_INFO_INDEX_COHERENT_DMA_FLAGS
 *     This index is used to request coherent dma transfer flags.
 *     Valid coherent dma transfer flags include:
 *       NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_CTXDMA
 *       NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_GPUGART
 *   NV2080_CTRL_BUS_INFO_INDEX_NONCOHERENT_DMA_FLAGS
 *     This index is used to request noncoherent dma transfer flags.
 *     Valid noncoherent dma transfer flags include:
 *       NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_CTXDMA
 *       NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_GPUGART
 *       NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_COH_MODE
 *   NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_SIZE
 *     This index is used to request the size of the GPU GART in MBytes.
 *   NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_FLAGS
 *     This index is used to request GPU GART flags.
 *     Valid gart flags include:
 *       NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_REQFLUSH
 *         This flag indicates that GPU GART clients need to do an explicit
 *         flush via an appropriate SetContextDma method.
 *       NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_UNIFIED
 *         This flag indicates that the GART address range includes both
 *         system and video memory.
 *   NV2080_CTRL_BUS_INFO_INDEX_BUS_NUMBER
 *     This index is used to request the PCI-based bus number of the GPU.
 *     Support for this index is platform-dependent.
 *   NV2080_CTRL_BUS_INFO_INDEX_DEVICE_NUMBER
 *     This index is used to request the PCI-based device number of the GPU.
 *     Support for this index is platform-dependent.
 *   NV2080_CTRL_BUS_INFO_INDEX_DOMAIN_NUMBER
 *     This index is used to request the PCI-based domain number of the GPU.
 *     Support for this index is platform-dependent.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_ERRORS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_ERRORS
 *     These indices are used to request PCI Express error status.
 *     The current status is cleared as part of these requests.
 *     Valid PCI Express error status values include:
 *       NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR
 *       NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR
 *       NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR
 *       NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST
 *   NV2080_CTRL_BUS_INFO_INDEX_INTERFACE_TYPE
 *     This index is used to request the bus interface type of the GPU.
 *     Legal return values for this index are:
 *       NV2080_CTRL_BUS_INFO_TYPE_PCI
 *       NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS
 *       NV2080_CTRL_BUS_INFO_TYPE_FPCI
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN2_INFO // DEPRECATED
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN_INFO  // REPLACES "GEN2" variant
 *     This index is used to retrieve PCI Express Gen configuration support
 *     This index is used to retrieve PCI Express Gen2 configuration support 
 *     for the GPU.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN1
 *          The GPU is PCI Express Gen1 capable.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN2
 *          The GPU is PCI Express Gen2 capable.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN3
 *          The GPU is PCI Express Gen3 capable.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN4
 *          The GPU is PCI Express Gen4 capable.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN5
 *          The GPU is PCI Express Gen5 capable.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN1
 *          The GPU is configured in PCI Express Gen1 mode.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN2
 *          The GPU is configured in PCI Express Gen2 mode.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN3
 *          The GPU is configured in PCI Express Gen3 mode.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN4
 *          The GPU is configured in PCI Express Gen4 mode.
 *      NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN5
 *          The GPU is configured in PCI Express Gen5 mode.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_AER
 *     This index retrieves PCI Express Advanced Error Reporting (AER) errors 
 *     for the GPU.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CAPS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CAPS
 *     This index retrieves the PCI Express link capabilities for the
 *     board.  For example, a Quadro FX4700X2 has two GPUs and PCIe
 *     switch.  With this board, this index returns the link
 *     capabilities of the PCIe switch.  In a single GPU board, this
 *     index returns the link capabilities of the GPU.  A value of
 *     zero is returned for non-PCIE GPUs.
 *     UPSTREAM_LINK_CAPS is kept for backwards compatibility.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CTRL_STATUS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CTRL_STATUS
 *     This index retrieves the PCI Express link status for the board.
 *     For example, a Quadro FX4700X2 has two GPUs and PCIe switch.
 *     With this board, this index returns the link capabilities of
 *     the PCIe switch.  In a single GPU board, this index returns the
 *     link status of the GPU.  A value of zero is returned for
 *     non-PCIE GPUs.
 *     UPSTREAM_LINK_CTRL_STATUS is kept for backwards compatibility.
 *   NV2080_CTRL_BUS_INFO_INDEX_ASLM_STATUS
 *     This index is used to request the PCI Express ASLM settings.
 *     This index is only valid when NV2080_CTRL_BUS_INFO_TYPE indicates PCIE.
 *     A value of zero is returned for non-PCI Express bus type.
 *     _ASLM_STATUS_PCIE is always _PRESENT if PCI Express bus type.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_LINK_WIDTH_SWITCH_ERROR_COUNT
 *     This index is used to get the ASLM switching error count.
 *     A value of zero will be returned if no errors occurs while
 *     ASLM switching
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN2_SWITCH_ERROR_COUNT
 *     This index is used to get the Gen1<-->Gen2 switching error count
 *     A value of zero will be returned in case speed change from Gen1 to
 *     Gen2 is clean or if chipset is not gen2 capable or if gen1<-->gen2 
 *     switching is disabled.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_CYA_ASPM
 *     This index is used to get the ASPM CYA L0s\L1 enable\disable status.
 *     Legal return value is specified as a mask of valid and data field
 *     possible return values are:
 *      NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_VALID_NO
 *      NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_VALID_YES
 *      NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_DISABLED
 *      NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_L0S
 *      NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_L1
 *      NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_L0S_L1
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS
 *     These indices are used to request detailed PCI Express error counters.
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS_CLEAR
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS_CLEAR
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED_CLEAR
 *   NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS_CLEAR
 *     These indices are used to clear detailed PCI Express error counters.
 *   NV2080_CTRL_BUS_INFO_INDEX_GPU_INTERFACE_TYPE
 *     This index is used to request the internal interface type of the GPU.
 *     Legal return values for this index are:
 *       NV2080_CTRL_BUS_INFO_TYPE_PCI
 *       NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS
 *       NV2080_CTRL_BUS_INFO_TYPE_FPCI
 *   NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE
 *     This index queries the type of sysmem connection to CPU
 *     NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_PCIE
 *     NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_NVLINK
 *     NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_C2C
 *
 */

typedef NVXXXX_CTRL_XXX_INFO NV2080_CTRL_BUS_INFO;

/* valid bus info index values */

/**
 *  This index is used to request the bus type of the GPU.
 *  Legal return values for this index are:
 *    NV2080_CTRL_BUS_INFO_TYPE_PCI
 *    NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS
 *    NV2080_CTRL_BUS_INFO_TYPE_FPCI
 */
#define NV2080_CTRL_BUS_INFO_INDEX_TYPE                                     (0x00000000)
#define NV2080_CTRL_BUS_INFO_INDEX_INTLINE                                  (0x00000001)
#define NV2080_CTRL_BUS_INFO_INDEX_CAPS                                     (0x00000002)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CAPS                       (0x00000003)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CAPS                      (0x00000004)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CAPS                  (0x00000005)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CAPS                (0x00000006)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CTRL_STATUS                (0x00000007)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CTRL_STATUS               (0x00000008)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CTRL_STATUS           (0x00000009)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CTRL_STATUS         (0x0000000A)
/**
 * This index is used to request coherent dma transfer flags.
 * Valid coherent dma transfer flags include:
 *   NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_CTXDMA
 *   NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_GPUGART
 */
#define NV2080_CTRL_BUS_INFO_INDEX_COHERENT_DMA_FLAGS                       (0x0000000B)
/**
 * This index is used to request noncoherent dma transfer flags.
 * Valid noncoherent dma transfer flags include:
 *   NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_CTXDMA
 *   NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_GPUGART
 *   NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_COH_MODE
 */
#define NV2080_CTRL_BUS_INFO_INDEX_NONCOHERENT_DMA_FLAGS                    (0x0000000C)
/**
 * This index is used to request the size of the GPU GART in MBytes.
 */
#define NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_SIZE                            (0x0000000D)
/**
 * This index is used to request GPU GART flags.
 * Valid gart flags include:
 *   NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_REQFLUSH
 *     This flag indicates that GPU GART clients need to do an explicit
 *     flush via an appropriate SetContextDma method.
 *   NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_UNIFIED
 *     This flag indicates that the GART address range includes both
 *     system and video memory.
 */
#define NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_FLAGS                           (0x0000000E)
#define NV2080_CTRL_BUS_INFO_INDEX_BUS_NUMBER                               (0x0000000F)
#define NV2080_CTRL_BUS_INFO_INDEX_DEVICE_NUMBER                            (0x00000010)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_ERRORS                     (0x00000011)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_ERRORS                    (0x00000012)
#define NV2080_CTRL_BUS_INFO_INDEX_INTERFACE_TYPE                           (0x00000013)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN2_INFO                           (0x00000014)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_AER                        (0x00000015)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CAPS                     (0x00000016)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CTRL_STATUS              (0x00000017)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_ASLM_STATUS                         (0x00000018)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_LINK_WIDTH_SWITCH_ERROR_COUNT       (0x00000019)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_LINK_SPEED_SWITCH_ERROR_COUNT       (0x0000001A)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_CYA_ASPM                        (0x0000001B)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS            (0x0000001C)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS                 (0x0000001D)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED              (0x0000001E)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS           (0x0000001F)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_LINECODE_ERRORS_CLEAR      (0x00000020)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CRC_ERRORS_CLEAR           (0x00000021)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NAKS_RECEIVED_CLEAR        (0x00000022)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FAILED_L0S_EXITS_CLEAR     (0x00000023)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CORRECTABLE_ERRORS         (0x00000024)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NONFATAL_ERRORS            (0x00000025)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FATAL_ERRORS               (0x00000026)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_UNSUPPORTED_REQUESTS       (0x00000027)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CORRECTABLE_ERRORS_CLEAR   (0x00000028)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_NONFATAL_ERRORS_CLEAR      (0x00000029)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_FATAL_ERRORS_CLEAR         (0x0000002A)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_UNSUPPORTED_REQUESTS_CLEAR (0x0000002B)
#define NV2080_CTRL_BUS_INFO_INDEX_DOMAIN_NUMBER                            (0x0000002C)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN_INFO                            (0x0000002D)
#define NV2080_CTRL_BUS_INFO_INDEX_GPU_INTERFACE_TYPE                       (0x0000002E)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_GEN_INFO                   (0x0000002F)
#define NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_GEN_INFO                      (0x00000030)
#define NV2080_CTRL_BUS_INFO_INDEX_MSI_INFO                                 (0x00000031)
/**
 * This index is used to request the top 32 bits of the size of the GPU
 * GART in MBytes.
 */
#define NV2080_CTRL_BUS_INFO_INDEX_GPU_GART_SIZE_HI                         (0x00000032)
#define NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE                   (0x00000033)
#define NV2080_CTRL_BUS_INFO_INDEX_MAX                                      NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE
#define NV2080_CTRL_BUS_INFO_MAX_LIST_SIZE                                  (0x00000034)

/* valid bus info type return values */
#define NV2080_CTRL_BUS_INFO_TYPE_PCI                                       (0x00000001)
#define NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS                               (0x00000003)
#define NV2080_CTRL_BUS_INFO_TYPE_FPCI                                      (0x00000004)
#define NV2080_CTRL_BUS_INFO_TYPE_AXI                                       (0x00000008)

/* valid bus capability flags */
#define NV2080_CTRL_BUS_INFO_CAPS_NEED_IO_FLUSH                             (0x00000001)
#define NV2080_CTRL_BUS_INFO_CAPS_CHIP_INTEGRATED                           (0x00000002)

/* 
 * Format of PCIE link caps return values
 * Note that Link Capabilities register format is followed only for bits 11:0
 */
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED               3:0
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_2500MBPS               (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_5000MBPS               (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_8000MBPS               (0x00000003)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_16000MBPS              (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_32000MBPS              (0x00000005)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_64000MBPS              (0x00000006)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_WIDTH               9:4
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_ASPM                    11:10
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_ASPM_NONE                        (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_ASPM_L0S                         (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_ASPM_L0S_L1                      (0x00000003)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN                     15:12
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN1                         (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN2                         (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN3                         (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN4                         (0x00000003)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN5                         (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GEN_GEN6                         (0x00000005)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL              19:16
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN1                  (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN2                  (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN3                  (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN4                  (0x00000003)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN5                  (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_CURR_LEVEL_GEN6                  (0x00000005)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GPU_GEN                 23:20
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GPU_GEN_GEN1                     (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GPU_GEN_GEN2                     (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GPU_GEN_GEN3                     (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GPU_GEN_GEN4                     (0x00000003)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GPU_GEN_GEN5                     (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_GPU_GEN_GEN6                     (0x00000005)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_SPEED_CHANGES           24:24
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_SPEED_CHANGES_ENABLED            (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_SPEED_CHANGES_DISABLED           (0x00000001)

/* format of PCIE control status return values */
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_ASPM                 1:0
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_ASPM_DISABLED            (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_ASPM_L0S                 (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_ASPM_L1                  (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_ASPM_L0S_L1              (0x00000003)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED           19:16
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_2500MBPS      (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_5000MBPS      (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_8000MBPS      (0x00000003)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_16000MBPS     (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_32000MBPS     (0x00000005)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_64000MBPS     (0x00000006)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH           25:20
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_UNDEFINED     (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_X1            (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_X2            (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_X4            (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_X8            (0x00000008)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_X12           (0x0000000C)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_X16           (0x00000010)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH_X32           (0x00000020)

/* coherent dma transfer flags */
#define NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_CTXDMA             0:0
#define NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_CTXDMA_FALSE                (0x00000000)
#define NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_CTXDMA_TRUE                 (0x00000001)
#define NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_GPUGART            2:2
#define NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_GPUGART_FALSE               (0x00000000)
#define NV2080_CTRL_BUS_INFO_COHERENT_DMA_FLAGS_GPUGART_TRUE                (0x00000001)

/* noncoherent dma transfer flags */
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_CTXDMA          0:0
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_CTXDMA_FALSE             (0x00000000)
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_CTXDMA_TRUE              (0x00000001)
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_GPUGART         2:2
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_GPUGART_FALSE            (0x00000000)
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_GPUGART_TRUE             (0x00000001)
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_COH_MODE        3:3
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_COH_MODE_FALSE           (0x00000000)
#define NV2080_CTRL_BUS_INFO_NONCOHERENT_DMA_FLAGS_COH_MODE_TRUE            (0x00000001)

/* GPU GART flags */
#define NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_REQFLUSH               0:0
#define NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_REQFLUSH_FALSE                  (0x00000000)
#define NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_REQFLUSH_TRUE                   (0x00000001)
#define NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_UNIFIED                1:1
#define NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_UNIFIED_FALSE                   (0x00000000)
#define NV2080_CTRL_BUS_INFO_GPU_GART_FLAGS_UNIFIED_TRUE                    (0x00000001)

/* format of PCIE errors return values */
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR                    (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR               (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR                   (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST                (0x00000008)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_ENTERED_RECOVERY              (0x00000010)

/* PCIE Gen2 capability and current level */
#define NV2080_CTRL_BUS_INFO_PCIE_GEN2_INFO_CAP                    0:0
#define NV2080_CTRL_BUS_INFO_PCIE_GEN2_INFO_CAP_FALSE                       (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_GEN2_INFO_CAP_TRUE                        (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_GEN2_INFO_CURR_LEVEL             1:1
#define NV2080_CTRL_BUS_INFO_PCIE_GEN2_INFO_CURR_LEVEL_GEN1                 (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_GEN2_INFO_CURR_LEVEL_GEN2                 (0x00000001)

/* format of PCIE AER return values */
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_TRAINING_ERR              (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR           (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP              (0x00000004)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_FC_PROTO_ERR              (0x00000008)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT               (0x00000010)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_ABORT                 (0x00000020)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL                 (0x00000040)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_RCVR_OVERFLOW             (0x00000080)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP             (0x00000100)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_ECRC_ERROR                (0x00000200)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ           (0x00000400)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR                     (0x00010000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP                     (0x00020000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP                    (0x00040000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER               (0x00080000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT                (0x00100000)
#define NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL           (0x00200000)

/* format of PCIE ASLM status return value */
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_PCIE                  0:0
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_PCIE_ERROR                    (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_PCIE_PRESENT                  (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_SUPPORTED             1:1
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_SUPPORTED_NO                  (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_SUPPORTED_YES                 (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_CL_CAPABLE            2:2
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_CL_CAPABLE_NO                 (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_CL_CAPABLE_YES                (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_OS_SUPPORTED          3:3
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_OS_SUPPORTED_NO               (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_OS_SUPPORTED_YES              (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_BR04                  4:4
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_BR04_MISSING                  (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_ASLM_STATUS_BR04_PRESENT                  (0x00000001)

/* format of GPU CYA CAPS return value */
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_VALID               0:0
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_VALID_NO                     (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_VALID_YES                    (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM                     2:1
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_DISABLED                     (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_L0S                          (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_L1                           (0x00000002)
#define NV2080_CTRL_BUS_INFO_PCIE_GPU_CYA_ASPM_L0S_L1                       (0x00000003)

/* format of MSI INFO return value */
#define NV2080_CTRL_BUS_INFO_MSI_STATUS                            0:0
#define NV2080_CTRL_BUS_INFO_MSI_STATUS_DISABLED                            (0x00000000)
#define NV2080_CTRL_BUS_INFO_MSI_STATUS_ENABLED                             (0x00000001)

/*format of L1PM Substates capabilities information */
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_PCIPM_L1_2_SUPPORTED         0:0
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_PCIPM_L1_2_SUPPORTED_YES        (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_PCIPM_L1_2_SUPPORTED_NO         (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_PCIPM_L1_1_SUPPORTED         1:1
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_PCIPM_L1_1_SUPPORTED_YES        (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_PCIPM_L1_1_SUPPORTED_NO         (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_ASPM_L1_2_SUPPORTED          2:2
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_ASPM_L1_2_SUPPORTED_YES         (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_ASPM_L1_2_SUPPORTED_NO          (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_ASPM_L1_1_SUPPORTED          3:3
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_ASPM_L1_1_SUPPORTED_YES         (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_ASPM_L1_1_SUPPORTED_NO          (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_L1PM_SUPPORTED               4:4
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_L1PM_SUPPORTED_YES              (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_L1PM_SUPPORTED_NO               (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_RESERVED                     7:5
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_PORT_RESTORE_TIME            15:8
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_T_POWER_ON_SCALE             17:16
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CAP_T_POWER_ON_VALUE             23:19

/*format of L1 PM Substates Control 1 Register */
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_PCIPM_L1_2_ENABLED         0:0
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_PCIPM_L1_2_ENABLED_YES        (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_PCIPM_L1_2_ENABLED_NO         (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_PCIPM_L1_1_ENABLED         1:1
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_PCIPM_L1_1_ENABLED_YES        (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_PCIPM_L1_1_ENABLED_NO         (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_ASPM_L1_2_ENABLED          2:2
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_ASPM_L1_2_ENABLED_YES         (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_ASPM_L1_2_ENABLED_NO          (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_ASPM_L1_1_ENABLED          3:3
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_ASPM_L1_1_ENABLED_YES         (0x00000001)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_ASPM_L1_1_ENABLED_NO          (0x00000000)
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_COMMON_MODE_RESTORE_TIME   15:8
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_LTR_L1_2_THRESHOLD_VALUE   25:16
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL1_LTR_L1_2_THRESHOLD_SCALE   31:29

/*format of L1 PM Substates Control 2 Register */
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL2_T_POWER_ON_SCALE           1:0
#define NV2080_CTRL_BUS_INFO_PCIE_L1_SS_CTRL2_T_POWER_ON_VALUE           7:3

/* valid sysmem connection type values */
#define NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_PCIE              (0x00000000)
#define NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_NVLINK            (0x00000001)
#define NV2080_CTRL_BUS_INFO_INDEX_SYSMEM_CONNECTION_TYPE_C2C               (0x00000002)

/**
 * NV2080_CTRL_CMD_BUS_GET_INFO
 *
 * This command returns bus engine information for the associated GPU.
 * Requests to retrieve bus information use a list of one or more
 * NV2080_CTRL_BUS_INFO structures.
 *
 *   busInfoListSize
 *     This field specifies the number of entries on the caller's
 *     busInfoList.
 *   busInfoList
 *     This field specifies a pointer in the caller's address space
 *     to the buffer into which the bus information is to be returned.
 *     This buffer must be at least as big as busInfoListSize multiplied
 *     by the size of the NV2080_CTRL_BUS_INFO structure.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_OPERATING_SYSTEM
 */
#define NV2080_CTRL_CMD_BUS_GET_INFO                                        (0x20801802) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_GET_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_BUS_GET_INFO_PARAMS {
    NvU32 busInfoListSize;
    NV_DECLARE_ALIGNED(NvP64 busInfoList, 8);
} NV2080_CTRL_BUS_GET_INFO_PARAMS;

#define NV2080_CTRL_CMD_BUS_GET_INFO_V2 (0x20801823) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_GET_INFO_V2_PARAMS_MESSAGE_ID (0x23U)

typedef struct NV2080_CTRL_BUS_GET_INFO_V2_PARAMS {
    NvU32                busInfoListSize;
    NV2080_CTRL_BUS_INFO busInfoList[NV2080_CTRL_BUS_INFO_MAX_LIST_SIZE];
} NV2080_CTRL_BUS_GET_INFO_V2_PARAMS;

/*
 * NV2080_CTRL_BUS_PCI_BAR_INFO
 *
 * This structure describes PCI bus BAR information.
 *
 *   flags
 *     This field contains any flags for the associated BAR.
 *   barSize
 *     This field contains the size in megabytes of the associated BAR.
 *     DEPRECATED, please use barSizeBytes.
 *   barSizeBytes
 *     This field contains the size in bytes of the associated BAR.
 *   barOffset
 *     This field contains the PCI bus offset in bytes of the associated BAR.
 */
typedef struct NV2080_CTRL_BUS_PCI_BAR_INFO {
    NvU32 flags;
    NvU32 barSize;
    NV_DECLARE_ALIGNED(NvU64 barSizeBytes, 8);
    NV_DECLARE_ALIGNED(NvU64 barOffset, 8);
} NV2080_CTRL_BUS_PCI_BAR_INFO;

/*
 * NV2080_CTRL_CMD_BUS_GET_PCI_BAR_INFO
 *
 * This command returns PCI bus BAR information.
 *
 *   barCount
 *     This field returns the number of BARs for the associated subdevice.
 *     Legal values for this parameter will be between one to
 *      NV2080_CTRL_BUS_MAX_BARS.
 *   barInfo
 *     This field returns per-BAR information in the form of an array of
 *     NV2080_CTRL_BUS_PCI_BAR_INFO structures.  Information for as many as
 *     NV2080_CTRL_BUS_MAX_PCI_BARS will be returned.  Any unused entries will
 *     be initialized to zero.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_BUS_GET_PCI_BAR_INFO (0x20801803) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS_MESSAGE_ID" */

/* maximum number of BARs per subdevice */
#define NV2080_CTRL_BUS_MAX_PCI_BARS         (8)

#define NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS {
    NvU32 pciBarCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_BUS_PCI_BAR_INFO pciBarInfo[NV2080_CTRL_BUS_MAX_PCI_BARS], 8);
} NV2080_CTRL_BUS_GET_PCI_BAR_INFO_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_SET_PCIE_LINK_WIDTH
 *
 * This command sets PCI-E link width to the specified new value.
 *
 *   pcieLinkWidth
 *      This field specifies the new PCI-E link width.
 *
 *   failingReason
 *      This field specifies the reason why the change of link width fails.
 *      It is valid only when this routine returns NV_ERR_GENERIC.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_GENERIC
 */
#define NV2080_CTRL_CMD_BUS_SET_PCIE_LINK_WIDTH (0x20801804) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS {
    NvU32 pcieLinkWidth;
    NvU32 failingReason;
} NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_PARAMS;

#define NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_ERROR_PSTATE          (0x00000001)
#define NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_ERROR_PCIE_CFG_ACCESS (0x00000002)
#define NV2080_CTRL_BUS_SET_PCIE_LINK_WIDTH_ERROR_TRAINING        (0x00000004)

/*
 * NV2080_CTRL_CMD_BUS_SET_PCIE_SPEED
 *
 * This command Initiates a change in PCIE Bus Speed
 *
 *   busSpeed
 *     This field is the target speed to train to.
 *     Legal values for this parameter are:
 *       NV2080_CTRL_BUS_SET_PCIE_SPEED_2500MBPS
 *       NV2080_CTRL_BUS_SET_PCIE_SPEED_5000MBPS
 *       NV2080_CTRL_BUS_SET_PCIE_SPEED_8000MBPS
 *       NV2080_CTRL_BUS_SET_PCIE_SPEED_16000MBPS
 *       NV2080_CTRL_BUS_SET_PCIE_SPEED_32000MBPS
 *
 *   Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_BUS_SET_PCIE_SPEED                        (0x20801805) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS {
    NvU32 busSpeed;
} NV2080_CTRL_BUS_SET_PCIE_SPEED_PARAMS;

#define NV2080_CTRL_BUS_SET_PCIE_SPEED_2500MBPS  (0x00000001)
#define NV2080_CTRL_BUS_SET_PCIE_SPEED_5000MBPS  (0x00000002)
#define NV2080_CTRL_BUS_SET_PCIE_SPEED_8000MBPS  (0x00000003)
#define NV2080_CTRL_BUS_SET_PCIE_SPEED_16000MBPS (0x00000004)
#define NV2080_CTRL_BUS_SET_PCIE_SPEED_32000MBPS (0x00000005)
#define NV2080_CTRL_BUS_SET_PCIE_SPEED_64000MBPS (0x00000006)


/*
 * NV2080_CTRL_CMD_BUS_MAP_BAR2
 *
 * This command sets up BAR2 page tables for passed-in memory handle.
 * This command MUST be executed before NV2080_CTRL_CMD_BUS_UNMAP_BAR2
 * or NV2080_CTRL_CMD_BUS_VERIFY_BAR2. Not supported on SLI.
 *
 * hMemory
 *    This field is a handle to physical memory.
 *
 * Possible status values returned are
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 *
 */
#define NV2080_CTRL_CMD_BUS_MAP_BAR2             (0x20801809) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_MAP_BAR2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_MAP_BAR2_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV2080_CTRL_BUS_MAP_BAR2_PARAMS {
    NvHandle hMemory;
} NV2080_CTRL_BUS_MAP_BAR2_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_UNMAP_BAR2
 *
 * This command unmaps any pending BAR2 page tables created with
 * NV2080_CTRL_CMD_BUS_MAP_BAR2 command. The handle passed in must
 * match the handle used to map the page tables. Not supported on SLI.
 *
 * hMemory
 *    This field is a handle to physical memory.
 *
 * Possible status values returned are
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 *
 */
#define NV2080_CTRL_CMD_BUS_UNMAP_BAR2 (0x2080180a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_UNMAP_BAR2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_UNMAP_BAR2_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV2080_CTRL_BUS_UNMAP_BAR2_PARAMS {
    NvHandle hMemory;
} NV2080_CTRL_BUS_UNMAP_BAR2_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_VERIFY_BAR2
 *
 * This command tests BAR2 against BAR0 if there are BAR2 page tables
 * set up with NV2080_CTRL_CMD_BUS_MAP_BAR2 command. The handle passed
 * in must match the handle used to map the page tables. Not supported on SLI.
 *
 * hMemory
 *    This field is a handle to physical memory.
 * offset
 *    Base offset of the surface where the test will make its first dword write.
 * size
 *    Test will write '(size/4)*4' bytes starting at surface offset `offset'.
 *
 * Possible status values returned are
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_NOT_SUPPORTED
 *
 */
#define NV2080_CTRL_CMD_BUS_VERIFY_BAR2 (0x2080180b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_VERIFY_BAR2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_VERIFY_BAR2_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_BUS_VERIFY_BAR2_PARAMS {
    NvHandle hMemory;
    NvU32    offset;
    NvU32    size;
} NV2080_CTRL_BUS_VERIFY_BAR2_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_SERVICE_GPU_MULTIFUNC_STATE
 *  This command would reports the current Audio device power state or Sets new power state.
 *
 * command
 *  This parametrer specifies the target GPU multifunction state.
 *      NV2080_CTRL_BUS_ENABLE_GPU_MULTIFUNC_STATE      Enables the multi function state
 *      NV2080_CTRL_BUS_DISABLE_GPU_MULTIFUNC_STATE     Disables the multi function state.
 *      NV2080_CTRL_BUS_GET_GPU_MULTIFUNC_STATE         Get the Current device power state.
 *
 * Possible status values returned are:
 *     NV_OK
 *     NV_ERR_GENERIC
 */

#define NV2080_CTRL_CMD_BUS_SERVICE_GPU_MULTIFUNC_STATE (0x20801812) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS_MESSAGE_ID (0x12U)

typedef struct NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS {
    NvU8  command;
    NvU32 deviceState;
} NV2080_CTRL_BUS_SERVICE_GPU_MULTIFUNC_STATE_PARAMS;

#define NV2080_CTRL_BUS_ENABLE_GPU_MULTIFUNC_STATE  (0x00000000)
#define NV2080_CTRL_BUS_DISABLE_GPU_MULTIFUNC_STATE (0x00000001)
#define NV2080_CTRL_BUS_GET_GPU_MULTIFUNC_STATE     (0x00000002)

/*
 * NV2080_CTRL_CMD_BUS_GET_PEX_COUNTERS
 *  This command gets the counts for different counter types.
 *
 * pexCounterMask
 *  This parameter specifies the input mask for desired counter types.
 *
 * pexTotalCorrectableErrors
 *  This parameter gives the total correctable errors which includes
 *  NV_XVE_ERROR_COUNTER1 plus LCRC Errors, 8B10B Errors, NAKS and Failed L0s
 *
 * pexCorrectableErrors
 *  This parameter only includes NV_XVE_ERROR_COUNTER1 value.
 *
 * pexTotalNonFatalErrors
 *  This parameter returns total Non-Fatal Errors which may or may not
 *  include Correctable Errors.
 *
 * pexTotalFatalErrors
 *  This parameter returns Total Fatal Errors
 *
 * pexTotalUnsupportedReqs
 *  This parameter returns Total Unsupported Requests
 *
 * pexErrors
 *  This array contains the error counts for each error type as requested from
 *  the pexCounterMask. The array indexes correspond to the mask bits one-to-one.
 */

#define NV2080_CTRL_CMD_BUS_GET_PEX_COUNTERS        (0x20801813) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PEX_MAX_COUNTER_TYPES           31
#define NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS {
    NvU32 pexCounterMask;
    NvU32 pexTotalCorrectableErrors;
    NvU16 pexCorrectableErrors;
    NvU8  pexTotalNonFatalErrors;
    NvU8  pexTotalFatalErrors;
    NvU8  pexTotalUnsupportedReqs;
    NvU16 pexCounters[NV2080_CTRL_PEX_MAX_COUNTER_TYPES];
} NV2080_CTRL_BUS_GET_PEX_COUNTERS_PARAMS;

/*
 * Note that MAX_COUNTER_TYPES will need to be updated each time
 * a new counter type gets added to the list below. The value
 * depends on the bits set for the last valid define. Look
 * at pexCounters[] comments above for details.
 *
 */
#define NV2080_CTRL_BUS_PEX_COUNTER_TYPE                          0x00000000
#define NV2080_CTRL_BUS_PEX_COUNTER_RECEIVER_ERRORS               0x00000001
#define NV2080_CTRL_BUS_PEX_COUNTER_REPLAY_COUNT                  0x00000002
#define NV2080_CTRL_BUS_PEX_COUNTER_REPLAY_ROLLOVER_COUNT         0x00000004
#define NV2080_CTRL_BUS_PEX_COUNTER_BAD_DLLP_COUNT                0x00000008
#define NV2080_CTRL_BUS_PEX_COUNTER_BAD_TLP_COUNT                 0x00000010
#define NV2080_CTRL_BUS_PEX_COUNTER_8B10B_ERRORS_COUNT            0x00000020
#define NV2080_CTRL_BUS_PEX_COUNTER_SYNC_HEADER_ERRORS_COUNT      0x00000040
#define NV2080_CTRL_BUS_PEX_COUNTER_LCRC_ERRORS_COUNT             0x00000080
#define NV2080_CTRL_BUS_PEX_COUNTER_FAILED_L0S_EXITS_COUNT        0x00000100
#define NV2080_CTRL_BUS_PEX_COUNTER_NAKS_SENT_COUNT               0x00000200
#define NV2080_CTRL_BUS_PEX_COUNTER_NAKS_RCVD_COUNT               0x00000400
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_ERRORS                   0x00000800
#define NV2080_CTRL_BUS_PEX_COUNTER_L1_TO_RECOVERY_COUNT          0x00001000
#define NV2080_CTRL_BUS_PEX_COUNTER_L0_TO_RECOVERY_COUNT          0x00002000
#define NV2080_CTRL_BUS_PEX_COUNTER_RECOVERY_COUNT                0x00004000
#define NV2080_CTRL_BUS_PEX_COUNTER_CHIPSET_XMIT_L0S_ENTRY_COUNT  0x00008000
#define NV2080_CTRL_BUS_PEX_COUNTER_GPU_XMIT_L0S_ENTRY_COUNT      0x00010000
#define NV2080_CTRL_BUS_PEX_COUNTER_L1_ENTRY_COUNT                0x00020000
#define NV2080_CTRL_BUS_PEX_COUNTER_L1P_ENTRY_COUNT               0x00040000
#define NV2080_CTRL_BUS_PEX_COUNTER_DEEP_L1_ENTRY_COUNT           0x00080000
#define NV2080_CTRL_BUS_PEX_COUNTER_ASLM_COUNT                    0x00100000
#define NV2080_CTRL_BUS_PEX_COUNTER_TOTAL_CORR_ERROR_COUNT        0x00200000
#define NV2080_CTRL_BUS_PEX_COUNTER_CORR_ERROR_COUNT              0x00400000
#define NV2080_CTRL_BUS_PEX_COUNTER_NON_FATAL_ERROR_COUNT         0x00800000
#define NV2080_CTRL_BUS_PEX_COUNTER_FATAL_ERROR_COUNT             0x01000000
#define NV2080_CTRL_BUS_PEX_COUNTER_UNSUPP_REQ_COUNT              0x02000000
#define NV2080_CTRL_BUS_PEX_COUNTER_L1_1_ENTRY_COUNT              0x04000000
#define NV2080_CTRL_BUS_PEX_COUNTER_L1_2_ENTRY_COUNT              0x08000000
#define NV2080_CTRL_BUS_PEX_COUNTER_L1_2_ABORT_COUNT              0x10000000
#define NV2080_CTRL_BUS_PEX_COUNTER_L1SS_TO_DEEP_L1_TIMEOUT_COUNT 0x20000000
#define NV2080_CTRL_BUS_PEX_COUNTER_L1_SHORT_DURATION_COUNT       0x40000000

/*
 * NV2080_CTRL_CMD_BUS_CLEAR_PEX_COUNTER_COUNTERS
 *  This command gets the counts for different counter types.
 *
 * pexCounterMask
 *  This parameter specifies the input mask for desired counters to be
 *  cleared. Note that all counters cannot be cleared.
 */

#define NV2080_CTRL_CMD_BUS_CLEAR_PEX_COUNTERS                    (0x20801814) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS_MESSAGE_ID (0x14U)

typedef struct NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS {
    NvU32 pexCounterMask;
} NV2080_CTRL_BUS_CLEAR_PEX_COUNTERS_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_FREEZE_PEX_COUNTERS
 *  This command gets the counts for different counter types.
 *
 * pexCounterMask
 *  This parameter specifies the input mask for desired counters to be
 *  freezed. Note that all counters cannot be frozen.
 * 
 * bFreezeRmCounter
 *  This parameter decides whether API will freeze it or unfreeze it.
 *  NV_TRUE for freeze and NV_FALSE for unfreeze.
 */

#define NV2080_CTRL_CMD_BUS_FREEZE_PEX_COUNTERS (0x20801815) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS {
    NvU32  pexCounterMask;
    NvBool bFreezeRmCounter;
} NV2080_CTRL_BUS_FREEZE_PEX_COUNTERS_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS
 *  This command gets the per Lane Counters and the type of errors.
 *
 * pexLaneErrorStatus
 *  This mask specifies the type of error detected on any of the Lanes.
 *
 * pexLaneCounter
 *  This array gives the counters per Lane. Each index corresponds to Lane
 *  index + 1
 */

#define NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS (0x20801816) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PEX_MAX_LANES                 16
#define NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS {
    NvU16 pexLaneErrorStatus;
    NvU8  pexLaneCounter[NV2080_CTRL_PEX_MAX_LANES];
} NV2080_CTRL_CMD_BUS_GET_PEX_LANE_COUNTERS_PARAMS;

#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_TYPE                  0x00000000
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_SYNC_HDR_CODING_ERR   0x00000001
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_SYNC_HDR_ORDER_ERR    0x00000002
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_OS_DATA_SEQ_ERR       0x00000004
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_TSX_DATA_SEQ_ERR      0x00000008
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_SKPOS_LFSR_ERR        0x00000010
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_RX_CLK_FIFO_OVERFLOW  0x00000020
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_ELASTIC_FIFO_OVERFLOW 0x00000040
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_RCVD_LINK_NUM_ERR     0x00000080
#define NV2080_CTRL_BUS_PEX_COUNTER_LANE_RCVD_LANE_NUM_ERR     0x00000100

#define NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY               (0x20801817) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS_MESSAGE_ID (0x17U)

typedef struct NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS {
    NvBool bPexLtrRegkeyOverride;
    NvBool bPexRootPortLtrSupported;
    NvBool bPexGpuLtrSupported;
    NvU16  pexLtrSnoopLatencyValue;
    NvU8   pexLtrSnoopLatencyScale;
    NvU16  pexLtrNoSnoopLatencyValue;
    NvU8   pexLtrNoSnoopLatencyScale;
} NV2080_CTRL_CMD_BUS_GET_PCIE_LTR_LATENCY_PARAMS;

#define NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY (0x20801818) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS {
    NvU16 pexLtrSnoopLatencyValue;
    NvU8  pexLtrSnoopLatencyScale;
    NvU16 pexLtrNoSnoopLatencyValue;
    NvU8  pexLtrNoSnoopLatencyScale;
} NV2080_CTRL_CMD_BUS_SET_PCIE_LTR_LATENCY_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_GET_PEX_UTIL_COUNTERS
 *  This command gets the counts for different counter types.
 *
 * pexCounterMask
 *  This parameter specifies the input mask for desired counter types.
 *
 */
#define NV2080_CTRL_BUS_PEX_UTIL_COUNTER_TX_BYTES   0x00000001
#define NV2080_CTRL_BUS_PEX_UTIL_COUNTER_RX_BYTES   0x00000002
#define NV2080_CTRL_BUS_PEX_UTIL_COUNTER_TX_L0      0x00000004
#define NV2080_CTRL_BUS_PEX_UTIL_COUNTER_RX_L0      0x00000008
#define NV2080_CTRL_BUS_PEX_UTIL_COUNTER_TX_L0S     0x00000010
#define NV2080_CTRL_BUS_PEX_UTIL_COUNTER_RX_L0S     0x00000020
#define NV2080_CTRL_BUS_PEX_UTIL_COUNTER_NON_L0_L0S 0x00000040
#define NV2080_CTRL_PEX_UTIL_MAX_COUNTER_TYPES      7

#define NV2080_CTRL_CMD_BUS_GET_PEX_UTIL_COUNTERS   (0x20801819) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS {
    NvU32 pexCounterMask;
    NvU32 pexCounters[NV2080_CTRL_PEX_UTIL_MAX_COUNTER_TYPES];
} NV2080_CTRL_BUS_GET_PEX_UTIL_COUNTERS_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_CLEAR_PEX_UTIL_COUNTER_COUNTERS
 *  This command gets the counts for different counter types.
 *
 * pexCounterMask
 *  This parameter specifies the input mask for desired counters to be
 *  cleared. Note that all counters cannot be cleared.
 * 
 * NOTE: EX_UTIL_COUNTER_UPSTREAM & NV2080_CTRL_BUS_PEX_UTIL_COUNTER_DOWNSTREAM
 *       belongs to PMU. The ctrl function will not reset nor disable/enable them.
 */
#define NV2080_CTRL_CMD_BUS_CLEAR_PEX_UTIL_COUNTERS (0x20801820) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS {
    NvU32 pexCounterMask;
} NV2080_CTRL_BUS_CLEAR_PEX_UTIL_COUNTERS_PARAMS;

#define NV2080_CTRL_CMD_BUS_GET_BFD (0x20801821) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_BFD_PARAMSARR_MESSAGE_ID" */

typedef struct NV2080_CTRL_BUS_GET_BFD_PARAMS {
    NvBool valid;
    NvU16  deviceID;
    NvU16  vendorID;
    NvU32  domain;
    NvU16  bus;
    NvU16  device;
    NvU8   function;
} NV2080_CTRL_BUS_GET_BFD_PARAMS;

#define NV2080_CTRL_BUS_GET_BFD_PARAMSARR_MESSAGE_ID (0x21U)

typedef struct NV2080_CTRL_BUS_GET_BFD_PARAMSARR {
    NV2080_CTRL_BUS_GET_BFD_PARAMS params[32];
} NV2080_CTRL_BUS_GET_BFD_PARAMSARR;

/*
 * NV2080_CTRL_CMD_BUS_GET_ASPM_DISABLE_FLAGS
 *  This command gets the following mentioned PDB Properties
 * 
 * aspmDisableFlags[] 
 *  NvBool array stores each of the properties' state. the array size can
 *  be increased as per requirement.
 *
 * NOTE: When adding more properties, increment NV2080_CTRL_ASPM_DISABLE_FLAGS_MAX_FLAGS.
 */

#define NV2080_CTRL_ASPM_DISABLE_FLAGS_L1_MASK_REGKEY_OVERRIDE                0x00000000
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_OS_RM_MAKES_POLICY_DECISIONS           0x00000001
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_BEHIND_BRIDGE                      0x00000002
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_UPSTREAM_PORT_L1_UNSUPPORTED       0x00000003
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED     0x00000004
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY   0x00000005
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_CL_ASPM_L1_CHIPSET_DISABLED            0x00000006
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_CL_ASPM_L1_CHIPSET_ENABLED_MOBILE_ONLY 0x00000007
#define NV2080_CTRL_ASPM_DISABLE_FLAGS_BIF_ENABLE_ASPM_DT_L1                  0x00000008
//append properties here

#define NV2080_CTRL_ASPM_DISABLE_FLAGS_MAX_FLAGS                              9

#define NV2080_CTRL_CMD_BUS_GET_ASPM_DISABLE_FLAGS                            (0x20801822) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS_MESSAGE_ID (0x22U)

typedef struct NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS {
    NvBool aspmDisableFlags[NV2080_CTRL_ASPM_DISABLE_FLAGS_MAX_FLAGS];
} NV2080_CTRL_BUS_GET_ASPM_DISABLE_FLAGS_PARAMS;

#define NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS (0x20801824) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS_MESSAGE_ID (0x24U)

typedef struct NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS {
    NvBool bEnable;
} NV2080_CTRL_CMD_BUS_CONTROL_PUBLIC_ASPM_BITS_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_GET_NVLINK_PEER_ID_MASK
 *
 * This command returns cached(SW only) NVLINK peer id mask. Currently, this control
 * call is only needed inside a SR-IOV enabled guest where page table management is
 * being done by the guest. Guest needs this mask to derive the peer id corresponding
 * to the peer GPU. This peer id will then be programmed inside the PTEs by guest RM.
 *
 *   nvlinkPeerIdMask[OUT]
 *      - The peer id mask is returned in this array.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_BUS_GET_NVLINK_PEER_ID_MASK (0x20801825) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_MAX_NUM_GPUS                32

#define NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS_MESSAGE_ID (0x25U)

typedef struct NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS {
    NvU32 nvlinkPeerIdMask[NV2080_CTRL_BUS_MAX_NUM_GPUS];
} NV2080_CTRL_BUS_GET_NVLINK_PEER_ID_MASK_PARAMS;

/* 
 * NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS
 * This command takes parameters eomMode, eomNblks and eomNerrs from the client
 * and then sends it out to PMU.
 */
#define NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS (0x20801826) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS_MESSAGE_ID (0x26U)

typedef struct NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS {
    NvU8 eomMode;
    NvU8 eomNblks;
    NvU8 eomNerrs;
} NV2080_CTRL_CMD_BUS_SET_EOM_PARAMETERS_PARAMS;

/* 
 * NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE
 * This command takes parameters UPHY register's address and lane from the client
 * and then sends it out to PMU.
 */
#define NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE (0x20801827) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS_MESSAGE_ID (0x27U)

typedef struct NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS {
    NvU32 regAddress;
    NvU32 laneSelectMask;
    NvU16 regValue;
} NV2080_CTRL_CMD_BUS_GET_UPHY_DLN_CFG_SPACE_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_GET_EOM_STATUS
 *
 */
#define NV2080_CTRL_CMD_BUS_GET_EOM_STATUS (0x20801828) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_MAX_NUM_LANES      32

#define NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS_MESSAGE_ID (0x28U)

typedef struct NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS {
    NvU8  eomMode;
    NvU8  eomNblks;
    NvU8  eomNerrs;
    NvU8  eomBerEyeSel;
    NvU8  eomPamEyeSel;
    NvU32 laneMask;
    NvU16 eomStatus[NV2080_CTRL_BUS_MAX_NUM_LANES];
} NV2080_CTRL_BUS_GET_EOM_STATUS_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS
 *
 * This command returns the PCIe requester atomics operation capabilities
 * of the GPU with regards to the aspect of support the client is asking for.
 *
 *
 * capType [IN]
 *  The aspect of the atomics support the client is querying atomics capability for.
 *  Supported types are defined under NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_*.
 *
 * dbdf [IN] -
 * Argument used to identify the PCIe peer endpoint. Used only for the _CAPTYPE_P2P.
 * Encoded as: domain (31:16), bus (15:8), device (7:3), function (2:0)
 *
 * atomicsCaps[OUT]
 *  Mask of supported PCIe requester atomic operations in the form of
 *  NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_*
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS (0x20801829) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS_MESSAGE_ID (0x29U)

typedef struct NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS {
    NvU32 capType;
    NvU32 dbdf;
    NvU32 atomicsCaps;
} NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS;

/*
 * Defined methods to expose atomics capability.
 *
 * NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_SYSMEM
 *  Exposes the state of atomics support between GPU and Sysmem.
 * NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_GPU
 *  Exposes the state of the GPU atomics support without taking into account PCIe topology.
 * NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_P2P
 *  Exposes the state of atomics support between the source (this GPU)
 *  and peer GPU identified by the dbdf argument.
 */
#define NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_SYSMEM 0x0
#define NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_GPU    0x1
#define NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_P2P    0x2

#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_FETCHADD_32      0:0
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_FETCHADD_32_YES (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_FETCHADD_32_NO  (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_FETCHADD_64      1:1
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_FETCHADD_64_YES (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_FETCHADD_64_NO  (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_SWAP_32          2:2
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_SWAP_32_YES     (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_SWAP_32_NO      (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_SWAP_64          3:3
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_SWAP_64_YES     (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_SWAP_64_NO      (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_32           4:4
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_32_YES      (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_32_NO       (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_64           5:5
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_64_YES      (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_64_NO       (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_128          6:6
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_128_YES     (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_CAS_128_NO      (0x00000000)

/*
 * NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS
 *
 * This command returns the supported GPU atomic operations
 * that map to the capable PCIe atomic operations from GPU to
 * coherent SYSMEM.
 *
 * capType [IN]
 *  The aspect of the atomics support the client is querying atomics capability for.
 *  Supported types are defined under NV2080_CTRL_CMD_BUS_PCIE_ATOMICS_CAPTYPE_*.
 *
 * dbdf [IN] -
 * Argument used to identify the PCIe peer endpoint. Used only for the _CAPTYPE_P2P.
 * Encoded as: domain (31:16), bus (15:8), device (7:3), function (2:0)
 *
 * atomicOp[OUT]
 *  Array of NV2080_CTRL_BUS_PCIE_GPU_ATOMICS that contains the atomic operation
 *  supported status and its attributes. The array can be
 *  indexed using one of NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_*
 *  
 */
#define NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS            (0x2080182a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_IADD           0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_IMIN           1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_IMAX           2
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_INC            3
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_DEC            4
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_IAND           5
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_IOR            6
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_IXOR           7
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_EXCH           8
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_CAS            9
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_FADD           10
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_FMIN           11
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_FMAX           12

#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT          13

/*
 * NV2080_CTRL_BUS_PCIE_GPU_ATOMIC_OP_INFO
 *
 *  Describes the support state and related attributes of a single GPU atomic op.
 *
 *  bSupported
 *   Is the GPU atomic operation natively supported by the PCIe
 *
 *  attributes
 *   Provides the attributes mask of the GPU atomic operation when supported
 *   in the form of
 *   NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_*
 */
typedef struct NV2080_CTRL_BUS_PCIE_GPU_ATOMIC_OP_INFO {
    NvBool bSupported;
    NvU32  attributes;
} NV2080_CTRL_BUS_PCIE_GPU_ATOMIC_OP_INFO;

#define NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS_MESSAGE_ID (0x2AU)

typedef struct NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS {
    NvU32                                   capType;
    NvU32                                   dbdf;
    NV2080_CTRL_BUS_PCIE_GPU_ATOMIC_OP_INFO atomicOp[NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT];
} NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS;

#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SCALAR         0:0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SCALAR_YES    1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SCALAR_NO     0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_VECTOR         1:1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_VECTOR_YES    1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_VECTOR_NO     0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_REDUCTION      2:2
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_REDUCTION_YES 1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_REDUCTION_NO  0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_32        3:3
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_32_YES   1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_32_NO    0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_64        4:4
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_64_YES   1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_64_NO    0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_128       5:5
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_128_YES  1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIZE_128_NO   0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIGNED         6:6
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIGNED_YES    1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_SIGNED_NO     0
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_UNSIGNED       7:7
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_UNSIGNED_YES  1
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_ATTRIB_UNSIGNED_NO   0

/*
 * NV2080_CTRL_CMD_BUS_GET_C2C_INFO
 *
 * This command returns the C2C links information.
 *
 *   bIsLinkUp[OUT]
 *       NV_TRUE if the C2C links are present and the links are up.
 *       The below remaining fields are valid only if return value is
 *       NV_OK and bIsLinkUp is NV_TRUE.
 *   nrLinks[OUT]
 *       Total number of C2C links that are up.
 *   maxNrLinks[OUT]
 *       Maximum number of C2C links that are supported.
 *   linkMask[OUT]
 *       Bitmask of the C2C links present and up.
 *   perLinkBwMBps[OUT]
 *       Theoretical per link bandwidth in MBps.
 *   perLinkLaneWidth[OUT]
 *       Lane width per link.
 *   remoteType[OUT]
 *       Type of the device connected to the remote end of the C2C link.
 *       Valid values are :
 *       NV2080_CTRL_BUS_GET_C2C_INFO_REMOTE_TYPE_CPU - connected to a CPU
 *       NV2080_CTRL_BUS_GET_C2C_INFO_REMOTE_TYPE_GPU - connected to another GPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *
 * Please also review the information below for additional information on
 * select fields:
 *
 *   remoteType[OUT]
 *       NV2080_CTRL_BUS_GET_C2C_INFO_REMOTE_TYPE_CPU - connected to a CPU
 *       in either self-hosted mode or externally-hostedmode.
 */

#define NV2080_CTRL_CMD_BUS_GET_C2C_INFO                            (0x2080182b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS_MESSAGE_ID (0x2BU)

typedef struct NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS {
    NvBool bIsLinkUp;
    NvU32  nrLinks;
    NvU32  maxNrLinks;
    NvU32  linkMask;
    NvU32  perLinkBwMBps;
    NvU32  perLinkLaneWidth;
    NvU32  remoteType;
} NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS;

#define NV2080_CTRL_BUS_GET_C2C_INFO_REMOTE_TYPE_CPU 1
#define NV2080_CTRL_BUS_GET_C2C_INFO_REMOTE_TYPE_GPU 2

/*
 * NV2080_CTRL_CMD_BUS_SYSMEM_ACCESS
 *
 * This command disables the GPU system memory access after quiescing the GPU,
 * or re-enables sysmem access.
 *
 *   bDisable
 *     If NV_TRUE the GPU is quiesced and system memory access is disabled .
 *     If NV_FALSE the GPU system memory access is re-enabled and the GPU is resumed.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV2080_CTRL_CMD_BUS_SYSMEM_ACCESS            (0x2080182c) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS_MESSAGE_ID (0x2CU)

typedef struct NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS {
    NvBool bDisable;
} NV2080_CTRL_BUS_SYSMEM_ACCESS_PARAMS;



/*
 * NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING
 *
 * This command sets P2P mapping.
 *
 * connectionType[IN]
 *   Connection type, one of NV2080_CTRL_CMD_BUS_SET_P2P_MAPPINGS_CONNECTION
 * peerId[IN]
 *   peerId of remote GPU from local GPU on which call is made.
 * bSpaAccessOnly[IN]
 *   SPA access only. SPA addressing mode is supported when we support ATS.
 * bUseUuid [in]
 *   Option only available for Guest RPCs and is not avaliable for external clients.
 *   Set to NV_TRUE to use remoteGpuUuid in lieu of remoteGpuId to identify target GPU.
 * remoteGpuId[IN]
 *   GPU ID of remote GPU.
 * remoteGpuUuid [in]
 *   Alternative to gpuId; used to identify target GPU for which caps are being queried.
 *   Option only available for Guest RPCs.
 *   If bUseUuid == NV_TRUE, gpuUuid is used in lieu of gpuId to identify target GPU.
 *   If bUseUuid == NV_FALSE, gpuUuid is ignored and gpuId is used by default.
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_INVALID_ARGUMENT
 *  NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING                           (0x2080182e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_INVALID   0
#define NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_NVLINK    1
#define NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_PCIE      2
#define NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_PCIE_BAR1 3
#define NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_C2C       4

#define NV2080_SET_P2P_MAPPING_UUID_LEN                               16U

#define NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_MESSAGE_ID (0x2EU)

typedef struct NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS {
    NvU32  connectionType;
    NvU32  peerId;
    NvBool bEgmPeer;
    NvBool bSpaAccessOnly;
    NvBool bUseUuid;
    NvU32  remoteGpuId;
    NvU8   remoteGpuUuid[NV2080_SET_P2P_MAPPING_UUID_LEN];
} NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_UNSET_P2P_MAPPING
 *
 * This command unsets P2P mapping.
 *
 * connectionType[IN]
 *   Connection type, one of NV2080_CTRL_CMD_BUS_SET_P2P_MAPPINGS_CONNECTION
 * peerId[IN]
 *   peerId of remote GPU from local GPU on which call is mad.
 * bUseUuid [in]
 *   Option only available for Guest RPCs and is not avaliable for external clients.
 *   Set to NV_TRUE to use remoteGpuUuid in lieu of remoteGpuId to identify target GPU.
 * remoteGpuId[IN]
 *   GPU ID of remote GPU.
 * remoteGpuUuid [in]
 *   Alternative to gpuId; used to identify target GPU for which caps are being queried.
 *   Option only available for Guest RPCs.
 *   If bUseUuid == NV_TRUE, gpuUuid is used in lieu of gpuId to identify target GPU.
 *   If bUseUuid == NV_FALSE, gpuUuid is ignored and gpuId is used by default.
 *
 * Possible status values returned are:
 *  NV_OK
 *  NV_ERR_INVALID_ARGUMENT
 *  NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_BUS_UNSET_P2P_MAPPING (0x2080182f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_MESSAGE_ID (0x2FU)

typedef struct NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS {
    NvU32  connectionType;
    NvU32  peerId;
    NvBool bUseUuid;
    NvU32  remoteGpuId;
    NvU8   remoteGpuUuid[NV2080_SET_P2P_MAPPING_UUID_LEN];
} NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS;

/*
 * NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS
 *
 * This command returns the PCIe completer atomics operation capabilities
 * of the GPU.
 *
 * atomicsCaps[OUT]
 *  Mask of supported PCIe completer atomic operations in the form of
 *  NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_*
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS (0x20801830) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_BUS_INTERFACE_ID << 8) | NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS_MESSAGE_ID (0x30U)

typedef struct NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS {
    NvU32 atomicsCaps;
} NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_PARAMS;

#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_FETCHADD_32      0:0
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_FETCHADD_32_YES (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_FETCHADD_32_NO  (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_FETCHADD_64      1:1
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_FETCHADD_64_YES (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_FETCHADD_64_NO  (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_SWAP_32          2:2
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_SWAP_32_YES     (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_SWAP_32_NO      (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_SWAP_64          3:3
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_SWAP_64_YES     (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_SWAP_64_NO      (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_32           4:4
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_32_YES      (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_32_NO       (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_64           5:5
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_64_YES      (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_64_NO       (0x00000000)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_128          6:6
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_128_YES     (0x00000001)
#define NV2080_CTRL_CMD_BUS_GET_PCIE_CPL_ATOMICS_CAPS_CAS_128_NO      (0x00000000)

