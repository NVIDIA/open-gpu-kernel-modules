/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef NV_CRASHCAT_H
#define NV_CRASHCAT_H

#include "nvtypes.h"
#include "nvmisc.h"
#include "nvctassert.h"

//
// NVIDIA CrashCat - Crash Reporting And Signaling Helpers for Peregrine
//
// When a crash occurs on a Peregrine core, NVIDIA firmware may report additional data for post-
// mortem analysis of the crash. The protocol is described in greater detail elsewhere, but in
// general is defined to be a multi-producer/single-consumer flow as follows:
//
//  1. Peregrine writes a wayfinder to a set of predefined registers to indicate a crash report is
//     in progress, and its general location (first crash only)
//  2. Peregrine writes crash report data in a tagged format to a circular queue accessible to both
//     reporter and consumer of the crash reports.
//  3. Peregrine completes the wayfinder by updating a put pointer to indicate the crash report is
//     complete.
//  4. Peregrine raises a beacon interrupt to the consumer to signal the presence of a crash report.
//
// This header contains the shared type and bitfield definitions that are common to both producer
// and consumer sides of the CrashCat protocol.
//

#define NV_CRASHCAT_SIGNATURE 0xdead

//
// CrashCat Wayfinder Protocol Versions
//  A new version is created when backward-incompatible changes are made (the wayfinders and queue
//  control cannot be handled by software written for a prior version).
//
//  This version indicates the handling sequence and format of the wayfinder registers, except for
//  the 16-bit signature and (this) 4-bit version number in the L0 wayfinder.
//
typedef enum {
    NV_CRASHCAT_WAYFINDER_VERSION_1    = 0x01,
    NV_CRASHCAT_WAYFINDER_VERSION_LAST = 0x01,
} NV_CRASHCAT_WAYFINDER_VERSION;

//
// CrashCat Packet Format Versions
//  A new version is created when backward-incompatible changes are made (packets cannot be handled
//  by software written for a prior version).
//
//  This version indicates the format of the upper 32 bits of the packet header, and, along with the
//  NV_CRASHCAT_PACKET_TYPE, the format of the payload.
//
//  The lower 32 bits of the packet header, which include the 16-bit signature, (this) 4-bit
//  version number, and 2-bit payload size unit and 10-bit payload size, are not covered by this
//  version number and their format must not change.
//
typedef enum {
    NV_CRASHCAT_PACKET_FORMAT_VERSION_1    = 0x01,
    NV_CRASHCAT_PACKET_FORMAT_VERSION_LAST = 0x01
} NV_CRASHCAT_PACKET_FORMAT_VERSION;

// Utility macro for ensuring the maximum enum value will fit in a DRF bitfield
#define STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(e, bf)  \
    ct_assert(e ## _LAST < NVBIT(DRF_SIZE(bf)))

//
// The below enum definitions are generally unversioned, and so new values must only be added to
// the end, and existing values cannot be changed or removed (except for the _LAST values).
// Note that adding a new value may require a new version of the wayfinder protocol or packet
// formats that use the enum to accommodate a new maximum value.
//

//
// CrashCat Scratch Group Identifier
//  Each enum value represents an ordered set of one or more scratch registers in the Peregrine IP.
//  See NV_CRASHCAT_DEF_SCRATCH_GROUP_V1_REGMAP_TABLE for the canonical list of registers in each
//  scratch group for version 1 of the wayfinder protocol.
//
//  This enum is used by the wayfinder protocol (version 1).
//
typedef enum {
    NV_CRASHCAT_SCRATCH_GROUP_ID_NONE     = 0x0,
    NV_CRASHCAT_SCRATCH_GROUP_ID_A        = 0x1,
    NV_CRASHCAT_SCRATCH_GROUP_ID_B        = 0x2,
    NV_CRASHCAT_SCRATCH_GROUP_ID_C        = 0x3,
    NV_CRASHCAT_SCRATCH_GROUP_ID_D        = 0x4,
    NV_CRASHCAT_SCRATCH_GROUP_ID_E        = 0x5,
    NV_CRASHCAT_SCRATCH_GROUP_ID_F        = 0x6,

    //
    //  Note: NV_CRASHCAT_SCRATCH_GROUP_ID_IMPL_DEF represents a set of registers which are defined
    //        by the implementer instead of the protocol specification - producer and consumer must
    //        agree on this set definition.
    //
    NV_CRASHCAT_SCRATCH_GROUP_ID_IMPL_DEF = 0x7,
    NV_CRASHCAT_SCRATCH_GROUP_ID_LAST     = 0x7
} NV_CRASHCAT_SCRATCH_GROUP_ID;

//
// Canonical CrashCat Scratch Group Register Mappings (V1)
//  This macro defines a designated-initializer table mapping NV_CRASHCAT_SCRATCH_GROUP_ID values to
//  0-terminated arrays of register offsets (relative to the NV_PFALCON register space base offset).
//  This mapping is defined for version 1 of the wayfinder protocol; future versions may use a
//  different mapping.
//
//  This header does not define, or include any header that defines, the register offset macros used
//  in the table entries. The caller should include the appropriate header defining these register
//  offsets before invoking this macro.
//
//  If the implementation intends to use the NV_CRASCHCAT_SCRATCH_GROUP_ID_IMPL_DEF group, it can
//  invoke NV_CRASHCAT_DEF_SCRATCH_GROUP_V1_REGMAP_TABLE_WITH_IMPL_DEF() macro with the list of
//  registers to be used for the IMPL_DEF group (up to 4). Example:
//
//      NV_CRASHCAT_DEF_SCRATCH_GROUP_V1_REGMAP_TABLE_WITH_IMPL_DEF(scratchOffsetTable,
//          NV_PUNIT_REG0, NV_PUNIT_REG1, NV_PUNIT_REG2, NV_PUNIT_REG3);
//

// Maximum number of registers in a scratch group for now
#define NV_CRASHCAT_SCRATCH_GROUP_V1_MAX_NUM_REGISTERS 4

#define NV_CRASHCAT_DEF_SCRATCH_GROUP_V1_REGMAP_TABLE(tblName)                                     \
    NV_CRASHCAT_DEF_SCRATCH_GROUP_V1_REGMAP_TABLE_WITH_IMPL_DEF(tblName, 0)

#define NV_CRASHCAT_DEF_SCRATCH_GROUP_V1_REGMAP_TABLE_WITH_IMPL_DEF(tblName, ...)                  \
    static const NvU32 tblName[][NV_CRASHCAT_SCRATCH_GROUP_V1_MAX_NUM_REGISTERS + 1] = {           \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_NONE] = {0},                                                 \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_A] = {                                                       \
            NV_PFALCON_FALCON_MAILBOX0, NV_PFALCON_FALCON_MAILBOX1, 0 },                           \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_B] = {                                                       \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_0(0),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_0(1),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_0(2),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_0(3), 0},                                       \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_C] = {                                                       \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_1(0),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_1(1),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_1(2),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_1(3), 0},                                       \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_D] = {                                                       \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_2(0),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_2(1),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_2(2),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_2(3), 0},                                       \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_E] = {                                                       \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_3(0),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_3(1),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_3(2),                                           \
            NV_PFALCON_FALCON_COMMON_SCRATCH_GROUP_3(3), 0},                                       \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_F] = {0},                                                    \
        [NV_CRASHCAT_SCRATCH_GROUP_ID_IMPL_DEF] = { __VA_ARGS__, 0 }                               \
    }

