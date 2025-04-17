
#ifndef _G_IOACCESS_NVOC_H_
#define _G_IOACCESS_NVOC_H_

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
#if (defined(NVRM) || defined(RMCFG_FEATURE_PLATFORM_GSP)) && !defined(NVWATCH)
#pragma once
#include "g_ioaccess_nvoc.h"
#endif

#ifndef _IO_ACCESS_H_
#define _IO_ACCESS_H_

#include "nvtypes.h"
#include "nvstatus.h"
#if (defined(NVRM) || defined(RMCFG_FEATURE_PLATFORM_GSP)) && !defined(NVWATCH)
#include "nvoc/prelude.h"
#endif

#define REG_DRF_SHIFT(drf)          ((0?drf) % 32)
#define REG_DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define REG_DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<REG_DRF_SHIFT(NV ## d ## r ## f))
#define REG_DRF_NUM(d,r,f,n)        (((n)&REG_DRF_MASK(NV ## d ## r ## f))<<REG_DRF_SHIFT(NV ## d ## r ## f))
#define REG_DRF_VAL(d,r,f,v)        (((v)>>REG_DRF_SHIFT(NV ## d ## r ## f))&REG_DRF_MASK(NV ## d ## r ## f))
#define REG_DRF_SHIFTMASK(drf)      (REG_DRF_MASK(drf)<<(REG_DRF_SHIFT(drf)))
#define REG_DRF_WIDTH(drf)          ((1?drf) - (0?drf) + 1)

#if (defined(NVRM) || defined(RMCFG_FEATURE_PLATFORM_GSP)) && !defined(NVWATCH)
#define REG_RD08(ap, addr)          regaprtReadReg08   (staticCast(ap, RegisterAperture), addr)
#define REG_RD16(ap, addr)          regaprtReadReg16   (staticCast(ap, RegisterAperture), addr)
#define REG_RD32(ap, addr)          regaprtReadReg32   (staticCast(ap, RegisterAperture), addr)
#define REG_WR08(ap, addr, val)     regaprtWriteReg08  (staticCast(ap, RegisterAperture), addr, val)
#define REG_WR16(ap, addr, val)     regaprtWriteReg16  (staticCast(ap, RegisterAperture), addr, val)
#define REG_WR32(ap, addr, val)     regaprtWriteReg32  (staticCast(ap, RegisterAperture), addr, val)
#define REG_WR32_UC(ap, addr, val)  regaprtWriteReg32Uc(staticCast(ap, RegisterAperture), addr, val)
#define REG_VALID(ap, addr)         regaprtIsRegValid  (staticCast(ap, RegisterAperture), addr)


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_IOACCESS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable
struct NVOC_METADATA__RegisterAperture;
struct NVOC_VTABLE__RegisterAperture;


struct RegisterAperture {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RegisterAperture *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Ancestor object pointers for `staticCast` feature
    struct RegisterAperture *__nvoc_pbase_RegisterAperture;    // regaprt
};


// Vtable with 8 per-class function pointers
struct NVOC_VTABLE__RegisterAperture {
    NvU8 (*__regaprtReadReg08__)(struct RegisterAperture * /*this*/, NvU32);  // pure virtual
    NvU16 (*__regaprtReadReg16__)(struct RegisterAperture * /*this*/, NvU32);  // pure virtual
    NvU32 (*__regaprtReadReg32__)(struct RegisterAperture * /*this*/, NvU32);  // pure virtual
    void (*__regaprtWriteReg08__)(struct RegisterAperture * /*this*/, NvU32, NvV8);  // pure virtual
    void (*__regaprtWriteReg16__)(struct RegisterAperture * /*this*/, NvU32, NvV16);  // pure virtual
    void (*__regaprtWriteReg32__)(struct RegisterAperture * /*this*/, NvU32, NvV32);  // pure virtual
    void (*__regaprtWriteReg32Uc__)(struct RegisterAperture * /*this*/, NvU32, NvV32);  // pure virtual
    NvBool (*__regaprtIsRegValid__)(struct RegisterAperture * /*this*/, NvU32);  // pure virtual
};

// Metadata with per-class RTTI and vtable
struct NVOC_METADATA__RegisterAperture {
    const struct NVOC_RTTI rtti;
    const struct NVOC_VTABLE__RegisterAperture vtable;
};

#ifndef __NVOC_CLASS_RegisterAperture_TYPEDEF__
#define __NVOC_CLASS_RegisterAperture_TYPEDEF__
typedef struct RegisterAperture RegisterAperture;
#endif /* __NVOC_CLASS_RegisterAperture_TYPEDEF__ */

#ifndef __nvoc_class_id_RegisterAperture
#define __nvoc_class_id_RegisterAperture 0xfcaf2e
#endif /* __nvoc_class_id_RegisterAperture */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterAperture;

#define __staticCast_RegisterAperture(pThis) \
    ((pThis)->__nvoc_pbase_RegisterAperture)

#ifdef __nvoc_ioaccess_h_disabled
#define __dynamicCast_RegisterAperture(pThis) ((RegisterAperture*) NULL)
#else //__nvoc_ioaccess_h_disabled
#define __dynamicCast_RegisterAperture(pThis) \
    ((RegisterAperture*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RegisterAperture)))
