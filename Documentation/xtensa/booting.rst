=====================================
Passing boot parameters to the kernel
=====================================

Boot parameters are represented as a TLV list in the memory. Please see
arch/xtensa/include/asm/bootparam.h for definition of the bp_tag structure and
tag value constants. First entry in the list must have type BP_TAG_FIRST, last
entry must have type BP_TAG_LAST. The address of the first list entry is
passed to the kernel in the register a2. The address type depends on MMU type:

- For configurations without MMU, with region protection or with MPU the
  address must be the physical address.
- For configurations with region translarion MMU or with MMUv3 and CONFIG_MMU=n
  the address must be a valid address in the current mapping. The kernel will
  not change the mapping on its own.
- For configurations with MMUv2 the address must be a virtual address in the
  default virtual mapping (0xd0000000..0xffffffff).
- For configurations with MMUv3 and CONFIG_MMU=y the address may be either a
  virtual or physical address. In either case it must be within the default
  virtual mapping. It is considered physical if it is within the range of
  physical addresses covered by the default KSEG mapping (XCHAL_KSEG_PADDR..
  XCHAL_KSEG_PADDR + XCHAL_KSEG_SIZE), otherwise it is considered virtual.
