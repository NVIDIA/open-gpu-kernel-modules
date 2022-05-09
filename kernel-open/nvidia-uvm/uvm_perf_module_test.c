/*******************************************************************************
    Copyright (c) 2016-2019 NVIDIA Corporation

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

#include "uvm_kvmalloc.h"
#include "uvm_perf_module.h"
#include "uvm_va_block.h"
#include "uvm_va_space.h"
#include "uvm_test.h"

static const uvm_perf_module_type_t module1_type = UVM_PERF_MODULE_TYPE_TEST;
typedef int module1_data_type_t;

static const uvm_perf_module_type_t module2_type = UVM_PERF_MODULE_TYPE_TEST;
typedef struct {
    int value[4];
} module2_data_type_t;

static uvm_perf_module_t module1;
static uvm_perf_module_t module2;

// Convenience function that obtains the data for the given module, and allocates it if it does not exist yet
static void *get_or_alloc_data(uvm_perf_module_data_desc_t *modules_data, uvm_perf_module_t *module, size_t type_size)
{
   void *data_ret;

   data_ret = uvm_perf_module_type_data(modules_data, module->type);
   if (!data_ret) {
       data_ret = uvm_kvmalloc_zero(type_size);
       if (data_ret)
           uvm_perf_module_type_set_data(modules_data, data_ret, module->type);
   }
   return data_ret;
}

// Block destruction callback for module1
static void module1_destroy(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    uvm_va_block_t *va_block;
    void *data;

    if (event_id == UVM_PERF_EVENT_BLOCK_DESTROY) {
        va_block = event_data->block_destroy.block;
    }
    else {
        if (event_data->module_unload.module != &module1 || !event_data->module_unload.block)
            return;

        va_block = event_data->module_unload.block;
    }

    data = uvm_perf_module_type_data(va_block->perf_modules_data, module1.type);

    if (data) {
        uvm_kvfree(data);
        uvm_perf_module_type_unset_data(va_block->perf_modules_data, module1.type);
    }
}

// Page fault callback for module1
static void module1_fault(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    module1_data_type_t *data;
    uvm_va_block_t *va_block;

    va_block = event_data->fault.block;

    data = get_or_alloc_data(va_block->perf_modules_data, &module1, sizeof(*data));
    if (!data)
        return;

    ++(*data);
}

// Block destruction callback for module2
static void module2_destroy(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    void *data;
    uvm_va_block_t *va_block;

    if (event_id == UVM_PERF_EVENT_BLOCK_DESTROY) {
        va_block = event_data->block_destroy.block;
    }
    else {
        if (event_data->module_unload.module != &module2 || !event_data->module_unload.block)
            return;

        va_block = event_data->module_unload.block;
    }

    data = uvm_perf_module_type_data(va_block->perf_modules_data, module2.type);
    if (data) {
        uvm_kvfree(data);
        uvm_perf_module_type_unset_data(va_block->perf_modules_data, module2.type);
    }
}

// Page fault callback for module2
static void module2_fault(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    module2_data_type_t *data;
    uvm_va_block_t *va_block;

    va_block = event_data->fault.block;

    data = get_or_alloc_data(va_block->perf_modules_data, &module2, sizeof(*data));
    if (!data)
        return;

    data->value[0] += 1;
    data->value[1] += 2;
    data->value[2] += 3;
    data->value[3] += 4;
}

// This test:
// 1) Initializes modules named module1 and module2
// 2) Loads module1
// 3) Gets two va_blocks that must already exist in the given address (created using user-level calls)
// 4) Notifies some page faults on the blocks
// 5) Checks if the callbacks have executed correctly
// 6) Unloads module1
// 7) Checks that data allocated by module1 has been freed
// 8) Loads module2
// 9) Notifies some page faults on the blocks
// 10) Checks if the callbacks have executed correctly
// 11) Unloads module2
// 12) Checks that data allocated by module2 has been freed
static NV_STATUS test_module_replace(uvm_va_space_t *va_space, NvU64 addr)
{
    NV_STATUS status;
    uvm_perf_event_data_t event_data;
    void *module1_data;
    void *module2_data;

    uvm_va_block_t *block1, *block2;

    uvm_perf_module_event_callback_desc_t module1_callbacks[] = {
        { UVM_PERF_EVENT_BLOCK_DESTROY, module1_destroy },
        { UVM_PERF_EVENT_MODULE_UNLOAD, module1_destroy },
        { UVM_PERF_EVENT_FAULT, module1_fault },
    };

    uvm_perf_module_event_callback_desc_t module2_callbacks[] = {
        { UVM_PERF_EVENT_BLOCK_DESTROY, module2_destroy},
        { UVM_PERF_EVENT_MODULE_UNLOAD, module2_destroy },
        { UVM_PERF_EVENT_FAULT, module2_fault},
    };

    memset(&event_data, 0, sizeof(event_data));

    // Use CPU id to avoid triggering the GPU stats update code
    event_data.fault.proc_id = UVM_ID_CPU;

    uvm_perf_module_init("module1", module1_type, module1_callbacks, ARRAY_SIZE(module1_callbacks), &module1);
    uvm_perf_module_init("module2", module2_type, module2_callbacks, ARRAY_SIZE(module2_callbacks), &module2);

    // We take va_space write lock during all the test to prevent blocks from disappearing
    uvm_va_space_down_write(va_space);

    status = uvm_perf_module_load(&module1, va_space);
    TEST_CHECK_GOTO(status == NV_OK, fail_space_write_status);

    status = uvm_va_block_find(va_space, addr, &block1);
    TEST_CHECK_GOTO(status == NV_OK, fail_space_write_status);

    status = uvm_va_block_find(va_space, (addr + UVM_VA_BLOCK_SIZE) & ~(UVM_VA_BLOCK_SIZE - 1), &block2);
    TEST_CHECK_GOTO(status == NV_OK, fail_space_write_status);

    // Notify (fake) page fault on block1
    event_data.fault.block = block1;
    uvm_mutex_lock(&block1->lock);
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_FAULT, &event_data);
    uvm_mutex_unlock(&block1->lock);

    // Notify two (fake) page faults on block2
    event_data.fault.block = block2;
    uvm_mutex_lock(&block2->lock);
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_FAULT, &event_data);
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_FAULT, &event_data);
    uvm_mutex_unlock(&block2->lock);

    module1_data = uvm_perf_module_type_data(block1->perf_modules_data, module1.type);
    if (module1_data)
        TEST_CHECK_GOTO((*(module1_data_type_t *) module1_data) == 1, fail_space_write_invalid_state);
    module1_data = uvm_perf_module_type_data(block2->perf_modules_data, module1.type);
    if (module1_data)
        TEST_CHECK_GOTO((*(module1_data_type_t *) module1_data) == 2, fail_space_write_invalid_state);

    uvm_perf_module_unload(&module1, va_space);

    // Module-allocated data must be freed on module unload
    module1_data = uvm_perf_module_type_data(block1->perf_modules_data, module1.type);
    TEST_CHECK_GOTO(module1_data == NULL, fail_space_write_invalid_state);
    module1_data = uvm_perf_module_type_data(block2->perf_modules_data, module1.type);
    TEST_CHECK_GOTO(module1_data == NULL, fail_space_write_invalid_state);

    status = uvm_perf_module_load(&module2, va_space);
    TEST_CHECK_GOTO(status == NV_OK, fail_space_write_status);

    // Notify two (fake) page faults on block1
    event_data.fault.block = block1;
    uvm_mutex_lock(&block1->lock);
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_FAULT, &event_data);
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_FAULT, &event_data);
    uvm_mutex_unlock(&block1->lock);

    // Notify (fake) page fault on block2
    event_data.fault.block = block2;
    uvm_mutex_lock(&block2->lock);
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_FAULT, &event_data);
    uvm_mutex_unlock(&block2->lock);

    module2_data = uvm_perf_module_type_data(block1->perf_modules_data, module2.type);
    if (module2_data) {
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[0] == 2, fail_space_write_invalid_state);
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[1] == 4, fail_space_write_invalid_state);
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[2] == 6, fail_space_write_invalid_state);
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[3] == 8, fail_space_write_invalid_state);
    }

    module2_data = uvm_perf_module_type_data(block2->perf_modules_data, module2.type);
    if (module2_data) {
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[0] == 1, fail_space_write_invalid_state);
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[1] == 2, fail_space_write_invalid_state);
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[2] == 3, fail_space_write_invalid_state);
        TEST_CHECK_GOTO(((module2_data_type_t *) module2_data)->value[3] == 4, fail_space_write_invalid_state);
    }

    uvm_perf_module_unload(&module2, va_space);

    // Module-allocated data must be freed on module unload
    module2_data = uvm_perf_module_type_data(block2->perf_modules_data, module2.type);
    TEST_CHECK_GOTO(module2_data == NULL, fail_space_write_invalid_state);
    module2_data = uvm_perf_module_type_data(block2->perf_modules_data, module2.type);
    TEST_CHECK_GOTO(module2_data == NULL, fail_space_write_invalid_state);

    uvm_va_space_up_write(va_space);

    return NV_OK;

fail_space_write_status:
    uvm_va_space_up_write(va_space);

    return status;

fail_space_write_invalid_state:
    uvm_va_space_up_write(va_space);

    return NV_ERR_INVALID_STATE;
}

NV_STATUS uvm_test_perf_module_sanity(UVM_TEST_PERF_MODULE_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space;

    // Two blocks are needed
    if (params->range_size <= UVM_VA_BLOCK_SIZE) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    va_space = uvm_va_space_get(filp);

    status = test_module_replace(va_space, params->range_address);

fail:
    return status;
}
