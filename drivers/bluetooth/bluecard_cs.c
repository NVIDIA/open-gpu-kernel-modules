/*
 *
 *  Bluetooth driver for the Anycom BlueCard (LSE039/LSE041)
 *
 *  Copyright (C) 2001-2002  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *  The initial developer of the original code is David A. Hinds
 *  <dahinds@users.sourceforge.net>.  Portions created by David A. Hinds
 *  are Copyright (C) 1999 David A. Hinds.  All Rights Reserved.
 *
 */

#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/spinlock.h>
#include <linux/moduleparam.h>
#include <linux/wait.h>

#include <linux/skbuff.h>
#include <linux/io.h>

#include <pcmcia/cistpl.h>
#include <pcmcia/ciscode.h>
#include <pcmcia/ds.h>
#include <pcmcia/cisreg.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>



/* ======================== Module parameters ======================== */


MODULE_AUTHOR("Marcel Holtmann <marcel@holtmann.org>");
MODULE_DESCRIPTION("Bluetooth driver for the Anycom BlueCard (LSE039/LSE041)");
MODULE_LICENSE("GPL");



/* ======================== Local structures ======================== */


struct bluecard_info {
	struct pcmcia_device *p_dev;

	struct hci_dev *hdev;

	spinlock_t lock;		/* For serializing operations */
	struct timer_list timer;	/* For LED control */

	struct sk_buff_head txq;
	unsigned long tx_state;

	unsigned long rx_state;
	unsigned long rx_count;
	struct sk_buff *rx_skb;

	unsigned char ctrl_reg;
	unsigned long hw_state;		/* Status of the hardware and LED control */
};


static int bluecard_config(struct pcmcia_device *link);
static void bluecard_release(struct pcmcia_device *link);

static void bluecard_detach(struct pcmcia_device *p_dev);


/* Default baud rate: 57600, 115200, 230400 or 460800 */
#define DEFAULT_BAUD_RATE  230400


/* Hardware states */
#define CARD_READY             1
#define CARD_ACTIVITY	       2
#define CARD_HAS_PCCARD_ID     4
#define CARD_HAS_POWER_LED     5
#define CARD_HAS_ACTIVITY_LED  6

/* Transmit states  */
#define XMIT_SENDING         1
#define XMIT_WAKEUP          2
#define XMIT_BUFFER_NUMBER   5	/* unset = buffer one, set = buffer two */
#define XMIT_BUF_ONE_READY   6
#define XMIT_BUF_TWO_READY   7
#define XMIT_SENDING_READY   8

/* Receiver states */
#define RECV_WAIT_PACKET_TYPE   0
#define RECV_WAIT_EVENT_HEADER  1
#define RECV_WAIT_ACL_HEADER    2
#define RECV_WAIT_SCO_HEADER    3
#define RECV_WAIT_DATA          4

/* Special packet types */
#define PKT_BAUD_RATE_57600   0x80
#define PKT_BAUD_RATE_115200  0x81
#define PKT_BAUD_RATE_230400  0x82
#define PKT_BAUD_RATE_460800  0x83


/* These are the register offsets */
#define REG_COMMAND     0x20
#define REG_INTERRUPT   0x21
#define REG_CONTROL     0x22
#define REG_RX_CONTROL  0x24
#define REG_CARD_RESET  0x30
#define REG_LED_CTRL    0x30

/* REG_COMMAND */
#define REG_COMMAND_TX_BUF_ONE  0x01
#define REG_COMMAND_TX_BUF_TWO  0x02
#define REG_COMMAND_RX_BUF_ONE  0x04
#define REG_COMMAND_RX_BUF_TWO  0x08
#define REG_COMMAND_RX_WIN_ONE  0x00
#define REG_COMMAND_RX_WIN_TWO  0x10

/* REG_CONTROL */
#define REG_CONTROL_BAUD_RATE_57600   0x00
#define REG_CONTROL_BAUD_RATE_115200  0x01
#define REG_CONTROL_BAUD_RATE_230400  0x02
#define REG_CONTROL_BAUD_RATE_460800  0x03
#define REG_CONTROL_RTS               0x04
#define REG_CONTROL_BT_ON             0x08
#define REG_CONTROL_BT_RESET          0x10
#define REG_CONTROL_BT_RES_PU         0x20
#define REG_CONTROL_INTERRUPT         0x40
#define REG_CONTROL_CARD_RESET        0x80

/* REG_RX_CONTROL */
#define RTS_LEVEL_SHIFT_BITS  0x02



