/*
 * Copyright 2020 Advanced Micro Devices, Inc.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#define SWSMU_CODE_LAYER_L4

#include "amdgpu.h"
#include "amdgpu_smu.h"
#include "smu_cmn.h"
#include "soc15_common.h"

/*
 * DO NOT use these for err/warn/info/debug messages.
 * Use dev_err, dev_warn, dev_info and dev_dbg instead.
 * They are more MGPU friendly.
 */
#undef pr_err
#undef pr_warn
#undef pr_info
#undef pr_debug

/*
 * Although these are defined in each ASIC's specific header file.
 * They share the same definitions and values. That makes common
 * APIs for SMC messages issuing for all ASICs possible.
 */
#define mmMP1_SMN_C2PMSG_66                                                                            0x0282
#define mmMP1_SMN_C2PMSG_66_BASE_IDX                                                                   0

#define mmMP1_SMN_C2PMSG_82                                                                            0x0292
#define mmMP1_SMN_C2PMSG_82_BASE_IDX                                                                   0

#define mmMP1_SMN_C2PMSG_90                                                                            0x029a
#define mmMP1_SMN_C2PMSG_90_BASE_IDX                                                                   0

#define MP1_C2PMSG_90__CONTENT_MASK                                                                    0xFFFFFFFFL

#undef __SMU_DUMMY_MAP
#define __SMU_DUMMY_MAP(type)	#type
static const char * const __smu_message_names[] = {
	SMU_MESSAGE_TYPES
};

static const char *smu_get_message_name(struct smu_context *smu,
					enum smu_message_type type)
{
	if (type < 0 || type >= SMU_MSG_MAX_COUNT)
		return "unknown smu message";

	return __smu_message_names[type];
}

static void smu_cmn_read_arg(struct smu_context *smu,
			     uint32_t *arg)
{
	struct amdgpu_device *adev = smu->adev;

	*arg = RREG32_SOC15(MP1, 0, mmMP1_SMN_C2PMSG_82);
}

/* Redefine the SMU error codes here.
 *
 * Note that these definitions are redundant and should be removed
 * when the SMU has exported a unified header file containing these
 * macros, which header file we can just include and use the SMU's
 * macros. At the moment, these error codes are defined by the SMU
 * per-ASIC unfortunately, yet we're a one driver for all ASICs.
 */
#define SMU_RESP_NONE           0
#define SMU_RESP_OK             1
#define SMU_RESP_CMD_FAIL       0xFF
#define SMU_RESP_CMD_UNKNOWN    0xFE
#define SMU_RESP_CMD_BAD_PREREQ 0xFD
#define SMU_RESP_BUSY_OTHER     0xFC
#define SMU_RESP_DEBUG_END      0xFB

/**
 * __smu_cmn_poll_stat -- poll for a status from the SMU
 * @smu: a pointer to SMU context
 *
 * Returns the status of the SMU, which could be,
 *    0, the SMU is busy with your command;
 *    1, execution status: success, execution result: success;
 * 0xFF, execution status: success, execution result: failure;
 * 0xFE, unknown command;
 * 0xFD, valid command, but bad (command) prerequisites;
 * 0xFC, the command was rejected as the SMU is busy;
 * 0xFB, "SMC_Result_DebugDataDumpEnd".
 *
 * The values here are not defined by macros, because I'd rather we
 * include a single header file which defines them, which is
 * maintained by the SMU FW team, so that we're impervious to firmware
 * changes. At the moment those values are defined in various header
 * files, one for each ASIC, yet here we're a single ASIC-agnostic
 * interface. Such a change can be followed-up by a subsequent patch.
 */
static u32 __smu_cmn_poll_stat(struct smu_context *smu)
{
	struct amdgpu_device *adev = smu->adev;
	int timeout = adev->usec_timeout * 20;
	u32 reg;

	for ( ; timeout > 0; timeout--) {
		reg = RREG32_SOC15(MP1, 0, mmMP1_SMN_C2PMSG_90);
		if ((reg & MP1_C2PMSG_90__CONTENT_MASK) != 0)
			break;

		udelay(1);
	}

	return reg;
}

