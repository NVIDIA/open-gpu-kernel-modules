/*
 * Copyright 2014 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef KFD_IOCTL_H_INCLUDED
#define KFD_IOCTL_H_INCLUDED

#include <drm/drm.h>
#include <linux/ioctl.h>

/*
 * - 1.1 - initial version
 * - 1.3 - Add SMI events support
 * - 1.4 - Indicate new SRAM EDC bit in device properties
 * - 1.5 - Add SVM API
 * - 1.6 - Query clear flags in SVM get_attr API
 * - 1.7 - Checkpoint Restore (CRIU) API
 * - 1.8 - CRIU - Support for SDMA transfers with GTT BOs
 */
#define KFD_IOCTL_MAJOR_VERSION 1
#define KFD_IOCTL_MINOR_VERSION 8

/*
 * Debug revision change log
 *
 * 0.1 - Initial revision
 * 0.2 - Fix to include querying pending event that is both trap and vmfault
 * 1.0 - Removed function to set debug data (renumbering functions broke ABI)
 * 1.1 - Allow attaching to processes that have not opened /dev/kfd yet
 * 1.2 - Allow flag option to clear queue status on queue suspend
 * 1.3 - Fix race condition between clear on suspend and trap event handling
 * 1.4 - Fix bad kfifo free
 * 1.5 - Fix ABA issue between queue snapshot and suspend
 * 2.0 - Return number of queues suspended/resumed and mask invalid/error
 *	 array slots
 * 2.1 - Add Set Address Watch, and Clear Address Watch support.
 * 3.0 - Overhaul set wave launch override API
 * 3.1 - Add support for GFX10
 * 3.2 - Add support for GFX10.3
 * 3.3 - Add precise memory operations enable
 * 4.0 - Remove gpu_id from api
 * 5.0 - Report exception codes to the debugger
 * 6.0 - Pass event file descriptor from userspace.
 * 6.1 - Add KFD_IOC_DBG_TRAP_QUERY_EXCEPTION_INFO.
 * 6.2 - Add KFD_IOC_DBG_TRAP_DEVICE_SNAPSHOT.
 * 7.0 - Redefine exception codes
 * 7.1 - Add KFD_IOC_DBG_TRAP_RUNTIME_ENABLE
 * 7.2 - Add KFD_IOC_DBG_TRAP_SEND_RUNTIME_EVENT
 * 8.0 - Expand runtime information given to the debugger
 * 8.1 - Allow the debugger to set the exception mask
 * 9.0 - Handle multiple exceptions from single trap signal
 * 10.0 - Query debug event returns both queue_id and gpu_id
 * 10.1 - Add additional debug capability information
 */
#define KFD_IOCTL_DBG_MAJOR_VERSION	10
#define KFD_IOCTL_DBG_MINOR_VERSION	1

struct kfd_ioctl_get_version_args {
	__u32 major_version;	/* from KFD */
	__u32 minor_version;	/* from KFD */
};

/* For kfd_ioctl_create_queue_args.queue_type. */
#define KFD_IOC_QUEUE_TYPE_COMPUTE		0x0
#define KFD_IOC_QUEUE_TYPE_SDMA			0x1
#define KFD_IOC_QUEUE_TYPE_COMPUTE_AQL		0x2
#define KFD_IOC_QUEUE_TYPE_SDMA_XGMI		0x3

#define KFD_MAX_QUEUE_PERCENTAGE	100
#define KFD_MAX_QUEUE_PRIORITY		15

struct kfd_ioctl_create_queue_args {
	__u64 ring_base_address;	/* to KFD */
	__u64 write_pointer_address;	/* from KFD */
	__u64 read_pointer_address;	/* from KFD */
	__u64 doorbell_offset;	/* from KFD */

	__u32 ring_size;		/* to KFD */
	__u32 gpu_id;		/* to KFD */
	__u32 queue_type;		/* to KFD */
	__u32 queue_percentage;	/* to KFD */
	__u32 queue_priority;	/* to KFD */
	__u32 queue_id;		/* from KFD */

	__u64 eop_buffer_address;	/* to KFD */
	__u64 eop_buffer_size;	/* to KFD */
	__u64 ctx_save_restore_address; /* to KFD */
	__u32 ctx_save_restore_size;	/* to KFD */
	__u32 ctl_stack_size;		/* to KFD */
};

struct kfd_ioctl_destroy_queue_args {
	__u32 queue_id;		/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_update_queue_args {
	__u64 ring_base_address;	/* to KFD */

	__u32 queue_id;		/* to KFD */
	__u32 ring_size;		/* to KFD */
	__u32 queue_percentage;	/* to KFD */
	__u32 queue_priority;	/* to KFD */
};

struct kfd_ioctl_set_cu_mask_args {
	__u32 queue_id;		/* to KFD */
	__u32 num_cu_mask;		/* to KFD */
	__u64 cu_mask_ptr;		/* to KFD */
};

struct kfd_ioctl_get_queue_wave_state_args {
	__u64 ctl_stack_address;	/* to KFD */
	__u32 ctl_stack_used_size;	/* from KFD */
	__u32 save_area_used_size;	/* from KFD */
	__u32 queue_id;			/* to KFD */
	__u32 pad;
};

struct kfd_queue_snapshot_entry {
	__u64 exception_status;
	__u64 ring_base_address;
	__u64 write_pointer_address;
	__u64 read_pointer_address;
	__u64 ctx_save_restore_address;
	__u32 queue_id;
	__u32 gpu_id;
	__u32 ring_size;
	__u32 queue_type;
	__u32 reserved[18];
};

struct kfd_dbg_device_info_entry {
	__u64 exception_status;
	__u64 lds_base;
	__u64 lds_limit;
	__u64 scratch_base;
	__u64 scratch_limit;
	__u64 gpuvm_base;
	__u64 gpuvm_limit;
	__u32 gpu_id;
	__u32 pad;
};

/* For kfd_ioctl_set_memory_policy_args.default_policy and alternate_policy */
#define KFD_IOC_CACHE_POLICY_COHERENT 0
#define KFD_IOC_CACHE_POLICY_NONCOHERENT 1

struct kfd_ioctl_set_memory_policy_args {
	__u64 alternate_aperture_base;	/* to KFD */
	__u64 alternate_aperture_size;	/* to KFD */

	__u32 gpu_id;			/* to KFD */
	__u32 default_policy;		/* to KFD */
	__u32 alternate_policy;		/* to KFD */
	__u32 pad;
};

/*
 * All counters are monotonic. They are used for profiling of compute jobs.
 * The profiling is done by userspace.
 *
 * In case of GPU reset, the counter should not be affected.
 */

struct kfd_ioctl_get_clock_counters_args {
	__u64 gpu_clock_counter;	/* from KFD */
	__u64 cpu_clock_counter;	/* from KFD */
	__u64 system_clock_counter;	/* from KFD */
	__u64 system_clock_freq;	/* from KFD */

