
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
 * SPDX-FileCopyrightText: Copyright (c) 2013-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __nvoc_class_id_OBJINVALID
#define __nvoc_class_id_OBJINVALID 0xb33b15u
typedef struct OBJINVALID OBJINVALID;
#endif /* __nvoc_class_id_OBJINVALID */

       // classId only.  Not a real class
struct OBJSWENG;

#ifndef __nvoc_class_id_OBJSWENG
#define __nvoc_class_id_OBJSWENG 0x95a6f5u
typedef struct OBJSWENG OBJSWENG;
#endif /* __nvoc_class_id_OBJSWENG */


struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */


struct Falcon;

#ifndef __nvoc_class_id_Falcon
#define __nvoc_class_id_Falcon 0xdc5264u
typedef struct Falcon Falcon;
#endif /* __nvoc_class_id_Falcon */


struct OBJMC;

#ifndef __nvoc_class_id_OBJMC
#define __nvoc_class_id_OBJMC 0x9aad0eu
typedef struct OBJMC OBJMC;
#endif /* __nvoc_class_id_OBJMC */


struct KernelMc;

#ifndef __nvoc_class_id_KernelMc
#define __nvoc_class_id_KernelMc 0x3827ffu
typedef struct KernelMc KernelMc;
#endif /* __nvoc_class_id_KernelMc */


struct PrivRing;

#ifndef __nvoc_class_id_PrivRing
#define __nvoc_class_id_PrivRing 0x4c57c4u
typedef struct PrivRing PrivRing;
#endif /* __nvoc_class_id_PrivRing */


struct SwIntr;

#ifndef __nvoc_class_id_SwIntr
#define __nvoc_class_id_SwIntr 0x5ca633u
typedef struct SwIntr SwIntr;
#endif /* __nvoc_class_id_SwIntr */


struct MemorySystem;

#ifndef __nvoc_class_id_MemorySystem
#define __nvoc_class_id_MemorySystem 0x174e21u
typedef struct MemorySystem MemorySystem;
#endif /* __nvoc_class_id_MemorySystem */


struct KernelMemorySystem;

#ifndef __nvoc_class_id_KernelMemorySystem
#define __nvoc_class_id_KernelMemorySystem 0x7faff1u
typedef struct KernelMemorySystem KernelMemorySystem;
#endif /* __nvoc_class_id_KernelMemorySystem */


struct MemoryManager;

#ifndef __nvoc_class_id_MemoryManager
#define __nvoc_class_id_MemoryManager 0x22ad47u
typedef struct MemoryManager MemoryManager;
#endif /* __nvoc_class_id_MemoryManager */


struct OBJFBFLCN;

#ifndef __nvoc_class_id_OBJFBFLCN
#define __nvoc_class_id_OBJFBFLCN 0x8a20bfu
typedef struct OBJFBFLCN OBJFBFLCN;
#endif /* __nvoc_class_id_OBJFBFLCN */


struct OBJHSHUBMANAGER;

#ifndef __nvoc_class_id_OBJHSHUBMANAGER
#define __nvoc_class_id_OBJHSHUBMANAGER 0xbb32b9u
typedef struct OBJHSHUBMANAGER OBJHSHUBMANAGER;
#endif /* __nvoc_class_id_OBJHSHUBMANAGER */


struct Hshub;

#ifndef __nvoc_class_id_Hshub
#define __nvoc_class_id_Hshub 0x5b3331u
typedef struct Hshub Hshub;
#endif /* __nvoc_class_id_Hshub */


struct OBJTMR;

#ifndef __nvoc_class_id_OBJTMR
#define __nvoc_class_id_OBJTMR 0x9ddedeu
typedef struct OBJTMR OBJTMR;
#endif /* __nvoc_class_id_OBJTMR */


struct VirtMemAllocator;

#ifndef __nvoc_class_id_VirtMemAllocator
#define __nvoc_class_id_VirtMemAllocator 0x899e48u
typedef struct VirtMemAllocator VirtMemAllocator;
#endif /* __nvoc_class_id_VirtMemAllocator */


struct Graphics;

#ifndef __nvoc_class_id_Graphics
#define __nvoc_class_id_Graphics 0xd334dfu
typedef struct Graphics Graphics;
#endif /* __nvoc_class_id_Graphics */


struct OBJGR;

