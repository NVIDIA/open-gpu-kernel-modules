/* SPDX-License-Identifier: GPL-2.0 */

#ifndef selftest
#define selftest(x, y)
#endif

/*
 * List each unit test as selftest(name, function)
 *
 * The name is used as both an enum and expanded as subtest__name to create
 * a module parameter. It must be unique and legal for a C identifier.
 *
 * The function should be of type int function(void). It may be conditionally
 * compiled using #if IS_ENABLED(CONFIG_DRM_I915_SELFTEST).
 *
 * Tests are executed in order by igt/i915_selftest
 */
selftest(sanitycheck, i915_mock_sanitycheck) /* keep first (igt selfcheck) */
selftest(shmem, shmem_utils_mock_selftests)
selftest(fence, i915_sw_fence_mock_selftests)
selftest(scatterlist, scatterlist_mock_selftests)
selftest(syncmap, i915_syncmap_mock_selftests)
selftest(uncore, intel_uncore_mock_selftests)
selftest(ring, intel_ring_mock_selftests)
selftest(engine, intel_engine_cs_mock_selftests)
selftest(timelines, intel_timeline_mock_selftests)
selftest(requests, i915_request_mock_selftests)
selftest(objects, i915_gem_object_mock_selftests)
selftest(phys, i915_gem_phys_mock_selftests)
selftest(dmabuf, i915_gem_dmabuf_mock_selftests)
selftest(vma, i915_vma_mock_selftests)
selftest(evict, i915_gem_evict_mock_selftests)
selftest(gtt, i915_gem_gtt_mock_selftests)
selftest(hugepages, i915_gem_huge_page_mock_selftests)
selftest(contexts, i915_gem_context_mock_selftests)
selftest(memory_region, intel_memory_region_mock_selftests)
