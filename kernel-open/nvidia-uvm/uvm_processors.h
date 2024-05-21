/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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

#ifndef __UVM_PROCESSORS_H__
#define __UVM_PROCESSORS_H__

#include "uvm_linux.h"
#include "uvm_common.h"
#include <linux/numa.h>

// Processor identifiers
// =====================
//
// UVM uses its own identifiers to refer to the processors in the system. For
// simplicity (and performance), integers are used. However, in order to
// provide type safety, they are wrapped within the uvm_parent_processor_id_t
// struct.
// The range of valid identifiers needs to cover the maximum number of
// supported GPUs on a system plus the CPU. CPU is assigned value 0, and GPUs
// range: [1, UVM_PARENT_ID_MAX_GPUS].
//
// There are some functions that only expect GPU identifiers and, in order to
// make it clearer, the uvm_parent_gpu_id_t alias type is provided. However, as
// this type is just a typedef of uvm_parent_processor_id_t, there is no type
// checking performed by the compiler.
//
// Identifier value vs index
// -------------------------
//
// Although we can rely on helpers for most of the operations related to
// processor ids, there are some scenarios in which we need to obtain their
// numerical value. Notably:
// - Logging
// - Array indexing
//
// Therefore, a helper is provided to obtain this value. However, there is a
// special case for array indexing, as there are some arrays that only contain
// entries for GPUs. In that case, the array cannot be directly indexed with
// the identifier's value. Instead, we use a helper that provides the index of
// the GPU within the GPU id space (basically id - 1).
//
// In the diagram below, MAX_SUB is used to abbreviate
// UVM_PARENT_ID_MAX_SUB_PROCESSORS.
//
//            |-------------------------- uvm_parent_processor_id_t ---------------------------------------------|
//            |                                                                                                  |
//            |     |----------------------- uvm_parent_gpu_id_t -----------------------------------------------||
//            |     |                                                                                           ||
// Proc type  | CPU | GPU |        ...        | GPU | ... GPU                                                   ||
//            |     |     |                   |     |                                                           ||
// ID values  |  0  |  1  |        ...        | i+1 | ... UVM_PARENT_ID_MAX_PROCESSORS-1                        ||
//            |     |     |                   |     |                                                           ||
// GPU index  |     |  0  |        ...        |  i  | ... UVM_PARENT_ID_MAX_GPUS-1                              ||
//            |     |     |                   |     |                                                           ||
//            +     +     +                   +     +                                                           ++
//            |     |     |-------------|     |     |-----------------------------|                             ||
//            |     |                   |     |                                   |                             ||
// GPU index  |     |  0  ... MAX_SUB-1 | ... |  i*MAX_SUB    ... (i+1)*MAX_SUB-1 | ... UVM_ID_MAX_GPUS-1       ||
//            |     |                   |     |                                   |                             ||
// ID values  |  0  |  1  ... MAX_SUB   | ... | (i*MAX_SUB)+1 ... (i+1)*MAX_SUB   | ... UVM_ID_MAX_PROCESSORS-1 ||
//            |     |                   |     |                                   |                             ||
// Proc type  | CPU | GPU ... GPU       | ... | GPU           ... GPU             | ... GPU                     ||
//            |     |                                                                                           ||
//            |     |-------------------------------------- uvm_gpu_id_t ---------------------------------------||
//            |                                                                                                  |
//            |----------------------------------------- uvm_processor_id_t -------------------------------------|
//
// When SMC is enabled, each GPU partition gets its own uvm_gpu_t object.

#define UVM_PROCESSOR_MASK(mask_t,                                                                           \
                           prefix_fn_mask,                                                                   \
                           maxval,                                                                           \
                           proc_id_t,                                                                        \
                           proc_id_ctor)                                                                     \
                                                                                                             \
typedef struct                                                                                               \
{                                                                                                            \
    DECLARE_BITMAP(bitmap, maxval);                                                                          \
} mask_t;                                                                                                    \
                                                                                                             \
