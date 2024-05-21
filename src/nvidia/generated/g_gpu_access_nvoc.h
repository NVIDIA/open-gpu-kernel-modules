
#ifndef _G_GPU_ACCESS_NVOC_H_
#define _G_GPU_ACCESS_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2004-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_access_nvoc.h"

#ifndef _GPU_ACCESS_H_
#define _GPU_ACCESS_H_

#include "core/core.h"
#include "ioaccess/ioaccess.h"
#include "gpu/gpu_device_mapping.h"

// Go straight at the memory or hardware.
#define MEM_RD08(a) (*(const volatile NvU8  *)(a))
#define MEM_RD16(a) (*(const volatile NvU16 *)(a))
#define MEM_RD32(a) (*(const volatile NvU32 *)(a))
#define MEM_WR08(a, d) do { *(volatile NvU8  *)(a) = (d); } while (0)
#define MEM_WR16(a, d) do { *(volatile NvU16 *)(a) = (d); } while (0)
#define MEM_WR32(a, d) do { *(volatile NvU32 *)(a) = (d); } while (0)
#define MEM_WR64(a, d) do { *(volatile NvU64 *)(a) = (d); } while (0)

//
// Define the signature of the register filter callback function
//
// flags can be optionally used for filters to decide whether to actually
// touch HW or not. flags should be OR'ed every time a new filter is found. (see objgpu.c)
//
typedef void (*GpuWriteRegCallback)(OBJGPU *, void *, NvU32 addr, NvU32 val, NvU32 accessSize, NvU32 flags);
typedef NvU32 (*GpuReadRegCallback)(OBJGPU *, void *, NvU32 addr, NvU32 accessSize, NvU32 flags);

union GPUHWREG
{
    volatile NvV8 Reg008[1];
    volatile NvV16 Reg016[1];
    volatile NvV32 Reg032[1];
};

typedef union  GPUHWREG  GPUHWREG;

//
// Register filter record
//
// If REGISTER_FILTER_FLAGS_READ is set, then that means that the base RegRead
// function will not read the register, so the provided read callback function
// is expected to read the register and return the value.
//
// If REGISTER_FILTER_FLAGS_WRITE is set, then that means that the base RegWrite
// function will not write the register, so the provided callback write function
// is expected to write the given value to the register.
//
// It is an error to specify REGISTER_FILTER_FLAGS_READ and not provide a
// read callback function.
//
// It is an error to specify REGISTER_FILTER_FLAGS_WRITE and not provide a
// write callback function.
//
#define REGISTER_FILTER_FLAGS_READ         (NVBIT(0))
#define REGISTER_FILTER_FLAGS_WRITE        (NVBIT(1))
// filter is in the list but it is invalid and should be removed
#define REGISTER_FILTER_FLAGS_INVALID      (NVBIT(2))

#define REGISTER_FILTER_FLAGS_VIRTUAL      (0)
#define REGISTER_FILTER_FLAGS_READ_WRITE   (REGISTER_FILTER_FLAGS_READ | REGISTER_FILTER_FLAGS_WRITE)

// Do not warn if attempting to add a filter on GSP [CORERM-5356]
#define REGISTER_FILTER_FLAGS_NO_GSP_WARNING (NVBIT(3))

typedef struct REGISTER_FILTER REGISTER_FILTER;

struct REGISTER_FILTER
{
    REGISTER_FILTER            *pNext;           //!< pointer to next filter
    NvU32                       flags;           //!< attributes of this filter
    DEVICE_INDEX                devIndex;        //!< filter device
    NvU32                       devInstance;     //!< filter device instance
    NvU32                       rangeStart;      //!< filter range start (can overlap)
    NvU32                       rangeEnd;        //!< filter range end   (can overlap)
    GpuWriteRegCallback         pWriteCallback;  //!< callback for write
    GpuReadRegCallback          pReadCallback;   //!< callback for read
    void                       *pParam;          //!< pointer to param which gets passed to callbacks
};

typedef struct {
    REGISTER_FILTER        *pRegFilterList;         // Active filters
    REGISTER_FILTER        *pRegFilterRecycleList;  // Inactive filters
    PORT_SPINLOCK *         pRegFilterLock;         // Thread-safe list management
    NvU32                   regFilterRefCnt;        // Thread-safe list management
    NvBool                  bRegFilterNeedRemove;   // Thread-safe list garbage collection
} DEVICE_REGFILTER_INFO;

typedef struct DEVICE_MAPPING
{
    GPUHWREG             *gpuNvAddr;        // CPU Virtual Address
    RmPhysAddr            gpuNvPAddr;       // Physical Base Address
    NvU32                 gpuNvLength;      // Length of the Aperture
    NvU32                 gpuNvSaveLength;
    NvU32                 gpuDeviceEnum;    // Device ID NV_DEVID_*
    NvU32                 refCount;         // refCount for the device map.
    DEVICE_REGFILTER_INFO devRegFilterInfo; // register filter range list
} DEVICE_MAPPING;

typedef struct
{
    // Pointer to GPU linked to this RegisterAccess object
    OBJGPU       *pGpu;

    // HW register access tools
    GPUHWREG     *gpuFbAddr;
    GPUHWREG     *gpuInstAddr;

    // Register access profiling
    NvU32         regReadCount;
    NvU32         regWriteCount;
} RegisterAccess;

