
#ifndef _G_MEM_DESC_NVOC_H_
#define _G_MEM_DESC_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_mem_desc_nvoc.h"

#ifndef _MEMDESC_H_
#define _MEMDESC_H_

#include "core/prelude.h"
#include "poolalloc.h"



struct OBJVASPACE;

#ifndef __NVOC_CLASS_OBJVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJVASPACE_TYPEDEF__
typedef struct OBJVASPACE OBJVASPACE;
#endif /* __NVOC_CLASS_OBJVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVASPACE
#define __nvoc_class_id_OBJVASPACE 0x6c347f
#endif /* __nvoc_class_id_OBJVASPACE */



struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct Heap;

#ifndef __NVOC_CLASS_Heap_TYPEDEF__
#define __NVOC_CLASS_Heap_TYPEDEF__
typedef struct Heap Heap;
#endif /* __NVOC_CLASS_Heap_TYPEDEF__ */

#ifndef __nvoc_class_id_Heap
#define __nvoc_class_id_Heap 0x556e9a
#endif /* __nvoc_class_id_Heap */



struct RsClient;

#ifndef __NVOC_CLASS_RsClient_TYPEDEF__
#define __NVOC_CLASS_RsClient_TYPEDEF__
typedef struct RsClient RsClient;
#endif /* __NVOC_CLASS_RsClient_TYPEDEF__ */

#ifndef __nvoc_class_id_RsClient
#define __nvoc_class_id_RsClient 0x8f87e5
#endif /* __nvoc_class_id_RsClient */


struct MEMORY_DESCRIPTOR;

typedef struct CTX_BUF_POOL_INFO CTX_BUF_POOL_INFO;
typedef struct COMPR_INFO COMPR_INFO;

//
// Address space identifiers.
// Note: This should match the NV2080_CTRL_GR_CTX_BUFFER_INFO_APERTURE_* defines
//       in ctrl2080gr.h
//
typedef NvU32      NV_ADDRESS_SPACE;
#define ADDR_UNKNOWN    0         // Address space is unknown
#define ADDR_SYSMEM     1         // System memory (PCI)
#define ADDR_FBMEM      2         // Frame buffer memory space
#define ADDR_REGMEM     3         // NV register memory space
#define ADDR_VIRTUAL    4         // Virtual address space only
#define ADDR_FABRIC_V2  6         // Fabric address space for the FLA based addressing. Will replace ADDR_FABRIC.
#define ADDR_EGM        7         // Extended GPU Memory (EGM)
#define ADDR_FABRIC_MC  8         // Multicast fabric address space (MCFLA)

//
// Address translation identifiers:
//
// Memory descriptors are used to describe physical block(s) of memory.
// That memory can be described at various levels of address translation
// using the address translation (AT) enumerates. The levels of translation
// supported is illustrated below.
//
// The diagram is drawn for system memory with SR-IOV but the translations
// are similar for video memory (replace IOMMU with VMMU). VGPU pre-SR-IOV
// is also different.
//
// +-------------------+           +-------------------+
// |       CPU         |           |     GPU Engine    |
// +-------------------+           +-------------------+
//          |                               |
//          |                               | GPU VA
//          |                               V
//          |                      +-------------------+
//          | CPU VA               |       GMMU        |
//          |                      +-------------------+
//          |                               |
//          |                               | GPU GPA (AT_GPU)
//          v                               v
// +-------------------+           +-------------------+
// |  MMU (1st level)| |           | IOMMU (1st level) |
// +-------------------+           +-------------------+
//          |                               |
//          | CPU GPA (AT_CPU)              |                   <---- AT_PA for VGPU guest
//          v                               v
// +-------------------+           +-------------------+
// |  MMU (2nd level)  |           | IOMMU (2nd level) |
// +-------------------+           +-------------------+
//          |                               |
//          | SPA                           | SPA               <---- AT_PA for bare metal
//          v                               v                         or VGPU host
// +---------------------------------------------------+
// |                System Memory                      |
// +---------------------------------------------------+
//
//
// Descriptions for *physical* address translation levels:
//
// AT_CPU - CPU physical address or guest physical address (GPA)
// AT_GPU - GPU physical address or guest physical address (GPA)
// AT_PA  - When running in host RM or bare metal this is the system physical address. When
//          running inside a VGPU guest environment, this is the last level of translation
//          visible to the OS context that RM is running in.
//
// AT_CPU should typically == AT_PA, but there might be cases such as IBM P9 where vidmem
// might be 0-based on GPU but exposed elsewhere in the CPU address space.
//
// Descriptions for *virtual* address translation levels:
//
// AT_GPU_VA - Memory descriptors can also describe virtual memory allocations. AT_GPU_VA
//             represents a GMMU virtual address.
//
#define AT_CPU      AT_VARIANT(0)
#define AT_GPU      AT_VARIANT(1)
#define AT_PA       AT_VARIANT(2)

#define AT_GPU_VA   AT_VARIANT(3)

//
// TODO - switch to using numeric values for AT_XYZ. Using pointers for
// typesafety after initial split from using class IDs/mmuContext
//
typedef struct ADDRESS_TRANSLATION_ *ADDRESS_TRANSLATION;
#define AT_VARIANT(x)  ((struct ADDRESS_TRANSLATION_ *)x)
#define AT_VALUE(x)    ((NvU64)(NvUPtr)(x))

//
// RM defined Memdesc surface names. The names are sent to Mods to enable feature verification.
//
#define NV_RM_SURF_NAME_INSTANCE_BLOCK                      "rm_instance_block_surface"
#define NV_RM_SURF_NAME_PAGE_TABLE                          "rm_page_table_surface"
#define NV_RM_SURF_NAME_NONREPLAYABLE_FAULT_BUFFER          "rm_non_replayable_fault_buffer_surface"
#define NV_RM_SURF_NAME_REPLAYABLE_FAULT_BUFFER             "rm_replayable_fault_buffer_surface"
#define NV_RM_SURF_NAME_CE_FAULT_METHOD_BUFFER              "rm_ce_fault_method_buffer_surface"
#define NV_RM_SURF_NAME_ACCESS_COUNTER_BUFFER               "rm_access_counter_buffer_surface"
#define NV_RM_SURF_NAME_VAB                                 "rm_vab_surface"
#define NV_RM_SURF_NAME_GR_CIRCULAR_BUFFER                  "rm_gr_ctx_circular_buffer_surface"

//
// Tagging wrapper macro for memdescAlloc
//
#define memdescTagAlloc(stat, tag, pMemdesc)                        {(pMemdesc)->allocTag = tag; stat = memdescAlloc(pMemdesc);}
#define memdescTagAllocList(stat, tag, pMemdesc, pList)             {(pMemdesc)->allocTag = tag; stat = memdescAllocList(pMemdesc, pList);}

