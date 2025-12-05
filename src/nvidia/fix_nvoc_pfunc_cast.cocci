// Coccinelle script to fix 'pFunc' function pointer casts to use proper
// types to prevent compile-time CFI violations, e.g. under RAP.
//
// After fix_nvoc_pfunc_2args.cocci has run, all *_EXPORT thunks will use a
// fitting type. The only task left is to drop the (now unnecessary) pFunc
// member assignment casts.
//
// (c) 2026 Open Source Security, Inc. All Rights Reserved.

@initialize:python@
@@
import re

# cocci's regex support is too limiting, use python for the filtering
type_match = re.compile(r"void *\( *\* *\) *\( *void *\)")	# cocci adds spaces

// search for (function) pointer casts (filtered in @pfunc_cast_filter@)
@pfunc_cast disable drop_cast@
identifier fn =~ "__EXPORT$";
type T;
@@
 (T) &fn

@script:python pfunc_cast_filter@
t << pfunc_cast.T;
f << pfunc_cast.fn;
@@
if not type_match.search(t):
#	print(f">>> '({t}) {f}' didn't match")
	cocci.include_match(False)

// just drop cast, the function already has the correct type
@remove_cast depends on pfunc_cast_filter@
identifier pfunc_cast.fn;
type pfunc_cast.T;
@@
- (T)
  &fn
