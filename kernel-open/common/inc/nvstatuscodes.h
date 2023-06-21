/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef SDK_NVSTATUSCODES_H
#define SDK_NVSTATUSCODES_H

NV_STATUS_CODE(NV_OK,                                  0x00000000, "Success")
NV_STATUS_CODE(NV_ERR_GENERIC,                         0x0000FFFF, "Failure: Generic Error")

NV_STATUS_CODE(NV_ERR_BROKEN_FB,                       0x00000001, "Frame-Buffer broken")
NV_STATUS_CODE(NV_ERR_BUFFER_TOO_SMALL,                0x00000002, "Buffer passed in is too small")
NV_STATUS_CODE(NV_ERR_BUSY_RETRY,                      0x00000003, "System is busy, retry later")
NV_STATUS_CODE(NV_ERR_CALLBACK_NOT_SCHEDULED,          0x00000004, "The requested callback API not scheduled")
NV_STATUS_CODE(NV_ERR_CARD_NOT_PRESENT,                0x00000005, "Card not detected")
NV_STATUS_CODE(NV_ERR_CYCLE_DETECTED,                  0x00000006, "Call cycle detected")
NV_STATUS_CODE(NV_ERR_DMA_IN_USE,                      0x00000007, "Requested DMA is in use")
NV_STATUS_CODE(NV_ERR_DMA_MEM_NOT_LOCKED,              0x00000008, "Requested DMA memory is not locked")
NV_STATUS_CODE(NV_ERR_DMA_MEM_NOT_UNLOCKED,            0x00000009, "Requested DMA memory is not unlocked")
NV_STATUS_CODE(NV_ERR_DUAL_LINK_INUSE,                 0x0000000A, "Dual-Link is in use")
NV_STATUS_CODE(NV_ERR_ECC_ERROR,                       0x0000000B, "Generic ECC error")
NV_STATUS_CODE(NV_ERR_FIFO_BAD_ACCESS,                 0x0000000C, "FIFO: Invalid access")
NV_STATUS_CODE(NV_ERR_FREQ_NOT_SUPPORTED,              0x0000000D, "Requested frequency is not supported")
NV_STATUS_CODE(NV_ERR_GPU_DMA_NOT_INITIALIZED,         0x0000000E, "Requested DMA not initialized")
NV_STATUS_CODE(NV_ERR_GPU_IS_LOST,                     0x0000000F, "GPU lost from the bus")
NV_STATUS_CODE(NV_ERR_GPU_IN_FULLCHIP_RESET,           0x00000010, "GPU currently in full-chip reset")
NV_STATUS_CODE(NV_ERR_GPU_NOT_FULL_POWER,              0x00000011, "GPU not in full power")
NV_STATUS_CODE(NV_ERR_GPU_UUID_NOT_FOUND,              0x00000012, "GPU UUID not found")
NV_STATUS_CODE(NV_ERR_HOT_SWITCH,                      0x00000013, "System in hot switch")
NV_STATUS_CODE(NV_ERR_I2C_ERROR,                       0x00000014, "I2C Error")
NV_STATUS_CODE(NV_ERR_I2C_SPEED_TOO_HIGH,              0x00000015, "I2C Error: Speed too high")
NV_STATUS_CODE(NV_ERR_ILLEGAL_ACTION,                  0x00000016, "Current action is not allowed")
NV_STATUS_CODE(NV_ERR_IN_USE,                          0x00000017, "Generic busy error")
NV_STATUS_CODE(NV_ERR_INFLATE_COMPRESSED_DATA_FAILED,  0x00000018, "Failed to inflate compressed data")
NV_STATUS_CODE(NV_ERR_INSERT_DUPLICATE_NAME,           0x00000019, "Found a duplicate entry in the requested btree")
NV_STATUS_CODE(NV_ERR_INSUFFICIENT_RESOURCES,          0x0000001A, "Ran out of a critical resource, other than memory")
NV_STATUS_CODE(NV_ERR_INSUFFICIENT_PERMISSIONS,        0x0000001B, "The requester does not have sufficient permissions")
NV_STATUS_CODE(NV_ERR_INSUFFICIENT_POWER,              0x0000001C, "Generic Error: Low power")
NV_STATUS_CODE(NV_ERR_INVALID_ACCESS_TYPE,             0x0000001D, "This type of access is not allowed")
NV_STATUS_CODE(NV_ERR_INVALID_ADDRESS,                 0x0000001E, "Address not valid")
NV_STATUS_CODE(NV_ERR_INVALID_ARGUMENT,                0x0000001F, "Invalid argument to call")
NV_STATUS_CODE(NV_ERR_INVALID_BASE,                    0x00000020, "Invalid base")
NV_STATUS_CODE(NV_ERR_INVALID_CHANNEL,                 0x00000021, "Given channel-id not valid")
NV_STATUS_CODE(NV_ERR_INVALID_CLASS,                   0x00000022, "Given class-id not valid")
NV_STATUS_CODE(NV_ERR_INVALID_CLIENT,                  0x00000023, "Given client not valid")
NV_STATUS_CODE(NV_ERR_INVALID_COMMAND,                 0x00000024, "Command passed is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_DATA,                    0x00000025, "Invalid data passed")
NV_STATUS_CODE(NV_ERR_INVALID_DEVICE,                  0x00000026, "Current device is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_DMA_SPECIFIER,           0x00000027, "The requested DMA specifier is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_EVENT,                   0x00000028, "Invalid event occurred")
NV_STATUS_CODE(NV_ERR_INVALID_FLAGS,                   0x00000029, "Invalid flags passed")
NV_STATUS_CODE(NV_ERR_INVALID_FUNCTION,                0x0000002A, "Called function is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_HEAP,                    0x0000002B, "Heap corrupted")
NV_STATUS_CODE(NV_ERR_INVALID_INDEX,                   0x0000002C, "Index invalid")
NV_STATUS_CODE(NV_ERR_INVALID_IRQ_LEVEL,               0x0000002D, "Requested IRQ level is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_LIMIT,                   0x0000002E, "Generic Error: Invalid limit")
NV_STATUS_CODE(NV_ERR_INVALID_LOCK_STATE,              0x0000002F, "Requested lock state not valid")
NV_STATUS_CODE(NV_ERR_INVALID_METHOD,                  0x00000030, "Requested method not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OBJECT,                  0x00000031, "Object not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OBJECT_BUFFER,           0x00000032, "Object buffer passed is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OBJECT_HANDLE,           0x00000033, "Object handle is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OBJECT_NEW,              0x00000034, "New object is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OBJECT_OLD,              0x00000035, "Old object is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OBJECT_PARENT,           0x00000036, "Object parent is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OFFSET,                  0x00000037, "The offset passed is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OPERATION,               0x00000038, "Requested operation is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_OWNER,                   0x00000039, "Owner not valid")
NV_STATUS_CODE(NV_ERR_INVALID_PARAM_STRUCT,            0x0000003A, "Invalid structure parameter")
NV_STATUS_CODE(NV_ERR_INVALID_PARAMETER,               0x0000003B, "At least one of the parameters passed is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_PATH,                    0x0000003C, "The requested path is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_POINTER,                 0x0000003D, "Pointer not valid")
NV_STATUS_CODE(NV_ERR_INVALID_REGISTRY_KEY,            0x0000003E, "Found an invalid registry key")
NV_STATUS_CODE(NV_ERR_INVALID_REQUEST,                 0x0000003F, "Generic Error: Invalid request")
NV_STATUS_CODE(NV_ERR_INVALID_STATE,                   0x00000040, "Generic Error: Invalid state")
NV_STATUS_CODE(NV_ERR_INVALID_STRING_LENGTH,           0x00000041, "The string length is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_READ,                    0x00000042, "The requested read operation is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_WRITE,                   0x00000043, "The requested write operation is not valid")
NV_STATUS_CODE(NV_ERR_INVALID_XLATE,                   0x00000044, "The requested translate operation is not valid")
NV_STATUS_CODE(NV_ERR_IRQ_NOT_FIRING,                  0x00000045, "Requested IRQ is not firing")
NV_STATUS_CODE(NV_ERR_IRQ_EDGE_TRIGGERED,              0x00000046, "IRQ is edge triggered")
NV_STATUS_CODE(NV_ERR_MEMORY_TRAINING_FAILED,          0x00000047, "Failed memory training sequence")
NV_STATUS_CODE(NV_ERR_MISMATCHED_SLAVE,                0x00000048, "Slave mismatch")
NV_STATUS_CODE(NV_ERR_MISMATCHED_TARGET,               0x00000049, "Target mismatch")
NV_STATUS_CODE(NV_ERR_MISSING_TABLE_ENTRY,             0x0000004A, "Requested entry missing not found in the table")
NV_STATUS_CODE(NV_ERR_MODULE_LOAD_FAILED,              0x0000004B, "Failed to load the requested module")
NV_STATUS_CODE(NV_ERR_MORE_DATA_AVAILABLE,             0x0000004C, "There is more data available")
NV_STATUS_CODE(NV_ERR_MORE_PROCESSING_REQUIRED,        0x0000004D, "More processing required for the given call")
NV_STATUS_CODE(NV_ERR_MULTIPLE_MEMORY_TYPES,           0x0000004E, "Multiple memory types found")
NV_STATUS_CODE(NV_ERR_NO_FREE_FIFOS,                   0x0000004F, "No more free FIFOs found")
NV_STATUS_CODE(NV_ERR_NO_INTR_PENDING,                 0x00000050, "No interrupt pending")
NV_STATUS_CODE(NV_ERR_NO_MEMORY,                       0x00000051, "Out of memory")
NV_STATUS_CODE(NV_ERR_NO_SUCH_DOMAIN,                  0x00000052, "Requested domain does not exist")
NV_STATUS_CODE(NV_ERR_NO_VALID_PATH,                   0x00000053, "Caller did not specify a valid path")
NV_STATUS_CODE(NV_ERR_NOT_COMPATIBLE,                  0x00000054, "Generic Error: Incompatible types")
NV_STATUS_CODE(NV_ERR_NOT_READY,                       0x00000055, "Generic Error: Not ready")
NV_STATUS_CODE(NV_ERR_NOT_SUPPORTED,                   0x00000056, "Call not supported")
NV_STATUS_CODE(NV_ERR_OBJECT_NOT_FOUND,                0x00000057, "Requested object not found")
NV_STATUS_CODE(NV_ERR_OBJECT_TYPE_MISMATCH,            0x00000058, "Specified objects do not match")
NV_STATUS_CODE(NV_ERR_OPERATING_SYSTEM,                0x00000059, "Generic operating system error")
NV_STATUS_CODE(NV_ERR_OTHER_DEVICE_FOUND,              0x0000005A, "Found other device instead of the requested one")
NV_STATUS_CODE(NV_ERR_OUT_OF_RANGE,                    0x0000005B, "The specified value is out of bounds")
NV_STATUS_CODE(NV_ERR_OVERLAPPING_UVM_COMMIT,          0x0000005C, "Overlapping unified virtual memory commit")
NV_STATUS_CODE(NV_ERR_PAGE_TABLE_NOT_AVAIL,            0x0000005D, "Requested page table not available")
NV_STATUS_CODE(NV_ERR_PID_NOT_FOUND,                   0x0000005E, "Process-Id not found")
NV_STATUS_CODE(NV_ERR_PROTECTION_FAULT,                0x0000005F, "Protection fault")
NV_STATUS_CODE(NV_ERR_RC_ERROR,                        0x00000060, "Generic RC error")
NV_STATUS_CODE(NV_ERR_REJECTED_VBIOS,                  0x00000061, "Given Video BIOS rejected/invalid")
NV_STATUS_CODE(NV_ERR_RESET_REQUIRED,                  0x00000062, "Reset required")
NV_STATUS_CODE(NV_ERR_STATE_IN_USE,                    0x00000063, "State in use")
NV_STATUS_CODE(NV_ERR_SIGNAL_PENDING,                  0x00000064, "Signal pending")
NV_STATUS_CODE(NV_ERR_TIMEOUT,                         0x00000065, "Call timed out")
NV_STATUS_CODE(NV_ERR_TIMEOUT_RETRY,                   0x00000066, "Call timed out, please retry later")
NV_STATUS_CODE(NV_ERR_TOO_MANY_PRIMARIES,              0x00000067, "Too many primaries")
NV_STATUS_CODE(NV_ERR_UVM_ADDRESS_IN_USE,              0x00000068, "Unified virtual memory requested address already in use")
NV_STATUS_CODE(NV_ERR_MAX_SESSION_LIMIT_REACHED,       0x00000069, "Maximum number of sessions reached")
NV_STATUS_CODE(NV_ERR_LIB_RM_VERSION_MISMATCH,         0x0000006A, "Library version doesn't match driver version")  //Contained within the RMAPI library
NV_STATUS_CODE(NV_ERR_PRIV_SEC_VIOLATION,              0x0000006B, "Priv security violation")
NV_STATUS_CODE(NV_ERR_GPU_IN_DEBUG_MODE,               0x0000006C, "GPU currently in debug mode")
NV_STATUS_CODE(NV_ERR_FEATURE_NOT_ENABLED,             0x0000006D, "Requested Feature functionality is not enabled")
NV_STATUS_CODE(NV_ERR_RESOURCE_LOST,                   0x0000006E, "Requested resource has been destroyed")
NV_STATUS_CODE(NV_ERR_PMU_NOT_READY,                   0x0000006F, "PMU is not ready or has not yet been initialized")
NV_STATUS_CODE(NV_ERR_FLCN_ERROR,                      0x00000070, "Generic falcon assert or halt")
NV_STATUS_CODE(NV_ERR_FATAL_ERROR,                     0x00000071, "Fatal/unrecoverable error")
NV_STATUS_CODE(NV_ERR_MEMORY_ERROR,                    0x00000072, "Generic memory error")
NV_STATUS_CODE(NV_ERR_INVALID_LICENSE,                 0x00000073, "License provided is rejected or invalid")
NV_STATUS_CODE(NV_ERR_NVLINK_INIT_ERROR,               0x00000074, "Nvlink Init Error")
NV_STATUS_CODE(NV_ERR_NVLINK_MINION_ERROR,             0x00000075, "Nvlink Minion Error")
NV_STATUS_CODE(NV_ERR_NVLINK_CLOCK_ERROR,              0x00000076, "Nvlink Clock Error")
NV_STATUS_CODE(NV_ERR_NVLINK_TRAINING_ERROR,           0x00000077, "Nvlink Training Error")
NV_STATUS_CODE(NV_ERR_NVLINK_CONFIGURATION_ERROR,      0x00000078, "Nvlink Configuration Error")
NV_STATUS_CODE(NV_ERR_RISCV_ERROR,                     0x00000079, "Generic RISC-V assert or halt")
NV_STATUS_CODE(NV_ERR_FABRIC_MANAGER_NOT_PRESENT,      0x0000007A, "Fabric Manager is not loaded")
NV_STATUS_CODE(NV_ERR_ALREADY_SIGNALLED,               0x0000007B, "Semaphore Surface value already >= requested wait value")

// Warnings:
NV_STATUS_CODE(NV_WARN_HOT_SWITCH,                     0x00010001, "WARNING Hot switch")
NV_STATUS_CODE(NV_WARN_INCORRECT_PERFMON_DATA,         0x00010002, "WARNING Incorrect performance monitor data")
NV_STATUS_CODE(NV_WARN_MISMATCHED_SLAVE,               0x00010003, "WARNING Slave mismatch")
NV_STATUS_CODE(NV_WARN_MISMATCHED_TARGET,              0x00010004, "WARNING Target mismatch")
NV_STATUS_CODE(NV_WARN_MORE_PROCESSING_REQUIRED,       0x00010005, "WARNING More processing required for the call")
NV_STATUS_CODE(NV_WARN_NOTHING_TO_DO,                  0x00010006, "WARNING Nothing to do")
NV_STATUS_CODE(NV_WARN_NULL_OBJECT,                    0x00010007, "WARNING NULL object found")
NV_STATUS_CODE(NV_WARN_OUT_OF_RANGE,                   0x00010008, "WARNING value out of range")

#endif /* SDK_NVSTATUSCODES_H */
