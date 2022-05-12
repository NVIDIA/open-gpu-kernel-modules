/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ISCSI_TARGET_LOGIN_H
#define ISCSI_TARGET_LOGIN_H

#include <linux/types.h>

struct iscsi_conn;
struct iscsi_login;
struct iscsi_np;
struct sockaddr_storage;

extern int iscsi_login_setup_crypto(struct iscsi_conn *);
extern int iscsi_check_for_session_reinstatement(struct iscsi_conn *);
extern int iscsi_login_post_auth_non_zero_tsih(struct iscsi_conn *, u16, u32);
extern int iscsit_setup_np(struct iscsi_np *,
				struct sockaddr_storage *);
extern int iscsi_target_setup_login_socket(struct iscsi_np *,
				struct sockaddr_storage *);
extern int iscsit_accept_np(struct iscsi_np *, struct iscsi_conn *);
extern int iscsit_get_login_rx(struct iscsi_conn *, struct iscsi_login *);
extern int iscsit_put_login_tx(struct iscsi_conn *, struct iscsi_login *, u32);
extern void iscsit_free_conn(struct iscsi_conn *);
extern int iscsit_start_kthreads(struct iscsi_conn *);
extern void iscsi_post_login_handler(struct iscsi_np *, struct iscsi_conn *, u8);
extern void iscsi_target_login_sess_out(struct iscsi_conn *, bool, bool);
extern int iscsi_target_login_thread(void *);
extern void iscsi_handle_login_thread_timeout(struct timer_list *t);

#endif   /*** ISCSI_TARGET_LOGIN_H ***/
