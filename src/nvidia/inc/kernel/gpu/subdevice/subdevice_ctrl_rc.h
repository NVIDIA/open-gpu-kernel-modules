/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_READ_VIRTUAL_MEM,
                  RMCTRL_FLAGS(KERNEL_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdRcReadVirtualMem(Subdevice *pSubdevice,
        NV2080_CTRL_RC_READ_VIRTUAL_MEM_PARAMS *pReadVirtMemParam);

    //
    // Note that if SMC is enabled, mig/monitor capability must be acquired
    // to query Xids
    //
    // These two calls must be NON_PRIVILEGED, and should NOT ROUTE_TO_PHYSICAL.
    // MIG monitor capability checks are performed on kernel side and the calls
    // are manually RPCed to physical RM.
    //
    // Physical RM checks if the call it received was an internal call. In
    // future, this should be migrated to a separate internal ctrl cmd.
    //
    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_GET_ERROR_COUNT,
                  RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdRcGetErrorCount(Subdevice *pSubdevice,
        NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pErrorCount);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_GET_ERROR_V2, RMCTRL_FLAGS(NON_PRIVILEGED))
    NV_STATUS subdeviceCtrlCmdRcGetErrorV2(Subdevice *pSubdevice,
        NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pErrorParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_SET_CLEAN_ERROR_HISTORY,
                  RMCTRL_FLAGS(PRIVILEGED, ROUTE_TO_PHYSICAL))
    NV_STATUS subdeviceCtrlCmdRcSetCleanErrorHistory(Subdevice *pSubdevice);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_GET_WATCHDOG_INFO,
                  RMCTRL_FLAGS(NON_PRIVILEGED, GPU_LOCK_DEVICE_ONLY, API_LOCK_READONLY))
    NV_STATUS subdeviceCtrlCmdRcGetWatchdogInfo(Subdevice *pSubdevice,
        NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_DISABLE_WATCHDOG,
                  RMCTRL_FLAGS(NON_PRIVILEGED, GPU_LOCK_DEVICE_ONLY))
    NV_STATUS subdeviceCtrlCmdRcDisableWatchdog(Subdevice *pSubdevice);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_SOFT_DISABLE_WATCHDOG,
                  RMCTRL_FLAGS(NON_PRIVILEGED, GPU_LOCK_DEVICE_ONLY))
    NV_STATUS subdeviceCtrlCmdRcSoftDisableWatchdog(Subdevice *pSubdevice);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_ENABLE_WATCHDOG,
                  RMCTRL_FLAGS(NON_PRIVILEGED, GPU_LOCK_DEVICE_ONLY))
    NV_STATUS subdeviceCtrlCmdRcEnableWatchdog(Subdevice *pSubdevice);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_RC_RELEASE_WATCHDOG_REQUESTS,
                  RMCTRL_FLAGS(NON_PRIVILEGED, GPU_LOCK_DEVICE_ONLY))
    NV_STATUS subdeviceCtrlCmdRcReleaseWatchdogRequests(Subdevice *pSubdevice);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_INTERNAL_RC_WATCHDOG_TIMEOUT,
                  RMCTRL_FLAGS(KERNEL_PRIVILEGED, INTERNAL, ROUTE_TO_PHYSICAL))
    NV_STATUS subdeviceCtrlCmdInternalRcWatchdogTimeout(Subdevice *pSubdevice);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_SET_RC_RECOVERY,
                  RMCTRL_FLAGS(PRIVILEGED, ROUTE_TO_PHYSICAL))
    NV_STATUS subdeviceCtrlCmdSetRcRecovery(Subdevice *pSubdevice,
        NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_GET_RC_RECOVERY,
                  RMCTRL_FLAGS(PRIVILEGED, ROUTE_TO_PHYSICAL))
    NV_STATUS subdeviceCtrlCmdGetRcRecovery(Subdevice *pSubdevice,
        NV2080_CTRL_CMD_RC_RECOVERY_PARAMS *pRcRecovery);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_GET_RC_INFO,
                  RMCTRL_FLAGS(PRIVILEGED, ROUTE_TO_PHYSICAL))
    NV_STATUS subdeviceCtrlCmdGetRcInfo(Subdevice *pSubdevice,
        NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_SET_RC_INFO,
                  RMCTRL_FLAGS(PRIVILEGED, ROUTE_TO_PHYSICAL))
    NV_STATUS subdeviceCtrlCmdSetRcInfo(Subdevice *pSubdevice,
        NV2080_CTRL_CMD_RC_INFO_PARAMS *pParams);

