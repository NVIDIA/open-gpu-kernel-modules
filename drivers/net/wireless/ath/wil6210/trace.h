/* SPDX-License-Identifier: ISC */
/*
 * Copyright (c) 2013-2016 Qualcomm Atheros, Inc.
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM wil6210
#if !defined(WIL6210_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define WIL6210_TRACE_H

#include <linux/tracepoint.h>
#include "wil6210.h"
#include "txrx.h"

/* create empty functions when tracing is disabled */
#if !defined(CONFIG_WIL6210_TRACING) || defined(__CHECKER__)

#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, ...) \
static inline void trace_ ## name(proto) {}
#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(...)
#undef DEFINE_EVENT
#define DEFINE_EVENT(evt_class, name, proto, ...) \
static inline void trace_ ## name(proto) {}
#endif /* !CONFIG_WIL6210_TRACING || defined(__CHECKER__) */

DECLARE_EVENT_CLASS(wil6210_wmi,
	TP_PROTO(struct wmi_cmd_hdr *wmi, void *buf, u16 buf_len),

	TP_ARGS(wmi, buf, buf_len),

	TP_STRUCT__entry(
		__field(u8, mid)
		__field(u16, command_id)
		__field(u32, fw_timestamp)
		__field(u16, buf_len)
		__dynamic_array(u8, buf, buf_len)
	),

	TP_fast_assign(
		__entry->mid = wmi->mid;
		__entry->command_id = le16_to_cpu(wmi->command_id);
		__entry->fw_timestamp = le32_to_cpu(wmi->fw_timestamp);
		__entry->buf_len = buf_len;
		memcpy(__get_dynamic_array(buf), buf, buf_len);
	),

	TP_printk(
		"MID %d id 0x%04x len %d timestamp %d",
		__entry->mid, __entry->command_id, __entry->buf_len,
		__entry->fw_timestamp
	)
);

DEFINE_EVENT(wil6210_wmi, wil6210_wmi_cmd,
	TP_PROTO(struct wmi_cmd_hdr *wmi, void *buf, u16 buf_len),
	TP_ARGS(wmi, buf, buf_len)
);

DEFINE_EVENT(wil6210_wmi, wil6210_wmi_event,
	TP_PROTO(struct wmi_cmd_hdr *wmi, void *buf, u16 buf_len),
	TP_ARGS(wmi, buf, buf_len)
);

#define WIL6210_MSG_MAX (200)

DECLARE_EVENT_CLASS(wil6210_log_event,
	TP_PROTO(struct va_format *vaf),
	TP_ARGS(vaf),
	TP_STRUCT__entry(
		__dynamic_array(char, msg, WIL6210_MSG_MAX)
	),
	TP_fast_assign(
		WARN_ON_ONCE(vsnprintf(__get_dynamic_array(msg),
				       WIL6210_MSG_MAX,
				       vaf->fmt,
				       *vaf->va) >= WIL6210_MSG_MAX);
	),
	TP_printk("%s", __get_str(msg))
);

DEFINE_EVENT(wil6210_log_event, wil6210_log_err,
	TP_PROTO(struct va_format *vaf),
	TP_ARGS(vaf)
);

DEFINE_EVENT(wil6210_log_event, wil6210_log_info,
	TP_PROTO(struct va_format *vaf),
	TP_ARGS(vaf)
);

DEFINE_EVENT(wil6210_log_event, wil6210_log_dbg,
	TP_PROTO(struct va_format *vaf),
	TP_ARGS(vaf)
);

#define wil_pseudo_irq_cause(x) __print_flags(x, "|",	\
	{BIT_DMA_PSEUDO_CAUSE_RX,	"Rx" },		\
	{BIT_DMA_PSEUDO_CAUSE_TX,	"Tx" },		\
	{BIT_DMA_PSEUDO_CAUSE_MISC,	"Misc" })

TRACE_EVENT(wil6210_irq_pseudo,
	TP_PROTO(u32 x),
	TP_ARGS(x),
	TP_STRUCT__entry(
		__field(u32, x)
	),
	TP_fast_assign(
		__entry->x = x;
	),
	TP_printk("cause 0x%08x : %s", __entry->x,
		  wil_pseudo_irq_cause(__entry->x))
);

DECLARE_EVENT_CLASS(wil6210_irq,
	TP_PROTO(u32 x),
	TP_ARGS(x),
	TP_STRUCT__entry(
		__field(u32, x)
	),
	TP_fast_assign(
		__entry->x = x;
	),
	TP_printk("cause 0x%08x", __entry->x)
);

DEFINE_EVENT(wil6210_irq, wil6210_irq_rx,
	TP_PROTO(u32 x),
	TP_ARGS(x)
);

DEFINE_EVENT(wil6210_irq, wil6210_irq_tx,
	TP_PROTO(u32 x),
	TP_ARGS(x)
);

DEFINE_EVENT(wil6210_irq, wil6210_irq_misc,
	TP_PROTO(u32 x),
	TP_ARGS(x)
);

DEFINE_EVENT(wil6210_irq, wil6210_irq_misc_thread,
	TP_PROTO(u32 x),
	TP_ARGS(x)
);

