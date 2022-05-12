/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * drivers/net/ethernet/ibm/emac/debug.h
 *
 * Driver for PowerPC 4xx on-chip ethernet controller, debug print routines.
 *
 * Copyright 2007 Benjamin Herrenschmidt, IBM Corp.
 *                <benh@kernel.crashing.org>
 *
 * Based on the arch/ppc version of the driver:
 *
 * Copyright (c) 2004, 2005 Zultys Technologies
 * Eugene Surovegin <eugene.surovegin@zultys.com> or <ebs@ebshome.net>
 */
#ifndef __IBM_NEWEMAC_DEBUG_H
#define __IBM_NEWEMAC_DEBUG_H

#include <linux/init.h>

#include "core.h"

#if defined(CONFIG_IBM_EMAC_DEBUG)
# define DBG_LEVEL		1
#else
# define DBG_LEVEL		0
#endif

#define EMAC_DBG(d, name, fmt, arg...) \
	printk(KERN_DEBUG #name "%pOF: " fmt, d->ofdev->dev.of_node, ## arg)

#if DBG_LEVEL > 0
#  define DBG(d,f,x...)		EMAC_DBG(d, emac, f, ##x)
#  define MAL_DBG(d,f,x...)	EMAC_DBG(d, mal, f, ##x)
#  define ZMII_DBG(d,f,x...)	EMAC_DBG(d, zmii, f, ##x)
#  define RGMII_DBG(d,f,x...)	EMAC_DBG(d, rgmii, f, ##x)
#  define NL			"\n"
#else
#  define DBG(f,x...)		((void)0)
#  define MAL_DBG(d,f,x...)	((void)0)
#  define ZMII_DBG(d,f,x...)	((void)0)
#  define RGMII_DBG(d,f,x...)	((void)0)
#endif
#if DBG_LEVEL > 1
#  define DBG2(d,f,x...) 	DBG(d,f, ##x)
#  define MAL_DBG2(d,f,x...) 	MAL_DBG(d,f, ##x)
#  define ZMII_DBG2(d,f,x...) 	ZMII_DBG(d,f, ##x)
#  define RGMII_DBG2(d,f,x...) 	RGMII_DBG(d,f, ##x)
#else
#  define DBG2(f,x...) 		((void)0)
#  define MAL_DBG2(d,f,x...) 	((void)0)
#  define ZMII_DBG2(d,f,x...) 	((void)0)
#  define RGMII_DBG2(d,f,x...) 	((void)0)
#endif

#endif /* __IBM_NEWEMAC_DEBUG_H */
