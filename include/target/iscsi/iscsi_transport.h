/* SPDX-License-Identifier: GPL-2.0 */
#include "iscsi_target_core.h" /* struct iscsi_cmd */

struct sockaddr_storage;

struct iscsit_transport {
#define ISCSIT_TRANSPORT_NAME	16
	char name[ISCSIT_TRANSPORT_NAME];
	int transport_type;
	bool rdma_shutdown;
	int priv_size;
	struct module *owner;
	struct list_head t_node;
	int (*iscsit_setup_np)(struct iscsi_np *, struct sockaddr_storage *);
	int (*iscsit_accept_np)(struct iscsi_np *, struct iscsi_conn *);
	void (*iscsit_free_np)(struct iscsi_np *);
	void (*iscsit_wait_conn)(struct iscsi_conn *);
	void (*iscsit_free_conn)(struct iscsi_conn *);
	int (*iscsit_get_login_rx)(struct iscsi_conn *, struct iscsi_login *);
	int (*iscsit_put_login_tx)(struct iscsi_conn *, struct iscsi_login *, u32);
	int (*iscsit_immediate_queue)(struct iscsi_conn *, struct iscsi_cmd *, int);
	int (*iscsit_response_queue)(struct iscsi_conn *, struct iscsi_cmd *, int);
	int (*iscsit_get_dataout)(struct iscsi_conn *, struct iscsi_cmd *, bool);
	int (*iscsit_queue_data_in)(struct iscsi_conn *, struct iscsi_cmd *);
	int (*iscsit_queue_status)(struct iscsi_conn *, struct iscsi_cmd *);
	void (*iscsit_aborted_task)(struct iscsi_conn *, struct iscsi_cmd *);
	int (*iscsit_xmit_pdu)(struct iscsi_conn *, struct iscsi_cmd *,
			       struct iscsi_datain_req *, const void *, u32);
	void (*iscsit_unmap_cmd)(struct iscsi_conn *, struct iscsi_cmd *);
	void (*iscsit_get_rx_pdu)(struct iscsi_conn *);
	int (*iscsit_validate_params)(struct iscsi_conn *);
	void (*iscsit_get_r2t_ttt)(struct iscsi_conn *, struct iscsi_cmd *,
				   struct iscsi_r2t *);
	enum target_prot_op (*iscsit_get_sup_prot_ops)(struct iscsi_conn *);
};

static inline void *iscsit_priv_cmd(struct iscsi_cmd *cmd)
{
	return (void *)(cmd + 1);
}

/*
 * From iscsi_target_transport.c
 */

extern void iscsit_register_transport(struct iscsit_transport *);
extern void iscsit_unregister_transport(struct iscsit_transport *);
extern struct iscsit_transport *iscsit_get_transport(int);
extern void iscsit_put_transport(struct iscsit_transport *);

/*
 * From iscsi_target.c
 */
extern int iscsit_setup_scsi_cmd(struct iscsi_conn *, struct iscsi_cmd *,
				unsigned char *);
extern void iscsit_set_unsolicited_dataout(struct iscsi_cmd *);
extern int iscsit_process_scsi_cmd(struct iscsi_conn *, struct iscsi_cmd *,
				struct iscsi_scsi_req *);
extern int
__iscsit_check_dataout_hdr(struct iscsi_conn *, void *,
			   struct iscsi_cmd *, u32, bool *);
extern int
iscsit_check_dataout_hdr(struct iscsi_conn *conn, void *buf,
			 struct iscsi_cmd **out_cmd);
extern int iscsit_check_dataout_payload(struct iscsi_cmd *, struct iscsi_data *,
				bool);
extern int iscsit_setup_nop_out(struct iscsi_conn *, struct iscsi_cmd *,
				struct iscsi_nopout *);
extern int iscsit_process_nop_out(struct iscsi_conn *, struct iscsi_cmd *,
				struct iscsi_nopout *);
extern int iscsit_handle_logout_cmd(struct iscsi_conn *, struct iscsi_cmd *,
				unsigned char *);
extern int iscsit_handle_task_mgt_cmd(struct iscsi_conn *, struct iscsi_cmd *,
				unsigned char *);
