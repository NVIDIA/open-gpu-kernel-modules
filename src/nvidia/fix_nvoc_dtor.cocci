// Coccinelle script to fix 'dtor' function pointer casts to use proper
// types to prevent runtime CFI violations, e.g. under RAP.
//
// We need to create a thunk in any case as there may be calls to the strongly
// typed function from other TUs, causing CFI violations if we'd just change
// the function's signature to match the dtor function pointer prototype.
//
// typedef void (*NVOC_DYNAMIC_DTOR)(Dynamic*);
//
// TODO: create thunks only for decls that don't match the expected prototype!
//
// (c) 2025,2026 Open Source Security, Inc. All Rights Reserved.

// replace function casts to (NVOC_DYNAMIC_DTOR) with its thunk
@dtor_cast@
identifier fn;
fresh identifier fnthunk = fn ## "_THUNK";
@@
- (NVOC_DYNAMIC_DTOR) &fn
+ &fnthunk

// add decl for the thunk, if needed, i.e. if the original func had one
@thunk_decl@
identifier dtor_cast.fn, dtor_cast.fnthunk, arg;
typedef Dynamic;
type T, R;
@@
// XXX: matching function decls is only poorly supported, so we need this hack
(
-R fn(T)
+R fn(T);
+static void fnthunk(Dynamic *)
;
|
-R fn(T arg)
+R fn(T arg);
+static void fnthunk(Dynamic *arg)
;
)

// add thunk function
@thunk_def@
identifier dtor_cast.fn, dtor_cast.fnthunk, arg;
typedef Dynamic;
type T, R;
@@
R fn(T arg) { ... }
+
+static void fnthunk(Dynamic *arg) {
+  fn((T)arg);
+}
