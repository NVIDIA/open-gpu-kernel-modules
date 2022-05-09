/*******************************************************************************
    Copyright (c) 2013 NVidia Corporation

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

#ifndef _UVM_LINUX_IOCTL_H
#define _UVM_LINUX_IOCTL_H

#include "uvm_ioctl.h"

// This ioctl must be the first operation performed on the UVM file descriptor
// after opening it. Until this ioctl is made, the UVM file descriptor is
// inoperable: all other ioctls will return NV_ERR_ILLEGAL_ACTION and mmap will
// return EBADFD.
#define UVM_INITIALIZE                                                0x30000001

typedef struct
{
    NvU64     flags     NV_ALIGN_BYTES(8); // IN
    NV_STATUS rmStatus;                    // OUT
} UVM_INITIALIZE_PARAMS;

#define UVM_DEINITIALIZE                                              0x30000002

#endif // _UVM_LINUX_IOCTL_H
