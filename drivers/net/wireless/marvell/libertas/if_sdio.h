/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  linux/drivers/net/wireless/libertas/if_sdio.h
 *
 *  Copyright 2007 Pierre Ossman
 */

#ifndef _LBS_IF_SDIO_H
#define _LBS_IF_SDIO_H

#define IF_SDIO_IOPORT		0x00

#define IF_SDIO_H_INT_MASK	0x04
#define   IF_SDIO_H_INT_OFLOW	0x08
#define   IF_SDIO_H_INT_UFLOW	0x04
#define   IF_SDIO_H_INT_DNLD	0x02
#define   IF_SDIO_H_INT_UPLD	0x01

#define IF_SDIO_H_INT_STATUS	0x05
#define IF_SDIO_H_INT_RSR	0x06
#define IF_SDIO_H_INT_STATUS2	0x07

#define IF_SDIO_RD_BASE		0x10

#define IF_SDIO_STATUS		0x20
#define   IF_SDIO_IO_RDY	0x08
#define   IF_SDIO_CIS_RDY	0x04
#define   IF_SDIO_UL_RDY	0x02
#define   IF_SDIO_DL_RDY	0x01

#define IF_SDIO_C_INT_MASK	0x24
#define IF_SDIO_C_INT_STATUS	0x28
#define IF_SDIO_C_INT_RSR	0x2C

#define IF_SDIO_SCRATCH		0x34
#define IF_SDIO_SCRATCH_OLD	0x80fe
#define IF_SDIO_FW_STATUS	0x40
#define   IF_SDIO_FIRMWARE_OK	0xfedc

#define IF_SDIO_RX_LEN		0x42
#define IF_SDIO_RX_UNIT		0x43

#define IF_SDIO_EVENT           0x80fc

#define IF_SDIO_BLOCK_SIZE	256
#define CONFIGURATION_REG               0x03
#define HOST_POWER_UP                   (0x1U << 1)
#endif
