/*
 * SPDX-FileCopyrightText: Copyright (c) 200-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVDEVID_H
#define NVDEVID_H



/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Private device ids defines  - only       defines !                  *
*                                                                           *
\***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////
//
// VENDOR/SUBVENDOR IDS
// XXX Cleanup to do: change PCI_VENDOR_* to NV_PCI_SUBID_VENDOR_*
//
///////////////////////////////////////////////////////////////////////////////////////////
#define NV_PCI_SUBID_VENDOR         15:0  /* RW--F */
#define NV_PCI_SUBID_VENDOR_AMD           0x1022
#define NV_PCI_SUBID_VENDOR_ALI           0x10B9
#define NV_PCI_SUBID_VENDOR_NVIDIA        0x10DE
#define NV_PCI_SUBID_VENDOR_INTEL         0x8086
#define NV_PCI_SUBID_VENDOR_VIA           0x1106
#define NV_PCI_SUBID_VENDOR_RCC           0x1166
#define NV_PCI_SUBID_VENDOR_MICRON_1      0x1042
#define NV_PCI_SUBID_VENDOR_MICRON_2      0x1344
#define NV_PCI_SUBID_VENDOR_APPLE         0x106B
#define NV_PCI_SUBID_VENDOR_SIS           0x1039
#define NV_PCI_SUBID_VENDOR_ATI           0x1002
#define NV_PCI_SUBID_VENDOR_TRANSMETA     0x1279
#define NV_PCI_SUBID_VENDOR_HP            0x103C
#define NV_PCI_SUBID_VENDOR_DELL          0x1028
#define NV_PCI_SUBID_VENDOR_FUJITSU       0x10cf
#define NV_PCI_SUBID_VENDOR_ASUS          0x1043
#define NV_PCI_SUBID_VENDOR_MSI           0x1462
#define NV_PCI_SUBID_VENDOR_FOXCONN       0x105B
#define NV_PCI_SUBID_VENDOR_ECS           0x1019
#define NV_PCI_SUBID_VENDOR_DFI_1         0x106E
#define NV_PCI_SUBID_VENDOR_TOSHIBA       0x1179
#define NV_PCI_SUBID_VENDOR_DFI_2         0x15BD
#define NV_PCI_SUBID_VENDOR_ACER          0x1025
#define NV_PCI_SUBID_VENDOR_GIGABYTE      0x1458
#define NV_PCI_SUBID_VENDOR_EVGA          0x3842
#define NV_PCI_SUBID_VENDOR_BROADCOM      0x1166
#define NV_PCI_SUBID_VENDOR_SUPERMICRO    0x15D9
#define NV_PCI_SUBID_VENDOR_BIOSTAR       0x1565
#define NV_PCI_SUBID_VENDOR_XFX           0x1682
#define NV_PCI_SUBID_VENDOR_PCPARTNER     0x19DA
#define NV_PCI_SUBID_VENDOR_LENOVO        0x17AA
#define NV_PCI_SUBID_VENDOR_FSC           0x1734
#define NV_PCI_SUBID_VENDOR_FTS           0x1734
#define NV_PCI_SUBID_VENDOR_COLORFUL      0x7377
#define NV_PCI_SUBID_VENDOR_ASROCK        0x1849
#define NV_PCI_SUBID_VENDOR_SHUTTLE       0x1297
#define NV_PCI_SUBID_VENDOR_CLEVO         0x1558
#define NV_PCI_SUBID_VENDOR_PEGATRON      0x1B0A
#define NV_PCI_SUBID_VENDOR_JETWAY        0x16F3
#define NV_PCI_SUBID_VENDOR_HIGHGRADE     0x1C6C
#define NV_PCI_SUBID_VENDOR_GALAXY        0x1B4C
#define NV_PCI_SUBID_VENDOR_ZOTAC         0x19DA
#define NV_PCI_SUBID_VENDOR_ARIMA         0x161F
#define NV_PCI_SUBID_VENDOR_BFG           0x19F1
#define NV_PCI_SUBID_VENDOR_SONY          0x104D
#define NV_PCI_SUBID_VENDOR_BITLAND       0x1642
#define NV_PCI_SUBID_VENDOR_PC_PARTNER    0x174B
#define NV_PCI_SUBID_VENDOR_CAVIUM        0x177D
#define NV_PCI_SUBID_VENDOR_NZXT          0x1D96

