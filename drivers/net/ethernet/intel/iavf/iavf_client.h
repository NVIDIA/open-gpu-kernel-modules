/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2013 - 2018 Intel Corporation. */

#ifndef _IAVF_CLIENT_H_
#define _IAVF_CLIENT_H_

#define IAVF_CLIENT_STR_LENGTH 10

/* Client interface version should be updated anytime there is a change in the
 * existing APIs or data structures.
 */
#define IAVF_CLIENT_VERSION_MAJOR 0
#define IAVF_CLIENT_VERSION_MINOR 01
#define IAVF_CLIENT_VERSION_BUILD 00
#define IAVF_CLIENT_VERSION_STR     \
	__stringify(IAVF_CLIENT_VERSION_MAJOR) "." \
	__stringify(IAVF_CLIENT_VERSION_MINOR) "." \
	__stringify(IAVF_CLIENT_VERSION_BUILD)

struct iavf_client_version {
	u8 major;
	u8 minor;
	u8 build;
	u8 rsvd;
};

enum iavf_client_state {
	__IAVF_CLIENT_NULL,
	__IAVF_CLIENT_REGISTERED
};

enum iavf_client_instance_state {
	__IAVF_CLIENT_INSTANCE_NONE,
	__IAVF_CLIENT_INSTANCE_OPENED,
};

struct iavf_ops;
struct iavf_client;

/* HW does not define a type value for AEQ; only for RX/TX and CEQ.
 * In order for us to keep the interface simple, SW will define a
 * unique type value for AEQ.
 */
#define IAVF_QUEUE_TYPE_PE_AEQ	0x80
#define IAVF_QUEUE_INVALID_IDX	0xFFFF

struct iavf_qv_info {
	u32 v_idx; /* msix_vector */
	u16 ceq_idx;
	u16 aeq_idx;
	u8 itr_idx;
};

struct iavf_qvlist_info {
	u32 num_vectors;
	struct iavf_qv_info qv_info[1];
};

#define IAVF_CLIENT_MSIX_ALL 0xFFFFFFFF

/* set of LAN parameters useful for clients managed by LAN */

/* Struct to hold per priority info */
struct iavf_prio_qos_params {
	u16 qs_handle; /* qs handle for prio */
	u8 tc; /* TC mapped to prio */
	u8 reserved;
};

#define IAVF_CLIENT_MAX_USER_PRIORITY	8
/* Struct to hold Client QoS */
struct iavf_qos_params {
	struct iavf_prio_qos_params prio_qos[IAVF_CLIENT_MAX_USER_PRIORITY];
};

struct iavf_params {
	struct iavf_qos_params qos;
	u16 mtu;
	u16 link_up; /* boolean */
};

/* Structure to hold LAN device info for a client device */
struct iavf_info {
	struct iavf_client_version version;
	u8 lanmac[6];
	struct net_device *netdev;
	struct pci_dev *pcidev;
	u8 __iomem *hw_addr;
	u8 fid;	/* function id, PF id or VF id */
#define IAVF_CLIENT_FTYPE_PF 0
#define IAVF_CLIENT_FTYPE_VF 1
	u8 ftype; /* function type, PF or VF */
	void *vf; /* cast to iavf_adapter */

	/* All L2 params that could change during the life span of the device
	 * and needs to be communicated to the client when they change
	 */
	struct iavf_params params;
	struct iavf_ops *ops;

	u16 msix_count;	 /* number of msix vectors*/
	/* Array down below will be dynamically allocated based on msix_count */
	struct msix_entry *msix_entries;
	u16 itr_index; /* Which ITR index the PE driver is suppose to use */
};

struct iavf_ops {
	/* setup_q_vector_list enables queues with a particular vector */
	int (*setup_qvlist)(struct iavf_info *ldev, struct iavf_client *client,
			    struct iavf_qvlist_info *qv_info);

	u32 (*virtchnl_send)(struct iavf_info *ldev, struct iavf_client *client,
			     u8 *msg, u16 len);

	/* If the PE Engine is unresponsive, RDMA driver can request a reset.*/
	void (*request_reset)(struct iavf_info *ldev,
			      struct iavf_client *client);
};

struct iavf_client_ops {
	/* Should be called from register_client() or whenever the driver is
	 * ready to create a specific client instance.
	 */
	int (*open)(struct iavf_info *ldev, struct iavf_client *client);

	/* Should be closed when netdev is unavailable or when unregister
	 * call comes in. If the close happens due to a reset, set the reset
	 * bit to true.
	 */
	void (*close)(struct iavf_info *ldev, struct iavf_client *client,
		      bool reset);

	/* called when some l2 managed parameters changes - mss */
	void (*l2_param_change)(struct iavf_info *ldev,
				struct iavf_client *client,
				struct iavf_params *params);

	/* called when a message is received from the PF */
	int (*virtchnl_receive)(struct iavf_info *ldev,
				struct iavf_client *client,
				u8 *msg, u16 len);
};

/* Client device */
struct iavf_client_instance {
	struct list_head list;
	struct iavf_info lan_info;
	struct iavf_client *client;
	unsigned long  state;
};

struct iavf_client {
	struct list_head list;		/* list of registered clients */
	char name[IAVF_CLIENT_STR_LENGTH];
	struct iavf_client_version version;
	unsigned long state;		/* client state */
	atomic_t ref_cnt;  /* Count of all the client devices of this kind */
	u32 flags;
#define IAVF_CLIENT_FLAGS_LAUNCH_ON_PROBE	BIT(0)
#define IAVF_TX_FLAGS_NOTIFY_OTHER_EVENTS	BIT(2)
	u8 type;
#define IAVF_CLIENT_IWARP 0
	struct iavf_client_ops *ops;	/* client ops provided by the client */
};

/* used by clients */
int iavf_register_client(struct iavf_client *client);
int iavf_unregister_client(struct iavf_client *client);
#endif /* _IAVF_CLIENT_H_ */
