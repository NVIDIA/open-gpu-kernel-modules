/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * RapidIO devices
 *
 * Copyright 2005 MontaVista Software, Inc.
 * Matt Porter <mporter@kernel.crashing.org>
 */

#ifndef LINUX_RIO_IDS_H
#define LINUX_RIO_IDS_H

#define RIO_VID_FREESCALE		0x0002
#define RIO_DID_MPC8560			0x0003

#define RIO_VID_TUNDRA			0x000d
#define RIO_DID_TSI500			0x0500
#define RIO_DID_TSI568			0x0568
#define RIO_DID_TSI572			0x0572
#define RIO_DID_TSI574			0x0574
#define RIO_DID_TSI576			0x0578 /* Same ID as Tsi578 */
#define RIO_DID_TSI577			0x0577
#define RIO_DID_TSI578			0x0578

#define RIO_VID_IDT			0x0038
#define RIO_DID_IDT70K200		0x0310
#define RIO_DID_IDTCPS8			0x035c
#define RIO_DID_IDTCPS12		0x035d
#define RIO_DID_IDTCPS16		0x035b
#define RIO_DID_IDTCPS6Q		0x035f
#define RIO_DID_IDTCPS10Q		0x035e
#define RIO_DID_IDTCPS1848		0x0374
#define RIO_DID_IDTCPS1432		0x0375
#define RIO_DID_IDTCPS1616		0x0379
#define RIO_DID_IDTVPS1616		0x0377
#define RIO_DID_IDTSPS1616		0x0378
#define RIO_DID_TSI721			0x80ab
#define RIO_DID_IDTRXS1632		0x80e5
#define RIO_DID_IDTRXS2448		0x80e6

#endif				/* LINUX_RIO_IDS_H */
