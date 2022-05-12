// SPDX-License-Identifier: GPL-2.0
/*
 *  Probe for 8250/16550-type ISAPNP serial ports.
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
 *  Copyright (C) 2001 Russell King, All Rights Reserved.
 *
 *  Ported to the Linux PnP Layer - (C) Adam Belay.
 */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/pnp.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/serial_core.h>
#include <linux/bitops.h>

#include <asm/byteorder.h>

#include "8250.h"

#define UNKNOWN_DEV 0x3000
#define CIR_PORT	0x0800

static const struct pnp_device_id pnp_dev_table[] = {
	/* Archtek America Corp. */
	/* Archtek SmartLink Modem 3334BT Plug & Play */
	{	"AAC000F",		0	},
	/* Anchor Datacomm BV */
	/* SXPro 144 External Data Fax Modem Plug & Play */
	{	"ADC0001",		0	},
	/* SXPro 288 External Data Fax Modem Plug & Play */
	{	"ADC0002",		0	},
	/* PROLiNK 1456VH ISA PnP K56flex Fax Modem */
	{	"AEI0250",		0	},
	/* Actiontec ISA PNP 56K X2 Fax Modem */
	{	"AEI1240",		0	},
	/* Rockwell 56K ACF II Fax+Data+Voice Modem */
	{	"AKY1021",		0 /*SPCI_FL_NO_SHIRQ*/	},
	/*
	 * ALi Fast Infrared Controller
	 * Native driver (ali-ircc) is broken so at least
	 * it can be used with irtty-sir.
	 */
	{	"ALI5123",		0	},
	/* AZT3005 PnP SOUND DEVICE */
	{	"AZT4001",		0	},
	/* Best Data Products Inc. Smart One 336F PnP Modem */
	{	"BDP3336",		0	},
	/*  Boca Research */
	/* Boca Complete Ofc Communicator 14.4 Data-FAX */
	{	"BRI0A49",		0	},
	/* Boca Research 33,600 ACF Modem */
	{	"BRI1400",		0	},
	/* Boca 33.6 Kbps Internal FD34FSVD */
	{	"BRI3400",		0	},
	/* Boca 33.6 Kbps Internal FD34FSVD */
	{	"BRI0A49",		0	},
	/* Best Data Products Inc. Smart One 336F PnP Modem */
	{	"BDP3336",		0	},
	/* Computer Peripherals Inc */
	/* EuroViVa CommCenter-33.6 SP PnP */
	{	"CPI4050",		0	},
	/* Creative Labs */
	/* Creative Labs Phone Blaster 28.8 DSVD PnP Voice */
	{	"CTL3001",		0	},
	/* Creative Labs Modem Blaster 28.8 DSVD PnP Voice */
	{	"CTL3011",		0	},
	/* Davicom ISA 33.6K Modem */
	{	"DAV0336",		0	},
	/* Creative */
	/* Creative Modem Blaster Flash56 DI5601-1 */
	{	"DMB1032",		0	},
	/* Creative Modem Blaster V.90 DI5660 */
	{	"DMB2001",		0	},
	/* E-Tech */
	/* E-Tech CyberBULLET PC56RVP */
	{	"ETT0002",		0	},
	/* FUJITSU */
	/* Fujitsu 33600 PnP-I2 R Plug & Play */
	{	"FUJ0202",		0	},
	/* Fujitsu FMV-FX431 Plug & Play */
	{	"FUJ0205",		0	},
	/* Fujitsu 33600 PnP-I4 R Plug & Play */
	{	"FUJ0206",		0	},
	/* Fujitsu Fax Voice 33600 PNP-I5 R Plug & Play */
	{	"FUJ0209",		0	},
	/* Archtek America Corp. */
	/* Archtek SmartLink Modem 3334BT Plug & Play */
	{	"GVC000F",		0	},
	/* Archtek SmartLink Modem 3334BRV 33.6K Data Fax Voice */
	{	"GVC0303",		0	},
	/* Hayes */
	/* Hayes Optima 288 V.34-V.FC + FAX + Voice Plug & Play */
	{	"HAY0001",		0	},
	/* Hayes Optima 336 V.34 + FAX + Voice PnP */
	{	"HAY000C",		0	},
	/* Hayes Optima 336B V.34 + FAX + Voice PnP */
	{	"HAY000D",		0	},
	/* Hayes Accura 56K Ext Fax Modem PnP */
	{	"HAY5670",		0	},
	/* Hayes Accura 56K Ext Fax Modem PnP */
	{	"HAY5674",		0	},
	/* Hayes Accura 56K Fax Modem PnP */
	{	"HAY5675",		0	},
	/* Hayes 288, V.34 + FAX */
	{	"HAYF000",		0	},
	/* Hayes Optima 288 V.34 + FAX + Voice, Plug & Play */
	{	"HAYF001",		0	},
	/* IBM */
	/* IBM Thinkpad 701 Internal Modem Voice */
	{	"IBM0033",		0	},
	/* Intermec */
	/* Intermec CV60 touchscreen port */
	{	"PNP4972",		0	},
	/* Intertex */
	/* Intertex 28k8 33k6 Voice EXT PnP */
	{	"IXDC801",		0	},
	/* Intertex 33k6 56k Voice EXT PnP */
	{	"IXDC901",		0	},
	/* Intertex 28k8 33k6 Voice SP EXT PnP */
	{	"IXDD801",		0	},
	/* Intertex 33k6 56k Voice SP EXT PnP */
	{	"IXDD901",		0	},
	/* Intertex 28k8 33k6 Voice SP INT PnP */
	{	"IXDF401",		0	},
	/* Intertex 28k8 33k6 Voice SP EXT PnP */
	{	"IXDF801",		0	},
	/* Intertex 33k6 56k Voice SP EXT PnP */
	{	"IXDF901",		0	},
	/* Kortex International */
	/* KORTEX 28800 Externe PnP */
	{	"KOR4522",		0	},
	/* KXPro 33.6 Vocal ASVD PnP */
	{	"KORF661",		0	},
	/* Lasat */
	/* LASAT Internet 33600 PnP */
	{	"LAS4040",		0	},
	/* Lasat Safire 560 PnP */
	{	"LAS4540",		0	},
	/* Lasat Safire 336  PnP */
	{	"LAS5440",		0	},
	/* Microcom, Inc. */
	/* Microcom TravelPorte FAST V.34 Plug & Play */
	{	"MNP0281",		0	},
	/* Microcom DeskPorte V.34 FAST or FAST+ Plug & Play */
	{	"MNP0336",		0	},
	/* Microcom DeskPorte FAST EP 28.8 Plug & Play */
	{	"MNP0339",		0	},
	/* Microcom DeskPorte 28.8P Plug & Play */
	{	"MNP0342",		0	},
	/* Microcom DeskPorte FAST ES 28.8 Plug & Play */
	{	"MNP0500",		0	},
	/* Microcom DeskPorte FAST ES 28.8 Plug & Play */
	{	"MNP0501",		0	},
	/* Microcom DeskPorte 28.8S Internal Plug & Play */
	{	"MNP0502",		0	},
	/* Motorola */
	/* Motorola BitSURFR Plug & Play */
	{	"MOT1105",		0	},
	/* Motorola TA210 Plug & Play */
	{	"MOT1111",		0	},
	/* Motorola HMTA 200 (ISDN) Plug & Play */
	{	"MOT1114",		0	},
	/* Motorola BitSURFR Plug & Play */
	{	"MOT1115",		0	},
	/* Motorola Lifestyle 28.8 Internal */
	{	"MOT1190",		0	},
	/* Motorola V.3400 Plug & Play */
	{	"MOT1501",		0	},
	/* Motorola Lifestyle 28.8 V.34 Plug & Play */
	{	"MOT1502",		0	},
	/* Motorola Power 28.8 V.34 Plug & Play */
	{	"MOT1505",		0	},
	/* Motorola ModemSURFR External 28.8 Plug & Play */
	{	"MOT1509",		0	},
	/* Motorola Premier 33.6 Desktop Plug & Play */
	{	"MOT150A",		0	},
	/* Motorola VoiceSURFR 56K External PnP */
	{	"MOT150F",		0	},
	/* Motorola ModemSURFR 56K External PnP */
	{	"MOT1510",		0	},
	/* Motorola ModemSURFR 56K Internal PnP */
	{	"MOT1550",		0	},
	/* Motorola ModemSURFR Internal 28.8 Plug & Play */
	{	"MOT1560",		0	},
	/* Motorola Premier 33.6 Internal Plug & Play */
	{	"MOT1580",		0	},
	/* Motorola OnlineSURFR 28.8 Internal Plug & Play */
	{	"MOT15B0",		0	},
	/* Motorola VoiceSURFR 56K Internal PnP */
	{	"MOT15F0",		0	},
	/* Com 1 */
	/*  Deskline K56 Phone System PnP */
	{	"MVX00A1",		0	},
	/* PC Rider K56 Phone System PnP */
	{	"MVX00F2",		0	},
	/* NEC 98NOTE SPEAKER PHONE FAX MODEM(33600bps) */
	{	"nEC8241",		0	},
	/* Pace 56 Voice Internal Plug & Play Modem */
	{	"PMC2430",		0	},
	/* Generic */
	/* Generic standard PC COM port	 */
	{	"PNP0500",		0	},
	/* Generic 16550A-compatible COM port */
	{	"PNP0501",		0	},
	/* Compaq 14400 Modem */
	{	"PNPC000",		0	},
	/* Compaq 2400/9600 Modem */
	{	"PNPC001",		0	},
	/* Dial-Up Networking Serial Cable between 2 PCs */
	{	"PNPC031",		0	},
	/* Dial-Up Networking Parallel Cable between 2 PCs */
	{	"PNPC032",		0	},
	/* Standard 9600 bps Modem */
	{	"PNPC100",		0	},
	/* Standard 14400 bps Modem */
	{	"PNPC101",		0	},
	/*  Standard 28800 bps Modem*/
	{	"PNPC102",		0	},
	/*  Standard Modem*/
	{	"PNPC103",		0	},
	/*  Standard 9600 bps Modem*/
	{	"PNPC104",		0	},
	/*  Standard 14400 bps Modem*/
	{	"PNPC105",		0	},
	/*  Standard 28800 bps Modem*/
	{	"PNPC106",		0	},
	/*  Standard Modem */
	{	"PNPC107",		0	},
	/* Standard 9600 bps Modem */
	{	"PNPC108",		0	},
	/* Standard 14400 bps Modem */
	{	"PNPC109",		0	},
	/* Standard 28800 bps Modem */
	{	"PNPC10A",		0	},
	/* Standard Modem */
	{	"PNPC10B",		0	},
	/* Standard 9600 bps Modem */
	{	"PNPC10C",		0	},
	/* Standard 14400 bps Modem */
	{	"PNPC10D",		0	},
	/* Standard 28800 bps Modem */
	{	"PNPC10E",		0	},
	/* Standard Modem */
	{	"PNPC10F",		0	},
	/* Standard PCMCIA Card Modem */
	{	"PNP2000",		0	},
	/* Rockwell */
	/* Modular Technology */
	/* Rockwell 33.6 DPF Internal PnP */
	/* Modular Technology 33.6 Internal PnP */
	{	"ROK0030",		0	},
	/* Kortex International */
	/* KORTEX 14400 Externe PnP */
	{	"ROK0100",		0	},
	/* Rockwell 28.8 */
	{	"ROK4120",		0	},
	/* Viking Components, Inc */
	/* Viking 28.8 INTERNAL Fax+Data+Voice PnP */
	{	"ROK4920",		0	},
	/* Rockwell */
	/* British Telecom */
	/* Modular Technology */
	/* Rockwell 33.6 DPF External PnP */
	/* BT Prologue 33.6 External PnP */
	/* Modular Technology 33.6 External PnP */
	{	"RSS00A0",		0	},
	/* Viking 56K FAX INT */
	{	"RSS0262",		0	},
	/* K56 par,VV,Voice,Speakphone,AudioSpan,PnP */
	{       "RSS0250",              0       },
	/* SupraExpress 28.8 Data/Fax PnP modem */
	{	"SUP1310",		0	},
	/* SupraExpress 336i PnP Voice Modem */
	{	"SUP1381",		0	},
	/* SupraExpress 33.6 Data/Fax PnP modem */
	{	"SUP1421",		0	},
	/* SupraExpress 33.6 Data/Fax PnP modem */
	{	"SUP1590",		0	},
	/* SupraExpress 336i Sp ASVD */
	{	"SUP1620",		0	},
	/* SupraExpress 33.6 Data/Fax PnP modem */
	{	"SUP1760",		0	},
	/* SupraExpress 56i Sp Intl */
	{	"SUP2171",		0	},
	/* Phoebe Micro */
	/* Phoebe Micro 33.6 Data Fax 1433VQH Plug & Play */
	{	"TEX0011",		0	},
	/* Archtek America Corp. */
	/* Archtek SmartLink Modem 3334BT Plug & Play */
	{	"UAC000F",		0	},
	/* 3Com Corp. */
	/* Gateway Telepath IIvi 33.6 */
	{	"USR0000",		0	},
	/* U.S. Robotics Sporster 33.6K Fax INT PnP */
	{	"USR0002",		0	},
	/*  Sportster Vi 14.4 PnP FAX Voicemail */
	{	"USR0004",		0	},
	/* U.S. Robotics 33.6K Voice INT PnP */
	{	"USR0006",		0	},
	/* U.S. Robotics 33.6K Voice EXT PnP */
	{	"USR0007",		0	},
	/* U.S. Robotics Courier V.Everything INT PnP */
	{	"USR0009",		0	},
	/* U.S. Robotics 33.6K Voice INT PnP */
	{	"USR2002",		0	},
	/* U.S. Robotics 56K Voice INT PnP */
	{	"USR2070",		0	},
	/* U.S. Robotics 56K Voice EXT PnP */
	{	"USR2080",		0	},
	/* U.S. Robotics 56K FAX INT */
	{	"USR3031",		0	},
	/* U.S. Robotics 56K FAX INT */
	{	"USR3050",		0	},
	/* U.S. Robotics 56K Voice INT PnP */
	{	"USR3070",		0	},
	/* U.S. Robotics 56K Voice EXT PnP */
	{	"USR3080",		0	},
	/* U.S. Robotics 56K Voice INT PnP */
	{	"USR3090",		0	},
	/* U.S. Robotics 56K Message  */
	{	"USR9100",		0	},
	/* U.S. Robotics 56K FAX EXT PnP*/
	{	"USR9160",		0	},
	/* U.S. Robotics 56K FAX INT PnP*/
	{	"USR9170",		0	},
	/* U.S. Robotics 56K Voice EXT PnP*/
	{	"USR9180",		0	},
	/* U.S. Robotics 56K Voice INT PnP*/
	{	"USR9190",		0	},
	/* Wacom tablets */
	{	"WACFXXX",		0	},
	/* Compaq touchscreen */
	{       "FPI2002",              0 },
	/* Fujitsu Stylistic touchscreens */
	{       "FUJ02B2",              0 },
	{       "FUJ02B3",              0 },
	/* Fujitsu Stylistic LT touchscreens */
	{       "FUJ02B4",              0 },
	/* Passive Fujitsu Stylistic touchscreens */
	{       "FUJ02B6",              0 },
	{       "FUJ02B7",              0 },
	{       "FUJ02B8",              0 },
	{       "FUJ02B9",              0 },
	{       "FUJ02BC",              0 },
	/* Fujitsu Wacom Tablet PC device */
	{	"FUJ02E5",		0	},
	/* Fujitsu P-series tablet PC device */
	{	"FUJ02E6",		0	},
	/* Fujitsu Wacom 2FGT Tablet PC device */
	{	"FUJ02E7",		0	},
	/* Fujitsu Wacom 1FGT Tablet PC device */
	{	"FUJ02E9",		0	},
	/*
	 * LG C1 EXPRESS DUAL (C1-PB11A3) touch screen (actually a FUJ02E6
	 * in disguise).
	 */
	{	"LTS0001",		0       },
	/* Rockwell's (PORALiNK) 33600 INT PNP */
	{	"WCI0003",		0	},
	/* Unknown PnP modems */
	{	"PNPCXXX",		UNKNOWN_DEV	},
	/* More unknown PnP modems */
	{	"PNPDXXX",		UNKNOWN_DEV	},
	/*
	 * Winbond CIR port, should not be probed. We should keep track of
	 * it to prevent the legacy serial driver from probing it.
	 */
	{	"WEC1022",		CIR_PORT	},
	/*
	 * SMSC IrCC SIR/FIR port, should not be probed by serial driver as
	 * well so its own driver can bind to it.
	 */
	{	"SMCF010",		CIR_PORT	},
	{	"",			0	}
};

