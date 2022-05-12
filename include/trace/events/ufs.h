/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM ufs

#if !defined(_TRACE_UFS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_UFS_H

#include <linux/tracepoint.h>

#define str_opcode(opcode)						\
	__print_symbolic(opcode,					\
		{ WRITE_16,		"WRITE_16" },			\
		{ WRITE_10,		"WRITE_10" },			\
		{ READ_16,		"READ_16" },			\
		{ READ_10,		"READ_10" },			\
		{ SYNCHRONIZE_CACHE,	"SYNC" },			\
		{ UNMAP,		"UNMAP" })

#define UFS_LINK_STATES						\
	EM(UIC_LINK_OFF_STATE,		"UIC_LINK_OFF_STATE")		\
	EM(UIC_LINK_ACTIVE_STATE,	"UIC_LINK_ACTIVE_STATE")	\
	EMe(UIC_LINK_HIBERN8_STATE,	"UIC_LINK_HIBERN8_STATE")

#define UFS_PWR_MODES							\
	EM(UFS_ACTIVE_PWR_MODE,		"UFS_ACTIVE_PWR_MODE")		\
	EM(UFS_SLEEP_PWR_MODE,		"UFS_SLEEP_PWR_MODE")		\
	EM(UFS_POWERDOWN_PWR_MODE,	"UFS_POWERDOWN_PWR_MODE")	\
	EMe(UFS_DEEPSLEEP_PWR_MODE,	"UFS_DEEPSLEEP_PWR_MODE")

#define UFSCHD_CLK_GATING_STATES				\
	EM(CLKS_OFF,			"CLKS_OFF")		\
	EM(CLKS_ON,			"CLKS_ON")		\
	EM(REQ_CLKS_OFF,		"REQ_CLKS_OFF")		\
	EMe(REQ_CLKS_ON,		"REQ_CLKS_ON")

#define UFS_CMD_TRACE_STRINGS					\
	EM(UFS_CMD_SEND,	"send_req")			\
	EM(UFS_CMD_COMP,	"complete_rsp")			\
	EM(UFS_DEV_COMP,	"dev_complete")			\
	EM(UFS_QUERY_SEND,	"query_send")			\
	EM(UFS_QUERY_COMP,	"query_complete")		\
	EM(UFS_QUERY_ERR,	"query_complete_err")		\
	EM(UFS_TM_SEND,		"tm_send")			\
	EM(UFS_TM_COMP,		"tm_complete")			\
	EMe(UFS_TM_ERR,		"tm_complete_err")

#define UFS_CMD_TRACE_TSF_TYPES					\
	EM(UFS_TSF_CDB,		"CDB")		                \
	EM(UFS_TSF_OSF,		"OSF")		                \
	EM(UFS_TSF_TM_INPUT,	"TM_INPUT")                     \
	EMe(UFS_TSF_TM_OUTPUT,	"TM_OUTPUT")

/* Enums require being exported to userspace, for user tool parsing */
#undef EM
#undef EMe
#define EM(a, b)	TRACE_DEFINE_ENUM(a);
#define EMe(a, b)	TRACE_DEFINE_ENUM(a);

UFS_LINK_STATES;
UFS_PWR_MODES;
UFSCHD_CLK_GATING_STATES;
UFS_CMD_TRACE_STRINGS
UFS_CMD_TRACE_TSF_TYPES

/*
 * Now redefine the EM() and EMe() macros to map the enums to the strings
 * that will be printed in the output.
 */
#undef EM
#undef EMe
#define EM(a, b)	{a, b},
#define EMe(a, b)	{a, b}

#define show_ufs_cmd_trace_str(str_t)	\
				__print_symbolic(str_t, UFS_CMD_TRACE_STRINGS)
#define show_ufs_cmd_trace_tsf(tsf)	\
				__print_symbolic(tsf, UFS_CMD_TRACE_TSF_TYPES)

TRACE_EVENT(ufshcd_clk_gating,

	TP_PROTO(const char *dev_name, int state),

	TP_ARGS(dev_name, state),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__field(int, state)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__entry->state = state;
	),

	TP_printk("%s: gating state changed to %s",
		__get_str(dev_name),
		__print_symbolic(__entry->state, UFSCHD_CLK_GATING_STATES))
);