static bool prefix_fn_mask##_test(const mask_t *mask, proc_id_t id)                                          \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    return test_bit(id.val, mask->bitmap);                                                                   \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_set_atomic(mask_t *mask, proc_id_t id)                                          \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    set_bit(id.val, mask->bitmap);                                                                           \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_set(mask_t *mask, proc_id_t id)                                                 \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    __set_bit(id.val, mask->bitmap);                                                                         \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_clear_atomic(mask_t *mask, proc_id_t id)                                        \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    clear_bit(id.val, mask->bitmap);                                                                         \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_clear(mask_t *mask, proc_id_t id)                                               \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    __clear_bit(id.val, mask->bitmap);                                                                       \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_test_and_set_atomic(mask_t *mask, proc_id_t id)                                 \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    return test_and_set_bit(id.val, mask->bitmap);                                                           \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_test_and_set(mask_t *mask, proc_id_t id)                                        \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    return __test_and_set_bit(id.val, mask->bitmap);                                                         \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_test_and_clear_atomic(mask_t *mask, proc_id_t id)                               \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    return test_and_clear_bit(id.val, mask->bitmap);                                                         \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_test_and_clear(mask_t *mask, proc_id_t id)                                      \
{                                                                                                            \
    UVM_ASSERT_MSG(id.val < (maxval), "id %u\n", id.val);                                                    \
                                                                                                             \
    return __test_and_clear_bit(id.val, mask->bitmap);                                                       \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_zero(mask_t *mask)                                                              \
{                                                                                                            \
    bitmap_zero(mask->bitmap, (maxval));                                                                     \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_empty(const mask_t *mask)                                                       \
{                                                                                                            \
    return bitmap_empty(mask->bitmap, (maxval));                                                             \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_copy(mask_t *dst, const mask_t *src)                                            \
{                                                                                                            \
    bitmap_copy(dst->bitmap, src->bitmap, (maxval));                                                         \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_and(mask_t *dst, const mask_t *src1, const mask_t *src2)                        \
{                                                                                                            \
    return bitmap_and(dst->bitmap, src1->bitmap, src2->bitmap, (maxval)) != 0;                               \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_or(mask_t *dst, const mask_t *src1, const mask_t *src2)                         \
{                                                                                                            \
    bitmap_or(dst->bitmap, src1->bitmap, src2->bitmap, (maxval));                                            \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_andnot(mask_t *dst, const mask_t *src1, const mask_t *src2)                     \
{                                                                                                            \
    return bitmap_andnot(dst->bitmap, src1->bitmap, src2->bitmap, (maxval));                                 \
}                                                                                                            \
                                                                                                             \
static void prefix_fn_mask##_xor(mask_t *dst, const mask_t *src1, const mask_t *src2)                        \
{                                                                                                            \
    bitmap_xor(dst->bitmap, src1->bitmap, src2->bitmap, (maxval));                                           \
}                                                                                                            \
                                                                                                             \
static proc_id_t prefix_fn_mask##_find_first_id(const mask_t *mask)                                          \
{                                                                                                            \
    return proc_id_ctor(find_first_bit(mask->bitmap, (maxval)));                                             \
}                                                                                                            \
                                                                                                             \
static proc_id_t prefix_fn_mask##_find_first_gpu_id(const mask_t *mask)                                      \
{                                                                                                            \
    return proc_id_ctor(find_next_bit(mask->bitmap, (maxval), UVM_PARENT_ID_GPU0_VALUE));                    \
}                                                                                                            \
                                                                                                             \
static proc_id_t prefix_fn_mask##_find_next_id(const mask_t *mask, proc_id_t min_id)                         \
{                                                                                                            \
    return proc_id_ctor(find_next_bit(mask->bitmap, (maxval), min_id.val));                                  \
}                                                                                                            \
                                                                                                             \
static proc_id_t prefix_fn_mask##_find_next_gpu_id(const mask_t *mask, proc_id_t min_gpu_id)                 \
{                                                                                                            \
    return proc_id_ctor(find_next_bit(mask->bitmap, (maxval), min_gpu_id.val));                              \
}                                                                                                            \
                                                                                                             \
static proc_id_t prefix_fn_mask##_find_first_unset_id(const mask_t *mask)                                    \
{                                                                                                            \
    return proc_id_ctor(find_first_zero_bit(mask->bitmap, (maxval)));                                        \
}                                                                                                            \
                                                                                                             \
static proc_id_t prefix_fn_mask##_find_next_unset_id(const mask_t *mask, proc_id_t min_id)                   \
{                                                                                                            \
    return proc_id_ctor(find_next_zero_bit(mask->bitmap, (maxval), min_id.val));                             \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_equal(const mask_t *mask_in1, const mask_t *mask_in2)                           \
{                                                                                                            \
    return bitmap_equal(mask_in1->bitmap, mask_in2->bitmap, (maxval)) != 0;                                  \
}                                                                                                            \
                                                                                                             \
static bool prefix_fn_mask##_subset(const mask_t *subset, const mask_t *mask)                                \
{                                                                                                            \
    return bitmap_subset(subset->bitmap, mask->bitmap, (maxval)) != 0;                                       \
}                                                                                                            \
                                                                                                             \
static NvU32 prefix_fn_mask##_get_count(const mask_t *mask)                                                  \
{                                                                                                            \
    return bitmap_weight(mask->bitmap, (maxval));                                                            \
}                                                                                                            \
                                                                                                             \
static NvU32 prefix_fn_mask##_get_gpu_count(const mask_t *mask)                                              \
{                                                                                                            \
    NvU32 gpu_count = prefix_fn_mask##_get_count(mask);                                                      \
                                                                                                             \
    if (prefix_fn_mask##_test(mask, proc_id_ctor(UVM_PARENT_ID_CPU_VALUE)))                                  \
        --gpu_count;                                                                                         \
                                                                                                             \
    return gpu_count;                                                                                        \
}

typedef struct
{
    NvU32 val;
} uvm_parent_processor_id_t;

typedef struct
{
    NvU32 val;
} uvm_processor_id_t;

typedef uvm_parent_processor_id_t uvm_parent_gpu_id_t;
typedef uvm_processor_id_t uvm_gpu_id_t;

// Static value assigned to the CPU
#define UVM_PARENT_ID_CPU_VALUE      0
#define UVM_PARENT_ID_GPU0_VALUE     (UVM_PARENT_ID_CPU_VALUE + 1)

// ID values for the CPU and first GPU, respectively; the values for both types
// of IDs must match to enable sharing of UVM_PROCESSOR_MASK().
#define UVM_ID_CPU_VALUE  UVM_PARENT_ID_CPU_VALUE
#define UVM_ID_GPU0_VALUE UVM_PARENT_ID_GPU0_VALUE

// Maximum number of GPUs/processors that can be represented with the id types
#define UVM_PARENT_ID_MAX_GPUS       NV_MAX_DEVICES
#define UVM_PARENT_ID_MAX_PROCESSORS (UVM_PARENT_ID_MAX_GPUS + 1)

#define UVM_ID_MAX_GPUS       (UVM_PARENT_ID_MAX_GPUS * UVM_PARENT_ID_MAX_SUB_PROCESSORS)
#define UVM_ID_MAX_PROCESSORS (UVM_ID_MAX_GPUS + 1)
#define UVM_MAX_UNIQUE_GPU_PAIRS SUM_FROM_0_TO_N(UVM_ID_MAX_GPUS - 1)

#define UVM_PARENT_ID_CPU     ((uvm_parent_processor_id_t) { .val = UVM_PARENT_ID_CPU_VALUE })
#define UVM_PARENT_ID_INVALID ((uvm_parent_processor_id_t) { .val = UVM_PARENT_ID_MAX_PROCESSORS })
#define UVM_ID_CPU     ((uvm_processor_id_t) { .val = UVM_ID_CPU_VALUE })
#define UVM_ID_INVALID ((uvm_processor_id_t) { .val = UVM_ID_MAX_PROCESSORS })

#define UVM_PARENT_ID_CHECK_BOUNDS(id) UVM_ASSERT_MSG(id.val <= UVM_PARENT_ID_MAX_PROCESSORS, "id %u\n", id.val)

#define UVM_ID_CHECK_BOUNDS(id) UVM_ASSERT_MSG(id.val <= UVM_ID_MAX_PROCESSORS, "id %u\n", id.val)

#define UVM_SUB_PROCESSOR_INDEX_CHECK_BOUNDS(sub_index) \
    UVM_ASSERT_MSG((sub_index) < UVM_PARENT_ID_MAX_SUB_PROCESSORS, "sub_index %u\n", (sub_index))

static int uvm_parent_id_cmp(uvm_parent_processor_id_t id1, uvm_parent_processor_id_t id2)
{
    UVM_PARENT_ID_CHECK_BOUNDS(id1);
    UVM_PARENT_ID_CHECK_BOUNDS(id2);

    return UVM_CMP_DEFAULT(id1.val, id2.val);
}

static bool uvm_parent_id_equal(uvm_parent_processor_id_t id1, uvm_parent_processor_id_t id2)
{
    UVM_PARENT_ID_CHECK_BOUNDS(id1);
    UVM_PARENT_ID_CHECK_BOUNDS(id2);

    return id1.val == id2.val;
}

static int uvm_id_cmp(uvm_processor_id_t id1, uvm_processor_id_t id2)
{
    UVM_ID_CHECK_BOUNDS(id1);
    UVM_ID_CHECK_BOUNDS(id2);

    return UVM_CMP_DEFAULT(id1.val, id2.val);
}

static bool uvm_id_equal(uvm_processor_id_t id1, uvm_processor_id_t id2)
{
    UVM_ID_CHECK_BOUNDS(id1);
    UVM_ID_CHECK_BOUNDS(id2);

    return id1.val == id2.val;
}

#define UVM_PARENT_ID_IS_CPU(id)     uvm_parent_id_equal(id, UVM_PARENT_ID_CPU)
#define UVM_PARENT_ID_IS_INVALID(id) uvm_parent_id_equal(id, UVM_PARENT_ID_INVALID)
#define UVM_PARENT_ID_IS_VALID(id)   (!UVM_PARENT_ID_IS_INVALID(id))
#define UVM_PARENT_ID_IS_GPU(id)     (!UVM_PARENT_ID_IS_CPU(id) && !UVM_PARENT_ID_IS_INVALID(id))

#define UVM_ID_IS_CPU(id)     uvm_id_equal(id, UVM_ID_CPU)
#define UVM_ID_IS_INVALID(id) uvm_id_equal(id, UVM_ID_INVALID)
#define UVM_ID_IS_VALID(id)   (!UVM_ID_IS_INVALID(id))
#define UVM_ID_IS_GPU(id)     (!UVM_ID_IS_CPU(id) && !UVM_ID_IS_INVALID(id))

static uvm_parent_processor_id_t uvm_parent_id_from_value(NvU32 val)
{
    uvm_parent_processor_id_t ret = { .val = val };

    UVM_PARENT_ID_CHECK_BOUNDS(ret);

    return ret;
}

static uvm_parent_gpu_id_t uvm_parent_gpu_id_from_value(NvU32 val)
{
    uvm_parent_gpu_id_t ret = uvm_parent_id_from_value(val);

    UVM_ASSERT(!UVM_PARENT_ID_IS_CPU(ret));

    return ret;
}

static uvm_processor_id_t uvm_id_from_value(NvU32 val)
{
    uvm_processor_id_t ret = { .val = val };

    UVM_ID_CHECK_BOUNDS(ret);

    return ret;
}

static uvm_gpu_id_t uvm_gpu_id_from_value(NvU32 val)
{
    uvm_gpu_id_t ret = uvm_id_from_value(val);

    UVM_ASSERT(!UVM_ID_IS_CPU(ret));

    return ret;
}

// Create a parent GPU id from the given parent GPU id index (previously
// obtained via uvm_parent_id_gpu_index)
static uvm_parent_gpu_id_t uvm_parent_gpu_id_from_index(NvU32 index)
{
    return uvm_parent_gpu_id_from_value(index + UVM_PARENT_ID_GPU0_VALUE);
}

static uvm_parent_processor_id_t uvm_parent_id_next(uvm_parent_processor_id_t id)
{
    ++id.val;

    UVM_PARENT_ID_CHECK_BOUNDS(id);

    return id;
}

static uvm_parent_gpu_id_t uvm_parent_gpu_id_next(uvm_parent_gpu_id_t id)
{
    UVM_ASSERT(UVM_PARENT_ID_IS_GPU(id));

    ++id.val;

    UVM_PARENT_ID_CHECK_BOUNDS(id);

    return id;
}

// Same as uvm_parent_gpu_id_from_index but for uvm_processor_id_t
static uvm_gpu_id_t uvm_gpu_id_from_index(NvU32 index)
{
    return uvm_gpu_id_from_value(index + UVM_ID_GPU0_VALUE);
}

static uvm_processor_id_t uvm_id_next(uvm_processor_id_t id)
{
    ++id.val;

    UVM_ID_CHECK_BOUNDS(id);

    return id;
}

static uvm_gpu_id_t uvm_gpu_id_next(uvm_gpu_id_t id)
{
    UVM_ASSERT(UVM_ID_IS_GPU(id));

    ++id.val;

    UVM_ID_CHECK_BOUNDS(id);

    return id;
}

// This function returns the numerical value within
// [0, UVM_PARENT_ID_MAX_PROCESSORS) of the given parent processor id.
static NvU32 uvm_parent_id_value(uvm_parent_processor_id_t id)
{
    UVM_ASSERT(UVM_PARENT_ID_IS_VALID(id));

    return id.val;
}

// This function returns the numerical value within
// [0, UVM_ID_MAX_PROCESSORS) of the given processor id
static NvU32 uvm_id_value(uvm_processor_id_t id)
{
    UVM_ASSERT(UVM_ID_IS_VALID(id));

    return id.val;
}

// This function returns the index of the given GPU id within the GPU id space
// [0, UVM_PARENT_ID_MAX_GPUS)
static NvU32 uvm_parent_id_gpu_index(uvm_parent_gpu_id_t id)
{
    UVM_ASSERT(UVM_PARENT_ID_IS_GPU(id));

    return id.val - UVM_PARENT_ID_GPU0_VALUE;
}

// This function returns the index of the given GPU id within the GPU id space
// [0, UVM_ID_MAX_GPUS)
static NvU32 uvm_id_gpu_index(const uvm_gpu_id_t id)
{
    UVM_ASSERT(UVM_ID_IS_GPU(id));

    return id.val - UVM_ID_GPU0_VALUE;
}

static NvU32 uvm_id_gpu_index_from_parent_gpu_id(const uvm_parent_gpu_id_t id)
{
    UVM_ASSERT(UVM_PARENT_ID_IS_GPU(id));

    return uvm_parent_id_gpu_index(id) * UVM_PARENT_ID_MAX_SUB_PROCESSORS;
}

// This function returns the numerical value of the parent processor ID from the
// given processor id.
static NvU32 uvm_parent_id_value_from_processor_id(const uvm_processor_id_t id)
{
    if (UVM_ID_IS_CPU(id))
        return UVM_PARENT_ID_CPU_VALUE;

    return (uvm_id_gpu_index(id) / UVM_PARENT_ID_MAX_SUB_PROCESSORS) + UVM_PARENT_ID_GPU0_VALUE;
}

static NvU32 uvm_parent_id_gpu_index_from_gpu_id(const uvm_gpu_id_t id)
{
    UVM_ASSERT(UVM_ID_IS_GPU(id));

    return uvm_id_gpu_index(id) / UVM_PARENT_ID_MAX_SUB_PROCESSORS;
}

static uvm_gpu_id_t uvm_gpu_id_from_parent_gpu_id(const uvm_parent_gpu_id_t id)
{
    UVM_ASSERT(UVM_PARENT_ID_IS_GPU(id));

    return uvm_gpu_id_from_index(uvm_id_gpu_index_from_parent_gpu_id(id));
}

static uvm_gpu_id_t uvm_gpu_id_from_sub_processor_index(NvU32 index, NvU32 sub_index)
{
    UVM_ASSERT(index < UVM_PARENT_ID_MAX_GPUS);
    UVM_SUB_PROCESSOR_INDEX_CHECK_BOUNDS(sub_index);

    return uvm_gpu_id_from_index(index * UVM_PARENT_ID_MAX_SUB_PROCESSORS + sub_index);
}

static uvm_gpu_id_t uvm_gpu_id_from_sub_processor(uvm_parent_gpu_id_t id, NvU32 sub_index)
{
    return uvm_gpu_id_from_sub_processor_index(uvm_parent_id_gpu_index(id), sub_index);
}

static uvm_parent_gpu_id_t uvm_parent_gpu_id_from_gpu_id(const uvm_gpu_id_t id)
{
    UVM_ASSERT(UVM_ID_IS_GPU(id));

    return uvm_parent_gpu_id_from_index(uvm_parent_id_gpu_index_from_gpu_id(id));
}

static NvU32 uvm_id_sub_processor_index(const uvm_gpu_id_t id)
{
    return uvm_id_gpu_index(id) % UVM_PARENT_ID_MAX_SUB_PROCESSORS;
}

UVM_PROCESSOR_MASK(uvm_parent_processor_mask_t,       \
                   uvm_parent_processor_mask,         \
                   UVM_PARENT_ID_MAX_PROCESSORS,      \
                   uvm_parent_processor_id_t,         \
                   uvm_parent_id_from_value)

UVM_PROCESSOR_MASK(uvm_processor_mask_t,              \
                   uvm_processor_mask,                \
                   UVM_ID_MAX_PROCESSORS,             \
                   uvm_processor_id_t,                \
                   uvm_id_from_value)

extern const uvm_processor_mask_t g_uvm_processor_mask_cpu;
extern const uvm_processor_mask_t g_uvm_processor_mask_empty;

// This is similar to uvm_parent_processor_mask_t and uvm_processor_mask_t
// but defined as a NvU8 in order to save memory since DECLARE_BITMAP() uses
// unsigned long. It also means we need to define our own bitops.
// Note that these are not atomic operations.
typedef struct
{
    NvU8 bitmap;
} uvm_sub_processor_mask_t;

static bool uvm_sub_processor_mask_test(const uvm_sub_processor_mask_t *mask, NvU32 sub_index)
{
    UVM_SUB_PROCESSOR_INDEX_CHECK_BOUNDS(sub_index);

    return mask->bitmap & (1 << sub_index);
}

static void uvm_sub_processor_mask_set(uvm_sub_processor_mask_t *mask, NvU32 sub_index)
{
    UVM_SUB_PROCESSOR_INDEX_CHECK_BOUNDS(sub_index);

    mask->bitmap |= 1 << sub_index;
}

static void uvm_sub_processor_mask_clear(uvm_sub_processor_mask_t *mask, NvU32 sub_index)
{
    UVM_SUB_PROCESSOR_INDEX_CHECK_BOUNDS(sub_index);

    mask->bitmap &= ~(1 << sub_index);
}

static bool uvm_sub_processor_mask_test_and_set(uvm_sub_processor_mask_t *mask, NvU32 sub_index)
{
    bool result = uvm_sub_processor_mask_test(mask, sub_index);

    if (!result)
        uvm_sub_processor_mask_set(mask, sub_index);

    return result;
}

static bool uvm_sub_processor_mask_test_and_clear(uvm_sub_processor_mask_t *mask, NvU32 sub_index)
{
    bool result = uvm_sub_processor_mask_test(mask, sub_index);

    if (result)
        uvm_sub_processor_mask_clear(mask, sub_index);

    return result;
}

static void uvm_sub_processor_mask_zero(uvm_sub_processor_mask_t *mask)
{
    mask->bitmap = 0;
}

static bool uvm_sub_processor_mask_empty(const uvm_sub_processor_mask_t *mask)
{
    return mask->bitmap == 0;
}

static NvU32 uvm_sub_processor_mask_get_count(const uvm_sub_processor_mask_t *mask)
{
    return hweight8(mask->bitmap);
}

// Like uvm_processor_mask_subset() but ignores the CPU in the subset mask.
// Returns whether the GPUs in subset are a subset of the GPUs in mask.
bool uvm_processor_mask_gpu_subset(const uvm_processor_mask_t *subset,
                                   const uvm_processor_mask_t *mask);

// Compress a uvm_processor_mask_t down to a uvm_parent_processor_mask_t
// by only copying the first subprocessor bit and ignoring the CPU bit.
void uvm_parent_gpus_from_processor_mask(uvm_parent_processor_mask_t *parent_mask,
                                         const uvm_processor_mask_t *mask);

#define for_each_parent_id_in_mask(id, mask)                                                          \
    for ((id) = uvm_parent_processor_mask_find_first_id(mask);                                        \
         UVM_PARENT_ID_IS_VALID(id);                                                                  \
         (id) = uvm_parent_processor_mask_find_next_id((mask), uvm_parent_id_next(id)))

#define for_each_parent_gpu_id_in_mask(gpu_id, mask)                                                  \
    for ((gpu_id) = uvm_parent_processor_mask_find_first_gpu_id((mask));                              \
         UVM_PARENT_ID_IS_VALID(gpu_id);                                                              \
         (gpu_id) = uvm_parent_processor_mask_find_next_id((mask), uvm_parent_gpu_id_next(gpu_id)))

#define for_each_id_in_mask(id, mask)                                                          \
    for ((id) = uvm_processor_mask_find_first_id(mask);                                        \
         UVM_ID_IS_VALID(id);                                                                  \
         (id) = uvm_processor_mask_find_next_id((mask), uvm_id_next(id)))

#define for_each_gpu_id_in_mask(gpu_id, mask)                                                  \
    for ((gpu_id) = uvm_processor_mask_find_first_gpu_id((mask));                              \
         UVM_ID_IS_VALID(gpu_id);                                                              \
         (gpu_id) = uvm_processor_mask_find_next_id((mask), uvm_gpu_id_next(gpu_id)))

// Helper to iterate over all valid parent gpu ids.
#define for_each_parent_gpu_id(i)  \
    for (i = uvm_parent_gpu_id_from_value(UVM_PARENT_ID_GPU0_VALUE); UVM_PARENT_ID_IS_VALID(i); i = uvm_parent_gpu_id_next(i))

// Helper to iterate over all valid gpu ids.
#define for_each_gpu_id(i)  \
    for (i = uvm_gpu_id_from_value(UVM_ID_GPU0_VALUE); UVM_ID_IS_VALID(i); i = uvm_gpu_id_next(i))

// Helper to iterate over all gpu ids in a given parent id.
#define for_each_sub_processor_id_in_parent_gpu(i, id) \
    for (i = uvm_gpu_id_from_parent_gpu_id(id); \
         UVM_ID_IS_VALID(i) && \
         (uvm_id_value(i) < uvm_id_value(uvm_gpu_id_from_parent_gpu_id(id)) + UVM_PARENT_ID_MAX_SUB_PROCESSORS); \
         i = uvm_gpu_id_next(i))

// Helper to iterate over all sub processor indexes.
#define for_each_sub_processor_index(sub_index) \
    for ((sub_index) = 0; (sub_index) < UVM_PARENT_ID_MAX_SUB_PROCESSORS; (sub_index)++)

static NvU32 uvm_sub_processor_mask_find_first_index(const uvm_sub_processor_mask_t *mask)
{
    unsigned long bitmap = mask->bitmap;

    return find_first_bit(&bitmap, UVM_PARENT_ID_MAX_SUB_PROCESSORS);
}

static NvU32 uvm_sub_processor_mask_find_next_index(const uvm_sub_processor_mask_t *mask, NvU32 min_index)
{
    unsigned long bitmap = mask->bitmap;

    return find_next_bit(&bitmap, UVM_PARENT_ID_MAX_SUB_PROCESSORS, min_index);
}

// Helper to iterate over all sub processor indexes in a given mask.
#define for_each_sub_processor_index_in_mask(sub_index, sub_mask)                           \
    for ((sub_index) = uvm_sub_processor_mask_find_first_index((sub_mask));                 \
         (sub_index) < UVM_PARENT_ID_MAX_SUB_PROCESSORS;                                    \
         (sub_index) = uvm_sub_processor_mask_find_next_index((sub_mask), (sub_index) + 1))

// Helper to iterate over all valid processor ids.
#define for_each_id(i) for (i = UVM_ID_CPU; UVM_ID_IS_VALID(i); i = uvm_id_next(i))

// Find the node in mask with the shorted distance (as returned by
// node_distance) for src.
// Note that the search is inclusive of src.
// If mask has no bits set, NUMA_NO_NODE is returned.
int uvm_find_closest_node_mask(int src, const nodemask_t *mask);

// Iterate over all nodes in mask with increasing distance from src.
// Note that this iterator is destructive of the mask.
#define for_each_closest_uvm_node(nid, src, mask)                                                                      \
    for ((nid) = uvm_find_closest_node_mask((src), &(mask));                                                           \
         (nid) != NUMA_NO_NODE;                                                                                        \
         node_clear((nid), (mask)), (nid) = uvm_find_closest_node_mask((src), &(mask)))

#define for_each_possible_uvm_node(nid) for_each_node_mask((nid), node_possible_map)

// Compare two NUMA node IDs for equality.
// The main purpose of this helper is to correctly compare
// in situations when the system has only a single NUMA node
// (which is also the case when NUMA support is disabled).
bool uvm_numa_id_eq(int nid0, int nid1);

static bool uvm_uuid_eq(const NvProcessorUuid *uuid0, const NvProcessorUuid *uuid1)
{
    return memcmp(uuid0, uuid1, sizeof(*uuid0)) == 0;
}

// Copies a UUID from source (src) to destination (dst).
static void uvm_uuid_copy(NvProcessorUuid *dst, const NvProcessorUuid *src)
{
    memcpy(dst, src, sizeof(*dst));
}

static inline NvBool uvm_uuid_is_cpu(const NvProcessorUuid *uuid)
{
    return memcmp(uuid, &NV_PROCESSOR_UUID_CPU_DEFAULT, sizeof(*uuid)) == 0;
}

// Dynamic uvm_processor_mask_t object allocation/maintenance.
NV_STATUS uvm_processor_mask_cache_init(void);
void uvm_processor_mask_cache_exit(void);
uvm_processor_mask_t *uvm_processor_mask_cache_alloc(void);
void uvm_processor_mask_cache_free(uvm_processor_mask_t *mask);

#endif
