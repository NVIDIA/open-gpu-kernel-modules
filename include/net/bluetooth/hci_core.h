/*
   BlueZ - Bluetooth protocol stack for Linux
   Copyright (c) 2000-2001, 2010, Code Aurora Forum. All rights reserved.

   Written 2000,2001 by Maxim Krasnyansky <maxk@qualcomm.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

#ifndef __HCI_CORE_H
#define __HCI_CORE_H

#include <linux/idr.h>
#include <linux/leds.h>
#include <linux/rculist.h>

#include <net/bluetooth/hci.h>
#include <net/bluetooth/hci_sock.h>

/* HCI priority */
#define HCI_PRIO_MAX	7

/* HCI Core structures */
struct inquiry_data {
	bdaddr_t	bdaddr;
	__u8		pscan_rep_mode;
	__u8		pscan_period_mode;
	__u8		pscan_mode;
	__u8		dev_class[3];
	__le16		clock_offset;
	__s8		rssi;
	__u8		ssp_mode;
};

struct inquiry_entry {
	struct list_head	all;		/* inq_cache.all */
	struct list_head	list;		/* unknown or resolve */
	enum {
		NAME_NOT_KNOWN,
		NAME_NEEDED,
		NAME_PENDING,
		NAME_KNOWN,
	} name_state;
	__u32			timestamp;
	struct inquiry_data	data;
};

struct discovery_state {
	int			type;
	enum {
		DISCOVERY_STOPPED,
		DISCOVERY_STARTING,
		DISCOVERY_FINDING,
		DISCOVERY_RESOLVING,
		DISCOVERY_STOPPING,
	} state;
	struct list_head	all;	/* All devices found during inquiry */
	struct list_head	unknown;	/* Name state not known */
	struct list_head	resolve;	/* Name needs to be resolved */
	__u32			timestamp;
	bdaddr_t		last_adv_addr;
	u8			last_adv_addr_type;
	s8			last_adv_rssi;
	u32			last_adv_flags;
	u8			last_adv_data[HCI_MAX_AD_LENGTH];
	u8			last_adv_data_len;
	bool			report_invalid_rssi;
	bool			result_filtering;
	bool			limited;
	s8			rssi;
	u16			uuid_count;
	u8			(*uuids)[16];
	unsigned long		scan_start;
	unsigned long		scan_duration;
};

#define SUSPEND_NOTIFIER_TIMEOUT	msecs_to_jiffies(2000) /* 2 seconds */

enum suspend_tasks {
	SUSPEND_PAUSE_DISCOVERY,
	SUSPEND_UNPAUSE_DISCOVERY,

	SUSPEND_PAUSE_ADVERTISING,
	SUSPEND_UNPAUSE_ADVERTISING,

	SUSPEND_SCAN_DISABLE,
	SUSPEND_SCAN_ENABLE,
	SUSPEND_DISCONNECTING,

	SUSPEND_POWERING_DOWN,

	SUSPEND_PREPARE_NOTIFIER,

	SUSPEND_SET_ADV_FILTER,
	__SUSPEND_NUM_TASKS
};

enum suspended_state {
	BT_RUNNING = 0,
	BT_SUSPEND_DISCONNECT,
	BT_SUSPEND_CONFIGURE_WAKE,
};

struct hci_conn_hash {
	struct list_head list;
	unsigned int     acl_num;
	unsigned int     amp_num;
	unsigned int     sco_num;
	unsigned int     le_num;
	unsigned int     le_num_slave;
};

struct bdaddr_list {
	struct list_head list;
	bdaddr_t bdaddr;
	u8 bdaddr_type;
};

struct bdaddr_list_with_irk {
	struct list_head list;
	bdaddr_t bdaddr;
	u8 bdaddr_type;
	u8 peer_irk[16];
	u8 local_irk[16];
};

struct bdaddr_list_with_flags {
	struct list_head list;
	bdaddr_t bdaddr;
	u8 bdaddr_type;
	u32 current_flags;
};

enum hci_conn_flags {
	HCI_CONN_FLAG_REMOTE_WAKEUP,
	HCI_CONN_FLAG_MAX
};

#define hci_conn_test_flag(nr, flags) ((flags) & (1U << nr))

/* Make sure number of flags doesn't exceed sizeof(current_flags) */
static_assert(HCI_CONN_FLAG_MAX < 32);

struct bt_uuid {
	struct list_head list;
	u8 uuid[16];
	u8 size;
	u8 svc_hint;
};

struct blocked_key {
	struct list_head list;
	struct rcu_head rcu;
	u8 type;
	u8 val[16];
};

struct smp_csrk {
	bdaddr_t bdaddr;
	u8 bdaddr_type;
	u8 type;
	u8 val[16];
};

struct smp_ltk {
	struct list_head list;
	struct rcu_head rcu;
	bdaddr_t bdaddr;
	u8 bdaddr_type;
	u8 authenticated;
	u8 type;
	u8 enc_size;
	__le16 ediv;
	__le64 rand;
	u8 val[16];
};

struct smp_irk {
	struct list_head list;
	struct rcu_head rcu;
	bdaddr_t rpa;
	bdaddr_t bdaddr;
	u8 addr_type;
	u8 val[16];
};

struct link_key {
	struct list_head list;
	struct rcu_head rcu;
	bdaddr_t bdaddr;
	u8 type;
	u8 val[HCI_LINK_KEY_SIZE];
	u8 pin_len;
};

struct oob_data {
	struct list_head list;
	bdaddr_t bdaddr;
	u8 bdaddr_type;
	u8 present;
	u8 hash192[16];
	u8 rand192[16];
	u8 hash256[16];
	u8 rand256[16];
};

struct adv_info {
	struct list_head list;
	bool pending;
	__u8	instance;
	__u32	flags;
	__u16	timeout;
	__u16	remaining_time;
	__u16	duration;
	__u16	adv_data_len;
	__u8	adv_data[HCI_MAX_AD_LENGTH];
	__u16	scan_rsp_len;
	__u8	scan_rsp_data[HCI_MAX_AD_LENGTH];
	__s8	tx_power;
	__u32   min_interval;
	__u32   max_interval;
	bdaddr_t	random_addr;
	bool 		rpa_expired;
	struct delayed_work	rpa_expired_cb;
};

#define HCI_MAX_ADV_INSTANCES		5
#define HCI_DEFAULT_ADV_DURATION	2

#define HCI_ADV_TX_POWER_NO_PREFERENCE 0x7F

struct adv_pattern {
	struct list_head list;
	__u8 ad_type;
	__u8 offset;
	__u8 length;
	__u8 value[HCI_MAX_AD_LENGTH];
};

struct adv_rssi_thresholds {
	__s8 low_threshold;
	__s8 high_threshold;
	__u16 low_threshold_timeout;
	__u16 high_threshold_timeout;
	__u8 sampling_period;
};

struct adv_monitor {
	struct list_head patterns;
	struct adv_rssi_thresholds rssi;
	__u16		handle;

	enum {
		ADV_MONITOR_STATE_NOT_REGISTERED,
		ADV_MONITOR_STATE_REGISTERED,
		ADV_MONITOR_STATE_OFFLOADED
	} state;
};

#define HCI_MIN_ADV_MONITOR_HANDLE		1
#define HCI_MAX_ADV_MONITOR_NUM_HANDLES		32
#define HCI_MAX_ADV_MONITOR_NUM_PATTERNS	16
#define HCI_ADV_MONITOR_EXT_NONE		1
#define HCI_ADV_MONITOR_EXT_MSFT		2

#define HCI_MAX_SHORT_NAME_LENGTH	10

/* Min encryption key size to match with SMP */
#define HCI_MIN_ENC_KEY_SIZE		7

/* Default LE RPA expiry time, 15 minutes */
#define HCI_DEFAULT_RPA_TIMEOUT		(15 * 60)

/* Default min/max age of connection information (1s/3s) */
#define DEFAULT_CONN_INFO_MIN_AGE	1000
#define DEFAULT_CONN_INFO_MAX_AGE	3000
/* Default authenticated payload timeout 30s */
#define DEFAULT_AUTH_PAYLOAD_TIMEOUT   0x0bb8

struct amp_assoc {
	__u16	len;
	__u16	offset;
	__u16	rem_len;
	__u16	len_so_far;
	__u8	data[HCI_MAX_AMP_ASSOC_SIZE];
};

#define HCI_MAX_PAGES	3

struct hci_dev {
	struct list_head list;
	struct mutex	lock;

