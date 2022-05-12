// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/*******************************************************************************
 *
 * Module Name: nsxfeval - Public interfaces to the ACPI subsystem
 *                         ACPI Object evaluation interfaces
 *
 ******************************************************************************/

#define EXPORT_ACPI_INTERFACES

#include <acpi/acpi.h>
#include "accommon.h"
#include "acnamesp.h"
#include "acinterp.h"

#define _COMPONENT          ACPI_NAMESPACE
ACPI_MODULE_NAME("nsxfeval")

/* Local prototypes */
static void acpi_ns_resolve_references(struct acpi_evaluate_info *info);

/*******************************************************************************
 *
 * FUNCTION:    acpi_evaluate_object_typed
 *
 * PARAMETERS:  handle              - Object handle (optional)
 *              pathname            - Object pathname (optional)
 *              external_params     - List of parameters to pass to a method,
 *                                    terminated by NULL. May be NULL
 *                                    if no parameters are being passed.
 *              return_buffer       - Where to put the object's return value (if
 *                                    any). If NULL, no value is returned.
 *              return_type         - Expected type of return object
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Find and evaluate the given object, passing the given
 *              parameters if necessary. One of "Handle" or "Pathname" must
 *              be valid (non-null)
 *
 ******************************************************************************/

acpi_status
acpi_evaluate_object_typed(acpi_handle handle,
			   acpi_string pathname,
			   struct acpi_object_list *external_params,
			   struct acpi_buffer *return_buffer,
			   acpi_object_type return_type)
{
	acpi_status status;
	u8 free_buffer_on_error = FALSE;
	acpi_handle target_handle;
	char *full_pathname;

	ACPI_FUNCTION_TRACE(acpi_evaluate_object_typed);

	/* Return buffer must be valid */

	if (!return_buffer) {
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}

	if (return_buffer->length == ACPI_ALLOCATE_BUFFER) {
		free_buffer_on_error = TRUE;
	}

	/* Get a handle here, in order to build an error message if needed */

	target_handle = handle;
	if (pathname) {
		status = acpi_get_handle(handle, pathname, &target_handle);
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}

	full_pathname = acpi_ns_get_external_pathname(target_handle);
	if (!full_pathname) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	/* Evaluate the object */

	status = acpi_evaluate_object(target_handle, NULL, external_params,
				      return_buffer);
	if (ACPI_FAILURE(status)) {
		goto exit;
	}

	/* Type ANY means "don't care about return value type" */

	if (return_type == ACPI_TYPE_ANY) {
		goto exit;
	}

	if (return_buffer->length == 0) {

		/* Error because caller specifically asked for a return value */

		ACPI_ERROR((AE_INFO, "%s did not return any object",
			    full_pathname));
		status = AE_NULL_OBJECT;
		goto exit;
	}

	/* Examine the object type returned from evaluate_object */

	if (((union acpi_object *)return_buffer->pointer)->type == return_type) {
		goto exit;
	}

	/* Return object type does not match requested type */

	ACPI_ERROR((AE_INFO,
		    "Incorrect return type from %s - received [%s], requested [%s]",
		    full_pathname,
		    acpi_ut_get_type_name(((union acpi_object *)return_buffer->
					   pointer)->type),
		    acpi_ut_get_type_name(return_type)));

	if (free_buffer_on_error) {
		/*
		 * Free a buffer created via ACPI_ALLOCATE_BUFFER.
		 * Note: We use acpi_os_free here because acpi_os_allocate was used
		 * to allocate the buffer. This purposefully bypasses the
		 * (optionally enabled) allocation tracking mechanism since we
		 * only want to track internal allocations.
		 */
		acpi_os_free(return_buffer->pointer);
		return_buffer->pointer = NULL;
	}

	return_buffer->length = 0;
	status = AE_TYPE;

exit:
	ACPI_FREE(full_pathname);
	return_ACPI_STATUS(status);
}

ACPI_EXPORT_SYMBOL(acpi_evaluate_object_typed)

/*******************************************************************************
 *
 * FUNCTION:    acpi_evaluate_object
 *
 * PARAMETERS:  handle              - Object handle (optional)
 *              pathname            - Object pathname (optional)
 *              external_params     - List of parameters to pass to method,
 *                                    terminated by NULL. May be NULL
 *                                    if no parameters are being passed.
 *              return_buffer       - Where to put method's return value (if
 *                                    any). If NULL, no value is returned.
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Find and evaluate the given object, passing the given
 *              parameters if necessary. One of "Handle" or "Pathname" must
 *              be valid (non-null)
 *
 ******************************************************************************/