#ifndef __nvoc_class_id_OBJGR
#define __nvoc_class_id_OBJGR 0xb0940au
typedef struct OBJGR OBJGR;
#endif /* __nvoc_class_id_OBJGR */

            // classId only. Not a real class. Bug 200664045
struct GraphicsManager;

#ifndef __nvoc_class_id_GraphicsManager
#define __nvoc_class_id_GraphicsManager 0x2f465au
typedef struct GraphicsManager GraphicsManager;
#endif /* __nvoc_class_id_GraphicsManager */


struct KernelGraphicsManager;

#ifndef __nvoc_class_id_KernelGraphicsManager
#define __nvoc_class_id_KernelGraphicsManager 0xd22179u
typedef struct KernelGraphicsManager KernelGraphicsManager;
#endif /* __nvoc_class_id_KernelGraphicsManager */


struct MIGManager;

#ifndef __nvoc_class_id_MIGManager
#define __nvoc_class_id_MIGManager 0xfd75d0u
typedef struct MIGManager MIGManager;
#endif /* __nvoc_class_id_MIGManager */


struct KernelMIGManager;

#ifndef __nvoc_class_id_KernelMIGManager
#define __nvoc_class_id_KernelMIGManager 0x01c1bfu
typedef struct KernelMIGManager KernelMIGManager;
#endif /* __nvoc_class_id_KernelMIGManager */


struct SMDebugger;

#ifndef __nvoc_class_id_SMDebugger
#define __nvoc_class_id_SMDebugger 0x12018bu
typedef struct SMDebugger SMDebugger;
#endif /* __nvoc_class_id_SMDebugger */


struct KernelGraphics;

#ifndef __nvoc_class_id_KernelGraphics
#define __nvoc_class_id_KernelGraphics 0xea3fa9u
typedef struct KernelGraphics KernelGraphics;
#endif /* __nvoc_class_id_KernelGraphics */


struct KernelFifo;

#ifndef __nvoc_class_id_KernelFifo
#define __nvoc_class_id_KernelFifo 0xf3e155u
typedef struct KernelFifo KernelFifo;
#endif /* __nvoc_class_id_KernelFifo */


struct OBJFIFO;

#ifndef __nvoc_class_id_OBJFIFO
#define __nvoc_class_id_OBJFIFO 0xb02365u
typedef struct OBJFIFO OBJFIFO;
#endif /* __nvoc_class_id_OBJFIFO */


struct OBJOS;

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70u
typedef struct OBJOS OBJOS;
#endif /* __nvoc_class_id_OBJOS */


struct OBJBUS;

#ifndef __nvoc_class_id_OBJBUS
#define __nvoc_class_id_OBJBUS 0xcc4c31u
typedef struct OBJBUS OBJBUS;
#endif /* __nvoc_class_id_OBJBUS */


struct KernelBus;

#ifndef __nvoc_class_id_KernelBus
#define __nvoc_class_id_KernelBus 0xd2ac57u
typedef struct KernelBus KernelBus;
#endif /* __nvoc_class_id_KernelBus */


struct OBJINFOROM;

#ifndef __nvoc_class_id_OBJINFOROM
#define __nvoc_class_id_OBJINFOROM 0x0e1639u
typedef struct OBJINFOROM OBJINFOROM;
#endif /* __nvoc_class_id_OBJINFOROM */


struct Perf;

#ifndef __nvoc_class_id_Perf
#define __nvoc_class_id_Perf 0xed0b65u
typedef struct Perf Perf;
#endif /* __nvoc_class_id_Perf */


struct KernelPerf;

#ifndef __nvoc_class_id_KernelPerf
#define __nvoc_class_id_KernelPerf 0xc53a57u
typedef struct KernelPerf KernelPerf;
#endif /* __nvoc_class_id_KernelPerf */


struct Pxuc;

#ifndef __nvoc_class_id_Pxuc
#define __nvoc_class_id_Pxuc 0xba06f5u
typedef struct Pxuc Pxuc;
#endif /* __nvoc_class_id_Pxuc */


struct OBJBIF;

#ifndef __nvoc_class_id_OBJBIF
#define __nvoc_class_id_OBJBIF 0xd1c956u
typedef struct OBJBIF OBJBIF;
#endif /* __nvoc_class_id_OBJBIF */


struct KernelBif;

#ifndef __nvoc_class_id_KernelBif
#define __nvoc_class_id_KernelBif 0xdbe523u
typedef struct KernelBif KernelBif;
#endif /* __nvoc_class_id_KernelBif */


