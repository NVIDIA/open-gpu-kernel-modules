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

#ifndef _ROM_NVSWITCH_H_
#define _ROM_NVSWITCH_H_

#include "pmgr_nvswitch.h"
#include "io_nvswitch.h"

//
// When parsing BIOS tables these wrappers help protect against reading and using
// fields that may not be present in the ROM image by checking the offset against
// the structure size.
//
#define NV_OFFSETOF_MEMBER(_basePtr, _member)                                  \
    ((NvUPtr)(((NvU8 *)(&((_basePtr)->_member))) - ((NvU8 *)(_basePtr))))

#define NVSWITCH_ELEMENT_PRESENT(_ptr, _element, _size)          \
    (NV_OFFSETOF_MEMBER((_ptr), _element) + sizeof((_ptr)->_element) <= (_size))

#define NVSWITCH_ELEMENT_READ(_ptr, _element, _size, _default)   \
    (NVSWITCH_ELEMENT_PRESENT(_ptr, _element, _size) ?           \
        ((_ptr)->_element) : (_default))

#define NVSWITCH_ELEMENT_VALIDATE(_ptr, _element, _size, _default, _expected)   \
    do                                                                          \
    {                                                                           \
        NvU32 data = NVSWITCH_ELEMENT_READ(_ptr, _element, _size, _default);    \
        if (data != (_expected))                                                \
        {                                                                       \
            NVSWITCH_PRINT(device, SETUP,                                       \
                "Element '%s->%s'=0x%x but expected 0x%x\n",                    \
                #_ptr, #_element, data, (NvU32) (_expected));                   \
        }                                                                       \
    } while(0)

#define NVSWITCH_ELEMENT_CHECK(_ptr, _element, _size, _default)                 \
    NVSWITCH_ELEMENT_VALIDATE(_ptr, _element, _size, _default, _default)

#define NVSWITCH_STRUCT_PACKED_ALIGNED(typeName, bytes)                        \
    typedef struct __attribute__((packed, aligned(bytes))) 

#define NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

//
// AT24CM02 EEPROM
// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8828-SEEPROM-AT24CM02-Datasheet.pdf
//

#define AT24CM02_INDEX_SIZE     18          // Addressing bits
#define AT24CM02_BLOCK_SIZE     256         // R/W block size (bytes)

//
// AT24C02C EEPROM
// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8700-SEEPROM-AT24C01C-02C-Datasheet.pdf
//

#define AT24C02C_INDEX_SIZE     8           // Addressing bits
#define AT24C02C_BLOCK_SIZE     8           // R/W block size (bytes)

//
// AT24C02D EEPROM
// https://ww1.microchip.com/downloads/en/devicedoc/atmel-8871f-seeprom-at24c01d-02d-datasheet.pdf
// 2kb EEPROM used on LR10 P4790 B00 platform
//

#define AT24C02D_INDEX_SIZE     8           // Addressing bits
#define AT24C02D_BLOCK_SIZE     8           // R/W block size (bytes)

typedef struct
{
    NvU32 i2c_port;
    NvU32 i2c_address;
    NvU32 device_type;
    NvU32 index_size;
    NvU32 block_size;
    NvU32 block_count;
    NvU32 eeprom_size;
} NVSWITCH_EEPROM_TYPE;

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_EEPROM_HEADER, 1)
{
    char    signature[4];
    NvU16   version;
    NvU16   header_size;
    NvU16   pci_vendor_id;
    NvU16   pci_device_id;
    NvU16   pci_system_vendor_id;
    NvU16   pci_system_device_id;
    NvU16   firmware_size;
    NvU8    reserved[13];
    NvU8    checksum;
} NVSWITCH_EEPROM_HEADER;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_BIT_HEADER, 1)
{
    NvU16   id;
    char    signature[4];
    NvU16   bcd_version;
    NvU8    header_size;
    NvU8    token_size;
    NvU8    token_entries;
    NvU8    checksum;
} NVSWITCH_BIT_HEADER;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_BIT_TOKEN_CLOCK_PTRS                0x43
#define NVSWITCH_BIT_TOKEN_NVINIT_PTRS               0x49
#define NVSWITCH_BIT_TOKEN_NOP                       0x4E
#define NVSWITCH_BIT_TOKEN_PERF_PTRS                 0x50
#define NVSWITCH_BIT_TOKEN_BRIDGE_FW_DATA            0x52
#define NVSWITCH_BIT_TOKEN_DCB_PTRS                  0x6E

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_BIT_TOKEN, 1)
{
    NvU8    id;
    NvU8    data_version;
    NvU16   data_size;
    NvU16   data_offset;
} NVSWITCH_BIT_TOKEN;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

