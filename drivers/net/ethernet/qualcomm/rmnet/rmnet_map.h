/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2013-2018, The Linux Foundation. All rights reserved.
 */

#ifndef _RMNET_MAP_H_
#define _RMNET_MAP_H_
#include <linux/if_rmnet.h>

struct rmnet_map_control_command {
	u8  command_name;
	u8  cmd_type:2;
	u8  reserved:6;
	u16 reserved2;
	u32 transaction_id;
	union {
		struct {
			u16 ip_family:2;
			u16 reserved:14;
			__be16 flow_control_seq_num;
			__be32 qos_id;
		} flow_control;
		u8 data[0];
	};
}  __aligned(1);

enum rmnet_map_commands {
	RMNET_MAP_COMMAND_NONE,
	RMNET_MAP_COMMAND_FLOW_DISABLE,
	RMNET_MAP_COMMAND_FLOW_ENABLE,
	/* These should always be the last 2 elements */
	RMNET_MAP_COMMAND_UNKNOWN,
	RMNET_MAP_COMMAND_ENUM_LENGTH
};

#define RMNET_MAP_COMMAND_REQUEST     0
#define RMNET_MAP_COMMAND_ACK         1
#define RMNET_MAP_COMMAND_UNSUPPORTED 2
#define RMNET_MAP_COMMAND_INVALID     3

#define RMNET_MAP_NO_PAD_BYTES        0
#define RMNET_MAP_ADD_PAD_BYTES       1

struct sk_buff *rmnet_map_deaggregate(struct sk_buff *skb,
				      struct rmnet_port *port);
struct rmnet_map_header *rmnet_map_add_map_header(struct sk_buff *skb,
						  int hdrlen, int pad);
void rmnet_map_command(struct sk_buff *skb, struct rmnet_port *port);
int rmnet_map_checksum_downlink_packet(struct sk_buff *skb, u16 len);
void rmnet_map_checksum_uplink_packet(struct sk_buff *skb,
				      struct net_device *orig_dev);

#endif /* _RMNET_MAP_H_ */
