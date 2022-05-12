===================
Legacy instructions
===================

The arm64 port of the Linux kernel provides infrastructure to support
emulation of instructions which have been deprecated, or obsoleted in
the architecture. The infrastructure code uses undefined instruction
hooks to support emulation. Where available it also allows turning on
the instruction execution in hardware.

The emulation mode can be controlled by writing to sysctl nodes
(/proc/sys/abi). The following explains the different execution
behaviours and the corresponding values of the sysctl nodes -

* Undef
    Value: 0

  Generates undefined instruction abort. Default for instructions that
  have been obsoleted in the architecture, e.g., SWP

* Emulate
    Value: 1

  Uses software emulation. To aid migration of software, in this mode
  usage of emulated instruction is traced as well as rate limited
  warnings are issued. This is the default for deprecated
  instructions, .e.g., CP15 barriers

* Hardware Execution
    Value: 2

  Although marked as deprecated, some implementations may support the
  enabling/disabling of hardware support for the execution of these
  instructions. Using hardware execution generally provides better
  performance, but at the loss of ability to gather runtime statistics
  about the use of the deprecated instructions.

The default mode depends on the status of the instruction in the
architecture. Deprecated instructions should default to emulation
while obsolete instructions must be undefined by default.

Note: Instruction emulation may not be possible in all cases. See
individual instruction notes for further information.

Supported legacy instructions
-----------------------------
* SWP{B}

:Node: /proc/sys/abi/swp
:Status: Obsolete
:Default: Undef (0)

* CP15 Barriers

:Node: /proc/sys/abi/cp15_barrier
:Status: Deprecated
:Default: Emulate (1)

* SETEND

:Node: /proc/sys/abi/setend
:Status: Deprecated
:Default: Emulate (1)*

  Note: All the cpus on the system must have mixed endian support at EL0
  for this feature to be enabled. If a new CPU - which doesn't support mixed
  endian - is hotplugged in after this feature has been enabled, there could
  be unexpected results in the application.
