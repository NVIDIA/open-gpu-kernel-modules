// Coccinelle script to get rid of conflicting typedef -- it's not even needed.
//
// (c) 2026 Open Source Security, Inc. All Rights Reserved.

@@ @@
-typedef struct MMU_TRACE_CALLBACKS MMU_TRACE_CALLBACKS;
