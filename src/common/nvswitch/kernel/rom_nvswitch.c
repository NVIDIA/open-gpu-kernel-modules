/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ctrl_dev_nvswitch.h"
#include "rom_nvswitch.h"
#include "common_nvswitch.h"
#include "haldef_nvswitch.h"

static NvU8
_nvswitch_calculate_checksum
(
    NvU8 *data,
    NvU32 size
)
{
    NvU32 i;
    NvU8 checksum = 0;

    for (i = 0; i < size; i++)
    {
        checksum += data[i];
    }
    return -checksum;
}

static NvlStatus
_nvswitch_read_rom_bytes
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NvU32   offset,
    NvU8    *buffer,
    NvU32   buffer_size
)
{
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS  i2cIndexed = {0};
    NvU32   i;
    NvlStatus retval;

    if (offset + buffer_size > (NvU32)(1 << eeprom->index_size))
    {
        NVSWITCH_PRINT(device, SETUP,
            "EEPROM offset 0x%x..0x%x out of range\n",
            offset, offset + buffer_size - 1);
        return -NVL_BAD_ARGS;
    }

    if (buffer_size > NVSWITCH_CTRL_I2C_MESSAGE_LENGTH_MAX)
    {
        NVSWITCH_PRINT(device, SETUP,
            "EEPROM read buffer (0x%x bytes) larger than max (0x%x bytes)\n",
            buffer_size, NVSWITCH_CTRL_I2C_MESSAGE_LENGTH_MAX);
        return -NVL_BAD_ARGS;
    }

    i2cIndexed.port = (NvU8)eeprom->i2c_port;
    i2cIndexed.bIsRead = NV_TRUE;
    i2cIndexed.address = (NvU16)eeprom->i2c_address;
    i2cIndexed.flags =
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _START,        _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _RESTART,      _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _STOP,         _SEND) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _ADDRESS_MODE, _7BIT) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _HW)         |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _400KHZ) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _DISABLED) |
        DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _TRANSACTION_MODE, _NORMAL) |
        0;

    if (eeprom->index_size <= 8)
    {
        i2cIndexed.flags |=
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ONE);
        i2cIndexed.index[0] =  offset & 0x000FF;        // Read [eeprom_offset]
    }
    else
    {
        i2cIndexed.address |= ((offset & 0x30000) >> 15);
        i2cIndexed.flags |=
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _TWO);
        i2cIndexed.index[0] = (offset & 0x0FF00) >> 8;  // Read [eeprom_offset]
        i2cIndexed.index[1] = (offset & 0x000FF);
    }

    i2cIndexed.messageLength = NV_MIN(buffer_size, NVSWITCH_CTRL_I2C_MESSAGE_LENGTH_MAX);

    retval = nvswitch_ctrl_i2c_indexed(device, &i2cIndexed);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    for (i = 0; i < i2cIndexed.messageLength; i++)
    {
        buffer[i] = i2cIndexed.message[i];
    }

    return retval;
}