struct OBJSF;

#ifndef __nvoc_class_id_OBJSF
#define __nvoc_class_id_OBJSF 0x0bd720u
typedef struct OBJSF OBJSF;
#endif /* __nvoc_class_id_OBJSF */


struct OBJGPIO;

#ifndef __nvoc_class_id_OBJGPIO
#define __nvoc_class_id_OBJGPIO 0x05c7b5u
typedef struct OBJGPIO OBJGPIO;
#endif /* __nvoc_class_id_OBJGPIO */


struct ClockManager;

#ifndef __nvoc_class_id_ClockManager
#define __nvoc_class_id_ClockManager 0xbcadd3u
typedef struct ClockManager ClockManager;
#endif /* __nvoc_class_id_ClockManager */


struct KernelDisplay;

#ifndef __nvoc_class_id_KernelDisplay
#define __nvoc_class_id_KernelDisplay 0x55952eu
typedef struct KernelDisplay KernelDisplay;
#endif /* __nvoc_class_id_KernelDisplay */


struct OBJDISP;

#ifndef __nvoc_class_id_OBJDISP
#define __nvoc_class_id_OBJDISP 0xd1755eu
typedef struct OBJDISP OBJDISP;
#endif /* __nvoc_class_id_OBJDISP */


struct OBJDPU;

#ifndef __nvoc_class_id_OBJDPU
#define __nvoc_class_id_OBJDPU 0x23486du
typedef struct OBJDPU OBJDPU;
#endif /* __nvoc_class_id_OBJDPU */


struct OBJDIP;

#ifndef __nvoc_class_id_OBJDIP
#define __nvoc_class_id_OBJDIP 0x1cc271u
typedef struct OBJDIP OBJDIP;
#endif /* __nvoc_class_id_OBJDIP */


struct Fan;

#ifndef __nvoc_class_id_Fan
#define __nvoc_class_id_Fan 0xadd018u
typedef struct Fan Fan;
#endif /* __nvoc_class_id_Fan */


struct DisplayInstanceMemory;

#ifndef __nvoc_class_id_DisplayInstanceMemory
#define __nvoc_class_id_DisplayInstanceMemory 0x8223e2u
typedef struct DisplayInstanceMemory DisplayInstanceMemory;
#endif /* __nvoc_class_id_DisplayInstanceMemory */


struct KernelHead;

#ifndef __nvoc_class_id_KernelHead
#define __nvoc_class_id_KernelHead 0x0145e6u
typedef struct KernelHead KernelHead;
#endif /* __nvoc_class_id_KernelHead */


struct OBJVOLT;

#ifndef __nvoc_class_id_OBJVOLT
#define __nvoc_class_id_OBJVOLT 0xa68120u
typedef struct OBJVOLT OBJVOLT;
#endif /* __nvoc_class_id_OBJVOLT */


struct Intr;

#ifndef __nvoc_class_id_Intr
#define __nvoc_class_id_Intr 0xc06e44u
typedef struct Intr Intr;
#endif /* __nvoc_class_id_Intr */


struct OBJHDA;

#ifndef __nvoc_class_id_OBJHDA
#define __nvoc_class_id_OBJHDA 0xd3bfb4u
typedef struct OBJHDA OBJHDA;
#endif /* __nvoc_class_id_OBJHDA */


struct I2c;

#ifndef __nvoc_class_id_I2c
#define __nvoc_class_id_I2c 0x48e035u
typedef struct I2c I2c;
#endif /* __nvoc_class_id_I2c */


struct KernelRc;

#ifndef __nvoc_class_id_KernelRc
#define __nvoc_class_id_KernelRc 0x4888dbu
typedef struct KernelRc KernelRc;
#endif /* __nvoc_class_id_KernelRc */


struct OBJRC;

#ifndef __nvoc_class_id_OBJRC
#define __nvoc_class_id_OBJRC 0x42d150u
typedef struct OBJRC OBJRC;
#endif /* __nvoc_class_id_OBJRC */


struct OBJSOR;

#ifndef __nvoc_class_id_OBJSOR
#define __nvoc_class_id_OBJSOR 0x5ccbfau
typedef struct OBJSOR OBJSOR;
#endif /* __nvoc_class_id_OBJSOR */


struct OBJDAC;

