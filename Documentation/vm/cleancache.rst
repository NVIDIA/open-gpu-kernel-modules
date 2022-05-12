.. _cleancache:

==========
Cleancache
==========

Motivation
==========

Cleancache is a new optional feature provided by the VFS layer that
potentially dramatically increases page cache effectiveness for
many workloads in many environments at a negligible cost.

Cleancache can be thought of as a page-granularity victim cache for clean
pages that the kernel's pageframe replacement algorithm (PFRA) would like
to keep around, but can't since there isn't enough memory.  So when the
PFRA "evicts" a page, it first attempts to use cleancache code to
put the data contained in that page into "transcendent memory", memory
that is not directly accessible or addressable by the kernel and is
of unknown and possibly time-varying size.

Later, when a cleancache-enabled filesystem wishes to access a page
in a file on disk, it first checks cleancache to see if it already
contains it; if it does, the page of data is copied into the kernel
and a disk access is avoided.

Transcendent memory "drivers" for cleancache are currently implemented
in Xen (using hypervisor memory) and zcache (using in-kernel compressed
memory) and other implementations are in development.

:ref:`FAQs <faq>` are included below.

Implementation Overview
=======================

A cleancache "backend" that provides transcendent memory registers itself
to the kernel's cleancache "frontend" by calling cleancache_register_ops,
passing a pointer to a cleancache_ops structure with funcs set appropriately.
The functions provided must conform to certain semantics as follows:

Most important, cleancache is "ephemeral".  Pages which are copied into
cleancache have an indefinite lifetime which is completely unknowable
by the kernel and so may or may not still be in cleancache at any later time.
Thus, as its name implies, cleancache is not suitable for dirty pages.
Cleancache has complete discretion over what pages to preserve and what
pages to discard and when.

Mounting a cleancache-enabled filesystem should call "init_fs" to obtain a
pool id which, if positive, must be saved in the filesystem's superblock;
a negative return value indicates failure.  A "put_page" will copy a
(presumably about-to-be-evicted) page into cleancache and associate it with
the pool id, a file key, and a page index into the file.  (The combination
of a pool id, a file key, and an index is sometimes called a "handle".)
A "get_page" will copy the page, if found, from cleancache into kernel memory.
An "invalidate_page" will ensure the page no longer is present in cleancache;
an "invalidate_inode" will invalidate all pages associated with the specified
file; and, when a filesystem is unmounted, an "invalidate_fs" will invalidate
all pages in all files specified by the given pool id and also surrender
the pool id.

An "init_shared_fs", like init_fs, obtains a pool id but tells cleancache
to treat the pool as shared using a 128-bit UUID as a key.  On systems
that may run multiple kernels (such as hard partitioned or virtualized
systems) that may share a clustered filesystem, and where cleancache
may be shared among those kernels, calls to init_shared_fs that specify the
same UUID will receive the same pool id, thus allowing the pages to
be shared.  Note that any security requirements must be imposed outside
of the kernel (e.g. by "tools" that control cleancache).  Or a
cleancache implementation can simply disable shared_init by always
returning a negative value.

If a get_page is successful on a non-shared pool, the page is invalidated
(thus making cleancache an "exclusive" cache).  On a shared pool, the page
is NOT invalidated on a successful get_page so that it remains accessible to
other sharers.  The kernel is responsible for ensuring coherency between
cleancache (shared or not), the page cache, and the filesystem, using
cleancache invalidate operations as required.

Note that cleancache must enforce put-put-get coherency and get-get
coherency.  For the former, if two puts are made to the same handle but
with different data, say AAA by the first put and BBB by the second, a
subsequent get can never return the stale data (AAA).  For get-get coherency,
if a get for a given handle fails, subsequent gets for that handle will
never succeed unless preceded by a successful put with that handle.

Last, cleancache provides no SMP serialization guarantees; if two
different Linux threads are simultaneously putting and invalidating a page
with the same handle, the results are indeterminate.  Callers must
lock the page to ensure serial behavior.

Cleancache Performance Metrics
==============================

If properly configured, monitoring of cleancache is done via debugfs in
the `/sys/kernel/debug/cleancache` directory.  The effectiveness of cleancache
can be measured (across all filesystems) with:

``succ_gets``
	number of gets that were successful

``failed_gets``
	number of gets that failed

``puts``
	number of puts attempted (all "succeed")

``invalidates``
	number of invalidates attempted

A backend implementation may provide additional metrics.

.. _faq:

FAQ
===

* Where's the value? (Andrew Morton)

Cleancache provides a significant performance benefit to many workloads
in many environments with negligible overhead by improving the
effectiveness of the pagecache.  Clean pagecache pages are
saved in transcendent memory (RAM that is otherwise not directly
addressable to the kernel); fetching those pages later avoids "refaults"
and thus disk reads.

Cleancache (and its sister code "frontswap") provide interfaces for
this transcendent memory (aka "tmem"), which conceptually lies between
fast kernel-directly-addressable RAM and slower DMA/asynchronous devices.
Disallowing direct kernel or userland reads/writes to tmem
is ideal when data is transformed to a different form and size (such
as with compression) or secretly moved (as might be useful for write-
balancing for some RAM-like devices).  Evicted page-cache pages (and
swap pages) are a great use for this kind of slower-than-RAM-but-much-
faster-than-disk transcendent memory, and the cleancache (and frontswap)
"page-object-oriented" specification provides a nice way to read and
write -- and indirectly "name" -- the pages.

In the virtual case, the whole point of virtualization is to statistically
multiplex physical resources across the varying demands of multiple
virtual machines.  This is really hard to do with RAM and efforts to
do it well with no kernel change have essentially failed (except in some
well-publicized special-case workloads).  Cleancache -- and frontswap --
with a fairly small impact on the kernel, provide a huge amount
of flexibility for more dynamic, flexible RAM multiplexing.
Specifically, the Xen Transcendent Memory backend allows otherwise
"fallow" hypervisor-owned RAM to not only be "time-shared" between multiple
virtual machines, but the pages can be compressed and deduplicated to
optimize RAM utilization.  And when guest OS's are induced to surrender
underutilized RAM (e.g. with "self-ballooning"), page cache pages
are the first to go, and cleancache allows those pages to be
saved and reclaimed if overall host system memory conditions allow.

And the identical interface used for cleancache can be used in
physical systems as well.  The zcache driver acts as a memory-hungry
device that stores pages of data in a compressed state.  And
the proposed "RAMster" driver shares RAM across multiple physical
systems.

* Why does cleancache have its sticky fingers so deep inside the
  filesystems and VFS? (Andrew Morton and Christoph Hellwig)

The core hooks for cleancache in VFS are in most cases a single line
and the minimum set are placed precisely where needed to maintain
coherency (via cleancache_invalidate operations) between cleancache,
the page cache, and disk.  All hooks compile into nothingness if
cleancache is config'ed off and turn into a function-pointer-
compare-to-NULL if config'ed on but no backend claims the ops
functions, or to a compare-struct-element-to-negative if a
backend claims the ops functions but a filesystem doesn't enable
cleancache.

Some filesystems are built entirely on top of VFS and the hooks
in VFS are sufficient, so don't require an "init_fs" hook; the
initial implementation of cleancache didn't provide this hook.
But for some filesystems (such as btrfs), the VFS hooks are
incomplete and one or more hooks in fs-specific code are required.
And for some other filesystems, such as tmpfs, cleancache may
be counterproductive.  So it seemed prudent to require a filesystem
to "opt in" to use cleancache, which requires adding a hook in
each filesystem.  Not all filesystems are supported by cleancache
only because they haven't been tested.  The existing set should
be sufficient to validate the concept, the opt-in approach means
that untested filesystems are not affected, and the hooks in the
existing filesystems should make it very easy to add more
filesystems in the future.

The total impact of the hooks to existing fs and mm files is only
about 40 lines added (not counting comments and blank lines).

* Why not make cleancache asynchronous and batched so it can more
  easily interface with real devices with DMA instead of copying each
  individual page? (Minchan Kim)