static void __smu_cmn_reg_print_error(struct smu_context *smu,
				      u32 reg_c2pmsg_90,
				      int msg_index,
				      u32 param,
				      enum smu_message_type msg)
{
	struct amdgpu_device *adev = smu->adev;
	const char *message = smu_get_message_name(smu, msg);

	switch (reg_c2pmsg_90) {
	case SMU_RESP_NONE: {
		u32 msg_idx = RREG32_SOC15(MP1, 0, mmMP1_SMN_C2PMSG_66);
		u32 prm     = RREG32_SOC15(MP1, 0, mmMP1_SMN_C2PMSG_82);
		dev_err_ratelimited(adev->dev,
				    "SMU: I'm not done with your command: SMN_C2PMSG_66:0x%08X SMN_C2PMSG_82:0x%08X",
				    msg_idx, prm);
	}
		break;
	case SMU_RESP_OK:
		/* The SMU executed the command. It completed with a
		 * successful result.
		 */
		break;
	case SMU_RESP_CMD_FAIL:
		/* The SMU executed the command. It completed with an
		 * unsuccessful result.
		 */
		break;
	case SMU_RESP_CMD_UNKNOWN:
		dev_err_ratelimited(adev->dev,
				    "SMU: unknown command: index:%d param:0x%08X message:%s",
				    msg_index, param, message);
		break;
	case SMU_RESP_CMD_BAD_PREREQ:
		dev_err_ratelimited(adev->dev,
				    "SMU: valid command, bad prerequisites: index:%d param:0x%08X message:%s",
				    msg_index, param, message);
		break;
	case SMU_RESP_BUSY_OTHER:
		dev_err_ratelimited(adev->dev,
				    "SMU: I'm very busy for your command: index:%d param:0x%08X message:%s",
				    msg_index, param, message);
		break;
	case SMU_RESP_DEBUG_END:
		dev_err_ratelimited(adev->dev,
				    "SMU: I'm debugging!");
		break;
	default:
		dev_err_ratelimited(adev->dev,
				    "SMU: response:0x%08X for index:%d param:0x%08X message:%s?",
				    reg_c2pmsg_90, msg_index, param, message);
		break;
	}
}

static int __smu_cmn_reg2errno(struct smu_context *smu, u32 reg_c2pmsg_90)
{
	int res;

	switch (reg_c2pmsg_90) {
	case SMU_RESP_NONE:
		/* The SMU is busy--still executing your command.
		 */
		res = -ETIME;
		break;
	case SMU_RESP_OK:
		res = 0;
		break;
	case SMU_RESP_CMD_FAIL:
		/* Command completed successfully, but the command
		 * status was failure.
		 */
		res = -EIO;
		break;
	case SMU_RESP_CMD_UNKNOWN:
		/* Unknown command--ignored by the SMU.
		 */
		res = -EOPNOTSUPP;
		break;
	case SMU_RESP_CMD_BAD_PREREQ:
		/* Valid command--bad prerequisites.
		 */
		res = -EINVAL;
		break;
	case SMU_RESP_BUSY_OTHER:
		/* The SMU is busy with other commands. The client
		 * should retry in 10 us.
		 */
		res = -EBUSY;
		break;
	default:
		/* Unknown or debug response from the SMU.
		 */
		res = -EREMOTEIO;
		break;
	}

	return res;
}

static void __smu_cmn_send_msg(struct smu_context *smu,
			       u16 msg,
			       u32 param)
{
	struct amdgpu_device *adev = smu->adev;

	WREG32_SOC15(MP1, 0, mmMP1_SMN_C2PMSG_90, 0);
	WREG32_SOC15(MP1, 0, mmMP1_SMN_C2PMSG_82, param);
	WREG32_SOC15(MP1, 0, mmMP1_SMN_C2PMSG_66, msg);
}