	__u32 gpu_id;		/* to KFD */
	__u32 pad;
};

struct kfd_process_device_apertures {
	__u64 lds_base;		/* from KFD */
	__u64 lds_limit;		/* from KFD */
	__u64 scratch_base;		/* from KFD */
	__u64 scratch_limit;		/* from KFD */
	__u64 gpuvm_base;		/* from KFD */
	__u64 gpuvm_limit;		/* from KFD */
	__u32 gpu_id;		/* from KFD */
	__u32 pad;
};

/*
 * AMDKFD_IOC_GET_PROCESS_APERTURES is deprecated. Use
 * AMDKFD_IOC_GET_PROCESS_APERTURES_NEW instead, which supports an
 * unlimited number of GPUs.
 */
#define NUM_OF_SUPPORTED_GPUS 7
struct kfd_ioctl_get_process_apertures_args {
	struct kfd_process_device_apertures
			process_apertures[NUM_OF_SUPPORTED_GPUS];/* from KFD */

	/* from KFD, should be in the range [1 - NUM_OF_SUPPORTED_GPUS] */
	__u32 num_of_nodes;
	__u32 pad;
};

struct kfd_ioctl_get_process_apertures_new_args {
	/* User allocated. Pointer to struct kfd_process_device_apertures
	 * filled in by Kernel
	 */
	__u64 kfd_process_device_apertures_ptr;
	/* to KFD - indicates amount of memory present in
	 *  kfd_process_device_apertures_ptr
	 * from KFD - Number of entries filled by KFD.
	 */
	__u32 num_of_nodes;
	__u32 pad;
};

#define MAX_ALLOWED_NUM_POINTS    100
#define MAX_ALLOWED_AW_BUFF_SIZE 4096
#define MAX_ALLOWED_WAC_BUFF_SIZE  128

struct kfd_ioctl_dbg_register_args {
	__u32 gpu_id;		/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_dbg_unregister_args {
	__u32 gpu_id;		/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_dbg_address_watch_args {
	__u64 content_ptr;		/* a pointer to the actual content */
	__u32 gpu_id;		/* to KFD */
	__u32 buf_size_in_bytes;	/*including gpu_id and buf_size */
};

struct kfd_ioctl_dbg_wave_control_args {
	__u64 content_ptr;		/* a pointer to the actual content */
	__u32 gpu_id;		/* to KFD */
	__u32 buf_size_in_bytes;	/*including gpu_id and buf_size */
};
#define	KFD_DBG_EV_FLAG_CLEAR_STATUS	1

/* queue states for suspend/resume */
#define KFD_DBG_QUEUE_ERROR_BIT		30
#define KFD_DBG_QUEUE_INVALID_BIT	31
#define KFD_DBG_QUEUE_ERROR_MASK	(1 << KFD_DBG_QUEUE_ERROR_BIT)
#define KFD_DBG_QUEUE_INVALID_MASK	(1 << KFD_DBG_QUEUE_INVALID_BIT)

#define KFD_INVALID_GPUID	0xffffffff
#define KFD_INVALID_QUEUEID	0xffffffff
#define KFD_INVALID_FD		0xffffffff

enum kfd_dbg_trap_override_mode {
	KFD_DBG_TRAP_OVERRIDE_OR = 0,
	KFD_DBG_TRAP_OVERRIDE_REPLACE = 1
};
enum kfd_dbg_trap_mask {
	KFD_DBG_TRAP_MASK_FP_INVALID = 1,
	KFD_DBG_TRAP_MASK_FP_INPUT_DENORMAL = 2,
	KFD_DBG_TRAP_MASK_FP_DIVIDE_BY_ZERO = 4,
	KFD_DBG_TRAP_MASK_FP_OVERFLOW = 8,
	KFD_DBG_TRAP_MASK_FP_UNDERFLOW = 16,
	KFD_DBG_TRAP_MASK_FP_INEXACT = 32,
	KFD_DBG_TRAP_MASK_INT_DIVIDE_BY_ZERO = 64,
	KFD_DBG_TRAP_MASK_DBG_ADDRESS_WATCH = 128,
	KFD_DBG_TRAP_MASK_DBG_MEMORY_VIOLATION = 256
};

enum kfd_dbg_trap_exception_code {
	EC_NONE = 0,
	/* per queue */
	EC_QUEUE_WAVE_ABORT = 1,
	EC_QUEUE_WAVE_TRAP = 2,
	EC_QUEUE_WAVE_MATH_ERROR = 3,
	EC_QUEUE_WAVE_ILLEGAL_INSTRUCTION = 4,
	EC_QUEUE_WAVE_MEMORY_VIOLATION = 5,
	EC_QUEUE_WAVE_APERTURE_VIOLATION = 6,
	EC_QUEUE_PACKET_DISPATCH_DIM_INVALID = 16,
	EC_QUEUE_PACKET_DISPATCH_GROUP_SEGMENT_SIZE_INVALID = 17,
	EC_QUEUE_PACKET_DISPATCH_CODE_INVALID = 18,
	EC_QUEUE_PACKET_RESERVED = 19,
	EC_QUEUE_PACKET_UNSUPPORTED = 20,
	EC_QUEUE_PACKET_DISPATCH_WORK_GROUP_SIZE_INVALID = 21,
	EC_QUEUE_PACKET_DISPATCH_REGISTER_INVALID = 22,
	EC_QUEUE_PACKET_VENDOR_UNSUPPORTED = 23,
	EC_QUEUE_PREEMPTION_ERROR = 30,
	EC_QUEUE_NEW = 31,
	/* per device */
	EC_DEVICE_QUEUE_DELETE = 32,
	EC_DEVICE_MEMORY_VIOLATION = 33,
	EC_DEVICE_RAS_ERROR = 34,
	EC_DEVICE_FATAL_HALT = 35,
	EC_DEVICE_NEW = 36,
	/* per process */
	EC_PROCESS_RUNTIME = 48,
	EC_PROCESS_DEVICE_REMOVE = 49,
	EC_MAX
};

/* Mask generated by ecode defined in enum above. */
#define KFD_EC_MASK(ecode)	(1ULL << (ecode - 1))

/* Masks for exception code type checks below. */
#define KFD_EC_MASK_QUEUE	(KFD_EC_MASK(EC_QUEUE_WAVE_ABORT) |	\
				 KFD_EC_MASK(EC_QUEUE_WAVE_TRAP) |	\
				 KFD_EC_MASK(EC_QUEUE_WAVE_MATH_ERROR) |	\
				 KFD_EC_MASK(EC_QUEUE_WAVE_ILLEGAL_INSTRUCTION) |	\
				 KFD_EC_MASK(EC_QUEUE_WAVE_MEMORY_VIOLATION) |	\
				 KFD_EC_MASK(EC_QUEUE_WAVE_APERTURE_VIOLATION) |	\
				 KFD_EC_MASK(EC_QUEUE_PACKET_DISPATCH_DIM_INVALID) |	\
				 KFD_EC_MASK(EC_QUEUE_PACKET_DISPATCH_GROUP_SEGMENT_SIZE_INVALID) |	\
				 KFD_EC_MASK(EC_QUEUE_PACKET_DISPATCH_CODE_INVALID) |	\
				 KFD_EC_MASK(EC_QUEUE_PACKET_UNSUPPORTED) |	\
				 KFD_EC_MASK(EC_QUEUE_PACKET_DISPATCH_WORK_GROUP_SIZE_INVALID) |	\
				 KFD_EC_MASK(EC_QUEUE_PACKET_DISPATCH_REGISTER_INVALID) |	\
				 KFD_EC_MASK(EC_QUEUE_PACKET_VENDOR_UNSUPPORTED)	|	\
				 KFD_EC_MASK(EC_QUEUE_PREEMPTION_ERROR)	|	\
				 KFD_EC_MASK(EC_QUEUE_NEW))
#define KFD_EC_MASK_DEVICE	(KFD_EC_MASK(EC_DEVICE_QUEUE_DELETE) |		\
				 KFD_EC_MASK(EC_DEVICE_RAS_ERROR) |		\
				 KFD_EC_MASK(EC_DEVICE_FATAL_HALT) |		\
				 KFD_EC_MASK(EC_DEVICE_MEMORY_VIOLATION) |	\
				 KFD_EC_MASK(EC_DEVICE_NEW))
#define KFD_EC_MASK_PROCESS	(KFD_EC_MASK(EC_PROCESS_RUNTIME) |	\
				 KFD_EC_MASK(EC_PROCESS_DEVICE_REMOVE))

