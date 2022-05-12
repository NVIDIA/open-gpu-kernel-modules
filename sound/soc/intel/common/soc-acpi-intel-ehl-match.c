// SPDX-License-Identifier: GPL-2.0-only
/*
 * soc-acpi-intel-ehl-match.c - tables and support for EHL ACPI enumeration.
 *
 * Copyright (c) 2019, Intel Corporation.
 *
 */

#include <sound/soc-acpi.h>
#include <sound/soc-acpi-intel-match.h>
#include "../skylake/skl.h"

struct snd_soc_acpi_mach snd_soc_acpi_intel_ehl_machines[] = {
	{
		.id = "10EC5660",
		.drv_name = "ehl_rt5660",
		.sof_fw_filename = "sof-ehl.ri",
		.sof_tplg_filename = "sof-ehl-rt5660.tplg",
	},
	{},
};
EXPORT_SYMBOL_GPL(snd_soc_acpi_intel_ehl_machines);
