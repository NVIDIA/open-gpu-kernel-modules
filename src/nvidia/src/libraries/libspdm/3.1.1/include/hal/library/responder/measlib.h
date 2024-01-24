/**
 *  Copyright Notice:
 *  Copyright 2021-2023 DMTF. All rights reserved.
 *  License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
 **/

#ifndef RESPONDER_MEASLIB_H
#define RESPONDER_MEASLIB_H

#include "hal/base.h"
#include "internal/libspdm_lib_config.h"
#include "library/spdm_return_status.h"
#include "industry_standard/spdm.h"

#if LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP
/**
 * Collect the device measurement.
 *
 * libspdm will call this function to retrieve the measurements for a device.
 * The "measurement_index" parameter indicates the measurement requested.
 *
 * @param spdm_version  Indicates the negotiated SPDM version.
 *
 * @param  measurement_specification  Indicates the measurement specification.
 * Must be a SPDM_MEASUREMENT_BLOCK_HEADER_SPECIFICATION_* value in spdm.h.
 *
 * @param  measurement_hash_algo  Indicates the measurement hash algorithm.
 * Must be SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_* value in spdm.h.
 *
 * @param  measurement_index  The index of the measurement to collect.
 * A value of 0x00 requests only the total number of measurements to be returned in
 * "measurements_count". The parameters "measurements" and "measurements_size" will be left
 * unmodified.
 *
 * A value of [0x01 - 0xFE] requests a single measurement for that measurement index
 * be returned. On success, "measurements_count" will be set to 1 and the
 * "measurements" and "measurements_size" fields will be set based
 * on the single measurement. An invalid measurement index will cause
 * "measurements_count" to return 0.
 *
 * A value of 0xFF requests all measurements be returned.
 * On success, "measurements_count", "measurements", and "measurements_size"
 * fields will be set with data from all measurements.
 *
 * @param request_attribute A bitmask who fields are SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_*.
 *
 * @param  measurements_count
 * When "measurement_index" is zero, returns the total count of
 * measurements available for the device. None of the actual measurements are
 * returned however, and "measurements" and "measurements_size" are unmodified.
 *
 * When "measurement_index" is non-zero, returns the number of measurements
 * returned in "measurements" and "measurements_size". If "measurements_index"
 * is an invalid index not supported by the device, "measurements_count" will
 * return 0 and the function will return LIBSPDM_STATUS_MEAS_INVALID_INDEX.
 *
 * @param  measurements
 * A pointer to a destination buffer to store the concatenation of all device
 * measurement blocks. This buffer will only be modified if "measurement_index" is non-zero.
 *
 * @param  measurements_size
 * On input, indicates the size in bytes of the destination buffer.
 * On output, indicates the total size in bytes of all device measurement
 * blocks in the buffer. This field should only be modified if "measurement_index" is non-zero.
 **/
extern libspdm_return_t libspdm_measurement_collection(
    spdm_version_number_t spdm_version,
    uint8_t measurement_specification,
    uint32_t measurement_hash_algo,
    uint8_t measurement_index,
    uint8_t request_attribute,
    uint8_t *content_changed,
    uint8_t *measurements_count,
    void *measurements,
    size_t *measurements_size);

/**
 * This functions returns the opaque data in a MEASUREMENTS response.
 *
 * It is called immediately after libspdm_measurement_collection() is called and allows the opaque
 * data field to vary based on the GET_MEASUREMENTS request.
 *
 * @param spdm_version  Indicates the negotiated SPDM version.
 *
 * @param  measurement_specification  Indicates the measurement specification.
 * Must be a SPDM_MEASUREMENT_BLOCK_HEADER_SPECIFICATION_* value in spdm.h.
 *
 * @param  measurement_hash_algo  Indicates the measurement hash algorithm.
 * Must be SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_* value in spdm.h.
 *
 * @param  measurement_index  The index of the measurement to collect.
 *
 * @param request_attribute A bitmask who fields are SPDM_GET_MEASUREMENTS_REQUEST_ATTRIBUTES_*.
 *
 * @param opaque_data
 * A pointer to a destination buffer whose size, in bytes, is opaque_data_size. The opaque data is
 * copied to this buffer.
 *
 * @param opaque_data_size
 * On input, indicates the size, in bytes, of the destination buffer.
 * On output, indicates the size of the opaque data.
 **/
extern bool libspdm_measurement_opaque_data(
    spdm_version_number_t spdm_version,
    uint8_t measurement_specification,
    uint32_t measurement_hash_algo,
    uint8_t measurement_index,
    uint8_t request_attribute,
    void *opaque_data,
    size_t *opaque_data_size);

/**
 * This function calculates the measurement summary hash.
 *
 * @param  spdm_version               The SPDM version.
 * @param  base_hash_algo             The hash algo to use on summary.
 * @param  measurement_specification  Indicates the measurement specification.
 *                                    It must align with measurement_specification.
 *                                    (SPDM_MEASUREMENT_BLOCK_HEADER_SPECIFICATION_*)
 * @param  measurement_hash_algo      Indicates the measurement hash algorithm.
 *                                    It must align with measurement_hash_alg
 *                                    (SPDM_ALGORITHMS_MEASUREMENT_HASH_ALGO_*)
 *
 * @param  measurement_summary_hash_type   The type of the measurement summary hash.
 * @param  measurement_summary_hash        The buffer to store the measurement summary hash.
 * @param  measurement_summary_hash_size   The size in bytes of the buffer.
 *
 * @retval true  measurement summary hash is generated or skipped.
 * @retval false measurement summary hash is not generated.
 **/
extern bool libspdm_generate_measurement_summary_hash(
    spdm_version_number_t spdm_version,
    uint32_t base_hash_algo,
    uint8_t measurement_specification,
    uint32_t measurement_hash_algo,
    uint8_t measurement_summary_hash_type,
    uint8_t *measurement_summary_hash,
    uint32_t measurement_summary_hash_size);
#endif /* LIBSPDM_ENABLE_CAPABILITY_MEAS_CAP */

#endif /* RESPONDER_MEASLIB_H */