// 0x43: BIT_TOKEN_CLOCK_PTRS
NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_BIT_CLOCK_PTRS, 1)
{
    NvU32 pll_info_table;
    NvU32 vbe_mode_pclk;
    NvU32 clocks_table;
    NvU32 clocks_programming;
    NvU32 nafll;
    NvU32 adc_table;
    NvU32 freq_control;
} NVSWITCH_BIT_CLOCK_PTRS;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_CLOCK_PTRS_PLL_INFO_VERSION    0x50

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_PLL_INFO_HEADER, 1)
{
    NvU8  version;
    NvU8  header_size;
    NvU8  entry_size;
    NvU8  entry_count;
} NVSWITCH_PLL_INFO_HEADER;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_PLL_INFO_ENTRY, 1)
{
    NvU8  pll_id;
    NvU16 ref_min_mhz;
    NvU16 ref_max_mhz;
    NvU16 vco_min_mhz;
    NvU16 vco_max_mhz;
    NvU16 update_min_mhz;
    NvU16 update_max_mhz;
    NvU8  m_min;
    NvU8  m_max;
    NvU8  n_min;
    NvU8  n_max;
    NvU8  pl_min;
    NvU8  pl_max;
} NVSWITCH_PLL_INFO_ENTRY;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_PLL_ID_SYSPLL      0x07

// 0x49: BIT_TOKEN_NVINIT_PTRS
NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_BIT_NVINIT_PTRS, 1)
{
    NvU16 init_script;
    NvU16 macro_index;
    NvU16 macro_table;
    NvU16 condition;
    NvU16 io_condition;
    NvU16 io_flag_condition;
    NvU16 init_function;
    NvU16 private_boot;
    NvU16 data_arrays;
    NvU16 pcie_settings;
    NvU16 devinit;
    NvU16 devinit_size;
    NvU16 boot_script;
    NvU16 boot_script_size;
    NvU16 nvlink_config;
    NvU16 boot_script_nonGC6;
    NvU16 boot_script_nonGC6_size;
} NVSWITCH_BIT_NVINIT_PTRS;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_NVLINK_CONFIG, 1)
{
    NvU8    version;
    NvU8    size;
    NvU16   reserved;
    NvU64   link_disable_mask;      // 1 = disable
    NvU64   link_speed_mask;        // 1 = safe mode
    NvU64   link_refclk_mask;       // 0 = 100MHz, 1 = 133MHz
    NvU8    flags;
    NvU64   ac_coupled_mask;        // 0 = DC, 1 = AC
} NVSWITCH_NVLINK_CONFIG;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

// 0x52: BIT_TOKEN_BRIDGE_FW_DATA
NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_BIT_BRIDGE_FW_DATA, 1)
{
    NvU32 firmware_version;
    NvU8  oem_version;
    NvU16 firmware_size;
    char  BIOS_MOD_date[8];
    NvU32 firmware_flags;
    NvU16 eng_product_name;
    NvU8  eng_product_name_size;
    NvU16 nvswitch_instance_id;
} NVSWITCH_BIT_BRIDGE_FW_DATA;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_BIT_BRIDGE_FW_DATA_FLAGS_BUILD               0:0
#define NVSWITCH_BIT_BRIDGE_FW_DATA_FLAGS_BUILD_REL           0
#define NVSWITCH_BIT_BRIDGE_FW_DATA_FLAGS_BUILD_ENG           1
#define NVSWITCH_BIT_BRIDGE_FW_DATA_FLAGS_I2C                 1:1
#define NVSWITCH_BIT_BRIDGE_FW_DATA_FLAGS_I2C_MASTER          0
#define NVSWITCH_BIT_BRIDGE_FW_DATA_FLAGS_I2C_NOT_MASTER      1

// 0x6E: BIT_TOKEN_DCB_PTRS
NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_BIT_DCB_PTRS, 1)
{
    NvU16 dcb_header_ptr;
} NVSWITCH_BIT_DCB_PTRS;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_DCB_HEADER_VERSION_41  0x41
#define NVSWITCH_DCB_HEADER_SIGNATURE   0x4edcbdcb

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_DCB_HEADER, 1)
{
    NvU8  version;
    NvU8  header_size;
    NvU8  entry_count;
    NvU8  entry_size;
    NvU16 ccb_block_ptr;
    NvU32 dcb_signature;
    NvU16 gpio_table;
    NvU16 input_devices;
    NvU16 personal_cinema;
    NvU16 spread_spectrum;
    NvU16 i2c_devices;
    NvU16 connectors;
    NvU8  flags;
    NvU16 hdtv;
    NvU16 switched_outputs;
    NvU32 display_patch;
    NvU32 connector_patch;
} NVSWITCH_DCB_HEADER;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_GPIO_TABLE_VERSION_42  0x42

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_GPIO_TABLE, 1)
{
    NvU8    version;
    NvU8    header_size;
    NvU8    entry_count;
    NvU8    entry_size;
    NvU16   ext_gpio_master;
} NVSWITCH_GPIO_TABLE;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_GPIO_ENTRY, 1)
{
    NvU8    pin;
    NvU8    function;
    NvU8    output;
    NvU8    input;
    NvU8    misc;
} NVSWITCH_GPIO_ENTRY;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_GPIO_ENTRY_PIN_NUM                   5:0
#define NVSWITCH_GPIO_ENTRY_PIN_IO_TYPE               6:6
#define NVSWITCH_GPIO_ENTRY_PIN_INIT_STATE            7:7

