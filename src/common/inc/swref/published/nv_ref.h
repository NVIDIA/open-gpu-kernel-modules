/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES
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

//
/***************************************************************************\
*                                                                           *
*    Hardware Reference Manual extracted defines.                           *
*    - Defines in this file are approved by the HW team for publishing.     *
*                                                                           *
\***************************************************************************/
#ifndef NV_REF_PUBLISHED_H
#define NV_REF_PUBLISHED_H



#include "nvmisc.h"

//
// These registers can be accessed by chip-independent code as
// well as chip-dependent code.
//
// NOTE: DO NOT ADD TO THIS FILE.  CREATE CHIP SPECIFIC HAL ROUTINES INSTEAD.
//

/*
 * Standard PCI config space header defines.
 * The defines here cannot change across generations.
 */

/* dev_nv_xve.ref */
/* PBUS field defines converted to NV_CONFIG field defines */
#define NV_CONFIG_PCI_NV_0                               0x00000000 /* R--4R */
#define NV_CONFIG_PCI_NV_0_VENDOR_ID                           15:0 /* C--UF */
#define NV_CONFIG_PCI_NV_0_VENDOR_ID_NVIDIA              0x000010DE /* C---V */
#define NV_CONFIG_PCI_NV_0_DEVICE_ID                          31:16 /* R--UF */
#define NV_CONFIG_PCI_NV_1                               0x00000004 /* RW-4R */
#define NV_CONFIG_PCI_NV_1_IO_SPACE                             0:0 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_IO_SPACE_DISABLED             0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_IO_SPACE_ENABLED              0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_MEMORY_SPACE                         1:1 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_MEMORY_SPACE_DISABLED         0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_MEMORY_SPACE_ENABLED          0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_1_BUS_MASTER                           2:2 /* RWIVF */
#define NV_CONFIG_PCI_NV_1_BUS_MASTER_DISABLED           0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_1_BUS_MASTER_ENABLED            0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_2                               0x00000008 /* R--4R */
#define NV_CONFIG_PCI_NV_2_REVISION_ID                          7:0 /* C--UF */
#define NV_CONFIG_PCI_NV_2_CLASS_CODE                          31:8 /* C--VF */
#define NV_CONFIG_PCI_NV_3                               0x0000000C /* RW-4R */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER                      15:11 /* RWIUF */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_0_CLOCKS        0x00000000 /* RWI-V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_8_CLOCKS        0x00000001 /* RW--V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_240_CLOCKS      0x0000001E /* RW--V */
#define NV_CONFIG_PCI_NV_3_LATENCY_TIMER_248_CLOCKS      0x0000001F /* RW--V */
#define NV_CONFIG_PCI_NV_4                               0x00000010 /* RW-4R */
#define NV_CONFIG_PCI_NV_5                               0x00000014 /* RW-4R */
#define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE                         2:1 /* C--VF */
#define NV_CONFIG_PCI_NV_5_ADDRESS_TYPE_64_BIT           0x00000002 /* ----V */
#define NV_CONFIG_PCI_NV_6                               0x00000018 /* RW-4R */
#define NV_CONFIG_PCI_NV_7(i)                    (0x0000001C+(i)*4) /* R--4A */
#define NV_CONFIG_PCI_NV_11                              0x0000002C /* R--4R */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID                15:0 /* R--UF */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_VENDOR_ID_NONE     0x00000000 /* R---V */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID                      31:16 /* R--UF */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID_NONE            0x00000000 /* R---V */
#define NV_CONFIG_PCI_NV_11_SUBSYSTEM_ID_TNT2PRO         0x0000001f
#define NV_CONFIG_PCI_NV_12                              0x00000030 /* RW-4R */
#define NV_CONFIG_PCI_NV_13                              0x00000034 /* RW-4R */
#define NV_CONFIG_PCI_NV_13_CAP_PTR                             7:0 /* C--VF */
#define NV_CONFIG_PCI_NV_14                              0x00000038 /* R--4R */
#define NV_CONFIG_PCI_NV_15                              0x0000003C /* RW-4R */
#define NV_CONFIG_PCI_NV_15_INTR_LINE                           7:0 /* RWIVF */
/*
 * These defines are the correct fields to be used to extract the
 * NEXT_PTR and CAP_ID from any PCI capability structure,
 * but they still have NV_24 in the name because they were from the
 * first PCI capability structure in the capability list in older GPUs.
 */
#define NV_CONFIG_PCI_NV_24_NEXT_PTR                             15:8 /* R--VF */
#define NV_CONFIG_PCI_NV_24_CAP_ID                                7:0 /* C--VF */

/*
 * Standard registers present on NVIDIA chips used to ID the chip.
 * Very stable across generations.
 */

