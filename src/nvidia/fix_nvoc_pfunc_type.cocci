// Coccinelle script to fix 'pFunc' member function pointer type to match what
// fix_nvoc_pfunc_*.cocci expects.
//
// The structure type is located in inc/libraries/nvoc/runtime.h, override
// the --dir ... parameter to only patch that:
//
// options: --dir inc/libraries/nvoc/
//
// (c) 2025,2026 Open Source Security, Inc. All Rights Reserved.

@pfunc_member@
typedef NV_STATUS;
@@
struct NVOC_EXPORTED_METHOD_DEF
{
	...
-	void (*pFunc) (...)
+	NV_STATUS (*pFunc)(void *, void *)
	;
	...
};