#define NVSWITCH_GPIO_ENTRY_FUNCTION                  7:0
#define NVSWITCH_GPIO_ENTRY_FUNCTION_THERMAL_EVENT    17
#define NVSWITCH_GPIO_ENTRY_FUNCTION_OVERTEMP         35
#define NVSWITCH_GPIO_ENTRY_FUNCTION_THERMAL_ALERT    52
#define NVSWITCH_GPIO_ENTRY_FUNCTION_THERMAL_CRITICAL 53
#define NVSWITCH_GPIO_ENTRY_FUNCTION_POWER_ALERT      76
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID0    209
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID1    210
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID2    211
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID3    212
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID4    213
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID5    214
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID6    215
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID7    216
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID8    217
#define NVSWITCH_GPIO_ENTRY_FUNCTION_INSTANCE_ID9    218
#define NVSWITCH_GPIO_ENTRY_FUNCTION_SKIP_ENTRY      255

#define NVSWITCH_GPIO_ENTRY_OUTPUT                    7:0

#define NVSWITCH_GPIO_ENTRY_INPUT_HW_SELECT           4:0
#define NVSWITCH_GPIO_ENTRY_INPUT_HW_SELECT_NONE            0
#define NVSWITCH_GPIO_ENTRY_INPUT_HW_SELECT_THERMAL_ALERT   22
#define NVSWITCH_GPIO_ENTRY_INPUT_HW_SELECT_POWER_ALERT     23
#define NVSWITCH_GPIO_ENTRY_INPUT_GSYNC               5:5
#define NVSWITCH_GPIO_ENTRY_INPUT_OPEN_DRAIN          6:6
#define NVSWITCH_GPIO_ENTRY_INPUT_PWM                 7:7
//#define NVSWITCH_GPIO_ENTRY_INPUT_3V3                ?:?

#define NVSWITCH_GPIO_ENTRY_MISC_LOCK                 3:0
#define NVSWITCH_GPIO_ENTRY_MISC_IO                   7:4
#define NVSWITCH_GPIO_ENTRY_MISC_IO_UNUSED              0x0
#define NVSWITCH_GPIO_ENTRY_MISC_IO_INV_OUT             0x1
#define NVSWITCH_GPIO_ENTRY_MISC_IO_INV_OUT_TRISTATE    0x3
#define NVSWITCH_GPIO_ENTRY_MISC_IO_OUT                 0x4
#define NVSWITCH_GPIO_ENTRY_MISC_IO_IN_STEREO_TRISTATE  0x6
#define NVSWITCH_GPIO_ENTRY_MISC_IO_INV_OUT_TRISTATE_LO 0x9
#define NVSWITCH_GPIO_ENTRY_MISC_IO_INV_IN              0xB
#define NVSWITCH_GPIO_ENTRY_MISC_IO_OUT_TRISTATE        0xC
#define NVSWITCH_GPIO_ENTRY_MISC_IO_IN                  0xE

#define NVSWITCH_I2C_VERSION            0x40

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_I2C_TABLE, 1)
{
    NvU8    version;
    NvU8    header_size;
    NvU8    entry_count;
    NvU8    entry_size;
    NvU8    flags;
} NVSWITCH_I2C_TABLE;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_I2C_ENTRY, 1)
{
    NvU32   device;
} NVSWITCH_I2C_ENTRY;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_I2C_ENTRY_TYPE         7:0
#define NVSWITCH_I2C_ENTRY_ADDRESS      15:8
#define NVSWITCH_I2C_ENTRY_RESERVED1    19:16
#define NVSWITCH_I2C_ENTRY_PORT_1       20:20
#define NVSWITCH_I2C_ENTRY_WR_ACCESS    23:21
#define NVSWITCH_I2C_ENTRY_RD_ACCESS    26:24
#define NVSWITCH_I2C_ENTRY_PORT_2       27:27
#define NVSWITCH_I2C_ENTRY_RESERVED2    31:28

#define NVSWITCH_CCB_VERSION            0x41

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_CCB_TABLE, 1)
{
    NvU8    version;
    NvU8    header_size;
    NvU8    entry_count;
    NvU8    entry_size;
    NvU8    comm_port[4];
} NVSWITCH_CCB_TABLE;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

