==========================
Regulator API design notes
==========================

This document provides a brief, partially structured, overview of some
of the design considerations which impact the regulator API design.

Safety
------

 - Errors in regulator configuration can have very serious consequences
   for the system, potentially including lasting hardware damage.
 - It is not possible to automatically determine the power configuration
   of the system - software-equivalent variants of the same chip may
   have different power requirements, and not all components with power
   requirements are visible to software.

.. note::

     The API should make no changes to the hardware state unless it has
     specific knowledge that these changes are safe to perform on this
     particular system.

Consumer use cases
------------------

 - The overwhelming majority of devices in a system will have no
   requirement to do any runtime configuration of their power beyond
   being able to turn it on or off.

 - Many of the power supplies in the system will be shared between many
   different consumers.

.. note::

     The consumer API should be structured so that these use cases are
     very easy to handle and so that consumers will work with shared
     supplies without any additional effort.
