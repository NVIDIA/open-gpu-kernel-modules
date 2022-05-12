/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright 2000-2014 Avago Technologies.  All rights reserved.
 *
 *
 *           Name:  mpi2_type.h
 *          Title:  MPI basic type definitions
 *  Creation Date:  August 16, 2006
 *
 *    mpi2_type.h Version:  02.00.01
 *
 *  Version History
 *  ---------------
 *
 *  Date      Version   Description
 *  --------  --------  ------------------------------------------------------
 *  04-30-07  02.00.00  Corresponds to Fusion-MPT MPI Specification Rev A.
 *  11-18-14  02.00.01  Updated copyright information.
 *  --------------------------------------------------------------------------
 */

#ifndef MPI2_TYPE_H
#define MPI2_TYPE_H

/*******************************************************************************
 * Define * if it hasn't already been defined. By default
 * * is defined to be a near pointer. MPI2_POINTER can be defined as
 * a far pointer by defining * as "far *" before this header file is
 * included.
 */

/* the basic types may have already been included by mpi_type.h */
#ifndef MPI_TYPE_H
/*****************************************************************************
*
*               Basic Types
*
*****************************************************************************/

typedef u8 U8;
typedef __le16 U16;
typedef __le32 U32;
typedef __le64 U64 __attribute__ ((aligned(4)));

/*****************************************************************************
*
*               Pointer Types
*
*****************************************************************************/

typedef U8 *PU8;
typedef U16 *PU16;
typedef U32 *PU32;
typedef U64 *PU64;

#endif

#endif
