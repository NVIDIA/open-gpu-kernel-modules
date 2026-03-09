/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*                                                                          *
*         GRID Displayless Object Function Definitions.                    *
*                                                                          *
\***************************************************************************/

#include "os/os.h"
#include "griddisplayless/objgriddisplayless.h"
#include "griddisplayless/griddisplayless.h"

//for unlicensed passthrough
#define GRID_DISPLAYLESS_NUM_HEADS                       1
#define GRID_DISPLAYLESS_WINDOWS_MAX_HRES                1280
#define GRID_DISPLAYLESS_WINDOWS_MAX_VRES                1024
#define GRID_DISPLAYLESS_LINUX_MAX_HRES                  2560
#define GRID_DISPLAYLESS_LINUX_MAX_VRES                  1600

#define GRID_DISPLAYLESS_GEFORCE_NUM_HEADS               1
#define GRID_DISPLAYLESS_GEFORCE_MAX_HRES                7680
#define GRID_DISPLAYLESS_GEFORCE_MAX_VRES                4320

#define GRID_DISPLAYLESS_QUADRO_NUM_HEADS                4
#define GRID_DISPLAYLESS_QUADRO_MAX_HRES                 7680
#define GRID_DISPLAYLESS_QUADRO_MAX_VRES                 4320

#define GRID_DISPLAYLESS_COMPUTE_NUM_HEADS               1
#define GRID_DISPLAYLESS_COMPUTE_MAX_HRES                4096
#define GRID_DISPLAYLESS_COMPUTE_MAX_VRES                2400

#define GRID_DISPLAYLESS_WINDOWS_MAX_PIXELS              1310720
#define GRID_DISPLAYLESS_LINUX_MAX_PIXELS                4096000
#define GRID_DISPLAYLESS_QUADRO_MAX_PIXELS               66355200
#define GRID_DISPLAYLESS_GEFORCE_MAX_PIXELS              33177600
#define GRID_DISPLAYLESS_COMPUTE_MAX_PIXELS              9216000

// EDID with Detailed Timing for following modes :
//  2560x1600 @ 59.860Hz
//  3840x2160 @ 59.998Hz
//  4096x2160 @ 60.026Hz
//  5120x2880 @ 59.992Hz
//  7680x4320 @ 59.939Hz

// EDID for DVI_D connector
const unsigned char g_GridDisplaylessDigitalEdid[] = { \
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x1A, 0x01, 0x04, 0xB5, 0x46, 0x28, 0x64, 0x06, 0x92, 0xB0, 0xA3, 0x54, 0x4C, 0x99, 0x26,
    0x0F, 0x50, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x68, 0x00, 0xA0, 0xA0, 0x40, 0x2E, 0x60, 0x30, 0x20,
    0x36, 0x00, 0x81, 0x90, 0x21, 0x00, 0x00, 0x1E, 0xD6, 0xE2, 0x00, 0x50, 0xF0, 0x60, 0x45, 0x90,
    0x08, 0x20, 0x78, 0x0C, 0x6D, 0x55, 0x21, 0x00, 0x00, 0x1A, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x46, 0x1E, 0x8C, 0x3C, 0x01, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 0x4E, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x56, 0x47, 0x58, 0x20, 0x0A, 0x20, 0x01, 0x68,
    0x70, 0x12, 0x5C, 0x03, 0x00, 0x03, 0x00, 0x14, 0x07, 0xE8, 0x00, 0x04, 0xFF, 0x0E, 0x2F, 0x02,
    0xAF, 0x80, 0x57, 0x00, 0x6F, 0x08, 0x59, 0x00, 0x07, 0x80, 0x09, 0x00, 0x03, 0x00, 0x14, 0xD1,
    0xD7, 0x00, 0x00, 0xFF, 0x0F, 0x7F, 0x00, 0x1F, 0x80, 0x1F, 0x00, 0x6F, 0x08, 0x12, 0x00, 0x02,
    0x80, 0x02, 0x00, 0x03, 0x00, 0x14, 0x80, 0x6E, 0x01, 0x04, 0xFF, 0x13, 0x9F, 0x00, 0x2F, 0x80,
    0x1F, 0x00, 0x3F, 0x0B, 0x51, 0x00, 0x02, 0x00, 0x04, 0x00, 0x03, 0x00, 0x14, 0x42, 0x27, 0x03,
    0x04, 0xFF, 0x1D, 0x4F, 0x00, 0x07, 0x80, 0x1F, 0x00, 0xDF, 0x10, 0x7A, 0x00, 0x6C, 0x00, 0x07,
    0x00, 0xAC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90
};

