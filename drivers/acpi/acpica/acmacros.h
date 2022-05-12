/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/******************************************************************************
 *
 * Name: acmacros.h - C macros for the entire subsystem.
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#ifndef __ACMACROS_H__
#define __ACMACROS_H__

/*
 * Extract data using a pointer. Any more than a byte and we
 * get into potential alignment issues -- see the STORE macros below.
 * Use with care.
 */
#define ACPI_CAST8(ptr)                 ACPI_CAST_PTR (u8, (ptr))
#define ACPI_CAST16(ptr)                ACPI_CAST_PTR (u16, (ptr))
#define ACPI_CAST32(ptr)                ACPI_CAST_PTR (u32, (ptr))
#define ACPI_CAST64(ptr)                ACPI_CAST_PTR (u64, (ptr))
#define ACPI_GET8(ptr)                  (*ACPI_CAST8 (ptr))
#define ACPI_GET16(ptr)                 (*ACPI_CAST16 (ptr))
#define ACPI_GET32(ptr)                 (*ACPI_CAST32 (ptr))
#define ACPI_GET64(ptr)                 (*ACPI_CAST64 (ptr))
#define ACPI_SET8(ptr, val)             (*ACPI_CAST8 (ptr) = (u8) (val))
#define ACPI_SET16(ptr, val)            (*ACPI_CAST16 (ptr) = (u16) (val))
#define ACPI_SET32(ptr, val)            (*ACPI_CAST32 (ptr) = (u32) (val))
#define ACPI_SET64(ptr, val)            (*ACPI_CAST64 (ptr) = (u64) (val))

/*
 * printf() format helper. This macro is a workaround for the difficulties
 * with emitting 64-bit integers and 64-bit pointers with the same code
 * for both 32-bit and 64-bit hosts.
 */
#define ACPI_FORMAT_UINT64(i)           ACPI_HIDWORD(i), ACPI_LODWORD(i)

/*
 * Macros for moving data around to/from buffers that are possibly unaligned.
 * If the hardware supports the transfer of unaligned data, just do the store.
 * Otherwise, we have to move one byte at a time.
 */
#ifdef ACPI_BIG_ENDIAN
/*
 * Macros for big-endian machines
 */

/* These macros reverse the bytes during the move, converting little-endian to big endian */

	 /* Big Endian      <==        Little Endian */
	 /*  Hi...Lo                     Lo...Hi     */
/* 16-bit source, 16/32/64 destination */

#define ACPI_MOVE_16_TO_16(d, s)        {((  u8 *)(void *)(d))[0] = ((u8 *)(void *)(s))[1];\
			  ((  u8 *)(void *)(d))[1] = ((u8 *)(void *)(s))[0];}

#define ACPI_MOVE_16_TO_32(d, s)        {(*(u32 *)(void *)(d))=0;\
					  ((u8 *)(void *)(d))[2] = ((u8 *)(void *)(s))[1];\
					  ((u8 *)(void *)(d))[3] = ((u8 *)(void *)(s))[0];}

#define ACPI_MOVE_16_TO_64(d, s)        {(*(u64 *)(void *)(d))=0;\
							   ((u8 *)(void *)(d))[6] = ((u8 *)(void *)(s))[1];\
							   ((u8 *)(void *)(d))[7] = ((u8 *)(void *)(s))[0];}

/* 32-bit source, 16/32/64 destination */

#define ACPI_MOVE_32_TO_16(d, s)        ACPI_MOVE_16_TO_16(d, s)	/* Truncate to 16 */

#define ACPI_MOVE_32_TO_32(d, s)        {((  u8 *)(void *)(d))[0] = ((u8 *)(void *)(s))[3];\
									  ((  u8 *)(void *)(d))[1] = ((u8 *)(void *)(s))[2];\
									  ((  u8 *)(void *)(d))[2] = ((u8 *)(void *)(s))[1];\
									  ((  u8 *)(void *)(d))[3] = ((u8 *)(void *)(s))[0];}