//
// Parse EEPROM header, if present
//
static NvlStatus
_nvswitch_read_rom_header
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NvU32 *offset
)
{
    NVSWITCH_EEPROM_HEADER eeprom_header = {{0}};
    NvlStatus retval;

    firmware->firmware_size = 0;
    *offset = 0x0000;

    retval = _nvswitch_read_rom_bytes(device,
        eeprom, *offset,
        (NvU8 *) &eeprom_header, sizeof(eeprom_header));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Unable to read ROM header\n");
        return retval;
    }

    if ((eeprom_header.signature[0] == 'N') &&
        (eeprom_header.signature[1] == 'V') &&
        (eeprom_header.signature[2] == 'L') &&
        (eeprom_header.signature[3] == 'S') &&
        (_nvswitch_calculate_checksum((NvU8 *) &eeprom_header, sizeof(eeprom_header)) == 0x00))
    {
        // Assume eeprom_header is version 1

        *offset += eeprom_header.header_size;

        firmware->pci_vendor_id = eeprom_header.pci_vendor_id;
        firmware->pci_device_id = eeprom_header.pci_device_id;
        firmware->pci_system_vendor_id = eeprom_header.pci_system_vendor_id;
        firmware->pci_system_device_id = eeprom_header.pci_system_device_id;

        // EEPROM header firmware size field is in 512 byte blocks
        firmware->firmware_size = eeprom_header.firmware_size * 512;
    }
    else
    {
        NVSWITCH_PRINT(device, SETUP,
            "Firmware header not found\n");
        return -NVL_NOT_FOUND;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_rom_parse_bit_bridge_fw_data
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NVSWITCH_BIT_TOKEN *bit_token
)
{
    NVSWITCH_BIT_BRIDGE_FW_DATA bit_bridge_fw = {0};
    NvU32 copy_size;
    NvU32 bridge_fw_size;
    NvlStatus retval;

    firmware->bridge.bridge_fw_found = NV_FALSE;

    if (bit_token->data_size != sizeof(bit_bridge_fw))
    {
        NVSWITCH_PRINT(device, SETUP,
            "BIT_BRIDGE_FW_DATA: Expected data size 0x%x but found 0x%x\n",
            (NvU32) sizeof(bit_bridge_fw), bit_token->data_size);
    }

    bridge_fw_size = NV_MIN(bit_token->data_size, sizeof(bit_bridge_fw));

    // Get basic bridge-specific firmware info
    retval = _nvswitch_read_rom_bytes(device, eeprom, bit_token->data_offset,
        (NvU8 *) &bit_bridge_fw, bridge_fw_size);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Failed to read BIT_BRIDGE_FW_DATA\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    firmware->bridge.bridge_fw_found = NV_TRUE;

    firmware->bridge.firmware_version =
        NVSWITCH_ELEMENT_READ(&bit_bridge_fw, firmware_version, bridge_fw_size, 0);

    firmware->bridge.oem_version =
        NVSWITCH_ELEMENT_READ(&bit_bridge_fw, oem_version, bridge_fw_size, 0);

    NVSWITCH_ELEMENT_VALIDATE(&bit_bridge_fw, firmware_size, bridge_fw_size, 0,
        firmware->firmware_size/512);

    if (NVSWITCH_ELEMENT_PRESENT(&bit_bridge_fw, BIOS_MOD_date, bridge_fw_size))
    {
        nvswitch_os_memcpy(firmware->bridge.BIOS_MOD_date, bit_bridge_fw.BIOS_MOD_date,
            sizeof(firmware->bridge.BIOS_MOD_date));
    }

    firmware->bridge.fw_release_build =
        (NVSWITCH_ELEMENT_PRESENT(&bit_bridge_fw, firmware_flags, bridge_fw_size) ?
            FLD_TEST_DRF(SWITCH_BIT_BRIDGE_FW_DATA, _FLAGS, _BUILD, _REL,
                bit_bridge_fw.firmware_flags) :
            NV_FALSE);

    copy_size = NV_MIN(NVSWITCH_PRODUCT_NAME_MAX_LEN,
        NVSWITCH_ELEMENT_READ(&bit_bridge_fw, eng_product_name_size, bridge_fw_size, 0));
    if (copy_size > 0)
    {
        retval = _nvswitch_read_rom_bytes(device, eeprom,
            bit_bridge_fw.eng_product_name,
            (NvU8 *) firmware->bridge.product_name, copy_size);
        if (retval != NVL_SUCCESS)
        {
            // Failed to read product name string
            copy_size = 0;
        }
    }
    firmware->bridge.product_name[copy_size] = 0;

    firmware->bridge.instance_id = NVSWITCH_ELEMENT_READ(
        &bit_bridge_fw,
        nvswitch_instance_id,
        bridge_fw_size,
        NVSWITCH_FIRMWARE_BRIDGE_INSTANCE_ID_UNKNOWN);

    return retval;
}