//
// RM internal allocations owner tags
// Total 200 tags are introduced, out of which some are already
// replaced with known verbose strings
//
typedef enum
{
    NV_FB_ALLOC_RM_INTERNAL_OWNER__MIN                  = 10U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_COMPBIT_STORE         = 11U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_CONTEXT_BUFFER        = 12U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_ATTR_BUFFER           = 13U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_PMU_SURFACE           = 14U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_CIRCULAR_BUFFER       = 15U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_PAGE_POOL             = 16U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_ACCESS_MAP            = 17U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_WPR_METADATA          = 18U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_LIBOS_ARGS            = 19U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_BOOTLOADER_ARGS       = 20U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_SR_METADATA           = 21U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_ACR_SETUP             = 22U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_ACR_SHADOW            = 23U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_ACR_BACKUP            = 24U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_ACR_BINARY            = 25U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_VBIOS_FRTS            = 26U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_USERD_BUFFER          = 27U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_RUNLIST_ENTRIES       = 28U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_PAGE_PTE              = 29U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_MMU_FAULT_BUFFER      = 30U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_FAULT_METHOD          = 31U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_WAR_PT                = 32U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_WAR_PD                = 33U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_1         = 34U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_2         = 35U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_3         = 36U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_4         = 37U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_5         = 38U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_6         = 39U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_7         = 40U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_8         = 41U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_9         = 42U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_10        = 43U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_11        = 44U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_12        = 45U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_13        = 46U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_14        = 47U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_15        = 48U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_16        = 49U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_17        = 50U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_18        = 51U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_19        = 52U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_20        = 53U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_21        = 54U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_22        = 55U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_23        = 56U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_24        = 57U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_25        = 58U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_26        = 59U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_27        = 60U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_28        = 61U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_29        = 62U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_30        = 63U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_31        = 64U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_32        = 65U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_33        = 66U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_34        = 67U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_RUSD_BUFFER           = 68U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_36        = 69U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_37        = 70U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_38        = 71U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_39        = 72U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_40        = 73U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_41        = 74U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_42        = 75U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_43        = 76U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_44        = 77U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_45        = 78U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_46        = 79U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_47        = 80U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_48        = 81U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_49        = 82U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_50        = 83U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_51        = 84U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_52        = 85U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_53        = 86U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_54        = 87U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_55        = 88U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_56        = 89U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_57        = 90U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_58        = 91U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_59        = 92U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_60        = 93U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_61        = 94U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_62        = 95U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_63        = 96U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_64        = 97U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_65        = 98U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_66        = 99U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_67        = 100U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_68        = 101U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_69        = 102U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_70        = 103U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_71        = 104U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_72        = 105U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_73        = 106U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_74        = 107U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_75        = 108U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_76        = 109U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_77        = 110U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_78        = 111U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_79        = 112U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_80        = 113U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_81        = 114U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_82        = 115U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_83        = 116U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_84        = 117U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_85        = 118U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_86        = 119U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_87        = 120U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_88        = 121U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_89        = 122U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_90        = 123U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_91        = 124U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_92        = 125U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_93        = 126U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_94        = 127U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_95        = 128U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_96        = 129U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_97        = 130U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_98        = 131U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_99        = 132U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_100       = 133U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_101       = 134U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_102       = 135U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_103       = 136U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_104       = 137U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_105       = 138U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_106       = 139U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_107       = 140U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_108       = 141U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_109       = 142U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_110       = 143U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_111       = 144U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_112       = 145U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_113       = 146U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_114       = 147U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_115       = 148U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_116       = 149U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_117       = 150U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_118       = 151U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_119       = 152U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_120       = 153U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_121       = 154U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_122       = 155U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_123       = 156U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_124       = 157U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_125       = 158U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_126       = 159U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_127       = 160U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_128       = 161U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_FBSR_CE_TEST_BUFFER   = 162U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_130       = 163U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_131       = 164U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_132       = 165U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_133       = 166U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_134       = 167U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_135       = 168U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_136       = 169U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_137       = 170U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_138       = 171U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_139       = 172U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_140       = 173U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_141       = 174U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_142       = 175U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_143       = 176U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_GSP_NOTIFY_OP_SURFACE = 177U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_FAKE_WPR_RSVD         = 178U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_GR_SCRUB_CHANNEL      = 179U,

    //
    // Unused tags from here, for any new use-case it's required 
    // to replace the below tags with known verbose strings
    //
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_147       = 180U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_148       = 181U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_149       = 182U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_150       = 183U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_151       = 184U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_152       = 185U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_153       = 186U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_154       = 187U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_155       = 188U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_156       = 189U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_157       = 190U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_158       = 191U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_159       = 192U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_160       = 193U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_161       = 194U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_162       = 195U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_163       = 196U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_164       = 197U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_165       = 198U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_166       = 199U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_167       = 200U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_168       = 201U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_169       = 202U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_170       = 203U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_171       = 204U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_172       = 205U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_173       = 206U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_174       = 207U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_175       = 208U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_176       = 209U,
    NV_FB_ALLOC_RM_INTERNAL_OWNER__MAX                  = 210U,
} NV_FB_ALLOC_RM_INTERNAL_OWNER;

//
// Overrides address translation in SR-IOV enabled usecases
//
// In SRIOV systems, an access from guest has to go through the following
// translations:
//
// GVA -> GPA -> SPA
//
// Given HOST manages channel/memory management for guest, there are certain
// code paths that expects VA -> GPA translations and some may need GPA -> SPA
// translations. We use address translation to differentiate between these
// cases.
//
// We use AT_PA to force GPA -> SPA translation for vidmem. In case of non-SRIOV systems,
// using IO_VASPACE_A will fall back to FERMI_VASPACE_A or default context.
//
#define FORCE_VMMU_TRANSLATION(pMemDesc, curAddressTranslation) \
    ((memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM) ? AT_PA : curAddressTranslation)

typedef struct _memdescDestroyCallback MEM_DESC_DESTROY_CALLBACK;

typedef void (MEM_DATA_RELEASE_CALL_BACK)(struct MEMORY_DESCRIPTOR *);

//
// A memory descriptor is an object that describes and can be used to manipulate
// a block of memory.  The memory can be video or system memory; it can be
// contiguous or noncontiguous; it can be tiled, block linear, etc.  However,
// regardless of what type of memory it is, clients can use a standard set of
// APIs to manipulate it.
//
DECLARE_INTRUSIVE_LIST(MEMORY_DESCRIPTOR_LIST);