/* ======================== LED handling routines ======================== */


static void bluecard_activity_led_timeout(struct timer_list *t)
{
	struct bluecard_info *info = from_timer(info, t, timer);
	unsigned int iobase = info->p_dev->resource[0]->start;

	if (test_bit(CARD_ACTIVITY, &(info->hw_state))) {
		/* leave LED in inactive state for HZ/10 for blink effect */
		clear_bit(CARD_ACTIVITY, &(info->hw_state));
		mod_timer(&(info->timer), jiffies + HZ / 10);
	}

	/* Disable activity LED, enable power LED */
	outb(0x08 | 0x20, iobase + 0x30);
}


static void bluecard_enable_activity_led(struct bluecard_info *info)
{
	unsigned int iobase = info->p_dev->resource[0]->start;

	/* don't disturb running blink timer */
	if (timer_pending(&(info->timer)))
		return;

	set_bit(CARD_ACTIVITY, &(info->hw_state));

	if (test_bit(CARD_HAS_ACTIVITY_LED, &(info->hw_state))) {
		/* Enable activity LED, keep power LED enabled */
		outb(0x18 | 0x60, iobase + 0x30);
	} else {
		/* Disable power LED */
		outb(0x00, iobase + 0x30);
	}

	/* Stop the LED after HZ/10 */
	mod_timer(&(info->timer), jiffies + HZ / 10);
}



/* ======================== Interrupt handling ======================== */


static int bluecard_write(unsigned int iobase, unsigned int offset, __u8 *buf, int len)
{
	int i, actual;

	actual = (len > 15) ? 15 : len;

	outb_p(actual, iobase + offset);

	for (i = 0; i < actual; i++)
		outb_p(buf[i], iobase + offset + i + 1);

	return actual;
}


static void bluecard_write_wakeup(struct bluecard_info *info)
{
	if (!info) {
		BT_ERR("Unknown device");
		return;
	}

	if (!test_bit(XMIT_SENDING_READY, &(info->tx_state)))
		return;

	if (test_and_set_bit(XMIT_SENDING, &(info->tx_state))) {
		set_bit(XMIT_WAKEUP, &(info->tx_state));
		return;
	}

	do {
		unsigned int iobase = info->p_dev->resource[0]->start;
		unsigned int offset;
		unsigned char command;
		unsigned long ready_bit;
		register struct sk_buff *skb;
		int len;

		clear_bit(XMIT_WAKEUP, &(info->tx_state));

		if (!pcmcia_dev_present(info->p_dev))
			return;

		if (test_bit(XMIT_BUFFER_NUMBER, &(info->tx_state))) {
			if (!test_bit(XMIT_BUF_TWO_READY, &(info->tx_state)))
				break;
			offset = 0x10;
			command = REG_COMMAND_TX_BUF_TWO;
			ready_bit = XMIT_BUF_TWO_READY;
		} else {
			if (!test_bit(XMIT_BUF_ONE_READY, &(info->tx_state)))
				break;
			offset = 0x00;
			command = REG_COMMAND_TX_BUF_ONE;
			ready_bit = XMIT_BUF_ONE_READY;
		}

		skb = skb_dequeue(&(info->txq));
		if (!skb)
			break;

		if (hci_skb_pkt_type(skb) & 0x80) {
			/* Disable RTS */
			info->ctrl_reg |= REG_CONTROL_RTS;
			outb(info->ctrl_reg, iobase + REG_CONTROL);
		}

		/* Activate LED */
		bluecard_enable_activity_led(info);

		/* Send frame */
		len = bluecard_write(iobase, offset, skb->data, skb->len);

		/* Tell the FPGA to send the data */
		outb_p(command, iobase + REG_COMMAND);

		/* Mark the buffer as dirty */
		clear_bit(ready_bit, &(info->tx_state));

		if (hci_skb_pkt_type(skb) & 0x80) {
			DECLARE_WAIT_QUEUE_HEAD_ONSTACK(wq);
			DEFINE_WAIT(wait);

			unsigned char baud_reg;

			switch (hci_skb_pkt_type(skb)) {
			case PKT_BAUD_RATE_460800:
				baud_reg = REG_CONTROL_BAUD_RATE_460800;
				break;
			case PKT_BAUD_RATE_230400:
				baud_reg = REG_CONTROL_BAUD_RATE_230400;
				break;
			case PKT_BAUD_RATE_115200:
				baud_reg = REG_CONTROL_BAUD_RATE_115200;
				break;
			case PKT_BAUD_RATE_57600:
			default:
				baud_reg = REG_CONTROL_BAUD_RATE_57600;
				break;
			}

			/* Wait until the command reaches the baseband */
			mdelay(100);

			/* Set baud on baseband */
			info->ctrl_reg &= ~0x03;
			info->ctrl_reg |= baud_reg;
			outb(info->ctrl_reg, iobase + REG_CONTROL);

			/* Enable RTS */
			info->ctrl_reg &= ~REG_CONTROL_RTS;
			outb(info->ctrl_reg, iobase + REG_CONTROL);

			/* Wait before the next HCI packet can be send */
			mdelay(1000);
		}

		if (len == skb->len) {
			kfree_skb(skb);
		} else {
			skb_pull(skb, len);
			skb_queue_head(&(info->txq), skb);
		}

		info->hdev->stat.byte_tx += len;

		/* Change buffer */
		change_bit(XMIT_BUFFER_NUMBER, &(info->tx_state));

	} while (test_bit(XMIT_WAKEUP, &(info->tx_state)));

	clear_bit(XMIT_SENDING, &(info->tx_state));
}