	char		name[8];
	unsigned long	flags;
	__u16		id;
	__u8		bus;
	__u8		dev_type;
	bdaddr_t	bdaddr;
	bdaddr_t	setup_addr;
	bdaddr_t	public_addr;
	bdaddr_t	random_addr;
	bdaddr_t	static_addr;
	__u8		adv_addr_type;
	__u8		dev_name[HCI_MAX_NAME_LENGTH];
	__u8		short_name[HCI_MAX_SHORT_NAME_LENGTH];
	__u8		eir[HCI_MAX_EIR_LENGTH];
	__u16		appearance;
	__u8		dev_class[3];
	__u8		major_class;
	__u8		minor_class;
	__u8		max_page;
	__u8		features[HCI_MAX_PAGES][8];
	__u8		le_features[8];
	__u8		le_white_list_size;
	__u8		le_resolv_list_size;
	__u8		le_num_of_adv_sets;
	__u8		le_states[8];
	__u8		commands[64];
	__u8		hci_ver;
	__u16		hci_rev;
	__u8		lmp_ver;
	__u16		manufacturer;
	__u16		lmp_subver;
	__u16		voice_setting;
	__u8		num_iac;
	__u8		stored_max_keys;
	__u8		stored_num_keys;
	__u8		io_capability;
	__s8		inq_tx_power;
	__u8		err_data_reporting;
	__u16		page_scan_interval;
	__u16		page_scan_window;
	__u8		page_scan_type;
	__u8		le_adv_channel_map;
	__u16		le_adv_min_interval;
	__u16		le_adv_max_interval;
	__u8		le_scan_type;
	__u16		le_scan_interval;
	__u16		le_scan_window;
	__u16		le_scan_int_suspend;
	__u16		le_scan_window_suspend;
	__u16		le_scan_int_discovery;
	__u16		le_scan_window_discovery;
	__u16		le_scan_int_adv_monitor;
	__u16		le_scan_window_adv_monitor;
	__u16		le_scan_int_connect;
	__u16		le_scan_window_connect;
	__u16		le_conn_min_interval;
	__u16		le_conn_max_interval;
	__u16		le_conn_latency;
	__u16		le_supv_timeout;
	__u16		le_def_tx_len;
	__u16		le_def_tx_time;
	__u16		le_max_tx_len;
	__u16		le_max_tx_time;
	__u16		le_max_rx_len;
	__u16		le_max_rx_time;
	__u8		le_max_key_size;
	__u8		le_min_key_size;
	__u16		discov_interleaved_timeout;
	__u16		conn_info_min_age;
	__u16		conn_info_max_age;
	__u16		auth_payload_timeout;
	__u8		min_enc_key_size;
	__u8		max_enc_key_size;
	__u8		pairing_opts;
	__u8		ssp_debug_mode;
	__u8		hw_error_code;
	__u32		clock;
	__u16		advmon_allowlist_duration;
	__u16		advmon_no_filter_duration;
	__u8		enable_advmon_interleave_scan;

	__u16		devid_source;
	__u16		devid_vendor;
	__u16		devid_product;
	__u16		devid_version;

	__u8		def_page_scan_type;
	__u16		def_page_scan_int;
	__u16		def_page_scan_window;
	__u8		def_inq_scan_type;
	__u16		def_inq_scan_int;
	__u16		def_inq_scan_window;
	__u16		def_br_lsto;
	__u16		def_page_timeout;
	__u16		def_multi_adv_rotation_duration;
	__u16		def_le_autoconnect_timeout;
	__s8		min_le_tx_power;
	__s8		max_le_tx_power;

	__u16		pkt_type;
	__u16		esco_type;
	__u16		link_policy;
	__u16		link_mode;

	__u32		idle_timeout;
	__u16		sniff_min_interval;
	__u16		sniff_max_interval;

	__u8		amp_status;
	__u32		amp_total_bw;
	__u32		amp_max_bw;
	__u32		amp_min_latency;
	__u32		amp_max_pdu;
	__u8		amp_type;
	__u16		amp_pal_cap;
	__u16		amp_assoc_size;
	__u32		amp_max_flush_to;
	__u32		amp_be_flush_to;

	struct amp_assoc	loc_assoc;

	__u8		flow_ctl_mode;

	unsigned int	auto_accept_delay;

	unsigned long	quirks;

	atomic_t	cmd_cnt;
	unsigned int	acl_cnt;
	unsigned int	sco_cnt;
	unsigned int	le_cnt;

	unsigned int	acl_mtu;
	unsigned int	sco_mtu;
	unsigned int	le_mtu;
	unsigned int	acl_pkts;
	unsigned int	sco_pkts;
	unsigned int	le_pkts;

	__u16		block_len;
	__u16		block_mtu;
	__u16		num_blocks;
	__u16		block_cnt;

	unsigned long	acl_last_tx;
	unsigned long	sco_last_tx;
	unsigned long	le_last_tx;

	__u8		le_tx_def_phys;
	__u8		le_rx_def_phys;

	struct workqueue_struct	*workqueue;
	struct workqueue_struct	*req_workqueue;

	struct work_struct	power_on;
	struct delayed_work	power_off;
	struct work_struct	error_reset;

	__u16			discov_timeout;
	struct delayed_work	discov_off;

	struct delayed_work	service_cache;

	struct delayed_work	cmd_timer;

	struct work_struct	rx_work;
	struct work_struct	cmd_work;
	struct work_struct	tx_work;

	struct work_struct	discov_update;
	struct work_struct	bg_scan_update;
	struct work_struct	scan_update;
	struct work_struct	connectable_update;
	struct work_struct	discoverable_update;
	struct delayed_work	le_scan_disable;
	struct delayed_work	le_scan_restart;

	struct sk_buff_head	rx_q;
	struct sk_buff_head	raw_q;
	struct sk_buff_head	cmd_q;

	struct sk_buff		*sent_cmd;

	struct mutex		req_lock;
	wait_queue_head_t	req_wait_q;
	__u32			req_status;
	__u32			req_result;
	struct sk_buff		*req_skb;

	void			*smp_data;
	void			*smp_bredr_data;

	struct discovery_state	discovery;

	int			discovery_old_state;
	bool			discovery_paused;
	int			advertising_old_state;
	bool			advertising_paused;

	struct notifier_block	suspend_notifier;
	struct work_struct	suspend_prepare;
	enum suspended_state	suspend_state_next;
	enum suspended_state	suspend_state;
	bool			scanning_paused;
	bool			suspended;
	u8			wake_reason;
	bdaddr_t		wake_addr;
	u8			wake_addr_type;

	wait_queue_head_t	suspend_wait_q;
	DECLARE_BITMAP(suspend_tasks, __SUSPEND_NUM_TASKS);

	struct hci_conn_hash	conn_hash;

	struct list_head	mgmt_pending;
	struct list_head	blacklist;
	struct list_head	whitelist;
	struct list_head	uuids;
	struct list_head	link_keys;
	struct list_head	long_term_keys;
	struct list_head	identity_resolving_keys;
	struct list_head	remote_oob_data;
	struct list_head	le_white_list;
	struct list_head	le_resolv_list;
	struct list_head	le_conn_params;
	struct list_head	pend_le_conns;
	struct list_head	pend_le_reports;
	struct list_head	blocked_keys;

	struct hci_dev_stats	stat;

	atomic_t		promisc;

	const char		*hw_info;
	const char		*fw_info;
	struct dentry		*debugfs;

	struct device		dev;

	struct rfkill		*rfkill;

	DECLARE_BITMAP(dev_flags, __HCI_NUM_FLAGS);

	__s8			adv_tx_power;
	__u8			adv_data[HCI_MAX_AD_LENGTH];
	__u8			adv_data_len;
	__u8			scan_rsp_data[HCI_MAX_AD_LENGTH];
	__u8			scan_rsp_data_len;

	struct list_head	adv_instances;
	unsigned int		adv_instance_cnt;
	__u8			cur_adv_instance;
	__u16			adv_instance_timeout;
	struct delayed_work	adv_instance_expire;

	struct idr		adv_monitors_idr;
	unsigned int		adv_monitors_cnt;

	__u8			irk[16];
	__u32			rpa_timeout;
	struct delayed_work	rpa_expired;
	bdaddr_t		rpa;

	enum {
		INTERLEAVE_SCAN_NONE,
		INTERLEAVE_SCAN_NO_FILTER,
		INTERLEAVE_SCAN_ALLOWLIST
	} interleave_scan_state;

