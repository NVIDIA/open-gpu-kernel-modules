// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2013  Intel Corporation. All rights reserved.
 */

#define pr_fmt(fmt) "nci_spi: %s: " fmt, __func__

#include <linux/module.h>

#include <linux/export.h>
#include <linux/spi/spi.h>
#include <linux/crc-ccitt.h>
#include <net/nfc/nci_core.h>

#define NCI_SPI_ACK_SHIFT		6
#define NCI_SPI_MSB_PAYLOAD_MASK	0x3F

#define NCI_SPI_SEND_TIMEOUT	(NCI_CMD_TIMEOUT > NCI_DATA_TIMEOUT ? \
					NCI_CMD_TIMEOUT : NCI_DATA_TIMEOUT)

#define NCI_SPI_DIRECT_WRITE	0x01
#define NCI_SPI_DIRECT_READ	0x02

#define ACKNOWLEDGE_NONE	0
#define ACKNOWLEDGE_ACK		1
#define ACKNOWLEDGE_NACK	2

#define CRC_INIT		0xFFFF

static int __nci_spi_send(struct nci_spi *nspi, struct sk_buff *skb,
			  int cs_change)
{
	struct spi_message m;
	struct spi_transfer t;

	memset(&t, 0, sizeof(struct spi_transfer));
	/* a NULL skb means we just want the SPI chip select line to raise */
	if (skb) {
		t.tx_buf = skb->data;
		t.len = skb->len;
	} else {
		/* still set tx_buf non NULL to make the driver happy */
		t.tx_buf = &t;
		t.len = 0;
	}
	t.cs_change = cs_change;
	t.delay.value = nspi->xfer_udelay;
	t.delay.unit = SPI_DELAY_UNIT_USECS;
	t.speed_hz = nspi->xfer_speed_hz;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);

	return spi_sync(nspi->spi, &m);
}

int nci_spi_send(struct nci_spi *nspi,
		 struct completion *write_handshake_completion,
		 struct sk_buff *skb)
{
	unsigned int payload_len = skb->len;
	unsigned char *hdr;
	int ret;
	long completion_rc;

	/* add the NCI SPI header to the start of the buffer */
	hdr = skb_push(skb, NCI_SPI_HDR_LEN);
	hdr[0] = NCI_SPI_DIRECT_WRITE;
	hdr[1] = nspi->acknowledge_mode;
	hdr[2] = payload_len >> 8;
	hdr[3] = payload_len & 0xFF;

	if (nspi->acknowledge_mode == NCI_SPI_CRC_ENABLED) {
		u16 crc;

		crc = crc_ccitt(CRC_INIT, skb->data, skb->len);
		skb_put_u8(skb, crc >> 8);
		skb_put_u8(skb, crc & 0xFF);
	}

	if (write_handshake_completion)	{
		/* Trick SPI driver to raise chip select */
		ret = __nci_spi_send(nspi, NULL, 1);
		if (ret)
			goto done;

		/* wait for NFC chip hardware handshake to complete */
		if (wait_for_completion_timeout(write_handshake_completion,
						msecs_to_jiffies(1000)) == 0) {
			ret = -ETIME;
			goto done;
		}
	}

	ret = __nci_spi_send(nspi, skb, 0);
	if (ret != 0 || nspi->acknowledge_mode == NCI_SPI_CRC_DISABLED)
		goto done;

	reinit_completion(&nspi->req_completion);
	completion_rc =	wait_for_completion_interruptible_timeout(
							&nspi->req_completion,
							NCI_SPI_SEND_TIMEOUT);

	if (completion_rc <= 0 || nspi->req_result == ACKNOWLEDGE_NACK)
		ret = -EIO;

done:
	kfree_skb(skb);

	return ret;
}
EXPORT_SYMBOL_GPL(nci_spi_send);

/* ---- Interface to NCI SPI drivers ---- */

/**
 * nci_spi_allocate_spi - allocate a new nci spi
 *
 * @spi: SPI device
 * @acknowledge_mode: Acknowledge mode used by the NFC device
 * @delay: delay between transactions in us
 * @ndev: nci dev to send incoming nci frames to
 */
struct nci_spi *nci_spi_allocate_spi(struct spi_device *spi,
				     u8 acknowledge_mode, unsigned int delay,
				     struct nci_dev *ndev)
{
	struct nci_spi *nspi;

	nspi = devm_kzalloc(&spi->dev, sizeof(struct nci_spi), GFP_KERNEL);
	if (!nspi)
		return NULL;

	nspi->acknowledge_mode = acknowledge_mode;
	nspi->xfer_udelay = delay;
	/* Use controller max SPI speed by default */
	nspi->xfer_speed_hz = 0;
	nspi->spi = spi;
	nspi->ndev = ndev;
	init_completion(&nspi->req_completion);

	return nspi;
}
EXPORT_SYMBOL_GPL(nci_spi_allocate_spi);