static int bluecard_read(unsigned int iobase, unsigned int offset, __u8 *buf, int size)
{
	int i, n, len;

	outb(REG_COMMAND_RX_WIN_ONE, iobase + REG_COMMAND);

	len = inb(iobase + offset);
	n = 0;
	i = 1;

	while (n < len) {

		if (i == 16) {
			outb(REG_COMMAND_RX_WIN_TWO, iobase + REG_COMMAND);
			i = 0;
		}

		buf[n] = inb(iobase + offset + i);

		n++;
		i++;

	}

	return len;
}


static void bluecard_receive(struct bluecard_info *info,
			     unsigned int offset)
{
	unsigned int iobase;
	unsigned char buf[31];
	int i, len;

	if (!info) {
		BT_ERR("Unknown device");
		return;
	}

	iobase = info->p_dev->resource[0]->start;

	if (test_bit(XMIT_SENDING_READY, &(info->tx_state)))
		bluecard_enable_activity_led(info);

	len = bluecard_read(iobase, offset, buf, sizeof(buf));

	for (i = 0; i < len; i++) {

		/* Allocate packet */
		if (!info->rx_skb) {
			info->rx_state = RECV_WAIT_PACKET_TYPE;
			info->rx_count = 0;
			info->rx_skb = bt_skb_alloc(HCI_MAX_FRAME_SIZE, GFP_ATOMIC);
			if (!info->rx_skb) {
				BT_ERR("Can't allocate mem for new packet");
				return;
			}
		}

		if (info->rx_state == RECV_WAIT_PACKET_TYPE) {

			hci_skb_pkt_type(info->rx_skb) = buf[i];

			switch (hci_skb_pkt_type(info->rx_skb)) {

			case 0x00:
				/* init packet */
				if (offset != 0x00) {
					set_bit(XMIT_BUF_ONE_READY, &(info->tx_state));
					set_bit(XMIT_BUF_TWO_READY, &(info->tx_state));
					set_bit(XMIT_SENDING_READY, &(info->tx_state));
					bluecard_write_wakeup(info);
				}

				kfree_skb(info->rx_skb);
				info->rx_skb = NULL;
				break;

			case HCI_EVENT_PKT:
				info->rx_state = RECV_WAIT_EVENT_HEADER;
				info->rx_count = HCI_EVENT_HDR_SIZE;
				break;

			case HCI_ACLDATA_PKT:
				info->rx_state = RECV_WAIT_ACL_HEADER;
				info->rx_count = HCI_ACL_HDR_SIZE;
				break;

			case HCI_SCODATA_PKT:
				info->rx_state = RECV_WAIT_SCO_HEADER;
				info->rx_count = HCI_SCO_HDR_SIZE;
				break;

			default:
				/* unknown packet */
				BT_ERR("Unknown HCI packet with type 0x%02x received",
				       hci_skb_pkt_type(info->rx_skb));
				info->hdev->stat.err_rx++;

				kfree_skb(info->rx_skb);
				info->rx_skb = NULL;
				break;

			}

		} else {

			skb_put_u8(info->rx_skb, buf[i]);
			info->rx_count--;

			if (info->rx_count == 0) {

				int dlen;
				struct hci_event_hdr *eh;
				struct hci_acl_hdr *ah;
				struct hci_sco_hdr *sh;

				switch (info->rx_state) {

				case RECV_WAIT_EVENT_HEADER:
					eh = hci_event_hdr(info->rx_skb);
					info->rx_state = RECV_WAIT_DATA;
					info->rx_count = eh->plen;
					break;

				case RECV_WAIT_ACL_HEADER:
					ah = hci_acl_hdr(info->rx_skb);
					dlen = __le16_to_cpu(ah->dlen);
					info->rx_state = RECV_WAIT_DATA;
					info->rx_count = dlen;
					break;

				case RECV_WAIT_SCO_HEADER:
					sh = hci_sco_hdr(info->rx_skb);
					info->rx_state = RECV_WAIT_DATA;
					info->rx_count = sh->dlen;
					break;

				case RECV_WAIT_DATA:
					hci_recv_frame(info->hdev, info->rx_skb);
					info->rx_skb = NULL;
					break;

				}

			}

		}


	}

	info->hdev->stat.byte_rx += len;
}


