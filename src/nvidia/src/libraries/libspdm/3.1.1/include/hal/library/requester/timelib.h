/**
 *  Copyright Notice:
 *  Copyright 2022-2023 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/
#ifndef REQUESTER_TIMELIB_H
#define REQUESTER_TIMELIB_H

#include "hal/base.h"

/**
 * Suspends the execution of the current thread until the interval elapses.
 *
 * @param duration  The time interval, in units of microseconds for which execution is to be
 *                  suspended.
 **/
extern void libspdm_sleep(uint64_t duration);

#endif /* REQUESTER_TIMERLIB_H */