/* Checks for exception code types for KFD search. */
#define KFD_DBG_EC_TYPE_IS_QUEUE(ecode)					\
			(!!(KFD_EC_MASK(ecode) & KFD_EC_MASK_QUEUE))
#define KFD_DBG_EC_TYPE_IS_DEVICE(ecode)				\
			(!!(KFD_EC_MASK(ecode) & KFD_EC_MASK_DEVICE))
#define KFD_DBG_EC_TYPE_IS_PROCESS(ecode)				\
			(!!(KFD_EC_MASK(ecode) & KFD_EC_MASK_PROCESS))

enum kfd_dbg_runtime_state {
	DEBUG_RUNTIME_STATE_DISABLED = 0,
	DEBUG_RUNTIME_STATE_ENABLED = 1,
	DEBUG_RUNTIME_STATE_ENABLED_BUSY = 2,
	DEBUG_RUNTIME_STATE_ENABLED_ERROR = 3
};

struct kfd_runtime_info {
	__u64 r_debug;
	__u32 runtime_state;
	__u32 ttmp_setup;
};

/* KFD_IOC_DBG_TRAP_ENABLE:
 * exception_mask: exceptions to be reported to the debugger
 * ptr:   runtime info buffer to copy to (IN)
 * data1: 0=disable, 1=enable
 * data2: return value for fd
 * data3: runtime info size
 * data4: unused
 *
 */
#define KFD_IOC_DBG_TRAP_ENABLE 0

/* KFD_IOC_DBG_TRAP_SET_WAVE_LAUNCH_OVERRIDE:
 * exception_mask: unused
 * ptr:   unused
 * data1: override mode (see enum kfd_dbg_trap_override_mode)
 * data2: [in/out] trap mask (see enum kfd_dbg_trap_mask)
 * data3: [in] requested mask, [out] supported mask
 * data4: unused
 *
 * May fail with -EPERM if the requested mode is not supported.
 *
 * May fail with -EACCES if requested trap mask bits are not supported.
 * In that case the supported trap mask bits are returned in data3.
 *
 * If successful, output parameters return the previous trap mask
 * value and the hardware-dependent mask of supported trap mask bits.
 */
#define KFD_IOC_DBG_TRAP_SET_WAVE_LAUNCH_OVERRIDE 1

/* KFD_IOC_DBG_TRAP_SET_WAVE_LAUNCH_MODE:
 * exception_mask: unused
 * ptr:   unused
 * data1: 0=normal, 1=halt, 2=kill, 3=singlestep, 4=disable
 * data2: unused
 * data3: unused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_SET_WAVE_LAUNCH_MODE 2

/* KFD_IOC_DBG_TRAP_NODE_SUSPEND:
 * exception_mask: exceptions to clear on suspend
 * ptr:   pointer to an array of Queues IDs (IN/OUT)
 * data1: number of queues (IN)
 * data2: grace period (IN)
 * data3: unused
 * data4: unused
 *
 * Returns the number of queues suspended from array of Queue IDs (ptr).
 * Requested queues that fail to suspend are masked in the array:
 *
 * KFD_DBG_QUEUE_INVALID_MASK - requested queue does not exist or cannot be
 * suspended (new or being destroyed).
 *
 * KFD_DBG_QUEUE_ERROR_MASK - bad internal operation occurred on requested
 * queue.
 *
 * NOTE!  All queue destroy requests will be blocked on a suspended queue.
 * Queue resume will unblock.
 *
 * Grace period (data2) is time allowed for waves to complete before CWSR.
 * 0 can be entered for immediate preemption.
 */
#define KFD_IOC_DBG_TRAP_NODE_SUSPEND 3

/* KFD_IOC_DBG_TRAP_NODE_RESUME:
 * exception_mask: unused
 * ptr:   pointer to an array of Queues IDs (IN/OUT)
 * data1: number of queues (IN)
 * data2: unused
 * data3: unused
 * data4: unused
 *
 * Returns the number of queues resumed from array of Queue IDs (ptr).
 * Requested queues that fail to resume are masked in the array:
 *
 * KFD_DBG_QUEUE_INVALID_MASK - requested queue does not exist.
 *
 * KFD_DBG_QUEUE_ERROR_MASK - bad internal operation occurred on requested
 * queue.
 */
#define KFD_IOC_DBG_TRAP_NODE_RESUME 4

/* KFD_IOC_DBG_TRAP_QUERY_DEBUG_EVENT:
 * exception_mask: exception to clear (IN) on query and report (OUT)
 * ptr: unused
 * data1: queue id (OUT)
 * data2: gpu id (OUT)
 * data3: unused
 * data4: unused
 *
 * Source ID (data 1 - OUT) returns the queue_id for a queue event, the gpu_id
 * for a device event or 0 for a process event.
 *
 * Returns EAGAIN if no event is found and 0 otherwise.
 */
#define KFD_IOC_DBG_TRAP_QUERY_DEBUG_EVENT 5

/* KFD_IOC_DBG_TRAP_GET_QUEUE_SNAPSHOT:
 * exception_mask: exception to clear on snapshot
 * ptr: user buffer (IN)
 * data1: number of queue snapshots (IN/OUT) - 0 for IN ignores buffer writes
 * data2: unused
 * data3: unused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_GET_QUEUE_SNAPSHOT 6

/* KFD_IOC_DBG_TRAP_GET_VERSION:
 * exception_mask: unused
 * ptr: unsused
 * data1: major version (OUT)
 * data2: minor version (OUT)
 * data3: unused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_GET_VERSION	7

/* KFD_IOC_DBG_TRAP_CLEAR_ADDRESS_WATCH:
 * exception_mask: unused
 * ptr: unused
 * data1: watch ID
 * data2: unused
 * data3: unused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_CLEAR_ADDRESS_WATCH 8

/* KFD_IOC_DBG_TRAP_SET_ADDRESS_WATCH:
 * exception_mask: unused
 * ptr:   Watch address
 * data1: Watch ID (OUT)
 * data2: watch_mode: 0=read, 1=nonread, 2=atomic, 3=all
 * data3: watch address mask
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_SET_ADDRESS_WATCH 9

/* KFD_IOC_DBG_TRAP_SET_PRECISE_MEM_OPS
 * exception_mask: unused
 * ptr:   unused
 * data1: 0=disable, 1=enable (IN)
 * data2: unused
 * data3: unused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_SET_PRECISE_MEM_OPS 10

/* KFD_IOC_DBG_TRAP_QUERY_EXCEPTION_INFO
 * exception_mask: unused
 * ptr:  exception info pointer to copy to
 * data1: source_id
 * data2: exception_code
 * data3: clear_exception (1 == true, 0 == false)
 * data4: exception info data size
 *
 * NOTE: If data2 == EC_PROCESS_RUNTIME, the saved runtime info will be copied
 * to the exception info pointer.
 */