// XXX CKEANUP TO REMOVE IN FAVOR OF NV_PCI_SUBID_VENDOR_*
#define PCI_VENDOR_ID_AMD           0x1022
#define PCI_VENDOR_ID_ALI           0x10B9
#define PCI_VENDOR_ID_NVIDIA        0x10DE
#define PCI_VENDOR_ID_INTEL         0x8086
#define PCI_VENDOR_ID_VIA           0x1106
#define PCI_VENDOR_ID_RCC           0x1166
#define PCI_VENDOR_ID_MICRON_1      0x1042
#define PCI_VENDOR_ID_MICRON_2      0x1344
#define PCI_VENDOR_ID_APPLE         0x106B
#define PCI_VENDOR_ID_SIS           0x1039
#define PCI_VENDOR_ID_ATI           0x1002
#define PCI_VENDOR_ID_TRANSMETA     0x1279
#define PCI_VENDOR_ID_HP            0x103C
#define PCI_VENDOR_ID_DELL          0x1028
#define PCI_VENDOR_ID_FUJITSU       0x10cf
#define PCI_VENDOR_ID_ASUS          0x1043
#define PCI_VENDOR_ID_MSI           0x1462
#define PCI_VENDOR_ID_FOXCONN       0x105B
#define PCI_VENDOR_ID_ECS           0x1019
#define PCI_VENDOR_ID_DFI_1         0x106E
#define PCI_VENDOR_ID_TOSHIBA       0x1179
#define PCI_VENDOR_ID_DFI_2         0x15BD
#define PCI_VENDOR_ID_ACER          0x1025
#define PCI_VENDOR_ID_GIGABYTE      0x1458
#define PCI_VENDOR_ID_EVGA          0x3842
#define PCI_VENDOR_ID_BROADCOM      0x1166
#define PCI_VENDOR_ID_SUPERMICRO    0x15D9
#define PCI_VENDOR_ID_BIOSTAR       0x1565
#define PCI_VENDOR_ID_XFX           0x1682
#define PCI_VENDOR_ID_PCPARTNER     0x19DA
#define PCI_VENDOR_ID_LENOVO        0x17AA
#define PCI_VENDOR_ID_FSC           0x1734
#define PCI_VENDOR_ID_FTS           0x1734
#define PCI_VENDOR_ID_COLORFUL      0x7377
#define PCI_VENDOR_ID_ASROCK        0x1849
#define PCI_VENDOR_ID_SHUTTLE       0x1297
#define PCI_VENDOR_ID_CLEVO         0x1558
#define PCI_VENDOR_ID_PEGATRON      0x1B0A
#define PCI_VENDOR_ID_JETWAY        0x16F3
#define PCI_VENDOR_ID_HIGHGRADE     0x1C6C
#define PCI_VENDOR_ID_GALAXY        0x1B4C
#define PCI_VENDOR_ID_ZOTAC         0x19DA
#define PCI_VENDOR_ID_ARIMA         0x161F
#define PCI_VENDOR_ID_PC_PARTNER    0x174B
#define PCI_VENDOR_ID_APM           0x10E8
#define PCI_VENDOR_ID_IBM           0x1014
#define PCI_VENDOR_ID_NZXT          0x1D96
#define PCI_VENDOR_ID_MARVELL       0x177D
#define PCI_VENDOR_ID_REDHAT        0x1B36
#define PCI_VENDOR_ID_AMPERE        0x1DEF
#define PCI_VENDOR_ID_HUAWEI        0x19E5
#define PCI_VENDOR_ID_MELLANOX      0x15B3
#define PCI_VENDOR_ID_AMAZON        0x1D0F
#define PCI_VENDOR_ID_CADENCE       0x17CD
#define PCI_VENDOR_ID_ARM           0x13B5
#define PCI_VENDOR_ID_HYGON         0x1D94
#define PCI_VENDOR_ID_ALIBABA       0x1DED
#define PCI_VENDOR_ID_SIFIVE        0xF15E
#define PCI_VENDOR_ID_PLDA          0x1556
#define PCI_VENDOR_ID_PHYTIUM       0x1DB7

#define NV_PCI_DEVID_DEVICE                    31:16  /* RW--F */
#define NV_PCI_SUBID_DEVICE                    31:16  /* RW--F */

///////////////////////////////////////////////////////////////////////////////////////////
//
// GPU DEVICE IDS
//
///////////////////////////////////////////////////////////////////////////////////////////

