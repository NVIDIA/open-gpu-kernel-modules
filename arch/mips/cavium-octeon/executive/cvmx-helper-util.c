/***********************license start***************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2008 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
 ***********************license end**************************************/

/*
 * Small helper utilities.
 */
#include <linux/kernel.h>

#include <asm/octeon/octeon.h>

#include <asm/octeon/cvmx-config.h>

#include <asm/octeon/cvmx-fpa.h>
#include <asm/octeon/cvmx-pip.h>
#include <asm/octeon/cvmx-pko.h>
#include <asm/octeon/cvmx-ipd.h>
#include <asm/octeon/cvmx-spi.h>

#include <asm/octeon/cvmx-helper.h>
#include <asm/octeon/cvmx-helper-util.h>

#include <asm/octeon/cvmx-ipd-defs.h>

/**
 * Convert a interface mode into a human readable string
 *
 * @mode:   Mode to convert
 *
 * Returns String
 */
const char *cvmx_helper_interface_mode_to_string(cvmx_helper_interface_mode_t
						 mode)
{
	switch (mode) {
	case CVMX_HELPER_INTERFACE_MODE_DISABLED:
		return "DISABLED";
	case CVMX_HELPER_INTERFACE_MODE_RGMII:
		return "RGMII";
	case CVMX_HELPER_INTERFACE_MODE_GMII:
		return "GMII";
	case CVMX_HELPER_INTERFACE_MODE_SPI:
		return "SPI";
	case CVMX_HELPER_INTERFACE_MODE_PCIE:
		return "PCIE";
	case CVMX_HELPER_INTERFACE_MODE_XAUI:
		return "XAUI";
	case CVMX_HELPER_INTERFACE_MODE_SGMII:
		return "SGMII";
	case CVMX_HELPER_INTERFACE_MODE_PICMG:
		return "PICMG";
	case CVMX_HELPER_INTERFACE_MODE_NPI:
		return "NPI";
	case CVMX_HELPER_INTERFACE_MODE_LOOP:
		return "LOOP";
	}
	return "UNKNOWN";
}

/**
 * Setup Random Early Drop on a specific input queue
 *
 * @queue:  Input queue to setup RED on (0-7)
 * @pass_thresh:
 *		 Packets will begin slowly dropping when there are less than
 *		 this many packet buffers free in FPA 0.
 * @drop_thresh:
 *		 All incoming packets will be dropped when there are less
 *		 than this many free packet buffers in FPA 0.
 * Returns Zero on success. Negative on failure
 */
static int cvmx_helper_setup_red_queue(int queue, int pass_thresh,
				       int drop_thresh)
{
	union cvmx_ipd_qosx_red_marks red_marks;
	union cvmx_ipd_red_quex_param red_param;

	/* Set RED to begin dropping packets when there are pass_thresh buffers
	   left. It will linearly drop more packets until reaching drop_thresh
	   buffers */
	red_marks.u64 = 0;
	red_marks.s.drop = drop_thresh;
	red_marks.s.pass = pass_thresh;
	cvmx_write_csr(CVMX_IPD_QOSX_RED_MARKS(queue), red_marks.u64);

	/* Use the actual queue 0 counter, not the average */
	red_param.u64 = 0;
	red_param.s.prb_con =
	    (255ul << 24) / (red_marks.s.pass - red_marks.s.drop);
	red_param.s.avg_con = 1;
	red_param.s.new_con = 255;
	red_param.s.use_pcnt = 1;
	cvmx_write_csr(CVMX_IPD_RED_QUEX_PARAM(queue), red_param.u64);
	return 0;
}

/**
 * Setup Random Early Drop to automatically begin dropping packets.
 *
 * @pass_thresh:
 *		 Packets will begin slowly dropping when there are less than
 *		 this many packet buffers free in FPA 0.
 * @drop_thresh:
 *		 All incoming packets will be dropped when there are less
 *		 than this many free packet buffers in FPA 0.
 * Returns Zero on success. Negative on failure
 */