acpi_status
acpi_evaluate_object(acpi_handle handle,
		     acpi_string pathname,
		     struct acpi_object_list *external_params,
		     struct acpi_buffer *return_buffer)
{
	acpi_status status;
	struct acpi_evaluate_info *info;
	acpi_size buffer_space_needed;
	u32 i;

	ACPI_FUNCTION_TRACE(acpi_evaluate_object);

	/* Allocate and initialize the evaluation information block */

	info = ACPI_ALLOCATE_ZEROED(sizeof(struct acpi_evaluate_info));
	if (!info) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	/* Convert and validate the device handle */

	info->prefix_node = acpi_ns_validate_handle(handle);
	if (!info->prefix_node) {
		status = AE_BAD_PARAMETER;
		goto cleanup;
	}

	/*
	 * Get the actual namespace node for the target object.
	 * Handles these cases:
	 *
	 * 1) Null node, valid pathname from root (absolute path)
	 * 2) Node and valid pathname (path relative to Node)
	 * 3) Node, Null pathname
	 */
	if ((pathname) && (ACPI_IS_ROOT_PREFIX(pathname[0]))) {

		/* The path is fully qualified, just evaluate by name */

		info->prefix_node = NULL;
	} else if (!handle) {
		/*
		 * A handle is optional iff a fully qualified pathname is specified.
		 * Since we've already handled fully qualified names above, this is
		 * an error.
		 */
		if (!pathname) {
			ACPI_DEBUG_PRINT((ACPI_DB_INFO,
					  "Both Handle and Pathname are NULL"));
		} else {
			ACPI_DEBUG_PRINT((ACPI_DB_INFO,
					  "Null Handle with relative pathname [%s]",
					  pathname));
		}

		status = AE_BAD_PARAMETER;
		goto cleanup;
	}

	info->relative_pathname = pathname;

	/*
	 * Convert all external objects passed as arguments to the
	 * internal version(s).
	 */
	if (external_params && external_params->count) {
		info->param_count = (u16)external_params->count;

		/* Warn on impossible argument count */

		if (info->param_count > ACPI_METHOD_NUM_ARGS) {
			ACPI_WARN_PREDEFINED((AE_INFO, pathname,
					      ACPI_WARN_ALWAYS,
					      "Excess arguments (%u) - using only %u",
					      info->param_count,
					      ACPI_METHOD_NUM_ARGS));

			info->param_count = ACPI_METHOD_NUM_ARGS;
		}

		/*
		 * Allocate a new parameter block for the internal objects
		 * Add 1 to count to allow for null terminated internal list
		 */
		info->parameters = ACPI_ALLOCATE_ZEROED(((acpi_size)info->
							 param_count +
							 1) * sizeof(void *));
		if (!info->parameters) {
			status = AE_NO_MEMORY;
			goto cleanup;
		}

		/* Convert each external object in the list to an internal object */

		for (i = 0; i < info->param_count; i++) {
			status =
			    acpi_ut_copy_eobject_to_iobject(&external_params->
							    pointer[i],
							    &info->
							    parameters[i]);
			if (ACPI_FAILURE(status)) {
				goto cleanup;
			}
		}

		info->parameters[info->param_count] = NULL;
	}

#ifdef _FUTURE_FEATURE

	/*
	 * Begin incoming argument count analysis. Check for too few args
	 * and too many args.
	 */
	switch (acpi_ns_get_type(info->node)) {
	case ACPI_TYPE_METHOD:

		/* Check incoming argument count against the method definition */

		if (info->obj_desc->method.param_count > info->param_count) {
			ACPI_ERROR((AE_INFO,
				    "Insufficient arguments (%u) - %u are required",
				    info->param_count,
				    info->obj_desc->method.param_count));

			status = AE_MISSING_ARGUMENTS;
			goto cleanup;
		}

		else if (info->obj_desc->method.param_count < info->param_count) {
			ACPI_WARNING((AE_INFO,
				      "Excess arguments (%u) - only %u are required",
				      info->param_count,
				      info->obj_desc->method.param_count));

			/* Just pass the required number of arguments */

			info->param_count = info->obj_desc->method.param_count;
		}

		/*
		 * Any incoming external objects to be passed as arguments to the
		 * method must be converted to internal objects
		 */
		if (info->param_count) {
			/*
			 * Allocate a new parameter block for the internal objects
			 * Add 1 to count to allow for null terminated internal list
			 */
			info->parameters = ACPI_ALLOCATE_ZEROED(((acpi_size)
								 info->
								 param_count +
								 1) *
								sizeof(void *));
			if (!info->parameters) {
				status = AE_NO_MEMORY;
				goto cleanup;
			}

			/* Convert each external object in the list to an internal object */

			for (i = 0; i < info->param_count; i++) {
				status =
				    acpi_ut_copy_eobject_to_iobject
				    (&external_params->pointer[i],
				     &info->parameters[i]);
				if (ACPI_FAILURE(status)) {
					goto cleanup;
				}
			}

			info->parameters[info->param_count] = NULL;
		}
		break;

	default:

		/* Warn if arguments passed to an object that is not a method */

		if (info->param_count) {
			ACPI_WARNING((AE_INFO,
				      "%u arguments were passed to a non-method ACPI object",
				      info->param_count));
		}
		break;
	}

#endif