TRACE_EVENT(ufshcd_clk_scaling,

	TP_PROTO(const char *dev_name, const char *state, const char *clk,
		u32 prev_state, u32 curr_state),

	TP_ARGS(dev_name, state, clk, prev_state, curr_state),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__string(state, state)
		__string(clk, clk)
		__field(u32, prev_state)
		__field(u32, curr_state)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__assign_str(state, state);
		__assign_str(clk, clk);
		__entry->prev_state = prev_state;
		__entry->curr_state = curr_state;
	),

	TP_printk("%s: %s %s from %u to %u Hz",
		__get_str(dev_name), __get_str(state), __get_str(clk),
		__entry->prev_state, __entry->curr_state)
);

TRACE_EVENT(ufshcd_auto_bkops_state,

	TP_PROTO(const char *dev_name, const char *state),

	TP_ARGS(dev_name, state),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__string(state, state)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__assign_str(state, state);
	),

	TP_printk("%s: auto bkops - %s",
		__get_str(dev_name), __get_str(state))
);

DECLARE_EVENT_CLASS(ufshcd_profiling_template,
	TP_PROTO(const char *dev_name, const char *profile_info, s64 time_us,
		 int err),

	TP_ARGS(dev_name, profile_info, time_us, err),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__string(profile_info, profile_info)
		__field(s64, time_us)
		__field(int, err)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__assign_str(profile_info, profile_info);
		__entry->time_us = time_us;
		__entry->err = err;
	),

	TP_printk("%s: %s: took %lld usecs, err %d",
		__get_str(dev_name), __get_str(profile_info),
		__entry->time_us, __entry->err)
);

DEFINE_EVENT(ufshcd_profiling_template, ufshcd_profile_hibern8,
	TP_PROTO(const char *dev_name, const char *profile_info, s64 time_us,
		 int err),
	TP_ARGS(dev_name, profile_info, time_us, err));

DEFINE_EVENT(ufshcd_profiling_template, ufshcd_profile_clk_gating,
	TP_PROTO(const char *dev_name, const char *profile_info, s64 time_us,
		 int err),
	TP_ARGS(dev_name, profile_info, time_us, err));

DEFINE_EVENT(ufshcd_profiling_template, ufshcd_profile_clk_scaling,
	TP_PROTO(const char *dev_name, const char *profile_info, s64 time_us,
		 int err),
	TP_ARGS(dev_name, profile_info, time_us, err));

DECLARE_EVENT_CLASS(ufshcd_template,
	TP_PROTO(const char *dev_name, int err, s64 usecs,
		 int dev_state, int link_state),

	TP_ARGS(dev_name, err, usecs, dev_state, link_state),

	TP_STRUCT__entry(
		__field(s64, usecs)
		__field(int, err)
		__string(dev_name, dev_name)
		__field(int, dev_state)
		__field(int, link_state)
	),

	TP_fast_assign(
		__entry->usecs = usecs;
		__entry->err = err;
		__assign_str(dev_name, dev_name);
		__entry->dev_state = dev_state;
		__entry->link_state = link_state;
	),

	TP_printk(
		"%s: took %lld usecs, dev_state: %s, link_state: %s, err %d",
		__get_str(dev_name),
		__entry->usecs,
		__print_symbolic(__entry->dev_state, UFS_PWR_MODES),
		__print_symbolic(__entry->link_state, UFS_LINK_STATES),
		__entry->err
	)
);

DEFINE_EVENT(ufshcd_template, ufshcd_system_suspend,
	     TP_PROTO(const char *dev_name, int err, s64 usecs,
		      int dev_state, int link_state),
	     TP_ARGS(dev_name, err, usecs, dev_state, link_state));

DEFINE_EVENT(ufshcd_template, ufshcd_system_resume,
	     TP_PROTO(const char *dev_name, int err, s64 usecs,
		      int dev_state, int link_state),
	     TP_ARGS(dev_name, err, usecs, dev_state, link_state));

DEFINE_EVENT(ufshcd_template, ufshcd_runtime_suspend,
	     TP_PROTO(const char *dev_name, int err, s64 usecs,
		      int dev_state, int link_state),
	     TP_ARGS(dev_name, err, usecs, dev_state, link_state));

DEFINE_EVENT(ufshcd_template, ufshcd_runtime_resume,
	     TP_PROTO(const char *dev_name, int err, s64 usecs,
		      int dev_state, int link_state),
	     TP_ARGS(dev_name, err, usecs, dev_state, link_state));

