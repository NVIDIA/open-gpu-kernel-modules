/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Intel(R) Trace Hub Software Trace Hub (STH) data structures
 *
 * Copyright (C) 2014-2015 Intel Corporation.
 */

#ifndef __INTEL_TH_STH_H__
#define __INTEL_TH_STH_H__

enum {
	REG_STH_STHCAP0		= 0x0000, /* capabilities pt1 */
	REG_STH_STHCAP1		= 0x0004, /* capabilities pt2 */
	REG_STH_TRIG		= 0x0008, /* TRIG packet payload */
	REG_STH_TRIG_TS		= 0x000c, /* TRIG_TS packet payload */
	REG_STH_XSYNC		= 0x0010, /* XSYNC packet payload */
	REG_STH_XSYNC_TS	= 0x0014, /* XSYNC_TS packet payload */
	REG_STH_GERR		= 0x0018, /* GERR packet payload */
};

struct intel_th_channel {
	u64	Dn;
	u64	DnM;
	u64	DnTS;
	u64	DnMTS;
	u64	USER;
	u64	USER_TS;
	u32	FLAG;
	u32	FLAG_TS;
	u32	MERR;
	u32	__unused;
} __packed;

#endif /* __INTEL_TH_STH_H__ */