int cvmx_helper_setup_red(int pass_thresh, int drop_thresh)
{
	union cvmx_ipd_portx_bp_page_cnt page_cnt;
	union cvmx_ipd_bp_prt_red_end ipd_bp_prt_red_end;
	union cvmx_ipd_red_port_enable red_port_enable;
	int queue;
	int interface;
	int port;

	/* Disable backpressure based on queued buffers. It needs SW support */
	page_cnt.u64 = 0;
	page_cnt.s.bp_enb = 0;
	page_cnt.s.page_cnt = 100;
	for (interface = 0; interface < 2; interface++) {
		for (port = cvmx_helper_get_first_ipd_port(interface);
		     port < cvmx_helper_get_last_ipd_port(interface); port++)
			cvmx_write_csr(CVMX_IPD_PORTX_BP_PAGE_CNT(port),
				       page_cnt.u64);
	}

	for (queue = 0; queue < 8; queue++)
		cvmx_helper_setup_red_queue(queue, pass_thresh, drop_thresh);

	/* Shutoff the dropping based on the per port page count. SW isn't
	   decrementing it right now */
	ipd_bp_prt_red_end.u64 = 0;
	ipd_bp_prt_red_end.s.prt_enb = 0;
	cvmx_write_csr(CVMX_IPD_BP_PRT_RED_END, ipd_bp_prt_red_end.u64);

	red_port_enable.u64 = 0;
	red_port_enable.s.prt_enb = 0xfffffffffull;
	red_port_enable.s.avg_dly = 10000;
	red_port_enable.s.prb_dly = 10000;
	cvmx_write_csr(CVMX_IPD_RED_PORT_ENABLE, red_port_enable.u64);

	return 0;
}
EXPORT_SYMBOL_GPL(cvmx_helper_setup_red);

/**
 * Setup the common GMX settings that determine the number of
 * ports. These setting apply to almost all configurations of all
 * chips.
 *
 * @interface: Interface to configure
 * @num_ports: Number of ports on the interface
 *
 * Returns Zero on success, negative on failure
 */
int __cvmx_helper_setup_gmx(int interface, int num_ports)
{
	union cvmx_gmxx_tx_prts gmx_tx_prts;
	union cvmx_gmxx_rx_prts gmx_rx_prts;
	union cvmx_pko_reg_gmx_port_mode pko_mode;
	union cvmx_gmxx_txx_thresh gmx_tx_thresh;
	int index;

	/* Tell GMX the number of TX ports on this interface */
	gmx_tx_prts.u64 = cvmx_read_csr(CVMX_GMXX_TX_PRTS(interface));
	gmx_tx_prts.s.prts = num_ports;
	cvmx_write_csr(CVMX_GMXX_TX_PRTS(interface), gmx_tx_prts.u64);

	/* Tell GMX the number of RX ports on this interface.  This only
	 ** applies to *GMII and XAUI ports */
	if (cvmx_helper_interface_get_mode(interface) ==
	    CVMX_HELPER_INTERFACE_MODE_RGMII
	    || cvmx_helper_interface_get_mode(interface) ==
	    CVMX_HELPER_INTERFACE_MODE_SGMII
	    || cvmx_helper_interface_get_mode(interface) ==
	    CVMX_HELPER_INTERFACE_MODE_GMII
	    || cvmx_helper_interface_get_mode(interface) ==
	    CVMX_HELPER_INTERFACE_MODE_XAUI) {
		if (num_ports > 4) {
			cvmx_dprintf("__cvmx_helper_setup_gmx: Illegal "
				     "num_ports\n");
			return -1;
		}

		gmx_rx_prts.u64 = cvmx_read_csr(CVMX_GMXX_RX_PRTS(interface));
		gmx_rx_prts.s.prts = num_ports;
		cvmx_write_csr(CVMX_GMXX_RX_PRTS(interface), gmx_rx_prts.u64);
	}

	/* Skip setting CVMX_PKO_REG_GMX_PORT_MODE on 30XX, 31XX, and 50XX */
	if (!OCTEON_IS_MODEL(OCTEON_CN30XX) && !OCTEON_IS_MODEL(OCTEON_CN31XX)
	    && !OCTEON_IS_MODEL(OCTEON_CN50XX)) {
		/* Tell PKO the number of ports on this interface */
		pko_mode.u64 = cvmx_read_csr(CVMX_PKO_REG_GMX_PORT_MODE);
		if (interface == 0) {
			if (num_ports == 1)
				pko_mode.s.mode0 = 4;
			else if (num_ports == 2)
				pko_mode.s.mode0 = 3;
			else if (num_ports <= 4)
				pko_mode.s.mode0 = 2;
			else if (num_ports <= 8)
				pko_mode.s.mode0 = 1;
			else
				pko_mode.s.mode0 = 0;
		} else {
			if (num_ports == 1)
				pko_mode.s.mode1 = 4;
			else if (num_ports == 2)
				pko_mode.s.mode1 = 3;
			else if (num_ports <= 4)
				pko_mode.s.mode1 = 2;
			else if (num_ports <= 8)
				pko_mode.s.mode1 = 1;
			else
				pko_mode.s.mode1 = 0;
		}
		cvmx_write_csr(CVMX_PKO_REG_GMX_PORT_MODE, pko_mode.u64);
	}

	/*
	 * Set GMX to buffer as much data as possible before starting
	 * transmit.  This reduces the chances that we have a TX under
	 * run due to memory contention. Any packet that fits entirely
	 * in the GMX FIFO can never have an under run regardless of
	 * memory load.
	 */
	gmx_tx_thresh.u64 = cvmx_read_csr(CVMX_GMXX_TXX_THRESH(0, interface));
	if (OCTEON_IS_MODEL(OCTEON_CN30XX) || OCTEON_IS_MODEL(OCTEON_CN31XX)
	    || OCTEON_IS_MODEL(OCTEON_CN50XX)) {
		/* These chips have a fixed max threshold of 0x40 */
		gmx_tx_thresh.s.cnt = 0x40;
	} else {
		/* Choose the max value for the number of ports */
		if (num_ports <= 1)
			gmx_tx_thresh.s.cnt = 0x100 / 1;
		else if (num_ports == 2)
			gmx_tx_thresh.s.cnt = 0x100 / 2;
		else
			gmx_tx_thresh.s.cnt = 0x100 / 4;
	}
	/*
	 * SPI and XAUI can have lots of ports but the GMX hardware
	 * only ever has a max of 4.
	 */
	if (num_ports > 4)
		num_ports = 4;
	for (index = 0; index < num_ports; index++)
		cvmx_write_csr(CVMX_GMXX_TXX_THRESH(index, interface),
			       gmx_tx_thresh.u64);

	return 0;
}