MODULE_DEVICE_TABLE(pnp, pnp_dev_table);

static const char *modem_names[] = {
	"MODEM", "Modem", "modem", "FAX", "Fax", "fax",
	"56K", "56k", "K56", "33.6", "28.8", "14.4",
	"33,600", "28,800", "14,400", "33.600", "28.800", "14.400",
	"33600", "28800", "14400", "V.90", "V.34", "V.32", NULL
};

static bool check_name(const char *name)
{
	const char **tmp;

	for (tmp = modem_names; *tmp; tmp++)
		if (strstr(name, *tmp))
			return true;

	return false;
}

static bool check_resources(struct pnp_dev *dev)
{
	static const resource_size_t base[] = {0x2f8, 0x3f8, 0x2e8, 0x3e8};
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(base); i++) {
		if (pnp_possible_config(dev, IORESOURCE_IO, base[i], 8))
			return true;
	}

	return false;
}

/*
 * Given a complete unknown PnP device, try to use some heuristics to
 * detect modems. Currently use such heuristic set:
 *     - dev->name or dev->bus->name must contain "modem" substring;
 *     - device must have only one IO region (8 byte long) with base address
 *       0x2e8, 0x3e8, 0x2f8 or 0x3f8.
 *
 * Such detection looks very ugly, but can detect at least some of numerous
 * PnP modems, alternatively we must hardcode all modems in pnp_devices[]
 * table.
 */
