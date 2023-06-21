/**
 *  Copyright Notice:
 *  Copyright 2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/
#ifndef PLATFORM_LIB_H
#define PLATFORM_LIB_H

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"

/**
 * Suspends the execution of the current thread until the time-out interval elapses.
 * This API is deprecated. Please use libspdm_sleep_in_us().
 *
 * @param milliseconds     The time interval for which execution is to be suspended, in milliseconds.
 *
 **/
void libspdm_sleep(uint64_t milliseconds);

/**
 * Suspends the execution of the current thread until the time-out interval elapses.
 *
 * @param microseconds     The time interval for which execution is to be suspended, in milliseconds.
 *
 **/
void libspdm_sleep_in_us(uint64_t microseconds);

#if LIBSPDM_ENABLE_CAPABILITY_HBEAT_CAP
/**
 * If no heartbeat arrives in seconds, the watchdog timeout event
 * should terminate the session.
 *
 * @param  session_id     Indicate the SPDM session ID.
 * @param  seconds        heartbeat period, in seconds.
 *
 **/
bool libspdm_start_watchdog(uint32_t session_id, uint16_t seconds);

/**
 * stop watchdog.
 *
 * @param  session_id     Indicate the SPDM session ID.
 *
 **/
bool libspdm_stop_watchdog(uint32_t session_id);

/**
 * Reset the watchdog in heartbeat response.
 *
 * @param  session_id     Indicate the SPDM session ID.
 *
 **/
bool libspdm_reset_watchdog(uint32_t session_id);
#endif /* LIBSPDM_ENABLE_CAPABILITY_HBEAT_CAP */

#endif /* PLATFORM_LIB_H */
