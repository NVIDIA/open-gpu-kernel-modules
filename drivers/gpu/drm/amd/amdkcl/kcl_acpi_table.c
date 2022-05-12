// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Module Name: tbxface - ACPI table-oriented external interfaces
 *
 * Copyright (C) 2000 - 2020, Intel Corp.
 *
 *****************************************************************************/
#include <linux/acpi.h>
#include <kcl/kcl_acpi_table.h>

#ifndef HAVE_ACPI_PUT_TABLE
amdkcl_dummy_symbol(acpi_put_table, void, return,
				  struct acpi_table_header *table)
#endif
