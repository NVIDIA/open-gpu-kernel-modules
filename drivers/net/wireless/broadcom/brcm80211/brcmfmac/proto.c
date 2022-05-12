// SPDX-License-Identifier: ISC
/*
 * Copyright (c) 2013 Broadcom Corporation
 */


 #include <linux/types.h>
#include <linux/slab.h>
#include <linux/netdevice.h>

#include <brcmu_wifi.h>
#include "core.h"
#include "bus.h"
#include "debug.h"
#include "proto.h"
#include "bcdc.h"
#include "msgbuf.h"


int brcmf_proto_attach(struct brcmf_pub *drvr)
{
	struct brcmf_proto *proto;

	brcmf_dbg(TRACE, "Enter\n");

	proto = kzalloc(sizeof(*proto), GFP_ATOMIC);
	if (!proto)
		goto fail;

	drvr->proto = proto;

	if (drvr->bus_if->proto_type == BRCMF_PROTO_BCDC) {
		if (brcmf_proto_bcdc_attach(drvr))
			goto fail;
	} else if (drvr->bus_if->proto_type == BRCMF_PROTO_MSGBUF) {
		if (brcmf_proto_msgbuf_attach(drvr))
			goto fail;
	} else {
		bphy_err(drvr, "Unsupported proto type %d\n",
			 drvr->bus_if->proto_type);
		goto fail;
	}
	if (!proto->tx_queue_data || (proto->hdrpull == NULL) ||
	    (proto->query_dcmd == NULL) || (proto->set_dcmd == NULL) ||
	    (proto->configure_addr_mode == NULL) ||
	    (proto->delete_peer == NULL) || (proto->add_tdls_peer == NULL) ||
	    (proto->debugfs_create == NULL)) {
		bphy_err(drvr, "Not all proto handlers have been installed\n");
		goto fail;
	}
	return 0;

fail:
	kfree(proto);
	drvr->proto = NULL;
	return -ENOMEM;
}

void brcmf_proto_detach(struct brcmf_pub *drvr)
{
	brcmf_dbg(TRACE, "Enter\n");

	if (drvr->proto) {
		if (drvr->bus_if->proto_type == BRCMF_PROTO_BCDC)
			brcmf_proto_bcdc_detach(drvr);
		else if (drvr->bus_if->proto_type == BRCMF_PROTO_MSGBUF)
			brcmf_proto_msgbuf_detach(drvr);
		kfree(drvr->proto);
		drvr->proto = NULL;
	}
}