#ifndef __nvoc_class_id_OBJDAC
#define __nvoc_class_id_OBJDAC 0x4b1802u
typedef struct OBJDAC OBJDAC;
#endif /* __nvoc_class_id_OBJDAC */


struct OBJPIOR;

#ifndef __nvoc_class_id_OBJPIOR
#define __nvoc_class_id_OBJPIOR 0x0128a3u
typedef struct OBJPIOR OBJPIOR;
#endif /* __nvoc_class_id_OBJPIOR */


struct OBJHEAD;

#ifndef __nvoc_class_id_OBJHEAD
#define __nvoc_class_id_OBJHEAD 0x74dd86u
typedef struct OBJHEAD OBJHEAD;
#endif /* __nvoc_class_id_OBJHEAD */


struct OBJVGA;

#ifndef __nvoc_class_id_OBJVGA
#define __nvoc_class_id_OBJVGA 0x84e0bcu
typedef struct OBJVGA OBJVGA;
#endif /* __nvoc_class_id_OBJVGA */


struct Stereo;

#ifndef __nvoc_class_id_Stereo
#define __nvoc_class_id_Stereo 0xbbc45du
typedef struct Stereo Stereo;
#endif /* __nvoc_class_id_Stereo */


struct OBJOR;

#ifndef __nvoc_class_id_OBJOR
#define __nvoc_class_id_OBJOR 0x215d6bu
typedef struct OBJOR OBJOR;
#endif /* __nvoc_class_id_OBJOR */


struct OBJBSP;

#ifndef __nvoc_class_id_OBJBSP
#define __nvoc_class_id_OBJBSP 0x8f99e1u
typedef struct OBJBSP OBJBSP;
#endif /* __nvoc_class_id_OBJBSP */


struct OBJCIPHER;

#ifndef __nvoc_class_id_OBJCIPHER
#define __nvoc_class_id_OBJCIPHER 0x8dd911u
typedef struct OBJCIPHER OBJCIPHER;
#endif /* __nvoc_class_id_OBJCIPHER */


struct OBJFUSE;

#ifndef __nvoc_class_id_OBJFUSE
#define __nvoc_class_id_OBJFUSE 0x95ba71u
typedef struct OBJFUSE OBJFUSE;
#endif /* __nvoc_class_id_OBJFUSE */


struct Jtag;

#ifndef __nvoc_class_id_Jtag
#define __nvoc_class_id_Jtag 0xd73cf9u
typedef struct Jtag Jtag;
#endif /* __nvoc_class_id_Jtag */


struct OBJHDCP;

#ifndef __nvoc_class_id_OBJHDCP
#define __nvoc_class_id_OBJHDCP 0x426d44u
typedef struct OBJHDCP OBJHDCP;
#endif /* __nvoc_class_id_OBJHDCP */


struct OBJHDMI;

#ifndef __nvoc_class_id_OBJHDMI
#define __nvoc_class_id_OBJHDMI 0x2213b6u
typedef struct OBJHDMI OBJHDMI;
#endif /* __nvoc_class_id_OBJHDMI */


struct Therm;

#ifndef __nvoc_class_id_Therm
#define __nvoc_class_id_Therm 0x6c1e56u
typedef struct Therm Therm;
#endif /* __nvoc_class_id_Therm */


struct OBJSEQ;

#ifndef __nvoc_class_id_OBJSEQ
#define __nvoc_class_id_OBJSEQ 0x45da4au
typedef struct OBJSEQ OBJSEQ;
#endif /* __nvoc_class_id_OBJSEQ */


struct OBJDPAUX;

#ifndef __nvoc_class_id_OBJDPAUX
#define __nvoc_class_id_OBJDPAUX 0xfd2ab9u
typedef struct OBJDPAUX OBJDPAUX;
#endif /* __nvoc_class_id_OBJDPAUX */


struct Pmu;

#ifndef __nvoc_class_id_Pmu
#define __nvoc_class_id_Pmu 0xf3d722u
typedef struct Pmu Pmu;
#endif /* __nvoc_class_id_Pmu */


struct KernelPmu;

#ifndef __nvoc_class_id_KernelPmu
#define __nvoc_class_id_KernelPmu 0xab9d7du
typedef struct KernelPmu KernelPmu;
#endif /* __nvoc_class_id_KernelPmu */


struct GCX;