/*! Init register IO access path */
NV_STATUS regAccessConstruct(RegisterAccess *, OBJGPU *pGpu);

/*! Shutdown register IO access path */
void regAccessDestruct(RegisterAccess *);

/*! Writes to 8 bit register */
void regWrite008(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32, NvV8);

/*! Writes to 16 bit register */
void regWrite016(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32, NvV16);

/*! Writes to 32 bit register, with thread state on the stack */
void regWrite032(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32, NvV32, THREAD_STATE_NODE *);

/*! Unicast register access, with thread state on the stack */
void regWrite032Unicast(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32, NvV32, THREAD_STATE_NODE *);

/*! Reads from 8 bit register */
NvU8 regRead008(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32);

/*! Reads from 16 bit register */
NvU16 regRead016(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32);

/*! Reads from 32 bit register, with thread state on the stack */
NvU32 regRead032(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32, THREAD_STATE_NODE *);

/*! Reads from 32 bit register and checks bit mask, with thread state on the stack */
NvU32 regCheckRead032(RegisterAccess *, NvU32, NvU32, THREAD_STATE_NODE *);

/*! Reads 32 bit register and polls bit field for specific value */
NV_STATUS regRead032_AndPoll(RegisterAccess *, DEVICE_INDEX, NvU32, NvU32, NvU32);

/*! Adds a register filter */
NV_STATUS regAddRegisterFilter(RegisterAccess *, NvU32, DEVICE_INDEX, NvU32, NvU32, NvU32, GpuWriteRegCallback, GpuReadRegCallback, void *, REGISTER_FILTER **);

/*! Removes register filter */
void regRemoveRegisterFilter(RegisterAccess *, REGISTER_FILTER *);

/*! Check status of read return value for GPU/bus errors */
void regCheckAndLogReadFailure(RegisterAccess *, NvU32 addr, NvU32 mask, NvU32 value);

//
// GPU register I/O macros.
//

//
// GPU neutral macros typically used for register I/O.
//
#define GPU_DRF_SHIFT(drf)          ((0?drf) % 32)
#define GPU_DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define GPU_DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<GPU_DRF_SHIFT(NV ## d ## r ## f))
#define GPU_DRF_NUM(d,r,f,n)        (((n)&GPU_DRF_MASK(NV ## d ## r ## f))<<GPU_DRF_SHIFT(NV ## d ## r ## f))
#define GPU_DRF_VAL(d,r,f,v)        (((v)>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))
#define GPU_DRF_SHIFTMASK(drf)      (GPU_DRF_MASK(drf)<<(GPU_DRF_SHIFT(drf)))
#define GPU_DRF_WIDTH(drf)          ((1?drf) - (0?drf) + 1)


// Device independent macros
// Multiple device instance macros

#define REG_INST_RD08(g,dev,inst,a)             regRead008(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a)
#define REG_INST_RD16(g,dev,inst,a)             regRead016(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a)
#define REG_INST_RD32(g,dev,inst,a)             regRead032(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a, NULL)

#define REG_INST_WR08(g,dev,inst,a,v)     regWrite008(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a, v)
#define REG_INST_WR16(g,dev,inst,a,v)     regWrite016(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a, v)
#define REG_INST_WR32(g,dev,inst,a,v)     regWrite032(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a, v, NULL)
#define REG_INST_WR32_UC(g,dev,inst,a,v)  regWrite032Unicast(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a, v, NULL)

#define REG_INST_RD32_EX(g,dev,inst,a,t)    regRead032(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a, t)
#define REG_INST_WR32_EX(g,dev,inst,a,v,t)  regWrite032(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, a, v, t)

#define REG_INST_DEVIDX_RD32_EX(g,devidx,inst,a,t)    regRead032(GPU_GET_REGISTER_ACCESS(g), devidx, inst, a, t)
#define REG_INST_DEVIDX_WR32_EX(g,devidx,inst,a,v,t)  regWrite032(GPU_GET_REGISTER_ACCESS(g), devidx, inst, a, v, t)

// Get the address of a register given the Aperture and offset.
#define REG_GET_ADDR(ap, offset) ioaprtGetRegAddr(ap, offset)

// GPU macros defined in terms of DEV_ macros
#define GPU_REG_RD08(g,a) REG_INST_RD08(g,GPU,0,a)
#define GPU_REG_RD16(g,a) REG_INST_RD16(g,GPU,0,a)
#define GPU_REG_RD32(g,a) REG_INST_RD32(g,GPU,0,a)
#define GPU_CHECK_REG_RD32(g,a,m) regCheckRead032(GPU_GET_REGISTER_ACCESS(g),a,m,NULL)
#define GPU_REG_RD32_AND_POLL(g,r,m,v) regRead032_AndPoll(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_GPU, r, m, v)

#define GPU_REG_WR08(g,a,v)    REG_INST_WR08(g,GPU,0,a,v)
#define GPU_REG_WR16(g,a,v)    REG_INST_WR16(g,GPU,0,a,v)
#define GPU_REG_WR32(g,a,v)    REG_INST_WR32(g,GPU,0,a,v)
#define GPU_REG_WR32_UC(g,a,v) REG_INST_WR32_UC(g,GPU,0,a,v)