// EDID for Analog VGA connector. This EDID has isDigital Flag turned off.
// Other EDID fields are same as g_GridDisplaylessDigitalEdid
const unsigned char g_GridDisplaylessAnalogEdid[] = { \
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x1A, 0x01, 0x04, 0x00, 0x46, 0x28, 0x78, 0x08, 0x92, 0xB0, 0xA3, 0x54, 0x4C, 0x99, 0x26,
    0x0F, 0x50, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x68, 0x00, 0xA0, 0xA0, 0x40, 0x2E, 0x60, 0x30, 0x20,
    0x36, 0x00, 0x81, 0x90, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x1E,
    0x46, 0x1E, 0x8C, 0x3C, 0x01, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 0x4E, 0x56, 0x49, 0x44, 0x49, 0x41, 0x20, 0x56, 0x47, 0x58, 0x20, 0x0A, 0x20, 0x01, 0xCD,
    0x70, 0x12, 0x2E, 0x00, 0x00, 0x03, 0x00, 0x14, 0x07, 0xE8, 0x00, 0x04, 0xFF, 0x0E, 0x2F, 0x02,
    0xAF, 0x80, 0x57, 0x00, 0x6F, 0x08, 0x59, 0x00, 0x07, 0x80, 0x09, 0x00, 0x03, 0x00, 0x14, 0xD1,
    0xD7, 0x00, 0x00, 0xFF, 0x0F, 0x7F, 0x00, 0x1F, 0x80, 0x1F, 0x00, 0x6F, 0x08, 0x12, 0x00, 0x02,
    0x80, 0x02, 0x00, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90
};

#define GRID_DISPLAYLESS_EDID_SIZE  sizeof(g_GridDisplaylessDigitalEdid)


NV_STATUS
griddisplaylessConstructEngine_IMPL(OBJGPU *pGpu, OBJGRIDDISPLAYLESS *pGridDisplayless, ENGDESCRIPTOR engDesc)
{
    NV_STATUS rmStatus = NV_OK;

    if (!IS_VIRTUAL(pGpu))
    {
        griddisplaylessSetNumHeads(pGpu, pGridDisplayless);
        griddisplaylessSetMaxResolution(pGpu, pGridDisplayless);
        griddisplaylessSetMaxPixels(pGpu, pGridDisplayless);
    }

    pGridDisplayless->heads.maxNumHeads = GRID_DISPLAYLESS_MAX_NUM_HEADS;

    return rmStatus;
}

NvU32
griddisplaylessGetNumHeads_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless)
{
    if (pGridDisplayless)
        return pGridDisplayless->heads.numHeads;

    return 0;
}

NvU32
griddisplaylessGetMaxNumHeads_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless)
{
    if (pGridDisplayless)
        return pGridDisplayless->heads.maxNumHeads;

    return 0;
}

NvU32
griddisplaylessGetMaxHResolution_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless)
{
    if (pGridDisplayless)
        return pGridDisplayless->maxResolution.maxHResolution;

    return 0;
}

NvU32
griddisplaylessGetMaxVResolution_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless)
{
    if (pGridDisplayless)
        return pGridDisplayless->maxResolution.maxVResolution;

    return 0;
}

NvBool
griddisplaylessIsDisplayActive_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless, NvU32 headIndex)
{
    if (pGridDisplayless)
        return pGridDisplayless->displayActive[headIndex];

    return NV_FALSE;
}

NV_STATUS
griddisplaylessUpdateHeadNumInfo_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless, NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS *pHeadNumParams)
{
    if (pGridDisplayless)
    {
        portMemCopy(&pGridDisplayless->heads, sizeof(NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS), pHeadNumParams, sizeof(NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS));
        return NV_OK;
    }

    return NV_ERR_INVALID_POINTER;
}

NV_STATUS
griddisplaylessUpdateMaxResInfo_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless, NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS *pMaxResParams)
{
    if (pGridDisplayless)
    {
        portMemCopy(&pGridDisplayless->maxResolution, sizeof(NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS), pMaxResParams, sizeof(NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS));
        return NV_OK;
    }

    return NV_ERR_INVALID_POINTER;
}

NV_STATUS
griddisplaylessUpdateDisplayActive_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless, NvU32 headIndex, NvBool displayActiveStatus)
{
    if (pGridDisplayless)
    {
        if (headIndex >= GRID_DISPLAYLESS_MAX_NUM_HEADS)
        {
            return NV_ERR_INVALID_INDEX;
        }
        pGridDisplayless->displayActive[headIndex] = displayActiveStatus;
        return NV_OK;
    }

    return NV_ERR_INVALID_POINTER;
}

