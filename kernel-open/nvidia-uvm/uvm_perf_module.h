/*******************************************************************************
    Copyright (c) 2016-2022 NVIDIA Corporation

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

#ifndef __UVM_PERF_MODULE_H__
#define __UVM_PERF_MODULE_H__

#include "uvm_common.h"
#include "uvm_forward_decl.h"
#include "uvm_perf_events.h"

// uvm_perf_module is an abstraction that provide the basic infrastructure to
// develop a performance heuristics module for UVM. A perf module defines
// callbacks for a number of perf events, and provides a placeholder to store
// data, that can be used by the module to create information regarding events
// to provide performance hints. Data is not created on a specific point of
// time, modules are in charge of allocating data the first time they need it.
// If data allocation fails, the module must keep being functional (it will
// just not give performance hints). Perf modules have a global description but
// they are loaded/unloaded within each va_space. When a module is unloaded it
// sends the destroy events to each block/range in the va space (and for the
// va_space itself), so that the data allocated by the module is freed.

// enum that contains the main sub-modules that handle performance
// optimizations in UVM.
//
// - UVM_PERF_MODULE_TYPE_THRASHING: detects memory thrashing scenarios and
// provides thrashing prevention mechanisms
// - UVM_PERF_MODULE_TYPE_ACCESS_COUNTERS: migrates memory using access counter
// notifications
typedef enum
{
    UVM_PERF_MODULE_FIRST_TYPE     = 0,

    UVM_PERF_MODULE_TYPE_TEST      = UVM_PERF_MODULE_FIRST_TYPE,
    UVM_PERF_MODULE_TYPE_THRASHING,
    UVM_PERF_MODULE_TYPE_ACCESS_COUNTERS,

    UVM_PERF_MODULE_TYPE_COUNT,
} uvm_perf_module_type_t;

struct uvm_perf_module_struct
{
    const char *name;

    uvm_perf_module_type_t type;

    // Array of event callbacks
    uvm_perf_event_callback_t callbacks[UVM_PERF_EVENT_COUNT];
};

// Placeholder to store module-allocated data
typedef struct
{
    // Pointer to module-allocated data
    void *data;
} uvm_perf_module_data_desc_t;

// Event callback descriptor
typedef struct
{
    uvm_perf_event_t event_id;

    uvm_perf_event_callback_t callback;
} uvm_perf_module_event_callback_desc_t;

// Obtain the descriptor of module-allocated data for the given module type
static inline void *uvm_perf_module_type_data(uvm_perf_module_data_desc_t *modules_data, uvm_perf_module_type_t type)
{
    UVM_ASSERT(type >= UVM_PERF_MODULE_FIRST_TYPE && type < UVM_PERF_MODULE_TYPE_COUNT);

    return modules_data[type].data;
}

void uvm_perf_module_type_set_data(uvm_perf_module_data_desc_t *modules_data, void *data, uvm_perf_module_type_t type);

// Clear data after the module has freed it
void uvm_perf_module_type_unset_data(uvm_perf_module_data_desc_t *modules_data, uvm_perf_module_type_t type);

// Register all the callbacks defined by the module in uvm_perf_module_init. Caller must hold va_space lock in write
// mode
NV_STATUS uvm_perf_module_load(uvm_perf_module_t *module, uvm_va_space_t *va_space);

// Remove data allocated by the module for all va_block/va_range in the va_space, and in the va_space struct itself
// It also unregisters all the callbacks defined by the module. Caller must hold va_space lock in write mode
void uvm_perf_module_unload(uvm_perf_module_t *module, uvm_va_space_t *va_space);

// Obtain the module loaded in the va_space for the given perf module type. Caller must hold va_space lock at least in
// read mode
uvm_perf_module_t *uvm_perf_module_for_type(uvm_va_space_t *va_space, uvm_perf_module_type_t type);

// Initialize a performance heuristics module. This must be called once, before any call to uvm_perf_module_load using
// this module
void uvm_perf_module_init(const char *name, uvm_perf_module_type_t type,
                          uvm_perf_module_event_callback_desc_t *callbacks, size_t callback_count,
                          uvm_perf_module_t *module);

#endif
