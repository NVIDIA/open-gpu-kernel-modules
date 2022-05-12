// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) ST-Ericsson AB 2010
 * Author:	Sjur Brendeland
 */

#define pr_fmt(fmt) KBUILD_MODNAME ":%s(): " fmt, __func__

#include <linux/stddef.h>
#include <linux/slab.h>
#include <net/caif/caif_layer.h>
#include <net/caif/cfsrvl.h>
#include <net/caif/cfpkt.h>

#define VEI_PAYLOAD  0x00
#define VEI_CMD_BIT  0x80
#define VEI_FLOW_OFF 0x81
#define VEI_FLOW_ON  0x80
#define VEI_SET_PIN  0x82

#define container_obj(layr) container_of(layr, struct cfsrvl, layer)

static int cfvei_receive(struct cflayer *layr, struct cfpkt *pkt);
static int cfvei_transmit(struct cflayer *layr, struct cfpkt *pkt);

struct cflayer *cfvei_create(u8 channel_id, struct dev_info *dev_info)
{
	struct cfsrvl *vei = kzalloc(sizeof(struct cfsrvl), GFP_ATOMIC);
	if (!vei)
		return NULL;
	caif_assert(offsetof(struct cfsrvl, layer) == 0);
	cfsrvl_init(vei, channel_id, dev_info, true);
	vei->layer.receive = cfvei_receive;
	vei->layer.transmit = cfvei_transmit;
	snprintf(vei->layer.name, CAIF_LAYER_NAME_SZ, "vei%d", channel_id);
	return &vei->layer;
}

static int cfvei_receive(struct cflayer *layr, struct cfpkt *pkt)
{
	u8 cmd;
	int ret;
	caif_assert(layr->up != NULL);
	caif_assert(layr->receive != NULL);
	caif_assert(layr->ctrlcmd != NULL);


	if (cfpkt_extr_head(pkt, &cmd, 1) < 0) {
		pr_err("Packet is erroneous!\n");
		cfpkt_destroy(pkt);
		return -EPROTO;
	}
	switch (cmd) {
	case VEI_PAYLOAD:
		ret = layr->up->receive(layr->up, pkt);
		return ret;
	case VEI_FLOW_OFF:
		layr->ctrlcmd(layr, CAIF_CTRLCMD_FLOW_OFF_IND, 0);
		cfpkt_destroy(pkt);
		return 0;
	case VEI_FLOW_ON:
		layr->ctrlcmd(layr, CAIF_CTRLCMD_FLOW_ON_IND, 0);
		cfpkt_destroy(pkt);
		return 0;
	case VEI_SET_PIN:	/* SET RS232 PIN */
		cfpkt_destroy(pkt);
		return 0;
	default:		/* SET RS232 PIN */
		pr_warn("Unknown VEI control packet %d (0x%x)!\n", cmd, cmd);
		cfpkt_destroy(pkt);
		return -EPROTO;
	}
}

static int cfvei_transmit(struct cflayer *layr, struct cfpkt *pkt)
{
	u8 tmp = 0;
	struct caif_payload_info *info;
	int ret;
	struct cfsrvl *service = container_obj(layr);
	if (!cfsrvl_ready(service, &ret))
		goto err;
	caif_assert(layr->dn != NULL);
	caif_assert(layr->dn->transmit != NULL);

	if (cfpkt_add_head(pkt, &tmp, 1) < 0) {
		pr_err("Packet is erroneous!\n");
		ret = -EPROTO;
		goto err;
	}

	/* Add info-> for MUX-layer to route the packet out. */
	info = cfpkt_info(pkt);
	info->channel_id = service->layer.id;
	info->hdr_len = 1;
	info->dev_info = &service->dev_info;
	return layr->dn->transmit(layr->dn, pkt);
err:
	cfpkt_destroy(pkt);
	return ret;
}
