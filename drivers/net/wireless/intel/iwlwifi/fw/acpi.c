// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/*
 * Copyright (C) 2017 Intel Deutschland GmbH
 * Copyright (C) 2019-2021 Intel Corporation
 */
#include <linux/uuid.h>
#include "iwl-drv.h"
#include "iwl-debug.h"
#include "acpi.h"
#include "fw/runtime.h"

const guid_t iwl_guid = GUID_INIT(0xF21202BF, 0x8F78, 0x4DC6,
				  0xA5, 0xB3, 0x1F, 0x73,
				  0x8E, 0x28, 0x5A, 0xDE);
IWL_EXPORT_SYMBOL(iwl_guid);

const guid_t iwl_rfi_guid = GUID_INIT(0x7266172C, 0x220B, 0x4B29,
				      0x81, 0x4F, 0x75, 0xE4,
				      0xDD, 0x26, 0xB5, 0xFD);
IWL_EXPORT_SYMBOL(iwl_rfi_guid);

static int iwl_acpi_get_handle(struct device *dev, acpi_string method,
			       acpi_handle *ret_handle)
{
	acpi_handle root_handle;
	acpi_status status;

	root_handle = ACPI_HANDLE(dev);
	if (!root_handle) {
		IWL_DEBUG_DEV_RADIO(dev,
				    "ACPI: Could not retrieve root port handle\n");
		return -ENOENT;
	}

	status = acpi_get_handle(root_handle, method, ret_handle);
	if (ACPI_FAILURE(status)) {
		IWL_DEBUG_DEV_RADIO(dev,
				    "ACPI: %s method not found\n", method);
		return -ENOENT;
	}
	return 0;
}

void *iwl_acpi_get_object(struct device *dev, acpi_string method)
{
	struct acpi_buffer buf = {ACPI_ALLOCATE_BUFFER, NULL};
	acpi_handle handle;
	acpi_status status;
	int ret;

	ret = iwl_acpi_get_handle(dev, method, &handle);
	if (ret)
		return ERR_PTR(-ENOENT);

	/* Call the method with no arguments */
	status = acpi_evaluate_object(handle, NULL, NULL, &buf);
	if (ACPI_FAILURE(status)) {
		IWL_DEBUG_DEV_RADIO(dev,
				    "ACPI: %s method invocation failed (status: 0x%x)\n",
				    method, status);
		return ERR_PTR(-ENOENT);
	}
	return buf.pointer;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_object);

/*
 * Generic function for evaluating a method defined in the device specific
 * method (DSM) interface. The returned acpi object must be freed by calling
 * function.
 */
static void *iwl_acpi_get_dsm_object(struct device *dev, int rev, int func,
				     union acpi_object *args,
				     const guid_t *guid)
{
	union acpi_object *obj;

	obj = acpi_evaluate_dsm(ACPI_HANDLE(dev), guid, rev, func,
				args);
	if (!obj) {
		IWL_DEBUG_DEV_RADIO(dev,
				    "ACPI: DSM method invocation failed (rev: %d, func:%d)\n",
				    rev, func);
		return ERR_PTR(-ENOENT);
	}
	return obj;
}

/*
 * Generic function to evaluate a DSM with no arguments
 * and an integer return value,
 * (as an integer object or inside a buffer object),
 * verify and assign the value in the "value" parameter.
 * return 0 in success and the appropriate errno otherwise.
 */
static int iwl_acpi_get_dsm_integer(struct device *dev, int rev, int func,
				    const guid_t *guid, u64 *value,
				    size_t expected_size)
{
	union acpi_object *obj;
	int ret = 0;

	obj = iwl_acpi_get_dsm_object(dev, rev, func, NULL, guid);
	if (IS_ERR(obj)) {
		IWL_DEBUG_DEV_RADIO(dev,
				    "Failed to get  DSM object. func= %d\n",
				    func);
		return -ENOENT;
	}

	if (obj->type == ACPI_TYPE_INTEGER) {
		*value = obj->integer.value;
	} else if (obj->type == ACPI_TYPE_BUFFER) {
		__le64 le_value = 0;

		if (WARN_ON_ONCE(expected_size > sizeof(le_value)))
			return -EINVAL;

		/* if the buffer size doesn't match the expected size */
		if (obj->buffer.length != expected_size)
			IWL_DEBUG_DEV_RADIO(dev,
					    "ACPI: DSM invalid buffer size, padding or truncating (%d)\n",
					    obj->buffer.length);

		 /* assuming LE from Intel BIOS spec */
		memcpy(&le_value, obj->buffer.pointer,
		       min_t(size_t, expected_size, (size_t)obj->buffer.length));
		*value = le64_to_cpu(le_value);
	} else {
		IWL_DEBUG_DEV_RADIO(dev,
				    "ACPI: DSM method did not return a valid object, type=%d\n",
				    obj->type);
		ret = -EINVAL;
		goto out;
	}

	IWL_DEBUG_DEV_RADIO(dev,
			    "ACPI: DSM method evaluated: func=%d, ret=%d\n",
			    func, ret);
out:
	ACPI_FREE(obj);
	return ret;
}

