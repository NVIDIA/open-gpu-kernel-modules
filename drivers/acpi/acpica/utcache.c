// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Module Name: utcache - local cache allocation routines
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#include <acpi/acpi.h>
#include "accommon.h"

#define _COMPONENT          ACPI_UTILITIES
ACPI_MODULE_NAME("utcache")

#ifdef ACPI_USE_LOCAL_CACHE
/*******************************************************************************
 *
 * FUNCTION:    acpi_os_create_cache
 *
 * PARAMETERS:  cache_name      - Ascii name for the cache
 *              object_size     - Size of each cached object
 *              max_depth       - Maximum depth of the cache (in objects)
 *              return_cache    - Where the new cache object is returned
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Create a cache object
 *
 ******************************************************************************/
acpi_status
acpi_os_create_cache(char *cache_name,
		     u16 object_size,
		     u16 max_depth, struct acpi_memory_list **return_cache)
{
	struct acpi_memory_list *cache;

	ACPI_FUNCTION_ENTRY();

	if (!cache_name || !return_cache || !object_size) {
		return (AE_BAD_PARAMETER);
	}

	/* Create the cache object */

	cache = acpi_os_allocate(sizeof(struct acpi_memory_list));
	if (!cache) {
		return (AE_NO_MEMORY);
	}

	/* Populate the cache object and return it */

	memset(cache, 0, sizeof(struct acpi_memory_list));
	cache->list_name = cache_name;
	cache->object_size = object_size;
	cache->max_depth = max_depth;

	*return_cache = cache;
	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_os_purge_cache
 *
 * PARAMETERS:  cache           - Handle to cache object
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Free all objects within the requested cache.
 *
 ******************************************************************************/

acpi_status acpi_os_purge_cache(struct acpi_memory_list *cache)
{
	void *next;
	acpi_status status;

	ACPI_FUNCTION_ENTRY();

	if (!cache) {
		return (AE_BAD_PARAMETER);
	}

	status = acpi_ut_acquire_mutex(ACPI_MTX_CACHES);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/* Walk the list of objects in this cache */

	while (cache->list_head) {

		/* Delete and unlink one cached state object */

		next = ACPI_GET_DESCRIPTOR_PTR(cache->list_head);
		ACPI_FREE(cache->list_head);

		cache->list_head = next;
		cache->current_depth--;
	}

	(void)acpi_ut_release_mutex(ACPI_MTX_CACHES);
	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_os_delete_cache
 *
 * PARAMETERS:  cache           - Handle to cache object
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Free all objects within the requested cache and delete the
 *              cache object.
 *
 ******************************************************************************/

acpi_status acpi_os_delete_cache(struct acpi_memory_list *cache)
{
	acpi_status status;

	ACPI_FUNCTION_ENTRY();

	/* Purge all objects in the cache */

	status = acpi_os_purge_cache(cache);
	if (ACPI_FAILURE(status)) {
		return (status);
	}

	/* Now we can delete the cache object */

	acpi_os_free(cache);
	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_os_release_object
 *
 * PARAMETERS:  cache       - Handle to cache object
 *              object      - The object to be released
 *
 * RETURN:      None
 *
 * DESCRIPTION: Release an object to the specified cache. If cache is full,
 *              the object is deleted.
 *
 ******************************************************************************/

acpi_status acpi_os_release_object(struct acpi_memory_list *cache, void *object)
{
	acpi_status status;

	ACPI_FUNCTION_ENTRY();

	if (!cache || !object) {
		return (AE_BAD_PARAMETER);
	}

	/* If cache is full, just free this object */

	if (cache->current_depth >= cache->max_depth) {
		ACPI_FREE(object);
		ACPI_MEM_TRACKING(cache->total_freed++);
	}

	/* Otherwise put this object back into the cache */

	else {
		status = acpi_ut_acquire_mutex(ACPI_MTX_CACHES);
		if (ACPI_FAILURE(status)) {
			return (status);
		}

		/* Mark the object as cached */

		memset(object, 0xCA, cache->object_size);
		ACPI_SET_DESCRIPTOR_TYPE(object, ACPI_DESC_TYPE_CACHED);

		/* Put the object at the head of the cache list */

		ACPI_SET_DESCRIPTOR_PTR(object, cache->list_head);
		cache->list_head = object;
		cache->current_depth++;

		(void)acpi_ut_release_mutex(ACPI_MTX_CACHES);
	}

	return (AE_OK);
}

/*******************************************************************************
 *
 * FUNCTION:    acpi_os_acquire_object
 *
 * PARAMETERS:  cache           - Handle to cache object
 *
 * RETURN:      the acquired object. NULL on error
 *
 * DESCRIPTION: Get an object from the specified cache. If cache is empty,
 *              the object is allocated.
 *
 ******************************************************************************/

void *acpi_os_acquire_object(struct acpi_memory_list *cache)
{
	acpi_status status;
	void *object;

	ACPI_FUNCTION_TRACE(os_acquire_object);

	if (!cache) {
		return_PTR(NULL);
	}

	status = acpi_ut_acquire_mutex(ACPI_MTX_CACHES);
	if (ACPI_FAILURE(status)) {
		return_PTR(NULL);
	}

	ACPI_MEM_TRACKING(cache->requests++);

	/* Check the cache first */

	if (cache->list_head) {

		/* There is an object available, use it */

		object = cache->list_head;
		cache->list_head = ACPI_GET_DESCRIPTOR_PTR(object);

		cache->current_depth--;

		ACPI_MEM_TRACKING(cache->hits++);
		ACPI_DEBUG_PRINT_RAW((ACPI_DB_EXEC,
				      "%s: Object %p from %s cache\n",
				      ACPI_GET_FUNCTION_NAME, object,
				      cache->list_name));

		status = acpi_ut_release_mutex(ACPI_MTX_CACHES);
		if (ACPI_FAILURE(status)) {
			return_PTR(NULL);
		}

		/* Clear (zero) the previously used Object */

		memset(object, 0, cache->object_size);
	} else {
		/* The cache is empty, create a new object */

		ACPI_MEM_TRACKING(cache->total_allocated++);

#ifdef ACPI_DBG_TRACK_ALLOCATIONS
		if ((cache->total_allocated - cache->total_freed) >
		    cache->max_occupied) {
			cache->max_occupied =
			    cache->total_allocated - cache->total_freed;
		}
#endif

		/* Avoid deadlock with ACPI_ALLOCATE_ZEROED */

		status = acpi_ut_release_mutex(ACPI_MTX_CACHES);
		if (ACPI_FAILURE(status)) {
			return_PTR(NULL);
		}

		object = ACPI_ALLOCATE_ZEROED(cache->object_size);
		if (!object) {
			return_PTR(NULL);
		}
	}

	return_PTR(object);
}
#endif				/* ACPI_USE_LOCAL_CACHE */
