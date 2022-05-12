// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Module Name: psxface - Parser external interfaces
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"
#include "acparser.h"
#include "acdispat.h"
#include "acinterp.h"
#include "actables.h"
#include "acnamesp.h"

#define _COMPONENT          ACPI_PARSER
ACPI_MODULE_NAME("psxface")

/* Local Prototypes */
static void
acpi_ps_update_parameter_list(struct acpi_evaluate_info *info, u16 action);

/*******************************************************************************
 *
 * FUNCTION:    acpi_debug_trace
 *
 * PARAMETERS:  method_name     - Valid ACPI name string
 *              debug_level     - Optional level mask. 0 to use default
 *              debug_layer     - Optional layer mask. 0 to use default
 *              flags           - bit 1: one shot(1) or persistent(0)
 *
 * RETURN:      Status
 *
 * DESCRIPTION: External interface to enable debug tracing during control
 *              method execution
 *
 ******************************************************************************/

acpi_status
acpi_debug_trace(const char *name, u32 debug_level, u32 debug_layer, u32 flags)
{
	acpi_status status;

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	acpi_gbl_trace_method_name = name;
	acpi_gbl_trace_flags = flags;
	acpi_gbl_trace_dbg_level = debug_level;
	acpi_gbl_trace_dbg_layer = debug_layer;
	status = AE_OK;

	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	return (status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ps_execute_method
 *
 * PARAMETERS:  info            - Method info block, contains:
 *                  node            - Method Node to execute
 *                  obj_desc        - Method object
 *                  parameters      - List of parameters to pass to the method,
 *                                    terminated by NULL. Params itself may be
 *                                    NULL if no parameters are being passed.
 *                  return_object   - Where to put method's return value (if
 *                                    any). If NULL, no value is returned.
 *                  parameter_type  - Type of Parameter list
 *                  return_object   - Where to put method's return value (if
 *                                    any). If NULL, no value is returned.
 *                  pass_number     - Parse or execute pass
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Execute a control method
 *
 ******************************************************************************/