/**
 * smu_cmn_send_msg_without_waiting -- send the message; don't wait for status
 * @smu: pointer to an SMU context
 * @msg_index: message index
 * @param: message parameter to send to the SMU
 *
 * Send a message to the SMU with the parameter passed. Do not wait
 * for status/result of the message, thus the "without_waiting".
 *
 * Return 0 on success, -errno on error if we weren't able to _send_
 * the message for some reason. See __smu_cmn_reg2errno() for details
 * of the -errno.
 */
int smu_cmn_send_msg_without_waiting(struct smu_context *smu,
				     uint16_t msg_index,
				     uint32_t param)
{
	struct amdgpu_device *adev = smu->adev;
	u32 reg;
	int res;

	if (adev->no_hw_access)
		return 0;

	reg = __smu_cmn_poll_stat(smu);
	res = __smu_cmn_reg2errno(smu, reg);
	if (reg == SMU_RESP_NONE ||
	    reg == SMU_RESP_BUSY_OTHER ||
	    res == -EREMOTEIO)
		goto Out;
	__smu_cmn_send_msg(smu, msg_index, param);
	res = 0;
Out:
	if (unlikely(adev->pm.smu_debug_mask & SMU_DEBUG_HALT_ON_ERROR) &&
	    res && (res != -ETIME)) {
		amdgpu_device_halt(adev);
		WARN_ON(1);
	}

	return res;
}

/**
 * smu_cmn_wait_for_response -- wait for response from the SMU
 * @smu: pointer to an SMU context
 *
 * Wait for status from the SMU.
 *
 * Return 0 on success, -errno on error, indicating the execution
 * status and result of the message being waited for. See
 * __smu_cmn_reg2errno() for details of the -errno.
 */
int smu_cmn_wait_for_response(struct smu_context *smu)
{
	u32 reg;
	int res;

	reg = __smu_cmn_poll_stat(smu);
	res = __smu_cmn_reg2errno(smu, reg);

	if (unlikely(smu->adev->pm.smu_debug_mask & SMU_DEBUG_HALT_ON_ERROR) &&
	    res && (res != -ETIME)) {
		amdgpu_device_halt(smu->adev);
		WARN_ON(1);
	}

	return res;
}

/**
 * smu_cmn_send_smc_msg_with_param -- send a message with parameter
 * @smu: pointer to an SMU context
 * @msg: message to send
 * @param: parameter to send to the SMU
 * @read_arg: pointer to u32 to return a value from the SMU back
 *            to the caller
 *
 * Send the message @msg with parameter @param to the SMU, wait for
 * completion of the command, and return back a value from the SMU in
 * @read_arg pointer.
 *
 * Return 0 on success, -errno on error, if we weren't able to send
 * the message or if the message completed with some kind of
 * error. See __smu_cmn_reg2errno() for details of the -errno.
 *
 * If we weren't able to send the message to the SMU, we also print
 * the error to the standard log.
 *
 * Command completion status is printed only if the -errno is
 * -EREMOTEIO, indicating that the SMU returned back an
 * undefined/unknown/unspecified result. All other cases are
 * well-defined, not printed, but instead given back to the client to
 * decide what further to do.
 *
 * The return value, @read_arg is read back regardless, to give back
 * more information to the client, which on error would most likely be
 * @param, but we can't assume that. This also eliminates more
 * conditionals.
 */
int smu_cmn_send_smc_msg_with_param(struct smu_context *smu,
				    enum smu_message_type msg,
				    uint32_t param,
				    uint32_t *read_arg)
{
	struct amdgpu_device *adev = smu->adev;
	int res, index;
	u32 reg;

	if (adev->no_hw_access)
		return 0;

	index = smu_cmn_to_asic_specific_index(smu,
					       CMN2ASIC_MAPPING_MSG,
					       msg);
	if (index < 0)
		return index == -EACCES ? 0 : index;

	mutex_lock(&smu->message_lock);
	reg = __smu_cmn_poll_stat(smu);
	res = __smu_cmn_reg2errno(smu, reg);
	if (reg == SMU_RESP_NONE ||
	    reg == SMU_RESP_BUSY_OTHER ||
	    res == -EREMOTEIO) {
		__smu_cmn_reg_print_error(smu, reg, index, param, msg);
		goto Out;
	}
	__smu_cmn_send_msg(smu, (uint16_t) index, param);
	reg = __smu_cmn_poll_stat(smu);
	res = __smu_cmn_reg2errno(smu, reg);
	if (res != 0)
		__smu_cmn_reg_print_error(smu, reg, index, param, msg);
	if (read_arg)
		smu_cmn_read_arg(smu, read_arg);
Out:
	if (unlikely(adev->pm.smu_debug_mask & SMU_DEBUG_HALT_ON_ERROR) && res) {
		amdgpu_device_halt(adev);
		WARN_ON(1);
	}

	mutex_unlock(&smu->message_lock);
	return res;
}

