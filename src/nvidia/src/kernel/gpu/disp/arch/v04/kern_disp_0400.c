/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"

#include "disp/v04_00/dev_disp.h"

#include "ctrl/ctrl2080/ctrl2080gpio.h"
#include "core/locks.h"

/*!
 * @brief Get the VGA workspace base address, if valid.
 *
 */
NvBool
kdispGetVgaWorkspaceBase_v04_00
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU64 *pOffset
)
{
    NvU32 vgaReg = GPU_REG_RD32(pGpu, NV_PDISP_VGA_WORKSPACE_BASE);

    if (FLD_TEST_DRF(_PDISP, _VGA_WORKSPACE_BASE, _STATUS, _VALID, vgaReg))
    {
        *pOffset = GPU_DRF_VAL(_PDISP, _VGA_WORKSPACE_BASE, _ADDR, vgaReg) << 16;
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NV_STATUS setSliLinkGpioSwControl(OBJGPU *pGpu,
    NvU32 pinSet, NvU32 *pGpioFunction, NvU32 *pGpioPin, NvBool *pGpioDirection)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL_PARAMS params = {0};
    params.pinSet = pinSet;

    status = pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_DISP_SET_SLI_LINK_GPIO_SW_CONTROL,
                            &params,
                            sizeof(params));

    if (status == NV_OK)
    {
        *pGpioFunction = params.gpioFunction;
        *pGpioPin = params.gpioPin;
        *pGpioDirection = params.gpioDirection;
    }

    return status;
}

static NV_STATUS programGpioDirection(OBJGPU *pGpu, NvU32 gpioPin, NvBool bInput)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_GPIO_PROGRAM_DIRECTION_PARAMS params = {0};
    params.gpioPin = gpioPin;
    params.bInput = bInput;

    status = pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPIO_PROGRAM_DIRECTION,
                            &params,
                            sizeof(params));
    return status;
}

static NV_STATUS programGpioOutput(OBJGPU *pGpu, NvU32 gpioPin, NvU32 value)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_GPIO_PROGRAM_OUTPUT_PARAMS params = {0};
    params.gpioPin = gpioPin;
    params.value = value;

    status = pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPIO_PROGRAM_OUTPUT,
                            &params,
                            sizeof(params));
    return status;
}

static NV_STATUS readGpioInput(OBJGPU *pGpu, NvU32 gpioPin, NvU32 *value)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_GPIO_READ_INPUT_PARAMS params = {0};
    params.gpioPin = gpioPin;

    status = pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPIO_READ_INPUT,
                            &params,
                            sizeof(params));

    if (status == NV_OK)
        *value = params.value;

    return status;
}

static NV_STATUS activateHwFunction(OBJGPU *pGpu, NvU32 gpioPin, NvU32 function)
{
    NV_STATUS status = NV_OK;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION_PARAMS params = {0};
    params.pin = gpioPin;
    params.function = function;

    status = pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GPIO_ACTIVATE_HW_FUNCTION,
                            &params,
                            sizeof(params));

    return status;
}

#define GPIO_INVALID_PIN 0xff