static NvlStatus
_nvswitch_rom_parse_bit_clock_ptrs
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NVSWITCH_BIT_TOKEN *bit_token
)
{
    NVSWITCH_BIT_CLOCK_PTRS bit_clock_ptrs = {0};
    NVSWITCH_PLL_INFO_HEADER pll_info_header;
    NVSWITCH_PLL_INFO_ENTRY pll_info;
    NvU32 pll_info_offset;
    NvU32 idx_pll;
    NvU32 clock_ptrs_size;
    NvU32 pll_info_table;
    NvlStatus retval;

    firmware->clocks.clocks_found = NV_FALSE;

    if (bit_token->data_size != sizeof(bit_clock_ptrs))
    {
        NVSWITCH_PRINT(device, SETUP,
            "CLOCK_PTRS: Expected data size 0x%x but found 0x%x\n",
            (NvU32) sizeof(bit_clock_ptrs), bit_token->data_size);
    }

    clock_ptrs_size = NV_MIN(bit_token->data_size, sizeof(bit_clock_ptrs));

     // Get PLL limits
    retval = _nvswitch_read_rom_bytes(device, eeprom, bit_token->data_offset,
        (NvU8 *) &bit_clock_ptrs, clock_ptrs_size);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVINIT_PTRS: Failed to read BIT_TOKEN_CLOCK_PTRS\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pll_info_table = NVSWITCH_ELEMENT_READ(&bit_clock_ptrs, pll_info_table, clock_ptrs_size, 0);

    if ((pll_info_table == 0) ||
        (pll_info_table + sizeof(pll_info_header) > firmware->firmware_size))
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVINIT_PTRS: BIT_TOKEN_CLOCK_PTRS not preset or out of range (0x%x)\n",
            bit_clock_ptrs.pll_info_table);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = _nvswitch_read_rom_bytes(device, eeprom, pll_info_table,
        (NvU8 *) &pll_info_header, sizeof(pll_info_header));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "CLOCK_PTRS: Failed to read NVSWITCH_PLL_INFO_HEADER\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (pll_info_header.version != NVSWITCH_CLOCK_PTRS_PLL_INFO_VERSION)
    {
        NVSWITCH_PRINT(device, SETUP,
            "PLL_INFO version (0x%x) != expected version (0x%x)\n",
            pll_info_header.version, NVSWITCH_CLOCK_PTRS_PLL_INFO_VERSION);
        return -NVL_ERR_NOT_SUPPORTED;
    }
    if (pll_info_header.header_size != sizeof(NVSWITCH_PLL_INFO_HEADER))
    {
        NVSWITCH_PRINT(device, SETUP,
            "PLL_INFO header size (0x%x) != expected (0x%x)\n",
            pll_info_header.header_size, (NvU32) sizeof(NVSWITCH_PLL_INFO_HEADER));
        return -NVL_ERR_NOT_SUPPORTED;
    }
    if (pll_info_header.entry_size != sizeof(NVSWITCH_PLL_INFO_ENTRY))
    {
        NVSWITCH_PRINT(device, SETUP,
            "PLL_INFO: Expected entry size 0x%x but found 0x%x\n",
            (NvU32) sizeof(NVSWITCH_PLL_INFO_ENTRY), pll_info_header.entry_size);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    firmware->clocks.clocks_found = NV_TRUE;
    firmware->clocks.sys_pll.valid = NV_FALSE;

    for (idx_pll = 0; idx_pll < pll_info_header.entry_count; idx_pll++)
    {
        pll_info_offset =
            bit_clock_ptrs.pll_info_table + pll_info_header.header_size +
            idx_pll*pll_info_header.entry_size;
        if (pll_info_offset + sizeof(pll_info) > firmware->firmware_size)
        {
            NVSWITCH_PRINT(device, SETUP,
                "PLL info #%d out of range (%x+%x > %x)\n", idx_pll,
                pll_info_offset, (NvU32) sizeof(pll_info), firmware->firmware_size);
            retval = -NVL_NOT_FOUND;
            break;
        }

        retval = _nvswitch_read_rom_bytes(device, eeprom, pll_info_offset,
            (NvU8 *) &pll_info, sizeof(pll_info));
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, SETUP,
                "CLOCK_PTRS: Failed to read NVSWITCH_PLL_INFO_ENTRY\n");
            retval = -NVL_ERR_NOT_SUPPORTED;
            break;
        }

        if (pll_info.pll_id == NVSWITCH_PLL_ID_SYSPLL)
        {
            if (firmware->clocks.sys_pll.valid)
            {
                NVSWITCH_PRINT(device, SETUP,
                    "NVINIT_PTRS: More than 1 SYSPLL entry found.  Skipping\n");
            }
            else
            {
                firmware->clocks.sys_pll.valid = NV_TRUE;
                firmware->clocks.sys_pll.ref_min_mhz = pll_info.ref_min_mhz;
                firmware->clocks.sys_pll.ref_max_mhz = pll_info.ref_max_mhz;
                firmware->clocks.sys_pll.vco_min_mhz = pll_info.vco_min_mhz;
                firmware->clocks.sys_pll.vco_max_mhz = pll_info.vco_max_mhz;
                firmware->clocks.sys_pll.update_min_mhz = pll_info.update_min_mhz;
                firmware->clocks.sys_pll.update_max_mhz = pll_info.update_max_mhz;
                firmware->clocks.sys_pll.m_min = pll_info.m_min;
                firmware->clocks.sys_pll.m_max = pll_info.m_max;
                firmware->clocks.sys_pll.n_min = pll_info.n_min;
                firmware->clocks.sys_pll.n_max = pll_info.n_max;
                firmware->clocks.sys_pll.pl_min = pll_info.pl_min;
                firmware->clocks.sys_pll.pl_max = pll_info.pl_max;
            }
        }
        else
        {
            NVSWITCH_PRINT(device, SETUP,
                "Ignoring PLL ID 0x%x\n", pll_info.pll_id);
        }
    }

    return retval;
}

