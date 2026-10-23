// Coccinelle script to fix 'pFunc' function pointer casts to use proper types.
//
// Similar to fix_nvoc_pfunc_cast.cocci, just for the NULL fptr.
//
// We simply want to drop the cast as it's unneeded. As it's in the '#if 0'
// block, we need to tell cocci to look at that:
//
// options: --noif0-passing
//
// (c) 2025,2026 Open Source Security, Inc. All Rights Reserved.

@initialize:python@
@@
import re

type_match = re.compile(r"void *\( *\* *\) *\( *void *\)")	# cocci adds spaces

// search for (function) pointer casts (filtered in @pfunc_cast_filter@)
@pfunc_null_cast disable drop_cast@
type T;
@@
 (T) NULL

@script:python pfunc_cast_filter@
t << pfunc_null_cast.T;
@@
if not type_match.search(t):
	cocci.include_match(False)

@remove_null_cast depends on pfunc_cast_filter@
type pfunc_null_cast.T;
@@
- (T)
  NULL

