/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *	watchdog_core.h
 *
 *	(c) Copyright 2008-2011 Alan Cox <alan@lxorguk.ukuu.org.uk>,
 *						All Rights Reserved.
 *
 *	(c) Copyright 2008-2011 Wim Van Sebroeck <wim@iguana.be>.
 *
 *	This source code is part of the generic code that can be used
 *	by all the watchdog timer drivers.
 *
 *	Based on source code of the following authors:
 *	  Matt Domsch <Matt_Domsch@dell.com>,
 *	  Rob Radez <rob@osinvestor.com>,
 *	  Rusty Lynch <rusty@linux.co.intel.com>
 *	  Satyam Sharma <satyam@infradead.org>
 *	  Randy Dunlap <randy.dunlap@oracle.com>
 *
 *	Neither Alan Cox, CymruNet Ltd., Wim Van Sebroeck nor Iguana vzw.
 *	admit liability nor provide warranty for any of this software.
 *	This material is provided "AS-IS" and at no charge.
 */

#define MAX_DOGS	32	/* Maximum number of watchdog devices */

/*
 *	Functions/procedures to be called by the core
 */
extern int watchdog_dev_register(struct watchdog_device *);
extern void watchdog_dev_unregister(struct watchdog_device *);
extern int __init watchdog_dev_init(void);
extern void __exit watchdog_dev_exit(void);
