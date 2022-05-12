// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/*******************************************************************************
 *
 * Module Name: utresrc - Resource management utilities
 *
 ******************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"
#include "acresrc.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utresrc")

/*
 * Base sizes of the raw AML resource descriptors, indexed by resource type.
 * Zero indicates a reserved (and therefore invalid) resource type.
 */
const u8 acpi_gbl_resource_aml_sizes[] = {
	/* Small descriptors */

	0,
	0,
	0,
	0,
	ACPI_AML_SIZE_SMALL(struct aml_resource_irq),
	ACPI_AML_SIZE_SMALL(struct aml_resource_dma),
	ACPI_AML_SIZE_SMALL(struct aml_resource_start_dependent),
	ACPI_AML_SIZE_SMALL(struct aml_resource_end_dependent),
	ACPI_AML_SIZE_SMALL(struct aml_resource_io),
	ACPI_AML_SIZE_SMALL(struct aml_resource_fixed_io),
	ACPI_AML_SIZE_SMALL(struct aml_resource_fixed_dma),
	0,
	0,
	0,
	ACPI_AML_SIZE_SMALL(struct aml_resource_vendor_small),
	ACPI_AML_SIZE_SMALL(struct aml_resource_end_tag),

	/* Large descriptors */

	0,
	ACPI_AML_SIZE_LARGE(struct aml_resource_memory24),
	ACPI_AML_SIZE_LARGE(struct aml_resource_generic_register),
	0,
	ACPI_AML_SIZE_LARGE(struct aml_resource_vendor_large),
	ACPI_AML_SIZE_LARGE(struct aml_resource_memory32),
	ACPI_AML_SIZE_LARGE(struct aml_resource_fixed_memory32),
	ACPI_AML_SIZE_LARGE(struct aml_resource_address32),
	ACPI_AML_SIZE_LARGE(struct aml_resource_address16),
	ACPI_AML_SIZE_LARGE(struct aml_resource_extended_irq),
	ACPI_AML_SIZE_LARGE(struct aml_resource_address64),
	ACPI_AML_SIZE_LARGE(struct aml_resource_extended_address64),
	ACPI_AML_SIZE_LARGE(struct aml_resource_gpio),
	ACPI_AML_SIZE_LARGE(struct aml_resource_pin_function),
	ACPI_AML_SIZE_LARGE(struct aml_resource_common_serialbus),
	ACPI_AML_SIZE_LARGE(struct aml_resource_pin_config),
	ACPI_AML_SIZE_LARGE(struct aml_resource_pin_group),
	ACPI_AML_SIZE_LARGE(struct aml_resource_pin_group_function),
	ACPI_AML_SIZE_LARGE(struct aml_resource_pin_group_config),
};

const u8 acpi_gbl_resource_aml_serial_bus_sizes[] = {
	0,
	ACPI_AML_SIZE_LARGE(struct aml_resource_i2c_serialbus),
	ACPI_AML_SIZE_LARGE(struct aml_resource_spi_serialbus),
	ACPI_AML_SIZE_LARGE(struct aml_resource_uart_serialbus),
	ACPI_AML_SIZE_LARGE(struct aml_resource_csi2_serialbus),
};

/*
 * Resource types, used to validate the resource length field.
 * The length of fixed-length types must match exactly, variable
 * lengths must meet the minimum required length, etc.
 * Zero indicates a reserved (and therefore invalid) resource type.
 */
