/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/******************************************************************************
 *
 * Name: acpixf.h - External interfaces to the ACPI subsystem
 *
 * Copyright (C) 2000 - 2020, Intel Corp.
 *
 *****************************************************************************/
#ifndef KCL_KCL_ACPI_TABLE_H
#define KCL_KCL_ACPI_TABLE_H

#include <linux/acpi.h>

#ifndef HAVE_ACPI_PUT_TABLE
void acpi_put_table(struct acpi_table_header *table);
#endif

#endif
