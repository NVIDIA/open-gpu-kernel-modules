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

#include "common_nvswitch.h"
#include "error_nvswitch.h"
#include "pmgr_nvswitch.h"
#include "soe/soeifcore.h"

void
nvswitch_i2c_init
(
    nvswitch_device *device
)
{
    PNVSWITCH_OBJI2C pI2c = nvswitch_os_malloc(sizeof(struct NVSWITCH_OBJI2C));

    if (pI2c == NULL)
    {
        device->pI2c = NULL;
        return;
    }
    nvswitch_os_memset(pI2c, 0, sizeof(struct NVSWITCH_OBJI2C));
    device->pI2c = pI2c;
}

void
nvswitch_i2c_destroy
(
    nvswitch_device *device
)
{
    if (device->pI2c == NULL)
    {
        return;
    }

    if (device->pI2c->soeI2CSupported)
    {
        nvswitch_os_unmap_dma_region(device->os_handle, device->pI2c->pCpuAddr, device->pI2c->dmaHandle,
                                 SOE_I2C_DMA_BUF_SIZE, NVSWITCH_DMA_DIR_BIDIRECTIONAL);
        nvswitch_os_free_contig_memory(device->os_handle, device->pI2c->pCpuAddr, SOE_I2C_DMA_BUF_SIZE);
        device->pI2c->pCpuAddr = NULL;
        device->pI2c->dmaHandle = 0;
    }

    nvswitch_os_free(device->pI2c);
    device->pI2c = NULL;
}

/*! @brief Set up a port to use a PMGR implementation.
 *
 *  @param[in]  device          NvSwitch device
 *  @param[in]  port            The port identifier for the bus.
 */
void
_nvswitch_i2c_set_port_pmgr
(
    nvswitch_device *device,
    NvU32   port
)
{
    NvU32 i;
    NvU32 device_allow_list_size;
    NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE *device_allow_list;
    PNVSWITCH_OBJI2C pI2c = device->pI2c;

    NVSWITCH_ASSERT(port < NVSWITCH_MAX_I2C_PORTS);

    pI2c->PortInfo[port] = FLD_SET_DRF(_I2C, _PORTINFO, _DEFINED, _PRESENT, pI2c->PortInfo[port]);
    pI2c->Ports[port].defaultSpeedMode = NVSWITCH_I2C_SPEED_MODE_100KHZ;

    device_allow_list = pI2c->i2c_allow_list;
    device_allow_list_size = pI2c->i2c_allow_list_size;

    for (i = 0; i < device_allow_list_size; i++)
    {
        if (port == device_allow_list[i].i2cPortLogical)
        {
            pI2c->PortInfo[port] = FLD_SET_DRF(_I2C, _PORTINFO,
                                               _ACCESS_ALLOWED, _TRUE,
                                               pI2c->PortInfo[port]);
            break;
        }
    }
}