/**
 * Returns the IPD/PKO port number for a port on the given
 * interface.
 *
 * @interface: Interface to use
 * @port:      Port on the interface
 *
 * Returns IPD/PKO port number
 */
int cvmx_helper_get_ipd_port(int interface, int port)
{
	switch (interface) {
	case 0:
		return port;
	case 1:
		return port + 16;
	case 2:
		return port + 32;
	case 3:
		return port + 36;
	case 4:
		return port + 40;
	case 5:
		return port + 44;
	}
	return -1;
}
EXPORT_SYMBOL_GPL(cvmx_helper_get_ipd_port);

/**
 * Returns the interface number for an IPD/PKO port number.
 *
 * @ipd_port: IPD/PKO port number
 *
 * Returns Interface number
 */
int cvmx_helper_get_interface_num(int ipd_port)
{
	if (ipd_port < 16)
		return 0;
	else if (ipd_port < 32)
		return 1;
	else if (ipd_port < 36)
		return 2;
	else if (ipd_port < 40)
		return 3;
	else if (ipd_port < 44)
		return 4;
	else if (ipd_port < 48)
		return 5;
	else
		cvmx_dprintf("cvmx_helper_get_interface_num: Illegal IPD "
			     "port number\n");

	return -1;
}
EXPORT_SYMBOL_GPL(cvmx_helper_get_interface_num);

/**
 * Returns the interface index number for an IPD/PKO port
 * number.
 *
 * @ipd_port: IPD/PKO port number
 *
 * Returns Interface index number
 */
int cvmx_helper_get_interface_index_num(int ipd_port)
{
	if (ipd_port < 32)
		return ipd_port & 15;
	else if (ipd_port < 36)
		return ipd_port & 3;
	else if (ipd_port < 40)
		return ipd_port & 3;
	else if (ipd_port < 44)
		return ipd_port & 3;
	else if (ipd_port < 48)
		return ipd_port & 3;
	else
		cvmx_dprintf("cvmx_helper_get_interface_index_num: "
			     "Illegal IPD port number\n");

	return -1;
}
EXPORT_SYMBOL_GPL(cvmx_helper_get_interface_index_num);