/*
 * Evaluate a DSM with no arguments and a u8 return value,
 */
int iwl_acpi_get_dsm_u8(struct device *dev, int rev, int func,
			const guid_t *guid, u8 *value)
{
	int ret;
	u64 val;

	ret = iwl_acpi_get_dsm_integer(dev, rev, func,
				       guid, &val, sizeof(u8));

	if (ret < 0)
		return ret;

	/* cast val (u64) to be u8 */
	*value = (u8)val;
	return 0;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_dsm_u8);

union acpi_object *iwl_acpi_get_wifi_pkg(struct device *dev,
					 union acpi_object *data,
					 int data_size, int *tbl_rev)
{
	int i;
	union acpi_object *wifi_pkg;

	/*
	 * We need at least one entry in the wifi package that
	 * describes the domain, and one more entry, otherwise there's
	 * no point in reading it.
	 */
	if (WARN_ON_ONCE(data_size < 2))
		return ERR_PTR(-EINVAL);

	/*
	 * We need at least two packages, one for the revision and one
	 * for the data itself.  Also check that the revision is valid
	 * (i.e. it is an integer (each caller has to check by itself
	 * if the returned revision is supported)).
	 */
	if (data->type != ACPI_TYPE_PACKAGE ||
	    data->package.count < 2 ||
	    data->package.elements[0].type != ACPI_TYPE_INTEGER) {
		IWL_DEBUG_DEV_RADIO(dev, "Invalid packages structure\n");
		return ERR_PTR(-EINVAL);
	}

	*tbl_rev = data->package.elements[0].integer.value;

	/* loop through all the packages to find the one for WiFi */
	for (i = 1; i < data->package.count; i++) {
		union acpi_object *domain;

		wifi_pkg = &data->package.elements[i];

		/* skip entries that are not a package with the right size */
		if (wifi_pkg->type != ACPI_TYPE_PACKAGE ||
		    wifi_pkg->package.count != data_size)
			continue;

		domain = &wifi_pkg->package.elements[0];
		if (domain->type == ACPI_TYPE_INTEGER &&
		    domain->integer.value == ACPI_WIFI_DOMAIN)
			goto found;
	}

	return ERR_PTR(-ENOENT);

found:
	return wifi_pkg;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_wifi_pkg);

int iwl_acpi_get_tas(struct iwl_fw_runtime *fwrt,
		     __le32 *block_list_array,
		     int *block_list_size)
{
	union acpi_object *wifi_pkg, *data;
	int ret, tbl_rev, i;
	bool enabled;

	data = iwl_acpi_get_object(fwrt->dev, ACPI_WTAS_METHOD);
	if (IS_ERR(data))
		return PTR_ERR(data);

	wifi_pkg = iwl_acpi_get_wifi_pkg(fwrt->dev, data,
					 ACPI_WTAS_WIFI_DATA_SIZE,
					 &tbl_rev);
	if (IS_ERR(wifi_pkg)) {
		ret = PTR_ERR(wifi_pkg);
		goto out_free;
	}

	if (wifi_pkg->package.elements[0].type != ACPI_TYPE_INTEGER ||
	    tbl_rev != 0) {
		ret = -EINVAL;
		goto out_free;
	}

	enabled = !!wifi_pkg->package.elements[0].integer.value;

	if (!enabled) {
		*block_list_size = -1;
		IWL_DEBUG_RADIO(fwrt, "TAS not enabled\n");
		ret = 0;
		goto out_free;
	}

	if (wifi_pkg->package.elements[1].type != ACPI_TYPE_INTEGER ||
	    wifi_pkg->package.elements[1].integer.value >
	    APCI_WTAS_BLACK_LIST_MAX) {
		IWL_DEBUG_RADIO(fwrt, "TAS invalid array size %llu\n",
				wifi_pkg->package.elements[1].integer.value);
		ret = -EINVAL;
		goto out_free;
	}
	*block_list_size = wifi_pkg->package.elements[1].integer.value;

	IWL_DEBUG_RADIO(fwrt, "TAS array size %d\n", *block_list_size);
	if (*block_list_size > APCI_WTAS_BLACK_LIST_MAX) {
		IWL_DEBUG_RADIO(fwrt, "TAS invalid array size value %u\n",
				*block_list_size);
		ret = -EINVAL;
		goto out_free;
	}

	for (i = 0; i < *block_list_size; i++) {
		u32 country;

		if (wifi_pkg->package.elements[2 + i].type !=
		    ACPI_TYPE_INTEGER) {
			IWL_DEBUG_RADIO(fwrt,
					"TAS invalid array elem %d\n", 2 + i);
			ret = -EINVAL;
			goto out_free;
		}

		country = wifi_pkg->package.elements[2 + i].integer.value;
		block_list_array[i] = cpu_to_le32(country);
		IWL_DEBUG_RADIO(fwrt, "TAS block list country %d\n", country);
	}

	ret = 0;
out_free:
	kfree(data);
	return ret;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_tas);

