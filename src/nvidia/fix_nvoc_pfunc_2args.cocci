// Coccinelle script to fix 'pFunc' function pointer casts to use proper
// types to prevent runtime CFI violations, e.g. under RAP.
//
// Since 610.43.02 the pFunc functions use a thunk with an almost fitting type:
//
//   NV_STATUS (*pFunc)(void *, void *);
//
// However, there are instances of thunks that get passed only a single
// argument, which will cause CFI violations again. Fix these, based on the
// heuristic that these functions will be called *__EXPORT() and have a
// signature of 'NV_STATUS ()(void *)'.
//
// (c) 2026 Open Source Security, Inc. All Rights Reserved.

@pfunc_one_arg@
identifier pfunc =~ "__EXPORT$";
identifier arg;
typedef NV_STATUS;
@@
 NV_STATUS pfunc(void *arg
+              , void *_null
 )
 {
+ (void) _null;
  ...
 }