	/* Now we can evaluate the object */

	status = acpi_ns_evaluate(info);

	/*
	 * If we are expecting a return value, and all went well above,
	 * copy the return value to an external object.
	 */
	if (!return_buffer) {
		goto cleanup_return_object;
	}

	if (!info->return_object) {
		return_buffer->length = 0;
		goto cleanup;
	}

	if (ACPI_GET_DESCRIPTOR_TYPE(info->return_object) ==
	    ACPI_DESC_TYPE_NAMED) {
		/*
		 * If we received a NS Node as a return object, this means that
		 * the object we are evaluating has nothing interesting to
		 * return (such as a mutex, etc.)  We return an error because
		 * these types are essentially unsupported by this interface.
		 * We don't check up front because this makes it easier to add
		 * support for various types at a later date if necessary.
		 */
		status = AE_TYPE;
		info->return_object = NULL;	/* No need to delete a NS Node */
		return_buffer->length = 0;
	}

	if (ACPI_FAILURE(status)) {
		goto cleanup_return_object;
	}

	/* Dereference Index and ref_of references */

	acpi_ns_resolve_references(info);

	/* Get the size of the returned object */

	status = acpi_ut_get_object_size(info->return_object,
					 &buffer_space_needed);
	if (ACPI_SUCCESS(status)) {

		/* Validate/Allocate/Clear caller buffer */

		status = acpi_ut_initialize_buffer(return_buffer,
						   buffer_space_needed);
		if (ACPI_FAILURE(status)) {
			/*
			 * Caller's buffer is too small or a new one can't
			 * be allocated
			 */
			ACPI_DEBUG_PRINT((ACPI_DB_INFO,
					  "Needed buffer size %X, %s\n",
					  (u32)buffer_space_needed,
					  acpi_format_exception(status)));
		} else {
			/* We have enough space for the object, build it */

			status =
			    acpi_ut_copy_iobject_to_eobject(info->return_object,
							    return_buffer);
		}
	}

cleanup_return_object:

	if (info->return_object) {
		/*
		 * Delete the internal return object. NOTE: Interpreter must be
		 * locked to avoid race condition.
		 */
		acpi_ex_enter_interpreter();

		/* Remove one reference on the return object (should delete it) */

		acpi_ut_remove_reference(info->return_object);
		acpi_ex_exit_interpreter();
	}

cleanup:

	/* Free the input parameter list (if we created one) */

	if (info->parameters) {

		/* Free the allocated parameter block */

		acpi_ut_delete_internal_object_list(info->parameters);
	}

	ACPI_FREE(info);
	return_ACPI_STATUS(status);
}

ACPI_EXPORT_SYMBOL(acpi_evaluate_object)

/*******************************************************************************
 *
 * FUNCTION:    acpi_ns_resolve_references
 *
 * PARAMETERS:  info                    - Evaluation info block
 *
 * RETURN:      Info->return_object is replaced with the dereferenced object
 *
 * DESCRIPTION: Dereference certain reference objects. Called before an
 *              internal return object is converted to an external union acpi_object.
 *
 * Performs an automatic dereference of Index and ref_of reference objects.
 * These reference objects are not supported by the union acpi_object, so this is a
 * last resort effort to return something useful. Also, provides compatibility
 * with other ACPI implementations.
 *
 * NOTE: does not handle references within returned package objects or nested
 * references, but this support could be added later if found to be necessary.
 *
 ******************************************************************************/
