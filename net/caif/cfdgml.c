// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) ST-Ericsson AB 2010
 * Author:	Sjur Brendeland
 */

#define pr_fmt(fmt) KBUILD_MODNAME ":%s(): " fmt, __func__

#include <linux/stddef.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <net/caif/caif_layer.h>
#include <net/caif/cfsrvl.h>
#include <net/caif/cfpkt.h>


#define container_obj(layr) ((struct cfsrvl *) layr)

#define DGM_CMD_BIT  0x80
#define DGM_FLOW_OFF 0x81
#define DGM_FLOW_ON  0x80
#define DGM_MTU 1500

static int cfdgml_receive(struct cflayer *layr, struct cfpkt *pkt);
static int cfdgml_transmit(struct cflayer *layr, struct cfpkt *pkt);

struct cflayer *cfdgml_create(u8 channel_id, struct dev_info *dev_info)
{
	struct cfsrvl *dgm = kzalloc(sizeof(struct cfsrvl), GFP_ATOMIC);
	if (!dgm)
		return NULL;
	caif_assert(offsetof(struct cfsrvl, layer) == 0);
	cfsrvl_init(dgm, channel_id, dev_info, true);
	dgm->layer.receive = cfdgml_receive;
	dgm->layer.transmit = cfdgml_transmit;
	snprintf(dgm->layer.name, CAIF_LAYER_NAME_SZ, "dgm%d", channel_id);
	return &dgm->layer;
}

static int cfdgml_receive(struct cflayer *layr, struct cfpkt *pkt)
{
	u8 cmd = -1;
	u8 dgmhdr[3];
	int ret;
	caif_assert(layr->up != NULL);
	caif_assert(layr->receive != NULL);
	caif_assert(layr->ctrlcmd != NULL);

	if (cfpkt_extr_head(pkt, &cmd, 1) < 0) {
		pr_err("Packet is erroneous!\n");
		cfpkt_destroy(pkt);
		return -EPROTO;
	}

	if ((cmd & DGM_CMD_BIT) == 0) {
		if (cfpkt_extr_head(pkt, &dgmhdr, 3) < 0) {
			pr_err("Packet is erroneous!\n");
			cfpkt_destroy(pkt);
			return -EPROTO;
		}
		ret = layr->up->receive(layr->up, pkt);
		return ret;
	}

	switch (cmd) {
	case DGM_FLOW_OFF:	/* FLOW OFF */
		layr->ctrlcmd(layr, CAIF_CTRLCMD_FLOW_OFF_IND, 0);
		cfpkt_destroy(pkt);
		return 0;
	case DGM_FLOW_ON:	/* FLOW ON */
		layr->ctrlcmd(layr, CAIF_CTRLCMD_FLOW_ON_IND, 0);
		cfpkt_destroy(pkt);
		return 0;
	default:
		cfpkt_destroy(pkt);
		pr_info("Unknown datagram control %d (0x%x)\n", cmd, cmd);
		return -EPROTO;
	}
}

static int cfdgml_transmit(struct cflayer *layr, struct cfpkt *pkt)
{
	u8 packet_type;
	u32 zero = 0;
	struct caif_payload_info *info;
	struct cfsrvl *service = container_obj(layr);
	int ret;

	if (!cfsrvl_ready(service, &ret)) {
		cfpkt_destroy(pkt);
		return ret;
	}

	/* STE Modem cannot handle more than 1500 bytes datagrams */
	if (cfpkt_getlen(pkt) > DGM_MTU) {
		cfpkt_destroy(pkt);
		return -EMSGSIZE;
	}

	cfpkt_add_head(pkt, &zero, 3);
	packet_type = 0x08; /* B9 set - UNCLASSIFIED */
	cfpkt_add_head(pkt, &packet_type, 1);

	/* Add info for MUX-layer to route the packet out. */
	info = cfpkt_info(pkt);
	info->channel_id = service->layer.id;
	/* To optimize alignment, we add up the size of CAIF header
	 * before payload.
	 */
	info->hdr_len = 4;
	info->dev_info = &service->dev_info;
	return layr->dn->transmit(layr->dn, pkt);
}