static NvlStatus
_nvswitch_rom_parse_bit_nvinit_ptrs
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NVSWITCH_BIT_TOKEN *bit_token
)
{
    NVSWITCH_BIT_NVINIT_PTRS bit_nvinit_ptrs = {0};
    NVSWITCH_NVLINK_CONFIG nvlink_config;
    NvU32 nvinit_ptrs_size;
    NvU32 nvlink_config_offset;
    NvU32 nvlink_config_size;
    NvlStatus retval;

    firmware->nvlink.link_config_found = NV_FALSE;

    if (bit_token->data_size != sizeof(bit_nvinit_ptrs))
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVINIT_PTRS: Expected data size 0x%x but found 0x%x\n",
            (NvU32) sizeof(bit_nvinit_ptrs), bit_token->data_size);
    }

    nvinit_ptrs_size = NV_MIN(bit_token->data_size, sizeof(bit_nvinit_ptrs));

    // Get basic NVLink settings
    retval = _nvswitch_read_rom_bytes(device, eeprom, bit_token->data_offset,
        (NvU8 *) &bit_nvinit_ptrs, nvinit_ptrs_size);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVINIT_PTRS: Failed to read NVSWITCH_BIT_NVINIT_PTRS\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    nvlink_config_offset = NVSWITCH_ELEMENT_READ(&bit_nvinit_ptrs, nvlink_config, nvinit_ptrs_size, 0);
    if ((nvlink_config_offset == 0) ||
        (nvlink_config_offset + sizeof(nvlink_config) > firmware->firmware_size))
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVINIT_PTRS: NVSWITCH_BIT_NVINIT_PTRS NVLink config absent or out of range (0x%x)\n",
            bit_nvinit_ptrs.nvlink_config);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = _nvswitch_read_rom_bytes(device, eeprom, nvlink_config_offset,
        (NvU8 *) &nvlink_config, sizeof(nvlink_config));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVINIT_PTRS: Failed to read NVSWITCH_NVLINK_CONFIG\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    nvlink_config_size = NV_MIN(nvlink_config.size, sizeof(nvlink_config));

    if (0x01 != NVSWITCH_ELEMENT_READ(&nvlink_config, version, nvlink_config_size, 0))
    {
        NVSWITCH_PRINT(device, SETUP,
            "NVINIT_PTRS: NVLINK_CONFIG version mismatch (0x01 != 0x%x)\n",
            NVSWITCH_ELEMENT_READ(&nvlink_config, version, nvlink_config_size, 0));
        return -NVL_ERR_NOT_SUPPORTED;
    }

    NVSWITCH_ELEMENT_CHECK(&nvlink_config, flags, nvlink_config_size, 0x0);
    NVSWITCH_ELEMENT_CHECK(&nvlink_config, link_speed_mask, nvlink_config_size, 0x0);
    NVSWITCH_ELEMENT_CHECK(&nvlink_config, link_refclk_mask, nvlink_config_size, 0x0);

    firmware->nvlink.link_config_found = NV_TRUE;

    //
    // If nvlink_config is incomplete, assume:
    //  1) all links enabled
    //  2) DC coupled
    //
    firmware->nvlink.link_enable_mask = ~NVSWITCH_ELEMENT_READ(&nvlink_config, link_disable_mask, nvlink_config_size, 0);
    firmware->nvlink.link_ac_coupled_mask = NVSWITCH_ELEMENT_READ(&nvlink_config, ac_coupled_mask, nvlink_config_size, 0);

    return retval;
}

