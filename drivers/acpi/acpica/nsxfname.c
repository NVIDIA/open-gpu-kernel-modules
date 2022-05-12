// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Module Name: nsxfname - Public interfaces to the ACPI subsystem
 *                         ACPI Namespace oriented interfaces
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#define EXPORT_ACPI_INTERFACES

#include <acpi/acpi.h>
#include "accommon.h"
#include "acnamesp.h"
#include "acparser.h"
#include "amlcode.h"

#define _COMPONENT          ACPI_NAMESPACE
ACPI_MODULE_NAME("nsxfname")

/* Local prototypes */
static char *acpi_ns_copy_device_id(struct acpi_pnp_device_id *dest,
				    struct acpi_pnp_device_id *source,
				    char *string_area);

/******************************************************************************
 *
 * FUNCTION:    acpi_get_handle
 *
 * PARAMETERS:  parent          - Object to search under (search scope).
 *              pathname        - Pointer to an asciiz string containing the
 *                                name
 *              ret_handle      - Where the return handle is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This routine will search for a caller specified name in the
 *              name space. The caller can restrict the search region by
 *              specifying a non NULL parent. The parent value is itself a
 *              namespace handle.
 *
 ******************************************************************************/

acpi_status
acpi_get_handle(acpi_handle parent,
		acpi_string pathname, acpi_handle *ret_handle)
{
	acpi_status status;
	struct acpi_namespace_node *node = NULL;
	struct acpi_namespace_node *prefix_node = NULL;

	ACPI_FUNCTION_ENTRY();

	/* Parameter Validation */

	if (!ret_handle || !pathname) {
		return (AE_BAD_PARAMETER);
	}

	/* Convert a parent handle to a prefix node */

	if (parent) {
		prefix_node = acpi_ns_validate_handle(parent);
		if (!prefix_node) {
			return (AE_BAD_PARAMETER);
		}
	}

	/*
	 * Valid cases are:
	 * 1) Fully qualified pathname
	 * 2) Parent + Relative pathname
	 *
	 * Error for <null Parent + relative path>
	 */
	if (ACPI_IS_ROOT_PREFIX(pathname[0])) {

		/* Pathname is fully qualified (starts with '\') */

		/* Special case for root-only, since we can't search for it */

		if (!strcmp(pathname, ACPI_NS_ROOT_PATH)) {
			*ret_handle =
			    ACPI_CAST_PTR(acpi_handle, acpi_gbl_root_node);
			return (AE_OK);
		}
	} else if (!prefix_node) {

		/* Relative path with null prefix is disallowed */

		return (AE_BAD_PARAMETER);
	}

	/* Find the Node and convert to a handle */

	status =
	    acpi_ns_get_node(prefix_node, pathname, ACPI_NS_NO_UPSEARCH, &node);
	if (ACPI_SUCCESS(status)) {
		*ret_handle = ACPI_CAST_PTR(acpi_handle, node);
	}

	return (status);
}

ACPI_EXPORT_SYMBOL(acpi_get_handle)

/******************************************************************************
 *
 * FUNCTION:    acpi_get_name
 *
 * PARAMETERS:  handle          - Handle to be converted to a pathname
 *              name_type       - Full pathname or single segment
 *              buffer          - Buffer for returned path
 *
 * RETURN:      Pointer to a string containing the fully qualified Name.
 *
 * DESCRIPTION: This routine returns the fully qualified name associated with
 *              the Handle parameter. This and the acpi_pathname_to_handle are
 *              complementary functions.
 *
 ******************************************************************************/
acpi_status
acpi_get_name(acpi_handle handle, u32 name_type, struct acpi_buffer *buffer)
{
	acpi_status status;

	/* Parameter validation */

	if (name_type > ACPI_NAME_TYPE_MAX) {
		return (AE_BAD_PARAMETER);
	}

	status = acpi_ut_validate_buffer(buffer);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/*
	 * Wants the single segment ACPI name.
	 * Validate handle and convert to a namespace Node
	 */
	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	if (name_type == ACPI_FULL_PATHNAME ||
	    name_type == ACPI_FULL_PATHNAME_NO_TRAILING) {

		/* Get the full pathname (From the namespace root) */

		status = acpi_ns_handle_to_pathname(handle, buffer,
						    name_type ==
						    ACPI_FULL_PATHNAME ? FALSE :
						    TRUE);
	} else {
		/* Get the single name */

		status = acpi_ns_handle_to_name(handle, buffer);
	}

	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	return (status);
}