#define NV_PCI_DEVID_DEVICE_PG171_SKU200_PG179_SKU220 0x25B6 /* NVIDIA A16 / NVIDIA A2 */
#define NV_PCI_DEVID_DEVICE_PG189_SKU600              0x1EBA

///////////////////////////////////////////////////////////////////////////////////////////
//
// SUBDEVICE IDs
//
///////////////////////////////////////////////////////////////////////////////////////////

// A16
#define NV_PCI_SUBID_DEVICE_PG171_SKU200             0x14A9

///////////////////////////////////////////////////////////////////////////////////////////
//
// CHIPSET IDs
//
///////////////////////////////////////////////////////////////////////////////////////////
// Desktop flavor of X58
#define X58_DESKTOP_DEVIDS                  0x3400, 0x3405
// Mobile version of X58
#define X58_MOBILE_DEVID                    0x3405
#define X58_MOBILE_CLEVO_7200_SSDEVID       0x7200

// Sandy bridge CLEVO platform
#define SANDYBRIDGE_P180HM_SSDEVID          0x8000
#define SandyBridge_E_X79_P270WM_SSDEVID    0x270
#define IvyBridge_Z75_P370EM_SSDEVID        0x371

// Device ID's of Devices present on Patsburg's PCIE bus.
#define PATSBURG_PCIE_DEVICE_MIN_DEVID      0x1D10
#define PATSBURG_PCIE_DEVICE_MAX_DEVID      0x1D1F
#define PATSBURG_PCIE_DEVICE_DEVID          0x244E

//Tylersburg Congurations
#define TYLERSBURG_DEVID                    0x3406

// Intel Barlow Ridge TB5 device IDs
#define DEVICE_ID_INTEL_BARLOW_RIDGE_5786_EGPU     0x5786
#define DEVICE_ID_INTEL_BARLOW_RIDGE_57A4_EGPU     0x57A4

// Intel Grantsdale definitions
#define DEVICE_ID_INTEL_2580_HOST_BRIDGE    0x2580
#define DEVICE_ID_INTEL_2581_ROOT_PORT      0x2581

// Intel Alderwood definitions
#define DEVICE_ID_INTEL_2584_HOST_BRIDGE    0x2584
#define DEVICE_ID_INTEL_2585_ROOT_PORT      0x2585

// Intel Alviso definitions
#define DEVICE_ID_INTEL_2590_HOST_BRIDGE    0x2590
#define DEVICE_ID_INTEL_2591_ROOT_PORT      0x2591

// Intel Tumwater definitions
#define DEVICE_ID_INTEL_359E_HOST_BRIDGE    0x359E
#define DEVICE_ID_INTEL_3597_ROOT_PORT      0x3597

// Intel Stoakley definitions
#define INTEL_4000_SUBDEVICE_ID             0x021D

// Intel SkullTrail definitions
#define INTEL_4003_SUBDEVICE_ID             0x5358

// Intel Core I7 CPU
#define INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I7   0x2C01

// Intel Core I5 CPU Lynnfield
#define INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_L 0x2C81
#define INTEL_LYNNFIELD_ROOTPORT_CPU1               0xD138
#define INTEL_LYNNFIELD_ROOTPORT_CPU2               0xD13A

// Intel Core I5 CPU Auburndale
#define INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_A 0x2D41

// Intel Core I5 CPU 650
#define INTEL_QUICKPATH_SYSTEM_ADDRESS_DECODER_I5_6 0x2D01

// Intel Poulsbo definitions
#define DEVICE_ID_INTEL_8100_HOST_BRIDGE    0x8100
#define DEVICE_ID_INTEL_8110_ROOT_PORT      0x8110
#define DEVICE_ID_INTEL_8112_ROOT_PORT      0x8112

// Intel TunnelCreek definitions
#define DEVICE_ID_INTEL_8180_ROOT_PORT      0x8180
#define DEVICE_ID_INTEL_8181_ROOT_PORT      0x8181
#define DEVICE_ID_INTEL_8184_ROOT_PORT      0x8184
#define DEVICE_ID_INTEL_8185_ROOT_PORT      0x8185

// Intel I/O Hub definitions
#define DEVICE_ID_INTEL_3408_ROOT_PORT      0x3408
#define DEVICE_ID_INTEL_3411_ROOT_PORT      0x3411
#define DEVICE_ID_INTEL_3420_ROOT_PORT      0x3420
#define DEVICE_ID_INTEL_3421_ROOT_PORT      0x3421

