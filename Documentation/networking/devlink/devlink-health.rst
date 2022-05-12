.. SPDX-License-Identifier: GPL-2.0

==============
Devlink Health
==============

Background
==========

The ``devlink`` health mechanism is targeted for Real Time Alerting, in
order to know when something bad happened to a PCI device.

  * Provide alert debug information.
  * Self healing.
  * If problem needs vendor support, provide a way to gather all needed
    debugging information.

Overview
========

The main idea is to unify and centralize driver health reports in the
generic ``devlink`` instance and allow the user to set different
attributes of the health reporting and recovery procedures.

The ``devlink`` health reporter:
Device driver creates a "health reporter" per each error/health type.
Error/Health type can be a known/generic (e.g. PCI error, fw error, rx/tx error)
or unknown (driver specific).
For each registered health reporter a driver can issue error/health reports
asynchronously. All health reports handling is done by ``devlink``.
Device driver can provide specific callbacks for each "health reporter", e.g.:

  * Recovery procedures
  * Diagnostics procedures
  * Object dump procedures
  * OOB initial parameters

Different parts of the driver can register different types of health reporters
with different handlers.

Actions
=======

Once an error is reported, devlink health will perform the following actions:

  * A log is being send to the kernel trace events buffer
  * Health status and statistics are being updated for the reporter instance
  * Object dump is being taken and saved at the reporter instance (as long as
    there is no other dump which is already stored)
  * Auto recovery attempt is being done. Depends on:

    - Auto-recovery configuration
    - Grace period vs. time passed since last recover

User Interface
==============

User can access/change each reporter's parameters and driver specific callbacks
via ``devlink``, e.g per error type (per health reporter):

  * Configure reporter's generic parameters (like: disable/enable auto recovery)
  * Invoke recovery procedure
  * Run diagnostics
  * Object dump

.. list-table:: List of devlink health interfaces
   :widths: 10 90

   * - Name
     - Description
   * - ``DEVLINK_CMD_HEALTH_REPORTER_GET``
     - Retrieves status and configuration info per DEV and reporter.
   * - ``DEVLINK_CMD_HEALTH_REPORTER_SET``
     - Allows reporter-related configuration setting.
   * - ``DEVLINK_CMD_HEALTH_REPORTER_RECOVER``
     - Triggers reporter's recovery procedure.
   * - ``DEVLINK_CMD_HEALTH_REPORTER_TEST``
     - Triggers a fake health event on the reporter. The effects of the test
       event in terms of recovery flow should follow closely that of a real
       event.
   * - ``DEVLINK_CMD_HEALTH_REPORTER_DIAGNOSE``
     - Retrieves current device state related to the reporter.
   * - ``DEVLINK_CMD_HEALTH_REPORTER_DUMP_GET``
     - Retrieves the last stored dump. Devlink health
       saves a single dump. If an dump is not already stored by devlink
       for this reporter, devlink generates a new dump.
       Dump output is defined by the reporter.
   * - ``DEVLINK_CMD_HEALTH_REPORTER_DUMP_CLEAR``
     - Clears the last saved dump file for the specified reporter.

The following diagram provides a general overview of ``devlink-health``::

                                                   netlink
                                          +--------------------------+
                                          |                          |
                                          |            +             |
                                          |            |             |
                                          +--------------------------+
                                                       |request for ops
                                                       |(diagnose,
      driver                               devlink     |recover,
                                                       |dump)
    +--------+                            +--------------------------+
    |        |                            |    reporter|             |
    |        |                            |  +---------v----------+  |
    |        |   ops execution            |  |                    |  |
    |     <----------------------------------+                    |  |
    |        |                            |  |                    |  |
    |        |                            |  + ^------------------+  |
    |        |                            |    | request for ops     |
    |        |                            |    | (recover, dump)     |
    |        |                            |    |                     |
    |        |                            |  +-+------------------+  |
    |        |     health report          |  | health handler     |  |
    |        +------------------------------->                    |  |
    |        |                            |  +--------------------+  |
    |        |     health reporter create |                          |
    |        +---------------------------->                          |
    +--------+                            +--------------------------+
