/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_PKT_SCHED_H
#define __NET_PKT_SCHED_H

#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/if_vlan.h>
#include <linux/netdevice.h>
#include <net/sch_generic.h>
#include <net/net_namespace.h>
#include <uapi/linux/pkt_sched.h>

#define DEFAULT_TX_QUEUE_LEN	1000

struct qdisc_walker {
	int	stop;
	int	skip;
	int	count;
	int	(*fn)(struct Qdisc *, unsigned long cl, struct qdisc_walker *);
};

static inline void *qdisc_priv(struct Qdisc *q)
{
	return &q->privdata;
}

static inline struct Qdisc *qdisc_from_priv(void *priv)
{
	return container_of(priv, struct Qdisc, privdata);
}

/* 
   Timer resolution MUST BE < 10% of min_schedulable_packet_size/bandwidth
   
   Normal IP packet size ~ 512byte, hence:

   0.5Kbyte/1Mbyte/sec = 0.5msec, so that we need 50usec timer for
   10Mbit ethernet.

   10msec resolution -> <50Kbit/sec.
   
   The result: [34]86 is not good choice for QoS router :-(

   The things are not so bad, because we may use artificial
   clock evaluated by integration of network data flow
   in the most critical places.
 */

typedef u64	psched_time_t;
typedef long	psched_tdiff_t;

/* Avoid doing 64 bit divide */
#define PSCHED_SHIFT			6
#define PSCHED_TICKS2NS(x)		((s64)(x) << PSCHED_SHIFT)
#define PSCHED_NS2TICKS(x)		((x) >> PSCHED_SHIFT)

#define PSCHED_TICKS_PER_SEC		PSCHED_NS2TICKS(NSEC_PER_SEC)
#define PSCHED_PASTPERFECT		0

static inline psched_time_t psched_get_time(void)
{
	return PSCHED_NS2TICKS(ktime_get_ns());
}

static inline psched_tdiff_t
psched_tdiff_bounded(psched_time_t tv1, psched_time_t tv2, psched_time_t bound)
{
	return min(tv1 - tv2, bound);
}

struct qdisc_watchdog {
	u64		last_expires;
	struct hrtimer	timer;
	struct Qdisc	*qdisc;
};

void qdisc_watchdog_init_clockid(struct qdisc_watchdog *wd, struct Qdisc *qdisc,
				 clockid_t clockid);
void qdisc_watchdog_init(struct qdisc_watchdog *wd, struct Qdisc *qdisc);

void qdisc_watchdog_schedule_range_ns(struct qdisc_watchdog *wd, u64 expires,
				      u64 delta_ns);

static inline void qdisc_watchdog_schedule_ns(struct qdisc_watchdog *wd,
					      u64 expires)
{
	return qdisc_watchdog_schedule_range_ns(wd, expires, 0ULL);
}

static inline void qdisc_watchdog_schedule(struct qdisc_watchdog *wd,
					   psched_time_t expires)
{
	qdisc_watchdog_schedule_ns(wd, PSCHED_TICKS2NS(expires));
}

void qdisc_watchdog_cancel(struct qdisc_watchdog *wd);

extern struct Qdisc_ops pfifo_qdisc_ops;
extern struct Qdisc_ops bfifo_qdisc_ops;
extern struct Qdisc_ops pfifo_head_drop_qdisc_ops;

int fifo_set_limit(struct Qdisc *q, unsigned int limit);
struct Qdisc *fifo_create_dflt(struct Qdisc *sch, struct Qdisc_ops *ops,
			       unsigned int limit,
			       struct netlink_ext_ack *extack);

int register_qdisc(struct Qdisc_ops *qops);
int unregister_qdisc(struct Qdisc_ops *qops);
void qdisc_get_default(char *id, size_t len);
int qdisc_set_default(const char *id);

void qdisc_hash_add(struct Qdisc *q, bool invisible);
void qdisc_hash_del(struct Qdisc *q);
struct Qdisc *qdisc_lookup(struct net_device *dev, u32 handle);
struct Qdisc *qdisc_lookup_rcu(struct net_device *dev, u32 handle);
struct qdisc_rate_table *qdisc_get_rtab(struct tc_ratespec *r,
					struct nlattr *tab,
					struct netlink_ext_ack *extack);
void qdisc_put_rtab(struct qdisc_rate_table *tab);
void qdisc_put_stab(struct qdisc_size_table *tab);
void qdisc_warn_nonwc(const char *txt, struct Qdisc *qdisc);
bool sch_direct_xmit(struct sk_buff *skb, struct Qdisc *q,
		     struct net_device *dev, struct netdev_queue *txq,
		     spinlock_t *root_lock, bool validate);

void __qdisc_run(struct Qdisc *q);

static inline void qdisc_run(struct Qdisc *q)
{
	if (qdisc_run_begin(q)) {
		__qdisc_run(q);
		qdisc_run_end(q);
	}
}

/* Calculate maximal size of packet seen by hard_start_xmit
   routine of this device.
 */
static inline unsigned int psched_mtu(const struct net_device *dev)
{
	return dev->mtu + dev->hard_header_len;
}

static inline struct net *qdisc_net(struct Qdisc *q)
{
	return dev_net(q->dev_queue->dev);
}

struct tc_cbs_qopt_offload {
	u8 enable;
	s32 queue;
	s32 hicredit;
	s32 locredit;
	s32 idleslope;
	s32 sendslope;
};

struct tc_etf_qopt_offload {
	u8 enable;
	s32 queue;
};

struct tc_taprio_sched_entry {
	u8 command; /* TC_TAPRIO_CMD_* */

	/* The gate_mask in the offloading side refers to traffic classes */
	u32 gate_mask;
	u32 interval;
};

struct tc_taprio_qopt_offload {
	u8 enable;
	ktime_t base_time;
	u64 cycle_time;
	u64 cycle_time_extension;

	size_t num_entries;
	struct tc_taprio_sched_entry entries[];
};

/* Reference counting */
struct tc_taprio_qopt_offload *taprio_offload_get(struct tc_taprio_qopt_offload
						  *offload);
void taprio_offload_free(struct tc_taprio_qopt_offload *offload);

/* Ensure skb_mstamp_ns, which might have been populated with the txtime, is
 * not mistaken for a software timestamp, because this will otherwise prevent
 * the dispatch of hardware timestamps to the socket.
 */
static inline void skb_txtime_consumed(struct sk_buff *skb)
{
	skb->tstamp = ktime_set(0, 0);
}

#endif