static void acpi_ns_resolve_references(struct acpi_evaluate_info *info)
{
	union acpi_operand_object *obj_desc = NULL;
	struct acpi_namespace_node *node;

	/* We are interested in reference objects only */

	if ((info->return_object)->common.type != ACPI_TYPE_LOCAL_REFERENCE) {
		return;
	}

	/*
	 * Two types of references are supported - those created by Index and
	 * ref_of operators. A name reference (AML_NAMEPATH_OP) can be converted
	 * to a union acpi_object, so it is not dereferenced here. A ddb_handle
	 * (AML_LOAD_OP) cannot be dereferenced, nor can it be converted to
	 * a union acpi_object.
	 */
	switch (info->return_object->reference.class) {
	case ACPI_REFCLASS_INDEX:

		obj_desc = *(info->return_object->reference.where);
		break;

	case ACPI_REFCLASS_REFOF:

		node = info->return_object->reference.object;
		if (node) {
			obj_desc = node->object;
		}
		break;

	default:

		return;
	}

	/* Replace the existing reference object */

	if (obj_desc) {
		acpi_ut_add_reference(obj_desc);
		acpi_ut_remove_reference(info->return_object);
		info->return_object = obj_desc;
	}

	return;
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_walk_namespace
 *
 * PARAMETERS:  type                - acpi_object_type to search for
 *              start_object        - Handle in namespace where search begins
 *              max_depth           - Depth to which search is to reach
 *              descending_callback - Called during tree descent
 *                                    when an object of "Type" is found
 *              ascending_callback  - Called during tree ascent
 *                                    when an object of "Type" is found
 *              context             - Passed to user function(s) above
 *              return_value        - Location where return value of
 *                                    user_function is put if terminated early
 *
 * RETURNS      Return value from the user_function if terminated early.
 *              Otherwise, returns NULL.
 *
 * DESCRIPTION: Performs a modified depth-first walk of the namespace tree,
 *              starting (and ending) at the object specified by start_handle.
 *              The callback function is called whenever an object that matches
 *              the type parameter is found. If the callback function returns
 *              a non-zero value, the search is terminated immediately and this
 *              value is returned to the caller.
 *
 *              The point of this procedure is to provide a generic namespace
 *              walk routine that can be called from multiple places to
 *              provide multiple services; the callback function(s) can be
 *              tailored to each task, whether it is a print function,
 *              a compare function, etc.
 *
 ******************************************************************************/

acpi_status
acpi_walk_namespace(acpi_object_type type,
		    acpi_handle start_object,
		    u32 max_depth,
		    acpi_walk_callback descending_callback,
		    acpi_walk_callback ascending_callback,
		    void *context, void **return_value)
{
	acpi_status status;

	ACPI_FUNCTION_TRACE(acpi_walk_namespace);

	/* Parameter validation */

	if ((type > ACPI_TYPE_LOCAL_MAX) ||
	    (!max_depth) || (!descending_callback && !ascending_callback)) {
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}

	/*
	 * Need to acquire the namespace reader lock to prevent interference
	 * with any concurrent table unloads (which causes the deletion of
	 * namespace objects). We cannot allow the deletion of a namespace node
	 * while the user function is using it. The exception to this are the
	 * nodes created and deleted during control method execution -- these
	 * nodes are marked as temporary nodes and are ignored by the namespace
	 * walk. Thus, control methods can be executed while holding the
	 * namespace deletion lock (and the user function can execute control
	 * methods.)
	 */
	status = acpi_ut_acquire_read_lock(&acpi_gbl_namespace_rw_lock);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * Lock the namespace around the walk. The namespace will be
	 * unlocked/locked around each call to the user function - since the user
	 * function must be allowed to make ACPICA calls itself (for example, it
	 * will typically execute control methods during device enumeration.)
	 */
	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		goto unlock_and_exit;
	}

	/* Now we can validate the starting node */

	if (!acpi_ns_validate_handle(start_object)) {
		status = AE_BAD_PARAMETER;
		goto unlock_and_exit2;
	}

	status = acpi_ns_walk_namespace(type, start_object, max_depth,
					ACPI_NS_WALK_UNLOCK,
					descending_callback, ascending_callback,
					context, return_value);

unlock_and_exit2:
	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);

unlock_and_exit:
	(void)acpi_ut_release_read_lock(&acpi_gbl_namespace_rw_lock);
	return_ACPI_STATUS(status);
}

ACPI_EXPORT_SYMBOL(acpi_walk_namespace)