//
// CrashCat Memory Aperture Identifier
//  Each enum value represents a target aperture through which a CrashCat memory buffer can be
//  accessed.
//
typedef enum {
    NV_CRASHCAT_MEM_APERTURE_SYSGPA  = 0x0,
    NV_CRASHCAT_MEM_APERTURE_FBGPA   = 0x1,
    NV_CRASHCAT_MEM_APERTURE_DMEM    = 0x2,
    NV_CRASHCAT_MEM_APERTURE_EMEM    = 0x3,
    NV_CRASHCAT_MEM_APERTURE_LAST    = 0x3,
    NV_CRASHCAT_MEM_APERTURE_UNKNOWN = 0xFF, // Used for error checking/translation failures
} NV_CRASHCAT_MEM_APERTURE;

//
// CrashCat Memory Unit Size
//  Each enum value represents a fixed unit size for a corresponding size field. This is used to
//  encode buffer sizes in compact register space.
//
typedef enum {
    NV_CRASHCAT_MEM_UNIT_SIZE_8B    = 0, // 8-byte units
    NV_CRASHCAT_MEM_UNIT_SIZE_1KB   = 1, // 1-kilobyte units
    NV_CRASHCAT_MEM_UNIT_SIZE_4KB   = 2, // 4-kilobyte units
    NV_CRASHCAT_MEM_UNIT_SIZE_64KB  = 3, // 64-kilobyte units
    NV_CRASHCAT_MEM_UNIT_SIZE_LAST  = 3,
} NV_CRASHCAT_MEM_UNIT_SIZE;

//
// CrashCat Packet Type
//  Encoded in the CrashCat packet header to indicate the format of the data.
//
typedef enum {
    NV_CRASHCAT_PACKET_TYPE_REPORT            = 0x00, // Base CrashCat report packet (required)
    NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE = 0x01, // Each 8-byte value is a RISC-V 64-bit CSR
    NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE = 0x02, // Each 8-byte value is a RISC-V 64-bit GPR
    NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE     = 0x03, // Each 8-byte value is a program counter/
                                                      // virtual address from a RISC-V 64-bit trace
    NV_CRASHCAT_PACKET_TYPE_IO32_STATE        = 0x04, // Each 8-byte value is a 32-bit register
                                                      // address in the upper bytes combined with
                                                      // a 32-bit value in the lower bytes
    NV_CRASHCAT_PACKET_TYPE_LAST              = 0x04
} NV_CRASHCAT_PACKET_TYPE;

//
// CrashCat RISC-V Mode
//  Indicates the execution mode of the Peregrine core.
//  Note: this does not include all RISC-V standard modes, only the ones supported by NVRISC-V.
//
typedef enum {
    NV_CRASHCAT_RISCV_MODE_UNSPECIFIED = 0x0,
    NV_CRASHCAT_RISCV_MODE_M           = 0x1,    // Machine Mode
    NV_CRASHCAT_RISCV_MODE_S           = 0x2,    // Supervisor Mode
    NV_CRASHCAT_RISCV_MODE_U           = 0x3,    // User Mode
    NV_CRASHCAT_RISCV_MODE_LAST        = 0x3,
} NV_CRASHCAT_RISCV_MODE;

typedef enum {
   NV_CRASHCAT_CONTAINMENT_UNSPECIFIED  = 0x0,
   NV_CRASHCAT_CONTAINMENT_RISCV_MODE_M = NV_CRASHCAT_RISCV_MODE_M,
   NV_CRASHCAT_CONTAINMENT_RISCV_MODE_S = NV_CRASHCAT_RISCV_MODE_S,
   NV_CRASHCAT_CONTAINMENT_RISCV_MODE_U = NV_CRASHCAT_RISCV_MODE_U,
   NV_CRASHCAT_CONTAINMENT_RISCV_HART   = 0x4,
   NV_CRASHCAT_CONTAINMENT_UNCONTAINED  = 0xF,
   NV_CRASHCAT_CONTAINMENT_LAST         = 0xF
} NV_CRASHCAT_CONTAINMENT;

