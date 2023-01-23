/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "export_nvswitch.h"
#include "common_nvswitch.h"
#include "error_nvswitch.h"
#include "rom_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/pmgr_lr10.h"
#include "nvswitch/lr10/dev_pmgr.h"

// Shared with LS10
void _nvswitch_i2c_set_port_pmgr(nvswitch_device *device, NvU32 port);

/*! The number of nanoseconds we will wait for slave clock stretching.
 *  Previously, this was set to 100us, but proved too
 *  short (see bug 630691) so was increased to 2ms.
 */
#define I2C_STRETCHED_LOW_TIMEOUT_NS_LR10 2000000

NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE nvswitch_i2c_device_allow_list_lr10[] =
{

};

const NvU32 nvswitch_i2c_device_allow_list_size_lr10 =
    NV_ARRAY_ELEMENTS(nvswitch_i2c_device_allow_list_lr10);

//
// PMGR functions
//

/*! 
 *  @brief Return I2c port info used in PMGR implementation.
 */
NvU32
nvswitch_i2c_get_port_info_lr10
(
    nvswitch_device *device,
    NvU32 port
)
{
    PNVSWITCH_OBJI2C pI2c = device->pI2c;

    if (port >= NVSWITCH_MAX_I2C_PORTS)
    {
        return 0;
    }
    else
    {
        return pI2c->PortInfo[port];
    }
}

//
// Pre-initialize the software & hardware state of the switch I2C & GPIO interface
//
void
nvswitch_init_pmgr_lr10
(
    nvswitch_device *device
)
{
    PNVSWITCH_OBJI2C pI2c;

    // Initialize I2C object
    nvswitch_i2c_init(device);

    pI2c = device->pI2c;

    //
    // Dynamically allocate the I2C device allowlist
    // once VBIOS table reads are implemented.
    //
    pI2c->i2c_allow_list = nvswitch_i2c_device_allow_list_lr10;
    pI2c->i2c_allow_list_size = nvswitch_i2c_device_allow_list_size_lr10;

    // Setup the 3 I2C ports
    _nvswitch_i2c_set_port_pmgr(device, NVSWITCH_I2C_PORT_I2CA);
    _nvswitch_i2c_set_port_pmgr(device, NVSWITCH_I2C_PORT_I2CB);
    _nvswitch_i2c_set_port_pmgr(device, NVSWITCH_I2C_PORT_I2CC);

}

static const NVSWITCH_GPIO_INFO nvswitch_gpio_pin_Default[] =
{
    NVSWITCH_DESCRIBE_GPIO_PIN( 0, _INSTANCE_ID0,   0, IN),          // Instance ID bit 0
    NVSWITCH_DESCRIBE_GPIO_PIN( 1, _INSTANCE_ID1,   0, IN),          // Instance ID bit 1
    NVSWITCH_DESCRIBE_GPIO_PIN( 2, _INSTANCE_ID2,   0, IN),          // Instance ID bit 2
    NVSWITCH_DESCRIBE_GPIO_PIN( 3, _INSTANCE_ID3,   0, IN),          // Instance ID bit 3
    NVSWITCH_DESCRIBE_GPIO_PIN( 4, _INSTANCE_ID4,   0, IN),          // Instance ID bit 4
    NVSWITCH_DESCRIBE_GPIO_PIN( 5, _INSTANCE_ID5,   0, IN),          // Instance ID bit 5
    NVSWITCH_DESCRIBE_GPIO_PIN( 6, _INSTANCE_ID6,   0, IN),          // Instance ID bit 6
};

static const NvU32 nvswitch_gpio_pin_Default_size = NV_ARRAY_ELEMENTS(nvswitch_gpio_pin_Default);

//
// Initialize the software state of the switch I2C & GPIO interface
// Temporarily forcing default GPIO values.
//

// TODO: This function should be updated with the board values from DCB.

void
nvswitch_init_pmgr_devices_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    PNVSWITCH_OBJI2C pI2c = device->pI2c;

    chip_device->gpio_pin = nvswitch_gpio_pin_Default;
    chip_device->gpio_pin_size = nvswitch_gpio_pin_Default_size;

    pI2c->device_list = NULL;
    pI2c->device_list_size = 0;
}

/*!
 * RM Control command to determine the physical id of the device.
 */
NvU32
nvswitch_read_physical_id_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
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
nvswitch_ctrl_i2c_indexed_lr10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams
)
{
    NvlStatus status = (-NVL_ERR_GENERIC);
    return status;
}

