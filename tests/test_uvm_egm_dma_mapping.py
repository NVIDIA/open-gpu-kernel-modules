# SPDX-License-Identifier: MIT
"""Host regression checks for EGM DMA mapping and its caller.

Run with: python3 -m unittest discover -s tests -p 'test_uvm_egm_dma_mapping.py' -v
Requires a C compiler. The actual driver functions are compiled with platform
stubs; these checks do not replace a kernel build or GPU hardware tests.
UVM_TEST_SOURCE_ROOT can select another source checkout for before/after tests.
"""

import ctypes
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest


ROOT = Path(os.environ.get("UVM_TEST_SOURCE_ROOT", Path(__file__).resolve().parents[1]))
SOURCE = ROOT / "kernel-open" / "nvidia-uvm"
NV_OK = 0
NV_ERR_NOT_SUPPORTED = 1
PAGE_SIZE = 4096


def driver_function(filename, name):
    source = (SOURCE / filename).read_text()
    start = source.index("NV_STATUS " + name + "(")
    opening = source.index("{", start)
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[start:index + 1]
    raise ValueError("Unterminated function: " + name)


PLATFORM = r"""
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint64_t NvU64;
typedef uint64_t dma_addr_t;
typedef uint64_t phys_addr_t;
typedef int NV_STATUS;
#define NV_OK 0
#define NV_ERR_NOT_SUPPORTED 1
#define UVM_ASSERT assert
#define PAGE_SHIFT 12
#define PAGE_ALIGNED(size) (((size) & 4095) == 0)
#define DMA_BIDIRECTIONAL 3
#define DMA_ADDR_INVALID UINT64_MAX
#define UVM_USE_DMA_IOVA_API() TEST_DMA_IOVA_API

struct device { int unused; };
struct pci_dev { struct device dev; };
struct page { phys_addr_t address; };
typedef struct {
    struct pci_dev *pci_dev;
    int closest_cpu_numa_node;
    int id;
    bool enabled, legacy, identity;
    int64_t mapped_cpu_pages_size;
} uvm_parent_gpu_t;
typedef struct { int lock; int egm_parent_id; } uvm_cpu_physical_chunk_t;
typedef struct uvm_cpu_chunk {
    struct page *page;
    dma_addr_t egm_dma_addr;
    uvm_cpu_physical_chunk_t *physical;
} uvm_cpu_chunk_t;
typedef struct { uvm_cpu_chunk_t *parent; } uvm_cpu_logical_chunk_t;

#define page_to_nid(page) 1
#define node_start_pfn(node) 0x100UL
#define page_to_phys(page) ((page)->address)
#define uvm_parent_gpu_egm_enabled(gpu) ((gpu)->enabled)
#define uvm_parent_gpu_egm_is_legacy(gpu) ((gpu)->legacy)
#define uvm_parent_gpu_egm_iommu_is_identity(gpu) ((gpu)->identity)
#define uvm_parent_gpu_egm_iova_address(gpu) 0x800000ULL
#define uvm_parent_gpu_egm_window_size(gpu) 0x400000ULL
#define get_physical_parent(chunk) ((chunk)->physical)
#define uvm_cpu_chunk_is_logical(chunk) false
#define uvm_cpu_chunk_to_logical(chunk) ((uvm_cpu_logical_chunk_t *)0)
#define uvm_cpu_chunk_get_size(chunk) 4096
#define uvm_mutex_lock(lock) assert(++*(lock) == 1)
#define uvm_mutex_unlock(lock) assert(--*(lock) == 0)

static int link_error, sync_error, links, syncs, unlinks;
#if TEST_DMA_IOVA_API
struct dma_iova_state { dma_addr_t addr; size_t __size; };
static NV_STATUS errno_to_nv_status(int error) { return 1000 - error; }
static void atomic64_add(size_t size, int64_t *value) { *value += size; }
static void check_window(struct device *dev, struct dma_iova_state *state,
                         NvU64 offset, size_t size)
{
    assert(dev != NULL);
    assert(state->addr == 0x800000 && state->__size == 0x400000);
    assert(offset + size <= state->__size && size == 4096);
}
static int dma_iova_link(struct device *dev, struct dma_iova_state *state,
                         phys_addr_t address, NvU64 offset, size_t size,
                         int direction, unsigned long attributes)
{
    check_window(dev, state, offset, size);
    assert(address == 0x100000 + offset);
    assert(direction == DMA_BIDIRECTIONAL && attributes == 0);
    ++links;
    return link_error;
}
static int dma_iova_sync(struct device *dev, struct dma_iova_state *state,
                         NvU64 offset, size_t size)
{
    check_window(dev, state, offset, size);
    ++syncs;
    return sync_error;
}
static void dma_iova_unlink(struct device *dev, struct dma_iova_state *state,
                            NvU64 offset, size_t size, int direction,
                            unsigned long attributes)
{
    check_window(dev, state, offset, size);
    assert(direction == DMA_BIDIRECTIONAL && attributes == 0);
    ++unlinks;
}
#endif
"""