int iwl_acpi_get_mcc(struct device *dev, char *mcc)
{
	union acpi_object *wifi_pkg, *data;
	u32 mcc_val;
	int ret, tbl_rev;

	data = iwl_acpi_get_object(dev, ACPI_WRDD_METHOD);
	if (IS_ERR(data))
		return PTR_ERR(data);

	wifi_pkg = iwl_acpi_get_wifi_pkg(dev, data, ACPI_WRDD_WIFI_DATA_SIZE,
					 &tbl_rev);
	if (IS_ERR(wifi_pkg)) {
		ret = PTR_ERR(wifi_pkg);
		goto out_free;
	}

	if (wifi_pkg->package.elements[1].type != ACPI_TYPE_INTEGER ||
	    tbl_rev != 0) {
		ret = -EINVAL;
		goto out_free;
	}

	mcc_val = wifi_pkg->package.elements[1].integer.value;

	mcc[0] = (mcc_val >> 8) & 0xff;
	mcc[1] = mcc_val & 0xff;
	mcc[2] = '\0';

	ret = 0;
out_free:
	kfree(data);
	return ret;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_mcc);

u64 iwl_acpi_get_pwr_limit(struct device *dev)
{
	union acpi_object *data, *wifi_pkg;
	u64 dflt_pwr_limit;
	int tbl_rev;

	data = iwl_acpi_get_object(dev, ACPI_SPLC_METHOD);
	if (IS_ERR(data)) {
		dflt_pwr_limit = 0;
		goto out;
	}

	wifi_pkg = iwl_acpi_get_wifi_pkg(dev, data,
					 ACPI_SPLC_WIFI_DATA_SIZE, &tbl_rev);
	if (IS_ERR(wifi_pkg) || tbl_rev != 0 ||
	    wifi_pkg->package.elements[1].integer.value != ACPI_TYPE_INTEGER) {
		dflt_pwr_limit = 0;
		goto out_free;
	}

	dflt_pwr_limit = wifi_pkg->package.elements[1].integer.value;
out_free:
	kfree(data);
out:
	return dflt_pwr_limit;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_pwr_limit);

int iwl_acpi_get_eckv(struct device *dev, u32 *extl_clk)
{
	union acpi_object *wifi_pkg, *data;
	int ret, tbl_rev;

	data = iwl_acpi_get_object(dev, ACPI_ECKV_METHOD);
	if (IS_ERR(data))
		return PTR_ERR(data);

	wifi_pkg = iwl_acpi_get_wifi_pkg(dev, data, ACPI_ECKV_WIFI_DATA_SIZE,
					 &tbl_rev);
	if (IS_ERR(wifi_pkg)) {
		ret = PTR_ERR(wifi_pkg);
		goto out_free;
	}

	if (wifi_pkg->package.elements[1].type != ACPI_TYPE_INTEGER ||
	    tbl_rev != 0) {
		ret = -EINVAL;
		goto out_free;
	}

	*extl_clk = wifi_pkg->package.elements[1].integer.value;

	ret = 0;

out_free:
	kfree(data);
	return ret;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_eckv);

