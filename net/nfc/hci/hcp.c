// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2012  Intel Corporation. All rights reserved.
 */

#define pr_fmt(fmt) "hci: %s: " fmt, __func__

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <net/nfc/hci.h>

#include "hci.h"

/*
 * Payload is the HCP message data only. Instruction will be prepended.
 * Guarantees that cb will be called upon completion or timeout delay
 * counted from the moment the cmd is sent to the transport.
 */
int nfc_hci_hcp_message_tx(struct nfc_hci_dev *hdev, u8 pipe,
			   u8 type, u8 instruction,
			   const u8 *payload, size_t payload_len,
			   data_exchange_cb_t cb, void *cb_context,
			   unsigned long completion_delay)
{
	struct nfc_dev *ndev = hdev->ndev;
	struct hci_msg *cmd;
	const u8 *ptr = payload;
	int hci_len, err;
	bool firstfrag = true;

	cmd = kzalloc(sizeof(struct hci_msg), GFP_KERNEL);
	if (cmd == NULL)
		return -ENOMEM;

	INIT_LIST_HEAD(&cmd->msg_l);
	skb_queue_head_init(&cmd->msg_frags);
	cmd->wait_response = (type == NFC_HCI_HCP_COMMAND) ? true : false;
	cmd->cb = cb;
	cmd->cb_context = cb_context;
	cmd->completion_delay = completion_delay;

	hci_len = payload_len + 1;
	while (hci_len > 0) {
		struct sk_buff *skb;
		int skb_len, data_link_len;
		struct hcp_packet *packet;

		if (NFC_HCI_HCP_PACKET_HEADER_LEN + hci_len <=
		    hdev->max_data_link_payload)
			data_link_len = hci_len;
		else
			data_link_len = hdev->max_data_link_payload -
					NFC_HCI_HCP_PACKET_HEADER_LEN;

		skb_len = ndev->tx_headroom + NFC_HCI_HCP_PACKET_HEADER_LEN +
			  data_link_len + ndev->tx_tailroom;
		hci_len -= data_link_len;

		skb = alloc_skb(skb_len, GFP_KERNEL);
		if (skb == NULL) {
			err = -ENOMEM;
			goto out_skb_err;
		}
		skb_reserve(skb, ndev->tx_headroom);

		skb_put(skb, NFC_HCI_HCP_PACKET_HEADER_LEN + data_link_len);

		/* Only the last fragment will have the cb bit set to 1 */
		packet = (struct hcp_packet *)skb->data;
		packet->header = pipe;
		if (firstfrag) {
			firstfrag = false;
			packet->message.header = HCP_HEADER(type, instruction);
			if (ptr) {
				memcpy(packet->message.data, ptr,
				       data_link_len - 1);
				ptr += data_link_len - 1;
			}
		} else {
			memcpy(&packet->message, ptr, data_link_len);
			ptr += data_link_len;
		}

		/* This is the last fragment, set the cb bit */
		if (hci_len == 0)
			packet->header |= ~NFC_HCI_FRAGMENT;

		skb_queue_tail(&cmd->msg_frags, skb);
	}

	mutex_lock(&hdev->msg_tx_mutex);

	if (hdev->shutting_down) {
		err = -ESHUTDOWN;
		mutex_unlock(&hdev->msg_tx_mutex);
		goto out_skb_err;
	}

	list_add_tail(&cmd->msg_l, &hdev->msg_tx_queue);
	mutex_unlock(&hdev->msg_tx_mutex);

	schedule_work(&hdev->msg_tx_work);

	return 0;

out_skb_err:
	skb_queue_purge(&cmd->msg_frags);
	kfree(cmd);

	return err;
}

/*
 * Receive hcp message for pipe, with type and cmd.
 * skb contains optional message data only.
 */
void nfc_hci_hcp_message_rx(struct nfc_hci_dev *hdev, u8 pipe, u8 type,
			    u8 instruction, struct sk_buff *skb)
{
	switch (type) {
	case NFC_HCI_HCP_RESPONSE:
		nfc_hci_resp_received(hdev, instruction, skb);
		break;
	case NFC_HCI_HCP_COMMAND:
		nfc_hci_cmd_received(hdev, pipe, instruction, skb);
		break;
	case NFC_HCI_HCP_EVENT:
		nfc_hci_event_received(hdev, pipe, instruction, skb);
		break;
	default:
		pr_err("UNKNOWN MSG Type %d, instruction=%d\n",
		       type, instruction);
		kfree_skb(skb);
		break;
	}
}