	struct delayed_work	interleave_scan;

#if IS_ENABLED(CONFIG_BT_LEDS)
	struct led_trigger	*power_led;
#endif

#if IS_ENABLED(CONFIG_BT_MSFTEXT)
	__u16			msft_opcode;
	void			*msft_data;
	bool			msft_curve_validity;
#endif

#if IS_ENABLED(CONFIG_BT_AOSPEXT)
	bool			aosp_capable;
#endif

	int (*open)(struct hci_dev *hdev);
	int (*close)(struct hci_dev *hdev);
	int (*flush)(struct hci_dev *hdev);
	int (*setup)(struct hci_dev *hdev);
	int (*shutdown)(struct hci_dev *hdev);
	int (*send)(struct hci_dev *hdev, struct sk_buff *skb);
	void (*notify)(struct hci_dev *hdev, unsigned int evt);
	void (*hw_error)(struct hci_dev *hdev, u8 code);
	int (*post_init)(struct hci_dev *hdev);
	int (*set_diag)(struct hci_dev *hdev, bool enable);
	int (*set_bdaddr)(struct hci_dev *hdev, const bdaddr_t *bdaddr);
	void (*cmd_timeout)(struct hci_dev *hdev);
	bool (*prevent_wake)(struct hci_dev *hdev);
};

#define HCI_PHY_HANDLE(handle)	(handle & 0xff)

enum conn_reasons {
	CONN_REASON_PAIR_DEVICE,
	CONN_REASON_L2CAP_CHAN,
	CONN_REASON_SCO_CONNECT,
};

struct hci_conn {
	struct list_head list;

	atomic_t	refcnt;

	bdaddr_t	dst;
	__u8		dst_type;
	bdaddr_t	src;
	__u8		src_type;
	bdaddr_t	init_addr;
	__u8		init_addr_type;
	bdaddr_t	resp_addr;
	__u8		resp_addr_type;
	__u16		handle;
	__u16		state;
	__u8		mode;
	__u8		type;
	__u8		role;
	bool		out;
	__u8		attempt;
	__u8		dev_class[3];
	__u8		features[HCI_MAX_PAGES][8];
	__u16		pkt_type;
	__u16		link_policy;
	__u8		key_type;
	__u8		auth_type;
	__u8		sec_level;
	__u8		pending_sec_level;
	__u8		pin_length;
	__u8		enc_key_size;
	__u8		io_capability;
	__u32		passkey_notify;
	__u8		passkey_entered;
	__u16		disc_timeout;
	__u16		conn_timeout;
	__u16		setting;
	__u16		auth_payload_timeout;
	__u16		le_conn_min_interval;
	__u16		le_conn_max_interval;
	__u16		le_conn_interval;
	__u16		le_conn_latency;
	__u16		le_supv_timeout;
	__u8		le_adv_data[HCI_MAX_AD_LENGTH];
	__u8		le_adv_data_len;
	__u8		le_tx_phy;
	__u8		le_rx_phy;
	__s8		rssi;
	__s8		tx_power;
	__s8		max_tx_power;
	unsigned long	flags;

	enum conn_reasons conn_reason;

	__u32		clock;
	__u16		clock_accuracy;

	unsigned long	conn_info_timestamp;

	__u8		remote_cap;
	__u8		remote_auth;
	__u8		remote_id;

	unsigned int	sent;

	struct sk_buff_head data_q;
	struct list_head chan_list;

	struct delayed_work disc_work;
	struct delayed_work auto_accept_work;
	struct delayed_work idle_work;
	struct delayed_work le_conn_timeout;
	struct work_struct  le_scan_cleanup;

	struct device	dev;
	struct dentry	*debugfs;

	struct hci_dev	*hdev;
	void		*l2cap_data;
	void		*sco_data;
	struct amp_mgr	*amp_mgr;

	struct hci_conn	*link;

	void (*connect_cfm_cb)	(struct hci_conn *conn, u8 status);
	void (*security_cfm_cb)	(struct hci_conn *conn, u8 status);
	void (*disconn_cfm_cb)	(struct hci_conn *conn, u8 reason);
};

struct hci_chan {
	struct list_head list;
	__u16 handle;
	struct hci_conn *conn;
	struct sk_buff_head data_q;
	unsigned int	sent;
	__u8		state;
	bool		amp;
};

struct hci_conn_params {
	struct list_head list;
	struct list_head action;

	bdaddr_t addr;
	u8 addr_type;

	u16 conn_min_interval;
	u16 conn_max_interval;
	u16 conn_latency;
	u16 supervision_timeout;

	enum {
		HCI_AUTO_CONN_DISABLED,
		HCI_AUTO_CONN_REPORT,
		HCI_AUTO_CONN_DIRECT,
		HCI_AUTO_CONN_ALWAYS,
		HCI_AUTO_CONN_LINK_LOSS,
		HCI_AUTO_CONN_EXPLICIT,
	} auto_connect;

	struct hci_conn *conn;
	bool explicit_connect;
	u32 current_flags;
};

extern struct list_head hci_dev_list;
extern struct list_head hci_cb_list;
extern rwlock_t hci_dev_list_lock;
extern struct mutex hci_cb_list_lock;

#define hci_dev_set_flag(hdev, nr)             set_bit((nr), (hdev)->dev_flags)
#define hci_dev_clear_flag(hdev, nr)           clear_bit((nr), (hdev)->dev_flags)
#define hci_dev_change_flag(hdev, nr)          change_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_flag(hdev, nr)            test_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_and_set_flag(hdev, nr)    test_and_set_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_and_clear_flag(hdev, nr)  test_and_clear_bit((nr), (hdev)->dev_flags)
#define hci_dev_test_and_change_flag(hdev, nr) test_and_change_bit((nr), (hdev)->dev_flags)

#define hci_dev_clear_volatile_flags(hdev)			\
	do {							\
		hci_dev_clear_flag(hdev, HCI_LE_SCAN);		\
		hci_dev_clear_flag(hdev, HCI_LE_ADV);		\
		hci_dev_clear_flag(hdev, HCI_LL_RPA_RESOLUTION);\
		hci_dev_clear_flag(hdev, HCI_PERIODIC_INQ);	\
	} while (0)

/* ----- HCI interface to upper protocols ----- */
int l2cap_connect_ind(struct hci_dev *hdev, bdaddr_t *bdaddr);
int l2cap_disconn_ind(struct hci_conn *hcon);
void l2cap_recv_acldata(struct hci_conn *hcon, struct sk_buff *skb, u16 flags);

#if IS_ENABLED(CONFIG_BT_BREDR)
int sco_connect_ind(struct hci_dev *hdev, bdaddr_t *bdaddr, __u8 *flags);
void sco_recv_scodata(struct hci_conn *hcon, struct sk_buff *skb);
#else
static inline int sco_connect_ind(struct hci_dev *hdev, bdaddr_t *bdaddr,
				  __u8 *flags)
{
	return 0;
}

static inline void sco_recv_scodata(struct hci_conn *hcon, struct sk_buff *skb)
{
}
#endif

/* ----- Inquiry cache ----- */
#define INQUIRY_CACHE_AGE_MAX   (HZ*30)   /* 30 seconds */
#define INQUIRY_ENTRY_AGE_MAX   (HZ*60)   /* 60 seconds */

static inline void discovery_init(struct hci_dev *hdev)
{
	hdev->discovery.state = DISCOVERY_STOPPED;
	INIT_LIST_HEAD(&hdev->discovery.all);
	INIT_LIST_HEAD(&hdev->discovery.unknown);
	INIT_LIST_HEAD(&hdev->discovery.resolve);
	hdev->discovery.report_invalid_rssi = true;
	hdev->discovery.rssi = HCI_RSSI_INVALID;
}

static inline void hci_discovery_filter_clear(struct hci_dev *hdev)
{
	hdev->discovery.result_filtering = false;
	hdev->discovery.report_invalid_rssi = true;
	hdev->discovery.rssi = HCI_RSSI_INVALID;
	hdev->discovery.uuid_count = 0;
	kfree(hdev->discovery.uuids);
	hdev->discovery.uuids = NULL;
	hdev->discovery.scan_start = 0;
	hdev->discovery.scan_duration = 0;
}

bool hci_discovery_active(struct hci_dev *hdev);

void hci_discovery_set_state(struct hci_dev *hdev, int state);

static inline int inquiry_cache_empty(struct hci_dev *hdev)
{
	return list_empty(&hdev->discovery.all);
}

static inline long inquiry_cache_age(struct hci_dev *hdev)
{
	struct discovery_state *c = &hdev->discovery;
	return jiffies - c->timestamp;
}

static inline long inquiry_entry_age(struct inquiry_entry *e)
{
	return jiffies - e->timestamp;
}