extern int iscsit_setup_text_cmd(struct iscsi_conn *, struct iscsi_cmd *,
				 struct iscsi_text *);
extern int iscsit_process_text_cmd(struct iscsi_conn *, struct iscsi_cmd *,
				   struct iscsi_text *);
extern void iscsit_build_rsp_pdu(struct iscsi_cmd *, struct iscsi_conn *,
				bool, struct iscsi_scsi_rsp *);
extern void iscsit_build_nopin_rsp(struct iscsi_cmd *, struct iscsi_conn *,
				struct iscsi_nopin *, bool);
extern void iscsit_build_task_mgt_rsp(struct iscsi_cmd *, struct iscsi_conn *,
				struct iscsi_tm_rsp *);
extern int iscsit_build_text_rsp(struct iscsi_cmd *, struct iscsi_conn *,
				struct iscsi_text_rsp *,
				enum iscsit_transport_type);
extern void iscsit_build_reject(struct iscsi_cmd *, struct iscsi_conn *,
				struct iscsi_reject *);
extern int iscsit_build_logout_rsp(struct iscsi_cmd *, struct iscsi_conn *,
				struct iscsi_logout_rsp *);
extern int iscsit_logout_post_handler(struct iscsi_cmd *, struct iscsi_conn *);
extern int iscsit_queue_rsp(struct iscsi_conn *, struct iscsi_cmd *);
extern void iscsit_aborted_task(struct iscsi_conn *, struct iscsi_cmd *);
extern int iscsit_add_reject(struct iscsi_conn *, u8, unsigned char *);
extern int iscsit_reject_cmd(struct iscsi_cmd *, u8, unsigned char *);
extern int iscsit_handle_snack(struct iscsi_conn *, unsigned char *);
extern void iscsit_build_datain_pdu(struct iscsi_cmd *, struct iscsi_conn *,
				    struct iscsi_datain *,
				    struct iscsi_data_rsp *, bool);
extern int iscsit_build_r2ts_for_cmd(struct iscsi_conn *, struct iscsi_cmd *,
				     bool);
extern int iscsit_immediate_queue(struct iscsi_conn *, struct iscsi_cmd *, int);
extern int iscsit_response_queue(struct iscsi_conn *, struct iscsi_cmd *, int);
/*
 * From iscsi_target_device.c
 */
extern void iscsit_increment_maxcmdsn(struct iscsi_cmd *, struct iscsi_session *);
/*
 * From iscsi_target_erl0.c
 */
extern void iscsit_cause_connection_reinstatement(struct iscsi_conn *, int);
/*
 * From iscsi_target_erl1.c
 */
extern void iscsit_stop_dataout_timer(struct iscsi_cmd *);

/*
 * From iscsi_target_tmr.c
 */
extern int iscsit_tmr_post_handler(struct iscsi_cmd *, struct iscsi_conn *);

/*
 * From iscsi_target_util.c
 */
extern struct iscsi_cmd *iscsit_allocate_cmd(struct iscsi_conn *, int);
extern int iscsit_sequence_cmd(struct iscsi_conn *, struct iscsi_cmd *,
			       unsigned char *, __be32);
extern void iscsit_release_cmd(struct iscsi_cmd *);
extern void iscsit_free_cmd(struct iscsi_cmd *, bool);
extern void iscsit_add_cmd_to_immediate_queue(struct iscsi_cmd *,
					      struct iscsi_conn *, u8);
extern struct iscsi_cmd *
iscsit_find_cmd_from_itt_or_dump(struct iscsi_conn *conn,
				 itt_t init_task_tag, u32 length);

/*
 * From iscsi_target_nego.c
 */
extern int iscsi_target_check_login_request(struct iscsi_conn *,
					    struct iscsi_login *);

/*
 * From iscsi_target_login.c
 */
extern __printf(2, 3) int iscsi_change_param_sprintf(
	struct iscsi_conn *, const char *, ...);

/*
 * From iscsi_target_parameters.c
 */
extern struct iscsi_param *iscsi_find_param_from_key(
	char *, struct iscsi_param_list *);
