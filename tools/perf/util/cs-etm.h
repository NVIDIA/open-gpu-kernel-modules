/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright(C) 2015 Linaro Limited. All rights reserved.
 * Author: Mathieu Poirier <mathieu.poirier@linaro.org>
 */

#ifndef INCLUDE__UTIL_PERF_CS_ETM_H__
#define INCLUDE__UTIL_PERF_CS_ETM_H__

#include "util/event.h"
#include <linux/bits.h>

struct perf_session;

/*
 * Versioning header in case things need to change in the future.  That way
 * decoding of old snapshot is still possible.
 */
enum {
	/* Starting with 0x0 */
	CS_HEADER_VERSION,
	/* PMU->type (32 bit), total # of CPUs (32 bit) */
	CS_PMU_TYPE_CPUS,
	CS_ETM_SNAPSHOT,
	CS_HEADER_VERSION_MAX,
};

/*
 * Update the version for new format.
 *
 * New version 1 format adds a param count to the per cpu metadata.
 * This allows easy adding of new metadata parameters.
 * Requires that new params always added after current ones.
 * Also allows client reader to handle file versions that are different by
 * checking the number of params in the file vs the number expected.
 */
#define CS_HEADER_CURRENT_VERSION 1

/* Beginning of header common to both ETMv3 and V4 */
enum {
	CS_ETM_MAGIC,
	CS_ETM_CPU,
	/* Number of trace config params in following ETM specific block */
	CS_ETM_NR_TRC_PARAMS,
	CS_ETM_COMMON_BLK_MAX_V1,
};

/* ETMv3/PTM metadata */
enum {
	/* Dynamic, configurable parameters */
	CS_ETM_ETMCR = CS_ETM_COMMON_BLK_MAX_V1,
	CS_ETM_ETMTRACEIDR,
	/* RO, taken from sysFS */
	CS_ETM_ETMCCER,
	CS_ETM_ETMIDR,
	CS_ETM_PRIV_MAX,
};

/* define fixed version 0 length - allow new format reader to read old files. */
#define CS_ETM_NR_TRC_PARAMS_V0 (CS_ETM_ETMIDR - CS_ETM_ETMCR + 1)

/* ETMv4 metadata */
enum {
	/* Dynamic, configurable parameters */
	CS_ETMV4_TRCCONFIGR = CS_ETM_COMMON_BLK_MAX_V1,
	CS_ETMV4_TRCTRACEIDR,
	/* RO, taken from sysFS */
	CS_ETMV4_TRCIDR0,
	CS_ETMV4_TRCIDR1,
	CS_ETMV4_TRCIDR2,
	CS_ETMV4_TRCIDR8,
	CS_ETMV4_TRCAUTHSTATUS,
	CS_ETMV4_PRIV_MAX,
};

/* define fixed version 0 length - allow new format reader to read old files. */
#define CS_ETMV4_NR_TRC_PARAMS_V0 (CS_ETMV4_TRCAUTHSTATUS - CS_ETMV4_TRCCONFIGR + 1)

/*
 * ETMv3 exception encoding number:
 * See Embedded Trace Macrocell specification (ARM IHI 0014Q)
 * table 7-12 Encoding of Exception[3:0] for non-ARMv7-M processors.
 */
enum {
	CS_ETMV3_EXC_NONE = 0,
	CS_ETMV3_EXC_DEBUG_HALT = 1,
	CS_ETMV3_EXC_SMC = 2,
	CS_ETMV3_EXC_HYP = 3,
	CS_ETMV3_EXC_ASYNC_DATA_ABORT = 4,
	CS_ETMV3_EXC_JAZELLE_THUMBEE = 5,
	CS_ETMV3_EXC_PE_RESET = 8,
	CS_ETMV3_EXC_UNDEFINED_INSTR = 9,
	CS_ETMV3_EXC_SVC = 10,
	CS_ETMV3_EXC_PREFETCH_ABORT = 11,
	CS_ETMV3_EXC_DATA_FAULT = 12,
	CS_ETMV3_EXC_GENERIC = 13,
	CS_ETMV3_EXC_IRQ = 14,
	CS_ETMV3_EXC_FIQ = 15,
};

/*
 * ETMv4 exception encoding number:
 * See ARM Embedded Trace Macrocell Architecture Specification (ARM IHI 0064D)
 * table 6-12 Possible values for the TYPE field in an Exception instruction
 * trace packet, for ARMv7-A/R and ARMv8-A/R PEs.
 */
enum {
	CS_ETMV4_EXC_RESET = 0,
	CS_ETMV4_EXC_DEBUG_HALT = 1,
	CS_ETMV4_EXC_CALL = 2,
	CS_ETMV4_EXC_TRAP = 3,
	CS_ETMV4_EXC_SYSTEM_ERROR = 4,
	CS_ETMV4_EXC_INST_DEBUG = 6,
	CS_ETMV4_EXC_DATA_DEBUG = 7,
	CS_ETMV4_EXC_ALIGNMENT = 10,
	CS_ETMV4_EXC_INST_FAULT = 11,
	CS_ETMV4_EXC_DATA_FAULT = 12,
	CS_ETMV4_EXC_IRQ = 14,
	CS_ETMV4_EXC_FIQ = 15,
	CS_ETMV4_EXC_END = 31,
};