The one-page-at-a-time copy semantics simplifies the implementation
on both the frontend and backend and also allows the backend to
do fancy things on-the-fly like page compression and
page deduplication.  And since the data is "gone" (copied into/out
of the pageframe) before the cleancache get/put call returns,
a great deal of race conditions and potential coherency issues
are avoided.  While the interface seems odd for a "real device"
or for real kernel-addressable RAM, it makes perfect sense for
transcendent memory.

* Why is non-shared cleancache "exclusive"?  And where is the
  page "invalidated" after a "get"? (Minchan Kim)

The main reason is to free up space in transcendent memory and
to avoid unnecessary cleancache_invalidate calls.  If you want inclusive,
the page can be "put" immediately following the "get".  If
put-after-get for inclusive becomes common, the interface could
be easily extended to add a "get_no_invalidate" call.

The invalidate is done by the cleancache backend implementation.

* What's the performance impact?

Performance analysis has been presented at OLS'09 and LCA'10.
Briefly, performance gains can be significant on most workloads,
especially when memory pressure is high (e.g. when RAM is
overcommitted in a virtual workload); and because the hooks are
invoked primarily in place of or in addition to a disk read/write,
overhead is negligible even in worst case workloads.  Basically
cleancache replaces I/O with memory-copy-CPU-overhead; on older
single-core systems with slow memory-copy speeds, cleancache
has little value, but in newer multicore machines, especially
consolidated/virtualized machines, it has great value.

* How do I add cleancache support for filesystem X? (Boaz Harrash)

Filesystems that are well-behaved and conform to certain
restrictions can utilize cleancache simply by making a call to
cleancache_init_fs at mount time.  Unusual, misbehaving, or
poorly layered filesystems must either add additional hooks
and/or undergo extensive additional testing... or should just
not enable the optional cleancache.

Some points for a filesystem to consider:

  - The FS should be block-device-based (e.g. a ram-based FS such
    as tmpfs should not enable cleancache)
  - To ensure coherency/correctness, the FS must ensure that all
    file removal or truncation operations either go through VFS or
    add hooks to do the equivalent cleancache "invalidate" operations
  - To ensure coherency/correctness, either inode numbers must
    be unique across the lifetime of the on-disk file OR the
    FS must provide an "encode_fh" function.
  - The FS must call the VFS superblock alloc and deactivate routines
    or add hooks to do the equivalent cleancache calls done there.
  - To maximize performance, all pages fetched from the FS should
    go through the do_mpag_readpage routine or the FS should add
    hooks to do the equivalent (cf. btrfs)
  - Currently, the FS blocksize must be the same as PAGESIZE.  This
    is not an architectural restriction, but no backends currently
    support anything different.
  - A clustered FS should invoke the "shared_init_fs" cleancache
    hook to get best performance for some backends.

* Why not use the KVA of the inode as the key? (Christoph Hellwig)

If cleancache would use the inode virtual address instead of
inode/filehandle, the pool id could be eliminated.  But, this
won't work because cleancache retains pagecache data pages
persistently even when the inode has been pruned from the
inode unused list, and only invalidates the data page if the file
gets removed/truncated.  So if cleancache used the inode kva,
there would be potential coherency issues if/when the inode
kva is reused for a different file.  Alternately, if cleancache
invalidated the pages when the inode kva was freed, much of the value
of cleancache would be lost because the cache of pages in cleanache
is potentially much larger than the kernel pagecache and is most
useful if the pages survive inode cache removal.

* Why is a global variable required?

The cleancache_enabled flag is checked in all of the frequently-used
cleancache hooks.  The alternative is a function call to check a static
variable. Since cleancache is enabled dynamically at runtime, systems
that don't enable cleancache would suffer thousands (possibly
tens-of-thousands) of unnecessary function calls per second.  So the
global variable allows cleancache to be enabled by default at compile
time, but have insignificant performance impact when cleancache remains
disabled at runtime.

* Does cleanache work with KVM?

The memory model of KVM is sufficiently different that a cleancache
backend may have less value for KVM.  This remains to be tested,
especially in an overcommitted system.

* Does cleancache work in userspace?  It sounds useful for
  memory hungry caches like web browsers.  (Jamie Lokier)

No plans yet, though we agree it sounds useful, at least for
apps that bypass the page cache (e.g. O_DIRECT).

Last updated: Dan Magenheimer, April 13 2011