ACPI_EXPORT_SYMBOL(acpi_get_name)

/******************************************************************************
 *
 * FUNCTION:    acpi_ns_copy_device_id
 *
 * PARAMETERS:  dest                - Pointer to the destination PNP_DEVICE_ID
 *              source              - Pointer to the source PNP_DEVICE_ID
 *              string_area         - Pointer to where to copy the dest string
 *
 * RETURN:      Pointer to the next string area
 *
 * DESCRIPTION: Copy a single PNP_DEVICE_ID, including the string data.
 *
 ******************************************************************************/
static char *acpi_ns_copy_device_id(struct acpi_pnp_device_id *dest,
				    struct acpi_pnp_device_id *source,
				    char *string_area)
{
	/* Create the destination PNP_DEVICE_ID */

	dest->string = string_area;
	dest->length = source->length;

	/* Copy actual string and return a pointer to the next string area */

	memcpy(string_area, source->string, source->length);
	return (string_area + source->length);
}

/******************************************************************************
 *
 * FUNCTION:    acpi_get_object_info
 *
 * PARAMETERS:  handle              - Object Handle
 *              return_buffer       - Where the info is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Returns information about an object as gleaned from the
 *              namespace node and possibly by running several standard
 *              control methods (Such as in the case of a device.)
 *
 * For Device and Processor objects, run the Device _HID, _UID, _CID,
 * _CLS, _ADR, _sx_w, and _sx_d methods.
 *
 * Note: Allocates the return buffer, must be freed by the caller.
 *
 * Note: This interface is intended to be used during the initial device
 * discovery namespace traversal. Therefore, no complex methods can be
 * executed, especially those that access operation regions. Therefore, do
 * not add any additional methods that could cause problems in this area.
 * Because of this reason support for the following methods has been removed:
 * 1) _SUB method was removed (11/2015)
 * 2) _STA method was removed (02/2018)
 *
 ******************************************************************************/

acpi_status
acpi_get_object_info(acpi_handle handle,
		     struct acpi_device_info **return_buffer)
{
	struct acpi_namespace_node *node;
	struct acpi_device_info *info;
	struct acpi_pnp_device_id_list *cid_list = NULL;
	struct acpi_pnp_device_id *hid = NULL;
	struct acpi_pnp_device_id *uid = NULL;
	struct acpi_pnp_device_id *cls = NULL;
	char *next_id_string;
	acpi_object_type type;
	acpi_name name;
	u8 param_count = 0;
	u16 valid = 0;
	u32 info_size;
	u32 i;
	acpi_status status;

	/* Parameter validation */

