// Coccinelle script to fix 'pFunc' function pointer users to use the type
// expected by fix_nvoc_pfunc_type.cocci and ensure it gets passed two args.
//
//  NV_STATUS (*pFunc)(void *, void *);
//
// pFunc use is in src/kernel/gpu/deferred_api.c, src/kernel/gpu/gpu.c and
// src/libraries/resserv/src/rs_resource.c, therefore override the --dir
// option:
//
// options: --dir src/
//
// (c) 2025,2026 Open Source Security, Inc. All Rights Reserved.

// cases with a local variable
@pfunc_use_var@
struct NVOC_EXPORTED_METHOD_DEF *e;
typedef NV_STATUS;
expression arg;
identifier fn;
type T;
@@

-T fn = ((T) e->pFunc)
+NV_STATUS (*fn)(void *, void *) = e->pFunc
 ;
 <...
	fn(
		arg
+		, NULL
	  )
 ...>

// cases which cast and use ->pFunc directly
@pfunc_use_cast@
struct NVOC_EXPORTED_METHOD_DEF *e;
expression arg1, arg2;
type T;
@@
(
-((T) e->pFunc)
+(e->pFunc)
	(arg1, arg2)
|
-((T) e->pFunc)
+(e->pFunc)
	(arg1
+	, NULL
	)
)

// get rid of the now unused typedef as well
@pfunc_typedef@
type pfunc_use_var.T;
typedef NV_STATUS;
@@
- typedef NV_STATUS (*T)(...);
