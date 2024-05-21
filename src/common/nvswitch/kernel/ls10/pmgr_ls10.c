/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "boards_nvswitch.h"
#include "regkey_nvswitch.h"
#include "ls10/ls10.h"
#include "ls10/pmgr_ls10.h"
#include "error_nvswitch.h"
#include "pmgr_nvswitch.h"
#include "rom_nvswitch.h"
#include "export_nvswitch.h"
#include "soe/soe_nvswitch.h"
#include "soe/soeifcore.h"

#include "nvswitch/ls10/dev_pmgr.h"

static NvBool
_nvswitch_i2c_ports_priv_locked_ls10
(
    nvswitch_device *device
)
{
    NvU32 regVal;

    regVal = NVSWITCH_REG_RD32(device, _PMGR, _I2C_PRIV_LEVEL_MASK(NVSWITCH_I2C_PORT_I2CA));
    if (FLD_TEST_DRF(_PMGR, _I2C_PRIV_LEVEL_MASK, _WRITE_PROTECTION_LEVEL0, _DISABLE, regVal))
    {
        return NV_TRUE;
    }

    regVal = NVSWITCH_REG_RD32(device, _PMGR, _I2C_PRIV_LEVEL_MASK(NVSWITCH_I2C_PORT_I2CB));
    if (FLD_TEST_DRF(_PMGR, _I2C_PRIV_LEVEL_MASK, _WRITE_PROTECTION_LEVEL0, _DISABLE, regVal))
    {
        return NV_TRUE;
    }

    regVal = NVSWITCH_REG_RD32(device, _PMGR, _I2C_PRIV_LEVEL_MASK(NVSWITCH_I2C_PORT_I2CC));
    if (FLD_TEST_DRF(_PMGR, _I2C_PRIV_LEVEL_MASK, _WRITE_PROTECTION_LEVEL0, _DISABLE, regVal))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NvlStatus
_nvswitch_i2c_init_soe_ls10
(
    nvswitch_device *device
)
{
    NvlStatus   ret;
    PNVSWITCH_OBJI2C pI2c;

    pI2c = device->pI2c;

    if (!nvswitch_is_soe_supported(device))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    // Setup DMA
    ret = nvswitch_os_alloc_contig_memory(device->os_handle, &pI2c->pCpuAddr, SOE_I2C_DMA_BUF_SIZE,
                                          (device->dma_addr_width == 32));
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: nvswitch_os_alloc_contig_memory returned %d\n", __FUNCTION__,  ret);
        return ret;
    }

    nvswitch_os_memset(pI2c->pCpuAddr, 0, SOE_I2C_DMA_BUF_SIZE);

    ret = nvswitch_os_map_dma_region(device->os_handle, pI2c->pCpuAddr, &pI2c->dmaHandle,
                                     SOE_I2C_DMA_BUF_SIZE, NVSWITCH_DMA_DIR_BIDIRECTIONAL);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: nvswitch_os_map_dma_region returned %d\n", __FUNCTION__,  ret);
        nvswitch_os_free_contig_memory(device->os_handle, pI2c->pCpuAddr, SOE_I2C_DMA_BUF_SIZE);
        pI2c->pCpuAddr = NULL;
        return ret;
    }

    return NVL_SUCCESS;
}

/*! @brief Set up a port to use a PMGR implementation.
 *
 *  @param[in]  device          NvSwitch device
 *  @param[in]  port            The port identifier for the bus.
 */
static void
_nvswitch_i2c_set_port_pmgr_ls10
(
    nvswitch_device *device,
    NvU32   port
)
{
    PNVSWITCH_OBJI2C pI2c = device->pI2c;

    NVSWITCH_ASSERT(port < NVSWITCH_MAX_I2C_PORTS);

    pI2c->PortInfo[port] = FLD_SET_DRF(_I2C, _PORTINFO, _DEFINED, _PRESENT, pI2c->PortInfo[port]);
    pI2c->Ports[port].defaultSpeedMode = NVSWITCH_I2C_SPEED_MODE_100KHZ;
    pI2c->PortInfo[port] = FLD_SET_DRF(_I2C, _PORTINFO,
                                       _ACCESS_ALLOWED, _TRUE,
                                       pI2c->PortInfo[port]);
}