NV_STATUS
griddisplaylessUpdateMaxPixels_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless, NvU64 maxPixels)
{
    if (pGridDisplayless)
    {
        pGridDisplayless->maxPixels = maxPixels;
        return NV_OK;
    }

    return NV_ERR_INVALID_POINTER;
}

NvU64
griddisplaylessGetMaxPixels_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless)
{
    if (pGridDisplayless)
        return pGridDisplayless->maxPixels;

    return 0;
}

NvBool
griddisplaylessVirtualDisplayIsActive_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless)
{
    if (pGridDisplayless)
    {
        NvU32 i, numHeads;

        numHeads = griddisplaylessGetNumHeads(pGridDisplayless);
        for (i = 0; i < numHeads; i++)
        {
            if (griddisplaylessIsDisplayActive(pGridDisplayless, i))
            {
                return NV_TRUE;
            }
        }

        return NV_FALSE;
    }

    return NV_FALSE;
}

NvBool
griddisplaylessVirtualDisplayIsConnected_IMPL(OBJGRIDDISPLAYLESS *pGridDisplayless)
{
    if (pGridDisplayless)
    {
        return griddisplaylessGetNumHeads(pGridDisplayless) > 0;
    }

    return NV_FALSE;
}

NV_STATUS griddisplaylessConstruct_IMPL
(
    Griddisplayless *pGriddisplayless,
    CALL_CONTEXT    *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU        *pGpu = GPU_RES_GET_GPU(pGriddisplayless);
    NV_STATUS      rmStatus = NV_OK;

    // NVA083_GRID_DISPLAYLESS is only supported on GRID boards with
    // Display engine NOT present.
    NV_CHECK_OR_RETURN(LEVEL_NOTICE, gpuIsGridDisplaylessClassSupported(pGpu),
                      NV_ERR_NOT_SUPPORTED);

    return rmStatus;
}

NV_STATUS 
griddisplaylessGetDefaultEDID_IMPL
(
    OBJGRIDDISPLAYLESS *pGridDisplayless,
    NvU8   connectorType,
    NvU32 *pEdidSize,
    NvU8  *pEdidBuffer
)
{
    NV_STATUS rmStatus = NV_OK;

    if (pEdidSize == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Parameter validation failed. NULL buffer passed.\n");
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    if ((*pEdidSize) == 0)
    {
        // Just update with the actual size of Edid buffer required and return.
        goto done;
    }
    if ((*pEdidSize) < GRID_DISPLAYLESS_EDID_SIZE)
    {
        // Buffer of insufficient size passed.
        NV_PRINTF(LEVEL_ERROR, "Edid buffer of insufficient size passed\n");
        rmStatus = NV_ERR_BUFFER_TOO_SMALL;
        goto done;
    }

    // check for NULL pEdidBuffer parameter passed
    if (pEdidBuffer == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Parameter validation failed. NULL buffer passed.\n");
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    if (connectorType == NVA083_CTRL_CMD_CONNECTOR_INFO_TYPE_DVI_D)
    {
        portMemCopy(pEdidBuffer, GRID_DISPLAYLESS_EDID_SIZE, (void *)g_GridDisplaylessDigitalEdid, GRID_DISPLAYLESS_EDID_SIZE);
    }
    else if (connectorType == NVA083_CTRL_CMD_CONNECTOR_INFO_TYPE_VGA_15_PIN)
    {
        portMemCopy(pEdidBuffer, GRID_DISPLAYLESS_EDID_SIZE, (void *)g_GridDisplaylessAnalogEdid, GRID_DISPLAYLESS_EDID_SIZE);
    }

done:
    // This is expected along with the return value
    if (pEdidSize)
    {
        *pEdidSize = GRID_DISPLAYLESS_EDID_SIZE;
    }
    return rmStatus;
}

void
griddisplaylessSetNumHeads_IMPL
(
    OBJGPU *pGpu,
    OBJGRIDDISPLAYLESS *pGridDisplayless
)
{
    pGridDisplayless->heads.numHeads = GRID_DISPLAYLESS_NUM_HEADS;
}

void
griddisplaylessSetMaxResolution_IMPL
(
    OBJGPU *pGpu,
    OBJGRIDDISPLAYLESS *pGridDisplayless
)
{
    pGridDisplayless->maxResolution.maxHResolution = GRID_DISPLAYLESS_LINUX_MAX_HRES;
    pGridDisplayless->maxResolution.maxVResolution = GRID_DISPLAYLESS_LINUX_MAX_VRES;
}

void
griddisplaylessSetMaxPixels_IMPL
(
    OBJGPU *pGpu,
    OBJGRIDDISPLAYLESS *pGridDisplayless
)
{
    pGridDisplayless->maxPixels = GRID_DISPLAYLESS_LINUX_MAX_PIXELS;
}