DEFINE_EVENT(ufshcd_template, ufshcd_init,
	     TP_PROTO(const char *dev_name, int err, s64 usecs,
		      int dev_state, int link_state),
	     TP_ARGS(dev_name, err, usecs, dev_state, link_state));

TRACE_EVENT(ufshcd_command,
	TP_PROTO(const char *dev_name, enum ufs_trace_str_t str_t,
		 unsigned int tag, u32 doorbell, int transfer_len, u32 intr,
		 u64 lba, u8 opcode, u8 group_id),

	TP_ARGS(dev_name, str_t, tag, doorbell, transfer_len,
				intr, lba, opcode, group_id),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__field(enum ufs_trace_str_t, str_t)
		__field(unsigned int, tag)
		__field(u32, doorbell)
		__field(int, transfer_len)
		__field(u32, intr)
		__field(u64, lba)
		__field(u8, opcode)
		__field(u8, group_id)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__entry->str_t = str_t;
		__entry->tag = tag;
		__entry->doorbell = doorbell;
		__entry->transfer_len = transfer_len;
		__entry->intr = intr;
		__entry->lba = lba;
		__entry->opcode = opcode;
		__entry->group_id = group_id;
	),

	TP_printk(
		"%s: %s: tag: %u, DB: 0x%x, size: %d, IS: %u, LBA: %llu, opcode: 0x%x (%s), group_id: 0x%x",
		show_ufs_cmd_trace_str(__entry->str_t), __get_str(dev_name),
		__entry->tag, __entry->doorbell, __entry->transfer_len,
		__entry->intr, __entry->lba, (u32)__entry->opcode,
		str_opcode(__entry->opcode), (u32)__entry->group_id
	)
);

TRACE_EVENT(ufshcd_uic_command,
	TP_PROTO(const char *dev_name, enum ufs_trace_str_t str_t, u32 cmd,
		 u32 arg1, u32 arg2, u32 arg3),

	TP_ARGS(dev_name, str_t, cmd, arg1, arg2, arg3),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__field(enum ufs_trace_str_t, str_t)
		__field(u32, cmd)
		__field(u32, arg1)
		__field(u32, arg2)
		__field(u32, arg3)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__entry->str_t = str_t;
		__entry->cmd = cmd;
		__entry->arg1 = arg1;
		__entry->arg2 = arg2;
		__entry->arg3 = arg3;
	),

	TP_printk(
		"%s: %s: cmd: 0x%x, arg1: 0x%x, arg2: 0x%x, arg3: 0x%x",
		show_ufs_cmd_trace_str(__entry->str_t), __get_str(dev_name),
		__entry->cmd, __entry->arg1, __entry->arg2, __entry->arg3
	)
);

TRACE_EVENT(ufshcd_upiu,
	TP_PROTO(const char *dev_name, enum ufs_trace_str_t str_t, void *hdr,
		 void *tsf, enum ufs_trace_tsf_t tsf_t),

	TP_ARGS(dev_name, str_t, hdr, tsf, tsf_t),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__field(enum ufs_trace_str_t, str_t)
		__array(unsigned char, hdr, 12)
		__array(unsigned char, tsf, 16)
		__field(enum ufs_trace_tsf_t, tsf_t)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__entry->str_t = str_t;
		memcpy(__entry->hdr, hdr, sizeof(__entry->hdr));
		memcpy(__entry->tsf, tsf, sizeof(__entry->tsf));
		__entry->tsf_t = tsf_t;
	),

	TP_printk(
		"%s: %s: HDR:%s, %s:%s",
		show_ufs_cmd_trace_str(__entry->str_t), __get_str(dev_name),
		__print_hex(__entry->hdr, sizeof(__entry->hdr)),
		show_ufs_cmd_trace_tsf(__entry->tsf_t),
		__print_hex(__entry->tsf, sizeof(__entry->tsf))
	)
);

TRACE_EVENT(ufshcd_exception_event,

	TP_PROTO(const char *dev_name, u16 status),

	TP_ARGS(dev_name, status),

	TP_STRUCT__entry(
		__string(dev_name, dev_name)
		__field(u16, status)
	),

	TP_fast_assign(
		__assign_str(dev_name, dev_name);
		__entry->status = status;
	),

	TP_printk("%s: status 0x%x",
		__get_str(dev_name), __entry->status
	)
);

#endif /* if !defined(_TRACE_UFS_H) || defined(TRACE_HEADER_MULTI_READ) */

/* This part must be outside protection */
#include <trace/define_trace.h>
