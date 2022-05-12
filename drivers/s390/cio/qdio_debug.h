/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright IBM Corp. 2008
 *
 *  Author: Jan Glauber (jang@linux.vnet.ibm.com)
 */
#ifndef QDIO_DEBUG_H
#define QDIO_DEBUG_H

#include <asm/debug.h>
#include <asm/qdio.h>
#include "qdio.h"

/* that gives us 15 characters in the text event views */
#define QDIO_DBF_LEN	32

extern debug_info_t *qdio_dbf_setup;
extern debug_info_t *qdio_dbf_error;

#define DBF_ERR		3	/* error conditions	*/
#define DBF_WARN	4	/* warning conditions	*/
#define DBF_INFO	6	/* informational	*/

#undef DBF_EVENT
#undef DBF_ERROR
#undef DBF_DEV_EVENT

#define DBF_EVENT(text...) \
	do { \
		char debug_buffer[QDIO_DBF_LEN]; \
		snprintf(debug_buffer, QDIO_DBF_LEN, text); \
		debug_text_event(qdio_dbf_setup, DBF_ERR, debug_buffer); \
	} while (0)

static inline void DBF_HEX(void *addr, int len)
{
	debug_event(qdio_dbf_setup, DBF_ERR, addr, len);
}

#define DBF_ERROR(text...) \
	do { \
		char debug_buffer[QDIO_DBF_LEN]; \
		snprintf(debug_buffer, QDIO_DBF_LEN, text); \
		debug_text_event(qdio_dbf_error, DBF_ERR, debug_buffer); \
	} while (0)

static inline void DBF_ERROR_HEX(void *addr, int len)
{
	debug_event(qdio_dbf_error, DBF_ERR, addr, len);
}

#define DBF_DEV_EVENT(level, device, text...) \
	do { \
		char debug_buffer[QDIO_DBF_LEN]; \
		if (debug_level_enabled(device->debug_area, level)) { \
			snprintf(debug_buffer, QDIO_DBF_LEN, text); \
			debug_text_event(device->debug_area, level, debug_buffer); \
		} \
	} while (0)

static inline void DBF_DEV_HEX(struct qdio_irq *dev, void *addr,
			       int len, int level)
{
	debug_event(dev->debug_area, level, addr, len);
}

int qdio_allocate_dbf(struct qdio_irq *irq_ptr);
void qdio_setup_debug_entries(struct qdio_irq *irq_ptr);
void qdio_shutdown_debug_entries(struct qdio_irq *irq_ptr);
int qdio_debug_init(void);
void qdio_debug_exit(void);

#endif