struct inquiry_entry *hci_inquiry_cache_lookup(struct hci_dev *hdev,
					       bdaddr_t *bdaddr);
struct inquiry_entry *hci_inquiry_cache_lookup_unknown(struct hci_dev *hdev,
						       bdaddr_t *bdaddr);
struct inquiry_entry *hci_inquiry_cache_lookup_resolve(struct hci_dev *hdev,
						       bdaddr_t *bdaddr,
						       int state);
void hci_inquiry_cache_update_resolve(struct hci_dev *hdev,
				      struct inquiry_entry *ie);
u32 hci_inquiry_cache_update(struct hci_dev *hdev, struct inquiry_data *data,
			     bool name_known);
void hci_inquiry_cache_flush(struct hci_dev *hdev);

/* ----- HCI Connections ----- */
enum {
	HCI_CONN_AUTH_PEND,
	HCI_CONN_REAUTH_PEND,
	HCI_CONN_ENCRYPT_PEND,
	HCI_CONN_RSWITCH_PEND,
	HCI_CONN_MODE_CHANGE_PEND,
	HCI_CONN_SCO_SETUP_PEND,
	HCI_CONN_MGMT_CONNECTED,
	HCI_CONN_SSP_ENABLED,
	HCI_CONN_SC_ENABLED,
	HCI_CONN_AES_CCM,
	HCI_CONN_POWER_SAVE,
	HCI_CONN_FLUSH_KEY,
	HCI_CONN_ENCRYPT,
	HCI_CONN_AUTH,
	HCI_CONN_SECURE,
	HCI_CONN_FIPS,
	HCI_CONN_STK_ENCRYPT,
	HCI_CONN_AUTH_INITIATOR,
	HCI_CONN_DROP,
	HCI_CONN_PARAM_REMOVAL_PEND,
	HCI_CONN_NEW_LINK_KEY,
	HCI_CONN_SCANNING,
	HCI_CONN_AUTH_FAILURE,
};

static inline bool hci_conn_ssp_enabled(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;
	return hci_dev_test_flag(hdev, HCI_SSP_ENABLED) &&
	       test_bit(HCI_CONN_SSP_ENABLED, &conn->flags);
}

static inline bool hci_conn_sc_enabled(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;
	return hci_dev_test_flag(hdev, HCI_SC_ENABLED) &&
	       test_bit(HCI_CONN_SC_ENABLED, &conn->flags);
}

static inline void hci_conn_hash_add(struct hci_dev *hdev, struct hci_conn *c)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	list_add_rcu(&c->list, &h->list);
	switch (c->type) {
	case ACL_LINK:
		h->acl_num++;
		break;
	case AMP_LINK:
		h->amp_num++;
		break;
	case LE_LINK:
		h->le_num++;
		if (c->role == HCI_ROLE_SLAVE)
			h->le_num_slave++;
		break;
	case SCO_LINK:
	case ESCO_LINK:
		h->sco_num++;
		break;
	}
}

static inline void hci_conn_hash_del(struct hci_dev *hdev, struct hci_conn *c)
{
	struct hci_conn_hash *h = &hdev->conn_hash;

	list_del_rcu(&c->list);
	synchronize_rcu();

	switch (c->type) {
	case ACL_LINK:
		h->acl_num--;
		break;
	case AMP_LINK:
		h->amp_num--;
		break;
	case LE_LINK:
		h->le_num--;
		if (c->role == HCI_ROLE_SLAVE)
			h->le_num_slave--;
		break;
	case SCO_LINK:
	case ESCO_LINK:
		h->sco_num--;
		break;
	}
}

static inline unsigned int hci_conn_num(struct hci_dev *hdev, __u8 type)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	switch (type) {
	case ACL_LINK:
		return h->acl_num;
	case AMP_LINK:
		return h->amp_num;
	case LE_LINK:
		return h->le_num;
	case SCO_LINK:
	case ESCO_LINK:
		return h->sco_num;
	default:
		return 0;
	}
}

static inline unsigned int hci_conn_count(struct hci_dev *hdev)
{
	struct hci_conn_hash *c = &hdev->conn_hash;

	return c->acl_num + c->amp_num + c->sco_num + c->le_num;
}

static inline __u8 hci_conn_lookup_type(struct hci_dev *hdev, __u16 handle)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn *c;
	__u8 type = INVALID_LINK;

	rcu_read_lock();

	list_for_each_entry_rcu(c, &h->list, list) {
		if (c->handle == handle) {
			type = c->type;
			break;
		}
	}

	rcu_read_unlock();

	return type;
}

static inline struct hci_conn *hci_conn_hash_lookup_handle(struct hci_dev *hdev,
								__u16 handle)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn  *c;

	rcu_read_lock();

	list_for_each_entry_rcu(c, &h->list, list) {
		if (c->handle == handle) {
			rcu_read_unlock();
			return c;
		}
	}
	rcu_read_unlock();

	return NULL;
}

static inline struct hci_conn *hci_conn_hash_lookup_ba(struct hci_dev *hdev,
							__u8 type, bdaddr_t *ba)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn  *c;

	rcu_read_lock();

	list_for_each_entry_rcu(c, &h->list, list) {
		if (c->type == type && !bacmp(&c->dst, ba)) {
			rcu_read_unlock();
			return c;
		}
	}

	rcu_read_unlock();

	return NULL;
}

static inline struct hci_conn *hci_conn_hash_lookup_le(struct hci_dev *hdev,
						       bdaddr_t *ba,
						       __u8 ba_type)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn  *c;

	rcu_read_lock();

	list_for_each_entry_rcu(c, &h->list, list) {
		if (c->type != LE_LINK)
		       continue;

		if (ba_type == c->dst_type && !bacmp(&c->dst, ba)) {
			rcu_read_unlock();
			return c;
		}
	}

	rcu_read_unlock();

	return NULL;
}

static inline struct hci_conn *hci_conn_hash_lookup_state(struct hci_dev *hdev,
							__u8 type, __u16 state)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn  *c;

	rcu_read_lock();

	list_for_each_entry_rcu(c, &h->list, list) {
		if (c->type == type && c->state == state) {
			rcu_read_unlock();
			return c;
		}
	}

	rcu_read_unlock();

	return NULL;
}

static inline struct hci_conn *hci_lookup_le_connect(struct hci_dev *hdev)
{
	struct hci_conn_hash *h = &hdev->conn_hash;
	struct hci_conn  *c;

	rcu_read_lock();

	list_for_each_entry_rcu(c, &h->list, list) {
		if (c->type == LE_LINK && c->state == BT_CONNECT &&
		    !test_bit(HCI_CONN_SCANNING, &c->flags)) {
			rcu_read_unlock();
			return c;
		}
	}

	rcu_read_unlock();

	return NULL;
}

int hci_disconnect(struct hci_conn *conn, __u8 reason);
bool hci_setup_sync(struct hci_conn *conn, __u16 handle);
void hci_sco_setup(struct hci_conn *conn, __u8 status);

struct hci_conn *hci_conn_add(struct hci_dev *hdev, int type, bdaddr_t *dst,
			      u8 role);
int hci_conn_del(struct hci_conn *conn);
void hci_conn_hash_flush(struct hci_dev *hdev);
void hci_conn_check_pending(struct hci_dev *hdev);

struct hci_chan *hci_chan_create(struct hci_conn *conn);
void hci_chan_del(struct hci_chan *chan);
void hci_chan_list_flush(struct hci_conn *conn);
struct hci_chan *hci_chan_lookup_handle(struct hci_dev *hdev, __u16 handle);

struct hci_conn *hci_connect_le_scan(struct hci_dev *hdev, bdaddr_t *dst,
				     u8 dst_type, u8 sec_level,
				     u16 conn_timeout,
				     enum conn_reasons conn_reason);
struct hci_conn *hci_connect_le(struct hci_dev *hdev, bdaddr_t *dst,
				u8 dst_type, u8 sec_level, u16 conn_timeout,
				u8 role, bdaddr_t *direct_rpa);
struct hci_conn *hci_connect_acl(struct hci_dev *hdev, bdaddr_t *dst,
				 u8 sec_level, u8 auth_type,
				 enum conn_reasons conn_reason);
struct hci_conn *hci_connect_sco(struct hci_dev *hdev, int type, bdaddr_t *dst,
				 __u16 setting);
int hci_conn_check_link_mode(struct hci_conn *conn);
int hci_conn_check_secure(struct hci_conn *conn, __u8 sec_level);
int hci_conn_security(struct hci_conn *conn, __u8 sec_level, __u8 auth_type,
		      bool initiator);