NVSWITCH_STRUCT_PACKED_ALIGNED(_NVSWITCH_CCB_ENTRY, 1)
{
    NvU32   device;
} NVSWITCH_CCB_ENTRY;
NVSWITCH_STRUCT_PACKED_ALIGNED_SUFFIX

#define NVSWITCH_CCB_DEVICE_I2C_PORT    4:0
#define NVSWITCH_CCB_DEVICE_DPAUX       9:5
#define NVSWITCH_CCB_DEVICE_VOLTAGE     10:10
#define NVSWITCH_CCB_DEVICE_RESERVED    27:11
#define NVSWITCH_CCB_DEVICE_I2C_SPEED   31:28

#define NVSWITCH_CCB_DEVICE_I2C_SPEED_DEFAULT   0x0
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_100KHZ    0x1
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_200KHZ    0x2
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_400KHZ    0x3
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_800KHZ    0x4
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_1600KHZ   0x5
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_3400KHZ   0x6
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_60KHZ     0x7
#define NVSWITCH_CCB_DEVICE_I2C_SPEED_300KHZ    0x8

//
// Firmware data
//

#define NVSWITCH_PRODUCT_NAME_MAX_LEN       64

typedef struct
{
    NvBool valid;
    NvU32 ref_min_mhz;
    NvU32 ref_max_mhz;
    NvU32 vco_min_mhz;
    NvU32 vco_max_mhz;
    NvU32 update_min_mhz;
    NvU32 update_max_mhz;
    NvU32 m_min;
    NvU32 m_max;
    NvU32 n_min;
    NvU32 n_max;
    NvU32 pl_min;
    NvU32 pl_max;
} NVSWITCH_PLL_LIMITS;

typedef struct
{
    NvBool valid;
    NvU32  i2c_speed;
    NvBool i2c_33v;
} NVSWITCH_I2C_PORT;

#define NVSWITCH_MAX_I2C_DEVICES    16

typedef struct
{
    NvU32   pin;
    NvU32   function;
    NvU32   hw_select;
    NvU32   misc;
} NVSWITCH_GPIO_INFO;

#define NVSWITCH_MAX_GPIO_PINS      25

typedef struct
{
    NvU32   firmware_size;

    // ROM Header
    NvU16   pci_vendor_id;
    NvU16   pci_device_id;
    NvU16   pci_system_vendor_id;
    NvU16   pci_system_device_id;

    // Firmware data
    struct
    {
        NvBool bridge_fw_found;
        NvU32 firmware_version;
        NvU8  oem_version;
        char  BIOS_MOD_date[8];
        NvBool fw_release_build;
        char  product_name[NVSWITCH_PRODUCT_NAME_MAX_LEN+1];
        NvU16 instance_id;
    } bridge;

    // Clocks
    struct
    {
        NvBool clocks_found;
        NVSWITCH_PLL_LIMITS sys_pll;
    } clocks;

    // NVLink init
    struct 
    {
        NvBool link_config_found;
        NvU64 link_enable_mask;             // 1 = enabled
        NvU64 link_ac_coupled_mask;         // 0 = DC, 1 = AC
    } nvlink;

    // DCB
    struct
    {
        NvBool              dcb_found;
        NVSWITCH_I2C_PORT   i2c[NVSWITCH_MAX_I2C_PORTS];
        NvU32               i2c_device_count;
        NVSWITCH_I2C_DEVICE_DESCRIPTOR_TYPE i2c_device[NVSWITCH_MAX_I2C_DEVICES];
        NvU32               gpio_pin_count;
        NVSWITCH_GPIO_INFO  gpio_pin[NVSWITCH_MAX_GPIO_PINS];
    } dcb;

} NVSWITCH_FIRMWARE;

#define NVSWITCH_FIRMWARE_BRIDGE_INSTANCE_ID_UNKNOWN    0xFFFF
#define NVSWITCH_FIRMWARE_BRIDGE_INSTANCE_ID_NORMAL     0xFFFE

void
nvswitch_read_rom_tables
(
    nvswitch_device *device,
    NVSWITCH_FIRMWARE *firmware
);


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#if !defined(BIOSTYPES_H_FILE)
#define bios_U008  NvU32
#define bios_U016  NvU32
#define bios_U032  NvU32
#define bios_S008  NvS32
#define bios_S016  NvS32
#define bios_S032  NvS32
#endif // !defined(BIOSTYPES_H_FILE)

/**************************************************************************************************************
*   Description:
*       Definitions of BIOS BIT structures as defined starting in Core 5
*
**************************************************************************************************************/
#if !defined(_BIT_H_)
#define BIT_HEADER_ID                     0xB8FF
#define BIT_HEADER_SIGNATURE              0x00544942  // "BIT\0"
#define BIT_HEADER_SIZE_OFFSET            8
#define BIT_HEADER_LATEST_KNOWN_VERSION   0x100
#endif // !defined(_BIT_H_)