#endif //__nvoc_ioaccess_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RegisterAperture(RegisterAperture**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RegisterAperture(RegisterAperture**, Dynamic*, NvU32);
#define __objCreate_RegisterAperture(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_RegisterAperture((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define regaprtReadReg08_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtReadReg08__
#define regaprtReadReg08(pAperture, addr) regaprtReadReg08_DISPATCH(pAperture, addr)
#define regaprtReadReg16_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtReadReg16__
#define regaprtReadReg16(pAperture, addr) regaprtReadReg16_DISPATCH(pAperture, addr)
#define regaprtReadReg32_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtReadReg32__
#define regaprtReadReg32(pAperture, addr) regaprtReadReg32_DISPATCH(pAperture, addr)
#define regaprtWriteReg08_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg08__
#define regaprtWriteReg08(pAperture, addr, value) regaprtWriteReg08_DISPATCH(pAperture, addr, value)
#define regaprtWriteReg16_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg16__
#define regaprtWriteReg16(pAperture, addr, value) regaprtWriteReg16_DISPATCH(pAperture, addr, value)
#define regaprtWriteReg32_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg32__
#define regaprtWriteReg32(pAperture, addr, value) regaprtWriteReg32_DISPATCH(pAperture, addr, value)
#define regaprtWriteReg32Uc_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg32Uc__
#define regaprtWriteReg32Uc(pAperture, addr, value) regaprtWriteReg32Uc_DISPATCH(pAperture, addr, value)
#define regaprtIsRegValid_FNPTR(pAperture) pAperture->__nvoc_metadata_ptr->vtable.__regaprtIsRegValid__
#define regaprtIsRegValid(pAperture, addr) regaprtIsRegValid_DISPATCH(pAperture, addr)

// Dispatch functions
static inline NvU8 regaprtReadReg08_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr) {
    return pAperture->__nvoc_metadata_ptr->vtable.__regaprtReadReg08__(pAperture, addr);
}

static inline NvU16 regaprtReadReg16_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr) {
    return pAperture->__nvoc_metadata_ptr->vtable.__regaprtReadReg16__(pAperture, addr);
}

static inline NvU32 regaprtReadReg32_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr) {
    return pAperture->__nvoc_metadata_ptr->vtable.__regaprtReadReg32__(pAperture, addr);
}

static inline void regaprtWriteReg08_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr, NvV8 value) {
    pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg08__(pAperture, addr, value);
}

static inline void regaprtWriteReg16_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr, NvV16 value) {
    pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg16__(pAperture, addr, value);
}

static inline void regaprtWriteReg32_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg32__(pAperture, addr, value);
}

static inline void regaprtWriteReg32Uc_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr, NvV32 value) {
    pAperture->__nvoc_metadata_ptr->vtable.__regaprtWriteReg32Uc__(pAperture, addr, value);
}

static inline NvBool regaprtIsRegValid_DISPATCH(struct RegisterAperture *pAperture, NvU32 addr) {
    return pAperture->__nvoc_metadata_ptr->vtable.__regaprtIsRegValid__(pAperture, addr);
}

#undef PRIVATE_FIELD


//
// TODO: Remove the wrapper structure WAR once NVOC supports in-place object construction
// The proxy structure was introduced to avoid major refactoring until the feature is implemented
// Also fix IoAperture getters
// Use the interface class in NVWATCH once NVOC is enabled there
//
#else // (defined(NVRM) || defined(RMCFG_FEATURE_PLATFORM_GSP)) && !defined(NVWATCH)
typedef struct IO_DEVICE IO_DEVICE;
typedef struct IO_APERTURE IO_APERTURE;

typedef NvU8     ReadReg008Fn(IO_APERTURE *a, NvU32 addr);
typedef NvU16    ReadReg016Fn(IO_APERTURE *a, NvU32 addr);
typedef NvU32    ReadReg032Fn(IO_APERTURE *a, NvU32 addr);
typedef void    WriteReg008Fn(IO_APERTURE *a, NvU32 addr, NvV8  value);
typedef void    WriteReg016Fn(IO_APERTURE *a, NvU32 addr, NvV16 value);
typedef void    WriteReg032Fn(IO_APERTURE *a, NvU32 addr, NvV32 value);
typedef NvBool     ValidRegFn(IO_APERTURE *a, NvU32 addr);

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
    IO_DEVICE *pDevice;     // Pointer to module specific IO_DEVICE
    NvU32      baseAddress; // register base address
    NvU32      length;      // length of aperture
};

NV_STATUS ioaccessInitIOAperture
(
    IO_APERTURE *pAperture,
    IO_APERTURE *pParentAperture,
    IO_DEVICE   *pDevice,
    NvU32 offset,
    NvU32 length
);
#endif // (defined(NVRM) || defined(RMCFG_FEATURE_PLATFORM_GSP)) && !defined(NVWATCH)


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

#endif // _IO_ACCESS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_IOACCESS_NVOC_H_