TRACE_EVENT(wil6210_rx,
	TP_PROTO(u16 index, struct vring_rx_desc *d),
	TP_ARGS(index, d),
	TP_STRUCT__entry(
		__field(u16, index)
		__field(unsigned int, len)
		__field(u8, mid)
		__field(u8, cid)
		__field(u8, tid)
		__field(u8, type)
		__field(u8, subtype)
		__field(u16, seq)
		__field(u8, mcs)
	),
	TP_fast_assign(
		__entry->index = index;
		__entry->len = d->dma.length;
		__entry->mid = wil_rxdesc_mid(d);
		__entry->cid = wil_rxdesc_cid(d);
		__entry->tid = wil_rxdesc_tid(d);
		__entry->type = wil_rxdesc_ftype(d);
		__entry->subtype = wil_rxdesc_subtype(d);
		__entry->seq = wil_rxdesc_seq(d);
		__entry->mcs = wil_rxdesc_mcs(d);
	),
	TP_printk("index %d len %d mid %d cid (%%8) %d tid %d mcs %d seq 0x%03x"
		  " type 0x%1x subtype 0x%1x", __entry->index, __entry->len,
		  __entry->mid, __entry->cid, __entry->tid, __entry->mcs,
		  __entry->seq, __entry->type, __entry->subtype)
);

TRACE_EVENT(wil6210_rx_status,
	    TP_PROTO(struct wil6210_priv *wil, u8 use_compressed, u16 buff_id,
		     void *msg),
	    TP_ARGS(wil, use_compressed, buff_id, msg),
	    TP_STRUCT__entry(__field(u8, use_compressed)
			     __field(u16, buff_id)
			     __field(unsigned int, len)
			     __field(u8, mid)
			     __field(u8, cid)
			     __field(u8, tid)
			     __field(u8, type)
			     __field(u8, subtype)
			     __field(u16, seq)
			     __field(u8, mcs)
	    ),
	    TP_fast_assign(__entry->use_compressed = use_compressed;
			   __entry->buff_id = buff_id;
			   __entry->len = wil_rx_status_get_length(msg);
			   __entry->mid = wil_rx_status_get_mid(msg);
			   __entry->cid = wil_rx_status_get_cid(msg);
			   __entry->tid = wil_rx_status_get_tid(msg);
			   __entry->type = wil_rx_status_get_frame_type(wil,
									msg);
			   __entry->subtype = wil_rx_status_get_fc1(wil, msg);
			   __entry->seq = wil_rx_status_get_seq(wil, msg);
			   __entry->mcs = wil_rx_status_get_mcs(msg);
	    ),
	    TP_printk(
		      "compressed %d buff_id %d len %d mid %d cid %d tid %d mcs %d seq 0x%03x type 0x%1x subtype 0x%1x",
		      __entry->use_compressed, __entry->buff_id, __entry->len,
		      __entry->mid, __entry->cid, __entry->tid, __entry->mcs,
		      __entry->seq, __entry->type, __entry->subtype)
);

TRACE_EVENT(wil6210_tx,
	TP_PROTO(u8 vring, u16 index, unsigned int len, u8 frags),
	TP_ARGS(vring, index, len, frags),
	TP_STRUCT__entry(
		__field(u8, vring)
		__field(u8, frags)
		__field(u16, index)
		__field(unsigned int, len)
	),
	TP_fast_assign(
		__entry->vring = vring;
		__entry->frags = frags;
		__entry->index = index;
		__entry->len = len;
	),
	TP_printk("vring %d index %d len %d frags %d",
		  __entry->vring, __entry->index, __entry->len, __entry->frags)
);

TRACE_EVENT(wil6210_tx_done,
	TP_PROTO(u8 vring, u16 index, unsigned int len, u8 err),
	TP_ARGS(vring, index, len, err),
	TP_STRUCT__entry(
		__field(u8, vring)
		__field(u8, err)
		__field(u16, index)
		__field(unsigned int, len)
	),
	TP_fast_assign(
		__entry->vring = vring;
		__entry->index = index;
		__entry->len = len;
		__entry->err = err;
	),
	TP_printk("vring %d index %d len %d err 0x%02x",
		  __entry->vring, __entry->index, __entry->len,
		  __entry->err)
);

TRACE_EVENT(wil6210_tx_status,
	    TP_PROTO(struct wil_ring_tx_status *msg, u16 index,
		     unsigned int len),
	    TP_ARGS(msg, index, len),
	    TP_STRUCT__entry(__field(u16, index)
			     __field(unsigned int, len)
			     __field(u8, num_descs)
			     __field(u8, ring_id)
			     __field(u8, status)
			     __field(u8, mcs)

	    ),
	    TP_fast_assign(__entry->index = index;
			   __entry->len = len;
			   __entry->num_descs = msg->num_descriptors;
			   __entry->ring_id = msg->ring_id;
			   __entry->status = msg->status;
			   __entry->mcs = wil_tx_status_get_mcs(msg);
	    ),
	    TP_printk(
		      "ring_id %d swtail 0x%x len %d num_descs %d status 0x%x mcs %d",
		      __entry->ring_id, __entry->index, __entry->len,
		      __entry->num_descs, __entry->status, __entry->mcs)
);

#endif /* WIL6210_TRACE_H || TRACE_HEADER_MULTI_READ*/

#if defined(CONFIG_WIL6210_TRACING) && !defined(__CHECKER__)
/* we don't want to use include/trace/events */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE trace

/* This part must be outside protection */
#include <trace/define_trace.h>
#endif /* defined(CONFIG_WIL6210_TRACING) && !defined(__CHECKER__) */