typedef struct MEMORY_DESCRIPTOR
{
    // The GPU that this memory belongs to
    OBJGPU *pGpu;

    // Flags field for optional behavior
    NvU64 _flags;

    // Size of mapping used for this allocation.  Multiple mappings on Fermi must always use the same page size.
    NvU64 _pageSize;

    // Size of the memory allocation in pages
    NvU64 PageCount;

    // Alignment of the memory allocation as size in bytes
    // XXX: would 32b work here?
    NvU64 Alignment;

    // Size of the memory allocation requested in bytes
    NvU64 Size;

    // Actual size of memory allocated to satisfy alignment.
    // We report the requested size, not the actual size. A number of callers
    // depend on this.
    NvU64 ActualSize;

    // The information returned from osAllocPages
    NvP64 _address;
    void *_pMemData;
    MEM_DATA_RELEASE_CALL_BACK *_pMemDataReleaseCallback;

    // When memory is allocated by a guest Virtual Machine (VM)
    // it is aliased by the host RM. We store a unique guest ID
    // for each piece of aliased memory to facilitate host RM mappings
    // to these pages (only in case of system memory).
    // XXX: would 32b work here?
    NvU64 _guestId;

    // To keep track of the offset from parent memdesc
    NvU64 subMemOffset;

    //
    // The byte offset at which the memory allocation begins within the first
    // PTE.  To locate the physical address of the byte at offset i in the memory
    // allocation, use the following logic:
    //   i += PteAdjust;
    //   if (PhysicallyContiguous)
    //       PhysAddr = PteArray[0] + i;
    //   else
    //       PhysAddr = PteArray[i >> RM_PAGE_SHIFT] + (i & RM_PAGE_MASK);
    //
    NvU32 PteAdjust;

    // Has the memory been allocated yet?
    NvBool Allocated;

    //
    // Marks that a request to deallocate memory has been called on this memdesc while it had multiple references
    // NV_TRUE denotes that memFree will be called when refcount reaches 0.
    //
    NvBool bDeferredFree;

    // Does this use SUBALLOCATOR?
    NvBool bUsingSuballocator;

    // Where does the memory live?  Video, system, other
    NV_ADDRESS_SPACE _addressSpace;

    // Attributes reflecting GPU caching of this memory.
    NvU32 _gpuCacheAttrib;

    // Peer vid mem cacheability
    NvU32 _gpuP2PCacheAttrib;

    // One of NV_MEMORY_CACHED, NV_MEMORY_UNCACHED, NV_MEMORY_WRITECOMBINED
    NvU32 _cpuCacheAttrib;

    // The page kind of this memory
    NvU32 _pteKind;
    NvU32 _pteKindCompressed;

    //
    // Scale memory allocation by this value
    //
    NvU32 _subDeviceAllocCount;

    //
    // Reference count for the object.
    //
    NvU32 RefCount;

    // Reference count for duplication of memory object via RmDupObject.
    NvU32 DupCount;

    //
    // The HwResId is used by the device dependent HAL to keep track of
    // resources attached to the memory (e.g.: compression tags, zcull).
    //
    NvU32 _hwResId;

    //
    // alloc tag for tracking internal allocations @ref NV_FB_ALLOC_RM_INTERNAL_OWNER
    //
    NV_FB_ALLOC_RM_INTERNAL_OWNER allocTag;

    //
    // Keep track which heap is actually used for this allocation
    //
    struct Heap *pHeap;

    //
    // GFID that this memory allocation belongs to
    //
    NvU32    gfid;

    //
    // Keep track of the PMA_ALLOC_INFO data.
    //
    struct PMA_ALLOC_INFO *pPmaAllocInfo;

    // Serve as head node in a list of page handles
    PoolPageHandleList *pPageHandleList;

    //
    // List of callbacks to call when destroying memory descriptor
    //
    MEM_DESC_DESTROY_CALLBACK *_pMemDestroyCallbackList;

    // pointer to descriptor which was used to subset current descriptor
    struct MEMORY_DESCRIPTOR *_pParentDescriptor;

    // Count used for sanity check
    NvU32 childDescriptorCnt;

    // Next memory descriptor in subdevice list
    struct MEMORY_DESCRIPTOR *_pNext;

    // Pointer to system Memory descriptor which used to back some FB content across S3/S4.
    struct MEMORY_DESCRIPTOR *_pStandbyBuffer;

    // Serve as a head node in a list of submemdescs
    MEMORY_DESCRIPTOR_LIST *pSubMemDescList;

    // Reserved for RM exclusive use
    NvBool bRmExclusiveUse;

    // If strung in a intrusive linked list
    ListNode   node;

    //
    // Pointer to IOVA mappings used to back the IOMMU VAs for different IOVA spaces
    // Submemory descriptors only have on mapping, but the root descriptor will have
    // one per IOVA space that the memory is mapped into.
    //
    struct IOVAMAPPING *_pIommuMappings;

    // Kernel mapping of the memory
    NvP64 _kernelMapping;
    NvP64 _kernelMappingPriv;

    // Internal mapping
    void *_pInternalMapping;
    void *_pInternalMappingPriv;
    NvU32 _internalMappingRefCount;

    // Array to hold SPA addresses when memdesc is allocated from GPA. Valid only for SRIOV cases
    RmPhysAddr *pPteSpaMappings;

    //
    // context buffer pool from which this memdesc is to be allocated.
    // This is controlled by PDB_PROP_GPU_MOVE_RM_BUFFERS_TO_PMA which is
    // enabled only for SMC today
    //
    CTX_BUF_POOL_INFO *pCtxBufPool;

    // Max physical address width to be override
    NvU32 _overridenAddressWidth;

    // We verified that memdesc is safe to be mapped as large pages
    NvBool bForceHugePages;

    // Memory handle that libos 3+ returns for dynamically mapped sysmem
    NvU32 libosRegionHandle;
    NvU64 baseVirtualAddress;

    // Indicates granularity of mapping. Will be used to implement dynamic page sizes.
    NvU32 pageArrayGranularity;

    // NUMA node ID from which memory should be allocated
    NvS32 numaNode;

    // Array to hold EGM addresses when EGM is enabled
    RmPhysAddr *pPteEgmMappings;

    //
    // If PhysicallyContiguous is NV_TRUE, this array consists of one element.
    // If PhysicallyContiguous is NV_FALSE, this array is actually larger and has
    // one entry for each physical page in the memory allocation.  As a result,
    // this structure must be allocated from the heap.
    // If the AddressSpace is ADDR_FBMEM, each entry is an FB offset.
    // Otherwise, each entry is a physical address on the system bus.
    // TBD: for now, the array will be sized at one entry for every 4KB, but
    // we probably want to optimize this later to support 64KB pages.
    //
    RmPhysAddr _pteArray[1];
    //!!! Place nothing behind PteArray!!!
} MEMORY_DESCRIPTOR, *PMEMORY_DESCRIPTOR;

MAKE_INTRUSIVE_LIST(MEMORY_DESCRIPTOR_LIST, MEMORY_DESCRIPTOR, node);

//
// Common address space lists
//
extern const NV_ADDRESS_SPACE ADDRLIST_FBMEM_PREFERRED[];
extern const NV_ADDRESS_SPACE ADDRLIST_SYSMEM_PREFERRED[];
extern const NV_ADDRESS_SPACE ADDRLIST_FBMEM_ONLY[];
extern const NV_ADDRESS_SPACE ADDRLIST_SYSMEM_ONLY[];

NvU32 memdescAddrSpaceListToU32(const NV_ADDRESS_SPACE *addrlist);
const NV_ADDRESS_SPACE *memdescU32ToAddrSpaceList(NvU32 index);