#ifndef __nvoc_class_id_GCX
#define __nvoc_class_id_GCX 0x4507c4u
typedef struct GCX GCX;
#endif /* __nvoc_class_id_GCX */


struct Lpwr;

#ifndef __nvoc_class_id_Lpwr
#define __nvoc_class_id_Lpwr 0x112230u
typedef struct Lpwr Lpwr;
#endif /* __nvoc_class_id_Lpwr */


struct Lpsec;

#ifndef __nvoc_class_id_Lpsec
#define __nvoc_class_id_Lpsec 0x3848adu
typedef struct Lpsec Lpsec;
#endif /* __nvoc_class_id_Lpsec */


struct OBJISOHUB;

#ifndef __nvoc_class_id_OBJISOHUB
#define __nvoc_class_id_OBJISOHUB 0x7c5e0du
typedef struct OBJISOHUB OBJISOHUB;
#endif /* __nvoc_class_id_OBJISOHUB */


struct Pmgr;

#ifndef __nvoc_class_id_Pmgr
#define __nvoc_class_id_Pmgr 0x894574u
typedef struct Pmgr Pmgr;
#endif /* __nvoc_class_id_Pmgr */


struct OBJHDACODEC;

#ifndef __nvoc_class_id_OBJHDACODEC
#define __nvoc_class_id_OBJHDACODEC 0xa576e2u
typedef struct OBJHDACODEC OBJHDACODEC;
#endif /* __nvoc_class_id_OBJHDACODEC */


struct Spi;

#ifndef __nvoc_class_id_Spi
#define __nvoc_class_id_Spi 0x824313u
typedef struct Spi Spi;
#endif /* __nvoc_class_id_Spi */


struct OBJUVM;

#ifndef __nvoc_class_id_OBJUVM
#define __nvoc_class_id_OBJUVM 0xf9a17du
typedef struct OBJUVM OBJUVM;
#endif /* __nvoc_class_id_OBJUVM */


struct OBJSEC2;

#ifndef __nvoc_class_id_OBJSEC2
#define __nvoc_class_id_OBJSEC2 0x28c408u
typedef struct OBJSEC2 OBJSEC2;
#endif /* __nvoc_class_id_OBJSEC2 */


struct OBJPMS;

#ifndef __nvoc_class_id_OBJPMS
#define __nvoc_class_id_OBJPMS 0x9e3810u
typedef struct OBJPMS OBJPMS;
#endif /* __nvoc_class_id_OBJPMS */


struct OBJENGSTATE;

#ifndef __nvoc_class_id_OBJENGSTATE
#define __nvoc_class_id_OBJENGSTATE 0x7a7ed6u
typedef struct OBJENGSTATE OBJENGSTATE;
#endif /* __nvoc_class_id_OBJENGSTATE */


struct OBJLSFM;

#ifndef __nvoc_class_id_OBJLSFM
#define __nvoc_class_id_OBJLSFM 0x9a25e4u
typedef struct OBJLSFM OBJLSFM;
#endif /* __nvoc_class_id_OBJLSFM */


struct OBJACR;

#ifndef __nvoc_class_id_OBJACR
#define __nvoc_class_id_OBJACR 0xdb32a1u
typedef struct OBJACR OBJACR;
#endif /* __nvoc_class_id_OBJACR */


struct OBJGPULOG;

#ifndef __nvoc_class_id_OBJGPULOG
#define __nvoc_class_id_OBJGPULOG 0xdd19beu
typedef struct OBJGPULOG OBJGPULOG;
#endif /* __nvoc_class_id_OBJGPULOG */


struct KernelNvlink;

#ifndef __nvoc_class_id_KernelNvlink
#define __nvoc_class_id_KernelNvlink 0xce6818u
typedef struct KernelNvlink KernelNvlink;
#endif /* __nvoc_class_id_KernelNvlink */


struct Nvlink;

#ifndef __nvoc_class_id_Nvlink
#define __nvoc_class_id_Nvlink 0x790a3cu
typedef struct Nvlink Nvlink;
#endif /* __nvoc_class_id_Nvlink */


struct KernelHwpm;

#ifndef __nvoc_class_id_KernelHwpm
#define __nvoc_class_id_KernelHwpm 0xc8c00fu
typedef struct KernelHwpm KernelHwpm;
#endif /* __nvoc_class_id_KernelHwpm */


struct OBJHWPM;

