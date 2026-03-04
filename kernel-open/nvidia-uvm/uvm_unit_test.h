/*******************************************************************************
    Copyright (c) 2015-2019 NVidia Corporation

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

#ifndef __UVM_UNIT_TEST_H__
#define __UVM_UNIT_TEST_H__

#include "nvstatus.h"
#include "uvm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// UvmGetGlobalStatePointer
//
// This allows an application and any number of shared libraries to effectively
// share an instance of UVM, even though it is a static library.
//
// Note that calling this function causes initialization of the global state
// if it has not already occurred.
//
//-----------------------------------------------------------------------------
UvmGlobalState *UvmGetGlobalStatePointer(void);

//-----------------------------------------------------------------------------
// UvmSetGlobalStatePointer
//
// This allows an instance of the UVM user mode library to share state with
// another, making it behave as a single instance. Because it is a static
// library, this is needed for an application and a shared library, or multiple
// shared libraries, to share a UVM instance. It is only used in testing.
//
// This method must be called before the global state has been initialized, so
// effectively before any other call to this library.  The global state used by
// the library is fixed once it has been set.
//
// Error codes:
//     NV_ERR_INVALID_STATE:
//         The global state has already been initialized.
//
//     NV_ERR_INVALID_ARGUMENT:
//         pGlobalState is NULL.
//
//-----------------------------------------------------------------------------
NV_STATUS UvmSetGlobalStatePointer(UvmGlobalState *pGlobalState);

#ifdef __cplusplus
}
#endif

#endif // __UVM_UNIT_TEST_H__