NV_STATUS _memdescUpdateSpaArray(PMEMORY_DESCRIPTOR   pMemDesc);
// Create a memory descriptor data structure (without allocating any physical
// storage).
NV_STATUS memdescCreate(MEMORY_DESCRIPTOR **ppMemDesc, OBJGPU *pGpu, NvU64 Size,
                        NvU64 alignment, NvBool PhysicallyContiguous,
                        NV_ADDRESS_SPACE AddressSpace, NvU32 CpuCacheAttrib, NvU64 Flags);

#define MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE_FB_BC_ONLY(pGpu, addressSpace) \
    ((gpumgrGetBcEnabledStatus(pGpu) && (pGpu != NULL) && (addressSpace == ADDR_FBMEM)) ?  MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE : MEMDESC_FLAGS_NONE)

// Initialize a caller supplied memory descriptor for use with memdescDescribe()
void memdescCreateExisting(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu, NvU64 Size,
                           NV_ADDRESS_SPACE AddressSpace,
                           NvU32 CpuCacheAttrib, NvU64 Flags);

// Increment reference count
void memdescAddRef(MEMORY_DESCRIPTOR *pMemDesc);

// Decrement reference count
void memdescRemoveRef(MEMORY_DESCRIPTOR *pMemDesc);

// Decrement reference count and reclaim any resources when possible
void memdescDestroy(MEMORY_DESCRIPTOR *pMemDesc);

//
// The destroy callback is called when the memory descriptor is
// destroyed with memdescDestroy().
//
// The caller is responsible for managing the memory used
// containing the callback.
//
typedef void (MemDescDestroyCallBack)(OBJGPU *, void *pObject, MEMORY_DESCRIPTOR *);
struct _memdescDestroyCallback
{
    MemDescDestroyCallBack *destroyCallback;
    void *pObject;
    MEM_DESC_DESTROY_CALLBACK *pNext;
};
void memdescAddDestroyCallback(MEMORY_DESCRIPTOR *pMemDesc, MEM_DESC_DESTROY_CALLBACK *);
void memdescRemoveDestroyCallback(MEMORY_DESCRIPTOR *pMemDesc, MEM_DESC_DESTROY_CALLBACK *);

// Allocate physical storage for a memory descriptor and fill in its PteArray
NV_STATUS memdescAlloc(MEMORY_DESCRIPTOR *pMemDesc);

// Allocate memory from one of the possible locations specified in pList.
NV_STATUS memdescAllocList(MEMORY_DESCRIPTOR *pMemDesc, const NV_ADDRESS_SPACE *pList);

// Free physical storage for a memory descriptor
void memdescFree(MEMORY_DESCRIPTOR *pMemDesc);

// Lock the paged virtual memory
NV_STATUS memdescLock(MEMORY_DESCRIPTOR *pMemDesc);

// Unlock the paged virtual memory
NV_STATUS memdescUnlock(MEMORY_DESCRIPTOR *pMemDesc);

// Allocate a CPU mapping of an arbitrary subrange of the memory.
// 64-bit clean (mac can have a 32-bit kernel pointer and 64-bit client pointers)
NV_STATUS memdescMap(MEMORY_DESCRIPTOR *pMemDesc, NvU64 Offset, NvU64 Size,
                     NvBool Kernel, NvU32 Protect, NvP64 *pAddress, NvP64 *pPriv);

// Free a CPU mapping of an arbitrary subrange of the memory.
void memdescUnmap(MEMORY_DESCRIPTOR *pMemDesc, NvBool Kernel, NvU32 ProcessId,
                  NvP64 Address, NvP64 Priv);

// Allocate a CPU mapping of an arbitrary subrange of the memory.
// fails unless Kernel == NV_TRUE
NV_STATUS memdescMapOld(MEMORY_DESCRIPTOR *pMemDesc, NvU64 Offset, NvU64 Size,
                        NvBool Kernel, NvU32 Protect, void **pAddress, void **pPriv);

// Free a CPU mapping of an arbitrary subrange of the memory.
void memdescUnmapOld(MEMORY_DESCRIPTOR *pMemDesc, NvBool Kernel, NvU32 ProcessId,
                     void *Address, void *Priv);

// Fill in a MEMORY_DESCRIPTOR with a description of a preexisting contiguous
// memory allocation.  It should already be initialized with
// memdescCreate*().
void memdescDescribe(MEMORY_DESCRIPTOR *pMemDesc,
                     NV_ADDRESS_SPACE AddressSpace,
                     RmPhysAddr Base, NvU64 Size);

// Fill in a MEMORY_DESCRIPTOR with the physical page addresses returned by PMA.
// It should already be initialized with memdescCreate*().
void memdescFillPages(MEMORY_DESCRIPTOR *pMemDesc, NvU32 offset,
                      NvU64 *pPages, NvU32 pageCount, NvU64 pageSize);

// Create a MEMORY_DESCRIPTOR for a subset of an existing memory allocation.
// The new MEMORY_DESCRIPTOR must be freed with memdescDestroy.
NV_STATUS memdescCreateSubMem(MEMORY_DESCRIPTOR **ppMemDescNew,
                              MEMORY_DESCRIPTOR *pMemDesc,
                              OBJGPU *pGpu, NvU64 Offset, NvU64 Size);

// Compute the physical address of a byte within a MEMORY_DESCRIPTOR
RmPhysAddr memdescGetPhysAddr(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU64 offset);

// Compute count physical addresses within a MEMORY_DESCRIPTOR. Starting at the
// given offset and advancing it by stride for each consecutive address.
void memdescGetPhysAddrs(MEMORY_DESCRIPTOR *pMemDesc,
                         ADDRESS_TRANSLATION addressTranslation,
                         NvU64 offset,
                         NvU64 stride,
                         NvU64 count,
                         RmPhysAddr *pAddresses);

// Compute count physical addresses within a MEMORY_DESCRIPTOR for a specific
// GPU. Starting at the given offset and advancing it by stride for each
// consecutive address.
void memdescGetPhysAddrsForGpu(MEMORY_DESCRIPTOR *pMemDesc,
                               OBJGPU *pGpu,
                               ADDRESS_TRANSLATION addressTranslation,
                               NvU64 offset,
                               NvU64 stride,
                               NvU64 count,
                               RmPhysAddr *pAddresses);

// Obtains one of the PTEs from the MEMORY_DESCRIPTOR.  Assumes 4KB pages,
// and works for either contiguous or noncontiguous descriptors.
RmPhysAddr memdescGetPte(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU32 PteIndex);

void memdescSetPte(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU32 PteIndex, RmPhysAddr PhysAddr);

// Obtains the PteArray from the MEMORY_DESCRIPTOR for the specified GPU.
RmPhysAddr * memdescGetPteArrayForGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu, ADDRESS_TRANSLATION addressTranslation);