//
// CrashCat Partition
//  Represents a NVRISC-V microcode partition index
//
typedef NvU8 NvCrashCatNvriscvPartition;
#define NV_CRASHCAT_NVRISCV_PARTITION_UNSPECIFIED   NV_U8_MAX

//
// CrashCat Ucode ID
//  Represents an NVRISC-V microcode ID
//
typedef NvU8 NvCrashCatNvriscvUcodeId;
#define NV_CRASHCAT_NVRISCV_UCODE_ID_UNSPECIFIED    NV_U8_MAX

//
// CrashCat Crash Cause Type
//  Indicates the general nature of the crash cause.
//
typedef enum {
    NV_CRASHCAT_CAUSE_TYPE_EXCEPTION = 0x0,  // Crash observed via Peregrine trap (exception or
                                             // unhandled interrupt)
    NV_CRASHCAT_CAUSE_TYPE_TIMEOUT   = 0x1,  // Crash observed via timeout or hang condition
    NV_CRASHCAT_CAUSE_TYPE_PANIC     = 0x2,  // Crash observed via direct panic condition
    NV_CRASHCAT_CAUSE_TYPE_LAST      = 0x2
} NV_CRASHCAT_CAUSE_TYPE;

//
// CrashCat I/O Aperture Identifier
//  Indicates the Peregrine MMIO aperture through which register offsets are accessed.
//
typedef enum {
    NV_CRASHCAT_IO_APERTURE_NONE    = 0x00, // Register offsets are not relative
    NV_CRASHCAT_IO_APERTURE_INTIO   = 0x01, // Register offsets are relative to local I/O base
    NV_CRASHCAT_IO_APERTURE_EXTIO   = 0x02, // Register offsets are relative to external I/O base
    NV_CRASHCAT_IO_APERTURE_LAST    = 0x02
} NV_CRASHCAT_IO_APERTURE;

//
// CrashCat Trace Type
//  Indicates the source of trace data (PC values)
//
typedef enum {
    NV_CRASHCAT_TRACE_TYPE_STACK  = 0x00, // The PC values are return addresses on a stack, walked
                                          // by CrashCat implementation
    NV_CRASHCAT_TRACE_TYPE_NVRVTB = 0x01, // The PC values are entries from the NVRISC-V PC trace
                                          // buffer
    NV_CRASHCAT_TRACE_TYPE_LAST   = 0x01
} NV_CRASHCAT_TRACE_TYPE;

//
// CrashCat Wayfinder Protocol is a mechanism for locating crash-reports in a programmatic way,
// since available memory for reports may vary across different Peregrines. In V1, the wayfinder
// protocol uses a single common scratch register (level 0, A.K.A. WFL0) to point to a secondary
// group of scratch registers (level 1, A.K.A. WFL1), which point to the full crash report queue.
// The queue is implemented as a circular buffer with classic put/get semantics, controlled through
// the wayfinder L1 registers.
//
// Crash Report Wayfinder Level 0 (NV_CRASHCAT_WAYFINDER_L0)
//  _SIGNATURE          : Initialized to NV_CRASHCAT_SIGNATURE after the level 1 wayfinder is
//                        initialized.
//  _VERSION            : NV_CRASHCAT_WAYFINDER_VERSION value of the protocol implemented for the
//                        crash report wayfinder on this Peregrine (must be consistent with all
//                        implementers on a Peregrine).
//
// Version 1 Fields:
//  _V1_WFL1_LOCATION   : Contains an NV_CRASHCAT_SCRATCH_GROUP_ID identifying the registers
//                        containing the level 1 wayfinder
//  _V1_RESERVED        : Reserved for future use (currently 0).
//
typedef NvU32 NvCrashCatWayfinderL0_V1;
#define NV_CRASHCAT_WAYFINDER_L0_SIGNATURE                  15:0
#define NV_CRASHCAT_WAYFINDER_L0_SIGNATURE_VALID            NV_CRASHCAT_SIGNATURE
#define NV_CRASHCAT_WAYFINDER_L0_VERSION                    19:16
#define NV_CRASHCAT_WAYFINDER_L0_VERSION_1                  NV_CRASHCAT_WAYFINDER_VERSION_1
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION           22:20
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_NONE      NV_CRASHCAT_SCRATCH_GROUP_ID_NONE
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_A         NV_CRASHCAT_SCRATCH_GROUP_ID_A
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_B         NV_CRASHCAT_SCRATCH_GROUP_ID_B
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_C         NV_CRASHCAT_SCRATCH_GROUP_ID_C
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_D         NV_CRASHCAT_SCRATCH_GROUP_ID_D
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_E         NV_CRASHCAT_SCRATCH_GROUP_ID_E
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_F         NV_CRASHCAT_SCRATCH_GROUP_ID_F
#define NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION_IMPL_DEF  NV_CRASHCAT_SCRATCH_GROUP_ID_IMPL_DEF
#define NV_CRASHCAT_WAYFINDER_L0_V1_RESERVED                31:23

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_WAYFINDER_VERSION,
                                    NV_CRASHCAT_WAYFINDER_L0_VERSION);
STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_SCRATCH_GROUP_ID,
                                    NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION);