#define PCI_ROM_HEADER_SIZE               0x18
#define PCI_DATA_STRUCT_SIZE              0x1c
#define PCI_ROM_HEADER_PCI_DATA_SIZE      (PCI_ROM_HEADER_SIZE + PCI_DATA_STRUCT_SIZE) // ROM Header + PCI Dat Structure size
#define PCI_EXP_ROM_SIGNATURE             0xaa55
#define PCI_DATA_STRUCT_SIGNATURE         0x52494350 // "PCIR" in dword format

#define NVLINK_CONFIG_DATA_HEADER_VER_20    0x2
#define NVLINK_CONFIG_DATA_HEADER_20_SIZE   8
#define NVLINK_CONFIG_DATA_HEADER_20_FMT    "6b1w"

#define NVLINK_CONFIG_DATA_HEADER_VER_30    0x3
#define NVLINK_CONFIG_DATA_HEADER_30_SIZE   8

typedef struct _PCI_DATA_STRUCT
{
    bios_U032       sig;                    //  00h: Signature, the string "PCIR" or NVIDIA's alternate "NPDS"
    bios_U016       vendorID;               //  04h: Vendor Identification
    bios_U016       deviceID;               //  06h: Device Identification
    bios_U016       deviceListPtr;          //  08h: Device List Pointer
    bios_U016       pciDataStructLen;       //  0Ah: PCI Data Structure Length
    bios_U008       pciDataStructRev;       //  0Ch: PCI Data Structure Revision
    bios_U008       classCode[3];           //  0Dh: Class Code
    bios_U016       imageLen;               //  10h: Image Length (units of 512 bytes)
    bios_U016       vendorRomRev;           //  12h: Revision Level of the Vendor's ROM
    bios_U008       codeType;               //  14h: holds NBSI_OBJ_CODE_TYPE (0x70) and others
    bios_U008       lastImage;              //  15h: Last Image Indicator: bit7=1 is lastImage
    bios_U016       maxRunTimeImageLen;     //  16h: Maximum Run-time Image Length (units of 512 bytes)
    bios_U016       configUtilityCodePtr;   //  18h: Pointer to Configurations Utility Code Header
    bios_U016       CMDTFCLPEntryPointPtr;  //  1Ah: Pointer to DMTF CLP Entry Point
} PCI_DATA_STRUCT, *PPCI_DATA_STRUCT;
#define PCI_DATA_STRUCT_FMT "1d4w4b2w2b3w"

// BIT_TOKEN_NVINIT_PTRS       0x49 // 'I' Initialization Table Pointers
struct BIT_DATA_NVINIT_PTRS_V1
{
   bios_U016 InitScriptTablePtr;      // Init script table pointer
   bios_U016 MacroIndexTablePtr;      // Macro index table pointer
   bios_U016 MacroTablePtr;           // Macro table pointer
   bios_U016 ConditionTablePtr;       // Condition table pointer
   bios_U016 IoConditionTablePtr;     // IO Condition table pointer
   bios_U016 IoFlagConditionTablePtr; // IO Flag Condition table pointer
   bios_U016 InitFunctionTablePtr;    // Init Function table pointer
   bios_U016 VBIOSPrivateTablePtr;    // VBIOS private table pointer
   bios_U016 DataArraysTablePtr;      // Data arrays table pointer
   bios_U016 PCIESettingsScriptPtr;   // PCI-E settings script pointer
   bios_U016 DevinitTablesPtr;        // Pointer to tables required by Devinit opcodes
   bios_U016 DevinitTablesSize;       // Size of tables required by Devinit opcodes
   bios_U016 BootScriptsPtr;          // Pointer to Devinit Boot Scripts
   bios_U016 BootScriptsSize;         // Size of Devinit Boot Scripts
   bios_U016 NvlinkConfigDataPtr;     // Pointer to NVLink Config Data
};
#define BIT_DATA_NVINIT_PTRS_V1_30_FMT "15w"
typedef struct BIT_DATA_NVINIT_PTRS_V1 BIT_DATA_NVINIT_PTRS_V1;

#define BIT_TOKEN_BIOSDATA          0x42 // 'B' BIOS Data
#define BIT_TOKEN_NVINIT_PTRS       0x49 // 'I'

struct BIT_HEADER_V1_00
{
    bios_U016 Id;            // BMP=0x7FFF/BIT=0xB8FF
    bios_U032 Signature;     // 0x00544942 - BIT Data Structure Signature
    bios_U016 BCD_Version;   // BIT Version - 0x0100 for 1.00
    bios_U008 HeaderSize;    // This version is 12 bytes long
    bios_U008 TokenSize;     // This version has 6 byte long Tokens
    bios_U008 TokenEntries;  // Number of Entries
    bios_U008 HeaderChksum;  // 0 Checksum of the header
};
#define BIT_HEADER_V1_00_FMT "1w1d1w4b"
typedef struct BIT_HEADER_V1_00 BIT_HEADER_V1_00;

