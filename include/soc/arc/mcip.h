/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ARConnect IP Support (Multi core enabler: Cross core IPI, RTC ...)
 *
 * Copyright (C) 2014-15 Synopsys, Inc. (www.synopsys.com)
 */

#ifndef __SOC_ARC_MCIP_H
#define __SOC_ARC_MCIP_H

#include <soc/arc/aux.h>

#define ARC_REG_MCIP_BCR	0x0d0
#define ARC_REG_MCIP_IDU_BCR	0x0D5
#define ARC_REG_GFRC_BUILD	0x0D6
#define ARC_REG_MCIP_CMD	0x600
#define ARC_REG_MCIP_WDATA	0x601
#define ARC_REG_MCIP_READBACK	0x602

struct mcip_cmd {
#ifdef CONFIG_CPU_BIG_ENDIAN
	unsigned int pad:8, param:16, cmd:8;
#else
	unsigned int cmd:8, param:16, pad:8;
#endif

#define CMD_INTRPT_GENERATE_IRQ		0x01
#define CMD_INTRPT_GENERATE_ACK		0x02
#define CMD_INTRPT_READ_STATUS		0x03
#define CMD_INTRPT_CHECK_SOURCE		0x04

/* Semaphore Commands */
#define CMD_SEMA_CLAIM_AND_READ		0x11
#define CMD_SEMA_RELEASE		0x12

#define CMD_DEBUG_SET_MASK		0x34
#define CMD_DEBUG_READ_MASK		0x35
#define CMD_DEBUG_SET_SELECT		0x36
#define CMD_DEBUG_READ_SELECT		0x37

#define CMD_GFRC_READ_LO		0x42
#define CMD_GFRC_READ_HI		0x43
#define CMD_GFRC_SET_CORE		0x47
#define CMD_GFRC_READ_CORE		0x48

#define CMD_IDU_ENABLE			0x71
#define CMD_IDU_DISABLE			0x72
#define CMD_IDU_SET_MODE		0x74
#define CMD_IDU_READ_MODE		0x75
#define CMD_IDU_SET_DEST		0x76
#define CMD_IDU_ACK_CIRQ		0x79
#define CMD_IDU_SET_MASK		0x7C

#define IDU_M_TRIG_LEVEL		0x0
#define IDU_M_TRIG_EDGE			0x1

#define IDU_M_DISTRI_RR			0x0
#define IDU_M_DISTRI_DEST		0x2
};

struct mcip_bcr {
#ifdef CONFIG_CPU_BIG_ENDIAN
		unsigned int pad4:6, pw_dom:1, pad3:1,
			     idu:1, pad2:1, num_cores:6,
			     pad:1,  gfrc:1, dbg:1, pw:1,
			     msg:1, sem:1, ipi:1, slv:1,
			     ver:8;
#else
		unsigned int ver:8,
			     slv:1, ipi:1, sem:1, msg:1,
			     pw:1, dbg:1, gfrc:1, pad:1,
			     num_cores:6, pad2:1, idu:1,
			     pad3:1, pw_dom:1, pad4:6;
#endif
};

struct mcip_idu_bcr {
#ifdef CONFIG_CPU_BIG_ENDIAN
	unsigned int pad:21, cirqnum:3, ver:8;
#else
	unsigned int ver:8, cirqnum:3, pad:21;
#endif
};


/*
 * Build register for IDU contains not an actual number of supported common
 * interrupts but an exponent of 2 which must be multiplied by 4 to
 * get a number of supported common interrupts.
 */
#define mcip_idu_bcr_to_nr_irqs(bcr) (4 * (1 << (bcr).cirqnum))

/*
 * MCIP programming model
 *
 * - Simple commands write {cmd:8,param:16} to MCIP_CMD aux reg
 *   (param could be irq, common_irq, core_id ...)
 * - More involved commands setup MCIP_WDATA with cmd specific data
 *   before invoking the simple command
 */
static inline void __mcip_cmd(unsigned int cmd, unsigned int param)
{
	struct mcip_cmd buf;

	buf.pad = 0;
	buf.cmd = cmd;
	buf.param = param;

	WRITE_AUX(ARC_REG_MCIP_CMD, buf);
}

/*
 * Setup additional data for a cmd
 * Callers need to lock to ensure atomicity
 */
static inline void __mcip_cmd_data(unsigned int cmd, unsigned int param,
				   unsigned int data)
{
	write_aux_reg(ARC_REG_MCIP_WDATA, data);

	__mcip_cmd(cmd, param);
}

/*
 * Read MCIP register
 */
static inline unsigned int __mcip_cmd_read(unsigned int cmd, unsigned int param)
{
	__mcip_cmd(cmd, param);
	return read_aux_reg(ARC_REG_MCIP_READBACK);
}

#endif