int hci_conn_switch_role(struct hci_conn *conn, __u8 role);

void hci_conn_enter_active_mode(struct hci_conn *conn, __u8 force_active);

void hci_le_conn_failed(struct hci_conn *conn, u8 status);

/*
 * hci_conn_get() and hci_conn_put() are used to control the life-time of an
 * "hci_conn" object. They do not guarantee that the hci_conn object is running,
 * working or anything else. They just guarantee that the object is available
 * and can be dereferenced. So you can use its locks, local variables and any
 * other constant data.
 * Before accessing runtime data, you _must_ lock the object and then check that
 * it is still running. As soon as you release the locks, the connection might
 * get dropped, though.
 *
 * On the other hand, hci_conn_hold() and hci_conn_drop() are used to control
 * how long the underlying connection is held. So every channel that runs on the
 * hci_conn object calls this to prevent the connection from disappearing. As
 * long as you hold a device, you must also guarantee that you have a valid
 * reference to the device via hci_conn_get() (or the initial reference from
 * hci_conn_add()).
 * The hold()/drop() ref-count is known to drop below 0 sometimes, which doesn't
 * break because nobody cares for that. But this means, we cannot use
 * _get()/_drop() in it, but require the caller to have a valid ref (FIXME).
 */

static inline struct hci_conn *hci_conn_get(struct hci_conn *conn)
{
	get_device(&conn->dev);
	return conn;
}

static inline void hci_conn_put(struct hci_conn *conn)
{
	put_device(&conn->dev);
}

static inline void hci_conn_hold(struct hci_conn *conn)
{
	BT_DBG("hcon %p orig refcnt %d", conn, atomic_read(&conn->refcnt));

	atomic_inc(&conn->refcnt);
	cancel_delayed_work(&conn->disc_work);
}

static inline void hci_conn_drop(struct hci_conn *conn)
{
	BT_DBG("hcon %p orig refcnt %d", conn, atomic_read(&conn->refcnt));

	if (atomic_dec_and_test(&conn->refcnt)) {
		unsigned long timeo;

		switch (conn->type) {
		case ACL_LINK:
		case LE_LINK:
			cancel_delayed_work(&conn->idle_work);
			if (conn->state == BT_CONNECTED) {
				timeo = conn->disc_timeout;
				if (!conn->out)
					timeo *= 2;
			} else {
				timeo = 0;
			}
			break;

		case AMP_LINK:
			timeo = conn->disc_timeout;
			break;

		default:
			timeo = 0;
			break;
		}

		cancel_delayed_work(&conn->disc_work);
		queue_delayed_work(conn->hdev->workqueue,
				   &conn->disc_work, timeo);
	}
}

/* ----- HCI Devices ----- */
static inline void hci_dev_put(struct hci_dev *d)
{
	BT_DBG("%s orig refcnt %d", d->name,
	       kref_read(&d->dev.kobj.kref));

	put_device(&d->dev);
}

static inline struct hci_dev *hci_dev_hold(struct hci_dev *d)
{
	BT_DBG("%s orig refcnt %d", d->name,
	       kref_read(&d->dev.kobj.kref));

	get_device(&d->dev);
	return d;
}

#define hci_dev_lock(d)		mutex_lock(&d->lock)
#define hci_dev_unlock(d)	mutex_unlock(&d->lock)

#define to_hci_dev(d) container_of(d, struct hci_dev, dev)
#define to_hci_conn(c) container_of(c, struct hci_conn, dev)

static inline void *hci_get_drvdata(struct hci_dev *hdev)
{
	return dev_get_drvdata(&hdev->dev);
}

static inline void hci_set_drvdata(struct hci_dev *hdev, void *data)
{
	dev_set_drvdata(&hdev->dev, data);
}

struct hci_dev *hci_dev_get(int index);
struct hci_dev *hci_get_route(bdaddr_t *dst, bdaddr_t *src, u8 src_type);

struct hci_dev *hci_alloc_dev(void);
void hci_free_dev(struct hci_dev *hdev);
int hci_register_dev(struct hci_dev *hdev);
void hci_unregister_dev(struct hci_dev *hdev);
int hci_suspend_dev(struct hci_dev *hdev);
int hci_resume_dev(struct hci_dev *hdev);
int hci_reset_dev(struct hci_dev *hdev);
int hci_recv_frame(struct hci_dev *hdev, struct sk_buff *skb);
int hci_recv_diag(struct hci_dev *hdev, struct sk_buff *skb);
__printf(2, 3) void hci_set_hw_info(struct hci_dev *hdev, const char *fmt, ...);
__printf(2, 3) void hci_set_fw_info(struct hci_dev *hdev, const char *fmt, ...);

static inline void hci_set_msft_opcode(struct hci_dev *hdev, __u16 opcode)
{
#if IS_ENABLED(CONFIG_BT_MSFTEXT)
	hdev->msft_opcode = opcode;
#endif
}

static inline void hci_set_aosp_capable(struct hci_dev *hdev)
{
#if IS_ENABLED(CONFIG_BT_AOSPEXT)
	hdev->aosp_capable = true;
#endif
}

int hci_dev_open(__u16 dev);
int hci_dev_close(__u16 dev);
int hci_dev_do_close(struct hci_dev *hdev);
int hci_dev_reset(__u16 dev);
int hci_dev_reset_stat(__u16 dev);
int hci_dev_cmd(unsigned int cmd, void __user *arg);
int hci_get_dev_list(void __user *arg);
int hci_get_dev_info(void __user *arg);
int hci_get_conn_list(void __user *arg);
int hci_get_conn_info(struct hci_dev *hdev, void __user *arg);
int hci_get_auth_info(struct hci_dev *hdev, void __user *arg);
int hci_inquiry(void __user *arg);

struct bdaddr_list *hci_bdaddr_list_lookup(struct list_head *list,
					   bdaddr_t *bdaddr, u8 type);
struct bdaddr_list_with_irk *hci_bdaddr_list_lookup_with_irk(
				    struct list_head *list, bdaddr_t *bdaddr,
				    u8 type);
struct bdaddr_list_with_flags *
hci_bdaddr_list_lookup_with_flags(struct list_head *list, bdaddr_t *bdaddr,
				  u8 type);
int hci_bdaddr_list_add(struct list_head *list, bdaddr_t *bdaddr, u8 type);
int hci_bdaddr_list_add_with_irk(struct list_head *list, bdaddr_t *bdaddr,
				 u8 type, u8 *peer_irk, u8 *local_irk);
int hci_bdaddr_list_add_with_flags(struct list_head *list, bdaddr_t *bdaddr,
				   u8 type, u32 flags);
int hci_bdaddr_list_del(struct list_head *list, bdaddr_t *bdaddr, u8 type);
int hci_bdaddr_list_del_with_irk(struct list_head *list, bdaddr_t *bdaddr,
				 u8 type);
int hci_bdaddr_list_del_with_flags(struct list_head *list, bdaddr_t *bdaddr,
				   u8 type);
void hci_bdaddr_list_clear(struct list_head *list);

struct hci_conn_params *hci_conn_params_lookup(struct hci_dev *hdev,
					       bdaddr_t *addr, u8 addr_type);
struct hci_conn_params *hci_conn_params_add(struct hci_dev *hdev,
					    bdaddr_t *addr, u8 addr_type);
void hci_conn_params_del(struct hci_dev *hdev, bdaddr_t *addr, u8 addr_type);
void hci_conn_params_clear_disabled(struct hci_dev *hdev);

struct hci_conn_params *hci_pend_le_action_lookup(struct list_head *list,
						  bdaddr_t *addr,
						  u8 addr_type);

void hci_uuids_clear(struct hci_dev *hdev);

void hci_link_keys_clear(struct hci_dev *hdev);
struct link_key *hci_find_link_key(struct hci_dev *hdev, bdaddr_t *bdaddr);
struct link_key *hci_add_link_key(struct hci_dev *hdev, struct hci_conn *conn,
				  bdaddr_t *bdaddr, u8 *val, u8 type,
				  u8 pin_len, bool *persistent);
struct smp_ltk *hci_add_ltk(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 addr_type, u8 type, u8 authenticated,
			    u8 tk[16], u8 enc_size, __le16 ediv, __le64 rand);
struct smp_ltk *hci_find_ltk(struct hci_dev *hdev, bdaddr_t *bdaddr,
			     u8 addr_type, u8 role);
int hci_remove_ltk(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 bdaddr_type);
void hci_smp_ltks_clear(struct hci_dev *hdev);
int hci_remove_link_key(struct hci_dev *hdev, bdaddr_t *bdaddr);