struct BIT_TOKEN_V1_00
{
    bios_U008 TokenId;
    bios_U008 DataVersion;
    bios_U016 DataSize;
    bios_U016 DataPtr;
};
#define BIT_TOKEN_V1_00_FMT "2b2w"
typedef struct BIT_TOKEN_V1_00 BIT_TOKEN_V1_00;


// BIT_TOKEN_BIOSDATA          0x42 // 'B' BIOS Data
struct BIT_DATA_BIOSDATA_V1
{
    bios_U032 Version;                // BIOS Binary Version Ex. 5.40.00.01.12 = 0x05400001
    bios_U008 OemVersion;             // OEM Version Number  Ex. 5.40.00.01.12 = 0x12
                                      // OEM can override the two fields above
    bios_U008 Checksum;               // Filled by MakeVGA
    bios_U016 Int15CallbacksPost;     //
    bios_U016 Int15CallbacksSystem;   //
    bios_U016 BoardId;                //
    bios_U016 FrameCount;             // Frame count for signon message delay
    bios_U008 BiosmodDate[8];         // '00/00/04' Date BIOSMod was last run
};
#define BIT_DATA_BIOSDATA_V1_FMT    "1d2b4w8b"
typedef struct BIT_DATA_BIOSDATA_V1 BIT_DATA_BIOSDATA_V1;

struct BIT_DATA_BIOSDATA_V2
{
    bios_U032 Version;                // BIOS Binary Version Ex. 5.40.00.01.12 = 0x05400001
    bios_U008 OemVersion;             // OEM Version Number  Ex. 5.40.00.01.12 = 0x12
    // OEM can override the two fields above
    bios_U008 Checksum;               // Filled by MakeVGA
    bios_U016 Int15CallbacksPost;     //
    bios_U016 Int15CallbacksSystem;   //
    bios_U016 FrameCount;             // Frame count for signon message delay
    bios_U032 Reserved1;
    bios_U032 Reserved2;
    bios_U008 MaxHeadsAtPost;
    bios_U008 MemorySizeReport;
    bios_U008 HorizontalScaleFactor;
    bios_U008 VerticalScaleFactor;
    bios_U016 DataTablePtr;
    bios_U016 RomPackPtr;
    bios_U016 AppliedRomPacksPtr;
    bios_U008 AppliedRomPackMax;
    bios_U008 AppliedRomPackCount;
    bios_U008 ModuleMapExternal;
    bios_U032 CompressionInfoPtr;
};
#define BIT_DATA_BIOSDATA_V2_FMT "1d2b3w2d4b3w3b1d"
typedef struct BIT_DATA_BIOSDATA_V2 BIT_DATA_BIOSDATA_V2;

#ifndef PCI_VENDOR_ID_NVIDIA
#define PCI_VENDOR_ID_NVIDIA            0x10DE
#endif

typedef struct _nvlink_Config_Data_Header_20
{
    bios_U008 Version;           // NVLink Config Data Structure version
    bios_U008 HeaderSize;        // Size of header
    bios_U008 BaseEntrySize;
    bios_U008 BaseEntryCount;
    bios_U008 LinkEntrySize;
    bios_U008 LinkEntryCount;
    bios_U016 Reserved;          // Reserved
} NVLINK_CONFIG_DATA_HEADER_20, *PNVLINK_CONFIG_DATA_HEADER_20;

#define NV_NVLINK_VBIOS_PARAM0_LINK                             0:0
#define NV_NVLINK_VBIOS_PARAM0_LINK_ENABLE                      0x0
#define NV_NVLINK_VBIOS_PARAM0_LINK_DISABLE                     0x1
#define NV_NVLINK_VBIOS_PARAM0_RESERVED1                        1:1
#define NV_NVLINK_VBIOS_PARAM0_ACDC_MODE                        2:2
#define NV_NVLINK_VBIOS_PARAM0_ACDC_MODE_DC                     0x0
#define NV_NVLINK_VBIOS_PARAM0_ACDC_MODE_AC                     0x1
#define NV_NVLINK_VBIOS_PARAM0_RECEIVER_DETECT                  3:3
#define NV_NVLINK_VBIOS_PARAM0_RECEIVER_DETECT_DISABLE          0x0
#define NV_NVLINK_VBIOS_PARAM0_RECEIVER_DETECT_ENABLE           0x1
#define NV_NVLINK_VBIOS_PARAM0_RESTORE_PHY_TRAINING             4:4
#define NV_NVLINK_VBIOS_PARAM0_RESTORE_PHY_TRAINING_DISABLE     0x0
#define NV_NVLINK_VBIOS_PARAM0_RESTORE_PHY_TRAINING_ENABLE      0x1
#define NV_NVLINK_VBIOS_PARAM0_SLM                              5:5
#define NV_NVLINK_VBIOS_PARAM0_SLM_DISABLE                      0x0
#define NV_NVLINK_VBIOS_PARAM0_SLM_ENABLE                       0x1
#define NV_NVLINK_VBIOS_PARAM0_L2                               6:6
#define NV_NVLINK_VBIOS_PARAM0_L2_DISABLE                       0x0
#define NV_NVLINK_VBIOS_PARAM0_L2_ENABLE                        0x1
#define NV_NVLINK_VBIOS_PARAM0_RESERVED2                        7:7