//
// Crash Report Wayfinder Level 1 and Queue Control Notes
//  Depending on how many scratch registers are in the set specified by the level 0 wayfinder, the
//  registers used for the level 1 wayfinder may need to be reused for the queue control registers.
//
//  The first two scratch registers in the set are used to compose the NvCrashCatWayfinderL1_V1
//  value, with the register with the lower address providing the bottom 32 bits and the register
//  with the higher address providing the upper 32 bits.
//
//  If four scratch registers are available, the last two are used for the queue put and get
//  control, respectively. The producer implementation should ensure these are initialized to zero,
//  and may update the put pointer without any synchronization with the consumer.
//
//  If only two scratch registers are available, the WFL1 registers are reclaimed after they are
//  decoded by the consumer and used for the queue put and get points. The producer must wait for
//  the consumer to set the NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION bits of the level 0 wayfinder
//  to _NONE before writing the put pointer. It is the responsibility of the consumer to clear the
//  WFL1 registers before updating the level 0 wayfinder - after the producer sees the
//  NV_CRASHCAT_WAYFINDER_L0_V1_WFL1_LOCATION change to _NONE, it may update the put pointer.
//
// CrashCat Wayfinder Locking Notes for Implementers
//  Due to the multi-producer nature of the crash report buffer, accesses to the following registers
//  must be synchronized when writing to avoid stomping other crash reports or otherwise corrupting
//  the queue:
//   - NV_CRASHCAT_WAYFINDER_L0
//     If the signature is already present when the reporter initializes, the reporter should panic
//     if the wayfinder protocol version is not the same as what the reporter implements.
//     Where possible, it is recommended to compile-time assert reporter version consistency.
//   - NV_CRASHCAT_WAYFINDER_L1
//     Writes to these registers must be synchronized during initialization by the reporter, to
//     ensure that only one chooses the value and writes it. If they are already initialized, the
//     reporter should not need to update them, and should instead queue its crash reports in the
//     buffer pointed to by these registers.
//   - NV_CRASHCAT_QUEUE_PUT
//     This register must be synchronized on during initialization and update by the reporter. The
//     interface should be locked before the start of writing the crash report and released after
//     this register is updated.
//   - NV_CRASHCAT_QUEUE_GET
//     The (single) consumer controls this register, so no explicit synchronization is needed.
//     The implementation should initialize to 0 when the level 0 wayfinder is initialized, and not
//     touch it afterward.
//
//  If no preemption is possible, then it is sufficient for a producer to push packets into the
//  queue one by one, and only update the put pointer once all packets from the report have been
//  queued. If the producer can be preempted while queuing report packets, it must hold a lock
//  synchronizing access to the CrashCat queue while it it pushes all report packets in the queue,
//  to prevent potential interleaving with packets from other reports.
//
//  It may be advantageous for the Peregrine FMC to receive the report queue location as a boot
//  argument and initialize the wayfinders accordingly during boot, rather than when a crash is
//  observed.
//

//
// Crash Report Wayfinder Level 1 (NV_CRASHCAT_WAYFINDER_L1) V1
//  _QUEUE_APERTURE   : NV_CRASHCAT_MEM_APERTURE value of the aperture through which the queue can
//                      be accessed
//  _QUEUE_UNIT_SIZE  : NV_CRASHCAT_MEM_UNIT_SIZE value indicating the units of the _SIZE field
//                      (1KB or greater)
//  _RESERVED         : Reserved for future use (currently 0)
//  _QUEUE_SIZE       : Size of the queue in _UNIT_SIZE minus 1 (_SIZE = 0 -> queue size is 1 unit)
//  _QUEUE_OFFSET_1KB : 1KB-aligned offset of the start of the queue in _QUEUE_APERTURE
//
typedef NvU64 NvCrashCatWayfinderL1_V1;
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_APERTURE          2:0
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_APERTURE_SYSGPA   NV_CRASHCAT_MEM_APERTURE_SYSGPA
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_APERTURE_FBGPA    NV_CRASHCAT_MEM_APERTURE_FBGPA
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_APERTURE_DMEM     NV_CRASHCAT_MEM_APERTURE_DMEM
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_APERTURE_EMEM     NV_CRASHCAT_MEM_APERTURE_EMEM
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_UNIT_SIZE         4:3
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_UNIT_SIZE_1KB     NV_CRASHCAT_MEM_UNIT_SIZE_1KB
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_UNIT_SIZE_4KB     NV_CRASHCAT_MEM_UNIT_SIZE_4KB
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_UNIT_SIZE_64KB    NV_CRASHCAT_MEM_UNIT_SIZE_64KB
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_RESERVED          5:5
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_SIZE              9:6
#define NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_OFFSET_1KB        63:10

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_MEM_APERTURE,
                                    NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_APERTURE);
STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_MEM_UNIT_SIZE,
                                    NV_CRASHCAT_WAYFINDER_L1_V1_QUEUE_UNIT_SIZE);

//
// CrashCat Queue Put Pointer
//  Offset in bytes into the CrashCat circular queue at which the next crash report will be written.
//  Reports may wrap around the end of the buffer to the start.
//
//  The implementation should only update the put pointer once all packets from the report have been
//  queued. This simplifies the consumer implementation, as it can assume that the report is
//  complete once the put pointer is updated.
//

//
// CrashCat Queue Get Pointer
//  Offset in bytes into the CrashCat circular queue at which the next crash report will be read by
//  the consumer (when get is behind put). The consumer advances this pointer to allow queue memory
//  to be reused by subsequent reports.
//

static NV_INLINE NvU32 crashcatQueueFreeBytes(NvU32 put, NvU32 get, NvU32 size)
{
    return (put >= get) ? (size - (put - get)) : (get - put);
}

static NV_INLINE NvU32 crashcatQueueBytesToRead(NvU32 put, NvU32 get, NvU32 size)
{
    return (put >= get) ? (put - get) : (size - (get - put));
}

