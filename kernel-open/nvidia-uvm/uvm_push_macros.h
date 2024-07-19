/*******************************************************************************
    Copyright (c) 2015-2024 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef __UVM_PUSH_MACROS_H__
#define __UVM_PUSH_MACROS_H__

#include "uvm_extern_decl.h"
#include "uvm_forward_decl.h"
#include "uvm_channel.h"
#include "nvtypes.h"
#include "nvmisc.h"
#include "cla06fsubch.h"
#include "cla16f.h"
#include "clb06f.h"

#define HWMASK(d, r, f) DRF_MASK(NV ## d ## _ ## r ## _ ## f)
#define HWSHIFT(d, r, f) DRF_SHIFT(NV ## d ## _ ## r ## _ ## f)
#define HWSHIFTMASK(d, r, f) DRF_SHIFTMASK(NV ## d ## _ ## r ## _ ## f)
#define HWSIZE(d, r, f) DRF_SIZE(NV ## d ## _ ## r ## _ ## f)
#define HWCONST(d, r, f, c) DRF_DEF(d, _ ## r, _ ## f, _ ## c)
#define HWVALUE(d, r, f, v) ({                                                                                      \
        NvU32 _v = (v);                                                                                             \
        NvU32 val = DRF_NUM(d, _ ## r, _ ## f, _v);                                                                 \
        UVM_ASSERT_MSG(_v == DRF_VAL(d, _ ## r, _ ## f, val), "v 0x%x mask 0x%x\n", _v, HWMASK(d, r, f));           \
        val;                                                                                                        \
    })

#define HWMASK64(d, r, f) DRF_MASK64(NV ## d ## _ ## r ## _ ## f)
#define HWCONST64(d, r, f, c) DRF_DEF64(d, _ ## r, _ ## f, _ ## c)
#define HWVALUE64(d, r, f, v) ({                                                                                    \
        NvU64 _v = (v);                                                                                             \
        NvU64 val = DRF_NUM64(d, _ ## r, _ ## f, _v);                                                               \
        UVM_ASSERT_MSG(_v == DRF_VAL64(d, _ ## r, _ ## f, val), "v 0x%llx mask 0x%llx\n", _v, HWMASK64(d, r, f));   \
        val;                                                                                                        \
    })

#define HWMASK_MW(d, r, f) DRF_MASK_MW(NV ## d ## _ ## r ## _ ## f)
#define HWSIZE_MW(d, r, f) DRF_SIZE_MW(NV ## d ## _ ## r ## _ ## f)
#define HWCONST_MW(d, r, f, c) DRF_DEF_MW(d, _ ## r, _ ## f, _ ## c)
#define HWVALUE_MW(d, r, f, v) ({                                                                               \
        NvU32 _v = (v);                                                                                         \
        NvU32 val = DRF_NUM_MW(d, _ ## r, _ ## f, _v);                                                          \
        UVM_ASSERT_MSG(_v == DRF_VAL_MW(d, _ ## r, _ ## f, val), "v 0x%x mask 0x%x\n", _v, HWMASK_MW(d, r, f)); \
        val;                                                                                                    \
    })

#define WRITE_HWCONST(v, d, r, f, c) FLD_SET_DRF(d,_##r,_##f,_##c, v)
#define WRITE_HWVALUE(v, d, r, f, n) FLD_SET_DRF_NUM(d,_##r,_##f, n, v)

// nvmisc.h has FLD_SET_DRF_NUM64, but not FLD_SET_DRF64
#define UVM_FLD_SET_DRF64(d,r,f,c,v) ((((NvU64)(v)) & ~DRF_SHIFTMASK64(NV##d##r##f)) | DRF_DEF64(d,r,f,c))

#define WRITE_HWCONST64(v, d, r, f, c) UVM_FLD_SET_DRF64(d,_##r,_##f,_##c, v)
#define WRITE_HWVALUE64(v, d, r, f, n) FLD_SET_DRF_NUM64(d,_##r,_##f, n, v)

#define READ_HWVALUE(v, d, r, f) DRF_VAL(d,_##r,_##f, v)
#define READ_HWVALUE_MW(v, d, r, f) DRF_VAL_MW(d,_##r,_##f, v)
#define WRITE_HWCONST_MW(v, d, r, f, c) FLD_ASSIGN_MW(NV##d##_##r##_##f, DRF_DEF_MW(d,_##r,_##f,_##c), v)
#define WRITE_HWVALUE_MW(v, d, r, f, n) FLD_ASSIGN_MW(NV##d##_##r##_##f, DRF_NUM_MW(d,_##r,_##f,n), v)

// Host methods ignore the subchannel, just use 0
#define UVM_SUBCHANNEL_HOST 0

// Starting with Kepler HW settled on using a fixed subchannel for CE.
#define UVM_SUBCHANNEL_CE NVA06F_SUBCHANNEL_COPY_ENGINE

#define UVM_SUBCHANNEL_A16F UVM_SUBCHANNEL_HOST

#define UVM_SUBCHANNEL_B06F UVM_SUBCHANNEL_HOST
#define UVM_SUBCHANNEL_B0B5 UVM_SUBCHANNEL_CE

#define UVM_SUBCHANNEL_C06F UVM_SUBCHANNEL_HOST
#define UVM_SUBCHANNEL_C0B5 UVM_SUBCHANNEL_CE

#define UVM_SUBCHANNEL_C36F UVM_SUBCHANNEL_HOST
#define UVM_SUBCHANNEL_C3B5 UVM_SUBCHANNEL_CE

#define UVM_SUBCHANNEL_C46F UVM_SUBCHANNEL_HOST

#define UVM_SUBCHANNEL_C56F UVM_SUBCHANNEL_HOST
#define UVM_SUBCHANNEL_C6B5 UVM_SUBCHANNEL_CE

#define UVM_SUBCHANNEL_C86F UVM_SUBCHANNEL_HOST
#define UVM_SUBCHANNEL_C8B5 UVM_SUBCHANNEL_CE

#define UVM_SUBCHANNEL_C96F UVM_SUBCHANNEL_HOST

// Channel for UVM SW methods. This is defined in nv_uvm_types.h. RM does not
// care about the specific number as long as it's bigger than the largest HW
// value. For example, Kepler reserves subchannels 5-7 for software objects.
#define UVM_SUBCHANNEL_C076 UVM_SW_OBJ_SUBCHANNEL

// NVA06F_SUBCHANNEL_COMPUTE is a semi-arbitrary value for UVM_SUBCHANNEL_SEC2.
// We need a "unique" subchannel across all subchannels UVM submits work. This
// is used when we are post-processing a pushbuffer and we need to extract SEC2
// methods from a it, having a unique subchannel facilitates the SEC2 method
// identification.
#define UVM_SUBCHANNEL_SEC2 NVA06F_SUBCHANNEL_COMPUTE
#define UVM_SUBCHANNEL_CBA2 UVM_SUBCHANNEL_SEC2

#define UVM_METHOD_SIZE 4
#define UVM_METHOD_COUNT_MAX HWMASK(B06F, DMA, INCR_COUNT)
#if HWMASK(B06F, DMA, INCR_COUNT) != HWMASK(B06F, DMA, NONINCR_COUNT)
#error "Unable to define UVM_METHOD_COUNT_MAX"
#endif

#define UVM_METHOD_INC(subch, address, count)                                   \
                        (HWCONST(B06F, DMA, SEC_OP, INC_METHOD) |               \
                         HWVALUE(B06F, DMA, INCR_ADDRESS, (address) >> 2) |     \
                         HWVALUE(B06F, DMA, INCR_SUBCHANNEL, (subch)) |         \
                         HWVALUE(B06F, DMA, INCR_COUNT, (count)))

#define UVM_METHOD_NONINC(subch, address, count)                                    \
                            (HWCONST(B06F, DMA, SEC_OP, NON_INC_METHOD) |           \
                             HWVALUE(B06F, DMA, NONINCR_ADDRESS, (address) >> 2) |  \
                             HWVALUE(B06F, DMA, NONINCR_SUBCHANNEL, (subch)) |      \
                             HWVALUE(B06F, DMA, NONINCR_COUNT, (count)))

#define __UVM_ASSERT_CONTIGUOUS_METHODS(a1, a2) BUILD_BUG_ON((a2) - (a1) != UVM_METHOD_SIZE)

// __NV_PUSH_*U support being called recursively from the N+1 sized method with
// the _0U doing all the common things.
// Notably all the push macros assume that symbol "push" of type uvm_push_t * is
// in scope.
#define __NV_PUSH_0U(subch, count, a1)                                                            \
    do {                                                                                          \
        UVM_ASSERT(!uvm_global_is_suspended());                                                   \
        UVM_ASSERT(uvm_push_get_size(push) + (count + 1) * UVM_METHOD_SIZE <= UVM_MAX_PUSH_SIZE); \
        UVM_ASSERT_MSG(IS_ALIGNED(a1, UVM_METHOD_SIZE), "Address %u\n", a1);                      \
                                                                                                  \
        push->next[0] = UVM_METHOD_INC(subch, a1, count);                                         \
        ++push->next;                                                                             \
    } while (0)

#define __NV_PUSH_1U(subch, count, a1,d1)                               \
    do {                                                                \
        __NV_PUSH_0U(subch, count, a1);                                 \
        UVM_ASSERT_MSG(uvm_push_method_is_valid(push, subch, a1, d1),   \
                       "Method validation failed in channel %s\n",      \
                       push->channel->name);                            \
        push->next[0] = d1;                                             \
        ++push->next;                                                   \
    } while (0)

#define __NV_PUSH_2U(subch, count, a1,d1, a2,d2)                        \
    do {                                                                \
        __UVM_ASSERT_CONTIGUOUS_METHODS(a1, a2);                        \
        __NV_PUSH_1U(subch, count, a1,d1);                              \
        UVM_ASSERT_MSG(uvm_push_method_is_valid(push, subch, a2, d2),   \
                       "Method validation failed in channel %s\n",      \
                       push->channel->name);                            \
        push->next[0] = d2;                                             \
        ++push->next;                                                   \
    } while (0)

#define __NV_PUSH_3U(subch, count, a1,d1, a2,d2, a3,d3)                 \
    do {                                                                \
        __UVM_ASSERT_CONTIGUOUS_METHODS(a2, a3);                        \
        __NV_PUSH_2U(subch, count, a1,d1, a2,d2);                       \
        UVM_ASSERT_MSG(uvm_push_method_is_valid(push, subch, a3, d3),   \
                       "Method validation failed in channel %s\n",      \
                       push->channel->name);                            \
        push->next[0] = d3;                                             \
        ++push->next;                                                   \
    } while (0)

#define __NV_PUSH_4U(subch, count, a1,d1, a2,d2, a3,d3, a4,d4)          \
    do {                                                                \
        __UVM_ASSERT_CONTIGUOUS_METHODS(a3, a4);                        \
        __NV_PUSH_3U(subch, count, a1,d1, a2,d2, a3,d3);                \
        UVM_ASSERT_MSG(uvm_push_method_is_valid(push, subch, a4, d4),   \
                       "Method validation failed in channel %s\n",      \
                       push->channel->name);                            \
        push->next[0] = d4;                                             \
        ++push->next;                                                   \
    } while (0)

#define __NV_PUSH_5U(subch, count, a1,d1, a2,d2, a3,d3, a4,d4, a5,d5)   \
    do {                                                                \
        __UVM_ASSERT_CONTIGUOUS_METHODS(a4, a5);                        \
        __NV_PUSH_4U(subch, count, a1,d1, a2,d2, a3,d3, a4,d4);         \
        UVM_ASSERT_MSG(uvm_push_method_is_valid(push, subch, a5, d5),   \
                       "Method validation failed in channel %s\n",      \
                       push->channel->name);                            \
        push->next[0] = d5;                                             \
        ++push->next;                                                   \
    } while (0)

#define __NV_PUSH_6U(subch, count, a1,d1, a2,d2, a3,d3, a4,d4, a5,d5, a6,d6)    \
    do {                                                                        \
        __UVM_ASSERT_CONTIGUOUS_METHODS(a5, a6);                                \
        __NV_PUSH_5U(subch, count, a1,d1, a2,d2, a3,d3, a4,d4, a5,d5);          \
        UVM_ASSERT_MSG(uvm_push_method_is_valid(push, subch, a6, d6),           \
                       "Method validation failed in channel %s\n",              \
                       push->channel->name);                                    \
        push->next[0] = d6;                                                     \
        ++push->next;                                                           \
    } while (0)

#define NV_PUSH_1U(class, a1,d1)                                    \
    __NV_PUSH_1U(UVM_SUBCHANNEL_ ## class, 1,                       \
                 NV ## class ## _ ## a1, d1)

#define NV_PUSH_2U(class, a1,d1, a2,d2)                             \
    __NV_PUSH_2U(UVM_SUBCHANNEL_ ## class, 2,                       \
                 NV ## class ## _ ## a1, d1,                        \
                 NV ## class ## _ ## a2, d2)

#define NV_PUSH_3U(class, a1,d1, a2,d2, a3,d3)                      \
    __NV_PUSH_3U(UVM_SUBCHANNEL_ ## class, 3,                       \
                 NV ## class ## _ ## a1, d1,                        \
                 NV ## class ## _ ## a2, d2,                        \
                 NV ## class ## _ ## a3, d3)

#define NV_PUSH_4U(class, a1,d1, a2,d2, a3,d3, a4,d4)               \
    __NV_PUSH_4U(UVM_SUBCHANNEL_ ## class, 4,                       \
                 NV ## class ## _ ## a1, d1,                        \
                 NV ## class ## _ ## a2, d2,                        \
                 NV ## class ## _ ## a3, d3,                        \
                 NV ## class ## _ ## a4, d4)

#define NV_PUSH_5U(class, a1,d1, a2,d2, a3,d3, a4,d4, a5,d5)        \
    __NV_PUSH_5U(UVM_SUBCHANNEL_ ## class, 5,                       \
                 NV ## class ## _ ## a1, d1,                        \
                 NV ## class ## _ ## a2, d2,                        \
                 NV ## class ## _ ## a3, d3,                        \
                 NV ## class ## _ ## a4, d4,                        \
                 NV ## class ## _ ## a5, d5)

#define NV_PUSH_6U(class, a1,d1, a2,d2, a3,d3, a4,d4, a5,d5, a6,d6) \
    __NV_PUSH_6U(UVM_SUBCHANNEL_ ## class, 6,                       \
                 NV ## class ## _ ## a1, d1,                        \
                 NV ## class ## _ ## a2, d2,                        \
                 NV ## class ## _ ## a3, d3,                        \
                 NV ## class ## _ ## a4, d4,                        \
                 NV ## class ## _ ## a5, d5,                        \
                 NV ## class ## _ ## a6, d6)

// Non-incrementing method with count data fields following it. The data is left
// untouched and hence it's primarily useful for a NOP method.
#define __NV_PUSH_NU_NONINC(subch, count, address)                                                \
    do {                                                                                          \
        UVM_ASSERT(!uvm_global_is_suspended());                                                   \
        UVM_ASSERT(uvm_push_get_size(push) + (count + 1) * UVM_METHOD_SIZE <= UVM_MAX_PUSH_SIZE); \
        UVM_ASSERT_MSG(IS_ALIGNED(address, UVM_METHOD_SIZE), "Address %u\n", address);            \
        push->next[0] = UVM_METHOD_NONINC(subch, address, count);                                 \
        push->next += count + 1;                                                                  \
    } while (0)

#define NV_PUSH_NU_NONINC(class, a1, count)                 \
    __NV_PUSH_NU_NONINC(UVM_SUBCHANNEL_ ## class, count,    \
       NV ## class ## _ ## a1)

#endif // __UVM_PUSH_MACROS_H__