#define ACPI_MOVE_32_TO_64(d, s)        {(*(u64 *)(void *)(d))=0;\
										   ((u8 *)(void *)(d))[4] = ((u8 *)(void *)(s))[3];\
										   ((u8 *)(void *)(d))[5] = ((u8 *)(void *)(s))[2];\
										   ((u8 *)(void *)(d))[6] = ((u8 *)(void *)(s))[1];\
										   ((u8 *)(void *)(d))[7] = ((u8 *)(void *)(s))[0];}

/* 64-bit source, 16/32/64 destination */

#define ACPI_MOVE_64_TO_16(d, s)        ACPI_MOVE_16_TO_16(d, s)	/* Truncate to 16 */

#define ACPI_MOVE_64_TO_32(d, s)        ACPI_MOVE_32_TO_32(d, s)	/* Truncate to 32 */

#define ACPI_MOVE_64_TO_64(d, s)        {((  u8 *)(void *)(d))[0] = ((u8 *)(void *)(s))[7];\
										 ((  u8 *)(void *)(d))[1] = ((u8 *)(void *)(s))[6];\
										 ((  u8 *)(void *)(d))[2] = ((u8 *)(void *)(s))[5];\
										 ((  u8 *)(void *)(d))[3] = ((u8 *)(void *)(s))[4];\
										 ((  u8 *)(void *)(d))[4] = ((u8 *)(void *)(s))[3];\
										 ((  u8 *)(void *)(d))[5] = ((u8 *)(void *)(s))[2];\
										 ((  u8 *)(void *)(d))[6] = ((u8 *)(void *)(s))[1];\
										 ((  u8 *)(void *)(d))[7] = ((u8 *)(void *)(s))[0];}
#else
/*
 * Macros for little-endian machines
 */

#ifndef ACPI_MISALIGNMENT_NOT_SUPPORTED

/* The hardware supports unaligned transfers, just do the little-endian move */

/* 16-bit source, 16/32/64 destination */

#define ACPI_MOVE_16_TO_16(d, s)        *(u16 *)(void *)(d) = *(u16 *)(void *)(s)
#define ACPI_MOVE_16_TO_32(d, s)        *(u32 *)(void *)(d) = *(u16 *)(void *)(s)
#define ACPI_MOVE_16_TO_64(d, s)        *(u64 *)(void *)(d) = *(u16 *)(void *)(s)

/* 32-bit source, 16/32/64 destination */

#define ACPI_MOVE_32_TO_16(d, s)        ACPI_MOVE_16_TO_16(d, s)	/* Truncate to 16 */
#define ACPI_MOVE_32_TO_32(d, s)        *(u32 *)(void *)(d) = *(u32 *)(void *)(s)
#define ACPI_MOVE_32_TO_64(d, s)        *(u64 *)(void *)(d) = *(u32 *)(void *)(s)

/* 64-bit source, 16/32/64 destination */

#define ACPI_MOVE_64_TO_16(d, s)        ACPI_MOVE_16_TO_16(d, s)	/* Truncate to 16 */
#define ACPI_MOVE_64_TO_32(d, s)        ACPI_MOVE_32_TO_32(d, s)	/* Truncate to 32 */
#define ACPI_MOVE_64_TO_64(d, s)        *(u64 *)(void *)(d) = *(u64 *)(void *)(s)

#else
/*
 * The hardware does not support unaligned transfers. We must move the
 * data one byte at a time. These macros work whether the source or
 * the destination (or both) is/are unaligned. (Little-endian move)
 */

/* 16-bit source, 16/32/64 destination */

#define ACPI_MOVE_16_TO_16(d, s)        {((  u8 *)(void *)(d))[0] = ((u8 *)(void *)(s))[0];\
										 ((  u8 *)(void *)(d))[1] = ((u8 *)(void *)(s))[1];}

#define ACPI_MOVE_16_TO_32(d, s)        {(*(u32 *)(void *)(d)) = 0; ACPI_MOVE_16_TO_16(d, s);}
#define ACPI_MOVE_16_TO_64(d, s)        {(*(u64 *)(void *)(d)) = 0; ACPI_MOVE_16_TO_16(d, s);}

/* 32-bit source, 16/32/64 destination */

#define ACPI_MOVE_32_TO_16(d, s)        ACPI_MOVE_16_TO_16(d, s)	/* Truncate to 16 */

