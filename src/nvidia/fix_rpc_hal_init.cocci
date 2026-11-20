// Coccinelle script to fix structure initialization to use designated
// initializer to make these compatible with Linux's RANDSTRUCT.
//
// As coccinelle has no support for referencing a field's member name, we
// post-process the patch with a sed script to do just that (the field name is
// mentioned by a trailing comment).
//
// (c) 2026 Open Source Security, Inc. All Rights Reserved.

@@
typedef RPC_HAL_IFACES, RPCSTRUCTURECOPY_HAL_IFACES;
identifier id;
@@

 static
(
 RPC_HAL_IFACES
|
 RPCSTRUCTURECOPY_HAL_IFACES
)
 id = {
-	...
 };