enum cs_etm_sample_type {
	CS_ETM_EMPTY,
	CS_ETM_RANGE,
	CS_ETM_DISCONTINUITY,
	CS_ETM_EXCEPTION,
	CS_ETM_EXCEPTION_RET,
};

enum cs_etm_isa {
	CS_ETM_ISA_UNKNOWN,
	CS_ETM_ISA_A64,
	CS_ETM_ISA_A32,
	CS_ETM_ISA_T32,
};

struct cs_etm_queue;

struct cs_etm_packet {
	enum cs_etm_sample_type sample_type;
	enum cs_etm_isa isa;
	u64 start_addr;
	u64 end_addr;
	u32 instr_count;
	u32 last_instr_type;
	u32 last_instr_subtype;
	u32 flags;
	u32 exception_number;
	u8 last_instr_cond;
	u8 last_instr_taken_branch;
	u8 last_instr_size;
	u8 trace_chan_id;
	int cpu;
};

#define CS_ETM_PACKET_MAX_BUFFER 1024

/*
 * When working with per-thread scenarios the process under trace can
 * be scheduled on any CPU and as such, more than one traceID may be
 * associated with the same process.  Since a traceID of '0' is illegal
 * as per the CoreSight architecture, use that specific value to
 * identify the queue where all packets (with any traceID) are
 * aggregated.
 */
#define CS_ETM_PER_THREAD_TRACEID 0

struct cs_etm_packet_queue {
	u32 packet_count;
	u32 head;
	u32 tail;
	u32 instr_count;
	u64 timestamp;
	u64 next_timestamp;
	struct cs_etm_packet packet_buffer[CS_ETM_PACKET_MAX_BUFFER];
};

#define KiB(x) ((x) * 1024)
#define MiB(x) ((x) * 1024 * 1024)

#define CS_ETM_INVAL_ADDR 0xdeadbeefdeadbeefUL

#define BMVAL(val, lsb, msb)	((val & GENMASK(msb, lsb)) >> lsb)

#define CS_ETM_HEADER_SIZE (CS_HEADER_VERSION_MAX * sizeof(u64))

#define __perf_cs_etmv3_magic 0x3030303030303030ULL
#define __perf_cs_etmv4_magic 0x4040404040404040ULL
#define CS_ETMV3_PRIV_SIZE (CS_ETM_PRIV_MAX * sizeof(u64))
#define CS_ETMV4_PRIV_SIZE (CS_ETMV4_PRIV_MAX * sizeof(u64))

#ifdef HAVE_CSTRACE_SUPPORT
int cs_etm__process_auxtrace_info(union perf_event *event,
				  struct perf_session *session);
int cs_etm__get_cpu(u8 trace_chan_id, int *cpu);
int cs_etm__get_pid_fmt(u8 trace_chan_id, u64 *pid_fmt);
int cs_etm__etmq_set_tid(struct cs_etm_queue *etmq,
			 pid_t tid, u8 trace_chan_id);
bool cs_etm__etmq_is_timeless(struct cs_etm_queue *etmq);
void cs_etm__etmq_set_traceid_queue_timestamp(struct cs_etm_queue *etmq,
					      u8 trace_chan_id);
struct cs_etm_packet_queue
*cs_etm__etmq_get_packet_queue(struct cs_etm_queue *etmq, u8 trace_chan_id);
#else
static inline int
cs_etm__process_auxtrace_info(union perf_event *event __maybe_unused,
			      struct perf_session *session __maybe_unused)
{
	return -1;
}

static inline int cs_etm__get_cpu(u8 trace_chan_id __maybe_unused,
				  int *cpu __maybe_unused)
{
	return -1;
}

static inline int cs_etm__etmq_set_tid(
				struct cs_etm_queue *etmq __maybe_unused,
				pid_t tid __maybe_unused,
				u8 trace_chan_id __maybe_unused)
{
	return -1;
}

static inline bool cs_etm__etmq_is_timeless(
				struct cs_etm_queue *etmq __maybe_unused)
{
	/* What else to return? */
	return true;
}

static inline void cs_etm__etmq_set_traceid_queue_timestamp(
				struct cs_etm_queue *etmq __maybe_unused,
				u8 trace_chan_id __maybe_unused) {}

static inline struct cs_etm_packet_queue *cs_etm__etmq_get_packet_queue(
				struct cs_etm_queue *etmq __maybe_unused,
				u8 trace_chan_id __maybe_unused)
{
	return NULL;
}
#endif

#endif
