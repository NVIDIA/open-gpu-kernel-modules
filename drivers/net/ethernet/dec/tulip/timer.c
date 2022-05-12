/*
	drivers/net/ethernet/dec/tulip/timer.c

	Copyright 2000,2001  The Linux Kernel Team
	Written/copyright 1994-2001 by Donald Becker.

	This software may be used and distributed according to the terms
	of the GNU General Public License, incorporated herein by reference.

	Please submit bugs to http://bugzilla.kernel.org/ .
*/


#include "tulip.h"


void tulip_media_task(struct work_struct *work)
{
	struct tulip_private *tp =
		container_of(work, struct tulip_private, media_work);
	struct net_device *dev = tp->dev;
	void __iomem *ioaddr = tp->base_addr;
	u32 csr12 = ioread32(ioaddr + CSR12);
	int next_tick = 2*HZ;
	unsigned long flags;

	if (tulip_debug > 2) {
		netdev_dbg(dev, "Media selection tick, %s, status %08x mode %08x SIA %08x %08x %08x %08x\n",
			   medianame[dev->if_port],
			   ioread32(ioaddr + CSR5), ioread32(ioaddr + CSR6),
			   csr12, ioread32(ioaddr + CSR13),
			   ioread32(ioaddr + CSR14), ioread32(ioaddr + CSR15));
	}
	switch (tp->chip_id) {
	case DC21140:
	case DC21142:
	case MX98713:
	case COMPEX9881:
	case DM910X:
	default: {
		struct medialeaf *mleaf;
		unsigned char *p;
		if (tp->mtable == NULL) {	/* No EEPROM info, use generic code. */
			/* Not much that can be done.
			   Assume this a generic MII or SYM transceiver. */
			next_tick = 60*HZ;
			if (tulip_debug > 2)
				netdev_dbg(dev, "network media monitor CSR6 %08x CSR12 0x%02x\n",
					   ioread32(ioaddr + CSR6),
					   csr12 & 0xff);
			break;
		}
		mleaf = &tp->mtable->mleaf[tp->cur_index];
		p = mleaf->leafdata;
		switch (mleaf->type) {
		case 0: case 4: {
			/* Type 0 serial or 4 SYM transceiver.  Check the link beat bit. */
			int offset = mleaf->type == 4 ? 5 : 2;
			s8 bitnum = p[offset];
			if (p[offset+1] & 0x80) {
				if (tulip_debug > 1)
					netdev_dbg(dev, "Transceiver monitor tick CSR12=%#02x, no media sense\n",
						   csr12);
				if (mleaf->type == 4) {
					if (mleaf->media == 3 && (csr12 & 0x02))
						goto select_next_media;
				}
				break;
			}
			if (tulip_debug > 2)
				netdev_dbg(dev, "Transceiver monitor tick: CSR12=%#02x bit %d is %d, expecting %d\n",
					   csr12, (bitnum >> 1) & 7,
					   (csr12 & (1 << ((bitnum >> 1) & 7))) != 0,
					   (bitnum >= 0));
			/* Check that the specified bit has the proper value. */
			if ((bitnum < 0) !=
				((csr12 & (1 << ((bitnum >> 1) & 7))) != 0)) {
				if (tulip_debug > 2)
					netdev_dbg(dev, "Link beat detected for %s\n",
						   medianame[mleaf->media & MEDIA_MASK]);
				if ((p[2] & 0x61) == 0x01)	/* Bogus Znyx board. */
					goto actually_mii;
				netif_carrier_on(dev);
				break;
			}
			netif_carrier_off(dev);
			if (tp->medialock)
				break;
	  select_next_media:
			if (--tp->cur_index < 0) {
				/* We start again, but should instead look for default. */
				tp->cur_index = tp->mtable->leafcount - 1;
			}
			dev->if_port = tp->mtable->mleaf[tp->cur_index].media;
			if (tulip_media_cap[dev->if_port] & MediaIsFD)
				goto select_next_media; /* Skip FD entries. */
			if (tulip_debug > 1)
				netdev_dbg(dev, "No link beat on media %s, trying transceiver type %s\n",
					   medianame[mleaf->media & MEDIA_MASK],
					   medianame[tp->mtable->mleaf[tp->cur_index].media]);
			tulip_select_media(dev, 0);
			/* Restart the transmit process. */
			tulip_restart_rxtx(tp);
			next_tick = (24*HZ)/10;
			break;
		}
		case 1:  case 3:		/* 21140, 21142 MII */
		actually_mii:
			if (tulip_check_duplex(dev) < 0) {
				netif_carrier_off(dev);
				next_tick = 3*HZ;
			} else {
				netif_carrier_on(dev);
				next_tick = 60*HZ;
			}
			break;
		case 2:					/* 21142 serial block has no link beat. */
		default:
			break;
		}
	}
	break;
	}


	spin_lock_irqsave(&tp->lock, flags);
	if (tp->timeout_recovery) {
		tulip_tx_timeout_complete(tp, ioaddr);
		tp->timeout_recovery = 0;
	}
	spin_unlock_irqrestore(&tp->lock, flags);

	/* mod_timer synchronizes us with potential add_timer calls
	 * from interrupts.
	 */
	mod_timer(&tp->timer, RUN_AT(next_tick));
}


void mxic_timer(struct timer_list *t)
{
	struct tulip_private *tp = from_timer(tp, t, timer);
	struct net_device *dev = tp->dev;
	void __iomem *ioaddr = tp->base_addr;
	int next_tick = 60*HZ;

	if (tulip_debug > 3) {
		dev_info(&dev->dev, "MXIC negotiation status %08x\n",
			 ioread32(ioaddr + CSR12));
	}
	if (next_tick) {
		mod_timer(&tp->timer, RUN_AT(next_tick));
	}
}


void comet_timer(struct timer_list *t)
{
	struct tulip_private *tp = from_timer(tp, t, timer);
	struct net_device *dev = tp->dev;
	int next_tick = 2*HZ;

	if (tulip_debug > 1)
		netdev_dbg(dev, "Comet link status %04x partner capability %04x\n",
			   tulip_mdio_read(dev, tp->phys[0], 1),
			   tulip_mdio_read(dev, tp->phys[0], 5));
	/* mod_timer synchronizes us with potential add_timer calls
	 * from interrupts.
	 */
	if (tulip_check_duplex(dev) < 0)
		{ netif_carrier_off(dev); }
	else
		{ netif_carrier_on(dev); }
	mod_timer(&tp->timer, RUN_AT(next_tick));
}