static irqreturn_t bluecard_interrupt(int irq, void *dev_inst)
{
	struct bluecard_info *info = dev_inst;
	unsigned int iobase;
	unsigned char reg;

	if (!info || !info->hdev)
		/* our irq handler is shared */
		return IRQ_NONE;

	if (!test_bit(CARD_READY, &(info->hw_state)))
		return IRQ_HANDLED;

	iobase = info->p_dev->resource[0]->start;

	spin_lock(&(info->lock));

	/* Disable interrupt */
	info->ctrl_reg &= ~REG_CONTROL_INTERRUPT;
	outb(info->ctrl_reg, iobase + REG_CONTROL);

	reg = inb(iobase + REG_INTERRUPT);

	if ((reg != 0x00) && (reg != 0xff)) {

		if (reg & 0x04) {
			bluecard_receive(info, 0x00);
			outb(0x04, iobase + REG_INTERRUPT);
			outb(REG_COMMAND_RX_BUF_ONE, iobase + REG_COMMAND);
		}

		if (reg & 0x08) {
			bluecard_receive(info, 0x10);
			outb(0x08, iobase + REG_INTERRUPT);
			outb(REG_COMMAND_RX_BUF_TWO, iobase + REG_COMMAND);
		}

		if (reg & 0x01) {
			set_bit(XMIT_BUF_ONE_READY, &(info->tx_state));
			outb(0x01, iobase + REG_INTERRUPT);
			bluecard_write_wakeup(info);
		}

		if (reg & 0x02) {
			set_bit(XMIT_BUF_TWO_READY, &(info->tx_state));
			outb(0x02, iobase + REG_INTERRUPT);
			bluecard_write_wakeup(info);
		}

	}

	/* Enable interrupt */
	info->ctrl_reg |= REG_CONTROL_INTERRUPT;
	outb(info->ctrl_reg, iobase + REG_CONTROL);

	spin_unlock(&(info->lock));

	return IRQ_HANDLED;
}



/* ======================== Device specific HCI commands ======================== */


static int bluecard_hci_set_baud_rate(struct hci_dev *hdev, int baud)
{
	struct bluecard_info *info = hci_get_drvdata(hdev);
	struct sk_buff *skb;

	/* Ericsson baud rate command */
	unsigned char cmd[] = { HCI_COMMAND_PKT, 0x09, 0xfc, 0x01, 0x03 };

	skb = bt_skb_alloc(HCI_MAX_FRAME_SIZE, GFP_KERNEL);
	if (!skb) {
		BT_ERR("Can't allocate mem for new packet");
		return -1;
	}

	switch (baud) {
	case 460800:
		cmd[4] = 0x00;
		hci_skb_pkt_type(skb) = PKT_BAUD_RATE_460800;
		break;
	case 230400:
		cmd[4] = 0x01;
		hci_skb_pkt_type(skb) = PKT_BAUD_RATE_230400;
		break;
	case 115200:
		cmd[4] = 0x02;
		hci_skb_pkt_type(skb) = PKT_BAUD_RATE_115200;
		break;
	case 57600:
	default:
		cmd[4] = 0x03;
		hci_skb_pkt_type(skb) = PKT_BAUD_RATE_57600;
		break;
	}

	skb_put_data(skb, cmd, sizeof(cmd));

	skb_queue_tail(&(info->txq), skb);

	bluecard_write_wakeup(info);

	return 0;
}



/* ======================== HCI interface ======================== */