int smu_cmn_send_smc_msg(struct smu_context *smu,
			 enum smu_message_type msg,
			 uint32_t *read_arg)
{
	return smu_cmn_send_smc_msg_with_param(smu,
					       msg,
					       0,
					       read_arg);
}

int smu_cmn_to_asic_specific_index(struct smu_context *smu,
				   enum smu_cmn2asic_mapping_type type,
				   uint32_t index)
{
	struct cmn2asic_msg_mapping msg_mapping;
	struct cmn2asic_mapping mapping;

	switch (type) {
	case CMN2ASIC_MAPPING_MSG:
		if (index >= SMU_MSG_MAX_COUNT ||
		    !smu->message_map)
			return -EINVAL;

		msg_mapping = smu->message_map[index];
		if (!msg_mapping.valid_mapping)
			return -EINVAL;

		if (amdgpu_sriov_vf(smu->adev) &&
		    !msg_mapping.valid_in_vf)
			return -EACCES;

		return msg_mapping.map_to;

	case CMN2ASIC_MAPPING_CLK:
		if (index >= SMU_CLK_COUNT ||
		    !smu->clock_map)
			return -EINVAL;

		mapping = smu->clock_map[index];
		if (!mapping.valid_mapping)
			return -EINVAL;

		return mapping.map_to;

	case CMN2ASIC_MAPPING_FEATURE:
		if (index >= SMU_FEATURE_COUNT ||
		    !smu->feature_map)
			return -EINVAL;

		mapping = smu->feature_map[index];
		if (!mapping.valid_mapping)
			return -EINVAL;

		return mapping.map_to;

	case CMN2ASIC_MAPPING_TABLE:
		if (index >= SMU_TABLE_COUNT ||
		    !smu->table_map)
			return -EINVAL;

		mapping = smu->table_map[index];
		if (!mapping.valid_mapping)
			return -EINVAL;

		return mapping.map_to;

	case CMN2ASIC_MAPPING_PWR:
		if (index >= SMU_POWER_SOURCE_COUNT ||
		    !smu->pwr_src_map)
			return -EINVAL;

		mapping = smu->pwr_src_map[index];
		if (!mapping.valid_mapping)
			return -EINVAL;

		return mapping.map_to;

	case CMN2ASIC_MAPPING_WORKLOAD:
		if (index > PP_SMC_POWER_PROFILE_CUSTOM ||
		    !smu->workload_map)
			return -EINVAL;

		mapping = smu->workload_map[index];
		if (!mapping.valid_mapping)
			return -EINVAL;

		return mapping.map_to;

	default:
		return -EINVAL;
	}
}

int smu_cmn_feature_is_supported(struct smu_context *smu,
				 enum smu_feature_mask mask)
{
	struct smu_feature *feature = &smu->smu_feature;
	int feature_id;

	feature_id = smu_cmn_to_asic_specific_index(smu,
						    CMN2ASIC_MAPPING_FEATURE,
						    mask);
	if (feature_id < 0)
		return 0;

	WARN_ON(feature_id > feature->feature_num);

	return test_bit(feature_id, feature->supported);
}

int smu_cmn_feature_is_enabled(struct smu_context *smu,
			       enum smu_feature_mask mask)
{
	struct amdgpu_device *adev = smu->adev;
	uint64_t enabled_features;
	int feature_id;

	if (smu_cmn_get_enabled_mask(smu, &enabled_features)) {
		dev_err(adev->dev, "Failed to retrieve enabled ppfeatures!\n");
		return 0;
	}

