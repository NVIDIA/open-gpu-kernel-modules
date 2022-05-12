// SPDX-License-Identifier: GPL-2.0-only
/// Use BUG_ON instead of a if condition followed by BUG.
///
//# This makes an effort to find cases where BUG() follows an if
//# condition on an expression and replaces the if condition and BUG()
//# with a BUG_ON having the conditional expression of the if statement
//# as argument.
//
// Confidence: High
// Copyright: (C) 2014 Himangi Saraogi.
// Comments:
// Options: --no-includes --include-headers

virtual patch
virtual context
virtual org
virtual report

//----------------------------------------------------------
//  For context mode
//----------------------------------------------------------

@depends on context@
expression e;
@@

*if (e) BUG();

//----------------------------------------------------------
//  For patch mode
//----------------------------------------------------------

@depends on patch@
expression e;
@@

-if (e) BUG();
+BUG_ON(e);

//----------------------------------------------------------
//  For org and report mode
//----------------------------------------------------------

@r depends on (org || report)@
expression e;
position p;
@@

 if (e) BUG@p ();

@script:python depends on org@
p << r.p;
@@

coccilib.org.print_todo(p[0], "WARNING use BUG_ON")

@script:python depends on report@
p << r.p;
@@

msg="WARNING: Use BUG_ON instead of if condition followed by BUG.\nPlease make sure the condition has no side effects (see conditional BUG_ON definition in include/asm-generic/bug.h)"
coccilib.report.print_report(p[0], msg)