#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE                        7:0
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_50_00000               0x00
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_16_00000               0x01
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_20_00000               0x02
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_25_00000               0x03
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_25_78125               0x04
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_32_00000               0x05
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_40_00000               0x06
#define NV_NVLINK_VBIOS_PARAM1_LINE_RATE_53_12500               0x07

#define NV_NVLINK_VBIOS_PARAM2_LINE_CODE_MODE                   7:0
#define NV_NVLINK_VBIOS_PARAM2_LINE_CODE_MODE_NRZ               0x00
#define NV_NVLINK_VBIOS_PARAM2_LINE_CODE_MODE_NRZ_128B130       0x01
#define NV_NVLINK_VBIOS_PARAM2_LINE_CODE_MODE_NRZ_PAM4          0x03

#define NV_NVLINK_VBIOS_PARAM3_REFERENCE_CLOCK_MODE                     1:0
#define NV_NVLINK_VBIOS_PARAM3_REFERENCE_CLOCK_MODE_COMMON              0x0
#define NV_NVLINK_VBIOS_PARAM3_REFERENCE_CLOCK_MODE_RSVD                0x1
#define NV_NVLINK_VBIOS_PARAM3_REFERENCE_CLOCK_MODE_NON_COMMON_NO_SS    0x2
#define NV_NVLINK_VBIOS_PARAM3_REFERENCE_CLOCK_MODE_NON_COMMON_SS       0x3

#define NV_NVLINK_VBIOS_PARAM3_RESERVED1                        3:2
#define NV_NVLINK_VBIOS_PARAM3_CLOCK_MODE_BLOCK_CODE            5:4
#define NV_NVLINK_VBIOS_PARAM3_CLOCK_MODE_BLOCK_CODE_OFF        0x0
#define NV_NVLINK_VBIOS_PARAM3_CLOCK_MODE_BLOCK_CODE_ECC96      0x1
#define NV_NVLINK_VBIOS_PARAM3_CLOCK_MODE_BLOCK_CODE_ECC88      0x2
#define NV_NVLINK_VBIOS_PARAM3_RESERVED2                        7:6

#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM                               7:0
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_RSVD                          0x00
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A0_SINGLE_PRESENT             0x01
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A1_PRESENT_ARRAY              0x02
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A2_FINE_GRAINED_EXHAUSTIVE    0x04
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A3_RSVD                       0x08
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A4_FOM_CENTRIOD               0x10
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A5_RSVD                       0x20
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A6_RSVD                       0x40
#define NV_NVLINK_VBIOS_PARAM4_TXTRAIN_OPTIMIZATION_ALGORITHM_A7_RSVD                       0x80

#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_ADJUSTMENT_ALGORITHM                                 4:0
#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_ADJUSTMENT_ALGORITHM_B0_NO_ADJUSTMENT                0x1
#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_ADJUSTMENT_ALGORITHM_B1_FIXED_ADJUSTMENT             0x2
#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_ADJUSTMENT_ALGORITHM_B2_RSVD                         0x4
#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_ADJUSTMENT_ALGORITHM_B3_RSVD                         0x8

#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_FOM_FORMAT                           7:5
#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_FOM_FORMAT_FOM_A                     0x1
#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_FOM_FORMAT_FOM_B                     0x2
#define NV_NVLINK_VBIOS_PARAM5_TXTRAIN_FOM_FORMAT_FOM_C                     0x4

#define NV_NVLINK_VBIOS_PARAM6_TXTRAIN_MINIMUM_TRAIN_TIME_MANTISSA           3:0
#define NV_NVLINK_VBIOS_PARAM6_TXTRAIN_MINIMUM_TRAIN_TIME_EXPONENT           7:4

#define NVLINK_CONFIG_DATA_BASEENTRY_FMT "1b"
#define NVLINK_CONFIG_DATA_LINKENTRY_FMT_20 "7b"
#define NVLINK_CONFIG_DATA_LINKENTRY_FMT_30 "10b"

// Version 2.0 Link Entry and Base Entry
typedef struct _nvlink_config_data_baseentry_20
{
     NvU8  positionId;
} NVLINK_CONFIG_DATA_BASEENTRY;