struct smp_irk *hci_find_irk_by_rpa(struct hci_dev *hdev, bdaddr_t *rpa);
struct smp_irk *hci_find_irk_by_addr(struct hci_dev *hdev, bdaddr_t *bdaddr,
				     u8 addr_type);
struct smp_irk *hci_add_irk(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 addr_type, u8 val[16], bdaddr_t *rpa);
void hci_remove_irk(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 addr_type);
bool hci_is_blocked_key(struct hci_dev *hdev, u8 type, u8 val[16]);
void hci_blocked_keys_clear(struct hci_dev *hdev);
void hci_smp_irks_clear(struct hci_dev *hdev);

bool hci_bdaddr_is_paired(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 type);

void hci_remote_oob_data_clear(struct hci_dev *hdev);
struct oob_data *hci_find_remote_oob_data(struct hci_dev *hdev,
					  bdaddr_t *bdaddr, u8 bdaddr_type);
int hci_add_remote_oob_data(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 bdaddr_type, u8 *hash192, u8 *rand192,
			    u8 *hash256, u8 *rand256);
int hci_remove_remote_oob_data(struct hci_dev *hdev, bdaddr_t *bdaddr,
			       u8 bdaddr_type);

void hci_adv_instances_clear(struct hci_dev *hdev);
struct adv_info *hci_find_adv_instance(struct hci_dev *hdev, u8 instance);
struct adv_info *hci_get_next_instance(struct hci_dev *hdev, u8 instance);
int hci_add_adv_instance(struct hci_dev *hdev, u8 instance, u32 flags,
			 u16 adv_data_len, u8 *adv_data,
			 u16 scan_rsp_len, u8 *scan_rsp_data,
			 u16 timeout, u16 duration, s8 tx_power,
			 u32 min_interval, u32 max_interval);
int hci_set_adv_instance_data(struct hci_dev *hdev, u8 instance,
			 u16 adv_data_len, u8 *adv_data,
			 u16 scan_rsp_len, u8 *scan_rsp_data);
int hci_remove_adv_instance(struct hci_dev *hdev, u8 instance);
void hci_adv_instances_set_rpa_expired(struct hci_dev *hdev, bool rpa_expired);

void hci_adv_monitors_clear(struct hci_dev *hdev);
void hci_free_adv_monitor(struct hci_dev *hdev, struct adv_monitor *monitor);
int hci_add_adv_patterns_monitor_complete(struct hci_dev *hdev, u8 status);
int hci_remove_adv_monitor_complete(struct hci_dev *hdev, u8 status);
bool hci_add_adv_monitor(struct hci_dev *hdev, struct adv_monitor *monitor,
			int *err);
bool hci_remove_single_adv_monitor(struct hci_dev *hdev, u16 handle, int *err);
bool hci_remove_all_adv_monitor(struct hci_dev *hdev, int *err);
bool hci_is_adv_monitoring(struct hci_dev *hdev);
int hci_get_adv_monitor_offload_ext(struct hci_dev *hdev);

void hci_event_packet(struct hci_dev *hdev, struct sk_buff *skb);

void hci_init_sysfs(struct hci_dev *hdev);
void hci_conn_init_sysfs(struct hci_conn *conn);
void hci_conn_add_sysfs(struct hci_conn *conn);
void hci_conn_del_sysfs(struct hci_conn *conn);

#define SET_HCIDEV_DEV(hdev, pdev) ((hdev)->dev.parent = (pdev))

/* ----- LMP capabilities ----- */
#define lmp_encrypt_capable(dev)   ((dev)->features[0][0] & LMP_ENCRYPT)
#define lmp_rswitch_capable(dev)   ((dev)->features[0][0] & LMP_RSWITCH)
#define lmp_hold_capable(dev)      ((dev)->features[0][0] & LMP_HOLD)
#define lmp_sniff_capable(dev)     ((dev)->features[0][0] & LMP_SNIFF)
#define lmp_park_capable(dev)      ((dev)->features[0][1] & LMP_PARK)
#define lmp_inq_rssi_capable(dev)  ((dev)->features[0][3] & LMP_RSSI_INQ)
#define lmp_esco_capable(dev)      ((dev)->features[0][3] & LMP_ESCO)
#define lmp_bredr_capable(dev)     (!((dev)->features[0][4] & LMP_NO_BREDR))
#define lmp_le_capable(dev)        ((dev)->features[0][4] & LMP_LE)
#define lmp_sniffsubr_capable(dev) ((dev)->features[0][5] & LMP_SNIFF_SUBR)
#define lmp_pause_enc_capable(dev) ((dev)->features[0][5] & LMP_PAUSE_ENC)
#define lmp_esco_2m_capable(dev)   ((dev)->features[0][5] & LMP_EDR_ESCO_2M)
#define lmp_ext_inq_capable(dev)   ((dev)->features[0][6] & LMP_EXT_INQ)
#define lmp_le_br_capable(dev)     (!!((dev)->features[0][6] & LMP_SIMUL_LE_BR))
#define lmp_ssp_capable(dev)       ((dev)->features[0][6] & LMP_SIMPLE_PAIR)
#define lmp_no_flush_capable(dev)  ((dev)->features[0][6] & LMP_NO_FLUSH)
#define lmp_lsto_capable(dev)      ((dev)->features[0][7] & LMP_LSTO)
#define lmp_inq_tx_pwr_capable(dev) ((dev)->features[0][7] & LMP_INQ_TX_PWR)
#define lmp_ext_feat_capable(dev)  ((dev)->features[0][7] & LMP_EXTFEATURES)
#define lmp_transp_capable(dev)    ((dev)->features[0][2] & LMP_TRANSPARENT)
#define lmp_edr_2m_capable(dev)    ((dev)->features[0][3] & LMP_EDR_2M)
#define lmp_edr_3m_capable(dev)    ((dev)->features[0][3] & LMP_EDR_3M)
#define lmp_edr_3slot_capable(dev) ((dev)->features[0][4] & LMP_EDR_3SLOT)
#define lmp_edr_5slot_capable(dev) ((dev)->features[0][5] & LMP_EDR_5SLOT)

/* ----- Extended LMP capabilities ----- */
#define lmp_csb_master_capable(dev) ((dev)->features[2][0] & LMP_CSB_MASTER)
#define lmp_csb_slave_capable(dev)  ((dev)->features[2][0] & LMP_CSB_SLAVE)
#define lmp_sync_train_capable(dev) ((dev)->features[2][0] & LMP_SYNC_TRAIN)
#define lmp_sync_scan_capable(dev)  ((dev)->features[2][0] & LMP_SYNC_SCAN)
#define lmp_sc_capable(dev)         ((dev)->features[2][1] & LMP_SC)
#define lmp_ping_capable(dev)       ((dev)->features[2][1] & LMP_PING)

/* ----- Host capabilities ----- */
#define lmp_host_ssp_capable(dev)  ((dev)->features[1][0] & LMP_HOST_SSP)
#define lmp_host_sc_capable(dev)   ((dev)->features[1][0] & LMP_HOST_SC)
#define lmp_host_le_capable(dev)   (!!((dev)->features[1][0] & LMP_HOST_LE))
#define lmp_host_le_br_capable(dev) (!!((dev)->features[1][0] & LMP_HOST_LE_BREDR))

#define hdev_is_powered(dev)   (test_bit(HCI_UP, &(dev)->flags) && \
				!hci_dev_test_flag(dev, HCI_AUTO_OFF))
#define bredr_sc_enabled(dev)  (lmp_sc_capable(dev) && \
				hci_dev_test_flag(dev, HCI_SC_ENABLED))

#define scan_1m(dev) (((dev)->le_tx_def_phys & HCI_LE_SET_PHY_1M) || \
		      ((dev)->le_rx_def_phys & HCI_LE_SET_PHY_1M))

#define scan_2m(dev) (((dev)->le_tx_def_phys & HCI_LE_SET_PHY_2M) || \
		      ((dev)->le_rx_def_phys & HCI_LE_SET_PHY_2M))

#define scan_coded(dev) (((dev)->le_tx_def_phys & HCI_LE_SET_PHY_CODED) || \
			 ((dev)->le_rx_def_phys & HCI_LE_SET_PHY_CODED))

/* Use LL Privacy based address resolution if supported */
#define use_ll_privacy(dev) ((dev)->le_features[0] & HCI_LE_LL_PRIVACY)

/* Use ext scanning if set ext scan param and ext scan enable is supported */
#define use_ext_scan(dev) (((dev)->commands[37] & 0x20) && \
			   ((dev)->commands[37] & 0x40))