static const u8 acpi_gbl_resource_types[] = {
	/* Small descriptors */

	0,
	0,
	0,
	0,
	ACPI_SMALL_VARIABLE_LENGTH,	/* 04 IRQ */
	ACPI_FIXED_LENGTH,	/* 05 DMA */
	ACPI_SMALL_VARIABLE_LENGTH,	/* 06 start_dependent_functions */
	ACPI_FIXED_LENGTH,	/* 07 end_dependent_functions */
	ACPI_FIXED_LENGTH,	/* 08 IO */
	ACPI_FIXED_LENGTH,	/* 09 fixed_IO */
	ACPI_FIXED_LENGTH,	/* 0A fixed_DMA */
	0,
	0,
	0,
	ACPI_VARIABLE_LENGTH,	/* 0E vendor_short */
	ACPI_FIXED_LENGTH,	/* 0F end_tag */

	/* Large descriptors */

	0,
	ACPI_FIXED_LENGTH,	/* 01 Memory24 */
	ACPI_FIXED_LENGTH,	/* 02 generic_register */
	0,
	ACPI_VARIABLE_LENGTH,	/* 04 vendor_long */
	ACPI_FIXED_LENGTH,	/* 05 Memory32 */
	ACPI_FIXED_LENGTH,	/* 06 memory32_fixed */
	ACPI_VARIABLE_LENGTH,	/* 07 Dword* address */
	ACPI_VARIABLE_LENGTH,	/* 08 Word* address */
	ACPI_VARIABLE_LENGTH,	/* 09 extended_IRQ */
	ACPI_VARIABLE_LENGTH,	/* 0A Qword* address */
	ACPI_FIXED_LENGTH,	/* 0B Extended* address */
	ACPI_VARIABLE_LENGTH,	/* 0C Gpio* */
	ACPI_VARIABLE_LENGTH,	/* 0D pin_function */
	ACPI_VARIABLE_LENGTH,	/* 0E *serial_bus */
	ACPI_VARIABLE_LENGTH,	/* 0F pin_config */
	ACPI_VARIABLE_LENGTH,	/* 10 pin_group */
	ACPI_VARIABLE_LENGTH,	/* 11 pin_group_function */
	ACPI_VARIABLE_LENGTH,	/* 12 pin_group_config */
};

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_walk_aml_resources
 *
 * PARAMETERS:  walk_state          - Current walk info
 * PARAMETERS:  aml                 - Pointer to the raw AML resource template
 *              aml_length          - Length of the entire template
 *              user_function       - Called once for each descriptor found. If
 *                                    NULL, a pointer to the end_tag is returned
 *              context             - Passed to user_function
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Walk a raw AML resource list(buffer). User function called
 *              once for each resource found.
 *
 ******************************************************************************/