typedef struct _nvlink_config_data_linkentry_20
{
    // VBIOS configuration Data
     NvU8  nvLinkparam0;
     NvU8  nvLinkparam1;
     NvU8  nvLinkparam2;
     NvU8  nvLinkparam3;
     NvU8  nvLinkparam4;
     NvU8  nvLinkparam5;
     NvU8  nvLinkparam6;
     NvU8  nvLinkparam7;
     NvU8  nvLinkparam8;
     NvU8  nvLinkparam9;
} NVLINK_CONFIG_DATA_LINKENTRY;

// Union of different VBIOS configuration table formats
typedef union __nvlink_Config_Data_Header
{
    NVLINK_CONFIG_DATA_HEADER_20 ver_20;
} NVLINK_CONFIG_DATA_HEADER, *PNVLINK_CONFIG_DATA_HEADER;

typedef struct _nvlink_vbios_config_data_baseentry_20
{
     bios_U008  positionId;
} NVLINK_VBIOS_CONFIG_DATA_BASEENTRY;

typedef struct _nvlink_vbios_config_data_linkentry_20
{
    // VBIOS configuration Data
     bios_U008  nvLinkparam0;
     bios_U008  nvLinkparam1;
     bios_U008  nvLinkparam2;
     bios_U008  nvLinkparam3;
     bios_U008  nvLinkparam4;
     bios_U008  nvLinkparam5;
     bios_U008  nvLinkparam6;
} NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_20, *PNVLINK_VBIOS_CONFIG_DATA_LINKENTRY_20;

typedef struct _nvlink_vbios_config_data_linkentry_30
{
    // VBIOS configuration Data
     bios_U008  nvLinkparam0;
     bios_U008  nvLinkparam1;
     bios_U008  nvLinkparam2;
     bios_U008  nvLinkparam3;
     bios_U008  nvLinkparam4;
     bios_U008  nvLinkparam5;
     bios_U008  nvLinkparam6;
     bios_U008  nvLinkparam7;
     bios_U008  nvLinkparam8;
     bios_U008  nvLinkparam9;
} NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_30, *PNVLINK_VBIOS_CONFIG_DATA_LINKENTRY_30;

//
// NVSwitch driver structures
//

#define NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY    12

typedef struct
{
    NVLINK_CONFIG_DATA_BASEENTRY link_vbios_base_entry[NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY];
    NVLINK_CONFIG_DATA_LINKENTRY link_vbios_entry[NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY][NVSWITCH_MAX_LINK_COUNT];
    NvU32                        identified_Link_entries[NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY];
    NvU32                        link_base_entry_assigned;
    NvU64                        vbios_disabled_link_mask;

    NvU32                        bit_address;
    NvU32                        pci_image_address;
    NvU32                        nvlink_config_table_address;
} NVSWITCH_BIOS_NVLINK_CONFIG;

#define NVSWITCH_DCB_PTR_OFFSET 0x36

typedef struct _nvswitch_vbios_dcb_header_41
{
    bios_U008 version;
    bios_U008 header_size;
    bios_U008 entry_count;
    bios_U008 entry_size;
    bios_U016 ccb_block_ptr;
    bios_U032 dcb_signature;
    bios_U016 gpio_table;
    bios_U016 input_devices;
    bios_U016 personal_cinema;
    bios_U016 spread_spectrum;
    bios_U016 i2c_devices;
    bios_U016 connectors;
    bios_U008 flags;
    bios_U016 hdtv;
    bios_U016 switched_outputs;
    bios_U032 display_patch;
    bios_U032 connector_patch;
} NVSWITCH_VBIOS_DCB_HEADER;
#define NVSWITCH_VBIOS_DCB_HEADER_FMT "4b1w1d6w1b2w2d"

typedef struct _nvswitch_vbios_ccb_table_41
{
    bios_U008    version;
    bios_U008    header_size;
    bios_U008    entry_count;
    bios_U008    entry_size;
    bios_U008    comm_port[4];
} NVSWITCH_VBIOS_CCB_TABLE;
#define NVSWITCH_VBIOS_CCB_TABLE_FMT "8b"

typedef struct _nvswitch_vbios_i2c_table_40
{
    bios_U008    version;
    bios_U008    header_size;
    bios_U008    entry_count;
    bios_U008    entry_size;
    bios_U008    flags;
} NVSWITCH_VBIOS_I2C_TABLE;
#define NVSWITCH_I2C_TABLE_FMT "5b"

typedef struct _nvswitch_vbios_i2c_entry
{
    bios_U032   device;
} NVSWITCH_VBIOS_I2C_ENTRY;
#define NVSWITCH_I2C_ENTRY_FMT "1d"

#endif //_ROM_NVSWITCH_H_