#define ACPI_MOVE_32_TO_32(d, s)        {((  u8 *)(void *)(d))[0] = ((u8 *)(void *)(s))[0];\
										 ((  u8 *)(void *)(d))[1] = ((u8 *)(void *)(s))[1];\
										 ((  u8 *)(void *)(d))[2] = ((u8 *)(void *)(s))[2];\
										 ((  u8 *)(void *)(d))[3] = ((u8 *)(void *)(s))[3];}

#define ACPI_MOVE_32_TO_64(d, s)        {(*(u64 *)(void *)(d)) = 0; ACPI_MOVE_32_TO_32(d, s);}

/* 64-bit source, 16/32/64 destination */

#define ACPI_MOVE_64_TO_16(d, s)        ACPI_MOVE_16_TO_16(d, s)	/* Truncate to 16 */
#define ACPI_MOVE_64_TO_32(d, s)        ACPI_MOVE_32_TO_32(d, s)	/* Truncate to 32 */
#define ACPI_MOVE_64_TO_64(d, s)        {((  u8 *)(void *)(d))[0] = ((u8 *)(void *)(s))[0];\
										 ((  u8 *)(void *)(d))[1] = ((u8 *)(void *)(s))[1];\
										 ((  u8 *)(void *)(d))[2] = ((u8 *)(void *)(s))[2];\
										 ((  u8 *)(void *)(d))[3] = ((u8 *)(void *)(s))[3];\
										 ((  u8 *)(void *)(d))[4] = ((u8 *)(void *)(s))[4];\
										 ((  u8 *)(void *)(d))[5] = ((u8 *)(void *)(s))[5];\
										 ((  u8 *)(void *)(d))[6] = ((u8 *)(void *)(s))[6];\
										 ((  u8 *)(void *)(d))[7] = ((u8 *)(void *)(s))[7];}
#endif
#endif

/*
 * Fast power-of-two math macros for non-optimized compilers
 */
#define _ACPI_DIV(value, power_of2)     ((u32) ((value) >> (power_of2)))
#define _ACPI_MUL(value, power_of2)     ((u32) ((value) << (power_of2)))
#define _ACPI_MOD(value, divisor)       ((u32) ((value) & ((divisor) -1)))

#define ACPI_DIV_2(a)                   _ACPI_DIV(a, 1)
#define ACPI_MUL_2(a)                   _ACPI_MUL(a, 1)
#define ACPI_MOD_2(a)                   _ACPI_MOD(a, 2)

#define ACPI_DIV_4(a)                   _ACPI_DIV(a, 2)
#define ACPI_MUL_4(a)                   _ACPI_MUL(a, 2)
#define ACPI_MOD_4(a)                   _ACPI_MOD(a, 4)

#define ACPI_DIV_8(a)                   _ACPI_DIV(a, 3)
#define ACPI_MUL_8(a)                   _ACPI_MUL(a, 3)
#define ACPI_MOD_8(a)                   _ACPI_MOD(a, 8)

#define ACPI_DIV_16(a)                  _ACPI_DIV(a, 4)
#define ACPI_MUL_16(a)                  _ACPI_MUL(a, 4)
#define ACPI_MOD_16(a)                  _ACPI_MOD(a, 16)

#define ACPI_DIV_32(a)                  _ACPI_DIV(a, 5)
#define ACPI_MUL_32(a)                  _ACPI_MUL(a, 5)
#define ACPI_MOD_32(a)                  _ACPI_MOD(a, 32)

/* Test for ASCII character */

#define ACPI_IS_ASCII(c)                ((c) < 0x80)

/* Signed integers */

#define ACPI_SIGN_POSITIVE              0
#define ACPI_SIGN_NEGATIVE              1

/*
 * Rounding macros (Power of two boundaries only)
 */
#define ACPI_ROUND_DOWN(value, boundary)    (((acpi_size)(value)) & \
												(~(((acpi_size) boundary)-1)))

#define ACPI_ROUND_UP(value, boundary)      ((((acpi_size)(value)) + \
												(((acpi_size) boundary)-1)) & \
												(~(((acpi_size) boundary)-1)))

