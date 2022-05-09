/*******************************************************************************
    Copyright (c) 2015-2019 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef __UVM_TRACKER_H__
#define __UVM_TRACKER_H__

#include "uvm_forward_decl.h"

#include "uvm_linux.h"
#include "nvtypes.h"
#include "nvstatus.h"

typedef struct
{
    // Tracked channel
    //
    // If NULL the entry is considered to be completed.
    uvm_channel_t *channel;

    // Tracked channel's tracking semaphore value
    NvU64 value;
} uvm_tracker_entry_t;

typedef struct
{
    union
    {
        // The default static storage can fit a single entry as that's likely
        // the most common use-case. If the tracker ever needs more space, a
        // dynamic allocation will be made as part of adding an entry and
        // dynamic_entries below will be used.
        uvm_tracker_entry_t static_entries[1];

        // Pointer to the array with dynamically allocated entries
        uvm_tracker_entry_t *dynamic_entries;
    };

    // Number of used entries in the tracker
    NvU32 size;

    // Max number of entries that the entries array can store currently
    NvU32 max_size;

} uvm_tracker_t;

// Static initializer for a tracker.
// Importantly max_size needs to be set to the size of the static_entries array
// so that uvm_tracker_get_entries() works correctly.
// Note that the extra braces are necessary to avoid missing braces warning all the way down to:
// (near initialization for tracker.<anonymous>.static_entries[0]) [-Wmissing-braces]
#define UVM_TRACKER_INIT() { { { { 0 } } }, 0, ARRAY_SIZE(((uvm_tracker_t *)0)->static_entries) }

// Initialize a tracker
// This is guaranteed not to allocate any memory.
static void uvm_tracker_init(uvm_tracker_t *tracker)
{
    *tracker = (uvm_tracker_t)UVM_TRACKER_INIT();
}

// Deinitialize a tracker
// This will free any dynamic entries from the tracker
void uvm_tracker_deinit(uvm_tracker_t *tracker);

// Overwrite a tracker using different tracker, which must be previously initialized.
// This may require allocating memory to fit entries in the tracker. On failure,
// dst is cleared and src is left unmodified.
NV_STATUS uvm_tracker_overwrite(uvm_tracker_t *dst, uvm_tracker_t *src);

// Initialize tracker from another tracker.
// This may require allocating memory to fit entries in the tracker.
// On failure, uvm_tracker_deinit(dst) is safe but not required.
NV_STATUS uvm_tracker_init_from(uvm_tracker_t *dst, uvm_tracker_t *src);

// Clear a tracker
// Remove all entries from tracker
//
// This won't change the max size of the tracker.
static void uvm_tracker_clear(uvm_tracker_t *tracker)
{
    tracker->size = 0;
}

// Reserve enough space so min_free_entries can be added to the tracker
// without requiring memory allocation.
NV_STATUS uvm_tracker_reserve(uvm_tracker_t *tracker, NvU32 min_free_entries);

// Add a push to the tracker
// The push needs be finished, i.e. uvm_push_end*() has been called on it.
// This may require allocating memory to fit a new entry in the tracker
NV_STATUS uvm_tracker_add_push(uvm_tracker_t *tracker, uvm_push_t *push);

// Add a uvm_tracker_entry_t to a tracker
// This may require allocating memory to fit a new entry in the tracker
NV_STATUS uvm_tracker_add_entry(uvm_tracker_t *tracker, uvm_tracker_entry_t *new_entry);

// Overwrite the tracker with a single entry
// This will never allocate memory as an empty tracker always has space for at
// least one entry and hence can never fail.
void uvm_tracker_overwrite_with_entry(uvm_tracker_t *tracker, uvm_tracker_entry_t *new_entry);

// Overwrite the tracker with an entry from a push
// This will never allocate memory as an empty tracker always has space for at
// least one entry and hence can never fail.
void uvm_tracker_overwrite_with_push(uvm_tracker_t *tracker, uvm_push_t *push);

// Add all entries from another tracker
// This may require allocating memory to fit a new entry in the tracker.
// On error no entries are added to destination tracker.
NV_STATUS uvm_tracker_add_tracker(uvm_tracker_t *dst, uvm_tracker_t *src);

// "Safe" versions of the above. If memory cannot be allocated to add the new
// entries, these functions stall until entries are free.
NV_STATUS uvm_tracker_overwrite_safe(uvm_tracker_t *dst, uvm_tracker_t *src);
NV_STATUS uvm_tracker_add_push_safe(uvm_tracker_t *tracker, uvm_push_t *push);
NV_STATUS uvm_tracker_add_entry_safe(uvm_tracker_t *tracker, uvm_tracker_entry_t *new_entry);
NV_STATUS uvm_tracker_add_tracker_safe(uvm_tracker_t *dst, uvm_tracker_t *src);

// Query whether all entries in the tracker are complete
//
// This won't change the max size of the tracker.
bool uvm_tracker_is_completed(uvm_tracker_t *tracker);

// Wait for all tracker entries to complete
// This can only fail if a fatal error is hit that uvm_tracker_check_errors()
// would return.
// Both, on success and failure, all the entries will be removed from the
// tracker. This is done even on failure as otherwise it could be impossible to
// remove some entries from the tracker and they would eventually become invalid
// after the channels they track are destroyed.
//
// This won't change the max size of the tracker.
NV_STATUS uvm_tracker_wait(uvm_tracker_t *tracker);

// Wait for all tracker entries for other GPUs to complete
//
// This can only fail if a fatal error is hit that uvm_tracker_check_errors()
// would return. On success, all the entries for GPUs other than the passed in
// GPU will be removed from the tracker. On failure, all entries will be removed
// from the tracker, same as for uvm_tracker_wait().
//
// This won't change the max size of the tracker.
NV_STATUS uvm_tracker_wait_for_other_gpus(uvm_tracker_t *tracker, uvm_gpu_t *gpu);

// Helper to wait for a tracker, then deinit it.
static NV_STATUS uvm_tracker_wait_deinit(uvm_tracker_t *tracker)
{
    NV_STATUS status = uvm_tracker_wait(tracker);
    uvm_tracker_deinit(tracker);
    return status;
}

// Wait for a single tracker entry
//
// Similarly to uvm_tracker_wait(), if a global error is hit, the tracker_entry
// will be set to an empty value that's considered complete (its channel set to NULL).
NV_STATUS uvm_tracker_wait_for_entry(uvm_tracker_entry_t *tracker_entry);

bool uvm_tracker_is_entry_completed(uvm_tracker_entry_t *tracker_entry);

// Check for a global error and errors on all the channels that are tracked by the tracker
//
// This won't change the max size of the tracker.
NV_STATUS uvm_tracker_check_errors(uvm_tracker_t *tracker);

// Check whether a tracker is complete and check for any errors
// This is a shortcut for uvm_tracker_is_completed() + uvm_tracker_check_errors().
// If the tracker is complete and there are no errors, NV_OK is returned.
// If the tracker is not complete and there are no errors, NV_WARN_MORE_PROCESSING_REQUIRED is returned.
// If there are any errors, the error is returned.
//
// This won't change the max size of the tracker.
//
// Warning: If you call this in a spin loop, you should call schedule()
// periodically to break deadlock between RM and UVM. See the comments in
// uvm_spin_loop.
NV_STATUS uvm_tracker_query(uvm_tracker_t *tracker);

// Query all entries for completion and remove the completed ones
//
// This won't change the max size of the tracker.
void uvm_tracker_remove_completed(uvm_tracker_t *tracker);

// Get the array of tracker entries
uvm_tracker_entry_t *uvm_tracker_get_entries(uvm_tracker_t *tracker);

static bool uvm_tracker_is_empty(uvm_tracker_t *tracker)
{
    return tracker->size == 0;
}

uvm_gpu_t *uvm_tracker_entry_gpu(uvm_tracker_entry_t *entry);

// Helper to iterate over all tracker entries
#define for_each_tracker_entry(entry, tracker)                          \
    for (entry = &uvm_tracker_get_entries(tracker)[0];                  \
         entry != &uvm_tracker_get_entries(tracker)[(tracker)->size];   \
         ++entry)

#endif // __UVM_TRACKER_H__