	/*
	 * For Renoir and Cyan Skillfish, they are assumed to have all features
	 * enabled. Also considering they have no feature_map available, the
	 * check here can avoid unwanted feature_map check below.
	 */
	if (enabled_features == ULLONG_MAX)
		return 1;

	feature_id = smu_cmn_to_asic_specific_index(smu,
						    CMN2ASIC_MAPPING_FEATURE,
						    mask);
	if (feature_id < 0)
		return 0;

	return test_bit(feature_id, (unsigned long *)&enabled_features);
}

bool smu_cmn_clk_dpm_is_enabled(struct smu_context *smu,
				enum smu_clk_type clk_type)
{
	enum smu_feature_mask feature_id = 0;

	switch (clk_type) {
	case SMU_MCLK:
	case SMU_UCLK:
		feature_id = SMU_FEATURE_DPM_UCLK_BIT;
		break;
	case SMU_GFXCLK:
	case SMU_SCLK:
		feature_id = SMU_FEATURE_DPM_GFXCLK_BIT;
		break;
	case SMU_SOCCLK:
		feature_id = SMU_FEATURE_DPM_SOCCLK_BIT;
		break;
	default:
		return true;
	}

	if (!smu_cmn_feature_is_enabled(smu, feature_id))
		return false;

	return true;
}

int smu_cmn_get_enabled_mask(struct smu_context *smu,
			     uint64_t *feature_mask)
{
	struct amdgpu_device *adev = smu->adev;
	uint32_t *feature_mask_high;
	uint32_t *feature_mask_low;
	int ret = 0;

	if (!feature_mask)
		return -EINVAL;

	feature_mask_low = &((uint32_t *)feature_mask)[0];
	feature_mask_high = &((uint32_t *)feature_mask)[1];

	switch (adev->ip_versions[MP1_HWIP][0]) {
	/* For Vangogh and Yellow Carp */
	case IP_VERSION(11, 5, 0):
	case IP_VERSION(13, 0, 1):
	case IP_VERSION(13, 0, 3):
	case IP_VERSION(13, 0, 8):
		ret = smu_cmn_send_smc_msg_with_param(smu,
						      SMU_MSG_GetEnabledSmuFeatures,
						      0,
						      feature_mask_low);
		if (ret)
			return ret;

		ret = smu_cmn_send_smc_msg_with_param(smu,
						      SMU_MSG_GetEnabledSmuFeatures,
						      1,
						      feature_mask_high);
		break;
	/*
	 * For Cyan Skillfish and Renoir, there is no interface provided by PMFW
	 * to retrieve the enabled features. So, we assume all features are enabled.
	 * TODO: add other APU ASICs which suffer from the same issue here
	 */
	case IP_VERSION(11, 0, 8):
	case IP_VERSION(12, 0, 0):
	case IP_VERSION(12, 0, 1):
		memset(feature_mask, 0xff, sizeof(*feature_mask));
		break;
	/* other dGPU ASICs */
	default:
		ret = smu_cmn_send_smc_msg(smu,
					   SMU_MSG_GetEnabledSmuFeaturesHigh,
					   feature_mask_high);
		if (ret)
			return ret;

		ret = smu_cmn_send_smc_msg(smu,
					   SMU_MSG_GetEnabledSmuFeaturesLow,
					   feature_mask_low);
		break;
	}

	return ret;
}

uint64_t smu_cmn_get_indep_throttler_status(
					const unsigned long dep_status,
					const uint8_t *throttler_map)
{
	uint64_t indep_status = 0;
	uint8_t dep_bit = 0;

	for_each_set_bit(dep_bit, &dep_status, 32)
		indep_status |= 1ULL << throttler_map[dep_bit];

	return indep_status;
}