/*******************************************************************************
 *
 * FUNCTION:    acpi_ns_get_device_callback
 *
 * PARAMETERS:  Callback from acpi_get_device
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Takes callbacks from walk_namespace and filters out all non-
 *              present devices, or if they specified a HID, it filters based
 *              on that.
 *
 ******************************************************************************/
static acpi_status
acpi_ns_get_device_callback(acpi_handle obj_handle,
			    u32 nesting_level,
			    void *context, void **return_value)
{
	struct acpi_get_devices_info *info = context;
	acpi_status status;
	struct acpi_namespace_node *node;
	u32 flags;
	struct acpi_pnp_device_id *hid;
	struct acpi_pnp_device_id_list *cid;
	u32 i;
	u8 found;
	int no_match;

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	node = acpi_ns_validate_handle(obj_handle);
	status = acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	if (!node) {
		return (AE_BAD_PARAMETER);
	}

	/*
	 * First, filter based on the device HID and CID.
	 *
	 * 01/2010: For this case where a specific HID is requested, we don't
	 * want to run _STA until we have an actual HID match. Thus, we will
	 * not unnecessarily execute _STA on devices for which the caller
	 * doesn't care about. Previously, _STA was executed unconditionally
	 * on all devices found here.
	 *
	 * A side-effect of this change is that now we will continue to search
	 * for a matching HID even under device trees where the parent device
	 * would have returned a _STA that indicates it is not present or
	 * not functioning (thus aborting the search on that branch).
	 */
	if (info->hid != NULL) {
		status = acpi_ut_execute_HID(node, &hid);
		if (status == AE_NOT_FOUND) {
			return (AE_OK);
		} else if (ACPI_FAILURE(status)) {
			return (AE_CTRL_DEPTH);
		}

		no_match = strcmp(hid->string, info->hid);
		ACPI_FREE(hid);

		if (no_match) {
			/*
			 * HID does not match, attempt match within the
			 * list of Compatible IDs (CIDs)
			 */
			status = acpi_ut_execute_CID(node, &cid);
			if (status == AE_NOT_FOUND) {
				return (AE_OK);
			} else if (ACPI_FAILURE(status)) {
				return (AE_CTRL_DEPTH);
			}

			/* Walk the CID list */

			found = FALSE;
			for (i = 0; i < cid->count; i++) {
				if (strcmp(cid->ids[i].string, info->hid) == 0) {

					/* Found a matching CID */

					found = TRUE;
					break;
				}
			}

			ACPI_FREE(cid);
			if (!found) {
				return (AE_OK);
			}
		}
	}

	/* Run _STA to determine if device is present */

	status = acpi_ut_execute_STA(node, &flags);
	if (ACPI_FAILURE(status)) {
		return (AE_CTRL_DEPTH);
	}

	if (!(flags & ACPI_STA_DEVICE_PRESENT) &&
	    !(flags & ACPI_STA_DEVICE_FUNCTIONING)) {
		/*
		 * Don't examine the children of the device only when the
		 * device is neither present nor functional. See ACPI spec,
		 * description of _STA for more information.
		 */
		return (AE_CTRL_DEPTH);
	}

	/* We have a valid device, invoke the user function */

	status = info->user_function(obj_handle, nesting_level,
				     info->context, return_value);
	return (status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_get_devices
 *
 * PARAMETERS:  HID                 - HID to search for. Can be NULL.
 *              user_function       - Called when a matching object is found
 *              context             - Passed to user function
 *              return_value        - Location where return value of
 *                                    user_function is put if terminated early
 *
 * RETURNS      Return value from the user_function if terminated early.
 *              Otherwise, returns NULL.
 *
 * DESCRIPTION: Performs a modified depth-first walk of the namespace tree,
 *              starting (and ending) at the object specified by start_handle.
 *              The user_function is called whenever an object of type
 *              Device is found. If the user function returns
 *              a non-zero value, the search is terminated immediately and this
 *              value is returned to the caller.
 *
 *              This is a wrapper for walk_namespace, but the callback performs
 *              additional filtering. Please see acpi_ns_get_device_callback.
 *
 ******************************************************************************/

acpi_status
acpi_get_devices(const char *HID,
		 acpi_walk_callback user_function,
		 void *context, void **return_value)
{
	acpi_status status;
	struct acpi_get_devices_info info;

	ACPI_FUNCTION_TRACE(acpi_get_devices);

	/* Parameter validation */

	if (!user_function) {
		return_ACPI_STATUS(AE_BAD_PARAMETER);
	}

	/*
	 * We're going to call their callback from OUR callback, so we need
	 * to know what it is, and their context parameter.
	 */
	info.hid = HID;
	info.context = context;
	info.user_function = user_function;

	/*
	 * Lock the namespace around the walk.
	 * The namespace will be unlocked/locked around each call
	 * to the user function - since this function
	 * must be allowed to make Acpi calls itself.
	 */
	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	status = acpi_ns_walk_namespace(ACPI_TYPE_DEVICE, ACPI_ROOT_OBJECT,
					ACPI_UINT32_MAX, ACPI_NS_WALK_UNLOCK,
					acpi_ns_get_device_callback, NULL,
					&info, return_value);

	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	return_ACPI_STATUS(status);
}

ACPI_EXPORT_SYMBOL(acpi_get_devices)

/*******************************************************************************
 *
 * FUNCTION:    acpi_attach_data
 *
 * PARAMETERS:  obj_handle          - Namespace node
 *              handler             - Handler for this attachment
 *              data                - Pointer to data to be attached
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Attach arbitrary data and handler to a namespace node.
 *
 ******************************************************************************/
acpi_status
acpi_attach_data(acpi_handle obj_handle,
		 acpi_object_handler handler, void *data)
{
	struct acpi_namespace_node *node;
	acpi_status status;

	/* Parameter validation */

	if (!obj_handle || !handler || !data) {
		return (AE_BAD_PARAMETER);
	}

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/* Convert and validate the handle */

	node = acpi_ns_validate_handle(obj_handle);
	if (!node) {
		status = AE_BAD_PARAMETER;
		goto unlock_and_exit;
	}

	status = acpi_ns_attach_data(node, handler, data);

unlock_and_exit:
	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	return (status);
}

ACPI_EXPORT_SYMBOL(acpi_attach_data)

/*******************************************************************************
 *
 * FUNCTION:    acpi_detach_data
 *
 * PARAMETERS:  obj_handle          - Namespace node handle
 *              handler             - Handler used in call to acpi_attach_data
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Remove data that was previously attached to a node.
 *
 ******************************************************************************/
acpi_status
acpi_detach_data(acpi_handle obj_handle, acpi_object_handler handler)
{
	struct acpi_namespace_node *node;
	acpi_status status;

	/* Parameter validation */

	if (!obj_handle || !handler) {
		return (AE_BAD_PARAMETER);
	}

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/* Convert and validate the handle */

	node = acpi_ns_validate_handle(obj_handle);
	if (!node) {
		status = AE_BAD_PARAMETER;
		goto unlock_and_exit;
	}

	status = acpi_ns_detach_data(node, handler);

unlock_and_exit:
	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	return (status);
}

ACPI_EXPORT_SYMBOL(acpi_detach_data)

/*******************************************************************************
 *
 * FUNCTION:    acpi_get_data_full
 *
 * PARAMETERS:  obj_handle          - Namespace node
 *              handler             - Handler used in call to attach_data
 *              data                - Where the data is returned
 *              callback            - function to execute before returning
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Retrieve data that was previously attached to a namespace node
 *              and execute a callback before returning.
 *
 ******************************************************************************/
acpi_status
acpi_get_data_full(acpi_handle obj_handle, acpi_object_handler handler,
		   void **data, void (*callback)(void *))
{
	struct acpi_namespace_node *node;
	acpi_status status;

	/* Parameter validation */

	if (!obj_handle || !handler || !data) {
		return (AE_BAD_PARAMETER);
	}

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/* Convert and validate the handle */

	node = acpi_ns_validate_handle(obj_handle);
	if (!node) {
		status = AE_BAD_PARAMETER;
		goto unlock_and_exit;
	}

	status = acpi_ns_get_attached_data(node, handler, data);
	if (ACPI_SUCCESS(status) && callback) {
		callback(*data);
	}

unlock_and_exit:
	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	return (status);
}

ACPI_EXPORT_SYMBOL(acpi_get_data_full)

/*******************************************************************************
 *
 * FUNCTION:    acpi_get_data
 *
 * PARAMETERS:  obj_handle          - Namespace node
 *              handler             - Handler used in call to attach_data
 *              data                - Where the data is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Retrieve data that was previously attached to a namespace node.
 *
 ******************************************************************************/
acpi_status
acpi_get_data(acpi_handle obj_handle, acpi_object_handler handler, void **data)
{
	return acpi_get_data_full(obj_handle, handler, data, NULL);
}

ACPI_EXPORT_SYMBOL(acpi_get_data)