/*!
 *  @brief Obtains the PteArray from the MEMORY_DESCRIPTOR.
 *
 *  @param[in]  pMemDesc           Memory descriptor to use
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns PageArray
 */
static inline RmPhysAddr *
memdescGetPteArray(PMEMORY_DESCRIPTOR  pMemDesc, ADDRESS_TRANSLATION addressTranslation)
{
    return memdescGetPteArrayForGpu(pMemDesc, pMemDesc->pGpu, addressTranslation);
}

// Obtains the PteArray size from the MEMORY_DESCRIPTOR based on the mmuContext.
NvU32 memdescGetPteArraySize(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation);

// Return the aperture of the NV_ADDRESS_SPACE as a null terminated string.
// Useful for print statements.
const char* memdescGetApertureString(NV_ADDRESS_SPACE addrSpace);

// Return true if two MEMORY_DESCRIPTOR are equal
NvBool memdescDescIsEqual(MEMORY_DESCRIPTOR *pMemDescOne, MEMORY_DESCRIPTOR *pMemDescTwo);

// Retrieve the per-GPU memory descriptor for a subdevice
MEMORY_DESCRIPTOR *memdescGetMemDescFromSubDeviceInst(MEMORY_DESCRIPTOR *pMemDesc, NvU32 subDeviceInst);

// Retrieve the per-GPU memory descriptor for a GPU
MEMORY_DESCRIPTOR *memdescGetMemDescFromGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu);

// Retrieve the per-GPU memory descriptor at an index
MEMORY_DESCRIPTOR *memdescGetMemDescFromIndex(MEMORY_DESCRIPTOR *pMemDesc, NvU32 index);

// Print information on memory descriptor
void memdescPrintMemdesc(MEMORY_DESCRIPTOR *pMemDesc, NvBool bPrintIndividualPages, const char *pPrefixMessage);

// Get the page offset for an arbitrary power of two page size
NvU64 memdescGetPageOffset(MEMORY_DESCRIPTOR *pMemDesc, NvU64 pageSize);

//
// Internal APIs for the IOVASPACE to manage IOMMU mappings in a memdesc.
//
// Note that the external APIs are memdescMapIommu(),
// memdescUnmapIommu() and memdescGetIommuMap().
//
NV_STATUS memdescAddIommuMap(PMEMORY_DESCRIPTOR pMemDesc, struct IOVAMAPPING *pIommuMap);
void memdescRemoveIommuMap(PMEMORY_DESCRIPTOR pMemDesc, struct IOVAMAPPING *pIommuMap);

//
// Map and unmap IOMMU for the specified VA space
//
// Each memdescUnmapIommu() call has to be paired with a previous successful
// memdescMapIommu() call for the same VA space. The calls are refcounted for
// each VA space and only the last Unmap will remove the mappings.
//
// The caller has to guarantee that before the VA space is destroyed, either the
// mapping is explicitly unmapped with memdescUnmapIommu() or the memdesc is
// freed (or destroyed for memdescs that are not memdescFree()d).
//
NV_STATUS memdescMapIommu(PMEMORY_DESCRIPTOR pMemDesc, NvU32 vaspaceId);
void memdescUnmapIommu(PMEMORY_DESCRIPTOR pMemDesc, NvU32 vaspaceId);

// Returns the IOVA mapping created by memdescMapIommu().
struct IOVAMAPPING *memdescGetIommuMap(PMEMORY_DESCRIPTOR pMemDesc, NvU32 vaspaceId);

//
// Check subdevice consistency functions
//
void memdescCheckSubDevicePageSizeConsistency(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, struct OBJVASPACE *pVAS,
                                              NvU64 pageSize, NvU64 pageOffset);
void memdescCheckSubDeviceMemContiguityConsistency(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, struct OBJVASPACE *pVAS,
                                                   NvBool bIsMemContiguous);
NV_STATUS memdescCheckSubDeviceKindComprConsistency(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, struct OBJVASPACE *pVAS,
                                                    NvU32 kind, COMPR_INFO *pComprInfo);