static int send_acknowledge(struct nci_spi *nspi, u8 acknowledge)
{
	struct sk_buff *skb;
	unsigned char *hdr;
	u16 crc;
	int ret;

	skb = nci_skb_alloc(nspi->ndev, 0, GFP_KERNEL);

	/* add the NCI SPI header to the start of the buffer */
	hdr = skb_push(skb, NCI_SPI_HDR_LEN);
	hdr[0] = NCI_SPI_DIRECT_WRITE;
	hdr[1] = NCI_SPI_CRC_ENABLED;
	hdr[2] = acknowledge << NCI_SPI_ACK_SHIFT;
	hdr[3] = 0;

	crc = crc_ccitt(CRC_INIT, skb->data, skb->len);
	skb_put_u8(skb, crc >> 8);
	skb_put_u8(skb, crc & 0xFF);

	ret = __nci_spi_send(nspi, skb, 0);

	kfree_skb(skb);

	return ret;
}

static struct sk_buff *__nci_spi_read(struct nci_spi *nspi)
{
	struct sk_buff *skb;
	struct spi_message m;
	unsigned char req[2], resp_hdr[2];
	struct spi_transfer tx, rx;
	unsigned short rx_len = 0;
	int ret;

	spi_message_init(&m);

	memset(&tx, 0, sizeof(struct spi_transfer));
	req[0] = NCI_SPI_DIRECT_READ;
	req[1] = nspi->acknowledge_mode;
	tx.tx_buf = req;
	tx.len = 2;
	tx.cs_change = 0;
	tx.speed_hz = nspi->xfer_speed_hz;
	spi_message_add_tail(&tx, &m);

	memset(&rx, 0, sizeof(struct spi_transfer));
	rx.rx_buf = resp_hdr;
	rx.len = 2;
	rx.cs_change = 1;
	rx.speed_hz = nspi->xfer_speed_hz;
	spi_message_add_tail(&rx, &m);

	ret = spi_sync(nspi->spi, &m);
	if (ret)
		return NULL;

	if (nspi->acknowledge_mode == NCI_SPI_CRC_ENABLED)
		rx_len = ((resp_hdr[0] & NCI_SPI_MSB_PAYLOAD_MASK) << 8) +
				resp_hdr[1] + NCI_SPI_CRC_LEN;
	else
		rx_len = (resp_hdr[0] << 8) | resp_hdr[1];

	skb = nci_skb_alloc(nspi->ndev, rx_len, GFP_KERNEL);
	if (!skb)
		return NULL;

	spi_message_init(&m);

	memset(&rx, 0, sizeof(struct spi_transfer));
	rx.rx_buf = skb_put(skb, rx_len);
	rx.len = rx_len;
	rx.cs_change = 0;
	rx.delay.value = nspi->xfer_udelay;
	rx.delay.unit = SPI_DELAY_UNIT_USECS;
	rx.speed_hz = nspi->xfer_speed_hz;
	spi_message_add_tail(&rx, &m);

	ret = spi_sync(nspi->spi, &m);
	if (ret)
		goto receive_error;

	if (nspi->acknowledge_mode == NCI_SPI_CRC_ENABLED) {
		*(u8 *)skb_push(skb, 1) = resp_hdr[1];
		*(u8 *)skb_push(skb, 1) = resp_hdr[0];
	}

	return skb;

receive_error:
	kfree_skb(skb);

	return NULL;
}

static int nci_spi_check_crc(struct sk_buff *skb)
{
	u16 crc_data = (skb->data[skb->len - 2] << 8) |
			skb->data[skb->len - 1];
	int ret;

	ret = (crc_ccitt(CRC_INIT, skb->data, skb->len - NCI_SPI_CRC_LEN)
			== crc_data);

	skb_trim(skb, skb->len - NCI_SPI_CRC_LEN);

	return ret;
}

static u8 nci_spi_get_ack(struct sk_buff *skb)
{
	u8 ret;

	ret = skb->data[0] >> NCI_SPI_ACK_SHIFT;

	/* Remove NFCC part of the header: ACK, NACK and MSB payload len */
	skb_pull(skb, 2);

	return ret;
}

/**
 * nci_spi_read - read frame from NCI SPI drivers
 *
 * @nspi: The nci spi
 * Context: can sleep
 *
 * This call may only be used from a context that may sleep.  The sleep
 * is non-interruptible, and has no timeout.
 *
 * It returns an allocated skb containing the frame on success, or NULL.
 */
struct sk_buff *nci_spi_read(struct nci_spi *nspi)
{
	struct sk_buff *skb;

	/* Retrieve frame from SPI */
	skb = __nci_spi_read(nspi);
	if (!skb)
		goto done;

	if (nspi->acknowledge_mode == NCI_SPI_CRC_ENABLED) {
		if (!nci_spi_check_crc(skb)) {
			send_acknowledge(nspi, ACKNOWLEDGE_NACK);
			goto done;
		}

		/* In case of acknowledged mode: if ACK or NACK received,
		 * unblock completion of latest frame sent.
		 */
		nspi->req_result = nci_spi_get_ack(skb);
		if (nspi->req_result)
			complete(&nspi->req_completion);
	}

	/* If there is no payload (ACK/NACK only frame),
	 * free the socket buffer
	 */
	if (!skb->len) {
		kfree_skb(skb);
		skb = NULL;
		goto done;
	}

	if (nspi->acknowledge_mode == NCI_SPI_CRC_ENABLED)
		send_acknowledge(nspi, ACKNOWLEDGE_ACK);

done:

	return skb;
}
EXPORT_SYMBOL_GPL(nci_spi_read);

MODULE_LICENSE("GPL");