	if (!handle || !return_buffer) {
		return (AE_BAD_PARAMETER);
	}

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	node = acpi_ns_validate_handle(handle);
	if (!node) {
		(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
		return (AE_BAD_PARAMETER);
	}

	/* Get the namespace node data while the namespace is locked */

	info_size = sizeof(struct acpi_device_info);
	type = node->type;
	name = node->name.integer;

	if (node->type == ACPI_TYPE_METHOD) {
		param_count = node->object->method.param_count;
	}

	status = acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	if ((type == ACPI_TYPE_DEVICE) || (type == ACPI_TYPE_PROCESSOR)) {
		/*
		 * Get extra info for ACPI Device/Processor objects only:
		 * Run the Device _HID, _UID, _CLS, and _CID methods.
		 *
		 * Note: none of these methods are required, so they may or may
		 * not be present for this device. The Info->Valid bitfield is used
		 * to indicate which methods were found and run successfully.
		 */

		/* Execute the Device._HID method */

		status = acpi_ut_execute_HID(node, &hid);
		if (ACPI_SUCCESS(status)) {
			info_size += hid->length;
			valid |= ACPI_VALID_HID;
		}

		/* Execute the Device._UID method */

		status = acpi_ut_execute_UID(node, &uid);
		if (ACPI_SUCCESS(status)) {
			info_size += uid->length;
			valid |= ACPI_VALID_UID;
		}

		/* Execute the Device._CID method */

		status = acpi_ut_execute_CID(node, &cid_list);
		if (ACPI_SUCCESS(status)) {

			/* Add size of CID strings and CID pointer array */

			info_size +=
			    (cid_list->list_size -
			     sizeof(struct acpi_pnp_device_id_list));
			valid |= ACPI_VALID_CID;
		}

		/* Execute the Device._CLS method */

		status = acpi_ut_execute_CLS(node, &cls);
		if (ACPI_SUCCESS(status)) {
			info_size += cls->length;
			valid |= ACPI_VALID_CLS;
		}
	}

	/*
	 * Now that we have the variable-length data, we can allocate the
	 * return buffer
	 */
	info = ACPI_ALLOCATE_ZEROED(info_size);
	if (!info) {
		status = AE_NO_MEMORY;
		goto cleanup;
	}

	/* Get the fixed-length data */

	if ((type == ACPI_TYPE_DEVICE) || (type == ACPI_TYPE_PROCESSOR)) {
		/*
		 * Get extra info for ACPI Device/Processor objects only:
		 * Run the _ADR and, sx_w, and _sx_d methods.
		 *
		 * Notes: none of these methods are required, so they may or may
		 * not be present for this device. The Info->Valid bitfield is used
		 * to indicate which methods were found and run successfully.
		 */

		/* Execute the Device._ADR method */

		status = acpi_ut_evaluate_numeric_object(METHOD_NAME__ADR, node,
							 &info->address);
		if (ACPI_SUCCESS(status)) {
			valid |= ACPI_VALID_ADR;
		}

		/* Execute the Device._sx_w methods */

		status = acpi_ut_execute_power_methods(node,
						       acpi_gbl_lowest_dstate_names,
						       ACPI_NUM_sx_w_METHODS,
						       info->lowest_dstates);
		if (ACPI_SUCCESS(status)) {
			valid |= ACPI_VALID_SXWS;
		}

		/* Execute the Device._sx_d methods */

		status = acpi_ut_execute_power_methods(node,
						       acpi_gbl_highest_dstate_names,
						       ACPI_NUM_sx_d_METHODS,
						       info->highest_dstates);
		if (ACPI_SUCCESS(status)) {
			valid |= ACPI_VALID_SXDS;
		}
	}

	/*
	 * Create a pointer to the string area of the return buffer.
	 * Point to the end of the base struct acpi_device_info structure.
	 */
	next_id_string = ACPI_CAST_PTR(char, info->compatible_id_list.ids);
	if (cid_list) {

		/* Point past the CID PNP_DEVICE_ID array */

		next_id_string +=
		    ((acpi_size)cid_list->count *
		     sizeof(struct acpi_pnp_device_id));
	}

	/*
	 * Copy the HID, UID, and CIDs to the return buffer. The variable-length
	 * strings are copied to the reserved area at the end of the buffer.
	 *
	 * For HID and CID, check if the ID is a PCI Root Bridge.
	 */
	if (hid) {
		next_id_string = acpi_ns_copy_device_id(&info->hardware_id,
							hid, next_id_string);

		if (acpi_ut_is_pci_root_bridge(hid->string)) {
			info->flags |= ACPI_PCI_ROOT_BRIDGE;
		}
	}

	if (uid) {
		next_id_string = acpi_ns_copy_device_id(&info->unique_id,
							uid, next_id_string);
	}

	if (cid_list) {
		info->compatible_id_list.count = cid_list->count;
		info->compatible_id_list.list_size = cid_list->list_size;

		/* Copy each CID */

		for (i = 0; i < cid_list->count; i++) {
			next_id_string =
			    acpi_ns_copy_device_id(&info->compatible_id_list.
						   ids[i], &cid_list->ids[i],
						   next_id_string);

			if (acpi_ut_is_pci_root_bridge(cid_list->ids[i].string)) {
				info->flags |= ACPI_PCI_ROOT_BRIDGE;
			}
		}
	}

	if (cls) {
		(void)acpi_ns_copy_device_id(&info->class_code,
					     cls, next_id_string);
	}

	/* Copy the fixed-length data */

	info->info_size = info_size;
	info->type = type;
	info->name = name;
	info->param_count = param_count;
	info->valid = valid;

	*return_buffer = info;
	status = AE_OK;

cleanup:
	if (hid) {
		ACPI_FREE(hid);
	}
	if (uid) {
		ACPI_FREE(uid);
	}
	if (cid_list) {
		ACPI_FREE(cid_list);
	}
	if (cls) {
		ACPI_FREE(cls);
	}
	return (status);
}

ACPI_EXPORT_SYMBOL(acpi_get_object_info)

/******************************************************************************
 *
 * FUNCTION:    acpi_install_method
 *
 * PARAMETERS:  buffer         - An ACPI table containing one control method
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Install a control method into the namespace. If the method
 *              name already exists in the namespace, it is overwritten. The
 *              input buffer must contain a valid DSDT or SSDT containing a
 *              single control method.
 *
 ******************************************************************************/
acpi_status acpi_install_method(u8 *buffer)
{
	struct acpi_table_header *table =
	    ACPI_CAST_PTR(struct acpi_table_header, buffer);
	u8 *aml_buffer;
	u8 *aml_start;
	char *path;
	struct acpi_namespace_node *node;
	union acpi_operand_object *method_obj;
	struct acpi_parse_state parser_state;
	u32 aml_length;
	u16 opcode;
	u8 method_flags;
	acpi_status status;

	/* Parameter validation */

	if (!buffer) {
		return (AE_BAD_PARAMETER);
	}

	/* Table must be a DSDT or SSDT */

	if (!ACPI_COMPARE_NAMESEG(table->signature, ACPI_SIG_DSDT) &&
	    !ACPI_COMPARE_NAMESEG(table->signature, ACPI_SIG_SSDT)) {
		return (AE_BAD_HEADER);
	}

	/* First AML opcode in the table must be a control method */

	parser_state.aml = buffer + sizeof(struct acpi_table_header);
	opcode = acpi_ps_peek_opcode(&parser_state);
	if (opcode != AML_METHOD_OP) {
		return (AE_BAD_PARAMETER);
	}

	/* Extract method information from the raw AML */

	parser_state.aml += acpi_ps_get_opcode_size(opcode);
	parser_state.pkg_end = acpi_ps_get_next_package_end(&parser_state);
	path = acpi_ps_get_next_namestring(&parser_state);

	method_flags = *parser_state.aml++;
	aml_start = parser_state.aml;
	aml_length = (u32)ACPI_PTR_DIFF(parser_state.pkg_end, aml_start);

	/*
	 * Allocate resources up-front. We don't want to have to delete a new
	 * node from the namespace if we cannot allocate memory.
	 */
	aml_buffer = ACPI_ALLOCATE(aml_length);
	if (!aml_buffer) {
		return (AE_NO_MEMORY);
	}

	method_obj = acpi_ut_create_internal_object(ACPI_TYPE_METHOD);
	if (!method_obj) {
		ACPI_FREE(aml_buffer);
		return (AE_NO_MEMORY);
	}

	/* Lock namespace for acpi_ns_lookup, we may be creating a new node */

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		goto error_exit;
	}

