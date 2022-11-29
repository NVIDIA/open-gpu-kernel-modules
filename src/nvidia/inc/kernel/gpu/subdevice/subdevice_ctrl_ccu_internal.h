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

    RMCTRL_EXPORT(NV2080_CTRL_CMD_INTERNAL_CCU_MAP,
                  RMCTRL_FLAGS(KERNEL_PRIVILEGED, ROUTE_TO_PHYSICAL, INTERNAL))
    NV_STATUS subdeviceCtrlCmdCcuMap(Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS *pParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_INTERNAL_CCU_UNMAP,
                  RMCTRL_FLAGS(KERNEL_PRIVILEGED, ROUTE_TO_PHYSICAL, INTERNAL))
    NV_STATUS subdeviceCtrlCmdCcuUnmap(Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS *pParams);

    RMCTRL_EXPORT(NV2080_CTRL_CMD_INTERNAL_CCU_SET_STREAM_STATE,
                  RMCTRL_FLAGS(KERNEL_PRIVILEGED, ROUTE_TO_PHYSICAL, INTERNAL))
    NV_STATUS subdeviceCtrlCmdCcuSetStreamState(Subdevice *pSubdevice, NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS *pParams);