#define KFD_IOC_DBG_TRAP_QUERY_EXCEPTION_INFO 11

/* KFD_IOC_DBG_TRAP_DEVICE_SNAPSHOT
 * exception_mask: exception to clear on snapshot
 * ptr: user buffer for 'struct kfd_dbg_device_info_entry' entries (IN)
 * data1: number of devices in snapshot (IN/OUT)
 * data2: unused
 * data3: unused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_DEVICE_SNAPSHOT  12

/* KFD_IOC_DBG_TRAP_RUNTIME_ENABLE
 * exception_mask: unused
 * ptr:   r_debug info to save
 * data1: enable (0=disable, 1=enable)
 * data2: enable ttmp save (0=disable, 1=enable)
 * data3: unused
 * data4: unused
 *
 * FIXME: This option is temporary.  Future upstream will use a separate IOCTL.
 */
#define KFD_IOC_DBG_TRAP_RUNTIME_ENABLE 13

/* KFD_IOC_DBG_TRAP_SEND_RUNTIME_EVENT
 * exception_mask: exception to send
 * ptr:   unused
 * data1: destination device id
 * data2: destination queue id
 * data3: usused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_SEND_RUNTIME_EVENT 14

/* KFD_IOC_DBG_TRAP_SET_EXCEPTION_ENABLED
 * exception_mask: exception to set
 * ptr:   unused
 * data1: unused
 * data2: unused
 * data3: usused
 * data4: unused
 */
#define KFD_IOC_DBG_TRAP_SET_EXCEPTIONS_ENABLED 15

struct kfd_ioctl_dbg_trap_args {
	__u64 exception_mask; /* to KFD */
	__u64 ptr;     /* to KFD */
	__u32 pid;     /* to KFD */
	__u32 op;      /* to KFD */
	__u32 data1;   /* to KFD */
	__u32 data2;   /* to KFD */
	__u32 data3;   /* to KFD */
	__u32 data4;   /* to KFD */
};

/* Matching HSA_EVENTTYPE */
#define KFD_IOC_EVENT_SIGNAL			0
#define KFD_IOC_EVENT_NODECHANGE		1
#define KFD_IOC_EVENT_DEVICESTATECHANGE		2
#define KFD_IOC_EVENT_HW_EXCEPTION		3
#define KFD_IOC_EVENT_SYSTEM_EVENT		4
#define KFD_IOC_EVENT_DEBUG_EVENT		5
#define KFD_IOC_EVENT_PROFILE_EVENT		6
#define KFD_IOC_EVENT_QUEUE_EVENT		7
#define KFD_IOC_EVENT_MEMORY			8

#define KFD_IOC_WAIT_RESULT_COMPLETE		0
#define KFD_IOC_WAIT_RESULT_TIMEOUT		1
#define KFD_IOC_WAIT_RESULT_FAIL		2

#define KFD_SIGNAL_EVENT_LIMIT			4096

/* For kfd_event_data.hw_exception_data.reset_type. */
#define KFD_HW_EXCEPTION_WHOLE_GPU_RESET	0
#define KFD_HW_EXCEPTION_PER_ENGINE_RESET	1

/* For kfd_event_data.hw_exception_data.reset_cause. */
#define KFD_HW_EXCEPTION_GPU_HANG	0
#define KFD_HW_EXCEPTION_ECC		1

/* For kfd_hsa_memory_exception_data.ErrorType */
#define KFD_MEM_ERR_NO_RAS		0
#define KFD_MEM_ERR_SRAM_ECC		1
#define KFD_MEM_ERR_POISON_CONSUMED	2
#define KFD_MEM_ERR_GPU_HANG		3

struct kfd_ioctl_create_event_args {
	__u64 event_page_offset;	/* from KFD */
	__u32 event_trigger_data;	/* from KFD - signal events only */
	__u32 event_type;		/* to KFD */
	__u32 auto_reset;		/* to KFD */
	__u32 node_id;		/* to KFD - only valid for certain
							event types */
	__u32 event_id;		/* from KFD */
	__u32 event_slot_index;	/* from KFD */
};

