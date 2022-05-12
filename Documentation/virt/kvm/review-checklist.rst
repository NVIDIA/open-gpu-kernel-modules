.. SPDX-License-Identifier: GPL-2.0

================================
Review checklist for kvm patches
================================

1.  The patch must follow Documentation/process/coding-style.rst and
    Documentation/process/submitting-patches.rst.

2.  Patches should be against kvm.git master branch.

3.  If the patch introduces or modifies a new userspace API:
    - the API must be documented in Documentation/virt/kvm/api.rst
    - the API must be discoverable using KVM_CHECK_EXTENSION

4.  New state must include support for save/restore.

5.  New features must default to off (userspace should explicitly request them).
    Performance improvements can and should default to on.

6.  New cpu features should be exposed via KVM_GET_SUPPORTED_CPUID2

7.  Emulator changes should be accompanied by unit tests for qemu-kvm.git
    kvm/test directory.

8.  Changes should be vendor neutral when possible.  Changes to common code
    are better than duplicating changes to vendor code.

9.  Similarly, prefer changes to arch independent code than to arch dependent
    code.

10. User/kernel interfaces and guest/host interfaces must be 64-bit clean
    (all variables and sizes naturally aligned on 64-bit; use specific types
    only - u64 rather than ulong).

11. New guest visible features must either be documented in a hardware manual
    or be accompanied by documentation.

12. Features must be robust against reset and kexec - for example, shared
    host/guest memory must be unshared to prevent the host from writing to
    guest memory that the guest has not reserved for this purpose.