static int iwl_sar_set_profile(union acpi_object *table,
			       struct iwl_sar_profile *profile,
			       bool enabled)
{
	int i;

	profile->enabled = enabled;

	for (i = 0; i < ACPI_SAR_TABLE_SIZE; i++) {
		if (table[i].type != ACPI_TYPE_INTEGER ||
		    table[i].integer.value > U8_MAX)
			return -EINVAL;

		profile->table[i] = table[i].integer.value;
	}

	return 0;
}

static int iwl_sar_fill_table(struct iwl_fw_runtime *fwrt,
			      __le16 *per_chain, u32 n_subbands,
			      int prof_a, int prof_b)
{
	int profs[ACPI_SAR_NUM_CHAIN_LIMITS] = { prof_a, prof_b };
	int i, j, idx;

	for (i = 0; i < ACPI_SAR_NUM_CHAIN_LIMITS; i++) {
		struct iwl_sar_profile *prof;

		/* don't allow SAR to be disabled (profile 0 means disable) */
		if (profs[i] == 0)
			return -EPERM;

		/* we are off by one, so allow up to ACPI_SAR_PROFILE_NUM */
		if (profs[i] > ACPI_SAR_PROFILE_NUM)
			return -EINVAL;

		/* profiles go from 1 to 4, so decrement to access the array */
		prof = &fwrt->sar_profiles[profs[i] - 1];

		/* if the profile is disabled, do nothing */
		if (!prof->enabled) {
			IWL_DEBUG_RADIO(fwrt, "SAR profile %d is disabled.\n",
					profs[i]);
			/*
			 * if one of the profiles is disabled, we
			 * ignore all of them and return 1 to
			 * differentiate disabled from other failures.
			 */
			return 1;
		}

		IWL_DEBUG_INFO(fwrt,
			       "SAR EWRD: chain %d profile index %d\n",
			       i, profs[i]);
		IWL_DEBUG_RADIO(fwrt, "  Chain[%d]:\n", i);
		for (j = 0; j < n_subbands; j++) {
			idx = i * ACPI_SAR_NUM_SUB_BANDS + j;
			per_chain[i * n_subbands + j] =
				cpu_to_le16(prof->table[idx]);
			IWL_DEBUG_RADIO(fwrt, "    Band[%d] = %d * .125dBm\n",
					j, prof->table[idx]);
		}
	}

	return 0;
}

int iwl_sar_select_profile(struct iwl_fw_runtime *fwrt,
			   __le16 *per_chain, u32 n_tables, u32 n_subbands,
			   int prof_a, int prof_b)
{
	int i, ret = 0;

	for (i = 0; i < n_tables; i++) {
		ret = iwl_sar_fill_table(fwrt,
			 &per_chain[i * n_subbands * ACPI_SAR_NUM_CHAIN_LIMITS],
			 n_subbands, prof_a, prof_b);
		if (ret)
			break;
	}

	return ret;
}
IWL_EXPORT_SYMBOL(iwl_sar_select_profile);

int iwl_sar_get_wrds_table(struct iwl_fw_runtime *fwrt)
{
	union acpi_object *wifi_pkg, *table, *data;
	bool enabled;
	int ret, tbl_rev;

	data = iwl_acpi_get_object(fwrt->dev, ACPI_WRDS_METHOD);
	if (IS_ERR(data))
		return PTR_ERR(data);

	wifi_pkg = iwl_acpi_get_wifi_pkg(fwrt->dev, data,
					 ACPI_WRDS_WIFI_DATA_SIZE, &tbl_rev);
	if (IS_ERR(wifi_pkg)) {
		ret = PTR_ERR(wifi_pkg);
		goto out_free;
	}

	if (tbl_rev != 0) {
		ret = -EINVAL;
		goto out_free;
	}

	if (wifi_pkg->package.elements[1].type != ACPI_TYPE_INTEGER) {
		ret = -EINVAL;
		goto out_free;
	}

	enabled = !!(wifi_pkg->package.elements[1].integer.value);

	/* position of the actual table */
	table = &wifi_pkg->package.elements[2];

	/* The profile from WRDS is officially profile 1, but goes
	 * into sar_profiles[0] (because we don't have a profile 0).
	 */
	ret = iwl_sar_set_profile(table, &fwrt->sar_profiles[0], enabled);
out_free:
	kfree(data);
	return ret;
}
IWL_EXPORT_SYMBOL(iwl_sar_get_wrds_table);