//
// CrashCat Packet Header (NV_CRASHCAT_PACKET_HEADER)
//  _SIGNATURE            : NV_CRASHCAT_SIGNATURE value to indicate the start of a new data value
//  _FORMAT_VERSION       : NV_CRASHCAT_PACKET_FORMAT_VERSION value
//  _PAYLOAD_UNIT_SIZE    : NV_CRASHCAT_MEM_UNIT_SIZE value indicating the units of the
//                          _PAYLOAD_SIZE field
//  _PAYLOAD_SIZE         : Size of the packet payload (excluding header) in _PAYLOAD_UNIT_SIZE
//                          minus 1 (_PAYLOAD_SIZE = 0 -> payload size is 1 unit)
//  _V1_TYPE              : NV_CRASHCAT_PACKET_TYPE value
//  _V1_META              : Additional packet metadata bits specific to the packet type
//
typedef NvU64 NvCrashCatPacketHeader;
typedef NvU64 NvCrashCatPacketHeader_V1;
#define NV_CRASHCAT_PACKET_HEADER_SIGNATURE                 15:0
#define NV_CRASHCAT_PACKET_HEADER_SIGNATURE_VALID           NV_CRASHCAT_SIGNATURE
#define NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION            19:16
#define NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION_1          NV_CRASHCAT_PACKET_FORMAT_VERSION_1
#define NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE         21:20
#define NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_8B      NV_CRASHCAT_MEM_UNIT_SIZE_8B
#define NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_1KB     NV_CRASHCAT_MEM_UNIT_SIZE_1KB
#define NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_4KB     NV_CRASHCAT_MEM_UNIT_SIZE_4KB
#define NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_64PKB    NV_CRASHCAT_MEM_UNIT_SIZE_64KB
#define NV_CRASHCAT_PACKET_HEADER_PAYLOAD_SIZE              31:22
#define NV_CRASHCAT_PACKET_HEADER_V1_TYPE                   39:32
#define NV_CRASHCAT_PACKET_HEADER_V1_TYPE_REPORT            NV_CRASHCAT_PACKET_TYPE_REPORT
#define NV_CRASHCAT_PACKET_HEADER_V1_TYPE_RISCV64_CSR_STATE \
        NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE
#define NV_CRASHCAT_PACKET_HEADER_V1_TYPE_RISCV64_GPR_STATE \
        NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE
#define NV_CRASHCAT_PACKET_HEADER_V1_TYPE_IO32_STATE        NV_CRASHCAT_PACKET_TYPE_IO32_STATE
#define NV_CRASHCAT_PACKET_HEADER_V1_TYPE_RISCV64_TRACE     NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE
#define NV_CRASHCAT_PACKET_HEADER_V1_META                   63:40

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_PACKET_FORMAT_VERSION,
                                    NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION);
STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_MEM_UNIT_SIZE,
                                    NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE);
STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_PACKET_TYPE,
                                    NV_CRASHCAT_PACKET_HEADER_V1_TYPE);

