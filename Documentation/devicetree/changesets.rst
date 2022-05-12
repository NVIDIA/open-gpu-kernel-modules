.. SPDX-License-Identifier: GPL-2.0

=====================
Devicetree Changesets
=====================

A Devicetree changeset is a method which allows one to apply changes
in the live tree in such a way that either the full set of changes
will be applied, or none of them will be. If an error occurs partway
through applying the changeset, then the tree will be rolled back to the
previous state. A changeset can also be removed after it has been
applied.

When a changeset is applied, all of the changes get applied to the tree
at once before emitting OF_RECONFIG notifiers. This is so that the
receiver sees a complete and consistent state of the tree when it
receives the notifier.

The sequence of a changeset is as follows.

1. of_changeset_init() - initializes a changeset

2. A number of DT tree change calls, of_changeset_attach_node(),
   of_changeset_detach_node(), of_changeset_add_property(),
   of_changeset_remove_property, of_changeset_update_property() to prepare
   a set of changes. No changes to the active tree are made at this point.
   All the change operations are recorded in the of_changeset 'entries'
   list.

3. of_changeset_apply() - Apply the changes to the tree. Either the
   entire changeset will get applied, or if there is an error the tree will
   be restored to the previous state. The core ensures proper serialization
   through locking. An unlocked version __of_changeset_apply is available,
   if needed.

If a successfully applied changeset needs to be removed, it can be done
with of_changeset_revert().