acpi_status acpi_ps_execute_method(struct acpi_evaluate_info *info)
{
	acpi_status status;
	union acpi_parse_object *op;
	struct acpi_walk_state *walk_state;

	ACPI_FUNCTION_TRACE(ps_execute_method);

	/* Quick validation of DSDT header */

	acpi_tb_check_dsdt_header();

	/* Validate the Info and method Node */

	if (!info || !info->node) {
		return_ACPI_STATUS(AE_NULL_ENTRY);
	}

	/* Init for new method, wait on concurrency semaphore */

	status =
	    acpi_ds_begin_method_execution(info->node, info->obj_desc, NULL);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * The caller "owns" the parameters, so give each one an extra reference
	 */
	acpi_ps_update_parameter_list(info, REF_INCREMENT);

	/*
	 * Execute the method. Performs parse simultaneously
	 */
	ACPI_DEBUG_PRINT((ACPI_DB_PARSE,
			  "**** Begin Method Parse/Execute [%4.4s] **** Node=%p Obj=%p\n",
			  info->node->name.ascii, info->node, info->obj_desc));

	/* Create and init a Root Node */

	op = acpi_ps_create_scope_op(info->obj_desc->method.aml_start);
	if (!op) {
		status = AE_NO_MEMORY;
		goto cleanup;
	}

	/* Create and initialize a new walk state */

	info->pass_number = ACPI_IMODE_EXECUTE;
	walk_state =
	    acpi_ds_create_walk_state(info->obj_desc->method.owner_id, NULL,
				      NULL, NULL);
	if (!walk_state) {
		status = AE_NO_MEMORY;
		goto cleanup;
	}

	status = acpi_ds_init_aml_walk(walk_state, op, info->node,
				       info->obj_desc->method.aml_start,
				       info->obj_desc->method.aml_length, info,
				       info->pass_number);
	if (ACPI_FAILURE(status)) {
		acpi_ds_delete_walk_state(walk_state);
		goto cleanup;
	}

	walk_state->method_pathname = info->full_pathname;
	walk_state->method_is_nested = FALSE;

	if (info->obj_desc->method.info_flags & ACPI_METHOD_MODULE_LEVEL) {
		walk_state->parse_flags |= ACPI_PARSE_MODULE_LEVEL;
	}

	/* Invoke an internal method if necessary */

	if (info->obj_desc->method.info_flags & ACPI_METHOD_INTERNAL_ONLY) {
		status =
		    info->obj_desc->method.dispatch.implementation(walk_state);
		info->return_object = walk_state->return_desc;

		/* Cleanup states */

		acpi_ds_scope_stack_clear(walk_state);
		acpi_ps_cleanup_scope(&walk_state->parser_state);
		acpi_ds_terminate_control_method(walk_state->method_desc,
						 walk_state);
		acpi_ds_delete_walk_state(walk_state);
		goto cleanup;
	}

	/*
	 * Start method evaluation with an implicit return of zero.
	 * This is done for Windows compatibility.
	 */
	if (acpi_gbl_enable_interpreter_slack) {
		walk_state->implicit_return_obj =
		    acpi_ut_create_integer_object((u64) 0);
		if (!walk_state->implicit_return_obj) {
			status = AE_NO_MEMORY;
			acpi_ds_delete_walk_state(walk_state);
			goto cleanup;
		}
	}

	/* Parse the AML */

	status = acpi_ps_parse_aml(walk_state);

	/* walk_state was deleted by parse_aml */

cleanup:
	acpi_ps_delete_parse_tree(op);

	/* Take away the extra reference that we gave the parameters above */

	acpi_ps_update_parameter_list(info, REF_DECREMENT);

	/* Exit now if error above */

	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * If the method has returned an object, signal this to the caller with
	 * a control exception code
	 */
	if (info->return_object) {
		ACPI_DEBUG_PRINT((ACPI_DB_PARSE, "Method returned ObjDesc=%p\n",
				  info->return_object));
		ACPI_DUMP_STACK_ENTRY(info->return_object);

		status = AE_CTRL_RETURN_VALUE;
	}

	return_ACPI_STATUS(status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ps_execute_table
 *
 * PARAMETERS:  info            - Method info block, contains:
 *              node            - Node to where the is entered into the
 *                                namespace
 *              obj_desc        - Pseudo method object describing the AML
 *                                code of the entire table
 *              pass_number     - Parse or execute pass
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Execute a table
 *
 ******************************************************************************/

acpi_status acpi_ps_execute_table(struct acpi_evaluate_info *info)
{
	acpi_status status;
	union acpi_parse_object *op = NULL;
	struct acpi_walk_state *walk_state = NULL;

	ACPI_FUNCTION_TRACE(ps_execute_table);

	/* Create and init a Root Node */

	op = acpi_ps_create_scope_op(info->obj_desc->method.aml_start);
	if (!op) {
		status = AE_NO_MEMORY;
		goto cleanup;
	}

	/* Create and initialize a new walk state */

	walk_state =
	    acpi_ds_create_walk_state(info->obj_desc->method.owner_id, NULL,
				      NULL, NULL);
	if (!walk_state) {
		status = AE_NO_MEMORY;
		goto cleanup;
	}

	status = acpi_ds_init_aml_walk(walk_state, op, info->node,
				       info->obj_desc->method.aml_start,
				       info->obj_desc->method.aml_length, info,
				       info->pass_number);
	if (ACPI_FAILURE(status)) {
		goto cleanup;
	}

	walk_state->method_pathname = info->full_pathname;
	walk_state->method_is_nested = FALSE;

	if (info->obj_desc->method.info_flags & ACPI_METHOD_MODULE_LEVEL) {
		walk_state->parse_flags |= ACPI_PARSE_MODULE_LEVEL;
	}

	/* Info->Node is the default location to load the table  */

	if (info->node && info->node != acpi_gbl_root_node) {
		status =
		    acpi_ds_scope_stack_push(info->node, ACPI_TYPE_METHOD,
					     walk_state);
		if (ACPI_FAILURE(status)) {
			goto cleanup;
		}
	}

	/*
	 * Parse the AML, walk_state will be deleted by parse_aml
	 */
	acpi_ex_enter_interpreter();
	status = acpi_ps_parse_aml(walk_state);
	acpi_ex_exit_interpreter();
	walk_state = NULL;

cleanup:
	if (walk_state) {
		acpi_ds_delete_walk_state(walk_state);
	}
	if (op) {
		acpi_ps_delete_parse_tree(op);
	}
	return_ACPI_STATUS(status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ps_update_parameter_list
 *
 * PARAMETERS:  info            - See struct acpi_evaluate_info
 *                                (Used: parameter_type and Parameters)
 *              action          - Add or Remove reference
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Update reference count on all method parameter objects
 *
 ******************************************************************************/

static void
acpi_ps_update_parameter_list(struct acpi_evaluate_info *info, u16 action)
{
	u32 i;

	if (info->parameters) {

		/* Update reference count for each parameter */

		for (i = 0; info->parameters[i]; i++) {

			/* Ignore errors, just do them all */

			(void)acpi_ut_update_object_reference(info->
							      parameters[i],
							      action);
		}
	}
}
