// SPDX-License-Identifier: GPL-2.0+
// Copyright (c) 2016-2017 Hisilicon Limited.

#include "hclge_mbx.h"
#include "hclgevf_main.h"
#include "hnae3.h"

#define CREATE_TRACE_POINTS
#include "hclgevf_trace.h"

static int hclgevf_resp_to_errno(u16 resp_code)
{
	return resp_code ? -resp_code : 0;
}

static void hclgevf_reset_mbx_resp_status(struct hclgevf_dev *hdev)
{
	/* this function should be called with mbx_resp.mbx_mutex held
	 * to prtect the received_response from race condition
	 */
	hdev->mbx_resp.received_resp  = false;
	hdev->mbx_resp.origin_mbx_msg = 0;
	hdev->mbx_resp.resp_status    = 0;
	memset(hdev->mbx_resp.additional_info, 0, HCLGE_MBX_MAX_RESP_DATA_SIZE);
}

/* hclgevf_get_mbx_resp: used to get a response from PF after VF sends a mailbox
 * message to PF.
 * @hdev: pointer to struct hclgevf_dev
 * @resp_msg: pointer to store the original message type and response status
 * @len: the resp_msg data array length.
 */
static int hclgevf_get_mbx_resp(struct hclgevf_dev *hdev, u16 code0, u16 code1,
				u8 *resp_data, u16 resp_len)
{
#define HCLGEVF_MAX_TRY_TIMES	500
#define HCLGEVF_SLEEP_USECOND	1000
	struct hclgevf_mbx_resp_status *mbx_resp;
	u16 r_code0, r_code1;
	int i = 0;

	if (resp_len > HCLGE_MBX_MAX_RESP_DATA_SIZE) {
		dev_err(&hdev->pdev->dev,
			"VF mbx response len(=%u) exceeds maximum(=%u)\n",
			resp_len,
			HCLGE_MBX_MAX_RESP_DATA_SIZE);
		return -EINVAL;
	}

	while ((!hdev->mbx_resp.received_resp) && (i < HCLGEVF_MAX_TRY_TIMES)) {
		if (test_bit(HCLGEVF_STATE_CMD_DISABLE, &hdev->state))
			return -EIO;

		usleep_range(HCLGEVF_SLEEP_USECOND, HCLGEVF_SLEEP_USECOND * 2);
		i++;
	}

	if (i >= HCLGEVF_MAX_TRY_TIMES) {
		dev_err(&hdev->pdev->dev,
			"VF could not get mbx(%u,%u) resp(=%d) from PF in %d tries\n",
			code0, code1, hdev->mbx_resp.received_resp, i);
		return -EIO;
	}

	mbx_resp = &hdev->mbx_resp;
	r_code0 = (u16)(mbx_resp->origin_mbx_msg >> 16);
	r_code1 = (u16)(mbx_resp->origin_mbx_msg & 0xff);

	if (mbx_resp->resp_status)
		return mbx_resp->resp_status;

	if (resp_data)
		memcpy(resp_data, &mbx_resp->additional_info[0], resp_len);

	hclgevf_reset_mbx_resp_status(hdev);

	if (!(r_code0 == code0 && r_code1 == code1 && !mbx_resp->resp_status)) {
		dev_err(&hdev->pdev->dev,
			"VF could not match resp code(code0=%u,code1=%u), %d\n",
			code0, code1, mbx_resp->resp_status);
		dev_err(&hdev->pdev->dev,
			"VF could not match resp r_code(r_code0=%u,r_code1=%u)\n",
			r_code0, r_code1);
		return -EIO;
	}

	return 0;
}

