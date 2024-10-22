/*******************************************************************************
    Copyright (c) 2013-2023 NVIDIA Corporation

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*******************************************************************************/

#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_forward_decl.h"

// TODO: Bug 1710855: Tweak this number through benchmarks
#define UVM_SPIN_LOOP_SCHEDULE_TIMEOUT_NS   (10*1000ULL)
#define UVM_SPIN_LOOP_PRINT_TIMEOUT_SEC     30ULL

// Default to debug prints being enabled for debug and develop builds and
// disabled for release builds.
static int uvm_debug_prints = UVM_IS_DEBUG() || UVM_IS_DEVELOP();

// Make the module param writable so that prints can be enabled or disabled at
// any time by modifying the module parameter.
module_param(uvm_debug_prints, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(uvm_debug_prints, "Enable uvm debug prints.");

bool uvm_debug_prints_enabled(void)
{
    return uvm_debug_prints != 0;
}

// This parameter allows a program in user mode to call the kernel tests
// defined in this module. This parameter should only be used for testing and
// must not be set to true otherwise since it breaks security when it is
// enabled. By default and for safety reasons this parameter is set to false.
int uvm_enable_builtin_tests __read_mostly = 0;
module_param(uvm_enable_builtin_tests, int, S_IRUGO);
MODULE_PARM_DESC(uvm_enable_builtin_tests,
                 "Enable the UVM built-in tests. (This is a security risk)");

// Default to release asserts being enabled.
int uvm_release_asserts __read_mostly = 1;

// Make the module param writable so that release asserts can be enabled or
// disabled at any time by modifying the module parameter.
module_param(uvm_release_asserts, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(uvm_release_asserts, "Enable uvm asserts included in release builds.");

// Default to failed release asserts not dumping stack.
int uvm_release_asserts_dump_stack __read_mostly = 0;

// Make the module param writable so that dumping the stack can be enabled and
// disabled at any time by modifying the module parameter.
module_param(uvm_release_asserts_dump_stack, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(uvm_release_asserts_dump_stack, "dump_stack() on failed UVM release asserts.");

// Default to failed release asserts not setting the global UVM error.
int uvm_release_asserts_set_global_error __read_mostly = 0;

// Make the module param writable so that setting the global fatal error can be
// enabled and disabled at any time by modifying the module parameter.
module_param(uvm_release_asserts_set_global_error, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(uvm_release_asserts_set_global_error, "Set UVM global fatal error on failed release asserts.");

// A separate flag to enable setting global error, to be used by tests only.
bool uvm_release_asserts_set_global_error_for_tests __read_mostly = false;

//
// Convert kernel errno codes to corresponding NV_STATUS
//
NV_STATUS errno_to_nv_status(int errnoCode)
{
    if (errnoCode < 0)
        errnoCode = -errnoCode;

    switch (errnoCode)
    {
        case 0:
            return NV_OK;

        case E2BIG:
        case EINVAL:
            return NV_ERR_INVALID_ARGUMENT;

        case EACCES:
            return NV_ERR_INVALID_ACCESS_TYPE;

        case EADDRINUSE:
        case EADDRNOTAVAIL:
            return NV_ERR_UVM_ADDRESS_IN_USE;

        case EFAULT:
            return NV_ERR_INVALID_ADDRESS;

        case EOVERFLOW:
            return NV_ERR_OUT_OF_RANGE;

        case EINTR:
        case EBUSY:
        case EAGAIN:
            return NV_ERR_BUSY_RETRY;

        case ENXIO:
        case ENODEV:
            return NV_ERR_MODULE_LOAD_FAILED;

        case ENOMEM:
            return NV_ERR_NO_MEMORY;

        case EPERM:
            return NV_ERR_INSUFFICIENT_PERMISSIONS;

        case ESRCH:
            return NV_ERR_PID_NOT_FOUND;

        case ETIMEDOUT:
            return NV_ERR_TIMEOUT;

        case EEXIST:
            return NV_ERR_IN_USE;

        case ENOSYS:
        case EOPNOTSUPP:
            return NV_ERR_NOT_SUPPORTED;

        case ENOENT:
            return NV_ERR_NO_VALID_PATH;

        case EIO:
            return NV_ERR_RC_ERROR;

        case ENODATA:
            return NV_ERR_OBJECT_NOT_FOUND;

        default:
            return NV_ERR_GENERIC;
    };
}

// Returns POSITIVE errno
int nv_status_to_errno(NV_STATUS status)
{
    switch (status) {
        case NV_OK:
            return 0;

        case NV_ERR_BUSY_RETRY:
            return EAGAIN;

        case NV_ERR_INSUFFICIENT_PERMISSIONS:
            return EPERM;

        case NV_ERR_GPU_UUID_NOT_FOUND:
            return ENODEV;

        case NV_ERR_INSUFFICIENT_RESOURCES:
        case NV_ERR_NO_MEMORY:
            return ENOMEM;

        case NV_ERR_INVALID_ACCESS_TYPE:
            return EACCES;

        case NV_ERR_INVALID_ADDRESS:
            return EFAULT;

        case NV_ERR_INVALID_ARGUMENT:
        case NV_ERR_INVALID_DEVICE:
        case NV_ERR_INVALID_PARAMETER:
        case NV_ERR_INVALID_REQUEST:
        case NV_ERR_INVALID_STATE:
            return EINVAL;

        case NV_ERR_NOT_SUPPORTED:
            return ENOSYS;

        case NV_ERR_OBJECT_NOT_FOUND:
            return ENODATA;

        case NV_ERR_MODULE_LOAD_FAILED:
            return ENXIO;

        case NV_ERR_OVERLAPPING_UVM_COMMIT:
        case NV_ERR_UVM_ADDRESS_IN_USE:
            return EADDRINUSE;

        case NV_ERR_PID_NOT_FOUND:
            return ESRCH;

        case NV_ERR_TIMEOUT:
        case NV_ERR_TIMEOUT_RETRY:
            return ETIMEDOUT;

        case NV_ERR_IN_USE:
            return EEXIST;

        case NV_ERR_NO_VALID_PATH:
            return ENOENT;

        case NV_ERR_RC_ERROR:
        case NV_ERR_ECC_ERROR:
            return EIO;

        case NV_ERR_OUT_OF_RANGE:
            return EOVERFLOW;

        default:
            UVM_ASSERT_MSG(0, "No errno conversion set up for NV_STATUS %s\n", nvstatusToString(status));
            return EINVAL;
    }
}

//
// This routine retrieves the process ID of current, but makes no attempt to
// refcount or lock the pid in place.
//
unsigned uvm_get_stale_process_id(void)
{
    return (unsigned)task_tgid_vnr(current);
}

unsigned uvm_get_stale_thread_id(void)
{
    return (unsigned)task_pid_vnr(current);
}

void on_uvm_test_fail(void)
{
    (void)NULL;
}

void on_uvm_assert(void)
{
    (void)NULL;
#ifdef __COVERITY__
    __coverity_panic__()
#endif
}

NV_STATUS uvm_spin_loop(uvm_spin_loop_t *spin)
{
    NvU64 curr = NV_GETTIME();

    // This schedule() is required for functionality, not just system
    // performance. It allows RM to run and unblock the UVM driver:
    //
    // - UVM must service faults in order for RM to idle/preempt a context
    // - RM must service interrupts which stall UVM (SW methods, stalling CE
    //   interrupts, etc) in order for UVM to service faults
    //
    // Even though UVM's bottom half is preemptable, we have encountered cases
    // in which a user thread running in RM won't preempt the UVM driver's
    // thread unless the UVM driver thread gives up its timeslice. This is also
    // theoretically possible if the RM thread has a low nice priority.
    //
    // TODO: Bug 1710855: Look into proper prioritization of these threads as a longer-term
    //       solution.
    if (curr - spin->start_time_ns >= UVM_SPIN_LOOP_SCHEDULE_TIMEOUT_NS && NV_MAY_SLEEP()) {
        schedule();
        curr = NV_GETTIME();
    }

    cpu_relax();

    // TODO: Bug 1710855: Also check fatal_signal_pending() here if the caller can handle it.

    if (curr - spin->print_time_ns >= 1000*1000*1000*UVM_SPIN_LOOP_PRINT_TIMEOUT_SEC) {
        spin->print_time_ns = curr;
        return NV_ERR_TIMEOUT_RETRY;
    }

    return NV_OK;
}

static char uvm_digit_to_hex(unsigned value)
{
    if (value >= 10)
        return value - 10 + 'a';
    else
        return value + '0';
}

void uvm_uuid_string(char *buffer, const NvProcessorUuid *pUuidStruct)
{
    char *str = buffer;
    unsigned i;
    unsigned dashMask = 1 << 4 | 1 << 6 | 1 << 8 | 1 << 10;

    for (i = 0; i < 16; i++) {
        *str++ = uvm_digit_to_hex(pUuidStruct->uuid[i] >> 4);
        *str++ = uvm_digit_to_hex(pUuidStruct->uuid[i] & 0xF);

        if (dashMask & (1 << (i + 1)))
            *str++ = '-';
    }

    *str = 0;
}