//
// Accessor functions
//
void memdescSetHeapOffset(MEMORY_DESCRIPTOR *pMemDesc, RmPhysAddr fbOffset);
void memdescSetCpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc, NvU32 cpuCacheAttrib);
void memdescSetGpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc, NvU32 GpuCacheAttrib);
NvU32 memdescGetGpuP2PCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetGpuP2PCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc, NvU32 GpuCacheAttrib);
NvU32 memdescGetPteKindForGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu);
void  memdescSetPteKindForGpu(MEMORY_DESCRIPTOR *pMemDesc, OBJGPU *pGpu, NvU32 pteKind);
NvU32 memdescGetPteKindCompressed(MEMORY_DESCRIPTOR *pMemDesc);
void  memdescSetPteKindCompressed(MEMORY_DESCRIPTOR *pMemDesc, NvU32 pteKindCmpr);
NvP64 memdescGetKernelMapping(MEMORY_DESCRIPTOR *pMemDesc);
void  memdescSetKernelMapping(MEMORY_DESCRIPTOR *pMemDesc, NvP64 kernelMapping);
NvP64 memdescGetKernelMappingPriv(MEMORY_DESCRIPTOR *pMemDesc);
void  memdescSetKernelMappingPriv(MEMORY_DESCRIPTOR *pMemDesc, NvP64 kernelMappingPriv);
MEMORY_DESCRIPTOR *memdescGetStandbyBuffer(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetStandbyBuffer(MEMORY_DESCRIPTOR *pMemDesc, MEMORY_DESCRIPTOR *pStandbyBuffer);
void memdescSetDestroyCallbackList(MEMORY_DESCRIPTOR *pMemDesc, MEM_DESC_DESTROY_CALLBACK *pCb);
NvU64 memdescGetGuestId(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetGuestId(MEMORY_DESCRIPTOR *pMemDesc, NvU64 guestId);
NvBool memdescGetFlag(MEMORY_DESCRIPTOR *pMemDesc, NvU64 flag);
void memdescSetFlag(MEMORY_DESCRIPTOR *pMemDesc, NvU64 flag, NvBool bValue);
NvP64 memdescGetAddress(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetAddress(MEMORY_DESCRIPTOR *pMemDesc, NvP64 pAddress);
void *memdescGetMemData(MEMORY_DESCRIPTOR *pMemDesc);
void memdescSetMemData(MEMORY_DESCRIPTOR *pMemDesc, void *pMemData, MEM_DATA_RELEASE_CALL_BACK *pMemDataReleaseCallback);
NvBool memdescGetVolatility(MEMORY_DESCRIPTOR *pMemDesc);
NvBool memdescGetContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation);
void memdescSetContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvBool isContiguous);
NvBool memdescCheckContiguity(PMEMORY_DESCRIPTOR pMemDesc, ADDRESS_TRANSLATION addressTranslation);
NV_ADDRESS_SPACE memdescGetAddressSpace(PMEMORY_DESCRIPTOR pMemDesc);
NvU64 memdescGetPageSize(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation);
void  memdescSetPageSize(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation, NvU64 pageSize);
PMEMORY_DESCRIPTOR memdescGetRootMemDesc(PMEMORY_DESCRIPTOR pMemDesc, NvU64 *pRootOffset);
void memdescSetCustomHeap(PMEMORY_DESCRIPTOR);
NvBool memdescGetCustomHeap(PMEMORY_DESCRIPTOR);
NV_STATUS memdescSetPageArrayGranularity(MEMORY_DESCRIPTOR *pMemDesc, NvU64 pageArrayGranularity);
NvBool memdescAcquireRmExclusiveUse(MEMORY_DESCRIPTOR *pMemDesc);
NV_STATUS memdescFillMemdescForPhysAttr(MEMORY_DESCRIPTOR *pMemDesc, ADDRESS_TRANSLATION addressTranslation,
                                        NvU64 *pOffset,NvU32 *pMemAperture, NvU32 *pMemKind, NvU32 *pZCullId,
                                        NvU32 *pGpuCacheAttr, NvU32 *pGpuP2PCacheAttr, NvU64 *contigSegmentSize);
NvBool memdescIsEgm(MEMORY_DESCRIPTOR *pMemDesc);

/*!
 *  @brief Get PTE kind
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  addressTranslation Address translation identifier
 *
 *  @returns Current PTE kind value.
 */
static inline NvU32
memdescGetPteKind(PMEMORY_DESCRIPTOR pMemDesc)
{
    return memdescGetPteKindForGpu(pMemDesc,  pMemDesc->pGpu);
}

/*!
 *  @brief Set PTE kind.
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  pteKind            New PTE kind
 *
 *  @returns nothing
 */
static inline void
memdescSetPteKind(PMEMORY_DESCRIPTOR pMemDesc, NvU32 pteKind)
{
    memdescSetPteKindForGpu(pMemDesc, pMemDesc->pGpu, pteKind);
}

/*!
 *  @brief Get HW resource identifier (HwResId)
 *
 *  TODO: Need to ensure this is checked per subdevice only.
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *
 *  @returns Current HW resource identifier
 */
static inline NvU32
memdescGetHwResId(PMEMORY_DESCRIPTOR pMemDesc)
{
    return pMemDesc->_hwResId;
}

/*!
 *  @brief Set HW resource identifier (HwResId)
 *
 *  @param[in]  pMemDesc           Memory descriptor pointer
 *  @param[in]  hwResId            New HW resource identifier
 *
 *  @returns nothing
 */
static inline void
memdescSetHwResId(PMEMORY_DESCRIPTOR pMemDesc, NvU32 hwResId)
{
    pMemDesc->_hwResId = hwResId;
}

/*!
 *  @brief Get mem destroy callback list pointer
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Pointer to mem destroy callback list
 */
static inline MEM_DESC_DESTROY_CALLBACK *
memdescGetDestroyCallbackList(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_pMemDestroyCallbackList;
}

/*!
 *  @brief Get the byte offset relative to the root memory descriptor.
 *
 *  Root memory descriptor is the Top level memory descriptor with no parent,
 *  from which this memory descriptor was derived.
 *
 *  @param[in]  pMemDesc  Return pointer to memory descriptor.
 *
 *  @returns the byte offset relative to Root memory descriptor.
 */
static inline NvU64
memdescGetRootOffset(PMEMORY_DESCRIPTOR pMemDesc)
{
    NvU64 rootOffset = 0;
    (void)memdescGetRootMemDesc(pMemDesc, &rootOffset);
    return rootOffset;
}

/*!
 *  @brief Get CPU cache attributes
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current CPU cache attributes
 */
static inline NvU32
memdescGetCpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_cpuCacheAttrib;
}

/*!
 *  @brief Get GPU cache attributes
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current GPU cache attributes
 */
static inline NvU32
memdescGetGpuCacheAttrib(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_gpuCacheAttrib;
}

/*!
 *  @brief Return pte adjust
 *
 *  PteAdjust is zero whenever the memory is allocated as allocations are always
 *  going to be page-size aligned. However, we can have memory descriptors
 *  created on pre-allocated addresses + offset that aren't page aligned.
 *  PteAdjust is non-zero in such cases. We do not allow memdescDescribe operation
 *  (i.e. memory descriptors created on pre-allocated address) for subdevice
 *  memdesc and hence top level memdesc is always used to access pte adjust.
 *
 *  @param[in]  pMemDesc   Memory descriptor to use
 *
 *  @returns PteAdjust
 */
static inline NvU32
memdescGetPteAdjust(PMEMORY_DESCRIPTOR pMemDesc)
{
    return pMemDesc->PteAdjust;
}

/*!
 *  @brief Get subdevice allocation count.
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Current subdevice allocation count value.
 */
static inline NvU32
memdescGetSubDeviceAllocCount (MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_subDeviceAllocCount;
}

/*!
 *  @brief Get memory descriptor of parent
 *
 *  @param[in]  pMemDesc    Memory descriptor pointer
 *
 *  @returns Memory descriptor of parent
 */
static inline MEMORY_DESCRIPTOR *
memdescGetParentDescriptor(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_pParentDescriptor;
}

/*!
 *  @brief Set the address space of the memory descriptor
 *
 *  @param[in]  pMemDesc           Memory descriptor used
 *  @param[in]  addressTranslation Address translation identifier
 *  @param[in]  addressSpace       Address Space
 *
 *  @returns nothing
 */
static inline void
memdescSetAddressSpace(PMEMORY_DESCRIPTOR pMemDesc, NV_ADDRESS_SPACE addressSpace)
{
    pMemDesc->_addressSpace = addressSpace;
}

/*!
 *  @brief Return size
 *
 *  @param[in]  pMemDesc   Memory descriptor to use
 *
 *  @returns Size
 */
static inline NvU64
memdescGetSize(PMEMORY_DESCRIPTOR pMemDesc)
{
    return pMemDesc->Size;
}

/*!
 *  @brief Set CPU NUMA node to allocate memory from
 *
 *  @param[in]  pMemDesc    Memory Descriptor to use
 *  @param[in]  numaNode    NUMA node to allocate memory from
 */
static NV_INLINE void
memdescSetNumaNode(MEMORY_DESCRIPTOR *pMemDesc, NvS32 numaNode)
{
    pMemDesc->numaNode = numaNode;
}

/*!
 *  @brief Get CPU NUMA node to allocate memory from
 *
 *  @param[in]  pMemDesc    Memory Descriptor to use
 *
 *  @returns    NUMA node to allocate memory from
 */
static NV_INLINE NvS32
memdescGetNumaNode(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->numaNode;
}

/*!
 *  @brief Checks if subdevice memory descriptors are present
 *
 *  See memdescGetMemDescFromSubDeviceInst for an explanation of subdevice memory
 *  descriptors
 *
 *  @param[in]  pMemDesc  Memory descriptor to query
 *
 *  @returns NV_TRUE if subdevice memory descriptors exist
 */
