// Coccinelle script to fix structure initialization to use designated
// initializer to make these compatible with Linux's RANDSTRUCT.
//
// Here we handle instances of HAL_IFACE_SETUP:
//
//   typedef struct {
//       void (*rpcHalIfacesSetupFn)(PRPC_HAL_IFACES pRpcHal);
//       void (*rpcstructurecopyHalIfacesSetupFn)(PRPCSTRUCTURECOPY_HAL_IFACES pRpcstructurecopyHal);
//   } HAL_IFACE_SETUP
//
// We simply hard-code the field names.
//
// (c) 2026 Open Source Security, Inc. All Rights Reserved.

@@
identifier his;
identifier f1, f2;
@@
 static HAL_IFACE_SETUP his = {
-	f1,
-	f2,
+   .rpcHalIfacesSetupFn = f1,
+	.rpcstructurecopyHalIfacesSetupFn = f2,
 };
