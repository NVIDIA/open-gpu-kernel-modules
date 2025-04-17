/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVCST_H
#define NVCST_H

#include <platform/chipset/chipset.h>
#include <platform/chipset/chipset_info.h>
#include <nvpcie.h>
#include <nvdevid.h>

#define CHIPSET_SETUP_FUNC(name) static NV_STATUS name(OBJCL *pCl);

CHIPSET_SETUP_FUNC(Intel_25XX_setupFunc)
CHIPSET_SETUP_FUNC(Intel_27XX_setupFunc)
CHIPSET_SETUP_FUNC(Intel_2A40_setupFunc)
CHIPSET_SETUP_FUNC(Intel_0040_setupFunc)
CHIPSET_SETUP_FUNC(Intel_2E00_setupFunc)
CHIPSET_SETUP_FUNC(Intel_25E0_setupFunc)
CHIPSET_SETUP_FUNC(Intel_29XX_setupFunc)
CHIPSET_SETUP_FUNC(Intel_29X0_setupFunc)
CHIPSET_SETUP_FUNC(Intel_29E0_setupFunc)
CHIPSET_SETUP_FUNC(Intel_359E_setupFunc)
CHIPSET_SETUP_FUNC(Intel_4000_setupFunc)
CHIPSET_SETUP_FUNC(Intel_4003_setupFunc)
CHIPSET_SETUP_FUNC(Intel_3400_setupFunc)
CHIPSET_SETUP_FUNC(Intel_3B42_setupFunc)
CHIPSET_SETUP_FUNC(Intel_1C46_setupFunc)
CHIPSET_SETUP_FUNC(Intel_1C10_setupFunc)
CHIPSET_SETUP_FUNC(Intel_1C4B_setupFunc)
CHIPSET_SETUP_FUNC(Intel_1C49_setupFunc)
CHIPSET_SETUP_FUNC(Intel_1D40_setupFunc)
CHIPSET_SETUP_FUNC(Intel_8D47_setupFunc)
CHIPSET_SETUP_FUNC(Intel_8D44_setupFunc)
CHIPSET_SETUP_FUNC(Intel_1E10_setupFunc)
CHIPSET_SETUP_FUNC(Intel_8C4B_setupFunc)
CHIPSET_SETUP_FUNC(Intel_8CC4_setupFunc)
CHIPSET_SETUP_FUNC(Intel_A145_setupFunc)
CHIPSET_SETUP_FUNC(Intel_A2C5_setupFunc)
CHIPSET_SETUP_FUNC(Intel_A242_setupFunc)
CHIPSET_SETUP_FUNC(Intel_A2D2_setupFunc)
CHIPSET_SETUP_FUNC(Intel_A2C9_setupFunc)
CHIPSET_SETUP_FUNC(Intel_A301_setupFunc)
CHIPSET_SETUP_FUNC(Intel_0685_setupFunc)
CHIPSET_SETUP_FUNC(Intel_4381_setupFunc)
CHIPSET_SETUP_FUNC(Intel_7A82_setupFunc)
CHIPSET_SETUP_FUNC(Intel_7A04_setupFunc)
CHIPSET_SETUP_FUNC(Intel_5795_setupFunc)
CHIPSET_SETUP_FUNC(Intel_1B81_setupFunc)
CHIPSET_SETUP_FUNC(SiS_656_setupFunc)
CHIPSET_SETUP_FUNC(ATI_RS400_setupFunc)
CHIPSET_SETUP_FUNC(ATI_RS480_setupFunc)
CHIPSET_SETUP_FUNC(ATI_RD870_setupFunc)
CHIPSET_SETUP_FUNC(ATI_RD890_setupFunc)
CHIPSET_SETUP_FUNC(ATI_RX780_setupFunc)
CHIPSET_SETUP_FUNC(ATI_FX990_setupFunc)
CHIPSET_SETUP_FUNC(AMD_RS780_setupFunc)
CHIPSET_SETUP_FUNC(AMD_FX790_setupFunc)
CHIPSET_SETUP_FUNC(AMD_FX890_setupFunc)
CHIPSET_SETUP_FUNC(AMD_X370_setupFunc)
CHIPSET_SETUP_FUNC(VIA_VX900_setupFunc)
CHIPSET_SETUP_FUNC(APM_Storm_setupFunc)
CHIPSET_SETUP_FUNC(ARMV8_generic_setupFunc)
CHIPSET_SETUP_FUNC(Marvell_ThunderX2_setupFunc)
CHIPSET_SETUP_FUNC(QEMU_setupFunc)
CHIPSET_SETUP_FUNC(Ampere_eMag_setupFunc)
CHIPSET_SETUP_FUNC(Huawei_Kunpeng920_setupFunc)
CHIPSET_SETUP_FUNC(Mellanox_BlueField_setupFunc)
CHIPSET_SETUP_FUNC(Mellanox_BlueField3_setupFunc)
CHIPSET_SETUP_FUNC(Amazon_Gravitron2_setupFunc)
CHIPSET_SETUP_FUNC(Fujitsu_A64FX_setupFunc)
CHIPSET_SETUP_FUNC(Ampere_Altra_setupFunc)
CHIPSET_SETUP_FUNC(Arm_NeoverseN1_setupFunc)
CHIPSET_SETUP_FUNC(Ampere_AmpereOne_setupFunc)
CHIPSET_SETUP_FUNC(Nvidia_T210_setupFunc)
CHIPSET_SETUP_FUNC(Nvidia_T194_setupFunc)
CHIPSET_SETUP_FUNC(Nvidia_TH500_setupFunc)
CHIPSET_SETUP_FUNC(PLDA_XpressRichAXI_setupFunc)
CHIPSET_SETUP_FUNC(Riscv_generic_setupFunc)