int hclgevf_send_mbx_msg(struct hclgevf_dev *hdev,
			 struct hclge_vf_to_pf_msg *send_msg, bool need_resp,
			 u8 *resp_data, u16 resp_len)
{
	struct hclge_mbx_vf_to_pf_cmd *req;
	struct hclgevf_desc desc;
	int status;

	req = (struct hclge_mbx_vf_to_pf_cmd *)desc.data;

	if (!send_msg) {
		dev_err(&hdev->pdev->dev,
			"failed to send mbx, msg is NULL\n");
		return -EINVAL;
	}

	hclgevf_cmd_setup_basic_desc(&desc, HCLGEVF_OPC_MBX_VF_TO_PF, false);
	if (need_resp)
		hnae3_set_bit(req->mbx_need_resp, HCLGE_MBX_NEED_RESP_B, 1);

	memcpy(&req->msg, send_msg, sizeof(struct hclge_vf_to_pf_msg));

	trace_hclge_vf_mbx_send(hdev, req);

	/* synchronous send */
	if (need_resp) {
		mutex_lock(&hdev->mbx_resp.mbx_mutex);
		hclgevf_reset_mbx_resp_status(hdev);
		status = hclgevf_cmd_send(&hdev->hw, &desc, 1);
		if (status) {
			dev_err(&hdev->pdev->dev,
				"VF failed(=%d) to send mbx message to PF\n",
				status);
			mutex_unlock(&hdev->mbx_resp.mbx_mutex);
			return status;
		}

		status = hclgevf_get_mbx_resp(hdev, send_msg->code,
					      send_msg->subcode, resp_data,
					      resp_len);
		mutex_unlock(&hdev->mbx_resp.mbx_mutex);
	} else {
		/* asynchronous send */
		status = hclgevf_cmd_send(&hdev->hw, &desc, 1);
		if (status) {
			dev_err(&hdev->pdev->dev,
				"VF failed(=%d) to send mbx message to PF\n",
				status);
			return status;
		}
	}

	return status;
}

static bool hclgevf_cmd_crq_empty(struct hclgevf_hw *hw)
{
	u32 tail = hclgevf_read_dev(hw, HCLGEVF_NIC_CRQ_TAIL_REG);

	return tail == hw->cmq.crq.next_to_use;
}

void hclgevf_mbx_handler(struct hclgevf_dev *hdev)
{
	struct hclgevf_mbx_resp_status *resp;
	struct hclge_mbx_pf_to_vf_cmd *req;
	struct hclgevf_cmq_ring *crq;
	struct hclgevf_desc *desc;
	u16 *msg_q;
	u16 flag;
	u8 *temp;
	int i;

	resp = &hdev->mbx_resp;
	crq = &hdev->hw.cmq.crq;

	while (!hclgevf_cmd_crq_empty(&hdev->hw)) {
		if (test_bit(HCLGEVF_STATE_CMD_DISABLE, &hdev->state)) {
			dev_info(&hdev->pdev->dev, "vf crq need init\n");
			return;
		}

		desc = &crq->desc[crq->next_to_use];
		req = (struct hclge_mbx_pf_to_vf_cmd *)desc->data;

		flag = le16_to_cpu(crq->desc[crq->next_to_use].flag);
		if (unlikely(!hnae3_get_bit(flag, HCLGEVF_CMDQ_RX_OUTVLD_B))) {
			dev_warn(&hdev->pdev->dev,
				 "dropped invalid mailbox message, code = %u\n",
				 req->msg.code);

			/* dropping/not processing this invalid message */
			crq->desc[crq->next_to_use].flag = 0;
			hclge_mbx_ring_ptr_move_crq(crq);
			continue;
		}

		trace_hclge_vf_mbx_get(hdev, req);

		/* synchronous messages are time critical and need preferential
		 * treatment. Therefore, we need to acknowledge all the sync
		 * responses as quickly as possible so that waiting tasks do not
		 * timeout and simultaneously queue the async messages for later
		 * prcessing in context of mailbox task i.e. the slow path.
		 */
		switch (req->msg.code) {
		case HCLGE_MBX_PF_VF_RESP:
			if (resp->received_resp)
				dev_warn(&hdev->pdev->dev,
					 "VF mbx resp flag not clear(%u)\n",
					 req->msg.vf_mbx_msg_code);
			resp->received_resp = true;

			resp->origin_mbx_msg =
					(req->msg.vf_mbx_msg_code << 16);
			resp->origin_mbx_msg |= req->msg.vf_mbx_msg_subcode;
			resp->resp_status =
				hclgevf_resp_to_errno(req->msg.resp_status);

			temp = (u8 *)req->msg.resp_data;
			for (i = 0; i < HCLGE_MBX_MAX_RESP_DATA_SIZE; i++) {
				resp->additional_info[i] = *temp;
				temp++;
			}
			break;
		case HCLGE_MBX_LINK_STAT_CHANGE:
		case HCLGE_MBX_ASSERTING_RESET:
		case HCLGE_MBX_LINK_STAT_MODE:
		case HCLGE_MBX_PUSH_VLAN_INFO:
		case HCLGE_MBX_PUSH_PROMISC_INFO:
			/* set this mbx event as pending. This is required as we
			 * might loose interrupt event when mbx task is busy
			 * handling. This shall be cleared when mbx task just
			 * enters handling state.
			 */
			hdev->mbx_event_pending = true;

			/* we will drop the async msg if we find ARQ as full
			 * and continue with next message
			 */
			if (atomic_read(&hdev->arq.count) >=
			    HCLGE_MBX_MAX_ARQ_MSG_NUM) {
				dev_warn(&hdev->pdev->dev,
					 "Async Q full, dropping msg(%u)\n",
					 req->msg.code);
				break;
			}

			/* tail the async message in arq */
			msg_q = hdev->arq.msg_q[hdev->arq.tail];
			memcpy(&msg_q[0], &req->msg,
			       HCLGE_MBX_MAX_ARQ_MSG_SIZE * sizeof(u16));
			hclge_mbx_tail_ptr_move_arq(hdev->arq);
			atomic_inc(&hdev->arq.count);

			hclgevf_mbx_task_schedule(hdev);

			break;
		default:
			dev_err(&hdev->pdev->dev,
				"VF received unsupported(%u) mbx msg from PF\n",
				req->msg.code);
			break;
		}
		crq->desc[crq->next_to_use].flag = 0;
		hclge_mbx_ring_ptr_move_crq(crq);
	}

	/* Write back CMDQ_RQ header pointer, M7 need this pointer */
	hclgevf_write_dev(&hdev->hw, HCLGEVF_NIC_CRQ_HEAD_REG,
			  crq->next_to_use);
}

