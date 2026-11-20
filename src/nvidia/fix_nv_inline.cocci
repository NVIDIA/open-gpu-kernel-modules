// Coccinelle script to fix return type vs 'NV_INLINE' keyword order to avoid warnings like:
//
// ../src/nvidia/generated/g_hal_register.h:25:1: warning: ‘inline’ is not at beginning of declaration [-Wold-style-declaration]
//    25 | static NV_STATUS NV_INLINE REGISTER_TU10X_HALS(void)
//       | ^~~~~~
//
// (c) 2026 Open Source Security, Inc. All Rights Reserved.

@nv_inline_order@
identifier func;
typedef NV_STATUS;
attribute name NV_INLINE;
@@
// XXX: Coccinelle doesn't handle wrapped keywords well, that's why we have to
// XXX: do this hack involving the "superfluous" remove&add of 'static'.
-static NV_STATUS
+static
 NV_INLINE
+NV_STATUS
 func(void)
 {
  ...
 }
