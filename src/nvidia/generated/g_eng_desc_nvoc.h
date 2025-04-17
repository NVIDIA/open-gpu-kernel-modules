
#ifndef _G_ENG_DESC_NVOC_H_
#define _G_ENG_DESC_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_eng_desc_nvoc.h"

#ifndef _ENG_DESC_H_
#define _ENG_DESC_H_


#include "core/core.h"

//
// Engine descriptors
//
// An ENGDESCRIPTOR carries both an NVOC_CLASS_ID and an instance ID. For example,
// to specify the engine CE1, use MKENGDESC(classId(OBJCE), 1).
//
#define ENGDESC_CLASS  31:8
#define ENGDESC_INST    7:0

#define MKENGDESC(class, inst)     ((((NvU32)(class)) << SF_SHIFT(ENGDESC_CLASS)) | \
                                    ((inst ) << SF_SHIFT(ENGDESC_INST )))

#define ENGDESC_FIELD(desc, field) (((desc) >> SF_SHIFT(ENGDESC ## field)) & \
                                    SF_MASK(ENGDESC ## field))

typedef NvU32 ENGDESCRIPTOR;


//
// Class declarations to get classIds for use with ENGDESCRIPTOR
//

struct OBJINVALID;

#ifndef __NVOC_CLASS_OBJINVALID_TYPEDEF__
#define __NVOC_CLASS_OBJINVALID_TYPEDEF__
typedef struct OBJINVALID OBJINVALID;
#endif /* __NVOC_CLASS_OBJINVALID_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJINVALID
#define __nvoc_class_id_OBJINVALID 0xb33b15
#endif /* __nvoc_class_id_OBJINVALID */

       // classId only.  Not a real class

struct OBJSWENG;

#ifndef __NVOC_CLASS_OBJSWENG_TYPEDEF__
#define __NVOC_CLASS_OBJSWENG_TYPEDEF__
typedef struct OBJSWENG OBJSWENG;
#endif /* __NVOC_CLASS_OBJSWENG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSWENG
#define __nvoc_class_id_OBJSWENG 0x95a6f5
#endif /* __nvoc_class_id_OBJSWENG */



struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct Falcon;

#ifndef __NVOC_CLASS_Falcon_TYPEDEF__
#define __NVOC_CLASS_Falcon_TYPEDEF__
typedef struct Falcon Falcon;
#endif /* __NVOC_CLASS_Falcon_TYPEDEF__ */

#ifndef __nvoc_class_id_Falcon
#define __nvoc_class_id_Falcon 0xdc5264
#endif /* __nvoc_class_id_Falcon */



struct OBJMC;

#ifndef __NVOC_CLASS_OBJMC_TYPEDEF__
#define __NVOC_CLASS_OBJMC_TYPEDEF__
typedef struct OBJMC OBJMC;
#endif /* __NVOC_CLASS_OBJMC_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJMC
#define __nvoc_class_id_OBJMC 0x9aad0e
#endif /* __nvoc_class_id_OBJMC */



struct KernelMc;

#ifndef __NVOC_CLASS_KernelMc_TYPEDEF__
#define __NVOC_CLASS_KernelMc_TYPEDEF__
typedef struct KernelMc KernelMc;
#endif /* __NVOC_CLASS_KernelMc_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMc
#define __nvoc_class_id_KernelMc 0x3827ff
#endif /* __nvoc_class_id_KernelMc */



struct PrivRing;

#ifndef __NVOC_CLASS_PrivRing_TYPEDEF__
#define __NVOC_CLASS_PrivRing_TYPEDEF__
typedef struct PrivRing PrivRing;
#endif /* __NVOC_CLASS_PrivRing_TYPEDEF__ */

#ifndef __nvoc_class_id_PrivRing
#define __nvoc_class_id_PrivRing 0x4c57c4
#endif /* __nvoc_class_id_PrivRing */



struct SwIntr;

#ifndef __NVOC_CLASS_SwIntr_TYPEDEF__
#define __NVOC_CLASS_SwIntr_TYPEDEF__
typedef struct SwIntr SwIntr;
#endif /* __NVOC_CLASS_SwIntr_TYPEDEF__ */

#ifndef __nvoc_class_id_SwIntr
#define __nvoc_class_id_SwIntr 0x5ca633
#endif /* __nvoc_class_id_SwIntr */



struct MemorySystem;

#ifndef __NVOC_CLASS_MemorySystem_TYPEDEF__
#define __NVOC_CLASS_MemorySystem_TYPEDEF__
typedef struct MemorySystem MemorySystem;
#endif /* __NVOC_CLASS_MemorySystem_TYPEDEF__ */

#ifndef __nvoc_class_id_MemorySystem
#define __nvoc_class_id_MemorySystem 0x174e21
#endif /* __nvoc_class_id_MemorySystem */



struct KernelMemorySystem;

#ifndef __NVOC_CLASS_KernelMemorySystem_TYPEDEF__
#define __NVOC_CLASS_KernelMemorySystem_TYPEDEF__
typedef struct KernelMemorySystem KernelMemorySystem;
#endif /* __NVOC_CLASS_KernelMemorySystem_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMemorySystem
#define __nvoc_class_id_KernelMemorySystem 0x7faff1
#endif /* __nvoc_class_id_KernelMemorySystem */



struct MemoryManager;

#ifndef __NVOC_CLASS_MemoryManager_TYPEDEF__
#define __NVOC_CLASS_MemoryManager_TYPEDEF__
typedef struct MemoryManager MemoryManager;
#endif /* __NVOC_CLASS_MemoryManager_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryManager
#define __nvoc_class_id_MemoryManager 0x22ad47
#endif /* __nvoc_class_id_MemoryManager */



struct OBJFBFLCN;

#ifndef __NVOC_CLASS_OBJFBFLCN_TYPEDEF__
#define __NVOC_CLASS_OBJFBFLCN_TYPEDEF__
typedef struct OBJFBFLCN OBJFBFLCN;
#endif /* __NVOC_CLASS_OBJFBFLCN_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJFBFLCN
#define __nvoc_class_id_OBJFBFLCN 0x8a20bf
#endif /* __nvoc_class_id_OBJFBFLCN */



struct OBJHSHUBMANAGER;

#ifndef __NVOC_CLASS_OBJHSHUBMANAGER_TYPEDEF__
#define __NVOC_CLASS_OBJHSHUBMANAGER_TYPEDEF__
typedef struct OBJHSHUBMANAGER OBJHSHUBMANAGER;
#endif /* __NVOC_CLASS_OBJHSHUBMANAGER_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHSHUBMANAGER
#define __nvoc_class_id_OBJHSHUBMANAGER 0xbb32b9
#endif /* __nvoc_class_id_OBJHSHUBMANAGER */



struct Hshub;

#ifndef __NVOC_CLASS_Hshub_TYPEDEF__
#define __NVOC_CLASS_Hshub_TYPEDEF__
typedef struct Hshub Hshub;
#endif /* __NVOC_CLASS_Hshub_TYPEDEF__ */

#ifndef __nvoc_class_id_Hshub
#define __nvoc_class_id_Hshub 0x5b3331
#endif /* __nvoc_class_id_Hshub */



struct OBJTMR;

#ifndef __NVOC_CLASS_OBJTMR_TYPEDEF__
#define __NVOC_CLASS_OBJTMR_TYPEDEF__
typedef struct OBJTMR OBJTMR;
#endif /* __NVOC_CLASS_OBJTMR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJTMR
#define __nvoc_class_id_OBJTMR 0x9ddede
#endif /* __nvoc_class_id_OBJTMR */



struct VirtMemAllocator;

#ifndef __NVOC_CLASS_VirtMemAllocator_TYPEDEF__
#define __NVOC_CLASS_VirtMemAllocator_TYPEDEF__
typedef struct VirtMemAllocator VirtMemAllocator;
#endif /* __NVOC_CLASS_VirtMemAllocator_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtMemAllocator
#define __nvoc_class_id_VirtMemAllocator 0x899e48
#endif /* __nvoc_class_id_VirtMemAllocator */



struct Graphics;

#ifndef __NVOC_CLASS_Graphics_TYPEDEF__
#define __NVOC_CLASS_Graphics_TYPEDEF__
typedef struct Graphics Graphics;
#endif /* __NVOC_CLASS_Graphics_TYPEDEF__ */

#ifndef __nvoc_class_id_Graphics
#define __nvoc_class_id_Graphics 0xd334df
#endif /* __nvoc_class_id_Graphics */



struct OBJGR;

#ifndef __NVOC_CLASS_OBJGR_TYPEDEF__
#define __NVOC_CLASS_OBJGR_TYPEDEF__
typedef struct OBJGR OBJGR;
#endif /* __NVOC_CLASS_OBJGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGR
#define __nvoc_class_id_OBJGR 0xb0940a
#endif /* __nvoc_class_id_OBJGR */

            // classId only. Not a real class. Bug 200664045

struct GraphicsManager;

#ifndef __NVOC_CLASS_GraphicsManager_TYPEDEF__
#define __NVOC_CLASS_GraphicsManager_TYPEDEF__
typedef struct GraphicsManager GraphicsManager;
#endif /* __NVOC_CLASS_GraphicsManager_TYPEDEF__ */

#ifndef __nvoc_class_id_GraphicsManager
#define __nvoc_class_id_GraphicsManager 0x2f465a
#endif /* __nvoc_class_id_GraphicsManager */



struct KernelGraphicsManager;

#ifndef __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__
typedef struct KernelGraphicsManager KernelGraphicsManager;
#endif /* __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsManager
#define __nvoc_class_id_KernelGraphicsManager 0xd22179
#endif /* __nvoc_class_id_KernelGraphicsManager */



struct MIGManager;

#ifndef __NVOC_CLASS_MIGManager_TYPEDEF__
#define __NVOC_CLASS_MIGManager_TYPEDEF__
typedef struct MIGManager MIGManager;
#endif /* __NVOC_CLASS_MIGManager_TYPEDEF__ */

#ifndef __nvoc_class_id_MIGManager
#define __nvoc_class_id_MIGManager 0xfd75d0
#endif /* __nvoc_class_id_MIGManager */



struct KernelMIGManager;

#ifndef __NVOC_CLASS_KernelMIGManager_TYPEDEF__
#define __NVOC_CLASS_KernelMIGManager_TYPEDEF__
typedef struct KernelMIGManager KernelMIGManager;
#endif /* __NVOC_CLASS_KernelMIGManager_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMIGManager
#define __nvoc_class_id_KernelMIGManager 0x01c1bf
#endif /* __nvoc_class_id_KernelMIGManager */



struct SMDebugger;

#ifndef __NVOC_CLASS_SMDebugger_TYPEDEF__
#define __NVOC_CLASS_SMDebugger_TYPEDEF__
typedef struct SMDebugger SMDebugger;
#endif /* __NVOC_CLASS_SMDebugger_TYPEDEF__ */

#ifndef __nvoc_class_id_SMDebugger
#define __nvoc_class_id_SMDebugger 0x12018b
#endif /* __nvoc_class_id_SMDebugger */



struct KernelGraphics;

#ifndef __NVOC_CLASS_KernelGraphics_TYPEDEF__
#define __NVOC_CLASS_KernelGraphics_TYPEDEF__
typedef struct KernelGraphics KernelGraphics;
#endif /* __NVOC_CLASS_KernelGraphics_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphics
#define __nvoc_class_id_KernelGraphics 0xea3fa9
#endif /* __nvoc_class_id_KernelGraphics */



struct KernelFifo;

#ifndef __NVOC_CLASS_KernelFifo_TYPEDEF__
#define __NVOC_CLASS_KernelFifo_TYPEDEF__
typedef struct KernelFifo KernelFifo;
#endif /* __NVOC_CLASS_KernelFifo_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFifo
#define __nvoc_class_id_KernelFifo 0xf3e155
#endif /* __nvoc_class_id_KernelFifo */



struct OBJFIFO;

#ifndef __NVOC_CLASS_OBJFIFO_TYPEDEF__
#define __NVOC_CLASS_OBJFIFO_TYPEDEF__
typedef struct OBJFIFO OBJFIFO;
#endif /* __NVOC_CLASS_OBJFIFO_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJFIFO
#define __nvoc_class_id_OBJFIFO 0xb02365
#endif /* __nvoc_class_id_OBJFIFO */



struct OBJOS;

#ifndef __NVOC_CLASS_OBJOS_TYPEDEF__
#define __NVOC_CLASS_OBJOS_TYPEDEF__
typedef struct OBJOS OBJOS;
#endif /* __NVOC_CLASS_OBJOS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70
#endif /* __nvoc_class_id_OBJOS */



struct OBJBUS;

#ifndef __NVOC_CLASS_OBJBUS_TYPEDEF__
#define __NVOC_CLASS_OBJBUS_TYPEDEF__
typedef struct OBJBUS OBJBUS;
#endif /* __NVOC_CLASS_OBJBUS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJBUS
#define __nvoc_class_id_OBJBUS 0xcc4c31
#endif /* __nvoc_class_id_OBJBUS */



struct KernelBus;

#ifndef __NVOC_CLASS_KernelBus_TYPEDEF__
#define __NVOC_CLASS_KernelBus_TYPEDEF__
typedef struct KernelBus KernelBus;
#endif /* __NVOC_CLASS_KernelBus_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelBus
#define __nvoc_class_id_KernelBus 0xd2ac57
#endif /* __nvoc_class_id_KernelBus */



struct OBJINFOROM;

#ifndef __NVOC_CLASS_OBJINFOROM_TYPEDEF__
#define __NVOC_CLASS_OBJINFOROM_TYPEDEF__
typedef struct OBJINFOROM OBJINFOROM;
#endif /* __NVOC_CLASS_OBJINFOROM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJINFOROM
#define __nvoc_class_id_OBJINFOROM 0x0e1639
#endif /* __nvoc_class_id_OBJINFOROM */



struct Perf;

#ifndef __NVOC_CLASS_Perf_TYPEDEF__
#define __NVOC_CLASS_Perf_TYPEDEF__
typedef struct Perf Perf;
#endif /* __NVOC_CLASS_Perf_TYPEDEF__ */

#ifndef __nvoc_class_id_Perf
#define __nvoc_class_id_Perf 0xed0b65
#endif /* __nvoc_class_id_Perf */



struct KernelPerf;

#ifndef __NVOC_CLASS_KernelPerf_TYPEDEF__
#define __NVOC_CLASS_KernelPerf_TYPEDEF__
typedef struct KernelPerf KernelPerf;
#endif /* __NVOC_CLASS_KernelPerf_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelPerf
#define __nvoc_class_id_KernelPerf 0xc53a57
#endif /* __nvoc_class_id_KernelPerf */



struct Pxuc;

#ifndef __NVOC_CLASS_Pxuc_TYPEDEF__
#define __NVOC_CLASS_Pxuc_TYPEDEF__
typedef struct Pxuc Pxuc;
#endif /* __NVOC_CLASS_Pxuc_TYPEDEF__ */

#ifndef __nvoc_class_id_Pxuc
#define __nvoc_class_id_Pxuc 0xba06f5
#endif /* __nvoc_class_id_Pxuc */



struct OBJBIF;

#ifndef __NVOC_CLASS_OBJBIF_TYPEDEF__
#define __NVOC_CLASS_OBJBIF_TYPEDEF__
typedef struct OBJBIF OBJBIF;
#endif /* __NVOC_CLASS_OBJBIF_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJBIF
#define __nvoc_class_id_OBJBIF 0xd1c956
#endif /* __nvoc_class_id_OBJBIF */



struct KernelBif;

#ifndef __NVOC_CLASS_KernelBif_TYPEDEF__
#define __NVOC_CLASS_KernelBif_TYPEDEF__
typedef struct KernelBif KernelBif;
#endif /* __NVOC_CLASS_KernelBif_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelBif
#define __nvoc_class_id_KernelBif 0xdbe523
#endif /* __nvoc_class_id_KernelBif */



struct OBJSF;

#ifndef __NVOC_CLASS_OBJSF_TYPEDEF__
#define __NVOC_CLASS_OBJSF_TYPEDEF__
typedef struct OBJSF OBJSF;
#endif /* __NVOC_CLASS_OBJSF_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSF
#define __nvoc_class_id_OBJSF 0x0bd720
#endif /* __nvoc_class_id_OBJSF */



struct OBJGPIO;

#ifndef __NVOC_CLASS_OBJGPIO_TYPEDEF__
#define __NVOC_CLASS_OBJGPIO_TYPEDEF__
typedef struct OBJGPIO OBJGPIO;
#endif /* __NVOC_CLASS_OBJGPIO_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPIO
#define __nvoc_class_id_OBJGPIO 0x05c7b5
#endif /* __nvoc_class_id_OBJGPIO */



struct ClockManager;

#ifndef __NVOC_CLASS_ClockManager_TYPEDEF__
#define __NVOC_CLASS_ClockManager_TYPEDEF__
typedef struct ClockManager ClockManager;
#endif /* __NVOC_CLASS_ClockManager_TYPEDEF__ */

#ifndef __nvoc_class_id_ClockManager
#define __nvoc_class_id_ClockManager 0xbcadd3
#endif /* __nvoc_class_id_ClockManager */



struct KernelDisplay;

#ifndef __NVOC_CLASS_KernelDisplay_TYPEDEF__
#define __NVOC_CLASS_KernelDisplay_TYPEDEF__
typedef struct KernelDisplay KernelDisplay;
#endif /* __NVOC_CLASS_KernelDisplay_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelDisplay
#define __nvoc_class_id_KernelDisplay 0x55952e
#endif /* __nvoc_class_id_KernelDisplay */



struct OBJDISP;

#ifndef __NVOC_CLASS_OBJDISP_TYPEDEF__
#define __NVOC_CLASS_OBJDISP_TYPEDEF__
typedef struct OBJDISP OBJDISP;
#endif /* __NVOC_CLASS_OBJDISP_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDISP
#define __nvoc_class_id_OBJDISP 0xd1755e
#endif /* __nvoc_class_id_OBJDISP */



struct OBJDPU;

#ifndef __NVOC_CLASS_OBJDPU_TYPEDEF__
#define __NVOC_CLASS_OBJDPU_TYPEDEF__
typedef struct OBJDPU OBJDPU;
#endif /* __NVOC_CLASS_OBJDPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDPU
#define __nvoc_class_id_OBJDPU 0x23486d
#endif /* __nvoc_class_id_OBJDPU */



struct OBJDIP;

#ifndef __NVOC_CLASS_OBJDIP_TYPEDEF__
#define __NVOC_CLASS_OBJDIP_TYPEDEF__
typedef struct OBJDIP OBJDIP;
#endif /* __NVOC_CLASS_OBJDIP_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDIP
#define __nvoc_class_id_OBJDIP 0x1cc271
#endif /* __nvoc_class_id_OBJDIP */



struct Fan;

#ifndef __NVOC_CLASS_Fan_TYPEDEF__
#define __NVOC_CLASS_Fan_TYPEDEF__
typedef struct Fan Fan;
#endif /* __NVOC_CLASS_Fan_TYPEDEF__ */

#ifndef __nvoc_class_id_Fan
#define __nvoc_class_id_Fan 0xadd018
#endif /* __nvoc_class_id_Fan */



struct DisplayInstanceMemory;

#ifndef __NVOC_CLASS_DisplayInstanceMemory_TYPEDEF__
#define __NVOC_CLASS_DisplayInstanceMemory_TYPEDEF__
typedef struct DisplayInstanceMemory DisplayInstanceMemory;
#endif /* __NVOC_CLASS_DisplayInstanceMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_DisplayInstanceMemory
#define __nvoc_class_id_DisplayInstanceMemory 0x8223e2
#endif /* __nvoc_class_id_DisplayInstanceMemory */



struct KernelHead;

#ifndef __NVOC_CLASS_KernelHead_TYPEDEF__
#define __NVOC_CLASS_KernelHead_TYPEDEF__
typedef struct KernelHead KernelHead;
#endif /* __NVOC_CLASS_KernelHead_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHead
#define __nvoc_class_id_KernelHead 0x0145e6
#endif /* __nvoc_class_id_KernelHead */



struct OBJVOLT;

#ifndef __NVOC_CLASS_OBJVOLT_TYPEDEF__
#define __NVOC_CLASS_OBJVOLT_TYPEDEF__
typedef struct OBJVOLT OBJVOLT;
#endif /* __NVOC_CLASS_OBJVOLT_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVOLT
#define __nvoc_class_id_OBJVOLT 0xa68120
#endif /* __nvoc_class_id_OBJVOLT */



struct Intr;

#ifndef __NVOC_CLASS_Intr_TYPEDEF__
#define __NVOC_CLASS_Intr_TYPEDEF__
typedef struct Intr Intr;
#endif /* __NVOC_CLASS_Intr_TYPEDEF__ */

#ifndef __nvoc_class_id_Intr
#define __nvoc_class_id_Intr 0xc06e44
#endif /* __nvoc_class_id_Intr */



struct OBJHDA;

#ifndef __NVOC_CLASS_OBJHDA_TYPEDEF__
#define __NVOC_CLASS_OBJHDA_TYPEDEF__
typedef struct OBJHDA OBJHDA;
#endif /* __NVOC_CLASS_OBJHDA_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHDA
#define __nvoc_class_id_OBJHDA 0xd3bfb4
#endif /* __nvoc_class_id_OBJHDA */



struct I2c;

#ifndef __NVOC_CLASS_I2c_TYPEDEF__
#define __NVOC_CLASS_I2c_TYPEDEF__
typedef struct I2c I2c;
#endif /* __NVOC_CLASS_I2c_TYPEDEF__ */

#ifndef __nvoc_class_id_I2c
#define __nvoc_class_id_I2c 0x48e035
#endif /* __nvoc_class_id_I2c */



struct KernelRc;

#ifndef __NVOC_CLASS_KernelRc_TYPEDEF__
#define __NVOC_CLASS_KernelRc_TYPEDEF__
typedef struct KernelRc KernelRc;
#endif /* __NVOC_CLASS_KernelRc_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelRc
#define __nvoc_class_id_KernelRc 0x4888db
#endif /* __nvoc_class_id_KernelRc */



struct OBJRC;

#ifndef __NVOC_CLASS_OBJRC_TYPEDEF__
#define __NVOC_CLASS_OBJRC_TYPEDEF__
typedef struct OBJRC OBJRC;
#endif /* __NVOC_CLASS_OBJRC_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJRC
#define __nvoc_class_id_OBJRC 0x42d150
#endif /* __nvoc_class_id_OBJRC */



struct OBJSOR;

#ifndef __NVOC_CLASS_OBJSOR_TYPEDEF__
#define __NVOC_CLASS_OBJSOR_TYPEDEF__
typedef struct OBJSOR OBJSOR;
#endif /* __NVOC_CLASS_OBJSOR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSOR
#define __nvoc_class_id_OBJSOR 0x5ccbfa
#endif /* __nvoc_class_id_OBJSOR */



struct OBJDAC;

#ifndef __NVOC_CLASS_OBJDAC_TYPEDEF__
#define __NVOC_CLASS_OBJDAC_TYPEDEF__
typedef struct OBJDAC OBJDAC;
#endif /* __NVOC_CLASS_OBJDAC_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDAC
#define __nvoc_class_id_OBJDAC 0x4b1802
#endif /* __nvoc_class_id_OBJDAC */



struct OBJPIOR;

#ifndef __NVOC_CLASS_OBJPIOR_TYPEDEF__
#define __NVOC_CLASS_OBJPIOR_TYPEDEF__
typedef struct OBJPIOR OBJPIOR;
#endif /* __NVOC_CLASS_OBJPIOR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJPIOR
#define __nvoc_class_id_OBJPIOR 0x0128a3
#endif /* __nvoc_class_id_OBJPIOR */



struct OBJHEAD;

#ifndef __NVOC_CLASS_OBJHEAD_TYPEDEF__
#define __NVOC_CLASS_OBJHEAD_TYPEDEF__
typedef struct OBJHEAD OBJHEAD;
#endif /* __NVOC_CLASS_OBJHEAD_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHEAD
#define __nvoc_class_id_OBJHEAD 0x74dd86
#endif /* __nvoc_class_id_OBJHEAD */



struct OBJVGA;

#ifndef __NVOC_CLASS_OBJVGA_TYPEDEF__
#define __NVOC_CLASS_OBJVGA_TYPEDEF__
typedef struct OBJVGA OBJVGA;
#endif /* __NVOC_CLASS_OBJVGA_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVGA
#define __nvoc_class_id_OBJVGA 0x84e0bc
#endif /* __nvoc_class_id_OBJVGA */



struct Stereo;

#ifndef __NVOC_CLASS_Stereo_TYPEDEF__
#define __NVOC_CLASS_Stereo_TYPEDEF__
typedef struct Stereo Stereo;
#endif /* __NVOC_CLASS_Stereo_TYPEDEF__ */

#ifndef __nvoc_class_id_Stereo
#define __nvoc_class_id_Stereo 0xbbc45d
#endif /* __nvoc_class_id_Stereo */



struct OBJOR;

#ifndef __NVOC_CLASS_OBJOR_TYPEDEF__
#define __NVOC_CLASS_OBJOR_TYPEDEF__
typedef struct OBJOR OBJOR;
#endif /* __NVOC_CLASS_OBJOR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJOR
#define __nvoc_class_id_OBJOR 0x215d6b
#endif /* __nvoc_class_id_OBJOR */



struct OBJBSP;

#ifndef __NVOC_CLASS_OBJBSP_TYPEDEF__
#define __NVOC_CLASS_OBJBSP_TYPEDEF__
typedef struct OBJBSP OBJBSP;
#endif /* __NVOC_CLASS_OBJBSP_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJBSP
#define __nvoc_class_id_OBJBSP 0x8f99e1
#endif /* __nvoc_class_id_OBJBSP */



struct OBJCIPHER;

#ifndef __NVOC_CLASS_OBJCIPHER_TYPEDEF__
#define __NVOC_CLASS_OBJCIPHER_TYPEDEF__
typedef struct OBJCIPHER OBJCIPHER;
#endif /* __NVOC_CLASS_OBJCIPHER_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJCIPHER
#define __nvoc_class_id_OBJCIPHER 0x8dd911
#endif /* __nvoc_class_id_OBJCIPHER */



struct OBJFUSE;

#ifndef __NVOC_CLASS_OBJFUSE_TYPEDEF__
#define __NVOC_CLASS_OBJFUSE_TYPEDEF__
typedef struct OBJFUSE OBJFUSE;
#endif /* __NVOC_CLASS_OBJFUSE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJFUSE
#define __nvoc_class_id_OBJFUSE 0x95ba71
#endif /* __nvoc_class_id_OBJFUSE */



struct Jtag;

#ifndef __NVOC_CLASS_Jtag_TYPEDEF__
#define __NVOC_CLASS_Jtag_TYPEDEF__
typedef struct Jtag Jtag;
#endif /* __NVOC_CLASS_Jtag_TYPEDEF__ */

#ifndef __nvoc_class_id_Jtag
#define __nvoc_class_id_Jtag 0xd73cf9
#endif /* __nvoc_class_id_Jtag */



struct OBJHDCP;

#ifndef __NVOC_CLASS_OBJHDCP_TYPEDEF__
#define __NVOC_CLASS_OBJHDCP_TYPEDEF__
typedef struct OBJHDCP OBJHDCP;
#endif /* __NVOC_CLASS_OBJHDCP_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHDCP
#define __nvoc_class_id_OBJHDCP 0x426d44
#endif /* __nvoc_class_id_OBJHDCP */



struct OBJHDMI;

#ifndef __NVOC_CLASS_OBJHDMI_TYPEDEF__
#define __NVOC_CLASS_OBJHDMI_TYPEDEF__
typedef struct OBJHDMI OBJHDMI;
#endif /* __NVOC_CLASS_OBJHDMI_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHDMI
#define __nvoc_class_id_OBJHDMI 0x2213b6
#endif /* __nvoc_class_id_OBJHDMI */



struct Therm;

#ifndef __NVOC_CLASS_Therm_TYPEDEF__
#define __NVOC_CLASS_Therm_TYPEDEF__
typedef struct Therm Therm;
#endif /* __NVOC_CLASS_Therm_TYPEDEF__ */

#ifndef __nvoc_class_id_Therm
#define __nvoc_class_id_Therm 0x6c1e56
#endif /* __nvoc_class_id_Therm */



struct OBJSEQ;

#ifndef __NVOC_CLASS_OBJSEQ_TYPEDEF__
#define __NVOC_CLASS_OBJSEQ_TYPEDEF__
typedef struct OBJSEQ OBJSEQ;
#endif /* __NVOC_CLASS_OBJSEQ_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSEQ
#define __nvoc_class_id_OBJSEQ 0x45da4a
#endif /* __nvoc_class_id_OBJSEQ */



struct OBJDPAUX;

#ifndef __NVOC_CLASS_OBJDPAUX_TYPEDEF__
#define __NVOC_CLASS_OBJDPAUX_TYPEDEF__
typedef struct OBJDPAUX OBJDPAUX;
#endif /* __NVOC_CLASS_OBJDPAUX_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDPAUX
#define __nvoc_class_id_OBJDPAUX 0xfd2ab9
#endif /* __nvoc_class_id_OBJDPAUX */



struct Pmu;

#ifndef __NVOC_CLASS_Pmu_TYPEDEF__
#define __NVOC_CLASS_Pmu_TYPEDEF__
typedef struct Pmu Pmu;
#endif /* __NVOC_CLASS_Pmu_TYPEDEF__ */

#ifndef __nvoc_class_id_Pmu
#define __nvoc_class_id_Pmu 0xf3d722
#endif /* __nvoc_class_id_Pmu */



struct KernelPmu;

#ifndef __NVOC_CLASS_KernelPmu_TYPEDEF__
#define __NVOC_CLASS_KernelPmu_TYPEDEF__
typedef struct KernelPmu KernelPmu;
#endif /* __NVOC_CLASS_KernelPmu_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelPmu
#define __nvoc_class_id_KernelPmu 0xab9d7d
#endif /* __nvoc_class_id_KernelPmu */


struct Lpwr;

#ifndef __NVOC_CLASS_Lpwr_TYPEDEF__
#define __NVOC_CLASS_Lpwr_TYPEDEF__
typedef struct Lpwr Lpwr;
#endif /* __NVOC_CLASS_Lpwr_TYPEDEF__ */

#ifndef __nvoc_class_id_Lpwr
#define __nvoc_class_id_Lpwr 0x112230
#endif /* __nvoc_class_id_Lpwr */



struct OBJISOHUB;

#ifndef __NVOC_CLASS_OBJISOHUB_TYPEDEF__
#define __NVOC_CLASS_OBJISOHUB_TYPEDEF__
typedef struct OBJISOHUB OBJISOHUB;
#endif /* __NVOC_CLASS_OBJISOHUB_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJISOHUB
#define __nvoc_class_id_OBJISOHUB 0x7c5e0d
#endif /* __nvoc_class_id_OBJISOHUB */



struct Pmgr;

#ifndef __NVOC_CLASS_Pmgr_TYPEDEF__
#define __NVOC_CLASS_Pmgr_TYPEDEF__
typedef struct Pmgr Pmgr;
#endif /* __NVOC_CLASS_Pmgr_TYPEDEF__ */

#ifndef __nvoc_class_id_Pmgr
#define __nvoc_class_id_Pmgr 0x894574
#endif /* __nvoc_class_id_Pmgr */



struct OBJHDACODEC;

#ifndef __NVOC_CLASS_OBJHDACODEC_TYPEDEF__
#define __NVOC_CLASS_OBJHDACODEC_TYPEDEF__
typedef struct OBJHDACODEC OBJHDACODEC;
#endif /* __NVOC_CLASS_OBJHDACODEC_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHDACODEC
#define __nvoc_class_id_OBJHDACODEC 0xa576e2
#endif /* __nvoc_class_id_OBJHDACODEC */



struct Spi;

#ifndef __NVOC_CLASS_Spi_TYPEDEF__
#define __NVOC_CLASS_Spi_TYPEDEF__
typedef struct Spi Spi;
#endif /* __NVOC_CLASS_Spi_TYPEDEF__ */

#ifndef __nvoc_class_id_Spi
#define __nvoc_class_id_Spi 0x824313
#endif /* __nvoc_class_id_Spi */



struct OBJUVM;

#ifndef __NVOC_CLASS_OBJUVM_TYPEDEF__
#define __NVOC_CLASS_OBJUVM_TYPEDEF__
typedef struct OBJUVM OBJUVM;
#endif /* __NVOC_CLASS_OBJUVM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJUVM
#define __nvoc_class_id_OBJUVM 0xf9a17d
#endif /* __nvoc_class_id_OBJUVM */



struct OBJSEC2;

#ifndef __NVOC_CLASS_OBJSEC2_TYPEDEF__
#define __NVOC_CLASS_OBJSEC2_TYPEDEF__
typedef struct OBJSEC2 OBJSEC2;
#endif /* __NVOC_CLASS_OBJSEC2_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSEC2
#define __nvoc_class_id_OBJSEC2 0x28c408
#endif /* __nvoc_class_id_OBJSEC2 */



struct OBJPMS;

#ifndef __NVOC_CLASS_OBJPMS_TYPEDEF__
#define __NVOC_CLASS_OBJPMS_TYPEDEF__
typedef struct OBJPMS OBJPMS;
#endif /* __NVOC_CLASS_OBJPMS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJPMS
#define __nvoc_class_id_OBJPMS 0x9e3810
#endif /* __nvoc_class_id_OBJPMS */



struct OBJENGSTATE;

#ifndef __NVOC_CLASS_OBJENGSTATE_TYPEDEF__
#define __NVOC_CLASS_OBJENGSTATE_TYPEDEF__
typedef struct OBJENGSTATE OBJENGSTATE;
#endif /* __NVOC_CLASS_OBJENGSTATE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJENGSTATE
#define __nvoc_class_id_OBJENGSTATE 0x7a7ed6
#endif /* __nvoc_class_id_OBJENGSTATE */



struct OBJLSFM;

#ifndef __NVOC_CLASS_OBJLSFM_TYPEDEF__
#define __NVOC_CLASS_OBJLSFM_TYPEDEF__
typedef struct OBJLSFM OBJLSFM;
#endif /* __NVOC_CLASS_OBJLSFM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJLSFM
#define __nvoc_class_id_OBJLSFM 0x9a25e4
#endif /* __nvoc_class_id_OBJLSFM */



struct OBJACR;

#ifndef __NVOC_CLASS_OBJACR_TYPEDEF__
#define __NVOC_CLASS_OBJACR_TYPEDEF__
typedef struct OBJACR OBJACR;
#endif /* __NVOC_CLASS_OBJACR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJACR
#define __nvoc_class_id_OBJACR 0xdb32a1
#endif /* __nvoc_class_id_OBJACR */



struct OBJGPULOG;

#ifndef __NVOC_CLASS_OBJGPULOG_TYPEDEF__
#define __NVOC_CLASS_OBJGPULOG_TYPEDEF__
typedef struct OBJGPULOG OBJGPULOG;
#endif /* __NVOC_CLASS_OBJGPULOG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPULOG
#define __nvoc_class_id_OBJGPULOG 0xdd19be
#endif /* __nvoc_class_id_OBJGPULOG */



struct KernelNvlink;

#ifndef __NVOC_CLASS_KernelNvlink_TYPEDEF__
#define __NVOC_CLASS_KernelNvlink_TYPEDEF__
typedef struct KernelNvlink KernelNvlink;
#endif /* __NVOC_CLASS_KernelNvlink_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelNvlink
#define __nvoc_class_id_KernelNvlink 0xce6818
#endif /* __nvoc_class_id_KernelNvlink */



struct Nvlink;

#ifndef __NVOC_CLASS_Nvlink_TYPEDEF__
#define __NVOC_CLASS_Nvlink_TYPEDEF__
typedef struct Nvlink Nvlink;
#endif /* __NVOC_CLASS_Nvlink_TYPEDEF__ */

#ifndef __nvoc_class_id_Nvlink
#define __nvoc_class_id_Nvlink 0x790a3c
#endif /* __nvoc_class_id_Nvlink */



struct KernelHwpm;

#ifndef __NVOC_CLASS_KernelHwpm_TYPEDEF__
#define __NVOC_CLASS_KernelHwpm_TYPEDEF__
typedef struct KernelHwpm KernelHwpm;
#endif /* __NVOC_CLASS_KernelHwpm_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHwpm
#define __nvoc_class_id_KernelHwpm 0xc8c00f
#endif /* __nvoc_class_id_KernelHwpm */



struct OBJHWPM;

#ifndef __NVOC_CLASS_OBJHWPM_TYPEDEF__
#define __NVOC_CLASS_OBJHWPM_TYPEDEF__
typedef struct OBJHWPM OBJHWPM;
#endif /* __NVOC_CLASS_OBJHWPM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHWPM
#define __nvoc_class_id_OBJHWPM 0x97e43b
#endif /* __nvoc_class_id_OBJHWPM */



struct OBJGPUMON;

#ifndef __NVOC_CLASS_OBJGPUMON_TYPEDEF__
#define __NVOC_CLASS_OBJGPUMON_TYPEDEF__
typedef struct OBJGPUMON OBJGPUMON;
#endif /* __NVOC_CLASS_OBJGPUMON_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUMON
#define __nvoc_class_id_OBJGPUMON 0x2b424b
#endif /* __nvoc_class_id_OBJGPUMON */



struct OBJGRIDDISPLAYLESS;

#ifndef __NVOC_CLASS_OBJGRIDDISPLAYLESS_TYPEDEF__
#define __NVOC_CLASS_OBJGRIDDISPLAYLESS_TYPEDEF__
typedef struct OBJGRIDDISPLAYLESS OBJGRIDDISPLAYLESS;
#endif /* __NVOC_CLASS_OBJGRIDDISPLAYLESS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGRIDDISPLAYLESS
#define __nvoc_class_id_OBJGRIDDISPLAYLESS 0x20fd5a
#endif /* __nvoc_class_id_OBJGRIDDISPLAYLESS */



struct FECS;

#ifndef __NVOC_CLASS_FECS_TYPEDEF__
#define __NVOC_CLASS_FECS_TYPEDEF__
typedef struct FECS FECS;
#endif /* __NVOC_CLASS_FECS_TYPEDEF__ */

#ifndef __nvoc_class_id_FECS
#define __nvoc_class_id_FECS 0x5ee8dc
#endif /* __nvoc_class_id_FECS */



struct GPCCS;

#ifndef __NVOC_CLASS_GPCCS_TYPEDEF__
#define __NVOC_CLASS_GPCCS_TYPEDEF__
typedef struct GPCCS GPCCS;
#endif /* __NVOC_CLASS_GPCCS_TYPEDEF__ */

#ifndef __nvoc_class_id_GPCCS
#define __nvoc_class_id_GPCCS 0x4781e8
#endif /* __nvoc_class_id_GPCCS */



struct OBJCE;

#ifndef __NVOC_CLASS_OBJCE_TYPEDEF__
#define __NVOC_CLASS_OBJCE_TYPEDEF__
typedef struct OBJCE OBJCE;
#endif /* __NVOC_CLASS_OBJCE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJCE
#define __nvoc_class_id_OBJCE 0x793ceb
#endif /* __nvoc_class_id_OBJCE */



struct KernelCE;

#ifndef __NVOC_CLASS_KernelCE_TYPEDEF__
#define __NVOC_CLASS_KernelCE_TYPEDEF__
typedef struct KernelCE KernelCE;
#endif /* __NVOC_CLASS_KernelCE_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCE
#define __nvoc_class_id_KernelCE 0x242aca
#endif /* __nvoc_class_id_KernelCE */



struct OBJMSENC;

#ifndef __NVOC_CLASS_OBJMSENC_TYPEDEF__
#define __NVOC_CLASS_OBJMSENC_TYPEDEF__
typedef struct OBJMSENC OBJMSENC;
#endif /* __NVOC_CLASS_OBJMSENC_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJMSENC
#define __nvoc_class_id_OBJMSENC 0xe97b6c
#endif /* __nvoc_class_id_OBJMSENC */



struct OBJNVJPG;

#ifndef __NVOC_CLASS_OBJNVJPG_TYPEDEF__
#define __NVOC_CLASS_OBJNVJPG_TYPEDEF__
typedef struct OBJNVJPG OBJNVJPG;
#endif /* __NVOC_CLASS_OBJNVJPG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJNVJPG
#define __nvoc_class_id_OBJNVJPG 0x2b3a54
#endif /* __nvoc_class_id_OBJNVJPG */



struct OBJVMMU;

#ifndef __NVOC_CLASS_OBJVMMU_TYPEDEF__
#define __NVOC_CLASS_OBJVMMU_TYPEDEF__
typedef struct OBJVMMU OBJVMMU;
#endif /* __NVOC_CLASS_OBJVMMU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVMMU
#define __nvoc_class_id_OBJVMMU 0xdf8918
#endif /* __nvoc_class_id_OBJVMMU */



struct Gsp;

#ifndef __NVOC_CLASS_Gsp_TYPEDEF__
#define __NVOC_CLASS_Gsp_TYPEDEF__
typedef struct Gsp Gsp;
#endif /* __NVOC_CLASS_Gsp_TYPEDEF__ */

#ifndef __nvoc_class_id_Gsp
#define __nvoc_class_id_Gsp 0xda3de4
#endif /* __nvoc_class_id_Gsp */



struct OBJFSP;

#ifndef __NVOC_CLASS_OBJFSP_TYPEDEF__
#define __NVOC_CLASS_OBJFSP_TYPEDEF__
typedef struct OBJFSP OBJFSP;
#endif /* __NVOC_CLASS_OBJFSP_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJFSP
#define __nvoc_class_id_OBJFSP 0xd39158
#endif /* __nvoc_class_id_OBJFSP */



struct KernelFsp;

#ifndef __NVOC_CLASS_KernelFsp_TYPEDEF__
#define __NVOC_CLASS_KernelFsp_TYPEDEF__
typedef struct KernelFsp KernelFsp;
#endif /* __NVOC_CLASS_KernelFsp_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFsp
#define __nvoc_class_id_KernelFsp 0x87fb96
#endif /* __nvoc_class_id_KernelFsp */



struct OBJOFA;

#ifndef __NVOC_CLASS_OBJOFA_TYPEDEF__
#define __NVOC_CLASS_OBJOFA_TYPEDEF__
typedef struct OBJOFA OBJOFA;
#endif /* __NVOC_CLASS_OBJOFA_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJOFA
#define __nvoc_class_id_OBJOFA 0xdd7bab
#endif /* __nvoc_class_id_OBJOFA */



struct KernelIoctrl;

#ifndef __NVOC_CLASS_KernelIoctrl_TYPEDEF__
#define __NVOC_CLASS_KernelIoctrl_TYPEDEF__
typedef struct KernelIoctrl KernelIoctrl;
#endif /* __NVOC_CLASS_KernelIoctrl_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelIoctrl
#define __nvoc_class_id_KernelIoctrl 0x880c7d
#endif /* __nvoc_class_id_KernelIoctrl */



struct Ioctrl;

#ifndef __NVOC_CLASS_Ioctrl_TYPEDEF__
#define __NVOC_CLASS_Ioctrl_TYPEDEF__
typedef struct Ioctrl Ioctrl;
#endif /* __NVOC_CLASS_Ioctrl_TYPEDEF__ */

#ifndef __nvoc_class_id_Ioctrl
#define __nvoc_class_id_Ioctrl 0x11ce10
#endif /* __nvoc_class_id_Ioctrl */



struct KernelSec2;

#ifndef __NVOC_CLASS_KernelSec2_TYPEDEF__
#define __NVOC_CLASS_KernelSec2_TYPEDEF__
typedef struct KernelSec2 KernelSec2;
#endif /* __NVOC_CLASS_KernelSec2_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSec2
#define __nvoc_class_id_KernelSec2 0x2f36c9
#endif /* __nvoc_class_id_KernelSec2 */



struct KernelGsp;

#ifndef __NVOC_CLASS_KernelGsp_TYPEDEF__
#define __NVOC_CLASS_KernelGsp_TYPEDEF__
typedef struct KernelGsp KernelGsp;
#endif /* __NVOC_CLASS_KernelGsp_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGsp
#define __nvoc_class_id_KernelGsp 0x311d4e
#endif /* __nvoc_class_id_KernelGsp */



struct OBJDCECLIENTRM;

#ifndef __NVOC_CLASS_OBJDCECLIENTRM_TYPEDEF__
#define __NVOC_CLASS_OBJDCECLIENTRM_TYPEDEF__
typedef struct OBJDCECLIENTRM OBJDCECLIENTRM;
#endif /* __NVOC_CLASS_OBJDCECLIENTRM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDCECLIENTRM
#define __nvoc_class_id_OBJDCECLIENTRM 0x61649c
#endif /* __nvoc_class_id_OBJDCECLIENTRM */



struct OBJDISPMACRO;

#ifndef __NVOC_CLASS_OBJDISPMACRO_TYPEDEF__
#define __NVOC_CLASS_OBJDISPMACRO_TYPEDEF__
typedef struct OBJDISPMACRO OBJDISPMACRO;
#endif /* __NVOC_CLASS_OBJDISPMACRO_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDISPMACRO
#define __nvoc_class_id_OBJDISPMACRO 0xa1cad2
#endif /* __nvoc_class_id_OBJDISPMACRO */



struct Nne;

#ifndef __NVOC_CLASS_Nne_TYPEDEF__
#define __NVOC_CLASS_Nne_TYPEDEF__
typedef struct Nne Nne;
#endif /* __NVOC_CLASS_Nne_TYPEDEF__ */

#ifndef __nvoc_class_id_Nne
#define __nvoc_class_id_Nne 0x2487e2
#endif /* __nvoc_class_id_Nne */



struct Oob;

#ifndef __NVOC_CLASS_Oob_TYPEDEF__
#define __NVOC_CLASS_Oob_TYPEDEF__
typedef struct Oob Oob;
#endif /* __NVOC_CLASS_Oob_TYPEDEF__ */

#ifndef __nvoc_class_id_Oob
#define __nvoc_class_id_Oob 0x98b919
#endif /* __nvoc_class_id_Oob */



struct OBJDSI;

#ifndef __NVOC_CLASS_OBJDSI_TYPEDEF__
#define __NVOC_CLASS_OBJDSI_TYPEDEF__
typedef struct OBJDSI OBJDSI;
#endif /* __NVOC_CLASS_OBJDSI_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDSI
#define __nvoc_class_id_OBJDSI 0x2e9a64
#endif /* __nvoc_class_id_OBJDSI */



struct OBJDCB;

#ifndef __NVOC_CLASS_OBJDCB_TYPEDEF__
#define __NVOC_CLASS_OBJDCB_TYPEDEF__
typedef struct OBJDCB OBJDCB;
#endif /* __NVOC_CLASS_OBJDCB_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDCB
#define __nvoc_class_id_OBJDCB 0xf931d4
#endif /* __nvoc_class_id_OBJDCB */



struct KernelGmmu;

#ifndef __NVOC_CLASS_KernelGmmu_TYPEDEF__
#define __NVOC_CLASS_KernelGmmu_TYPEDEF__
typedef struct KernelGmmu KernelGmmu;
#endif /* __NVOC_CLASS_KernelGmmu_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGmmu
#define __nvoc_class_id_KernelGmmu 0x29362f
#endif /* __nvoc_class_id_KernelGmmu */



struct OBJGMMU;

#ifndef __NVOC_CLASS_OBJGMMU_TYPEDEF__
#define __NVOC_CLASS_OBJGMMU_TYPEDEF__
typedef struct OBJGMMU OBJGMMU;
#endif /* __NVOC_CLASS_OBJGMMU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGMMU
#define __nvoc_class_id_OBJGMMU 0xd7a41d
#endif /* __nvoc_class_id_OBJGMMU */


struct ConfidentialCompute;

#ifndef __NVOC_CLASS_ConfidentialCompute_TYPEDEF__
#define __NVOC_CLASS_ConfidentialCompute_TYPEDEF__
typedef struct ConfidentialCompute ConfidentialCompute;
#endif /* __NVOC_CLASS_ConfidentialCompute_TYPEDEF__ */

#ifndef __nvoc_class_id_ConfidentialCompute
#define __nvoc_class_id_ConfidentialCompute 0x9798cc
#endif /* __nvoc_class_id_ConfidentialCompute */


struct KernelCcu;

#ifndef __NVOC_CLASS_KernelCcu_TYPEDEF__
#define __NVOC_CLASS_KernelCcu_TYPEDEF__
typedef struct KernelCcu KernelCcu;
#endif /* __NVOC_CLASS_KernelCcu_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCcu
#define __nvoc_class_id_KernelCcu 0x5d5b68
#endif /* __nvoc_class_id_KernelCcu */


struct KernelGsplite;

#ifndef __NVOC_CLASS_KernelGsplite_TYPEDEF__
#define __NVOC_CLASS_KernelGsplite_TYPEDEF__
typedef struct KernelGsplite KernelGsplite;
#endif /* __NVOC_CLASS_KernelGsplite_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGsplite
#define __nvoc_class_id_KernelGsplite 0x927969
#endif /* __nvoc_class_id_KernelGsplite */


struct Spdm;

#ifndef __NVOC_CLASS_Spdm_TYPEDEF__
#define __NVOC_CLASS_Spdm_TYPEDEF__
typedef struct Spdm Spdm;
#endif /* __NVOC_CLASS_Spdm_TYPEDEF__ */

#ifndef __nvoc_class_id_Spdm
#define __nvoc_class_id_Spdm 0x261ee8
#endif /* __nvoc_class_id_Spdm */


#define ENG_CLASS_INVALID              classId(OBJINVALID)
#define ENG_CLASS_SW                   classId(OBJSWENG)
#define ENG_CLASS_GPU                  classId(OBJGPU)
#define ENG_CLASS_FLCN                 classId(Falcon)
#define ENG_CLASS_MC                   classId(OBJMC)
#define ENG_CLASS_KERNEL_MC            classId(KernelMc)
#define ENG_CLASS_PRIV_RING            classId(PrivRing)
#define ENG_CLASS_SW_INTR              classId(SwIntr)
#define ENG_CLASS_MEMORY_SYSTEM        classId(MemorySystem)
#define ENG_CLASS_KERNEL_MEMORY_SYSTEM classId(KernelMemorySystem)
#define ENG_CLASS_MEMORY_MANAGER       classId(MemoryManager)
#define ENG_CLASS_FBFLCN               classId(OBJFBFLCN)
#define ENG_CLASS_TMR                  classId(OBJTMR)
#define ENG_CLASS_DMA                  classId(VirtMemAllocator)
#define ENG_CLASS_KERNEL_FIFO          classId(KernelFifo)
#define ENG_CLASS_FIFO                 classId(OBJFIFO)
#define ENG_CLASS_OS                   classId(OBJOS)
#define ENG_CLASS_BUS                  classId(OBJBUS)
#define ENG_CLASS_KERNEL_BUS           classId(KernelBus)
#define ENG_CLASS_INFOROM              classId(OBJINFOROM)
#define ENG_CLASS_PERF                 classId(Perf)
#define ENG_CLASS_KERNEL_PERF          classId(KernelPerf)
#define ENG_CLASS_PXUC                 classId(Pxuc)
#define ENG_CLASS_BIF                  classId(OBJBIF)
#define ENG_CLASS_KERNEL_BIF           classId(KernelBif)
#define ENG_CLASS_HSHUBMANAGER         classId(OBJHSHUBMANAGER)
#define ENG_CLASS_SF                   classId(OBJSF)
#define ENG_CLASS_GPIO                 classId(OBJGPIO)
#define ENG_CLASS_CLK                  classId(ClockManager)
#define ENG_CLASS_KERNEL_DISPLAY       classId(KernelDisplay)
#define ENG_CLASS_DISP                 classId(OBJDISP)
#define ENG_CLASS_DPU                  classId(OBJDPU)
#define ENG_CLASS_DIP                  classId(OBJDIP)
#define ENG_CLASS_FAN                  classId(Fan)
#define ENG_CLASS_INST                 classId(DisplayInstanceMemory)
#define ENG_CLASS_KERNEL_HEAD          classId(KernelHead)
#define ENG_CLASS_VOLT                 classId(OBJVOLT)
#define ENG_CLASS_INTR                 classId(Intr)
#define ENG_CLASS_HDA                  classId(OBJHDA)
#define ENG_CLASS_I2C                  classId(I2c)
#define ENG_CLASS_KERNEL_RC            classId(KernelRc)
#define ENG_CLASS_RC                   classId(OBJRC)
#define ENG_CLASS_SOR                  classId(OBJSOR)
#define ENG_CLASS_DAC                  classId(OBJDAC)
#define ENG_CLASS_PIOR                 classId(OBJPIOR)
#define ENG_CLASS_HEAD                 classId(OBJHEAD)
#define ENG_CLASS_VGA                  classId(OBJVGA)
#define ENG_CLASS_STEREO               classId(Stereo)
#define ENG_CLASS_OR                   classId(OBJOR)
#define ENG_CLASS_NVDEC                classId(OBJBSP)
#define ENG_CLASS_CIPHER               classId(OBJCIPHER)
#define ENG_CLASS_FUSE                 classId(OBJFUSE)
#define ENG_CLASS_JTAG                 classId(Jtag)
#define ENG_CLASS_HDCP                 classId(OBJHDCP)
#define ENG_CLASS_HDMI                 classId(OBJHDMI)
#define ENG_CLASS_THERM                classId(Therm)
#define ENG_CLASS_SEQ                  classId(OBJSEQ)
#define ENG_CLASS_DPAUX                classId(OBJDPAUX)
#define ENG_CLASS_PMU                  classId(Pmu)
#define ENG_CLASS_KERNEL_PMU           classId(KernelPmu)
#define ENG_CLASS_LPWR                 classId(Lpwr)
#define ENG_CLASS_ISOHUB               classId(OBJISOHUB)
#define ENG_CLASS_PMGR                 classId(Pmgr)
#define ENG_CLASS_HDACODEC             classId(OBJHDACODEC)
#define ENG_CLASS_SPI                  classId(Spi)
#define ENG_CLASS_UVM                  classId(OBJUVM)
#define ENG_CLASS_SEC2                 classId(OBJSEC2)
#define ENG_CLASS_PMS                  classId(OBJPMS)
#define ENG_CLASS_ENGSTATE             classId(OBJENGSTATE)
#define ENG_CLASS_LSFM                 classId(OBJLSFM)
#define ENG_CLASS_ACR                  classId(OBJACR)
#define ENG_CLASS_GPULOG               classId(OBJGPULOG)
#define ENG_CLASS_NVLINK               classId(Nvlink)
#define ENG_CLASS_HWPM                 classId(OBJHWPM)
#define ENG_CLASS_KERNEL_HWPM          classId(KernelHwpm)
#define ENG_CLASS_GPUMON               classId(OBJGPUMON)
#define ENG_CLASS_GRIDDISPLAYLESS      classId(OBJGRIDDISPLAYLESS)
#define ENG_CLASS_VMMU                 classId(OBJVMMU)
#define ENG_CLASS_NVJPG                classId(OBJNVJPG)
#define ENG_CLASS_GSP                  classId(Gsp)
#define ENG_CLASS_FSP                  classId(OBJFSP)
#define ENG_CLASS_KERNEL_FSP           classId(KernelFsp)
#define ENG_CLASS_KERNEL_GSP           classId(KernelGsp)
#define ENG_CLASS_KERNEL_SEC2          classId(KernelSec2)
#define ENG_CLASS_DISPMACRO            classId(OBJDISPMACRO)
#define ENG_CLASS_NNE                  classId(OBJNNE)
#define ENG_CLASS_OOB                  classId(Oob)
#define ENG_CLASS_DSI                  classId(OBJDSI)
#define ENG_CLASS_DCECLIENTRM          classId(OBJDCECLIENTRM)
#define ENG_CLASS_DCB                  classId(OBJDCB)
#define ENG_CLASS_KERNEL_NVLINK        classId(KernelNvlink)
#define ENG_CLASS_GMMU                 classId(OBJGMMU)
#define ENG_CLASS_KERNEL_GMMU          classId(KernelGmmu)
#define ENG_CLASS_CONF_COMPUTE         classId(ConfidentialCompute)
#define ENG_CLASS_KERNEL_CCU           classId(KernelCcu)
#define ENG_CLASS_KERNEL_GSPLITE       classId(KernelGsplite)
#define ENG_CLASS_CE                   classId(OBJCE)
#define ENG_CLASS_NVENC                classId(OBJMSENC)
#define ENG_CLASS_NVDEC                classId(OBJBSP)
#define ENG_CLASS_GR                   classId(Graphics)
#define ENG_CLASS_NVJPEG               classId(OBJNVJPG)
#define ENG_CLASS_FECS                 classId(FECS)
#define ENG_CLASS_GPCCS                classId(GPCCS)
#define ENG_CLASS_IOCTRL               classId(Ioctrl)
#define ENG_CLASS_HSHUB                classId(Hshub)
#define ENG_CLASS_KERNEL_IOCTRL        classId(KernelIoctrl)
#define ENG_CLASS_OFA                  classId(OBJOFA)


//
// Engine tags to be used by both RM/HAL to reference specific engines.
//
// These values are used in the engine descriptor table
// as well as in the class descriptor table.
//
#define ENG_INVALID              MKENGDESC(ENG_CLASS_INVALID,              0)
#define ENG_SW                   MKENGDESC(ENG_CLASS_SW,                   0)
#define ENG_GPU                  MKENGDESC(ENG_CLASS_GPU,                  0)
#define ENG_FLCN                 MKENGDESC(ENG_CLASS_FLCN,                 0)
#define ENG_MC                   MKENGDESC(ENG_CLASS_MC,                   0)
#define ENG_KERNEL_MC            MKENGDESC(ENG_CLASS_KERNEL_MC,            0)
#define ENG_PRIV_RING            MKENGDESC(ENG_CLASS_PRIV_RING,            0)
#define ENG_SW_INTR              MKENGDESC(ENG_CLASS_SW_INTR,              0)
#define ENG_MEMORY_SYSTEM        MKENGDESC(ENG_CLASS_MEMORY_SYSTEM,        0)
#define ENG_KERNEL_MEMORY_SYSTEM MKENGDESC(ENG_CLASS_KERNEL_MEMORY_SYSTEM, 0)
#define ENG_MEMORY_MANAGER       MKENGDESC(ENG_CLASS_MEMORY_MANAGER,       0)
#define ENG_FBFLCN               MKENGDESC(ENG_CLASS_FBFLCN,               0)
#define ENG_TMR                  MKENGDESC(ENG_CLASS_TMR,                  0)
#define ENG_DMA                  MKENGDESC(ENG_CLASS_DMA,                  0)
#define ENG_KERNEL_FIFO          MKENGDESC(ENG_CLASS_KERNEL_FIFO,          0)
#define ENG_FIFO                 MKENGDESC(ENG_CLASS_FIFO,                 0)
#define ENG_OS                   MKENGDESC(ENG_CLASS_OS,                   0)
#define ENG_BUS                  MKENGDESC(ENG_CLASS_BUS,                  0)
#define ENG_KERNEL_BUS           MKENGDESC(ENG_CLASS_KERNEL_BUS,           0)
#define ENG_INFOROM              MKENGDESC(ENG_CLASS_INFOROM,              0)
#define ENG_PERF                 MKENGDESC(ENG_CLASS_PERF,                 0)
#define ENG_KERNEL_PERF          MKENGDESC(ENG_CLASS_KERNEL_PERF,          0)
#define ENG_PXUC                 MKENGDESC(ENG_CLASS_PXUC,                 0)
#define ENG_BIF                  MKENGDESC(ENG_CLASS_BIF,                  0)
#define ENG_KERNEL_BIF           MKENGDESC(ENG_CLASS_KERNEL_BIF,           0)
#define ENG_HSHUBMANAGER         MKENGDESC(ENG_CLASS_HSHUBMANAGER,         0)
#define ENG_SF                   MKENGDESC(ENG_CLASS_SF,                   0)
#define ENG_GPIO                 MKENGDESC(ENG_CLASS_GPIO,                 0)
#define ENG_CLK                  MKENGDESC(ENG_CLASS_CLK,                  0)
#define ENG_KERNEL_DISPLAY       MKENGDESC(ENG_CLASS_KERNEL_DISPLAY,       0)
#define ENG_DISP                 MKENGDESC(ENG_CLASS_DISP,                 0)
#define ENG_DPU                  MKENGDESC(ENG_CLASS_DPU,                  0)
#define ENG_DIP                  MKENGDESC(ENG_CLASS_DIP,                  0)
#define ENG_FAN                  MKENGDESC(ENG_CLASS_FAN,                  0)
#define ENG_INST                 MKENGDESC(ENG_CLASS_INST,                 0)
#define ENG_KERNEL_HEAD          MKENGDESC(ENG_CLASS_KERNEL_HEAD,          0)
#define ENG_VOLT                 MKENGDESC(ENG_CLASS_VOLT,                 0)
#define ENG_INTR                 MKENGDESC(ENG_CLASS_INTR,                 0)
#define ENG_HDA                  MKENGDESC(ENG_CLASS_HDA,                  0)
#define ENG_I2C                  MKENGDESC(ENG_CLASS_I2C,                  0)
#define ENG_KERNEL_RC            MKENGDESC(ENG_CLASS_KERNEL_RC,            0)
#define ENG_RC                   MKENGDESC(ENG_CLASS_RC,                   0)
#define ENG_SOR                  MKENGDESC(ENG_CLASS_SOR,                  0)
#define ENG_DAC                  MKENGDESC(ENG_CLASS_DAC,                  0)
#define ENG_PIOR                 MKENGDESC(ENG_CLASS_PIOR,                 0)
#define ENG_HEAD                 MKENGDESC(ENG_CLASS_HEAD,                 0)
#define ENG_VGA                  MKENGDESC(ENG_CLASS_VGA,                  0)
#define ENG_STEREO               MKENGDESC(ENG_CLASS_STEREO,               0)
#define ENG_OR                   MKENGDESC(ENG_CLASS_OR,                   0)
#define ENG_CIPHER               MKENGDESC(ENG_CLASS_CIPHER,               0)
#define ENG_FUSE                 MKENGDESC(ENG_CLASS_FUSE,                 0)
#define ENG_JTAG                 MKENGDESC(ENG_CLASS_JTAG,                 0)
#define ENG_HDCP                 MKENGDESC(ENG_CLASS_HDCP,                 0)
#define ENG_HDMI                 MKENGDESC(ENG_CLASS_HDMI,                 0)
#define ENG_THERM                MKENGDESC(ENG_CLASS_THERM,                0)
#define ENG_SEQ                  MKENGDESC(ENG_CLASS_SEQ,                  0)
#define ENG_DPAUX                MKENGDESC(ENG_CLASS_DPAUX,                0)
#define ENG_PMU                  MKENGDESC(ENG_CLASS_PMU,                  0)
#define ENG_KERNEL_PMU           MKENGDESC(ENG_CLASS_KERNEL_PMU,           0)
#define ENG_LPWR                 MKENGDESC(ENG_CLASS_LPWR,            0)
#define ENG_ISOHUB               MKENGDESC(ENG_CLASS_ISOHUB,          0)
#define ENG_PMGR                 MKENGDESC(ENG_CLASS_PMGR,            0)
#define ENG_HDACODEC             MKENGDESC(ENG_CLASS_HDACODEC,        0)
#define ENG_SPI                  MKENGDESC(ENG_CLASS_SPI,             0)
#define ENG_UVM                  MKENGDESC(ENG_CLASS_UVM,             0)
#define ENG_SEC2                 MKENGDESC(ENG_CLASS_SEC2,            0)
#define ENG_PMS                  MKENGDESC(ENG_CLASS_PMS,             0)
#define ENG_ENGSTATE             MKENGDESC(ENG_CLASS_ENGSTATE,        0)
#define ENG_LSFM                 MKENGDESC(ENG_CLASS_LSFM,            0)
#define ENG_ACR                  MKENGDESC(ENG_CLASS_ACR,             0)
#define ENG_GPULOG               MKENGDESC(ENG_CLASS_GPULOG,          0)
#define ENG_NVLINK               MKENGDESC(ENG_CLASS_NVLINK,          0)
#define ENG_HWPM                 MKENGDESC(ENG_CLASS_HWPM,            0)
#define ENG_KERNEL_HWPM          MKENGDESC(ENG_CLASS_KERNEL_HWPM,     0)
#define ENG_GPUMON               MKENGDESC(ENG_CLASS_GPUMON,          0)
#define ENG_GRIDDISPLAYLESS      MKENGDESC(ENG_CLASS_GRIDDISPLAYLESS, 0)
#define ENG_VMMU                 MKENGDESC(ENG_CLASS_VMMU,            0)
#define ENG_NVJPG                MKENGDESC(ENG_CLASS_NVJPG,           0)
#define ENG_GSP                  MKENGDESC(ENG_CLASS_GSP,             0)
#define ENG_FSP                  MKENGDESC(ENG_CLASS_FSP,             0)
#define ENG_KERNEL_FSP           MKENGDESC(ENG_CLASS_KERNEL_FSP,      0)
#define ENG_KERNEL_GSP           MKENGDESC(ENG_CLASS_KERNEL_GSP,      0)
#define ENG_KERNEL_SEC2          MKENGDESC(ENG_CLASS_KERNEL_SEC2,     0)
#define ENG_DISPMACRO            MKENGDESC(ENG_CLASS_DISPMACRO,       0)
#define ENG_NNE                  MKENGDESC(ENG_CLASS_NNE,             0)
#define ENG_OOB                  MKENGDESC(ENG_CLASS_OOB,             0)
#define ENG_DSI                  MKENGDESC(ENG_CLASS_DSI,             0)
#define ENG_DCECLIENTRM          MKENGDESC(ENG_CLASS_DCECLIENTRM,     0)
#define ENG_DCB                  MKENGDESC(ENG_CLASS_DCB,             0)
#define ENG_KERNEL_NVLINK        MKENGDESC(ENG_CLASS_KERNEL_NVLINK,   0)
#define ENG_GMMU                 MKENGDESC(ENG_CLASS_GMMU,            0)
#define ENG_KERNEL_GMMU          MKENGDESC(ENG_CLASS_KERNEL_GMMU,     0)
#define ENG_CONF_COMPUTE         MKENGDESC(ENG_CLASS_CONF_COMPUTE,    0)
#define ENG_KERNEL_CCU           MKENGDESC(ENG_CLASS_KERNEL_CCU,      0)
#define ENG_SPDM                 MKENGDESC(classId(Spdm),             0)

// Indexed GSPLITE Engine Tag Reference

#define ENG_KERNEL_GSPLITE(x)           MKENGDESC(ENG_CLASS_KERNEL_GSPLITE,         x)
#define ENG_KERNEL_GSPLITE__SIZE_1      4
#define IS_KERNEL_GSPLITE(engDesc)      (ENGDESC_FIELD(engDesc, _CLASS) == classId(KernelGsplite))
#define GET_KERNEL_GSPLITE_IDX(engDesc) ENGDESC_FIELD(engDesc, _INST)

// Indexed CE engine tag reference
#define ENG_CE(x)                MKENGDESC(ENG_CLASS_CE,              x)
#define ENG_CE__SIZE_1           20
#define IS_CE(engDesc)           (ENGDESC_FIELD(engDesc, _CLASS) == classId(OBJCE))
#define GET_CE_IDX(engDesc)      ENGDESC_FIELD(engDesc, _INST)

// Indexed MSENC Engine Tag Reference
#define ENG_NVENC(x)             MKENGDESC(ENG_CLASS_NVENC,           x)
// Bug 4175886 - Use this new value for all chips once GB20X is released
#define ENG_NVENC__SIZE_1        4
#define IS_MSENC(engDesc)        (ENGDESC_FIELD(engDesc, _CLASS) == classId(OBJMSENC))
#define GET_MSENC_IDX(engDesc)   ENGDESC_FIELD(engDesc, _INST)

// Indexed BSP/NVDEC Engine Tag Reference
#define ENG_NVDEC(x)             MKENGDESC(ENG_CLASS_NVDEC,           x)
#define ENG_NVDEC__SIZE_1        8
#define IS_NVDEC(engDesc)        (ENGDESC_FIELD(engDesc, _CLASS) == classId(OBJBSP))
#define GET_NVDEC_IDX(engDesc)   ENGDESC_FIELD(engDesc, _INST)

// Indexed GR engine tag reference
#define ENG_GR(x)                MKENGDESC(ENG_CLASS_GR,              x)
#define ENG_GR__SIZE_1           8
#define IS_GR(engDesc)           (ENGDESC_FIELD(engDesc, _CLASS) == classId(Graphics))
#define GET_GR_IDX(engDesc)      ENGDESC_FIELD(engDesc, _INST)

// Indexed NVJPEG Engine Tag Reference
#define ENG_NVJPEG(x)            MKENGDESC(ENG_CLASS_NVJPEG,          x)
#define ENG_NVJPEG__SIZE_1       8
#define IS_NVJPEG(engDesc)       (ENGDESC_FIELD(engDesc, _CLASS) == classId(OBJNVJPG))
#define GET_NVJPEG_IDX(engDesc)  ENGDESC_FIELD(engDesc, _INST)

// Indexed FECS engine tag reference
#define ENG_FECS(x)              MKENGDESC(ENG_CLASS_FECS,            x)
#define ENG_FECS__SIZE_1         8
#define IS_FECS(engDesc)         (ENGDESC_FIELD(engDesc, _CLASS) == classId(FECS))
#define GET_FECS_IDX(engDesc)    ENGDESC_FIELD(engDesc, _INST)

// Indexed GPCCS engine tag reference
#define ENG_GPCCS(x)             MKENGDESC(ENG_CLASS_GPCCS,           x)
#define ENG_GPCCS__SIZE_1        8
#define IS_GPCCS(engDesc)        (ENGDESC_FIELD(engDesc, _CLASS) == classId(GPCCS))
#define GET_GPCCS_IDX(engDesc)   ENGDESC_FIELD(engDesc, _INST)

// Indexed IOCTRL engine tag reference
#define ENG_IOCTRL(x)            MKENGDESC(ENG_CLASS_IOCTRL,          x)
#define ENG_IOCTRL__SIZE_1       3
#define IS_IOCTRL(engDesc)       (ENGDESC_FIELD(engDesc, _CLASS) == classId(Ioctrl))
#define GET_IOCTRL_IDX(engDesc)  ENGDESC_FIELD(engDesc, _INST)

// Indexed HSHUB engine tag reference
#define ENG_HSHUB(x)             MKENGDESC(ENG_CLASS_HSHUB,           x)
#define ENG_HSHUB__SIZE_1        12
#define IS_HSHUB(engDesc)        (ENGDESC_FIELD(engDesc, _CLASS) == classId(Hshub))
#define GET_HSHUB_IDX(engDesc)   ENGDESC_FIELD(engDesc, _INST)

// Indexed IOCTRL engine tag reference
#define ENG_KERNEL_IOCTRL(x)     MKENGDESC(ENG_CLASS_KERNEL_IOCTRL,   x)
#define ENG_KERNEL_IOCTRL__SIZE_1        3
#define IS_KERNEL_IOCTRL(engDesc)        (ENGDESC_FIELD(engDesc, _CLASS) == classId(KernelIoctrl))
#define GET_KERNEL_IOCTRL_IDX(engDesc)   ENGDESC_FIELD(engDesc, _INST)

// Indexed OFA engine tag reference
#define ENG_OFA(x)               MKENGDESC(ENG_CLASS_OFA,             x)
#define ENG_OFA__SIZE_1             2
#define IS_OFA(engDesc)             (ENGDESC_FIELD(engDesc, _CLASS) == classId(OBJOFA))
#define GET_OFA_IDX(engDesc)        ENGDESC_FIELD(engDesc, _INST)

#endif // _ENG_DESC_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_ENG_DESC_NVOC_H_
