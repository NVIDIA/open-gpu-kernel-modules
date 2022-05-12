// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/*******************************************************************************
 *
 * Module Name: utmisc - common utility procedures
 *
 ******************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"
#include "acnamesp.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utmisc")

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_is_pci_root_bridge
 *
 * PARAMETERS:  id              - The HID/CID in string format
 *
 * RETURN:      TRUE if the Id is a match for a PCI/PCI-Express Root Bridge
 *
 * DESCRIPTION: Determine if the input ID is a PCI Root Bridge ID.
 *
 ******************************************************************************/
u8 acpi_ut_is_pci_root_bridge(char *id)
{

	/*
	 * Check if this is a PCI root bridge.
	 * ACPI 3.0+: check for a PCI Express root also.
	 */
	if (!(strcmp(id,
		     PCI_ROOT_HID_STRING)) ||
	    !(strcmp(id, PCI_EXPRESS_ROOT_HID_STRING))) {
		return (TRUE);
	}

	return (FALSE);
}

#if (defined ACPI_ASL_COMPILER || defined ACPI_EXEC_APP || defined ACPI_NAMES_APP)
/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_is_aml_table
 *
 * PARAMETERS:  table               - An ACPI table
 *
 * RETURN:      TRUE if table contains executable AML; FALSE otherwise
 *
 * DESCRIPTION: Check ACPI Signature for a table that contains AML code.
 *              Currently, these are DSDT,SSDT,PSDT. All other table types are
 *              data tables that do not contain AML code.
 *
 ******************************************************************************/

u8 acpi_ut_is_aml_table(struct acpi_table_header *table)
{

	/* These are the only tables that contain executable AML */

	if (ACPI_COMPARE_NAMESEG(table->signature, ACPI_SIG_DSDT) ||
	    ACPI_COMPARE_NAMESEG(table->signature, ACPI_SIG_PSDT) ||
	    ACPI_COMPARE_NAMESEG(table->signature, ACPI_SIG_SSDT) ||
	    ACPI_COMPARE_NAMESEG(table->signature, ACPI_SIG_OSDT) ||
	    ACPI_IS_OEM_SIG(table->signature)) {
		return (TRUE);
	}

	return (FALSE);
}
#endif

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_dword_byte_swap
 *
 * PARAMETERS:  value           - Value to be converted
 *
 * RETURN:      u32 integer with bytes swapped
 *
 * DESCRIPTION: Convert a 32-bit value to big-endian (swap the bytes)
 *
 ******************************************************************************/

