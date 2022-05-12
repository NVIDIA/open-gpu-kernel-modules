.. SPDX-License-Identifier: GPL-2.0

===========================
Test Style and Nomenclature
===========================

To make finding, writing, and using KUnit tests as simple as possible, it's
strongly encouraged that they are named and written according to the guidelines
below. While it's possible to write KUnit tests which do not follow these rules,
they may break some tooling, may conflict with other tests, and may not be run
automatically by testing systems.

It's recommended that you only deviate from these guidelines when:

1. Porting tests to KUnit which are already known with an existing name, or
2. Writing tests which would cause serious problems if automatically run (e.g.,
   non-deterministically producing false positives or negatives, or taking an
   extremely long time to run).

Subsystems, Suites, and Tests
=============================

In order to make tests as easy to find as possible, they're grouped into suites
and subsystems. A test suite is a group of tests which test a related area of
the kernel, and a subsystem is a set of test suites which test different parts
of the same kernel subsystem or driver.

Subsystems
----------

Every test suite must belong to a subsystem. A subsystem is a collection of one
or more KUnit test suites which test the same driver or part of the kernel. A
rule of thumb is that a test subsystem should match a single kernel module. If
the code being tested can't be compiled as a module, in many cases the subsystem
should correspond to a directory in the source tree or an entry in the
MAINTAINERS file. If unsure, follow the conventions set by tests in similar
areas.

Test subsystems should be named after the code being tested, either after the
module (wherever possible), or after the directory or files being tested. Test
subsystems should be named to avoid ambiguity where necessary.

If a test subsystem name has multiple components, they should be separated by
underscores. *Do not* include "test" or "kunit" directly in the subsystem name
unless you are actually testing other tests or the kunit framework itself.

Example subsystems could be:

``ext4``
  Matches the module and filesystem name.
``apparmor``
  Matches the module name and LSM name.
``kasan``
  Common name for the tool, prominent part of the path ``mm/kasan``
``snd_hda_codec_hdmi``
  Has several components (``snd``, ``hda``, ``codec``, ``hdmi``) separated by
  underscores. Matches the module name.

Avoid names like these:

``linear-ranges``
  Names should use underscores, not dashes, to separate words. Prefer
  ``linear_ranges``.
``qos-kunit-test``
  As well as using underscores, this name should not have "kunit-test" as a
  suffix, and ``qos`` is ambiguous as a subsystem name. ``power_qos`` would be a
  better name.
``pc_parallel_port``
  The corresponding module name is ``parport_pc``, so this subsystem should also
  be named ``parport_pc``.

.. note::
        The KUnit API and tools do not explicitly know about subsystems. They're
        simply a way of categorising test suites and naming modules which
        provides a simple, consistent way for humans to find and run tests. This
        may change in the future, though.

Suites
------

KUnit tests are grouped into test suites, which cover a specific area of
functionality being tested. Test suites can have shared initialisation and
shutdown code which is run for all tests in the suite.
Not all subsystems will need to be split into multiple test suites (e.g. simple drivers).

Test suites are named after the subsystem they are part of. If a subsystem
contains several suites, the specific area under test should be appended to the
subsystem name, separated by an underscore.

In the event that there are multiple types of test using KUnit within a
subsystem (e.g., both unit tests and integration tests), they should be put into
separate suites, with the type of test as the last element in the suite name.
Unless these tests are actually present, avoid using ``_test``, ``_unittest`` or
similar in the suite name.

The full test suite name (including the subsystem name) should be specified as
the ``.name`` member of the ``kunit_suite`` struct, and forms the base for the
module name (see below).

Example test suites could include:

``ext4_inode``
  Part of the ``ext4`` subsystem, testing the ``inode`` area.
``kunit_try_catch``
  Part of the ``kunit`` implementation itself, testing the ``try_catch`` area.
``apparmor_property_entry``
  Part of the ``apparmor`` subsystem, testing the ``property_entry`` area.
``kasan``
  The ``kasan`` subsystem has only one suite, so the suite name is the same as
  the subsystem name.

Avoid names like:

``ext4_ext4_inode``
  There's no reason to state the subsystem twice.
``property_entry``
  The suite name is ambiguous without the subsystem name.
``kasan_integration_test``
  Because there is only one suite in the ``kasan`` subsystem, the suite should
  just be called ``kasan``. There's no need to redundantly add
  ``integration_test``. Should a separate test suite with, for example, unit
  tests be added, then that suite could be named ``kasan_unittest`` or similar.

Test Cases
----------

Individual tests consist of a single function which tests a constrained
codepath, property, or function. In the test output, individual tests' results
will show up as subtests of the suite's results.

Tests should be named after what they're testing. This is often the name of the
function being tested, with a description of the input or codepath being tested.
As tests are C functions, they should be named and written in accordance with
the kernel coding style.

.. note::
        As tests are themselves functions, their names cannot conflict with
        other C identifiers in the kernel. This may require some creative
        naming. It's a good idea to make your test functions `static` to avoid
        polluting the global namespace.

Example test names include:

``unpack_u32_with_null_name``
  Tests the ``unpack_u32`` function when a NULL name is passed in.
``test_list_splice``
  Tests the ``list_splice`` macro. It has the prefix ``test_`` to avoid a
  name conflict with the macro itself.


Should it be necessary to refer to a test outside the context of its test suite,
the *fully-qualified* name of a test should be the suite name followed by the
test name, separated by a colon (i.e. ``suite:test``).

Test Kconfig Entries
====================

Every test suite should be tied to a Kconfig entry.

This Kconfig entry must:

* be named ``CONFIG_<name>_KUNIT_TEST``: where <name> is the name of the test
  suite.
* be listed either alongside the config entries for the driver/subsystem being
  tested, or be under [Kernel Hacking]→[Kernel Testing and Coverage]
* depend on ``CONFIG_KUNIT``
* be visible only if ``CONFIG_KUNIT_ALL_TESTS`` is not enabled.
* have a default value of ``CONFIG_KUNIT_ALL_TESTS``.
* have a brief description of KUnit in the help text

Unless there's a specific reason not to (e.g. the test is unable to be built as
a module), Kconfig entries for tests should be tristate.

An example Kconfig entry:

.. code-block:: none

	config FOO_KUNIT_TEST
		tristate "KUnit test for foo" if !KUNIT_ALL_TESTS
		depends on KUNIT
		default KUNIT_ALL_TESTS
		help
		  This builds unit tests for foo.

		  For more information on KUnit and unit tests in general, please refer
		  to the KUnit documentation in Documentation/dev-tools/kunit/.

		  If unsure, say N.


Test File and Module Names
==========================

KUnit tests can often be compiled as a module. These modules should be named
after the test suite, followed by ``_test``. If this is likely to conflict with
non-KUnit tests, the suffix ``_kunit`` can also be used.

The easiest way of achieving this is to name the file containing the test suite
``<suite>_test.c`` (or, as above, ``<suite>_kunit.c``). This file should be
placed next to the code under test.

If the suite name contains some or all of the name of the test's parent
directory, it may make sense to modify the source filename to reduce redundancy.
For example, a ``foo_firmware`` suite could be in the ``foo/firmware_test.c``
file.