/* Note: sizeof(acpi_size) evaluates to either 4 or 8 (32- vs 64-bit mode) */

#define ACPI_ROUND_DOWN_TO_32BIT(a)         ACPI_ROUND_DOWN(a, 4)
#define ACPI_ROUND_DOWN_TO_64BIT(a)         ACPI_ROUND_DOWN(a, 8)
#define ACPI_ROUND_DOWN_TO_NATIVE_WORD(a)   ACPI_ROUND_DOWN(a, sizeof(acpi_size))

#define ACPI_ROUND_UP_TO_32BIT(a)           ACPI_ROUND_UP(a, 4)
#define ACPI_ROUND_UP_TO_64BIT(a)           ACPI_ROUND_UP(a, 8)
#define ACPI_ROUND_UP_TO_NATIVE_WORD(a)     ACPI_ROUND_UP(a, sizeof(acpi_size))

#define ACPI_ROUND_BITS_UP_TO_BYTES(a)      ACPI_DIV_8((a) + 7)
#define ACPI_ROUND_BITS_DOWN_TO_BYTES(a)    ACPI_DIV_8((a))

#define ACPI_ROUND_UP_TO_1K(a)              (((a) + 1023) >> 10)

/* Generic (non-power-of-two) rounding */

#define ACPI_ROUND_UP_TO(value, boundary)   (((value) + ((boundary)-1)) / (boundary))

#define ACPI_IS_MISALIGNED(value)           (((acpi_size) value) & (sizeof(acpi_size)-1))

/* Generic bit manipulation */

#ifndef ACPI_USE_NATIVE_BIT_FINDER

#define __ACPI_FIND_LAST_BIT_2(a, r)        ((((u8)  (a)) & 0x02) ? (r)+1 : (r))
#define __ACPI_FIND_LAST_BIT_4(a, r)        ((((u8)  (a)) & 0x0C) ? \
											 __ACPI_FIND_LAST_BIT_2  ((a)>>2,  (r)+2) : \
											 __ACPI_FIND_LAST_BIT_2  ((a), (r)))
#define __ACPI_FIND_LAST_BIT_8(a, r)        ((((u8)  (a)) & 0xF0) ? \
											 __ACPI_FIND_LAST_BIT_4  ((a)>>4,  (r)+4) : \
											 __ACPI_FIND_LAST_BIT_4  ((a), (r)))
#define __ACPI_FIND_LAST_BIT_16(a, r)       ((((u16) (a)) & 0xFF00) ? \
											 __ACPI_FIND_LAST_BIT_8  ((a)>>8,  (r)+8) : \
											 __ACPI_FIND_LAST_BIT_8  ((a), (r)))
#define __ACPI_FIND_LAST_BIT_32(a, r)       ((((u32) (a)) & 0xFFFF0000) ? \
											 __ACPI_FIND_LAST_BIT_16 ((a)>>16, (r)+16) : \
											 __ACPI_FIND_LAST_BIT_16 ((a), (r)))
#define __ACPI_FIND_LAST_BIT_64(a, r)       ((((u64) (a)) & 0xFFFFFFFF00000000) ? \
											 __ACPI_FIND_LAST_BIT_32 ((a)>>32, (r)+32) : \
											 __ACPI_FIND_LAST_BIT_32 ((a), (r)))

#define ACPI_FIND_LAST_BIT_8(a)             ((a) ? __ACPI_FIND_LAST_BIT_8 (a, 1) : 0)
#define ACPI_FIND_LAST_BIT_16(a)            ((a) ? __ACPI_FIND_LAST_BIT_16 (a, 1) : 0)
#define ACPI_FIND_LAST_BIT_32(a)            ((a) ? __ACPI_FIND_LAST_BIT_32 (a, 1) : 0)
#define ACPI_FIND_LAST_BIT_64(a)            ((a) ? __ACPI_FIND_LAST_BIT_64 (a, 1) : 0)

