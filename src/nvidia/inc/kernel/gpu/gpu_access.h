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
#ifndef _GPU_ACCESS_H_
#define _GPU_ACCESS_H_

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
    IO_DEVICE             parent;
    OBJGPU               *pGpu;
    DEVICE_INDEX          deviceIndex;
    NvU32                 instance;
    // The following members are initialized in objgpu.c,
    // but are not used anywhere. gpuApertureReg* functions
    // fall back to DEVICE_MAPPING instead
    GPUHWREG             *gpuNvAddr;        // CPU Virtual Address
    RmPhysAddr            gpuNvPAddr;       // Physical Base Address
    NvU32                 gpuNvLength;      // Length of Aperture
    NvU32                 gpuDeviceEnum;    // Device ID NV_DEVID_*
    NvU32                 refCount;         // refCount for the device map.
    DEVICE_REGFILTER_INFO devRegFilterInfo; // register filter range list
} GPU_IO_DEVICE;

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

#endif // _GPU_ACCESS_H_
