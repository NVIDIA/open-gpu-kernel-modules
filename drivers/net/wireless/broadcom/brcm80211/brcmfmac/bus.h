// SPDX-License-Identifier: ISC
/*
 * Copyright (c) 2010 Broadcom Corporation
 */

#ifndef BRCMFMAC_BUS_H
#define BRCMFMAC_BUS_H

#include "debug.h"

/* IDs of the 6 default common rings of msgbuf protocol */
#define BRCMF_H2D_MSGRING_CONTROL_SUBMIT	0
#define BRCMF_H2D_MSGRING_RXPOST_SUBMIT		1
#define BRCMF_H2D_MSGRING_FLOWRING_IDSTART	2
#define BRCMF_D2H_MSGRING_CONTROL_COMPLETE	2
#define BRCMF_D2H_MSGRING_TX_COMPLETE		3
#define BRCMF_D2H_MSGRING_RX_COMPLETE		4


#define BRCMF_NROF_H2D_COMMON_MSGRINGS		2
#define BRCMF_NROF_D2H_COMMON_MSGRINGS		3
#define BRCMF_NROF_COMMON_MSGRINGS	(BRCMF_NROF_H2D_COMMON_MSGRINGS + \
					 BRCMF_NROF_D2H_COMMON_MSGRINGS)

/* The level of bus communication with the dongle */
enum brcmf_bus_state {
	BRCMF_BUS_DOWN,		/* Not ready for frame transfers */
	BRCMF_BUS_UP		/* Ready for frame transfers */
};

/* The level of bus communication with the dongle */
enum brcmf_bus_protocol_type {
	BRCMF_PROTO_BCDC,
	BRCMF_PROTO_MSGBUF
};

struct brcmf_mp_device;

struct brcmf_bus_dcmd {
	char *name;
	char *param;
	int param_len;
	struct list_head list;
};

/**
 * struct brcmf_bus_ops - bus callback operations.
 *
 * @preinit: execute bus/device specific dongle init commands (optional).
 * @init: prepare for communication with dongle.
 * @stop: clear pending frames, disable data flow.
 * @txdata: send a data frame to the dongle. When the data
 *	has been transferred, the common driver must be
 *	notified using brcmf_txcomplete(). The common
 *	driver calls this function with interrupts
 *	disabled.
 * @txctl: transmit a control request message to dongle.
 * @rxctl: receive a control response message from dongle.
 * @gettxq: obtain a reference of bus transmit queue (optional).
 * @wowl_config: specify if dongle is configured for wowl when going to suspend
 * @get_ramsize: obtain size of device memory.
 * @get_memdump: obtain device memory dump in provided buffer.
 * @get_fwname: obtain firmware name.
 *
 * This structure provides an abstract interface towards the
 * bus specific driver. For control messages to common driver
 * will assure there is only one active transaction. Unless
 * indicated otherwise these callbacks are mandatory.
 */
struct brcmf_bus_ops {
	int (*preinit)(struct device *dev);
	void (*stop)(struct device *dev);
	int (*txdata)(struct device *dev, struct sk_buff *skb);
	int (*txctl)(struct device *dev, unsigned char *msg, uint len);
	int (*rxctl)(struct device *dev, unsigned char *msg, uint len);
	struct pktq * (*gettxq)(struct device *dev);
	void (*wowl_config)(struct device *dev, bool enabled);
	size_t (*get_ramsize)(struct device *dev);
	int (*get_memdump)(struct device *dev, void *data, size_t len);
	int (*get_fwname)(struct device *dev, const char *ext,
			  unsigned char *fw_name);
	void (*debugfs_create)(struct device *dev);
	int (*reset)(struct device *dev);
};


/**
 * struct brcmf_bus_msgbuf - bus ringbuf if in case of msgbuf.
 *
 * @commonrings: commonrings which are always there.
 * @flowrings: commonrings which are dynamically created and destroyed for data.
 * @rx_dataoffset: if set then all rx data has this this offset.
 * @max_rxbufpost: maximum number of buffers to post for rx.
 * @max_flowrings: maximum number of tx flow rings supported.
 * @max_submissionrings: maximum number of submission rings(h2d) supported.
 * @max_completionrings: maximum number of completion rings(d2h) supported.
 */
struct brcmf_bus_msgbuf {
	struct brcmf_commonring *commonrings[BRCMF_NROF_COMMON_MSGRINGS];
	struct brcmf_commonring **flowrings;
	u32 rx_dataoffset;
	u32 max_rxbufpost;
	u16 max_flowrings;
	u16 max_submissionrings;
	u16 max_completionrings;
};


/**
 * struct brcmf_bus_stats - bus statistic counters.
 *
 * @pktcowed: packets cowed for extra headroom/unorphan.
 * @pktcow_failed: packets dropped due to failed cow-ing.
 */
struct brcmf_bus_stats {
	atomic_t pktcowed;
	atomic_t pktcow_failed;
};

/**
 * struct brcmf_bus - interface structure between common and bus layer
 *
 * @bus_priv: pointer to private bus device.
 * @proto_type: protocol type, bcdc or msgbuf
 * @dev: device pointer of bus device.
 * @drvr: public driver information.
 * @state: operational state of the bus interface.
 * @stats: statistics shared between common and bus layer.
 * @maxctl: maximum size for rxctl request message.
 * @chip: device identifier of the dongle chip.
 * @always_use_fws_queue: bus wants use queue also when fwsignal is inactive.
 * @wowl_supported: is wowl supported by bus driver.
 * @chiprev: revision of the dongle chip.
 * @msgbuf: msgbuf protocol parameters provided by bus layer.
 */