/* dev_boot */
#define NV_PMC_BOOT_0                                    0x00000000 /* R--4R */
#define NV_PMC_BOOT_0_MINOR_REVISION                            3:0 /* R--VF */
#define NV_PMC_BOOT_0_MAJOR_REVISION                            7:4 /* R--VF */
#define NV_PMC_BOOT_0_ARCHITECTURE_1                            8:8 /* R--VF */
#define NV_PMC_BOOT_0_IMPLEMENTATION                          23:20 /* R--VF */
#define NV_PMC_BOOT_0_IMPLEMENTATION_0                   0x00000000 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_1                   0x00000001 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_2                   0x00000002 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_3                   0x00000003 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_4                   0x00000004 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_5                   0x00000005 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_6                   0x00000006 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_7                   0x00000007 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_8                   0x00000008 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_9                   0x00000009 /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_A                   0x0000000A /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_B                   0x0000000B /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_C                   0x0000000C /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_D                   0x0000000D /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_E                   0x0000000E /* R---V */
#define NV_PMC_BOOT_0_IMPLEMENTATION_F                   0x0000000F /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_0                          28:24 /* R--VF */
#define NV_PMC_BOOT_0_ARCHITECTURE_TU100                 0x00000016 /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_TU110                 0x00000016 /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_GA100                 0x00000017 /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_GH100                 0x00000018 /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_AD100                 0x00000019 /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_GB100                 0x0000001A /* R---V */
#define NV_PMC_BOOT_0_ARCHITECTURE_GB200                 0x0000001B /* R---V */

#define NV_PMC_BOOT_1                                    0x00000004 /* R--4R */
#define NV_PMC_BOOT_1_VGPU8                                     8:8 /* R--VF */
#define NV_PMC_BOOT_1_VGPU8_REAL                         0x00000000 /* R-I-V */
#define NV_PMC_BOOT_1_VGPU8_VIRTUAL                      0x00000001 /* R---V */
#define NV_PMC_BOOT_1_VGPU16                                  16:16 /* R--VF */
#define NV_PMC_BOOT_1_VGPU16_REAL                        0x00000000 /* R-I-V */
#define NV_PMC_BOOT_1_VGPU16_VIRTUAL                     0x00000001 /* R---V */
#define NV_PMC_BOOT_1_VGPU                                    17:16 /* C--VF */
#define NV_PMC_BOOT_1_VGPU_REAL                          0x00000000 /* C---V */
#define NV_PMC_BOOT_1_VGPU_PV                            0x00000001 /* ----V */
#define NV_PMC_BOOT_1_VGPU_VF                            0x00000002 /* ----V */
#define NV_PMC_BOOT_42                                   0x00000A00 /* R--4R */
#define NV_PMC_BOOT_42_MINOR_EXTENDED_REVISION                 11:8 /* R-XVF */
#define NV_PMC_BOOT_42_MINOR_REVISION                         15:12 /* R-XVF */
#define NV_PMC_BOOT_42_MAJOR_REVISION                         19:16 /* R-XVF */
#define NV_PMC_BOOT_42_IMPLEMENTATION                         23:20 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE                           29:24 /*       */
#define NV_PMC_BOOT_42_CHIP_ID                                29:20 /* R-XVF */

#define NV_PMC_BOOT_42_ARCHITECTURE_GM100                0x00000011 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GM200                0x00000012 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GP100                0x00000013 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GV100                0x00000014 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GV110                0x00000015 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_TU100                0x00000016 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GA100                0x00000017 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GH100                0x00000018 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_AD100                0x00000019 /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GB100                0x0000001A /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_GB200                0x0000001B /*       */
#define NV_PMC_BOOT_42_ARCHITECTURE_AMODEL               0x0000001F /*       */

#define NV_PMC_BOOT_42_CHIP_ID_GA100                     0x00000170 /*       */

/* dev_arapb_misc.h */
#define NV_PAPB_MISC_GP_HIDREV_CHIPID                    15:8 /* ----F */
#define NV_PAPB_MISC_GP_HIDREV_MAJORREV                   7:4 /* ----F */


//
// Helper to return NV_PMC_BOOT_0 architecture, which is split across fields:
// ARCHITECTURE_1 (msb) and ARCHITECTURE_0 (lsb)
//
static inline NvU32
decodePmcBoot0Architecture(NvU32 pmcBoot0RegVal)
{
    return (DRF_VAL(_PMC, _BOOT_0, _ARCHITECTURE_1, pmcBoot0RegVal) << DRF_SIZE(NV_PMC_BOOT_0_ARCHITECTURE_0)) |
            DRF_VAL(_PMC, _BOOT_0, _ARCHITECTURE_0, pmcBoot0RegVal);
}

// Helpers to return NV_PMC_BOOT_42 architecture and chip ID
static inline NvU32
decodePmcBoot42Architecture(NvU32 pmcBoot42RegVal)
{
    return DRF_VAL(_PMC, _BOOT_42, _ARCHITECTURE, pmcBoot42RegVal);
}

static inline NvU32
decodePmcBoot42ChipId(NvU32 pmcBoot42RegVal)
{
    return DRF_VAL(_PMC, _BOOT_42, _CHIP_ID, pmcBoot42RegVal);
}

#endif // NV_REF_PUBLISHED_H