// Keep string length <=32 (including termination) to avoid string copy overflow
CSINFO chipsetInfo[] =
{
    // PCI Express chipset

    {PCI_VENDOR_ID_INTEL,       0x2580, CS_INTEL_2580,      "Grantsdale",   Intel_25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2584, CS_INTEL_2584,      "Alderwood",    Intel_25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2588, CS_INTEL_2588,      "Intel2588",    Intel_25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2590, CS_INTEL_2590,      "Alviso",       Intel_25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x25C0, CS_INTEL_25E0,      "Greencreek",   Intel_25E0_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x25E0, CS_INTEL_25E0,      "Greencreek",   Intel_25E0_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x29B0, CS_INTEL_29X0,      "IntelQ35",     Intel_29X0_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x29C0, CS_INTEL_29X0,      "BearlakeB",    Intel_29X0_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x29D0, CS_INTEL_29X0,      "IntelQ33",     Intel_29X0_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x29E0, CS_INTEL_29E0,      "BearlakeX",    Intel_29E0_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x359E, CS_INTEL_359E,      "Tumwater",     Intel_359E_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x4000, CS_INTEL_4000,      "Stoakley",     Intel_4000_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x4003, CS_INTEL_4003,      "SkullTrail",   Intel_4003_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x3400, CS_INTEL_3400,      "IntelX58",     Intel_3400_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x3403, CS_INTEL_3400,      "IntelX58",     Intel_3400_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x3405, CS_INTEL_3400,      "IntelX58",     Intel_3400_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x3406, CS_INTEL_3400,      "Tylersburg",   Intel_3400_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2770, CS_INTEL_2770,      "Lakeport",     Intel_25XX_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2774, CS_INTEL_2774,      "Glenwood",     Intel_27XX_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x277C, CS_INTEL_277C,      "Glenwood",     Intel_27XX_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2A40, CS_INTEL_2A40,      "Montevina",    Intel_2A40_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2E00, CS_INTEL_2E00,      "Eaglelake",    Intel_2E00_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2E10, CS_INTEL_2E00,      "Eaglelake",    Intel_2E00_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2E20, CS_INTEL_2E00,      "Eaglelake",    Intel_2E00_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x2E30, CS_INTEL_2E00,      "Eaglelake",    Intel_2E00_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x0044, CS_INTEL_0040,      "Arrandale/Auburndale", Intel_0040_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x0062, CS_INTEL_0040,      "Arrandale/Auburndale", Intel_0040_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xD130, CS_INTEL_3B42,      "Clarksfield",          Intel_3B42_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xD132, CS_INTEL_3B42,      "Clarksfield",          Intel_3B42_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x3B42, CS_INTEL_3B42,      "P55/PM55/H57",         Intel_3B42_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1C46, CS_INTEL_1C46,      "IntelP67-CougarPoint", Intel_1C46_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1C4B, CS_INTEL_1C46,      "HuronRiver-HM67",      Intel_1C4B_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1C4F, CS_INTEL_1C46,      "HuronRiver-QM67",      Intel_1C4B_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1C49, CS_INTEL_1C49,      "HuronRiver-HM65",      Intel_1C49_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1C44, CS_INTEL_1C46,      "IntelZ68",             Intel_1C46_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1C10, CS_INTEL_1C10,      "IntelP67",             Intel_1C10_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1D40, CS_INTEL_1D40,      "IntelX79",             Intel_1D40_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1D41, CS_INTEL_1D40,      "IntelX79",             Intel_1D40_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1E10, CS_INTEL_1E10,      "IntelZ75",             Intel_1E10_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x0150, CS_INTEL_1E10,      "IntelZ77A-GD55",       Intel_1E10_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x0151, CS_INTEL_1E10,      "IntelZ77A-GD55",       Intel_1E10_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x0100, CS_INTEL_1E10,      "IntelZ77A-GD55",       Intel_1E10_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8C4B, CS_INTEL_8C4B,      "SharkBay-HM87",        Intel_8C4B_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8C44, CS_INTEL_8C4B,      "SharkBay-Z87",         Intel_8C4B_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8C41, CS_INTEL_8C4B,      "SharkBay-H8x/P8x",     Intel_8C4B_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8C49, CS_INTEL_8C4B,      "SharkBay-HM86",        Intel_8C4B_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8C52, CS_INTEL_8C4B,      "SharkBay-E3",          Intel_8C4B_setupFunc}, // Does not support SLI
    {PCI_VENDOR_ID_INTEL,       0x8CC4, CS_INTEL_8CC4,      "IntelZ97",             Intel_8CC4_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8CC3, CS_INTEL_8CC4,      "IntelHM97",            Intel_8CC4_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA145, CS_INTEL_A145,      "IntelZ170",            Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA14E, CS_INTEL_A145,      "IntelHM170",           Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA150, CS_INTEL_A145,      "IntelHM170",           Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA149, CS_INTEL_A145,      "SkyLake C236",         Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA14A, CS_INTEL_A145,      "SkyLake C232",         Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA14D, CS_INTEL_A145,      "SkyLake-H",            Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA244, CS_INTEL_A145,      "SkyLake C620",         Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C8, CS_INTEL_A145,      "SkyLake C620",         Intel_A145_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8D47, CS_INTEL_8D47,      "IntelX99",             Intel_8D47_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x8D44, CS_INTEL_8D47,      "IntelC612",            Intel_8D44_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA2C5, CS_INTEL_A2C5,      "IntelZ270",            Intel_A2C5_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA154, CS_INTEL_A2C5,      "IntelZ270",            Intel_A2C5_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA152, CS_INTEL_A2C5,      "IntelRX9S",            Intel_A2C5_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA242, CS_INTEL_A242,      "IntelC422",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA241, CS_INTEL_A242,      "IntelC422",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA243, CS_INTEL_A242,      "IntelC422",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA2D2, CS_INTEL_A2D2,      "IntelX299",            Intel_A2D2_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA2D3, CS_INTEL_A242,      "IntelC422",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C1, CS_INTEL_A242,      "IntelC621",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C2, CS_INTEL_A242,      "IntelC622",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C3, CS_INTEL_A242,      "IntelC624",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C4, CS_INTEL_A242,      "IntelC625",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C5, CS_INTEL_A242,      "IntelC626",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C6, CS_INTEL_A242,      "IntelC627",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1C7, CS_INTEL_A242,      "IntelC628",            Intel_A242_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA2C9, CS_INTEL_A2C9,      "IntelZ370",            Intel_A2C9_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA310, CS_INTEL_A2C9,      "IntelZ370",            Intel_A2C9_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA30E, CS_INTEL_A2C9,      "IntelZ370",            Intel_A2C9_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA305, CS_INTEL_A2C9,      "IntelZ390",            Intel_A2C9_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA30D, CS_INTEL_A2C9,      "IntelH370",            Intel_A2C9_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA301, CS_INTEL_A301,      "Intel-CannonLake",     Intel_A301_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x0685, CS_INTEL_0685,      "Intel-CometLake",      Intel_0685_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0xA1CB, CS_INTEL_C620,      "Intel-IceLake",        NULL},
	{PCI_VENDOR_ID_INTEL,       0x4381, CS_INTEL_4381,      "Intel-RocketLake",     Intel_4381_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x4385, CS_INTEL_4381,      "Intel-RocketLake",     Intel_4381_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x7A82, CS_INTEL_7A82,      "Intel-AlderLake",      Intel_7A82_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x7A84, CS_INTEL_7A82,      "Intel-AlderLake",      Intel_7A82_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x1B81, CS_INTEL_1B81,      "Intel-SapphireRapids", Intel_1B81_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x7A8A, CS_INTEL_1B81,      "Intel-SapphireRapids", Intel_1B81_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x18DC, CS_INTEL_18DC,      "Intel-IceLake",        NULL},
    {PCI_VENDOR_ID_INTEL,       0x7A04, CS_INTEL_7A04,      "Intel-RaptorLake",     Intel_7A04_setupFunc},
    {PCI_VENDOR_ID_INTEL,       0x5795, CS_INTEL_5795,      "Intel-GraniteRapids",  Intel_5795_setupFunc},

    {PCI_VENDOR_ID_NVIDIA,      0x0FAE, CS_NVIDIA_T210,      "T210",      Nvidia_T210_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x0FAF, CS_NVIDIA_T210,      "T210",      Nvidia_T210_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x10E5, CS_NVIDIA_T186,      "T186",      Nvidia_T210_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x10E6, CS_NVIDIA_T186,      "T186",      Nvidia_T210_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x1AD0, CS_NVIDIA_T194,      "T194",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x1AD1, CS_NVIDIA_T194,      "T194",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x1AD2, CS_NVIDIA_T194,      "T194",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x229A, CS_NVIDIA_T234,      "T234",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x229C, CS_NVIDIA_T234,      "T234",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x229E, CS_NVIDIA_T234,      "T234",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22C2, CS_NVIDIA_T23x,      "T23x",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22C3, CS_NVIDIA_T23x,      "T23x",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22B1, CS_NVIDIA_TH500,     "TH500",     Nvidia_TH500_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22B2, CS_NVIDIA_TH500,     "TH500",     Nvidia_TH500_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22B3, CS_NVIDIA_TH500,     "TH500",     Nvidia_TH500_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22B4, CS_NVIDIA_TH500,     "TH500",     Nvidia_TH500_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22B8, CS_NVIDIA_TH500,     "TH500",     Nvidia_TH500_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22B9, CS_NVIDIA_TH500,     "TH500",     Nvidia_TH500_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22D6, CS_NVIDIA_T264,      "T264",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22D8, CS_NVIDIA_T264,      "T264",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x22DA, CS_NVIDIA_T264,      "T264",      Nvidia_T194_setupFunc},
    {PCI_VENDOR_ID_NVIDIA,      0x2B00, CS_NVIDIA_T264,      "T264",      Nvidia_T194_setupFunc},

    {PCI_VENDOR_ID_SIS,         0x0649, CS_SIS_649,          "649",          SiS_656_setupFunc},
    {PCI_VENDOR_ID_SIS,         0x0656, CS_SIS_656,          "656",          SiS_656_setupFunc},

    {PCI_VENDOR_ID_ATI,         0x5A31, CS_ATI_RS400,        "RS400",        ATI_RS400_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5A33, CS_ATI_RS400,        "RS400",        ATI_RS400_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5950, CS_ATI_RS480,        "RS480",        ATI_RS480_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5951, CS_ATI_RS480,        "RS480",        ATI_RS480_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5956, CS_ATI_FX790,        "FX790"        ,AMD_FX790_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5A11, CS_ATI_FX890,        "FX890"        ,AMD_FX890_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5a13, CS_ATI_RD850,        "RD850"        ,ATI_RD870_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5a12, CS_ATI_RD870,        "RD870"        ,ATI_RD870_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5a10, CS_ATI_RD890,        "RD890"        ,ATI_RD890_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5957, CS_ATI_RX780,        "RX780"        ,ATI_RX780_setupFunc},
    {PCI_VENDOR_ID_ATI,         0x5A14, CS_ATI_FX990,        "FX990/X990/970",ATI_FX990_setupFunc},

    {PCI_VENDOR_ID_AMD,         0x9601, CS_AMD_GX890,        "GX890"        ,AMD_FX890_setupFunc},
    {PCI_VENDOR_ID_AMD,         0x9600, CS_AMD_RS780,        "RS780"        ,AMD_RS780_setupFunc},
    {PCI_VENDOR_ID_AMD,         0x790e, CS_AMD_X370,         "X370/X399/X470/ TRX40/X570/WRX80", AMD_X370_setupFunc },

    {PCI_VENDOR_ID_VIA,         0x0308, CS_VIA_VT8369B,      "VT8369B",      NULL},
    {PCI_VENDOR_ID_VIA,         0x0410, CS_VIA_VX900,        "VX900",        VIA_VX900_setupFunc},
    {PCI_VENDOR_ID_APM,         0xe004, CS_APM_STORM,        "X-Gene Storm", APM_Storm_setupFunc},
    {PCI_VENDOR_ID_IBM,         0x03DC, CS_IBM_VENICE,       "Venice",       NULL},
    {PCI_VENDOR_ID_MARVELL,     0xAF00, CS_MARVELL_THUNDERX2, "Marvell ThunderX2", Marvell_ThunderX2_setupFunc},
    {PCI_VENDOR_ID_REDHAT,      0x0008, CS_REDHAT_QEMU,      "QEMU Redhat",  QEMU_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE005, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE006, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE007, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE008, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE009, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE00A, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE00B, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE00C, CS_AMPERE_EMAG,      "AMPERE eMag",  Ampere_eMag_setupFunc},
    {PCI_VENDOR_ID_HUAWEI,      0xA120, CS_HUAWEI_KUNPENG920, "Huawei Kunpeng920",  Huawei_Kunpeng920_setupFunc},
    {PCI_VENDOR_ID_MELLANOX,    0xA2D0, CS_MELLANOX_BLUEFIELD, "Mellanox BlueField",  Mellanox_BlueField_setupFunc},
    {PCI_VENDOR_ID_MELLANOX,    0xA2D4, CS_MELLANOX_BLUEFIELD2, "Mellanox BlueField 2",  NULL},
    {PCI_VENDOR_ID_MELLANOX,    0xA2D5, CS_MELLANOX_BLUEFIELD2, "Mellanox BlueField 2 Crypto disabled",  NULL},
    {PCI_VENDOR_ID_MELLANOX,    0xA2DA, CS_MELLANOX_BLUEFIELD3, "Mellanox BlueField 3 Crypto enabled",  Mellanox_BlueField3_setupFunc},
    {PCI_VENDOR_ID_MELLANOX,    0xA2DB, CS_MELLANOX_BLUEFIELD3, "Mellanox BlueField 3 Crypto disabled",  Mellanox_BlueField3_setupFunc},
    {PCI_VENDOR_ID_AMAZON,      0x0200, CS_AMAZON_GRAVITRON2, "Amazon Gravitron2",  Amazon_Gravitron2_setupFunc},
    {PCI_VENDOR_ID_FUJITSU,     0x1952, CS_FUJITSU_A64FX,     "Fujitsu A64FX", Fujitsu_A64FX_setupFunc},
    {PCI_VENDOR_ID_CADENCE,     0xDC01, CS_PHYTIUM_S2500,  "Phytium S2500",  NULL},
    {PCI_VENDOR_ID_CADENCE,     0xDC08, CS_PHYTIUM_S2500,  "Phytium S2500",  NULL},
    {PCI_VENDOR_ID_CADENCE,     0xDC16, CS_PHYTIUM_S2500,  "Phytium S2500",  NULL},
    {PCI_VENDOR_ID_CADENCE,     0xFC01, CS_PHYTIUM_S2500,  "Phytium S2500",  NULL},
    {PCI_VENDOR_ID_CADENCE,     0xFC08, CS_PHYTIUM_S2500,  "Phytium S2500",  NULL},
    {PCI_VENDOR_ID_CADENCE,     0xFC16, CS_PHYTIUM_S2500,  "Phytium S2500",  NULL},
    {PCI_VENDOR_ID_AMPERE,      0xE000, CS_AMPERE_ALTRA, "Ampere Altra", Ampere_Altra_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE00D, CS_AMPERE_ALTRA, "Ampere Altra", Ampere_Altra_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE00E, CS_AMPERE_ALTRA, "Ampere Altra", Ampere_Altra_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE010, CS_AMPERE_ALTRA, "Ampere Altra", Ampere_Altra_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE100, CS_AMPERE_ALTRA, "Ampere Altra", Ampere_Altra_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE110, CS_AMPERE_ALTRA, "Ampere Altra", Ampere_Altra_setupFunc},
    {PCI_VENDOR_ID_ARM,         0x0100, CS_ARM_NEOVERSEN1, "Arm Neoverse N1",  Arm_NeoverseN1_setupFunc},
    {PCI_VENDOR_ID_HYGON,       0x790E, CS_HYGON_C86,      "Hygon-C86-7151",   NULL},
    {PCI_VENDOR_ID_MARVELL,     0xA02D, CS_MARVELL_OCTEON_CN96XX, "Marvell Octeon CN96xx", ARMV8_generic_setupFunc},
    {PCI_VENDOR_ID_MARVELL,     0xA02D, CS_MARVELL_OCTEON_CN98XX, "Marvell Octeon CN98xx", ARMV8_generic_setupFunc},
    {PCI_VENDOR_ID_SIFIVE,      0x0000, CS_SIFIVE_FU740_C000, "SiFive FU740-000", Riscv_generic_setupFunc},
    {PCI_VENDOR_ID_PLDA,        0x1111, CS_PLDA_XPRESSRICH_AXI_REF, "XpressRich-AXI Ref Design", PLDA_XpressRichAXI_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE200, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE201, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE202, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE203, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE204, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE205, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE206, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE207, CS_AMPERE_AMPEREONE160, "Ampere AmpereOne-160", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_PHYTIUM,     0x5C16, CS_PHYTIUM_S5000,    "Phytium S5000", NULL},
    {PCI_VENDOR_ID_AMPERE,      0xE208, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE209, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE20A, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE20B, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE20C, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE20D, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE20E, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},
    {PCI_VENDOR_ID_AMPERE,      0xE20F, CS_AMPERE_AMPEREONE192, "Ampere AmpereOne-192", Ampere_AmpereOne_setupFunc},

///////////////////////////////////////////////////////////////////////////////////////////////////

    // last element must have chipset CS_UNKNOWN (zero)
    {0,                         0,      CS_UNKNOWN,         "Unknown",      NULL}
};


