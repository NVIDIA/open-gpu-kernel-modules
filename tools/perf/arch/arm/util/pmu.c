// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright(C) 2015 Linaro Limited. All rights reserved.
 * Author: Mathieu Poirier <mathieu.poirier@linaro.org>
 */

#include <string.h>
#include <linux/coresight-pmu.h>
#include <linux/perf_event.h>
#include <linux/string.h>

#include "arm-spe.h"
#include "../../util/pmu.h"

struct perf_event_attr
*perf_pmu__get_default_config(struct perf_pmu *pmu __maybe_unused)
{
#ifdef HAVE_AUXTRACE_SUPPORT
	if (!strcmp(pmu->name, CORESIGHT_ETM_PMU_NAME)) {
		/* add ETM default config here */
		pmu->selectable = true;
#if defined(__aarch64__)
	} else if (strstarts(pmu->name, ARM_SPE_PMU_NAME)) {
		return arm_spe_pmu_default_config(pmu);
#endif
	}

#endif
	return NULL;
}
