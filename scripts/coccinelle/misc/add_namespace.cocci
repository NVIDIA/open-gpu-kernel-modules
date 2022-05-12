// SPDX-License-Identifier: GPL-2.0-only
//
/// Adds missing MODULE_IMPORT_NS statements to source files
///
/// This script is usually called from scripts/nsdeps with -D ns=<namespace> to
/// add a missing namespace tag to a module source file.
///

virtual nsdeps
virtual report

@has_ns_import@
declarer name MODULE_IMPORT_NS;
identifier virtual.ns;
@@
MODULE_IMPORT_NS(ns);

// Add missing imports, but only adjacent to a MODULE_LICENSE statement.
// That ensures we are adding it only to the main module source file.
@do_import depends on !has_ns_import && nsdeps@
declarer name MODULE_LICENSE;
expression license;
identifier virtual.ns;
@@
MODULE_LICENSE(license);
+ MODULE_IMPORT_NS(ns);

// Dummy rule for report mode that would otherwise be empty and make spatch
// fail ("No rules apply.")
@script:python depends on report@
@@
