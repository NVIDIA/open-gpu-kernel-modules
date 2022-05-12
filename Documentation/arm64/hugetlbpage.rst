.. _hugetlbpage_index:

====================
HugeTLBpage on ARM64
====================

Hugepage relies on making efficient use of TLBs to improve performance of
address translations. The benefit depends on both -

  - the size of hugepages
  - size of entries supported by the TLBs

The ARM64 port supports two flavours of hugepages.

1) Block mappings at the pud/pmd level
--------------------------------------

These are regular hugepages where a pmd or a pud page table entry points to a
block of memory. Regardless of the supported size of entries in TLB, block
mappings reduce the depth of page table walk needed to translate hugepage
addresses.

2) Using the Contiguous bit
---------------------------

The architecture provides a contiguous bit in the translation table entries
(D4.5.3, ARM DDI 0487C.a) that hints to the MMU to indicate that it is one of a
contiguous set of entries that can be cached in a single TLB entry.

The contiguous bit is used in Linux to increase the mapping size at the pmd and
pte (last) level. The number of supported contiguous entries varies by page size
and level of the page table.


The following hugepage sizes are supported -

  ====== ========   ====    ========    ===
  -      CONT PTE    PMD    CONT PMD    PUD
  ====== ========   ====    ========    ===
  4K:         64K     2M         32M     1G
  16K:         2M    32M          1G
  64K:         2M   512M         16G
  ====== ========   ====    ========    ===