// GPU macros for SR-IOV
#define GPU_VREG_RD32(g, a)                GPU_REG_RD32(g, g->sriovState.virtualRegPhysOffset + a)
#define GPU_VREG_WR32(g, a, v)             GPU_REG_WR32(g, g->sriovState.virtualRegPhysOffset + a, v)
#define GPU_VREG_RD32_EX(g,a,t)            REG_INST_RD32_EX(g, GPU, 0, g->sriovState.virtualRegPhysOffset + a, t)
#define GPU_VREG_WR32_EX(g,a,v,t)          REG_INST_WR32_EX(g, GPU, 0, g->sriovState.virtualRegPhysOffset + a, v, t)
#define GPU_VREG_FLD_WR_DRF_DEF(g,d,r,f,c) GPU_VREG_WR32(g, NV##d##r,(GPU_VREG_RD32(g,NV##d##r)&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))
#define GPU_VREG_RD_DRF(g,d,r,f)        (((GPU_VREG_RD32(g, NV ## d ## r))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))

#define VREG_INST_RD32(g,dev,inst,a)            regRead032(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, g->sriovState.virtualRegPhysOffset + a, NULL)
#define VREG_INST_WR32(g,dev,inst,a,v)          regWrite032(GPU_GET_REGISTER_ACCESS(g), DEVICE_INDEX_##dev, inst, g->sriovState.virtualRegPhysOffset + a, v, NULL)
#define GPU_VREG_FLD_WR_DRF_NUM(g,d,r,f,n) VREG_INST_WR32(g,GPU,0,NV##d##r,(VREG_INST_RD32(g,GPU,0,NV##d##r)&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))

#define GPU_VREG_FLD_TEST_DRF_DEF(g,d,r,f,c) (GPU_VREG_RD_DRF(g, d, r, f) == NV##d##r##f##c)

#define GPU_GET_VREG_OFFSET(g, a) (g->sriovState.virtualRegPhysOffset + a)

#define GPU_VREG_IDX_RD_DRF(g,d,r,i,f)           (((GPU_VREG_RD32(g, NV ## d ## r(i)))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))
#define GPU_VREG_FLD_IDX_WR_DRF_DEF(g,d,r,i,f,c) GPU_VREG_WR32(g, NV##d##r(i),(GPU_VREG_RD32(g,NV##d##r(i))&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))
#define GPU_VREG_FLD_IDX_WR_DRF_NUM(g,d,r,i,f,n) GPU_VREG_WR32(g, NV##d##r(i),(GPU_VREG_RD32(g,NV##d##r(i))&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))


#define GPU_REG_RD32_EX(g,a,t)    REG_INST_RD32_EX(g,GPU,0,a,t)
#define GPU_REG_WR32_EX(g,a,v,t)  REG_INST_WR32_EX(g,GPU,0,a,v,t)

// Uncomment this to enable register access dump in gsp client
// #define GPU_REGISTER_ACCESS_DUMP    RMCFG_FEATURE_GSP_CLIENT_RM
#ifndef GPU_REGISTER_ACCESS_DUMP
#define GPU_REGISTER_ACCESS_DUMP    0
#endif

#if GPU_REGISTER_ACCESS_DUMP

NvU8  gpuRegRd08_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr);
NvU16 gpuRegRd16_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr);
NvU32 gpuRegRd32_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr);
void  gpuRegWr08_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV8 val);
void  gpuRegWr16_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV16 val);
void  gpuRegWr32_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV32 val);
void  gpuRegWr32Uc_dumpinfo(const char *func, const char *addrStr, const char *vreg, OBJGPU *pGpu, NvU32 addr, NvV32 val);

#undef GPU_REG_RD08
#undef GPU_REG_RD16
#undef GPU_REG_RD32
#undef GPU_REG_WR08
#undef GPU_REG_WR16
#undef GPU_REG_WR32
#undef GPU_REG_WR32_UC
#undef GPU_VREG_RD32
#undef GPU_VREG_WR32

#define GPU_REG_RD08(g,a)       gpuRegRd08_dumpinfo(__FUNCTION__,#a,"",g,a)
#define GPU_REG_RD16(g,a)       gpuRegRd16_dumpinfo(__FUNCTION__,#a,"",g,a)
#define GPU_REG_RD32(g,a)       gpuRegRd32_dumpinfo(__FUNCTION__,#a,"",g,a)
#define GPU_REG_WR08(g,a,v)     gpuRegWr08_dumpinfo(__FUNCTION__,#a,"",g,a,v)
#define GPU_REG_WR16(g,a,v)     gpuRegWr16_dumpinfo(__FUNCTION__,#a,"",g,a,v)
#define GPU_REG_WR32(g,a,v)     gpuRegWr32_dumpinfo(__FUNCTION__,#a,"",g,a,v)
#define GPU_REG_WR32_UC(g,a,v)  gpuRegWr32Uc_dumpinfo(__FUNCTION__,#a,"",g,a,v)
#define GPU_VREG_RD32(g, a)     gpuRegRd32_dumpinfo(__FUNCTION__,#a,"(VREG)",g, g->sriovState.virtualRegPhysOffset + a)
#define GPU_VREG_WR32(g, a, v)  gpuRegWr32_dumpinfo(__FUNCTION__,#a,"(VREG)",g, g->sriovState.virtualRegPhysOffset + a, v)

#endif // GPU_REGISTER_ACCESS_DUMP