#define __ACPI_FIND_FIRST_BIT_2(a, r)       ((((u8) (a)) & 0x01) ? (r) : (r)+1)
#define __ACPI_FIND_FIRST_BIT_4(a, r)       ((((u8) (a)) & 0x03) ? \
											 __ACPI_FIND_FIRST_BIT_2  ((a), (r)) : \
											 __ACPI_FIND_FIRST_BIT_2  ((a)>>2, (r)+2))
#define __ACPI_FIND_FIRST_BIT_8(a, r)       ((((u8) (a)) & 0x0F) ? \
											 __ACPI_FIND_FIRST_BIT_4  ((a), (r)) : \
											 __ACPI_FIND_FIRST_BIT_4  ((a)>>4, (r)+4))
#define __ACPI_FIND_FIRST_BIT_16(a, r)      ((((u16) (a)) & 0x00FF) ? \
											 __ACPI_FIND_FIRST_BIT_8  ((a), (r)) : \
											 __ACPI_FIND_FIRST_BIT_8  ((a)>>8, (r)+8))
#define __ACPI_FIND_FIRST_BIT_32(a, r)      ((((u32) (a)) & 0x0000FFFF) ? \
											 __ACPI_FIND_FIRST_BIT_16 ((a), (r)) : \
											 __ACPI_FIND_FIRST_BIT_16 ((a)>>16, (r)+16))
#define __ACPI_FIND_FIRST_BIT_64(a, r)      ((((u64) (a)) & 0x00000000FFFFFFFF) ? \
											 __ACPI_FIND_FIRST_BIT_32 ((a), (r)) : \
											 __ACPI_FIND_FIRST_BIT_32 ((a)>>32, (r)+32))

#define ACPI_FIND_FIRST_BIT_8(a)            ((a) ? __ACPI_FIND_FIRST_BIT_8 (a, 1) : 0)
#define ACPI_FIND_FIRST_BIT_16(a)           ((a) ? __ACPI_FIND_FIRST_BIT_16 (a, 1) : 0)
#define ACPI_FIND_FIRST_BIT_32(a)           ((a) ? __ACPI_FIND_FIRST_BIT_32 (a, 1) : 0)
#define ACPI_FIND_FIRST_BIT_64(a)           ((a) ? __ACPI_FIND_FIRST_BIT_64 (a, 1) : 0)

#endif				/* ACPI_USE_NATIVE_BIT_FINDER */

/* Generic (power-of-two) rounding */

#define ACPI_ROUND_UP_POWER_OF_TWO_8(a)     ((u8) \
											(((u16) 1) <<  ACPI_FIND_LAST_BIT_8  ((a)  - 1)))
#define ACPI_ROUND_DOWN_POWER_OF_TWO_8(a)   ((u8) \
											(((u16) 1) << (ACPI_FIND_LAST_BIT_8  ((a)) - 1)))
#define ACPI_ROUND_UP_POWER_OF_TWO_16(a)    ((u16) \
											(((u32) 1) <<  ACPI_FIND_LAST_BIT_16 ((a)  - 1)))
#define ACPI_ROUND_DOWN_POWER_OF_TWO_16(a)  ((u16) \
											(((u32) 1) << (ACPI_FIND_LAST_BIT_16 ((a)) - 1)))
#define ACPI_ROUND_UP_POWER_OF_TWO_32(a)    ((u32) \
											(((u64) 1) <<  ACPI_FIND_LAST_BIT_32 ((a)  - 1)))
#define ACPI_ROUND_DOWN_POWER_OF_TWO_32(a)  ((u32) \
											(((u64) 1) << (ACPI_FIND_LAST_BIT_32 ((a)) - 1)))
#define ACPI_IS_ALIGNED(a, s)               (((a) & ((s) - 1)) == 0)
#define ACPI_IS_POWER_OF_TWO(a)             ACPI_IS_ALIGNED(a, a)

/*
 * Bitmask creation
 * Bit positions start at zero.
 * MASK_BITS_ABOVE creates a mask starting AT the position and above
 * MASK_BITS_BELOW creates a mask starting one bit BELOW the position
 * MASK_BITS_ABOVE/BELOW accepts a bit offset to create a mask
 * MASK_BITS_ABOVE/BELOW_32/64 accepts a bit width to create a mask
 * Note: The ACPI_INTEGER_BIT_SIZE check is used to bypass compiler
 * differences with the shift operator
 */
