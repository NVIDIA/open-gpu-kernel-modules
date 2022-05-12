// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Module Name: utalloc - local memory allocation routines
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"
#include "acdebug.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utalloc")

#if !defined (USE_NATIVE_ALLOCATE_ZEROED)
/*******************************************************************************
 *
 * FUNCTION:    acpi_os_allocate_zeroed
 *
 * PARAMETERS:  size                - Size of the allocation
 *
 * RETURN:      Address of the allocated memory on success, NULL on failure.
 *
 * DESCRIPTION: Subsystem equivalent of calloc. Allocate and zero memory.
 *              This is the default implementation. Can be overridden via the
 *              USE_NATIVE_ALLOCATE_ZEROED flag.
 *
 ******************************************************************************/
void *acpi_os_allocate_zeroed(acpi_size size)
{
	void *allocation;

	ACPI_FUNCTION_ENTRY();

	allocation = acpi_os_allocate(size);
	if (allocation) {

		/* Clear the memory block */

		memset(allocation, 0, size);
	}

	return (allocation);
}

#endif				/* !USE_NATIVE_ALLOCATE_ZEROED */

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_create_caches
 *
 * PARAMETERS:  None
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Create all local caches
 *
 ******************************************************************************/

acpi_status acpi_ut_create_caches(void)
{
	acpi_status status;

	/* Object Caches, for frequently used objects */

	status =
	    acpi_os_create_cache("Acpi-Namespace",
				 sizeof(struct acpi_namespace_node),
				 ACPI_MAX_NAMESPACE_CACHE_DEPTH,
				 &acpi_gbl_namespace_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	status =
	    acpi_os_create_cache("Acpi-State", sizeof(union acpi_generic_state),
				 ACPI_MAX_STATE_CACHE_DEPTH,
				 &acpi_gbl_state_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	status =
	    acpi_os_create_cache("Acpi-Parse",
				 sizeof(struct acpi_parse_obj_common),
				 ACPI_MAX_PARSE_CACHE_DEPTH,
				 &acpi_gbl_ps_node_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	status =
	    acpi_os_create_cache("Acpi-ParseExt",
				 sizeof(struct acpi_parse_obj_named),
				 ACPI_MAX_EXTPARSE_CACHE_DEPTH,
				 &acpi_gbl_ps_node_ext_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	status =
	    acpi_os_create_cache("Acpi-Operand",
				 sizeof(union acpi_operand_object),
				 ACPI_MAX_OBJECT_CACHE_DEPTH,
				 &acpi_gbl_operand_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}
#ifdef ACPI_ASL_COMPILER
	/*
	 * For use with the ASL-/ASL+ option. This cache keeps track of regular
	 * 0xA9 0x01 comments.
	 */
	status =
	    acpi_os_create_cache("Acpi-Comment",
				 sizeof(struct acpi_comment_node),
				 ACPI_MAX_COMMENT_CACHE_DEPTH,
				 &acpi_gbl_reg_comment_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/*
	 * This cache keeps track of the starting addresses of where the comments
	 * lie. This helps prevent duplication of comments.
	 */
	status =
	    acpi_os_create_cache("Acpi-Comment-Addr",
				 sizeof(struct acpi_comment_addr_node),
				 ACPI_MAX_COMMENT_CACHE_DEPTH,
				 &acpi_gbl_comment_addr_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/*
	 * This cache will be used for nodes that represent files.
	 */
	status =
	    acpi_os_create_cache("Acpi-File", sizeof(struct acpi_file_node),
				 ACPI_MAX_COMMENT_CACHE_DEPTH,
				 &acpi_gbl_file_cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}
#endif

#ifdef ACPI_DBG_TRACK_ALLOCATIONS

	/* Memory allocation lists */

	status = acpi_ut_create_list("Acpi-Global", 0, &acpi_gbl_global_list);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	status =
	    acpi_ut_create_list("Acpi-Namespace",
				sizeof(struct acpi_namespace_node),
				&acpi_gbl_ns_node_list);
	if (ACPI_FAILURE(status)) {
		return (status);
	}
#endif

	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_delete_caches
 *
 * PARAMETERS:  None
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Purge and delete all local caches
 *
 ******************************************************************************/

acpi_status acpi_ut_delete_caches(void)
{
#ifdef ACPI_DBG_TRACK_ALLOCATIONS
	char buffer[7];

	if (acpi_gbl_display_final_mem_stats) {
		strcpy(buffer, "MEMORY");
		(void)acpi_db_display_statistics(buffer);
	}
#endif

	(void)acpi_os_delete_cache(acpi_gbl_namespace_cache);
	acpi_gbl_namespace_cache = NULL;

	(void)acpi_os_delete_cache(acpi_gbl_state_cache);
	acpi_gbl_state_cache = NULL;

	(void)acpi_os_delete_cache(acpi_gbl_operand_cache);
	acpi_gbl_operand_cache = NULL;

	(void)acpi_os_delete_cache(acpi_gbl_ps_node_cache);
	acpi_gbl_ps_node_cache = NULL;

	(void)acpi_os_delete_cache(acpi_gbl_ps_node_ext_cache);
	acpi_gbl_ps_node_ext_cache = NULL;

#ifdef ACPI_ASL_COMPILER
	(void)acpi_os_delete_cache(acpi_gbl_reg_comment_cache);
	acpi_gbl_reg_comment_cache = NULL;

	(void)acpi_os_delete_cache(acpi_gbl_comment_addr_cache);
	acpi_gbl_comment_addr_cache = NULL;

	(void)acpi_os_delete_cache(acpi_gbl_file_cache);
	acpi_gbl_file_cache = NULL;
#endif

#ifdef ACPI_DBG_TRACK_ALLOCATIONS

	/* Debug only - display leftover memory allocation, if any */

	acpi_ut_dump_allocations(ACPI_UINT32_MAX, NULL);

	/* Free memory lists */

	acpi_os_free(acpi_gbl_global_list);
	acpi_gbl_global_list = NULL;

	acpi_os_free(acpi_gbl_ns_node_list);
	acpi_gbl_ns_node_list = NULL;
#endif

	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_validate_buffer
 *
 * PARAMETERS:  buffer              - Buffer descriptor to be validated
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Perform parameter validation checks on an struct acpi_buffer
 *
 ******************************************************************************/

acpi_status acpi_ut_validate_buffer(struct acpi_buffer *buffer)
{

	/* Obviously, the structure pointer must be valid */

	if (!buffer) {
		return (AE_BAD_PARAMETER);
	}

	/* Special semantics for the length */

	if ((buffer->length == ACPI_NO_BUFFER) ||
	    (buffer->length == ACPI_ALLOCATE_BUFFER) ||
	    (buffer->length == ACPI_ALLOCATE_LOCAL_BUFFER)) {
		return (AE_OK);
	}

	/* Length is valid, the buffer pointer must be also */

	if (!buffer->pointer) {
		return (AE_BAD_PARAMETER);
	}

	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_initialize_buffer
 *
 * PARAMETERS:  buffer              - Buffer to be validated
 *              required_length     - Length needed
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Validate that the buffer is of the required length or
 *              allocate a new buffer. Returned buffer is always zeroed.
 *
 ******************************************************************************/

acpi_status
acpi_ut_initialize_buffer(struct acpi_buffer *buffer, acpi_size required_length)
{
	acpi_size input_buffer_length;

	/* Parameter validation */

	if (!buffer || !required_length) {
		return (AE_BAD_PARAMETER);
	}

	/*
	 * Buffer->Length is used as both an input and output parameter. Get the
	 * input actual length and set the output required buffer length.
	 */
	input_buffer_length = buffer->length;
	buffer->length = required_length;

	/*
	 * The input buffer length contains the actual buffer length, or the type
	 * of buffer to be allocated by this routine.
	 */
	switch (input_buffer_length) {
	case ACPI_NO_BUFFER:

		/* Return the exception (and the required buffer length) */

		return (AE_BUFFER_OVERFLOW);

	case ACPI_ALLOCATE_BUFFER:
		/*
		 * Allocate a new buffer. We directectly call acpi_os_allocate here to
		 * purposefully bypass the (optionally enabled) internal allocation
		 * tracking mechanism since we only want to track internal
		 * allocations. Note: The caller should use acpi_os_free to free this
		 * buffer created via ACPI_ALLOCATE_BUFFER.
		 */
		buffer->pointer = acpi_os_allocate(required_length);
		break;

	case ACPI_ALLOCATE_LOCAL_BUFFER:

		/* Allocate a new buffer with local interface to allow tracking */

		buffer->pointer = ACPI_ALLOCATE(required_length);
		break;

	default:

		/* Existing buffer: Validate the size of the buffer */

		if (input_buffer_length < required_length) {
			return (AE_BUFFER_OVERFLOW);
		}
		break;
	}

	/* Validate allocation from above or input buffer pointer */

	if (!buffer->pointer) {
		return (AE_NO_MEMORY);
	}

	/* Have a valid buffer, clear it */

	memset(buffer->pointer, 0, required_length);
	return (AE_OK);
}
