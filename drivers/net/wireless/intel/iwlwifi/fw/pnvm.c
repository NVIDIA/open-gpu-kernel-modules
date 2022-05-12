// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/*
 * Copyright(c) 2020-2021 Intel Corporation
 */

#include "iwl-drv.h"
#include "pnvm.h"
#include "iwl-prph.h"
#include "iwl-io.h"
#include "fw/api/commands.h"
#include "fw/api/nvm-reg.h"
#include "fw/api/alive.h"
#include <linux/efi.h>

struct iwl_pnvm_section {
	__le32 offset;
	const u8 data[];
} __packed;

static bool iwl_pnvm_complete_fn(struct iwl_notif_wait_data *notif_wait,
				 struct iwl_rx_packet *pkt, void *data)
{
	struct iwl_trans *trans = (struct iwl_trans *)data;
	struct iwl_pnvm_init_complete_ntfy *pnvm_ntf = (void *)pkt->data;

	IWL_DEBUG_FW(trans,
		     "PNVM complete notification received with status %d\n",
		     le32_to_cpu(pnvm_ntf->status));

	return true;
}

static int iwl_pnvm_handle_section(struct iwl_trans *trans, const u8 *data,
				   size_t len)
{
	struct iwl_ucode_tlv *tlv;
	u32 sha1 = 0;
	u16 mac_type = 0, rf_id = 0;
	u8 *pnvm_data = NULL, *tmp;
	u32 size = 0;
	int ret;

	IWL_DEBUG_FW(trans, "Handling PNVM section\n");

	while (len >= sizeof(*tlv)) {
		u32 tlv_len, tlv_type;

		len -= sizeof(*tlv);
		tlv = (void *)data;

		tlv_len = le32_to_cpu(tlv->length);
		tlv_type = le32_to_cpu(tlv->type);

		if (len < tlv_len) {
			IWL_ERR(trans, "invalid TLV len: %zd/%u\n",
				len, tlv_len);
			ret = -EINVAL;
			goto out;
		}

		data += sizeof(*tlv);

		switch (tlv_type) {
		case IWL_UCODE_TLV_PNVM_VERSION:
			if (tlv_len < sizeof(__le32)) {
				IWL_DEBUG_FW(trans,
					     "Invalid size for IWL_UCODE_TLV_PNVM_VERSION (expected %zd, got %d)\n",
					     sizeof(__le32), tlv_len);
				break;
			}

			sha1 = le32_to_cpup((__le32 *)data);

			IWL_DEBUG_FW(trans,
				     "Got IWL_UCODE_TLV_PNVM_VERSION %0x\n",
				     sha1);
			break;
		case IWL_UCODE_TLV_HW_TYPE:
			if (tlv_len < 2 * sizeof(__le16)) {
				IWL_DEBUG_FW(trans,
					     "Invalid size for IWL_UCODE_TLV_HW_TYPE (expected %zd, got %d)\n",
					     2 * sizeof(__le16), tlv_len);
				break;
			}

			mac_type = le16_to_cpup((__le16 *)data);
			rf_id = le16_to_cpup((__le16 *)(data + sizeof(__le16)));

			IWL_DEBUG_FW(trans,
				     "Got IWL_UCODE_TLV_HW_TYPE mac_type 0x%0x rf_id 0x%0x\n",
				     mac_type, rf_id);

			if (mac_type != CSR_HW_REV_TYPE(trans->hw_rev) ||
			    rf_id != CSR_HW_RFID_TYPE(trans->hw_rf_id)) {
				IWL_DEBUG_FW(trans,
					     "HW mismatch, skipping PNVM section, mac_type 0x%0x, rf_id 0x%0x.\n",
					     CSR_HW_REV_TYPE(trans->hw_rev), trans->hw_rf_id);
				ret = -ENOENT;
				goto out;
			}

			break;
		case IWL_UCODE_TLV_SEC_RT: {
			struct iwl_pnvm_section *section = (void *)data;
			u32 data_len = tlv_len - sizeof(*section);

			IWL_DEBUG_FW(trans,
				     "Got IWL_UCODE_TLV_SEC_RT len %d\n",
				     tlv_len);

			/* TODO: remove, this is a deprecated separator */
			if (le32_to_cpup((__le32 *)data) == 0xddddeeee) {
				IWL_DEBUG_FW(trans, "Ignoring separator.\n");
				break;
			}

			IWL_DEBUG_FW(trans, "Adding data (size %d)\n",
				     data_len);

			tmp = krealloc(pnvm_data, size + data_len, GFP_KERNEL);
			if (!tmp) {
				IWL_DEBUG_FW(trans,
					     "Couldn't allocate (more) pnvm_data\n");

				ret = -ENOMEM;
				goto out;
			}

			pnvm_data = tmp;

			memcpy(pnvm_data + size, section->data, data_len);

			size += data_len;

			break;
		}
		case IWL_UCODE_TLV_PNVM_SKU:
			IWL_DEBUG_FW(trans,
				     "New PNVM section started, stop parsing.\n");
			goto done;
		default:
			IWL_DEBUG_FW(trans, "Found TLV 0x%0x, len %d\n",
				     tlv_type, tlv_len);
			break;
		}

		len -= ALIGN(tlv_len, 4);
		data += ALIGN(tlv_len, 4);
	}

done:
	if (!size) {
		IWL_DEBUG_FW(trans, "Empty PNVM, skipping.\n");
		ret = -ENOENT;
		goto out;
	}

	IWL_INFO(trans, "loaded PNVM version 0x%0x\n", sha1);

	ret = iwl_trans_set_pnvm(trans, pnvm_data, size);
out:
	kfree(pnvm_data);
	return ret;
}