struct kfd_ioctl_destroy_event_args {
	__u32 event_id;		/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_set_event_args {
	__u32 event_id;		/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_reset_event_args {
	__u32 event_id;		/* to KFD */
	__u32 pad;
};

struct kfd_memory_exception_failure {
	__u32 NotPresent;	/* Page not present or supervisor privilege */
	__u32 ReadOnly;	/* Write access to a read-only page */
	__u32 NoExecute;	/* Execute access to a page marked NX */
	__u32 imprecise;	/* Can't determine the	exact fault address */
};

/* memory exception data */
struct kfd_hsa_memory_exception_data {
	struct kfd_memory_exception_failure failure;
	__u64 va;
	__u32 gpu_id;
	__u32 ErrorType; /* 0 = no RAS error,
			  * 1 = ECC_SRAM,
			  * 2 = Link_SYNFLOOD (poison),
			  * 3 = GPU hang (not attributable to a specific cause),
			  * other values reserved
			  */
};

/* hw exception data */
struct kfd_hsa_hw_exception_data {
	__u32 reset_type;
	__u32 reset_cause;
	__u32 memory_lost;
	__u32 gpu_id;
};

/* Event data */
struct kfd_event_data {
	union {
		struct kfd_hsa_memory_exception_data memory_exception_data;
		struct kfd_hsa_hw_exception_data hw_exception_data;
	};				/* From KFD */
	__u64 kfd_event_data_ext;	/* pointer to an extension structure
					   for future exception types */
	__u32 event_id;		/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_wait_events_args {
	__u64 events_ptr;		/* pointed to struct
					   kfd_event_data array, to KFD */
	__u32 num_events;		/* to KFD */
	__u32 wait_for_all;		/* to KFD */
	__u32 timeout;		/* to KFD */
	__u32 wait_result;		/* from KFD */
};

struct kfd_ioctl_set_scratch_backing_va_args {
	__u64 va_addr;	/* to KFD */
	__u32 gpu_id;	/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_get_tile_config_args {
	/* to KFD: pointer to tile array */
	__u64 tile_config_ptr;
	/* to KFD: pointer to macro tile array */
	__u64 macro_tile_config_ptr;
	/* to KFD: array size allocated by user mode
	 * from KFD: array size filled by kernel
	 */
	__u32 num_tile_configs;
	/* to KFD: array size allocated by user mode
	 * from KFD: array size filled by kernel
	 */
	__u32 num_macro_tile_configs;

	__u32 gpu_id;		/* to KFD */
	__u32 gb_addr_config;	/* from KFD */
	__u32 num_banks;		/* from KFD */
	__u32 num_ranks;		/* from KFD */
	/* struct size can be extended later if needed
	 * without breaking ABI compatibility
	 */
};

struct kfd_ioctl_set_trap_handler_args {
	__u64 tba_addr;		/* to KFD */
	__u64 tma_addr;		/* to KFD */
	__u32 gpu_id;		/* to KFD */
	__u32 pad;
};

struct kfd_ioctl_acquire_vm_args {
	__u32 drm_fd;	/* to KFD */
	__u32 gpu_id;	/* to KFD */
};

/* Allocation flags: memory types */
#define KFD_IOC_ALLOC_MEM_FLAGS_VRAM		(1 << 0)
#define KFD_IOC_ALLOC_MEM_FLAGS_GTT		(1 << 1)
#define KFD_IOC_ALLOC_MEM_FLAGS_USERPTR		(1 << 2)
#define KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL	(1 << 3)
#define KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP	(1 << 4)
/* Allocation flags: attributes/access options */
#define KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE	(1 << 31)
#define KFD_IOC_ALLOC_MEM_FLAGS_EXECUTABLE	(1 << 30)
#define KFD_IOC_ALLOC_MEM_FLAGS_PUBLIC		(1 << 29)
#define KFD_IOC_ALLOC_MEM_FLAGS_NO_SUBSTITUTE	(1 << 28)
#define KFD_IOC_ALLOC_MEM_FLAGS_AQL_QUEUE_MEM	(1 << 27)
#define KFD_IOC_ALLOC_MEM_FLAGS_COHERENT	(1 << 26)
#define KFD_IOC_ALLOC_MEM_FLAGS_UNCACHED	(1 << 25)

/* Allocate memory for later SVM (shared virtual memory) mapping.
 *
 * @va_addr:     virtual address of the memory to be allocated
 *               all later mappings on all GPUs will use this address
 * @size:        size in bytes
 * @handle:      buffer handle returned to user mode, used to refer to
 *               this allocation for mapping, unmapping and freeing
 * @mmap_offset: for CPU-mapping the allocation by mmapping a render node
 *               for userptrs this is overloaded to specify the CPU address
 * @gpu_id:      device identifier
 * @flags:       memory type and attributes. See KFD_IOC_ALLOC_MEM_FLAGS above
 */
struct kfd_ioctl_alloc_memory_of_gpu_args {
	__u64 va_addr;		/* to KFD */
	__u64 size;		/* to KFD */
	__u64 handle;		/* from KFD */
	__u64 mmap_offset;	/* to KFD (userptr), from KFD (mmap offset) */
	__u32 gpu_id;		/* to KFD */
	__u32 flags;
};

/* Free memory allocated with kfd_ioctl_alloc_memory_of_gpu
 *
 * @handle: memory handle returned by alloc
 */
struct kfd_ioctl_free_memory_of_gpu_args {
	__u64 handle;		/* to KFD */
};

/* Map memory to one or more GPUs
 *
 * @handle:                memory handle returned by alloc
 * @device_ids_array_ptr:  array of gpu_ids (__u32 per device)
 * @n_devices:             number of devices in the array
 * @n_success:             number of devices mapped successfully
 *
 * @n_success returns information to the caller how many devices from
 * the start of the array have mapped the buffer successfully. It can
 * be passed into a subsequent retry call to skip those devices. For
 * the first call the caller should initialize it to 0.
 *
 * If the ioctl completes with return code 0 (success), n_success ==
 * n_devices.
 */
struct kfd_ioctl_map_memory_to_gpu_args {
	__u64 handle;			/* to KFD */
	__u64 device_ids_array_ptr;	/* to KFD */
	__u32 n_devices;		/* to KFD */
	__u32 n_success;		/* to/from KFD */
};

/* Unmap memory from one or more GPUs
 *
 * same arguments as for mapping
 */
struct kfd_ioctl_unmap_memory_from_gpu_args {
	__u64 handle;			/* to KFD */
	__u64 device_ids_array_ptr;	/* to KFD */
	__u32 n_devices;		/* to KFD */
	__u32 n_success;		/* to/from KFD */
};

/* Allocate GWS for specific queue
 *
 * @queue_id:    queue's id that GWS is allocated for
 * @num_gws:     how many GWS to allocate
 * @first_gws:   index of the first GWS allocated.
 *               only support contiguous GWS allocation
 */
struct kfd_ioctl_alloc_queue_gws_args {
	__u32 queue_id;		/* to KFD */
	__u32 num_gws;		/* to KFD */
	__u32 first_gws;	/* from KFD */
	__u32 pad;
};

struct kfd_ioctl_get_dmabuf_info_args {
	__u64 size;		/* from KFD */
	__u64 metadata_ptr;	/* to KFD */
	__u32 metadata_size;	/* to KFD (space allocated by user)
				 * from KFD (actual metadata size)
				 */
	__u32 gpu_id;	/* from KFD */
	__u32 flags;		/* from KFD (KFD_IOC_ALLOC_MEM_FLAGS) */
	__u32 dmabuf_fd;	/* to KFD */
};

struct kfd_ioctl_import_dmabuf_args {
	__u64 va_addr;	/* to KFD */
	__u64 handle;	/* from KFD */
	__u32 gpu_id;	/* to KFD */
	__u32 dmabuf_fd;	/* to KFD */
};

/*
 * KFD SMI(System Management Interface) events
 */
enum kfd_smi_event {
	KFD_SMI_EVENT_NONE = 0, /* not used */
	KFD_SMI_EVENT_VMFAULT = 1, /* event start counting at 1 */
	KFD_SMI_EVENT_THERMAL_THROTTLE = 2,
	KFD_SMI_EVENT_GPU_PRE_RESET = 3,
	KFD_SMI_EVENT_GPU_POST_RESET = 4,
};

#define KFD_SMI_EVENT_MASK_FROM_INDEX(i) (1ULL << ((i) - 1))

struct kfd_ioctl_smi_events_args {
	__u32 gpuid;	/* to KFD */
	__u32 anon_fd;	/* from KFD */
};

/**
 * kfd_ioctl_spm_op - SPM ioctl operations
 *
 * @KFD_IOCTL_SPM_OP_ACQUIRE: acquire exclusive access to SPM
 * @KFD_IOCTL_SPM_OP_RELEASE: release exclusive access to SPM
 * @KFD_IOCTL_SPM_OP_SET_DEST_BUF: set or unset destination buffer for SPM streaming
 */
enum kfd_ioctl_spm_op {
	KFD_IOCTL_SPM_OP_ACQUIRE,
	KFD_IOCTL_SPM_OP_RELEASE,
	KFD_IOCTL_SPM_OP_SET_DEST_BUF
};

/**
 * kfd_ioctl_spm_args - Arguments for SPM ioctl
 *
 * @op[in]:            specifies the operation to perform
 * @gpu_id[in]:        GPU ID of the GPU to profile
 * @dst_buf[in]:       used for the address of the destination buffer
 *                      in @KFD_IOCTL_SPM_SET_DEST_BUFFER
 * @buf_size[in]:      size of the destination buffer
 * @timeout[in/out]:   [in]: timeout in milliseconds, [out]: amount of time left
 *                      `in the timeout window
 * @bytes_copied[out]: amount of data that was copied to the previous dest_buf
 * @has_data_loss:     boolean indicating whether data was lost
 *                      (e.g. due to a ring-buffer overflow)
 *
 * This ioctl performs different functions depending on the @op parameter.
 *
 * KFD_IOCTL_SPM_OP_ACQUIRE
 * ------------------------
 *
 * Acquires exclusive access of SPM on the specified @gpu_id for the calling process.
 * This must be called before using KFD_IOCTL_SPM_OP_SET_DEST_BUF.
 *
 * KFD_IOCTL_SPM_OP_RELEASE
 * ------------------------
 *
 * Releases exclusive access of SPM on the specified @gpu_id for the calling process,
 * which allows another process to acquire it in the future.
 *
 * KFD_IOCTL_SPM_OP_SET_DEST_BUF
 * -----------------------------
 *
 * If @dst_buf is NULL, the destination buffer address is unset and copying of counters
 * is stopped.
 *
 * If @dst_buf is not NULL, it specifies the pointer to a new destination buffer.
 * @buf_size specifies the size of the buffer.
 *
 * If @timeout is non-0, the call will wait for up to @timeout ms for the previous
 * buffer to be filled. If previous buffer to be filled before timeout, the @timeout
 * will be updated value with the time remaining. If the timeout is exceeded, the function
 * copies any partial data available into the previous user buffer and returns success.
 * The amount of valid data in the previous user buffer is indicated by @bytes_copied.
 *
 * If @timeout is 0, the function immediately replaces the previous destination buffer
 * without waiting for the previous buffer to be filled. That means the previous buffer
 * may only be partially filled, and @bytes_copied will indicate how much data has been
 * copied to it.
 *
 * If data was lost, e.g. due to a ring buffer overflow, @has_data_loss will be non-0.
 *
 * Returns negative error code on failure, 0 on success.
 */
struct kfd_ioctl_spm_args {
	__u64 dest_buf;
	__u32 buf_size;
	__u32 op;
	__u32 timeout;
	__u32 gpu_id;
	__u32 bytes_copied;
	__u32 has_data_loss;
};

/**************************************************************************************************
 * CRIU IOCTLs (Checkpoint Restore In Userspace)
 *
 * When checkpointing a process, the userspace application will perform:
 * 1. PROCESS_INFO op to determine current process information. This pauses execution and evicts
 *    all the queues.
 * 2. CHECKPOINT op to checkpoint process contents (BOs, queues, events, svm-ranges)
 * 3. UNPAUSE op to un-evict all the queues
 *
 * When restoring a process, the CRIU userspace application will perform:
 *
 * 1. RESTORE op to restore process contents
 * 2. RESUME op to start the process
 *
 * Note: Queues are forced into an evicted state after a successful PROCESS_INFO. User
 * application needs to perform an UNPAUSE operation after calling PROCESS_INFO.
 */

enum kfd_criu_op {
	KFD_CRIU_OP_PROCESS_INFO,
	KFD_CRIU_OP_CHECKPOINT,
	KFD_CRIU_OP_UNPAUSE,
	KFD_CRIU_OP_RESTORE,
	KFD_CRIU_OP_RESUME,
};

/**
 * kfd_ioctl_criu_args - Arguments perform CRIU operation
 * @devices:		[in/out] User pointer to memory location for devices information.
 * 			This is an array of type kfd_criu_device_bucket.
 * @bos:		[in/out] User pointer to memory location for BOs information
 * 			This is an array of type kfd_criu_bo_bucket.
 * @priv_data:		[in/out] User pointer to memory location for private data
 * @priv_data_size:	[in/out] Size of priv_data in bytes
 * @num_devices:	[in/out] Number of GPUs used by process. Size of @devices array.
 * @num_bos		[in/out] Number of BOs used by process. Size of @bos array.
 * @num_objects:	[in/out] Number of objects used by process. Objects are opaque to
 *				 user application.
 * @pid:		[in/out] PID of the process being checkpointed
 * @op			[in] Type of operation (kfd_criu_op)
 *
 * Return: 0 on success, -errno on failure
 */
struct kfd_ioctl_criu_args {
	__u64 devices;		/* Used during ops: CHECKPOINT, RESTORE */
	__u64 bos;		/* Used during ops: CHECKPOINT, RESTORE */
	__u64 priv_data;	/* Used during ops: CHECKPOINT, RESTORE */
	__u64 priv_data_size;	/* Used during ops: PROCESS_INFO, RESTORE */
	__u32 num_devices;	/* Used during ops: PROCESS_INFO, RESTORE */
	__u32 num_bos;		/* Used during ops: PROCESS_INFO, RESTORE */
	__u32 num_objects;	/* Used during ops: PROCESS_INFO, RESTORE */
	__u32 pid;		/* Used during ops: PROCESS_INFO, RESUME */
	__u32 op;
};

struct kfd_criu_device_bucket {
	__u32 user_gpu_id;
	__u32 actual_gpu_id;
	__u32 drm_fd;
	__u32 pad;
};

struct kfd_criu_bo_bucket {
	__u64 addr;
	__u64 size;
	__u64 offset;
	__u64 restored_offset;    /* During restore, updated offset for BO */
	__u32 gpu_id;             /* This is the user_gpu_id */
	__u32 alloc_flags;
	__u32 dmabuf_fd;
	__u32 pad;
};

/* CRIU IOCTLs - END */
/**************************************************************************************************/

/* Register offset inside the remapped mmio page
 */
enum kfd_mmio_remap {
	KFD_MMIO_REMAP_HDP_MEM_FLUSH_CNTL = 0,
	KFD_MMIO_REMAP_HDP_REG_FLUSH_CNTL = 4,
};

struct kfd_ioctl_ipc_export_handle_args {
	__u64 handle;		/* to KFD */
	__u32 share_handle[4];	/* from KFD */
	__u32 gpu_id;		/* to KFD */
	__u32 flags;		/* to KFD */
};

struct kfd_ioctl_ipc_import_handle_args {
	__u64 handle;		/* from KFD */
	__u64 va_addr;		/* to KFD */
	__u64 mmap_offset;	/* from KFD */
	__u32 share_handle[4];	/* to KFD */
	__u32 gpu_id;		/* to KFD */
	__u32 flags;		/* from KFD */
};

struct kfd_memory_range {
	__u64 va_addr;
	__u64 size;
};

/* flags definitions
 * BIT0: 0: read operation, 1: write operation.
 * This also identifies if the src or dst array belongs to remote process
 */
#define KFD_CROSS_MEMORY_RW_BIT (1 << 0)
#define KFD_SET_CROSS_MEMORY_READ(flags) (flags &= ~KFD_CROSS_MEMORY_RW_BIT)
#define KFD_SET_CROSS_MEMORY_WRITE(flags) (flags |= KFD_CROSS_MEMORY_RW_BIT)
#define KFD_IS_CROSS_MEMORY_WRITE(flags) (flags & KFD_CROSS_MEMORY_RW_BIT)

struct kfd_ioctl_cross_memory_copy_args {
	/* to KFD: Process ID of the remote process */
	__u32 pid;
	/* to KFD: See above definition */
	__u32 flags;
	/* to KFD: Source GPU VM range */
	__u64 src_mem_range_array;
	/* to KFD: Size of above array */
	__u64 src_mem_array_size;
	/* to KFD: Destination GPU VM range */
	__u64 dst_mem_range_array;
	/* to KFD: Size of above array */
	__u64 dst_mem_array_size;
	/* from KFD: Total amount of bytes copied */
	__u64 bytes_copied;
};

/* Guarantee host access to memory */
#define KFD_IOCTL_SVM_FLAG_HOST_ACCESS 0x00000001
/* Fine grained coherency between all devices with access */
#define KFD_IOCTL_SVM_FLAG_COHERENT    0x00000002
/* Use any GPU in same hive as preferred device */
#define KFD_IOCTL_SVM_FLAG_HIVE_LOCAL  0x00000004
/* GPUs only read, allows replication */
#define KFD_IOCTL_SVM_FLAG_GPU_RO      0x00000008
/* Allow execution on GPU */
#define KFD_IOCTL_SVM_FLAG_GPU_EXEC    0x00000010
/* GPUs mostly read, may allow similar optimizations as RO, but writes fault */
#define KFD_IOCTL_SVM_FLAG_GPU_READ_MOSTLY     0x00000020

/**
 * kfd_ioctl_svm_op - SVM ioctl operations
 *
 * @KFD_IOCTL_SVM_OP_SET_ATTR: Modify one or more attributes
 * @KFD_IOCTL_SVM_OP_GET_ATTR: Query one or more attributes
 */
enum kfd_ioctl_svm_op {
	KFD_IOCTL_SVM_OP_SET_ATTR,
	KFD_IOCTL_SVM_OP_GET_ATTR
};

/** kfd_ioctl_svm_location - Enum for preferred and prefetch locations
 *
 * GPU IDs are used to specify GPUs as preferred and prefetch locations.
 * Below definitions are used for system memory or for leaving the preferred
 * location unspecified.
 */
enum kfd_ioctl_svm_location {
	KFD_IOCTL_SVM_LOCATION_SYSMEM = 0,
	KFD_IOCTL_SVM_LOCATION_UNDEFINED = 0xffffffff
};

/**
 * kfd_ioctl_svm_attr_type - SVM attribute types
 *
 * @KFD_IOCTL_SVM_ATTR_PREFERRED_LOC: gpuid of the preferred location, 0 for
 *                                    system memory
 * @KFD_IOCTL_SVM_ATTR_PREFETCH_LOC: gpuid of the prefetch location, 0 for
 *                                   system memory. Setting this triggers an
 *                                   immediate prefetch (migration).
 * @KFD_IOCTL_SVM_ATTR_ACCESS:
 * @KFD_IOCTL_SVM_ATTR_ACCESS_IN_PLACE:
 * @KFD_IOCTL_SVM_ATTR_NO_ACCESS: specify memory access for the gpuid given
 *                                by the attribute value
 * @KFD_IOCTL_SVM_ATTR_SET_FLAGS: bitmask of flags to set (see
 *                                KFD_IOCTL_SVM_FLAG_...)
 * @KFD_IOCTL_SVM_ATTR_CLR_FLAGS: bitmask of flags to clear
 * @KFD_IOCTL_SVM_ATTR_GRANULARITY: migration granularity
 *                                  (log2 num pages)
 */
enum kfd_ioctl_svm_attr_type {
	KFD_IOCTL_SVM_ATTR_PREFERRED_LOC,
	KFD_IOCTL_SVM_ATTR_PREFETCH_LOC,
	KFD_IOCTL_SVM_ATTR_ACCESS,
	KFD_IOCTL_SVM_ATTR_ACCESS_IN_PLACE,
	KFD_IOCTL_SVM_ATTR_NO_ACCESS,
	KFD_IOCTL_SVM_ATTR_SET_FLAGS,
	KFD_IOCTL_SVM_ATTR_CLR_FLAGS,
	KFD_IOCTL_SVM_ATTR_GRANULARITY
};

/**
 * kfd_ioctl_svm_attribute - Attributes as pairs of type and value
 *
 * The meaning of the @value depends on the attribute type.
 *
 * @type: attribute type (see enum @kfd_ioctl_svm_attr_type)
 * @value: attribute value
 */
struct kfd_ioctl_svm_attribute {
	__u32 type;
	__u32 value;
};

/**
 * kfd_ioctl_svm_args - Arguments for SVM ioctl
 *
 * @op specifies the operation to perform (see enum
 * @kfd_ioctl_svm_op).  @start_addr and @size are common for all
 * operations.
 *
 * A variable number of attributes can be given in @attrs.
 * @nattr specifies the number of attributes. New attributes can be
 * added in the future without breaking the ABI. If unknown attributes
 * are given, the function returns -EINVAL.
 *
 * @KFD_IOCTL_SVM_OP_SET_ATTR sets attributes for a virtual address
 * range. It may overlap existing virtual address ranges. If it does,
 * the existing ranges will be split such that the attribute changes
 * only apply to the specified address range.
 *
 * @KFD_IOCTL_SVM_OP_GET_ATTR returns the intersection of attributes
 * over all memory in the given range and returns the result as the
 * attribute value. If different pages have different preferred or
 * prefetch locations, 0xffffffff will be returned for
 * @KFD_IOCTL_SVM_ATTR_PREFERRED_LOC or
 * @KFD_IOCTL_SVM_ATTR_PREFETCH_LOC resepctively. For
 * @KFD_IOCTL_SVM_ATTR_SET_FLAGS, flags of all pages will be
 * aggregated by bitwise AND. That means, a flag will be set in the
 * output, if that flag is set for all pages in the range. For
 * @KFD_IOCTL_SVM_ATTR_CLR_FLAGS, flags of all pages will be
 * aggregated by bitwise NOR. That means, a flag will be set in the
 * output, if that flag is clear for all pages in the range.
 * The minimum migration granularity throughout the range will be
 * returned for @KFD_IOCTL_SVM_ATTR_GRANULARITY.
 *
 * Querying of accessibility attributes works by initializing the
 * attribute type to @KFD_IOCTL_SVM_ATTR_ACCESS and the value to the
 * GPUID being queried. Multiple attributes can be given to allow
 * querying multiple GPUIDs. The ioctl function overwrites the
 * attribute type to indicate the access for the specified GPU.
 */
struct kfd_ioctl_svm_args {
	__u64 start_addr;
	__u64 size;
	__u32 op;
	__u32 nattr;
	/* Variable length array of attributes */
	struct kfd_ioctl_svm_attribute attrs[];
};

/**
 * kfd_ioctl_set_xnack_mode_args - Arguments for set_xnack_mode
 *
 * @xnack_enabled:       [in/out] Whether to enable XNACK mode for this process
 *
 * @xnack_enabled indicates whether recoverable page faults should be
 * enabled for the current process. 0 means disabled, positive means
 * enabled, negative means leave unchanged. If enabled, virtual address
 * translations on GFXv9 and later AMD GPUs can return XNACK and retry
 * the access until a valid PTE is available. This is used to implement
 * device page faults.
 *
 * On output, @xnack_enabled returns the (new) current mode (0 or
 * positive). Therefore, a negative input value can be used to query
 * the current mode without changing it.
 *
 * The XNACK mode fundamentally changes the way SVM managed memory works
 * in the driver, with subtle effects on application performance and
 * functionality.
 *
 * Enabling XNACK mode requires shader programs to be compiled
 * differently. Furthermore, not all GPUs support changing the mode
 * per-process. Therefore changing the mode is only allowed while no
 * user mode queues exist in the process. This ensure that no shader
 * code is running that may be compiled for the wrong mode. And GPUs
 * that cannot change to the requested mode will prevent the XNACK
 * mode from occurring. All GPUs used by the process must be in the
 * same XNACK mode.
 *
 * GFXv8 or older GPUs do not support 48 bit virtual addresses or SVM.
 * Therefore those GPUs are not considered for the XNACK mode switch.
 *
 * Return: 0 on success, -errno on failure
 */
struct kfd_ioctl_set_xnack_mode_args {
	__s32 xnack_enabled;
};

#define AMDKFD_IOCTL_BASE 'K'
#define AMDKFD_IO(nr)			_IO(AMDKFD_IOCTL_BASE, nr)
#define AMDKFD_IOR(nr, type)		_IOR(AMDKFD_IOCTL_BASE, nr, type)
#define AMDKFD_IOW(nr, type)		_IOW(AMDKFD_IOCTL_BASE, nr, type)
#define AMDKFD_IOWR(nr, type)		_IOWR(AMDKFD_IOCTL_BASE, nr, type)

#define AMDKFD_IOC_GET_VERSION			\
		AMDKFD_IOR(0x01, struct kfd_ioctl_get_version_args)

#define AMDKFD_IOC_CREATE_QUEUE			\
		AMDKFD_IOWR(0x02, struct kfd_ioctl_create_queue_args)

#define AMDKFD_IOC_DESTROY_QUEUE		\
		AMDKFD_IOWR(0x03, struct kfd_ioctl_destroy_queue_args)

#define AMDKFD_IOC_SET_MEMORY_POLICY		\
		AMDKFD_IOW(0x04, struct kfd_ioctl_set_memory_policy_args)

#define AMDKFD_IOC_GET_CLOCK_COUNTERS		\
		AMDKFD_IOWR(0x05, struct kfd_ioctl_get_clock_counters_args)

#define AMDKFD_IOC_GET_PROCESS_APERTURES	\
		AMDKFD_IOR(0x06, struct kfd_ioctl_get_process_apertures_args)

#define AMDKFD_IOC_UPDATE_QUEUE			\
		AMDKFD_IOW(0x07, struct kfd_ioctl_update_queue_args)

#define AMDKFD_IOC_CREATE_EVENT			\
		AMDKFD_IOWR(0x08, struct kfd_ioctl_create_event_args)

#define AMDKFD_IOC_DESTROY_EVENT		\
		AMDKFD_IOW(0x09, struct kfd_ioctl_destroy_event_args)

#define AMDKFD_IOC_SET_EVENT			\
		AMDKFD_IOW(0x0A, struct kfd_ioctl_set_event_args)

#define AMDKFD_IOC_RESET_EVENT			\
		AMDKFD_IOW(0x0B, struct kfd_ioctl_reset_event_args)

#define AMDKFD_IOC_WAIT_EVENTS			\
		AMDKFD_IOWR(0x0C, struct kfd_ioctl_wait_events_args)

#define AMDKFD_IOC_DBG_REGISTER_DEPRECATED	\
		AMDKFD_IOW(0x0D, struct kfd_ioctl_dbg_register_args)

#define AMDKFD_IOC_DBG_UNREGISTER_DEPRECATED	\
		AMDKFD_IOW(0x0E, struct kfd_ioctl_dbg_unregister_args)

#define AMDKFD_IOC_DBG_ADDRESS_WATCH_DEPRECATED	\
		AMDKFD_IOW(0x0F, struct kfd_ioctl_dbg_address_watch_args)

#define AMDKFD_IOC_DBG_WAVE_CONTROL_DEPRECATED	\
		AMDKFD_IOW(0x10, struct kfd_ioctl_dbg_wave_control_args)

#define AMDKFD_IOC_SET_SCRATCH_BACKING_VA	\
		AMDKFD_IOWR(0x11, struct kfd_ioctl_set_scratch_backing_va_args)

#define AMDKFD_IOC_GET_TILE_CONFIG                                      \
		AMDKFD_IOWR(0x12, struct kfd_ioctl_get_tile_config_args)

#define AMDKFD_IOC_SET_TRAP_HANDLER		\
		AMDKFD_IOW(0x13, struct kfd_ioctl_set_trap_handler_args)

#define AMDKFD_IOC_GET_PROCESS_APERTURES_NEW	\
		AMDKFD_IOWR(0x14,		\
			struct kfd_ioctl_get_process_apertures_new_args)

#define AMDKFD_IOC_ACQUIRE_VM			\
		AMDKFD_IOW(0x15, struct kfd_ioctl_acquire_vm_args)

#define AMDKFD_IOC_ALLOC_MEMORY_OF_GPU		\
		AMDKFD_IOWR(0x16, struct kfd_ioctl_alloc_memory_of_gpu_args)

#define AMDKFD_IOC_FREE_MEMORY_OF_GPU		\
		AMDKFD_IOW(0x17, struct kfd_ioctl_free_memory_of_gpu_args)

#define AMDKFD_IOC_MAP_MEMORY_TO_GPU		\
		AMDKFD_IOWR(0x18, struct kfd_ioctl_map_memory_to_gpu_args)

#define AMDKFD_IOC_UNMAP_MEMORY_FROM_GPU	\
		AMDKFD_IOWR(0x19, struct kfd_ioctl_unmap_memory_from_gpu_args)

#define AMDKFD_IOC_SET_CU_MASK		\
		AMDKFD_IOW(0x1A, struct kfd_ioctl_set_cu_mask_args)

#define AMDKFD_IOC_GET_QUEUE_WAVE_STATE		\
		AMDKFD_IOWR(0x1B, struct kfd_ioctl_get_queue_wave_state_args)

#define AMDKFD_IOC_GET_DMABUF_INFO		\
		AMDKFD_IOWR(0x1C, struct kfd_ioctl_get_dmabuf_info_args)

#define AMDKFD_IOC_IMPORT_DMABUF		\
		AMDKFD_IOWR(0x1D, struct kfd_ioctl_import_dmabuf_args)

#define AMDKFD_IOC_ALLOC_QUEUE_GWS		\
		AMDKFD_IOWR(0x1E, struct kfd_ioctl_alloc_queue_gws_args)

#define AMDKFD_IOC_SMI_EVENTS			\
		AMDKFD_IOWR(0x1F, struct kfd_ioctl_smi_events_args)

#define AMDKFD_IOC_SVM	AMDKFD_IOWR(0x20, struct kfd_ioctl_svm_args)

#define AMDKFD_IOC_SET_XNACK_MODE		\
		AMDKFD_IOWR(0x21, struct kfd_ioctl_set_xnack_mode_args)

#define AMDKFD_IOC_CRIU_OP			\
		AMDKFD_IOWR(0x22, struct kfd_ioctl_criu_args)

#define AMDKFD_COMMAND_START		0x01
#define AMDKFD_COMMAND_END		0x23

/* non-upstream ioctls */
#define AMDKFD_IOC_IPC_IMPORT_HANDLE                                    \
		AMDKFD_IOWR(0x80, struct kfd_ioctl_ipc_import_handle_args)

#define AMDKFD_IOC_IPC_EXPORT_HANDLE		\
		AMDKFD_IOWR(0x81, struct kfd_ioctl_ipc_export_handle_args)

#define AMDKFD_IOC_DBG_TRAP			\
		AMDKFD_IOWR(0x82, struct kfd_ioctl_dbg_trap_args)

#define AMDKFD_IOC_CROSS_MEMORY_COPY		\
		AMDKFD_IOWR(0x83, struct kfd_ioctl_cross_memory_copy_args)

#define AMDKFD_IOC_RLC_SPM		\
		AMDKFD_IOWR(0x84, struct kfd_ioctl_spm_args)


#define AMDKFD_COMMAND_START_2		0x80
#define AMDKFD_COMMAND_END_2		0x85

#endif
