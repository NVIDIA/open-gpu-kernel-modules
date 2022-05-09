/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _IO_ACCESS_H_
#define _IO_ACCESS_H_

#include "nvtypes.h"
#include "nvstatus.h"

typedef struct IO_DEVICE IO_DEVICE, *PIO_DEVICE;
typedef struct IO_APERTURE IO_APERTURE, *PIO_APERTURE;

typedef NvU8     ReadReg008Fn(PIO_APERTURE a, NvU32 addr);
typedef NvU16    ReadReg016Fn(PIO_APERTURE a, NvU32 addr);
typedef NvU32    ReadReg032Fn(PIO_APERTURE a, NvU32 addr);
typedef void    WriteReg008Fn(PIO_APERTURE a, NvU32 addr, NvV8  value);
typedef void    WriteReg016Fn(PIO_APERTURE a, NvU32 addr, NvV16 value);
typedef void    WriteReg032Fn(PIO_APERTURE a, NvU32 addr, NvV32 value);
typedef NvBool     ValidRegFn(PIO_APERTURE a, NvU32 addr);

#define REG_DRF_SHIFT(drf)          ((0?drf) % 32)
#define REG_DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define REG_DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<REG_DRF_SHIFT(NV ## d ## r ## f))
#define REG_DRF_NUM(d,r,f,n)        (((n)&REG_DRF_MASK(NV ## d ## r ## f))<<REG_DRF_SHIFT(NV ## d ## r ## f))
#define REG_DRF_VAL(d,r,f,v)        (((v)>>REG_DRF_SHIFT(NV ## d ## r ## f))&REG_DRF_MASK(NV ## d ## r ## f))
#define REG_DRF_SHIFTMASK(drf)      (REG_DRF_MASK(drf)<<(REG_DRF_SHIFT(drf)))
#define REG_DRF_WIDTH(drf)          ((1?drf) - (0?drf) + 1)

#define REG_RD08(ap, addr)          (ap)->pDevice->pReadReg008Fn((ap), (addr))
#define REG_RD16(ap, addr)          (ap)->pDevice->pReadReg016Fn((ap), (addr))
#define REG_RD32(ap, addr)          (ap)->pDevice->pReadReg032Fn((ap), (addr))
#define REG_WR08(ap, addr, val)     (ap)->pDevice->pWriteReg008Fn((ap), (addr), (val))
#define REG_WR16(ap, addr, val)     (ap)->pDevice->pWriteReg016Fn((ap), (addr), (val))
#define REG_WR32(ap, addr, val)     (ap)->pDevice->pWriteReg032Fn((ap), (addr), (val))
#define REG_WR32_UC(ap, addr, val)  (ap)->pDevice->pWriteReg032UcFn((ap), (addr), (val))
#define REG_VALID(ap, addr)         (ap)->pDevice->pValidRegFn((ap), (addr))

// Get the address of a register given the Aperture and offset.
#define REG_GET_ADDR(ap, offset)    ((ap)->baseAddress + (offset))

//
// Macros for register I/O
//

#define REG_FLD_WR_DRF_NUM(ap,d,r,f,n) REG_WR32(ap,NV##d##r,(REG_RD32(ap,NV##d##r)&~(REG_DRF_MASK(NV##d##r##f)<<REG_DRF_SHIFT(NV##d##r##f)))|REG_DRF_NUM(d,r,f,n))
#define REG_FLD_WR_DRF_DEF(ap,d,r,f,c) REG_WR32(ap,NV##d##r,(REG_RD32(ap,NV##d##r)&~(REG_DRF_MASK(NV##d##r##f)<<REG_DRF_SHIFT(NV##d##r##f)))|REG_DRF_DEF(d,r,f,c))
#define REG_RD_DRF(ap,d,r,f)       (((REG_RD32(ap,NV ## d ## r))>>REG_DRF_SHIFT(NV ## d ## r ## f))&REG_DRF_MASK(NV ## d ## r ## f))
#define REG_FLD_TEST_DRF_DEF(ap,d,r,f,c) (REG_RD_DRF(ap,d, r, f) == NV##d##r##f##c)
#define REG_FLD_TEST_DRF_NUM(ap,d,r,f,n) (REG_RD_DRF(ap,d, r, f) == n)
#define REG_FLD_IDX_TEST_DRF_DEF(ap,d,r,f,c,i) (REG_IDX_RD_DRF(ap, d, r, i, f) == NV##d##r##f##c)