static int serial_pnp_guess_board(struct pnp_dev *dev)
{
	if (!(check_name(pnp_dev_name(dev)) ||
	    (dev->card && check_name(dev->card->name))))
		return -ENODEV;

	if (check_resources(dev))
		return 0;

	return -ENODEV;
}

static int
serial_pnp_probe(struct pnp_dev *dev, const struct pnp_device_id *dev_id)
{
	struct uart_8250_port uart, *port;
	int ret, line, flags = dev_id->driver_data;

	if (flags & UNKNOWN_DEV) {
		ret = serial_pnp_guess_board(dev);
		if (ret < 0)
			return ret;
	}

	memset(&uart, 0, sizeof(uart));
	if (pnp_irq_valid(dev, 0))
		uart.port.irq = pnp_irq(dev, 0);
	if ((flags & CIR_PORT) && pnp_port_valid(dev, 2)) {
		uart.port.iobase = pnp_port_start(dev, 2);
		uart.port.iotype = UPIO_PORT;
	} else if (pnp_port_valid(dev, 0)) {
		uart.port.iobase = pnp_port_start(dev, 0);
		uart.port.iotype = UPIO_PORT;
	} else if (pnp_mem_valid(dev, 0)) {
		uart.port.mapbase = pnp_mem_start(dev, 0);
		uart.port.iotype = UPIO_MEM;
		uart.port.flags = UPF_IOREMAP;
	} else
		return -ENODEV;

	dev_dbg(&dev->dev,
		 "Setup PNP port: port %#lx, mem %#llx, irq %u, type %u\n",
		 uart.port.iobase, (unsigned long long)uart.port.mapbase,
		 uart.port.irq, uart.port.iotype);

	if (flags & CIR_PORT) {
		uart.port.flags |= UPF_FIXED_PORT | UPF_FIXED_TYPE;
		uart.port.type = PORT_8250_CIR;
	}

	uart.port.flags |= UPF_SKIP_TEST | UPF_BOOT_AUTOCONF;
	if (pnp_irq_flags(dev, 0) & IORESOURCE_IRQ_SHAREABLE)
		uart.port.flags |= UPF_SHARE_IRQ;
	uart.port.uartclk = 1843200;
	uart.port.dev = &dev->dev;

	line = serial8250_register_8250_port(&uart);
	if (line < 0 || (flags & CIR_PORT))
		return -ENODEV;

	port = serial8250_get_port(line);
	if (uart_console(&port->port))
		dev->capabilities |= PNP_CONSOLE;

	pnp_set_drvdata(dev, (void *)((long)line + 1));
	return 0;
}