//
// Macros for register I/O
//
#define GPU_FLD_WR_DRF_NUM(g,d,r,f,n) REG_INST_WR32(g,GPU,0,NV##d##r,(REG_INST_RD32(g,GPU,0,NV##d##r)&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))
#define GPU_FLD_WR_DRF_NUM_UC(g,d,r,f,n) GPU_REG_WR32_UC(g, NV##d##r,(GPU_REG_RD32(g,NV##d##r)&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))
#define GPU_FLD_WR_DRF_DEF(g,d,r,f,c) GPU_REG_WR32(g, NV##d##r,(GPU_REG_RD32(g,NV##d##r)&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))
#define GPU_REG_RD_DRF(g,d,r,f)       (((GPU_REG_RD32(g, NV ## d ## r))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))
#define GPU_FLD_TEST_DRF_DEF(g,d,r,f,c) (GPU_REG_RD_DRF(g, d, r, f) == NV##d##r##f##c)
#define GPU_FLD_TEST_DRF_NUM(g,d,r,f,n) (GPU_REG_RD_DRF(g, d, r, f) == n)
#define GPU_FLD_IDX_TEST_DRF_DEF(g,d,r,f,c,i) (GPU_REG_IDX_RD_DRF(g, d, r, i, f) == NV##d##r##f##c)
#define GPU_FLD_2IDX_TEST_DRF_DEF(g,d,r,f,c,i,j) (GPU_REG_2IDX_RD_DRF(g, d, r, i, j, f) == NV##d##r##f##c)

#define GPU_REG_RD_DRF_EX(g,d,r,f,t)       (((GPU_REG_RD32_EX(g, NV ## d ## r, t))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))

#define GPU_FLD_WR_DRF_NUM_EX(g,d,r,f,n,t) REG_INST_WR32_EX(g,GPU,0,NV##d##r,(REG_INST_RD32_EX(g,GPU,0,NV##d##r,t)&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n),t)

// Read/write a field or entire register of which there are several copies each accessed via an index
#define GPU_REG_IDX_WR_DRF_NUM(g,d,r,i,f,n) GPU_REG_WR32(g, NV ## d ## r(i), GPU_DRF_NUM(d,r,f,n))
#define GPU_REG_IDX_WR_DRF_DEF(g,d,r,i,f,c) GPU_REG_WR32(g, NV ## d ## r(i), GPU_DRF_DEF(d,r,f,c))
#define GPU_FLD_IDX_WR_DRF_NUM(g,d,r,i,f,n) GPU_REG_WR32(g, NV##d##r(i),(GPU_REG_RD32(g,NV##d##r(i))&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))
#define GPU_FLD_IDX_WR_DRF_DEF(g,d,r,i,f,c) GPU_REG_WR32(g, NV##d##r(i),(GPU_REG_RD32(g,NV##d##r(i))&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))
#define GPU_REG_IDX_WR_DRF_NUM_UC(g,d,r,i,f,n) GPU_REG_WR32_UC(g, NV ## d ## r(i), GPU_DRF_NUM(d,r,f,n))
#define GPU_REG_IDX_WR_DRF_DEF_UC(g,d,r,i,f,c) GPU_REG_WR32_UC(g, NV ## d ## r(i), GPU_DRF_DEF(d,r,f,c))
#define GPU_FLD_IDX_WR_DRF_DEF_UC(g,d,r,i,f,c) GPU_REG_WR32_UC(g, NV##d##r(i),(GPU_REG_RD32(g,NV##d##r(i))&~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))
#define GPU_REG_IDX_RD_DRF(g,d,r,i,f)              (((GPU_REG_RD32(g, NV ## d ## r(i)))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))
#define GPU_REG_2IDX_RD_DRF(g,d,r,i,j,f)             (((GPU_REG_RD32(g, NV ## d ## r(i, j)))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))
#define GPU_REG_RD_DRF_IDX(g,d,r,f,i)              (((GPU_REG_RD32(g, NV ## d ## r))>>GPU_DRF_SHIFT(NV ## d ## r ## f(i)))&GPU_DRF_MASK(NV ## d ## r ## f(i)))
#define GPU_REG_IDX_OFFSET_RD_DRF(g,d,r,i,o,f)     (((GPU_REG_RD32(g, NV ## d ## r(i,o)))>>GPU_DRF_SHIFT(NV ## d ## r ## f))&GPU_DRF_MASK(NV ## d ## r ## f))

//
// Macros that abstract the use of bif object to access GPU bus config registers
// This is the preferred set >= NV50
//
#define GPU_BUS_CFG_RD32(g,r,d)             gpuReadBusConfigReg_HAL(g, r, d)
#define GPU_BUS_CFG_WR32(g,r,d)             gpuWriteBusConfigReg_HAL(g, r, d)
#define GPU_BUS_CFG_FLD_WR_DRF_DEF(g,x,d,r,f,c) GPU_BUS_CFG_WR32(g, NV##d##r,(x &~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))
#define GPU_BUS_CFG_FLD_WR_DRF_NUM(g,x,d,r,f,n) GPU_BUS_CFG_WR32(g, NV##d##r,(x &~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))

#define GPU_BUS_CFG_RD32_EX(g,r,d,t)        gpuReadBusConfigRegEx_HAL(g, r, d, t)