// Intel SandyBridge IIO definitions
#define DEVICE_ID_INTEL_3C02_ROOT_PORT      0x3c02
#define DEVICE_ID_INTEL_3C03_ROOT_PORT      0x3c03
#define DEVICE_ID_INTEL_3C04_ROOT_PORT      0x3c04
#define DEVICE_ID_INTEL_3C05_ROOT_PORT      0x3c05
#define DEVICE_ID_INTEL_3C06_ROOT_PORT      0x3c06
#define DEVICE_ID_INTEL_3C07_ROOT_PORT      0x3c07
#define DEVICE_ID_INTEL_3C08_ROOT_PORT      0x3c08
#define DEVICE_ID_INTEL_3C09_ROOT_PORT      0x3c09
#define DEVICE_ID_INTEL_3C0A_ROOT_PORT      0x3c0a
#define DEVICE_ID_INTEL_3C0B_ROOT_PORT      0x3c0b

// Intel Haswell-E definitions
#define DEVICE_ID_INTEL_2F00_HOST_BRIDGE    0x2f00
#define DEVICE_ID_INTEL_2F01_ROOT_PORT      0x2f01
#define DEVICE_ID_INTEL_2F02_ROOT_PORT      0x2f02
#define DEVICE_ID_INTEL_2F03_ROOT_PORT      0x2f03
#define DEVICE_ID_INTEL_2F04_ROOT_PORT      0x2f04
#define DEVICE_ID_INTEL_2F05_ROOT_PORT      0x2f05
#define DEVICE_ID_INTEL_2F06_ROOT_PORT      0x2f06
#define DEVICE_ID_INTEL_2F07_ROOT_PORT      0x2f07
#define DEVICE_ID_INTEL_2F08_ROOT_PORT      0x2f08
#define DEVICE_ID_INTEL_2F09_ROOT_PORT      0x2f09
#define DEVICE_ID_INTEL_2F0A_ROOT_PORT      0x2f0a
#define DEVICE_ID_INTEL_2F0B_ROOT_PORT      0x2f0b

#define DEVICE_ID_INTEL_0C01_ROOT_PORT      0x0c01

// Intel IvyTown definitions

#define DEVICE_ID_INTEL_0E02_ROOT_PORT      0x0e02
#define DEVICE_ID_INTEL_0E03_ROOT_PORT      0x0e03
#define DEVICE_ID_INTEL_0E04_ROOT_PORT      0x0e04
#define DEVICE_ID_INTEL_0E05_ROOT_PORT      0x0e05
#define DEVICE_ID_INTEL_0E06_ROOT_PORT      0x0e06
#define DEVICE_ID_INTEL_0E07_ROOT_PORT      0x0e07
#define DEVICE_ID_INTEL_0E08_ROOT_PORT      0x0e08
#define DEVICE_ID_INTEL_0E09_ROOT_PORT      0x0e09
#define DEVICE_ID_INTEL_0E0A_ROOT_PORT      0x0e0a
#define DEVICE_ID_INTEL_0E0B_ROOT_PORT      0x0e0b
// Intel Ivy Bridge E definitions
#define DEVICE_ID_INTEL_0E00_HOST_BRIDGE    0x0E00

// Intel PCH definitions
#define DEVICE_ID_INTEL_9D10_PCH_BRIDGE     0x9d10
#define DEVICE_ID_INTEL_9D18_PCH_BRIDGE     0x9d18
#define DEVICE_ID_INTEL_A117_PCH_BRIDGE     0xa117
#define DEVICE_ID_INTEL_A118_PCH_BRIDGE     0xa118
#define DEVICE_ID_INTEL_9C98_PCH_BRIDGE     0x9c98