static void serial_pnp_remove(struct pnp_dev *dev)
{
	long line = (long)pnp_get_drvdata(dev);

	dev->capabilities &= ~PNP_CONSOLE;
	if (line)
		serial8250_unregister_port(line - 1);
}

static int __maybe_unused serial_pnp_suspend(struct device *dev)
{
	long line = (long)dev_get_drvdata(dev);

	if (!line)
		return -ENODEV;
	serial8250_suspend_port(line - 1);
	return 0;
}

static int __maybe_unused serial_pnp_resume(struct device *dev)
{
	long line = (long)dev_get_drvdata(dev);

	if (!line)
		return -ENODEV;
	serial8250_resume_port(line - 1);
	return 0;
}

static SIMPLE_DEV_PM_OPS(serial_pnp_pm_ops, serial_pnp_suspend, serial_pnp_resume);

static struct pnp_driver serial_pnp_driver = {
	.name		= "serial",
	.probe		= serial_pnp_probe,
	.remove		= serial_pnp_remove,
	.driver         = {
		.pm     = &serial_pnp_pm_ops,
	},
	.id_table	= pnp_dev_table,
};

int serial8250_pnp_init(void)
{
	return pnp_register_driver(&serial_pnp_driver);
}

void serial8250_pnp_exit(void)
{
	pnp_unregister_driver(&serial_pnp_driver);
}

