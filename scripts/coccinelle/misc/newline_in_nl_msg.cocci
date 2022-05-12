// SPDX-License-Identifier: GPL-2.0-only
///
/// Catch strings ending in newline with GENL_SET_ERR_MSG, NL_SET_ERR_MSG,
/// NL_SET_ERR_MSG_MOD.
///
// Confidence: Very High
// Copyright: (C) 2020 Intel Corporation
// URL: http://coccinelle.lip6.fr/
// Options: --no-includes --include-headers

virtual context
virtual org
virtual report

@r depends on context || org || report@
expression e;
constant m;
position p;
@@
  \(GENL_SET_ERR_MSG\|NL_SET_ERR_MSG\|NL_SET_ERR_MSG_MOD\)(e,m@p)

@script:python@
m << r.m;
@@

if not m.endswith("\\n\""):
	cocci.include_match(False)

@r1 depends on r@
identifier fname;
expression r.e;
constant r.m;
position r.p;
@@
  fname(e,m@p)

//----------------------------------------------------------
//  For context mode
//----------------------------------------------------------

@depends on context && r@
identifier r1.fname;
expression r.e;
constant r.m;
@@
* fname(e,m)

//----------------------------------------------------------
//  For org mode
//----------------------------------------------------------

@script:python depends on org@
fname << r1.fname;
m << r.m;
p << r.p;
@@

if m.endswith("\\n\""):
	msg="WARNING avoid newline at end of message in %s" % (fname)
	msg_safe=msg.replace("[","@(").replace("]",")")
	coccilib.org.print_todo(p[0], msg_safe)

//----------------------------------------------------------
//  For report mode
//----------------------------------------------------------

@script:python depends on report@
fname << r1.fname;
m << r.m;
p << r.p;
@@

if m.endswith("\\n\""):
	msg="WARNING avoid newline at end of message in %s" % (fname)
	coccilib.report.print_report(p[0], msg)
