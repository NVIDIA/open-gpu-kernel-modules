/*
 * Copyright 2019 Advanced Micro Devices, Inc.
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

#ifndef __MES_API_DEF_H__
#define __MES_API_DEF_H__

#pragma pack(push, 4)

#define MES_API_VERSION 1

/* Driver submits one API(cmd) as a single Frame and this command size is same
 * for all API to ease the debugging and parsing of ring buffer.
 */
enum { API_FRAME_SIZE_IN_DWORDS = 64 };

/* To avoid command in scheduler context to be overwritten whenenver mutilple
 * interrupts come in, this creates another queue.
 */
enum { API_NUMBER_OF_COMMAND_MAX = 32 };

enum MES_API_TYPE {
	MES_API_TYPE_SCHEDULER = 1,
	MES_API_TYPE_MAX
};

enum MES_SCH_API_OPCODE {
	MES_SCH_API_SET_HW_RSRC			= 0,
	MES_SCH_API_SET_SCHEDULING_CONFIG	= 1, /* agreegated db, quantums, etc */
	MES_SCH_API_ADD_QUEUE			= 2,
	MES_SCH_API_REMOVE_QUEUE		= 3,
	MES_SCH_API_PERFORM_YIELD		= 4,
	MES_SCH_API_SET_GANG_PRIORITY_LEVEL	= 5,
	MES_SCH_API_SUSPEND			= 6,
	MES_SCH_API_RESUME			= 7,
	MES_SCH_API_RESET			= 8,
	MES_SCH_API_SET_LOG_BUFFER		= 9,
	MES_SCH_API_CHANGE_GANG_PRORITY		= 10,
	MES_SCH_API_QUERY_SCHEDULER_STATUS	= 11,
	MES_SCH_API_PROGRAM_GDS			= 12,
	MES_SCH_API_SET_DEBUG_VMID		= 13,
	MES_SCH_API_MISC			= 14,
	MES_SCH_API_MAX				= 0xFF
};

union MES_API_HEADER {
	struct {
		uint32_t type		: 4; /* 0 - Invalid; 1 - Scheduling; 2 - TBD */
		uint32_t opcode		: 8;
		uint32_t dwsize		: 8; /* including header */
		uint32_t reserved	: 12;
	};

	uint32_t	u32All;
};

enum MES_AMD_PRIORITY_LEVEL {
	AMD_PRIORITY_LEVEL_LOW		= 0,
	AMD_PRIORITY_LEVEL_NORMAL	= 1,
	AMD_PRIORITY_LEVEL_MEDIUM	= 2,
	AMD_PRIORITY_LEVEL_HIGH		= 3,
	AMD_PRIORITY_LEVEL_REALTIME	= 4,
	AMD_PRIORITY_NUM_LEVELS
};

enum MES_QUEUE_TYPE {
	MES_QUEUE_TYPE_GFX,
	MES_QUEUE_TYPE_COMPUTE,
	MES_QUEUE_TYPE_SDMA,
	MES_QUEUE_TYPE_MAX,
};

struct MES_API_STATUS {
	uint64_t	api_completion_fence_addr;
	uint64_t	api_completion_fence_value;
};

enum { MAX_COMPUTE_PIPES = 8 };
enum { MAX_GFX_PIPES = 2 };
enum { MAX_SDMA_PIPES = 2 };

enum { MAX_COMPUTE_HQD_PER_PIPE = 8 };
enum { MAX_GFX_HQD_PER_PIPE = 8 };
enum { MAX_SDMA_HQD_PER_PIPE = 10 };

enum { MAX_QUEUES_IN_A_GANG = 8 };

enum VM_HUB_TYPE {
	VM_HUB_TYPE_GC = 0,
	VM_HUB_TYPE_MM = 1,
	VM_HUB_TYPE_MAX,
};

enum { VMID_INVALID = 0xffff };

enum { MAX_VMID_GCHUB = 16 };
enum { MAX_VMID_MMHUB = 16 };

enum MES_LOG_OPERATION {
	MES_LOG_OPERATION_CONTEXT_STATE_CHANGE = 0
};

enum MES_LOG_CONTEXT_STATE {
	MES_LOG_CONTEXT_STATE_IDLE		= 0,
	MES_LOG_CONTEXT_STATE_RUNNING		= 1,
	MES_LOG_CONTEXT_STATE_READY		= 2,
	MES_LOG_CONTEXT_STATE_READY_STANDBY	= 3,
};