ENTRY = r"""
void exercise(int enabled, int legacy, int identity, uint64_t offset,
              int link_failure, int sync_failure, int use_caller,
              int64_t result[8])
{
    struct pci_dev pci = {0};
    struct page page = {.address = 0x100000 + offset};
    uvm_parent_gpu_t gpu = {.pci_dev = &pci, .closest_cpu_numa_node = 1,
                           .id = 7, .enabled = enabled, .legacy = legacy,
                           .identity = identity};
    uvm_cpu_physical_chunk_t physical = {.egm_parent_id = -1};
    uvm_cpu_chunk_t chunk = {.page = &page, .egm_dma_addr = DMA_ADDR_INVALID,
                            .physical = &physical};
    NvU64 address = DMA_ADDR_INVALID;
    NV_STATUS status;

    link_error = link_failure;
    sync_error = sync_failure;
    links = syncs = unlinks = 0;
    if (use_caller) {
        status = uvm_cpu_chunk_map_gpu_egm(&chunk, &gpu);
        address = chunk.egm_dma_addr;
    }
    else {
        status = uvm_gpu_map_cpu_pages_for_egm(&gpu, &page, 4096, &address);
    }
    result[0] = status;
    result[1] = address == DMA_ADDR_INVALID ? -1 : (int64_t)address;
    result[2] = gpu.mapped_cpu_pages_size;
    result[3] = links;
    result[4] = syncs;
    result[5] = unlinks;
    result[6] = physical.egm_parent_id;
    result[7] = physical.lock;
}
"""


class EgmDmaMappingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        temporary = tempfile.TemporaryDirectory(prefix="uvm-egm-test-")
        cls.addClassCleanup(temporary.cleanup)
        directory = Path(temporary.name)
        code = "\n".join((
            PLATFORM,
            driver_function("uvm_gpu.c", "uvm_gpu_map_cpu_pages_for_egm"),
            driver_function("uvm_pmm_sysmem.c", "uvm_cpu_chunk_map_gpu_egm"),
            ENTRY,
        ))
        compiler = shlex.split(os.environ.get("CC", "cc"))
        cls.libraries = {}
        for api in (0, 1):
            library = directory / ("egm_%d.so" % api)
            # Permit the original unused status variable so the regression
            # fails on the observed return value, rather than a build warning.
            command = compiler + ["-x", "c", "-std=gnu11", "-O2", "-shared", "-fPIC",
                                  "-Wall", "-Wextra", "-Werror",
                                  "-Wno-unused-but-set-variable",
                                  "-DTEST_DMA_IOVA_API=%d" % api,
                                  "-", "-o", str(library)]
            result = subprocess.run(command, input=code, text=True, capture_output=True)
            if result.returncode:
                raise RuntimeError(result.stderr)
            loaded = ctypes.CDLL(str(library))
            loaded.exercise.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int,
                                        ctypes.c_uint64, ctypes.c_int, ctypes.c_int,
                                        ctypes.c_int, ctypes.POINTER(ctypes.c_int64)]
            loaded.exercise.restype = None
            cls.libraries[api] = loaded

    def run_mapping(self, api=1, enabled=1, legacy=0, identity=0,
                    offset=8192, link_error=0, sync_error=0, caller=0):
        result = (ctypes.c_int64 * 8)()
        self.libraries[api].exercise(enabled, legacy, identity, offset,
                                    link_error, sync_error, caller, result)
        return tuple(result)

    def test_disabled_egm_does_not_map(self):
        for api in (0, 1):
            with self.subTest(api=api):
                self.assertEqual(self.run_mapping(api=api, enabled=0),
                                 (NV_OK, 0, 0, 0, 0, 0, -1, 0))

    def test_legacy_egm_does_not_map(self):
        for api in (0, 1):
            with self.subTest(api=api):
                self.assertEqual(self.run_mapping(api=api, legacy=1),
                                 (NV_OK, 0, 0, 0, 0, 0, -1, 0))

    def test_identity_mapping_preserves_offset(self):
        for api in (0, 1):
            for offset in (0, 8192):
                with self.subTest(api=api, offset=offset):
                    self.assertEqual(self.run_mapping(api=api, identity=1, offset=offset),
                                     (NV_OK, offset, 0, 0, 0, 0, -1, 0))

    def test_success_accounts_for_mapping(self):
        for offset in (0, 8192):
            with self.subTest(offset=offset):
                self.assertEqual(self.run_mapping(offset=offset),
                                 (NV_OK, offset, PAGE_SIZE, 1, 1, 0, -1, 0))

    def test_link_errors_are_returned_without_sync(self):
        for error in (-12, -22):
            with self.subTest(error=error):
                self.assertEqual(self.run_mapping(link_error=error),
                                 (1000 - error, 0, 0, 1, 0, 0, -1, 0))

    def test_sync_error_is_returned_after_unlink(self):
        self.assertEqual(self.run_mapping(sync_error=-5),
                         (1005, 0, 0, 1, 1, 1, -1, 0))

    def test_unavailable_api_reports_not_supported(self):
        self.assertEqual(self.run_mapping(api=0),
                         (NV_ERR_NOT_SUPPORTED, 0, 0, 0, 0, 0, -1, 0))

    def test_caller_caches_successful_mapping(self):
        self.assertEqual(self.run_mapping(caller=1),
                         (NV_OK, 8192, PAGE_SIZE, 1, 1, 0, 7, 0))

    def test_caller_does_not_cache_failed_mapping(self):
        for link_error, sync_error, expected in ((-12, 0, (1012, 1, 0, 0)),
                                                (0, -5, (1005, 1, 1, 1))):
            with self.subTest(link=link_error, sync=sync_error):
                status, links, syncs, unlinks = expected
                self.assertEqual(self.run_mapping(caller=1, link_error=link_error, sync_error=sync_error),
                                 (status, -1, 0, links, syncs, unlinks, -1, 0))

    def test_caller_falls_back_when_api_is_unavailable(self):
        self.assertEqual(self.run_mapping(api=0, caller=1),
                         (NV_OK, -1, 0, 0, 0, 0, -1, 0))


if __name__ == "__main__":
    unittest.main()