//
// Macros that provide access to the config space of functions other than the gpu
//
#define PCI_FUNCTION_BUS_CFG_RD32(g,f,r,d)            gpuReadFunctionConfigReg_HAL(g, f, r, d)
#define PCI_FUNCTION_BUS_CFG_WR32(g,f,r,d)            gpuWriteFunctionConfigReg_HAL(g, f, r, d)
#define PCI_FUNCTION_BUS_CFG_FLD_WR_DRF_NUM(g,fn,x,d,r,f,n) gpuWriteFunctionConfigReg_HAL(g, fn, NV##d##r, (x &~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_NUM(d,r,f,n))
#define PCI_FUNCTION_BUS_CFG_FLD_WR_DRF_DEF(g,fn,x,d,r,f,c) gpuWriteFunctionConfigReg_HAL(g, fn, NV##d##r, (x &~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))

#define PCI_FUNCTION_BUS_CFG_WR32_EX(g,f,r,d,t)       gpuWriteFunctionConfigRegEx_HAL(g, f, r, d, t)

#define GPU_BUS_CFG_CYCLE_RD32(g,r,d)                  gpuReadBusConfigCycle(g, r, d)
#define GPU_BUS_CFG_CYCLE_WR32(g,r,d)                  gpuWriteBusConfigCycle(g, r, d)
#define GPU_BUS_CFG_CYCLE_FLD_WR_DRF_DEF(g,x,d,r,f,c)  gpuWriteBusConfigCycle(g, NV##d##r,(x &~(GPU_DRF_MASK(NV##d##r##f)<<GPU_DRF_SHIFT(NV##d##r##f)))|GPU_DRF_DEF(d,r,f,c))