// Intel Broadwell definitions
#define DEVICE_ID_INTEL_6F00_HOST_BRIDGE    0x6f00
#define DEVICE_ID_INTEL_6F01_ROOT_PORT      0x6f01
#define DEVICE_ID_INTEL_6F02_ROOT_PORT      0x6f02
#define DEVICE_ID_INTEL_6F03_ROOT_PORT      0x6f03
#define DEVICE_ID_INTEL_6F04_ROOT_PORT      0x6f04
#define DEVICE_ID_INTEL_6F05_ROOT_PORT      0x6f05
#define DEVICE_ID_INTEL_6F06_ROOT_PORT      0x6f06
#define DEVICE_ID_INTEL_6F07_ROOT_PORT      0x6f07
#define DEVICE_ID_INTEL_6F08_ROOT_PORT      0x6f08
#define DEVICE_ID_INTEL_6F09_ROOT_PORT      0x6f09
#define DEVICE_ID_INTEL_6F0A_ROOT_PORT      0x6f0A
#define DEVICE_ID_INTEL_6F0B_ROOT_PORT      0x6f0B
#define DEVICE_ID_INTEL_1601_ROOT_PORT                  0x1601
#define DEVICE_ID_INTEL_1605_ROOT_PORT                  0x1605
#define DEVICE_ID_INTEL_1609_ROOT_PORT                  0x1609
#define DEVICE_ID_INTEL_BROADWELL_U_HOST_BRIDGE         0x1604
#define DEVICE_ID_INTEL_BROADWELL_H_HOST_BRIDGE         0x1614

// Intel Skylake definitions
#define DEVICE_ID_INTEL_1901_ROOT_PORT                  0x1901
#define DEVICE_ID_INTEL_1905_ROOT_PORT                  0x1905
#define DEVICE_ID_INTEL_1909_ROOT_PORT                  0x1909
#define DEVICE_ID_INTEL_SKYLAKE_U_HOST_BRIDGE           0x1904
#define DEVICE_ID_INTEL_SKYLAKE_S_HOST_BRIDGE           0x191F
#define DEVICE_ID_INTEL_SKYLAKE_H_HOST_BRIDGE           0x1910

// Intel Skylake-E definitions
#define DEVICE_ID_INTEL_2030_ROOT_PORT                  0x2030
#define DEVICE_ID_INTEL_2033_ROOT_PORT                  0x2033

// Intel Kabylake definitions
#define DEVICE_ID_INTEL_KABYLAKE_U_HOST_BRIDGE          0x5904
#define DEVICE_ID_INTEL_KABYLAKE_H_HOST_BRIDGE          0x5910

// AMD Matisse, Rome definitions
#define DEVICE_ID_AMD_1483_ROOT_PORT        0x1483
// AMD Castle Peak definition
#define DEVICE_ID_AMD_1480_ROOT_PORT        0x1480
// AMD Renoir-H definition
#define DEVICE_ID_AMD_1630_ROOT_PORT        0x1630

// Dell SkullTrail definitions
#define DELL_4003_SUBDEVICE_ID              0x021D

// Dell Quicksilver MLK definitions
#define DELL_0040_SUBDEVICE_ID              0x043a

// HP Tylersburg definitions
#define TYLERSBURG_Z800_SSDEVID             0x130B

// HP Romley definitions
#define ROMLEY_Z820_SSDEVID                 0x158B
#define ROMLEY_Z620_SSDEVID                 0x158A
#define ROMLEY_Z420_SSDEVID                 0x1589

// HP Grantley definitions
#define GRANTLEY_Z840_SSDEVID               0x2129
#define GRANTLEY_Z640_SSDEVID               0x212A
#define GRANTLEY_Z440_SSDEVID               0x212B

// HP PURELY definitions
#define HP_QUADRO_Z4GEN4_DEVID              0xA2D2
#define PURLEY_Z8GEN4_SSDEVID               0x81C7
#define PURLEY_Z6GEN4_SSDEVID               0x81C6
#define PURLEY_Z4GEN4_SSDEVID               0x81C5

// Lenovo Romley definitions
#define ROMLEY_C30_SSDEVID                  0x1028
#define ROMLEY_D30_SSDEVID                  0x1027
#define ROMLEY_S30_SSDEVID                  0x1026

// Dell Romley definitions
#define ROMLEY_T7600_SSDEVID                0x0495
#define ROMLEY_T5600_SSDEVID                0x0496
#define ROMLEY_T3600_SSDEVID                0x0497

// Dell Romley + IVB-EP CPU Refresh
#define IVYTOWN_T7610_SSDEVID               0x05D4
#define IVYTOWN_T5610_SSDEVID               0x05D3

// Dell Romley (Ipanema)
#define ROMLEY_R7610_SSDEVID                0x05A1

// FTS Romley definitions
#define ROMLEY_R920_SSDEVID                 0x11B6

