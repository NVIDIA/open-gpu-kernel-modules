// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Module Name: utuuid -- UUID support functions
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"

#define _COMPONENT          ACPI_COMPILER
ACPI_MODULE_NAME("utuuid")

#if (defined ACPI_ASL_COMPILER || defined ACPI_EXEC_APP || defined ACPI_HELP_APP)
/*
 * UUID support functions.
 *
 * This table is used to convert an input UUID ascii string to a 16 byte
 * buffer and the reverse. The table maps a UUID buffer index 0-15 to
 * the index within the 36-byte UUID string where the associated 2-byte
 * hex value can be found.
 *
 * 36-byte UUID strings are of the form:
 *     aabbccdd-eeff-gghh-iijj-kkllmmnnoopp
 * Where aa-pp are one byte hex numbers, made up of two hex digits
 *
 * Note: This table is basically the inverse of the string-to-offset table
 * found in the ACPI spec in the description of the to_UUID macro.
 */
const u8 acpi_gbl_map_to_uuid_offset[UUID_BUFFER_LENGTH] = {
	6, 4, 2, 0, 11, 9, 16, 14, 19, 21, 24, 26, 28, 30, 32, 34
};

/*******************************************************************************
 *
 * FUNCTION:    acpi_ut_convert_string_to_uuid
 *
 * PARAMETERS:  in_string           - 36-byte formatted UUID string
 *              uuid_buffer         - Where the 16-byte UUID buffer is returned
 *
 * RETURN:      None. Output data is returned in the uuid_buffer
 *
 * DESCRIPTION: Convert a 36-byte formatted UUID string to 16-byte UUID buffer
 *
 ******************************************************************************/

void acpi_ut_convert_string_to_uuid(char *in_string, u8 *uuid_buffer)
{
	u32 i;

	for (i = 0; i < UUID_BUFFER_LENGTH; i++) {
		uuid_buffer[i] =
		    (acpi_ut_ascii_char_to_hex
		     (in_string[acpi_gbl_map_to_uuid_offset[i]]) << 4);

		uuid_buffer[i] |=
		    acpi_ut_ascii_char_to_hex(in_string
					      [acpi_gbl_map_to_uuid_offset[i] +
					       1]);
	}
}
#endif