	/* The lookup either returns an existing node or creates a new one */

	status =
	    acpi_ns_lookup(NULL, path, ACPI_TYPE_METHOD, ACPI_IMODE_LOAD_PASS1,
			   ACPI_NS_DONT_OPEN_SCOPE | ACPI_NS_ERROR_IF_FOUND,
			   NULL, &node);

	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);

	if (ACPI_FAILURE(status)) {	/* ns_lookup */
		if (status != AE_ALREADY_EXISTS) {
			goto error_exit;
		}

		/* Node existed previously, make sure it is a method node */

		if (node->type != ACPI_TYPE_METHOD) {
			status = AE_TYPE;
			goto error_exit;
		}
	}

	/* Copy the method AML to the local buffer */

	memcpy(aml_buffer, aml_start, aml_length);

	/* Initialize the method object with the new method's information */

	method_obj->method.aml_start = aml_buffer;
	method_obj->method.aml_length = aml_length;

	method_obj->method.param_count = (u8)
	    (method_flags & AML_METHOD_ARG_COUNT);

	if (method_flags & AML_METHOD_SERIALIZED) {
		method_obj->method.info_flags = ACPI_METHOD_SERIALIZED;

		method_obj->method.sync_level = (u8)
		    ((method_flags & AML_METHOD_SYNC_LEVEL) >> 4);
	}

	/*
	 * Now that it is complete, we can attach the new method object to
	 * the method Node (detaches/deletes any existing object)
	 */
	status = acpi_ns_attach_object(node, method_obj, ACPI_TYPE_METHOD);

	/*
	 * Flag indicates AML buffer is dynamic, must be deleted later.
	 * Must be set only after attach above.
	 */
	node->flags |= ANOBJ_ALLOCATED_BUFFER;

	/* Remove local reference to the method object */

	acpi_ut_remove_reference(method_obj);
	return (status);

error_exit:

	ACPI_FREE(aml_buffer);
	ACPI_FREE(method_obj);
	return (status);
}
ACPI_EXPORT_SYMBOL(acpi_install_method)
