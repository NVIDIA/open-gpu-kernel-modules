.. SPDX-License-Identifier: GPL-2.0

===================
Devicetree (DT) ABI
===================

I. Regarding stable bindings/ABI, we quote from the 2013 ARM mini-summit
   summary document:

     "That still leaves the question of, what does a stable binding look
     like?  Certainly a stable binding means that a newer kernel will not
     break on an older device tree, but that doesn't mean the binding is
     frozen for all time. Grant said there are ways to change bindings that
     don't result in breakage. For instance, if a new property is added,
     then default to the previous behaviour if it is missing. If a binding
     truly needs an incompatible change, then change the compatible string
     at the same time.  The driver can bind against both the old and the
     new. These guidelines aren't new, but they desperately need to be
     documented."

II.  General binding rules

  1) Maintainers, don't let perfect be the enemy of good.  Don't hold up a
     binding because it isn't perfect.

  2) Use specific compatible strings so that if we need to add a feature (DMA)
     in the future, we can create a new compatible string.  See I.

  3) Bindings can be augmented, but the driver shouldn't break when given
     the old binding. ie. add additional properties, but don't change the
     meaning of an existing property. For drivers, default to the original
     behaviour when a newly added property is missing.

  4) Don't submit bindings for staging or unstable.  That will be decided by
     the devicetree maintainers *after* discussion on the mailinglist.

III. Notes

  1) This document is intended as a general familiarization with the process as
     decided at the 2013 Kernel Summit.  When in doubt, the current word of the
     devicetree maintainers overrules this document.  In that situation, a patch
     updating this document would be appreciated.