static int iwl_pnvm_parse(struct iwl_trans *trans, const u8 *data,
			  size_t len)
{
	struct iwl_ucode_tlv *tlv;

	IWL_DEBUG_FW(trans, "Parsing PNVM file\n");

	while (len >= sizeof(*tlv)) {
		u32 tlv_len, tlv_type;

		len -= sizeof(*tlv);
		tlv = (void *)data;

		tlv_len = le32_to_cpu(tlv->length);
		tlv_type = le32_to_cpu(tlv->type);

		if (len < tlv_len) {
			IWL_ERR(trans, "invalid TLV len: %zd/%u\n",
				len, tlv_len);
			return -EINVAL;
		}

		if (tlv_type == IWL_UCODE_TLV_PNVM_SKU) {
			struct iwl_sku_id *sku_id =
				(void *)(data + sizeof(*tlv));

			IWL_DEBUG_FW(trans,
				     "Got IWL_UCODE_TLV_PNVM_SKU len %d\n",
				     tlv_len);
			IWL_DEBUG_FW(trans, "sku_id 0x%0x 0x%0x 0x%0x\n",
				     le32_to_cpu(sku_id->data[0]),
				     le32_to_cpu(sku_id->data[1]),
				     le32_to_cpu(sku_id->data[2]));

			data += sizeof(*tlv) + ALIGN(tlv_len, 4);
			len -= ALIGN(tlv_len, 4);

			if (trans->sku_id[0] == le32_to_cpu(sku_id->data[0]) &&
			    trans->sku_id[1] == le32_to_cpu(sku_id->data[1]) &&
			    trans->sku_id[2] == le32_to_cpu(sku_id->data[2])) {
				int ret;

				ret = iwl_pnvm_handle_section(trans, data, len);
				if (!ret)
					return 0;
			} else {
				IWL_DEBUG_FW(trans, "SKU ID didn't match!\n");
			}
		} else {
			data += sizeof(*tlv) + ALIGN(tlv_len, 4);
			len -= ALIGN(tlv_len, 4);
		}
	}

	return -ENOENT;
}

#if defined(CONFIG_EFI)

#define IWL_EFI_VAR_GUID EFI_GUID(0x92daaf2f, 0xc02b, 0x455b,	\
				  0xb2, 0xec, 0xf5, 0xa3,	\
				  0x59, 0x4f, 0x4a, 0xea)

#define IWL_UEFI_OEM_PNVM_NAME	L"UefiCnvWlanOemSignedPnvm"

#define IWL_HARDCODED_PNVM_SIZE 4096

struct pnvm_sku_package {
	u8 rev;
	u8 reserved1[3];
	u32 total_size;
	u8 n_skus;
	u8 reserved2[11];
	u8 data[];
};

