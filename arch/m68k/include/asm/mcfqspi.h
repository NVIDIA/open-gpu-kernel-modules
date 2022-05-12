/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Definitions for Freescale Coldfire QSPI module
 *
 * Copyright 2010 Steven King <sfking@fdwdc.com>
*/

#ifndef mcfqspi_h
#define mcfqspi_h

/**
 * struct mcfqspi_cs_control - chip select control for the coldfire qspi driver
 * @setup: setup the control; allocate gpio's, etc. May be NULL.
 * @teardown: finish with the control; free gpio's, etc. May be NULL.
 * @select: output the signals to select the device.  Can not be NULL.
 * @deselect: output the signals to deselect the device. Can not be NULL.
 *
 * The QSPI module has 4 hardware chip selects.  We don't use them.  Instead
 * platforms are required to supply a mcfqspi_cs_control as a part of the
 * platform data for each QSPI master controller.  Only the select and
 * deselect functions are required.
*/
struct mcfqspi_cs_control {
	int 	(*setup)(struct mcfqspi_cs_control *);
	void	(*teardown)(struct mcfqspi_cs_control *);
	void	(*select)(struct mcfqspi_cs_control *, u8, bool);
	void	(*deselect)(struct mcfqspi_cs_control *, u8, bool);
};

/**
 * struct mcfqspi_platform_data - platform data for the coldfire qspi driver
 * @bus_num: board specific identifier for this qspi driver.
 * @num_chipselects: number of chip selects supported by this qspi driver.
 * @cs_control: platform dependent chip select control.
*/
struct mcfqspi_platform_data {
	s16	bus_num;
	u16	num_chipselect;
	struct mcfqspi_cs_control *cs_control;
};

#endif /* mcfqspi_h */
