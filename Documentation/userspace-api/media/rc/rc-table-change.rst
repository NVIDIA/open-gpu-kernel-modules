.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

.. _Remote_controllers_table_change:

*******************************************
Changing default Remote Controller mappings
*******************************************

The event interface provides two ioctls to be used against the
/dev/input/event device, to allow changing the default keymapping.

This program demonstrates how to replace the keymap tables.


.. toctree::
    :maxdepth: 1

    keytable.c
