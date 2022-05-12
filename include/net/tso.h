/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _TSO_H
#define _TSO_H

#include <net/ip.h>

#define TSO_HEADER_SIZE		256

struct tso_t {
	int	next_frag_idx;
	int	size;
	void	*data;
	u16	ip_id;
	u8	tlen; /* transport header len */
	bool	ipv6;
	u32	tcp_seq;
};

int tso_count_descs(const struct sk_buff *skb);
void tso_build_hdr(const struct sk_buff *skb, char *hdr, struct tso_t *tso,
		   int size, bool is_last);
void tso_build_data(const struct sk_buff *skb, struct tso_t *tso, int size);
int tso_start(struct sk_buff *skb, struct tso_t *tso);

#endif	/* _TSO_H */