/* Use ext create connection if command is supported */
#define use_ext_conn(dev) ((dev)->commands[37] & 0x80)

/* Extended advertising support */
#define ext_adv_capable(dev) (((dev)->le_features[1] & HCI_LE_EXT_ADV))

/* ----- HCI protocols ----- */
#define HCI_PROTO_DEFER             0x01

static inline int hci_proto_connect_ind(struct hci_dev *hdev, bdaddr_t *bdaddr,
					__u8 type, __u8 *flags)
{
	switch (type) {
	case ACL_LINK:
		return l2cap_connect_ind(hdev, bdaddr);

	case SCO_LINK:
	case ESCO_LINK:
		return sco_connect_ind(hdev, bdaddr, flags);

	default:
		BT_ERR("unknown link type %d", type);
		return -EINVAL;
	}
}

static inline int hci_proto_disconn_ind(struct hci_conn *conn)
{
	if (conn->type != ACL_LINK && conn->type != LE_LINK)
		return HCI_ERROR_REMOTE_USER_TERM;

	return l2cap_disconn_ind(conn);
}

/* ----- HCI callbacks ----- */
struct hci_cb {
	struct list_head list;

	char *name;

	void (*connect_cfm)	(struct hci_conn *conn, __u8 status);
	void (*disconn_cfm)	(struct hci_conn *conn, __u8 status);
	void (*security_cfm)	(struct hci_conn *conn, __u8 status,
								__u8 encrypt);
	void (*key_change_cfm)	(struct hci_conn *conn, __u8 status);
	void (*role_switch_cfm)	(struct hci_conn *conn, __u8 status, __u8 role);
};

static inline void hci_connect_cfm(struct hci_conn *conn, __u8 status)
{
	struct hci_cb *cb;

	mutex_lock(&hci_cb_list_lock);
	list_for_each_entry(cb, &hci_cb_list, list) {
		if (cb->connect_cfm)
			cb->connect_cfm(conn, status);
	}
	mutex_unlock(&hci_cb_list_lock);

	if (conn->connect_cfm_cb)
		conn->connect_cfm_cb(conn, status);
}

static inline void hci_disconn_cfm(struct hci_conn *conn, __u8 reason)
{
	struct hci_cb *cb;

	mutex_lock(&hci_cb_list_lock);
	list_for_each_entry(cb, &hci_cb_list, list) {
		if (cb->disconn_cfm)
			cb->disconn_cfm(conn, reason);
	}
	mutex_unlock(&hci_cb_list_lock);

	if (conn->disconn_cfm_cb)
		conn->disconn_cfm_cb(conn, reason);
}

static inline void hci_auth_cfm(struct hci_conn *conn, __u8 status)
{
	struct hci_cb *cb;
	__u8 encrypt;

	if (test_bit(HCI_CONN_ENCRYPT_PEND, &conn->flags))
		return;

	encrypt = test_bit(HCI_CONN_ENCRYPT, &conn->flags) ? 0x01 : 0x00;

	mutex_lock(&hci_cb_list_lock);
	list_for_each_entry(cb, &hci_cb_list, list) {
		if (cb->security_cfm)
			cb->security_cfm(conn, status, encrypt);
	}
	mutex_unlock(&hci_cb_list_lock);

	if (conn->security_cfm_cb)
		conn->security_cfm_cb(conn, status);
}

static inline void hci_encrypt_cfm(struct hci_conn *conn, __u8 status)
{
	struct hci_cb *cb;
	__u8 encrypt;

	if (conn->state == BT_CONFIG) {
		if (!status)
			conn->state = BT_CONNECTED;

		hci_connect_cfm(conn, status);
		hci_conn_drop(conn);
		return;
	}

	if (!test_bit(HCI_CONN_ENCRYPT, &conn->flags))
		encrypt = 0x00;
	else if (test_bit(HCI_CONN_AES_CCM, &conn->flags))
		encrypt = 0x02;
	else
		encrypt = 0x01;

	if (!status) {
		if (conn->sec_level == BT_SECURITY_SDP)
			conn->sec_level = BT_SECURITY_LOW;

		if (conn->pending_sec_level > conn->sec_level)
			conn->sec_level = conn->pending_sec_level;
	}

	mutex_lock(&hci_cb_list_lock);
	list_for_each_entry(cb, &hci_cb_list, list) {
		if (cb->security_cfm)
			cb->security_cfm(conn, status, encrypt);
	}
	mutex_unlock(&hci_cb_list_lock);

	if (conn->security_cfm_cb)
		conn->security_cfm_cb(conn, status);
}

static inline void hci_key_change_cfm(struct hci_conn *conn, __u8 status)
{
	struct hci_cb *cb;

	mutex_lock(&hci_cb_list_lock);
	list_for_each_entry(cb, &hci_cb_list, list) {
		if (cb->key_change_cfm)
			cb->key_change_cfm(conn, status);
	}
	mutex_unlock(&hci_cb_list_lock);
}

static inline void hci_role_switch_cfm(struct hci_conn *conn, __u8 status,
								__u8 role)
{
	struct hci_cb *cb;

	mutex_lock(&hci_cb_list_lock);
	list_for_each_entry(cb, &hci_cb_list, list) {
		if (cb->role_switch_cfm)
			cb->role_switch_cfm(conn, status, role);
	}
	mutex_unlock(&hci_cb_list_lock);
}

static inline void *eir_get_data(u8 *eir, size_t eir_len, u8 type,
				 size_t *data_len)
{
	size_t parsed = 0;

	if (eir_len < 2)
		return NULL;

	while (parsed < eir_len - 1) {
		u8 field_len = eir[0];

		if (field_len == 0)
			break;

		parsed += field_len + 1;

		if (parsed > eir_len)
			break;

		if (eir[1] != type) {
			eir += field_len + 1;
			continue;
		}

		/* Zero length data */
		if (field_len == 1)
			return NULL;

		if (data_len)
			*data_len = field_len - 1;

		return &eir[2];
	}

	return NULL;
}

static inline bool hci_bdaddr_is_rpa(bdaddr_t *bdaddr, u8 addr_type)
{
	if (addr_type != ADDR_LE_DEV_RANDOM)
		return false;

	if ((bdaddr->b[5] & 0xc0) == 0x40)
	       return true;

	return false;
}

static inline bool hci_is_identity_address(bdaddr_t *addr, u8 addr_type)
{
	if (addr_type == ADDR_LE_DEV_PUBLIC)
		return true;

	/* Check for Random Static address type */
	if ((addr->b[5] & 0xc0) == 0xc0)
		return true;

	return false;
}

static inline struct smp_irk *hci_get_irk(struct hci_dev *hdev,
					  bdaddr_t *bdaddr, u8 addr_type)
{
	if (!hci_bdaddr_is_rpa(bdaddr, addr_type))
		return NULL;

	return hci_find_irk_by_rpa(hdev, bdaddr);
}

static inline int hci_check_conn_params(u16 min, u16 max, u16 latency,
					u16 to_multiplier)
{
	u16 max_latency;

	if (min > max || min < 6 || max > 3200)
		return -EINVAL;

	if (to_multiplier < 10 || to_multiplier > 3200)
		return -EINVAL;

	if (max >= to_multiplier * 8)
		return -EINVAL;

	max_latency = (to_multiplier * 4 / max) - 1;
	if (latency > 499 || latency > max_latency)
		return -EINVAL;

	return 0;
}

int hci_register_cb(struct hci_cb *hcb);
int hci_unregister_cb(struct hci_cb *hcb);

struct sk_buff *__hci_cmd_sync(struct hci_dev *hdev, u16 opcode, u32 plen,
			       const void *param, u32 timeout);
struct sk_buff *__hci_cmd_sync_ev(struct hci_dev *hdev, u16 opcode, u32 plen,
				  const void *param, u8 event, u32 timeout);
int __hci_cmd_send(struct hci_dev *hdev, u16 opcode, u32 plen,
		   const void *param);

int hci_send_cmd(struct hci_dev *hdev, __u16 opcode, __u32 plen,
		 const void *param);
void hci_send_acl(struct hci_chan *chan, struct sk_buff *skb, __u16 flags);
void hci_send_sco(struct hci_conn *conn, struct sk_buff *skb);

void *hci_sent_cmd_data(struct hci_dev *hdev, __u16 opcode);

struct sk_buff *hci_cmd_sync(struct hci_dev *hdev, u16 opcode, u32 plen,
			     const void *param, u32 timeout);

u32 hci_conn_get_phy(struct hci_conn *conn);