int smu_cmn_feature_update_enable_state(struct smu_context *smu,
					uint64_t feature_mask,
					bool enabled)
{
	int ret = 0;

	if (enabled) {
		ret = smu_cmn_send_smc_msg_with_param(smu,
						  SMU_MSG_EnableSmuFeaturesLow,
						  lower_32_bits(feature_mask),
						  NULL);
		if (ret)
			return ret;
		ret = smu_cmn_send_smc_msg_with_param(smu,
						  SMU_MSG_EnableSmuFeaturesHigh,
						  upper_32_bits(feature_mask),
						  NULL);
	} else {
		ret = smu_cmn_send_smc_msg_with_param(smu,
						  SMU_MSG_DisableSmuFeaturesLow,
						  lower_32_bits(feature_mask),
						  NULL);
		if (ret)
			return ret;
		ret = smu_cmn_send_smc_msg_with_param(smu,
						  SMU_MSG_DisableSmuFeaturesHigh,
						  upper_32_bits(feature_mask),
						  NULL);
	}

	return ret;
}

int smu_cmn_feature_set_enabled(struct smu_context *smu,
				enum smu_feature_mask mask,
				bool enable)
{
	int feature_id;

	feature_id = smu_cmn_to_asic_specific_index(smu,
						    CMN2ASIC_MAPPING_FEATURE,
						    mask);
	if (feature_id < 0)
		return -EINVAL;

	return smu_cmn_feature_update_enable_state(smu,
					       1ULL << feature_id,
					       enable);
}

#undef __SMU_DUMMY_MAP
#define __SMU_DUMMY_MAP(fea)	#fea
static const char* __smu_feature_names[] = {
	SMU_FEATURE_MASKS
};

static const char *smu_get_feature_name(struct smu_context *smu,
					enum smu_feature_mask feature)
{
	if (feature < 0 || feature >= SMU_FEATURE_COUNT)
		return "unknown smu feature";
	return __smu_feature_names[feature];
}

size_t smu_cmn_get_pp_feature_mask(struct smu_context *smu,
				   char *buf)
{
	uint64_t feature_mask;
	int feature_index = 0;
	uint32_t count = 0;
	int8_t sort_feature[SMU_FEATURE_COUNT];
	size_t size = 0;
	int ret = 0, i;
	int feature_id;

	ret = smu_cmn_get_enabled_mask(smu,
				       &feature_mask);
	if (ret)
		return 0;

	size =  sysfs_emit_at(buf, size, "features high: 0x%08x low: 0x%08x\n",
			upper_32_bits(feature_mask), lower_32_bits(feature_mask));

	memset(sort_feature, -1, sizeof(sort_feature));

	for (i = 0; i < SMU_FEATURE_COUNT; i++) {
		feature_index = smu_cmn_to_asic_specific_index(smu,
							       CMN2ASIC_MAPPING_FEATURE,
							       i);
		if (feature_index < 0)
			continue;

		sort_feature[feature_index] = i;
	}

	size += sysfs_emit_at(buf, size, "%-2s. %-20s  %-3s : %-s\n",
			"No", "Feature", "Bit", "State");

	for (i = 0; i < SMU_FEATURE_COUNT; i++) {
		if (sort_feature[i] < 0)
			continue;

		/* convert to asic spcific feature ID */
		feature_id = smu_cmn_to_asic_specific_index(smu,
							    CMN2ASIC_MAPPING_FEATURE,
							    sort_feature[i]);
		if (feature_id < 0)
			continue;

		size += sysfs_emit_at(buf, size, "%02d. %-20s (%2d) : %s\n",
				count++,
				smu_get_feature_name(smu, sort_feature[i]),
				i,
				!!test_bit(feature_id, (unsigned long *)&feature_mask) ?
				"enabled" : "disabled");
	}

	return size;
}

int smu_cmn_set_pp_feature_mask(struct smu_context *smu,
				uint64_t new_mask)
{
	int ret = 0;
	uint64_t feature_mask;
	uint64_t feature_2_enabled = 0;
	uint64_t feature_2_disabled = 0;

	ret = smu_cmn_get_enabled_mask(smu,
				       &feature_mask);
	if (ret)
		return ret;

	feature_2_enabled  = ~feature_mask & new_mask;
	feature_2_disabled = feature_mask & ~new_mask;

	if (feature_2_enabled) {
		ret = smu_cmn_feature_update_enable_state(smu,
							  feature_2_enabled,
							  true);
		if (ret)
			return ret;
	}
	if (feature_2_disabled) {
		ret = smu_cmn_feature_update_enable_state(smu,
							  feature_2_disabled,
							  false);
		if (ret)
			return ret;
	}

	return ret;
}