u32 acpi_ut_dword_byte_swap(u32 value)
{
	union {
		u32 value;
		u8 bytes[4];
	} out;
	union {
		u32 value;
		u8 bytes[4];
	} in;

	ACPI_FUNCTION_ENTRY();

	in.value = value;

	out.bytes[0] = in.bytes[3];
	out.bytes[1] = in.bytes[2];
	out.bytes[2] = in.bytes[1];
	out.bytes[3] = in.bytes[0];

	return (out.value);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_set_integer_width
 *
 * PARAMETERS:  Revision            From DSDT header
 *
 * RETURN:      None
 *
 * DESCRIPTION: Set the global integer bit width based upon the revision
 *              of the DSDT. For Revision 1 and 0, Integers are 32 bits.
 *              For Revision 2 and above, Integers are 64 bits. Yes, this
 *              makes a difference.
 *
 ******************************************************************************/

void acpi_ut_set_integer_width(u8 revision)
{

	if (revision < 2) {

		/* 32-bit case */

		acpi_gbl_integer_bit_width = 32;
		acpi_gbl_integer_nybble_width = 8;
		acpi_gbl_integer_byte_width = 4;
	} else {
		/* 64-bit case (ACPI 2.0+) */

		acpi_gbl_integer_bit_width = 64;
		acpi_gbl_integer_nybble_width = 16;
		acpi_gbl_integer_byte_width = 8;
	}
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_create_update_state_and_push
 *
 * PARAMETERS:  object          - Object to be added to the new state
 *              action          - Increment/Decrement
 *              state_list      - List the state will be added to
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Create a new state and push it
 *
 ******************************************************************************/

acpi_status
acpi_ut_create_update_state_and_push(union acpi_operand_object *object,
				     u16 action,
				     union acpi_generic_state **state_list)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	/* Ignore null objects; these are expected */

	if (!object) {
		return (AE_OK);
	}

	state = acpi_ut_create_update_state(object, action);
	if (!state) {
		return (AE_NO_MEMORY);
	}

	acpi_ut_push_generic_state(state_list, state);
	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_walk_package_tree
 *
 * PARAMETERS:  source_object       - The package to walk
 *              target_object       - Target object (if package is being copied)
 *              walk_callback       - Called once for each package element
 *              context             - Passed to the callback function
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Walk through a package, including subpackages
 *
 ******************************************************************************/

acpi_status
acpi_ut_walk_package_tree(union acpi_operand_object *source_object,
			  void *target_object,
			  acpi_pkg_callback walk_callback, void *context)
{
	acpi_status status = AE_OK;
	union acpi_generic_state *state_list = NULL;
	union acpi_generic_state *state;
	union acpi_operand_object *this_source_obj;
	u32 this_index;

	ACPI_FUNCTION_TRACE(ut_walk_package_tree);

	state = acpi_ut_create_pkg_state(source_object, target_object, 0);
	if (!state) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	while (state) {

		/* Get one element of the package */

		this_index = state->pkg.index;
		this_source_obj =
		    state->pkg.source_object->package.elements[this_index];
		state->pkg.this_target_obj =
		    &state->pkg.source_object->package.elements[this_index];

		/*
		 * Check for:
		 * 1) An uninitialized package element. It is completely
		 *    legal to declare a package and leave it uninitialized
		 * 2) Not an internal object - can be a namespace node instead
		 * 3) Any type other than a package. Packages are handled in else
		 *    case below.
		 */
		if ((!this_source_obj) ||
		    (ACPI_GET_DESCRIPTOR_TYPE(this_source_obj) !=
		     ACPI_DESC_TYPE_OPERAND) ||
		    (this_source_obj->common.type != ACPI_TYPE_PACKAGE)) {
			status =
			    walk_callback(ACPI_COPY_TYPE_SIMPLE,
					  this_source_obj, state, context);
			if (ACPI_FAILURE(status)) {
				return_ACPI_STATUS(status);
			}

			state->pkg.index++;
			while (state->pkg.index >=
			       state->pkg.source_object->package.count) {
				/*
				 * We've handled all of the objects at this level,  This means
				 * that we have just completed a package. That package may
				 * have contained one or more packages itself.
				 *
				 * Delete this state and pop the previous state (package).
				 */
				acpi_ut_delete_generic_state(state);
				state = acpi_ut_pop_generic_state(&state_list);

				/* Finished when there are no more states */

				if (!state) {
					/*
					 * We have handled all of the objects in the top level
					 * package just add the length of the package objects
					 * and exit
					 */
					return_ACPI_STATUS(AE_OK);
				}

				/*
				 * Go back up a level and move the index past the just
				 * completed package object.
				 */
				state->pkg.index++;
			}
		} else {
			/* This is a subobject of type package */

			status =
			    walk_callback(ACPI_COPY_TYPE_PACKAGE,
					  this_source_obj, state, context);
			if (ACPI_FAILURE(status)) {
				return_ACPI_STATUS(status);
			}

			/*
			 * Push the current state and create a new one
			 * The callback above returned a new target package object.
			 */
			acpi_ut_push_generic_state(&state_list, state);
			state =
			    acpi_ut_create_pkg_state(this_source_obj,
						     state->pkg.this_target_obj,
						     0);
			if (!state) {

				/* Free any stacked Update State objects */

				while (state_list) {
					state =
					    acpi_ut_pop_generic_state
					    (&state_list);
					acpi_ut_delete_generic_state(state);
				}
				return_ACPI_STATUS(AE_NO_MEMORY);
			}
		}
	}

	/* We should never get here */

	ACPI_ERROR((AE_INFO, "State list did not terminate correctly"));

	return_ACPI_STATUS(AE_AML_INTERNAL);
}

#ifdef ACPI_DEBUG_OUTPUT
/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_display_init_pathname
 *
 * PARAMETERS:  type                - Object type of the node
 *              obj_handle          - Handle whose pathname will be displayed
 *              path                - Additional path string to be appended.
 *                                      (NULL if no extra path)
 *
 * RETURN:      acpi_status
 *
 * DESCRIPTION: Display full pathname of an object, DEBUG ONLY
 *
 ******************************************************************************/

void
acpi_ut_display_init_pathname(u8 type,
			      struct acpi_namespace_node *obj_handle,
			      const char *path)
{
	acpi_status status;
	struct acpi_buffer buffer;

	ACPI_FUNCTION_ENTRY();

	/* Only print the path if the appropriate debug level is enabled */

	if (!(acpi_dbg_level & ACPI_LV_INIT_NAMES)) {
		return;
	}

	/* Get the full pathname to the node */

	buffer.length = ACPI_ALLOCATE_LOCAL_BUFFER;
	status = acpi_ns_handle_to_pathname(obj_handle, &buffer, TRUE);
	if (ACPI_FAILURE(status)) {
		return;
	}

	/* Print what we're doing */

	switch (type) {
	case ACPI_TYPE_METHOD:

		acpi_os_printf("Executing  ");
		break;

	default:

		acpi_os_printf("Initializing ");
		break;
	}

	/* Print the object type and pathname */

	acpi_os_printf("%-12s %s",
		       acpi_ut_get_type_name(type), (char *)buffer.pointer);

	/* Extra path is used to append names like _STA, _INI, etc. */

	if (path) {
		acpi_os_printf(".%s", path);
	}
	acpi_os_printf("\n");

	ACPI_FREE(buffer.pointer);
}
#endif