//
// Instance memory structure access definitions.
//
// DRF macros (nvmisc.h) should be used when possible instead of these
// definitions.
//
// Key difference is SF variants take structure ## field (2-level nested
// namespace), DRF take device ## register ## field (3-level nested
// namespace).
//
// SF variants are primarily used for GPU host memory structures. DRF
// should be used for manipulation of most registers
//
#define SF_INDEX(sf)            ((0?sf)/32)
#define SF_OFFSET(sf)           (((0?sf)/32)<<2)
#define SF_SHIFT(sf)            ((0?sf)&31)
#undef  SF_MASK
#define SF_MASK(sf)             (0xFFFFFFFF>>(31-(1?sf)+(0?sf)))
#define SF_SHIFTMASK(sf)        (SF_MASK(sf) << SF_SHIFT(sf))
#define SF_DEF(s,f,c)           ((NV ## s ## f ## c)<<SF_SHIFT(NV ## s ## f))
#define SF_IDX_DEF(s,f,c,i)     ((NV ## s ## f ## c)<<SF_SHIFT(NV ## s ## f(i)))
#define SF_NUM(s,f,n)           (((n)&SF_MASK(NV ## s ## f))<<SF_SHIFT(NV ## s ## f))
#define SF_IDX_NUM(s,f,n,i)     (((n)&SF_MASK(NV ## s ## f(i)))<<SF_SHIFT(NV ## s ## f(i)))
#define SF_VAL(s,f,v)           (((v)>>SF_SHIFT(NV ## s ## f))&SF_MASK(NV ## s ## f))
#define SF_WIDTH(sf)            ((1?sf) - (0?sf) + 1)
// This macro parses multi-word/array defines
#define SF_ARR32_VAL(s,f,arr) \
    (((arr)[SF_INDEX(NV ## s ## f)] >> SF_SHIFT(NV ## s ## f)) & SF_MASK(NV ## s ## f))
#define FLD_SF_DEF(s,f,d,l)     ((l)&~(SF_MASK(NV##s##f) << SF_SHIFT(NV##s##f)))| SF_DEF(s,f,d)
#define FLD_SF_NUM(s,f,n,l)     ((l)&~(SF_MASK(NV##s##f) << SF_SHIFT(NV##s##f)))| SF_NUM(s,f,n)
#define FLD_SF_IDX_DEF(s,f,c,i,l) (((l) & ~SF_SHIFTMASK(NV ## s ## f(i))) | SF_IDX_DEF(s,f,c,i))
#define FLD_SF_IDX_NUM(s,f,n,i,l) (((l) & ~SF_SHIFTMASK(NV ## s ## f(i))) | SF_IDX_NUM(s,f,n,i))


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_ACCESS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct IoAperture {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;
    struct RegisterAperture __nvoc_base_RegisterAperture;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct RegisterAperture *__nvoc_pbase_RegisterAperture;    // regaprt super
    struct IoAperture *__nvoc_pbase_IoAperture;    // ioaprt

    // Vtable with 8 per-object function pointers
    NvU8 (*__ioaprtReadReg08__)(struct IoAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)
    NvU16 (*__ioaprtReadReg16__)(struct IoAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)
    NvU32 (*__ioaprtReadReg32__)(struct IoAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)
    void (*__ioaprtWriteReg08__)(struct IoAperture * /*this*/, NvU32, NvV8);  // virtual override (regaprt) base (regaprt)
    void (*__ioaprtWriteReg16__)(struct IoAperture * /*this*/, NvU32, NvV16);  // virtual override (regaprt) base (regaprt)
    void (*__ioaprtWriteReg32__)(struct IoAperture * /*this*/, NvU32, NvV32);  // virtual override (regaprt) base (regaprt)
    void (*__ioaprtWriteReg32Uc__)(struct IoAperture * /*this*/, NvU32, NvV32);  // virtual override (regaprt) base (regaprt)
    NvBool (*__ioaprtIsRegValid__)(struct IoAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)

    // Data members
    OBJGPU *pGpu;
    NvU32 deviceIndex;
    NvU32 deviceInstance;
    DEVICE_MAPPING *pMapping;
    NvU32 mappingStartAddr;
    NvU32 baseAddress;
    NvU32 length;
};

#ifndef __NVOC_CLASS_IoAperture_TYPEDEF__
#define __NVOC_CLASS_IoAperture_TYPEDEF__
typedef struct IoAperture IoAperture;
#endif /* __NVOC_CLASS_IoAperture_TYPEDEF__ */

#ifndef __nvoc_class_id_IoAperture
#define __nvoc_class_id_IoAperture 0x40549c
#endif /* __nvoc_class_id_IoAperture */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IoAperture;

#define __staticCast_IoAperture(pThis) \
    ((pThis)->__nvoc_pbase_IoAperture)

#ifdef __nvoc_gpu_access_h_disabled
#define __dynamicCast_IoAperture(pThis) ((IoAperture*)NULL)
#else //__nvoc_gpu_access_h_disabled
#define __dynamicCast_IoAperture(pThis) \
    ((IoAperture*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(IoAperture)))
#endif //__nvoc_gpu_access_h_disabled

NV_STATUS __nvoc_objCreateDynamic_IoAperture(IoAperture**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_IoAperture(IoAperture**, Dynamic*, NvU32, struct IoAperture * arg_pParentAperture, OBJGPU * arg_pGpu, NvU32 arg_deviceIndex, NvU32 arg_deviceInstance, DEVICE_MAPPING * arg_pMapping, NvU32 arg_mappingStartAddr, NvU32 arg_offset, NvU32 arg_length);
#define __objCreate_IoAperture(ppNewObj, pParent, createFlags, arg_pParentAperture, arg_pGpu, arg_deviceIndex, arg_deviceInstance, arg_pMapping, arg_mappingStartAddr, arg_offset, arg_length) \
    __nvoc_objCreate_IoAperture((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pParentAperture, arg_pGpu, arg_deviceIndex, arg_deviceInstance, arg_pMapping, arg_mappingStartAddr, arg_offset, arg_length)


// Wrapper macros
#define ioaprtReadReg08_FNPTR(pAperture) pAperture->__ioaprtReadReg08__
#define ioaprtReadReg08(pAperture, addr) ioaprtReadReg08_DISPATCH(pAperture, addr)
#define ioaprtReadReg16_FNPTR(pAperture) pAperture->__ioaprtReadReg16__
#define ioaprtReadReg16(pAperture, addr) ioaprtReadReg16_DISPATCH(pAperture, addr)
#define ioaprtReadReg32_FNPTR(pAperture) pAperture->__ioaprtReadReg32__
#define ioaprtReadReg32(pAperture, addr) ioaprtReadReg32_DISPATCH(pAperture, addr)
#define ioaprtWriteReg08_FNPTR(pAperture) pAperture->__ioaprtWriteReg08__
#define ioaprtWriteReg08(pAperture, addr, value) ioaprtWriteReg08_DISPATCH(pAperture, addr, value)
#define ioaprtWriteReg16_FNPTR(pAperture) pAperture->__ioaprtWriteReg16__
#define ioaprtWriteReg16(pAperture, addr, value) ioaprtWriteReg16_DISPATCH(pAperture, addr, value)
#define ioaprtWriteReg32_FNPTR(pAperture) pAperture->__ioaprtWriteReg32__
#define ioaprtWriteReg32(pAperture, addr, value) ioaprtWriteReg32_DISPATCH(pAperture, addr, value)
#define ioaprtWriteReg32Uc_FNPTR(pAperture) pAperture->__ioaprtWriteReg32Uc__
#define ioaprtWriteReg32Uc(pAperture, addr, value) ioaprtWriteReg32Uc_DISPATCH(pAperture, addr, value)
#define ioaprtIsRegValid_FNPTR(pAperture) pAperture->__ioaprtIsRegValid__
#define ioaprtIsRegValid(pAperture, addr) ioaprtIsRegValid_DISPATCH(pAperture, addr)

// Dispatch functions
static inline NvU8 ioaprtReadReg08_DISPATCH(struct IoAperture *pAperture, NvU32 addr) {
    return pAperture->__ioaprtReadReg08__(pAperture, addr);
}

static inline NvU16 ioaprtReadReg16_DISPATCH(struct IoAperture *pAperture, NvU32 addr) {
    return pAperture->__ioaprtReadReg16__(pAperture, addr);
}

static inline NvU32 ioaprtReadReg32_DISPATCH(struct IoAperture *pAperture, NvU32 addr) {
    return pAperture->__ioaprtReadReg32__(pAperture, addr);
}

static inline void ioaprtWriteReg08_DISPATCH(struct IoAperture *pAperture, NvU32 addr, NvV8 value) {
    pAperture->__ioaprtWriteReg08__(pAperture, addr, value);
}

static inline void ioaprtWriteReg16_DISPATCH(struct IoAperture *pAperture, NvU32 addr, NvV16 value) {
    pAperture->__ioaprtWriteReg16__(pAperture, addr, value);
}

static inline void ioaprtWriteReg32_DISPATCH(struct IoAperture *pAperture, NvU32 addr, NvV32 value) {
    pAperture->__ioaprtWriteReg32__(pAperture, addr, value);
}

static inline void ioaprtWriteReg32Uc_DISPATCH(struct IoAperture *pAperture, NvU32 addr, NvV32 value) {
    pAperture->__ioaprtWriteReg32Uc__(pAperture, addr, value);
}

static inline NvBool ioaprtIsRegValid_DISPATCH(struct IoAperture *pAperture, NvU32 addr) {
    return pAperture->__ioaprtIsRegValid__(pAperture, addr);
}

NvU8 ioaprtReadReg08_IMPL(struct IoAperture *pAperture, NvU32 addr);

NvU16 ioaprtReadReg16_IMPL(struct IoAperture *pAperture, NvU32 addr);

NvU32 ioaprtReadReg32_IMPL(struct IoAperture *pAperture, NvU32 addr);

void ioaprtWriteReg08_IMPL(struct IoAperture *pAperture, NvU32 addr, NvV8 value);

void ioaprtWriteReg16_IMPL(struct IoAperture *pAperture, NvU32 addr, NvV16 value);

void ioaprtWriteReg32_IMPL(struct IoAperture *pAperture, NvU32 addr, NvV32 value);

void ioaprtWriteReg32Uc_IMPL(struct IoAperture *pAperture, NvU32 addr, NvV32 value);

NvBool ioaprtIsRegValid_IMPL(struct IoAperture *pAperture, NvU32 addr);

static inline NvU32 ioaprtGetRegAddr(struct IoAperture *pAperture, NvU32 addr) {
    return pAperture->baseAddress + addr;
}

static inline NvU32 ioaprtGetBaseAddr(struct IoAperture *pAperture) {
    return pAperture->baseAddress;
}

static inline NvU32 ioaprtGetLength(struct IoAperture *pAperture) {
    return pAperture->length;
}

NV_STATUS ioaprtConstruct_IMPL(struct IoAperture *arg_pAperture, struct IoAperture *arg_pParentAperture, OBJGPU *arg_pGpu, NvU32 arg_deviceIndex, NvU32 arg_deviceInstance, DEVICE_MAPPING *arg_pMapping, NvU32 arg_mappingStartAddr, NvU32 arg_offset, NvU32 arg_length);

#define __nvoc_ioaprtConstruct(arg_pAperture, arg_pParentAperture, arg_pGpu, arg_deviceIndex, arg_deviceInstance, arg_pMapping, arg_mappingStartAddr, arg_offset, arg_length) ioaprtConstruct_IMPL(arg_pAperture, arg_pParentAperture, arg_pGpu, arg_deviceIndex, arg_deviceInstance, arg_pMapping, arg_mappingStartAddr, arg_offset, arg_length)
#undef PRIVATE_FIELD


// In-place construct wrapper
NV_STATUS ioaprtInit(struct IoAperture *pAperture, struct IoAperture *pParentAperture, NvU32 offset, NvU32 length);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_ACCESS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct SwBcAperture {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;
    struct RegisterAperture __nvoc_base_RegisterAperture;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct RegisterAperture *__nvoc_pbase_RegisterAperture;    // regaprt super
    struct SwBcAperture *__nvoc_pbase_SwBcAperture;    // swbcaprt

    // Vtable with 8 per-object function pointers
    NvU8 (*__swbcaprtReadReg08__)(struct SwBcAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)
    NvU16 (*__swbcaprtReadReg16__)(struct SwBcAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)
    NvU32 (*__swbcaprtReadReg32__)(struct SwBcAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)
    void (*__swbcaprtWriteReg08__)(struct SwBcAperture * /*this*/, NvU32, NvV8);  // virtual override (regaprt) base (regaprt)
    void (*__swbcaprtWriteReg16__)(struct SwBcAperture * /*this*/, NvU32, NvV16);  // virtual override (regaprt) base (regaprt)
    void (*__swbcaprtWriteReg32__)(struct SwBcAperture * /*this*/, NvU32, NvV32);  // virtual override (regaprt) base (regaprt)
    void (*__swbcaprtWriteReg32Uc__)(struct SwBcAperture * /*this*/, NvU32, NvV32);  // virtual override (regaprt) base (regaprt)
    NvBool (*__swbcaprtIsRegValid__)(struct SwBcAperture * /*this*/, NvU32);  // virtual override (regaprt) base (regaprt)

    // Data members
    struct IoAperture *pApertures;
    NvU32 numApertures;
};

#ifndef __NVOC_CLASS_SwBcAperture_TYPEDEF__
#define __NVOC_CLASS_SwBcAperture_TYPEDEF__
typedef struct SwBcAperture SwBcAperture;
#endif /* __NVOC_CLASS_SwBcAperture_TYPEDEF__ */

#ifndef __nvoc_class_id_SwBcAperture
#define __nvoc_class_id_SwBcAperture 0x6d0f88
#endif /* __nvoc_class_id_SwBcAperture */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwBcAperture;

#define __staticCast_SwBcAperture(pThis) \
    ((pThis)->__nvoc_pbase_SwBcAperture)

#ifdef __nvoc_gpu_access_h_disabled
#define __dynamicCast_SwBcAperture(pThis) ((SwBcAperture*)NULL)
#else //__nvoc_gpu_access_h_disabled
#define __dynamicCast_SwBcAperture(pThis) \
    ((SwBcAperture*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SwBcAperture)))
#endif //__nvoc_gpu_access_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SwBcAperture(SwBcAperture**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SwBcAperture(SwBcAperture**, Dynamic*, NvU32, struct IoAperture * arg_pApertures, NvU32 arg_numApertures);
#define __objCreate_SwBcAperture(ppNewObj, pParent, createFlags, arg_pApertures, arg_numApertures) \
    __nvoc_objCreate_SwBcAperture((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pApertures, arg_numApertures)


// Wrapper macros
#define swbcaprtReadReg08_FNPTR(pAperture) pAperture->__swbcaprtReadReg08__
#define swbcaprtReadReg08(pAperture, addr) swbcaprtReadReg08_DISPATCH(pAperture, addr)
#define swbcaprtReadReg16_FNPTR(pAperture) pAperture->__swbcaprtReadReg16__
#define swbcaprtReadReg16(pAperture, addr) swbcaprtReadReg16_DISPATCH(pAperture, addr)
#define swbcaprtReadReg32_FNPTR(pAperture) pAperture->__swbcaprtReadReg32__
#define swbcaprtReadReg32(pAperture, addr) swbcaprtReadReg32_DISPATCH(pAperture, addr)
#define swbcaprtWriteReg08_FNPTR(pAperture) pAperture->__swbcaprtWriteReg08__
#define swbcaprtWriteReg08(pAperture, addr, value) swbcaprtWriteReg08_DISPATCH(pAperture, addr, value)
#define swbcaprtWriteReg16_FNPTR(pAperture) pAperture->__swbcaprtWriteReg16__
#define swbcaprtWriteReg16(pAperture, addr, value) swbcaprtWriteReg16_DISPATCH(pAperture, addr, value)
#define swbcaprtWriteReg32_FNPTR(pAperture) pAperture->__swbcaprtWriteReg32__
#define swbcaprtWriteReg32(pAperture, addr, value) swbcaprtWriteReg32_DISPATCH(pAperture, addr, value)
#define swbcaprtWriteReg32Uc_FNPTR(pAperture) pAperture->__swbcaprtWriteReg32Uc__
#define swbcaprtWriteReg32Uc(pAperture, addr, value) swbcaprtWriteReg32Uc_DISPATCH(pAperture, addr, value)
#define swbcaprtIsRegValid_FNPTR(pAperture) pAperture->__swbcaprtIsRegValid__
#define swbcaprtIsRegValid(pAperture, addr) swbcaprtIsRegValid_DISPATCH(pAperture, addr)

// Dispatch functions
static inline NvU8 swbcaprtReadReg08_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr) {
    return pAperture->__swbcaprtReadReg08__(pAperture, addr);
}

static inline NvU16 swbcaprtReadReg16_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr) {
    return pAperture->__swbcaprtReadReg16__(pAperture, addr);
}

static inline NvU32 swbcaprtReadReg32_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr) {
    return pAperture->__swbcaprtReadReg32__(pAperture, addr);
}

static inline void swbcaprtWriteReg08_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr, NvV8 value) {
    pAperture->__swbcaprtWriteReg08__(pAperture, addr, value);
}

static inline void swbcaprtWriteReg16_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr, NvV16 value) {
    pAperture->__swbcaprtWriteReg16__(pAperture, addr, value);
}

static inline void swbcaprtWriteReg32_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr, NvV32 value) {
    pAperture->__swbcaprtWriteReg32__(pAperture, addr, value);
}