// Lenovo Grantley (Messi, Pele, Ronaldo)
#define GRANTLEY_V40_SSDEVID                0x1031
#define GRANTLEY_D40_SSDEVID                0x1030
#define GRANTLEY_S40_SSDEVID                0x102F

// Dell Grantley (Avalon)
#define GRANTLEY_T7810_SSDEVID              0x0618
#define GRANTLEY_T7910_SSDEVID              0x0619

// Lenovo Purley (Nile, Volga)
#define PURLEY_P920_SSDEVID                 0x1038
#define PURLEY_P720_SSDEVID                 0x1037
#define PURLEY_P520_SSDEVID                 0x1036

// Dell Purley(Matira)
#define PURLEY_MATIRA3X_DEVID               0xA2D2
#define PURLEY_MATIRA3X_SSDEVID             0x08B1
#define PURLEY_MATIRA3_SSDEVID              0x0738
#define PURLEY_MATIRA5_SSDEVID              0x0739
#define PURLEY_MATIRA7_SSDEVID              0x073A

//FTS Purley
#define PURLEY_R970_SSDEVID                 0x1230
#define PURLEY_M770_SSDEVID                 0x1231

// HP Arrandale, Clarksfield, X58 workstation definitions
#define ARRANDALE_Z200SFF_SSDEVID           0x304A
#define CLARKSFIELD_Z200_SSDEVID            0x170B
#define X58_Z400_SSDEVID                    0x1309

// GIGABYTE Sniper 3 (Z77)
#define GIGABYTE_SNIPER_3_SSDEVID_1         0x5000
#define GIGABYTE_SNIPER_3_SSDEVID_2         0x5001

// Supermicro Quadro VCA definitions
#define SUPERMICRO_QUADRO_VCA_DEVID         0x8D44
#define SUPERMICRO_QUADRO_VCA_SSDEVID       0x7270

// Asus Quadro BOXX definitions
#define ASUS_QUADRO_BOXX_DEVID              0x8D44
#define ASUS_QUADRO_BOXX_SSDEVID            0x85F6

// APEXX8 Quadro BOXX definitions
#define APEXX8_QUADRO_BOXX_DEVID            0xA2D3
#define APEXX8_QUADRO_BOXX_SSDEVID          0x098e

// APEXX5 Quadro BOXX definitions
#define APEXX5_QUADRO_BOXX_DEVID            0xA2D3
#define APEXX5_QUADRO_BOXX_SSDEVID          0x1000

// ASUS X99-E-10G
#define ASUS_X99_E_10G_SSDEVID              0x8600

// VIA definitions
#define DEVICE_ID_VIA_VT8369B_HOST_BRIDGE   0x0308

// Foxconn Einstein 64 [8086:a1c1][105b:7270]
#define FOXCONN_EINSTEIN_64_DEVID           0xA1C1
#define FOXCONN_EINSTEIN_64_SSDEVID         0x7270

// Cavium, Inc. CN99xx [ThunderX2]  [177d:af00]
#define CAVIUM_X2_DEVID                     0xAF00

// Lenovo Tomcat/Falcon/Hornet Workstations
#define LENOVO_TOMCAT_DEVID                 0x1B81
#define LENOVO_TOMCAT_SSDEVID               0x104e
#define LENOVO_FALCON_DEVID                 0x7A8A
#define LENOVO_FALCON_SSDEVID               0x1055
#define LENOVO_HORNET_DEVID                 0x7A8A
#define LENOVO_HORNET_SSDEVID               0x1056

// NVIDIA C51
#define NVIDIA_C51_DEVICE_ID_MIN                        0x2F0
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_0               0x2F0
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_1               0x2F1
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_2               0x2F2
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_3               0x2F3
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_IGPU_DISABLE_0  0x2F4
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_IGPU_DISABLE_1  0x2F5
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_IGPU_DISABLE_2  0x2F6
#define NVIDIA_C51_ULDT_CFG_0_DEVICE_ID_IGPU_DISABLE_3  0x2F7
#define NVIDIA_C51_DEVICE_ID_MAX                        0x2F7

// NVIDIA MCP55
#define NVIDIA_MCP55_ULDT_CFG_0_DEVICE_ID_DEFAULT      0x0369

// NVIDIA MCP61
#define NVIDIA_MCP61_ULDT_CFG_0_DEVICE_ID_DEFAULT      0x03EA
#define NVIDIA_MCP61_ULDT_CFG_0_DEVICE_ID_PA           0x03E2

