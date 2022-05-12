/* SPDX-License-Identifier: GPL-2.0 */
#if !defined(__TRACE_ATH5K_H) || defined(TRACE_HEADER_MULTI_READ)
#define __TRACE_ATH5K_H

#include <linux/tracepoint.h>


#if !defined(CONFIG_ATH5K_TRACER) || defined(__CHECKER__)
#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, ...) \
static inline void trace_ ## name(proto) {}
#endif

struct sk_buff;
struct ath5k_txq;
struct ath5k_tx_status;

#undef TRACE_SYSTEM
#define TRACE_SYSTEM ath5k

TRACE_EVENT(ath5k_rx,
	TP_PROTO(struct ath5k_hw *priv, struct sk_buff *skb),
	TP_ARGS(priv, skb),
	TP_STRUCT__entry(
		__field(struct ath5k_hw *, priv)
		__field(unsigned long, skbaddr)
		__dynamic_array(u8, frame, skb->len)
	),
	TP_fast_assign(
		__entry->priv = priv;
		__entry->skbaddr = (unsigned long) skb;
		memcpy(__get_dynamic_array(frame), skb->data, skb->len);
	),
	TP_printk(
		"[%p] RX skb=%lx", __entry->priv, __entry->skbaddr
	)
);

TRACE_EVENT(ath5k_tx,
	TP_PROTO(struct ath5k_hw *priv, struct sk_buff *skb,
		 struct ath5k_txq *q),

	TP_ARGS(priv, skb, q),

	TP_STRUCT__entry(
		__field(struct ath5k_hw *, priv)
		__field(unsigned long, skbaddr)
		__field(u8, qnum)
		__dynamic_array(u8, frame, skb->len)
	),

	TP_fast_assign(
		__entry->priv = priv;
		__entry->skbaddr = (unsigned long) skb;
		__entry->qnum = (u8) q->qnum;
		memcpy(__get_dynamic_array(frame), skb->data, skb->len);
	),

	TP_printk(
		"[%p] TX skb=%lx q=%d", __entry->priv, __entry->skbaddr,
		__entry->qnum
	)
);

TRACE_EVENT(ath5k_tx_complete,
	TP_PROTO(struct ath5k_hw *priv, struct sk_buff *skb,
		 struct ath5k_txq *q, struct ath5k_tx_status *ts),

	TP_ARGS(priv, skb, q, ts),

	TP_STRUCT__entry(
		__field(struct ath5k_hw *, priv)
		__field(unsigned long, skbaddr)
		__field(u8, qnum)
		__field(u8, ts_status)
		__field(s8, ts_rssi)
		__field(u8, ts_antenna)
	),

	TP_fast_assign(
		__entry->priv = priv;
		__entry->skbaddr = (unsigned long) skb;
		__entry->qnum = (u8) q->qnum;
		__entry->ts_status = ts->ts_status;
		__entry->ts_rssi =  ts->ts_rssi;
		__entry->ts_antenna = ts->ts_antenna;
	),

	TP_printk(
		"[%p] TX end skb=%lx q=%d stat=%x rssi=%d ant=%x",
		__entry->priv, __entry->skbaddr, __entry->qnum,
		__entry->ts_status, __entry->ts_rssi, __entry->ts_antenna
	)
);

#endif /* __TRACE_ATH5K_H */

#if defined(CONFIG_ATH5K_TRACER) && !defined(__CHECKER__)

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE trace

#include <trace/define_trace.h>

#endif
