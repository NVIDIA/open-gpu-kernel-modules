/*******************************************************************************
    Copyright (c) 2016 NVIDIA Corporation

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

#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_kvmalloc.h"
#include "uvm_test.h"

typedef enum
{
    ALLOC_TYPE_MALLOC,
    ALLOC_TYPE_ZALLOC,
    ALLOC_TYPE_REALLOC_NULL,
    ALLOC_TYPE_REALLOC_ZERO,
    ALLOC_TYPE_MAX
} alloc_type_t;

static NV_STATUS check_alloc(void *p, size_t size)
{
    if (size == 0) {
        TEST_CHECK_RET(p == ZERO_SIZE_PTR);
        TEST_CHECK_RET(uvm_kvsize(p) == 0);
    }
    else if (size <= UVM_KMALLOC_THRESHOLD) {
        TEST_CHECK_RET(!is_vmalloc_addr(p));

        // In theory it's possible to use kmalloc yet have ksize(p) be larger
        // than our arbitrary UVM_KMALLOC_THRESHOLD. In practice, as long as
        // UVM_KMALLOC_THRESHOLD is a multiple of PAGE_SIZE, that's highly
        // unlikely.
        TEST_CHECK_RET(uvm_kvsize(p) == ksize(p));
        TEST_CHECK_RET(uvm_kvsize(p) >= size);
    }
    else {
        TEST_CHECK_RET(is_vmalloc_addr(p));
        TEST_CHECK_RET(uvm_kvsize(p) == size);
    }

    return NV_OK;
}

static NV_STATUS test_uvm_kvmalloc(void)
{
    static const size_t sizes[] = {0, UVM_KMALLOC_THRESHOLD, UVM_KMALLOC_THRESHOLD + 1};
    uint8_t *p;
    uint8_t expected;
    size_t i, j, size;
    alloc_type_t alloc_type;

    for (i = 0; i < ARRAY_SIZE(sizes); i++) {
        size = sizes[i];
        for (alloc_type = 0; alloc_type < ALLOC_TYPE_MAX; alloc_type++) {
            switch (alloc_type) {
                case ALLOC_TYPE_MALLOC:
                    p = uvm_kvmalloc(size);
                    break;
                case ALLOC_TYPE_ZALLOC:
                    p = uvm_kvmalloc_zero(size);
                    break;
                case ALLOC_TYPE_REALLOC_NULL:
                    p = uvm_kvrealloc(NULL, size);
                    break;
                case ALLOC_TYPE_REALLOC_ZERO:
                    p = uvm_kvrealloc(ZERO_SIZE_PTR, size);
                    break;
                default:
                    UVM_ASSERT(0);
                    p = NULL;
            }
            if (!p)
                return NV_ERR_NO_MEMORY;

            // On failure, this macro returns and thus leaks the allocation. But
            // if the check fails, our allocation state is messed up so we can't
            // reasonably free the allocation anyway.
            MEM_NV_CHECK_RET(check_alloc(p, size), NV_OK);

            // Scribble on the allocation to make sure we don't crash
            if (alloc_type == ALLOC_TYPE_ZALLOC) {
                expected = 0;
            }
            else {
                expected = (uint8_t)(current->pid + i);
                memset(p, expected, size);
            }

            for (j = 0; j < size; j++) {
                if (p[j] != expected) {
                    UVM_TEST_PRINT("p[%zu] is 0x%x instead of expected value 0x%x\n", j, p[j], expected);
                    uvm_kvfree(p);
                    TEST_CHECK_RET(0);
                }
            }

            uvm_kvfree(p);
        }
    }

    return NV_OK;
}

static NV_STATUS test_uvm_kvrealloc(void)
{
    size_t i, j, k, old_size, new_size;
    uint8_t *old_p, *new_p;
    uint8_t expected = (uint8_t)current->pid;

    static const size_t sizes[] = {0,
                                   UVM_KMALLOC_THRESHOLD / 2,
                                   UVM_KMALLOC_THRESHOLD,
                                   UVM_KMALLOC_THRESHOLD + 1,
                                   2*UVM_KMALLOC_THRESHOLD};

    // uvm_kvrealloc(NULL, size) and uvm_kvrealloc(ZERO_SIZE_PTR, size) are
    // tested in test_uvm_alloc so we don't have to do them here.

    for (i = 0; i < ARRAY_SIZE(sizes); i++) {
        old_size = sizes[i];
        for (j = 0; j < ARRAY_SIZE(sizes); j++) {
            new_size = sizes[j];

            old_p = uvm_kvmalloc(old_size);
            if (!old_p)
                return NV_ERR_NO_MEMORY;
            MEM_NV_CHECK_RET(check_alloc(old_p, old_size), NV_OK);

            ++expected;
            memset(old_p, expected, old_size);

            new_p = uvm_kvrealloc(old_p, new_size);
            if (!new_p) {
                uvm_kvfree(old_p);
                return NV_ERR_NO_MEMORY;
            }

            // At this point, either new_p == old_p or old_p should have been
            // freed. In either case there's no need to free old_p.

            MEM_NV_CHECK_RET(check_alloc(new_p, new_size), NV_OK);

            // Make sure the data is still present
            for (k = 0; k < min(new_size, old_size); k++) {
                if (new_p[k] != expected) {
                    UVM_TEST_PRINT("new_p[%zu] is 0x%x instead of expected value 0x%x\n", k, new_p[k], expected);
                    uvm_kvfree(new_p);
                    TEST_CHECK_RET(0);
                }
            }

            // Exercise the free-via-realloc path
            TEST_CHECK_RET(uvm_kvrealloc(new_p, 0) == ZERO_SIZE_PTR);
        }
    }

    return NV_OK;
}

NV_STATUS uvm_test_kvmalloc(UVM_TEST_KVMALLOC_PARAMS *params, struct file *filp)
{
    NV_STATUS status = test_uvm_kvmalloc();
    if (status != NV_OK)
        return status;
    return test_uvm_kvrealloc();
}