NvlStatus
nvswitch_get_rom_info_lr10
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom
)
{
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, SETUP,
            "ROM configuration not supported on Fmodel/RTL/emulation\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return -NVL_ERR_NOT_SUPPORTED;
}

/*!
 * Set the speed of the HW I2C controller on a given port.
 *
 * @param[in] port          The port identifying the controller.
 *
 * @param[in] speedMode     The speed mode to run at.
 */
void
nvswitch_i2c_set_hw_speed_mode_lr10
(
    nvswitch_device *device,
    NvU32 port,
    NvU32 speedMode
)
{
    NvU32 timing = DRF_DEF(_PMGR, _I2C_TIMING, _IGNORE_ACK, _DISABLE) |
                   DRF_DEF(_PMGR, _I2C_TIMING, _TIMEOUT_CHECK, _ENABLE);

    switch (speedMode)
    {
        // Default should not be hit if above layers work correctly.
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: undefined speed\n",
                __FUNCTION__);
            // Deliberate fallthrough
        case NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_100KHZ:
            timing = FLD_SET_DRF(_PMGR, _I2C_TIMING, _SCL_PERIOD, _100KHZ, timing);
            timing = FLD_SET_DRF_NUM(_PMGR, _I2C_TIMING, _TIMEOUT_CLK_CNT, NVSWITCH_I2C_SCL_CLK_TIMEOUT_100KHZ, timing);
            break;

        case NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_200KHZ:
            timing = FLD_SET_DRF(_PMGR, _I2C_TIMING, _SCL_PERIOD, _200KHZ, timing);
            timing = FLD_SET_DRF_NUM(_PMGR, _I2C_TIMING, _TIMEOUT_CLK_CNT, NVSWITCH_I2C_SCL_CLK_TIMEOUT_200KHZ, timing);
            break;

        case NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_300KHZ:
            timing = FLD_SET_DRF(_PMGR, _I2C_TIMING, _SCL_PERIOD, _300KHZ, timing);
            timing = FLD_SET_DRF_NUM(_PMGR, _I2C_TIMING, _TIMEOUT_CLK_CNT, NVSWITCH_I2C_SCL_CLK_TIMEOUT_300KHZ, timing);
            break;

        case NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_400KHZ:
            timing = FLD_SET_DRF(_PMGR, _I2C_TIMING, _SCL_PERIOD, _400KHZ, timing);
            timing = FLD_SET_DRF_NUM(_PMGR, _I2C_TIMING, _TIMEOUT_CLK_CNT, NVSWITCH_I2C_SCL_CLK_TIMEOUT_400KHZ, timing);
            break;

        case NVSWITCH_CTRL_I2C_FLAGS_SPEED_MODE_1000KHZ:
            timing = FLD_SET_DRF(_PMGR, _I2C_TIMING, _SCL_PERIOD, _1000KHZ, timing);
            timing = FLD_SET_DRF_NUM(_PMGR, _I2C_TIMING, _TIMEOUT_CLK_CNT, NVSWITCH_I2C_SCL_CLK_TIMEOUT_1000KHZ, timing);
            break;
    }

    NVSWITCH_REG_WR32(device, _PMGR, _I2C_TIMING(port), timing);
}

/*!
 * Return if I2C transactions are supported.
 *
 * @param[in] device        The NvSwitch Device.
 *
 */
NvBool
nvswitch_is_i2c_supported_lr10
(
    nvswitch_device *device
)
{
    return NV_TRUE;
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
nvswitch_i2c_is_device_access_allowed_lr10
(
    nvswitch_device *device,
    NvU32 port,
    NvU8 addr,
    NvBool bIsRead
)
{
    NvU32 i;
    NvU32 device_allow_list_size;
    NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE *device_allow_list;
    NvBool bAllow = NV_FALSE;
    PNVSWITCH_OBJI2C pI2c = device->pI2c;

    device_allow_list = pI2c->i2c_allow_list;
    device_allow_list_size = pI2c->i2c_allow_list_size;

    for (i = 0; i < device_allow_list_size; i++)
    {
        NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE i2c_device = device_allow_list[i];

        if ((port == i2c_device.i2cPortLogical) &&
            (addr == i2c_device.i2cAddress))
        {
            bAllow = bIsRead ?
                         FLD_TEST_DRF(_NVSWITCH, _I2C_DEVICE, _READ_ACCESS_LEVEL,
                                      _PUBLIC, i2c_device.i2cRdWrAccessMask) :
                         FLD_TEST_DRF(_NVSWITCH, _I2C_DEVICE, _WRITE_ACCESS_LEVEL,
                                      _PUBLIC, i2c_device.i2cRdWrAccessMask);
            break;
        }
    }

    return bAllow;
}