static void
_nvswitch_rom_parse_bit_dcb_ccb_block
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NvU32 ccb_block_offset
)
{
    NVSWITCH_CCB_TABLE ccb;
    NVSWITCH_CCB_ENTRY ccb_entry;
    NvU32 ccb_table_offset;
    NvU32 idx_ccb;
    NvU32 retval;

    // dcb:ccb_block_ptr
    if ((ccb_block_offset == 0) ||
        (ccb_block_offset + sizeof(NVSWITCH_CCB_TABLE) > firmware->firmware_size))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: CCB_BLOCK absent or out of range (0x%x)\n",
            ccb_block_offset);
        return;
    }

    retval = _nvswitch_read_rom_bytes(device, eeprom, ccb_block_offset,
        (NvU8 *) &ccb, sizeof(ccb));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP, "DCB_PTRS: CCB header read failure\n");
        return;
    }

    if ((ccb.version != NVSWITCH_CCB_VERSION) ||
        (ccb.header_size != sizeof(ccb)))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: CCB_BLOCK version (0x%x) or size mismatch (0x%x)\n",
            ccb.version, ccb.header_size);
        return;
    }

    ccb_table_offset = ccb_block_offset + ccb.header_size;

    for (idx_ccb = 0; idx_ccb < ccb.entry_count; idx_ccb++)
    {
        NvU32 ccb_entry_offset = ccb_table_offset + idx_ccb*ccb.entry_size;
        NvU32 i2c_bus_idx;
        NvU32 idx_i2c_port;

        if (ccb_entry_offset + sizeof(ccb_entry) > firmware->firmware_size)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: CCB out of range\n");
            break;
        }

        retval = _nvswitch_read_rom_bytes(device, eeprom, ccb_entry_offset,
            (NvU8 *) &ccb_entry, sizeof(ccb_entry));
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: CCB entry[%d] read failure\n",
                idx_ccb);
            break;
        }

        i2c_bus_idx = DRF_VAL(SWITCH_CCB, _DEVICE, _I2C_PORT,  ccb_entry.device);
        if (i2c_bus_idx >= NVSWITCH_MAX_I2C_PORTS)
        {
            continue;
        }

        for (idx_i2c_port = 0; idx_i2c_port < NVSWITCH_MAX_I2C_PORTS; idx_i2c_port++)
        {
            if (ccb.comm_port[idx_i2c_port] == i2c_bus_idx)
            {
                break;
            }
        }

        if (idx_i2c_port >= NVSWITCH_MAX_I2C_PORTS)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: CCB entry[%d] I2C port %x out of range\n",
                idx_ccb, idx_i2c_port);
            continue;
        }

        firmware->dcb.i2c[idx_i2c_port].valid = NV_TRUE;
        firmware->dcb.i2c[idx_i2c_port].i2c_speed = DRF_VAL(SWITCH_CCB, _DEVICE, _I2C_SPEED, ccb_entry.device);
        firmware->dcb.i2c[idx_i2c_port].i2c_33v = DRF_VAL(SWITCH_CCB, _DEVICE, _VOLTAGE, ccb_entry.device);
    }
}

static void
_nvswitch_rom_parse_bit_dcb_gpio_table
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NvU32 gpio_table_offset
)
{
    NVSWITCH_GPIO_TABLE gpio;
    NVSWITCH_GPIO_ENTRY gpio_entry;
    NvU32 idx_gpio;
    NvU32 retval;

    // gpio_tables
    if ((gpio_table_offset == 0) ||
        (gpio_table_offset + sizeof(NVSWITCH_GPIO_TABLE) > firmware->firmware_size))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: GPIO_TABLE absent or out of range (0x%x)\n",
            gpio_table_offset);
        return;
    }

    retval = _nvswitch_read_rom_bytes(device, eeprom, gpio_table_offset,
        (NvU8 *) &gpio, sizeof(gpio));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: GPIO table read failure\n");
        return;
    }

    if ((gpio.version != NVSWITCH_GPIO_TABLE_VERSION_42) ||
        (gpio.header_size != sizeof(gpio)))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: GPIO_TABLE version (0x%x) or size mismatch (0x%x)\n",
            gpio.version, gpio.header_size);
        return;
    }

    if (gpio.entry_size != sizeof(gpio_entry))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: GPIO_ENTRY size mismatch (0x%x != 0x%x)\n",
            gpio.entry_size, (NvU32) sizeof(gpio_entry));
        return;
    }

    NVSWITCH_ELEMENT_CHECK(&gpio, ext_gpio_master, gpio.header_size, 0x0000);

    gpio_table_offset += gpio.header_size;
    firmware->dcb.gpio_pin_count = 0;

    for (idx_gpio = 0; idx_gpio < gpio.entry_count; idx_gpio++)
    {
        NVSWITCH_GPIO_INFO *gpio_pin;
        NvU32 gpio_entry_offset = gpio_table_offset + idx_gpio*gpio.entry_size;

        if (gpio_entry_offset + gpio.entry_size > firmware->firmware_size)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: GPIO entry[%d] out of range\n",
                idx_gpio);
            break;
        }

        if (firmware->dcb.gpio_pin_count == NVSWITCH_MAX_GPIO_PINS)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: Too many GPIO pins listed\n");
            break;
        }

        retval = _nvswitch_read_rom_bytes(device, eeprom, gpio_entry_offset,
            (NvU8 *) &gpio_entry, sizeof(gpio_entry));
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: GPIO entry read failure\n");
            break;
        }

        if (!FLD_TEST_DRF(SWITCH_GPIO_ENTRY, , _FUNCTION, _SKIP_ENTRY, gpio_entry.function))
        {
            gpio_pin = &firmware->dcb.gpio_pin[firmware->dcb.gpio_pin_count];
            firmware->dcb.gpio_pin_count++;

            gpio_pin->pin = DRF_VAL(SWITCH_GPIO_ENTRY, _PIN, _NUM, gpio_entry.pin);
            gpio_pin->function = DRF_VAL(SWITCH_GPIO_ENTRY, , _FUNCTION, gpio_entry.function);
            gpio_pin->hw_select = DRF_VAL(SWITCH_GPIO_ENTRY, _INPUT, _HW_SELECT, gpio_entry.input);
            gpio_pin->misc = DRF_VAL(SWITCH_GPIO_ENTRY, _MISC, _IO, gpio_entry.misc);
        }
    }
}