#define ACPI_MASK_BITS_ABOVE(position)      (~((ACPI_UINT64_MAX) << ((u32) (position))))
#define ACPI_MASK_BITS_BELOW(position)      ((ACPI_UINT64_MAX) << ((u32) (position)))
#define ACPI_MASK_BITS_ABOVE_32(width)      ((u32) ACPI_MASK_BITS_ABOVE(width))
#define ACPI_MASK_BITS_BELOW_32(width)      ((u32) ACPI_MASK_BITS_BELOW(width))
#define ACPI_MASK_BITS_ABOVE_64(width)      ((width) == ACPI_INTEGER_BIT_SIZE ? \
												ACPI_UINT64_MAX : \
												ACPI_MASK_BITS_ABOVE(width))
#define ACPI_MASK_BITS_BELOW_64(width)      ((width) == ACPI_INTEGER_BIT_SIZE ? \
												(u64) 0 : \
												ACPI_MASK_BITS_BELOW(width))

/* Bitfields within ACPI registers */

#define ACPI_REGISTER_PREPARE_BITS(val, pos, mask) \
	((val << pos) & mask)

#define ACPI_REGISTER_INSERT_VALUE(reg, pos, mask, val) \
	reg = (reg & (~(mask))) | ACPI_REGISTER_PREPARE_BITS(val, pos, mask)

#define ACPI_INSERT_BITS(target, mask, source) \
	target = ((target & (~(mask))) | (source & mask))

/* Generic bitfield macros and masks */

#define ACPI_GET_BITS(source_ptr, position, mask) \
	((*(source_ptr) >> (position)) & (mask))

#define ACPI_SET_BITS(target_ptr, position, mask, value) \
	(*(target_ptr) |= (((value) & (mask)) << (position)))

#define ACPI_1BIT_MASK      0x00000001
#define ACPI_2BIT_MASK      0x00000003
#define ACPI_3BIT_MASK      0x00000007
#define ACPI_4BIT_MASK      0x0000000F
#define ACPI_5BIT_MASK      0x0000001F
#define ACPI_6BIT_MASK      0x0000003F
#define ACPI_7BIT_MASK      0x0000007F
#define ACPI_8BIT_MASK      0x000000FF
#define ACPI_16BIT_MASK     0x0000FFFF
#define ACPI_24BIT_MASK     0x00FFFFFF

/* Macros to extract flag bits from position zero */

#define ACPI_GET_1BIT_FLAG(value)                   ((value) & ACPI_1BIT_MASK)
#define ACPI_GET_2BIT_FLAG(value)                   ((value) & ACPI_2BIT_MASK)
#define ACPI_GET_3BIT_FLAG(value)                   ((value) & ACPI_3BIT_MASK)
#define ACPI_GET_4BIT_FLAG(value)                   ((value) & ACPI_4BIT_MASK)

/* Macros to extract flag bits from position one and above */

#define ACPI_EXTRACT_1BIT_FLAG(field, position)     (ACPI_GET_1BIT_FLAG ((field) >> position))
#define ACPI_EXTRACT_2BIT_FLAG(field, position)     (ACPI_GET_2BIT_FLAG ((field) >> position))
#define ACPI_EXTRACT_3BIT_FLAG(field, position)     (ACPI_GET_3BIT_FLAG ((field) >> position))
#define ACPI_EXTRACT_4BIT_FLAG(field, position)     (ACPI_GET_4BIT_FLAG ((field) >> position))

/* ACPI Pathname helpers */

#define ACPI_IS_ROOT_PREFIX(c)      ((c) == (u8) 0x5C)	/* Backslash */
#define ACPI_IS_PARENT_PREFIX(c)    ((c) == (u8) 0x5E)	/* Carat */
#define ACPI_IS_PATH_SEPARATOR(c)   ((c) == (u8) 0x2E)	/* Period (dot) */

/*
 * An object of type struct acpi_namespace_node can appear in some contexts
 * where a pointer to an object of type union acpi_operand_object can also
 * appear. This macro is used to distinguish them.
 *
 * The "DescriptorType" field is the second field in both structures.
 */
