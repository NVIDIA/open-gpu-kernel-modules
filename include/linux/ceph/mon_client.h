/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _FS_CEPH_MON_CLIENT_H
#define _FS_CEPH_MON_CLIENT_H

#include <linux/completion.h>
#include <linux/kref.h>
#include <linux/rbtree.h>

#include <linux/ceph/messenger.h>

struct ceph_client;
struct ceph_mount_args;
struct ceph_auth_client;

/*
 * The monitor map enumerates the set of all monitors.
 */
struct ceph_monmap {
	struct ceph_fsid fsid;
	u32 epoch;
	u32 num_mon;
	struct ceph_entity_inst mon_inst[];
};

struct ceph_mon_client;
struct ceph_mon_generic_request;


/*
 * Generic mechanism for resending monitor requests.
 */
typedef void (*ceph_monc_request_func_t)(struct ceph_mon_client *monc,
					 int newmon);

/* a pending monitor request */
struct ceph_mon_request {
	struct ceph_mon_client *monc;
	struct delayed_work delayed_work;
	unsigned long delay;
	ceph_monc_request_func_t do_request;
};

typedef void (*ceph_monc_callback_t)(struct ceph_mon_generic_request *);

/*
 * ceph_mon_generic_request is being used for the statfs and
 * mon_get_version requests which are being done a bit differently
 * because we need to get data back to the caller
 */
struct ceph_mon_generic_request {
	struct ceph_mon_client *monc;
	struct kref kref;
	u64 tid;
	struct rb_node node;
	int result;

	struct completion completion;
	ceph_monc_callback_t complete_cb;
	u64 private_data;          /* r_tid/linger_id */

	struct ceph_msg *request;  /* original request */
	struct ceph_msg *reply;    /* and reply */

	union {
		struct ceph_statfs *st;
		u64 newest;
	} u;
};

struct ceph_mon_client {
	struct ceph_client *client;
	struct ceph_monmap *monmap;

	struct mutex mutex;
	struct delayed_work delayed_work;

	struct ceph_auth_client *auth;
	struct ceph_msg *m_auth, *m_auth_reply, *m_subscribe, *m_subscribe_ack;
	int pending_auth;

	bool hunting;
	int cur_mon;                       /* last monitor i contacted */
	unsigned long sub_renew_after;
	unsigned long sub_renew_sent;
	struct ceph_connection con;

	bool had_a_connection;
	int hunt_mult; /* [1..CEPH_MONC_HUNT_MAX_MULT] */

	/* pending generic requests */
	struct rb_root generic_request_tree;
	u64 last_tid;

	/* subs, indexed with CEPH_SUB_* */
	struct {
		struct ceph_mon_subscribe_item item;
		bool want;
		u32 have; /* epoch */
	} subs[4];
	int fs_cluster_id; /* "mdsmap.<id>" sub */

#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_file;
#endif
};

extern int ceph_monmap_contains(struct ceph_monmap *m,
				struct ceph_entity_addr *addr);

extern int ceph_monc_init(struct ceph_mon_client *monc, struct ceph_client *cl);
extern void ceph_monc_stop(struct ceph_mon_client *monc);
extern void ceph_monc_reopen_session(struct ceph_mon_client *monc);

enum {
	CEPH_SUB_MONMAP = 0,
	CEPH_SUB_OSDMAP,
	CEPH_SUB_FSMAP,
	CEPH_SUB_MDSMAP,
};

extern const char *ceph_sub_str[];

/*
 * The model here is to indicate that we need a new map of at least
 * epoch @epoch, and also call in when we receive a map.  We will
 * periodically rerequest the map from the monitor cluster until we
 * get what we want.
 */
bool ceph_monc_want_map(struct ceph_mon_client *monc, int sub, u32 epoch,
			bool continuous);
void ceph_monc_got_map(struct ceph_mon_client *monc, int sub, u32 epoch);
void ceph_monc_renew_subs(struct ceph_mon_client *monc);

extern int ceph_monc_wait_osdmap(struct ceph_mon_client *monc, u32 epoch,
				 unsigned long timeout);

int ceph_monc_do_statfs(struct ceph_mon_client *monc, u64 data_pool,
			struct ceph_statfs *buf);

int ceph_monc_get_version(struct ceph_mon_client *monc, const char *what,
			  u64 *newest);
int ceph_monc_get_version_async(struct ceph_mon_client *monc, const char *what,
				ceph_monc_callback_t cb, u64 private_data);

int ceph_monc_blocklist_add(struct ceph_mon_client *monc,
			    struct ceph_entity_addr *client_addr);

extern int ceph_monc_open_session(struct ceph_mon_client *monc);

extern int ceph_monc_validate_auth(struct ceph_mon_client *monc);

#endif
