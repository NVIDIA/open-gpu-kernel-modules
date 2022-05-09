/*******************************************************************************
    Copyright (c) 2021 NVIDIA Corporation

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
#include "uvm_test.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_hmm.h"

NV_STATUS uvm_test_hmm_sanity(UVM_TEST_HMM_SANITY_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    struct mm_struct *mm;
    uvm_va_block_t *hmm_block = NULL;
    NV_STATUS status;

    mm = uvm_va_space_mm_retain(va_space);
    if (!mm)
        return NV_WARN_NOTHING_TO_DO;

    uvm_down_write_mmap_lock(mm);
    uvm_va_space_down_write(va_space);

    // TODO: Bug 3351822: [UVM-HMM] Remove temporary testing changes.
    // By default, HMM is enabled system wide but disabled per va_space.
    // This will initialize the va_space for HMM.
    status = uvm_hmm_va_space_initialize_test(va_space);
    if (status != NV_OK)
        goto out;

    uvm_va_space_up_write(va_space);
    uvm_up_write_mmap_lock(mm);

    uvm_down_read_mmap_lock(mm);
    uvm_va_space_down_read(va_space);

    // Try to create an HMM va_block to virtual address zero (NULL).
    // It should fail. There should be no VMA but a va_block for range
    // [0x0 0x1fffff] is possible.
    status = uvm_hmm_va_block_find_create(va_space, 0UL, NULL, &hmm_block);
    TEST_CHECK_GOTO(status == NV_ERR_INVALID_ADDRESS, done);

    // Try to create an HMM va_block which overlaps a UVM managed block.
    // It should fail.
    status = uvm_hmm_va_block_find_create(va_space, params->uvm_address, NULL, &hmm_block);
    TEST_CHECK_GOTO(status == NV_ERR_INVALID_ADDRESS, done);

    // Try to create an HMM va_block; it should succeed.
    status = uvm_hmm_va_block_find_create(va_space, params->hmm_address, NULL, &hmm_block);
    TEST_CHECK_GOTO(status == NV_OK, done);

    // Try to find an existing HMM va_block; it should succeed.
    status = uvm_hmm_va_block_find(va_space, params->hmm_address, &hmm_block);
    TEST_CHECK_GOTO(status == NV_OK, done);

done:
    uvm_va_space_up_read(va_space);
    uvm_up_read_mmap_lock(mm);
    uvm_va_space_mm_release(va_space);

    return status;

out:
    uvm_va_space_up_write(va_space);
    uvm_up_write_mmap_lock(mm);
    uvm_va_space_mm_release(va_space);

    return status;
}
