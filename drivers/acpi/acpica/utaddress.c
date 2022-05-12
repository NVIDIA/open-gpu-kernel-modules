// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Module Name: utaddress - op_region address range check
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"
#include "acnamesp.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utaddress")

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_add_address_range
 *
 * PARAMETERS:  space_id            - Address space ID
 *              address             - op_region start address
 *              length              - op_region length
 *              region_node         - op_region namespace node
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Add the Operation Region address range to the global list.
 *              The only supported Space IDs are Memory and I/O. Called when
 *              the op_region address/length operands are fully evaluated.
 *
 * MUTEX:       Locks the namespace
 *
 * NOTE: Because this interface is only called when an op_region argument
 * list is evaluated, there cannot be any duplicate region_nodes.
 * Duplicate Address/Length values are allowed, however, so that multiple
 * address conflicts can be detected.
 *
 ******************************************************************************/
acpi_status
acpi_ut_add_address_range(acpi_adr_space_type space_id,
			  acpi_physical_address address,
			  u32 length, struct acpi_namespace_node *region_node)
{
	struct acpi_address_range *range_info;

	ACPI_FUNCTION_TRACE(ut_add_address_range);

	if ((space_id != ACPI_ADR_SPACE_SYSTEM_MEMORY) &&
	    (space_id != ACPI_ADR_SPACE_SYSTEM_IO)) {
		return_ACPI_STATUS(AE_OK);
	}

	/* Allocate/init a new info block, add it to the appropriate list */

	range_info = ACPI_ALLOCATE(sizeof(struct acpi_address_range));
	if (!range_info) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	range_info->start_address = address;
	range_info->end_address = (address + length - 1);
	range_info->region_node = region_node;

	range_info->next = acpi_gbl_address_range_list[space_id];
	acpi_gbl_address_range_list[space_id] = range_info;

	ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
			  "\nAdded [%4.4s] address range: 0x%8.8X%8.8X-0x%8.8X%8.8X\n",
			  acpi_ut_get_node_name(range_info->region_node),
			  ACPI_FORMAT_UINT64(address),
			  ACPI_FORMAT_UINT64(range_info->end_address)));

	return_ACPI_STATUS(AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_remove_address_range
 *
 * PARAMETERS:  space_id            - Address space ID
 *              region_node         - op_region namespace node
 *
 * RETURN:      None
 *
 * DESCRIPTION: Remove the Operation Region from the global list. The only
 *              supported Space IDs are Memory and I/O. Called when an
 *              op_region is deleted.
 *
 * MUTEX:       Assumes the namespace is locked
 *
 ******************************************************************************/

void
acpi_ut_remove_address_range(acpi_adr_space_type space_id,
			     struct acpi_namespace_node *region_node)
{
	struct acpi_address_range *range_info;
	struct acpi_address_range *prev;

	ACPI_FUNCTION_TRACE(ut_remove_address_range);

	if ((space_id != ACPI_ADR_SPACE_SYSTEM_MEMORY) &&
	    (space_id != ACPI_ADR_SPACE_SYSTEM_IO)) {
		return_VOID;
	}

	/* Get the appropriate list head and check the list */

	range_info = prev = acpi_gbl_address_range_list[space_id];
	while (range_info) {
		if (range_info->region_node == region_node) {
			if (range_info == prev) {	/* Found at list head */
				acpi_gbl_address_range_list[space_id] =
				    range_info->next;
			} else {
				prev->next = range_info->next;
			}

			ACPI_DEBUG_PRINT((ACPI_DB_NAMES,
					  "\nRemoved [%4.4s] address range: 0x%8.8X%8.8X-0x%8.8X%8.8X\n",
					  acpi_ut_get_node_name(range_info->
								region_node),
					  ACPI_FORMAT_UINT64(range_info->
							     start_address),
					  ACPI_FORMAT_UINT64(range_info->
							     end_address)));

			ACPI_FREE(range_info);
			return_VOID;
		}

		prev = range_info;
		range_info = range_info->next;
	}

	return_VOID;
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_check_address_range
 *
 * PARAMETERS:  space_id            - Address space ID
 *              address             - Start address
 *              length              - Length of address range
 *              warn                - TRUE if warning on overlap desired
 *
 * RETURN:      Count of the number of conflicts detected. Zero is always
 *              returned for Space IDs other than Memory or I/O.
 *
 * DESCRIPTION: Check if the input address range overlaps any of the
 *              ASL operation region address ranges. The only supported
 *              Space IDs are Memory and I/O.
 *
 * MUTEX:       Assumes the namespace is locked.
 *
 ******************************************************************************/

u32
acpi_ut_check_address_range(acpi_adr_space_type space_id,
			    acpi_physical_address address, u32 length, u8 warn)
{
	struct acpi_address_range *range_info;
	acpi_physical_address end_address;
	char *pathname;
	u32 overlap_count = 0;

	ACPI_FUNCTION_TRACE(ut_check_address_range);

	if ((space_id != ACPI_ADR_SPACE_SYSTEM_MEMORY) &&
	    (space_id != ACPI_ADR_SPACE_SYSTEM_IO)) {
		return_UINT32(0);
	}

	range_info = acpi_gbl_address_range_list[space_id];
	end_address = address + length - 1;

	/* Check entire list for all possible conflicts */

	while (range_info) {
		/*
		 * Check if the requested address/length overlaps this
		 * address range. There are four cases to consider:
		 *
		 * 1) Input address/length is contained completely in the
		 *    address range
		 * 2) Input address/length overlaps range at the range start
		 * 3) Input address/length overlaps range at the range end
		 * 4) Input address/length completely encompasses the range
		 */
		if ((address <= range_info->end_address) &&
		    (end_address >= range_info->start_address)) {

			/* Found an address range overlap */

			overlap_count++;
			if (warn) {	/* Optional warning message */
				pathname =
				    acpi_ns_get_normalized_pathname(range_info->
								    region_node,
								    TRUE);

				ACPI_WARNING((AE_INFO,
					      "%s range 0x%8.8X%8.8X-0x%8.8X%8.8X conflicts with OpRegion 0x%8.8X%8.8X-0x%8.8X%8.8X (%s)",
					      acpi_ut_get_region_name(space_id),
					      ACPI_FORMAT_UINT64(address),
					      ACPI_FORMAT_UINT64(end_address),
					      ACPI_FORMAT_UINT64(range_info->
								 start_address),
					      ACPI_FORMAT_UINT64(range_info->
								 end_address),
					      pathname));
				ACPI_FREE(pathname);
			}
		}

		range_info = range_info->next;
	}

	return_UINT32(overlap_count);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_delete_address_lists
 *
 * PARAMETERS:  None
 *
 * RETURN:      None
 *
 * DESCRIPTION: Delete all global address range lists (called during
 *              subsystem shutdown).
 *
 ******************************************************************************/

void acpi_ut_delete_address_lists(void)
{
	struct acpi_address_range *next;
	struct acpi_address_range *range_info;
	int i;

	/* Delete all elements in all address range lists */

	for (i = 0; i < ACPI_ADDRESS_RANGE_MAX; i++) {
		next = acpi_gbl_address_range_list[i];

		while (next) {
			range_info = next;
			next = range_info->next;
			ACPI_FREE(range_info);
		}

		acpi_gbl_address_range_list[i] = NULL;
	}
}
