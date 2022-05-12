// SPDX-License-Identifier: GPL-2.0-only
/*
 * VMware VMCI Driver
 *
 * Copyright (C) 2012 VMware, Inc. All rights reserved.
 */

#include <linux/slab.h>
#include "vmci_handle_array.h"

static size_t handle_arr_calc_size(u32 capacity)
{
	return VMCI_HANDLE_ARRAY_HEADER_SIZE +
	    capacity * sizeof(struct vmci_handle);
}

struct vmci_handle_arr *vmci_handle_arr_create(u32 capacity, u32 max_capacity)
{
	struct vmci_handle_arr *array;

	if (max_capacity == 0 || capacity > max_capacity)
		return NULL;

	if (capacity == 0)
		capacity = min((u32)VMCI_HANDLE_ARRAY_DEFAULT_CAPACITY,
			       max_capacity);

	array = kmalloc(handle_arr_calc_size(capacity), GFP_ATOMIC);
	if (!array)
		return NULL;

	array->capacity = capacity;
	array->max_capacity = max_capacity;
	array->size = 0;

	return array;
}

void vmci_handle_arr_destroy(struct vmci_handle_arr *array)
{
	kfree(array);
}

int vmci_handle_arr_append_entry(struct vmci_handle_arr **array_ptr,
				 struct vmci_handle handle)
{
	struct vmci_handle_arr *array = *array_ptr;

	if (unlikely(array->size >= array->capacity)) {
		/* reallocate. */
		struct vmci_handle_arr *new_array;
		u32 capacity_bump = min(array->max_capacity - array->capacity,
					array->capacity);
		size_t new_size = handle_arr_calc_size(array->capacity +
						       capacity_bump);

		if (array->size >= array->max_capacity)
			return VMCI_ERROR_NO_MEM;

		new_array = krealloc(array, new_size, GFP_ATOMIC);
		if (!new_array)
			return VMCI_ERROR_NO_MEM;

		new_array->capacity += capacity_bump;
		*array_ptr = array = new_array;
	}

	array->entries[array->size] = handle;
	array->size++;

	return VMCI_SUCCESS;
}

/*
 * Handle that was removed, VMCI_INVALID_HANDLE if entry not found.
 */
struct vmci_handle vmci_handle_arr_remove_entry(struct vmci_handle_arr *array,
						struct vmci_handle entry_handle)
{
	struct vmci_handle handle = VMCI_INVALID_HANDLE;
	u32 i;

	for (i = 0; i < array->size; i++) {
		if (vmci_handle_is_equal(array->entries[i], entry_handle)) {
			handle = array->entries[i];
			array->size--;
			array->entries[i] = array->entries[array->size];
			array->entries[array->size] = VMCI_INVALID_HANDLE;
			break;
		}
	}

	return handle;
}

/*
 * Handle that was removed, VMCI_INVALID_HANDLE if array was empty.
 */
struct vmci_handle vmci_handle_arr_remove_tail(struct vmci_handle_arr *array)
{
	struct vmci_handle handle = VMCI_INVALID_HANDLE;

	if (array->size) {
		array->size--;
		handle = array->entries[array->size];
		array->entries[array->size] = VMCI_INVALID_HANDLE;
	}

	return handle;
}

/*
 * Handle at given index, VMCI_INVALID_HANDLE if invalid index.
 */
struct vmci_handle
vmci_handle_arr_get_entry(const struct vmci_handle_arr *array, u32 index)
{
	if (unlikely(index >= array->size))
		return VMCI_INVALID_HANDLE;

	return array->entries[index];
}

bool vmci_handle_arr_has_entry(const struct vmci_handle_arr *array,
			       struct vmci_handle entry_handle)
{
	u32 i;

	for (i = 0; i < array->size; i++)
		if (vmci_handle_is_equal(array->entries[i], entry_handle))
			return true;

	return false;
}

/*
 * NULL if the array is empty. Otherwise, a pointer to the array
 * of VMCI handles in the handle array.
 */
struct vmci_handle *vmci_handle_arr_get_handles(struct vmci_handle_arr *array)
{
	if (array->size)
		return array->entries;

	return NULL;
}
