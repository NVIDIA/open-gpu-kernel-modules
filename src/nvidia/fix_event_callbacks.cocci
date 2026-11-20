// Coccinelle script to make EVENT_BUS_CALLBACKS and EventBufferProducerState
// type definitions __no_const as these get embedded into types that cannot be
// constified.
//
// As there is no easy way to define new attributes for cocci, post-process the
// patch.
//
// (c) 2026 Open Source Security, Inc. All Rights Reserved.

@event_bus_callbacks@
@@
 typedef struct
 {
 ...
-} EVENT_BUS_CALLBACKS;
+} /*__no_const*/ EVENT_BUS_CALLBACKS;

@event_buffer_producer_state@
@@
 typedef struct EventBufferProducerState
 {
 ...
-} EventBufferProducerState;
+} /*__no_const*/ EventBufferProducerState;
