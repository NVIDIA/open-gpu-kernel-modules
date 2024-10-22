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

#include "uvm_perf_events.h"
#include "uvm_perf_module.h"
#include "uvm_va_block.h"
#include "uvm_va_range.h"
#include "uvm_va_space.h"

void uvm_perf_module_type_set_data(uvm_perf_module_data_desc_t *modules_data, void *data, uvm_perf_module_type_t type)
{
    UVM_ASSERT(type >= 0 && type < UVM_PERF_MODULE_TYPE_COUNT);
    UVM_ASSERT(data);
    // Data can only be created once per module
    UVM_ASSERT(!modules_data[type].data);

    modules_data[type].data = data;
}

void uvm_perf_module_type_unset_data(uvm_perf_module_data_desc_t *modules_data, uvm_perf_module_type_t type)
{
    UVM_ASSERT(type >= 0 && type < UVM_PERF_MODULE_TYPE_COUNT);
    // Data should have been previously set
    UVM_ASSERT(modules_data[type].data);

    memset(&modules_data[type], 0, sizeof(modules_data[type]));
}

NV_STATUS uvm_perf_module_load(uvm_perf_module_t *module, uvm_va_space_t *va_space)
{
    NV_STATUS status;
    size_t i, j;

    uvm_assert_rwsem_locked_write(&va_space->lock);
    UVM_ASSERT(va_space->perf_modules[module->type] == NULL);

    for (i = 0; i < UVM_PERF_EVENT_COUNT; ++i) {
        if (module->callbacks[i] != NULL) {
            status = uvm_perf_register_event_callback(&va_space->perf_events, i, module->callbacks[i]);
            if (status != NV_OK)
                goto error;
        }
    }

    va_space->perf_modules[module->type] = module;

    return NV_OK;

error:
    for (j = 0; j < i; ++j) {
        if (module->callbacks[j] != NULL)
            uvm_perf_unregister_event_callback(&va_space->perf_events, j, module->callbacks[j]);
    }

    return status;
}

void uvm_perf_module_unload(uvm_perf_module_t *module, uvm_va_space_t *va_space)
{
    uvm_perf_event_data_t event_data;
    uvm_va_range_managed_t *managed_range;
    uvm_va_block_t *block;
    size_t i;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    if (!va_space->perf_modules[module->type])
        return;

    event_data.module_unload.module = module;

    // Iterate over all managed ranges/va_blocks in the va_space
    uvm_for_each_va_range_managed(managed_range, va_space) {

        for_each_va_block_in_va_range(managed_range, block) {
            uvm_mutex_lock(&block->lock);

            // Notify a fake va_block destruction to destroy the module-allocated data
            event_data.module_unload.block = block;
            event_data.module_unload.range = NULL;
            uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_MODULE_UNLOAD, &event_data);

            uvm_mutex_unlock(&block->lock);
        }
        // Notify a fake va_range destruction to destroy the module-allocated data
        event_data.module_unload.block = NULL;
        event_data.module_unload.range = managed_range;
        uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_MODULE_UNLOAD, &event_data);
    }

    for (i = 0; i < UVM_PERF_EVENT_COUNT; ++i) {
        if (module->callbacks[i] != NULL)
            uvm_perf_unregister_event_callback(&va_space->perf_events, i, module->callbacks[i]);
    }

    va_space->perf_modules[module->type] = NULL;
}

uvm_perf_module_t *uvm_perf_module_for_type(uvm_va_space_t *va_space, uvm_perf_module_type_t type)
{
    uvm_assert_rwsem_locked(&va_space->lock);

    return va_space->perf_modules[type];
}


void uvm_perf_module_init(const char *name, uvm_perf_module_type_t type,
                          uvm_perf_module_event_callback_desc_t *callbacks, size_t callback_count,
                          uvm_perf_module_t *module)
{
    size_t i;

    UVM_ASSERT(callbacks);
    UVM_ASSERT(module);
    UVM_ASSERT(callback_count <= UVM_PERF_EVENT_COUNT);

    memset(module->callbacks, 0, sizeof(module->callbacks));

    // Register all the given callbacks
    for (i = 0; i < callback_count; ++i) {
        UVM_ASSERT(callbacks[i].event_id >= 0 && callbacks[i].event_id < UVM_PERF_EVENT_COUNT);
        UVM_ASSERT(module->callbacks[callbacks[i].event_id] == NULL);

        module->callbacks[callbacks[i].event_id] = callbacks[i].callback;
    }

    module->name = name;
    module->type = type;
}
