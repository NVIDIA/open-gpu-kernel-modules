/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Surface System Aggregator Module bus and device integration.
 *
 * Copyright (C) 2019-2020 Maximilian Luz <luzmaximilian@gmail.com>
 */

#ifndef _SURFACE_AGGREGATOR_BUS_H
#define _SURFACE_AGGREGATOR_BUS_H

#include <linux/surface_aggregator/controller.h>

#ifdef CONFIG_SURFACE_AGGREGATOR_BUS

void ssam_controller_remove_clients(struct ssam_controller *ctrl);

int ssam_bus_register(void);
void ssam_bus_unregister(void);

#else /* CONFIG_SURFACE_AGGREGATOR_BUS */

static inline void ssam_controller_remove_clients(struct ssam_controller *ctrl) {}
static inline int ssam_bus_register(void) { return 0; }
static inline void ssam_bus_unregister(void) {}

#endif /* CONFIG_SURFACE_AGGREGATOR_BUS */
#endif /* _SURFACE_AGGREGATOR_BUS_H */
