.. SPDX-License-Identifier: GPL-2.0

arch/riscv maintenance guidelines for developers
================================================

Overview
--------
The RISC-V instruction set architecture is developed in the open:
in-progress drafts are available for all to review and to experiment
with implementations.  New module or extension drafts can change
during the development process - sometimes in ways that are
incompatible with previous drafts.  This flexibility can present a
challenge for RISC-V Linux maintenance.  Linux maintainers disapprove
of churn, and the Linux development process prefers well-reviewed and
tested code over experimental code.  We wish to extend these same
principles to the RISC-V-related code that will be accepted for
inclusion in the kernel.

Submit Checklist Addendum
-------------------------
We'll only accept patches for new modules or extensions if the
specifications for those modules or extensions are listed as being
"Frozen" or "Ratified" by the RISC-V Foundation.  (Developers may, of
course, maintain their own Linux kernel trees that contain code for
any draft extensions that they wish.)

Additionally, the RISC-V specification allows implementors to create
their own custom extensions.  These custom extensions aren't required
to go through any review or ratification process by the RISC-V
Foundation.  To avoid the maintenance complexity and potential
performance impact of adding kernel code for implementor-specific
RISC-V extensions, we'll only to accept patches for extensions that
have been officially frozen or ratified by the RISC-V Foundation.
(Implementors, may, of course, maintain their own Linux kernel trees
containing code for any custom extensions that they wish.)