static int iwl_pnvm_get_from_efi(struct iwl_trans *trans,
				 u8 **data, size_t *len)
{
	struct efivar_entry *pnvm_efivar;
	struct pnvm_sku_package *package;
	unsigned long package_size;
	int err;

	pnvm_efivar = kzalloc(sizeof(*pnvm_efivar), GFP_KERNEL);
	if (!pnvm_efivar)
		return -ENOMEM;

	memcpy(&pnvm_efivar->var.VariableName, IWL_UEFI_OEM_PNVM_NAME,
	       sizeof(IWL_UEFI_OEM_PNVM_NAME));
	pnvm_efivar->var.VendorGuid = IWL_EFI_VAR_GUID;

	/*
	 * TODO: we hardcode a maximum length here, because reading
	 * from the UEFI is not working.  To implement this properly,
	 * we have to call efivar_entry_size().
	 */
	package_size = IWL_HARDCODED_PNVM_SIZE;

	package = kmalloc(package_size, GFP_KERNEL);
	if (!package) {
		err = -ENOMEM;
		goto out;
	}

	err = efivar_entry_get(pnvm_efivar, NULL, &package_size, package);
	if (err) {
		IWL_DEBUG_FW(trans,
			     "PNVM UEFI variable not found %d (len %lu)\n",
			     err, package_size);
		goto out;
	}

	IWL_DEBUG_FW(trans, "Read PNVM fro UEFI with size %lu\n", package_size);

	*data = kmemdup(package->data, *len, GFP_KERNEL);
	if (!*data)
		err = -ENOMEM;
	*len = package_size - sizeof(*package);

out:
	kfree(package);
	kfree(pnvm_efivar);

	return err;
}
#else /* CONFIG_EFI */
static inline int iwl_pnvm_get_from_efi(struct iwl_trans *trans,
					u8 **data, size_t *len)
{
	return -EOPNOTSUPP;
}
#endif /* CONFIG_EFI */

static int iwl_pnvm_get_from_fs(struct iwl_trans *trans, u8 **data, size_t *len)
{
	const struct firmware *pnvm;
	char pnvm_name[64];
	int ret;

	/*
	 * The prefix unfortunately includes a hyphen at the end, so
	 * don't add the dot here...
	 */
	snprintf(pnvm_name, sizeof(pnvm_name), "%spnvm",
		 trans->cfg->fw_name_pre);

	/* ...but replace the hyphen with the dot here. */
	if (strlen(trans->cfg->fw_name_pre) < sizeof(pnvm_name))
		pnvm_name[strlen(trans->cfg->fw_name_pre) - 1] = '.';

	ret = firmware_request_nowarn(&pnvm, pnvm_name, trans->dev);
	if (ret) {
		IWL_DEBUG_FW(trans, "PNVM file %s not found %d\n",
			     pnvm_name, ret);
		return ret;
	}

	*data = kmemdup(pnvm->data, pnvm->size, GFP_KERNEL);
	if (!*data)
		return -ENOMEM;

	*len = pnvm->size;

	return 0;
}

int iwl_pnvm_load(struct iwl_trans *trans,
		  struct iwl_notif_wait_data *notif_wait)
{
	u8 *data;
	size_t len;
	struct iwl_notification_wait pnvm_wait;
	static const u16 ntf_cmds[] = { WIDE_ID(REGULATORY_AND_NVM_GROUP,
						PNVM_INIT_COMPLETE_NTFY) };
	int ret;

	/* if the SKU_ID is empty, there's nothing to do */
	if (!trans->sku_id[0] && !trans->sku_id[1] && !trans->sku_id[2])
		return 0;

	/*
	 * If we already loaded (or tried to load) it before, we just
	 * need to set it again.
	 */
	if (trans->pnvm_loaded) {
		ret = iwl_trans_set_pnvm(trans, NULL, 0);
		if (ret)
			return ret;
		goto skip_parse;
	}

	/* First attempt to get the PNVM from BIOS */
	ret = iwl_pnvm_get_from_efi(trans, &data, &len);
	if (!ret)
		goto parse;

	/* If it's not available, try from the filesystem */
	ret = iwl_pnvm_get_from_fs(trans, &data, &len);
	if (ret) {
		/*
		 * Pretend we've loaded it - at least we've tried and
		 * couldn't load it at all, so there's no point in
		 * trying again over and over.
		 */
		trans->pnvm_loaded = true;

		goto skip_parse;
	}

parse:
	iwl_pnvm_parse(trans, data, len);

	kfree(data);

skip_parse:
	iwl_init_notification_wait(notif_wait, &pnvm_wait,
				   ntf_cmds, ARRAY_SIZE(ntf_cmds),
				   iwl_pnvm_complete_fn, trans);

	/* kick the doorbell */
	iwl_write_umac_prph(trans, UREG_DOORBELL_TO_ISR6,
			    UREG_DOORBELL_TO_ISR6_PNVM);

	return iwl_wait_notification(notif_wait, &pnvm_wait,
				     MVM_UCODE_PNVM_TIMEOUT);
}
IWL_EXPORT_SYMBOL(iwl_pnvm_load);
