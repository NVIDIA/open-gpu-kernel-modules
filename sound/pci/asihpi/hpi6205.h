/* SPDX-License-Identifier: GPL-2.0-only */
/*****************************************************************************

    AudioScience HPI driver
    Copyright (C) 1997-2011  AudioScience Inc. <support@audioscience.com>


Host Interface module for an ASI6205 based
bus mastering PCI adapter.

Copyright AudioScience, Inc., 2003
******************************************************************************/

#ifndef _HPI6205_H_
#define _HPI6205_H_

#include "hpi_internal.h"

/***********************************************************
	Defines used for basic messaging
************************************************************/
#define H620_HIF_RESET          0
#define H620_HIF_IDLE           1
#define H620_HIF_GET_RESP       2
#define H620_HIF_DATA_DONE      3
#define H620_HIF_DATA_MASK      0x10
#define H620_HIF_SEND_DATA      0x14
#define H620_HIF_GET_DATA       0x15
#define H620_HIF_UNKNOWN                0x0000ffff

/***********************************************************
	Types used for mixer control caching
************************************************************/

#define H620_MAX_ISTREAMS 32
#define H620_MAX_OSTREAMS 32
#define HPI_NMIXER_CONTROLS 2048

/*********************************************************************
This is used for dynamic control cache allocation
**********************************************************************/
struct controlcache_6205 {
	u32 number_of_controls;
	u32 physical_address32;
	u32 size_in_bytes;
};

/*********************************************************************
This is used for dynamic allocation of async event array
**********************************************************************/
struct async_event_buffer_6205 {
	u32 physical_address32;
	u32 spare;
	struct hpi_fifo_buffer b;
};

/***********************************************************
The Host located memory buffer that the 6205 will bus master
in and out of.
************************************************************/
#define HPI6205_SIZEOF_DATA (16*1024)

struct message_buffer_6205 {
	struct hpi_message message;
	char data[256];
};

struct response_buffer_6205 {
	struct hpi_response response;
	char data[256];
};

union buffer_6205 {
	struct message_buffer_6205 message_buffer;
	struct response_buffer_6205 response_buffer;
	u8 b_data[HPI6205_SIZEOF_DATA];
};

struct bus_master_interface {
	u32 host_cmd;
	u32 dsp_ack;
	u32 transfer_size_in_bytes;
	union buffer_6205 u;
	struct controlcache_6205 control_cache;
	struct async_event_buffer_6205 async_buffer;
	struct hpi_hostbuffer_status
	 instream_host_buffer_status[H620_MAX_ISTREAMS];
	struct hpi_hostbuffer_status
	 outstream_host_buffer_status[H620_MAX_OSTREAMS];
};

#endif