#ifndef __nvoc_class_id_OBJHWPM
#define __nvoc_class_id_OBJHWPM 0x97e43bu
typedef struct OBJHWPM OBJHWPM;
#endif /* __nvoc_class_id_OBJHWPM */


struct OBJGPUMON;

#ifndef __nvoc_class_id_OBJGPUMON
#define __nvoc_class_id_OBJGPUMON 0x2b424bu
typedef struct OBJGPUMON OBJGPUMON;
#endif /* __nvoc_class_id_OBJGPUMON */


struct OBJGRIDDISPLAYLESS;

#ifndef __nvoc_class_id_OBJGRIDDISPLAYLESS
#define __nvoc_class_id_OBJGRIDDISPLAYLESS 0x20fd5au
typedef struct OBJGRIDDISPLAYLESS OBJGRIDDISPLAYLESS;
#endif /* __nvoc_class_id_OBJGRIDDISPLAYLESS */


struct FECS;

#ifndef __nvoc_class_id_FECS
#define __nvoc_class_id_FECS 0x5ee8dcu
typedef struct FECS FECS;
#endif /* __nvoc_class_id_FECS */


struct GPCCS;

#ifndef __nvoc_class_id_GPCCS
#define __nvoc_class_id_GPCCS 0x4781e8u
typedef struct GPCCS GPCCS;
#endif /* __nvoc_class_id_GPCCS */


struct OBJCE;

#ifndef __nvoc_class_id_OBJCE
#define __nvoc_class_id_OBJCE 0x793cebu
typedef struct OBJCE OBJCE;
#endif /* __nvoc_class_id_OBJCE */


struct KernelCE;

#ifndef __nvoc_class_id_KernelCE
#define __nvoc_class_id_KernelCE 0x242acau
typedef struct KernelCE KernelCE;
#endif /* __nvoc_class_id_KernelCE */


struct OBJMSENC;

#ifndef __nvoc_class_id_OBJMSENC
#define __nvoc_class_id_OBJMSENC 0xe97b6cu
typedef struct OBJMSENC OBJMSENC;
#endif /* __nvoc_class_id_OBJMSENC */


struct OBJNVJPG;

#ifndef __nvoc_class_id_OBJNVJPG
#define __nvoc_class_id_OBJNVJPG 0x2b3a54u
typedef struct OBJNVJPG OBJNVJPG;
#endif /* __nvoc_class_id_OBJNVJPG */


struct OBJVMMU;

#ifndef __nvoc_class_id_OBJVMMU
#define __nvoc_class_id_OBJVMMU 0xdf8918u
typedef struct OBJVMMU OBJVMMU;
#endif /* __nvoc_class_id_OBJVMMU */


struct Gsp;

#ifndef __nvoc_class_id_Gsp
#define __nvoc_class_id_Gsp 0xda3de4u
typedef struct Gsp Gsp;
#endif /* __nvoc_class_id_Gsp */


struct OBJFSP;

#ifndef __nvoc_class_id_OBJFSP
#define __nvoc_class_id_OBJFSP 0xd39158u
typedef struct OBJFSP OBJFSP;
#endif /* __nvoc_class_id_OBJFSP */


struct KernelFsp;

#ifndef __nvoc_class_id_KernelFsp
#define __nvoc_class_id_KernelFsp 0x87fb96u
typedef struct KernelFsp KernelFsp;
#endif /* __nvoc_class_id_KernelFsp */


struct OBJOFA;

#ifndef __nvoc_class_id_OBJOFA
#define __nvoc_class_id_OBJOFA 0xdd7babu
typedef struct OBJOFA OBJOFA;
#endif /* __nvoc_class_id_OBJOFA */


struct KernelIoctrl;

#ifndef __nvoc_class_id_KernelIoctrl
#define __nvoc_class_id_KernelIoctrl 0x880c7du
typedef struct KernelIoctrl KernelIoctrl;
#endif /* __nvoc_class_id_KernelIoctrl */


struct Ioctrl;

#ifndef __nvoc_class_id_Ioctrl
#define __nvoc_class_id_Ioctrl 0x11ce10u
typedef struct Ioctrl Ioctrl;
#endif /* __nvoc_class_id_Ioctrl */


struct KernelSec2;

#ifndef __nvoc_class_id_KernelSec2
#define __nvoc_class_id_KernelSec2 0x2f36c9u
typedef struct KernelSec2 KernelSec2;
#endif /* __nvoc_class_id_KernelSec2 */