static inline void swbcaprtWriteReg32Uc_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr, NvV32 value) {
    pAperture->__swbcaprtWriteReg32Uc__(pAperture, addr, value);
}

static inline NvBool swbcaprtIsRegValid_DISPATCH(struct SwBcAperture *pAperture, NvU32 addr) {
    return pAperture->__swbcaprtIsRegValid__(pAperture, addr);
}

NvU8 swbcaprtReadReg08_IMPL(struct SwBcAperture *pAperture, NvU32 addr);

NvU16 swbcaprtReadReg16_IMPL(struct SwBcAperture *pAperture, NvU32 addr);

NvU32 swbcaprtReadReg32_IMPL(struct SwBcAperture *pAperture, NvU32 addr);

void swbcaprtWriteReg08_IMPL(struct SwBcAperture *pAperture, NvU32 addr, NvV8 value);

void swbcaprtWriteReg16_IMPL(struct SwBcAperture *pAperture, NvU32 addr, NvV16 value);

void swbcaprtWriteReg32_IMPL(struct SwBcAperture *pAperture, NvU32 addr, NvV32 value);

void swbcaprtWriteReg32Uc_IMPL(struct SwBcAperture *pAperture, NvU32 addr, NvV32 value);

NvBool swbcaprtIsRegValid_IMPL(struct SwBcAperture *pAperture, NvU32 addr);

NV_STATUS swbcaprtConstruct_IMPL(struct SwBcAperture *arg_pAperture, struct IoAperture *arg_pApertures, NvU32 arg_numApertures);

#define __nvoc_swbcaprtConstruct(arg_pAperture, arg_pApertures, arg_numApertures) swbcaprtConstruct_IMPL(arg_pAperture, arg_pApertures, arg_numApertures)
#undef PRIVATE_FIELD


#endif // _GPU_ACCESS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_ACCESS_NVOC_H_
