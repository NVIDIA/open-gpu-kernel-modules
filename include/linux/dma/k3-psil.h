/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright (C) 2019 Texas Instruments Incorporated - https://www.ti.com
 */

#ifndef K3_PSIL_H_
#define K3_PSIL_H_

#include <linux/types.h>

#define K3_PSIL_DST_THREAD_ID_OFFSET 0x8000

struct device;

/**
 * enum udma_tp_level - Channel Throughput Levels
 * @UDMA_TP_NORMAL:	Normal channel
 * @UDMA_TP_HIGH:	High Throughput channel
 * @UDMA_TP_ULTRAHIGH:	Ultra High Throughput channel
 */
enum udma_tp_level {
	UDMA_TP_NORMAL = 0,
	UDMA_TP_HIGH,
	UDMA_TP_ULTRAHIGH,
	UDMA_TP_LAST,
};

/**
 * enum psil_endpoint_type - PSI-L Endpoint type
 * @PSIL_EP_NATIVE:	Normal channel
 * @PSIL_EP_PDMA_XY:	XY mode PDMA
 * @PSIL_EP_PDMA_MCAN:	MCAN mode PDMA
 * @PSIL_EP_PDMA_AASRC: AASRC mode PDMA
 */
enum psil_endpoint_type {
	PSIL_EP_NATIVE = 0,
	PSIL_EP_PDMA_XY,
	PSIL_EP_PDMA_MCAN,
	PSIL_EP_PDMA_AASRC,
};

/**
 * struct psil_endpoint_config - PSI-L Endpoint configuration
 * @ep_type:		PSI-L endpoint type
 * @channel_tpl:	Desired throughput level for the channel
 * @pkt_mode:		If set, the channel must be in Packet mode, otherwise in
 *			TR mode
 * @notdpkt:		TDCM must be suppressed on the TX channel
 * @needs_epib:		Endpoint needs EPIB
 * @pdma_acc32:		ACC32 must be enabled on the PDMA side
 * @pdma_burst:		BURST must be enabled on the PDMA side
 * @psd_size:		If set, PSdata is used by the endpoint
 * @mapped_channel_id:	PKTDMA thread to channel mapping for mapped channels.
 *			The thread must be serviced by the specified channel if
 *			mapped_channel_id is >= 0 in case of PKTDMA
 * @flow_start:		PKDMA flow range start of mapped channel. Unmapped
 *			channels use flow_id == chan_id
 * @flow_num:		PKDMA flow count of mapped channel. Unmapped channels
 *			use flow_id == chan_id
 * @default_flow_id:	PKDMA default (r)flow index of mapped channel.
 *			Must be within the flow range of the mapped channel.
 */
struct psil_endpoint_config {
	enum psil_endpoint_type ep_type;
	enum udma_tp_level channel_tpl;

	unsigned pkt_mode:1;
	unsigned notdpkt:1;
	unsigned needs_epib:1;
	/* PDMA properties, valid for PSIL_EP_PDMA_* */
	unsigned pdma_acc32:1;
	unsigned pdma_burst:1;

	u32 psd_size;
	/* PKDMA mapped channel */
	s16 mapped_channel_id;
	/* PKTDMA tflow and rflow ranges for mapped channel */
	u16 flow_start;
	u16 flow_num;
	s16 default_flow_id;
};

int psil_set_new_ep_config(struct device *dev, const char *name,
			   struct psil_endpoint_config *ep_config);

#endif /* K3_PSIL_H_ */