VENDORNAME vendorName[] =
{
    {PCI_VENDOR_ID_NVIDIA,      "NVIDIA"},
    {PCI_VENDOR_ID_INTEL,       "Intel"},
    {PCI_VENDOR_ID_VIA,         "VIA"},
    {PCI_VENDOR_ID_RCC,         "ServerWorks"},
    {PCI_VENDOR_ID_MICRON_1,    "Micron"},
    {PCI_VENDOR_ID_MICRON_2,    "Micron"},
    {PCI_VENDOR_ID_APPLE,       "Apple"},
    {PCI_VENDOR_ID_SIS,         "SiS"},
    {PCI_VENDOR_ID_ATI,         "ATI"},
    {PCI_VENDOR_ID_TRANSMETA,   "Transmeta"},
    {PCI_VENDOR_ID_HP,          "HP"},
    {PCI_VENDOR_ID_AMD,         "AMD"},
    {PCI_VENDOR_ID_ALI,         "ALi"},
    {PCI_VENDOR_ID_APM,         "AppliedMicro"},
    {PCI_VENDOR_ID_IBM,         "IBM"},
    {PCI_VENDOR_ID_MARVELL,     "MarvellThunderX2"},
    {PCI_VENDOR_ID_REDHAT,      "QemuRedhat"},
    {PCI_VENDOR_ID_AMPERE,      "AmpereComputing"},
    {PCI_VENDOR_ID_HUAWEI,      "Huawei"},
    {PCI_VENDOR_ID_MELLANOX,    "Mellanox"},
    {PCI_VENDOR_ID_AMAZON,      "Amazon"},
    {PCI_VENDOR_ID_FUJITSU,     "Fujitsu"},
    {PCI_VENDOR_ID_CADENCE,     "Cadence"},
    {PCI_VENDOR_ID_ARM,         "ARM"},
    {PCI_VENDOR_ID_ALIBABA,     "Alibaba"},
    {PCI_VENDOR_ID_SIFIVE,      "SiFive"},
    {PCI_VENDOR_ID_PLDA,        "PLDA"},
    {PCI_VENDOR_ID_PHYTIUM,     "Phytium"},
    {0,                         "Unknown"} // Indicates end of the table
};


