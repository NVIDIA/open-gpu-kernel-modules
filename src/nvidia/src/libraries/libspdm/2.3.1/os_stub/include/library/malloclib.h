/**
 *  Copyright Notice:
 *  Copyright 2021-2022 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

/** @file
 * Provides services to allocate and free memory buffers of various memory types and alignments.
 *
 * The Memory Allocation Library abstracts various common memory allocation operations. This library
 * allows code to be written in a phase-independent manner because the allocation of memory in PEI, DXE,
 * and SMM (for example) is done via a different mechanism. Using a common library interface makes it
 * much easier to port algorithms from phase to phase.
 **/

#ifndef __MEMORY_ALLOCATION_LIB_H__
#define __MEMORY_ALLOCATION_LIB_H__

/**
 * Allocates a buffer of type EfiBootServicesData.
 *
 * Allocates the number bytes specified by AllocationSize of type EfiBootServicesData and returns a
 * pointer to the allocated buffer.  If AllocationSize is 0, then a valid buffer of 0 size is
 * returned.  If there is not enough memory remaining to satisfy the request, then NULL is returned.
 *
 * @param  AllocationSize        The number of bytes to allocate.
 *
 * @return A pointer to the allocated buffer or NULL if allocation fails.
 *
 **/
void *allocate_pool(size_t AllocationSize);

/**
 * Allocates and zeros a buffer of type EfiBootServicesData.
 *
 * Allocates the number bytes specified by AllocationSize of type EfiBootServicesData, clears the
 * buffer with zeros, and returns a pointer to the allocated buffer.  If AllocationSize is 0, then a
 * valid buffer of 0 size is returned.  If there is not enough memory remaining to satisfy the
 * request, then NULL is returned.
 *
 * @param  AllocationSize        The number of bytes to allocate and zero.
 *
 * @return A pointer to the allocated buffer or NULL if allocation fails.
 *
 **/
void *allocate_zero_pool(size_t AllocationSize);

/**
 * Frees a buffer that was previously allocated with one of the pool allocation functions in the
 * Memory Allocation Library.
 *
 * Frees the buffer specified by buffer.  buffer must have been allocated on a previous call to the
 * pool allocation services of the Memory Allocation Library.  If it is not possible to free pool
 * resources, then this function will perform no actions.
 *
 * If buffer was not allocated with a pool allocation function in the Memory Allocation Library,
 * then ASSERT().
 *
 * @param  buffer                Pointer to the buffer to free.
 *
 **/
void free_pool(void *buffer);

#endif