static NV_INLINE NvBool
memdescHasSubDeviceMemDescs(MEMORY_DESCRIPTOR *pMemDesc)
{
    return (pMemDesc->_subDeviceAllocCount > 1);
}

/*!
 *  @brief Checks if memory descriptor describes memory that is submemory
 *
 *  @param[in]  pMemDesc  Memory descriptor to query
 *
 *  @returns NV_TRUE if it is a submemory desc, NV_FALSE otherwise.
 */
static NV_INLINE NvBool
memdescIsSubMemoryMemDesc(MEMORY_DESCRIPTOR *pMemDesc)
{
    return pMemDesc->_pParentDescriptor != NULL ? NV_TRUE : NV_FALSE;
}

NV_STATUS memdescGetNvLinkGpa(OBJGPU *pGpu, NvU64 pageCount, RmPhysAddr *pGpa);

NV_STATUS memdescSetCtxBufPool(PMEMORY_DESCRIPTOR pMemDesc, CTX_BUF_POOL_INFO* pCtxBufPool);
CTX_BUF_POOL_INFO* memdescGetCtxBufPool(PMEMORY_DESCRIPTOR pMemDesc);

/*!
 * @brief Override the registry INST_LOC two-bit enum to an aperture (list) + cpu attr.
 *
 * loc parameters uses NV_REG_STR_RM_INST_LOC defines.
 * Caller must set initial default values.
 */
void memdescOverrideInstLoc(NvU32 loc, const char *name, NV_ADDRESS_SPACE *pAddrSpace, NvU32 *pCpuMappingAttr);
void memdescOverrideInstLocList(NvU32 loc, const char *name, const NV_ADDRESS_SPACE **ppAllocList, NvU32 *pCpuMappingAttr);

/*!
* @brief Override the physical system address limit.
*
*/
void memdescOverridePhysicalAddressWidthWindowsWAR(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 addressWidth);

/*!
* @brief Send memory descriptor from CPU-RM to GSP
*
* This function will create a MemoryList object with the MEMORY_DESCRIPTOR information on CPU-RM
* It will then use memRegisterWithGsp API to create a corresponding MemoryList object on GSP-RM
* with the same Handle as that on CPU-RM
*
* This MemoryList object has the same MEMORY_DESCRIPTOR info as the input pMemDesc
* The CPU-RM handle can be sent to GSP-RM and then used on GSP end to retrieve the MemoryList object
* and then the corresponding MEMORY_DESCRIPTOR
*
* @param[in]  pGpu          OBJGPU pointer
* @param[in]  pMemDesc      MemDesc pointer
* @param[out] pHandle       Pointer to handle of MemoryList object
*
* @returns NV_STATUS
*/
NV_STATUS memdescSendMemDescToGSP(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvHandle *pHandle);

// cache maintenance functions
void memdescFlushGpuCaches(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc);
void memdescFlushCpuCaches(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc);

// Map memory descriptor for RM internal access
void* memdescMapInternal(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);
void memdescUnmapInternal(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags);

/*!
 * @brief Set the name of the surface.
 *
 * @param[in] pGpu     OBJGPU pointer.
 * @param[in] pMemDesc MEMORY_DESCRIPTOR pointer that the name is to be set for.
 * @param[in] name     const char pointer to the name to be set.
 */
void memdescSetName(OBJGPU*, MEMORY_DESCRIPTOR *pMemDesc, const char *name, const char *suffix);

//
// External flags:
//   ALLOC_PER_SUBDEVICE    Allocate independent system memory for each GPU
//   LOST_ON_SUSPEND        PM code will skip this allocation during S/R
//   LOCKLESS_SYSMEM_ALLOC  System memory should be allocated unprotected by
//                          the  RM lock
//   GPU_PRIVILEGED         This memory will be marked as privileged in the GPU
//                          page tables.  When set only GPU requestors who are
//                          "privileged" are allowed to access this memory.
//                          This can be used for mapping sensitive memory into
//                          a user's GPU address space (like context buffers).
//                          Note support for this in our GPUs is limited, so
//                          only use it if you know the HW accessing the memory
//                          makes privileged requests.
//
// Internal flags:
//   SET_KIND               Whether or not the kind was set a different value
//                          than default.
//   PRE_ALLOCATED          Caller provided memory descriptor memory
//   FIXED_ADDRESS_ALLOCATE Allocate from the heap with a fixed address
//   ALLOCATED              Has the memory been allocated yet?
//   GUEST_ALLOCATED        Is the memory allocated by a guest VM?
//                          We make aliased memory descriptors to guest
//                          allocated memory and mark it so, so that we know
//                          how to deal with it in memdescMap() etc.
//   KERNEL_MODE            Is the memory for a user or kernel context?
//                          XXX This is lame, and it would be best if we could
//                          get rid of it.  Memory *storage* isn't either user
//                          or kernel -- only mappings are user or kernel.
//                          Unfortunately, osAllocPages requires that we
//                          provide this information.
//  PHYSICALLY_CONTIGUOUS   Are the underlying physical pages of this memory
//                          allocation contiguous?
//  ENCRYPTED               TurboCipher allocations need a bit in the PTE to
//                          indicate encrypted
//  UNICAST                 Memory descriptor was created via UC path
//  PAGED_SYSMEM            Allocate the memory from paged system memory. When
//                          this flag is used, memdescLock() should be called
//                          to lock the memory in physical pages before we
//                          access this memory descriptor.
//  CPU_ONLY                Allocate memory only accessed by CPU.
//
#define MEMDESC_FLAGS_NONE                         ((NvU64)0x0)
#define MEMDESC_FLAGS_ALLOC_PER_SUBDEVICE          NVBIT64(0)
#define MEMDESC_FLAGS_SET_KIND                     NVBIT64(1)
#define MEMDESC_FLAGS_LOST_ON_SUSPEND              NVBIT64(2)
#define MEMDESC_FLAGS_PRE_ALLOCATED                NVBIT64(3)
#define MEMDESC_FLAGS_FIXED_ADDRESS_ALLOCATE       NVBIT64(4)
#define MEMDESC_FLAGS_LOCKLESS_SYSMEM_ALLOC        NVBIT64(5)
#define MEMDESC_FLAGS_GPU_IN_RESET                 NVBIT64(6)
#define MEMDESC_ALLOC_FLAGS_PROTECTED              NVBIT64(7)
#define MEMDESC_FLAGS_GUEST_ALLOCATED              NVBIT64(8)
#define MEMDESC_FLAGS_KERNEL_MODE                  NVBIT64(9)
#define MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS        NVBIT64(10)
#define MEMDESC_FLAGS_ENCRYPTED                    NVBIT64(11)
#define MEMDESC_FLAGS_PAGED_SYSMEM                 NVBIT64(12)
#define MEMDESC_FLAGS_GPU_PRIVILEGED               NVBIT64(13)
#define MEMDESC_FLAGS_PRESERVE_CONTENT_ON_SUSPEND  NVBIT64(14)
#define MEMDESC_FLAGS_DUMMY_TOPLEVEL               NVBIT64(15)