/**
 * smu_cmn_disable_all_features_with_exception - disable all dpm features
 *                                               except this specified by
 *                                               @mask
 *
 * @smu:               smu_context pointer
 * @mask:              the dpm feature which should not be disabled
 *                     SMU_FEATURE_COUNT: no exception, all dpm features
 *                     to disable
 *
 * Returns:
 * 0 on success or a negative error code on failure.
 */
int smu_cmn_disable_all_features_with_exception(struct smu_context *smu,
						enum smu_feature_mask mask)
{
	uint64_t features_to_disable = U64_MAX;
	int skipped_feature_id;

	if (mask != SMU_FEATURE_COUNT) {
		skipped_feature_id = smu_cmn_to_asic_specific_index(smu,
								    CMN2ASIC_MAPPING_FEATURE,
								    mask);
		if (skipped_feature_id < 0)
			return -EINVAL;

		features_to_disable &= ~(1ULL << skipped_feature_id);
	}

	return smu_cmn_feature_update_enable_state(smu,
						   features_to_disable,
						   0);
}

int smu_cmn_get_smc_version(struct smu_context *smu,
			    uint32_t *if_version,
			    uint32_t *smu_version)
{
	int ret = 0;

	if (!if_version && !smu_version)
		return -EINVAL;

	if (smu->smc_fw_if_version && smu->smc_fw_version)
	{
		if (if_version)
			*if_version = smu->smc_fw_if_version;

		if (smu_version)
			*smu_version = smu->smc_fw_version;

		return 0;
	}

	if (if_version) {
		ret = smu_cmn_send_smc_msg(smu, SMU_MSG_GetDriverIfVersion, if_version);
		if (ret)
			return ret;

		smu->smc_fw_if_version = *if_version;
	}

	if (smu_version) {
		ret = smu_cmn_send_smc_msg(smu, SMU_MSG_GetSmuVersion, smu_version);
		if (ret)
			return ret;

		smu->smc_fw_version = *smu_version;
	}

	return ret;
}

int smu_cmn_update_table(struct smu_context *smu,
			 enum smu_table_id table_index,
			 int argument,
			 void *table_data,
			 bool drv2smu)
{
	struct smu_table_context *smu_table = &smu->smu_table;
	struct amdgpu_device *adev = smu->adev;
	struct smu_table *table = &smu_table->driver_table;
	int table_id = smu_cmn_to_asic_specific_index(smu,
						      CMN2ASIC_MAPPING_TABLE,
						      table_index);
	uint32_t table_size;
	int ret = 0;
	if (!table_data || table_id >= SMU_TABLE_COUNT || table_id < 0)
		return -EINVAL;

	table_size = smu_table->tables[table_index].size;

	if (drv2smu) {
		memcpy(table->cpu_addr, table_data, table_size);
		/*
		 * Flush hdp cache: to guard the content seen by
		 * GPU is consitent with CPU.
		 */
		amdgpu_asic_flush_hdp(adev, NULL);
	}

	ret = smu_cmn_send_smc_msg_with_param(smu, drv2smu ?
					  SMU_MSG_TransferTableDram2Smu :
					  SMU_MSG_TransferTableSmu2Dram,
					  table_id | ((argument & 0xFFFF) << 16),
					  NULL);
	if (ret)
		return ret;

	if (!drv2smu) {
		amdgpu_asic_invalidate_hdp(adev, NULL);
		memcpy(table_data, table->cpu_addr, table_size);
	}

	return 0;
}

int smu_cmn_write_watermarks_table(struct smu_context *smu)
{
	void *watermarks_table = smu->smu_table.watermarks_table;

	if (!watermarks_table)
		return -EINVAL;

	return smu_cmn_update_table(smu,
				    SMU_TABLE_WATERMARKS,
				    0,
				    watermarks_table,
				    true);
}