//
// Pre-initialize the software & hardware state of the switch I2C & GPIO interface
//
void
nvswitch_init_pmgr_ls10
(
    nvswitch_device *device
)
{
    PNVSWITCH_OBJI2C pI2c;

    // Initialize I2C object
    nvswitch_i2c_init(device);

    pI2c = device->pI2c;

    if (pI2c != NULL)
    {
        pI2c->kernelI2CSupported = NV_TRUE;
        pI2c->soeI2CSupported = NV_TRUE;

        if (_nvswitch_i2c_ports_priv_locked_ls10(device))
        {
            NVSWITCH_PRINT(device, WARN, "%s: I2C ports priv locked!\n", __FUNCTION__);
            pI2c->kernelI2CSupported = NV_FALSE;
        }          
        
        if (_nvswitch_i2c_init_soe_ls10(device) != NVL_SUCCESS)
        {
            pI2c->soeI2CSupported = NV_FALSE;
        }

        // Setup the 3 I2C ports
        _nvswitch_i2c_set_port_pmgr_ls10(device, NVSWITCH_I2C_PORT_I2CA);
        _nvswitch_i2c_set_port_pmgr_ls10(device, NVSWITCH_I2C_PORT_I2CB);
        _nvswitch_i2c_set_port_pmgr_ls10(device, NVSWITCH_I2C_PORT_I2CC);
    }

}

static const NVSWITCH_GPIO_INFO nvswitch_gpio_pin_Default[] =
{
    NVSWITCH_DESCRIBE_GPIO_PIN( 0, _INSTANCE_ID0,   0, IN),          // Instance ID bit 0
    NVSWITCH_DESCRIBE_GPIO_PIN( 1, _INSTANCE_ID1,   0, IN),          // Instance ID bit 1
    NVSWITCH_DESCRIBE_GPIO_PIN( 2, _INSTANCE_ID2,   0, IN),          // Instance ID bit 2
};

static const NvU32 nvswitch_gpio_pin_Default_size = NV_ARRAY_ELEMENTS(nvswitch_gpio_pin_Default);

static const NVSWITCH_GPIO_INFO nvswitch_gpio_pin_4300[] =
{
    NVSWITCH_DESCRIBE_GPIO_PIN( 0, _INSTANCE_ID0,   0, IN),          // Instance ID bit 0
    NVSWITCH_DESCRIBE_GPIO_PIN( 1, _INSTANCE_ID1,   0, IN),          // Instance ID bit 1
    NVSWITCH_DESCRIBE_GPIO_PIN( 2, _INSTANCE_ID2,   0, IN),          // Instance ID bit 2
    NVSWITCH_DESCRIBE_GPIO_PIN( 6, _INSTANCE_ID3,   0, IN),          // Instance ID bit 3
    NVSWITCH_DESCRIBE_GPIO_PIN( 7, _INSTANCE_ID4,   0, IN),          // Instance ID bit 4
};
static const NvU32 nvswitch_gpio_pin_4300_size = NV_ARRAY_ELEMENTS(nvswitch_gpio_pin_4300);

//
// Initialize the software state of the switch I2C & GPIO interface
// Temporarily forcing default GPIO values.
//

// TODO: This function should be updated with the board values from DCB.

void
nvswitch_init_pmgr_devices_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    PNVSWITCH_OBJI2C pI2c = device->pI2c;
    NvlStatus retval;
    NvU16 boardId;

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        // GPIOs not modelled on non-silicon
        chip_device->gpio_pin = NULL;
        chip_device->gpio_pin_size = 0;
    }
    else
    {
        retval = nvswitch_get_board_id(device, &boardId);
        if (retval == NVL_SUCCESS &&
            boardId == NVSWITCH_BOARD_LS10_4300_0000_895)
        {
            chip_device->gpio_pin = nvswitch_gpio_pin_4300;
            chip_device->gpio_pin_size = nvswitch_gpio_pin_4300_size;
        }
        else
        {
            chip_device->gpio_pin = nvswitch_gpio_pin_Default;
            chip_device->gpio_pin_size = nvswitch_gpio_pin_Default_size;
        }
    }

    pI2c->device_list = NULL;
    pI2c->device_list_size = 0;
}

NvlStatus
nvswitch_get_rom_info_ls10
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom
)
{
    NVSWITCH_PRINT(device, WARN, "%s: Function not implemented\n", __FUNCTION__);
    return NVL_SUCCESS;
}

/*!
 * RM Control command to determine the physical id of the device.
 */