// Read/write a field or entire register of which there are several copies each accessed via an index
#define REG_IDX_WR_DRF_NUM(ap,d,r,i,f,n) REG_WR32(ap,NV ## d ## r(i), REG_DRF_NUM(d,r,f,n))
#define REG_IDX_WR_DRF_DEF(ap,d,r,i,f,c) REG_WR32(ap,NV ## d ## r(i), REG_DRF_DEF(d,r,f,c))
#define REG_FLD_IDX_WR_DRF_NUM(ap,d,r,i,f,n) REG_WR32(ap,NV##d##r(i),(REG_RD32(ap,NV##d##r(i))&~(REG_DRF_MASK(NV##d##r##f)<<REG_DRF_SHIFT(NV##d##r##f)))|REG_DRF_NUM(d,r,f,n))
#define REG_FLD_IDX_WR_DRF_DEF(ap,d,r,i,f,c) REG_WR32(ap,NV##d##r(i),(REG_RD32(ap,NV##d##r(i))&~(REG_DRF_MASK(NV##d##r##f)<<REG_DRF_SHIFT(NV##d##r##f)))|REG_DRF_DEF(d,r,f,c))
#define REG_IDX_RD_DRF(ap,d,r,i,f)              (((REG_RD32(ap,NV ## d ## r(i)))>>REG_DRF_SHIFT(NV ## d ## r ## f))&REG_DRF_MASK(NV ## d ## r ## f))
#define REG_RD_DRF_IDX(ap,d,r,f,i)              (((REG_RD32(ap,NV ## d ## r))>>REG_DRF_SHIFT(NV ## d ## r ## f(i)))&REG_DRF_MASK(NV ## d ## r ## f(i)))
#define REG_IDX_OFFSET_RD_DRF(ap,d,r,i,o,f)     (((REG_RD32(ap,NV ## d ## r(i,o)))>>REG_DRF_SHIFT(NV ## d ## r ## f))&REG_DRF_MASK(NV ## d ## r ## f))

struct IO_DEVICE
{
    ReadReg008Fn         *pReadReg008Fn;
    ReadReg016Fn         *pReadReg016Fn;
    ReadReg032Fn         *pReadReg032Fn;
    WriteReg008Fn        *pWriteReg008Fn;
    WriteReg016Fn        *pWriteReg016Fn;
    WriteReg032Fn        *pWriteReg032Fn;
    WriteReg032Fn        *pWriteReg032UcFn;
    ValidRegFn           *pValidRegFn;
};

struct IO_APERTURE
{
    PIO_DEVICE   pDevice;         // Pointer to module specific IO_DEVICE
    NvU32        baseAddress;     // register base address
    NvU32        length;          // length of aperture
};

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

NV_STATUS ioaccessCreateIOAperture
(
    IO_APERTURE **ppAperture,
    IO_APERTURE *pParentAperture,
    IO_DEVICE *pDevice,
    NvU32 offset,
    NvU32 length
);

NV_STATUS ioaccessInitIOAperture
(
    IO_APERTURE *pAperture,
    IO_APERTURE *pParentAperture,
    IO_DEVICE *pDevice,
    NvU32 offset,
    NvU32 length
);

void ioaccessDestroyIOAperture(IO_APERTURE *pAperture);

#endif // _IO_ACCESS_H_