//
// Allowlist all chipsets with which dGPU over PCIe is supported on ARM
// (both v7 and v8) platforms
//
ARMCSALLOWLISTINFO armChipsetAllowListInfo[] =
{
    {PCI_VENDOR_ID_NVIDIA,      0x0FAE, CS_NVIDIA_T210},        // NVIDIA Tegra X1 RP0
    {PCI_VENDOR_ID_NVIDIA,      0x0FAF, CS_NVIDIA_T210},        // NVIDIA Tegra X1 RP1
    {PCI_VENDOR_ID_NVIDIA,      0x10E5, CS_NVIDIA_T186},        // NVIDIA Tegra P1 RP0
    {PCI_VENDOR_ID_NVIDIA,      0x10E6, CS_NVIDIA_T186},        // NVIDIA Tegra P1 RP1
    {PCI_VENDOR_ID_NVIDIA,      0x1AD0, CS_NVIDIA_T194},        // NVIDIA Tegra V1 RP0
    {PCI_VENDOR_ID_NVIDIA,      0x1AD1, CS_NVIDIA_T194},        // NVIDIA Tegra V1 RP1
    {PCI_VENDOR_ID_NVIDIA,      0x1AD2, CS_NVIDIA_T194},        // NVIDIA Tegra V1 RP2
    {PCI_VENDOR_ID_NVIDIA,      0x229A, CS_NVIDIA_T234},        // NVIDIA Tegra Orin RP0
    {PCI_VENDOR_ID_NVIDIA,      0x229C, CS_NVIDIA_T234},        // NVIDIA Tegra Orin RP1
    {PCI_VENDOR_ID_NVIDIA,      0x229E, CS_NVIDIA_T234},        // NVIDIA Tegra Orin RP2
    {PCI_VENDOR_ID_NVIDIA,      0x22C2, CS_NVIDIA_T23x},        // NVIDIA Tegra RP0
    {PCI_VENDOR_ID_NVIDIA,      0x22C3, CS_NVIDIA_T23x},        // NVIDIA Tegra RP1
    {PCI_VENDOR_ID_NVIDIA,      0x22B1, CS_NVIDIA_TH500},       // NVIDIA TH500 RP for GH100 GPU in GH180.
    {PCI_VENDOR_ID_NVIDIA,      0x22B2, CS_NVIDIA_TH500},       // NVIDIA TH500 RP x16
    {PCI_VENDOR_ID_NVIDIA,      0x22B3, CS_NVIDIA_TH500},       // NVIDIA TH500 RP x4
    {PCI_VENDOR_ID_NVIDIA,      0x22B4, CS_NVIDIA_TH500},       // NVIDIA TH500 RP x1
    {PCI_VENDOR_ID_NVIDIA,      0x22B8, CS_NVIDIA_TH500},       // NVIDIA TH500 RP x8
    {PCI_VENDOR_ID_NVIDIA,      0x22B9, CS_NVIDIA_TH500},       // NVIDIA TH500 RP x2
    {PCI_VENDOR_ID_NVIDIA,      0x22D6, CS_NVIDIA_T264},        // NVIDIA Tegra RP x8
    {PCI_VENDOR_ID_NVIDIA,      0x22D8, CS_NVIDIA_T264},        // NVIDIA Tegra RP x4
    {PCI_VENDOR_ID_NVIDIA,      0x22DA, CS_NVIDIA_T264},        // NVIDIA Tegra RP x2
    {PCI_VENDOR_ID_NVIDIA,      0x2B00, CS_NVIDIA_T264},        // NVIDIA Tegra RP iGPU

    {PCI_VENDOR_ID_APM,         0xe004, CS_APM_STORM},          // Applied Micro X-Gene "Storm"
    {PCI_VENDOR_ID_MARVELL,     0xAF00, CS_MARVELL_THUNDERX2},  // Marvell ThunderX2
    {PCI_VENDOR_ID_REDHAT,      0x0008, CS_REDHAT_QEMU},        // Redhat QEMU
    {PCI_VENDOR_ID_AMPERE,      0xE005, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_AMPERE,      0xE006, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_AMPERE,      0xE007, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_AMPERE,      0xE008, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_AMPERE,      0xE009, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_AMPERE,      0xE00A, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_AMPERE,      0xE00B, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_AMPERE,      0xE00C, CS_AMPERE_EMAG},        // Ampere eMag
    {PCI_VENDOR_ID_HUAWEI,      0xA120, CS_HUAWEI_KUNPENG920},  // Huawei Kunpeng 920
    {PCI_VENDOR_ID_MELLANOX,    0xA2D0, CS_MELLANOX_BLUEFIELD}, // Mellanox BlueField
    {PCI_VENDOR_ID_MELLANOX,    0xA2D4, CS_MELLANOX_BLUEFIELD2},// Mellanox BlueField 2
    {PCI_VENDOR_ID_MELLANOX,    0xA2D5, CS_MELLANOX_BLUEFIELD2},// Mellanox BlueField 2 Crypto disabled
    {PCI_VENDOR_ID_MELLANOX,    0xA2DB, CS_MELLANOX_BLUEFIELD3},// Mellanox BlueField 3 Crypto disabled
    {PCI_VENDOR_ID_MELLANOX,    0xA2DA, CS_MELLANOX_BLUEFIELD3},// Mellanox BlueField 3 Crypto enabled
    {PCI_VENDOR_ID_AMAZON,      0x0200, CS_AMAZON_GRAVITRON2},  // Amazon Gravitron2
    {PCI_VENDOR_ID_FUJITSU,     0x1952, CS_FUJITSU_A64FX},      // Fujitsu A64FX
    {PCI_VENDOR_ID_CADENCE,     0xDC01, CS_PHYTIUM_S2500},      // Phytium S2500
    {PCI_VENDOR_ID_CADENCE,     0xDC08, CS_PHYTIUM_S2500},      // Phytium S2500
    {PCI_VENDOR_ID_CADENCE,     0xDC16, CS_PHYTIUM_S2500},      // Phytium S2500
    {PCI_VENDOR_ID_CADENCE,     0xFC01, CS_PHYTIUM_S2500},      // Phytium S2500
    {PCI_VENDOR_ID_CADENCE,     0xFC08, CS_PHYTIUM_S2500},      // Phytium S2500
    {PCI_VENDOR_ID_CADENCE,     0xFC16, CS_PHYTIUM_S2500},      // Phytium S2500
    {PCI_VENDOR_ID_AMPERE,      0xE000, CS_AMPERE_ALTRA},       // Ampere Altra
    {PCI_VENDOR_ID_AMPERE,      0xE00D, CS_AMPERE_ALTRA},       // Ampere Altra
    {PCI_VENDOR_ID_AMPERE,      0xE00E, CS_AMPERE_ALTRA},       // Ampere Altra
    {PCI_VENDOR_ID_AMPERE,      0xE010, CS_AMPERE_ALTRA},       // Ampere Altra
    {PCI_VENDOR_ID_AMPERE,      0xE100, CS_AMPERE_ALTRA},       // Ampere Altra
    {PCI_VENDOR_ID_AMPERE,      0xE110, CS_AMPERE_ALTRA},       // Ampere Altra
    {PCI_VENDOR_ID_ARM,         0x0100, CS_ARM_NEOVERSEN1},     // Arm Neoverse N1
    {PCI_VENDOR_ID_MARVELL,     0xA02D, CS_MARVELL_OCTEON_CN96XX}, // Marvell OCTEON CN96xx
    {PCI_VENDOR_ID_MARVELL,     0xA02D, CS_MARVELL_OCTEON_CN98XX}, // Marvell OCTEON CN98xx
    {PCI_VENDOR_ID_ALIBABA,     0x8000, CS_ALIBABA_YITIAN},      // Alibaba Yitian
    {PCI_VENDOR_ID_AMPERE,      0xE200, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_AMPERE,      0xE201, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_AMPERE,      0xE202, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_AMPERE,      0xE203, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_AMPERE,      0xE204, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_AMPERE,      0xE205, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_AMPERE,      0xE206, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_AMPERE,      0xE207, CS_AMPERE_AMPEREONE160},   // Ampere AmpereOne-160
    {PCI_VENDOR_ID_PHYTIUM,     0x5C16, CS_PHYTIUM_S5000},         // Phytium S5000
    {PCI_VENDOR_ID_AMPERE,      0xE208, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192
    {PCI_VENDOR_ID_AMPERE,      0xE209, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192
    {PCI_VENDOR_ID_AMPERE,      0xE20A, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192
    {PCI_VENDOR_ID_AMPERE,      0xE20B, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192
    {PCI_VENDOR_ID_AMPERE,      0xE20C, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192
    {PCI_VENDOR_ID_AMPERE,      0xE20D, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192
    {PCI_VENDOR_ID_AMPERE,      0xE20E, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192
    {PCI_VENDOR_ID_AMPERE,      0xE20F, CS_AMPERE_AMPEREONE192},   // Ampere AmpereOne-192

    // last element must have chipset CS_UNKNOWN (zero)
    {0,                         0,      CS_UNKNOWN}
};

#endif /* NVCST_H */