NV_STATUS kdispDetectSliLink_v04_00(KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 parentPinSet, NvU32 childPinSet)
{
    // Sanity check, this function should be called with pDisp of Parent GPU
    NV_ASSERT(pKernelDisplay == GPU_GET_KERNEL_DISPLAY(pParentGpu));

    NvU32 parentGpioFunction = 0;
    NvU32 childGpioFunction = 0;
    NvBool parentGpioDirection = NV_FALSE;
    NvBool childGpioDirection = NV_FALSE;

    NvU32 parentGpioPin = GPIO_INVALID_PIN;
    NvU32 childGpioPin  = GPIO_INVALID_PIN;
    NvU32 value         = 0;

    NV_STATUS rmStatus;

    // Check the  pinset info, needs to have only one bit set
    if (!ONEBITSET(parentPinSet) || !ONEBITSET(childPinSet))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NvU32 parentGpuLockMask = 0, childGpuLockMask = 0;
    if (IS_GSP_CLIENT(pParentGpu) && !rmDeviceGpuLockIsOwner(pParentGpu->gpuInstance))
        NV_ASSERT_OK_OR_GOTO(rmStatus, rmGpuGroupLockAcquire(pParentGpu->gpuInstance,
                                                       GPU_LOCK_GRP_SUBDEVICE,
                                                       GPUS_LOCK_FLAGS_NONE,
                                                       RM_LOCK_MODULES_RPC,
                                                       &parentGpuLockMask), done);

    if (IS_GSP_CLIENT(pChildGpu) && !rmDeviceGpuLockIsOwner(pChildGpu->gpuInstance))
        NV_ASSERT_OK_OR_GOTO(rmStatus, rmGpuGroupLockAcquire(pChildGpu->gpuInstance,
                                                       GPU_LOCK_GRP_SUBDEVICE,
                                                       GPUS_LOCK_FLAGS_NONE,
                                                       RM_LOCK_MODULES_RPC,
                                                       &childGpuLockMask), done);

    // Set the gpio on parent gpu to sw control mode(aka normal) for twiddling
    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_INFO,
        setSliLinkGpioSwControl(pParentGpu, parentPinSet,
            &parentGpioFunction, &parentGpioPin, &parentGpioDirection), done);

    NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_INFO,
        setSliLinkGpioSwControl(pChildGpu, childPinSet,
            &childGpioFunction, &childGpioPin, &childGpioDirection), done);

    rmStatus = NV_ERR_NO_VALID_PATH;

    // Program the GPIO direction on parent and child
    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioDirection(pParentGpu, parentGpioPin, NV_FALSE), done);
    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioDirection(pChildGpu, childGpioPin, NV_TRUE), done);

    // Toggle GPIO on parent, and read from child
    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioOutput(pParentGpu, parentGpioPin, 1), done);
    NV_ASSERT_OK_OR_GOTO(rmStatus, readGpioInput(pChildGpu, childGpioPin, &value), done);
    NV_ASSERT_OR_GOTO(value == 1, done);

    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioOutput(pParentGpu, parentGpioPin, 0), done);
    NV_ASSERT_OK_OR_GOTO(rmStatus, readGpioInput(pChildGpu, childGpioPin, &value), done);
    NV_ASSERT_OR_GOTO(value == 0, done);

    // Program the GPIO direction on parent and child
    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioDirection(pParentGpu, parentGpioPin, NV_TRUE), done);
    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioDirection(pChildGpu, childGpioPin, NV_FALSE), done);

    // Toggle GPIO on child, and read from parent
    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioOutput(pChildGpu, childGpioPin, 1), done);
    NV_ASSERT_OK_OR_GOTO(rmStatus, readGpioInput(pParentGpu, parentGpioPin, &value), done);
    NV_ASSERT_OR_GOTO(value == 1, done);

    NV_ASSERT_OK_OR_GOTO(rmStatus, programGpioOutput(pChildGpu, childGpioPin, 0), done);
    NV_ASSERT_OK_OR_GOTO(rmStatus, readGpioInput(pParentGpu, parentGpioPin, &value), done);
    NV_ASSERT_OR_GOTO(value == 0, done);

    // Link in both direction is verified
    rmStatus = NV_OK;

done:
    // Set the GPIOs on both GPUs to original direction and hw function state
    if (parentGpioPin != GPIO_INVALID_PIN)
    {
        NV_ASSERT_OK(programGpioDirection(pParentGpu, parentGpioPin, parentGpioDirection));
        // restore the gpio to hw control mode. (aka non-normal mode)
        NV_ASSERT_OK(activateHwFunction(pParentGpu, parentGpioPin, parentGpioFunction));
    }

    if (childGpioPin != GPIO_INVALID_PIN)
    {
        NV_ASSERT_OK(programGpioDirection(pChildGpu, childGpioPin, childGpioDirection));
        // restore the gpio to hw control mode. (aka non-normal mode)
        NV_ASSERT_OK(activateHwFunction(pChildGpu, childGpioPin, childGpioFunction));
    }

    if (parentGpuLockMask != 0)
        rmGpuGroupLockRelease(parentGpuLockMask, GPUS_LOCK_FLAGS_NONE);

    if (childGpuLockMask != 0)
        rmGpuGroupLockRelease(childGpuLockMask, GPUS_LOCK_FLAGS_NONE);

    return rmStatus;
}