struct KernelGsp;

#ifndef __nvoc_class_id_KernelGsp
#define __nvoc_class_id_KernelGsp 0x311d4eu
typedef struct KernelGsp KernelGsp;
#endif /* __nvoc_class_id_KernelGsp */


struct OBJDCECLIENTRM;

#ifndef __nvoc_class_id_OBJDCECLIENTRM
#define __nvoc_class_id_OBJDCECLIENTRM 0x61649cu
typedef struct OBJDCECLIENTRM OBJDCECLIENTRM;
#endif /* __nvoc_class_id_OBJDCECLIENTRM */


struct OBJDISPMACRO;

#ifndef __nvoc_class_id_OBJDISPMACRO
#define __nvoc_class_id_OBJDISPMACRO 0xa1cad2u
typedef struct OBJDISPMACRO OBJDISPMACRO;
#endif /* __nvoc_class_id_OBJDISPMACRO */


struct Nne;

#ifndef __nvoc_class_id_Nne
#define __nvoc_class_id_Nne 0x2487e2u
typedef struct Nne Nne;
#endif /* __nvoc_class_id_Nne */


struct Oob;

#ifndef __nvoc_class_id_Oob
#define __nvoc_class_id_Oob 0x98b919u
typedef struct Oob Oob;
#endif /* __nvoc_class_id_Oob */


struct OBJDSI;

#ifndef __nvoc_class_id_OBJDSI
#define __nvoc_class_id_OBJDSI 0x2e9a64u
typedef struct OBJDSI OBJDSI;
#endif /* __nvoc_class_id_OBJDSI */


struct OBJDCB;

#ifndef __nvoc_class_id_OBJDCB
#define __nvoc_class_id_OBJDCB 0xf931d4u
typedef struct OBJDCB OBJDCB;
#endif /* __nvoc_class_id_OBJDCB */


struct KernelGmmu;

#ifndef __nvoc_class_id_KernelGmmu
#define __nvoc_class_id_KernelGmmu 0x29362fu
typedef struct KernelGmmu KernelGmmu;
#endif /* __nvoc_class_id_KernelGmmu */


struct OBJGMMU;

#ifndef __nvoc_class_id_OBJGMMU
#define __nvoc_class_id_OBJGMMU 0xd7a41du
typedef struct OBJGMMU OBJGMMU;
#endif /* __nvoc_class_id_OBJGMMU */


struct ConfidentialCompute;

#ifndef __nvoc_class_id_ConfidentialCompute
#define __nvoc_class_id_ConfidentialCompute 0x9798ccu
typedef struct ConfidentialCompute ConfidentialCompute;
#endif /* __nvoc_class_id_ConfidentialCompute */


struct KernelCcu;

#ifndef __nvoc_class_id_KernelCcu
#define __nvoc_class_id_KernelCcu 0x5d5b68u
typedef struct KernelCcu KernelCcu;
#endif /* __nvoc_class_id_KernelCcu */


struct KernelGsplite;

#ifndef __nvoc_class_id_KernelGsplite
#define __nvoc_class_id_KernelGsplite 0x927969u
typedef struct KernelGsplite KernelGsplite;
#endif /* __nvoc_class_id_KernelGsplite */


struct KernelHFRP;

#ifndef __nvoc_class_id_KernelHFRP
#define __nvoc_class_id_KernelHFRP 0xa9fc13u
typedef struct KernelHFRP KernelHFRP;
#endif /* __nvoc_class_id_KernelHFRP */


struct Spdm;

#ifndef __nvoc_class_id_Spdm
#define __nvoc_class_id_Spdm 0x261ee8u
typedef struct Spdm Spdm;
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
#define ENG_CLASS_GCX                  classId(GCX)
#define ENG_CLASS_LPWR                 classId(Lpwr)
#define ENG_CLASS_LPSEC                classId(Lpsec)
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
#define ENG_CLASS_KERNEL_HFRP          classId(KernelHFRP)


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
#define ENG_GCX                  MKENGDESC(ENG_CLASS_GCX,                  0)
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

// Indexed LPSEC Engine Tag Reference

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

#define ENG_KERNEL_HFRP          MKENGDESC(ENG_CLASS_KERNEL_HFRP,           0)

#endif // _ENG_DESC_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_ENG_DESC_NVOC_H_