int iwl_sar_get_ewrd_table(struct iwl_fw_runtime *fwrt)
{
	union acpi_object *wifi_pkg, *data;
	bool enabled;
	int i, n_profiles, tbl_rev, pos;
	int ret = 0;

	data = iwl_acpi_get_object(fwrt->dev, ACPI_EWRD_METHOD);
	if (IS_ERR(data))
		return PTR_ERR(data);

	wifi_pkg = iwl_acpi_get_wifi_pkg(fwrt->dev, data,
					 ACPI_EWRD_WIFI_DATA_SIZE, &tbl_rev);
	if (IS_ERR(wifi_pkg)) {
		ret = PTR_ERR(wifi_pkg);
		goto out_free;
	}

	if (tbl_rev != 0) {
		ret = -EINVAL;
		goto out_free;
	}

	if (wifi_pkg->package.elements[1].type != ACPI_TYPE_INTEGER ||
	    wifi_pkg->package.elements[2].type != ACPI_TYPE_INTEGER) {
		ret = -EINVAL;
		goto out_free;
	}

	enabled = !!(wifi_pkg->package.elements[1].integer.value);
	n_profiles = wifi_pkg->package.elements[2].integer.value;

	/*
	 * Check the validity of n_profiles.  The EWRD profiles start
	 * from index 1, so the maximum value allowed here is
	 * ACPI_SAR_PROFILES_NUM - 1.
	 */
	if (n_profiles <= 0 || n_profiles >= ACPI_SAR_PROFILE_NUM) {
		ret = -EINVAL;
		goto out_free;
	}

	/* the tables start at element 3 */
	pos = 3;

	for (i = 0; i < n_profiles; i++) {
		/* The EWRD profiles officially go from 2 to 4, but we
		 * save them in sar_profiles[1-3] (because we don't
		 * have profile 0).  So in the array we start from 1.
		 */
		ret = iwl_sar_set_profile(&wifi_pkg->package.elements[pos],
					  &fwrt->sar_profiles[i + 1],
					  enabled);
		if (ret < 0)
			break;

		/* go to the next table */
		pos += ACPI_SAR_TABLE_SIZE;
	}

out_free:
	kfree(data);
	return ret;
}
IWL_EXPORT_SYMBOL(iwl_sar_get_ewrd_table);

int iwl_sar_get_wgds_table(struct iwl_fw_runtime *fwrt)
{
	union acpi_object *wifi_pkg, *data;
	int i, j, ret, tbl_rev;
	int idx = 1;

	data = iwl_acpi_get_object(fwrt->dev, ACPI_WGDS_METHOD);
	if (IS_ERR(data))
		return PTR_ERR(data);

	wifi_pkg = iwl_acpi_get_wifi_pkg(fwrt->dev, data,
					 ACPI_WGDS_WIFI_DATA_SIZE, &tbl_rev);

	if (IS_ERR(wifi_pkg)) {
		ret = PTR_ERR(wifi_pkg);
		goto out_free;
	}

	if (tbl_rev > 1) {
		ret = -EINVAL;
		goto out_free;
	}

	fwrt->geo_rev = tbl_rev;
	for (i = 0; i < ACPI_NUM_GEO_PROFILES; i++) {
		for (j = 0; j < ACPI_GEO_TABLE_SIZE; j++) {
			union acpi_object *entry;

			entry = &wifi_pkg->package.elements[idx++];
			if (entry->type != ACPI_TYPE_INTEGER ||
			    entry->integer.value > U8_MAX) {
				ret = -EINVAL;
				goto out_free;
			}

			fwrt->geo_profiles[i].values[j] = entry->integer.value;
		}
	}
	ret = 0;
out_free:
	kfree(data);
	return ret;
}
IWL_EXPORT_SYMBOL(iwl_sar_get_wgds_table);

bool iwl_sar_geo_support(struct iwl_fw_runtime *fwrt)
{
	/*
	 * The GEO_TX_POWER_LIMIT command is not supported on earlier
	 * firmware versions.  Unfortunately, we don't have a TLV API
	 * flag to rely on, so rely on the major version which is in
	 * the first byte of ucode_ver.  This was implemented
	 * initially on version 38 and then backported to 17.  It was
	 * also backported to 29, but only for 7265D devices.  The
	 * intention was to have it in 36 as well, but not all 8000
	 * family got this feature enabled.  The 8000 family is the
	 * only one using version 36, so skip this version entirely.
	 */
	return IWL_UCODE_SERIAL(fwrt->fw->ucode_ver) >= 38 ||
	       IWL_UCODE_SERIAL(fwrt->fw->ucode_ver) == 17 ||
	       (IWL_UCODE_SERIAL(fwrt->fw->ucode_ver) == 29 &&
		((fwrt->trans->hw_rev & CSR_HW_REV_TYPE_MSK) ==
		 CSR_HW_REV_TYPE_7265D));
}
IWL_EXPORT_SYMBOL(iwl_sar_geo_support);