int smu_cmn_write_pptable(struct smu_context *smu)
{
	void *pptable = smu->smu_table.driver_pptable;

	return smu_cmn_update_table(smu,
				    SMU_TABLE_PPTABLE,
				    0,
				    pptable,
				    true);
}

int smu_cmn_get_metrics_table(struct smu_context *smu,
			      void *metrics_table,
			      bool bypass_cache)
{
	struct smu_table_context *smu_table= &smu->smu_table;
	uint32_t table_size =
		smu_table->tables[SMU_TABLE_SMU_METRICS].size;
	int ret = 0;

	if (bypass_cache ||
	    !smu_table->metrics_time ||
	    time_after(jiffies, smu_table->metrics_time + msecs_to_jiffies(1))) {
		ret = smu_cmn_update_table(smu,
				       SMU_TABLE_SMU_METRICS,
				       0,
				       smu_table->metrics_table,
				       false);
		if (ret) {
			dev_info(smu->adev->dev, "Failed to export SMU metrics table!\n");
			return ret;
		}
		smu_table->metrics_time = jiffies;
	}

	if (metrics_table)
		memcpy(metrics_table, smu_table->metrics_table, table_size);

	return 0;
}

void smu_cmn_init_soft_gpu_metrics(void *table, uint8_t frev, uint8_t crev)
{
	struct metrics_table_header *header = (struct metrics_table_header *)table;
	uint16_t structure_size;

#define METRICS_VERSION(a, b)	((a << 16) | b )

	switch (METRICS_VERSION(frev, crev)) {
	case METRICS_VERSION(1, 0):
		structure_size = sizeof(struct gpu_metrics_v1_0);
		break;
	case METRICS_VERSION(1, 1):
		structure_size = sizeof(struct gpu_metrics_v1_1);
		break;
	case METRICS_VERSION(1, 2):
		structure_size = sizeof(struct gpu_metrics_v1_2);
		break;
	case METRICS_VERSION(1, 3):
		structure_size = sizeof(struct gpu_metrics_v1_3);
		break;
	case METRICS_VERSION(2, 0):
		structure_size = sizeof(struct gpu_metrics_v2_0);
		break;
	case METRICS_VERSION(2, 1):
		structure_size = sizeof(struct gpu_metrics_v2_1);
		break;
	case METRICS_VERSION(2, 2):
		structure_size = sizeof(struct gpu_metrics_v2_2);
		break;
	default:
		return;
	}

#undef METRICS_VERSION

	memset(header, 0xFF, structure_size);

	header->format_revision = frev;
	header->content_revision = crev;
	header->structure_size = structure_size;

}

int smu_cmn_set_mp1_state(struct smu_context *smu,
			  enum pp_mp1_state mp1_state)
{
	enum smu_message_type msg;
	int ret;

	switch (mp1_state) {
	case PP_MP1_STATE_SHUTDOWN:
		msg = SMU_MSG_PrepareMp1ForShutdown;
		break;
	case PP_MP1_STATE_UNLOAD:
		msg = SMU_MSG_PrepareMp1ForUnload;
		break;
	case PP_MP1_STATE_RESET:
		msg = SMU_MSG_PrepareMp1ForReset;
		break;
	case PP_MP1_STATE_NONE:
	default:
		return 0;
	}

	ret = smu_cmn_send_smc_msg(smu, msg, NULL);
	if (ret)
		dev_err(smu->adev->dev, "[PrepareMp1] Failed!\n");

	return ret;
}

bool smu_cmn_is_audio_func_enabled(struct amdgpu_device *adev)
{
	struct pci_dev *p = NULL;
	bool snd_driver_loaded;

	/*
	 * If the ASIC comes with no audio function, we always assume
	 * it is "enabled".
	 */
	p = pci_get_domain_bus_and_slot(pci_domain_nr(adev->pdev->bus),
			adev->pdev->bus->number, 1);
	if (!p)
		return true;

	snd_driver_loaded = pci_is_enabled(p) ? true : false;

	pci_dev_put(p);

	return snd_driver_loaded;
}
