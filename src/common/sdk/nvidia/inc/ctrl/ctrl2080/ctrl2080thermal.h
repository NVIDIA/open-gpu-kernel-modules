/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080thermal.finn
//



/*
 * Thermal System rmcontrol api versioning
 */
#define THERMAL_SYSTEM_API_VER                                       1U
#define THERMAL_SYSTEM_API_REV                                       0U

/*
 * NV2080_CTRL_THERMAL_SYSTEM constants
 *
 */

/*
 * NV2080_CTRL_THERMAL_SYSTEM_TARGET
 *
 *  Targets (ie the things the thermal system can observe). Target mask
 *  have to be in sync with corresponding element of NVAPI_THERMAL_TARGET
 *  enum, until there is a translation layer between these two.
 *
 *   NV2080_CTRL_THERMAL_SYSTEM_TARGET_NONE
 *       There is no target.
 *
 *   NV2080_CTRL_THERMAL_SYSTEM_TARGET_GPU
 *       The GPU is the target.
 *
 *   NV2080_CTRL_THERMAL_SYSTEM_TARGET_MEMORY
 *       The memory is the target.
 *
 *   NV2080_CTRL_THERMAL_SYSTEM_TARGET_POWER_SUPPLY
 *       The power supply is the target.
 *
 *   NV2080_CTRL_THERMAL_SYSTEM_TARGET_BOARD
 *       The board (PCB) is the target.
 */


 /*   NV2080_CTRL_THERMAL_SYSTEM_TARGET_UNKNOWN
 *       The target is unknown.
 */
#define NV2080_CTRL_THERMAL_SYSTEM_TARGET_NONE                       (0x00000000U)
#define NV2080_CTRL_THERMAL_SYSTEM_TARGET_GPU                        (0x00000001U)
#define NV2080_CTRL_THERMAL_SYSTEM_TARGET_MEMORY                     (0x00000002U)
#define NV2080_CTRL_THERMAL_SYSTEM_TARGET_POWER_SUPPLY               (0x00000004U)
#define NV2080_CTRL_THERMAL_SYSTEM_TARGET_BOARD                      (0x00000008U)


#define NV2080_CTRL_THERMAL_SYSTEM_TARGET_UNKNOWN                    (0xFFFFFFFFU)

/*
 * executeFlags values
 */
#define NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_FLAGS_DEFAULT             (0x00000000U)
#define NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_FLAGS_IGNORE_FAIL         (0x00000001U)


/*
 * NV2080_CTRL_CMD_THERMAL_SYSTEM_EXECUTE_V2
 *
 * This command will execute a list of thermal system instructions:
 *
 *   clientAPIVersion
 *       This field must be set by the client to THERMAL_SYSTEM_API_VER,
 *       which allows the driver to determine api compatibility.
 *
 *   clientAPIRevision
 *       This field must be set by the client to THERMAL_SYSTEM_API_REV,
 *       which allows the driver to determine api compatibility.
 *
 *   clientInstructionSizeOf
 *       This field must be set by the client to
 *       sizeof(NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION), which allows the
 *       driver to determine api compatibility.
 *
 *   executeFlags
 *       This field is set by the client to control instruction execution.
 *        NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_FLAGS_DEFAULT
 *         Execute instructions normally. The first instruction
 *         failure will cause execution to stop.
 *        NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_FLAGS_IGNORE_FAIL
 *         Execute all instructions, ignoring individual instruction failures.
 *
 *   successfulInstructions
 *       This field is set by the driver and is the number of instructions
 *       that returned NV_OK on execution.  If this field
 *       matches instructionListSize, all instructions executed successfully.
 *
 *   instructionListSize
 *       This field is set by the client to the number of instructions in
 *       instruction list.
 *
 *   instructionList
 *       This field is set an array of thermal system instructions
 *       (NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION) to execute, filled in by the
 *       client.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV2080_CTRL_CMD_THERMAL_SYSTEM_EXECUTE_V2                    (0x20800513U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_THERMAL_INTERFACE_ID << 8) | NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_THERMAL_SYSTEM_EXECUTE_V2_PHYSICAL           (0x20808513U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_THERMAL_LEGACY_NON_PRIVILEGED_INTERFACE_ID << 8) | NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO instructions...
 *
 */

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGETS_AVAILABLE instruction
 *
 * Get the number of available targets.
 *
 *   availableTargets
 *       Returns the number of available targets.  Targets are
 *       identified by an index, starting with 0.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGETS_AVAILABLE_OPCODE (0x00000100U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGETS_AVAILABLE_OPERANDS {
    NvU32 availableTargets;
} NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGETS_AVAILABLE_OPERANDS;

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE instruction
 *
 * Get a target's type.
 *
 *   targetIndex
 *       Set by the client to the desired target index.
 *
 *   type
 *       Returns a target's type.
 *       Possible values returned are:
 *          NV2080_CTRL_THERMAL_SYSTEM_TARGET_NONE
 *          NV2080_CTRL_THERMAL_SYSTEM_TARGET_GPU
 *          NV2080_CTRL_THERMAL_SYSTEM_TARGET_MEMORY
 *          NV2080_CTRL_THERMAL_SYSTEM_TARGET_POWER_SUPPLY
 *          NV2080_CTRL_THERMAL_SYSTEM_TARGET_BOARD
 *          NV2080_CTRL_THERMAL_SYSTEM_TARGET_UNKNOWN
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE_OPCODE (0x00000101U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE_OPERANDS {
    NvU32 targetIndex;
    NvU32 type;
} NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE_OPERANDS;

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_PROVIDER_TYPE instruction
 *
 * Get a providers's type.
 *
 *   providerIndex
 *       Set by the client to the desired provider index.
 *
 *   type
 *       Returns a provider's type.
 */


 /*
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_PROVIDER_TYPE_OPCODE (0x00000301U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_PROVIDER_TYPE_OPERANDS {
    NvU32 providerIndex;
    NvU32 type;
} NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_PROVIDER_TYPE_OPERANDS;

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE instruction
 *
 * Get the number of available sensors.
 *
 *   availableSensors
 *       Returns the number of available sensors.  Sensors are
 *       identified by an index, starting with 0.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE_OPCODE (0x00000500U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE_OPERANDS {
    NvU32 availableSensors;
} NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE_OPERANDS;

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_PROVIDER instruction
 *
 * Get a sensor's provider index.
 *
 *   sensorIndex
 *       Set by the client to the desired sensor index.
 *
 *   providerIndex
 *       Returns a sensor's provider index.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_PROVIDER_OPCODE (0x00000510U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_PROVIDER_OPERANDS {
    NvU32 sensorIndex;
    NvU32 providerIndex;
} NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_PROVIDER_OPERANDS;

/*!
 * Union of mode-specific arguments.
 */


/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING instruction
 *
 * Get a sensor's current reading.
 *
 *   sensorIndex
 *       Set by the client to the desired sensor index.
 *
 *   value
 *       Returns a sensor's current reading.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING_OPCODE (0x00001500U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING_OPERANDS {
    NvU32 sensorIndex;
    NvS32 value;
} NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING_OPERANDS;

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET instruction
 *
 * Get a sensor's target index.
 *
 *   sensorIndex
 *       Set by the client to the desired sensor index.
 *
 *   targetIndex
 *       Returns a sensor's target index.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET_OPCODE (0x00000520U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET_OPERANDS {
    NvU32 sensorIndex;
    NvU32 targetIndex;
} NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET_OPERANDS;