#define ACPI_GET_DESCRIPTOR_PTR(d)      (((union acpi_descriptor *)(void *)(d))->common.common_pointer)
#define ACPI_SET_DESCRIPTOR_PTR(d, p)   (((union acpi_descriptor *)(void *)(d))->common.common_pointer = (p))
#define ACPI_GET_DESCRIPTOR_TYPE(d)     (((union acpi_descriptor *)(void *)(d))->common.descriptor_type)
#define ACPI_SET_DESCRIPTOR_TYPE(d, t)  (((union acpi_descriptor *)(void *)(d))->common.descriptor_type = (t))

/*
 * Macros for the master AML opcode table
 */
#if defined (ACPI_DISASSEMBLER) || defined (ACPI_DEBUG_OUTPUT)
#define ACPI_OP(name, Pargs, Iargs, obj_type, class, type, flags) \
	{name, (u32)(Pargs), (u32)(Iargs), (u32)(flags), obj_type, class, type}
#else
#define ACPI_OP(name, Pargs, Iargs, obj_type, class, type, flags) \
	{(u32)(Pargs), (u32)(Iargs), (u32)(flags), obj_type, class, type}
#endif

#define ARG_TYPE_WIDTH                  5
#define ARG_1(x)                        ((u32)(x))
#define ARG_2(x)                        ((u32)(x) << (1 * ARG_TYPE_WIDTH))
#define ARG_3(x)                        ((u32)(x) << (2 * ARG_TYPE_WIDTH))
#define ARG_4(x)                        ((u32)(x) << (3 * ARG_TYPE_WIDTH))
#define ARG_5(x)                        ((u32)(x) << (4 * ARG_TYPE_WIDTH))
#define ARG_6(x)                        ((u32)(x) << (5 * ARG_TYPE_WIDTH))

#define ARGI_LIST1(a)                   (ARG_1(a))
#define ARGI_LIST2(a, b)                (ARG_1(b)|ARG_2(a))
#define ARGI_LIST3(a, b, c)             (ARG_1(c)|ARG_2(b)|ARG_3(a))
#define ARGI_LIST4(a, b, c, d)          (ARG_1(d)|ARG_2(c)|ARG_3(b)|ARG_4(a))
#define ARGI_LIST5(a, b, c, d, e)       (ARG_1(e)|ARG_2(d)|ARG_3(c)|ARG_4(b)|ARG_5(a))
#define ARGI_LIST6(a, b, c, d, e, f)    (ARG_1(f)|ARG_2(e)|ARG_3(d)|ARG_4(c)|ARG_5(b)|ARG_6(a))

#define ARGP_LIST1(a)                   (ARG_1(a))
#define ARGP_LIST2(a, b)                (ARG_1(a)|ARG_2(b))
#define ARGP_LIST3(a, b, c)             (ARG_1(a)|ARG_2(b)|ARG_3(c))
#define ARGP_LIST4(a, b, c, d)          (ARG_1(a)|ARG_2(b)|ARG_3(c)|ARG_4(d))
#define ARGP_LIST5(a, b, c, d, e)       (ARG_1(a)|ARG_2(b)|ARG_3(c)|ARG_4(d)|ARG_5(e))
#define ARGP_LIST6(a, b, c, d, e, f)    (ARG_1(a)|ARG_2(b)|ARG_3(c)|ARG_4(d)|ARG_5(e)|ARG_6(f))

#define GET_CURRENT_ARG_TYPE(list)      (list & ((u32) 0x1F))
#define INCREMENT_ARG_LIST(list)        (list >>= ((u32) ARG_TYPE_WIDTH))

/*
 * Ascii error messages can be configured out
 */
#ifndef ACPI_NO_ERROR_MESSAGES
/*
 * Error reporting. The callers module and line number are inserted by AE_INFO,
 * the plist contains a set of parens to allow variable-length lists.
 * These macros are used for both the debug and non-debug versions of the code.
 */
