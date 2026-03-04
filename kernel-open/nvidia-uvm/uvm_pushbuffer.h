/*******************************************************************************
    Copyright (c) 2015-2023 NVIDIA Corporation

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

#ifndef __UVM_PUSHBUFFER_H__
#define __UVM_PUSHBUFFER_H__

#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_linux.h"
#include "nvtypes.h"

//
// UVM pushbuffer
//
// The UVM pushbuffer is a memory allocator specialized for managing allocations
// used as the backing store for methods sent to the GPU (pushes, abstracted by
// uvm_push_t). Each pushbuffer is usable only with a specific channel manager
// and hence a specific GPU as that allows for greater flexibility down the road
// (e.g. moving the pushbuffer allocation to GPU memory in some cases).
//
// The usage of the pushbuffer always follows the same pattern:
//  1) The CPU requests a new allocation to do a push in. The allocation is
//     always initially of UVM_MAX_PUSH_SIZE and its usage is tracked by the
//     UVM push abstraction (uvm_push_t).
//  2) The CPU writes some GPU methods
//  3) The CPU finishes and reports how much of the UVM_MAX_PUSH_SIZE space was used
//  4) The methods are queued to be read by the GPU (by referencing them in a GPFIFO entry)
//  5) At some later time the CPU notices the methods have been completed and
//     reports that the allocation can now be reused.
//
// Notably 1) could happen concurrently from multiple CPU threads and in 4) the
// GPU has multiple independent queues of execution (UVM channels).
//
// With the above in mind, we can go through the implementation details of the
// current solution.
// The pushbuffer backing store is a single big allocation logically divided
// into largely independent parts called chunks.
// Each chunk is roughly a ringbuffer tracking multiple pending pushes being
// processed by the GPU. The pushbuffer maintains two bitmaps, one tracking
// completely idle (with no pending pushes) chunks and a second one tracking
// available (with pending pushes, but still enough space for a new push)
// chunks. When a new allocation is requested, idle chunks are always used
// first and after that available chunks are consulted. If none are available,
// the CPU spin waits on the GPU to complete some of the pending pushes making
// space for a new one.
//
// To explain how chunks track pending pushes we will go through an example
// modifying a chunk's state. Let's start with a few pending pushes in the
// chunk:
//
// [   [push P1][push P2][free unusable space][push P3][push P4]        ]
//     ^ gpu_get                                               ^ cpu_put
//
// The beginning of the first pending push is called the GPU get and the end of the
// last push is called the CPU put. This follows the HW GPFIFO naming that's a true
// ringbuffer (always completing in order and supporting transparent
// wrap-around). All the memory between gpu_get and cpu_put is considered
// unusable. Pushes within a chunk can finish out of order as each chunk can
// service pushes from multiple channels. And hence there can be some space
// between the first and last push that's already free, but unusable. The space
// after cpu_put and before gpu_get is available to be allocated to a new push,
// and if that happens the chunk above could change to:
//
// [   [push P1][push P2][free unusable space][push P3][push P4][push P5]       ]
//     ^ gpu_get                                                        ^ cpu_put
//
// Then, say push P2 completes:
//
// [   [push P1][free unusable space         ][push P3][push P4][push P5]       ]
//     ^ gpu_get                                                        ^ cpu_put
//
// We can see that P2 completing only expands the unusable free space, but if P1
// finishes we get:
//
// [                                          [push P3][push P4][push P5]       ]
//                                            ^ gpu_get                 ^ cpu_put
//
// This shows that some cases cause waste, but on the other hand allow for
// tracking of pending pushes and free space to be trivial. Each pending push of
// a chunk is in a doubly linked list with its head in the chunk. Each new push
// is added at the tail and when a push completes it's removed from the list.
// The gpu_get and/or cpu_put only change when the last/first push in the
// list finishes or a new push is added. The pending pushes are represented by
// the software state tracking GPFIFO entries (uvm_gpfifo_entry_t in
// uvm_channel.h) that are all allocated at channel creation (a HW channel has
// a fixed limit of GPFIFO entries it supports that's chosen at channel creation
// and we allocate all the SW state for them at channel creation as well). This
// allows all the operations of the pushbuffer to be free of any memory
// allocation.
//
// To illustrate that a chunk is only roughly a ringbuffer, let's see what
// happens when another push is added to the chunk above, but there is not
// enough space between cpu_put and the end of the chunk to fit UVM_MAX_PUSH_SIZE:
// [[push P6][free space                     ][push P3][push P4][push P5]       ]
//          ^ cpu_put                         ^ gpu_get
//
// The GPU reading the pushbuffer expects it to be in a consecutive VA and hence
// the pending pushes cannot wrap around in the chunk leading to some potential
// waste at the end.
//
// The pushbuffer implementation is configurable through a few defines below,
// but careful tweaking of them is yet to be done.
//

// TODO: Bug 1764958: Calculate/measure the maximum push size and tweak the
// number of chunks and size of each after benchmarks.
//
// Below are the measurements borrowed from uvm_channel_mgmt.h. They will need
// to be adjusted and verified once all the operations are implemented in this
// driver, but for now we will set the MAX_PUSH to 128K as that seems pretty
// safe.
//
// A pushbuffer needs to accomodate all possible operations on a 2 Mb Va region
// per gpu. The longest sequence of operations would be:
// Acquire 3 + 32 trackers:
// replay tracker, instancePtr tracker, 2Mb descriptor tracker and 32 trackers
// one each for 64Kb of phys mem.
// Each tracker can have ~64 tracker items  (35 x 64 x 20 bytes acquire  = 45k)
// Unmap 4k ptes for 2Mb va                 (Inline pte data + header    = ~4k)
// Invalidate for every 4k                  (512 * 20 bytes              = 10k)
// Migrate data worth 2 Mb                  (512 * 48 bytes to do copy   = 24k)
// Map 4k ptes for 2Mb va                   (4k inline pte data + header = ~4k)
// Invalidate for every 4k                  (512 * 20 bytes              = 10k)
// Total                                                            Total= ~100k
//
#define UVM_MAX_PUSH_SIZE (128 * 1024)
#define UVM_PUSHBUFFER_CHUNK_SIZE (8 * UVM_MAX_PUSH_SIZE)
#define UVM_PUSHBUFFER_CHUNKS 16

// Total size of the pushbuffer
#define UVM_PUSHBUFFER_SIZE (UVM_PUSHBUFFER_CHUNK_SIZE * UVM_PUSHBUFFER_CHUNKS)

// The max number of concurrent pushes that can be happening at the same time.
// Concurrent pushes are ones that are after uvm_push_begin*(), but before
// uvm_push_end().
#define UVM_PUSH_MAX_CONCURRENT_PUSHES UVM_PUSHBUFFER_CHUNKS

// Push space needed for static part for the WLC schedule, as initialized in
// 'setup_wlc_schedule':
// * CE decrypt (of WLC PB): 56B
// * WFI:                     8B
// Total:                    64B
//
// Push space needed for secure work launch is 364B. The push is constructed
// in 'internal_channel_submit_work_indirect' and 'uvm_channel_end_push'
// * CE decrypt (of indirect PB):                   56B
// * memset_8 (indirect GPFIFO entry):              44B
// * semaphore release (indirect GPPUT):            24B
// * semaphore release (indirect doorbell):         24B
// Appendix added in 'uvm_channel_end_push':
// * semaphore release (WLC tracking):             168B
//      * semaphore release (payload):              24B
//      * notifier memset:                          40B
//      * payload encryption:                       64B
//      * notifier memset:                          40B
// * semaphore increment (LCIC GPPUT):              24B
// * semaphore release (LCIC doorbell):             24B
// Total:                                          364B
#define UVM_MAX_WLC_PUSH_SIZE (364)

// Push space needed for static LCIC schedule, as initialized in
// 'setup_lcic_schedule':
// * WFI:                                   8B
// * semaphore increment (WLC GPPUT):      24B
// * semaphore increment (WLC GPPUT):      24B
// * semaphore increment (LCIC tracking): 160B
//      * semaphore increment (payload):   24B
//      * notifier memcopy:                36B
//      * payload encryption:              64B
//      * notifier memcopy:                36B
// Total:                                 216B
#define UVM_LCIC_PUSH_SIZE (216)

typedef struct
{
    // Offset within the chunk of where a next push should begin if there is
    // space for one. Updated in update_chunk().
    NvU32 next_push_start;

    // List of uvm_gpfifo_entry_t that are pending and used this chunk. New
    // entries are always added at the tail of the list.
    struct list_head pending_gpfifos;

    // Currently on-going push in the chunk. There can be only one at a time.
    uvm_push_t *current_push;
} uvm_pushbuffer_chunk_t;

struct uvm_pushbuffer_struct
{
    uvm_channel_manager_t *channel_manager;

    // Memory allocation backing the pushbuffer
    uvm_rm_mem_t *memory;

    // Mirror image of memory in dma sysmem
    uvm_rm_mem_t *memory_unprotected_sysmem;

    // Secure sysmem backing memory
    void *memory_protected_sysmem;

    // Array of the pushbuffer chunks
    uvm_pushbuffer_chunk_t chunks[UVM_PUSHBUFFER_CHUNKS];

    // Chunks that do not have an on-going push and have at least
    // UVM_MAX_PUSH_SIZE space free.
    DECLARE_BITMAP(available_chunks, UVM_PUSHBUFFER_CHUNKS);

    // Chunks that do not have an on-going push nor any pending pushes.
    DECLARE_BITMAP(idle_chunks, UVM_PUSHBUFFER_CHUNKS);

    // Lock protecting chunk state and the bitmaps.
    uvm_spinlock_t lock;

    // Semaphore enforcing a limited number of concurrent pushes.
    // Decremented in uvm_pushbuffer_begin_push(), incremented in
    // uvm_pushbuffer_end_push().
    // Initialized to the number of chunks as that's how many concurrent pushes
    // are supported.
    uvm_semaphore_t concurrent_pushes_sema;

    struct
    {
        struct proc_dir_entry *info_file;
    } procfs;
};

// Create a pushbuffer
NV_STATUS uvm_pushbuffer_create(uvm_channel_manager_t *channel_manager, uvm_pushbuffer_t **pushbuffer_out);

// Destroy the pushbuffer
void uvm_pushbuffer_destroy(uvm_pushbuffer_t *pushbuffer);

// Get an allocation for a push from the pushbuffer
// Waits until a chunk is available and claims it for the push. The chunk used
// for the push will be unavailable for any new pushes until
// uvm_pushbuffer_end_push() for the push is called.
NV_STATUS uvm_pushbuffer_begin_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push);

// Complete a pending push
// Updates the chunk state the pending push used
void uvm_pushbuffer_mark_completed(uvm_channel_t *channel, uvm_gpfifo_entry_t *gpfifo);

// Get the GPU VA for an ongoing push
NvU64 uvm_pushbuffer_get_gpu_va_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push);

// Get the CPU VA for encrypted sysmem mirror
void *uvm_pushbuffer_get_unprotected_cpu_va_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push);

// Get the GPU VA for encrypted sysmem mirror
NvU64 uvm_pushbuffer_get_unprotected_gpu_va_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push);

// Get the offset of the beginning of the push from the base of the pushbuffer allocation
NvU32 uvm_pushbuffer_get_offset_for_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push);

// End an on-going push
// Updates the state of the chunk making it available for new pushes if it has
// enough space left.
void uvm_pushbuffer_end_push(uvm_pushbuffer_t *pushbuffer, uvm_push_t *push, uvm_gpfifo_entry_t *gpfifo);

// Query whether the pushbuffer has space for another push
// Mostly useful in pushbuffer tests
bool uvm_pushbuffer_has_space(uvm_pushbuffer_t *pushbuffer);

// Helper to print pushbuffer state for debugging
void uvm_pushbuffer_print(uvm_pushbuffer_t *pushbuffer);

// Helper to retrieve the pushbuffer->memory GPU VA.
NvU64 uvm_pushbuffer_get_gpu_va_base(uvm_pushbuffer_t *pushbuffer);

// SEC2 variant to retrieve GPU VA for push location.
// Unlike other channels, SEC2 uses signed pushes in unprotected sysmem.
NvU64 uvm_pushbuffer_get_sec2_gpu_va_base(uvm_pushbuffer_t *pushbuffer);

#endif // __UVM_PUSHBUFFER_H__