//
// CrashCat Report (NV_CRASHCAT_PACKET_TYPE_REPORT) V1
//  A fixed-size packet including
//      8  bytes: packet header (see NV_CRASHCAT_PACKET_HEADER)
//      8  bytes: implementer signature
//      16 bytes: reporter information
//      32 bytes: crash source information
//
// A report packet sets the context for the remaining packets that come after it (until the next
// NV_CRASHCAT_PACKET_TYPE_REPORT packet).
//
typedef struct NvCrashCatReport_V1 {
    //
    // CrashCat Report V1 Header (NV_CRASHCAT_REPORT_V1_HEADER)
    //  _SIGNATURE            : NV_CRASHCAT_SIGNATURE value to indicate the start of a new packet
    //  _FORMAT_VERSION       : NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    //  _PAYLOAD_UNIT_SIZE    : NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_8B
    //  _PAYLOAD_SIZE         : 6 (56 bytes)
    //  _TYPE                 : NV_CRASHCAT_PACKET_TYPE_REPORT value
    //  _RESERVED             : Reserved for future use (currently 0)
    //
    NvCrashCatPacketHeader_V1 header;
    #define NV_CRASHCAT_REPORT_V1_HEADER_SIGNATURE                  \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE
    #define NV_CRASHCAT_REPORT_V1_HEADER_SIGNATURE_VALID            \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE_VALID
    #define NV_CRASHCAT_REPORT_V1_HEADER_FORMAT_VERSION             \
                NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION
    #define NV_CRASHCAT_REPORT_V1_HEADER_FORMAT_VERSION_VALID       \
                NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    #define NV_CRASHCAT_REPORT_V1_HEADER_PAYLOAD_UNIT_SIZE          \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE
    #define NV_CRASHCAT_REPORT_V1_HEADER_PAYLOAD_UNIT_SIZE_VALID    \
                NV_CRASHCAT_MEM_UNIT_SIZE_8B
    #define NV_CRASHCAT_REPORT_V1_HEADER_PAYLOAD_SIZE               \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_SIZE
    #define NV_CRASHCAT_REPORT_V1_HEADER_PAYLOAD_SIZE_VALID         \
                (((sizeof(NvCrashCatReport_V1) - sizeof(NvCrashCatPacketHeader_V1)) >> 3) - 1)
    #define NV_CRASHCAT_REPORT_V1_HEADER_PACKET_TYPE                \
                NV_CRASHCAT_PACKET_HEADER_V1_TYPE
    #define NV_CRASHCAT_REPORT_V1_HEADER_PACKET_TYPE_VALID          \
                NV_CRASHCAT_PACKET_TYPE_REPORT
    #define NV_CRASHCAT_REPORT_V1_HEADER_RESERVED                   63:40

    //
    // CrashCat Report V1 Implementer Signature
    //  Provides a unique 64-bit identifier for the decoder to use to interpret the
    //  implementation-defined bits
    //
    NvU64 implementerSignature;

    //
    // CrashCat Report V1 Reporter ID (NV_CRASHCAT_REPORT_V1_REPORTER_ID)
    //  _NVRISCV_PARTITION  : Partition index of the crash reporter (depends on FMC configuration)
    //  _NVRISCV_UCODE_ID   : Ucode ID of the crash reporter (read from the relevant curruid
    //                        CSR/field)
    //  _RISCV_MODE         : Current RISC-V mode of the crash reporter
    //  _IMPL_DEF           : Implementation-defined identifier
    //
    NvU64 reporterId;
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_NVRISCV_PARTITION              7:0
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_NVRISCV_PARTITION_UNSPECIFIED  \
                NV_CRASHCAT_NVRISCV_PARTITION_UNSPECIFIED
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_NVRISCV_UCODE_ID               15:8
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_NVRISCV_UCODE_ID_UNSPECIFIED   \
                NV_CRASHCAT_NVRISCV_UCODE_ID_UNSPECIFIED
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_RISCV_MODE                     18:16
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_RISCV_MODE_M                   \
                NV_CRASHCAT_RISCV_MODE_M
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_RISCV_MODE_S                   \
                NV_CRASHCAT_RISCV_MODE_S
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_RISCV_MODE_U                   \
                NV_CRASHCAT_RISCV_MODE_U
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_RISCV_MODE_UNSPECIFIED         \
                NV_CRASHCAT_RISCV_MODE_UNSPECIFIED
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_RESERVED                       23:19
    #define NV_CRASHCAT_REPORT_V1_REPORTER_ID_IMPL_DEF                       63:24

    //
    // CrashCat Report V1 Reporter Data (NV_CRASHCAT_REPORT_V1_REPORTER_DATA)
    //  _VERSION   : Implementation-defined version identifier (recommend CL number)
    //  _TIMESTAMP : Seconds since epoch (Jan 1, 1970) or cold reset of when the crash report was
    //               generated. Since this value is read from a local clock source, the consumer
    //               is responsible for adjusting this value to a relevant reference point.
    //
    NvU64 reporterData;
    #define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION         31:0
    #define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_TIMESTAMP       63:32

    //
    // CrashCat Report V1 Source ID (NV_CRASHCAT_REPORT_V1_SOURCE_ID)
    //  _NVRISCV_PARTITION  : Partition ID of the crashing code (depends on FMC configuration)
    //  _NVRISCV_UCODE_ID   : Ucode ID of the crashing code (read from the relevant curruid
    //                        CSR/field)
    //  _RISCV_MODE         : RISC-V mode of the crashing code
    //  _IMPL_DEF           : Implementation-defined identifier
    //
    NvU64 sourceId;
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_NVRISCV_PARTITION             7:0
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_NVRISCV_PARTITION_UNSPECIFIED \
                NV_CRASHCAT_NVRISCV_PARTITION_UNSPECIFIED
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_NVRISCV_UCODE_ID              15:8
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_NVRISCV_UCODE_ID_UNSPECIFIED  \
                NV_CRASHCAT_NVRISCV_UCODE_ID_UNSPECIFIED
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_RISCV_MODE                    18:16
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_RISCV_MODE_M                  NV_CRASHCAT_RISCV_MODE_M
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_RISCV_MODE_S                  NV_CRASHCAT_RISCV_MODE_S
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_RISCV_MODE_U                  NV_CRASHCAT_RISCV_MODE_U
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_RISCV_MODE_UNSPECIFIED        \
                NV_CRASHCAT_RISCV_MODE_UNSPECIFIED
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_RESERVED                      23:19
    #define NV_CRASHCAT_REPORT_V1_SOURCE_ID_IMPL_DEF                      63:24

    //
    // CrashCat Report V1 Source Cause (NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE)
    //  _TYPE    : CrashCat general failure type for the crash (i.e., how the crash was observed)
    //  _RESERVED: Reserved for future use (currently 0)
    //  _IMPL_DEF: Implementation-defined reason code for the crash
    //
    NvU64 sourceCause;
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_TYPE             3:0
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_TYPE_EXCEPTION   NV_CRASHCAT_CAUSE_TYPE_EXCEPTION
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_TYPE_TIMEOUT     NV_CRASHCAT_CAUSE_TYPE_TIMEOUT
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_TYPE_PANIC       NV_CRASHCAT_CAUSE_TYPE_PANIC
    
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_CONTAINMENT      7:4
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_CONTAINMENT_UNSPECIFIED  \
               NV_CRASHCAT_CONTAINMENT_UNSPECIFIED
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_CONTAINMENT_RISCV_MODE_M \
               NV_CRASHCAT_CONTAINMENT_RISCV_MODE_M 
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_CONTAINMENT_RISCV_MODE_S \
               NV_CRASHCAT_CONTAINMENT_RISCV_MODE_S
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_CONTAINMENT_RISCV_MODE_U \
               NV_CRASHCAT_CONTAINMENT_RISCV_MODE_U
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_CONTAINMENT_RISCV_HART \
               NV_CRASHCAT_CONTAINMENT_RISCV_HART
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_CONTAINMENT_UNCONTAINED \
               NV_CRASHCAT_CONTAINMENT_UNCONTAINED  

    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_RESERVED         31:8
    #define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_IMPL_DEF         63:32

    //
    // CrashCat Report V1 Source PC
    //  Program counter of the instruction where the crash occurred
    //
    NvU64 sourcePc;

    //
    // CrashCat Report V1 Source Data
    //  Additional crash source data (implementation-defined)
    //
    NvU64 sourceData;
} NvCrashCatReport_V1;

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_RISCV_MODE,
                                    NV_CRASHCAT_REPORT_V1_REPORTER_ID_RISCV_MODE);
STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_RISCV_MODE,
                                    NV_CRASHCAT_REPORT_V1_SOURCE_ID_RISCV_MODE);
STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_CAUSE_TYPE,
                                    NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_TYPE);