/* ----- HCI Sockets ----- */
void hci_send_to_sock(struct hci_dev *hdev, struct sk_buff *skb);
void hci_send_to_channel(unsigned short channel, struct sk_buff *skb,
			 int flag, struct sock *skip_sk);
void hci_send_to_monitor(struct hci_dev *hdev, struct sk_buff *skb);
void hci_send_monitor_ctrl_event(struct hci_dev *hdev, u16 event,
				 void *data, u16 data_len, ktime_t tstamp,
				 int flag, struct sock *skip_sk);

void hci_sock_dev_event(struct hci_dev *hdev, int event);

#define HCI_MGMT_VAR_LEN	BIT(0)
#define HCI_MGMT_NO_HDEV	BIT(1)
#define HCI_MGMT_UNTRUSTED	BIT(2)
#define HCI_MGMT_UNCONFIGURED	BIT(3)
#define HCI_MGMT_HDEV_OPTIONAL	BIT(4)

struct hci_mgmt_handler {
	int (*func) (struct sock *sk, struct hci_dev *hdev, void *data,
		     u16 data_len);
	size_t data_len;
	unsigned long flags;
};

struct hci_mgmt_chan {
	struct list_head list;
	unsigned short channel;
	size_t handler_count;
	const struct hci_mgmt_handler *handlers;
	void (*hdev_init) (struct sock *sk, struct hci_dev *hdev);
};

int hci_mgmt_chan_register(struct hci_mgmt_chan *c);
void hci_mgmt_chan_unregister(struct hci_mgmt_chan *c);

/* Management interface */
#define DISCOV_TYPE_BREDR		(BIT(BDADDR_BREDR))
#define DISCOV_TYPE_LE			(BIT(BDADDR_LE_PUBLIC) | \
					 BIT(BDADDR_LE_RANDOM))
#define DISCOV_TYPE_INTERLEAVED		(BIT(BDADDR_BREDR) | \
					 BIT(BDADDR_LE_PUBLIC) | \
					 BIT(BDADDR_LE_RANDOM))

/* These LE scan and inquiry parameters were chosen according to LE General
 * Discovery Procedure specification.
 */
#define DISCOV_LE_SCAN_WIN		0x12
#define DISCOV_LE_SCAN_INT		0x12
#define DISCOV_LE_TIMEOUT		10240	/* msec */
#define DISCOV_INTERLEAVED_TIMEOUT	5120	/* msec */
#define DISCOV_INTERLEAVED_INQUIRY_LEN	0x04
#define DISCOV_BREDR_INQUIRY_LEN	0x08
#define DISCOV_LE_RESTART_DELAY		msecs_to_jiffies(200)	/* msec */
#define DISCOV_LE_FAST_ADV_INT_MIN	0x00A0	/* 100 msec */
#define DISCOV_LE_FAST_ADV_INT_MAX	0x00F0	/* 150 msec */

void mgmt_fill_version_info(void *ver);
int mgmt_new_settings(struct hci_dev *hdev);
void mgmt_index_added(struct hci_dev *hdev);
void mgmt_index_removed(struct hci_dev *hdev);
void mgmt_set_powered_failed(struct hci_dev *hdev, int err);
void mgmt_power_on(struct hci_dev *hdev, int err);
void __mgmt_power_off(struct hci_dev *hdev);
void mgmt_new_link_key(struct hci_dev *hdev, struct link_key *key,
		       bool persistent);
void mgmt_device_connected(struct hci_dev *hdev, struct hci_conn *conn,
			   u32 flags, u8 *name, u8 name_len);
void mgmt_device_disconnected(struct hci_dev *hdev, bdaddr_t *bdaddr,
			      u8 link_type, u8 addr_type, u8 reason,
			      bool mgmt_connected);
void mgmt_disconnect_failed(struct hci_dev *hdev, bdaddr_t *bdaddr,
			    u8 link_type, u8 addr_type, u8 status);
void mgmt_connect_failed(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 link_type,
			 u8 addr_type, u8 status);
void mgmt_pin_code_request(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 secure);
void mgmt_pin_code_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				  u8 status);
void mgmt_pin_code_neg_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				      u8 status);
int mgmt_user_confirm_request(struct hci_dev *hdev, bdaddr_t *bdaddr,
			      u8 link_type, u8 addr_type, u32 value,
			      u8 confirm_hint);
int mgmt_user_confirm_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				     u8 link_type, u8 addr_type, u8 status);
int mgmt_user_confirm_neg_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
					 u8 link_type, u8 addr_type, u8 status);
int mgmt_user_passkey_request(struct hci_dev *hdev, bdaddr_t *bdaddr,
			      u8 link_type, u8 addr_type);
int mgmt_user_passkey_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
				     u8 link_type, u8 addr_type, u8 status);
int mgmt_user_passkey_neg_reply_complete(struct hci_dev *hdev, bdaddr_t *bdaddr,
					 u8 link_type, u8 addr_type, u8 status);
int mgmt_user_passkey_notify(struct hci_dev *hdev, bdaddr_t *bdaddr,
			     u8 link_type, u8 addr_type, u32 passkey,
			     u8 entered);
void mgmt_auth_failed(struct hci_conn *conn, u8 status);
void mgmt_auth_enable_complete(struct hci_dev *hdev, u8 status);
void mgmt_ssp_enable_complete(struct hci_dev *hdev, u8 enable, u8 status);
void mgmt_set_class_of_dev_complete(struct hci_dev *hdev, u8 *dev_class,
				    u8 status);
void mgmt_set_local_name_complete(struct hci_dev *hdev, u8 *name, u8 status);
void mgmt_start_discovery_complete(struct hci_dev *hdev, u8 status);
void mgmt_stop_discovery_complete(struct hci_dev *hdev, u8 status);
void mgmt_device_found(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 link_type,
		       u8 addr_type, u8 *dev_class, s8 rssi, u32 flags,
		       u8 *eir, u16 eir_len, u8 *scan_rsp, u8 scan_rsp_len);
void mgmt_remote_name(struct hci_dev *hdev, bdaddr_t *bdaddr, u8 link_type,
		      u8 addr_type, s8 rssi, u8 *name, u8 name_len);
void mgmt_discovering(struct hci_dev *hdev, u8 discovering);
void mgmt_suspending(struct hci_dev *hdev, u8 state);
void mgmt_resuming(struct hci_dev *hdev, u8 reason, bdaddr_t *bdaddr,
		   u8 addr_type);
bool mgmt_powering_down(struct hci_dev *hdev);
void mgmt_new_ltk(struct hci_dev *hdev, struct smp_ltk *key, bool persistent);
void mgmt_new_irk(struct hci_dev *hdev, struct smp_irk *irk, bool persistent);
void mgmt_new_csrk(struct hci_dev *hdev, struct smp_csrk *csrk,
		   bool persistent);
void mgmt_new_conn_param(struct hci_dev *hdev, bdaddr_t *bdaddr,
			 u8 bdaddr_type, u8 store_hint, u16 min_interval,
			 u16 max_interval, u16 latency, u16 timeout);
void mgmt_smp_complete(struct hci_conn *conn, bool complete);
bool mgmt_get_connectable(struct hci_dev *hdev);
void mgmt_set_connectable_complete(struct hci_dev *hdev, u8 status);
void mgmt_set_discoverable_complete(struct hci_dev *hdev, u8 status);
u8 mgmt_get_adv_discov_flags(struct hci_dev *hdev);
void mgmt_advertising_added(struct sock *sk, struct hci_dev *hdev,
			    u8 instance);
void mgmt_advertising_removed(struct sock *sk, struct hci_dev *hdev,
			      u8 instance);
void mgmt_adv_monitor_removed(struct hci_dev *hdev, u16 handle);
int mgmt_phy_configuration_changed(struct hci_dev *hdev, struct sock *skip);
int mgmt_add_adv_patterns_monitor_complete(struct hci_dev *hdev, u8 status);
int mgmt_remove_adv_monitor_complete(struct hci_dev *hdev, u8 status);

u8 hci_le_conn_update(struct hci_conn *conn, u16 min, u16 max, u16 latency,
		      u16 to_multiplier);
void hci_le_start_enc(struct hci_conn *conn, __le16 ediv, __le64 rand,
		      __u8 ltk[16], __u8 key_size);

void hci_copy_identity_address(struct hci_dev *hdev, bdaddr_t *bdaddr,
			       u8 *bdaddr_type);

#define SCO_AIRMODE_MASK       0x0003
#define SCO_AIRMODE_CVSD       0x0000
#define SCO_AIRMODE_TRANSP     0x0003

#endif /* __HCI_CORE_H */