// NVIDIA C55
#define NVIDIA_C55_CPU_PCI_0_DEVICE_ID_PRO             0x03A0
#define NVIDIA_C55_CPU_PCI_0_DEVICE_ID_PRO             0x03A0
#define NVIDIA_C55_CPU_PCI_0_DEVICE_ID_SLIX16          0x03A1
#define NVIDIA_C55_CPU_PCI_0_DEVICE_ID_SLI             0x03A3
#define NVIDIA_C55_CPU_PCI_0_DEVICE_ID_U               0x03A2

// NVIDIA MCP65
#define NVIDIA_MCP65_ULDT_CFG_0_DEVICE_ID_DEFAULT      0x0444

// NVIDIA MCP67/MCP68
#define NVIDIA_MCP67_ULDT_CFG_0_DEVICE_ID_DEFAULT      0x0547

// NVIDIA MCP73
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_PV            0x07C0
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_O             0x07C1
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_S             0x07C2
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_V             0x07C3
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_RSVD_0        0x07C4
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_RSVD_1        0x07C5
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_RSVD_2        0x07C6
#define NVIDIA_MCP73_CPU_PCI_0_DEVICE_ID_D             0x07C7

// NVIDIA C73
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_SLI2            0x0800
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_SLI_ALL         0x0801
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_SLIX8           0x0802
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_U               0x0803
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_RESERVED_0      0x0804
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_RESERVED_1      0x0805
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_RESERVED_2      0x0806
#define NVIDIA_C73_CPU_PCI_0_DEVICE_ID_RESERVED_3      0x0807

// NVIDIA MCP77/78
#define NVIDIA_MCP77_ULDT_CFG_0_DEVICE_ID_DEFAULT      0x0754
#define NVIDIA_MCP77_ULDT_CFG_0_DEVICE_ID_1            0x0755
#define NVIDIA_MCP77_ULDT_CFG_0_DEVICE_ID_2            0x0756
#define NVIDIA_MCP77_ULDT_CFG_0_DEVICE_ID_3            0x0757
#define NVIDIA_MCP77_MCP_SM_CFG_0_DEVICE_ID_UNIT_SM    0x0752

// NVIDIA MCP79/7A
#define NVIDIA_MCP79_CPU_PCI_0_DEVICE_ID_DEFAULT       0x0A80
#define NVIDIA_MCP79_CPU_PCI_0_DEVICE_ID_SLIX16        0x0A81
#define NVIDIA_MCP79_CPU_PCI_0_DEVICE_ID_SLI           0x0A82
#define NVIDIA_MCP79_CPU_PCI_0_DEVICE_ID_U             0x0A83
#define NVIDIA_MCP79_CPU_PCI_0_DEVICE_ID_GM            0x0A84
#define NVIDIA_MCP79_CPU_PCI_0_DEVICE_ID_GVM           0x0A85
#define NVIDIA_MCP79_MCP_SM_CFG_0_DEVICE_ID_UNIT_SM    0x0AA2

// NVIDIA MCP89/P83
#define NVIDIA_MCP89_CPU_PCI_0_DEVICE_ID_DEFAULT   0x00000D60

///////////////////////////////////////////////////////////////////////////////////////////
//
// enumeration of chipset families
//
///////////////////////////////////////////////////////////////////////////////////////////

