// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/*******************************************************************************
 *
 * Module Name: rsutils - Utilities for the resource manager
 *
 ******************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"
#include "acnamesp.h"
#include "acresrc.h"

#define _COMPONENT          ACPI_RESOURCES
ACPI_MODULE_NAME("rsutils")

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_decode_bitmask
 *
 * PARAMETERS:  mask            - Bitmask to decode
 *              list            - Where the converted list is returned
 *
 * RETURN:      Count of bits set (length of list)
 *
 * DESCRIPTION: Convert a bit mask into a list of values
 *
 ******************************************************************************/
u8 acpi_rs_decode_bitmask(u16 mask, u8 * list)
{
	u8 i;
	u8 bit_count;

	ACPI_FUNCTION_ENTRY();

	/* Decode the mask bits */

	for (i = 0, bit_count = 0; mask; i++) {
		if (mask & 0x0001) {
			list[bit_count] = i;
			bit_count++;
		}

		mask >>= 1;
	}

	return (bit_count);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_encode_bitmask
 *
 * PARAMETERS:  list            - List of values to encode
 *              count           - Length of list
 *
 * RETURN:      Encoded bitmask
 *
 * DESCRIPTION: Convert a list of values to an encoded bitmask
 *
 ******************************************************************************/

u16 acpi_rs_encode_bitmask(u8 * list, u8 count)
{
	u32 i;
	u16 mask;

	ACPI_FUNCTION_ENTRY();

	/* Encode the list into a single bitmask */

	for (i = 0, mask = 0; i < count; i++) {
		mask |= (0x1 << list[i]);
	}

	return (mask);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_move_data
 *
 * PARAMETERS:  destination         - Pointer to the destination descriptor
 *              source              - Pointer to the source descriptor
 *              item_count          - How many items to move
 *              move_type           - Byte width
 *
 * RETURN:      None
 *
 * DESCRIPTION: Move multiple data items from one descriptor to another. Handles
 *              alignment issues and endian issues if necessary, as configured
 *              via the ACPI_MOVE_* macros. (This is why a memcpy is not used)
 *
 ******************************************************************************/

void
acpi_rs_move_data(void *destination, void *source, u16 item_count, u8 move_type)
{
	u32 i;

	ACPI_FUNCTION_ENTRY();

	/* One move per item */

	for (i = 0; i < item_count; i++) {
		switch (move_type) {
			/*
			 * For the 8-bit case, we can perform the move all at once
			 * since there are no alignment or endian issues
			 */
		case ACPI_RSC_MOVE8:
		case ACPI_RSC_MOVE_GPIO_RES:
		case ACPI_RSC_MOVE_SERIAL_VEN:
		case ACPI_RSC_MOVE_SERIAL_RES:

			memcpy(destination, source, item_count);
			return;

			/*
			 * 16-, 32-, and 64-bit cases must use the move macros that perform
			 * endian conversion and/or accommodate hardware that cannot perform
			 * misaligned memory transfers
			 */
		case ACPI_RSC_MOVE16:
		case ACPI_RSC_MOVE_GPIO_PIN:

			ACPI_MOVE_16_TO_16(&ACPI_CAST_PTR(u16, destination)[i],
					   &ACPI_CAST_PTR(u16, source)[i]);
			break;

		case ACPI_RSC_MOVE32:

			ACPI_MOVE_32_TO_32(&ACPI_CAST_PTR(u32, destination)[i],
					   &ACPI_CAST_PTR(u32, source)[i]);
			break;

		case ACPI_RSC_MOVE64:

			ACPI_MOVE_64_TO_64(&ACPI_CAST_PTR(u64, destination)[i],
					   &ACPI_CAST_PTR(u64, source)[i]);
			break;

		default:

			return;
		}
	}
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_set_resource_length
 *
 * PARAMETERS:  total_length        - Length of the AML descriptor, including
 *                                    the header and length fields.
 *              aml                 - Pointer to the raw AML descriptor
 *
 * RETURN:      None
 *
 * DESCRIPTION: Set the resource_length field of an AML
 *              resource descriptor, both Large and Small descriptors are
 *              supported automatically. Note: Descriptor Type field must
 *              be valid.
 *
 ******************************************************************************/

void
acpi_rs_set_resource_length(acpi_rsdesc_size total_length,
			    union aml_resource *aml)
{
	acpi_rs_length resource_length;

	ACPI_FUNCTION_ENTRY();

	/* Length is the total descriptor length minus the header length */

	resource_length = (acpi_rs_length)
	    (total_length - acpi_ut_get_resource_header_length(aml));

	/* Length is stored differently for large and small descriptors */

	if (aml->small_header.descriptor_type & ACPI_RESOURCE_NAME_LARGE) {

		/* Large descriptor -- bytes 1-2 contain the 16-bit length */

		ACPI_MOVE_16_TO_16(&aml->large_header.resource_length,
				   &resource_length);
	} else {
		/*
		 * Small descriptor -- bits 2:0 of byte 0 contain the length
		 * Clear any existing length, preserving descriptor type bits
		 */
		aml->small_header.descriptor_type = (u8)
		    ((aml->small_header.descriptor_type &
		      ~ACPI_RESOURCE_NAME_SMALL_LENGTH_MASK)
		     | resource_length);
	}
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_set_resource_header
 *
 * PARAMETERS:  descriptor_type     - Byte to be inserted as the type
 *              total_length        - Length of the AML descriptor, including
 *                                    the header and length fields.
 *              aml                 - Pointer to the raw AML descriptor
 *
 * RETURN:      None
 *
 * DESCRIPTION: Set the descriptor_type and resource_length fields of an AML
 *              resource descriptor, both Large and Small descriptors are
 *              supported automatically
 *
 ******************************************************************************/

void
acpi_rs_set_resource_header(u8 descriptor_type,
			    acpi_rsdesc_size total_length,
			    union aml_resource *aml)
{
	ACPI_FUNCTION_ENTRY();

	/* Set the Resource Type */

	aml->small_header.descriptor_type = descriptor_type;

	/* Set the Resource Length */

	acpi_rs_set_resource_length(total_length, aml);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_strcpy
 *
 * PARAMETERS:  destination         - Pointer to the destination string
 *              source              - Pointer to the source string
 *
 * RETURN:      String length, including NULL terminator
 *
 * DESCRIPTION: Local string copy that returns the string length, saving a
 *              strcpy followed by a strlen.
 *
 ******************************************************************************/

static u16 acpi_rs_strcpy(char *destination, char *source)
{
	u16 i;

	ACPI_FUNCTION_ENTRY();

	for (i = 0; source[i]; i++) {
		destination[i] = source[i];
	}

	destination[i] = 0;

	/* Return string length including the NULL terminator */

	return ((u16) (i + 1));
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_get_resource_source
 *
 * PARAMETERS:  resource_length     - Length field of the descriptor
 *              minimum_length      - Minimum length of the descriptor (minus
 *                                    any optional fields)
 *              resource_source     - Where the resource_source is returned
 *              aml                 - Pointer to the raw AML descriptor
 *              string_ptr          - (optional) where to store the actual
 *                                    resource_source string
 *
 * RETURN:      Length of the string plus NULL terminator, rounded up to native
 *              word boundary
 *
 * DESCRIPTION: Copy the optional resource_source data from a raw AML descriptor
 *              to an internal resource descriptor
 *
 ******************************************************************************/

acpi_rs_length
acpi_rs_get_resource_source(acpi_rs_length resource_length,
			    acpi_rs_length minimum_length,
			    struct acpi_resource_source * resource_source,
			    union aml_resource * aml, char *string_ptr)
{
	acpi_rsdesc_size total_length;
	u8 *aml_resource_source;

	ACPI_FUNCTION_ENTRY();

	total_length =
	    resource_length + sizeof(struct aml_resource_large_header);
	aml_resource_source = ACPI_ADD_PTR(u8, aml, minimum_length);

	/*
	 * resource_source is present if the length of the descriptor is longer
	 * than the minimum length.
	 *
	 * Note: Some resource descriptors will have an additional null, so
	 * we add 1 to the minimum length.
	 */
	if (total_length > (acpi_rsdesc_size)(minimum_length + 1)) {

		/* Get the resource_source_index */

		resource_source->index = aml_resource_source[0];

		resource_source->string_ptr = string_ptr;
		if (!string_ptr) {
			/*
			 * String destination pointer is not specified; Set the String
			 * pointer to the end of the current resource_source structure.
			 */
			resource_source->string_ptr =
			    ACPI_ADD_PTR(char, resource_source,
					 sizeof(struct acpi_resource_source));
		}

		/*
		 * In order for the Resource length to be a multiple of the native
		 * word, calculate the length of the string (+1 for NULL terminator)
		 * and expand to the next word multiple.
		 *
		 * Zero the entire area of the buffer.
		 */
		total_length =
		    (u32)strlen(ACPI_CAST_PTR(char, &aml_resource_source[1])) +
		    1;

		total_length = (u32)ACPI_ROUND_UP_TO_NATIVE_WORD(total_length);

		memset(resource_source->string_ptr, 0, total_length);

		/* Copy the resource_source string to the destination */

		resource_source->string_length =
		    acpi_rs_strcpy(resource_source->string_ptr,
				   ACPI_CAST_PTR(char,
						 &aml_resource_source[1]));

		return ((acpi_rs_length)total_length);
	}

	/* resource_source is not present */

	resource_source->index = 0;
	resource_source->string_length = 0;
	resource_source->string_ptr = NULL;
	return (0);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_set_resource_source
 *
 * PARAMETERS:  aml                 - Pointer to the raw AML descriptor
 *              minimum_length      - Minimum length of the descriptor (minus
 *                                    any optional fields)
 *              resource_source     - Internal resource_source

 *
 * RETURN:      Total length of the AML descriptor
 *
 * DESCRIPTION: Convert an optional resource_source from internal format to a
 *              raw AML resource descriptor
 *
 ******************************************************************************/

acpi_rsdesc_size
acpi_rs_set_resource_source(union aml_resource *aml,
			    acpi_rs_length minimum_length,
			    struct acpi_resource_source *resource_source)
{
	u8 *aml_resource_source;
	acpi_rsdesc_size descriptor_length;

	ACPI_FUNCTION_ENTRY();

	descriptor_length = minimum_length;

	/* Non-zero string length indicates presence of a resource_source */

	if (resource_source->string_length) {

		/* Point to the end of the AML descriptor */

		aml_resource_source = ACPI_ADD_PTR(u8, aml, minimum_length);

		/* Copy the resource_source_index */

		aml_resource_source[0] = (u8) resource_source->index;

		/* Copy the resource_source string */

		strcpy(ACPI_CAST_PTR(char, &aml_resource_source[1]),
		       resource_source->string_ptr);

		/*
		 * Add the length of the string (+ 1 for null terminator) to the
		 * final descriptor length
		 */
		descriptor_length += ((acpi_rsdesc_size)
				      resource_source->string_length + 1);
	}

	/* Return the new total length of the AML descriptor */

	return (descriptor_length);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_get_prt_method_data
 *
 * PARAMETERS:  node            - Device node
 *              ret_buffer      - Pointer to a buffer structure for the
 *                                results
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to get the _PRT value of an object
 *              contained in an object specified by the handle passed in
 *
 *              If the function fails an appropriate status will be returned
 *              and the contents of the callers buffer is undefined.
 *
 ******************************************************************************/

acpi_status
acpi_rs_get_prt_method_data(struct acpi_namespace_node *node,
			    struct acpi_buffer *ret_buffer)
{
	union acpi_operand_object *obj_desc;
	acpi_status status;

	ACPI_FUNCTION_TRACE(rs_get_prt_method_data);

	/* Parameters guaranteed valid by caller */

	/* Execute the method, no parameters */

	status =
	    acpi_ut_evaluate_object(node, METHOD_NAME__PRT, ACPI_BTYPE_PACKAGE,
				    &obj_desc);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * Create a resource linked list from the byte stream buffer that comes
	 * back from the _CRS method execution.
	 */
	status = acpi_rs_create_pci_routing_table(obj_desc, ret_buffer);

	/* On exit, we must delete the object returned by evaluate_object */

	acpi_ut_remove_reference(obj_desc);
	return_ACPI_STATUS(status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_get_crs_method_data
 *
 * PARAMETERS:  node            - Device node
 *              ret_buffer      - Pointer to a buffer structure for the
 *                                results
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to get the _CRS value of an object
 *              contained in an object specified by the handle passed in
 *
 *              If the function fails an appropriate status will be returned
 *              and the contents of the callers buffer is undefined.
 *
 ******************************************************************************/

acpi_status
acpi_rs_get_crs_method_data(struct acpi_namespace_node *node,
			    struct acpi_buffer *ret_buffer)
{
	union acpi_operand_object *obj_desc;
	acpi_status status;

	ACPI_FUNCTION_TRACE(rs_get_crs_method_data);

	/* Parameters guaranteed valid by caller */

	/* Execute the method, no parameters */

	status =
	    acpi_ut_evaluate_object(node, METHOD_NAME__CRS, ACPI_BTYPE_BUFFER,
				    &obj_desc);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * Make the call to create a resource linked list from the
	 * byte stream buffer that comes back from the _CRS method
	 * execution.
	 */
	status = acpi_rs_create_resource_list(obj_desc, ret_buffer);

	/* On exit, we must delete the object returned by evaluateObject */

	acpi_ut_remove_reference(obj_desc);
	return_ACPI_STATUS(status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_get_prs_method_data
 *
 * PARAMETERS:  node            - Device node
 *              ret_buffer      - Pointer to a buffer structure for the
 *                                results
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to get the _PRS value of an object
 *              contained in an object specified by the handle passed in
 *
 *              If the function fails an appropriate status will be returned
 *              and the contents of the callers buffer is undefined.
 *
 ******************************************************************************/

acpi_status
acpi_rs_get_prs_method_data(struct acpi_namespace_node *node,
			    struct acpi_buffer *ret_buffer)
{
	union acpi_operand_object *obj_desc;
	acpi_status status;

	ACPI_FUNCTION_TRACE(rs_get_prs_method_data);

	/* Parameters guaranteed valid by caller */

	/* Execute the method, no parameters */

	status =
	    acpi_ut_evaluate_object(node, METHOD_NAME__PRS, ACPI_BTYPE_BUFFER,
				    &obj_desc);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * Make the call to create a resource linked list from the
	 * byte stream buffer that comes back from the _CRS method
	 * execution.
	 */
	status = acpi_rs_create_resource_list(obj_desc, ret_buffer);

	/* On exit, we must delete the object returned by evaluateObject */

	acpi_ut_remove_reference(obj_desc);
	return_ACPI_STATUS(status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_get_aei_method_data
 *
 * PARAMETERS:  node            - Device node
 *              ret_buffer      - Pointer to a buffer structure for the
 *                                results
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to get the _AEI value of an object
 *              contained in an object specified by the handle passed in
 *
 *              If the function fails an appropriate status will be returned
 *              and the contents of the callers buffer is undefined.
 *
 ******************************************************************************/

acpi_status
acpi_rs_get_aei_method_data(struct acpi_namespace_node *node,
			    struct acpi_buffer *ret_buffer)
{
	union acpi_operand_object *obj_desc;
	acpi_status status;

	ACPI_FUNCTION_TRACE(rs_get_aei_method_data);

	/* Parameters guaranteed valid by caller */

	/* Execute the method, no parameters */

	status =
	    acpi_ut_evaluate_object(node, METHOD_NAME__AEI, ACPI_BTYPE_BUFFER,
				    &obj_desc);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * Make the call to create a resource linked list from the
	 * byte stream buffer that comes back from the _CRS method
	 * execution.
	 */
	status = acpi_rs_create_resource_list(obj_desc, ret_buffer);

	/* On exit, we must delete the object returned by evaluateObject */

	acpi_ut_remove_reference(obj_desc);
	return_ACPI_STATUS(status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_get_method_data
 *
 * PARAMETERS:  handle          - Handle to the containing object
 *              path            - Path to method, relative to Handle
 *              ret_buffer      - Pointer to a buffer structure for the
 *                                results
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to get the _CRS or _PRS value of an
 *              object contained in an object specified by the handle passed in
 *
 *              If the function fails an appropriate status will be returned
 *              and the contents of the callers buffer is undefined.
 *
 ******************************************************************************/

acpi_status
acpi_rs_get_method_data(acpi_handle handle,
			const char *path, struct acpi_buffer *ret_buffer)
{
	union acpi_operand_object *obj_desc;
	acpi_status status;

	ACPI_FUNCTION_TRACE(rs_get_method_data);

	/* Parameters guaranteed valid by caller */

	/* Execute the method, no parameters */

	status =
	    acpi_ut_evaluate_object(ACPI_CAST_PTR
				    (struct acpi_namespace_node, handle), path,
				    ACPI_BTYPE_BUFFER, &obj_desc);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	/*
	 * Make the call to create a resource linked list from the
	 * byte stream buffer that comes back from the method
	 * execution.
	 */
	status = acpi_rs_create_resource_list(obj_desc, ret_buffer);

	/* On exit, we must delete the object returned by evaluate_object */

	acpi_ut_remove_reference(obj_desc);
	return_ACPI_STATUS(status);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_rs_set_srs_method_data
 *
 * PARAMETERS:  node            - Device node
 *              in_buffer       - Pointer to a buffer structure of the
 *                                parameter
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to set the _SRS of an object contained
 *              in an object specified by the handle passed in
 *
 *              If the function fails an appropriate status will be returned
 *              and the contents of the callers buffer is undefined.
 *
 * Note: Parameters guaranteed valid by caller
 *
 ******************************************************************************/

acpi_status
acpi_rs_set_srs_method_data(struct acpi_namespace_node *node,
			    struct acpi_buffer *in_buffer)
{
	struct acpi_evaluate_info *info;
	union acpi_operand_object *args[2];
	acpi_status status;
	struct acpi_buffer buffer;

	ACPI_FUNCTION_TRACE(rs_set_srs_method_data);

	/* Allocate and initialize the evaluation information block */

	info = ACPI_ALLOCATE_ZEROED(sizeof(struct acpi_evaluate_info));
	if (!info) {
		return_ACPI_STATUS(AE_NO_MEMORY);
	}

	info->prefix_node = node;
	info->relative_pathname = METHOD_NAME__SRS;
	info->parameters = args;
	info->flags = ACPI_IGNORE_RETURN_VALUE;

	/*
	 * The in_buffer parameter will point to a linked list of
	 * resource parameters. It needs to be formatted into a
	 * byte stream to be sent in as an input parameter to _SRS
	 *
	 * Convert the linked list into a byte stream
	 */
	buffer.length = ACPI_ALLOCATE_LOCAL_BUFFER;
	status = acpi_rs_create_aml_resources(in_buffer, &buffer);
	if (ACPI_FAILURE(status)) {
		goto cleanup;
	}

	/* Create and initialize the method parameter object */

	args[0] = acpi_ut_create_internal_object(ACPI_TYPE_BUFFER);
	if (!args[0]) {
		/*
		 * Must free the buffer allocated above (otherwise it is freed
		 * later)
		 */
		ACPI_FREE(buffer.pointer);
		status = AE_NO_MEMORY;
		goto cleanup;
	}

	args[0]->buffer.length = (u32) buffer.length;
	args[0]->buffer.pointer = buffer.pointer;
	args[0]->common.flags = AOPOBJ_DATA_VALID;
	args[1] = NULL;

	/* Execute the method, no return value is expected */

	status = acpi_ns_evaluate(info);

	/* Clean up and return the status from acpi_ns_evaluate */

	acpi_ut_remove_reference(args[0]);

cleanup:
	ACPI_FREE(info);
	return_ACPI_STATUS(status);
}