static int bluecard_hci_flush(struct hci_dev *hdev)
{
	struct bluecard_info *info = hci_get_drvdata(hdev);

	/* Drop TX queue */
	skb_queue_purge(&(info->txq));

	return 0;
}


static int bluecard_hci_open(struct hci_dev *hdev)
{
	struct bluecard_info *info = hci_get_drvdata(hdev);
	unsigned int iobase = info->p_dev->resource[0]->start;

	if (test_bit(CARD_HAS_PCCARD_ID, &(info->hw_state)))
		bluecard_hci_set_baud_rate(hdev, DEFAULT_BAUD_RATE);

	/* Enable power LED */
	outb(0x08 | 0x20, iobase + 0x30);

	return 0;
}


static int bluecard_hci_close(struct hci_dev *hdev)
{
	struct bluecard_info *info = hci_get_drvdata(hdev);
	unsigned int iobase = info->p_dev->resource[0]->start;

	bluecard_hci_flush(hdev);

	/* Stop LED timer */
	del_timer_sync(&(info->timer));

	/* Disable power LED */
	outb(0x00, iobase + 0x30);

	return 0;
}


static int bluecard_hci_send_frame(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct bluecard_info *info = hci_get_drvdata(hdev);

	switch (hci_skb_pkt_type(skb)) {
	case HCI_COMMAND_PKT:
		hdev->stat.cmd_tx++;
		break;
	case HCI_ACLDATA_PKT:
		hdev->stat.acl_tx++;
		break;
	case HCI_SCODATA_PKT:
		hdev->stat.sco_tx++;
		break;
	}

	/* Prepend skb with frame type */
	memcpy(skb_push(skb, 1), &hci_skb_pkt_type(skb), 1);
	skb_queue_tail(&(info->txq), skb);

	bluecard_write_wakeup(info);

	return 0;
}



/* ======================== Card services HCI interaction ======================== */


static int bluecard_open(struct bluecard_info *info)
{
	unsigned int iobase = info->p_dev->resource[0]->start;
	struct hci_dev *hdev;
	unsigned char id;

	spin_lock_init(&(info->lock));

	timer_setup(&info->timer, bluecard_activity_led_timeout, 0);

	skb_queue_head_init(&(info->txq));

	info->rx_state = RECV_WAIT_PACKET_TYPE;
	info->rx_count = 0;
	info->rx_skb = NULL;

	/* Initialize HCI device */
	hdev = hci_alloc_dev();
	if (!hdev) {
		BT_ERR("Can't allocate HCI device");
		return -ENOMEM;
	}

	info->hdev = hdev;

	hdev->bus = HCI_PCCARD;
	hci_set_drvdata(hdev, info);
	SET_HCIDEV_DEV(hdev, &info->p_dev->dev);

	hdev->open  = bluecard_hci_open;
	hdev->close = bluecard_hci_close;
	hdev->flush = bluecard_hci_flush;
	hdev->send  = bluecard_hci_send_frame;

	id = inb(iobase + 0x30);

	if ((id & 0x0f) == 0x02)
		set_bit(CARD_HAS_PCCARD_ID, &(info->hw_state));

	if (id & 0x10)
		set_bit(CARD_HAS_POWER_LED, &(info->hw_state));

	if (id & 0x20)
		set_bit(CARD_HAS_ACTIVITY_LED, &(info->hw_state));

	/* Reset card */
	info->ctrl_reg = REG_CONTROL_BT_RESET | REG_CONTROL_CARD_RESET;
	outb(info->ctrl_reg, iobase + REG_CONTROL);

	/* Turn FPGA off */
	outb(0x80, iobase + 0x30);

	/* Wait some time */
	msleep(10);

	/* Turn FPGA on */
	outb(0x00, iobase + 0x30);

	/* Activate card */
	info->ctrl_reg = REG_CONTROL_BT_ON | REG_CONTROL_BT_RES_PU;
	outb(info->ctrl_reg, iobase + REG_CONTROL);

	/* Enable interrupt */
	outb(0xff, iobase + REG_INTERRUPT);
	info->ctrl_reg |= REG_CONTROL_INTERRUPT;
	outb(info->ctrl_reg, iobase + REG_CONTROL);

	if ((id & 0x0f) == 0x03) {
		/* Disable RTS */
		info->ctrl_reg |= REG_CONTROL_RTS;
		outb(info->ctrl_reg, iobase + REG_CONTROL);

		/* Set baud rate */
		info->ctrl_reg |= 0x03;
		outb(info->ctrl_reg, iobase + REG_CONTROL);

		/* Enable RTS */
		info->ctrl_reg &= ~REG_CONTROL_RTS;
		outb(info->ctrl_reg, iobase + REG_CONTROL);

		set_bit(XMIT_BUF_ONE_READY, &(info->tx_state));
		set_bit(XMIT_BUF_TWO_READY, &(info->tx_state));
		set_bit(XMIT_SENDING_READY, &(info->tx_state));
	}

	/* Start the RX buffers */
	outb(REG_COMMAND_RX_BUF_ONE, iobase + REG_COMMAND);
	outb(REG_COMMAND_RX_BUF_TWO, iobase + REG_COMMAND);

	/* Signal that the hardware is ready */
	set_bit(CARD_READY, &(info->hw_state));

	/* Drop TX queue */
	skb_queue_purge(&(info->txq));

	/* Control the point at which RTS is enabled */
	outb((0x0f << RTS_LEVEL_SHIFT_BITS) | 1, iobase + REG_RX_CONTROL);

	/* Timeout before it is safe to send the first HCI packet */
	msleep(1250);

	/* Register HCI device */
	if (hci_register_dev(hdev) < 0) {
		BT_ERR("Can't register HCI device");
		info->hdev = NULL;
		hci_free_dev(hdev);
		return -ENODEV;
	}

	return 0;
}