/*
 * NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE instruction
 *
 * Get a sensor's readings range (ie min, max).
 *
 *   sensorIndex
 *       Set by the client to the desired sensor index.
 *
 *   minimum
 *       Returns a sensor's range minimum.
 *
 *   maximum
 *       Returns a sensor's range maximum.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE_OPCODE (0x00000540U)
typedef struct NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE_OPERANDS {
    NvU32 sensorIndex;
    NvS32 minimum;
    NvS32 maximum;
} NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE_OPERANDS;

/*
 * Thermal System instruction operand
 */
typedef union NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION_OPERANDS {

    NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGETS_AVAILABLE_OPERANDS          getInfoTargetsAvailable;

    NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE_OPERANDS                getInfoTargetType;

    NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_PROVIDER_TYPE_OPERANDS              getInfoProviderType;

    NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE_OPERANDS          getInfoSensorsAvailable;

    NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_PROVIDER_OPERANDS            getInfoSensorProvider;

    NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET_OPERANDS              getInfoSensorTarget;

    NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE_OPERANDS       getInfoSensorReadingRange;

    NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING_OPERANDS           getStatusSensorReading;


    NvU32                                                                   space[8];
} NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION_OPERANDS;

/*
 * NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION
 *
 * All thermal system instructions have the following layout:
 *
 *   result
 *       This field is set by the driver, and is the result of the
 *       instruction's execution. This value is only valid if the
 *       executed field is not 0 upon return.
 *       Possible status values returned are:
 *        NV_OK
 *        NV_ERR_INVALID_ARGUMENT
 *        NV_ERR_INVALID_PARAM_STRUCT
 *
 *   executed
 *       This field is set by the driver, and
 *       indicates if the instruction was executed.
 *       Possible status values returned are:
 *        0: Not executed
 *        1: Executed
 *
 *   opcode
 *       This field is set by the client to the desired instruction opcode.
 *       Possible values are:
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE_OPCODE
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_PROVIDER_TYPE_OPCODE
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE_OPCODE
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_PROVIDER_OPCODE
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET_OPCODE
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE_OPCODE
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING_OPCODE
 */


 /*
 *   operands
 *       This field is actually a union of all of the available operands.
 *       The interpretation of this field is opcode context dependent.
 *       Possible values are:
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE_OPERANDS
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_PROVIDER_TYPE_OPERANDS
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE_OPERANDS
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_PROVIDER_OPERANDS
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET_OPERANDS
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE_OPERANDS
 *        NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING_OPERANDS
 */


typedef struct NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION {
    NvU32                                           result;
    NvU32                                           executed;
    NvU32                                           opcode;
    NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION_OPERANDS operands;
} NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION;

#define NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION_MAX_COUNT 0x20U
#define NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS_MESSAGE_ID (0x13U)

typedef struct NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS {
    NvU32                                  clientAPIVersion;
    NvU32                                  clientAPIRevision;
    NvU32                                  clientInstructionSizeOf;
    NvU32                                  executeFlags;
    NvU32                                  successfulInstructions;
    NvU32                                  instructionListSize;
    NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION instructionList[NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION_MAX_COUNT];
} NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS;

/* _ctrl2080thermal_h_ */