//
// CrashCat RISC-V CSR State (NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE) V1
//  A fixed-size packet containing values for RISC-V Control and Status Registers (CSRs) that are
//  commonly relevant in debugging crashes.
//
//  Note: all CSRs in this structure must be defined in the standard RISC-V specification.
//        Do not add NVRISC-V-specific CSRs to this packet.
//
typedef struct NvCrashCatRiscv64CsrState_V1 {
    //
    // CrashCat RISC-V CSR State Header (NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER)
    //  _SIGNATURE            : NV_CRASHCAT_SIGNATURE value to indicate the start of a new packet
    //  _FORMAT_VERSION       : NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    //  _PAYLOAD_UNIT_SIZE    : NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_8B
    //  _PAYLOAD_SIZE         : 6 (56 bytes)
    //  _TYPE                 : NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE
    //  _RISCV_MODE           : NV_CRASHCAT_RISCV_MODE that indicates the RISC-V mode in which the
    //                          CSR values are captured from
    //  _RESERVED             : Reserved for future use (currently 0)
    //
    NvCrashCatPacketHeader_V1 header;
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_SIGNATURE               \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_SIGNATURE_VALID         \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE_VALID
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_FORMAT_VERSION          \
                NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_FORMAT_VERSION_VALID    \
                NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_PAYLOAD_UNIT_SIZE       \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_PAYLOAD_UNIT_SIZE_VALID \
                NV_CRASHCAT_MEM_UNIT_SIZE_8B
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_PAYLOAD_SIZE            \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_SIZE
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_PAYLOAD_SIZE_VALID      \
                (((sizeof(NvCrashCatRiscv64CsrState_V1) - sizeof(NvCrashCatPacketHeader_V1)) >> 3) \
                    - 1)
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_PACKET_TYPE             \
                NV_CRASHCAT_PACKET_HEADER_V1_TYPE
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_PACKET_TYPE_VALID       \
                NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_RISCV_MODE              42:40
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_RISCV_MODE_M            NV_CRASHCAT_RISCV_MODE_M
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_RISCV_MODE_S            NV_CRASHCAT_RISCV_MODE_S
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_RISCV_MODE_U            NV_CRASHCAT_RISCV_MODE_U
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_RISCV_MODE_UNSPECIFIED  \
                NV_CRASHCAT_RISCV_MODE_UNSPECIFIED
    #define NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_RESERVED                63:43

    NvU64 xstatus;  // mstatus or sstatus
    NvU64 xie;      // mie or sie
    NvU64 xip;      // mip or sip
    NvU64 xepc;     // mepc or sepc
    NvU64 xtval;    // mbadaddr, mtval or stval
    NvU64 xcause;   // mcause or scause
    NvU64 xscratch; // mscratch or sscratch
} NvCrashCatRiscv64CsrState_V1;

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_RISCV_MODE,
                                    NV_CRASHCAT_RISCV64_CSR_STATE_V1_HEADER_RISCV_MODE);

//
// CrashCat RISC-V GPR State (NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE) V1
//  A fixed-size packet containing values for RISC-V general purpose registers (GPRs).
//
//  These are defined to match the RISC-V standard calling convention for x1-x31.
//  x0 is hardwired to 0, so we don't include it in dumps, and the packet header takes its place.
//
typedef struct NvCrashCatRiscv64GprState_V1 {
    //
    // CrashCat RISC-V GPR State Header (NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER)
    //  _SIGNATURE            : NV_CRASHCAT_SIGNATURE value to indicate the start of a new packet
    //  _FORMAT_VERSION       : NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    //  _PAYLOAD_UNIT_SIZE    : NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_8B
    //  _PAYLOAD_SIZE         : 30 (188 bytes)
    //  _TYPE                 : NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE
    //  _RESERVED             : Reserved for future use (currently 0)
    //
    NvCrashCatPacketHeader_V1 header;
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_SIGNATURE               \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_SIGNATURE_VALID         \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE_VALID
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_FORMAT_VERSION          \
                NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_FORMAT_VERSION_VALID    \
                NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_PAYLOAD_UNIT_SIZE       \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_PAYLOAD_UNIT_SIZE_VALID \
                NV_CRASHCAT_MEM_UNIT_SIZE_8B
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_PAYLOAD_SIZE            \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_SIZE
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_PAYLOAD_SIZE_VALID      \
                (((sizeof(NvCrashCatRiscv64GprState_V1) - sizeof(NvCrashCatPacketHeader_V1)) >> 3) \
                    - 1)
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_PACKET_TYPE             \
                NV_CRASHCAT_PACKET_HEADER_V1_TYPE
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_PACKET_TYPE_VALID       \
                NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_RISCV_MODE              42:40
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_RISCV_MODE_M            NV_CRASHCAT_RISCV_MODE_M
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_RISCV_MODE_S            NV_CRASHCAT_RISCV_MODE_S
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_RISCV_MODE_U            NV_CRASHCAT_RISCV_MODE_U
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_RISCV_MODE_UNSPECIFIED  \
                NV_CRASHCAT_RISCV_MODE_UNSPECIFIED
    #define NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_RESERVED                63:40

    NvU64 ra;   // Return address
    NvU64 sp;   // Stack pointer
    NvU64 gp;   // Global pointer
    NvU64 tp;   // Thread pointer
    NvU64 t0;   // Temporary register 0
    NvU64 t1;   // Temporary register 1
    NvU64 t2;   // Temporary register 2
    NvU64 s0;   // Saved register 0
    NvU64 s1;   // Saved register 1
    NvU64 a0;   // Argument/return value register 0
    NvU64 a1;   // Argument/return value register 1
    NvU64 a2;   // Argument register 2
    NvU64 a3;   // Argument register 3
    NvU64 a4;   // Argument register 4
    NvU64 a5;   // Argument register 5
    NvU64 a6;   // Argument register 6
    NvU64 a7;   // Argument register 7
    NvU64 s2;   // Saved register 2
    NvU64 s3;   // Saved register 3
    NvU64 s4;   // Saved register 4
    NvU64 s5;   // Saved register 5
    NvU64 s6;   // Saved register 6
    NvU64 s7;   // Saved register 7
    NvU64 s8;   // Saved register 8
    NvU64 s9;   // Saved register 9
    NvU64 s10;  // Saved register 10
    NvU64 s11;  // Saved register 11
    NvU64 t3;   // Temporary register 3
    NvU64 t4;   // Temporary register 4
    NvU64 t5;   // Temporary register 5
    NvU64 t6;   // Temporary register 6
} NvCrashCatRiscv64GprState_V1;

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_RISCV_MODE,
                                    NV_CRASHCAT_RISCV64_GPR_STATE_V1_HEADER_RISCV_MODE);