static int bluecard_close(struct bluecard_info *info)
{
	unsigned int iobase = info->p_dev->resource[0]->start;
	struct hci_dev *hdev = info->hdev;

	if (!hdev)
		return -ENODEV;

	bluecard_hci_close(hdev);

	clear_bit(CARD_READY, &(info->hw_state));

	/* Reset card */
	info->ctrl_reg = REG_CONTROL_BT_RESET | REG_CONTROL_CARD_RESET;
	outb(info->ctrl_reg, iobase + REG_CONTROL);

	/* Turn FPGA off */
	outb(0x80, iobase + 0x30);

	hci_unregister_dev(hdev);
	hci_free_dev(hdev);

	return 0;
}

static int bluecard_probe(struct pcmcia_device *link)
{
	struct bluecard_info *info;

	/* Create new info device */
	info = devm_kzalloc(&link->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->p_dev = link;
	link->priv = info;

	link->config_flags |= CONF_ENABLE_IRQ;

	return bluecard_config(link);
}


static void bluecard_detach(struct pcmcia_device *link)
{
	bluecard_release(link);
}


static int bluecard_config(struct pcmcia_device *link)
{
	struct bluecard_info *info = link->priv;
	int i, n;

	link->config_index = 0x20;

	link->resource[0]->flags |= IO_DATA_PATH_WIDTH_8;
	link->resource[0]->end = 64;
	link->io_lines = 6;

	for (n = 0; n < 0x400; n += 0x40) {
		link->resource[0]->start = n ^ 0x300;
		i = pcmcia_request_io(link);
		if (i == 0)
			break;
	}

	if (i != 0)
		goto failed;

	i = pcmcia_request_irq(link, bluecard_interrupt);
	if (i != 0)
		goto failed;

	i = pcmcia_enable_device(link);
	if (i != 0)
		goto failed;

	if (bluecard_open(info) != 0)
		goto failed;

	return 0;

failed:
	bluecard_release(link);
	return -ENODEV;
}


static void bluecard_release(struct pcmcia_device *link)
{
	struct bluecard_info *info = link->priv;

	bluecard_close(info);

	del_timer_sync(&(info->timer));

	pcmcia_disable_device(link);
}

static const struct pcmcia_device_id bluecard_ids[] = {
	PCMCIA_DEVICE_PROD_ID12("BlueCard", "LSE041", 0xbaf16fbf, 0x657cc15e),
	PCMCIA_DEVICE_PROD_ID12("BTCFCARD", "LSE139", 0xe3987764, 0x2524b59c),
	PCMCIA_DEVICE_PROD_ID12("WSS", "LSE039", 0x0a0736ec, 0x24e6dfab),
	PCMCIA_DEVICE_NULL
};
MODULE_DEVICE_TABLE(pcmcia, bluecard_ids);

static struct pcmcia_driver bluecard_driver = {
	.owner		= THIS_MODULE,
	.name		= "bluecard_cs",
	.probe		= bluecard_probe,
	.remove		= bluecard_detach,
	.id_table	= bluecard_ids,
};
module_pcmcia_driver(bluecard_driver);