int iwl_sar_geo_init(struct iwl_fw_runtime *fwrt,
		     struct iwl_per_chain_offset *table, u32 n_bands)
{
	int ret, i, j;

	if (!iwl_sar_geo_support(fwrt))
		return -EOPNOTSUPP;

	ret = iwl_sar_get_wgds_table(fwrt);
	if (ret < 0) {
		IWL_DEBUG_RADIO(fwrt,
				"Geo SAR BIOS table invalid or unavailable. (%d)\n",
				ret);
		/* we don't fail if the table is not available */
		return -ENOENT;
	}

	for (i = 0; i < ACPI_NUM_GEO_PROFILES; i++) {
		for (j = 0; j < n_bands; j++) {
			struct iwl_per_chain_offset *chain =
				&table[i * n_bands + j];
			u8 *value;

			if (j * ACPI_GEO_PER_CHAIN_SIZE >=
			    ARRAY_SIZE(fwrt->geo_profiles[0].values))
				/*
				 * Currently we only store lb an hb values, and
				 * don't have any special ones for uhb. So leave
				 * those empty for the time being
				 */
				break;

			value = &fwrt->geo_profiles[i].values[j *
				ACPI_GEO_PER_CHAIN_SIZE];
			chain->max_tx_power = cpu_to_le16(value[0]);
			chain->chain_a = value[1];
			chain->chain_b = value[2];
			IWL_DEBUG_RADIO(fwrt,
					"SAR geographic profile[%d] Band[%d]: chain A = %d chain B = %d max_tx_power = %d\n",
					i, j, value[1], value[2], value[0]);
		}
	}

	return 0;
}
IWL_EXPORT_SYMBOL(iwl_sar_geo_init);

static u32 iwl_acpi_eval_dsm_func(struct device *dev, enum iwl_dsm_funcs_rev_0 eval_func)
{
	union acpi_object *obj;
	u32 ret;

	obj = iwl_acpi_get_dsm_object(dev, 0,
				      eval_func, NULL,
				      &iwl_guid);

	if (IS_ERR(obj)) {
		IWL_DEBUG_DEV_RADIO(dev,
				    "ACPI: DSM func '%d': Got Error in obj = %ld\n",
				    eval_func,
				    PTR_ERR(obj));
		return 0;
	}

	if (obj->type != ACPI_TYPE_INTEGER) {
		IWL_DEBUG_DEV_RADIO(dev,
				    "ACPI: DSM func '%d' did not return a valid object, type=%d\n",
				    eval_func,
				    obj->type);
		ret = 0;
		goto out;
	}

	ret = obj->integer.value;
	IWL_DEBUG_DEV_RADIO(dev,
			    "ACPI: DSM method evaluated: func='%d', ret=%d\n",
			    eval_func,
			    ret);
out:
	ACPI_FREE(obj);
	return ret;
}

__le32 iwl_acpi_get_lari_config_bitmap(struct iwl_fw_runtime *fwrt)
{
	u32 ret;
	__le32 config_bitmap = 0;

	/*
	 ** Evaluate func 'DSM_FUNC_ENABLE_INDONESIA_5G2'
	 */
	ret = iwl_acpi_eval_dsm_func(fwrt->dev, DSM_FUNC_ENABLE_INDONESIA_5G2);

	if (ret == DSM_VALUE_INDONESIA_ENABLE)
		config_bitmap |=
			cpu_to_le32(LARI_CONFIG_ENABLE_5G2_IN_INDONESIA_MSK);

	/*
	 ** Evaluate func 'DSM_FUNC_DISABLE_SRD'
	 */
	ret = iwl_acpi_eval_dsm_func(fwrt->dev, DSM_FUNC_DISABLE_SRD);

	if (ret == DSM_VALUE_SRD_PASSIVE)
		config_bitmap |=
			cpu_to_le32(LARI_CONFIG_CHANGE_ETSI_TO_PASSIVE_MSK);

	else if (ret == DSM_VALUE_SRD_DISABLE)
		config_bitmap |=
			cpu_to_le32(LARI_CONFIG_CHANGE_ETSI_TO_DISABLED_MSK);

	return config_bitmap;
}
IWL_EXPORT_SYMBOL(iwl_acpi_get_lari_config_bitmap);
