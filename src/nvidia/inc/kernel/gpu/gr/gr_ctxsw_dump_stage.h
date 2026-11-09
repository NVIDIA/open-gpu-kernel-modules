#ifndef GR_CTXSW_DUMP_STAGE_H
#define GR_CTXSW_DUMP_STAGE_H

#include "nvtypes.h"

//
// Staging buffer for the ctxsw-timeout (Xid 109) hang-debug dump.
//
// GSP-RM encodes the dump into payload[] from rcErrorRecovery, before the
// engine reset clears the register state, then writes len. The RC_TRIGGERED
// event that follows carries bHangDebugDumpStaged; CPU-RM copies the payload
// out on that flag and journals it via rcdbAddProtobufDump.
//
// only the first ctxsw timeout is captured.
//
//
#define GR_CTXSW_DUMP_STAGE_MAGIC   0x43545347U  /* 'CTSG' */

#define GR_CTXSW_DUMP_STAGE_SIZE    (1536u * 1024u)

typedef struct
{
    NvU32 magic;     /* GR_CTXSW_DUMP_STAGE_MAGIC, written by CPU-RM at alloc */
    NvU32 len;       /* encoded payload bytes; 0 = nothing staged */
    NvU8  payload[]; /* prb-encoded HangDebug message; carries its own xid and timestamp */
} GR_CTXSW_DUMP_STAGE;

#define GR_CTXSW_DUMP_PAYLOAD_SIZE  (GR_CTXSW_DUMP_STAGE_SIZE - sizeof(GR_CTXSW_DUMP_STAGE))

#endif // GR_CTXSW_DUMP_STAGE_H
