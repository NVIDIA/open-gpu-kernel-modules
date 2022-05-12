/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * acpi.h - ACPI Interface
 *
 * Copyright (C) 2001 Paul Diefenbaugh <paul.s.diefenbaugh@intel.com>
 */
#ifndef AMDKCL_ACPI_H
#define AMDKCL_ACPI_H

/**
 * interface change in mainline kernel 3.13
 * but only affect RHEL6 without backport
 * v3.7-rc5-12-g95f8a082b9b1 ACPI / driver core: Introduce struct acpi_dev_node
 * and related macros
 * v3.12-8048-g7b1998116bbb ACPI / driver core: Store an ACPI device pointer in
 * struct acpi_dev_node
 */

#include <linux/acpi.h>

/* Copied from include/linux/acpi.h> */
#ifndef ACPI_HANDLE
#define ACPI_HANDLE(dev) DEVICE_ACPI_HANDLE(dev)
#endif

#endif /* AMDKCL_ACPI_H */