acpi_status
acpi_ut_walk_aml_resources(struct acpi_walk_state *walk_state,
			   u8 *aml,
			   acpi_size aml_length,
			   acpi_walk_aml_callback user_function, void **context)
{
	acpi_status status;
	u8 *end_aml;
	u8 resource_index;
	u32 length;
	u32 offset = 0;
	u8 end_tag[2] = { 0x79, 0x00 };

	ACPI_FUNCTION_TRACE(ut_walk_aml_resources);

	/* The absolute minimum resource template is one end_tag descriptor */

	if (aml_length < sizeof(struct aml_resource_end_tag)) {
		return_ACPI_STATUS(AE_AML_NO_RESOURCE_END_TAG);
	}

	/* Point to the end of the resource template buffer */

	end_aml = aml + aml_length;

	/* Walk the byte list, abort on any invalid descriptor type or length */

	while (aml < end_aml) {

		/* Validate the Resource Type and Resource Length */

		status =
		    acpi_ut_validate_resource(walk_state, aml, &resource_index);
		if (ACPI_FAILURE(status)) {
			/*
			 * Exit on failure. Cannot continue because the descriptor
			 * length may be bogus also.
			 */
			return_ACPI_STATUS(status);
		}

		/* Get the length of this descriptor */

		length = acpi_ut_get_descriptor_length(aml);

		/* Invoke the user function */

		if (user_function) {
			status =
			    user_function(aml, length, offset, resource_index,
					  context);
			if (ACPI_FAILURE(status)) {
				return_ACPI_STATUS(status);
			}
		}

		/* An end_tag descriptor terminates this resource template */

		if (acpi_ut_get_resource_type(aml) ==
		    ACPI_RESOURCE_NAME_END_TAG) {
			/*
			 * There must be at least one more byte in the buffer for
			 * the 2nd byte of the end_tag
			 */
			if ((aml + 1) >= end_aml) {
				return_ACPI_STATUS(AE_AML_NO_RESOURCE_END_TAG);
			}

			/*
			 * Don't attempt to perform any validation on the 2nd byte.
			 * Although all known ASL compilers insert a zero for the 2nd
			 * byte, it can also be a checksum (as per the ACPI spec),
			 * and this is occasionally seen in the field. July 2017.
			 */

			/* Return the pointer to the end_tag if requested */

			if (!user_function) {
				*context = aml;
			}

			/* Normal exit */

			return_ACPI_STATUS(AE_OK);
		}

		aml += length;
		offset += length;
	}

	/* Did not find an end_tag descriptor */

	if (user_function) {

		/* Insert an end_tag anyway. acpi_rs_get_list_length always leaves room */

		(void)acpi_ut_validate_resource(walk_state, end_tag,
						&resource_index);
		status =
		    user_function(end_tag, 2, offset, resource_index, context);
		if (ACPI_FAILURE(status)) {
			return_ACPI_STATUS(status);
		}
	}

	return_ACPI_STATUS(AE_AML_NO_RESOURCE_END_TAG);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_validate_resource
 *
 * PARAMETERS:  walk_state          - Current walk info
 *              aml                 - Pointer to the raw AML resource descriptor
 *              return_index        - Where the resource index is returned. NULL
 *                                    if the index is not required.
 *
 * RETURN:      Status, and optionally the Index into the global resource tables
 *
 * DESCRIPTION: Validate an AML resource descriptor by checking the Resource
 *              Type and Resource Length. Returns an index into the global
 *              resource information/dispatch tables for later use.
 *
 ******************************************************************************/

acpi_status
acpi_ut_validate_resource(struct acpi_walk_state *walk_state,
			  void *aml, u8 *return_index)
{
	union aml_resource *aml_resource;
	u8 resource_type;
	u8 resource_index;
	acpi_rs_length resource_length;
	acpi_rs_length minimum_resource_length;

	ACPI_FUNCTION_ENTRY();

	/*
	 * 1) Validate the resource_type field (Byte 0)
	 */
	resource_type = ACPI_GET8(aml);

	/*
	 * Byte 0 contains the descriptor name (Resource Type)
	 * Examine the large/small bit in the resource header
	 */
	if (resource_type & ACPI_RESOURCE_NAME_LARGE) {

		/* Verify the large resource type (name) against the max */

		if (resource_type > ACPI_RESOURCE_NAME_LARGE_MAX) {
			goto invalid_resource;
		}

		/*
		 * Large Resource Type -- bits 6:0 contain the name
		 * Translate range 0x80-0x8B to index range 0x10-0x1B
		 */
		resource_index = (u8) (resource_type - 0x70);
	} else {
		/*
		 * Small Resource Type -- bits 6:3 contain the name
		 * Shift range to index range 0x00-0x0F
		 */
		resource_index = (u8)
		    ((resource_type & ACPI_RESOURCE_NAME_SMALL_MASK) >> 3);
	}

	/*
	 * Check validity of the resource type, via acpi_gbl_resource_types.
	 * Zero indicates an invalid resource.
	 */
	if (!acpi_gbl_resource_types[resource_index]) {
		goto invalid_resource;
	}

	/*
	 * Validate the resource_length field. This ensures that the length
	 * is at least reasonable, and guarantees that it is non-zero.
	 */
	resource_length = acpi_ut_get_resource_length(aml);
	minimum_resource_length = acpi_gbl_resource_aml_sizes[resource_index];

	/* Validate based upon the type of resource - fixed length or variable */

	switch (acpi_gbl_resource_types[resource_index]) {
	case ACPI_FIXED_LENGTH:

		/* Fixed length resource, length must match exactly */

		if (resource_length != minimum_resource_length) {
			goto bad_resource_length;
		}
		break;

	case ACPI_VARIABLE_LENGTH:

		/* Variable length resource, length must be at least the minimum */

		if (resource_length < minimum_resource_length) {
			goto bad_resource_length;
		}
		break;

	case ACPI_SMALL_VARIABLE_LENGTH:

		/* Small variable length resource, length can be (Min) or (Min-1) */

		if ((resource_length > minimum_resource_length) ||
		    (resource_length < (minimum_resource_length - 1))) {
			goto bad_resource_length;
		}
		break;

	default:

		/* Shouldn't happen (because of validation earlier), but be sure */

		goto invalid_resource;
	}

	aml_resource = ACPI_CAST_PTR(union aml_resource, aml);
	if (resource_type == ACPI_RESOURCE_NAME_SERIAL_BUS) {

		/* Validate the bus_type field */

		if ((aml_resource->common_serial_bus.type == 0) ||
		    (aml_resource->common_serial_bus.type >
		     AML_RESOURCE_MAX_SERIALBUSTYPE)) {
			if (walk_state) {
				ACPI_ERROR((AE_INFO,
					    "Invalid/unsupported SerialBus resource descriptor: BusType 0x%2.2X",
					    aml_resource->common_serial_bus.
					    type));
			}
			return (AE_AML_INVALID_RESOURCE_TYPE);
		}
	}

	/* Optionally return the resource table index */

	if (return_index) {
		*return_index = resource_index;
	}

	return (AE_OK);

invalid_resource:

	if (walk_state) {
		ACPI_ERROR((AE_INFO,
			    "Invalid/unsupported resource descriptor: Type 0x%2.2X",
			    resource_type));
	}
	return (AE_AML_INVALID_RESOURCE_TYPE);

bad_resource_length:

	if (walk_state) {
		ACPI_ERROR((AE_INFO,
			    "Invalid resource descriptor length: Type "
			    "0x%2.2X, Length 0x%4.4X, MinLength 0x%4.4X",
			    resource_type, resource_length,
			    minimum_resource_length));
	}
	return (AE_AML_BAD_RESOURCE_LENGTH);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_get_resource_type
 *
 * PARAMETERS:  aml             - Pointer to the raw AML resource descriptor
 *
 * RETURN:      The Resource Type with no extraneous bits (except the
 *              Large/Small descriptor bit -- this is left alone)
 *
 * DESCRIPTION: Extract the Resource Type/Name from the first byte of
 *              a resource descriptor.
 *
 ******************************************************************************/

u8 acpi_ut_get_resource_type(void *aml)
{
	ACPI_FUNCTION_ENTRY();

	/*
	 * Byte 0 contains the descriptor name (Resource Type)
	 * Examine the large/small bit in the resource header
	 */
	if (ACPI_GET8(aml) & ACPI_RESOURCE_NAME_LARGE) {

		/* Large Resource Type -- bits 6:0 contain the name */

		return (ACPI_GET8(aml));
	} else {
		/* Small Resource Type -- bits 6:3 contain the name */

		return ((u8) (ACPI_GET8(aml) & ACPI_RESOURCE_NAME_SMALL_MASK));
	}
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_get_resource_length
 *
 * PARAMETERS:  aml             - Pointer to the raw AML resource descriptor
 *
 * RETURN:      Byte Length
 *
 * DESCRIPTION: Get the "Resource Length" of a raw AML descriptor. By
 *              definition, this does not include the size of the descriptor
 *              header or the length field itself.
 *
 ******************************************************************************/

u16 acpi_ut_get_resource_length(void *aml)
{
	acpi_rs_length resource_length;

	ACPI_FUNCTION_ENTRY();

	/*
	 * Byte 0 contains the descriptor name (Resource Type)
	 * Examine the large/small bit in the resource header
	 */
	if (ACPI_GET8(aml) & ACPI_RESOURCE_NAME_LARGE) {

		/* Large Resource type -- bytes 1-2 contain the 16-bit length */

		ACPI_MOVE_16_TO_16(&resource_length, ACPI_ADD_PTR(u8, aml, 1));

	} else {
		/* Small Resource type -- bits 2:0 of byte 0 contain the length */

		resource_length = (u16) (ACPI_GET8(aml) &
					 ACPI_RESOURCE_NAME_SMALL_LENGTH_MASK);
	}

	return (resource_length);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_get_resource_header_length
 *
 * PARAMETERS:  aml             - Pointer to the raw AML resource descriptor
 *
 * RETURN:      Length of the AML header (depends on large/small descriptor)
 *
 * DESCRIPTION: Get the length of the header for this resource.
 *
 ******************************************************************************/

u8 acpi_ut_get_resource_header_length(void *aml)
{
	ACPI_FUNCTION_ENTRY();

	/* Examine the large/small bit in the resource header */

	if (ACPI_GET8(aml) & ACPI_RESOURCE_NAME_LARGE) {
		return (sizeof(struct aml_resource_large_header));
	} else {
		return (sizeof(struct aml_resource_small_header));
	}
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_get_descriptor_length
 *
 * PARAMETERS:  aml             - Pointer to the raw AML resource descriptor
 *
 * RETURN:      Byte length
 *
 * DESCRIPTION: Get the total byte length of a raw AML descriptor, including the
 *              length of the descriptor header and the length field itself.
 *              Used to walk descriptor lists.
 *
 ******************************************************************************/

u32 acpi_ut_get_descriptor_length(void *aml)
{
	ACPI_FUNCTION_ENTRY();

	/*
	 * Get the Resource Length (does not include header length) and add
	 * the header length (depends on if this is a small or large resource)
	 */
	return (acpi_ut_get_resource_length(aml) +
		acpi_ut_get_resource_header_length(aml));
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_get_resource_end_tag
 *
 * PARAMETERS:  obj_desc        - The resource template buffer object
 *              end_tag         - Where the pointer to the end_tag is returned
 *
 * RETURN:      Status, pointer to the end tag
 *
 * DESCRIPTION: Find the end_tag resource descriptor in an AML resource template
 *              Note: allows a buffer length of zero.
 *
 ******************************************************************************/

acpi_status
acpi_ut_get_resource_end_tag(union acpi_operand_object *obj_desc, u8 **end_tag)
{
	acpi_status status;

	ACPI_FUNCTION_TRACE(ut_get_resource_end_tag);

	/* Allow a buffer length of zero */

	if (!obj_desc->buffer.length) {
		*end_tag = obj_desc->buffer.pointer;
		return_ACPI_STATUS(AE_OK);
	}

	/* Validate the template and get a pointer to the end_tag */

	status = acpi_ut_walk_aml_resources(NULL, obj_desc->buffer.pointer,
					    obj_desc->buffer.length, NULL,
					    (void **)end_tag);

	return_ACPI_STATUS(status);
}
