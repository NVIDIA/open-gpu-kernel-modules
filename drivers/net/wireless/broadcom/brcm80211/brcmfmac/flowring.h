// SPDX-License-Identifier: ISC
/*
 * Copyright (c) 2014 Broadcom Corporation
 */
#ifndef BRCMFMAC_FLOWRING_H
#define BRCMFMAC_FLOWRING_H


#define BRCMF_FLOWRING_HASHSIZE		512		/* has to be 2^x */
#define BRCMF_FLOWRING_INVALID_ID	0xFFFFFFFF


struct brcmf_flowring_hash {
	u8 mac[ETH_ALEN];
	u8 fifo;
	u8 ifidx;
	u16 flowid;
};

enum ring_status {
	RING_CLOSED,
	RING_CLOSING,
	RING_OPEN
};

struct brcmf_flowring_ring {
	u16 hash_id;
	bool blocked;
	enum ring_status status;
	struct sk_buff_head skblist;
};

struct brcmf_flowring_tdls_entry {
	u8 mac[ETH_ALEN];
	struct brcmf_flowring_tdls_entry *next;
};

struct brcmf_flowring {
	struct device *dev;
	struct brcmf_flowring_hash hash[BRCMF_FLOWRING_HASHSIZE];
	struct brcmf_flowring_ring **rings;
	spinlock_t block_lock;
	enum proto_addr_mode addr_mode[BRCMF_MAX_IFS];
	u16 nrofrings;
	bool tdls_active;
	struct brcmf_flowring_tdls_entry *tdls_entry;
};


u32 brcmf_flowring_lookup(struct brcmf_flowring *flow, u8 da[ETH_ALEN],
			  u8 prio, u8 ifidx);
u32 brcmf_flowring_create(struct brcmf_flowring *flow, u8 da[ETH_ALEN],
			  u8 prio, u8 ifidx);
void brcmf_flowring_delete(struct brcmf_flowring *flow, u16 flowid);
void brcmf_flowring_open(struct brcmf_flowring *flow, u16 flowid);
u8 brcmf_flowring_tid(struct brcmf_flowring *flow, u16 flowid);
u32 brcmf_flowring_enqueue(struct brcmf_flowring *flow, u16 flowid,
			   struct sk_buff *skb);
struct sk_buff *brcmf_flowring_dequeue(struct brcmf_flowring *flow, u16 flowid);
void brcmf_flowring_reinsert(struct brcmf_flowring *flow, u16 flowid,
			     struct sk_buff *skb);
u32 brcmf_flowring_qlen(struct brcmf_flowring *flow, u16 flowid);
u8 brcmf_flowring_ifidx_get(struct brcmf_flowring *flow, u16 flowid);
struct brcmf_flowring *brcmf_flowring_attach(struct device *dev, u16 nrofrings);
void brcmf_flowring_detach(struct brcmf_flowring *flow);
void brcmf_flowring_configure_addr_mode(struct brcmf_flowring *flow, int ifidx,
					enum proto_addr_mode addr_mode);
void brcmf_flowring_delete_peer(struct brcmf_flowring *flow, int ifidx,
				u8 peer[ETH_ALEN]);
void brcmf_flowring_add_tdls_peer(struct brcmf_flowring *flow, int ifidx,
				  u8 peer[ETH_ALEN]);


#endif /* BRCMFMAC_FLOWRING_H */