static void
_nvswitch_rom_parse_bit_dcb_i2c_devices
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NvU32 i2c_devices_offset
)
{
    NVSWITCH_I2C_TABLE i2c;
    NVSWITCH_I2C_ENTRY i2c_entry;
    NvU32 i2c_table_offset;
    NvU32 idx_i2c;
    NvU32 retval;

    // i2c_devices
    if ((i2c_devices_offset == 0) ||
        (i2c_devices_offset + sizeof(NVSWITCH_I2C_TABLE) > firmware->firmware_size))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: I2C_DEVICES absent or out of range (0x%x)\n",
            i2c_devices_offset);
        return;
    }

    retval = _nvswitch_read_rom_bytes(device, eeprom, i2c_devices_offset,
        (NvU8 *) &i2c, sizeof(i2c));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: I2C device read failure\n");
        return;
    }

    if ((i2c.version != NVSWITCH_I2C_VERSION) ||
        (i2c.header_size != sizeof(i2c)) ||
        (i2c.entry_size != sizeof(i2c_entry)))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: I2C header version (0x%x) or header/entry size mismatch (0x%x/0x%x)\n",
            i2c.version, i2c.header_size, i2c.entry_size);
        return;
    }

    i2c_table_offset = i2c_devices_offset + i2c.header_size;

    firmware->dcb.i2c_device_count = 0;

    for (idx_i2c = 0; idx_i2c < i2c.entry_count; idx_i2c++)
    {
        NvU32 i2c_entry_offset = i2c_table_offset + idx_i2c*i2c.entry_size;
        NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE *i2c_device;

        if (i2c_entry_offset + sizeof(i2c_entry) > firmware->firmware_size)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: I2C[%d] out of range\n",
                idx_i2c);
            break;
        }

        if (firmware->dcb.i2c_device_count >= NVSWITCH_MAX_I2C_DEVICES)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: Too many I2C devices listed\n");
            break;
        }

        retval = _nvswitch_read_rom_bytes(device, eeprom, i2c_entry_offset,
            (NvU8 *) &i2c_entry, sizeof(i2c_entry));
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, SETUP,
                "DCB_PTRS: I2C read failure\n");
            break;
        }

        if (NVSWITCH_I2C_DEVICE_SKIP != DRF_VAL(SWITCH_I2C, _ENTRY, _TYPE, i2c_entry.device))
        {
            i2c_device = &firmware->dcb.i2c_device[firmware->dcb.i2c_device_count];
            firmware->dcb.i2c_device_count++;

            i2c_device->i2cDeviceType = DRF_VAL(SWITCH_I2C, _ENTRY, _TYPE, i2c_entry.device);
            i2c_device->i2cAddress = DRF_VAL(SWITCH_I2C, _ENTRY, _ADDRESS, i2c_entry.device);
            i2c_device->i2cPortLogical =
                (DRF_VAL(SWITCH_I2C, _ENTRY, _PORT_2, i2c_entry.device) << 1) |
                DRF_VAL(SWITCH_I2C, _ENTRY, _PORT_1, i2c_entry.device);
        }
    }
}