#define ACPI_ERROR_NAMESPACE(s, p, e)       acpi_ut_prefixed_namespace_error (AE_INFO, s, p, e);
#define ACPI_ERROR_METHOD(s, n, p, e)       acpi_ut_method_error (AE_INFO, s, n, p, e);
#define ACPI_WARN_PREDEFINED(plist)         acpi_ut_predefined_warning plist
#define ACPI_INFO_PREDEFINED(plist)         acpi_ut_predefined_info plist
#define ACPI_BIOS_ERROR_PREDEFINED(plist)   acpi_ut_predefined_bios_error plist
#define ACPI_ERROR_ONLY(s)                  s

#else

/* No error messages */

#define ACPI_ERROR_NAMESPACE(s, p, e)
#define ACPI_ERROR_METHOD(s, n, p, e)
#define ACPI_WARN_PREDEFINED(plist)
#define ACPI_INFO_PREDEFINED(plist)
#define ACPI_BIOS_ERROR_PREDEFINED(plist)
#define ACPI_ERROR_ONLY(s)

#endif				/* ACPI_NO_ERROR_MESSAGES */

#if (!ACPI_REDUCED_HARDWARE)
#define ACPI_HW_OPTIONAL_FUNCTION(addr)     addr
#else
#define ACPI_HW_OPTIONAL_FUNCTION(addr)     NULL
#endif

/*
 * Macros used for ACPICA utilities only
 */

/* Generate a UUID */

#define ACPI_INIT_UUID(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7) \
	(a) & 0xFF, ((a) >> 8) & 0xFF, ((a) >> 16) & 0xFF, ((a) >> 24) & 0xFF, \
	(b) & 0xFF, ((b) >> 8) & 0xFF, \
	(c) & 0xFF, ((c) >> 8) & 0xFF, \
	(d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7)

#define ACPI_IS_OCTAL_DIGIT(d)              (((char)(d) >= '0') && ((char)(d) <= '7'))

/*
 * Macros used for the ASL-/ASL+ converter utility
 */
#ifdef ACPI_ASL_COMPILER

#define ASL_CV_LABEL_FILENODE(a)         cv_label_file_node(a);
#define ASL_CV_CAPTURE_COMMENTS_ONLY(a)   cv_capture_comments_only (a);
#define ASL_CV_CAPTURE_COMMENTS(a)       cv_capture_comments (a);
#define ASL_CV_TRANSFER_COMMENTS(a)      cv_transfer_comments (a);
#define ASL_CV_CLOSE_PAREN(a,b)          cv_close_paren_write_comment(a,b);
#define ASL_CV_CLOSE_BRACE(a,b)          cv_close_brace_write_comment(a,b);
#define ASL_CV_SWITCH_FILES(a,b)         cv_switch_files(a,b);
#define ASL_CV_CLEAR_OP_COMMENTS(a)       cv_clear_op_comments(a);
#define ASL_CV_PRINT_ONE_COMMENT(a,b,c,d) cv_print_one_comment_type (a,b,c,d);
#define ASL_CV_PRINT_ONE_COMMENT_LIST(a,b) cv_print_one_comment_list (a,b);
#define ASL_CV_FILE_HAS_SWITCHED(a)       cv_file_has_switched(a)
#define ASL_CV_INIT_FILETREE(a,b)      cv_init_file_tree(a,b);

#else

#define ASL_CV_LABEL_FILENODE(a)
#define ASL_CV_CAPTURE_COMMENTS_ONLY(a)
#define ASL_CV_CAPTURE_COMMENTS(a)
#define ASL_CV_TRANSFER_COMMENTS(a)
#define ASL_CV_CLOSE_PAREN(a,b)          acpi_os_printf (")");
#define ASL_CV_CLOSE_BRACE(a,b)          acpi_os_printf ("}");
#define ASL_CV_SWITCH_FILES(a,b)
#define ASL_CV_CLEAR_OP_COMMENTS(a)
#define ASL_CV_PRINT_ONE_COMMENT(a,b,c,d)
#define ASL_CV_PRINT_ONE_COMMENT_LIST(a,b)
#define ASL_CV_FILE_HAS_SWITCHED(a)       0
#define ASL_CV_INIT_FILETREE(a,b)

#endif

#endif				/* ACMACROS_H */
