// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/*******************************************************************************
 *
 * Module Name: utstate - state object support procedures
 *
 ******************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utstate")

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_push_generic_state
 *
 * PARAMETERS:  list_head           - Head of the state stack
 *              state               - State object to push
 *
 * RETURN:      None
 *
 * DESCRIPTION: Push a state object onto a state stack
 *
 ******************************************************************************/
void
acpi_ut_push_generic_state(union acpi_generic_state **list_head,
			   union acpi_generic_state *state)
{
	ACPI_FUNCTION_ENTRY();

	/* Push the state object onto the front of the list (stack) */

	state->common.next = *list_head;
	*list_head = state;
	return;
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_pop_generic_state
 *
 * PARAMETERS:  list_head           - Head of the state stack
 *
 * RETURN:      The popped state object
 *
 * DESCRIPTION: Pop a state object from a state stack
 *
 ******************************************************************************/

union acpi_generic_state *acpi_ut_pop_generic_state(union acpi_generic_state
						    **list_head)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	/* Remove the state object at the head of the list (stack) */

	state = *list_head;
	if (state) {

		/* Update the list head */

		*list_head = state->common.next;
	}

	return (state);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_create_generic_state
 *
 * PARAMETERS:  None
 *
 * RETURN:      The new state object. NULL on failure.
 *
 * DESCRIPTION: Create a generic state object. Attempt to obtain one from
 *              the global state cache;  If none available, create a new one.
 *
 ******************************************************************************/

union acpi_generic_state *acpi_ut_create_generic_state(void)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	state = acpi_os_acquire_object(acpi_gbl_state_cache);
	if (state) {

		/* Initialize */
		state->common.descriptor_type = ACPI_DESC_TYPE_STATE;
	}

	return (state);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_create_thread_state
 *
 * PARAMETERS:  None
 *
 * RETURN:      New Thread State. NULL on failure
 *
 * DESCRIPTION: Create a "Thread State" - a flavor of the generic state used
 *              to track per-thread info during method execution
 *
 ******************************************************************************/

struct acpi_thread_state *acpi_ut_create_thread_state(void)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	/* Create the generic state object */

	state = acpi_ut_create_generic_state();
	if (!state) {
		return (NULL);
	}

	/* Init fields specific to the update struct */

	state->common.descriptor_type = ACPI_DESC_TYPE_STATE_THREAD;
	state->thread.thread_id = acpi_os_get_thread_id();

	/* Check for invalid thread ID - zero is very bad, it will break things */

	if (!state->thread.thread_id) {
		ACPI_ERROR((AE_INFO, "Invalid zero ID from AcpiOsGetThreadId"));
		state->thread.thread_id = (acpi_thread_id) 1;
	}

	return ((struct acpi_thread_state *)state);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_create_update_state
 *
 * PARAMETERS:  object          - Initial Object to be installed in the state
 *              action          - Update action to be performed
 *
 * RETURN:      New state object, null on failure
 *
 * DESCRIPTION: Create an "Update State" - a flavor of the generic state used
 *              to update reference counts and delete complex objects such
 *              as packages.
 *
 ******************************************************************************/

union acpi_generic_state *acpi_ut_create_update_state(union acpi_operand_object
						      *object, u16 action)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	/* Create the generic state object */

	state = acpi_ut_create_generic_state();
	if (!state) {
		return (NULL);
	}

	/* Init fields specific to the update struct */

	state->common.descriptor_type = ACPI_DESC_TYPE_STATE_UPDATE;
	state->update.object = object;
	state->update.value = action;
	return (state);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_create_pkg_state
 *
 * PARAMETERS:  object          - Initial Object to be installed in the state
 *              action          - Update action to be performed
 *
 * RETURN:      New state object, null on failure
 *
 * DESCRIPTION: Create a "Package State"
 *
 ******************************************************************************/

union acpi_generic_state *acpi_ut_create_pkg_state(void *internal_object,
						   void *external_object,
						   u32 index)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	/* Create the generic state object */

	state = acpi_ut_create_generic_state();
	if (!state) {
		return (NULL);
	}

	/* Init fields specific to the update struct */

	state->common.descriptor_type = ACPI_DESC_TYPE_STATE_PACKAGE;
	state->pkg.source_object = (union acpi_operand_object *)internal_object;
	state->pkg.dest_object = external_object;
	state->pkg.index = index;
	state->pkg.num_packages = 1;

	return (state);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_create_control_state
 *
 * PARAMETERS:  None
 *
 * RETURN:      New state object, null on failure
 *
 * DESCRIPTION: Create a "Control State" - a flavor of the generic state used
 *              to support nested IF/WHILE constructs in the AML.
 *
 ******************************************************************************/

union acpi_generic_state *acpi_ut_create_control_state(void)
{
	union acpi_generic_state *state;

	ACPI_FUNCTION_ENTRY();

	/* Create the generic state object */

	state = acpi_ut_create_generic_state();
	if (!state) {
		return (NULL);
	}

	/* Init fields specific to the control struct */

	state->common.descriptor_type = ACPI_DESC_TYPE_STATE_CONTROL;
	state->common.state = ACPI_CONTROL_CONDITIONAL_EXECUTING;

	return (state);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_delete_generic_state
 *
 * PARAMETERS:  state               - The state object to be deleted
 *
 * RETURN:      None
 *
 * DESCRIPTION: Release a state object to the state cache. NULL state objects
 *              are ignored.
 *
 ******************************************************************************/

void acpi_ut_delete_generic_state(union acpi_generic_state *state)
{
	ACPI_FUNCTION_ENTRY();

	/* Ignore null state */

	if (state) {
		(void)acpi_os_release_object(acpi_gbl_state_cache, state);
	}

	return;
}