static NvlStatus
_nvswitch_rom_parse_bit_dcb_ptrs
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NVSWITCH_BIT_TOKEN *bit_token
)
{
    NVSWITCH_BIT_DCB_PTRS dcb_ptrs;
    NVSWITCH_DCB_HEADER dcb;
    NvU32 dcb_ptrs_size;
    NvU32 dcb_version;
    NvU32 dcb_signature;
    NvlStatus retval = NVL_SUCCESS;

    firmware->dcb.dcb_found = NV_FALSE;

    if (bit_token->data_size != sizeof(dcb_ptrs))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: Expected data size 0x%x but found 0x%x\n",
            (NvU32) sizeof(dcb_ptrs), bit_token->data_size);
    }

    dcb_ptrs_size = NV_MIN(bit_token->data_size, sizeof(dcb_ptrs));

    // Get I2C & GPIO tables
    retval = _nvswitch_read_rom_bytes(device, eeprom, bit_token->data_offset, 
        (NvU8 *) &dcb_ptrs, dcb_ptrs_size);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: Failed to read NVSWITCH_BIT_DCB_PTRS\n");
        return retval;
    }

    if ((dcb_ptrs.dcb_header_ptr == 0) ||
        (dcb_ptrs.dcb_header_ptr >= firmware->firmware_size))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: DCB header absent or out of range (0x%x)\n",
            dcb_ptrs.dcb_header_ptr);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = _nvswitch_read_rom_bytes(device, eeprom, dcb_ptrs.dcb_header_ptr,
        (NvU8 *) &dcb, sizeof(dcb));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: DCB header read failure\n");
        return retval;
    }

    dcb_version = NVSWITCH_ELEMENT_READ(&dcb, version, dcb.header_size, 0x0);
    dcb_signature = NVSWITCH_ELEMENT_READ(&dcb, dcb_signature, dcb.header_size, 0x0);
    if ((dcb_version != NVSWITCH_DCB_HEADER_VERSION_41) ||
        (dcb_signature != NVSWITCH_DCB_HEADER_SIGNATURE))
    {
        NVSWITCH_PRINT(device, SETUP,
            "DCB_PTRS: DCB header version (0x%x) or signature mismatch (0x%x)\n",
            dcb_version, dcb_signature);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    _nvswitch_rom_parse_bit_dcb_ccb_block(device, eeprom, firmware, dcb.ccb_block_ptr);
    _nvswitch_rom_parse_bit_dcb_i2c_devices(device, eeprom, firmware, dcb.i2c_devices);
    _nvswitch_rom_parse_bit_dcb_gpio_table(device, eeprom, firmware, dcb.gpio_table);

    return retval;
}

//
// Parse BIT tokens, if present
//
static NvlStatus
_nvswitch_read_bit_tokens
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NVSWITCH_BIT_HEADER *bit_header,
    NvU32 *offset
)
{
    NvU32 idx_token;
    NvU32 bit_entry_offset;
    NVSWITCH_BIT_TOKEN bit_token;
    NvlStatus retval = NVL_SUCCESS;

    for (idx_token = 0; idx_token < bit_header->token_entries; idx_token++)
    {
        bit_entry_offset = *offset + idx_token*bit_header->token_size;
        if (bit_entry_offset >= firmware->firmware_size)
        {
            NVSWITCH_PRINT(device, SETUP,
                "BIT token out of range (%x >= %x)\n",
                bit_entry_offset, firmware->firmware_size);
            return -NVL_NOT_FOUND;
        }

        retval = _nvswitch_read_rom_bytes(device,
            eeprom, bit_entry_offset,
            (NvU8 *) &bit_token, sizeof(bit_token));
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, SETUP,
                "Error reading BIT token[%d]\n", idx_token);
            return -NVL_NOT_FOUND;
        }

        if (bit_token.data_offset >= firmware->firmware_size)
        {
            NVSWITCH_PRINT(device, SETUP,
                "BIT 0x%x target data out of range (%x >= %x)\n",
                bit_token.id,
                bit_token.data_offset, firmware->firmware_size);
            // Soldier on to the next one.  Hopefully it's valid
            continue;
        }

        switch (bit_token.id)
        {
            case NVSWITCH_BIT_TOKEN_CLOCK_PTRS:
            {
                retval = _nvswitch_rom_parse_bit_clock_ptrs(device, eeprom, firmware, &bit_token);
                break;
            }
            case NVSWITCH_BIT_TOKEN_NVINIT_PTRS:
            {
                retval = _nvswitch_rom_parse_bit_nvinit_ptrs(device, eeprom, firmware, &bit_token);
                break;
            }
            case NVSWITCH_BIT_TOKEN_NOP:
            {
                // Ignore
                break;
            }
            case NVSWITCH_BIT_TOKEN_PERF_PTRS:
            {
                NVSWITCH_PRINT(device, INFO, "Skipping parsing BIT_TOKEN_PERF_PTRS\n");
                break;
            }
            case NVSWITCH_BIT_TOKEN_BRIDGE_FW_DATA:
            {
                retval = _nvswitch_rom_parse_bit_bridge_fw_data(device, eeprom, firmware, &bit_token);
                break;
            }
            case NVSWITCH_BIT_TOKEN_DCB_PTRS:
            {
                retval = _nvswitch_rom_parse_bit_dcb_ptrs(device, eeprom, firmware, &bit_token);
                break;
            }
            default:
            {
                NVSWITCH_PRINT(device, SETUP,
                    "Unrecognized BIT_TOKEN 0x%02x\n", bit_token.id);
                break;
            }
        }
    }

    return retval;
}