struct MES_LOG_CONTEXT_STATE_CHANGE {
	void				*h_context;
	enum MES_LOG_CONTEXT_STATE	new_context_state;
};

struct MES_LOG_ENTRY_HEADER {
	uint32_t	first_free_entry_index;
	uint32_t	wraparound_count;
	uint64_t	number_of_entries;
	uint64_t	reserved[2];
};

struct MES_LOG_ENTRY_DATA {
	uint64_t	gpu_time_stamp;
	uint32_t	operation_type; /* operation_type is of MES_LOG_OPERATION type */
	uint32_t	reserved_operation_type_bits;
	union {
		struct MES_LOG_CONTEXT_STATE_CHANGE	context_state_change;
		uint64_t				reserved_operation_data[2];
	};
};

struct MES_LOG_BUFFER {
	struct MES_LOG_ENTRY_HEADER	header;
	struct MES_LOG_ENTRY_DATA	entries[1];
};

union MESAPI_SET_HW_RESOURCES {
	struct {
		union MES_API_HEADER	header;
		uint32_t		vmid_mask_mmhub;
		uint32_t		vmid_mask_gfxhub;
		uint32_t		gds_size;
		uint32_t		paging_vmid;
		uint32_t		compute_hqd_mask[MAX_COMPUTE_PIPES];
		uint32_t		gfx_hqd_mask[MAX_GFX_PIPES];
		uint32_t		sdma_hqd_mask[MAX_SDMA_PIPES];
		uint32_t		agreegated_doorbells[AMD_PRIORITY_NUM_LEVELS];
		uint64_t		g_sch_ctx_gpu_mc_ptr;
		uint64_t		query_status_fence_gpu_mc_ptr;
		struct MES_API_STATUS	api_status;
		union {
			struct {
				uint32_t disable_reset	: 1;
				uint32_t reserved	: 31;
			};
			uint32_t	uint32_t_all;
		};
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__ADD_QUEUE {
	struct {
		union MES_API_HEADER		header;
		uint32_t			process_id;
		uint64_t			page_table_base_addr;
		uint64_t			process_va_start;
		uint64_t			process_va_end;
		uint64_t			process_quantum;
		uint64_t			process_context_addr;
		uint64_t			gang_quantum;
		uint64_t			gang_context_addr;
		uint32_t			inprocess_gang_priority;
		enum MES_AMD_PRIORITY_LEVEL	gang_global_priority_level;
		uint32_t			doorbell_offset;
		uint64_t			mqd_addr;
		uint64_t			wptr_addr;
		enum MES_QUEUE_TYPE		queue_type;
		uint32_t			gds_base;
		uint32_t			gds_size;
		uint32_t			gws_base;
		uint32_t			gws_size;
		uint32_t			oa_mask;

		struct {
			uint32_t paging			: 1;
			uint32_t debug_vmid		: 4;
			uint32_t program_gds		: 1;
			uint32_t is_gang_suspended	: 1;
			uint32_t is_tmz_queue		: 1;
			uint32_t reserved		: 24;
		};
		struct MES_API_STATUS		api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__REMOVE_QUEUE {
	struct {
		union MES_API_HEADER	header;
		uint32_t		doorbell_offset;
		uint64_t		gang_context_addr;

		struct {
			uint32_t unmap_legacy_gfx_queue	: 1;
			uint32_t reserved		: 31;
		};
		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__SET_SCHEDULING_CONFIG {
	struct {
		union MES_API_HEADER	header;
		/* Grace period when preempting another priority band for this
		 * priority band. The value for idle priority band is ignored,
		 * as it never preempts other bands.
		 */
		uint64_t		grace_period_other_levels[AMD_PRIORITY_NUM_LEVELS];
		/* Default quantum for scheduling across processes within
		 * a priority band.
		 */
		uint64_t		process_quantum_for_level[AMD_PRIORITY_NUM_LEVELS];
		/* Default grace period for processes that preempt each other
		 * within a priority band.
		 */
		uint64_t		process_grace_period_same_level[AMD_PRIORITY_NUM_LEVELS];
		/* For normal level this field specifies the target GPU
		 * percentage in situations when it's starved by the high level.
		 * Valid values are between 0 and 50, with the default being 10.
		 */
		uint32_t		normal_yield_percent;
		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__PERFORM_YIELD {
	struct {
		union MES_API_HEADER	header;
		uint32_t		dummy;
		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__CHANGE_GANG_PRIORITY_LEVEL {
	struct {
		union MES_API_HEADER		header;
		uint32_t			inprocess_gang_priority;
		enum MES_AMD_PRIORITY_LEVEL	gang_global_priority_level;
		uint64_t			gang_quantum;
		uint64_t			gang_context_addr;
		struct MES_API_STATUS		api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__SUSPEND {
	struct {
		union MES_API_HEADER	header;
		/* false - suspend all gangs; true - specific gang */
		struct {
			uint32_t suspend_all_gangs	: 1;
			uint32_t reserved		: 31;
		};
		/* gang_context_addr is valid only if suspend_all = false */
		uint64_t		gang_context_addr;

		uint64_t		suspend_fence_addr;
		uint32_t		suspend_fence_value;

		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__RESUME {
	struct {
		union MES_API_HEADER	header;
		/* false - resume all gangs; true - specified gang */
		struct {
			uint32_t resume_all_gangs	: 1;
			uint32_t reserved		: 31;
		};
		/* valid only if resume_all_gangs = false */
		uint64_t		gang_context_addr;

		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__RESET {
	struct {
		union MES_API_HEADER	header;

		struct {
			uint32_t reset_queue	: 1;
			uint32_t reserved	: 31;
		};

		uint64_t		gang_context_addr;
		uint32_t		doorbell_offset; /* valid only if reset_queue = true */
		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__SET_LOGGING_BUFFER {
	struct {
		union MES_API_HEADER	header;
		/* There are separate log buffers for each queue type */
		enum MES_QUEUE_TYPE	log_type;
		/* Log buffer GPU Address */
		uint64_t		logging_buffer_addr;
		/* number of entries in the log buffer */
		uint32_t		number_of_entries;
		/* Entry index at which CPU interrupt needs to be signalled */
		uint32_t		interrupt_entry;

		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__QUERY_MES_STATUS {
	struct {
		union MES_API_HEADER	header;
		bool			mes_healthy; /* 0 - not healthy, 1 - healthy */
		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__PROGRAM_GDS {
	struct {
		union MES_API_HEADER	header;
		uint64_t		process_context_addr;
		uint32_t		gds_base;
		uint32_t		gds_size;
		uint32_t		gws_base;
		uint32_t		gws_size;
		uint32_t		oa_mask;
		struct MES_API_STATUS	api_status;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

union MESAPI__SET_DEBUG_VMID {
	struct {
		union MES_API_HEADER	header;
		struct MES_API_STATUS	api_status;
		union {
			struct {
				uint32_t use_gds	: 1;
				uint32_t reserved	: 31;
			} flags;
			uint32_t	u32All;
		};
		uint32_t		reserved;
		uint32_t		debug_vmid;
		uint64_t		process_context_addr;
		uint64_t		page_table_base_addr;
		uint64_t		process_va_start;
		uint64_t		process_va_end;
		uint32_t		gds_base;
		uint32_t		gds_size;
		uint32_t		gws_base;
		uint32_t		gws_size;
		uint32_t		oa_mask;
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

enum MESAPI_MISC_OPCODE {
	MESAPI_MISC__MODIFY_REG,
	MESAPI_MISC__MAX,
};

enum MODIFY_REG_SUBCODE {
	MODIFY_REG__OVERWRITE,
	MODIFY_REG__RMW_OR,
	MODIFY_REG__RMW_AND,
	MODIFY_REG__MAX,
};

enum { MISC_DATA_MAX_SIZE_IN_DWORDS = 20 };

union MESAPI__MISC {
	struct {
		union MES_API_HEADER	header;
		enum MESAPI_MISC_OPCODE	opcode;
		struct MES_API_STATUS	api_status;

		union {
			struct {
				enum MODIFY_REG_SUBCODE	subcode;
				uint32_t		reg_offset;
				uint32_t		reg_value;
			} modify_reg;
			uint32_t	data[MISC_DATA_MAX_SIZE_IN_DWORDS];
		};
	};

	uint32_t	max_dwords_in_api[API_FRAME_SIZE_IN_DWORDS];
};

#pragma pack(pop)
#endif