NvU32
nvswitch_read_physical_id_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 physical_id = 0;
    NvU32 data;
    NvU32 idx_gpio;
    NvU32 input_inv;
    NvU32 function_offset;

    for (idx_gpio = 0; idx_gpio < chip_device->gpio_pin_size; idx_gpio++)
    {
        if ((chip_device->gpio_pin[idx_gpio].function >= NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID0) &&
            (chip_device->gpio_pin[idx_gpio].function <= NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID6))
        {
            if (chip_device->gpio_pin[idx_gpio].misc == NVSWITCH_GPIO_ENTRY_MISC_IO_INV_IN)
            {
                input_inv = NV_PMGR_GPIO_INPUT_CNTL_1_INV_YES;
            }
            else
            {
                input_inv = NV_PMGR_GPIO_INPUT_CNTL_1_INV_NO;
            }

            NVSWITCH_REG_WR32(device, _PMGR, _GPIO_INPUT_CNTL_1,
                DRF_NUM(_PMGR, _GPIO_INPUT_CNTL_1, _PINNUM, chip_device->gpio_pin[idx_gpio].pin) |
                DRF_NUM(_PMGR, _GPIO_INPUT_CNTL_1, _INV, input_inv) |
                DRF_DEF(_PMGR, _GPIO_INPUT_CNTL_1, _BYPASS_FILTER, _NO));

            data = NVSWITCH_REG_RD32(device, _PMGR, _GPIO_INPUT_CNTL_1);
            function_offset = chip_device->gpio_pin[idx_gpio].function -
                          NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID0;
            physical_id |=
                (DRF_VAL(_PMGR, _GPIO_INPUT_CNTL_1, _READ, data) << function_offset);
        }
    }

    NVSWITCH_PRINT(device, SETUP, "%s Device position Id = 0x%x\n", __FUNCTION__, physical_id);

    return physical_id;
}

/*!
 * RM Control command to perform indexed I2C.
 */
NvlStatus
nvswitch_ctrl_i2c_indexed_ls10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams
)
{
    PNVSWITCH_OBJI2C pI2c;

    pI2c = device->pI2c;

    if (pI2c == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (pParams == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    // SW I2C only supported by kernel driver
    if (device->regkeys.force_kernel_i2c == NV_SWITCH_REGKEY_FORCE_KERNEL_I2C_ENABLE ||
        FLD_TEST_DRF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _SW, pParams->flags))
    {
        if (!pI2c->kernelI2CSupported)
        {
            return -NVL_ERR_NOT_SUPPORTED;
        }
        return nvswitch_ctrl_i2c_indexed_lr10(device, pParams);
    }

    if (pParams->port == NVSWITCH_I2C_PORT_I2CA)
    {
        pParams->flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _100KHZ, pParams->flags);
    }

    if (pI2c->soeI2CSupported)
    {
        return soeI2CAccess_HAL(device, pParams);
    }

    return -NVL_ERR_NOT_SUPPORTED;
}

/*!
 * Return if I2C transactions can be supported.
 *
 * @param[in] device        The NvSwitch Device.
 *
 */
NvBool
nvswitch_is_i2c_supported_ls10
(
    nvswitch_device *device
)
{
    return ((device->pI2c != NULL) && 
            (device->pI2c->soeI2CSupported || device->pI2c->kernelI2CSupported));
}

/*!
 * Return if I2C device and port is allowed access
 *
 * @param[in] device        The NvSwitch Device.
 * @param[in] port          The I2C Port.
 * @param[in] addr          The I2C device to access.
 * @param[in] bIsRead       Boolean if I2C transaction is a read.
 *
 */
NvBool
nvswitch_i2c_is_device_access_allowed_ls10
(
    nvswitch_device *device,
    NvU32 port,
    NvU8 addr,
    NvBool bIsRead
)
{
    // Check will be performed in SOE
    return NV_TRUE;
}

/*! 
 *  @brief Return I2c port info used in PMGR implementation.
 */
NvU32
nvswitch_i2c_get_port_info_ls10
(
    nvswitch_device *device,
    NvU32 port
)
{
    NvU16 boardId;
    NvlStatus status;
  
    status = nvswitch_get_board_id(device, &boardId);
    if (status != NVL_SUCCESS)
    {
        return 0;
    }

    //
    // Board has no devices we care about on I2C port C
    // Bug 4312082
    //
    if ((boardId == NVSWITCH_BOARD_LS10_4262_0000_895) &&
        (port == NVSWITCH_I2C_PORT_I2CC))
    {
        return 0;
    }
    return nvswitch_i2c_get_port_info_lr10(device, port);
}