//
// Parse BIT table, if present
//
static NvlStatus
_nvswitch_read_bit_table
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom,
    NVSWITCH_FIRMWARE *firmware,
    NvU32 *offset
)
{
    NVSWITCH_BIT_HEADER  bit_header = {0};
    NvlStatus retval;

    retval = _nvswitch_read_rom_bytes(device,
        eeprom, *offset,
        (NvU8 *) &bit_header, sizeof(bit_header));
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Unable to read BIT header @%04x\n",
            *offset);
        return retval;
    }

    if ((bit_header.id           == 0xB8FF) &&
        (bit_header.signature[0] == 'B') &&
        (bit_header.signature[1] == 'I') &&
        (bit_header.signature[2] == 'T') &&
        (bit_header.signature[3] == 0x00) &&
        (bit_header.bcd_version  == 0x0100) &&
        (_nvswitch_calculate_checksum((NvU8 *) &bit_header, sizeof(bit_header)) == 0x00))
    {
        *offset += bit_header.header_size;
        if (*offset >= firmware->firmware_size)
        {
            NVSWITCH_PRINT(device, SETUP,
                "BIT token table out of range (%x >= %x)\n",
                *offset, firmware->firmware_size);
            return -NVL_NOT_FOUND;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, SETUP,
            "BIT header not found @%04x\n",
            *offset);
        return -NVL_NOT_FOUND;
    }

    retval = _nvswitch_read_bit_tokens(device, eeprom, firmware, &bit_header, offset);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Unable to read BIT tokens\n");
        return retval;
    }

    return NVL_SUCCESS;
}

//
// Print BIT table information
//
static void
_nvswitch_print_bit_table_info
(
    nvswitch_device *device,
    NVSWITCH_FIRMWARE *firmware
)
{
    if (firmware->firmware_size > 0)
    {
        NVSWITCH_PRINT(device, SETUP, "PCI ID:           %04x/%04x\n",
            firmware->pci_vendor_id,
            firmware->pci_device_id);
        NVSWITCH_PRINT(device, SETUP, "Subsystem PCI ID: %04x/%04x\n",
            firmware->pci_system_vendor_id,
            firmware->pci_system_device_id);

        if (firmware->bridge.bridge_fw_found)
        {
            NVSWITCH_PRINT(device, SETUP, "firmware_version: %08x\n",
                firmware->bridge.firmware_version);
            NVSWITCH_PRINT(device, SETUP, "oem_version:      %02x\n",
                firmware->bridge.oem_version);
            NVSWITCH_PRINT(device, SETUP, "BIOS_MOD_date:    '%.8s'\n",
                firmware->bridge.BIOS_MOD_date);
            NVSWITCH_PRINT(device, SETUP, "fw_release_build: %s\n",
                (firmware->bridge.fw_release_build ? "REL" : "ENG"));
            NVSWITCH_PRINT(device, SETUP, "product_name:     '%s'\n",
                firmware->bridge.product_name);
            if (firmware->bridge.instance_id != NVSWITCH_FIRMWARE_BRIDGE_INSTANCE_ID_UNKNOWN)
            {
                NVSWITCH_PRINT(device, SETUP, "instance_id:      %04x\n",
                    firmware->bridge.instance_id);
            }
        }

        if (firmware->nvlink.link_config_found)
        {
            NVSWITCH_PRINT(device, SETUP, "link_enable: %016llx\n", firmware->nvlink.link_enable_mask);
            NVSWITCH_PRINT(device, SETUP, "ac_coupled:  %016llx\n", firmware->nvlink.link_ac_coupled_mask);
        }
    }
}

//
// Parse EEPROM BIT tables, if present
//
void
nvswitch_read_rom_tables
(
    nvswitch_device *device,
    NVSWITCH_FIRMWARE *firmware
)
{
    NVSWITCH_EEPROM_TYPE eeprom = {0};
    NvU32 offset;
    NvlStatus retval;

    retval = nvswitch_get_rom_info(device, &eeprom);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "ROM configuration not supported\n");
        return;
    }

    retval = _nvswitch_read_rom_header(device, &eeprom, firmware, &offset);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Unable to read ROM header\n");
        return;
    }

    retval = _nvswitch_read_bit_table(device, &eeprom, firmware, &offset);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Unable to read BIT table\n");
        return;
    }

    _nvswitch_print_bit_table_info(device, firmware);

    return;
}

