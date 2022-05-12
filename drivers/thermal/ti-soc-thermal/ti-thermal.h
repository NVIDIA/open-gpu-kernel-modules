/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * OMAP thermal definitions
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 * Contact:
 *   Eduardo Valentin <eduardo.valentin@ti.com>
 */
#ifndef __TI_THERMAL_H
#define __TI_THERMAL_H

#include "ti-bandgap.h"

/* PCB sensor calculation constants */
#define OMAP_GRADIENT_SLOPE_W_PCB_4430				0
#define OMAP_GRADIENT_CONST_W_PCB_4430				20000
#define OMAP_GRADIENT_SLOPE_W_PCB_4460				1142
#define OMAP_GRADIENT_CONST_W_PCB_4460				-393
#define OMAP_GRADIENT_SLOPE_W_PCB_4470				1063
#define OMAP_GRADIENT_CONST_W_PCB_4470				-477

#define OMAP_GRADIENT_SLOPE_W_PCB_5430_CPU			100
#define OMAP_GRADIENT_CONST_W_PCB_5430_CPU			484
#define OMAP_GRADIENT_SLOPE_W_PCB_5430_GPU			464
#define OMAP_GRADIENT_CONST_W_PCB_5430_GPU			-5102

#define DRA752_GRADIENT_SLOPE_W_PCB				0
#define DRA752_GRADIENT_CONST_W_PCB				2000

/* trip points of interest in milicelsius (at hotspot level) */
#define OMAP_TRIP_COLD						100000
#define OMAP_TRIP_HOT						110000
#define OMAP_TRIP_SHUTDOWN					125000
#define OMAP_TRIP_NUMBER					2
#define OMAP_TRIP_STEP							\
	((OMAP_TRIP_SHUTDOWN - OMAP_TRIP_HOT) / (OMAP_TRIP_NUMBER - 1))

/* Update rates */
#define FAST_TEMP_MONITORING_RATE				250

/* helper macros */
/**
 * ti_thermal_get_trip_value - returns trip temperature based on index
 * @i:	trip index
 */
#define ti_thermal_get_trip_value(i)					\
	(OMAP_TRIP_HOT + ((i) * OMAP_TRIP_STEP))

/**
 * ti_thermal_is_valid_trip - check for trip index
 * @i:	trip index
 */
#define ti_thermal_is_valid_trip(trip)				\
	((trip) >= 0 && (trip) < OMAP_TRIP_NUMBER)

#ifdef CONFIG_TI_THERMAL
int ti_thermal_expose_sensor(struct ti_bandgap *bgp, int id, char *domain);
int ti_thermal_remove_sensor(struct ti_bandgap *bgp, int id);
int ti_thermal_report_sensor_temperature(struct ti_bandgap *bgp, int id);
int ti_thermal_register_cpu_cooling(struct ti_bandgap *bgp, int id);
int ti_thermal_unregister_cpu_cooling(struct ti_bandgap *bgp, int id);
#else
static inline
int ti_thermal_expose_sensor(struct ti_bandgap *bgp, int id, char *domain)
{
	return 0;
}

static inline
int ti_thermal_remove_sensor(struct ti_bandgap *bgp, int id)
{
	return 0;
}

static inline
int ti_thermal_report_sensor_temperature(struct ti_bandgap *bgp, int id)
{
	return 0;
}

static inline
int ti_thermal_register_cpu_cooling(struct ti_bandgap *bgp, int id)
{
	return 0;
}

static inline
int ti_thermal_unregister_cpu_cooling(struct ti_bandgap *bgp, int id)
{
	return 0;
}
#endif
#endif
