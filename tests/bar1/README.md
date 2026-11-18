# BAR1 mapping regressions

This patch addresses the invalid mappings exposed by small BAR1 apertures in
[issue #1396](https://github.com/NVIDIA/open-gpu-kernel-modules/issues/1396)
and the [Wayland forum report](https://forums.developer.nvidia.com/t/bug-report-wayland-only-dmaallocmapping-gm107-va-mapping-failures-does-not-reproduce-on-x11/353598/13).
It does not require enabling Resizable BAR.

## Root cause

`memMap_IMPL()` used `!bKernel` as the condition for allowing discontiguous BAR1
mappings. However, a kernel RM client such as NVKMS uses `MEM_SPACE_USER` to
request a physical address. `rmapiValidateKernelMapping()` sets `bKernel` to
false for this request too.

When the first contiguous allocation fails, `kbusMapFbAperture_GM107()` can
retry with multiple smaller BAR1 ranges. `osMapPciMemoryAreaUser()` returns the
first address and puts the full range list in a thread-local mapping context.
The ordinary userspace mmap path consumes this list. `rm_kernel_rmapi_op()`
instead frees it, returning only the first address to NVKMS.

DRM then uses `ioremap_wc(first_address, object_size)` and handles user faults
as `first_pfn + page_offset`. Neither operation can represent the remaining
ranges. Addresses beyond the first range can refer to another allocation or
fall outside BAR1. The forum's resource-boundary warning is consistent with
this path. A larger BAR1 reduces the chance of reaching the fragmented fallback
but does not repair the API contract.

The patch keeps scatter mappings for ordinary userspace RM clients and requires
one contiguous BAR1 range for kernel clients returning a single address.
Exhaustion returns an error before DRM receives a physical mapping.

The reuse database also incremented an existing mapping's reference count
before allocating the returned range array. If that allocation failed, there
was no successful map for the caller to unmap, leaving the aperture pinned.
The patch commits the reference only after allocation succeeds, initializes
failure outputs, checks a newly allocated context before initializing it, and
removes empty context maps on failure and final unmap. Untracked mappings no
longer allocate unused context maps. Expected mapping failures use checks
instead of assertions in the two reuse call sites.

## Automated tests

Run from the repository root:

```sh
python3 tests/bar1/run.py
```

Requires Python 3 and a C compiler with undefined-behavior sanitizer support.
The sanitizer traps directly and needs no runtime library. `CC` may select a
different compiler. No GPU, loaded NVIDIA driver, or root privileges are needed.

The tests compile the real `mapping_reuse.c`, real map container, and the
production mapping-validation and BAR1 flag-selection code. Only memory
allocation and GPU map/unmap callbacks are mocked. The runner extracts the
flag-selection block because compiling all of `memMap_IMPL()` requires the
rest of RM; source-boundary changes intentionally fail the test extraction.

Coverage includes:

- Ordinary user, kernel virtual, and NVKMS physical mapping contracts.
- Fragmented aperture rejection for a single-address caller and recovery on
  the next smaller mapping.
- Failure at each allocation of a new tracked mapping and a fragmented map.
- Failed reuse followed by successful reuse and final physical unmap.
- Rollback after a GPU callback fails partway through a fragmented mapping.
- Overlapping mappings, preservation of live mappings after a failed request,
  and repeated allocation-context reuse without growing the metadata maps.

Validation performed on the 615.71.09 checkout:

- The regression suite passes with undefined-behavior instrumentation.
- Running the contract test against the original `mapping_cpu.c` fails; running
  the reuse test against the original `mapping_reuse.c` confirms that the last
  successful unmap does not release the physical mapping after an injected
  result-allocation failure.
- Full module builds completed against Fedora kernel headers
  `7.2.5-200.fc44.x86_64` and `7.2.7-200.fc44.x86_64`. The latter emits objtool
  warnings and UVM format warnings outside the changed code. BTF generation was
  skipped because `vmlinux` is unavailable. Modules were not installed or loaded.

## Hardware validation still required

These tests prove the mapping contract and failure bookkeeping, not that the
reported Xid 8 has no additional cause. A device with ReBAR disabled and matching
615.71.09 firmware/userspace is required to validate the original browser and
compositor workloads.

1. Record BAR1 total/used/free with `nvidia-smi -q -d MEMORY` and confirm the
   small aperture. A firmware ReBAR setting alone does not establish its size.
2. In a Wayland session, repeatedly open the URLs from the linked reports,
   including simultaneous video playback. Monitor the kernel log and BAR1 use.
3. Verify that there are no resource-boundary warnings or accesses to unrelated
   surfaces, and that the desktop remains responsive if a mapping is rejected.
4. Close the workload and check that BAR1 usage returns toward its baseline.
5. Repeat with X11 and with ReBAR enabled to check the unaffected paths.

A 256 MiB aperture can still run out of contiguous space. This patch deliberately
returns allocation failure in that case; an application that aborts on `ENOMEM`
may still abort. Supporting more simultaneously CPU-mapped video memory requires
an API that carries all mapping ranges, or a separate eviction/migration design.
The patch does not claim to implement either feature.
