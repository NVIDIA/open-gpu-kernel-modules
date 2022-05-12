/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2011 Instituto Nokia de Tecnologia
 *
 * Authors:
 *    Lauro Ramos Venancio <lauro.venancio@openbossa.org>
 *    Aloisio Almeida Jr <aloisio.almeida@openbossa.org>
 */

#ifndef __LOCAL_NFC_H
#define __LOCAL_NFC_H

#include <net/nfc/nfc.h>
#include <net/sock.h>

#define NFC_TARGET_MODE_IDLE 0
#define NFC_TARGET_MODE_SLEEP 1

struct nfc_protocol {
	int id;
	struct proto *proto;
	struct module *owner;
	int (*create)(struct net *net, struct socket *sock,
		      const struct nfc_protocol *nfc_proto, int kern);
};

struct nfc_rawsock {
	struct sock sk;
	struct nfc_dev *dev;
	u32 target_idx;
	struct work_struct tx_work;
	bool tx_work_scheduled;
};

struct nfc_sock_list {
	struct hlist_head head;
	rwlock_t          lock;
};

#define nfc_rawsock(sk) ((struct nfc_rawsock *) sk)
#define to_rawsock_sk(_tx_work) \
	((struct sock *) container_of(_tx_work, struct nfc_rawsock, tx_work))

struct nfc_llcp_sdp_tlv;

void nfc_llcp_mac_is_down(struct nfc_dev *dev);
void nfc_llcp_mac_is_up(struct nfc_dev *dev, u32 target_idx,
			u8 comm_mode, u8 rf_mode);
int nfc_llcp_register_device(struct nfc_dev *dev);
void nfc_llcp_unregister_device(struct nfc_dev *dev);
int nfc_llcp_set_remote_gb(struct nfc_dev *dev, u8 *gb, u8 gb_len);
u8 *nfc_llcp_general_bytes(struct nfc_dev *dev, size_t *general_bytes_len);
int nfc_llcp_data_received(struct nfc_dev *dev, struct sk_buff *skb);
struct nfc_llcp_local *nfc_llcp_find_local(struct nfc_dev *dev);
int __init nfc_llcp_init(void);
void nfc_llcp_exit(void);
void nfc_llcp_free_sdp_tlv(struct nfc_llcp_sdp_tlv *sdp);
void nfc_llcp_free_sdp_tlv_list(struct hlist_head *head);

int __init rawsock_init(void);
void rawsock_exit(void);

int __init af_nfc_init(void);
void af_nfc_exit(void);
int nfc_proto_register(const struct nfc_protocol *nfc_proto);
void nfc_proto_unregister(const struct nfc_protocol *nfc_proto);

extern int nfc_devlist_generation;
extern struct mutex nfc_devlist_mutex;

int __init nfc_genl_init(void);
void nfc_genl_exit(void);

void nfc_genl_data_init(struct nfc_genl_data *genl_data);
void nfc_genl_data_exit(struct nfc_genl_data *genl_data);

int nfc_genl_targets_found(struct nfc_dev *dev);
int nfc_genl_target_lost(struct nfc_dev *dev, u32 target_idx);

int nfc_genl_device_added(struct nfc_dev *dev);
int nfc_genl_device_removed(struct nfc_dev *dev);

int nfc_genl_dep_link_up_event(struct nfc_dev *dev, u32 target_idx,
			       u8 comm_mode, u8 rf_mode);
int nfc_genl_dep_link_down_event(struct nfc_dev *dev);

int nfc_genl_tm_activated(struct nfc_dev *dev, u32 protocol);
int nfc_genl_tm_deactivated(struct nfc_dev *dev);

int nfc_genl_llc_send_sdres(struct nfc_dev *dev, struct hlist_head *sdres_list);

int nfc_genl_se_added(struct nfc_dev *dev, u32 se_idx, u16 type);
int nfc_genl_se_removed(struct nfc_dev *dev, u32 se_idx);
int nfc_genl_se_transaction(struct nfc_dev *dev, u8 se_idx,
			    struct nfc_evt_transaction *evt_transaction);
int nfc_genl_se_connectivity(struct nfc_dev *dev, u8 se_idx);

struct nfc_dev *nfc_get_device(unsigned int idx);

static inline void nfc_put_device(struct nfc_dev *dev)
{
	put_device(&dev->dev);
}

static inline void nfc_device_iter_init(struct class_dev_iter *iter)
{
	class_dev_iter_init(iter, &nfc_class, NULL, NULL);
}

static inline struct nfc_dev *nfc_device_iter_next(struct class_dev_iter *iter)
{
	struct device *d = class_dev_iter_next(iter);
	if (!d)
		return NULL;

	return to_nfc_dev(d);
}

static inline void nfc_device_iter_exit(struct class_dev_iter *iter)
{
	class_dev_iter_exit(iter);
}

int nfc_fw_download(struct nfc_dev *dev, const char *firmware_name);
int nfc_genl_fw_download_done(struct nfc_dev *dev, const char *firmware_name,
			      u32 result);

int nfc_dev_up(struct nfc_dev *dev);

int nfc_dev_down(struct nfc_dev *dev);

int nfc_start_poll(struct nfc_dev *dev, u32 im_protocols, u32 tm_protocols);

int nfc_stop_poll(struct nfc_dev *dev);

int nfc_dep_link_up(struct nfc_dev *dev, int target_idx, u8 comm_mode);

int nfc_dep_link_down(struct nfc_dev *dev);

int nfc_activate_target(struct nfc_dev *dev, u32 target_idx, u32 protocol);

int nfc_deactivate_target(struct nfc_dev *dev, u32 target_idx, u8 mode);

int nfc_data_exchange(struct nfc_dev *dev, u32 target_idx, struct sk_buff *skb,
		      data_exchange_cb_t cb, void *cb_context);

int nfc_enable_se(struct nfc_dev *dev, u32 se_idx);
int nfc_disable_se(struct nfc_dev *dev, u32 se_idx);

#endif /* __LOCAL_NFC_H */