enum {
      CS_UNKNOWN      = 0x0000,
      CS_UNKNOWN_PCIE = 0x1000
,     CS_INTEL_2580
,     CS_INTEL_2584
,     CS_INTEL_2588
,     CS_INTEL_2590
,     CS_INTEL_25E0
,     CS_INTEL_29X0
,     CS_INTEL_29E0
,     CS_INTEL_359E
,     CS_INTEL_4000
,     CS_INTEL_4003
,     CS_INTEL_3400
,     CS_INTEL_3B42
,     CS_INTEL_2770
,     CS_INTEL_2774
,     CS_INTEL_277C
,     CS_INTEL_2A40
,     CS_INTEL_2E00
,     CS_INTEL_0040
,     CS_INTEL_1C10
,     CS_INTEL_1C46
,     CS_INTEL_1C49
,     CS_INTEL_1D40
,     CS_INTEL_8D47
,     CS_INTEL_1E10
,     CS_INTEL_8C4B
,     CS_INTEL_8CC4
,     CS_INTEL_A145
,     CS_INTEL_A2C5
,     CS_INTEL_A242
,     CS_INTEL_A2D2
,     CS_INTEL_A2C9
,     CS_INTEL_A301
,     CS_INTEL_0685
,     CS_INTEL_4381
,     CS_INTEL_7A82
,     CS_NVIDIA_CK804
,     CS_NVIDIA_C19
,     CS_NVIDIA_C51
,     CS_NVIDIA_MCP55
,     CS_NVIDIA_MCP61
,     CS_NVIDIA_C55
,     CS_NVIDIA_MCP65
,     CS_NVIDIA_MCP67
,     CS_NVIDIA_MCP73
,     CS_NVIDIA_C73
,     CS_NVIDIA_MCP77
,     CS_NVIDIA_MCP79
,     CS_NVIDIA_MCP89
,     CS_NVIDIA_TEGRA3
,     CS_SIS_649
,     CS_SIS_656
,     CS_ATI_RS400
,     CS_ATI_RS400_A21
,     CS_ATI_RS480
,     CS_ATI_RS480_A21
,     CS_AMD_RS780
,     CS_VIA_VT8369B
,     CS_ATI_FX790
,     CS_ATI_RD850
,     CS_ATI_RD870
,     CS_ATI_RD890
,     CS_ATI_FX890
,     CS_ATI_RX780
,     CS_ATI_FX990
,     CS_AMD_GX890
,     CS_AMD_X370
,     CS_VIA_VX900
,     CS_APM_STORM
,     CS_IBM_VENICE
,     CS_NVIDIA_T124
,     CS_NVIDIA_T210
,     CS_NVIDIA_T186
,     CS_NVIDIA_T194
,     CS_NVIDIA_T234
,     CS_NVIDIA_T23x
,     CS_NVIDIA_TH500
,     CS_MARVELL_THUNDERX2
,     CS_REDHAT_QEMU
,     CS_AMPERE_EMAG
,     CS_HUAWEI_KUNPENG920
,     CS_MELLANOX_BLUEFIELD
,     CS_AMAZON_GRAVITRON2
,     CS_FUJITSU_A64FX
,     CS_AMPERE_ALTRA
,     CS_ARM_NEOVERSEN1
,     CS_MARVELL_OCTEON_CN96XX
,     CS_MARVELL_OCTEON_CN98XX
,     CS_INTEL_C620
,     CS_HYGON_C86
,     CS_PHYTIUM_S2500
,     CS_MELLANOX_BLUEFIELD2
,     CS_MELLANOX_BLUEFIELD3
,     CS_ALIBABA_YITIAN
,     CS_INTEL_1B81
,     CS_INTEL_18DC
,     CS_INTEL_7A04
,     CS_INTEL_5795
,     CS_SIFIVE_FU740_C000
,     CS_PLDA_XPRESSRICH_AXI_REF
,     CS_AMPERE_AMPEREONE
,     CS_PHYTIUM_S5000
,     CS_MAX_PCIE
};

enum {
    RP_UNKNOWN = 0
,   RP_BROADCOM_HT2100
,   RP_INTEL_2581
,   RP_INTEL_2585
,   RP_INTEL_2589
,   RP_INTEL_2591
,   RP_INTEL_3597
,   RP_INTEL_2775
,   RP_INTEL_2771
,   RP_INTEL_8110
,   RP_INTEL_8112
,   RP_INTEL_8180
,   RP_INTEL_8181
,   RP_INTEL_8184
,   RP_INTEL_8185
,   RP_INTEL_3C02
,   RP_INTEL_3C03
,   RP_INTEL_3C04
,   RP_INTEL_3C05
,   RP_INTEL_3C06
,   RP_INTEL_3C07
,   RP_INTEL_3C08
,   RP_INTEL_3C09
,   RP_INTEL_3C0A
,   RP_INTEL_3C0B
,   RP_INTEL_2F04
,   RP_INTEL_2F08
,   RP_INTEL_0C01
,   RP_INTEL_1601
,   RP_INTEL_1605
,   RP_INTEL_1609
,   RP_INTEL_1901
,   RP_INTEL_1905
,   RP_INTEL_1909
,   RP_INTEL_5904
,   RP_NVIDIA_CK804
,   RP_NVIDIA_C19
,   RP_NVIDIA_C51
,   RP_NVIDIA_MCP55
,   RP_NVIDIA_MCP61
,   RP_NVIDIA_C55
,   RP_NVIDIA_MCP65
};

#endif //NVDEVID_H