// Don't use the below two flags. For memdesc internal use only.
// These flags will be removed on memory allocation refactoring in RM
#define MEMDESC_FLAGS_PROVIDE_IOMMU_MAP            NVBIT64(16)
#define MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE        NVBIT64(17)

#define MEMDESC_FLAGS_CUSTOM_HEAP_ACR              NVBIT64(18)

// Allocate in "fast" or "slow" memory, if there are multiple grades of memory (like mixed density)
#define MEMDESC_FLAGS_HIGH_PRIORITY                NVBIT64(19)
#define MEMDESC_FLAGS_LOW_PRIORITY                 NVBIT64(20)

// Flag to specify if requested size should be rounded to page size
#define MEMDESC_FLAGS_PAGE_SIZE_ALIGN_IGNORE       NVBIT64(21)

#define MEMDESC_FLAGS_CPU_ONLY                     NVBIT64(22)

// This flags is used for a special SYSMEM descriptor that points to a memory
// region allocated externally (e.g. malloc, kmalloc etc.)
#define MEMDESC_FLAGS_EXT_PAGE_ARRAY_MEM           NVBIT64(23)

// Owned by Physical Memory Allocator (PMA).
#define MEMDESC_FLAGS_ALLOC_PMA_OWNED              NVBIT64(24)

// This flag is added as part of Sub-Allocator feature meant to be used by VGPU clients.
// Once VGPU clients allocate a large block of memory for their use, they carve-out a small
// portion of it to be used for RM internal allocations originating from a given client. Each
// allocation can choose to use this carved-out memory owned by client or be part of global heap.
// This flag has to be used in RM internal allocation only when a particular allocation is tied to
// the life-time of this client and will be freed before client gets destroyed.
#define MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE      NVBIT64(25)

// This flag is used to specify the pages are pinned using other kernel module or API
// Currently, this flag is used for vGPU on KVM where RM calls vfio APIs to pin and unpin pages
// instead of using os_lock_user_pages() and os_unlock_user_pages().
#define MEMDESC_FLAGS_FOREIGN_PAGE                 NVBIT64(26)

// These flags are used for SYSMEM descriptors that point to a physical BAR
// range and do not take the usual memory mapping paths. Currently, these are used for vGPU.
#define MEMDESC_FLAGS_BAR0_REFLECT                 NVBIT64(27)
#define MEMDESC_FLAGS_BAR1_REFLECT                 NVBIT64(28)

// This flag is used to create shared memory required for vGPU operation.
// During RPC and all other shared memory allocations, VF RM will set this flag to instruct mods
// layer to create shared memory between VF process and PF process.
#define MEMDESC_FLAGS_MODS_SHARED_MEM              NVBIT64(29)

// This flag is set in memdescs that describe client (currently MODS) managed VPR allocations.
#define MEMDESC_FLAGS_VPR_REGION_CLIENT_MANAGED    NVBIT64(30)

// This flags is used for a special SYSMEM descriptor that points to physical BAR
// range of a third party device.
#define MEMDESC_FLAGS_PEER_IO_MEM                  NVBIT64(31)

// If the flag is set, the RM will only allow read-only CPU user-mappings
// to the descriptor.
#define MEMDESC_FLAGS_USER_READ_ONLY               NVBIT64(32)

// If the flag is set, the RM will only allow read-only DMA mappings
// to the descriptor.
#define MEMDESC_FLAGS_DEVICE_READ_ONLY             NVBIT64(33)

// This flag is used to denote the memory descriptor that is part of larger memory descriptor;
// created using NV01_MEMORY_LIST_SYSTEM, NV01_MEMORY_LIST_FBMEM or NV01_MEMORY_LIST_OBJECT.
#define MEMDESC_FLAGS_LIST_MEMORY                  NVBIT64(34)

// unused                                          NVBIT64(35)

// This flag is used to denote that this memdesc is allocated from
// a context buffer pool. When this flag is set, we expect a pointer
// to this context buffer pool to be cached in memdesc.
#define MEMDESC_FLAGS_OWNED_BY_CTX_BUF_POOL        NVBIT64(36)

//
// This flag is used to skip privilege checks for the ADDR_REGMEM mapping type.
// This flag is useful for cases like UserModeApi where we want to use this memory type
// in a non-privileged user context
#define MEMDESC_FLAGS_SKIP_REGMEM_PRIV_CHECK       NVBIT64(37)

// This flag denotes the memory descriptor of type Display non iso
#define MEMDESC_FLAGS_MEMORY_TYPE_DISPLAY_NISO     NVBIT64(38)

// This flag is used to force mapping of coherent sysmem through
// the GMMU over BAR1. This is useful when we need some form
// of special translation of the SYSMEM_COH aperture by the GMMU.
#define MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1         NVBIT64(39)

// This flag is used to override system memory limit to be allocated
// within override address width.
#define MEMDESC_FLAGS_OVERRIDE_SYSTEM_ADDRESS_LIMIT   NVBIT64(40)

//
// If this flag is set, Linux RM will ensure that the allocated memory is
// 32-bit addressable.
#define MEMDESC_FLAGS_ALLOC_32BIT_ADDRESSABLE      NVBIT64(41)

// unused                                          NVBIT64(42)

//
// If this flag is set then it indicates that the memory associated with
// this descriptor was allocated from local EGM.
//
#define MEMDESC_FLAGS_ALLOC_FROM_EGM               NVBIT64(43)

//
// Indicates that this memdesc is tracking client sysmem allocation as
// against RM internal sysmem allocation
//
#define MEMDESC_FLAGS_SYSMEM_OWNED_BY_CLIENT       NVBIT64(44)
//
// Clients (including RM) should set this flag to request allocations in
// unprotected memory. This is required for Confidential Compute cases
//
#define MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY  NVBIT64(45)

//
// The following is a special use case for sharing memory between
// the GPU and a WSL client. There is no IOMMU-compliant support
// currently for this, so a WAR is required for r515. The intent
// is to remove this by r525.
//
#define MEMDESC_FLAGS_WSL_SHARED_MEMORY            NVBIT64(46)

//
// Skip IOMMU mapping creation during alloc for sysmem.
// A mapping might be requested later with custom parameters.
//
#define MEMDESC_FLAGS_SKIP_IOMMU_MAPPING           NVBIT64(47)

//
// Specical case to allocate the runlists for Guests from its GPA
// In MODS, VM's GPA allocated from subheap so using this define to
// Forcing memdesc to allocated from subheap
//
#define MEMDESC_FLAGS_FORCE_ALLOC_FROM_SUBHEAP     NVBIT64(48)

//
// Indicate if memdesc needs to restore pte kind in the static bar1 mode
// when it is freed.
//
#define MEMDESC_FLAGS_RESTORE_PTE_KIND_ON_FREE     NVBIT64(49)


#endif // _MEMDESC_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_DESC_NVOC_H_