struct brcmf_bus {
	union {
		struct brcmf_sdio_dev *sdio;
		struct brcmf_usbdev *usb;
		struct brcmf_pciedev *pcie;
	} bus_priv;
	enum brcmf_bus_protocol_type proto_type;
	struct device *dev;
	struct brcmf_pub *drvr;
	enum brcmf_bus_state state;
	struct brcmf_bus_stats stats;
	uint maxctl;
	u32 chip;
	u32 chiprev;
	bool always_use_fws_queue;
	bool wowl_supported;

	const struct brcmf_bus_ops *ops;
	struct brcmf_bus_msgbuf *msgbuf;
};

/*
 * callback wrappers
 */
static inline int brcmf_bus_preinit(struct brcmf_bus *bus)
{
	if (!bus->ops->preinit)
		return 0;
	return bus->ops->preinit(bus->dev);
}

static inline void brcmf_bus_stop(struct brcmf_bus *bus)
{
	bus->ops->stop(bus->dev);
}

static inline int brcmf_bus_txdata(struct brcmf_bus *bus, struct sk_buff *skb)
{
	return bus->ops->txdata(bus->dev, skb);
}

static inline
int brcmf_bus_txctl(struct brcmf_bus *bus, unsigned char *msg, uint len)
{
	return bus->ops->txctl(bus->dev, msg, len);
}

static inline
int brcmf_bus_rxctl(struct brcmf_bus *bus, unsigned char *msg, uint len)
{
	return bus->ops->rxctl(bus->dev, msg, len);
}

static inline
struct pktq *brcmf_bus_gettxq(struct brcmf_bus *bus)
{
	if (!bus->ops->gettxq)
		return ERR_PTR(-ENOENT);

	return bus->ops->gettxq(bus->dev);
}

static inline
void brcmf_bus_wowl_config(struct brcmf_bus *bus, bool enabled)
{
	if (bus->ops->wowl_config)
		bus->ops->wowl_config(bus->dev, enabled);
}

static inline size_t brcmf_bus_get_ramsize(struct brcmf_bus *bus)
{
	if (!bus->ops->get_ramsize)
		return 0;

	return bus->ops->get_ramsize(bus->dev);
}

static inline
int brcmf_bus_get_memdump(struct brcmf_bus *bus, void *data, size_t len)
{
	if (!bus->ops->get_memdump)
		return -EOPNOTSUPP;

	return bus->ops->get_memdump(bus->dev, data, len);
}

static inline
int brcmf_bus_get_fwname(struct brcmf_bus *bus, const char *ext,
			 unsigned char *fw_name)
{
	return bus->ops->get_fwname(bus->dev, ext, fw_name);
}

static inline
void brcmf_bus_debugfs_create(struct brcmf_bus *bus)
{
	if (!bus->ops->debugfs_create)
		return;

	return bus->ops->debugfs_create(bus->dev);
}

static inline
int brcmf_bus_reset(struct brcmf_bus *bus)
{
	if (!bus->ops->reset)
		return -EOPNOTSUPP;

	return bus->ops->reset(bus->dev);
}

/*
 * interface functions from common layer
 */

/* Receive frame for delivery to OS.  Callee disposes of rxp. */
void brcmf_rx_frame(struct device *dev, struct sk_buff *rxp, bool handle_event,
		    bool inirq);
/* Receive async event packet from firmware. Callee disposes of rxp. */
void brcmf_rx_event(struct device *dev, struct sk_buff *rxp);

int brcmf_alloc(struct device *dev, struct brcmf_mp_device *settings);
/* Indication from bus module regarding presence/insertion of dongle. */
int brcmf_attach(struct device *dev);
/* Indication from bus module regarding removal/absence of dongle */
void brcmf_detach(struct device *dev);
void brcmf_free(struct device *dev);
/* Indication from bus module that dongle should be reset */
void brcmf_dev_reset(struct device *dev);
/* Request from bus module to initiate a coredump */
void brcmf_dev_coredump(struct device *dev);
/* Indication that firmware has halted or crashed */
void brcmf_fw_crashed(struct device *dev);

/* Configure the "global" bus state used by upper layers */
void brcmf_bus_change_state(struct brcmf_bus *bus, enum brcmf_bus_state state);

s32 brcmf_iovar_data_set(struct device *dev, char *name, void *data, u32 len);
void brcmf_bus_add_txhdrlen(struct device *dev, uint len);

#ifdef CONFIG_BRCMFMAC_SDIO
void brcmf_sdio_exit(void);
int brcmf_sdio_register(void);
#else
static inline void brcmf_sdio_exit(void) { }
static inline int brcmf_sdio_register(void) { return 0; }
#endif

#ifdef CONFIG_BRCMFMAC_USB
void brcmf_usb_exit(void);
int brcmf_usb_register(void);
#else
static inline void brcmf_usb_exit(void) { }
static inline int brcmf_usb_register(void) { return 0; }
#endif

#ifdef CONFIG_BRCMFMAC_PCIE
void brcmf_pcie_exit(void);
int brcmf_pcie_register(void);
#else
static inline void brcmf_pcie_exit(void) { }
static inline int brcmf_pcie_register(void) { return 0; }
#endif

#endif /* BRCMFMAC_BUS_H */
