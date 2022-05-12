======================
Memory Management APIs
======================

User Space Memory Access
========================

.. kernel-doc:: arch/x86/include/asm/uaccess.h
   :internal:

.. kernel-doc:: arch/x86/lib/usercopy_32.c
   :export:

.. kernel-doc:: mm/gup.c
   :functions: get_user_pages_fast

.. _mm-api-gfp-flags:

Memory Allocation Controls
==========================

.. kernel-doc:: include/linux/gfp.h
   :internal:

.. kernel-doc:: include/linux/gfp.h
   :doc: Page mobility and placement hints

.. kernel-doc:: include/linux/gfp.h
   :doc: Watermark modifiers

.. kernel-doc:: include/linux/gfp.h
   :doc: Reclaim modifiers

.. kernel-doc:: include/linux/gfp.h
   :doc: Useful GFP flag combinations

The Slab Cache
==============

.. kernel-doc:: include/linux/slab.h
   :internal:

.. kernel-doc:: mm/slab.c
   :export:

.. kernel-doc:: mm/slab_common.c
   :export:

.. kernel-doc:: mm/util.c
   :functions: kfree_const kvmalloc_node kvfree

Virtually Contiguous Mappings
=============================

.. kernel-doc:: mm/vmalloc.c
   :export:

File Mapping and Page Cache
===========================

.. kernel-doc:: mm/readahead.c
   :export:

.. kernel-doc:: mm/filemap.c
   :export:

.. kernel-doc:: mm/page-writeback.c
   :export:

.. kernel-doc:: mm/truncate.c
   :export:

.. kernel-doc:: include/linux/pagemap.h
   :internal:

Memory pools
============

.. kernel-doc:: mm/mempool.c
   :export:

DMA pools
=========

.. kernel-doc:: mm/dmapool.c
   :export:

More Memory Management Functions
================================

.. kernel-doc:: mm/memory.c
   :export:

.. kernel-doc:: mm/page_alloc.c
.. kernel-doc:: mm/mempolicy.c
.. kernel-doc:: include/linux/mm_types.h
   :internal:
.. kernel-doc:: include/linux/mm.h
   :internal:
.. kernel-doc:: include/linux/mmzone.h
