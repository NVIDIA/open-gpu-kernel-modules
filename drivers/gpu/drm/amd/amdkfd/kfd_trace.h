/*
 * Copyright 2018 Advanced Micro Devices, Inc.
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
 *
 */

#if !defined(_AMDKFD_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _KFD_TRACE_H_


#include <linux/stringify.h>
#include <linux/types.h>
#include <linux/tracepoint.h>

#include "kfd_priv.h"
#include <linux/kfd_ioctl.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM amdkfd
#define TRACE_INCLUDE_FILE kfd_trace


TRACE_EVENT(kfd_map_memory_to_gpu_start,
	    TP_PROTO(struct kfd_process *p),
	    TP_ARGS(p),
	    TP_STRUCT__entry(
			    __field(unsigned int, pasid)
			    ),
	    TP_fast_assign(
			   __entry->pasid = p->pasid;
			   ),
	    TP_printk("pasid =%u", __entry->pasid)
);


TRACE_EVENT(kfd_map_memory_to_gpu_end,
	    TP_PROTO(struct kfd_process *p, u32 array_size, char *pStatusMsg),
	    TP_ARGS(p, array_size, pStatusMsg),
	    TP_STRUCT__entry(
				__field(unsigned int, pasid)
				__field(unsigned int, array_size)
				__string(pStatusMsg, pStatusMsg)
			    ),
	    TP_fast_assign(
			   __entry->pasid = p->pasid;
				__entry->array_size = array_size;
				__assign_str(pStatusMsg, pStatusMsg);
			   ),
	    TP_printk("pasid = %u, array_size =	%u, StatusMsg=%s",
				__entry->pasid,
				 __entry->array_size,
				 __get_str(pStatusMsg))
);


TRACE_EVENT(kfd_kgd2kfd_schedule_evict_and_restore_process,
	    TP_PROTO(struct kfd_process *p, u32 delay_jiffies),
	    TP_ARGS(p, delay_jiffies),
	    TP_STRUCT__entry(
				__field(unsigned int, pasid)
				__field(unsigned int, delay_jiffies)
			    ),
	    TP_fast_assign(
			   __entry->pasid = p->pasid;
			   __entry->delay_jiffies = delay_jiffies;
			),
	    TP_printk("pasid = %u, delay_jiffies = %u",
		      __entry->pasid,
		      __entry->delay_jiffies)
);


TRACE_EVENT(kfd_evict_process_worker_start,
	    TP_PROTO(struct kfd_process *p),
	    TP_ARGS(p),
	    TP_STRUCT__entry(
				__field(unsigned int, pasid)
			    ),
	    TP_fast_assign(
			   __entry->pasid = p->pasid;
			   ),
	    TP_printk("pasid=%u", __entry->pasid)
);


TRACE_EVENT(kfd_evict_process_worker_end,
	    TP_PROTO(struct kfd_process *p, char *pStatusMsg),
	    TP_ARGS(p, pStatusMsg),
	    TP_STRUCT__entry(
			    __field(unsigned int, pasid)
			    __string(pStatusMsg, pStatusMsg)
			    ),
	    TP_fast_assign(
			    __entry->pasid = p->pasid;
			    __assign_str(pStatusMsg, pStatusMsg);
			   ),
	    TP_printk("pasid=%u, StatusMsg=%s",
			    __entry->pasid, __get_str(pStatusMsg))
);


TRACE_EVENT(kfd_restore_process_worker_start,
	    TP_PROTO(struct kfd_process *p),
	    TP_ARGS(p),
	    TP_STRUCT__entry(
				__field(unsigned int, pasid)
			    ),
	    TP_fast_assign(
			   __entry->pasid = p->pasid;
			   ),
	    TP_printk("pasid=%u", __entry->pasid)
);

TRACE_EVENT(kfd_restore_process_worker_end,
	    TP_PROTO(struct kfd_process *p, char *pStatusMsg),
	    TP_ARGS(p, pStatusMsg),
	    TP_STRUCT__entry(
				__field(unsigned int, pasid)
				__string(pStatusMsg, pStatusMsg)
			    ),
	    TP_fast_assign(
				 entry->pasid = p->pasid;
				__assign_str(pStatusMsg, pStatusMsg);
			   ),
	    TP_printk("pasid=%u, StatusMsg=%s",
			    __entry->pasid, __get_str(pStatusMsg))
);

#endif

/* This part must be outside protection */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#include <trace/define_trace.h>