static void hclgevf_parse_promisc_info(struct hclgevf_dev *hdev,
				       u16 promisc_info)
{
	if (!promisc_info)
		dev_info(&hdev->pdev->dev,
			 "Promisc mode is closed by host for being untrusted.\n");
}

void hclgevf_mbx_async_handler(struct hclgevf_dev *hdev)
{
	enum hnae3_reset_type reset_type;
	u16 link_status, state;
	u16 *msg_q, *vlan_info;
	u8 duplex;
	u32 speed;
	u32 tail;
	u8 flag;
	u8 idx;

	/* we can safely clear it now as we are at start of the async message
	 * processing
	 */
	hdev->mbx_event_pending = false;

	tail = hdev->arq.tail;

	/* process all the async queue messages */
	while (tail != hdev->arq.head) {
		if (test_bit(HCLGEVF_STATE_CMD_DISABLE, &hdev->state)) {
			dev_info(&hdev->pdev->dev,
				 "vf crq need init in async\n");
			return;
		}

		msg_q = hdev->arq.msg_q[hdev->arq.head];

		switch (msg_q[0]) {
		case HCLGE_MBX_LINK_STAT_CHANGE:
			link_status = msg_q[1];
			memcpy(&speed, &msg_q[2], sizeof(speed));
			duplex = (u8)msg_q[4];
			flag = (u8)msg_q[5];

			/* update upper layer with new link link status */
			hclgevf_update_link_status(hdev, link_status);
			hclgevf_update_speed_duplex(hdev, speed, duplex);

			if (flag & HCLGE_MBX_PUSH_LINK_STATUS_EN)
				set_bit(HCLGEVF_STATE_PF_PUSH_LINK_STATUS,
					&hdev->state);

			break;
		case HCLGE_MBX_LINK_STAT_MODE:
			idx = (u8)msg_q[1];
			if (idx)
				memcpy(&hdev->hw.mac.supported, &msg_q[2],
				       sizeof(unsigned long));
			else
				memcpy(&hdev->hw.mac.advertising, &msg_q[2],
				       sizeof(unsigned long));
			break;
		case HCLGE_MBX_ASSERTING_RESET:
			/* PF has asserted reset hence VF should go in pending
			 * state and poll for the hardware reset status till it
			 * has been completely reset. After this stack should
			 * eventually be re-initialized.
			 */
			reset_type = (enum hnae3_reset_type)msg_q[1];
			set_bit(reset_type, &hdev->reset_pending);
			set_bit(HCLGEVF_RESET_PENDING, &hdev->reset_state);
			hclgevf_reset_task_schedule(hdev);

			break;
		case HCLGE_MBX_PUSH_VLAN_INFO:
			state = msg_q[1];
			vlan_info = &msg_q[1];
			hclgevf_update_port_base_vlan_info(hdev, state,
							   (u8 *)vlan_info, 8);
			break;
		case HCLGE_MBX_PUSH_PROMISC_INFO:
			hclgevf_parse_promisc_info(hdev, msg_q[1]);
			break;
		default:
			dev_err(&hdev->pdev->dev,
				"fetched unsupported(%u) message from arq\n",
				msg_q[0]);
			break;
		}

		hclge_mbx_head_ptr_move_arq(hdev->arq);
		atomic_dec(&hdev->arq.count);
		msg_q = hdev->arq.msg_q[hdev->arq.head];
	}
}