//
// CrashCat Trace (NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE) V1
//  A variable-size packet in which each 64-bit payload value is a virtual address from a trace
//  (such as from a stack or PC trace buffer). The packet header metadata includes details to help
//  differentiate types of traces.
//
typedef struct NvCrashCatRiscv64Trace_V1 {
    //
    // CrashCat Stack Trace Header (NV_CRASHCAT_RISCV64_TRACE_V1_HEADER)
    //  _SIGNATURE            : NV_CRASHCAT_SIGNATURE value to indicate the start of a new packet
    //  _FORMAT_VERSION       : NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    //  _PAYLOAD_UNIT_SIZE    : NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_8B
    //  _PAYLOAD_SIZE         : Variable
    //  _TYPE                 : NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE
    //  _RISCV_MODE           : The NV_CRASHCAT_RISCV_MODE context of the trace (e.g., the RISC-V
    //                          mode in which the trace addresses are relevant in)
    //  _RESERVED             : Reserved for future use (currently 0)
    //
    NvCrashCatPacketHeader_V1 header;
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_SIGNATURE              \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_SIGNATURE_VALID        \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE_VALID
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_FORMAT_VERSION         \
                NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_FORMAT_VERSION_VALID   \
                NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_PAYLOAD_UNIT_SIZE      \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_PAYLOAD_SIZE           \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_SIZE
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_PACKET_TYPE            \
                NV_CRASHCAT_PACKET_HEADER_V1_TYPE
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_PACKET_TYPE_VALID      \
                NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_RISCV_MODE             42:40
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_RISCV_MODE_M           NV_CRASHCAT_RISCV_MODE_M
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_RISCV_MODE_S           NV_CRASHCAT_RISCV_MODE_S
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_RISCV_MODE_U           NV_CRASHCAT_RISCV_MODE_U
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_RISCV_MODE_UNSPECIFIED \
                NV_CRASHCAT_RISCV_MODE_UNSPECIFIED
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_TRACE_TYPE             43:43
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_TRACE_TYPE_STACK       NV_CRASHCAT_TRACE_TYPE_STACK
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_TRACE_TYPE_NVRVTB      NV_CRASHCAT_TRACE_TYPE_NVRVTB
    #define NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_RESERVED               63:44

    NvU64 addr[];
} NvCrashCatRiscv64Trace_V1;

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_RISCV_MODE,
                                    NV_CRASHCAT_RISCV64_TRACE_V1_HEADER_RISCV_MODE);

//
// CrashCat 32-bit I/O State (NV_CRASHCAT_PACKET_TYPE_IO32_STATE) V1
//  A variable-size packet in which a 32-bit address and a 32-bit value are encoded into each 64-bit
//  payload value.
//
typedef struct NvCrashCatIo32State_V1 {
    //
    // CrashCat 32-bit I/O Trace Header (NV_CRASHCAT_IO32_STATE_V1_HEADER)
    //  _SIGNATURE            : NV_CRASHCAT_SIGNATURE value to indicate the start of a new packet
    //  _FORMAT_VERSION       : NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    //  _PAYLOAD_UNIT_SIZE    : NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE_8B
    //  _PAYLOAD_SIZE         : Variable
    //  _TYPE                 : NV_CRASHCAT_PACKET_TYPE_IO32_STATE
    //  _APERTURE             : NV_CRASHCAT_IO_APERTURE value identifying the aperture that the
    //                          offset is relative to
    //  _RESERVED             : Reserved for future use (currently 0)
    //
    NvCrashCatPacketHeader_V1 header;
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_SIGNATURE              \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_SIGNATURE_VALID        \
                NV_CRASHCAT_PACKET_HEADER_SIGNATURE_VALID
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_FORMAT_VERSION         \
                NV_CRASHCAT_PACKET_HEADER_FORMAT_VERSION
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_FORMAT_VERSION_VALID   \
                NV_CRASHCAT_PACKET_FORMAT_VERSION_1
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_PAYLOAD_UNIT_SIZE      \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_UNIT_SIZE
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_PAYLOAD_SIZE           \
                NV_CRASHCAT_PACKET_HEADER_PAYLOAD_SIZE
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_PACKET_TYPE            \
                NV_CRASHCAT_PACKET_HEADER_V1_TYPE
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_PACKET_TYPE_VALID      \
                NV_CRASHCAT_PACKET_TYPE_IO32_STATE
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_APERTURE               41:40
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_APERTURE_NONE          NV_CRASHCAT_IO_APERTURE_NONE
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_APERTURE_INTIO         NV_CRASHCAT_IO_APERTURE_INTIO
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_APERTURE_EXTIO         NV_CRASHCAT_IO_APERTURE_EXTIO
    #define NV_CRASHCAT_IO32_STATE_V1_HEADER_RESERVED               63:42

    struct {
        NvU32 value;
        NvU32 offset;
    } regs[];
} NvCrashCatIo32State_V1;

STATIC_ASSERT_ENUM_FITS_IN_BITFIELD(NV_CRASHCAT_IO_APERTURE,
                                    NV_CRASHCAT_IO32_STATE_V1_HEADER_APERTURE);

#endif  // NV_CRASHCAT_H
