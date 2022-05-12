.. SPDX-License-Identifier: GPL-2.0

=========================
MPIC interrupt controller
=========================

Device types supported:

  - KVM_DEV_TYPE_FSL_MPIC_20     Freescale MPIC v2.0
  - KVM_DEV_TYPE_FSL_MPIC_42     Freescale MPIC v4.2

Only one MPIC instance, of any type, may be instantiated.  The created
MPIC will act as the system interrupt controller, connecting to each
vcpu's interrupt inputs.

Groups:
  KVM_DEV_MPIC_GRP_MISC
   Attributes:

    KVM_DEV_MPIC_BASE_ADDR (rw, 64-bit)
      Base address of the 256 KiB MPIC register space.  Must be
      naturally aligned.  A value of zero disables the mapping.
      Reset value is zero.

  KVM_DEV_MPIC_GRP_REGISTER (rw, 32-bit)
    Access an MPIC register, as if the access were made from the guest.
    "attr" is the byte offset into the MPIC register space.  Accesses
    must be 4-byte aligned.

    MSIs may be signaled by using this attribute group to write
    to the relevant MSIIR.

  KVM_DEV_MPIC_GRP_IRQ_ACTIVE (rw, 32-bit)
    IRQ input line for each standard openpic source.  0 is inactive and 1
    is active, regardless of interrupt sense.

    For edge-triggered interrupts:  Writing 1 is considered an activating
    edge, and writing 0 is ignored.  Reading returns 1 if a previously
    signaled edge has not been acknowledged, and 0 otherwise.

    "attr" is the IRQ number.  IRQ numbers for standard sources are the
    byte offset of the relevant IVPR from EIVPR0, divided by 32.

IRQ Routing:

  The MPIC emulation supports IRQ routing. Only a single MPIC device can
  be instantiated. Once that device has been created, it's available as
  irqchip id 0.

  This irqchip 0 has 256 interrupt pins, which expose the interrupts in
  the main array of interrupt sources (a.k.a. "SRC" interrupts).

  The numbering is the same as the MPIC device tree binding -- based on
  the register offset from the beginning of the sources array, without
  regard to any subdivisions in chip documentation such as "internal"
  or "external" interrupts.

  Access to non-SRC interrupts is not implemented through IRQ routing mechanisms.
