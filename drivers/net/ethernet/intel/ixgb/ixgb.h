/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 1999 - 2008 Intel Corporation. */

#ifndef _IXGB_H_
#define _IXGB_H_

#include <linux/stddef.h>
#include <linux/module.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/pagemap.h>
#include <linux/dma-mapping.h>
#include <linux/bitops.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/capability.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/pkt_sched.h>
#include <linux/list.h>
#include <linux/reboot.h>
#include <net/checksum.h>

#include <linux/ethtool.h>
#include <linux/if_vlan.h>

#define BAR_0		0
#define BAR_1		1

struct ixgb_adapter;
#include "ixgb_hw.h"
#include "ixgb_ee.h"
#include "ixgb_ids.h"

/* TX/RX descriptor defines */
#define DEFAULT_TXD      256
#define MAX_TXD         4096
#define MIN_TXD           64

/* hardware cannot reliably support more than 512 descriptors owned by
 * hardware descriptor cache otherwise an unreliable ring under heavy
 * receive load may result */
#define DEFAULT_RXD      512
#define MAX_RXD          512
#define MIN_RXD           64

/* Supported Rx Buffer Sizes */
#define IXGB_RXBUFFER_2048  2048
#define IXGB_RXBUFFER_4096  4096
#define IXGB_RXBUFFER_8192  8192
#define IXGB_RXBUFFER_16384 16384

/* How many Rx Buffers do we bundle into one write to the hardware ? */
#define IXGB_RX_BUFFER_WRITE	8	/* Must be power of 2 */

/* wrapper around a pointer to a socket buffer,
 * so a DMA handle can be stored along with the buffer */
struct ixgb_buffer {
	struct sk_buff *skb;
	dma_addr_t dma;
	unsigned long time_stamp;
	u16 length;
	u16 next_to_watch;
	u16 mapped_as_page;
};

struct ixgb_desc_ring {
	/* pointer to the descriptor ring memory */
	void *desc;
	/* physical address of the descriptor ring */
	dma_addr_t dma;
	/* length of descriptor ring in bytes */
	unsigned int size;
	/* number of descriptors in the ring */
	unsigned int count;
	/* next descriptor to associate a buffer with */
	unsigned int next_to_use;
	/* next descriptor to check for DD status bit */
	unsigned int next_to_clean;
	/* array of buffer information structs */
	struct ixgb_buffer *buffer_info;
};

#define IXGB_DESC_UNUSED(R) \
	((((R)->next_to_clean > (R)->next_to_use) ? 0 : (R)->count) + \
	(R)->next_to_clean - (R)->next_to_use - 1)

#define IXGB_GET_DESC(R, i, type)	(&(((struct type *)((R).desc))[i]))
#define IXGB_RX_DESC(R, i)		IXGB_GET_DESC(R, i, ixgb_rx_desc)
#define IXGB_TX_DESC(R, i)		IXGB_GET_DESC(R, i, ixgb_tx_desc)
#define IXGB_CONTEXT_DESC(R, i)	IXGB_GET_DESC(R, i, ixgb_context_desc)

/* board specific private data structure */

struct ixgb_adapter {
	struct timer_list watchdog_timer;
	unsigned long active_vlans[BITS_TO_LONGS(VLAN_N_VID)];
	u32 bd_number;
	u32 rx_buffer_len;
	u32 part_num;
	u16 link_speed;
	u16 link_duplex;
	struct work_struct tx_timeout_task;

	/* TX */
	struct ixgb_desc_ring tx_ring ____cacheline_aligned_in_smp;
	unsigned int restart_queue;
	unsigned long timeo_start;
	u32 tx_cmd_type;
	u64 hw_csum_tx_good;
	u64 hw_csum_tx_error;
	u32 tx_int_delay;
	u32 tx_timeout_count;
	bool tx_int_delay_enable;
	bool detect_tx_hung;

	/* RX */
	struct ixgb_desc_ring rx_ring;
	u64 hw_csum_rx_error;
	u64 hw_csum_rx_good;
	u32 rx_int_delay;
	bool rx_csum;

	/* OS defined structs */
	struct napi_struct napi;
	struct net_device *netdev;
	struct pci_dev *pdev;

	/* structs defined in ixgb_hw.h */
	struct ixgb_hw hw;
	u16 msg_enable;
	struct ixgb_hw_stats stats;
	u32 alloc_rx_buff_failed;
	bool have_msi;
	unsigned long flags;
};

enum ixgb_state_t {
	/* TBD
	__IXGB_TESTING,
	__IXGB_RESETTING,
	*/
	__IXGB_DOWN
};

/* Exported from other modules */
void ixgb_check_options(struct ixgb_adapter *adapter);
void ixgb_set_ethtool_ops(struct net_device *netdev);
extern char ixgb_driver_name[];

void ixgb_set_speed_duplex(struct net_device *netdev);

int ixgb_up(struct ixgb_adapter *adapter);
void ixgb_down(struct ixgb_adapter *adapter, bool kill_watchdog);
void ixgb_reset(struct ixgb_adapter *adapter);
int ixgb_setup_rx_resources(struct ixgb_adapter *adapter);
int ixgb_setup_tx_resources(struct ixgb_adapter *adapter);
void ixgb_free_rx_resources(struct ixgb_adapter *adapter);
void ixgb_free_tx_resources(struct ixgb_adapter *adapter);
void ixgb_update_stats(struct ixgb_adapter *adapter);


#endif /* _IXGB_H_ */
